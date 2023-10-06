/* x86-64 Strack Frame Structure:
  *    %rax (as return value)
  *    %rsp (as strack pointer)
  *    %rdi，%rsi，%rdx，%rcx，%r8，%r9 (for passing argument)
  *    %rbx，%rbp，%r12，%r13，%14，%15 (callee usage rule)
  *    %r10，%r11 (caller usage rule)
  */

#include <stdio.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"
#include "codegen.h"

static AS_instrList iList = NULL, last = NULL;
static Temp_label fname = NULL; // initialized in codegen and accessed in 
                                // munchStm and munchExp

static void emit(AS_instr inst);
static void munchStm(T_stm stm);
static Temp_temp munchExp(T_exp exp);
static Temp_tempList callDefs();

static void emit(AS_instr inst) {
	if(last)
		last = last->tail = AS_InstrList(inst, NULL);
	else
		iList = last = AS_InstrList(inst, NULL);
}

static Temp_tempList callDefs() {
	static Temp_tempList defs = NULL;
	if(!defs) {
		Temp_tempList last;
		defs = last = Temp_TempList(F_RV(), NULL); // %rax
		for(Temp_tempList p = F_PARAS(); p; p = p->tail)
			last = last->tail = Temp_TempList(p->head, NULL); // %rdi, %rsi, ...
		for(Temp_tempList p = F_TEMPS(); p; p = p->tail)
			last = last->tail = Temp_TempList(p->head, NULL); // %r10, %r11
	}
	return defs;
}

static void munchStm(T_stm stm) {
	char assem[64];
	switch(stm->kind) {
		case T_MOVE: {
			T_exp dst = stm->u.MOVE.dst;
			T_exp src = stm->u.MOVE.src;
			if(dst->kind == T_TEMP) {
				if(src->kind == T_CONST) {
					// TEMP <- CONST
					sprintf(assem, "mov $%d, `d0", src->u.CONST);
					emit(AS_Oper(String(assem),
					      Temp_TempList(dst->u.TEMP, NULL),
					      NULL,
					      NULL));		
				}
				else if(src->kind == T_MEM) {
					T_exp addr = src->u.MEM;
					if(addr->kind == T_BINOP) {
						if(addr->u.BINOP.op == T_plus) {
							T_exp left = addr->u.BINOP.left;
							T_exp right = addr->u.BINOP.right;	
							if(right->kind == T_CONST) {
								if(left->kind == T_TEMP && left->u.TEMP == F_FP()) {
									// TEMP <- MEM[CONST + FRAMESIZE + SP] 
									sprintf(assem, "mov %d+%s_framesize(`s0), `d0", right->u.CONST, Temp_labelstring(fname));
									emit(AS_Oper(String(assem),
									      Temp_TempList(dst->u.TEMP, NULL),
									      Temp_TempList(F_SP(), NULL),
									      NULL));
								}
								else {
									// TEMP <- MEM[CONST + TEMP]
									sprintf(assem, "mov %d(`s0), `d0", right->u.CONST);
									emit(AS_Oper(String(assem),
									      Temp_TempList(dst->u.TEMP, NULL),
									      Temp_TempList(munchExp(left), NULL),
									      NULL));
								}
								break;
							}
						}
					}
					// TEMP <- MEM[TEMP]
					sprintf(assem, "mov 0(`s0), `d0");
					emit(AS_Oper(String(assem),
					      Temp_TempList(dst->u.TEMP, NULL),
					      Temp_TempList(munchExp(addr), NULL),
					      NULL));
				}
				else {
					// TEMP <- TEMP
					sprintf(assem, "mov `s0, `d0");
					emit(AS_Move(String(assem),
					      Temp_TempList(dst->u.TEMP, NULL),
					      Temp_TempList(munchExp(src), NULL)));
				}
			}
			else if(dst->kind == T_MEM) {
				Temp_tempList srcList, srcLast; 
				srcList = srcLast = Temp_TempList(NULL, NULL);
				// record the number of source registers in the instruction
				int srcNum = 0; 
				// scan the source
				if(src->kind == T_CONST) 
					// MEM[???] <- CONST
					sprintf(assem, "movq $%d, ", src->u.CONST);
				else {
					// MEM[???] <- TEMP
					sprintf(assem, "mov `s%d, ", srcNum++);
					srcLast = srcLast->tail = Temp_TempList(munchExp(src), NULL);
				}
				// scan the destination
				T_exp addr = dst->u.MEM;
				if(addr->kind == T_BINOP) {
					if(addr->u.BINOP.op == T_plus) {
						T_exp left = addr->u.BINOP.left;
						T_exp right = addr->u.BINOP.right;
						if(right->kind == T_CONST) {
							if(left->kind == T_TEMP && left->u.TEMP == F_FP()) {
								// MEM[CONST + FRAMESIZE + SP] <- ???
								sprintf(assem+strlen(assem), "%d+%s_framesize(`s%d)", right->u.CONST, Temp_labelstring(fname), srcNum++);
								srcLast = srcLast->tail = Temp_TempList(F_SP(), NULL);
							}
							else {
								// MEM[CONST + TEMP] <- ???
								sprintf(assem+strlen(assem), "%d(`s%d)", right->u.CONST, srcNum++);
								srcLast = srcLast->tail = Temp_TempList(munchExp(left), NULL);
							}
							emit(AS_Oper(String(assem),
							      NULL,	
							      srcList->tail,
							      NULL));
							break;
						}
					}
				}
				// MEM[TEMP] <- ???
				sprintf(assem+strlen(assem), "0(`s%d)", srcNum++);
				srcLast = srcLast->tail = Temp_TempList(munchExp(addr), NULL);
				emit(AS_Oper(String(assem),
				      NULL,
				      srcList->tail,
				      NULL));
			}
			else
				assert(0);
			break;
		}
		case T_JUMP: {	
			T_exp dest = stm->u.JUMP.exp;
			if(dest->kind == T_NAME) {
				sprintf(assem, "jmp `j0");
				emit(AS_Oper(String(assem),
				      NULL,
				      NULL,
				      AS_Targets(Temp_LabelList(dest->u.NAME, NULL))));
			}
			else {
				sprintf(assem, "jmp *`s0");
				emit(AS_Oper(String(assem),
				      NULL,
				      Temp_TempList(munchExp(dest), NULL),
				      NULL));
			}
			break;
		}
		case T_CJUMP: {
			T_exp left = stm->u.CJUMP.left;
			T_exp right = stm->u.CJUMP.right;
			T_relOp op = stm->u.CJUMP.op;
			char *jumpStrs[6] = {"je", "jne", "jl", "jg", "jle", "jge"};
			if(left->kind != T_MEM && right->kind == T_MEM) {
				// convert "CMP MEM, NON_MEM" to "CMP NON_MEM, MEM" to simplify the
				// scan of right tree 
				stm->u.CJUMP.left = right;
				stm->u.CJUMP.right = left;
				stm->u.CJUMP.op = T_commute(op);
				return munchStm(stm);
			}
			Temp_tempList srcList, srcLast;
			srcList = srcLast = Temp_TempList(NULL, NULL);
			// record the number of source register in the compare instruction
			int srcNum = 0;
			// scan the right
			if(right->kind == T_CONST) 
				// CMP CONST, ???
				sprintf(assem, "cmpq $%d, ",right->u.CONST);
			else {
				// CMP TEMP, ???
				sprintf(assem, "cmp `s%d, ", srcNum++);
				srcLast = srcLast->tail = Temp_TempList(munchExp(right), NULL);
			}
			// scan the left
			if(left->kind == T_CONST) 
				// CMP ???, CONST
				sprintf(assem+strlen(assem), "$%d", left->u.CONST);
			else if(left->kind == T_MEM) {
				T_exp addr = left->u.MEM;
				if(addr->kind == T_BINOP && addr->u.BINOP.op == T_plus && addr->u.BINOP.right->kind == T_CONST) {
					T_exp west = addr->u.BINOP.left;
					T_exp east = addr->u.BINOP.right;
					if(west->kind == T_TEMP && west->u.TEMP == F_FP()) {
						// CMP ???, MEM[CONST + FRAMESIZE + SP]
						sprintf(assem+strlen(assem), "%d+%s_framesize(`s%d)", east->u.CONST, Temp_labelstring(fname), srcNum++);
						srcLast = srcLast->tail = Temp_TempList(F_SP(), NULL);
					}
					else {
						// CMP ???, MEM[CONST + TEMP]
						sprintf(assem+strlen(assem), "%d(`s%d)", east->u.CONST, srcNum++);
						srcLast = srcLast->tail = Temp_TempList(munchExp(west), NULL);
					}
				}
				else {
					// CMP ???, MEM[TEMP]
					sprintf(assem+strlen(assem), "0(`s%d)", srcNum++);
					srcLast = srcLast->tail = Temp_TempList(munchExp(addr), NULL);
				}
			}
			else {
				// CMP ???, TEMP
				sprintf(assem+strlen(assem), "`s%d", srcNum++);
				srcLast = srcLast->tail = Temp_TempList(munchExp(left), NULL);
			}
			emit(AS_Oper(String(assem),
			      NULL,
			      srcList->tail,
			      NULL));
			assert(op < 6);
			// J? TRUE_LABEL
			sprintf(assem, "%s `j0", jumpStrs[op]);
			emit(AS_Oper(String(assem),
			      NULL,
			      NULL,
			      AS_Targets(Temp_LabelList(stm->u.CJUMP.true,
			                  Temp_LabelList(stm->u.CJUMP.false, NULL)))));
			break;
		}
		case T_LABEL: {
			Temp_label lab = stm->u.LABEL;
			sprintf(assem, "%s:", Temp_labelstring(lab));
			emit(AS_Label(String(assem),
			      lab));
			break;
		}
		case T_EXP: {
			munchExp(stm->u.EXP);
			break;
		}
		default:
			assert(0);
	}
}

static Temp_temp munchExp(T_exp exp) {
	char assem[64];
	Temp_temp r = Temp_newtemp();
	switch(exp->kind) {
		case T_BINOP: {
			T_binOp op = exp->u.BINOP.op;
			T_exp left = exp->u.BINOP.left;
			T_exp right = exp->u.BINOP.right;
			char *opStrs[4] = {"add", "sub", "imul", "idiv"};
			Temp_temp rdx = F_PARAS()->tail->tail->head; // %rdx for idiv
			if(op == T_div) {
				r = F_RV(); // %rax
				// %rdx <- 0
				sprintf(assem, "xor `s0, `d0");
				emit(AS_Oper(String(assem),
				      Temp_TempList(rdx, NULL),
				      Temp_TempList(rdx, NULL),
				      NULL));
			}
			Temp_tempList srcList = NULL;
			Temp_tempList dstList = Temp_TempList(r, NULL);
			// scan the left
			if(left->kind == T_CONST) 
				// r <- CONST
				sprintf(assem, "mov $%d, `d0", left->u.CONST);
			else if(left->kind == T_MEM) {
				T_exp addr = left->u.MEM;
				if(addr->kind == T_BINOP && addr->u.BINOP.op == T_plus && addr->u.BINOP.right->kind == T_CONST) {
					T_exp west = addr->u.BINOP.left;
					T_exp east = addr->u.BINOP.right;
					if(west->kind == T_TEMP && west->u.TEMP == F_FP()) {
						// r <- MEM[CONST + FRAMESIZE + SP]
						sprintf(assem, "mov %d+%s_framesize(`s0), `d0", east->u.CONST, Temp_labelstring(fname)); 
						srcList = Temp_TempList(F_SP(), NULL);
					}
					else {
						// r <- MEM[CONST + TEMP]
						sprintf(assem, "mov %d(`s0), `d0", east->u.CONST);
						srcList = Temp_TempList(munchExp(west), NULL);
					}
				}
				else {
					// r <- MEM[TEMP]
					sprintf(assem, "mov 0(`s0), `d0");
					srcList = Temp_TempList(munchExp(addr), NULL);
				}
			}
			else {
				// r <- TEMP
				sprintf(assem, "mov `s0, `d0");
				srcList = Temp_TempList(munchExp(left), NULL);
			}
			if(left->kind != T_CONST && left->kind != T_MEM) {
				emit(AS_Move(String(assem),
				      dstList,
				      srcList));
			} 
			else {
				emit(AS_Oper(String(assem),
				      dstList,
				      srcList,
				      NULL));
			}
			// scan the right
			srcList = Temp_TempList(r, NULL);
			dstList = Temp_TempList(r, NULL);
			if(op == T_div) {
				dstList->tail = Temp_TempList(rdx, NULL); // %rax ->  %rdx -> NULL
				srcList->tail = Temp_TempList(rdx, NULL); // %rax -> %rdx -> NULL
			}
			assert(op < 4);
			if(right->kind == T_CONST && op != T_div) 
				// r <- r op CONST
				sprintf(assem, "%s $%d, `d0", opStrs[op], right->u.CONST);
			else if(right->kind == T_MEM) {
				T_exp addr = right->u.MEM;
				if(addr->kind == T_BINOP && addr->u.BINOP.op == T_plus && addr->u.BINOP.right->kind == T_CONST) {
					T_exp west = addr->u.BINOP.left;
					T_exp east = addr->u.BINOP.right;
					if(west->kind == T_TEMP && west->u.TEMP == F_FP()) {
						// r <- r op MEM[CONST + FRAMESIZE + SP]
						sprintf(assem, "%s %d+%s_framesize(`s0), `d0", opStrs[op], east->u.CONST, Temp_labelstring(fname)); 
						srcList = Temp_TempList(F_SP(), srcList);
					}
					else {
						// r <- r op MEM[CONST + TEMP]
						sprintf(assem, "%s %d(`s0), `d0", opStrs[op], east->u.CONST);
						srcList = Temp_TempList(munchExp(west), srcList);
					}
				}
				else {
					// r <- r op MEM[TEMP]
					sprintf(assem, "%s 0(`s0), `d0", opStrs[op]);
					srcList = Temp_TempList(munchExp(addr), srcList);
				}
			}
			else {
				// r <- r op TEMP
				sprintf(assem, "%s `s0, `d0", opStrs[op]);
				srcList = Temp_TempList(munchExp(right), srcList);
			}
			if(op == T_div) 
				assem[strlen(assem) - 5] = '\0'; // delete the substring ", `d0"
			emit(AS_Oper(String(assem),
			      dstList,
			      srcList,
			      NULL));
			break;
		}
		case T_MEM: {
			// Though I have written similar code segments for many times, I hope the
			// number of instruction and register could be minimal as much as 
			// possible.
			T_exp addr = exp->u.MEM;
			if(addr->kind == T_BINOP) {
				if(addr->u.BINOP.op == T_plus) {
					T_exp left = addr->u.BINOP.left;
					T_exp right = addr->u.BINOP.right;
					if(right->kind == T_CONST) {
						if(left->kind == T_TEMP && left->u.TEMP == F_FP()) {
							// r <- MEM[CONST + FRAMESIZE + SP]
							sprintf(assem, "mov %d+%s_framesize(`s0), `d0", right->u.CONST, Temp_labelstring(fname)); 
							emit(AS_Oper(String(assem),
							      Temp_TempList(r, NULL),
							      Temp_TempList(F_SP(), NULL),
							      NULL));
						}
						else {
							// r <- MEM[CONST + TEMP]
							sprintf(assem, "mov %d(`s0), `d0", right->u.CONST);
							emit(AS_Oper(String(assem),
							      Temp_TempList(r, NULL),
							      Temp_TempList(munchExp(left), NULL),
							      NULL));
						}
						break;
					}
				}
			}
			// r <- MEM[TEMP]
			sprintf(assem, "mov 0(`s0), `d0");
			emit(AS_Oper(String(assem),
			      Temp_TempList(r, NULL),
			      Temp_TempList(munchExp(addr), NULL),
			      NULL));
			break;
		}
		case T_TEMP: {
			if(exp->u.TEMP == F_FP()) {
				// r <- SP
				sprintf(assem, "mov `s0, `d0");
				emit(AS_Move(String(assem),
				      Temp_TempList(r, NULL),
				      Temp_TempList(F_SP(), NULL)));
				// r <- r + FRAMESIZE
				sprintf(assem, "add $%s_framesize, `d0", Temp_labelstring(fname));
				emit(AS_Oper(String(assem),
				      Temp_TempList(r, NULL),
				      Temp_TempList(r, NULL),
				      NULL));
			}
			else
				r = exp->u.TEMP;
			break;
		}
		case T_NAME: {
			// r <- LABEL
			sprintf(assem, "mov $%s, `d0", Temp_labelstring(exp->u.NAME));
			emit(AS_Oper(String(assem),
			      Temp_TempList(r, NULL),
			      NULL,
			      NULL));
			break;
		}
		case T_CONST: {
			// r <- CONST
			sprintf(assem, "mov $%d, `d0", exp->u.CONST);
			emit(AS_Oper(String(assem),
			      Temp_TempList(r, NULL),
			      NULL,
			      NULL));
			break;
		}
		case T_CALL: {
			T_stmList stmList = NULL;
			Temp_tempList srcList = NULL;
			Temp_tempList p = F_PARAS();
			int paraIndex = 0;
			for(T_expList q = exp->u.CALL.args; q; q = q->tail, ++paraIndex) {
				if(p) {
					stmList = T_StmList(T_Move(T_Temp(p->head), q->head), stmList);
					srcList = Temp_TempList(p->head, srcList);
					p = p->tail;
				}
				else {
					stmList = T_StmList(T_Move(T_Mem(T_Binop(T_plus,
					                                  T_Temp(F_SP()),
					                                  T_Const(paraIndex * F_wordSize))),
					                     q->head), stmList);
				} 
			}
			// compute the parameter from right to left
			for(T_stmList q = stmList; q; q = q->tail)
				munchStm(q->head);
			// call the function
			T_exp fun = exp->u.CALL.fun;
			if(fun->kind != T_NAME) {
				sprintf(assem, "callq *`s0");
				srcList = Temp_TempList(munchExp(fun), srcList);
				emit(AS_Oper(String(assem),
				      callDefs(),
				      srcList,
				      NULL));
			}
			else {
				sprintf(assem, "callq `j0");
				emit(AS_Oper(String(assem),
				      callDefs(),
				      srcList,
				      AS_Targets(Temp_LabelList(fun->u.NAME, NULL))));
			}     
			r = F_RV();
			break;
		}
		default:
			assert(0);
	}	
	return r;
}

AS_instrList F_codegen(F_frame f, T_stmList stmList) {
	AS_instrList list;
	T_stmList sl;
	fname = F_name(f);
	for(sl = stmList; sl; sl = sl->tail) 
		munchStm(sl->head);
	list = iList;
	iList = last = NULL;
	return F_procEntryExit2(list);
}

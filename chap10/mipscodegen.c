#include <stdio.h>
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
static Temp_tempList munchArgs(int* n, T_expList args);
static Temp_tempList callDefs();

static void emit(AS_instr inst) {
	if(last)
		last = last->tail = AS_InstrList(inst, NULL);
	else
		iList = last = AS_InstrList(inst, NULL);
}

static void munchStm(T_stm stm) {
	char assem[64]; // to construct the assembly
	switch(stm->kind) {
		case T_LABEL: {
			Temp_label lab = stm->u.LABEL;
			sprintf(assem, "%s:", Temp_labelstring(lab));
			emit(AS_Label(String(assem), lab));
			break;
		}
		case T_JUMP: {
			T_exp dst = stm->u.JUMP.exp;
			if(dst->kind == T_NAME) {
				// 'jump label' is the only form of jumping in the IR tree.
				Temp_label lab = dst->u.NAME;
				sprintf(assem, "j `j0");
				emit(AS_Oper(String(assem),
				      NULL,
				      NULL,
				      AS_Targets(Temp_LabelList(lab, NULL))));
			}
			else {
				sprintf(assem, "jr `s0");
				emit(AS_Oper(String(assem),
					    NULL,
				      Temp_TempList(munchExp(dst), NULL),
				      NULL));
			}
			break;
		}
		case T_CJUMP: {
			T_exp left = stm->u.CJUMP.left;
			T_exp right = stm->u.CJUMP.right;
			T_relOp op = stm->u.CJUMP.op;
			Temp_label true = stm->u.CJUMP.true;
			Temp_label false = stm->u.CJUMP.false;
			Temp_temp r = Temp_newtemp(); // maybe useless in some cases
			switch(op) {
				case T_eq: {
					if(right->kind == T_CONST) {
						if(!right->u.CONST) {
							// beq left, zero, lab		
							sprintf(assem, "beq `s0, `s1, `j0");
							emit(AS_Oper(String(assem),
							      NULL,
					          Temp_TempList(munchExp(left),
					           Temp_TempList(F_ZERO(), NULL)),
					          AS_Targets(Temp_LabelList(true, 
							       Temp_LabelList(false, NULL)))));	
							break;			
						}
						// addi r, left, -right
						sprintf(assem, "addi `d0, `s0, %d", -right->u.CONST);
						emit(AS_Oper(String(assem), 
					        Temp_TempList(r, NULL),
					        Temp_TempList(munchExp(left), NULL),
					        NULL));						
					}
					else {
						// sub r, left, right
						sprintf(assem, "sub `d0, `s0, `s1");
						emit(AS_Oper(String(assem), 
					        Temp_TempList(r, NULL),
					        Temp_TempList(munchExp(left),
					         Temp_TempList(munchExp(right), NULL)),
					        NULL));
					}
					// beq r, zero, lab
					sprintf(assem, "beq `s0, `s1, `j0");
					emit(AS_Oper(String(assem),
							  NULL,
					      Temp_TempList(r,
					       Temp_TempList(F_ZERO(), NULL)),
					      AS_Targets(Temp_LabelList(true, 
							   Temp_LabelList(false, NULL)))));	
					break;
				}
				case T_ne: {
					if(right->kind == T_CONST) {
						if(!right->u.CONST) {
							// bne left, zero, lab		
							sprintf(assem, "bne `s0, `s1, `j0");
							emit(AS_Oper(String(assem),
							      NULL,
					          Temp_TempList(munchExp(left),
					           Temp_TempList(F_ZERO(), NULL)),
					          AS_Targets(Temp_LabelList(true, 
							       Temp_LabelList(false, NULL)))));	
							break;			
						}
						// addi r, left, -right
						sprintf(assem, "addi `d0, `s0, %d", -right->u.CONST);
						emit(AS_Oper(String(assem), 
					        Temp_TempList(r, NULL),
					        Temp_TempList(munchExp(left), NULL),
					        NULL));						
					}
					else {
						// sub r, left, right
						sprintf(assem, "sub `d0, `s0, `s1");
						emit(AS_Oper(String(assem), 
					        Temp_TempList(r, NULL),
					        Temp_TempList(munchExp(left),
					         Temp_TempList(munchExp(right), NULL)),
					        NULL));
					}
					// bne r, zero, lab
					sprintf(assem, "bne `s0, `s1, `j0");
					emit(AS_Oper(String(assem),
							  NULL,
					      Temp_TempList(r,
					       Temp_TempList(F_ZERO(), NULL)),
					      AS_Targets(Temp_LabelList(true, 
							   Temp_LabelList(false, NULL)))));	
					break;
				}
				case T_lt: {
					if(right->kind == T_CONST) {
						// slti r, left, right(CONST)
						sprintf(assem, "slti `d0, `s0, %d", right->u.CONST);
						emit(AS_Oper(String(assem),
						      Temp_TempList(r, NULL),
						      Temp_TempList(munchExp(left), NULL),
						      NULL));
					}
					else {
						// slt r, left, right
						sprintf(assem, "slt `d0, `s0, `s1");
						emit(AS_Oper(String(assem),
					        Temp_TempList(r, NULL),
					        Temp_TempList(munchExp(left), 
					         Temp_TempList(munchExp(right), NULL)),
					        NULL));
					}
					// bne r, zero, lab
					sprintf(assem, "bne `s0, `s1, `j0");
					emit(AS_Oper(String(assem),
							  NULL,
					      Temp_TempList(r,
					       Temp_TempList(F_ZERO(), NULL)),
					      AS_Targets(Temp_LabelList(true, 
							   Temp_LabelList(false, NULL)))));	
					break;
				}
				case T_le: {
					// (!(a <= b)) <=> (a > b) <=> (b < a)
					if(left->kind == T_CONST) {
						// slti r, right, left(CONST)
						sprintf(assem, "slti `d0, `s0, %d", left->u.CONST);
						emit(AS_Oper(String(assem),
						      Temp_TempList(r, NULL),
						      Temp_TempList(munchExp(right), NULL),
						      NULL));
					}
					else {
						// slt r, right, left
						sprintf(assem, "slt `d0, `s0, `s1");
						emit(AS_Oper(String(assem),
					        Temp_TempList(r, NULL),
					        Temp_TempList(munchExp(right), 
					         Temp_TempList(munchExp(left), NULL)),
					        NULL));
					}
					// beq r, zero, lab
					sprintf(assem, "beq `s0, `s1, `j0");
					emit(AS_Oper(String(assem),
							  NULL,
					      Temp_TempList(r,
					       Temp_TempList(F_ZERO(), NULL)),
					      AS_Targets(Temp_LabelList(true, 
							   Temp_LabelList(false, NULL)))));	
					break;
				}
				case T_gt:
				case T_ge: {
					// CJUPM a >= b, true, false
					// <=>
					// CJUMP b <= a, true, false
					stm->u.CJUMP.left = right;
					stm->u.CJUMP.right = left;
					stm->u.CJUMP.op = T_commute(op);
					munchStm(stm);
					break;
				}
				default:
					assert(0); // I never use unsigned comparison in the IR tree.
			}
			break;
		}
		case T_MOVE: {
			T_exp dst = stm->u.MOVE.dst;
			T_exp src = stm->u.MOVE.src;
			if(dst->kind == T_TEMP) {
				if(src->kind == T_CONST) {
					// TEMP <- CONST
					sprintf(assem, "addi `d0, `s0, %d", src->u.CONST);
					emit(AS_Oper(String(assem), 
					      Temp_TempList(dst->u.TEMP, NULL),
					      Temp_TempList(F_ZERO(), NULL),
					      NULL));
				}
				else if(src->kind == T_BINOP && src->u.BINOP.op == T_plus && src->u.BINOP.left->kind == T_CONST) {
					T_exp left = src->u.BINOP.left;
					T_exp right = src->u.BINOP.right;
					// TEMP <- CONST + TEMP
					sprintf(assem, "addi `d0, `s0, %d", left->u.CONST);
					emit(AS_Oper(String(assem), 
					      Temp_TempList(dst->u.TEMP, NULL),
					      Temp_TempList(munchExp(right), NULL),
					   	  NULL));
					}
				else if(src->kind == T_BINOP && src->u.BINOP.op == T_plus && src->u.BINOP.right->kind == T_CONST) {
					T_exp left = src->u.BINOP.left;
					T_exp right = src->u.BINOP.right;
					// TEMP <- TEMP + CONST
					sprintf(assem, "addi `d0, `s0, %d", right->u.CONST);
					emit(AS_Oper(String(assem), 
					      Temp_TempList(dst->u.TEMP, NULL),
					      Temp_TempList(munchExp(left), NULL),
					   	  NULL));
				}
				else if(src->kind == T_BINOP && src->u.BINOP.op == T_minus && src->u.BINOP.right->kind == T_CONST) {
					T_exp left = src->u.BINOP.left;
					T_exp right = src->u.BINOP.right;
					// TEMP <- TEMP - CONST
					sprintf(assem, "addi `d0, `s0, %d", -right->u.CONST);
					emit(AS_Oper(String(assem), 
					      Temp_TempList(dst->u.TEMP, NULL),
					      Temp_TempList(munchExp(left), NULL),
					   	  NULL));
				}
				else if(src->kind == T_MEM) {
					T_exp addr = src->u.MEM;
					if(addr->kind == T_BINOP && addr->u.BINOP.op == T_plus && addr->u.BINOP.right->kind == T_CONST) {
						T_exp left = addr->u.BINOP.left;
						T_exp right = addr->u.BINOP.right;
						if(left->kind == T_TEMP && left->u.TEMP == F_FP()) {
							// TEMP <- MEM[SP + FRAME_SIZE + CONST]
							sprintf(assem, "lw `d0, %d+%s_framesize(`s0)", right->u.CONST, Temp_labelstring(fname));
							emit(AS_Oper(String(assem), 
				            Temp_TempList(dst->u.TEMP, NULL),
				            Temp_TempList(F_SP(), NULL),
					          NULL));
						}
						else {
							// TEMP <- MEM[TEMP + CONST]
							sprintf(assem, "lw `d0, %d(`s0)", right->u.CONST);
							emit(AS_Oper(String(assem), 
				            Temp_TempList(dst->u.TEMP, NULL),
				            Temp_TempList(munchExp(left), NULL),
					          NULL));
						}
					}
					else {
						// TEMP <- MEM[TEMP]
						sprintf(assem, "lw `d0, 0(`s0)");
						emit(AS_Oper(String(assem), 
				          Temp_TempList(dst->u.TEMP, NULL),
				          Temp_TempList(munchExp(addr), NULL),
					        NULL));
					}
				}
				else {
					// TEMP <- TEMP
					sprintf(assem, "add `d0, `s0, `s1");
					// use AS_Move to elimate the copy propagation
					emit(AS_Move(String(assem), 
					      Temp_TempList(dst->u.TEMP, NULL),
					      Temp_TempList(munchExp(src),
					       Temp_TempList(F_ZERO(), NULL))));	
				}
			}
			else if(dst->kind == T_MEM) {
				T_exp addr = dst->u.MEM;
				if(addr->kind == T_BINOP && addr->u.BINOP.op == T_plus && addr->u.BINOP.right->kind == T_CONST) {
					T_exp left = addr->u.BINOP.left;
					T_exp right = addr->u.BINOP.right;
					if(left->kind == T_TEMP && left->u.TEMP == F_FP()) {
						// MEM[SP + FRAMESIZE + CONST] <- TEMP
						sprintf(assem, "sw `s0, %d+%s_framesize(`s1)", right->u.CONST, Temp_labelstring(fname));
						emit(AS_Oper(String(assem), 
					      	NULL,
					      	Temp_TempList(munchExp(src),
					      	Temp_TempList(F_SP(), NULL)),
					      	NULL));
					}
					else {
						// MEM[TEMP + CONST] <- TEMP
						sprintf(assem, "sw `s0, %d(`s1)", right->u.CONST);
						emit(AS_Oper(String(assem), 
					  	    NULL,
					  	    Temp_TempList(munchExp(src),
					  	    Temp_TempList(munchExp(left), NULL)),
					  	    NULL));
					}
				}
				else {
					// MEM[TEMP] <- TEMP
					sprintf(assem, "sw `s0, 0(`s1)");
					emit(AS_Oper(String(assem), 
					      NULL,
					      Temp_TempList(munchExp(src),
					       Temp_TempList(munchExp(addr), NULL)),
					      NULL));				
				}	
			}
			else
				assert(0);
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
	Temp_temp r = Temp_newtemp();
	char assem[64];
	switch(exp->kind) {
		case T_BINOP: {
			T_exp left = exp->u.BINOP.left;
			T_exp right = exp->u.BINOP.right;
			T_binOp op = exp->u.BINOP.op;
			switch(op) {
				case T_plus: {
					if(left->kind == T_CONST) {
						// addi r, right, left(CONST)
						sprintf(assem, "addi `d0, `s0, %d", left->u.CONST);
						emit(AS_Oper(String(assem),
					  	    Temp_TempList(r, NULL),
					  	    Temp_TempList(munchExp(right), NULL),
					  	    NULL)); 
					}
					else if(right->kind == T_CONST) {
						// addi r, left, right(CONST)
						sprintf(assem, "addi `d0, `s0, %d", right->u.CONST);
						emit(AS_Oper(String(assem),
					      	Temp_TempList(r, NULL),
					      	Temp_TempList(munchExp(left), NULL),
					      	NULL)); 
					}
					else {
						// add r, left, right
						sprintf(assem, "add `d0, `s0, `s1");
						emit(AS_Oper(String(assem),
					      	Temp_TempList(r, NULL),
					      	Temp_TempList(munchExp(left),
					      	 Temp_TempList(munchExp(right), NULL)),
					      	NULL));
					}
					break;
				}
				case T_minus: {
					if(right->kind == T_CONST) {
						// addi r, left, -right(CONST)
						sprintf(assem, "addi `d0, `s0, %d", -right->u.CONST);
						emit(AS_Oper(String(assem),
						      Temp_TempList(r, NULL),
						      Temp_TempList(munchExp(left), NULL),
						      NULL));
					}
					else {
						// sub r, left, right
						sprintf(assem, "sub `d0, `s0, `s1");
						emit(AS_Oper(String(assem),
					      	Temp_TempList(r, NULL),
					      	Temp_TempList(munchExp(left),
					      	 Temp_TempList(munchExp(right), NULL)),
					      	NULL));
					}
					break;				
				}		
				case T_mul: {
					// mult left, right
					sprintf(assem, "mult `s0, `s1");
					emit(AS_Oper(String(assem),
					      NULL,
					      Temp_TempList(munchExp(left),
					       Temp_TempList(munchExp(right), NULL)),
					      NULL));
					// mflo r
					sprintf(assem, "mflo `d0");
					emit(AS_Oper(String(assem),
						    Temp_TempList(r, NULL),
						    NULL,
						    NULL));
					break;
				}
				case T_div: {
					// div left, right
					sprintf(assem, "div `s0, `s1");
					emit(AS_Oper(String(assem),
					      NULL,
					      Temp_TempList(munchExp(left),
					       Temp_TempList(munchExp(right), NULL)),
					      NULL));
					// mflo r
					sprintf(assem, "mflo `d0");
					emit(AS_Oper(String(assem),
						    Temp_TempList(r, NULL),
						    NULL,
						    NULL));
					break;
				}
				default: 
					assert(0); // I never use other computation in the IR tree.
			}
			break;
		}
		case T_MEM: {
			T_exp addr = exp->u.MEM;
			if(addr->kind == T_BINOP && addr->u.BINOP.op == T_plus && addr->u.BINOP.right->kind == T_CONST) {
				T_exp left = addr->u.BINOP.left;
				T_exp right = addr->u.BINOP.right;
				if(left->kind == T_TEMP && left->u.TEMP == F_FP()) {
					// lw r, MEM[SP + FRAMESIZE + CONST]
					sprintf(assem, "lw `d0, %d+%s_framesize(`s0)", right->u.CONST, Temp_labelstring(fname));
					emit(AS_Oper(String(assem),
				  	    Temp_TempList(r, NULL),
				  	    Temp_TempList(F_SP(), NULL),
				   	   NULL));
				}
				else {
					// lw r, MEM[CONST + left]
					sprintf(assem, "lw `d0, %d(`s0)", right->u.CONST);
					emit(AS_Oper(String(assem),
				        Temp_TempList(r, NULL),
				        Temp_TempList(munchExp(left), NULL),
				        NULL));
				}
			}
			else {
				// lw r, MEM[addr]
				sprintf(assem, "lw `d0, 0(`s0)");
				emit(AS_Oper(String(assem),
				      Temp_TempList(r, NULL),
				      Temp_TempList(munchExp(addr), NULL),
				      NULL));
			}
			break;
		}
		case T_TEMP: {
			Temp_temp temp = exp->u.TEMP;
			if(temp == F_FP()) {
				// I think I have been coding in a terrible and obscure way when
				// I attempt to substitute virtual FP with SP + FRAMESIZE.
				// CALL(FUNC, FP, ...)
				sprintf(assem, "addi `d0, `s0, %s_framesize", Temp_labelstring(fname));
				emit(AS_Oper(String(assem),
				      Temp_TempList(r, NULL),
				      Temp_TempList(F_SP(), NULL),
				      NULL));
			}
			else
				r = temp;
			break;
		}
		case T_NAME: {
			// addi r, zero, lab
			Temp_label lab = exp->u.NAME;
			sprintf(assem, "addi `d0, `s0, %s", Temp_labelstring(lab));
			emit(AS_Oper(String(assem),
			      Temp_TempList(r, NULL),
			      Temp_TempList(F_ZERO(), NULL),
			      NULL));
			break;
		}
		case T_CONST: {
			// addi r, zero, CONST
			sprintf(assem, "addi `d0, `s0, %d", exp->u.CONST);
			emit(AS_Oper(String(assem),
			      Temp_TempList(r, NULL),
			      Temp_TempList(F_ZERO(), NULL),
			      NULL));
			break;
		}
		case T_CALL: {
			int n = 0;
			int i;
			T_exp fun = exp->u.CALL.fun;
			Temp_tempList arg = munchArgs(&n, exp->u.CALL.args); // loop variable
			Temp_tempList args = NULL; // function call 's source registers
			// addi sp, sp, - n * 4	
			sprintf(assem, "addi `d0, `s0, %d", -(n < 4 ? 4 : n) * F_wordSize);
			emit(AS_Oper(String(assem),
			      Temp_TempList(F_SP(), NULL),
			      Temp_TempList(F_SP(), NULL),
			      NULL));
			// move the parameter to correct position
			i = 0;
			for(Temp_tempList para = F_PARAS(); arg; arg = arg->tail, ++i) {
				if(para) {
					sprintf(assem, "add `d0, `s0, `s1");
					emit(AS_Move(String(assem),
					      Temp_TempList(para->head, NULL),
					      Temp_TempList(arg->head,
					       Temp_TempList(F_ZERO(), NULL))));
					args = Temp_TempList(para->head, args);
					para = para->tail;		
				}
				else {
					// sw arg, 4 * i(sp)
					sprintf(assem, "sw `s0, %d(`s1)", i * F_wordSize);
					emit(AS_Oper(String(assem),
					       NULL,
					       Temp_TempList(arg->head,
				          Temp_TempList(F_SP(), NULL)),
					       NULL));
				}
			}
			if(fun->kind == T_NAME) {
				// jal lab
				Temp_label lab = fun->u.NAME;
				sprintf(assem, "jal `j0");
				emit(AS_Oper(String(assem),
				      callDefs(),
				      args,
							AS_Targets(Temp_LabelList(lab, NULL))));
			}
			else {
				sprintf(assem, "jalr `s0");
				emit(AS_Oper(String(assem),
				      callDefs(),
				      Temp_TempList(munchExp(fun), args),
				      NULL));
			}
			// addi sp, sp, 4 * n
			sprintf(assem, "addi `d0, `s0, %d", (n < 4 ? 4 : n) * F_wordSize);
			emit(AS_Oper(String(assem),
			      Temp_TempList(F_SP(), NULL),
			      Temp_TempList(F_SP(), NULL),
			      NULL));
			r = F_RV();
			break;
		}
		default:
			assert(0);	
	}
	return r;
}

static Temp_tempList callDefs() {
	static Temp_tempList defs = NULL;
	if(!defs) {
		defs = Temp_TempList(F_RV(),
		        Temp_TempList(F_RA(),
		         F_TEMPS()));
	}
	return defs;
}

static Temp_tempList munchArgs(int* n, T_expList args) {
	if(!args)
		return NULL;
	++*n;
	return Temp_TempList(munchExp(args->head), munchArgs(n, args->tail));
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

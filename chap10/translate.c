#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"
#include "translate.h"

struct Tr_level_ {
	Tr_level parent; // The best method to code is to guess.
	F_frame frame;
};

struct Tr_access_ {
	Tr_level level;
	F_access access;
};

struct patchList_ {
	Temp_label* head; // pointer to pointer to Temp_label_
	patchList tail;
};

struct Cx {
	T_stm stm;
	patchList trues;
	patchList falses;
};

struct Tr_exp_ {
	enum {Tr_ex, Tr_nx, Tr_cx} kind;
	union {T_exp ex; T_stm nx; struct Cx cx;} u;
};

static F_fragList fList = NULL, last = NULL;

static Tr_access Tr_Access(Tr_level level, F_access f_access);
static patchList PatchList(Temp_label* head, patchList tail);
static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(T_stm, patchList trues, patchList falses);
static T_exp unEx(Tr_exp e);
static T_stm unNx(Tr_exp e);
static struct Cx unCx(Tr_exp e);
static void doPatch(patchList pl, Temp_label label);

static Tr_access Tr_Access(Tr_level level, F_access f_access) {
	Tr_access tr_access = checked_malloc(sizeof(*tr_access));
	tr_access->level = level;
	tr_access->access = f_access;
	return tr_access;
}

static patchList PatchList(Temp_label* head, patchList tail) {
	patchList pl = checked_malloc(sizeof(*pl));
	pl->head = head;
	pl->tail = tail;
	return pl;
}

static Tr_exp Tr_Ex(T_exp ex) {
	Tr_exp e = checked_malloc(sizeof(*e));
	e->kind = Tr_ex;
	e->u.ex = ex;
	return e;
}

static Tr_exp Tr_Nx(T_stm nx) {
	Tr_exp e = checked_malloc(sizeof(*e));
	e->kind = Tr_nx;
	e->u.nx = nx;
	return e;
}


static Tr_exp Tr_Cx(T_stm stm, patchList trues, patchList falses) {
	Tr_exp e = checked_malloc(sizeof(*e));
	e->kind = Tr_cx;
	e->u.cx.trues = trues;
	e->u.cx.falses = falses;
	e->u.cx.stm = stm;
	return e;
}

static T_exp unEx(Tr_exp e) {
	switch(e->kind) {
		case Tr_ex:
			return e->u.ex;
		case Tr_nx:
			return T_Eseq(e->u.nx, T_Const(0));
		case Tr_cx: {
			Temp_temp r = Temp_newtemp();
			Temp_label t = Temp_newlabel(), f = Temp_newlabel();
			doPatch(e->u.cx.trues, t);
			doPatch(e->u.cx.falses, f);
			return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
						  T_Eseq(e->u.cx.stm,
						   T_Eseq(T_Label(f),
						    T_Eseq(T_Move(T_Temp(r), T_Const(0)),
						     T_Eseq(T_Label(t),
						             T_Temp(r))))));						
		}	
	}
}

static T_stm unNx(Tr_exp e) {
	switch(e->kind) {
		case Tr_nx:
			return e->u.nx;
		case Tr_ex:
			return T_Exp(e->u.ex);
		case Tr_cx: {
			// return T_Exp(unEx(e)); // Is it wise or stupid?
			// After all we would discard the value of Cx expression, so we do not
			// need to allocate a register and whatever the value of conditional
			// expression is, we always jump to the next instruction.
 
			Temp_label f = Temp_newlabel();
			doPatch(e->u.cx.trues, f);
			doPatch(e->u.cx.falses, f);
			return T_Seq(e->u.cx.stm,
			              T_Label(f));		
		}
	}
}

static struct Cx unCx(Tr_exp e) {
	switch(e->kind) {
		case Tr_cx:
			return e->u.cx;
		case Tr_ex: {
			// I have not yet conceived a solution to treat the cases of CONST 0 
			// and CONST 1 specially.
			
			// We could use Jump instead Cjump to resolve the case of CONST 0 and
			// CONST 1.

			struct Cx cx;
			
			if(e->u.ex->kind == T_CONST) {
				if(e->u.ex->u.CONST == 0) {
					cx.stm = T_Jump(T_Name(0), Temp_LabelList(0, 0));
					cx.trues = 0;
					cx.falses = PatchList(&cx.stm->u.JUMP.exp->u.NAME,
					             PatchList(&cx.stm->u.JUMP.jumps->head, 0));
					return cx;
				}
				if(e->u.ex->u.CONST == 1) {
					cx.stm = T_Jump(T_Name(0), Temp_LabelList(0, 0));
					cx.trues = PatchList(&cx.stm->u.JUMP.exp->u.NAME,
					             PatchList(&cx.stm->u.JUMP.jumps->head, 0));
					cx.falses = 0;
					return cx;
				}			
			}

			cx.stm = T_Cjump(T_ne, e->u.ex, T_Const(0), 0, 0);
			cx.trues = PatchList(&cx.stm->u.CJUMP.true, 0);
			cx.falses = PatchList(&cx.stm->u.CJUMP.false, 0);

			return cx;
		}
		case Tr_nx:
			assert(0);	
	}
}

static void doPatch(patchList pl, Temp_label label) {
	for(; pl; pl = pl->tail) 
		*pl->head = label;	
}

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
	Tr_accessList al = checked_malloc(sizeof(*al));
	al->head = head;
	al->tail = tail;
	return al;
}

Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail) {
	Tr_expList el = checked_malloc(sizeof(*el));
	el->head = head;
	el->tail = tail;
	return el;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
	Tr_level level = checked_malloc(sizeof(*level));
	level->parent = parent;
	level->frame = F_newFrame(name, U_BoolList(TRUE, formals));
	return level;
}

Tr_level Tr_outermost() {
	static Tr_level level = 0;
	if(!level) 
		level = Tr_newLevel(0, Temp_newlabel(), 0);
	return level;
}

Tr_accessList Tr_formals(Tr_level level) {
	// I hope the function must be called when level->frame->accessList is 
	// only constitude by parameters, which means the body of 
	// function corresponding to level->frame has not been resolved.
	
	F_accessList f_al = F_formals(level->frame);
	Tr_accessList dummy, rear;
	
	f_al = f_al->tail; // skip the static link parameter
	dummy = rear = Tr_AccessList(0, 0);

	while(f_al) {
		Tr_access tr_access = Tr_Access(level, f_al->head);
		rear = rear->tail = Tr_AccessList(tr_access, 0);
		f_al = f_al->tail;
	}

	return dummy->tail;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
	F_access f_access = F_allocLocal(level->frame, escape);
	return Tr_Access(level, f_access);
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level) {
	T_exp framePtr = T_Temp(F_FP());
	while(level != access->level) {
		framePtr = T_Mem(T_Binop(T_plus, framePtr, T_Const(F_staticLinkOffset)));
		level = level->parent;	
	}
	return Tr_Ex(F_Exp(access->access, framePtr));
}

Tr_exp Tr_subscriptVar(Tr_exp e, Tr_exp offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus,
	                    T_Binop(T_mul, 
	                     unEx(offset), T_Const(F_wordSize)), unEx(e))));
}

Tr_exp Tr_fieldVar(Tr_exp e, int offset) {
	return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(e), T_Const(F_wordSize * offset))));
}

Tr_exp Tr_nilExp() {
	return Tr_Ex(T_Const(0));
}

Tr_exp Tr_intExp(int intt) {
	return Tr_Ex(T_Const(intt));
}

Tr_exp Tr_opExp(A_oper oper, Tr_exp left, Tr_exp right) {
	switch(oper) {
		case A_plusOp:
			return Tr_Ex(T_Binop(T_plus, unEx(left), unEx(right)));
		case A_minusOp:
			return Tr_Ex(T_Binop(T_minus, unEx(left), unEx(right)));
		case A_timesOp:
			return Tr_Ex(T_Binop(T_mul, unEx(left), unEx(right)));
		case A_divideOp:	
			return Tr_Ex(T_Binop(T_div, unEx(left), unEx(right)));
		case A_eqOp: {
			T_stm stm = T_Cjump(T_eq, unEx(left), unEx(right), 0, 0);
			patchList trues = PatchList(&stm->u.CJUMP.true, 0);
			patchList falses = PatchList(&stm->u.CJUMP.false, 0);
			return Tr_Cx(stm, trues, falses);		
		}
		case A_neqOp: {
			T_stm stm = T_Cjump(T_ne, unEx(left), unEx(right), 0, 0);
			patchList trues = PatchList(&stm->u.CJUMP.true, 0);
			patchList falses = PatchList(&stm->u.CJUMP.false, 0);
			return Tr_Cx(stm, trues, falses);		
		}
		case A_ltOp: {
			T_stm stm = T_Cjump(T_lt, unEx(left), unEx(right), 0, 0);
			patchList trues = PatchList(&stm->u.CJUMP.true, 0);
			patchList falses = PatchList(&stm->u.CJUMP.false, 0);
			return Tr_Cx(stm, trues, falses);		
		}
		case A_leOp: {
			T_stm stm = T_Cjump(T_le, unEx(left), unEx(right), 0, 0);
			patchList trues = PatchList(&stm->u.CJUMP.true, 0);
			patchList falses = PatchList(&stm->u.CJUMP.false, 0);
			return Tr_Cx(stm, trues, falses);		
		}
		case A_gtOp: {
			T_stm stm = T_Cjump(T_gt, unEx(left), unEx(right), 0, 0);
			patchList trues = PatchList(&stm->u.CJUMP.true, 0);
			patchList falses = PatchList(&stm->u.CJUMP.false, 0);
			return Tr_Cx(stm, trues, falses);		
		}
		case A_geOp: {
			T_stm stm = T_Cjump(T_ge, unEx(left), unEx(right), 0, 0);
			patchList trues = PatchList(&stm->u.CJUMP.true, 0);
			patchList falses = PatchList(&stm->u.CJUMP.false, 0);
			return Tr_Cx(stm, trues, falses);		
		}
	}
}

Tr_exp Tr_seqExp(Tr_expList el) {
	if(!el) {
		// no value
		return Tr_Nx(T_Exp(T_Const(0)));	
	}
	if(!el->tail) 
		return el->head;

	T_stm prev = unNx(el->head), curr;
	Tr_exp exp = Tr_Ex(T_Eseq(prev, 0));
	T_stm *parent = &exp->u.ex->u.ESEQ.stm;
	
	el = el->tail;
	while(el->tail) {
		curr = unNx(el->head);
		*parent = T_Seq(prev, curr);
		parent = &(*parent)->u.SEQ.right;
		prev = curr;
		el = el->tail;
	}
	
	exp->u.ex->u.ESEQ.exp = unEx(el->head);
	if(el->head->kind == Tr_nx)
		return Tr_Nx(unNx(exp));
	else
		return exp;
}

Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee) {
	struct Cx testCx = unCx(test);
	Temp_label t = Temp_newlabel();
	Temp_label f = Temp_newlabel();
	doPatch(testCx.trues, t);
	doPatch(testCx.falses, f);

	// If there is no 'else' expression, then 'elsee' would be null.
	if(!elsee) {
		// If 'then' expression is a Cx expression, then actually I could 
		// discard the 'then' expression as whole 'if then' expression would not
		// produce value.
		// However, it is not necessary for us to consider such optimization
		// which would go against the intention of the source code in current
		// phase.
		// Therefore, I decided to describe the behavior of source code 
		// truthfully.

		if(then->kind == Tr_cx) {
			doPatch(then->u.cx.trues, f);
			doPatch(then->u.cx.falses, f);
			// Maybe label which is not followed by statement of expression is 
			// somewhat odd.
			return Tr_Nx(T_Seq(testCx.stm,
			                    T_Seq(T_Label(t),
			                           T_Seq(then->u.cx.stm,
			                                  T_Label(f)))));
		}
		// Even if 'then' expression would produce value, we still discard that
		// value.
		// Thereofre, I choose unNx() to strip out the 'then' expression. 
		else
			return Tr_Nx(T_Seq(testCx.stm,
			                    T_Seq(T_Label(t),
			                           T_Seq(unNx(then),
			                                  T_Label(f)))));
	} 
	else {
		// I would not think that whether the type of 'then' expression is
		// compatible with the type of 'else' expression, which is the 
		// responsiblity of the module 'semant'.
		
		// As I would not like to modify the code in 'semant.c', which only print
		// the error message when the type of 'then' expression is not compatible
		// with that of 'else' expression, then I modify the former condition
		// judgement slightly to filter the cases where 'then' expression or 
		// 'else' expression has been translated to 'Tr_nx' firstly to tolerate
		// erratic input as much as possible.
		if(then->kind == Tr_nx || elsee->kind == Tr_nx) {
			Temp_label z = Temp_newlabel(); // meeting point
			// If the module 'semant' take the responsiblity correctly, then
			// the 'else' expression must be a statement which produces no value. 
			return Tr_Nx(T_Seq(testCx.stm,
			              T_Seq(T_Label(t),
			               T_Seq(unNx(then),
			                T_Seq(T_Jump(T_Name(z), Temp_LabelList(z, 0)),
			                  T_Seq(T_Label(f),
			                   T_Seq(unNx(elsee),
			                    T_Label(z))))))));
		}
		Temp_temp r = Temp_newtemp(); // store the result aboout expression
		if(then->kind == Tr_ex && elsee->kind == Tr_ex) {
			Temp_label z = Temp_newlabel(); // meeting point
			return Tr_Ex(T_Eseq(testCx.stm,
			              T_Eseq(T_Label(t),
			               T_Eseq(T_Move(T_Temp(r), then->u.ex),
			                T_Eseq(T_Jump(T_Name(z), Temp_LabelList(z, 0)),
			                 T_Eseq(T_Label(f),
			                  T_Eseq(T_Move(T_Temp(r), elsee->u.ex),
			                   T_Eseq(T_Label(z), T_Temp(r)))))))));
		}
		// one of 'then' expression and 'else' expression is a Cx expression.
		Temp_label tt = Temp_newlabel(); 
		Temp_label ff = Temp_newlabel(); // provide for 'then' or 'else'
		T_stm thenStm, elseeStm; // the left child of Eseq node 
		if(then->kind == Tr_cx) {
			doPatch(then->u.cx.trues, tt);
			doPatch(then->u.cx.falses, ff);
			thenStm = then->u.cx.stm;
		}
		else
			thenStm = T_Move(T_Temp(r), then->u.ex);
		if(elsee->kind == Tr_cx) {
			doPatch(elsee->u.cx.trues, tt);
			doPatch(elsee->u.cx.falses, ff);
			elseeStm = elsee->u.cx.stm;		
		}
		else
			elseeStm = T_Move(T_Temp(r), elsee->u.ex);
		return Tr_Ex(T_Eseq(T_Move(T_Temp(r), T_Const(1)),
		              T_Eseq(testCx.stm,
		               T_Eseq(T_Label(t),
		                T_Eseq(thenStm,
		                 T_Eseq(T_Jump(T_Name(tt), Temp_LabelList(tt, 0)),
		                  T_Eseq(T_Label(f),
		                   T_Eseq(elseeStm,
		                    T_Eseq(T_Jump(T_Name(tt), Temp_LabelList(tt, 0)),
		                     T_Eseq(T_Label(ff),
		                      T_Eseq(T_Move(T_Temp(r), T_Const(0)),
		                       T_Eseq(T_Label(tt),
		                               T_Temp(r)))))))))))));
	}
}

Tr_exp Tr_stringExp(string literal) {
	Temp_label label = Temp_newlabel();
	F_frag stringFrag = F_StringFrag(label, literal);
	if(!last)
		fList = last = F_FragList(stringFrag, NULL);
	else
		last = last->tail = F_FragList(stringFrag, NULL);	
	return Tr_Ex(T_Name(label));
}

Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init) {
	Temp_temp r = Temp_newtemp();
	return Tr_Ex(T_Eseq(T_Seq(T_Move(T_Temp(r),
	                           F_externalCall("malloc",	
	                            T_ExpList(T_Binop(T_mul,
	                                              unEx(size),
	                                              T_Const(F_wordSize)), 0))),
	                     T_Exp(F_externalCall("initArray",
	                                    T_ExpList(T_Temp(r),
	                                     T_ExpList(unEx(size),
	                                      T_ExpList(unEx(init), 0)))))),
	                    T_Temp(r)));	                                 
}

Tr_exp Tr_recordExp(int size, Tr_expList el) {
	// 'el' may be null, 'size' means the number of fields in record.
	// If 'el' is not null, then the module 'semant' must assure that the 
	// order of 'el' must correspond to the order of fieldList used during 
	// declaring record type. 
	
	Temp_temp r = Temp_newtemp();
	T_exp exp = T_Eseq(T_Move(T_Temp(r),
	                    F_externalCall("malloc",
	                     T_ExpList(T_Const(size * F_wordSize), 0))),
	                   T_Temp(r));
	T_stm *parent = &exp->u.ESEQ.stm;
	T_stm prev = *parent, curr;
	for(int i = 0; el; el = el->tail, ++i) {
		curr = T_Move(T_Mem(T_Binop(T_plus, 
		                     T_Temp(r), T_Const(i * F_wordSize))),
		              unEx(el->head));
		*parent = T_Seq(prev, curr);
		parent = &(*parent)->u.SEQ.right;
		prev = curr;	
	}
	return Tr_Ex(exp);
}

Tr_exp Tr_whileExp(Tr_exp test, Tr_exp body, Temp_label done) {
	Temp_label te = Temp_newlabel(); // the label about 'test' expression
	Temp_label bo = Temp_newlabel(); // the label about 'body' expression
	struct Cx testCx = unCx(test);
	doPatch(testCx.trues, bo);
	doPatch(testCx.falses, done);

	return Tr_Nx(T_Seq(T_Label(te),
	              T_Seq(testCx.stm,
	               T_Seq(T_Label(bo),
	                T_Seq(unNx(body),
	                 T_Seq(T_Jump(T_Name(te), Temp_LabelList(te, 0)),
	                  T_Label(done)))))));
}

Tr_exp Tr_forExp(Tr_exp var, Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label done) {
	Temp_label te = Temp_newlabel(); // the label about 'test' expression
	Temp_label bo = Temp_newlabel(); // the label about 'body' expression
	Temp_label inc = Temp_newlabel(); // the label about increment

	return Tr_Nx(T_Seq(T_Move(unEx(var), unEx(lo)),
	              T_Seq(T_Label(te),
	               T_Seq(T_Cjump(T_le, unEx(var), unEx(hi), bo, done),
	                T_Seq(T_Label(bo),
	                 T_Seq(unNx(body),
	                  T_Seq(T_Cjump(T_lt, unEx(var), unEx(hi), inc, done),
	                   T_Seq(T_Label(inc),
	                    T_Seq(T_Move(unEx(var), 
	                           T_Binop(T_plus, unEx(var), T_Const(1))),
	                     T_Seq(T_Jump(T_Name(bo), Temp_LabelList(bo, 0)),
	                      T_Label(done)))))))))));
}

Tr_exp Tr_breakExp(Temp_label done) {
	return Tr_Nx(T_Jump(T_Name(done), Temp_LabelList(done, 0)));
}

Tr_exp Tr_callExp(Temp_label label, Tr_level caller, Tr_level callee, Tr_expList args, bool hasReturn) {
	// <1> callee -> caller -> ... -> outmostLevel
	// <2> caller(callee) -> ... -> outmostLevel
	// <3> caller -> ... -> callee -> ... -> outmostLevel
	// <4> caller 
	//						\
	//             -> parentLevel -> ... -> outmostLevel
	//            /
	//     callee
	// <5> caller -> parentLevel
	//                           \
	//                            -> otherLevl -> ... -> outmostLevel
	//                           /
	//                    callee
	// There exists no other cases because of the access scope of function.
	// In fact, all the cases discussed could be resolved in an while loop
	// elegantly.
	
	T_exp static_link = T_Temp(F_FP());
	T_expList front, rear;
	T_exp callExp;

	while(caller != callee->parent) {
		static_link = T_Mem(T_Binop(T_plus, static_link, T_Const(F_staticLinkOffset)));
		caller = caller->parent;	
	}

	front = rear = T_ExpList(static_link, 0);
	while(args) {
		T_expList el = T_ExpList(unEx(args->head), 0);
		rear->tail = el;
		rear = el;
		args = args->tail;	
	}
	
	// As we all know, the function call does not always return a value.
	// I noticed such an ordinary knowledge again for the reason that I 
	// encountered such a Tiger language code segmant like "if 1 then print
	// ("\n") else ()".
	// "print("\n")" in 'then' expression would produce no value so do "()" in 
	// the 'else' expression.
	// However, no matter whether the function call would return a value I 
	// always return a Tr_Ex(...) when resolving the call expression.
	// Therefore, during translating the 'if' expression, 'then' expression 
	// would be considered as a expression and 'else' expression would be 
	// considered as a statement.
	// But the function designed to translate the 'if' expression could not
	// resolve such case as I think that we would never meet such case if
	// our type checker executes correctly.
	// But the irnoy is that the above Tiger code segamnt would pass the type 
	// check and may generate a destructive bug during the construction
	// of IR tree thanks to wrong usage about union.
	// So we must add a new parameter named 'hasReturn' to be aware of 
	// whether the function would return a value and adjust the IR tree.
	// Also, we would meet the same bug during translating the sequence 
	// expression.
	// However, if the type of 'then' expression does not compatible with the 
	// type of 'else' expression in the 'if' expression indeed, the function
	// about translating the 'if' expression would also meet the case which
	// it can not handle with.
	// Therefore, the solution discussed above would only eliminate such 
	// embarrassment while semantically analzing the code which would pass the 	 // type check.
	// The fundamental solution is to redesign the function used to translating
	// the 'if' expression.
	// However, I have no such a good mood to think that temporarily, which 
	// seems somewhat meaningless.

	callExp = T_Call(T_Name(label), front);
	if(hasReturn)	
		return Tr_Ex(callExp);
	else
		return Tr_Nx(T_Exp(callExp));
}

Tr_exp Tr_assignExp(Tr_exp var, Tr_exp exp) {
	return Tr_Nx(T_Move(unEx(var), unEx(exp)));
}

F_fragList Tr_getResult() {
	return fList;
}

Tr_exp Tr_letExp(Tr_expList el) {
	return Tr_seqExp(el); // a redundant function
}

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals) {
	// How should I do to resolve the 'formals'?
	T_stm stm = F_procEntryExit1(level->frame, 
	             T_Move(T_Temp(F_RV()), unEx(body))); // move the result into rv
	F_frag procFrag = F_ProcFrag(stm, level->frame);
	if(!last)
		fList = last = F_FragList(procFrag, NULL);
	else
		last = last->tail = F_FragList(procFrag, NULL);	
}

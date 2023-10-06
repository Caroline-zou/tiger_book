#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"
#include "frame.h"

#define PARA_REG_NUM 4
#define SAVE_REG_NUM 8
#define TEMP_REG_NUM 10

const int F_wordSize = 4;
const int F_staticLinkOffset = 0;

struct F_frame_ {
	Temp_label label; // the label which indicates the begining address of 
											// function
	F_accessList formals; // indicate the location about parameters 
	int stackSize; // indicate the size about stack frame	
};

struct F_access_ {
	enum {inFrame, inReg} kind;
	union {
		int offset;
		Temp_temp reg;	
	} u;
};

static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);
static F_accessList F_AccessList(F_access head, F_accessList tail);

static F_access InFrame(int offset) {
	F_access access = checked_malloc(sizeof(*access));
	access->kind = inFrame;
	access->u.offset = offset;
	return access;
}

static F_access InReg(Temp_temp reg) {
	F_access access = checked_malloc(sizeof(*access));
	access->kind = inReg;
	access->u.reg = reg;
	return access;
}

static F_accessList F_AccessList(F_access head, F_accessList tail) {
	F_accessList al = checked_malloc(sizeof(*al));
	al->head = head;
	al->tail = tail;
	return al;
}

F_frame F_newFrame(Temp_label name, U_boolList formals) {
	F_frame frame = checked_malloc(sizeof(*frame));
	U_boolList bl = formals;
	F_accessList dummy, rear;
	int para_count = 0;

	frame->label = name;
	frame->stackSize = 0;
	dummy = rear = F_AccessList(0, 0);

	// If I would like to optimize the allocation about local varible, I think
	// I should introduce a new field to record the rear part about 
	// accessList which would pollute the neat definition of structure 
	// F_frame.
	// Therefore, eventually I still apply the head insertion to construct
	// the accessList.
	
	// Since I noticed that it is not necessary to record the location about
	// local variable in structure Frame, which would be recorded in variable 
	// environment later.
	// Therefore, I modify the definition about structure Frame and for now we
	// only care about the locations about formal parameters.

	while(bl) {
		F_access ac;
		if(bl->head || para_count >= PARA_REG_NUM) 
			ac = InFrame(para_count * F_wordSize);
		else
			ac = InReg(Temp_newtemp());
		rear = rear->tail = F_AccessList(ac, 0);
		bl = bl->tail;
		++para_count;
	}

	frame->formals = dummy->tail;
	return frame;
}

Temp_label F_name(F_frame f) {
	return f->label;
}

F_accessList F_formals(F_frame f) {
	// The function caller must acquire a accessList whose order is somewhat
	// odd.
	// If the caller call F_formals(f) when f->accessList is only constitued
	// by parameters, the order about f->accessList is reverse order about
	// parameters.
	// Therefore, I hope the caller only call F_formals(f) in above case.

	// Since I have modified the definition about structure Frame, we would
	// assure that f->accessList is constitued by parameters at any time.
	return f->formals;
}

F_access F_allocLocal(F_frame f, bool escape) {
	F_access access;
	if(escape) {
		f->stackSize += F_wordSize;
		access = InFrame(-f->stackSize);
	}
	else
		access = InReg(Temp_newtemp());
	return access;
}

Temp_temp F_FP() {
	static Temp_temp fp = 0;
	if(!fp)
		fp = Temp_newtemp();
	return fp;
}

Temp_temp F_SP() {
	static Temp_temp sp = 0;
	if(!sp)
		sp = Temp_newtemp();
	return sp;
}

Temp_temp F_RV() {
	static Temp_temp rv = 0;
	if(!rv)
		rv = Temp_newtemp();
	return rv;
}


Temp_temp F_ZERO() {
	static Temp_temp zero = 0;
	if(!zero)
		zero = Temp_newtemp();
	return zero;
}

Temp_temp F_RA() {
	static Temp_temp ra = 0;
	if(!ra)
		ra = Temp_newtemp();
	return ra;
}

Temp_tempList F_PARAS() {
	static Temp_tempList paras = 0;
	if(!paras) {
		Temp_tempList rear;
		paras = rear = Temp_TempList(Temp_newtemp(), 0);
		for(int i = 1; i < PARA_REG_NUM; ++i) 
			rear = rear->tail = Temp_TempList(Temp_newtemp(), 0);
	}
	return paras;
}

Temp_tempList F_SAVES() {
	static Temp_tempList saves = 0;
	if(!saves) {
		Temp_tempList rear;
		saves = rear = Temp_TempList(Temp_newtemp(), 0);
		for(int i = 1; i < SAVE_REG_NUM; ++i) 
			rear = rear->tail = Temp_TempList(Temp_newtemp(), 0);
	}
	return saves;
}

Temp_tempList F_TEMPS() {
	static Temp_tempList temps = 0;
	if(!temps) {
		Temp_tempList rear;
		temps = rear = Temp_TempList(Temp_newtemp(), 0);
		for(int i = 1; i < TEMP_REG_NUM; ++i) 
			rear = rear->tail = Temp_TempList(Temp_newtemp(), 0);
	}
	return temps;
}

Temp_map F_TempMap() {
	static Temp_map map = 0;
	if(!map) {
		char buf[4];
		Temp_tempList temp;
		map = Temp_empty();
		Temp_enter(map, F_ZERO(), String("$zero"));
		Temp_enter(map, F_SP(), String("$sp"));
		Temp_enter(map, F_RA(), String("$ra"));
		Temp_enter(map, F_RV(), String("$v0"));

		temp = F_PARAS();
		for(int i = 0; temp; ++i, temp = temp->tail) {
			sprintf(buf, "$a%d", i);
			Temp_enter(map, temp->head, String(buf));
		}

		temp = F_SAVES();
		for(int i = 0; temp; ++i, temp = temp->tail) {
			sprintf(buf, "$s%d", i);
			Temp_enter(map, temp->head, String(buf));
		}

		temp = F_TEMPS();
		for(int i = 0; temp; ++i, temp = temp->tail) {
			sprintf(buf, "$t%d", i);
			Temp_enter(map, temp->head, String(buf));
		}
	}
	return map;
}

T_exp F_Exp(F_access access, T_exp framePtr) {
	if(access->kind == inReg)
		return T_Temp(access->u.reg);
	else
		return T_Mem(T_Binop(T_plus, framePtr, T_Const(access->u.offset)));
}

T_exp F_externalCall(string s, T_expList args) {
	return T_Call(T_Name(Temp_namedlabel(s)), args);
}

F_frag F_StringFrag(Temp_label label, string str) {
	F_frag frag = checked_malloc(sizeof(*frag));
	frag->kind = F_stringFrag;
	frag->u.stringg.label = label;
	frag->u.stringg.str = str;
	return frag;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
	F_frag frag = checked_malloc(sizeof(*frag));
	frag->kind = F_procFrag;
	frag->u.proc.body = body;
	frag->u.proc.frame = frame;
	return frag;
}

F_fragList F_FragList(F_frag head, F_fragList tail) {
	F_fragList fl = checked_malloc(sizeof(*fl));
	fl->head = head;
	fl->tail = tail;
	return fl;
}

T_stm F_procEntryExit1(F_frame frame, T_stm stm) {
	F_accessList access = frame->formals;
	Temp_tempList para = F_PARAS();
	T_exp framePtr = T_Temp(F_FP());
	T_stm ret, *stmRef; // stmRef is the pointer to pointer to 'stm'
	int i = 0; // trace the index of parameters

	if(!access)
		return stm; // no parameter

	ret = T_Seq(T_Move(F_Exp(access->head, framePtr), T_Temp(para->head)), stm);
	stmRef = &ret->u.SEQ.right;
	++i;
	para = para->tail;
	access = access->tail; // skip the first parameter
	for(; access; access = access->tail, ++i) {
		// Though I have given up the following method to implement the view of 
		// shif about parameters in the chapter 12, I still decide to fix the bug
		// which occurs in the following code since I hope the commit of each 
		// chapter is correct which has accomplished the task of corresponding 
		// chapter successfully.
		if(i < PARA_REG_NUM) {
			// parameters[i] was passed by parameter register
			//*stmRef = T_Seq(T_Move(F_Exp(access->head, framePtr), T_Temp(para->head)), stm);	

			// The above line only complished the view of shift about two parameters
			// if the value of pointer stmRef would never change.
			T_stm seq = T_Seq(T_Move(F_Exp(access->head, framePtr), T_Temp(para->head)), stm);	
			*stmRef = seq;
			stmRef = &seq->u.SEQ.right;
			para = para->tail;		
		}
		else {
			/*// parameters[i] was passed by memory
			if(access->head->kind == inReg)
				*stmRef = T_Seq(T_Move(F_Exp(access->head, framePtr), T_Temp(para->head)), stm);	
			// If access->kind was equal to InFrame, then the position of 
			// parameters[i] is the same in the perspective of both caller and 
			// callee.
			// Therefore, there is no need to move the memory.*/

			// The above code would seem somewhat meaningless as I never allocate a 
			// temporary for the parameter passed by stack.
			// Therefore, what we should do is to break the loop.
			break;
		}
	}
	
	return ret;	
}

AS_instrList F_procEntryExit2(AS_instrList body) {
	static Temp_tempList returnSink = 0;
	if(!returnSink)
		returnSink = Temp_TempList(F_ZERO(),
		              Temp_TempList(F_RA(),
		               Temp_TempList(F_SP(), F_SAVES())));
	return AS_splice(body, AS_InstrList(AS_Oper("", NULL, returnSink, NULL), NULL));
}

AS_proc F_procEntryExit3(F_frame frame, AS_instrList body) {
	char buf[100];
	sprintf(buf, "PROCEDURE %s\n", Temp_labelstring(frame->label));
	return AS_Proc(String(buf), body, "END\n");
}

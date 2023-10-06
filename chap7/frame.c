#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "tree.h"
#include "frame.h"

#define PARA_REG_NUM 4

const int F_wordSize = 4;

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

Temp_temp F_RV() {
	static Temp_temp rv = 0;
	if(!rv)
		rv = Temp_newtemp();
	return rv;
}

T_exp F_Exp(F_access access, T_exp framePtr) {
	if(access->kind == inReg)
		return T_Temp(access->u.reg);
	else
		return T_Mem(T_Binop(T_plus, T_Const(access->u.offset), framePtr));
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
	return stm;
}

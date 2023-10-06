#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "frame.h"

#define REG_SIZE 4
#define PARA_REG_NUM 4

struct F_frame_ {
	Temp_label address; // the label which indicates the begining address of 
											// function
	F_accessList accessList; // indicate the location about parameters and 
													 // local variables
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
	int para_count = 0;

	frame->address = name;
	frame->stackSize = 0;
	frame->accessList = 0;

	// If I would like to optimize the allocation about local varible, I think
	// I should introduce a new field to record the rear part about 
	// accessList which would pollute the neat definition of structure 
	// F_frame.
	// Therefore, eventually I still apply the head insertion to construct
	// the accessList.

	while(bl) {
		F_access access;
		if(bl->head || para_count >= PARA_REG_NUM) 
			access = InFrame(para_count * REG_SIZE);
		else
			access = InReg(Temp_newtemp());
		frame->accessList = F_AccessList(access, frame->accessList);
		bl = bl->tail;
		++para_count;
	}

	return frame;
}

Temp_label F_name(F_frame f) {
	return f->address;
}

F_accessList F_formals(F_frame f) {
	// The function caller must acquire a accessList whose order is somewhat
	// odd.
	// If the caller call F_formals(f) when f->accessList is only constitued
	// by parameters, the order about f->accessList is reverse order about
	// parameters.
	// Therefore, I hope the caller only call F_formals(f) in above case. 
	return f->accessList;
}

F_access F_allocLocal(F_frame f, bool escape) {
	F_access access;
	if(escape) {
		f->stackSize += REG_SIZE;	
		access = InFrame(-f->stackSize);
	}
	else
		access = InReg(Temp_newtemp());
	f->accessList = F_AccessList(access, f->accessList);
	return access;
}

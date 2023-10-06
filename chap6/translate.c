#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
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

static Tr_access Tr_Access(Tr_level level, F_access f_access) {
	Tr_access tr_access = checked_malloc(sizeof(*tr_access));
	tr_access->level = level;
	tr_access->access = f_access;
	return tr_access;
}

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
	Tr_accessList al = checked_malloc(sizeof(*al));
	al->head = head;
	al->tail = tail;
	return al;
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
	Tr_accessList tr_al = 0;

	// reverse the reverse order, then we acquire a forward order.
	while(f_al->tail) {
		Tr_access tr_access = Tr_Access(level, f_al->head);
		tr_al = Tr_AccessList(tr_access, tr_al);
		f_al = f_al->tail; 
	}

	return tr_al;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
	F_access f_access = F_allocLocal(level->frame, escape);
	return Tr_Access(level, f_access);
}

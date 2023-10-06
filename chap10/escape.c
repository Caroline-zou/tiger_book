#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "escape.h"

typedef struct escapeEntry_* escapeEntry;

struct escapeEntry_ {
	int depth;
	bool* escapePtr;
};

static escapeEntry EscapeEntry(int depth, bool* escapePtr);
static void traverseExp(S_table env, int depth, A_exp e);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);

static escapeEntry EscapeEntry(int depth, bool* escapePtr) {
	escapeEntry entry = checked_malloc(sizeof(*entry));
	entry->depth = depth;
	entry->escapePtr = escapePtr;
	return entry;
}

void Esc_findEscape(A_exp exp) {
	S_table env = S_empty();
	traverseExp(env, 0, exp);
}

// only resolve the case about nested function
static void traverseExp(S_table env, int depth, A_exp e) {
	switch(e->kind) {
		case(A_varExp): {
			traverseVar(env, depth, e->u.var);
			break;		
		}
		case(A_intExp):
		case(A_nilExp):
		case(A_stringExp): break;
		case(A_callExp): {
			for(A_expList el = e->u.call.args; el; el = el->tail) 
				traverseExp(env, depth, el->head);
			break;
		}	
		case(A_opExp): {
			traverseExp(env, depth, e->u.op.left);
			traverseExp(env, depth, e->u.op.right);
			break;
		}
		case(A_recordExp): {
			for(A_efieldList efl = e->u.record.fields; efl; efl = efl->tail) 
				traverseExp(env, depth, efl->head->exp);
			break;
		}
		case(A_seqExp): {
			for(A_expList el = e->u.seq; el; el = el->tail)
				traverseExp(env, depth, el->head);		
			break;
		}
		case(A_assignExp): {
				traverseExp(env, depth, e->u.assign.exp);
				break;		
		}
		case(A_ifExp): {
				traverseExp(env, depth, e->u.iff.test);
				traverseExp(env, depth, e->u.iff.then);
				if(e->u.iff.elsee)
					traverseExp(env, depth, e->u.iff.elsee); // avoid null pointer
																									 // exception
				break;		
		}
		case(A_whileExp): {
				traverseExp(env, depth, e->u.whilee.test);
				traverseExp(env, depth, e->u.whilee.body);
				break;		
		}
		case(A_forExp): {
			// Maybe we should consider for loop as a procedure?
			escapeEntry entry = EscapeEntry(depth, &(e->u.forr.escape));
			*entry->escapePtr = 0; // assume no escape
			traverseExp(env, depth, e->u.forr.lo);
			traverseExp(env, depth, e->u.forr.hi);
			S_beginScope(env);
			S_enter(env, e->u.forr.var, entry);			
			traverseExp(env, depth, e->u.forr.body);
			S_endScope(env);
			break;
		}
		case(A_letExp): {
			// Maybe we should consider let exp as a function?
			S_beginScope(env);
			for(A_decList dl = e->u.let.decs; dl; dl = dl->tail)
				traverseDec(env, depth, dl->head);
			traverseExp(env, depth, e->u.let.body);
			S_endScope(env);
			break;		
		}
		case(A_arrayExp): {
			traverseExp(env, depth, e->u.array.size);
			traverseExp(env, depth, e->u.array.init);
			break;
		}
	}
}

static void traverseVar(S_table env, int depth, A_var v) {
	switch(v->kind) {
		case(A_simpleVar): {
			escapeEntry entry = S_look(env, v->u.simple);
			if(entry && entry->depth < depth) 
				*entry->escapePtr = 1; // need to escape
			break;
		}
		case(A_fieldVar): {
			traverseVar(env, depth, v->u.field.var);
			break;
		}
		case(A_subscriptVar): {
			traverseVar(env, depth, v->u.subscript.var);
			traverseExp(env, depth,	v->u.subscript.exp);
			break;
		}
	}
}

static void traverseDec(S_table env, int depth, A_dec d) {
	switch(d->kind) {
		case(A_functionDec): {
			for(A_fundecList fdl = d->u.function; fdl; fdl = fdl->tail) {
				S_beginScope(env);
				for(A_fieldList fl = fdl->head->params; fl; fl = fl->tail) {
					escapeEntry entry = EscapeEntry(depth + 1, &fl->head->escape);
					*entry->escapePtr = 0; // assume no escape
					S_enter(env, fl->head->name, entry);
				}
				traverseExp(env, depth + 1, fdl->head->body);
				S_endScope(env);
			}
			break;
		}
		case(A_varDec): {
			escapeEntry entry = EscapeEntry(depth, &d->u.var.escape);
			*entry->escapePtr = 0; // assume no escape
			S_enter(env, d->u.var.var, entry); // enhance the environment
			traverseExp(env, depth, d->u.var.init);
			break;
		}
	}
}

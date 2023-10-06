#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "types.h"
#include "temp.h"
#include "absyn.h"
#include "translate.h"
#include "env.h"

E_enventry E_VarEntry(Tr_access access, Ty_ty ty) {
	E_enventry e = (E_enventry) checked_malloc(sizeof(*e));
	e->kind = E_varEntry;
	e->u.var.ty = ty;
	e->u.var.access = access;
	return e;
}

E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty result) {
	E_enventry e = (E_enventry) checked_malloc(sizeof(*e));
	e->kind = E_funEntry;
	e->u.fun.formals = formals;
	e->u.fun.result = result;
	e->u.fun.level = level;
	e->u.fun.label = label;
	return e;
}

S_table E_base_tenv() {
    // S_symbol -> Ty_ty 

	S_table table = S_empty();
    S_enter(table, S_Symbol("int"), Ty_Int());
	S_enter(table, S_Symbol("string"), Ty_String());
	return table;
}

S_table E_base_venv() {
	// S_symbol -> E_enventry

	S_table table; 
	Ty_tyList formals;	
	Ty_ty result;
	Tr_level level;
	Temp_label label;
	U_boolList boolList;

	table = S_empty();
	// print(s : string)
	formals = Ty_TyList(Ty_String(), 0);
	boolList = U_BoolList(TRUE, 0);
	result = Ty_Void();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("print"), E_FunEntry(level, label, formals, result));

	// Eventually I decided to use Ty_Void() to indicate the function 		
	// has no argument and to avoid the fact that the formals field of 
	// E_funEntry may be null though that would means I must add some 
	// condition judgement when resolving the call and declaration of 
	// function.
	// However, the code is not elegant when using Ty_Void() as we
	// had to add some condition judgement.
	// Finally, I still decided to use null to indicate the function has
	// no argument

	// flush()	
	formals = 0; // no argument 
	boolList = 0;
	result = Ty_Void();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("flush"), E_FunEntry(level, label, formals, result));
	// getchar() : string
	formals = 0; // no argument
	boolList = 0;
	result = Ty_String();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("getchar"), E_FunEntry(level, label, formals, result));
	// ord(s : string) : int
	formals = Ty_TyList(Ty_String(), 0);
	boolList = U_BoolList(TRUE, 0);
	result = Ty_Int();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("ord"), E_FunEntry(level, label, formals, result));
	// chr(i : int) : string
	formals = Ty_TyList(Ty_Int(), 0);
	boolList = U_BoolList(TRUE, 0);
	result = Ty_String();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("chr"), E_FunEntry(level, label, formals, result));
	// size(s : string) : int
	formals = Ty_TyList(Ty_String(), 0);
	boolList = U_BoolList(TRUE, 0);
	result = Ty_Int();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("size"), E_FunEntry(level, label, formals, result));
	// substring(s:string, first:int, n:int) : string
	formals = Ty_TyList(Ty_String(), Ty_TyList(Ty_Int(), Ty_TyList(Ty_Int(), 0)));
	boolList = U_BoolList(TRUE,
	            U_BoolList(TRUE,
	             U_BoolList(TRUE, 0)));
	result = Ty_String();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("substring"), E_FunEntry(level, label, formals, result));
	// concat(s1: string, s2: string) : string
	formals = Ty_TyList(Ty_String(), Ty_TyList(Ty_String(), 0));
	boolList = U_BoolList(TRUE,
	            U_BoolList(TRUE, 0));
	result = Ty_String();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("concat"), E_FunEntry(level, label, formals, result));
	// not(i : integer) : integer (integer?)

	// Let's assume 'integer' is alias about 'int'.
	// However, according to the assumption above, we should add a new
	// binding into the type environment or else the function 'not' would
	// contain a undeclared type parameter, which is so monstrous.
	// So let's substitute 'integer' with 'int' temporarily to avoid above
	// confusion.

	formals = Ty_TyList(Ty_Int(), 0);	
	boolList = U_BoolList(TRUE, 0);
	result = Ty_Int();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("not"), E_FunEntry(level, label, formals, result));
	// exit(i : int)
	formals = Ty_TyList(Ty_Int(), 0);
	boolList = U_BoolList(TRUE, 0);
	result = Ty_Void();
	label = Temp_newlabel();
	level = Tr_newLevel(Tr_outermost(), label, boolList); 
	S_enter(table, S_Symbol("exit"), E_FunEntry(level, label, formals, result));
	
	return table;
}

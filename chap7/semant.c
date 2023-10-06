#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "types.h"
#include "temp.h"
#include "translate.h"
#include "env.h"
#include "escape.h"
#include "semant.h"

//typedef struct expty_ *expty; // I think it is not necessary to use
                                // pointer.
 
typedef struct expty_ expty;

struct expty_ {
	Tr_exp exp; 
	Ty_ty ty;
};

expty ExpTy(Tr_exp exp, Ty_ty ty) {
	expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

//static int loop_level = 0; // trace the loop depth

static expty transVar(S_table venv, S_table tenv, Tr_level level, A_var v, Temp_label done);
static expty transExp(S_table venv, S_table tenv, Tr_level level, A_exp a, Temp_label done);
static Tr_exp transDec(S_table venv, S_table tenv, Tr_level level, A_dec d, Temp_label done);
static Ty_ty transTy(S_table tenv, A_ty a);
static Ty_ty actualTy(Ty_ty ty);
static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList fl);
static U_boolList makeBoolList(S_table tenv, A_fieldList fl);

F_fragList SEM_transProg(A_exp exp) {
	S_table tenv = E_base_tenv();
	S_table venv = E_base_venv();
	//Esc_findEscape(exp); // set the 'escape' field
	expty et = transExp(venv, tenv, Tr_outermost(), exp, 0);
	// Maybe overall program is a huge function fragment
	Tr_procEntryExit(Tr_outermost(), et.exp, 0); 
	return Tr_getResult();
}

static expty transExp(S_table venv, S_table tenv, Tr_level level, A_exp a, Temp_label done) {
	// Here I would make the type_checker proceed as much as possible 	
	// during which it could expose more type_error.

	switch(a->kind) {
		case(A_varExp): 
			return transVar(venv, tenv, level, a->u.var, done);
		case(A_nilExp): 
			return ExpTy(Tr_nilExp(), Ty_Nil());
		case(A_intExp):
			return ExpTy(Tr_intExp(a->u.intt), Ty_Int());
		case(A_stringExp):
			return ExpTy(Tr_stringExp(a->u.stringg), Ty_String());
		case(A_callExp): {
			E_enventry funEntry = S_look(venv, a->u.call.func);
			Ty_tyList tyList;
			A_expList a_expList;
			Tr_expList dummy, rear;
			Tr_exp callExp;
			bool hasReturn = TRUE;

			if(!funEntry || funEntry->kind != E_funEntry) {
				EM_error(a->pos, "undeclared function named %s", S_name(a->u.call.func));
				return ExpTy(Tr_intExp(-1), Ty_Void());			
			}
			// As I use Ty_void to indicate the function has no argument,
			// so I had to add some condition judgement here.
			// However, I still apply the former solution which uses null
			// to indicate the function has no argument.
			// Therefore, the redundant codes have been commented.

			/*if(x->u.fun.formals->head->kind == Ty_void) {
				if(!a->u.call.args)
					return ExpTy(0, actualTy(x->u.fun.result));
				else 
					tl = 0; // skip the error output "unmatched type"
			}
			else
				tl = x->u.fun.formals;*/
			
			dummy = rear = Tr_ExpList(0, 0);
			tyList = funEntry->u.fun.formals;
			a_expList = a->u.call.args;
			for(;tyList && a_expList; tyList = tyList->tail, a_expList = a_expList->tail) {
				expty argEt = transExp(venv, tenv, level, a_expList->head, done);
				Tr_expList tr_expList = Tr_ExpList(argEt.exp, 0);
				rear->tail = tr_expList;
				rear = tr_expList;

				// Accoring to the function definition, transExp() would return the 
				// structure expty whose 'ty' field is an actual type.
				// However, the type in type list of E_funEntry may be not an 
				// actual type.
				// Here, I decided to call actualTy() to ensure that both two type 
				// used for comparison are actual types, which would ease the 
				// burden of function transDec when resolving function declaration. 

				if(argEt.ty != actualTy(tyList->head)) 
					EM_error(a_expList->head->pos, "unmatched argument type");	
			}
 		
			if(tyList) 
				EM_error(a->pos, "too few arguments");
			if(a_expList) 
				EM_error(a->pos, "too many arguments");
			
			if(funEntry->u.fun.result->kind == Ty_void)
				hasReturn = FALSE;
			callExp = Tr_callExp(funEntry->u.fun.label, level, funEntry->u.fun.level, dummy->tail, hasReturn);
			return ExpTy(callExp, actualTy(funEntry->u.fun.result));
		}
		case(A_opExp): {
			expty leftEt, rightEt;
			Tr_exp opExp;
			leftEt = transExp(venv, tenv, level, a->u.op.left, done);
			rightEt = transExp(venv, tenv, level, a->u.op.right, done);
			opExp = Tr_opExp(a->u.op.oper, leftEt.exp, rightEt.exp);

			switch(a->u.op.oper) {
				case(A_ltOp):
				case(A_leOp):
				case(A_gtOp):
				case(A_geOp):
				case(A_divideOp):
				case(A_timesOp):
				case(A_minusOp):
				case(A_plusOp): {
					if(leftEt.ty->kind != Ty_int)	
						EM_error(a->u.op.left->pos, "integer required");
					if(rightEt.ty->kind != Ty_int)
						EM_error(a->u.op.right->pos, "integer required");
					return ExpTy(opExp, Ty_Int());
				}
				case(A_eqOp):
				case(A_neqOp): {
					if(leftEt.ty->kind == rightEt.ty->kind) {	
						// int == int or string == string
						if(leftEt.ty->kind == Ty_int || leftEt.ty->kind == Ty_string)	
							return ExpTy(opExp, Ty_Int());
						// record == record
						if(leftEt.ty->kind == Ty_record && leftEt.ty == rightEt.ty)
							return ExpTy(opExp, Ty_Int());
						// array == array
						if(leftEt.ty->kind == Ty_array && leftEt.ty == rightEt.ty)
							return ExpTy(opExp, Ty_Int());
						EM_error(a->u.op.left->pos, "incompatibale type");		
					}	
					// Actually I think both value about record and array are the 
					// reference, so I feel that the array could compare with nil.
					// However, I could not find such evidence to support my thought 
					// in the book where it only emphasized that the nil must be 
					// constrained by record type.
					// Eventually, I still decided to make the comparison between 
					// array and nill legal, which would not effect the correctness 
					// of the program.

					// record(array) == nil
					if((leftEt.ty->kind == Ty_record || leftEt.ty->kind == Ty_array) && rightEt.ty->kind == Ty_nil)
						return ExpTy(opExp, Ty_Int());
					// nil == record(array)
					if((rightEt.ty->kind == Ty_record || rightEt.ty->kind == Ty_array) && leftEt.ty->kind == Ty_nil)
						return ExpTy(opExp, Ty_Int());
					EM_error(a->u.op.left->pos, "incompatibale type");		
					return ExpTy(opExp, Ty_Int());
				}
			}
		}
		case(A_recordExp): {
			// firstly ignore a->u.record.fields
			// Since we apply the placeholder to resolve the recursive data
			// type, so all the type_id would bind with Ty_name in the
			// type environment except 'int' and 'string'.
			// Therefore, we had to call actualTy() to acquire the 
			// underlying type.

			Ty_ty recordTy = actualTy(S_look(tenv, a->u.record.typ));
			A_efieldList a_efieldList = a->u.record.fields;
			Ty_fieldList ty_fieldList;
			int size = 0;
			Tr_expList dummy, rear;

			if(!recordTy || recordTy->kind != Ty_record) {
				EM_error(a->pos,"undeclared record type named %s", S_name(a->u.record.typ));
				return ExpTy(Tr_recordExp(0, 0), Ty_Nil());
			}

			ty_fieldList = recordTy->u.record;
			dummy = rear = Tr_ExpList(0, 0);
			for(; ty_fieldList && a_efieldList; ty_fieldList = ty_fieldList->tail, a_efieldList = a_efieldList->tail, ++size) {
				expty et = transExp(venv, tenv, level, a_efieldList->head->exp, done);
				Tr_expList tr_expList = Tr_ExpList(et.exp, 0);
				if(a_efieldList->head->name != ty_fieldList->head->name) 
					EM_error(a_efieldList->head->exp->pos, "unknown field named %s", S_name(a_efieldList->head->name));											
				else {
					Ty_ty fieldTy = actualTy(ty_fieldList->head->ty);
					if((et.ty != fieldTy) &&
				     (et.ty->kind != Ty_nil || fieldTy->kind != Ty_record))  
						EM_error(a_efieldList->head->exp->pos, "the type of expression does not correspond to the type of %s.%s", S_name(a->u.record.typ), S_name(ty_fieldList->head->name));
				}	
				rear->tail = tr_expList;
				rear = tr_expList;
			}

			while(ty_fieldList) {
				ty_fieldList = ty_fieldList->tail;
				++size;			
			}

			return ExpTy(Tr_recordExp(size, dummy->tail), recordTy);
		}
		case(A_seqExp): {
			A_expList a_expList;
			Tr_expList dummy, rear;
 			expty et = ExpTy(0, Ty_Void());

			dummy = rear = Tr_ExpList(0, 0);
			for(a_expList = a->u.seq; a_expList; a_expList = a_expList->tail) {
				et = transExp(venv, tenv, level, a_expList->head, done);
				Tr_expList tr_expList = Tr_ExpList(et.exp, 0);
				rear->tail = tr_expList;
				rear = tr_expList;
			}

			return ExpTy(Tr_seqExp(dummy->tail), et.ty);
		}
		case(A_assignExp): {
			expty varEt = transVar(venv, tenv, level, a->u.assign.var, done);
			expty expEt = transExp(venv, tenv, level, a->u.assign.exp, done);
			Tr_exp assignExp = Tr_assignExp(varEt.exp, expEt.exp);

			if(varEt.ty == expEt.ty)
				return ExpTy(assignExp, Ty_Void());
			// record := nil
			if(varEt.ty->kind == Ty_record && expEt.ty->kind == Ty_nil)
				return ExpTy(assignExp, Ty_Void());
			EM_error(a->pos, "incompatible type");
			return ExpTy(assignExp, Ty_Void());
		}
		case(A_ifExp): {
			expty testEt, thenEt, elseEt;
			Tr_exp ifExp;
			testEt = transExp(venv, tenv, level, a->u.iff.test, done);
			thenEt = transExp(venv, tenv, level, a->u.iff.then, done);

			if(testEt.ty->kind != Ty_int)
				EM_error(a->u.iff.test->pos, "illegal condition expression");
			if(a->u.iff.elsee) {
				elseEt = transExp(venv, tenv, level, a->u.iff.elsee, done);
				ifExp = Tr_ifExp(testEt.exp, thenEt.exp, elseEt.exp);
				if(thenEt.ty == elseEt.ty)
					return ExpTy(ifExp, thenEt.ty);
				// record and nil
				if(thenEt.ty->kind == Ty_record && elseEt.ty->kind == Ty_nil) 
					return ExpTy(ifExp, thenEt.ty);
				// nil and record
				if(thenEt.ty->kind == Ty_nil && elseEt.ty->kind == Ty_record)	
					return ExpTy(ifExp, elseEt.ty);
				EM_error(a->pos, "the types about then and else expression are not the same");
				return ExpTy(ifExp, Ty_Void());
			}
			else {
				ifExp = Tr_ifExp(testEt.exp, thenEt.exp, 0);
				return ExpTy(ifExp, Ty_Void());
			}
		}
		case(A_whileExp): {
			expty testEt, bodyEt;
			Tr_exp whileExp;
			Temp_label nextDone = Temp_newlabel();

			testEt = transExp(venv, tenv, level, a->u.whilee.test, done);
			if(testEt.ty->kind != Ty_int) 
				EM_error(a->u.whilee.test->pos, "illegal condition expression");
			//++loop_level;
			bodyEt = transExp(venv, tenv, level, a->u.whilee.body, nextDone);	
			//--loop_level;
			if(bodyEt.ty->kind != Ty_void)
				EM_error(a->u.whilee.body->pos, "the loop body expression in while must produce no value");
			whileExp = Tr_whileExp(testEt.exp, bodyEt.exp, nextDone);
			return ExpTy(whileExp, Ty_Void());
		}
		case(A_forExp): {
			expty lowEt, highEt, bodyEt;
			Tr_access varAccess = Tr_allocLocal(level, a->u.forr.escape);
			Tr_exp forExp;
			Temp_label nextDone = Temp_newlabel();

			S_beginScope(venv);
			S_enter(venv, a->u.forr.var, E_VarEntry(varAccess, Ty_Int()));
			lowEt = transExp(venv, tenv, level, a->u.forr.lo, done);
			if(lowEt.ty->kind != Ty_int)
				EM_error(a->u.forr.lo->pos, "the lower expression must be integer");
			highEt = transExp(venv, tenv, level, a->u.forr.hi, done);
			if(highEt.ty->kind != Ty_int)
				EM_error(a->u.forr.hi->pos, "the upper expression must be integer");
			//++loop_level;
			bodyEt = transExp(venv, tenv, level, a->u.forr.body, nextDone);
			//--loop_level;
			if(bodyEt.ty->kind != Ty_void)
				EM_error(a->u.forr.body->pos, "the loop body expression in for must produce no value");
			S_endScope(venv);
			forExp = Tr_forExp(Tr_simpleVar(varAccess, level), lowEt.exp, highEt.exp, bodyEt.exp, nextDone);
			return ExpTy(forExp, Ty_Void());						
		}
		case(A_breakExp): {
			Tr_exp breakExp;

			if(!done) {
				EM_error(a->pos, "break expression must be included in for or while expression");
				breakExp = Tr_breakExp(Temp_namedlabel("Break Down!"));			
			}
			else
				breakExp = Tr_breakExp(done);
			
			return ExpTy(breakExp, Ty_Void());
		}
		case(A_letExp): {
			expty et;
			Tr_expList dummy, rear;
			dummy = rear = Tr_ExpList(0, 0);
			S_beginScope(venv);
			S_beginScope(tenv);
			for(A_decList decList = a->u.let.decs; decList; decList = decList->tail) {				
				Tr_exp exp = transDec(venv, tenv, level, decList->head, done);
				Tr_expList expList = Tr_ExpList(exp, 0);
				rear->tail = expList;
				rear = expList;
			}
			et = transExp(venv, tenv, level, a->u.let.body, done);
			S_endScope(tenv);
			S_endScope(venv);
			rear->tail = Tr_ExpList(et.exp, 0);
			et.exp = Tr_letExp(dummy->tail);
			return et;
		}
		case(A_arrayExp): {
			// I has ever believed that the "type-id" was to represent the type 
			// about each element in array util I browsed the Appendix A again.
			// In fact, I guess that the 'array_ty'->u.array was to represent the 
			// element type or else we would have no information to check the 
			// type of the initail expression with the element type in array.
 
			Ty_ty arrayTy, elemTy;
			expty sizeEt, initEt;
			
			// Since all the type-id would bind with Ty_name during the type
			// declaration except 'int' and 'string', so we had to call 
			// actualTy() to obtain the underlying type.

			arrayTy = actualTy(S_look(tenv, a->u.array.typ));
			if(!arrayTy || arrayTy->kind != Ty_array) {
				EM_error(a->pos, "undeclared array type named %s", S_name(a->u.array.typ));
				// pass Ty_Void() as second argument to avoid null-pointer 
				// exception.
				// However, I thought that the 'nil' would be better than 'void'.
				// However, nil must be constrained by record type or else I had to
				// check whether the value of array varibale is nil.
				// Therefore, I still decided to apply the Ty_Void();
 
				return ExpTy(Tr_arrayExp(0, 0), Ty_Void());			
			}
			sizeEt = transExp(venv, tenv, level, a->u.array.size, done);
			if(sizeEt.ty->kind != Ty_int) 
				EM_error(a->pos, "require integer to declare size");	
			initEt = transExp(venv, tenv, level, a->u.array.init, done);
			elemTy = actualTy(arrayTy->u.array);
			if(elemTy != initEt.ty) 
				EM_error(a->pos, "initial value does not correspond to element type");
			return ExpTy(Tr_arrayExp(sizeEt.exp, initEt.exp), arrayTy);
		}
	}
}

// I suspect that the argument 'tenv' is redundant as I never have chance to
// use 'tenv' during transVar().
// Actuallly, the 'tenv' is to call transExp() when resolving the subscript
// var.

static expty transVar(S_table venv, S_table tenv, Tr_level level, A_var v, Temp_label done) {
	switch(v->kind) {
		case(A_simpleVar): {
			E_enventry varEntry = S_look(venv, v->u.simple);
			if(varEntry && varEntry->kind == E_varEntry)
				return ExpTy(Tr_simpleVar(varEntry->u.var.access, level), actualTy(varEntry->u.var.ty));
			else {
				EM_error(v->pos, "undeclard variable named %s", S_name(v->u.simple));
				return ExpTy(Tr_intExp(-1), Ty_Void());
			}
		}
		case(A_fieldVar): {
			expty varEt = transVar(venv, tenv, level, v->u.field.var, done);
			int offset = 0;
			if(varEt.ty->kind != Ty_record) {
				EM_error(v->pos, "illegal record variable");
				return ExpTy(Tr_fieldVar(varEt.exp, -1), Ty_Void());
			}
			for(Ty_fieldList fieldList = varEt.ty->u.record; fieldList; fieldList = fieldList->tail, ++offset) {
				if(fieldList->head->name == v->u.field.sym)
					return ExpTy(Tr_fieldVar(varEt.exp, offset), actualTy(fieldList->head->ty));
			}
			EM_error(v->pos, "illegal field named %s", S_name(v->u.field.sym));
			return ExpTy(Tr_fieldVar(varEt.exp, -1), Ty_Void());
		}
		case(A_subscriptVar): {
			// If variable 'arr' is an array variable, then there exists such 
			// expression like "arr[break]".
			// In order to fetch the done label about nearest enclosing loop
			// during calling transExp upon the subscript of array, we had to
			// add a new formal parament named 'done' like transExp and transDec.
			// However, such expression must not pass the type check.

			expty varEt = transVar(venv, tenv, level, v->u.subscript.var, done);
			expty indexEt = transExp(venv, tenv, level, v->u.subscript.exp, done);
			if(varEt.ty->kind != Ty_array) {
				EM_error(v->pos, "illegal array variable");
				return ExpTy(Tr_subscriptVar(varEt.exp, Tr_intExp(-1)), Ty_Void());
			}
			if(indexEt.ty->kind != Ty_int) {
				EM_error(v->u.subscript.exp->pos, "illegal index");
				return ExpTy(Tr_subscriptVar(varEt.exp, Tr_intExp(-1)), Ty_Void());			
		  }
			return ExpTy(Tr_subscriptVar(varEt.exp, indexEt.exp), actualTy(varEt.ty->u.array));
		}	
	}
}

static Tr_exp transDec(S_table venv, S_table tenv, Tr_level level, A_dec d, Temp_label done) {
	switch(d->kind) {
		case(A_varDec): {
			Tr_access varAccess = Tr_allocLocal(level, d->u.var.escape);
			expty initEt = transExp(venv, tenv, level, d->u.var.init, done);
			if(d->u.var.typ) {
				Ty_ty varTy = actualTy(S_look(tenv, d->u.var.typ));
				if(initEt.ty == varTy)
					S_enter(venv, d->u.var.var, E_VarEntry(varAccess, varTy));		
				// var-id : record-type := nil 		
				else if(varTy->kind == Ty_record && initEt.ty->kind == Ty_nil)
					S_enter(venv, d->u.var.var, E_VarEntry(varAccess, varTy));
				else {
					EM_error(d->pos, "imcompatible type");
					S_enter(venv, d->u.var.var, E_VarEntry(varAccess, varTy));	
				}					
			}
			else {
				if(initEt.ty->kind != Ty_nil)
					S_enter(venv, d->u.var.var, E_VarEntry(varAccess, initEt.ty));
				else {
					EM_error(d->pos, "nil must be constrained by record type");				
					S_enter(venv, d->u.var.var, E_VarEntry(varAccess, Ty_Void()));
				}
			}
			return Tr_assignExp(Tr_simpleVar(varAccess, level), initEt.exp);
		}
		case(A_functionDec): {
			// I could not think a good method to detect function redeclaration
			// in the same consecutive batch of mutually recursive function.
			// The method I could consider is that to eliminate function 
			// redeclaration to accquire a neat funDecList.
			// However, that method would change the structure of abstract syntax
			// tree.
			// But maybe we would not have a chance to recur the abstract syntax.
			// Therefore, I think the method may work out though it is not 
			// elegant.

			for(A_fundecList p = d->u.function; p; p = p->tail) {
				A_fundecList q = p;
				while(q->tail) {
					if(q->tail->head->name == p->head->name) {
						EM_error(q->tail->head->pos, "function %s redeclaration", S_name(q->tail->head->name));
						q->tail = q->tail->tail;
					}
					else
						q = q->tail;				
				}
			}

			// During the pass we would not care the process of the body and
			// only bind the function-id with {result, tyList} in the venv.

			for(A_fundecList fdl = d->u.function; fdl; fdl = fdl->tail) {
				Ty_ty resultTy;
				Ty_tyList formalTyList;
				U_boolList boolList;
				Temp_label name = Temp_newlabel();
				Tr_level newLevel;

				if(!fdl->head->result)
					resultTy = Ty_Void();
				else
					resultTy = S_look(tenv, fdl->head->result);

				if(!resultTy) {
					EM_error(fdl->head->pos, "undeclared return type named %s", S_name(fdl->head->result));	
					resultTy = Ty_Void();			
				}		
				
				formalTyList = makeFormalTyList(tenv, fdl->head->params);
				boolList = makeBoolList(tenv, fdl->head->params);
				newLevel = Tr_newLevel(level, name, boolList);
				S_enter(venv, fdl->head->name, E_FunEntry(newLevel, name, formalTyList, resultTy));	
			}
			
			// We process the function body util the second pass.
			for(A_fundecList fdl = d->u.function; fdl; fdl = fdl->tail) {
				E_enventry funEntry;
				Ty_tyList tyList;
				Tr_accessList acList, formals;
				A_fieldList fieldList;
				expty et;
             		
				funEntry = S_look(venv, fdl->head->name);
				// acquire the symbol about parameters
				fieldList = fdl->head->params;
				// acquire the type about paramters 
				tyList = funEntry->u.fun.formals; 
				// acquire the access about parameters
				acList = formals = Tr_formals(funEntry->u.fun.level); 
				S_beginScope(venv);
				for(; tyList; fieldList = fieldList->tail, tyList = tyList->tail, acList = acList->tail) 
					S_enter(venv, fieldList->head->name, E_VarEntry(acList->head, tyList->head));
				et = transExp(venv, tenv, funEntry->u.fun.level, fdl->head->body, done);
        // test40.tig
        if(et.ty != actualTy(funEntry->u.fun.result))
					EM_error(fdl->head->pos, "the type about function body expression dose not match return type");
				S_endScope(venv);
				Tr_procEntryExit(funEntry->u.fun.level, et.exp, formals);
			}
			return Tr_intExp(0);
		}
		case(A_typeDec): {
			// First pass is to bind the type_id with Ty_Name(type_id, 0) whose 
			// ty field within the structure serves as a placeholder that would 
			// be complemented later, which would let the type environment be 
			// aware of the existence of type_id in the second pass. 

			for(A_nametyList ntl = d->u.type; ntl; ntl = ntl->tail) {
				// test38.tig
				Ty_ty ty = S_look(tenv, ntl->head->name);
					if(ty && ty->kind == Ty_name && !ty->u.name.ty) {
						EM_error(ntl->head->ty->pos, "there are more than one type with the same name %s", S_name(ntl->head->name));
					}
				else
					S_enter(tenv, ntl->head->name, Ty_Name(ntl->head->name, 0));
			}
			
			// During the second pass we would call transTy() to complement the 
			// placeholder.
			
			for(A_nametyList ntl = d->u.type; ntl; ntl = ntl->tail) {
				Ty_ty nameTy = S_look(tenv, ntl->head->name);
				Ty_ty targetTy = transTy(tenv, ntl->head->ty);
				// The type could not depend on itself directly.
				// However, I have a confusion whether the type declaration about 
				// "type a = array of a" is legal.
				// If we transfer the such type declaration in C language, that
				// would be like struct a {
				// 						     struct a* tmp[];
				//					     }, which is correct during compilation.
				// Therefore, I infer that the above type declaration may be legal 
				// in Tiger language.
				// I have an evidence to support the idea that the author has
				// written such a sentence "recursive type declaraions must pass 
				// through a record or array declartion.

				if(nameTy != targetTy) 
					nameTy->u.name.ty = targetTy;
				else {
					EM_error(ntl->head->ty->pos, "there exists an illegal cycle about type identifier %s", S_name(ntl->head->name));
					nameTy->u.name.ty = Ty_Void();
				}
			} 			
			return Tr_intExp(0);
		} 
	}
}

static Ty_ty transTy(S_table tenv, A_ty a) {
	switch(a->kind) {
		case(A_nameTy): {
			Ty_ty ty = S_look(tenv, a->u.name);
  			//Ty_ty temp_ty;
            
			if(!ty) {
				EM_error(a->pos, "undeclared type named %s", S_name(a->u.name));		
				return Ty_Void();			
			}

			if(ty->kind != Ty_name)
				return ty;
			
			//temp_ty = name_ty->u.name.ty;
            /*printf("%s->", S_name(a->u.name));
            while(temp_ty && temp_ty->kind == Ty_name) {
				printf("%s->", S_name(temp_ty->u.name.sym));
            	if(temp_ty->u.name.sym == a->u.name) {
					// Let us call 'a->u.name' as A and 'temp_ty->u.name.sym' as B.
					// As we all know, if a type structure whose kind field is 
					// Ty_name, then it means that the type must depend on other type 
					// which we could recur the ty field of the type structure to 
					// find.
					// Therefore, we could infer that type A depends on type B.
					// However, when temp_ty->u.name.sym is equal to a->u.name, it 
					// means that the type B depended by type A is type A actually. 
					// Then we could detect there exists an illegal cycles like 
					// A->......->A.  

					EM_error(a->pos, "there exists an illegal cycle about type identifier", S_name(a->u.name));		
					return Ty_Void();	       
				}
				temp_ty = temp_ty->u.name.ty;
            }
			printf("nil\n");*/
			
			// However, the above solution has made a fault that it has mistaken 
			// the symbol in name field within 'a' as the symbol that would be 
			// compared with each symbol within Ty_name structure in dependancy 	
			// Ty_name list.
			// In fact, the symbol that would be compared is not exposed to 
			// the transTy function in the term of argument.
			// Therefore, the above solution could not detect illegal cycle.
			// So, I have to conceive another solution.

			// The revised solution behaves like function actualTy() and would 
			// return the uyderlying type structure if the type structure bound 
			// to the symbol is Ty_name.
			// As we would not modify the function definition about transTy,
			// therefore, the function transDec would bear the responsibility to 
			// detect an illegal cycle for it has enough information about the 
			// type that would be declared.
			// Let us draw a diagram to illustrate the solution.
			// In the begining, there exists such type declarations below.

			// type a = b
			// type b = c
			// type c = a

			// Then the procedure about the typeDec would be like the below
			// diagram.

			// a <-> Type a --- 
			// 				  |
			// b <-> Type b <--
			//			| 			
			//			-------
			// 				  |
			// c <-> Type c <--
			//			^ |
			// 			| |
			// 			---

			// Firstly, we call transTy(b) and return the type b and assign type 
			// b into the ty field with type a.
			// Secondly, we call transTy(c) and return the type c and assign type 
			// c into the ty_field with type b.
			// Thirdly, we call transTy(a) and return type c and then we would 
			// find that the type c would depend on itself.
			// Therefore, we could use such solution to detect an illegal cycle.

			while(ty->u.name.ty) {
				ty = ty->u.name.ty;
				if(ty->kind != Ty_name)
					break;			
			}
			return ty;
		}
		case(A_recordTy): {
			A_fieldList a_fieldList;
			Ty_fieldList dummy, rear;

			dummy = rear = Ty_FieldList(0, 0);
			for(a_fieldList = a->u.record; a_fieldList; a_fieldList = a_fieldList->tail) {
				Ty_ty fieldTy = S_look(tenv, a_fieldList->head->typ);
				Ty_field ty_field;
				Ty_fieldList ty_fieldList;
				if(!fieldTy) {
					EM_error(a_fieldList->head->pos, "undeclared field type named %s", S_name(a_fieldList->head->typ));
					return Ty_Void();
				}
				ty_field = Ty_Field(a_fieldList->head->name, fieldTy);
				ty_fieldList = Ty_FieldList(ty_field, 0);
				rear->tail = ty_fieldList;
				rear = ty_fieldList;
			}
			return Ty_Record(dummy->tail);
		}
		case(A_arrayTy): {
			Ty_ty elemTy = S_look(tenv, a->u.array);

			if(!elemTy) {
				EM_error(a->pos, "undeclard element type named %s", S_name(a->u.array));
				return Ty_Void();
			}
			else
				return Ty_Array(elemTy);
		}	
	}
}

static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList fl) {
	Ty_tyList dummy, rear;
	
	// If fieldList is emtpy, return typeList with only a void type which
	// indicates the function has no arguments.
	// However, I have decided to abandon such solution as it is not elegant.

	dummy = rear = Ty_TyList(0, 0);
	for(; fl; fl = fl->tail) {
		Ty_ty ty = S_look(tenv, fl->head->typ);
		if(!ty) {
			EM_error(fl->head->pos, "undeclared argument type named %s", S_name(fl->head->typ));
			return 0;		
		}	
		Ty_tyList tyList = Ty_TyList(ty, 0);
		rear->tail = tyList;
		rear = tyList;
	}
	return dummy->tail;
}

static Ty_ty actualTy(Ty_ty ty) {
	if(!ty)
		return Ty_Void(); // When undeclared record(array)-type occurs, then we 											// would call actualTy(NULL) while resolving record
											// (array)Exp.
	else if(ty->kind == Ty_name) 
		return actualTy(ty->u.name.ty);
	else
		return ty;
}

static U_boolList makeBoolList(S_table tenv, A_fieldList fl) {
	U_boolList dummy, rear;
	dummy = rear= U_BoolList(TRUE, 0);
	for(; fl; fl = fl->tail) {
		U_boolList bl;
		Ty_ty ty = S_look(tenv, fl->head->typ);
		if(!ty) 
			return 0;		// coincide with makeFormalTyList 
		bl = U_BoolList(fl->head->escape, 0);
		rear->tail = bl;
		rear = bl;	
	}
	return dummy->tail;
}

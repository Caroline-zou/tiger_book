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

//typedef struct expty_ *expty; // I think it is not necessary to use
// pointer.

typedef struct expty_ expty;
typedef void *Tr_exp;

struct expty_ {
	Tr_exp exp; 
	Ty_ty ty;
};

expty expTy(Tr_exp exp, Ty_ty ty) {
	expty e;
	e.exp = exp;
	e.ty = ty;
	return e;
}

static int loop_level = 0; // trace the loop depth

static expty transVar(S_table venv, S_table tenv, Tr_level level, A_var v);
static expty transExp(S_table venv, S_table tenv, Tr_level level, A_exp a);
static void transDec(S_table venv, S_table tenv, Tr_level level, A_dec d);
static Ty_ty transTy(S_table tenv, A_ty a);
static Ty_ty actualTy(Ty_ty ty);
static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList fl);
static U_boolList makeBoolList(S_table tenv, A_fieldList fl);

void SEM_transProg(A_exp exp);

void SEM_transProg(A_exp exp) {
	S_table tenv = E_base_tenv();
	S_table venv = E_base_venv();
	Esc_findEscape(exp); // set the 'escape' field
	transExp(venv, tenv, Tr_outermost(), exp);
}

static expty transExp(S_table venv, S_table tenv, Tr_level level, A_exp a) {
	// Here I would make the type_checker proceed as much as possible 	
	// during which it could expose more type_error.

	switch(a->kind) {
		case(A_varExp): 
			return transVar(venv, tenv, level, a->u.var);
		case(A_nilExp): 
			return expTy(0, Ty_Nil());
		case(A_intExp):
			return expTy(0, Ty_Int());
		case(A_stringExp):
			return expTy(0, Ty_String());
		case(A_callExp): {
			E_enventry x = S_look(venv, a->u.call.func);
			Ty_tyList tl;
			A_expList el;
			
			if(!x || x->kind != E_funEntry) {
				EM_error(a->pos, "undeclared function named %s", S_name(a->u.call.func));
				return expTy(0, Ty_Void());			
			}
			// As I use Ty_void to indicate the function has no argument,
			// so I had to add some condition judgement here.
			// However, I still apply the former solution which uses null
			// to indicate the function has no argument.
			// Therefore, the redundant codes have been commented.

			/*if(x->u.fun.formals->head->kind == Ty_void) {
				if(!a->u.call.args)
					return expTy(0, actualTy(x->u.fun.result));
				else 
					tl = 0; // skip the error output "unmatched type"
			}
			else
				tl = x->u.fun.formals;*/

			for(tl = x->u.fun.formals, el = a->u.call.args; tl && el; tl = tl->tail, el = el->tail) {
				expty arg_ty = transExp(venv, tenv, level, el->head);
				// Accoring to the function definition, transExp() would return the 
				// structure expty whose 'ty' field is an actual type.
				// However, the type in type list of E_funEntry may be not an 
				// actual type.
				// Here, I decided to call actualTy() to ensure that both two type 
				// used for comparison are actual types, which would ease the 
				// burden of function transDec when resolving function declaration. 

				if(arg_ty.ty != actualTy(tl->head)) 
					EM_error(el->head->pos, "unmatched argument type");	
			} 		
			if(tl) 
				EM_error(a->pos, "too few arguments");
			if(el) 
				EM_error(a->pos, "too many arguments");
			return expTy(0, actualTy(x->u.fun.result));
		}
		case(A_opExp): {
			expty left, right;
			left = transExp(venv, tenv, level, a->u.op.left);
			right = transExp(venv, tenv, level, a->u.op.right);
			switch(a->u.op.oper) {
				case(A_ltOp):
				case(A_leOp):
				case(A_gtOp):
				case(A_geOp):
				case(A_divideOp):
				case(A_timesOp):
				case(A_minusOp):
				case(A_plusOp): {
					if(left.ty->kind != Ty_int)	
						EM_error(a->u.op.left->pos, "integer required");
					if(right.ty->kind != Ty_int)
						EM_error(a->u.op.right->pos, "integer required");
					return expTy(0, Ty_Int());
				}
				case(A_eqOp):
				case(A_neqOp): {
					if(left.ty->kind == right.ty->kind) {	
						// int == int or string == string
						if(left.ty->kind == Ty_int || left.ty->kind == Ty_string)	
							return expTy(0, Ty_Int());
						// record == record
						if(left.ty->kind == Ty_record && left.ty == right.ty)
							return expTy(0, Ty_Int());
						// array == array
						if(left.ty->kind == Ty_array && left.ty == right.ty)
							return expTy(0, Ty_Int());
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
					if((left.ty->kind == Ty_record || left.ty->kind == Ty_array) && right.ty->kind == Ty_nil)
						return expTy(0, Ty_Int());
					// nil == record(array)
					if((right.ty->kind == Ty_record || right.ty->kind == Ty_array) && left.ty->kind == Ty_nil)
						return expTy(0, Ty_Int());
					EM_error(a->u.op.left->pos, "incompatibale type");		
					return expTy(0, Ty_Int());
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

			Ty_ty typ_ty = actualTy(S_look(tenv, a->u.record.typ));
			if(!typ_ty || typ_ty->kind != Ty_record) {
				EM_error(a->pos,"undeclared record type named %s", S_name(a->u.record.typ));
				return expTy(0, Ty_Nil());
			}
			else {
				A_efieldList efl;
				Ty_fieldList fl;

				for(efl = a->u.record.fields; efl; efl = efl-> tail){
					for(fl = typ_ty->u.record; fl; fl = fl->tail) {
						// field exists in record type
						if(fl->head->name == efl->head->name) {
							expty exp_ty = transExp(venv, tenv, level, efl->head->exp);
							Ty_ty field_ty = actualTy(fl->head->ty);
							if(exp_ty.ty == field_ty)
								break;
							if(exp_ty.ty->kind == Ty_nil && field_ty->kind == Ty_record)
								break;
							else {
								EM_error(efl->head->exp->pos, "the type about expression does not correspond to the type of field %s with record type %s", S_name(fl->head->name), S_name(a->u.record.typ));
								break;
							}
						}
					}
					// field does not exist in type record
					if(!fl)
						EM_error(efl->head->exp->pos, "unknown filed named %s", S_name(efl->head->name));				
				}
				return expTy(0, typ_ty);
			}
		}
		case(A_seqExp): {
			A_expList el;

			if(!a->u.seq)
				return expTy(0, Ty_Void());
			for(el = a->u.seq; el->tail; el = el->tail) 
				transExp(venv, tenv, level, el->head);
			return transExp(venv, tenv, level, el->head);
		}
		case(A_assignExp): {
			expty var_ty = transVar(venv, tenv, level, a->u.assign.var);
			expty exp_ty = transExp(venv, tenv, level, a->u.assign.exp);
			if(var_ty.ty == exp_ty.ty)
				return expTy(0, Ty_Void());
			// record := nil
			if(var_ty.ty->kind == Ty_record && exp_ty.ty->kind == Ty_nil)
				return expTy(0, Ty_Void());
			EM_error(a->pos, "incompatible type");
			return expTy(0, Ty_Void());
		}
		case(A_ifExp): {
			expty test_ty, exp1_ty, exp2_ty;
			test_ty = transExp(venv, tenv, level, a->u.iff.test);
			if(test_ty.ty->kind != Ty_int)
				EM_error(a->u.iff.test->pos, "illegal condition expression");
			if(a->u.iff.elsee) {
				exp1_ty = transExp(venv, tenv, level, a->u.iff.then);
				exp2_ty = transExp(venv, tenv, level, a->u.iff.elsee);
				if(exp1_ty.ty == exp2_ty.ty)
					return expTy(0, exp1_ty.ty);
				// record and nil
				if(exp1_ty.ty->kind == Ty_record && exp2_ty.ty->kind == Ty_nil) 
					return expTy(0, exp1_ty.ty);
				// nil and record
				if(exp1_ty.ty->kind == Ty_nil && exp2_ty.ty->kind == Ty_record)	
					return expTy(0, exp2_ty.ty);
				EM_error(a->pos, "the types about then and else expression are not the same");
				return expTy(0, Ty_Void());
			}
			else {
				exp1_ty = transExp(venv, tenv, level, a->u.iff.then);
				return expTy(0, Ty_Void());
			}
		}
		case(A_whileExp): {
			expty test_ty, body_ty;
			test_ty = transExp(venv, tenv, level, a->u.whilee.test);
			if(test_ty.ty->kind != Ty_int) 
				EM_error(a->u.whilee.test->pos, "illegal condition expression");
			++loop_level;
			body_ty = transExp(venv, tenv, level, a->u.whilee.body);	
			--loop_level;
			if(body_ty.ty->kind != Ty_void)
				EM_error(a->u.whilee.body->pos, "the loop body expression in while must produce no value");
			return expTy(0, Ty_Void());
		}
		case(A_forExp): {
			expty low_ty, high_ty, body_ty;
			Tr_access access = Tr_allocLocal(level, a->u.forr.escape);
			S_beginScope(venv);
			S_enter(venv, a->u.forr.var, E_VarEntry(access, Ty_Int()));
			low_ty = transExp(venv, tenv, level, a->u.forr.lo);
			if(low_ty.ty->kind != Ty_int)
				EM_error(a->u.forr.lo->pos, "the lower expression must be integer");
			high_ty = transExp(venv, tenv, level, a->u.forr.hi);
			if(high_ty.ty->kind != Ty_int)
				EM_error(a->u.forr.hi->pos, "the upper expression must be integer");
			++loop_level;
			body_ty = transExp(venv, tenv, level, a->u.forr.body);
			--loop_level;
			if(body_ty.ty->kind != Ty_void)
				EM_error(a->u.forr.body->pos, "the loop body expression in for must produce no value");
			S_endScope(venv);
			return expTy(0, Ty_Void());						
		}
		case(A_breakExp): {
			if(!loop_level)
				EM_error(a->pos, "break expression must be included in for or while expression");
			return expTy(0, Ty_Void());
		}
		case(A_letExp): {
			expty exp_ty;
			S_beginScope(venv);
			S_beginScope(tenv);
			for(A_decList dl = a->u.let.decs; dl; dl = dl->tail)
				transDec(venv, tenv, level, dl->head);
			exp_ty = transExp(venv, tenv, level, a->u.let.body);
			S_endScope(tenv);
			S_endScope(venv);
			return exp_ty;
		}
		case(A_arrayExp): {
			// I has ever believed that the "type-id" was to represent the type 
			// about each element in array util I browsed the Appendix A again.
			// In fact, I guess that the 'array_ty'->u.array was to represent the 
			// element type or else we would have no information to check the 
			// type of the initail expression with the element type in array.
 
			Ty_ty array_ty; 
			expty size_ty, init_ty;
			
			// Since all the type-id would bind with Ty_name during the type
			// declaration except 'int' and 'string', so we had to call 
			// actualTy() to obtain the underlying type.

			array_ty = actualTy(S_look(tenv, a->u.array.typ));
			if(!array_ty || array_ty->kind != Ty_array) {
				EM_error(a->pos, "undeclared array type named %s", S_name(a->u.array.typ));
				// pass Ty_Void() as second argument to avoid null-pointer 
				// exception.
				// However, I thought that the 'nil' would be better than 'void'.
				// However, nil must be constrained by record type or else I had to
				// check whether the value of array varibale is nil.
				// Therefore, I still decided to apply the Ty_Void();
 
				return expTy(0, Ty_Void());			
			}
			size_ty = transExp(venv, tenv, level, a->u.array.size);
			if(size_ty.ty->kind != Ty_int) 
				EM_error(a->pos, "require integer to declare size");	
			init_ty = transExp(venv, tenv, level, a->u.array.init);
			if(actualTy(array_ty->u.array) != init_ty.ty) 
				EM_error(a->pos, "initial value does not correspond to element type");
			return expTy(0, array_ty);
		}
	}
}

// I suspect that the argument 'tenv' is redundant as I never have chance to
// use 'tenv' during transVar().

static expty transVar(S_table venv, S_table tenv, Tr_level level, A_var v) {
	switch(v->kind) {
		case(A_simpleVar): {
			E_enventry x = S_look(venv, v->u.simple);
			if(x && x->kind == E_varEntry)
				return expTy(0, actualTy(x->u.var.ty));
			else {
				EM_error(v->pos, "undeclard variable named %s", S_name(v->u.simple));
				return expTy(0, Ty_Void());
			}
		}
		case(A_fieldVar): {
			expty var_ty = transVar(venv, tenv, level, v->u.field.var);
			if(var_ty.ty->kind != Ty_record) {
				EM_error(v->pos, "illegal record variable");
				return expTy(0, Ty_Void());
			}
			for(Ty_fieldList fl = var_ty.ty->u.record; fl; fl = fl->tail) {
				if(fl->head->name == v->u.field.sym)
					return expTy(0, actualTy(fl->head->ty));
			}
			EM_error(v->pos, "illegal field");
			return expTy(0, Ty_Void());
		}
		case(A_subscriptVar): {
			expty var_ty = transVar(venv, tenv, level, v->u.subscript.var);
			if(var_ty.ty->kind != Ty_array) {
				EM_error(v->pos, "illegal array variable");
				return expTy(0, Ty_Void());
			}
			return expTy(0, actualTy(var_ty.ty->u.array));
		}	
	}
}

static void transDec(S_table venv, S_table tenv, Tr_level level, A_dec d) {
	switch(d->kind) {
		case(A_varDec): {
			Tr_access access = Tr_allocLocal(level, d->u.var.escape);
			if(d->u.var.typ) {
				expty init_ty = transExp(venv, tenv, level, d->u.var.init);
				Ty_ty typ_ty = actualTy(S_look(tenv, d->u.var.typ));
				if(init_ty.ty == typ_ty)
					S_enter(venv, d->u.var.var, E_VarEntry(access, typ_ty));		
				// var-id : record-type := nil 		
				else if(typ_ty->kind == Ty_record && init_ty.ty->kind == Ty_nil)
					S_enter(venv, d->u.var.var, E_VarEntry(access, typ_ty));
				else {
					EM_error(d->pos, "imcompatible type");
					S_enter(venv, d->u.var.var, E_VarEntry(access, typ_ty));	
				}					
			}
			else {
				expty init_ty = transExp(venv, tenv, level, d->u.var.init);
				if(init_ty.ty->kind != Ty_nil)
					S_enter(venv, d->u.var.var, E_VarEntry(access, init_ty.ty));
				else {
					EM_error(d->pos, "nil must be constrained by record type");				
					S_enter(venv, d->u.var.var, E_VarEntry(access, Ty_Void()));
				}
			}
			break;
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
				Ty_ty result_ty;
				Ty_tyList formalTyList;
				U_boolList boolList;
				Temp_label name = Temp_newlabel();
				Tr_level newLevel;

				if(!fdl->head->result)
					result_ty = Ty_Void();
				else
					result_ty = S_look(tenv, fdl->head->result);

				if(!result_ty) {
					EM_error(fdl->head->pos, "undeclared return type named %s", S_name(fdl->head->result));	
					result_ty = Ty_Void();			
				}		
				
				formalTyList = makeFormalTyList(tenv, fdl->head->params);
				boolList = makeBoolList(tenv, fdl->head->params);
				newLevel = Tr_newLevel(level, name, boolList);
				S_enter(venv, fdl->head->name, E_FunEntry(newLevel, name, formalTyList, result_ty));	
			}
			
			// We process the function body util the second pass.
			for(A_fundecList fdl = d->u.function; fdl; fdl = fdl->tail) {
				E_enventry entry;
				Ty_tyList tl;
				Tr_accessList al;
				A_fieldList fl;
				expty exp_ty;
             		
				entry = S_look(venv, fdl->head->name);
				fl = fdl->head->params; // acquire the symbol about parameters
				tl = entry->u.fun.formals; // acquire the type about paramters
				al = Tr_formals(entry->u.fun.level); // acquire the access about 
																						 // parameters
				S_beginScope(venv);
				for(; tl; fl = fl->tail, tl = tl->tail, al = al->tail) 
					S_enter(venv, fl->head->name, E_VarEntry(al->head, tl->head));
				exp_ty= transExp(venv, tenv, entry->u.fun.level, fdl->head->body);
        // test40.tig
        if(exp_ty.ty != actualTy(entry->u.fun.result))
					EM_error(fdl->head->pos, "the type about function body expression dose not match return type");
				S_endScope(venv);
			}
			break;
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
				Ty_ty name_ty = S_look(tenv, ntl->head->name);
				Ty_ty ty = transTy(tenv, ntl->head->ty);
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

				if(name_ty != ty) 
					name_ty->u.name.ty = ty;
				else {
					EM_error(ntl->head->ty->pos, "there exists an illegal cycle about type identifier %s", S_name(ntl->head->name));
					name_ty->u.name.ty = Ty_Void();
				}
			} 			
			break;
		} 
	}
}

static Ty_ty transTy(S_table tenv, A_ty a) {
	switch(a->kind) {
		case(A_nameTy): {
			Ty_ty name_ty = S_look(tenv, a->u.name);
  			//Ty_ty temp_ty;
            
			if(!name_ty) {
				EM_error(a->pos, "undeclared type named %s", S_name(a->u.name));		
				return Ty_Void();			
			}

			if(name_ty->kind != Ty_name)
				return name_ty;
			
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

			while(name_ty->u.name.ty) {
				name_ty = name_ty->u.name.ty;
				if(name_ty->kind != Ty_name)
					break;			
			}
			return name_ty;
		}
		case(A_recordTy): {
			A_fieldList fl;
			Ty_fieldList dummy, rear;

			dummy = rear = Ty_FieldList(0, 0);
			for(fl = a->u.record; fl; fl = fl->tail) {
				Ty_ty typ_ty = S_look(tenv, fl->head->typ);
				Ty_field field_ty;
				Ty_fieldList fieldList_ty;
				if(!typ_ty) {
					EM_error(fl->head->pos, "undeclared field type named %s", S_name(fl->head->typ));
					return Ty_Void();
				}
				field_ty = Ty_Field(fl->head->name, typ_ty);
				fieldList_ty = Ty_FieldList(field_ty, 0);
				rear->tail = fieldList_ty;
				rear = fieldList_ty;
			}
			return Ty_Record(dummy->tail);
		}
		case(A_arrayTy): {
			Ty_ty elem_ty = S_look(tenv, a->u.array);

			if(!elem_ty) {
				EM_error(a->pos, "undeclard element type named %s", S_name(a->u.array));
				return Ty_Array(Ty_Void());
			}
			else
				return Ty_Array(elem_ty);
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
		Ty_ty typ_ty = S_look(tenv, fl->head->typ);
		if(!typ_ty) {
			EM_error(fl->head->pos, "undeclared argument type named %s", S_name(fl->head->typ));
			return 0;		
		}	
		Ty_tyList tmp = Ty_TyList(typ_ty, 0);
		rear->tail = tmp;
		rear = tmp;
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
		Ty_ty typ_ty = S_look(tenv, fl->head->typ);
		if(!typ_ty) 
			return 0;		// coincide with makeFormalTyList 
		bl = U_BoolList(fl->head->escape, 0);
		rear->tail = bl;
		rear = bl;	
	}
	return dummy->tail;
}

/* 
 * env.h - enventry binding to variable or function
 *
 */

typedef struct E_enventry_ *E_enventry;

struct E_enventry_ {
	enum {E_varEntry, E_funEntry} kind;
	union {
		struct {Ty_ty ty;} var;
		struct {Ty_tyList formals; Ty_ty result;} fun;
	} u;
};

/* enventry constructor */
E_enventry E_VarEntry(Ty_ty ty);
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result);


/* basic environment constructor */
S_table E_base_tenv(void);
S_table E_base_venv(void);



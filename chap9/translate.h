typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;
typedef struct Tr_accessList_ *Tr_accessList;
typedef struct Tr_exp_* Tr_exp;
typedef struct patchList_* patchList;
typedef struct Tr_expList_* Tr_expList;
typedef struct F_fragList_* F_fragList;

struct Tr_accessList_ {
	Tr_access head;
	Tr_accessList tail;
};

struct Tr_expList_ {
	Tr_exp head;
	Tr_expList tail;
};

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);
Tr_expList Tr_ExpList(Tr_exp head, Tr_expList tail);
Tr_level Tr_outermost();
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);
Tr_exp Tr_simpleVar(Tr_access access, Tr_level level);
Tr_exp Tr_fieldVar(Tr_exp e, int offset);
Tr_exp Tr_subscriptVar(Tr_exp e, Tr_exp offset);
Tr_exp Tr_nilExp();
Tr_exp Tr_intExp(int intt);
Tr_exp Tr_opExp(A_oper oper, Tr_exp left, Tr_exp right);
Tr_exp Tr_seqExp(Tr_expList el);
Tr_exp Tr_ifExp(Tr_exp test, Tr_exp then, Tr_exp elsee);
Tr_exp Tr_stringExp(string literal);
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init);
Tr_exp Tr_recordExp(int size, Tr_expList el);
Tr_exp Tr_whileExp(Tr_exp test, Tr_exp body, Temp_label done);
Tr_exp Tr_forExp(Tr_exp var, Tr_exp lo, Tr_exp hi, Tr_exp body, Temp_label done);
Tr_exp Tr_breakExp(Temp_label done);
Tr_exp Tr_callExp(Temp_label label, Tr_level caller, Tr_level callee, Tr_expList args, bool hasReturn);
Tr_exp Tr_assignExp(Tr_exp var, Tr_exp exp);
Tr_exp Tr_letExp(Tr_expList el);
void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals);
F_fragList Tr_getResult();

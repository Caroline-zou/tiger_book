typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;
typedef struct F_frag_ *F_frag;
typedef struct F_fragList_ *F_fragList;

struct F_accessList_ {
	F_access head;
	F_accessList tail;
};

struct F_frag_ {
	enum {F_stringFrag, F_procFrag} kind;
	union {
		struct {Temp_label label; string str;} stringg;
		struct {T_stm body; F_frame frame;} proc;
	} u;
};

struct F_fragList_ {
	F_frag head;
	F_fragList tail;
};

extern const int F_wordSize;
extern const int F_staticLinkOffset;

F_frame F_newFrame(Temp_label name, U_boolList formals);
Temp_label F_name(F_frame f);
F_accessList F_formals(F_frame f);
F_access F_allocLocal(F_frame f, bool escape);
Temp_temp F_FP(); // $sp
Temp_temp F_SP(); // $sp
Temp_temp F_RV(); // $v0
Temp_temp F_ZERO(); // $zero
Temp_temp F_RA(); // $ra
Temp_tempList F_PARAS(); // $a0 ~ $a3
Temp_tempList F_SAVES(); // $s0 ~ $s7
Temp_tempList F_TEMPS(); // $t0 ~ $t9
Temp_map F_TempMap();
T_exp F_Exp(F_access access, T_exp framePtr);
T_exp F_externalCall(string s, T_expList args);
T_stm F_procEntryExit1(F_frame frame, T_stm stm);
AS_instrList F_procEntryExit2(AS_instrList body);
AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);
F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);
F_fragList F_FragList(F_frag head, F_fragList tail);

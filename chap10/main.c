/*
 * main.c
 */

#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "temp.h" /* needed by translate.h */
#include "tree.h" /* needed by frame.h */
#include "assem.h"
#include "frame.h" /* needed by translate.h and printfrags prototype */
#include "semant.h" /* function prototype for transProg */
#include "canon.h"
#include "escape.h"
#include "parse.h"
#include "codegen.h"
#include "graph.h"
#include "flowgraph.h"
#include "liveness.h"

extern bool anyErrors;
static FILE *out;

static void showFlowGraphInfo(void *info) {
	if(!info)
		return;
	AS_instrList *refInsList = info;
	AS_instrList insList = *refInsList;
	assert(insList);
	AS_instr ins = insList->head;
	assert(ins);
	return AS_print(out, ins, Temp_layerMap(F_TempMap(), Temp_name()));
}

static void showInterGraphInfo(void *info) {
	fprintf(out, "%s\n", Temp_look(Temp_layerMap(F_TempMap(), Temp_name()), info));
}

/* print the assembly language instructions to filename.s */
static void doProc(FILE *out, F_frame frame, T_stm body)
{
 //AS_proc proc;
 //struct RA_result allocation;
 T_stmList stmList;
 AS_instrList iList;

 stmList = C_linearize(body);
 stmList = C_traceSchedule(C_basicBlocks(stmList));
 //printStmList(stdout, stmList);

 iList  = F_codegen(frame, stmList); /* 9 */
 G_graph flow = FG_AssemFlowGraph(&iList);
 fprintf(out, "BEGIN %s\n", Temp_labelstring(F_name(frame)));
 // output the control flow graph
 fprintf(out, "control flow graph:\n");
 G_show(out, G_nodes(flow), showFlowGraphInfo);
 //AS_printInstrList (out, iList,
 //                      Temp_layerMap(F_TempMap(),Temp_name()));
 struct Live_graph inter = Live_liveness(flow);
 // output the dataflow equation
 fprintf(out, "\ndataflow equation:\n");
 Live_show(flow, out, Temp_layerMap(F_TempMap(), Temp_name()));
 // output the interfere graph
 fprintf(out, "\ninterfere graph:\n");
 G_show(out, G_nodes(inter.graph), showInterGraphInfo);
 fprintf(out, "END %s\n\n", Temp_labelstring(F_name(frame)));
}

int main(int argc, string *argv)
{
 A_exp absyn_root;
 F_fragList frags;
 char outfile[100];

 if (argc==2) {
   absyn_root = parse(argv[1]);
   if (!absyn_root)
     return 1;
     
#if 0
   pr_exp(out, absyn_root, 0); /* print absyn data structure */
   fprintf(out, "\n");
#endif

   Esc_findEscape(absyn_root); /* set varDec's escape field */

   frags = SEM_transProg(absyn_root);
   if (anyErrors) return 1; /* don't continue */

   /* convert the filename */
   sprintf(outfile, "%s.s", argv[1]);
   out = fopen(outfile, "w");
   /* Chapter 8, 9, 10, 11 & 12 */
   for (;frags;frags=frags->tail) {
     if (frags->head->kind == F_procFrag) 
       doProc(out, frags->head->u.proc.frame, frags->head->u.proc.body);
     else if (frags->head->kind == F_stringFrag) 
       fprintf(out, "%s\n", frags->head->u.stringg.str);
   }
   fclose(out);
   return 0;
 }
 EM_error(0,"usage: tiger file.tig");
 return 1;
}

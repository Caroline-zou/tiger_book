#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "temp.h"
#include "tree.h"
#include "frame.h"
#include "translate.h"
#include "parse.h"
#include "semant.h"
#include "printtree.h"

int main(int argc, char** argv) {
	A_exp root;
	F_fragList fragList;
	T_stmList stmList = 0;
	FILE* out;

 	if(argc != 2) {
		fprintf(stderr,"usage: a.out filename\n"); 
		exit(1);
 	}

	root = parse(argv[1]);
	if(root) {
		printf("Parsing successfully\n");	
		fragList = SEM_transProg(root);
		out = fopen("IR", "w");
		for(; fragList; fragList = fragList->tail) {
			if(fragList->head->kind == F_stringFrag) 
				fprintf(out, "%s: %s\n", Temp_labelstring(fragList->head->u.stringg.label), fragList->head->u.stringg.str);
			else 
				stmList = T_StmList(fragList->head->u.proc.body, stmList);
		}
		printStmList (out, stmList);
		fclose(out);
	}
	else
		printf("Parsing unsuccessfully\n");
	
	/*char fname[65];
 
	for(int i = 1; i <= 49; ++i) {
   		sprintf(fname, "../testcases/test%d.tig", i);
   		root = parse(fname);
		if(root) {
			printf("Parsing successfully\n");	
			SEM_transProg(root);
		}
		else
			printf("Parsing unsuccessfully\n");
 	}
	
	char fname[65];
 
	for(int i = 1; i <= 49; ++i) {
   	sprintf(fname, "../testcases/test%d.tig", i);
   	root = parse(fname);
		if(root) {
			printf("Parsing successfully\n");	
			fragList = SEM_transProg(root);
			out = fopen("IR", "w");
			for(; fragList; fragList = fragList->tail) {
				if(fragList->head->kind == F_stringFrag) 
					fprintf(out, "%s: %s\n", Temp_labelstring(fragList->head->u.stringg.label), fragList->head->u.stringg.str);
				else 
					stmList = T_StmList(fragList->head->u.proc.body, stmList);
			}
			printStmList (out, stmList);
			fclose(out);
		}
		else
			printf("Parsing unsuccessfully\n");
 	}*/
}

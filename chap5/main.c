#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "parse.h"
#include "semant.h"

int main(int argc, char** argv) {
	A_exp root;
	
 	/*if(argc != 2) {
		fprintf(stderr,"usage: a.out filename\n"); 
		exit(1);
 	}

	root = parse(argv[1]);
	if(root) {
		printf("Parsing successfully\n");	
		SEM_transProg(root);
	}
	else
		printf("Parsing unsuccessfully\n");*/
	
	char fname[65];
 
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
}

#include <stdio.h>
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "parse.h"
#include "prabsyn.h"

int main(int argc, char **argv) {
 A_exp root;
 FILE *file;

 if (argc!=2) {
	fprintf(stderr,"usage: a.out filename\n"); 
	exit(1);
 }
 root = parse(argv[1]);

 if(root) {
	fprintf(stderr,"Parsing successful!\n");
 	file = fopen("ast", "w");
	if(file) {
    	pr_exp(file, root, 0);
      	fclose(file);	
    }
 }
 else fprintf(stderr,"Parsing failed\n");

 return 0;
}


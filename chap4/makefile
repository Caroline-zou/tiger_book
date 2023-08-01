a.out: parsetest.o prabsyn.o parse.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o
	cc -g parsetest.o prabsyn.o parse.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o

parsetest.o: util.h symbol.h absyn.h parse.h prabsyn.h
	cc -g -c parsetest.c

prabsyn.o: prabsyn.c util.h symbol.h absyn.h prabsyn.h
	cc -g -c prabsyn.c

parse.o: parse.c util.h symbol.h absyn.h errormsg.h parse.h
	cc -g -c parse.c

absyn.o: absyn.c util.h symbol.h absyn.h
	cc -g -c absyn.c

symbol.o: symbol.c util.h symbol.h table.h
	cc -g -c symbol.c

table.o: table.c util.h table.h
	cc -g -c table.c

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: tiger.grm
	yacc -dv tiger.grm

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f a.out parsetest.o prabsyn.o parse.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o

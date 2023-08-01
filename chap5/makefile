a.out: main.o parse.o semant.o env.o types.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o
	cc -g main.o parse.o semant.o env.o types.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o

main.o: main.c util.h symbol.h absyn.h parse.h semant.h
	cc -g -c main.c

parse.o: parse.c util.h symbol.h absyn.h errormsg.h parse.h
	cc -g -c parse.c

semant.o: semant.c util.h errormsg.h symbol.h absyn.h types.h env.h
	cc -g -c semant.c

env.o: env.c util.h symbol.h types.h env.h
	cc -g -c env.c

types.o: types.c util.h symbol.h types.h
	cc -g -c types.c

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
	rm -f a.out main.o parse.o semant.o env.o types.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o

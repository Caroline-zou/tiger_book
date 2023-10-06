target = main.o parse.o semant.o env.o types.o absyn.o symbol.o table.o y.tab.o lex.yy.o errormsg.o util.o escape.o frame.o temp.o translate.o tree.o printtree.o

a.out: $(target)
	cc -g $(target)

main.o: main.c util.h symbol.h absyn.h temp.h translate.h parse.h semant.h
	cc -g -c main.c

parse.o: parse.c util.h symbol.h absyn.h errormsg.h parse.h
	cc -g -c parse.c

semant.o: semant.c util.h errormsg.h symbol.h absyn.h types.h env.h temp.h translate.h escape.h semant.h
	cc -g -c semant.c

env.o: env.c util.h symbol.h absyn.h types.h temp.h translate.h env.h
	cc -g -c env.c

types.o: types.c util.h symbol.h types.h
	cc -g -c types.c

escape.o: escape.c util.h symbol.h absyn.h escape.h
	cc -g -c escape.c

translate.o: translate.c util.h symbol.h absyn.h temp.h tree.h frame.h translate.h
	cc -g -c translate.c

absyn.o: absyn.c util.h symbol.h absyn.h
	cc -g -c absyn.c

frame.o: frame.c util.h symbol.h temp.h tree.h frame.h
	cc -g -c frame.c

printtree.o: printtree.c util.h symbol.h temp.h tree.h printtree.h
	cc -g -c printtree.c

tree.o: tree.c util.h symbol.h temp.h tree.h
	cc -g -c tree.c

temp.o: temp.c util.h symbol.h temp.h table.h
	cc -g -c temp.c

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
	rm -f a.out $(target)

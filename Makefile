all:
	bison -d -Wcounterexamples lang.y -o lang.tab.c
	flex lang.l
	gcc -o brainrot lib/hm.c lang.tab.c lex.yy.c ast.c -lfl -lm

test:
	pytest -v

clean:
	rm -rf lang.lex.c lang.tab.c lang.tab.h lex.yy.c brainrot

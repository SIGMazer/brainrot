all:
	bison -d -Wcounterexamples lang.y -o lang.tab.c
	flex lang.l
	gcc -o brainrot lang.tab.c lex.yy.c ast.c -lfl

test:
	pytest -v

clean:
	rm -rf lang.lex.c lang.tab.c lang.tab.h lex.yy.c brainrot
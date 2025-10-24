

all : calc 
calc : parser.y lexer.x main.c
	bison -Wcounterexamples -d parser.y
	flex lexer.x
	gcc -o calc ast.c parser.tab.c lex.yy.c main.c -lm

clean :
	rm lex.yy.c parser.tab.* calc

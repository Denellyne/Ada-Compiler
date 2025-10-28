CXX = gcc
CFLAGS = -lm
OUTPUT = ../bin/ada
INCLUDES := $(shell find . -name '*.c')
OBJ := $(patsubst %.c,%.o,$(INCLUDES))


all : build

%.o: %.c 
	@echo "Compiling..."
	$(CXX) $(CFLAGS) -c $< -o $@  	
	@echo "Compiled:" $@

parser.tab.c :
	@bison -Wcounterexamples -d Parser/parser.y

lex.yy.c : 
	@flex Lexer/lexer.x

build : parser.tab.c lex.yy.c $(OBJ)
	@echo "Builidng"
	@mkdir -p ../bin
	$(CXX) -o $(OUTPUT) $(OBJ) -lm

run : build
	@echo "Running..."
	$(OUTPUT)
	
clean :
	rm lex.yy.c parser.tab.* $(OUTPUT) $(OBJ)

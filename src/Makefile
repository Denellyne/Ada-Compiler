CXX = gcc
CFLAGS = -lm
OUTPUT = ../bin/ada
INCLUDES := ./lex.yy.c ./parser.tab.c $(shell find . -name '*.c')
OBJ_TEMP =  $(patsubst %.c,%.o,$(INCLUDES))
OBJ = $(sort $(OBJ_TEMP))


all : build

%.o: %.c 
	@echo "Compiling..."
	$(CXX) $(CFLAGS) -c $< -o $@  	
	@echo "Compiled:" $@

parser :
	@bison -Wcounterexamples -d Parser/parser.y

lex : 
	@flex Lexer/lexer.x

build : parser lex $(OBJ)
	@echo "Builidng"
	@mkdir -p ../bin
	$(CXX) -o $(OUTPUT) $(OBJ) -lm

run : build
	@echo "Running..."
	$(OUTPUT)
	
clean :
	rm parser.tab.h $(OUTPUT) $(OBJ)

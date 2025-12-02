CXX = gcc
CFLAGS = -lm -g
OUTPUT = ../bin/ada
INCLUDES := ./lex.yy.c ./parser.tab.c $(shell find . -name '*.c')
OBJ_TEMP =  $(patsubst %.c,%.o,$(INCLUDES))
OBJ = $(sort $(OBJ_TEMP))


all : build

%.o: %.c 
	@echo "Compiling $@"
	@$(CXX) $(CFLAGS) -c $< -o $@  	

parser :
	@bison -Wcounterexamples -d Parser/parser.y

lex : 
	@flex Lexer/lexer.x

build : parser lex $(OBJ)
	@echo "Builidng executable"
	@mkdir -p ../bin
	$(CXX) -g -o $(OUTPUT) $(OBJ) -lm
	@echo "Done"

run : build
	@echo "Running..."
	$(OUTPUT)

test:
	@bash ../tests/test.sh
	
clean :
	rm lex.yy.c parser.tab.h parser.tab.c $(OUTPUT) $(OBJ)

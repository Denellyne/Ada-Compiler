CXX = gcc
CFLAGS = -lm -g
OUTPUT = ../bin/ada
INCLUDES := ./lex.yy.c ./parser.tab.c $(shell find . -name '*.c')
OBJ_TEMP =  $(patsubst %.c,%.o,$(INCLUDES))
OBJ = $(sort $(OBJ_TEMP))

all : build

parser.tab.c parser.tab.h: Parser/parser.y
	@bison -Wcounterexamples -d $<

lex.yy.c: Lexer/lexer.x parser.tab.h
	@flex $<

lex.yy.o: lex.yy.c

$(OBJ): parser.tab.h lex.yy.c

%.o: %.c
        @echo "Compiling $@"
        @$(CXX) $(CFLAGS) -c $< -o $@

release : $(OBJ)
        @echo "Builidng executable"
        @mkdir -p ../bin
        $(CXX) -O2 -o $(OUTPUT) $(OBJ) -lm
        @echo "Done"

build : $(OBJ)
        @echo "Builidng executable"
        @mkdir -p ../bin
        $(CXX) -g -o $(OUTPUT) $(OBJ) -lm
        @echo "Done"

run : build
        @echo "Running..."
        $(OUTPUT)

test :
	@bash ../tests/test.sh

clean :
	rm lex.yy.c parser.tab.h parser.tab.c $(OUTPUT) $(OBJ)

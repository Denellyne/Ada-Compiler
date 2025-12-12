# Ada Compiler

### Ada compiler project

#### Group 21
Gustavo dos Santos, up202309627<br>
Maria Eduarda Toigo, up202309487


## Introduction

This project implements a complete compiler for a simplified subset of the Ada programming language, transforming high-level Ada source code into executable MIPS assembly. The compiler follows the traditional multi-phase architecture, covering the entire compilation process from lexical analysis to machine code generation. 

### How to run

Go to the folder [./src/] and run make, after that you go to folder [./tests/] and run [./tests/gen.sh] to generate all the test cases, then you can run the binary by running ../bin/ada {Name of TestFile} [-o if you wish to optimize the code] which will generate a {Name of TestFile}.adb.bin which you can pass to Mars.

For automatic tests, if you have mars.jar inside [./tests/] you can run just run [./tests/test.sh]

### Project Scope

The development occurred in two major phases:

**Phase 1: Front-end Development** (First Coursework)
- Lexical analysis using Flex for token recognition 
- Syntactic analysis using Bison for grammar validation 
- Abstract Syntax Tree (AST) construction representing program structure
- Basic semantic validation and error reporting 

**Phase 2: Back-end Development** (Second Coursework)
- Symbol table construction with comprehensive type information 
- Intermediate code generation in three-address form 
- Code optimization for improved execution efficiency 
- MIPS assembly generation compatible with the MARS simulator 

### Supported Language Features
Our Ada subset centers around variable declarations and statements with support for essential programming constructs:<br>
Expressions:<br>
- Arithmetic operations (+, -, *, /, **)<br>
- Boolean logic (AND, OR, NOT, XOR) <br>
- Comparison operations  (=, <, >, <=, >=, /=)


Control Structures:<br>
- Variable assignments (:=)<br>
- Conditional branching (if-then-elsif-else)<br>
- Iterative loops (while)<br>
- Procedure calls with flexible arguments

Data Types:<br> 
- Integer: 32-bit signed values
- Float: Double-precision floating point 
- String: ASCIIZ string literals 
- Boolean: LOgical true/false values

This foundation provides the basis for the complete compilation pipeline while demonstrating core principles of language processing.

### Technical Specifications

Build requirements: flex, bison, gcc<br>
Memory Management: malloc/strdup allocation (no specific cleanup)<br>
Input Methods: File input via command-line argument<br>
Output: Single pretty-print mode<br>


## Lexical Analyzer Implementation


The lexical analyzer transforms Ada source code into structured tokens using Flex, with regular expressions for efficient pattern matching. This separation from syntactic analysis simplifies both phases by handling low-level lexical concerns independently from grammatical structure.


### Keywords and reserved Patterns


```lex
"begin" {printf("%s ",yytext); return TOK_BEGIN;}
"end" {printf("%s ",yytext); return TOK_END;}
"procedure" {printf("%s ",yytext); return TOK_PROC;}
"is" {printf("%s ",yytext); return TOK_IS;}
"Main" {printf("%s ",yytext); return TOK_MAIN;}
"loop" {printf("%s ",yytext); return TOK_LOOP;}
```


### Type System Support


```lex
"String" {printf("%s ",yytext); return TOK_STRING;}
"Natural" {printf("%s ",yytext); return TOK_INTEGER;}
"Integer" {printf("%s ",yytext); return TOK_INTEGER;}
"Boolean" {printf("%s ",yytext); return TOK_BOOL;}
```


### Operator Recognition


```lex
"=" {printf("= "); return TOK_EQ;}
"/=" {printf("/= "); return TOK_NOTEQ;}
":=" {printf(":= "); return TOK_ASSIGN;}
"<=" {printf("<= "); return TOK_LESSEQ;}
">=" {printf(">= "); return TOK_GREATEREQ;}
"<" {printf("< "); return TOK_LESS;}
">" {printf("> "); return TOK_GREATER;}
```


### Literal Processing
Numeric Literal:
- Integers: `{digit}+`
- Floating-point: `{digit}+"."{digit}+`
- Conversion: Automatic string-to-number via atoi() and atof()


Identifiers:
- Pattern: `[[:alpha:]](_?[a-zA-Z0-9])*`
- Support for Ada's underscore-in-identifiers convention
- Case-sensitive recognition with symbol preservation


String Literal:
- Full quoted string support: `\"(\\.|[^"\\])*\"`
- Escape sequence handling for complex strings


## Abstract Syntax Tree (AST) Analysis


### Main structures


### `struct_exp`: Expressions (Calculations/Values)


Any expression that produces a value


#### Expression Node Types


**ID - Identifier/Variable**


```c
char *id;
```


— The ID node represents variable references within expressions, storing the identifier name as a string for symbol table lookup during semantic analysis.
Examples:
- Variable references in expressions: `x + 5`
- Assignment targets: `x := 10`
- Procedure calls: `Calculate(result)`  


**NUM - Numeric Literal**


```c
double val;
```


— Represents a constant number in the code, and stores numerical values for calculations.<br>
Examples:
- Arithmetic operation: `3.14 * radius`
- Array indices: `array[5]`
- Loop counters: `for i in 1..10`
- Constant assignments: `max_size := 10`


**BINOP - Binary Operation**


  ```c
  struct {
    op op;
    struct _exp *left, *right;
  } binop;
  ```


— The BINOP node is used to represent all binary operations in the language, acting as an unified structure that covers arithmetic, relational, and logical operations. This approach simplifies the tree design by avoiding the need to define separate node types for each operator category.<br>
Examples:
- Comparison operations: `age >= 18`
- Arithmetic expressions: `a + b * c`
- Logical expressions: `x > 0 AND x < 100`
- String Concatenation : `file_path := directory & "/" & filename`


**BOOL - Boolean Value**


```c
int bool_val;
```


— Represents logical values and stores true/false states.<br>
Examples:
- Boolean assignments: `is_valid := TRUE, flag := FALSE`
- Conditional checks: `IF is_active THEN ... END IF`
- Logical operations: `result := (x > 5) AND (y < 10)`


**STRLITERAL - String Literal**


```c
char *str;
```


Represents constant text and stores character strings.<br>
Examples:
- String assignments: `name := "Maria"`
- Concatenation: `file_path := directory & "/" & filename & ".txt"`
- Output operations: `Print("Hello World")`
- Comparison operations:`IF name = "Admin" THEN ... END_IF`


**UNARYOP - Unary Operation**




```c
struct {
    op op;
    struct _exp *exp;
} unaryop;
```


— Represents operations with one operand and applies an operator to a single expression.<br>
Examples:
- Arithmetic negation: - (x * y + z)
- Logical negation: NOT (x > 3 AND y < 10)
- Address operations: variable'Address
- Type conversions: `Float(count)`


### `struct _stm`: Statements (Actions/Commands)


Instructions that perform actions or control program flow (they don't evaluate to values).


#### Statement Node Types


**COMPOUND - Statement Sequence**


```c
struct {
    struct _stm *fst, *snd;
} compound;
```


— Chains multiple statements for sequential execution within blocks and procedure bodies.<br>
Examples:
- Begin-end blocks: begin stmt1; stmt2; end
- Loop bodies: with sequential operations
- Then/else branches with multiple statements
- Procedure bodies with multiple statements


**ASSIGN - Variable Assignment**


```c
struct {
  char *ident;
  int type;
  struct _exp *expr;
} assign;
```


— Handles variable assignments with type information for semantic analysis.<br>
Examples:
- Initialization: `count: Integer := 0`
- Simple assignments: `x := 5`
- Calculation results: `result := a + b * c`
- String assignments: `message := "Error"`
 
**INCR -  Increment Operation**


```c
char *ident;
```


— Specialized statements for increment operations as a common pattern.<br>
Examples:
- Accumulators: `total := total + 1`
- Loop counters: `i := i + 1`
- Simple increments in algorithms
 
**FUNCTION - Procedure Calls**


```c
struct {
   char *ident;
   struct _args *args;
} function;
```


— Represents function and procedure calls with flexible argument lists.<br>
Examples:
- Generic function calls: `Calculate(x, y, z)`
- Procedure calls: `Initialize()`
- Single argument calls: `Process(data)`
- No argument calls: `Cleanup()`


**IF - Conditional Statement**


```c
struct {
   Exp cond;
   struct _stm *thenBranch;
   struct _stm *elsifBranch;
   struct _stm *elseBranch;
} ifStmt;
```


— Comprehensive conditional branching with full Ada support.<br>
Examples:
- Nested conditionals: `IF...IF...END IF; END IF`
- Guarded statements: `IF condition1 AND condition2 THEN...`
- Multiple branches: `IF...ELSIF...ELSIF...ELSE`
- Simple conditions: `IF x > 0 THEN...`


**WHILE - Loop Construct**


```c
struct {
   Exp cond;
   struct _stm *body;
} whileStmt;
```


—Represents while loops with condition evaluation and loop body.<br>
Examples:
- Counter loop: `WHILE i < 10 LOOP i := i + 1; END LOOP;`
- Conditional Processing: `WHILE NOT end_of_file LOOP Read(line); Process(line); END LOOP;`
- Input validation: `WHILE input /= "quit" LOOP Get(input); Process(input); END LOOP;`


## AST Construction


The AST represents the primary output of the syntactic analysis phase, transforming a linear token sequence into a hierarchical program structure.  




### Parser Architecture and Input Validation


#### Parser Architecture


The Syntactic Analysis phase, which receives the sequence of tokens from the lexical Analyzer, is responsible for validating the structural correctness of the input and generating the Abstract Syntax Tree. The AST then serves as the input for the Semantic Analysis phase.


##### Input Processing Pipeline:


**Processing Pipeline:**
Characters -> Flex (tokens) -> Bison (AST via mkBinOp, mkAssign, etc.)


##### Structural Validation


**Parser Validates:**
- Structure: balanced `()`, statements with `;`
- Control: `if...then...end if`, `while...loop...end loop`
- Variables: `name : Type [:= value]`
- Functions: flexible arguments lists


**Grammar Patterns:**
- `TOK_IF expr TOK_THEN stmt`
- `TOK_WHILE expr TOK_LOOP stmt TOK_END TOK_LOOP`
- `TOK_ID TOK_COLON Type [TOK_ASSIGN expr]`
- `procedure Main is...begin...end Main;`


##### Expression Hierarchy with Operator Precedence:
The grammar implements proper operator precedence ensuring correct interpretation of complex expressions like a + b * d where multiplication binds tighter than addition.
 
#### Generic Function System:


Our implementation features a generic call system that supports any function with variable arguments.


##### Important Generic Features:
- Flexible Arguments Lists
- No Hardcoded Logic
- Easy Extension: New Functions need only lexical recognition


```bison
function : TOK_ID TOK_LP args TOK_RP TOK_END_STATEMENT {$$ = mkFuncCall($1,$3);}
         ;


args : %empty {$$ = NULL;}
     | expr TOK_COMMA args {$$ = appendArg(mkArg($1),$3);}
     | expr {$$ = mkArg($1);}
     ;
```


This function supports any function call with variable arguments through recursive argument list construction.


#### Flexible variable Declaration System:
Supports both initialized and uninitialized variable declarations through optional expression assignment.


### AST Building Process


#### Recursive Construction Process


The AST is built bottom - up during parsing through type- safe constructor functions:


```c
Exp mkBinOp(Exp lExp, op op, Exp rExp) {


  Exp e = (Exp)malloc(sizeof(struct _exp));
  e->tag = BINOP;
  e->binop.left = lExp;
  e->binop.right = rExp;
  e->binop.op = op;
  return e;
}
Func mkFunc(char *id, int returnValue, Stm args) {


  Func ptr = (Func)malloc(sizeof(struct _func));
  ptr->ident = id;
  ptr->returnValueTag = returnValue;
  ptr->args = args;


  return ptr;
}
```


#### Leaf Creation


Basic elements are created first as terminal nodes:


- Identifiers: mkId(“x”) for variable names
- Literals: mkNum(5.0) for numbers, mkStringLiteral(“text”)  for strings


These become the leaves of the AST with no children


#### Composite Building


Operators combine simpler expressions into complex ones:


```bison
expr : term {$$ =$1;}
     | expr TOK_OP_ADD expr {$$ = mkBinOp($1,PLUS,$3);}
     | expr TOK_OP_MINUS expr {$$ = mkBinOp($1,MINUS,$3);}
     | expr TOK_OP_MULT expr {$$ = mkBinOp($1,TIMES,$3);}
```


- Binary operations build tree structures with left/right children
- Unary operations apply to single expressions
- Function calls combine identifiers with argument lists


#### Statement Assembly
Expressions are embedded into executable statements:


```bison
stmtAssign : TOK_ID TOK_ASSIGN expr TOK_END_STATEMENT{$$ = mkAssign($1,-1,$3);};


loop : TOK_WHILE expr TOK_LOOP stmt TOK_END TOK_LOOP TOK_END_STATEMENT { $$ = mkWhile($2,$4);}
     | if {$$=$1;}
     ;
if : TOK_IF expr TOK_THEN stmt ifBody TOK_ELSE stmt TOK_END TOK_IF TOK_END_STATEMENT{ $$ = mkIf($2,$4,$5,$7);}
   | TOK_IF expr TOK_THEN stmt ifBody TOK_END TOK_IF TOK_END_STATEMENT{ $$ = mkIf($2,$4,$5,NULL);}
   ;
```


#### Program Structure


All components are combined into the final program:


```C
Prog mkProg(Stm varDec, Stm statments) {


  Prog ptr = (Prog)malloc(sizeof(struct _prog));
  ptr->varDec = varDec;
  ptr->statements = statments;
  ptr->ident = "Prog";
  return ptr;
}
```


This bottom-up approach ensures the AST accurately represents the program’s hierarchical structure maintaining the logical relationships between code elements.

## Implementation Modules 

The compiler is organized into modular components, responsible for a specific compilation phases. 

### Symbol Table Module  

The symbol table module implements semantic analysis and type checking using a linked list-based key-value store. It validates program correctness by tracking variable types and ensuring type compatibility throughout expressions. Ensures type correctness of the generated AST.

The table uses a simple linled list structure where each entry contains:

- `key`: Identifier representing the data type
- `typeTag`: Bitmask representing the data type 
- `numArgs`: For function entries, number of parameters
- `argType`: Linked list of parameter tyes for functions
- `next`: Pointer to next entry in collision chain

**Type System** 

```c 
enum {
  TBL_INT = 1,
  TBL_BOOL = 2,
  TBL_STRING = 4, 
  TBL_FUNCTION = 8,
  TBL_ID = 16, 
  TBL_FLOAT = 32,
  TBL_ERROR = 64,
};

```
**Core Functions** 
- **Type Conversion** 

```c 
int convertType(int type) {
    switch (type) {
    case FLOAT: return TBL_FLOAT;
    case NUM: return TBL_INT;
    case STRLITERAL: return TBL_STRING;
    case BOOL: return TBL_BOOL;
    case ID: return TBL_ID;
    default: return TBL_ERROR;
    }
}
```


**Symbol Management**

- `addEntry()`: Inserts new symbols with optional parameter lists
- `lookup()`: Linear search through linked list (O(n))
- `addVariableDeclaratios()`: Processes variable declaration statements 

**Type Compatibility Rules** 

- **Arithmetic Operations (+, -, *, /)**
```c
if (typeL & (TBL_INT | TBL_FLOAT) && (typeR & (TBL_FLOAT | TBL_INT))) {
    if (typeL == TBL_FLOAT && typeR == TBL_FLOAT)
        return TBL_FLOAT;
    return TBL_INT;
}
```
- **Exponentiation (`**`)**
- Base: Integer or float
- Exponent: Integer only 
- Result type matches base type 

- **Logical Operations(`AND`, `OR`, `NOT`, `XOR`)** 
- Operands: Boolean or Integer 
- Result: Same as operands 

- **Comparison Operations(`=`, `/=`, `<`, `>`, `<=`, `>=`)** 
- Operands: Compatible types (Integer, Boolean)
- Result: Boolean

**Error Handling** 
The module provides detailed error messages: 

- Duplicate variable declarations 
- Type mismatches in assignments 
- Invalid operand types for operations 
- Function call argument count/type mismatches

**Integration with Compilation Pipeline** 
- 1.Declaration Phase: `addVariableDeclarations()` builds initial symbol table 
- 2.Validation Phase: `validateAST()`checks entire program semantics 
- 3.Type Query Phase: `checkExprType()`used during code generation

### Intermediate Representation Module(IR)

The IR translates the AST with lightweight optimizations, such as temporary reuse and conversion of two-operand operations into immediate operations where possible.<br>
For floats and strings, values are stored in the `.data` section, and temporaries load them as needed.<br>
After IR generation --and if the `-o`flag is enabled, after optimization--temporaries are mapped to actual MIPS registers.


**Register Management System** 

The module manages two separate register pools:
**Integer Registers** 
- Temporary registers: `$t0` through `$t9` 
- Saved registers: `$s0` through `$s7` 
- Allocation tracking via `used[18]` bitmask array 

**Float Registers**
- Even numbered registers only
- Allocation tracking via `usedFloats[14]`bitmask array 
- Double-precision operations requiring even-odd register pairs

**Register Allocation Functions** 

```c 
char *newTemp() {
  for (int i = 0; i < 18; i++) {
    if (!used[i]) {
      used[i] = 1;
      tempCount++;
      assert(tempCount < 18);
      return strdup(temps[i]);
    }
  }

  return NULL;
}

char *newTempFloat() {
  for (int i = 0; i < 14; i++) {
    if (!usedFloats[i]) {
      usedFloats[i] = 1;
      floatsCount++;
      assert(floatsCount < 14);
      return strdup(tempFloats[i]);
    }
  }
}

``` 
#### Three-Address Code Intermediate Representation

The compiler uses **three-address code** as its intermediate representation (IR), implementing a comprehensive system for temporary register management, literal pooling, and control flow generation.

##### Instruction Representation 

```c 
struct _instruction {
  Opcode opcode;
  char *arg1;
  char *arg2;
  char *arg3;
  char *arg4;
  int num;
  double val;
  op binop;
};
``` 

The IR implements a compehensive instruction set covering all Ada subset operations:

**Data Movement Instructions** 
- `MOVE dest, src`: Register-to-Register
- `MOVEI dest, value`: Load immediate value
- `MOVEF dest, src`: Float register tranfer 
- `MOVEFI dest, label`: Load float constant 
- `LOADADRESS dest, label`: For strings 

**Arithmetic Instructions** 

- Integer: `AND`, `SUB`, `MULT`, `DIV`
- Float: `ADDF`, `SUBF`, `MULTF`, `DIVIDEF`
- Immediate variants: `ADDI`, `SUBI`, `MULTI`, `DIVIDEI`

**Control Flow Instructions** 

- `JUMP label`: Unconditional jump 
- `COND op, src1, src2, labelTrue, labelFalse`: Conditional branch 
- `BNEZ src, label`: Branch if not equal zero 
- `LABEL name`: Code label definition 

**Function Call Instruction** 

- `CALL functionName`: Procedure invocation 
- `SAVEREGISTERS count`: Preserve register state 
- `LOADREGISTERS count`: Restore register state 
   

#### IR Optimization 

The compiler includes a lightweight peephole optimization phase that operates directly on the intermediate representation (IR) before assembly generation. This module performs local transformations on consecutive instructions in order to reduce redundancy and improve the quality of the generated MIPS code.

**Arithmetic Optimizations** 
Expressions known at compile-time are replaced with immediate loads carrying their precomputd values.

**Move Optimization** 
Statements of the form: 

`ADD t0 t1 t2`
`MOVE a0 t0`

are optimized to:


`ADD a0 t1 t2`

This avoids an unnecessary intermediate instruction.<br> 
Move optimizations can also transform arithmetic operations into immediate operations if one operand can be replaced by its value. It both operands become constants, the operation is replaced by an immediate load.

**Dead Assignment Elimination** 

For repeated assignments to the same `str`variable, only the final assignment is generated, as it is the only one affecting the program's outcome.

**Condition Optimization** 

Conditions are optimized using move optimizations:<br>
- First transformed into immediate conditions when possible 
- If both operands become constants, the condition is evaluated at compile-time, and all irrelevant branches are eliminated, only the taken path is preserved.

Unlike arithmetic operations (except `ADD` and `MULT`), any operand is a condition can be swapped by reversing the comparator while preserving semantics. 
Example:
`COND t0 >= t1 L0 L1`is generated from `t0`, `t1`, and `L0`.
If `t0` is replaced by constant `3`, we transform: 
`COND 3 >= t1 L0 L1` into `COND t1 <= 3 L0 L1`, which is semantically equivalent and compatible with our code generator.
### Assembly Code Generation


The final compilation stage is handled by two coordinated files:
 **Header File (`codeGen.h`)**
 Defines the public interface with a single entry point:
 ```c 
int generateASM(char *fileName, Table tbl, Stm varDecl, instrList *ir,
                stringLiterals *strs, floatLiterals *floats);
``` 
This function accepts all compiler data structure and produces the final output.

**Implementation File (`codeGen.c`)**
Contains the complete translation logic from intermediate representation to executable MIPS code.

###  Assembly Output Format 
The generator creates structured MIPS assembly with two main segments:
- `.data`
Stores static content: string constants, numeric literals.
- `.text`
Contains executable instructions, such as program initialization and termination routines, built-in supported functions and translated user program code.

#### Code Generation for Built-in Functions 

`Put_Line` 
Implemented as a single syscall 

`Get_Line`
Reads user input into a 64-byte buffer, allocates memory for the string, copies the buffer content to this memory, assigns the address to the given string variable, and stores the string length in the second argument.

`Put_Num`
Prints integers or floats. A hidden second argument indicates the type, and the function branches accordingly.

**Power Operator** 
Follows the same pattern as `Put_Num`, with a hidden type argument allowing the same function to handle different numeric types.

**Main Program Translation** - Converts IR instructions to MIPS

#### Instruction Translation Mechanism 
Ã central dispatch function (`printInstr`) maps each intermediate instruction type to appropriate MIPS code:
- Arithmetic operations - `addu`, `add.d`, `mul`, `mul.d`
- Data movement - `move`, `la`, `ldc1`
- Control flow - `j`, `beq`, `blt`
- Function calls - `jal` with proper stack management 

## Conclusion

This project implemented a complete compiler for a subset of the Ada programming language. The system reads Ada source code, analyzes ts structure, performs type checking, generates optimized intermediate code, and produces executable MIPS assembly. The compiler supports arithmetic expressions, logical operations, conditional structures, loops, and calls to built-in functions.

### References


Andrew Appel. Modern Compiler Implementation. 1998.








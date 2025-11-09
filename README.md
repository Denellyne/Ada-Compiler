# Ada Compiler

### Ada compiler project

#### Group 21
Gustavo dos Santos, up202309627<br>
Maria Eduarda Toigo, up202309487


## Introduction


This project implements the initial phase of a compiler for a simplified subset of the Ada programming language, focusing on lexical and syntactic analysis. The implementation consists of two main components working in sequence:<br>
The scanner processes raw source code, transforming character streams into structured tokens that identify the fundamental elements of the language. Following this, the parser analyzes the token sequence against Ada’s grammatical rules, constructing an Abstract Syntax Tree (AST) that captures the program’s hierarchical structure.


Our Ada subset centers around a primary main procedure and supports essential programming constructs:<br>
Expressions:<br>
- Arithmetic operations (+, -, *, /, **)<br>
- Boolean logic (AND, OR, NOT, XOR) <br>
- Comparison operations  (=, <, >, <=, >=, /=)


Control Structures:<br>
- Variable assignments (:=)<br>
- Conditional branching (if-then-elsif-else)<br>
- Iterative loops (while)<br>
- Procedure calls with flexible arguments


This foundation provides the basis  for further compilation stages while demonstrating core principles of language processing.


## Technical Specifications


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


### Conclusion




This project implements the core stages of a compiler construction for a simplified Ada subset. The lexical analyzer, developed with Flex, efficiently converts the program's source code into tokens using regular expressions, while the syntactic parser, built with Bison, validates grammatical structure and constructs a comprehensive Abstract Syntax Tree. The system demonstrates effective management of Ada's essential features including expressions, control structures, variable declarations, and procedure calls. This work establishes a solid foundation for subsequent compilation stages, showcasing practical application of formal language processing techniques.




### References


Andrew Appel. Modern Compiler Implementation. 1998.








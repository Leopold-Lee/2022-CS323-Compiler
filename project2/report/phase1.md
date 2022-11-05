# CS323 Project phase1

**Goupe Member:**

**12012705 李嘉成**

**11910504 梁家源**

In this project, we use the tool flex and bison to realize Lexical Analysis & Syntax Analysis. We use C/C++ code to propagate the information and generate the syntax tree.

To generate the excitable program, just run `make splc` command,  you need the following dependencies

```
g++
flex
bison
```

## Lexer

The function of Lexer is to identify lexeme and handle type A error

1. Lexer uses regular expression to identify lexemes. Once a lexeme is identified, lexer will construct and return a `Node` object which contains inforamtion of the lexeme.
2. Lexer also uses regular expression to recognize erroneous patterns, e.g, illiegal identifiers, hexidecimal number with leading zeros, etc. Once an incorrect pattern is recognized, lexer will print the error message.

We use a node class to record the information of each token, it contains the following information.

```c++
bool is_key;
string name;
string value;
vector<Node *> sub_nodes;
int line_num;
bool is_terminal;
bool is_empty;
```

## Parser

We use the grammar given to build our parser, for nonterminal symbols, we set the tokes as its sub-nodes. The nonterminal `Program` is the root node of parsing tree. When the parsing process succeed, the program will pre-traverse the root-node to print the syntax tree.

#### Solve shift/reduce conflicts

We define the precedence as the following to solve some shift/reduce conflicts

```
%right ASSIGN
%left OR
%left AND
%left LT LE GT GE EQ NE
%left PLUS MINUS 
%left MUL DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_ELSE
%nonassoc ELSE
```

## Bonus

We only implement the `for statement` recognition

A for statement syntax pattern：

```
FOR LP Specifier Dec SEMI Exp SEMI Exp RP Stmt
```

It can recognize the following code:

```c
int a = 0;
for (int i = 0; i < 10; i = i + 1)
{
    a = a + 1;
}
```

## Test cases

The test/test_1_1.sql contains two type A error and one type B error

The test/test_1_2.sql contains one type A error and one type B error

The test/test_1_3.sql test the precedence of `&&, ||, =, ==`

The test/test_1_4.sql test the precedence of `+, - *, /` and if and else statements

The test/test_1_5.sql contains one type A error and four type B error

The test-ex/test_1.sql test the `for statement` recognition.

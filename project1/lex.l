%{
    #include "node.hpp"
    int isError = 0;
%}
%{
    #include"syntax.tab.h"
    int yycolno = 1;
    extern int yylineno;
    #define YY_USER_ACTION \
        yylloc.first_line = yylineno; \
        yylloc.first_column = yycolno; \
        yylloc.last_line = yylineno; \
        yylloc.last_column = yycolno + yyleng;\
        yycolno += yyleng;
%}
%option noyywrap
%option yylineno
letters                     [A-Za-z]
letters_                    {letters}|_
digit                       [0-9]
digit_hex                   [0-9a-fA-F]
int                         ([+-]?)(0|([1-9]{digit}*))
int_hex                     0[xX](0|[1-9a-fA-F]{digit_hex}*)
float                       [+-]?{int}\.{digit}+
identifier                  {letters_}({letters_}|{digit})*
char                        '.'
char_hex                    '\\x(0|[1-9a-fA-F]{digit_hex})'
leading_zeros_in_hex_error  0x(00{digit_hex}*)
leading_zeros_in_hex_char_error  '\\x(00{digit_hex}*)''
illegal_hex_error           '\\x[0-9a-zA-Z]*'
illegal_id_error            {digit}+{identifier}
%%
"//" {
    char c = yyinput();
    while(c!='\n'){c=yyinput();}
}
[\t\r ]+ {}
"\n" {yycolno = 1;}
"if" {
    yylval.node = new Node(true, "IF", "");
    return IF;
}
"else" {
    yylval.node = new Node(true, "ELSE", "");
    return ELSE;
}
"struct" {
    //attention
    yylval.node = new Node(true, "STRUCT", "");
    return STRUCT;
}
"while" {
    yylval.node = new Node(true, "WHILE", "");
    return WHILE;
}
"for" {
    yylval.node = new Node(true, "FOR", "");
    return FOR; 
}
"return" {
    yylval.node = new Node(true, "RETURN", "");
    return RETURN;
}
"int" {
    yylval.node = new Node(false, "TYPE", "int");
    return TYPE;
}
"char" {
    yylval.node = new Node(false, "TYPE", "char");
    return TYPE;
}
"float" {
    yylval.node = new Node(false, "TYPE", "float");
    return TYPE;
}
"." {
    yylval.node = new Node(true, "DOT", "");
    return DOT;
}
";" {
    yylval.node = new Node(true, "SEMI", "");
    return SEMI;
}
"," {
    yylval.node = new Node(true, "COMMA", "");
    return COMMA;}
"=" {
    yylval.node = new Node(true, "ASSIGN", "");
    return ASSIGN;
}
"<" {
    yylval.node = new Node(true, "LT", "");
    return LT;
}
"<=" {
    yylval.node = new Node(true, "LE", "");
    return LE;
}
">" {
    yylval.node = new Node(true, "GT", "");
    return GT;
}
">=" {
    yylval.node = new Node(true, "GE", "");
    return GE;
}
"!=" {
    yylval.node = new Node(true, "NE", "");
    return NE;
}
"==" {
    yylval.node = new Node(true, "EQ", "");
    return EQ;
}
"+" {
    yylval.node = new Node(true, "PLUS", "");
    return PLUS;
}
"-" {
    yylval.node = new Node(true, "MINUS", "");
    return MINUS;
}
"*" {
    yylval.node = new Node(true, "MUL", "");
    return MUL;
}
"/" {
    yylval.node = new Node(true, "DIV", "");
    return DIV;
}
"&&" {
    yylval.node = new Node(true, "AND", "");
    return AND;
}
"||" {
    yylval.node = new Node(true, "OR", "");
    return OR;
}
"!" {
    yylval.node = new Node(true, "NOT", "");
    return NOT;
}
"(" {
    yylval.node = new Node(true, "LP", "");
    return LP;
}
")" {
    yylval.node = new Node(true, "RP", "");
    return RP;
}
"[" {
    yylval.node = new Node(true, "LB", "");
    return LB;
}
"]" {
    yylval.node = new Node(true, "RB", "");
    return RB;
}
"{" {
    yylval.node = new Node(true, "LC", "");
    return LC;
}
"}" {
    yylval.node = new Node(true, "RC", "");
    return RC;
}
{int} {
    yylval.node = new Node(false, "INT", yytext);
    return INT;
}
{int_hex} {
    yylval.node = new Node(false, "INT", yytext);
    return INT;
}
{char} {
    yylval.node = new Node(false, "CHAR", yytext);
    return CHAR;
}
{char_hex} {
    yylval.node = new Node(false, "CHAR", yytext);
    return CHAR;
}
{float} {
    yylval.node = new Node(false, "FLOAT", yytext);
    return FLOAT;
}
{identifier} {
    yylval.node = new Node(false, "ID", yytext);
    return ID;
}
{leading_zeros_in_hex_error} {
    isError = 1;
    fprintf(stderr, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    yylval.node = new Node(false, "ERROR", yytext);
    return ERROR;
}
{leading_zeros_in_hex_char_error} {
    isError = 1;
    fprintf(stderr, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    yylval.node = new Node(false, "ERROR", yytext);
    return ERROR;
}
{illegal_hex_error} {
    isError = 1;
    fprintf(stderr, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    yylval.node = new Node(false, "ERROR", yytext);
    return ERROR;
}
{illegal_id_error} {
    isError = 1;
    fprintf(stderr, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    yylval.node = new Node(false, "ERROR", yytext);
    return ERROR;
}
. {
    isError = 1;
    fprintf(stderr, "Error type A at Line %d: unknown lexeme %s\n", yylineno, yytext);
    yylval.node = new Node(false, "ERROR", yytext);
    return ERROR;
}
%%

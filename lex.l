%{
    #include "syntax.tab.h"
%}
letters [A-Za-z]
letter_ {letter}|_
digit [0-9]
digit_hex [0-9a-fA-F]
int 0|([1-9]{digit}*)
int_hex 0x(0|[1-9a-fA-F]{digit_hex}*)
float {int}\.{digit}+
identifier {letters_}({letters_}|{digit})*
char '.'
char_hex '\\x(0|[1-9a-fA-F]{digit_16})'
%%
"//" {
    char c = yyinput();
    while(c!='\n'){c=yyinput();}
}
[\t\r ]+ {}
"if" {return IF;}
"else" {return ELSE;}
"struct" {return STRUCT;}
"while" {return WHILE;}
"return" {return RETURN;}
"int" {return TYPE;}
"char" {return TYPE;}
"float" {return TYPE;}
"." {return DOT;}
";" {return SEMI;}
"," {return COMMA;}
"=" {return ASSIGN;}
"<" {return LT;}
"<=" {return LE;}
">" {return GT;}
">=" {return GE;}
"!=" {return NE;}
"==" {return EQ;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return MUL;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"!" {return NOT;}
"(" {return LP;}
")" {return RP;}
"[" {return LB;}
"]" {return RB;}
"{" {return LC;}
"}" {return RC;}
{int} {return INT;}
{int_hex} {return INT;}
{char} {return CHAR;}
{char_hex} {return CHAR;}
{float} {return FLOAT;}
{identifier} {return ID;}
[0-9]+{ID} {}
%%

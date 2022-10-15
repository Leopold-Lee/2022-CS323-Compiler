%{
    #include "lex.yy.c"
    #include <iostream>
    void yyerror(const char *s);
    void lineinfor(void);
    void traverse(string tab, Node *node);
    Node* root;
    extern int isError;
    #define PARSER_error_OUTPUT stdout
    //yydebug = 1;
%}
%locations
%union{
    Node* node;
}
%token INT FLOAT CHAR ID TYPE STRUCT IF ELSE WHILE RETURN DOT SEMI COMMA ASSIGN     
%token GT GE NE EQ PLUS MINUS MUL DIV AND OR NOT LP RP LB RB LC RC LT LE  

%type<node> INT FLOAT CHAR ID TYPE STRUCT IF ELSE WHILE RETURN DOT SEMI COMMA ASSIGN LT      
%type<node> LE GT GE NE EQ PLUS MINUS MUL DIV AND OR NOT LP RP LB RB LC RC      

%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier VarDec
%type <node> FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Args Exp

%%
Program:
    ExtDefList {
        $$ = new Node("Program", @$.first_line);
        $$->add_sub($1);
        root = $$;
    }
    ;
ExtDefList: {
        $$ = new Node();
    }
    | ExtDef ExtDefList {
        $$ = new Node("ExtDefList", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    ;
ExtDef:
    Specifier ExtDecList SEMI {
        $$ = new Node("ExtDef", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Specifier SEMI {
        $$ = new Node("ExtDef", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    | Specifier FunDec CompSt {
        $$ = new Node("ExtDef", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    ;
ExtDecList:
    VarDec {
        $$ = new Node("ExtDecList", @$.first_line);
        $$->add_sub($1);
    }
    | VarDec COMMA ExtDecList {
        $$ = new Node("ExtDecList", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    ;
/* specifier */
Specifier:
    TYPE {
        $$ = new Node("Specifier", @$.first_line);
        $$->add_sub($1);
    }
    | StructSpecifier {
        $$ = new Node("Specifier", @$.first_line);
        $$->add_sub($1);        
    }
    ;
StructSpecifier: 
    STRUCT ID LC DefList RC {
        $$ = new Node("StructSpecifier", @$.first_line);
        $$->add_sub($1);  
        $$->add_sub($2);  
        $$->add_sub($3);  
        $$->add_sub($4);  
        $$->add_sub($5);  
    }
    | STRUCT ID {
        $$ = new Node("StructSpecifier", @$.first_line);
        $$->add_sub($1);  
        $$->add_sub($2);  
    }
    ;
/* declarator */
VarDec:
    ID {
        $$ = new Node("VarDec", @$.first_line);
        $$->add_sub($1);
    }
    | VarDec LB INT RB {
        $$ = new Node("VarDec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
    }
    ;
FunDec:
    ID LP VarList RP {
        $$ = new Node("FunDec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
    }
    | ID LP RP {
        $$ = new Node("FunDec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    ;
VarList:
    ParamDec COMMA VarList {
        $$ = new Node("VarList", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | ParamDec {
        $$ = new Node("VarList", @$.first_line);
        $$->add_sub($1);
    }
    ;
ParamDec:
    Specifier VarDec {
        $$ = new Node("ParamDec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    ;
/* statement */
CompSt:
    LC DefList StmtList RC{
        $$ = new Node("CompSt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
    }
    ;
StmtList:{
    $$ = new Node();
    }
    | Stmt StmtList {
        $$ = new Node("StmtList", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    ;
Stmt:
    Exp SEMI {
        $$ = new Node("Stmt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    | CompSt {
        $$ = new Node("Stmt", @$.first_line);
        $$->add_sub($1);
    }
    | RETURN Exp SEMI {
        $$ = new Node("Stmt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | IF LP Exp RP Stmt {
        $$ = new Node("Stmt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
        $$->add_sub($5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = new Node("Stmt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
        $$->add_sub($5);
        $$->add_sub($6);
        $$->add_sub($7);
    }
    | WHILE LP Exp RP Stmt {
        $$ = new Node("Stmt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
        $$->add_sub($5);
    }
    ;
/* local definition */
DefList: {
        $$ = new Node();
    }
    | Def DefList {
        $$ = new Node("DefList", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    ;
Def:
    Specifier DecList SEMI {
        $$ = new Node("Def", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);        
    }
    ;
DecList:
    Dec {
        $$ = new Node("DecList", @$.first_line);
        $$->add_sub($1);
    }
    | Dec COMMA DecList {
        $$ = new Node("DecList", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    ;
Dec:
    VarDec {
        $$ = new Node("Dec", @$.first_line);
        $$->add_sub($1);
    }
    | VarDec ASSIGN Exp {
        $$ = new Node("Dec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    ;
/* Expression */
Exp:
    Exp ASSIGN Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp AND Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);        
    }
    | Exp OR Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp LT Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp LE Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp GT Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp GE Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp NE Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp EQ Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp PLUS Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp MINUS Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp MUL Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Exp DIV Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | LP Exp RP {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | MINUS Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    | NOT Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    | ID LP Args RP {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
    }
    | ID LP RP {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);        
    }
    | Exp LB Exp RB {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);    
    }
    | Exp DOT ID {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);        
    }
    | ID {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);        
    }
    | INT {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);
    }
    | FLOAT {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);    
    }
    | CHAR {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);
    }
    ;
Args:
    Exp COMMA Args {
        $$ = new Node("Args", @$.first_line); 
        $$->add_sub($1);        
        $$->add_sub($2);
        $$->add_sub($3);        
    }
    | Exp {
        $$ = new Node("Args", @$.first_line); 
        $$->add_sub($1);          
    }
    ;

%%
void traverse(string tab, Node* root) {
    if(root->is_empty) return;
    vector<Node*> subs = root->sub_nodes;
    std::cout << tab;
    if(root->is_terminal){
        if (root->is_key)
        {
            std::cout << root->name << std::endl;
        }
        else
        {
            std::cout << root->name << ": " << root->value << endl;
        }
    }
    else{
        std::cout << root->name << " (" << root -> line_num << ")" << std::endl;
    }
    tab = tab + "  ";
    for(Node* n : subs){
        traverse(tab, n);
    }
}

void yyerror(const char *s){
    isError=1;
    fprintf(PARSER_error_OUTPUT,"Error type B at Line %d: ",yylloc.first_line-1);
    //fprintf(PARSER_error_OUTPUT, "syntax Error: %s\n", s);
    //lineinfor();
}

/* void lineinfor(void){
    // fprintf(PARSER_error_OUTPUT, "begin at:(%d,%d)\n",yylloc.first_line,yylloc.first_column);
    // fprintf(PARSER_error_OUTPUT, "end at:(%d,%d)\n",yylloc.last_line,yylloc.last_column);
} */

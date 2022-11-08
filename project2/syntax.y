%{
    #include "lex.yy.c"
    #include "semantic_check.hpp"
    #include <unordered_map> //hash_map
    #include <iostream>
    void yyerror(const char *s);
    void error_info(std::string s);
    void lineinfor(void);
    void traverse(string tab, Node *node);
    Node* root;
    extern int isError;
    #define PARSER_error_OUTPUT stdout
    std::unordered_map<std::string, Variable *> variable_map;
    //yydebug = 1;
%}
%locations
%union{
    Node* node;
}
%token INT FLOAT CHAR ID TYPE STRUCT IF ELSE WHILE RETURN DOT SEMI COMMA ASSIGN     
%token GT GE NE EQ PLUS MINUS MUL DIV AND OR NOT LP RP LB RB LC RC LT LE ERROR FOR

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

%type<node> INT FLOAT CHAR ID TYPE STRUCT IF ELSE WHILE RETURN DOT SEMI COMMA ASSIGN LT      
%type<node> LE GT GE NE EQ PLUS MINUS MUL DIV AND OR NOT LP RP LB RB LC RC ERROR FOR

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
    |  Specifier ExtDecList error {error_info("Missing semicolon ';'");}
    |  Specifier error {error_info("Missing semicolon ';'");}
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
    | VarDec ExtDecList error {error_info("Missing comma ','");}
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
    | STRUCT ID LC DefList error {error_info("Missing closing curly braces '}'");}
    | STRUCT ID DefList RC error {error_info("Missing beginning curly braces '{'");}
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
    | ERROR {
        $$ = new Node("VarDec", @$.first_line);
        $$->add_sub($1);     
    }
    | VarDec LB INT error {error_info("Missing closing bracket ']'");}
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
    | ID LP error { error_info("Missing closing parenthesis ')'"); }
    | ID LP VarList error { error_info("Missing closing parenthesis ')'"); }
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
    /* | LC DefList StmtList error %prec LOWER_ERROR {error_info("Missing closing curly braces '}'");} */
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
    | IF LP Exp RP Stmt %prec LOWER_ELSE{
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
    | FOR LP Specifier Dec SEMI Exp SEMI Exp RP Stmt {
        $$ = new Node("Stmt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
        $$->add_sub($5);
        $$->add_sub($6);
        $$->add_sub($7);
        $$->add_sub($8);
        $$->add_sub($9);
        $$->add_sub($10);
    }
    | RETURN Exp error { error_info("Missing semicolon ';'"); }
    | Exp error { error_info("Missing semicolon ';'"); }
    | IF LP Exp error Stmt { error_info("Missing closing parenthesis ')'"); }
    | IF LP Exp error Stmt ELSE Stmt {error_info("Missing closing parenthesis ')'");}
    | WHILE LP Exp error Stmt {error_info("Missing closing parenthesis ')'");}
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
        
        std::string type = $1->sub_nodes[0]->value;
        def_variable($2);
        assign_type(type, $2);
    
    }
    | error DecList SEMI {error_info("Missing specifier"); }
    | Specifier DecList error { error_info("Missing semicolon ';'"); }
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
    | LP Exp error {error_info("Missing closing parenthesis ')'");}
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
    | ID LP Args error {error_info("Missing closing parenthesis ')'");}
    | ID LP RP {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);        
    }
    | ID LP error {error_info("Missing closing parenthesis ')'");}
    | Exp LB Exp RB {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);    
        
        if(!array_index_check($1)) std::cout << "Type 10 error at line " << @$.first_line << " applying indexing operator ([...]) on non-array type variables\n";
        if(!check_is_int($3)) std::cout << "Type 12 error at line " << @$.first_line << " array indexing with a non-integer type expression\n";
        
    }
    | Exp LB Exp error {error_info("Missing closing bracket ']'");}
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
    | ERROR {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);
    }
    | Exp ERROR Exp {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
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
    fprintf(stderr,"Error type B at Line %d: ",yylloc.first_line-1);
}

void error_info(std::string s){
    std::cerr << s << std::endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "no input path");
        return 1;
    } else if (argc > 2) {
        fprintf(stderr, "too much input path");
        return 1;
    } else {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, "error of path %s", argv[1]);
            return 1;
        }
        yyrestart(f);
        /* yydebug = 1; */
        yyparse();
    }
    for ( auto var = variable_map.begin(); var != variable_map.end(); ++var)
    {
        std::cout << var->first << ": " << var->second->t << " is_array: " << var->second->is_array << std::endl;
    }
    /* if (isError == 0) {
        traverse("", root);
    } */
    return 0;
}
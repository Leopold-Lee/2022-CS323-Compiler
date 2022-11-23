%{
    #include "lex.yy.c"
    #include "semantic_check.hpp"
    #include <unordered_map> //hash_map
    #include <iostream>
    #include <stack>
    void yyerror(const char *s);
    void error_info(std::string s);
    void lineinfor(void);
    void traverse(string tab, Node *node);
    Variable *find_variable(std::string ID); 
    Node* root;
    extern int isError;
    #define PARSER_error_OUTPUT stdout
    std::unordered_map<std::string, Variable *> *variable_map = new std::unordered_map<std::string, Variable*>();
    std::unordered_map<std::string, Function*> function_map;
    std::unordered_map<std::string, my_struct*> struct_map;
    std::vector<std::unordered_map<std::string, Variable *> *> scope;
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
%type<node> LE GT GE NE EQ PLUS MINUS MUL DIV AND OR NOT LP RP LB RB LC RC ERROR FOR LC_T RC_T Fun_Scope

%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier VarDec Specifier_FunDec
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
    Specifier ExtDecList SEMI { //define global variables
        $$ = new Node("ExtDef", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        if ($1->sub_nodes[0]->name.compare("TYPE") == 0) {
            std::string type = $1->sub_nodes[0]->value;
            def_variable($2);
            assign_type(type, $2, false);
        } 
        else {
            def_variable($2);
            assign_type($1->at.struct_name, $2, true);
        }
    }
    | Specifier SEMI { //define struct
        $$ = new Node("ExtDef", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
    }
    | Specifier_FunDec CompSt { //define functions
        $$ = new Node("ExtDef", @$.first_line);
        $$->add_sub($1->sub_nodes[0]);
        $$->add_sub($1->sub_nodes[1]);
        $$->add_sub($2);
        std::string fun_name = $1->sub_nodes[1]->sub_nodes[0]->value;
        if (function_map.count(fun_name) && $1->at.type != TYPE_ERROR) {
            Function* fun = function_map[fun_name];
            check_return(fun->return_type, $2);
        }
        scope.pop_back();
        if(!scope.empty()) {
            variable_map = scope.back();
            // cout << variable_map->size();
        }
        else {
            variable_map = NULL;
        }
    }
    |  Specifier ExtDecList error {error_info("Missing semicolon ';'");}
    |  Specifier error {error_info("Missing semicolon ';'");}
    ;
Specifier_FunDec:
    Specifier FunDec {
        $$ = new Node("Specifier_FunDec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        std::string fun_name = $2->sub_nodes[0]->value;
        if (function_map.count(fun_name)) {
            semantic_error(4, $$->line_num, fun_name);
            $$->at.type = TYPE_ERROR;
        }
        else{
            Function *fun = new Function(fun_name, $1, $2);
            function_map[fun_name] = fun;
        }
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
    | VarDec ExtDecList error {error_info("Missing comma ','");}
    ;
/* specifier */
Specifier:
    TYPE {
        $$ = new Node("Specifier", @$.first_line);
        $$->add_sub($1);
        string type = $1->value;
        if (type.compare("int") == 0) {
            $$->at.type = TYPE_INT;
        }
        else if (type.compare("char") == 0) {
            $$->at.type = TYPE_CHAR;
        }
        else if (type.compare("float") == 0) {
            $$->at.type = TYPE_FLOAT;
        }
    }
    | StructSpecifier {
        $$ = new Node("Specifier", @$.first_line);
        $$->add_sub($1);        
        $$->at.type = TYPE_STRUCT;
        $$->at.struct_name = $1->at.struct_name;
    }
    ;
StructSpecifier: 
    STRUCT ID LC_T DefList RC_T {
        $$ = new Node("StructSpecifier", @$.first_line);
        $$->add_sub($1);  
        $$->add_sub($2);  
        $$->add_sub($3);  
        $$->add_sub($4);  
        $$->add_sub($5);  
        my_struct* stc = new my_struct($2->value);
        if(struct_map.count($2->value)) {
            semantic_error(15, $$->line_num, $2->value);
            // std::cout << "Error type 15 at Line " << $$.line_num << ": redefine struct: "<< $2->value << std::endl;
        }
        else {
            struct_map[$2->value] = stc;
            def_struct(stc, $4);
        }
        $$->at.struct_name = $2->value;
    }
    | STRUCT ID {
        $$ = new Node("StructSpecifier", @$.first_line);
        $$->add_sub($1);  
        $$->add_sub($2);  
        if (!struct_map.count($2->value)) {
            cout << "Error: struct " << $2->value << " not defined" << endl; 
        }
        $$->at.struct_name = $2->value;
    }
    | STRUCT ID LC_T DefList error {error_info("Missing closing curly braces '}'");}
    | STRUCT ID DefList RC_T error {error_info("Missing beginning curly braces '{'");}
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
    Fun_Scope LP VarList RP {
        $$ = new Node("FunDec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
    }
    | Fun_Scope LP RP {
        $$ = new Node("FunDec", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
    }
    | Fun_Scope LP error { error_info("Missing closing parenthesis ')'"); }
    | Fun_Scope LP VarList error { error_info("Missing closing parenthesis ')'"); }
    ;
Fun_Scope:
    ID
    {
        $$ = $1;
        std::unordered_map<std::string, Variable *> * new_variable_map = new unordered_map<std::string, Variable *>();
        scope.push_back(new_variable_map);
        variable_map = new_variable_map;
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

        if ($1->sub_nodes[0]->name.compare("TYPE") == 0) {
            std::string type = $1->sub_nodes[0]->value;
            def_variable($2);
            assign_type(type, $2, false);
        } 
        else {
            std::string struct_type = $1->sub_nodes[0]->sub_nodes[1]->value;
            def_variable($2);
            assign_type(struct_type, $2, true);
        }
    }
    ;
/* statement */
CompSt:
    LC_T DefList StmtList RC_T{
        $$ = new Node("CompSt", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
    }
    /* | LC DefList StmtList error %prec LOWER_ERROR {error_info("Missing closing curly braces '}'");} */
    ;
LC_T:
    LC {
        $$ = $1;
        std::unordered_map<std::string, Variable *> * new_variable_map = new unordered_map<std::string, Variable *>();
        scope.push_back(new_variable_map);
        variable_map = new_variable_map;
        // cout << variable_map << endl;
    }
    ;
RC_T:
    RC {
        $$ = $1;
        scope.pop_back();
        if(!scope.empty()) {
            variable_map = scope.back();
            // cout << variable_map->size();
        }
        else {
            variable_map = NULL;
        }
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
        
        if ($1->sub_nodes[0]->name.compare("TYPE") == 0) {
            std::string type = $1->sub_nodes[0]->value;
            def_variable($2);
            assign_type(type, $2, false);
        } 
        else {
            std::string struct_type = $1->sub_nodes[0]->sub_nodes[1]->value;
            def_variable($2);
            assign_type(struct_type, $2, true);
        }
    
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
        check_rvalue($$, @$.first_line);
        v_type t1 = $1->at;
        v_type t2 = $3->at;
        if (t2.type != TYPE_ERROR && (t1.type != t2.type || t1.struct_name != t2.struct_name || t1.array_dim != t2.array_dim))
        {
            semantic_error(5, $$->line_num, "");
            // std::cout << "Type 5 error at line: " << $$->line_num << " unmatching types appear at both sides of the assignment operatot" << std::endl;
        }
    }
    | Exp AND Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);        
        $$->at.type = TYPE_INT;
    }
    | Exp OR Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at.type = TYPE_INT;
    }
    | Exp LT Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at.type = TYPE_INT;
    }
    | Exp LE Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at.type = TYPE_INT;
    }
    | Exp GT Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at.type = TYPE_INT;
    }
    | Exp GE Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at.type = TYPE_INT;
    }
    | Exp NE Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at.type = TYPE_INT;
    }
    | Exp EQ Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at.type = TYPE_INT;
    }
    | Exp PLUS Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at = $1->at;
        if ($1->at.type != $3->at.type && !($1->at.type == TYPE_ERROR || $3->at.type == TYPE_ERROR)) {
            semantic_error(7, $$->line_num, "");
            $$->at.type = TYPE_ERROR;
        }
    }
    | Exp MINUS Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at = $1->at;
        if ($1->at.type != $3->at.type && !($1->at.type == TYPE_ERROR || $3->at.type == TYPE_ERROR)) {
            semantic_error(7, $$->line_num, "");
            $$->at.type = TYPE_ERROR;
        }
    }
    | Exp MUL Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at = $1->at;
        if ($1->at.type != $3->at.type && !($1->at.type == TYPE_ERROR || $3->at.type == TYPE_ERROR)) {
            semantic_error(7, $$->line_num, "");
            $$->at.type = TYPE_ERROR;
        }
    }
    | Exp DIV Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at = $1->at;
        if ($1->at.type != $3->at.type && !($1->at.type == TYPE_ERROR || $3->at.type == TYPE_ERROR)) {
            semantic_error(7, $$->line_num, "");
            $$->at.type = TYPE_ERROR;
        }
    }
    | LP Exp RP {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->at = $2->at;
    }
    | LP Exp error {error_info("Missing closing parenthesis ')'");}
    | MINUS Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->at = $2->at;
    }
    | NOT Exp {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->at = $2->at;
    }
    | ID LP Args RP {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);
        if (find_variable($1->value)) {
            semantic_error(11, $$->line_num, $1->value);
            $$->at.type = TYPE_ERROR;
            // cout << "Error type 11 at " << $$->line_num << "applying function invocation operator (foo(...)) on non-function names" << endl;
        }
        else{
            if (function_map.count($1->value)) {
                Function* fun = function_map[$1->value];
                v_type* t = fun->return_type;
                $$->at.type = t->type;
                $$->at.struct_name = t->struct_name;
                $$->at.array_dim = t->array_dim;
                check_fun($1, $3);
            }
            else {
                semantic_error(2, $$->line_num, $1->value);
                $$->at.type = TYPE_ERROR;
                // cout << "Type 11 error at line " << $$->line_num << "applying function invocation operator (foo(...)) on non-function names" << endl;
            }
        }
    }
    | ID LP Args error {error_info("Missing closing parenthesis ')'");}
    | ID LP RP {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);      
        check_fun($1, NULL);  
        if (find_variable($1->value)) {
            semantic_error(11, $$->line_num, "");
            $$->at.type = TYPE_ERROR;
            // cout << "Error type 11 at " << $$->line_num << "applying function invocation operator (foo(...)) on non-function names" << endl;
        }
        else {
            if (function_map.count($1->value)) {
                Function* fun = function_map[$1->value];
                v_type* t = fun->return_type;
                $$->at.type = t->type;
                $$->at.struct_name = t->struct_name;
                $$->at.array_dim = t->array_dim;
                check_fun($1, $3);
            }
            else {
                semantic_error(2, $$->line_num, $1->value);
                $$->at.type = TYPE_ERROR;
                // cout << "Type 11 error at line " << $$->line_num << "applying function invocation operator (foo(...)) on non-function names" << endl;
            }
        }
    }
    | ID LP error {error_info("Missing closing parenthesis ')'");}
    | Exp LB Exp RB {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        $$->add_sub($4);    
        if ($1->at.array_dim <= 0) {
            semantic_error(10, $$->line_num, "");
            // std::cout << "Type 10 error at line " << @$.first_line << " applying indexing operator ([...]) on non-array type variables\n";
        }
        if ($3->at.type != TYPE_INT){
            semantic_error(12, $$->line_num, "");
            // std::cout << "Type 12 error at line " << @$.first_line << " array indexing with a non-integer type expression\n";
        }
        v_type t = $1->at;
        $$->at.type = t.type;
        $$->at.struct_name = t.struct_name;
        $$->at.array_dim = t.array_dim - 1;
        
    }
    | Exp LB Exp error {error_info("Missing closing bracket ']'");}
    | Exp DOT ID {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        $$->add_sub($2);
        $$->add_sub($3);
        v_type t = $1->at;
        if (t.array_dim == 0 && struct_map.count(t.struct_name)) {
            v_type *temp = check_struct_member(struct_map[t.struct_name], $3->value, $$->line_num);
            if (temp) {
                $$->at.type = temp->type;
                $$->at.struct_name = temp->struct_name;
                $$->at.array_dim = temp->array_dim;
            }
        }
        else {
            semantic_error(13, $$->line_num, "");
            // std::cout << "Type 13 error: at line " << $$->line_num << " accessing members of a non-structure variable" << std::endl;
        }
    }
    | ID {
        $$ = new Node("Exp", @$.first_line);
        $$->add_sub($1);
        Variable *v = find_variable($1->value); 
        if (v) {
            v_type* t = v->t;
            $$->at.type = t->type;
            $$->at.struct_name = t->struct_name;
            $$->at.array_dim = t->array_dim;
        }
        else {
            semantic_error(1, $$->line_num, $1->value);
            $$->at.type = TYPE_ERROR;
            // cout << "Error type 1 at Line "<< $$->line_nume << ": undefined variable: "<< $1->value << std::endl; 
        }
    }
    | INT {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);
        $$->at.type = TYPE_INT;
    }
    | FLOAT {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);    
        $$->at.type = TYPE_FLOAT;
    }
    | CHAR {
        $$ = new Node("Exp", @$.first_line); 
        $$->add_sub($1);
        $$->at.type = TYPE_CHAR;
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
        std::cout << root->name << " (" << root -> line_num << ")";
        if (root->name.compare("Exp") == 0) {
            v_type t = root->at;
            std::cout << t.type << " " << t.struct_name << " " << t.array_dim;
        }
        std::cout << std::endl;
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

Variable *find_variable(std::string ID) {
    int l = scope.size() - 1;
    while (l >= 0) {
        if(scope[l]->count(ID)) {
            return scope[l]->at(ID);
        }
        l --;
    }
    return NULL;
}
int main(int argc, char **argv) {
    scope.push_back(variable_map);
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
    /* std::cout << "traversal variable map:\n";
    for ( auto var = variable_map.begin(); var != variable_map.end(); ++var)
    {
        std::cout << var->first << ": " << var->second->t->type << " is_array: " << var->second->t->array_dim << std::endl;
    }
    std::cout << "traversal function map:\n";
    for ( auto var = function_map.begin(); var != function_map.end(); ++var)
    {
        Function * fun = var->second;
        std::cout << var->first << ": return type:" << fun->return_type->type;
        for(v_type* t: fun->parameters)
        {
            std::cout << " parameter" << " type:" << t->type;
        }
        std::cout << std::endl;
    }
    std::cout << "traversal struct map:\n";
    for ( auto var = struct_map.begin(); var != struct_map.end(); ++var)
    {
        my_struct* stc = var->second;
        std::cout << var->first;
        for(Variable* v: stc->variables)
        {
            std::cout << " " << v->name << " type:" << v->t->type;
        }
        std::cout << std::endl;
    } */
    /* if (isError == 0) {
        traverse("", root);
    } */
    return 0;
}
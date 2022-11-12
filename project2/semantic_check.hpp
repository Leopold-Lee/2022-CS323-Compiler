#pragma once
#include "node.hpp"
#include "function.hpp"
#include "variable.hpp"
#include "unordered_map"
#include "struct.hpp"
extern std::unordered_map<std::string, Variable *> variable_map;
extern std::unordered_map<std::string, Function*> function_map;
extern std::unordered_map<std::string, my_struct*> struct_map;

bool array_index_check(Node *check) {
    Node *temp = check;
    int dimension = 0;
    while(!temp->sub_nodes.empty()) {
        dimension ++;
        temp = temp->sub_nodes[0];
    }
    if(dimension && temp->name.compare("ID") == 0) {
        // std::cout << temp->name << std::endl;
        // std::cout << "is id\n";
        if(variable_map.count(temp->value)){
            // std::cout << "exsit variable\n";
            Variable *variable = variable_map[temp->value];
            if (variable->is_array && variable->array_dimension >= dimension)
            {
                return true;
                // std::cout << "is array\n";
            }
        }
    }
    return false;
}
///////////////////////////////////////
void def_variable(Node *node) {
    for(Node *sub : node->sub_nodes)
    {
        def_variable(sub);
    }
    if(node->name.compare("VarDec")==0) {
        Node* temp = node->sub_nodes[0];
        if(temp->name.compare("ID")==0) {
            std::string variable_name = temp->value;
            if(variable_map.count(variable_name))
            {
                std::cout << "variable redefined\n"; 
            }
            else{
                Variable* new_variable = new Variable(variable_name);
                variable_map[variable_name] = new_variable;
            }
        }
        else if(temp->name.compare("VarDec")==0) {
            int dimension = 0;
            while(!temp->sub_nodes.empty())
            {
                dimension ++;
                temp = temp->sub_nodes[0];
            }
            if(dimension && temp->name.compare("ID") == 0){
                std::string variable_name = temp->value;
                Variable* variable = variable_map[variable_name];
                variable->is_array = true;
                variable->array_dimension = dimension;
                // std::cout << variable->name << " " << variable->is_array << std::endl;
            }
        }
    }
    
}


void assign_type(std::string type, Node * dec, bool is_struct) {
    if(dec->name.compare("ID") == 0) {
        std::string name = dec->value;
        if (!is_struct) {
            if (type.compare("int") == 0) {
                variable_map[name]->t = TYPE_T::TYPE_INT;
            }
            else if (type.compare("char") == 0) {
                variable_map[name]->t = TYPE_T::TYPE_CHAR;
            }
            else if (type.compare("float") == 0) {
                variable_map[name]->t = TYPE_T::TYPE_FLOAT;
            }
        }
        else {
            Variable * var = variable_map[name];
            var->is_struct = true;
            var->struct_name = type;
        }
        return;
    }
    vector<Node*> subs = dec->sub_nodes;
    for(Node* n : subs){
        assign_type(type, n, is_struct);
    }
}

bool check_is_type(TYPE_T type, Node* exp) {
    bool is_type = true;
    if(exp->is_terminal) {
        if(exp->name.compare("ID") == 0) {
            std::string name = exp->value;
            Variable* var = variable_map[name];
            if(!(var->t == type)) is_type = false;
        }
        else if(exp->name.compare("INT") == 0) {
            if(!(type == TYPE_INT)) is_type = false;
        }
        else if(exp->name.compare("FLOAT") == 0) {
            if(!(type == TYPE_FLOAT)) is_type = false;
        }
        else if(exp->name.compare("CHAR") == 0) {
            if(!(type == TYPE_CHAR)) is_type = false;
        }
    }
    vector<Node*> subs = exp->sub_nodes;
    for(Node* n : subs){
        if(!check_is_type(type, n)) {
            is_type = false;
        }
    }
    return is_type;
}

void check_return(TYPE_T return_type, Node* stmt)
{
    if(stmt->name.compare("Stmt") == 0) {
        if (stmt->sub_nodes[0]->name.compare("RETURN") == 0) {
            bool match = check_is_type(return_type, stmt->sub_nodes[1]);
            // cout << "return type matched: " << match << endl;
            if (!match) cout << "Type 8 error: a function’s return value type mismatches the declared type" << endl;
        }
    }
    vector<Node*> subs = stmt->sub_nodes;
    for(Node* n : subs){
        check_return(return_type, n);
    }
}


TYPE_T get_exp_type(Node* exp) {
    TYPE_T type = TYPE_UNKNOW;
    if(exp->is_terminal) {
        if(exp->name.compare("ID") == 0) {
            std::string name = exp->value;
            Variable* var = variable_map[name];
            type = var->t;
        }
        else if(exp->name.compare("INT") == 0) {
            type = TYPE_INT;
        }
        else if(exp->name.compare("FLOAT") == 0) {
            type = TYPE_FLOAT;
        }
        else if(exp->name.compare("CHAR") == 0) {
            type = TYPE_CHAR;
        }
    }
    vector<Node*> subs = exp->sub_nodes;
    for(Node* n : subs){
        TYPE_T temp = get_exp_type(n);
        // type = get_exp_type(n);
        if(type == TYPE_UNKNOW) type = temp;
        else {
            if (temp != TYPE_UNKNOW && temp != type) cout << "Exp type mismatch" << endl;
        }
    }
    return type;
}

//handle array and struct

void get_type(vector<TYPE_T> & type, Node* args) {
    if(args->name.compare("Exp") == 0) {
        TYPE_T exp_type = get_exp_type(args);
        type.push_back(exp_type);
    }
    else {
        vector<Node*> subs = args->sub_nodes;
        for(Node* n : subs){
            get_type(type, n);
        }
    }
}

void check_fun(Node *id, Node* args) {
    if (!function_map.count(id->value)) {
        cout << "Type 11 error: applying function invocation operator (foo(...)) on non-function names" << endl;
    }
    else {
        vector<TYPE_T> args_type;
        if (args) get_type(args_type, args);
        Function *fun = function_map[id->value];
        vector<TYPE_T> fun_type = fun->parameters;
        // cout << args_type.size() << endl;
        if(args_type.size() == fun_type.size()) {
            for (size_t i = 0; i < args_type.size(); i++)
            {
                // cout << args_type[i] << endl;
                if(args_type[i] != fun_type[i]) {
                    cout << "Type 9 a function’s arguments mismatch the declared parameters" << endl;
                    break;
                }
            }
        }
        else cout << "Type 9 a function’s arguments mismatch the declared parameters" << endl;
    }
}

void def_struct_type(my_struct* stc, TYPE_T type, Node* node) {
    for(Node *sub : node->sub_nodes)
    {
        def_struct_type(stc, type, sub);
    }
    if(node->name.compare("VarDec")==0) {
        Node* temp = node->sub_nodes[0];
        if(temp->name.compare("ID")==0) {
            std::string variable_name = temp->value;
            Variable* new_variable = new Variable(variable_name);
            new_variable->t = type;
            stc->variables.push_back(new_variable);
            // variable_map[variable_name] = new_variable;
        }
        else if(temp->name.compare("VarDec")==0) {
            int dimension = 0;
            while(!temp->sub_nodes.empty())
            {
                dimension ++;
                temp = temp->sub_nodes[0];
            }
            if(dimension && temp->name.compare("ID") == 0){
                std::string variable_name = temp->value;
                for (Variable *var : stc->variables)
                {
                    if (var->name.compare(variable_name) == 0) {
                        var->is_array = true;
                        var->array_dimension = dimension;
                    }
                }
            }
        }
    }
}

void def_struct(my_struct* stc, Node* node) {
    for(Node *sub : node->sub_nodes)
    {
        def_struct(stc, sub);
    }
    if(node->name.compare("Def")==0) {
        Node* specifier = node->sub_nodes[0]->sub_nodes[0];
        if (specifier->name.compare("TYPE") == 0) {
            string type = specifier->value;
            TYPE_T var_type;
            if (type.compare("int") == 0) {
                var_type = TYPE_INT;
            }
            else if (type.compare("char") == 0) {
                var_type = TYPE_CHAR;
            }
            else if (type.compare("float") == 0) {
                var_type = TYPE_FLOAT;
            }
            def_struct_type(stc, var_type, node->sub_nodes[1]);
        }
        return;
    }
}

void check_struct_member(Node* exp, Node* ID) {
    Node *temp = exp->sub_nodes[0];
    if (temp->name.compare("ID") == 0) {
        string name = temp->value;
        // cout << name;
        if (variable_map.count(name)) {
            string stc_name = variable_map[name]->struct_name;
            // cout << stc_name;
            if (struct_map.count(stc_name)) {
                bool has_member = false;
                my_struct* stc = struct_map[stc_name];
                for(Variable * var: stc->variables) {
                    if(var->name.compare(ID->value) == 0) {
                        has_member = true;
                    }
                }
                if (!has_member) {
                    cout << "Type 14 error: accessing an undefined structure member" << endl;
                }
            }
            else {
                cout << "Type 13 error: accessing members of a non-structure variable" << endl;
            }
        }
        
    }
}
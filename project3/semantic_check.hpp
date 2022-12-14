#pragma once
#include "node.hpp"
#include "function.hpp"
#include "variable.hpp"
#include "unordered_map"
#include "struct.hpp"
extern std::unordered_map<std::string, Variable *> *variable_map;
extern std::unordered_map<std::string, Function*> function_map;
extern std::unordered_map<std::string, my_struct*> struct_map;

void semantic_error(int i, int line, string content) {
    cout << "Error type "<< i << " at Line " << line << ": ";
    switch(i){
        case 1:
            cout << "undefined variable: " << content;
        break;
        case 2:
            cout << "undefined function: " << content;
        break;
        case 3:
            cout << "redefine variable: " << content;
        break;
        case 4:
            cout << "redefine function: " << content;
        break;
        case 5:
            cout << "unmatching type on both sides of assignment";
        break;
        case 6:
            cout << "left side in assignment is rvalue";
        break;
        case 7:
            cout << "unmatching operands";
        break;
        case 8:
            cout << "incompatiable return type";
        break;
        case 9:
            cout << "a function’s arguments mismatch the declared parameters";
        break;
        case 10:
            cout << "indexing on non-array variable";
        break;
        case 11:
            cout << "invoking non-function variable: " << content;
        break;
        case 12:
            cout << "indexing by non-integer";
        break;
        case 13:
            cout << "accessing with non-struct variable";
        break;
        case 14:
            cout << "no such member: " << content;
        break;
        case 15:
            cout << "redefine struct: " << content;
        break;
    }
    cout << endl;
}



void def_variable(Node *node) {
    for(Node *sub : node->sub_nodes)
    {
        def_variable(sub);
    }
    if(node->name.compare("VarDec")==0) {
        Node* temp = node->sub_nodes[0];
        if(temp->name.compare("ID")==0) {
            std::string variable_name = temp->value;
            if(variable_map->count(variable_name))
            {
                semantic_error(3, node->line_num, variable_name);
                // std::cout << "variable redefined\n"; 
            }
            else{
                Variable* new_variable = new Variable(variable_name);
                variable_map->emplace(variable_name, new_variable);
            }
        }
        else if(temp->name.compare("VarDec")==0) {
            int dimension = 0;
            vector<int> array_rec; array_rec.push_back(stoi(node->sub_nodes[2]->value));
            while(!temp->sub_nodes.empty())
            {
                if (temp->sub_nodes.size() == 4) array_rec.push_back(stoi(temp->sub_nodes[2]->value));
                dimension ++;
                temp = temp->sub_nodes[0];
            }
            if(dimension && temp->name.compare("ID") == 0){
                std::string variable_name = temp->value;
                Variable* variable = variable_map->at(variable_name);
                variable->t->array_dim = dimension;
                variable->t->array_rec = array_rec;
            }
        }
    }
    
}


void assign_type(std::string type, Node * dec, bool is_struct) {
    if(dec->name.compare("VarDec") == 0) {
        Node* temp = dec;
        while(!temp->sub_nodes.empty())
        {
            temp = temp->sub_nodes[0];
        }
        std::string name = temp->value;
        if (!is_struct) {
            if (type.compare("int") == 0) {
                variable_map->at(name)->t->type = TYPE_T::TYPE_INT;
            }
            else if (type.compare("char") == 0) {
                variable_map->at(name)->t->type = TYPE_T::TYPE_CHAR;
            }
            else if (type.compare("float") == 0) {
                variable_map->at(name)->t->type = TYPE_T::TYPE_FLOAT;
            }
        }
        else {
            Variable * var = variable_map->at(name);
            var->t->type = TYPE_STRUCT;
            var->t->struct_name = type;
        }
        return;
    }
    vector<Node*> subs = dec->sub_nodes;
    for(Node* n : subs){
        assign_type(type, n, is_struct);
    }
}

void check_return(v_type* return_type, Node* stmt)
{
    if(stmt->name.compare("Stmt") == 0) {
        if (stmt->sub_nodes[0]->name.compare("RETURN") == 0) {
            // bool match = check_is_type(return_type, stmt->sub_nodes[1]);
            Node* exp = stmt->sub_nodes[1];
            bool match = exp->at.type == return_type->type;
            if (match && return_type->type == TYPE_STRUCT) {
                match = exp->at.struct_name.compare(return_type->struct_name)==0;
            }
            // cout << "return type matched: " << match << endl;
            if (!match && exp->at.type != TYPE_ERROR) semantic_error(8, stmt->line_num, ""); //cout << "Type 8 error at line " << stmt->line_num << ": a function’s return value type mismatches the declared type" << endl;
        }
    }
    vector<Node*> subs = stmt->sub_nodes;
    for(Node* n : subs){
        check_return(return_type, n);
    }
}

void get_type(vector<v_type> & type, Node* args) {
    if(args->name.compare("Exp") == 0) {
        // TYPE_T exp_type = get_exp_type(args);
        type.push_back(args->at);
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
        // cout << "Type 11 error at line " << args->line_num << "applying function invocation operator (foo(...)) on non-function names" << endl;
    }
    else {
        vector<v_type> args_type;
        if (args) get_type(args_type, args);
        Function *fun = function_map[id->value];
        vector<v_type*> fun_type = fun->parameters;
        if(args_type.size() == fun_type.size()) {
            for (size_t i = 0; i < args_type.size(); i++)
            {
                // cout << args_type[i] << endl;
                if(args_type[i].type != fun_type[i]->type || args_type[i].struct_name.compare(fun_type[i]->struct_name) != 0 || args_type[i].array_dim != fun_type[i]->array_dim) {
                    semantic_error(9, args->line_num, "");
                    // cout << "Type 9 at line " << args->line_num << ": a function’s arguments mismatch the declared parameters" << endl;
                    break;
                }
            }
        }
        else cout << "Error type 9 at Line " << args->line_num << ": invalid argument number for " << id->value << ", expect " << fun_type.size() <<", got "<< args_type.size() << endl;
    }
}

void def_struct_type(my_struct* stc, TYPE_T type, string struct_name, Node* node) {
    for(Node *sub : node->sub_nodes)
    {
        def_struct_type(stc, type, struct_name, sub);
    }
    if(node->name.compare("VarDec")==0) {
        Node* temp = node->sub_nodes[0];
        if(temp->name.compare("ID")==0) {
            std::string variable_name = temp->value;
            Variable* new_variable = new Variable(variable_name);
            new_variable->t->type = type;
            if (type == TYPE_STRUCT) {
                new_variable->t->struct_name = struct_name;
            }
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
                        var->t->array_dim = dimension;
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
            def_struct_type(stc, var_type, "", node->sub_nodes[1]);
        }
        else {
            string struct_name = specifier->sub_nodes[1]->value;
            def_struct_type(stc, TYPE_STRUCT, struct_name, node->sub_nodes[1]);
        }
        return;
    }
}

v_type* check_struct_member(my_struct* stc, string member, int line) {
    bool has_member = false;
    // my_struct* stc = struct_map[stc_name];
    for(Variable * var: stc->variables) {
        if(var->name.compare(member) == 0) {
            has_member = true;
            return var->t;
        }
    }
    if (!has_member) {
        semantic_error(14, line, member);
        // cout << "Type 14 error: at line " << line << " accessing an undefined structure member" << endl;
    }
    return NULL;
}

void check_rvalue(Node *node, int line) {
    Node *left_node = node->sub_nodes[0];
    int size = left_node->sub_nodes.size();
    if (!((size == 1 && left_node->sub_nodes[0]->name == "ID") || 
        (size == 3 && left_node->sub_nodes[0]->name == "Exp" && left_node->sub_nodes[1]->name == "DOT" && left_node->sub_nodes[2]->name == "ID")||
        (size == 4 && left_node->sub_nodes[0]->name == "Exp" && left_node->sub_nodes[1]->name == "LB" && left_node->sub_nodes[2]->name == "Exp" && left_node->sub_nodes[3]->name == "RB"))) {
        semantic_error(6, line, "");
    } 
}
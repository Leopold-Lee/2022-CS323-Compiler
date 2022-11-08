#pragma once
#include "node.hpp"
#include "variable.hpp"
#include "unordered_map"
extern std::unordered_map<std::string, Variable *> variable_map;

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


void assign_type(std::string type, Node * dec) {
    if(dec->name.compare("ID") == 0) {
        std::string name = dec->value;
        if (type.compare("int") == 0) {
            variable_map[name]->t = TYPE_T::TYPE_INT;
        }
        else if (type.compare("char") == 0) {
            variable_map[name]->t = TYPE_T::TYPE_CHAR;
        }
        else if (type.compare("float") == 0) {
            variable_map[name]->t = TYPE_T::TYPE_FLOAT;
        }
        return;
    }
    vector<Node*> subs = dec->sub_nodes;
    for(Node* n : subs){
        assign_type(type, n);
    }
}

bool check_is_int(Node* exp) {
    bool is_int = true;
    if(exp->is_terminal) {
        if(exp->name.compare("ID") == 0) {
            std::string name = exp->value;
            Variable* var = variable_map[name];
            if(!var->t == TYPE_T::TYPE_INT) is_int = false;
        }
    }
    vector<Node*> subs = exp->sub_nodes;
    for(Node* n : subs){
        if(!check_is_int(n)) {
            is_int = false;
        }
    }
    return is_int;
}
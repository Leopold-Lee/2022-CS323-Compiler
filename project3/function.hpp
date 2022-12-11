#pragma once
#include <string>
#include "node.hpp"
#include "variable.hpp"
#include <vector>
using namespace std;
class Function
{
public:
    string name;
    v_type* return_type;
    vector<v_type*> parameters; 
    Function(string name, Node* specifier, Node* funDec){
        this->name = name;
        v_type rt = specifier->at;
        return_type = new v_type(rt.type);
        return_type->struct_name = rt.struct_name;
        addParameters(funDec);
    }
    void addParameters(Node* node) {
        for(Node *sub : node->sub_nodes)
        {
            addParameters(sub);
        }
        if(node->name.compare("Specifier")==0) {
            v_type pt = node->at;
            v_type *para = new v_type(pt.type);
            para->struct_name = pt.struct_name;
            parameters.push_back(para);
        }
    }

    ~Function(){}
};

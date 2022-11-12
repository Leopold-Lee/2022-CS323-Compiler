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
    TYPE_T return_type;
    vector<TYPE_T> parameters; 
    Function(string name, Node* specifier, Node* funDec){
        this->name = name;
        Node* temp = specifier->sub_nodes[0];
        if(temp->name.compare("TYPE") == 0) {
            string type = temp->value;
            if (type.compare("int") == 0) {
            this->return_type = TYPE_T::TYPE_INT;
            }
            else if (type.compare("char") == 0) {
                this->return_type = TYPE_T::TYPE_CHAR;
            }
            else if (type.compare("float") == 0) {
                this->return_type = TYPE_T::TYPE_FLOAT;
            }
        }
        addParameters(funDec);
    }
    void addParameters(Node* node) {
        for(Node *sub : node->sub_nodes)
        {
            addParameters(sub);
        }
        if(node->name.compare("Specifier")==0) {
            Node *temp = node->sub_nodes[0];
            if(temp->name.compare("TYPE") == 0) {
                string type = temp->value;
                if (type.compare("int") == 0) {
                    parameters.push_back(TYPE_T::TYPE_INT);
                }
                else if (type.compare("char") == 0) {
                    parameters.push_back(TYPE_T::TYPE_CHAR);
                }
                else if (type.compare("float") == 0) {
                    parameters.push_back(TYPE_T::TYPE_FLOAT);
                }
            }
        }
    }

    ~Function(){}
};

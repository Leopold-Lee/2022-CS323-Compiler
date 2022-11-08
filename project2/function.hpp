#pragma once
#include <string>
#include <variable.hpp>
#include <node.hpp>
#include <vector>
using namespace std;
class Function
{
public:
    string name;
    TYPE_T return_type;
    vector<Variable *> parameters; 
    Function(string specifier, Node* funDec){
        if (specifier.compare("int") == 0) {
            this->return_type = TYPE_T::TYPE_INT;
        }
        else if (specifier.compare("char") == 0) {
            this->return_type = TYPE_T::TYPE_CHAR;
        }
        else if (specifier.compare("float") == 0) {
            this->return_type = TYPE_T::TYPE_FLOAT;
        }
    }
    void addParameters(Node* varList) {
        
    }

    ~Function(){}
};

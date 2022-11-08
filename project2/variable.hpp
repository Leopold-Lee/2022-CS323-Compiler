#pragma once
#include <string>
using namespace std;
enum TYPE_T {TYPE_INT, TYPE_FLOAT, TYPE_CHAR};
class Variable
{
public:
    string name;
    TYPE_T t;
    bool is_array;
    int array_dimension;
    Variable(string name){
        this->name = name;
    }
    ~Variable(){}
};

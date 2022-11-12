#pragma once
#include <string>
using namespace std;
enum TYPE_T {TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_UNKNOW};
class Variable
{
public:
    string name;
    TYPE_T t = TYPE_UNKNOW;
    bool is_array = 0;
    int array_dimension;
    bool is_struct = 0;
    string struct_name;
    Variable(string name){
        this->name = name;
    }
    ~Variable(){}
};

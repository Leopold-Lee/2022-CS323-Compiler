#pragma once
#include <string>
using namespace std;
enum TYPE_T {TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_UNKNOW, TYPE_STRUCT, TYPE_ERROR};

class v_type
{
public:
    TYPE_T type;
    string struct_name = "";
    int array_dim = 0;
    vector<int> array_rec;
    v_type() {
        this->type = TYPE_UNKNOW;
    }
    v_type(TYPE_T t) {
        this->type = t; 
    }
    v_type(string stc) {
        this->type = TYPE_STRUCT;
        this->struct_name = stc;
    }
    // v_type(const v_type& vt) {
    //     this->type = vt.type;
    //     this->struct_name = vt.struct_name;
    //     this->array_dim = vt.array_dim;
    // }
};

class Variable
{
public:
    string name;
    v_type* t;
    Variable(string name){
        this->t = new v_type(TYPE_UNKNOW);
        this->name = name;
    }
    ~Variable(){}
};


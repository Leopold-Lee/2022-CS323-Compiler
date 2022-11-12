#pragma once
#include "node.hpp"
#include "variable.hpp"
#include <string>
#include <vector>

using namespace std;
class my_struct
{
public:
    string name;
    vector<Variable* > variables;
    my_struct(string name) {
        this->name = name;
    }
};
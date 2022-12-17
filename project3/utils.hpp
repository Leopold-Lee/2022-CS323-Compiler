#include "variable.hpp"
#include "struct.hpp"
#include <unordered_map>
#include <vector>

extern std::unordered_map<std::string, my_struct*> struct_map;

enum Operation {START, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_ASSIGN, 
OP_LT, OP_LE, OP_GT, OP_GE, OP_NE, OP_EQ, OP_FUN, OP_PARAM, GOTO, 
LABLE, OP_RETURN, READ, WRITE, CALL, ARG, PARA, DEC, ADDR, ADDR_A, STAR};

class IR {
public:
    string target;
    Operation op;
    string arg1;
    string arg2;
    IR *next;
    IR *pre;
    IR(string target, Operation op, string arg1, string arg2) {
        this->target = target;
        this->op = op;
        this->arg1 = arg1;
        this->arg2 = arg2;
        this->pre = NULL;
        this->next = NULL;
        if (this->arg1.length() > 0 && this->arg1[0] >= '0' && this->arg1[0] <= '9')
        {
            this->arg1 = "#" + this->arg1;
        }
        if (this->arg2.length() > 0 && this->arg2[0] >= '0' && this->arg2[0] <= '9')
        {
            this->arg2 = "#" + this->arg2;
        }
    }
    void append(IR *ir) {
        this->next = ir;
        ir->pre = this;
    }
};


string transform(IR *code) {
    string result = "";
    while (code)
    {
        switch (code->op)
        {
            case OP_ADD: result += (code->target + " := " + code->arg1 + " + " + code->arg2 + "\n"); break;
            case OP_SUB: result += (code->target + " := " + code->arg1 + " - " + code->arg2 + "\n"); break;
            case OP_MUL: result += (code->target + " := " + code->arg1 + " * " + code->arg2 + "\n"); break;
            case OP_DIV: result += (code->target + " := " + code->arg1 + " / " + code->arg2 + "\n"); break;
            case OP_ASSIGN: result += (code->target + " := " + code->arg1 + "\n"); break;
            case LABLE: result += "LABEL " + code->target + " :\n"; break;
            case GOTO: result += "GOTO " + code->target + "\n";break;
            case OP_EQ: result += "IF " + code->arg1 + " == " + code->arg2 + " GOTO " + code->target + "\n"; break;
            case OP_NE: result += "IF " + code->arg1 + " != " + code->arg2 + " GOTO " + code->target + "\n"; break;
            case OP_LT: result += "IF " + code->arg1 + " < " + code->arg2 + " GOTO " + code->target + "\n"; break;
            case OP_LE: result += "IF " + code->arg1 + " <= " + code->arg2 + " GOTO " + code->target + "\n"; break;
            case OP_GT: result += "IF " + code->arg1 + " > " + code->arg2 + " GOTO " + code->target + "\n"; break;
            case OP_GE: result += "IF " + code->arg1 + " >= " + code->arg2 + " GOTO " + code->target + "\n"; break;
            case OP_RETURN: result += "RETURN " + code->target + "\n"; break;
            case READ: result += "READ " + code->target + "\n"; break;
            case WRITE: result += "WRITE " + code->target + "\n"; break;
            case OP_FUN: result += "FUNCTION " + code->target + " :\n"; break;
            case ARG: result += "ARG " + code->target + "\n"; break;
            case CALL: result += code->target + " := CALL " + code->arg1 + "\n"; break;
            case PARA: result += "PARAM " + code->target + "\n"; break;
            case DEC: result += "DEC " + code->arg1 + " " + code->target + "\n"; break;
            case ADDR: result += code->target + " := &" + code->arg1 + " + " + code->arg2 + "\n"; break;
            case ADDR_A: result += "*" + code->target + " := " + code->arg1+ "\n"; break;
            case STAR: result += code->target + " := *" + code->arg1 + "\n"; break;
            default: break;
        }
        code = code->next;
    }
    return result;
}


int place_count = 0;

int lable_count = 0;

string new_place(){
    place_count+= 1;
    return "t" + to_string(place_count);
}

string new_lable(){
    lable_count+= 1;
    return "L" + to_string(lable_count);
}

IR* combine_codes(IR* code1, IR* code2, IR* code3) {
    if (code1 && code2 && code3)
    {
        /* code */
        IR* temp = code1;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->append(code2);
        temp = code2;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->append(code3);
        return code1;
    }
    else {
        cout << "null code" << endl;
        return NULL;
    }
    
}

IR* combine_codes(IR* code1, IR* code2) {
    if (code1 && code2)
    {
        IR* temp = code1;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->append(code2);
        return code1;
    } else {
        cout << "null code" << endl;
        return NULL;
    }
}

void show_code(IR* code) {
    cout << transform(code);
}

void debug(int line) {
    cout << "line" << line << endl;
}

int calculate_type_size(v_type* type) {
    int base_size = 0;
    int num = 1;
    if (type->type != TYPE_STRUCT)
    {
        base_size = 4; //INT
    } else {
        my_struct* st = struct_map[type->struct_name];
        for(Variable* v : st->variables) {
            base_size += calculate_type_size(v->t);
        } 
    }
    if (type->array_dim >= 1)
    {
        vector<int> array_rec = type->origin_rec;
        for(int i : array_rec) {
            num *= i;
        }
    }
    return base_size * num;     
}

int calculate_single_size(v_type* type) {
    int base_size = 0;
    if (type->type != TYPE_STRUCT)
    {
        base_size = 4; //INT
    } else {
        my_struct* st = struct_map[type->struct_name];
        for(Variable* v : st->variables) {
            base_size += calculate_type_size(v->t);
        } 
    }
    return base_size;     
}

void transform_array() {
    for (auto var = variable_map->begin(); var != variable_map->end(); ++var)
    {
        Variable* variable = var->second;
        if (variable->t->array_dim >= 1)
        {
            vector<int> array_rec;
            v_type *t = variable->t;
            int size = 1;
            for (size_t i = 0; i < t->array_rec.size(); i++)
            {
                array_rec.push_back(size);
                size = size * t->array_rec[i];
            }
            variable->t->array_rec = array_rec;
        }
    } 
}
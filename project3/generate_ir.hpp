#include "node.hpp"
#include "string.h"

enum Operation {START, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_ASSIGN, OP_LT, OP_LE, OP_GT, OP_GE, OP_NE, OP_EQ, OP_FUN, OP_PARAM, GOTO, LABLE, OP_RETURN, READ, WRITE, CALL, ARG, PARA};

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
            default: break;
        }
        code = code->next;
    }
    return result;
}

void show_code(IR* code) {
    cout << transform(code);
}

void debug(int line) {
    cout << "line" << line << endl;
}

int place_count = 0;
int lable_count = 0;

IR* translate_args(Node* args, vector<string> &args_list);

void translate_compst(Node* node, IR* code);

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

IR* translate_exp(Node* exp, string place) {
    int size = exp->sub_nodes.size();
    if (size == 1)
    {
        if (exp->sub_nodes[0]->name.compare("INT") == 0)
        {
            string int_value = exp->sub_nodes[0]->value;
            return new IR(place, OP_ASSIGN, int_value, "");
        } else if(exp->sub_nodes[0]->name.compare("ID") == 0) {
            string int_value = exp->sub_nodes[0]->value;
            return new IR(place, OP_ASSIGN, int_value, "");
        }
    } else if(size == 2) { //MINUS Exp
        string tp = new_place();
        IR* code1 = translate_exp(exp->sub_nodes[1], tp);
        return combine_codes(code1, new IR(place, OP_SUB, "0", tp));
    }
    else if(size == 3) {
        Node* exp1 = exp->sub_nodes[0];
        Node* op = exp->sub_nodes[1];
        Node* exp2 = exp->sub_nodes[2];
        if (op->name.compare("PLUS")==0 || op->name.compare("MINUS")==0 || op->name.compare("MUL")==0 || op->name.compare("DIV")==0)
        {
            Operation opron;
            if (op->name.compare("PLUS")==0) opron = OP_ADD;
            else if (op->name.compare("MINUS")==0) opron = OP_SUB;
            else if (op->name.compare("MUL")==0) opron = OP_MUL;
            else if (op->name.compare("DIV")==0) opron = OP_DIV;
            
            string t1 = new_place();
            string t2 = new_place();
            IR* code1 = translate_exp(exp1, t1);
            IR* code2 = translate_exp(exp2, t2);
            IR* code3 = new IR(place, opron, t1, t2);
            return combine_codes(code1, code2, code3);
        } else if(op->name.compare("ASSIGN") == 0) {
            string variable = exp1->sub_nodes[0]->value; //ID
            string tp = new_place();
            IR* code1 = translate_exp(exp2, tp);
            IR* code2 = new IR(variable, OP_ASSIGN, tp, "");
            return combine_codes(code1, code2);
        } else if(op->name.compare("Exp") == 0) { //LP Exp RP
            IR* code = translate_exp(op, place);
            return code;
        }
        //functions
        else if(exp1->name.compare("ID") == 0 ){
            if (exp1->value.compare("read") == 0)
            {
                return new IR(place, READ, "", "");
            }
            else {
                return new IR(place, CALL, exp1->value, "");
            }    
        } 
    }
    else if(size == 4) {
        if (exp->sub_nodes[0]->value.compare("write") == 0)//write LP Args RP
        {
            string tp = new_place();
            if (exp->sub_nodes[2]->sub_nodes.size() == 1)
            {
                IR* code = translate_exp(exp->sub_nodes[2]->sub_nodes[0], tp);
                return combine_codes(code, new IR(tp, WRITE, "", ""));
            }
            else {
                cout << "wrong use of wirte" << endl;
                return NULL;
            }
        } else {//ID LP Args RP
            vector<string> args_list;
            IR* code1 = translate_args(exp->sub_nodes[2], args_list);
            IR* code2 = new IR(args_list[args_list.size() - 1], ARG, "", "");
            for (size_t i = 2; i <= args_list.size(); i++)
            {
                code2->append(new IR(args_list[args_list.size() - i], ARG, "", ""));
            }
            IR* call = new IR(place, CALL, exp->sub_nodes[0]->value, "");
            return combine_codes(code1, code2, call);
        }
    }
    return NULL;
}

IR* translate_args(Node* args, vector<string> &args_list) {
    if (args->sub_nodes.size() == 1) //Exp
    {
        string tp = new_place();
        IR* code = translate_exp(args->sub_nodes[0], tp);
        args_list.push_back(tp);
        return code;
    } else { // Exp comma args
        string tp = new_place();
        IR* code1 = translate_exp(args->sub_nodes[0], tp);
        args_list.push_back(tp);
        IR* code2 = translate_args(args->sub_nodes[2], args_list); 
        return combine_codes(code1, code2);
    }
}

IR* translate_cond_exp(Node* exp, string lbt, string lbf) {
    if (exp->sub_nodes.size() == 3)
    {
        Node* exp1 = exp->sub_nodes[0];
        Node* op = exp->sub_nodes[1];
        Node* exp2 = exp->sub_nodes[2];
        if (op->name.compare("EQ") == 0 || op->name.compare("NE") == 0 || op->name.compare("LT") == 0 || op->name.compare("LE") == 0 || op->name.compare("GT") == 0 || op->name.compare("GE") == 0)
        {
            // OP_LT, OP_LE, OP_GT, OP_GE, OP_NE, OP_EQ
            Operation opron;
            if (op->name.compare("EQ")==0) opron = OP_EQ;
            else if (op->name.compare("NE")==0) opron = OP_NE;
            else if (op->name.compare("LT")==0) opron = OP_LT;
            else if (op->name.compare("LE")==0) opron = OP_LE;
            else if (op->name.compare("GT")==0) opron = OP_GT;
            else if (op->name.compare("GE")==0) opron = OP_GE;
            string t1 = new_place();
            string t2 = new_place();
            IR* code1 = translate_exp(exp1, t1);
            IR* code2 = translate_exp(exp2, t2);
            IR* code3 = new IR(lbt, opron, t1, t2);
            code3->append(new IR(lbf, GOTO, "", ""));
            return combine_codes(code1, code2, code3);
        }
        else if (op->name.compare("AND") == 0) {
            string lb1 = new_lable();
            IR* code1 = translate_cond_exp(exp1, lb1, lbf);
            IR* code2 = translate_cond_exp(exp2, lbt, lbf);
            return combine_codes(code1, new IR(lb1, LABLE, "", ""), code2);
        } else if (op->name.compare("OR") == 0) {
            string lb1 = new_lable();
            IR* code1 = translate_cond_exp(exp1, lb1, lbf);
            IR* code2 = translate_cond_exp(exp2, lbt, lbf);
            return combine_codes(code1, new IR(lb1, LABLE, "", ""), code2);
        } else if(op->name.compare("Exp") == 0) { //LP Exp RP
            IR* code = translate_cond_exp(op, lbt, lbf);
            return code;
        }
    } else if (exp->sub_nodes.size() == 2 && exp->sub_nodes[0]->name.compare("NOT") == 0) {
        return translate_cond_exp(exp->sub_nodes[1], lbf, lbt);
    }
    return NULL;
}

IR* translate_stmt(Node* stmt) {
    int size = stmt->sub_nodes.size();
    if(size == 1) {
        IR* code = new IR("", START, "", "");
        translate_compst(stmt->sub_nodes[0], code);
        return code;
    }
    if(size == 2) {//Exp SEMI
        string tp = new_place();
        IR* code = translate_exp(stmt->sub_nodes[0], tp);
        return code;
    }
    else if(size == 3) {//RETURN Exp SEMI
        string tp = new_place();
        IR* code = translate_exp(stmt->sub_nodes[1], tp);
        return combine_codes(code, new IR(tp, OP_RETURN, "", ""));
    }
    else if(size == 5 && stmt->sub_nodes[0]->name.compare("WHILE") == 0) {//WHILE LP Exp RP Stmt
        string lb1 = new_lable();
        string lb2 = new_lable();
        string lb3 = new_lable();
        IR* code1 = new IR(lb1, LABLE, "", "");
        code1->append(translate_cond_exp(stmt->sub_nodes[2], lb2, lb3));
        IR* code2 = new IR(lb2, LABLE, "", "");
        code2->next = translate_stmt(stmt->sub_nodes[4]);
        combine_codes(code2, new IR(lb1, GOTO, "", ""));
        return combine_codes(code1, code2, new IR(lb3, LABLE, "", ""));
    }
    else if(size == 5 && stmt->sub_nodes[0]->name.compare("IF") == 0) { //IF LP Exp RP Stmt
        string lb1 = new_lable();
        string lb2 = new_lable();
        IR* code1 = translate_cond_exp(stmt->sub_nodes[2], lb1, lb2);
        combine_codes(code1, new IR(lb1, LABLE, "", ""));
        IR* code2 = translate_stmt(stmt->sub_nodes[4]);
        combine_codes(code2, new IR(lb2, LABLE, "", ""));
        return combine_codes(code1, code2);
    }
    else if(size == 7) {//IF LP Exp RP Stmt ELSE Stmt
        string lb1 = new_lable();
        string lb2 = new_lable();
        string lb3 = new_lable();
        IR* code1 = translate_cond_exp(stmt->sub_nodes[2], lb1, lb2);
        combine_codes(code1, new IR(lb1, LABLE, "", ""));
        IR* code2 = translate_stmt(stmt->sub_nodes[4]);
        IR* temp = new IR(lb3, GOTO, "", "");
        temp->next = new IR(lb2, LABLE, "", "");
        combine_codes(code2, temp);
        IR* code3 = translate_stmt(stmt->sub_nodes[6]);
        combine_codes(code3, new IR(lb3, LABLE, "", ""));
        return combine_codes(code1, code2, code3);
    }
    return NULL;
}

IR* translate_dec(Node* dec) {
    if (dec->sub_nodes.size() == 3)//VarDec ASSIGN Exp
    {
        string varialbe = dec->sub_nodes[0]->sub_nodes[0]->value; //ID                
        string tp = new_place();
        IR* code1 = translate_exp(dec->sub_nodes[2], tp); //exp
        IR* code2 = new IR(varialbe, OP_ASSIGN, tp, "");
        return combine_codes(code1, code2);
    }
    return NULL;
}

void translate_compst(Node* node, IR* code) {
    if (node->name.compare("Dec") == 0)
    {
        IR* translated = translate_dec(node);
        if (translated)
        {
            combine_codes(code, translated);
        } else {
            // cout << "null pointer" << endl;
        }
    } else if(node->name.compare("Stmt") == 0) {
        IR* translated = translate_stmt(node);
        if (translated)
        {
            combine_codes(code, translated);
        } else {
            cout << "null pointer" << endl;
        }
    }
    else {
        for (size_t i = 0; i < node->sub_nodes.size(); i++)
        {
            translate_compst(node->sub_nodes[i], code);
        }
    }
}

void translate_parms(Node* node, IR* code) {
    if (node->name.compare("VarDec") == 0)
    {
        if (node->sub_nodes.size() == 1) //ID
        {
            IR* para = new IR(node->sub_nodes[0]->value, PARA, "", "");
            combine_codes(code, para);
        } else { //VarDec LB INT RB

        }
    }
    else {
        for (size_t i = 0; i < node->sub_nodes.size(); i++)
        {
            translate_parms(node->sub_nodes[i], code);
        }
    }
}

void translate_fundec(Node *node, IR* code) {
    if (node->name.compare("FunDec") == 0)
    {
        if (node->sub_nodes.size() == 4) //ID LP VarList RP
        {
            IR *def_fun = new IR(node->sub_nodes[0]->value, OP_FUN, "", "");
            combine_codes(code, def_fun);
            translate_parms(node, code);
        }
        else { //ID LP RP
            IR *def_fun = new IR(node->sub_nodes[0]->value, OP_FUN, "", "");
            combine_codes(code, def_fun);
        }
    }
    else {
        for (size_t i = 0; i < node->sub_nodes.size(); i++)
        {
            translate_fundec(node->sub_nodes[i], code);
        }
    }
}

void translate_extdef(Node *node, IR* code) {
    if (node->name.compare("ExtDef") == 0)
    {
        if (node->sub_nodes[1]->name.compare("FunDec") == 0)
        {
            translate_fundec(node->sub_nodes[1], code);
            translate_compst(node->sub_nodes[2], code);
        }
    }
    else {
        for (size_t i = 0; i < node->sub_nodes.size(); i++)
        {
            translate_extdef(node->sub_nodes[i], code);
        }
    }
}

void main_translate(Node* root) {
    IR* code = new IR("", START, "", "");
    translate_extdef(root, code);
    show_code(code);
}
#include "node.hpp"
#include "string.h"

enum Operation {START, OP_ADD, OP_SUB, OP_ASSIGN, OP_EQ, GOTO, LABLE, OP_RETURN, READ, WRITE, CALL, ARG};

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
    }

    void append(IR *ir) {
        this->next = ir;
        ir->pre = this;
    }
};

void show_code(IR* code) {
    while (code)
    {
        cout << code->target << " " << code->op << " " << code->arg1 << " " << code->arg2 << endl;
        code = code->next;
    }
}

void debug(int line) {
    cout << "line" << line << endl;
}

int place_count = 0;
int lable_count = 0;

IR* translate_args(Node* args, vector<string> args_list);

void translate(Node* node, IR* code);


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
    }
    else if(size == 3) {
        Node* exp1 = exp->sub_nodes[0];
        Node* op = exp->sub_nodes[1];
        Node* exp2 = exp->sub_nodes[2];
        if (op->name.compare("PLUS")==0)
        {
            string t1 = new_place();
            string t2 = new_place();
            IR* code1 = translate_exp(exp1, t1);
            IR* code2 = translate_exp(exp2, t2);
            IR* code3 = new IR(place, OP_ADD, t1, t2);
            return combine_codes(code1, code2, code3);
        }
        else if(op->name.compare("ASSIGN") == 0) {
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
                return new IR(exp->value, CALL, "", "");
            }    
        } 
    }
    else if(size == 4) {
        if (exp->sub_nodes[0]->value.compare("write") == 0)//write LP Exp RP
        {
            string tp = new_place();
            IR* code = translate_exp(exp->sub_nodes[2], tp);
            code->append(new IR(tp, WRITE, "", ""));
            return code;
        } else {//ID LP Args RP
            vector<string> args_list;
            IR* code1 = translate_args(exp->sub_nodes[2], args_list);
            IR* code2 = new IR(args_list[0], ARG, "", "");
            for (size_t i = 1; i < args_list.size(); i++)
            {
                code2->append(new IR(args_list[i], ARG, "", ""));
            }
            IR* call = new IR(exp->sub_nodes[0]->value, CALL, "", "");
            return combine_codes(code1, code2, call);
        }
    }
    return NULL;
}

IR* translate_args(Node* args, vector<string> args_list) {
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
        if (op->name.compare("EQ") == 0)
        {
            string t1 = new_place();
            string t2 = new_place();
            IR* code1 = translate_exp(exp1, t1);
            IR* code2 = translate_exp(exp2, t2);
            IR* code3 = new IR(lbt, OP_EQ, t1, t2);
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
        translate(stmt->sub_nodes[0], code);
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
        code->append(new IR(tp, OP_RETURN, "", ""));
        return code;
    }
    else if(size == 5) {//WHILE LP Exp RP Stmt
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
    else if(size == 6) { //IF LP Exp RP Stmt LOWER_ELSE
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

void translate(Node* node, IR* code) {
    if (node->name.compare("Dec") == 0)
    {
        IR* translated = translate_dec(node);
        if (translated)
        {
            combine_codes(code, translated);
        } else {
            cout << "null pointer" << endl;
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
            translate(node->sub_nodes[i], code);
        }
    }
    
}

void main_translate(Node* root) {
    IR* code = new IR("", START, "", "");
    translate(root, code);
    show_code(code);
}
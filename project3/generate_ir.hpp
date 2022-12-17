#include "node.hpp"
#include "string.h"
#include "struct.hpp"
#include "utils.hpp"
#include <unordered_map>

extern std::unordered_map<std::string, Variable *> *variable_map;
extern std::unordered_map<std::string, my_struct*> struct_map;

IR* translate_args(Node* args, vector<string> &args_list);

void translate_compst(Node* node, IR* code);

IR* translate_exp(Node* exp, string place, bool get_content = true);

IR* translate_array(Node* exp, string place, int layer) {
    if (exp->sub_nodes[0]->sub_nodes.size() == 4)
    {
        string tp = new_place();
        IR* sub_code = translate_array(exp->sub_nodes[0], tp, layer + 1);
        string arr = exp->sub_nodes[0]->value;
        Variable* variable = variable_map->at(arr);
        int width = variable->t->array_rec[layer];
        string tp1 = new_place();
        IR* code = translate_exp(exp->sub_nodes[2], tp1);
        string tp2 = new_place();
        IR* now = new IR(tp2, OP_MUL, tp1, to_string(width * calculate_single_size(variable->t)));
        IR* combine = new IR(place, OP_ADD, tp, tp2);
        now->append(combine);
        return combine_codes(sub_code, code, now);
    }
    else {//Exp LB Exp RB
        string arr = exp->sub_nodes[0]->value;
        Variable* variable = variable_map->at(arr);
        int width = variable->t->array_rec[layer];
        string tp = new_place();
        IR* code = translate_exp(exp->sub_nodes[2], tp);
        string tp1 = new_place();
        IR* code1 = new IR(tp1, OP_MUL, tp, to_string(width * calculate_single_size(variable->t)));
        string tp2 = new_place();
        IR *code2 = translate_exp(exp->sub_nodes[0], tp2, false);
        IR* code3 = new IR(place, OP_ADD, tp1, tp2);
        combine_codes(code2, code3);
        return combine_codes(code, code1, code2);
    }
    return NULL;
}

IR* translate_exp(Node* exp, string place, bool get_content) {
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
            string variable;
            if (exp1->sub_nodes.size() == 1) { //ID
                variable = exp1->sub_nodes[0]->value;
                string tp = new_place();
                IR* code1 = translate_exp(exp2, tp);
                IR* code2 = new IR(variable, OP_ASSIGN, tp, "");
                return combine_codes(code1, code2);
            } 
            else if (exp1->sub_nodes.size() == 3) { //Exp DOT ID
                string tp = new_place();
                string member = exp1->sub_nodes[2]->value;
                IR* code = translate_exp(exp1->sub_nodes[0], tp, false);
                string st_name = exp1->sub_nodes[0]->value; 
                Variable* v = variable_map->at(st_name);
                my_struct* st = struct_map[v->t->struct_name];
                int bias = 0;
                for(Variable* v : st->variables) {
                    if (v->name.compare(member) == 0) break;
                    else bias += calculate_type_size(v->t);
                }
                string tp_addr = new_place();
                IR* addr = new IR(tp_addr, OP_ADD, tp, to_string(bias));
                string tp1 = new_place();
                IR* code1 = translate_exp(exp2, tp1);
                IR* code2 = new IR(tp_addr, ADDR_A, tp1, "");
                combine_codes(code1, code2);
                return combine_codes(code, addr, code1);
            }
            else { //Exp LB Exp RB
                string tp_addr = new_place();
                IR* code = translate_array(exp1, tp_addr, 0);
                string tp1 = new_place();
                IR* code1 = translate_exp(exp2, tp1);
                IR* code2 = new IR(tp_addr, ADDR_A, tp1, "");
                combine_codes(code1, code2);
                return combine_codes(code, code1);
            }
        } else if(op->name.compare("Exp") == 0) { //LP Exp RP
            IR* code = translate_exp(op, place);
            return code;
        }
        //functions
        else if(op->name.compare("LP") == 0 ){
            if (exp1->value.compare("read") == 0)
            {
                return new IR(place, READ, "", "");
            }
            else {
                return new IR(place, CALL, exp1->value, "");
            }    
        } 
        else if(op->name.compare("DOT") == 0) { //Exp DOT ID
            string tp = new_place();
            string member = exp2->value;
            IR* code = translate_exp(exp1, tp, false);

            Variable* v = variable_map->at(exp1->value);
            my_struct* st = struct_map[v->t->struct_name];
            int bias = 0;
            for(Variable* v : st->variables) {
                if (v->name.compare(member) == 0) break;
                else bias += calculate_type_size(v->t);
            }
            string tp_addr = new_place();
            if(get_content) {
                IR* addr = new IR(tp_addr, OP_ADD, tp, to_string(bias));
                string tp1 = new_place();
                IR* code1 = new IR(place, STAR, tp_addr, "");
                return combine_codes(code, addr, code1);
            } else {
                IR* addr = new IR(place, OP_ADD, tp, to_string(bias));
                IR* temp = combine_codes(code, addr);
                return temp;
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
        } else if (exp->sub_nodes[0]->name.compare("ID") == 0){//ID LP Args RP
            vector<string> args_list;
            IR* code1 = translate_args(exp->sub_nodes[2], args_list);
            IR* code2 = new IR(args_list[args_list.size() - 1], ARG, "", "");
            for (size_t i = 2; i <= args_list.size(); i++)
            {
                combine_codes(code2, new IR(args_list[args_list.size() - i], ARG, "", ""));
            }
            IR* call = new IR(place, CALL, exp->sub_nodes[0]->value, "");
            return combine_codes(code1, code2, call);
        } else if (exp->sub_nodes[0]->name.compare("Exp") == 0) { //Exp LB Exp RB
            string tp_addr = new_place();
            IR* code = translate_array(exp, tp_addr, 0);
            if(get_content) {
                IR* code1 = new IR(place, STAR, tp_addr, ""); //!!!!!!weather take this action
                return combine_codes(code, code1);
            } else return combine_codes(code, new IR(place, OP_ASSIGN, tp_addr, ""));
        }
    }
    return NULL;
}

IR* translate_args(Node* args, vector<string> &args_list) {
    if (args->sub_nodes.size() == 1) //Exp
    {
        string tp = new_place();
        IR* code;
        if (variable_map->count(args->sub_nodes[0]->value))
        {
            Variable* v = variable_map->at(args->sub_nodes[0]->value);
            if (v->t->type != TYPE_STRUCT) code = translate_exp(args->sub_nodes[0], tp);
            else code = translate_exp(args->sub_nodes[0], tp, false);
        }
        else code = translate_exp(args->sub_nodes[0], tp);
        args_list.push_back(tp);
        return code;
    } else { // Exp comma args
        string tp = new_place();
        IR* code;
        if (variable_map->count(args->sub_nodes[0]->value))
        {
            Variable* v = variable_map->at(args->sub_nodes[0]->value);
            if (v->t->type != TYPE_STRUCT) code = translate_exp(args->sub_nodes[0], tp);
            else code = translate_exp(args->sub_nodes[0], tp, false);
        }
        else code = translate_exp(args->sub_nodes[0], tp);       
        args_list.push_back(tp);
        IR* code2 = translate_args(args->sub_nodes[2], args_list); 
        return combine_codes(code, code2);
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
    } else { //VarDec
        Node* var_dec = dec->sub_nodes[0];
        Variable *v = variable_map->at(var_dec->value);
        int v_size = calculate_type_size(v->t);
        if (var_dec->sub_nodes.size() == 4)
        {
            string tp = new_place();
            return combine_codes(new IR(to_string(v_size), DEC, tp, ""), new IR(var_dec->value, ADDR, tp, "0"));
        } else if(v->t->type == TYPE_STRUCT) {
            string tp = new_place();
            return combine_codes(new IR(to_string(v_size), DEC, tp, ""), new IR(var_dec->value, ADDR, tp, "0"));
        }
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
        IR* para = new IR(node->sub_nodes[0]->value, PARA, "", "");
        combine_codes(code, para);
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
    transform_array();
    IR* code = new IR("", START, "", "");
    translate_extdef(root, code);
    show_code(code);
}
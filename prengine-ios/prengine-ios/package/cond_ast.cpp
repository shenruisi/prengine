//
//  cond_ast.cpp
//  PoolRouter
//
//  Created by ris on 1/12/17.
//  Copyright © 2017 ris. All rights reserved.
//
#include "cond_ast.h"

int get_operator_priority(string s){
    if (s == "*" || s == "/" || s == "%")
        return 3;
    else if (s == "+" || s == "-")
        return 4;
    else if (s == ">" || s == "<" || s == ">=" || s == "<=")
        return 6;
    else if (s == "==" || s == "!=")
        return 7;
    else if (s == "&")
        return 8;
    else if (s == "^")
        return 9;
    else if (s == "|")
        return 10;
    else if (s == "&&")
        return 11;
    else if (s == "||")
        return 12;
    else if (s == "=")
        return 14;

    return PRIORITY_UNKNOW;
}

typedef enum {
    EXP_STAT_SCAN,
    EXP_STAT_CLT_NUMBER,
    EXP_STAT_CLT_STRING,
    EXP_STAT_CLT_OPERATOR
}EXP_STAT;

#define EXP_CAP_STR() strcaper.push_back(c);
#define EXP_CAP_NUM() \
if (numcaper == __DBL_MAX__) numcaper = 0;\
if (integerbit == 1){\
    numcaper = numcaper * 10 + c - '0';\
}\
else{\
    numcaper += (c - '0') * integerbit;\
}
#define EXP_CAP_OP() opcaper.push_back(c);

#define EXP_CLEAN_CAP_STR() strcaper.clear();
#define EXP_CLEAN_CAP_NUM() integerbit = 1,numcaper = __DBL_MAX__;
#define EXP_CLEAN_CAP_OP()  opcaper.clear();
#define EXP_MOVE_STAT(__new_stat__) do{ stat = __new_stat__; EXP_CLEAN_CAP_STR(); EXP_CLEAN_CAP_NUM(); EXP_CLEAN_CAP_OP(); }while(0)
#define EXP_APPEND_NODE(node)\
do{\
    if (NULL == curroot->val){\
        curroot->left = node;\
    }\
    else{\
        curroot->right = node;\
    }\
    node->parent = curroot;\
}while(0);

#define EXP_RESCAN_C() i--;

static inline cond_node* _get_top_root(cond_node *curroot){
    cond_node *toproot = curroot;
    while(toproot->parent){
        toproot = toproot->parent;
    }

    return toproot;
}

cond_node* _inner_cond_ast_creat(string exp,int *index){
    cond_node *curroot = new cond_node();
    EXP_STAT stat = EXP_STAT_SCAN;
    string strcaper = string();
    string opcaper = string();
    float integerbit = 1;
    double numcaper = __DBL_MAX__;
    for (int i = *index; i < exp.size(); i++){
        char c = exp[i];

        switch (stat) {
            case EXP_STAT_SCAN:{
                if (c == ' ') continue;

                if (c >= '0' && c <= '9'){
                    EXP_MOVE_STAT(EXP_STAT_CLT_NUMBER);
                    EXP_RESCAN_C();
                }
                else if (c == '.'){
                    EXP_MOVE_STAT(EXP_STAT_CLT_NUMBER);
                    EXP_RESCAN_C();
                }
                else if (c == '"'){
                    EXP_MOVE_STAT(EXP_STAT_CLT_STRING);
                }
                else if (c == '('){
                    *index = i+1;
                    cond_node *subroot = _inner_cond_ast_creat(exp,index);
                    subroot->isbracketoprator = true;
                    i = *index;
                    if (NULL == curroot->val){
                        curroot = subroot;
                    }
                    else if (NULL == curroot->right){
                        curroot->right = subroot;
                        subroot->parent = curroot;
                    }
                }
                else if (c == ')'){
                    *index = i;
                    return _get_top_root(curroot);
                }
                else{
                    EXP_MOVE_STAT(EXP_STAT_CLT_OPERATOR);
                    EXP_CAP_OP();
                }
            }
                break;
            case EXP_STAT_CLT_NUMBER:{
                if (c >= '0' && c <= '9'){
                    if (integerbit < 0) integerbit/=10;
                    EXP_CAP_NUM();
                }
                else if (c == '.'){
                    integerbit /= 10;
                }
                else {
                    pr_id *id;
                    if (integerbit == 1){
                        id = new pr_id((long long)numcaper);
                    }
                    else {
                        id = new pr_id(numcaper);
                    }

                    cond_node *node = new cond_node();
                    node->val = id;
                    EXP_APPEND_NODE(node);
                    EXP_MOVE_STAT(EXP_STAT_SCAN);
                    EXP_RESCAN_C();
                }
            }
                break;
            case EXP_STAT_CLT_STRING:{
                if (c == '"'){
                    cond_node *node = new cond_node();
                    node->val = new pr_id(string(strcaper));
                    EXP_APPEND_NODE(node);
                    EXP_MOVE_STAT(EXP_STAT_SCAN);
                }
                else  EXP_CAP_STR();
            }
                break;
            case EXP_STAT_CLT_OPERATOR:{
                if (c == ' ') continue;

                if (PRIORITY_UNKNOW == get_operator_priority(string(1,c))){
                    pr_id *id = new pr_id(opcaper);
                    if(NULL == curroot->val){
                        curroot->val = id;
                    }
                    else{
                        int topp = get_operator_priority(_get_top_root(curroot)->val->stringValue());
                        int curp = get_operator_priority(id->stringValue());
                        cond_node *newroot = new cond_node();
                        if (curp >= topp || curroot->isbracketoprator){ //当前操作符优先级小于最顶层操作符优先级
                            newroot->val = id;
                            newroot->left = _get_top_root(curroot);
                            curroot->parent = newroot;
                        }
                        else{
                            cond_node *right = curroot->right;
                            if (NULL == right){ //如果当前符号节点下右子节点为空，说明为单目左结合运算符(例如:-1)
                                newroot->val = id;
                                curroot->right = newroot;
                                newroot->parent = curroot;
                            }
                            else{
                                newroot->val = id;
                                newroot->left = right;
                                right->parent = newroot;
                                curroot->right = newroot;
                                newroot->parent = curroot;
                            }
                        }
                        curroot = newroot;

                    }
                    EXP_MOVE_STAT(EXP_STAT_SCAN);
                    EXP_RESCAN_C();
                }
                else EXP_CAP_OP();
            }
                break;
            default:
                break;
        }
    }

    if (numcaper != __DBL_MAX__){
        pr_id *id;
        if (integerbit == 1){
            id = new pr_id((long long)numcaper);
        }
        else {
            id = new pr_id(numcaper);
        }
        cond_node *node = new cond_node();
        node->val = id;
        EXP_APPEND_NODE(node);
    }

    return _get_top_root(curroot);
}

cond_node* cond_ast_creat(string exp){
    int index = 0;
    return _inner_cond_ast_creat(exp,&index);
}


pr_id _post_order_traverse(cond_node *op){
    if (op == NULL) return pr_undefined();
    pr_id left = _post_order_traverse(op->left);
    pr_id right = _post_order_traverse(op->right);
    if (left == pr_undefined() && right == pr_undefined()) {
        return *op->val;
    }
    else{
        if (left == pr_undefined()){
            if ("-" == op->val->stringValue()){
                switch (right.type) {
                    case PR_ID_LONGLONG:{
                        return pr_id(-right.longLongValue());
                    }
                    case PR_ID_DOUBLE:{
                        return pr_id(-right.doubleValue());
                    }
                    default:
                        break;
                }
            }
            else{
                return right;
            }
        }
        else if (right == pr_undefined() && NULL == op->val){ //单一节点情况,进行布尔转化
            if (PR_ID_LONGLONG == left.type){
                if (left.longLongValue() > 0) return pr_yes();
                else return pr_no();
            }
            else if (PR_ID_DOUBLE == left.type){
                if (left.doubleValue() > 0) return pr_yes();
                else return pr_no();
            }
            else if (PR_ID_STRING == left.type){
                return left.stringValue().empty()? pr_no():pr_yes();
            }
            else return pr_undefined();
        }
        else {
            string operatorstr = op->val->stringValue();
            if ("-" == operatorstr){
                return left - right;
            }
            else if ("+" == operatorstr){
                return left + right;
            }
            else if ("*" == operatorstr){
                return left * right;
            }
            else if ("/" == operatorstr){
                return left / right;
            }
            else if ("%" == operatorstr){
                return left % right;
            }
            else if ("==" == operatorstr){
                if (left == right) return pr_yes();
                else return pr_no();
            }
            else if (">" == operatorstr){
                if (left > right) return pr_yes();
                else return pr_no();
            }
            else if (">=" == operatorstr){
                if (left >= right) return pr_yes();
            }
            else if ("<" == operatorstr){
                if (left < right) return pr_yes();
                else return pr_no();
            }
            else if ("<=" == operatorstr){
                if (left <= right) return pr_yes();
                else return pr_no();
            }
            else if ("!=" == operatorstr){
                if (left != right) return  pr_yes();
                else return pr_no();
            }
            else if ("&&" == operatorstr){
                if (left && right) return pr_yes();
                else return pr_no();
            }
            else if ("||" == operatorstr){
                if (left || right) return pr_yes();
                else return pr_no();
            }
        }
    }

    return pr_undefined();
}

pr_id calc_cond_ast(cond_node *root){
    return _post_order_traverse(root);
}


int  _count_ast_deep(cond_node *root){
    if (root == NULL) return 0;
    int left = 1 + _count_ast_deep(root->left);
    int right = 1 + _count_ast_deep(root->right);
    return left > right ? left : right;
}

void print_cond_ast(queue<cond_node *>rootq){
    queue<cond_node *>nextroot = queue<cond_node *>();
    if (rootq.empty()) return;
    while (!rootq.empty()) {
        cond_node *node = rootq.front();
        node->val->print();
        if (node->left){
            nextroot.push(node->left);
        }
        if (node->right){
            nextroot.push(node->right);
        }
        rootq.pop();
        printf(" ");
    }
    printf("\n");
    print_cond_ast(nextroot);
}

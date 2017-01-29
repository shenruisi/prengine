//
//  cond_ast.hpp
//  PoolRouter
//
//  Created by ris on 1/12/17.
//  Copyright © 2017 ris. All rights reserved.
//

#ifndef cond_ast_hpp
#define cond_ast_hpp

#include <stdio.h>
#include <queue>
#include "prengine_val.h"

using namespace std;

#define PRIORITY_UNKNOW (20)

struct cond_node{
    cond_node *left;
    cond_node *right;
    cond_node *parent;
    pr_id *val;
    bool isbracketoprator;
    
    cond_node(){
        isbracketoprator = false;
        left = NULL;
        right = NULL;
        parent = NULL;
        val = NULL;
    }
};

cond_node* cond_ast_creat(string exp);
pr_id calc_cond_ast(cond_node *root);
void print_cond_ast(queue<cond_node *>rootq);
int get_operator_priority(string s);

#endif /* cond_ast_hpp */

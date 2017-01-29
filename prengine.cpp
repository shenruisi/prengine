//
//  prengine.cpp
//  PoolRouter
//
//  Created by ris on 12/29/16.
//  Copyright © 2016 ris. All rights reserved.
//

#include <stack>
#include <deque>
#include <string>
#include <regex.h>

using namespace std;
#include "prengine_val.h"
#include "cond_ast.h"

typedef enum{
    PR_ENTITY_SCHEME,
    PR_ENTITY_SERVER,
    PR_ENTITY_REWRITE,
    PR_ENTITY_IFBLOCK
}PR_ENTITY_TYPE;

struct pr_entity_t{
    PR_ENTITY_TYPE type;
};

#define pr_safe_delete(__x__) \
do{\
    if (__x__){\
        delete __x__ , __x__ = NULL;\
    }\
}while(0)
struct pr_rewrite_t : pr_entity_t{
    string *regex;
    string *replacement;
    string *outval;
    string *rule;
    string *labeled;
    string *schemen;
    string *servern;
    bool incap;
    pr_rewrite_t(){
        regex = replacement = outval = rule = labeled = schemen = servern = NULL;
        type = PR_ENTITY_REWRITE;
        incap = false;
    }
    
    ~pr_rewrite_t(){
        pr_safe_delete(regex);
        pr_safe_delete(replacement);
        pr_safe_delete(outval);
        pr_safe_delete(rule);
        pr_safe_delete(labeled);
        pr_safe_delete(schemen);
        pr_safe_delete(servern);
    }
    pr_rewrite_t *copy(){
        pr_rewrite_t *cpy = new pr_rewrite_t();
        cpy->regex = regex ? new string(*regex) : NULL;
        cpy->replacement = replacement ? new string(*replacement) : NULL;
        cpy->outval = outval ? new string(*outval) : NULL;
        cpy->rule = rule ? new string(*rule) : NULL;
        cpy->labeled = labeled ? new string(*labeled) : NULL;
        cpy->schemen = schemen ? new string(*schemen) : NULL;
        cpy->servern = servern ? new string(*servern) : NULL;
        cpy->type = PR_ENTITY_REWRITE;
        cpy->incap = false;
        return cpy;
    }
};

struct pr_server_t : pr_entity_t{
    string servern;
    deque<pr_entity_t*> *entityq;
    pr_server_t(){
        entityq = new deque<pr_entity_t*>();
        type = PR_ENTITY_SERVER;
    }
};

typedef enum{
    PR_IF,
    PR_ELIF,
    PR_ELSE
}PR_IF_TYPE;

struct pr_if_block_t : pr_entity_t{
    string cond;
    deque<pr_entity_t*> *entityq;
    PR_IF_TYPE iftype;
    pr_if_block_t(PR_IF_TYPE _iftype){
        iftype = _iftype;
        entityq = new deque<pr_entity_t*>();
        type = PR_ENTITY_IFBLOCK;
    }
};

struct pr_scheme_t : pr_entity_t{
    string schemen;
    deque<pr_entity_t*> *entityq;
    pr_scheme_t(string _schemen){
        schemen = string(_schemen);
        entityq = new deque<pr_entity_t*>();
        type = PR_ENTITY_SCHEME;
    }
};

struct pr_file_t {
    deque<pr_scheme_t*> *schemeq;
    pr_file_t(){
        schemeq = new deque<pr_scheme_t*>();
    }
};

typedef enum{
    PR_STAT_START,
    PR_STAT_SCHEME_OPT,
    PR_STAT_SCHEME,
    PR_STAT_SERVER_OPT,
    PR_STAT_SERVER,
    PR_STAT_SERVER_NAME,
    PR_STAT_REWRITE,
    PR_STAT_REWRITE_REGEX,
    PR_STAT_REWRITE_REPLACEMENT,
    PR_STAT_REWRITE_RULE,
    PR_STAT_REWRITE_LABELED,
    PR_STAT_IF_OPT,
    PR_STAT_IF,
    PR_STAT_IF_COND,
    PR_STAT_IF_BLOCK
}PR_STAT;

typedef enum{
    PR_FLAG_IN_SCHEME,
    PR_FLAG_IN_SERVER,
    PR_FLAG_IN_IF,
}PR_FLAG;

#define STR_NULL_CHECK(__str__) __str__?__str__->c_str():""

void _pr_print_entityq(deque<pr_entity_t*> *entityq,int deep){
    string t = string();
    for(int j = 0; j < deep; j++) t+="\t";
    
    for (int i = 0; i < entityq->size(); i++){
        pr_entity_t* e = entityq->at(i);
        if (e->type == PR_ENTITY_REWRITE){
            pr_rewrite_t *rw = (pr_rewrite_t *)e;
            printf("%srewrite %s %s %s %s\n",
                   t.c_str(),
                   STR_NULL_CHECK(rw->regex),
                   STR_NULL_CHECK(rw->replacement),
                   STR_NULL_CHECK(rw->rule),
                   STR_NULL_CHECK(rw->labeled));
        }
        else if (e->type == PR_ENTITY_IFBLOCK){
            pr_if_block_t *ifb = (pr_if_block_t *)e;
            if (ifb->iftype == PR_IF) {
                printf("%sif ( %s ){\n",t.c_str(),ifb->cond.c_str());
                _pr_print_entityq(ifb->entityq, deep+1);
                printf("%s}\n",t.c_str());
            }
            else if (ifb->iftype == PR_ELIF){
                printf("%selif ( %s ){\n",t.c_str(),ifb->cond.c_str());
                _pr_print_entityq(ifb->entityq, deep+1);
                printf("%s}\n",t.c_str());
            }
            else if (ifb->iftype == PR_ELSE){
                printf("%selse{\n",t.c_str());
                _pr_print_entityq(ifb->entityq, deep+1);
                printf("%s}\n",t.c_str());
            }
        }
        else if (e->type == PR_ENTITY_SERVER){
            pr_server_t *s = (pr_server_t *)e;
            printf("%sserver{\n",t.c_str());
            printf("%s\tserver_name %s\n",t.c_str(),s->servern.c_str());
            _pr_print_entityq(s->entityq, deep+1);
            printf("%s}\n",t.c_str());
        }
    }
}

void _pr_printfile(pr_file_t *f){
    for (int i = 0; i < f->schemeq->size(); i++){
        pr_scheme_t *scheme = f->schemeq->at(i);
        printf("%s{\n",scheme->schemen.c_str());
        _pr_print_entityq(scheme->entityq,1);
        printf("}\n");
        
    }
}

pr_entity_t* _pr_occur_scheme(pr_file_t *f,string cap){
    f->schemeq->push_back(new pr_scheme_t(cap));
    return f->schemeq->back();
}

pr_entity_t* _pr_occur_server(pr_entity_t *curentity){
    if (PR_ENTITY_SCHEME == curentity->type){
        ((pr_scheme_t *)curentity)->entityq->push_back(new pr_server_t());
        return ((pr_server_t *)curentity)->entityq->back();
    }
    return NULL;
}

pr_entity_t* _pr_occur_rewrite(pr_entity_t *curentity){
    if (PR_ENTITY_SCHEME == curentity->type){
        pr_scheme_t *scheme = (pr_scheme_t *)curentity;
        scheme->entityq->push_back(new pr_rewrite_t());
        return scheme->entityq->back();
    }
    else if (PR_ENTITY_IFBLOCK == curentity->type){
        pr_if_block_t *ifblock = (pr_if_block_t *)curentity;
        ifblock->entityq->push_back(new pr_rewrite_t());
        return ifblock->entityq->back();
    }
    else if (PR_ENTITY_SERVER == curentity->type){
        pr_server_t *server = (pr_server_t *)curentity;
        server->entityq->push_back(new pr_rewrite_t());
        return server->entityq->back();
    }
    return NULL;
}

pr_entity_t* _pr_occur_if_block(pr_entity_t *curentity,PR_IF_TYPE iftype){
    if (PR_ENTITY_SCHEME ==  curentity->type){
        pr_scheme_t *scheme = (pr_scheme_t *)curentity;
        scheme->entityq->push_back(new pr_if_block_t(iftype));
        return scheme->entityq->back();
    }
    else if (PR_ENTITY_IFBLOCK == curentity->type){
        pr_if_block_t *ifblock = (pr_if_block_t *)curentity;
        ifblock->entityq->push_back(new pr_if_block_t(iftype));
        return ifblock->entityq->back();
    }
    else if (PR_ENTITY_SERVER == curentity->type){
        pr_server_t *server = (pr_server_t *)curentity;
        server->entityq->push_back(new pr_if_block_t(iftype));
        return server->entityq->back();
    }
    
    return NULL;
}

#define nothing do{;}while(0)
#define PR_CLEAN_CAP() cap.clear();
#define PR_CAP() \
if (c == ' ' || c == '\r' || c == '\n') nothing;\
else cap.push_back(c);
#define PR_MOVE_STAT(__new_stat__) do{stat = __new_stat__; PR_CLEAN_CAP();}while(0)
#define PR_ESCAPE_WHITESPACE() if (c == ' ') continue;
#define PR_ESCAPE_NEWLINE() if (c == '\r' || c == '\n') continue;
#define PR_ENTITY_PUSH(__entity__)\
do{\
if (__entity__ && ( ebt.empty() || __entity__ != ebt.top()))\
    ebt.push(__entity__);\
}while(0);
#define PR_STAT_PUSH()\
do{\
    sbt.push(stat);\
}while(0);
#define PR_CURENTITY ebt.top()
#define PR_ENTITY_POP()\
do{\
    if (!ebt.empty()){\
        poped = ebt.top();\
        ebt.pop();\
    }\
}while(0)
#define PR_STAT_POP()\
do{\
    PR_CLEAN_CAP();\
    if (!sbt.empty()){\
        stat = sbt.top();\
        sbt.pop();\
        PR_ENTITY_POP();\
    }\
    else stat = PR_STAT_START;\
}while(0)

#define PR_RESCAN_C() i--;

typedef enum {
    EXP_REFLECT_SCAN,
    EXP_REFLECT_VAL,
    EXP_REFLECT_STRJUMP
}EXP_REFLECT_STAT;

string _reflect_val(string exp){
    string cap = string();
    string ret = string();
    EXP_REFLECT_STAT stat;
    PR_MOVE_STAT(EXP_REFLECT_SCAN);
    
    for (int i = 0; i < exp.size(); i++){
        char c = exp[i];
        switch (stat) {
            case EXP_REFLECT_SCAN:{
                if (c == '"'){
                    PR_MOVE_STAT(EXP_REFLECT_STRJUMP);
                    ret.push_back(c);
                }
                else if (c == '$'){
                    PR_MOVE_STAT(EXP_REFLECT_VAL);
                }
                else ret.push_back(c);
            }
                break;
            case EXP_REFLECT_VAL:{
                if (c == ' ' || PRIORITY_UNKNOW != get_operator_priority(string(1,c))){
                    pr_id id = pr_getval(cap.c_str());
                    if (id != pr_undefined()){
                        if (PR_ID_LONGLONG == id.type){
                            char buffer [20];
                            int n = sprintf(buffer, "%lld", id.longLongValue());
                            buffer[n] = '\0';
                            ret += string(buffer);
                        }
                        else if (PR_ID_DOUBLE == id.type){
                            char buffer [20];
                            int n = sprintf(buffer, "%f", id.doubleValue());
                            buffer[n] = '\0';
                            ret += string(buffer);
                        }
                        else if (PR_ID_STRING == id.type){
                            ret += id.stringValue();
                        }
                    }
                    PR_MOVE_STAT(EXP_REFLECT_SCAN);
                    PR_RESCAN_C();
                }
                else PR_CAP();
            }
                break;
            case EXP_REFLECT_STRJUMP:{
                ret.push_back(c);
                if (c == '"'){
                    PR_MOVE_STAT(EXP_REFLECT_SCAN);
                }
            }
                break;
            default:
                break;
        }
    }
    
    return ret;
}

void pr_parse(struct pr_file_t *f,const char *cnt){
    size_t len = strlen(cnt);
    string cap = string();
    PR_STAT stat;
    PR_MOVE_STAT(PR_STAT_START);
    
    stack<pr_entity_t *>ebt = stack<pr_entity_t *>();
    pr_entity_t *poped = NULL;
    stack<PR_STAT>sbt = stack<PR_STAT>();
    for (int i = 0; i < len; i++){
        char c = cnt[i];
        switch (stat) {
            case PR_STAT_START:{
                PR_STAT_PUSH();
                PR_MOVE_STAT(PR_STAT_SCHEME_OPT);
                PR_CAP();
            }break;
            case PR_STAT_SCHEME_OPT:{
                if (c == '{'){
                    pr_entity_t *e = _pr_occur_scheme(f,cap);
                    PR_ENTITY_PUSH(e);
                    PR_MOVE_STAT(PR_STAT_SCHEME);
                }else PR_CAP();
            }break;
            case PR_STAT_SCHEME:{
                PR_CAP();
                if (cap == "rewrite") {
                    pr_entity_t *e = _pr_occur_rewrite(PR_CURENTITY);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_REWRITE);
                }
                else if (cap == "server"){
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_SERVER_OPT);
                }
                else if (cap == "if"){
                    pr_entity_t *e = _pr_occur_if_block(PR_CURENTITY,PR_IF);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_IF);
                }
                else if (cap == "elif"){
                    pr_entity_t *e = _pr_occur_if_block(PR_CURENTITY,PR_ELIF);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_IF);
                }
                else if (cap == "else"){
                    pr_entity_t *e = _pr_occur_if_block(PR_CURENTITY,PR_ELSE);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_IF);
                }
                else if (c == '}') {
                    PR_STAT_POP();
                }
            }break;
            case PR_STAT_SERVER_OPT:{
                PR_ESCAPE_WHITESPACE();
                PR_ESCAPE_NEWLINE();
                if (c == '{'){
                    pr_entity_t *e = _pr_occur_server(PR_CURENTITY);
                    PR_ENTITY_PUSH(e);
                    PR_MOVE_STAT(PR_STAT_SERVER);
                }
            }break;
            case PR_STAT_SERVER:{
                PR_CAP();
                if (cap == "rewrite"){
                    pr_entity_t *e = _pr_occur_rewrite(PR_CURENTITY);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_REWRITE);
                }
                if (cap == "server_name"){
                    PR_MOVE_STAT(PR_STAT_SERVER_NAME);
                }
                else if (c == '}') PR_STAT_POP();
            }break;
            case PR_STAT_SERVER_NAME:{
                if (c == ';') {
                    pr_server_t *curs = (pr_server_t *)PR_CURENTITY;
                    curs->servern = cap;
                    PR_MOVE_STAT(PR_STAT_SERVER);
                }
                else PR_CAP();
            }break;
            case PR_STAT_REWRITE:{
                PR_CAP();
                PR_MOVE_STAT(PR_STAT_REWRITE_REGEX);
            }break;
            case PR_STAT_REWRITE_REGEX:{
                pr_rewrite_t *currw = (pr_rewrite_t *)PR_CURENTITY;
                if (!currw->incap){
                    PR_ESCAPE_WHITESPACE();
                    currw->incap = true;
                    PR_CAP();
                }
                else{
                    if (c == ' '){
                        currw->regex = new string(cap);
                        currw->incap = false;
                        PR_MOVE_STAT(PR_STAT_REWRITE_REPLACEMENT);
                    }
                    else PR_CAP();
                }
            }break;
            case PR_STAT_REWRITE_REPLACEMENT:{
                pr_rewrite_t *currw = (pr_rewrite_t *)PR_CURENTITY;
                if (!currw->incap){
                    PR_ESCAPE_WHITESPACE();
                    currw->incap = true;
                    PR_CAP();
                }
                else{
                    if (c == ' '){
                        currw->replacement = new string(cap);;
                        currw->incap = false;
                        PR_MOVE_STAT(PR_STAT_REWRITE_RULE);
                    }
                    else PR_CAP();
                }
            }break;
            case PR_STAT_REWRITE_RULE:{
                pr_rewrite_t *currw = (pr_rewrite_t *)PR_CURENTITY;
                if (!currw->incap){
                    PR_ESCAPE_WHITESPACE();
                    currw->incap = true;
                    PR_CAP();
                }
                else{
                    if (c == ' '){
                        currw->rule = new string(cap);;
                        currw->incap = false;
                        PR_MOVE_STAT(PR_STAT_REWRITE_LABELED);
                    }
                    else if (c == ';'){
                        currw->rule = new string(cap);;
                        currw->incap = false;
                        PR_STAT_POP();
                    }
                    else PR_CAP();
                }
            }break;
            case PR_STAT_REWRITE_LABELED:{
                pr_rewrite_t *currw = (pr_rewrite_t *)PR_CURENTITY;
                if (!currw->incap){
                    PR_ESCAPE_WHITESPACE();
                    currw->incap = true;
                    PR_CAP();
                }
                else{
                    if (c == ';'){
                        currw->labeled= new string(cap);;
                        currw->incap = false;
                        PR_STAT_POP();
                    }
                    else PR_CAP();
                }
            }break;
            case PR_STAT_IF:{
                PR_ESCAPE_WHITESPACE();
                PR_ESCAPE_NEWLINE();
                if (c == '('){
                    PR_MOVE_STAT(PR_STAT_IF_COND);
                }
                else if (c == '{'){
                    PR_MOVE_STAT(PR_STAT_IF_BLOCK);
                }
                else PR_CAP();
            }break;
            case PR_STAT_IF_COND:{
                PR_ESCAPE_WHITESPACE();
                if (c == ')'){
                    pr_if_block_t *ifb = (pr_if_block_t *)PR_CURENTITY;
                    ifb->cond = _reflect_val(cap);
                }
                else if (c == '{'){
                    PR_MOVE_STAT(PR_STAT_IF_BLOCK);
                }
                else PR_CAP();
            }break;
            case PR_STAT_IF_BLOCK:{
                PR_CAP();
                if (cap == "rewrite"){
                    pr_entity_t *e = _pr_occur_rewrite(PR_CURENTITY);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_REWRITE);
                }
                else if (cap == "if"){
                    pr_entity_t *e = _pr_occur_if_block(PR_CURENTITY,PR_IF);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_IF);
                }
                else if (cap == "elif"){
                    pr_entity_t *e = _pr_occur_if_block(PR_CURENTITY,PR_ELIF);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_IF);
                }
                else if (cap == "else"){
                    pr_entity_t *e = _pr_occur_if_block(PR_CURENTITY,PR_ELSE);
                    PR_ENTITY_PUSH(e);
                    PR_STAT_PUSH();
                    PR_MOVE_STAT(PR_STAT_IF);
                }
                else if (c == '}'){
                    PR_STAT_POP();
                }
            }break;
            default:break;
        }
    }
}

struct pr_file_t* pr_creat(const char *cnt){
    pr_file_t *prf = new pr_file_t();
//    FILE *fp = fopen("/Users/ris/Desktop/PoolRouter/PoolRouter/pr.txt","r");
//    int err = ferror(fp);
//    int size = fseek(fp, 0, SEEK_END);
    pr_parse(prf,cnt);
    
    _pr_printfile(prf);
    return prf;
}

bool _pr_parse_targeturi(pr_rewrite_t*rw,string uri){
    regex_t regcomped;
    if (0 == regcomp(&regcomped,rw->regex->c_str(), REG_EXTENDED)){
        size_t ngroups = regcomped.re_nsub + 1;
        regmatch_t groups[ngroups];
        int stat = regexec(&regcomped, uri.c_str(), ngroups, groups, 0);
        if (0 == stat){
            rw->outval = new string();
            bool isdollarval = false;
            int icap = 0;
            for (int i = 0; i < rw->replacement->size(); i++){
                char c = (*rw->replacement)[i];
                if (isdollarval){
                    if (c >= '0' && c <= '9'){
                        icap = 10 * icap + c - '0';
                        if (i == rw->replacement->size() - 1){
                            regmatch_t mach = groups[icap];
                            *rw->outval += uri.substr(mach.rm_so,mach.rm_eo-mach.rm_so);
                        }
                    }
                    else{
                        
                        regmatch_t mach = groups[icap];
                        *rw->outval += uri.substr(mach.rm_so,mach.rm_eo-mach.rm_so);
                        rw->outval->push_back(c);
                        isdollarval = false,icap = 0;
                        
                    }
                }
                else{
                    if (c == '$'){
                        isdollarval = true;
                    }
                    else rw->outval->push_back(c);
                }
            }
        }
        else return false;
    }
    return true;
}

struct pr_uri_t{
    string scheme;
    string host;
    string path;
    string muri;
};

struct pr_rewrite_t* _pr_search_entityq(deque<pr_entity_t*> *entityq,pr_uri_t uri){
    struct pr_rewrite_t* ret = NULL;
    bool ignoreifb = false; //if ... elif ... else ... 某个分支进入后忽略非if分支
    for (int i = 0; i < entityq->size(); i++) {
        pr_entity_t *e = entityq->at(i);
        if (e->type == PR_ENTITY_REWRITE){
            pr_rewrite_t *rw = (pr_rewrite_t *)e;
            if (_pr_parse_targeturi(rw, uri.muri)){
                if (ret) pr_safe_delete(ret);
                ret = rw->copy();
                ret->schemen = new string(uri.scheme);
                ret->servern = new string(uri.host);
                if (rw->labeled && *rw->labeled == "break") return ret;
            }
        }
        else if (e->type == PR_ENTITY_IFBLOCK){
            pr_if_block_t *ifb = (pr_if_block_t *)e;
            if (PR_IF == ifb->iftype){
                pr_id condret = calc_cond_ast(cond_ast_creat(ifb->cond));
                if(condret >= pr_yes()){
                    ignoreifb = true;
                    pr_rewrite_t* tmp = _pr_search_entityq(ifb->entityq,uri);
                    if(tmp){
                        if (ret) pr_safe_delete(ret);
                        ret = tmp;
                        if (ret->labeled && *ret->labeled == "break") return ret;
                    }
                }
                else ignoreifb = false;
            }
            else if (PR_ELIF == ifb->iftype){
                if (ignoreifb) continue;
                pr_id condret = calc_cond_ast(cond_ast_creat(ifb->cond));
                if(condret >= pr_yes()){
                    ignoreifb = true;
                    pr_rewrite_t* tmp = _pr_search_entityq(ifb->entityq,uri);
                    if(tmp){
                        if (ret) pr_safe_delete(ret);
                        ret = tmp;
                        if (ret->labeled && *ret->labeled == "break") return ret;
                    }
                    
                }
                else ignoreifb = false;
            }
            else if (PR_ELSE == ifb->iftype){
                if (ignoreifb) continue;
                pr_rewrite_t* tmp = _pr_search_entityq(ifb->entityq,uri);
                if(tmp){
                    if (ret) pr_safe_delete(ret);
                    ret = tmp;
                    if (ret->labeled && *ret->labeled == "break") return ret;
                }
            }
        }
        else if (e->type == PR_ENTITY_SERVER){
            pr_server_t *s = (pr_server_t *)e;
            if (s->servern == uri.host){
                
                pr_rewrite_t* tmp = _pr_search_entityq(s->entityq,
                                                       pr_uri_t{
                                                           .scheme = uri.scheme,
                                                           .host = uri.host,
                                                           .path = uri.path,
                                                           .muri = uri.path
                                                       });
                
                if (tmp){
                    if (ret) pr_safe_delete(ret);
                    ret = tmp;
                    if (ret->labeled && *ret->labeled == "break") return ret;
                }
            }
        }
    }
    
    return ret;
}

struct pr_rewrite_t* pr_rewrite_matched_creat(struct pr_file_t*f,const char *scheme,const char *host,const char *path){
    pr_uri_t uri = pr_uri_t{
        .scheme = string(scheme),
        .host = string(host),
        .path = string(path),
        .muri = string(host) + string (path)
    };
    
    struct pr_rewrite_t* ret = NULL;
    for(int i = 0; i < f->schemeq->size(); i++){
        pr_scheme_t *scheme = f->schemeq->at(i);
        if (scheme->schemen == uri.scheme){
            ret = _pr_search_entityq(scheme->entityq, uri);
            break;
        }
    }
    
    return ret;
}

const char *pr_getserver(struct pr_rewrite_t *rw){
    return rw->servern->c_str();
}

const char *pr_getscheme(struct pr_rewrite_t *rw){
    return rw->schemen->c_str();
}

const char *pr_geturl(struct pr_rewrite_t *rw){
    return rw->outval->c_str();
}

const char *pr_getrule(struct pr_rewrite_t *rw){
    return rw->rule->c_str();
}

void pr_rewritefree(struct pr_rewrite_t *rw){
    pr_safe_delete(rw);
}




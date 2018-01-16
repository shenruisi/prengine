//
//  prengine_val.cpp
//  PoolRouter
//
//  Created by ris on 1/11/17.
//  Copyright © 2017 ris. All rights reserved.
//

#include "prengine_val.h"
#include <stdio.h>
#include <string.h>

pr_id pr_yes(){
    return pr_id((long long)1);
}

pr_id pr_no(){
    return pr_id((long long)0);
}

pr_id pr_undefined(){
    return pr_id();
}

string pr_id::stringValue(){
    return this->str;
}

long long pr_id::longLongValue(){
    return this->number.ll;
}

double pr_id::doubleValue(){
    return this->number.d;
}

pr_id::pr_id(){
    this->undefined = true;
}

pr_id::pr_id(double val){
    this->undefined = false;
    this->type = PR_ID_DOUBLE;
    this->number.d = val;
}

pr_id::pr_id(long long val){
    this->undefined = false;
    this->type = PR_ID_LONGLONG;
    this->number.ll = val;
}

pr_id::pr_id(string val){
    this->undefined = false;
    this->type = PR_ID_STRING;
    this->str = val;
}

void pr_id::print(){
    switch (this->type) {
        case PR_ID_LONGLONG:
            printf("%lld",this->longLongValue());
            break;
        case PR_ID_DOUBLE:
            printf("%f",this->doubleValue());
            break;
        case PR_ID_STRING:
            printf("%s",this->stringValue().c_str());
            break;
        default:
            break;
    }
}

pr_id pr_id::operator+(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        return pr_id(number.ll+id.number.ll);
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        return pr_id(number.ll+id.number.d);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        return pr_id(number.d+id.number.ll);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        return pr_id(number.d+id.number.d);
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return pr_id(str+id.str);
    }
    else{
        return pr_yes();
    }
}

pr_id pr_id::operator-(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        return pr_id(number.ll-id.number.ll);
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        return pr_id(number.ll-id.number.d);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        return pr_id(number.d-id.number.ll);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        return pr_id(number.d-id.number.d);
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return pr_yes();
    }
    else{
        return pr_no();
    }
}

pr_id pr_id::operator*(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        return pr_id(number.ll*id.number.ll);
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        return pr_id(number.ll*id.number.d);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        return pr_id(number.d*id.number.ll);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        return pr_id(number.d*id.number.d);
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return pr_yes();
    }
    else{
        return pr_no();
    }
}

pr_id pr_id::operator/(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        return pr_id(number.ll/id.number.ll);
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        return pr_id(number.ll/id.number.d);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        return pr_id(number.d/id.number.ll);
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        return pr_id(number.d/id.number.d);
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return pr_yes();
    }
    else{
        return pr_no();
    }
}

pr_id pr_id::operator%(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        return pr_id(number.ll%id.number.ll);
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        return pr_no();
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        return pr_no();
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return pr_yes();
    }
    else{
        return pr_no();
    }
}

bool pr_id::operator==(const pr_id &id){
    if (undefined && id.undefined){
        return true;
    }
    else if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG && !undefined && !id.undefined){
        if (number.ll == id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE && !undefined && !id.undefined){
        if (number.ll == id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG && !undefined && !id.undefined){
        if (number.ll == id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE && !undefined && !id.undefined){
        if (number.d == id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING && !undefined && !id.undefined){
        if (str == id.str) return true;
        else return false;
    }
    else{
        return false;
    }
}

bool pr_id::operator>(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        if (number.ll > id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        if (number.ll > id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        if (number.ll > id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        if (number.d > id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return false;
    }
    else{
        return false;
    }
}

bool pr_id::operator>=(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        if (number.ll >= id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        if (number.ll >= id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        if (number.ll >= id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        if (number.d >= id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return false;
    }
    else{
        return false;
    }
}

bool pr_id::operator<(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        if (number.ll < id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        if (number.ll < id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        if (number.ll < id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        if (number.d < id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return false;
    }
    else{
        return false;
    }
}

bool pr_id::operator<=(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        if (number.ll <= id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        if (number.ll <= id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        if (number.ll <= id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        if (number.d <= id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return false;
    }
    else{
        return false;
    }
}

bool pr_id::operator!=(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        if (number.ll != id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        if (number.ll != id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        if (number.ll != id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        if (number.d != id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        return true;
    }
    else{
        return true;
    }
}

bool pr_id::operator&&(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        if (number.ll && id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        if (number.ll && id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        if (number.ll && id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        if (number.d && id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        if (!str.empty() && !id.str.empty()) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_LONGLONG){
        if (!str.empty() && id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_DOUBLE){
        if (!str.empty() && id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_STRING){
        if (number.ll && !id.str.empty()) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_STRING){
        if (number.d && !id.str.empty()) return true;
        else return false;
    }
    else{
        return false;
    }
}


bool pr_id::operator||(const pr_id &id){
    if(type == PR_ID_LONGLONG && id.type == PR_ID_LONGLONG){
        if (number.ll || id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_DOUBLE){
        if (number.ll || id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_LONGLONG){
        if (number.ll || id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_DOUBLE){
        if (number.d && id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_STRING){
        if (!str.empty() || !id.str.empty()) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_LONGLONG){
        if (!str.empty() || id.number.ll) return true;
        else return false;
    }
    else if (type == PR_ID_STRING && id.type == PR_ID_DOUBLE){
        if (!str.empty() || id.number.d) return true;
        else return false;
    }
    else if (type == PR_ID_LONGLONG && id.type == PR_ID_STRING){
        if (number.ll || !id.str.empty()) return true;
        else return false;
    }
    else if (type == PR_ID_DOUBLE && id.type == PR_ID_STRING){
        if (number.d || !id.str.empty()) return true;
        else return false;
    }
    else{
        return false;
    }
}


pr_id pr_getval(const char* vname){
    if(strcmp(vname, "version") == 0){
        return pr_id((double)0.01);
    }
    return pr_undefined();
}

void pr_valfree(pr_id* id){
    if(id) { delete id,id = NULL; }
}

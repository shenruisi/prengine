//
//  prengine_val.hpp
//  PoolRouter
//
//  Created by ris on 1/11/17.
//  Copyright © 2017 ris. All rights reserved.
//

#ifndef prengine_val_hpp
#define prengine_val_hpp


#include <string>
using namespace std;
typedef enum {
    PR_ID_LONGLONG,
    PR_ID_DOUBLE,
    PR_ID_STRING
}PR_ID_TYPE;



class pr_id{
public:
    union{
        double d;
        long long ll;
    }number;
    bool undefined = false;
    string str;
    PR_ID_TYPE type;

    pr_id();
    pr_id(double);
    pr_id(long long);
    pr_id(string);

    double doubleValue();
    long long longLongValue();
    string stringValue();

    void print();

    pr_id operator+(const pr_id &);
    pr_id operator-(const pr_id &);
    pr_id operator*(const pr_id &);
    pr_id operator/(const pr_id &);
    pr_id operator%(const pr_id &);
    bool operator==(const pr_id &);
    bool operator>(const pr_id &);
    bool operator>=(const pr_id &);
    bool operator<(const pr_id &);
    bool operator<=(const pr_id &);
    bool operator!=(const pr_id &);
    bool operator&&(const pr_id &);
    bool operator||(const pr_id &);
};

pr_id pr_yes();
pr_id pr_no();
pr_id pr_undefined();
pr_id pr_getval(const char*);
void pr_valfree(pr_id*);

#endif /* prengine_val_hpp */

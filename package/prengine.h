//
//  prengine.h
//  PoolRouter
//
//  Created by ris on 12/29/16.
//  Copyright © 2016 ris. All rights reserved.
//

#ifndef prengine_h
#define prengine_h

#include <stdio.h>
#include "prengine_val.h"

typedef struct pr_file_t PRFILE;
typedef struct pr_rewrite_t PRREWRITE;

PRFILE* pr_creat(char *cnt);
PRREWRITE* pr_rewrite_matched_creat(PRFILE *f,const char *uri);
void pr_set_valhandler(PRFILE *f,pr_id(*valhandler)(const char *vname));
const char *pr_getserver(PRREWRITE *);
const char *pr_getscheme(PRREWRITE *);
const char *pr_getoutval(PRREWRITE *);
const char *pr_getrule(PRREWRITE *);
void pr_rewritefree(PRREWRITE *);
#endif /* prengine_h */

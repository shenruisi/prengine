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

typedef struct pr_file_t PRFILE;
typedef struct pr_rewrite_t PRREWRITE;

PRFILE* pr_creat(const char *path);
PRREWRITE* pr_rewrite_matched_creat(PRFILE *f,const char *scheme,const char *host,const char *path);
const char *pr_getserver(PRREWRITE *);
const char *pr_getscheme(PRREWRITE *);
const char *pr_geturl(PRREWRITE *);
const char *pr_getrule(PRREWRITE *);
void pr_rewritefree(PRREWRITE *);
#endif /* prengine_h */

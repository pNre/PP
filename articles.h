#ifndef ARTICLES_H
#define ARTICLES_H

#include <time.h>

#include "lib/ht.h"

typedef struct article {
    char *name;
    char *title;
    char *contents;
    char *summary;
    time_t timestamp;
} article_t;

void articles_load(char *path, ht_t *table);
int articles_compare(const void *lhs, const void *rhs);

#endif


#ifndef SUPPORT_HT_H
#define SUPPORT_HT_H

#include <string.h>

typedef struct ht_node_s {

    char *key;
    void *value;

    void (*value_free)(void *);

    struct ht_node_s *next;

} ht_node_t;

typedef struct ht_s {

    ht_node_t **nodes;
    size_t size;
    size_t count;

} ht_t;

ht_t *ht_alloc(size_t size);
void *ht_find(ht_t *table, char *key);
void **ht_sorted_values(ht_t *table, int (*sorting_function)(const void *, const void *), size_t *count);
void ht_insert(ht_t *table, char *key, void *value, void (*value_free)(void *));
void ht_free(ht_t *table);

#endif


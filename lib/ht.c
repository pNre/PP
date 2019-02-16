#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>

#include "ht.h"

uint32_t hash_f(char *key, uint32_t table_size) {

    uint32_t hash = 0x811C9DC5;
    char c;

    while ((c = *key++)) {
        hash = (c ^ hash) * 0x01000193;
    }

    return hash % table_size;

}

ht_t *ht_alloc(size_t size) {

    ht_t *table = (ht_t *)malloc(sizeof(ht_t));
    if (!table) {
        return NULL;
    }

    table->count = 0;
    table->size = size;
    table->nodes = (ht_node_t **)calloc(size, sizeof(ht_node_t *));
    if (!table->nodes) {
        free(table);
        return NULL;
    }

    return table;

}

void *ht_find(ht_t *table, char *key) {

    ht_node_t *node = table->nodes[hash_f(key, table->size)];

    while (node) {
        if (!strcmp(node->key, key)) {
            return node->value;
        }

        node = node->next;
    }

    return NULL;

}

void **ht_sorted_values(ht_t *table, int (*sorting_function)(const void *, const void *), size_t *count) {

    void **values = calloc(table->count, sizeof(void *));

    *count = 0;

    size_t i;
    for (i = 0; i < table->size; i++) {
        ht_node_t *node = table->nodes[i];
        while (node) {
            values[*count] = node->value;
            *count += 1;
            node = node->next;
        }
    }

    qsort(values, *count, sizeof(void *), sorting_function);

    return values;

}

void ht_insert(ht_t *table, char *key, void *value, void (*value_free)(void *)) {

    size_t index = hash_f(key, table->size);

    ht_node_t *node = table->nodes[index];
    ht_node_t *previous_node = NULL;

    while (node) {
        //  key already present, update
        if (!strcmp(node->key, key)) {
            node->value_free(node->value);
            node->value = value;
            node->value_free = value_free;
            return;
        }

        previous_node = node;
        node = node->next;
    }

    ht_node_t *new_node = (ht_node_t *)malloc(sizeof(ht_node_t));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->value_free = value_free;
    new_node->next = NULL;

    if (previous_node == NULL) {
        table->nodes[index] = new_node;
    } else {
        previous_node->next = new_node;
    }

    table->count += 1;

}

void ht_node_free(ht_node_t *node) {

    node->value_free(node->value);
    free(node->key);
    free(node);

}

void ht_free(ht_t *table) {

    size_t i;
    for (i = 0; i < table->size; i++) {
        ht_node_t *node = table->nodes[i];
        while (node) {
            ht_node_t *current = node;
            node = node->next;
            ht_node_free(current);
        }
    }

    free(table->nodes);
    free(table);

}

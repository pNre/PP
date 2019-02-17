#include <stdio.h>
#include <stdlib.h>
#include <sexpr.h>
#include "lib/io.h"
#include "lib/ht.h"

list_t *read_config(char *filename) {
    char *config = read_file(filename, NULL);
    if (!config) {
        fprintf(stderr, "no config file %s\n", filename);
        return NULL;
    }

    sexpr_parse_error_t err;
    list_t *exprs = sexpr_from_string(config, &err);
    if (!exprs) {
        fprintf(stderr, "invalid config file %s near %zu\n", filename, err.pos);
        free(config);
        return NULL;
    }

    free(config);
    return exprs;
}

char *config_string_at(sexpr_t *config, char *path, char *fallback) {
    sexpr_t * list = sexpr_list_with_symbol_at(config, path);
    if (!list) {
        return fallback;
    }

    sexpr_t *expr = sexpr_list_nth_item(list, 1);
    if (!expr && expr->type != SEXPR_TYPE_STRING) {
        return fallback;
    }

    return expr->string_val;
}


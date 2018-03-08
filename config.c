#include <stdio.h>
#include <stdlib.h>

#include "support/io.h"
#include "support/ht.h"

ht_t *read_config(char *filename) {

    char *config = read_file(filename);
    if (!config) {
        return NULL;
    }

    ht_t *config_table = ht_alloc(16);
    if (!config_table) {
        free(config);
        return NULL;
    }

    char *line = strtok(config, "\n");
    size_t eqoffset = 0;

    while (line) {

        eqoffset = strcspn(line, "=");
        if (eqoffset == strlen(line)) {
            line = strtok(NULL, "\n");
            continue;
        }

        line[eqoffset] = '\0';

        char *key = line;
        char *value = strdup(line + eqoffset + 1);
        ht_insert(config_table, key, value, free);

        line = strtok(NULL, "\n");

    }

    free(config);

    return config_table;

}


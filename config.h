#ifndef CONFIG_H
#define CONFIG_H

#include <sexpr.h>

list_t *read_config(char *filename);
char *config_string_at(sexpr_t *config, char *path, char *fallback);
void config_iter_list_at(sexpr_t *config, char *path, void(*iterator)(void *, sexpr_t *), void *ctx);

#endif


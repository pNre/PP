#include <stdio.h>
#include <string.h>

char *html_strip_tags(char *buffer) {
    char *stripped = strdup(buffer);
    size_t i, j = 0;
    for (i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '<') {
            while (buffer[++i] != '>');
        } else {
            stripped[j++] = buffer[i];
        }
    }
    stripped[j] = 0;

    return stripped;
}

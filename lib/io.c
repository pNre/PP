#include <stdio.h>
#include <stdlib.h>

char *read_file(char *pathname, long *length) {
    FILE *f;
    char *buff;
    long size;

    f = fopen(pathname, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    if (size <= 0) {
        fclose(f);
        return NULL;
    }

    buff = malloc(size + 1);
    if (!buff) {
        fclose(f);
        return NULL;
    }

    if (fread(buff, size, 1, f) <= 0) {
        free(buff);
        fclose(f);
        return NULL;
    }

    fclose(f);

    if (length) {
        *length = size;
     }

    buff[size] = 0;
    return buff;
}


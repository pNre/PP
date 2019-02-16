#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>
#include <evhtp/evhtp.h>
#include <time.h>
#include "articles.h"
#include "support/html.h"
#include "support/io.h"

void article_free(void *data) {

    struct article *article = data;

    if (article->name) {
        free(article->name);
    }

    if (article->title) {
        free(article->title);
    }

    if (article->contents) {
        free(article->contents);
    }

    if (article->summary) {
        free(article->summary);
    }

    free(article);

}

int articles_compare(const void *lhs, const void *rhs) {

    time_t lhs_ts = ((struct article **)lhs)[0]->timestamp;
    time_t rhs_ts = ((struct article **)rhs)[0]->timestamp;

    if (lhs_ts > rhs_ts) {
        return -1;
    } else if (rhs_ts > lhs_ts) {
        return 1;
    } else {
        return 0;
    }

}

struct article *article_parse(char *pathname) {

    char *data, *data_ptr;
    long data_length = 0;
    char *date;
    char *title;
    struct article *art;
    struct tm tm = {0};

    data = read_file(pathname, &data_length);
    data_ptr = data;
    if (!data) {
        return NULL;
    }

    art = malloc(sizeof(struct article));
    if (!art) {
        free(data);
        return NULL;
    }

    bzero(art, sizeof(struct article));

    //  use the filename as name
    char *temp_pathname = strdup(pathname);
    if (temp_pathname) {
        art->name = strdup(basename(temp_pathname));
        char *dot = strchr(art->name, '.');
        if (dot) {
            *dot = '\0';
        }
        free(temp_pathname);
    }

    size_t offset = strcspn(data_ptr, "\n");
    date = strndup(data_ptr, offset);
    data_ptr += offset + 1;
    if (!date) {
        free(data);
        return NULL;
    }

    //  line 1: article date
    strptime(date, "%d/%m/%Y %H:%M", &tm);
    art->timestamp = mktime(&tm);

    //  line 2: article title
    if ((data_ptr - data) < data_length) {
        offset = strcspn(data_ptr, "\n");
	    art->title = strndup(data_ptr, offset);
        data_ptr += offset + 1;
    } else {
        art->title = NULL;
    }

    //  line 3: article contents
    if (offset < data_length) {
	    art->contents = strndup(data_ptr, data_length - offset);
    } else {
        art->contents = NULL;
    }

    free(data);

    if (!art->contents) {
        article_free(art);
        return NULL;
    }

    //  the the first sentence of the article and make it the summary
    char *stripped = html_strip_tags(art->contents);
    if (stripped) {
        char *stop = strchr(stripped, '.');
        if (stop) {
            *stop = 0;
        }

        char *trimmed = stripped;
        while (isspace(*trimmed)) {
            trimmed++;
        }

        art->summary = strdup(trimmed);

        free(stripped);
    } else {
        art->summary = NULL;
    }

    return art;

}

void articles_load(char *path, ht_t *table) {

    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path))) {
        return;
    }

    while ((entry = readdir(dir))) {

        if (entry->d_type != DT_REG) {
            continue;
        }

        size_t name_length = strlen(entry->d_name);
        if (name_length < 4 || strncmp(entry->d_name + name_length - 4, "html", 4) != 0) {
            continue;
        }

        char *filename = malloc(strlen(path) + strlen(entry->d_name) + 1);
        sprintf(filename, "%s/%s", path, entry->d_name);

        printf("Loading %s\n", filename);

        struct article *art = article_parse(filename);
        if (art) {
            printf("(Loaded) %s\n", art->name);
            ht_insert(table, art->name, art, article_free);
        }

        free(filename);

    }

    closedir(dir);

}

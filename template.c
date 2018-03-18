#include <sys/stat.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>

#include "articles.h"
#include "support/ht.h"

void evbuffer_add_page_header(struct evbuffer *buffer, ht_t *config) {

    char *title, *email;

    if (config != NULL) {
        title = ht_find(config, "title") ?: "";
        email = ht_find(config, "email") ?: "";
    } else {
        title = "";
        email = "";
    }

    evbuffer_add_printf(buffer,
            "<!DOCTYPE html>\n"
            "<html>\n"
            " <head>\n"
            "  <meta charset=\"utf-8\">\n"
            "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
            "  <title>%s</title>\n"
            "  <link href=\"/style.css\" rel=\"stylesheet\" type=\"text/css\"/>\n"
            " </head>\n"
            " <body>\n"
            "  <div class=\"navigation\">\n"
            "   <a href=\"/\">index</a>\n"
            "   <a href=\"mailto:%s\">mail</a>\n"
            "  </div>\n",
            title, email);

}

void evbuffer_add_page_footer(struct evbuffer *buffer) {

    evbuffer_add_printf(buffer,
            " </body>\n"
            "<html>");

}

void evbuffer_add_article(struct evbuffer *buffer, article_t *article) {

    char formatted_date[64];
    time_t timestamp = article->timestamp;

    strftime(formatted_date, sizeof(formatted_date) - 1, "%A %e %Y, %H:%M", localtime(&timestamp));

    evbuffer_add_printf(buffer,
            "<div class=\"title\">\n"
            " <a href=\"/a/%s\">%s</a>\n"
            " <div class=\"date\">%s</div>\n"
            "</div>\n"
            "<div class=\"contents\">%s</div>\n",
            article->name, article->title, formatted_date, article->contents);

}

void evbuffer_add_separator(struct evbuffer *buffer) {

    evbuffer_add_printf(buffer, "<div class=\"separator\"></div>");

}

#include <sys/stat.h>
#include <fcntl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <sexpr.h>
#include "articles.h"
#include "config.h"
#include "lib/ht.h"

void evbuffer_add_page_header_links(void *ctx, sexpr_t *link) {
    struct evbuffer *buffer = ctx;
    sexpr_t *title_expr = sexpr_list_nth_item(link, 0);
    sexpr_t *href_expr = sexpr_list_nth_item(link, 1);

    if (!title_expr || !href_expr) {
        return;
    }

    if (title_expr->type != SEXPR_TYPE_STRING || href_expr->type != SEXPR_TYPE_STRING) {
        return;
    }

    evbuffer_add_printf(buffer,
                        "   <a href=\"%s\">%s</a>",
                        (char *)href_expr->string_val, (char *)title_expr->string_val);
}

void evbuffer_add_page_header(struct evbuffer *buffer, list_t *config) {
    char *title = config_string_at(config->value, "conf.title", "");

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
                        "   <a href=\"/\">index</a>\n", title);

    config_iter_list_at(config->value, "conf.links", evbuffer_add_page_header_links, buffer);
    evbuffer_add_printf(buffer, "  </div>");
}

void evbuffer_add_page_footer(struct evbuffer *buffer) {
    evbuffer_add_printf(buffer,
            " </body>\n"
            "<html>");
}

void evbuffer_add_article(struct evbuffer *buffer, article_t *article, int include_contents) {
    char formatted_date[64];
    time_t timestamp = article->timestamp;

    strftime(formatted_date, sizeof(formatted_date) - 1, "%A %e %Y, %H:%M", localtime(&timestamp));

    evbuffer_add_printf(
        buffer,
        "<div class=\"title\">\n"
        " <a href=\"/a/%s\">%s</a>\n"
        " <div class=\"date\">%s</div>\n"
        "</div>\n",
        article->name, article->title, formatted_date);

    if (include_contents) {
        evbuffer_add_printf(buffer, "<div class=\"contents\">%s</div>\n", article->contents);
    }
}

void evbuffer_add_separator(struct evbuffer *buffer) {
    evbuffer_add_printf(buffer, "<div class=\"separator\"></div>");
}

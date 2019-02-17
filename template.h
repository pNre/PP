#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <evhtp/evhtp.h>
#include <sexpr.h>

typedef struct article article_t;

void evbuffer_add_page_header(struct evbuffer *buffer, list_t *config);
void evbuffer_add_page_footer(struct evbuffer *buffer);
void evbuffer_add_article(struct evbuffer *buffer, article_t *article, int include_contents);
void evbuffer_add_separator(struct evbuffer *buffer);

#endif

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <evhtp.h>

typedef struct article article_t;
typedef struct ht_s ht_t;

void evbuffer_add_page_header(struct evbuffer *buffer, ht_t *config);
void evbuffer_add_page_footer(struct evbuffer *buffer);
void evbuffer_add_article(struct evbuffer *buffer, article_t *article);
void evbuffer_add_separator(struct evbuffer *buffer);

#endif

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <evhtp.h>

#include "articles.h"
#include "config.h"
#include "template.h"

#define DEFAULT_PORT 8080

ht_t *articles;
ht_t *config;
pthread_mutex_t articles_lock;

void index_cb(evhtp_request_t *request, void *arg) {

    pthread_mutex_lock(&articles_lock);

    size_t articles_count = 0;
    article_t **articles_list = (article_t **)ht_sorted_values(articles, articles_compare, &articles_count);

    evbuffer_add_page_header(request->buffer_out, config);

    size_t index;
    for (index = 0; index < articles_count; index++) {
        evbuffer_add_article(request->buffer_out, articles_list[index]);
        if (index < articles_count - 1) {
            evbuffer_add_separator(request->buffer_out);
        }
    }

    evbuffer_add_page_footer(request->buffer_out);

    free(articles_list);

    evhtp_header_t *content_type = evhtp_header_new("Content-Type", "text/html; charset=utf-8", 1, 1);
    evhtp_headers_add_header(request->headers_out, content_type);

    evhtp_send_reply(request, EVHTP_RES_OK);

    pthread_mutex_unlock(&articles_lock);

}

void redirect_to_index(evhtp_request_t *request) {

    evhtp_header_t *location = evhtp_header_new("Location", "/", 1, 1);
    evhtp_headers_add_header(request->headers_out, location);
    evhtp_send_reply(request, EVHTP_RES_MOVEDPERM);

}

void article_cb(evhtp_request_t *request, void *arg) {

    char *file = request->uri->path->file;

    if (!file) {
        return redirect_to_index(request);
    }

    pthread_mutex_lock(&articles_lock);
    article_t *article = ht_find(articles, file);
    if (!article) {
        pthread_mutex_unlock(&articles_lock);
        return redirect_to_index(request);
    }

    evbuffer_add_page_header(request->buffer_out, config);
    evbuffer_add_article(request->buffer_out, article);
    evbuffer_add_page_footer(request->buffer_out);

    evhtp_header_t *content_type = evhtp_header_new("Content-Type", "text/html; charset=utf-8", 1, 1);
    evhtp_headers_add_header(request->headers_out, content_type);

    evhtp_send_reply(request, EVHTP_RES_OK);

    pthread_mutex_unlock(&articles_lock);

}

int parse_opts(ev_uint16_t *port, int argc, char ** argv) {

    unsigned long port_arg = 0;

    int arg;

    while ((arg = getopt(argc, argv, "p:")) != -1) {
        switch (arg) {
            case 'p':
                port_arg = strtoul(optarg, NULL, 10);
                break;
            default:
                return 0;
        }
    }

    if (port_arg == 0 || port_arg > EV_UINT16_MAX) {
        *port = DEFAULT_PORT;
    } else {
        *port = (ev_uint16_t)port_arg;
    }

    return 1;

}

void init_thread(evhtp_t *htp, evthr_t *thread, void *arg) {
}

int main(int argc, char **argv) {

    evhtp_t *htp;
    evbase_t *base;

    signal(SIGPIPE, SIG_IGN);

    //  configure http server
    ev_uint16_t port = 0;
    if (!parse_opts(&port, argc, argv)) {
        fprintf(stderr, "parse_opts\n");
        return EXIT_FAILURE;
    }

    if (pthread_mutex_init(&articles_lock, NULL) != 0) {
        fprintf(stderr, "pthread_mutex_init\n");
        return EXIT_FAILURE;
    }

    //  load the articles
    pthread_mutex_lock(&articles_lock);
    articles = ht_alloc(32);
    articles_load("resources/articles", articles);
    pthread_mutex_unlock(&articles_lock);

    //  load config (if present)
    config = read_config("resources/config.txt");

    //
    base = event_base_new();
    if (!base) {
        fprintf(stderr, "event_base\n");
        return EXIT_FAILURE;
    }

    htp = evhtp_new(base, NULL);
    if (!htp) {
        fprintf(stderr, "evhtp\n");
        return EXIT_FAILURE;
    }

    //  article callback
    evhtp_set_glob_cb(htp, "/a/*", article_cb, NULL);
    //  generic callback
    evhtp_set_gencb(htp, index_cb, NULL);
    //  set max threads to use
    int threads = (int)sysconf(_SC_NPROCESSORS_CONF);
    evhtp_use_threads(htp, init_thread, threads, NULL);
    //  max requests per connection
    evhtp_set_max_keepalive_requests(htp, 1);

    //  listen
    evhtp_bind_socket(htp, "127.0.0.1", port, 1024);
    printf("Listening on port %d\n", port);

    event_base_loop(base, 0);

    evhtp_unbind_socket(htp);
    event_base_free(base);

    return EXIT_SUCCESS;

}

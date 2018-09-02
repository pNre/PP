#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <evhtp.h>
#if __linux__
#include <sys/inotify.h>
#endif

#include "fs.h"

#define INOTIFY_MASK (IN_MODIFY | IN_MOVE)

watching_ctx_t *inotify_watch(char *path, evbase_t *evbase);

#if __linux__
static void inotify_on_event(struct bufferevent* event, void* context) {
    watching_ctx_t *ctx = context;
    struct inotify_event ev;
    while (bufferevent_read(event, &ev, sizeof(ev)) != 0) {
        if ((ev.mask & INOTIFY_MASK) == 0) {
            continue;
        }

        if (ev.len > 0) {
            evbuf_t *buffer = bufferevent_get_output(event);
            evbuffer_drain(buffer, ev.len);
        }

        ctx->callback();
    }
}
#endif

watching_ctx_t *watch(char *path, evbase_t *evbase, void(*callback)(void)) {
    #if __linux__
    watching_ctx_t *ctx;

    ctx = malloc(sizeof(watching_ctx_t));
    if (ctx == NULL) {
        fprintf(stderr, "couldn't allocate watching_ctx_t");
        return NULL;
    }

    ctx->callback = callback;
    ctx->event = NULL;
    ctx->infd = -1;
    ctx->inwd = -1;

    ctx->infd = inotify_init();
    if (ctx->infd == -1) {
        fprintf(stderr, "inotify initialization failed\n");
        watching_ctx_free(ctx);
        return NULL;
    }

    ctx->inwd = inotify_add_watch(ctx->infd, path, INOTIFY_MASK | IN_ONLYDIR);
    if (ctx->inwd == -1) {
        fprintf(stderr, "couldn't setup watchdir: %s", strerror(errno));
        watching_ctx_free(ctx);
        return NULL;
    }

    ctx->event = bufferevent_socket_new(evbase, ctx->infd, 0);
    if (ctx->event == NULL) {
        fprintf(stderr, "couldn't create event buffer: %s", strerror(errno));
        watching_ctx_free(ctx);
        return NULL;
    }

    bufferevent_setwatermark(ctx->event, EV_READ, sizeof(struct inotify_event), 0);
    bufferevent_setcb(ctx->event, &inotify_on_event, NULL, NULL, ctx);
    bufferevent_enable(ctx->event, EV_READ);

    callback();
    return ctx;
    #else
    callback();
    return NULL;
    #endif
}

void watching_ctx_free(watching_ctx_t *ctx) {
    if (ctx->event) {
        bufferevent_disable(ctx->event, EV_READ);
        bufferevent_free(ctx->event);
    }

    if (ctx->infd != -1) {
        #if __linux__
        if (ctx->inwd != -1) {
            inotify_rm_watch(ctx->infd, ctx->inwd);
        }
        #endif
        close(ctx->infd);
    }

    free(ctx);
}

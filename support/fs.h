#ifndef FS_H
#define FS_H

typedef struct bufferevent evbev_t;

typedef struct watching_ctx_s {
    evbev_t *event;
    void(*callback)(void);
    int infd;
    int inwd;
} watching_ctx_t;

watching_ctx_t *watch(char *path, evbase_t *evbase, void(*callback)(void));
void watching_ctx_free(watching_ctx_t *ctx);

#endif

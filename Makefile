openssl_flags = $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config openssl --cflags --libs)
flags = -levent -levent_openssl -levhtp $(openssl_flags)

all: clean main run

run:
	./main.out

main:
	lessc -x resources/main.less resources/main.css
	xxd -i resources/main.css resources/css.h
	clang main.c template.c articles.c config.c support/io.c support/ht.c support/html.c $(flags) -o main.out

clean:
	-rm main.out

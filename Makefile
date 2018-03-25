LIBS=-levent -levent_openssl -levhtp -lssl -lcrypto -L/usr/local/Cellar/openssl/1.0.2n/lib -I/usr/local/Cellar/openssl/1.0.2n/include

all: clean main run

run:
	./main.out

main:
	lessc -x resources/main.less resources/main.css
	xxd -i resources/main.css resources/css.h
	clang main.c template.c articles.c config.c support/io.c support/ht.c support/html.c $(LIBS) -o main.out

clean:
	-rm main.out

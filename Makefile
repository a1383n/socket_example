CC=gcc
CFLAGS=-Wall -Wextra

all: server

server: main.c tcp.c tcp.h
	$(CC) $(CFLAGS) -o server main.c tcp.c -lpthread

clean:
	rm -f server

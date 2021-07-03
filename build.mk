CFLAGS = -Wall -lpcre2-8

all:
	gcc $(CFLAGS) queue.c mh.c -o mh

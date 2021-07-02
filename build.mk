CFLAGS = -Wall -lpcre2-8

all:
	gcc $(CFLAGS) mh.c -o mh

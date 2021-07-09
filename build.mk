DEBUG = -g -fsanitize=address
CFLAGS = -Wall -lpcre2-8 #$(DEBUG)

all:
	gcc $(CFLAGS) queue.c mh.c -o mh

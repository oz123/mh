DEBUG = -g -fsanitize=address
CFLAGS = -Wall -lpcre2-8 #$(DEBUG)
OPTS = -D COLOROUTPUT
all:
	gcc $(CFLAGS) queue.c mh.c $(OPTS) -o mh

###
#  general configuration of the makefile itself
###
SHELL := /bin/bash
.DEFAULT_GOAL := help

.PHONY: help
help:
	@mh -f $(MAKEFILE_LIST) $(target)
ifndef target
	@echo ""
	@echo "Use \`make help target=foo\` to learn more about foo."
endif

DEBUG = -g -fsanitize=address
CFLAGS = -Wall -lpcre2-8 #$(DEBUG)
OPTS = -D COLOROUTPUT
PREFIX = /usr/local
BINDIR = /bin
PROGNAME = mh
VERSION = $(shell git describe --always)
OBJECTS = queue.c mh.c main.c
clean:
	rm -fv $(PROGNAME)

mh:  ## compile this software
	gcc -o $(PROGNAME) -D VERSION=\"$(VERSION)\" $(OBJECTS) $(CFLAGS) $(OPTS)

install:  ## install this software
	install -m 755 mh $(PREFIX)$(BINDIR)/$(PROGNAME)

uninstall: ## remove this software
	rm -iv $(PREFIX)$(BINDIR)/$(PROGNAME)

lint:
	cppcheck --std=c99 --enable=all .

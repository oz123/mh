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
CFLAGS ?= -Wall
LIBS ?= $(shell pkg-config --libs libpcre2-8)
OPTS ?= -D COLOROUTPUT
PREFIX ?= /usr/local
MANPATH = $(PREFIX)/share/man/man
SECTION = 1
BINDIR = /bin
PROGNAME = mh
VERSION = $(shell git describe --always)
CC ?= gcc
OBJECTS = queue.c mh.c main.c

clean:
	rm -fv $(PROGNAME)

mh:  ## compile this software
	$(CC) -o $(PROGNAME) -D VERSION=\"$(VERSION)\" $(OBJECTS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(OPTS)

install:  ## install this software
	install -D -m 755 $(PROGNAME) $(DESTDIR)$(PREFIX)$(BINDIR)/$(PROGNAME)
	install -D -m 755 $(PROGNAME).$(SECTION) $(DESTDIR)$(MANPATH)$(SECTION)/$(PROGNAME).$(SECTION)

uninstall: ## remove this software
	rm -iv $(PREFIX)$(BINDIR)/$(PROGNAME)
	rm -iv $(PREFIX)$(MANPATH)$(SECTION)/$(PROGNAME).$(SECTION)

lint: CHECKS ?= all #? which check to enable (e.g. warning, style, etc...)
lint:  ## static analysis with cppcheck
	cppcheck --std=c99 --enable=$(CHECKS) --inline-suppr .

deb-export-archive:
	git archive --format=tar  HEAD | xz > ../mh_$(shell echo $(VERSION) | sed "s/^v//; s/-[0-9a-g]*$$//").orig.tar.xz

deb-version:
	debchange -b -v $(shell echo $(VERSION) | sed "s/^v//") bump git version

deb-build:
	dpkg-buildpackage -us -uc


test::
	rm -f ./test
	$(CC) -o test $(CFLAGS) $(shell pkg-config -libs cmocka) $(LIBS) $(LDFLAGS) mh.c queue.c test.c -o test
	CMOCKA_MESSAGE_OUTPUT=SUBUNIT ./test

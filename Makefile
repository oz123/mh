###
#  general configuration of the makefile itself
###
SHELL := /bin/bash
.DEFAULT_GOAL := help

.PHONY: help
help:
	@mh -f $(MAKEFILE_LIST) $(target) || echo "Please install mh from https://github.com/oz123/mh/releases/latest"
ifndef target
	@(which mh > /dev/null 2>&1 && echo -e "\nUse \`make help target=foo\` to learn more about foo.")
endif

GLOBAL_K8S = foo #? has
PKG_CONFIG ?= pkg-config
DEBUG = -g #-fsanitize=address
CFLAGS ?= -Wall
LIBS ?= $(shell $(PKG_CONFIG) --libs libpcre2-8) -lbsd
OPTS ?= -D COLOROUTPUT
PREFIX ?= /usr/local
MANPATH = $(PREFIX)/share/man/man
SECTION = 1
BINDIR = /bin
PROGNAME = mh
VERSION = $(shell git describe --always)
CC ?= gcc
OBJECTS = queue.c mh.c main.c
STATIC_STUFFIX=$(shell uname -o | cut -d"/" -f2 | tr '[:upper:]' '[:lower:]')-$(shell uname -m)

clean:
	rm -fv $(PROGNAME)
clean-static:
	rm -fv $(PROGNAME)-$(STATIC_STUFFIX)

mh:  ## compile this software
	$(CC) $(CPPFLAGS) -o $(PROGNAME) -D VERSION=\"$(VERSION)\" $(OBJECTS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(OPTS)

mh-debug:
	$(CC) $(CPPFLAGS) $(DEBUG) -o $(PROGNAME) -D VERSION=\"$(VERSION)\" $(OBJECTS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(OPTS)

mh-static:  ## compile this software for a static build
	$(CC) $(CPPFLAGS) -static -o $(PROGNAME)-$(STATIC_STUFFIX) -D VERSION=\"$(VERSION)\" $(OBJECTS) $(CFLAGS) $(LIBS) $(LDFLAGS) $(OPTS)

install-static:  ## install this software
	install -D -m 755 $(PROGNAME)-$(STATIC_STUFFIX) $(DESTDIR)$(PREFIX)$(BINDIR)/$(PROGNAME)
	install -D -m 755 $(PROGNAME).$(SECTION) $(DESTDIR)$(MANPATH)$(SECTION)/$(PROGNAME).$(SECTION)

install:  ## install this software
	install -D -m 755 $(PROGNAME) $(DESTDIR)$(PREFIX)$(BINDIR)/$(PROGNAME)
	install -D -m 755 $(PROGNAME).$(SECTION) $(DESTDIR)$(MANPATH)$(SECTION)/$(PROGNAME).$(SECTION)

uninstall: ## remove this software
	-rm -v $(PREFIX)$(BINDIR)/$(PROGNAME)
	-rm -v $(MANPATH)/$(SECTION)/$(PROGNAME).$(SECTION)

lint: CHECKS ?= all #? which check to enable (e.g. warning, style, etc...)
lint: OPTS ?= #? for example: --force
lint: FILES ?= . #? which files to check
lint:  ## static analysis with cppcheck
	cppcheck --std=c99 $(OPTS) --enable=$(CHECKS) --inline-suppr -I /usr/include/ -I /usr/include/linux/ -I /usr/lib/clang/17/include/ $(FILES)

deb-export-archive:
	git archive --format=tar  HEAD | xz > ../mh_$(shell echo $(VERSION) | sed "s/^v//; s/-[0-9a-g]*$$//").orig.tar.xz

deb-install-deps:
	apt install -y libpcre2-dev libbsd-dev libcmocka-dev debhelper devscripts

brew-install-deps:
	brew update
	brew install pcre2 cmocka

deb-version:
	debchange -b -v $(shell echo $(VERSION) | sed "s/^v//") bump git version

deb-build: deb-export-archive
	dpkg-buildpackage -us -uc

test::
	rm -f ./test
	$(CC) $(CPPFLAGS) -o test mh.c queue.c test.c $(CFLAGS) $(shell $(PKG_CONFIG) --libs cmocka) $(LIBS) $(LDFLAGS) 
	CMOCKA_MESSAGE_OUTPUT=SUBUNIT ./test

checksum.txt: mh-$(STATIC_STUFFIX)
	sha512sum mh-linux-x86_64 > checksum.txt


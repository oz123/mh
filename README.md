# mh

## make help

A small program you put in your Makefile to produce fancy help

Building modern software requires a lot of tools. I can't and I don't want
to memorize or type all these commands over and over again (docker, npm,
terraform, ansible, scss, etc). What ever your stack looks like, ``Makefile``
and the ``make`` command help you avoid unnecessary typing and save time.
All too often, I forget which targets are available in the `Makefile`s of
different project. Also, as time goes on, these Makefiles can become gigantic,
and getting a quick overview by reading the `Makefile` becomes impossible.
The purpose of this program is to fix this situatuion by turning a `Makefile`
to self-documenting, by using a simple syntax for documenting target, variable
and global make option.
This is much better than maintaining a help target manually for larger projects.
This program parses all the lines and checks for target, local variables
and global variables. If they are suffixed with `##` or `#?` the string that
follows is interpreted as the help for that variable or target.

The targets can be used with parameters like this:

```
make docker-build IMG=foo TAG=0.1
```

Or:

```
make test COLOR=red HOST=red.example.org
```

(As a side note: If you have a CI\CD pipeline a `Makefile` helps abstracting all
the stages and allows you to run all the tests, build and deploy phases locally).

## Installation

Install this package in your path with:
```
$ git clone https://github.com/oz123/mh
$ cd mh
$ make mh
$ sudo make install
```

## Usage

Add PHONY target to your `Makefile` called `help` and set it to be the default:
```
.DEFAULT_GOAL := help

.PHONY: help

help:
	@mh < $(MAKEFILE_LIST)

```

You can then document the `Makefile` targets with `##` after each target,
or `#?` after a variable
*Note*: the help text for for local variables should not have be prefixed
with spaces before `#?`.
Here is how you document targes and varibles:

```
AUTHOR ?= Oz#? The program author

build: PROGNAME ?= poof#? The binary name to output
build: ## build the project
    gcc -Wall -D AUTHOR=$(AUTHOR) project.c -o $(PROGNAME)
```

For example, the `Makefile` in this repository will print the following,
when no target is given or when `make help` is called:

```
Targets:

bbbbb/color		test that the HOST shows background COLOR

	Options:

	HOST: what host is running NGinx (default: localhost)
	COLOR: which color should be the backround (default: green)

some/people/organize/their/targets/like/directories		you know just because you can

ffff-color		test that the HOST shows background COLOR

	Options:

	HOST: what host is running NGinx (default: localhost)
	COLOR: which color should be the backround (default: green)

test_color		test that the HOST shows background COLOR

	Options:

	HOST: what host is running NGinx (default: localhost)
	COLOR: which color should be the backround (default: green)

docker-build		build a docker image

docker-push		push docker image to $(REGISTRY)

mh		compile this software

install		install this software

Global options you can override:

VERSION:	 version (default: $(shell git describe --always))
REGISTRY:	 container registry to push (default: docker.io)
IMG:	 image name (default: $(shell basename $(CURDIR)))
OPTS:	 add extra OPTS to misc commands
ORG:	 organization to push (default: oz123)
```

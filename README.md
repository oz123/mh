# mh

A small program you put in your Makefile to produce fancy help

Building modern software requires a lot of tools. I can't and I don't want
to memorize or type all these commands over and over again (docker, npm,
terraform, ansible, scss, etc). What ever your stack looks like, ``Makefile``
and the ``make`` command help you avoid unnecessary typing and save time.

If you have a CI\CD pipeline this also help abstract all the stages and allows
you to run all the test, build and deploy phases locally.

Install this package in your path with:
```
$ git clone https://github.com/oz123/mh
$ cd mh
$ make mh
$ sudo make install
```

Add PHONY target to your `Makefile` called `help` and set it to be the default:
```
.DEFAULT_GOAL := help

.PHONY: help

help:
	@mh < $(MAKEFILE_LIST)

```

You can then document the `Makefile` targets with `##` after each target,
or `#?` after a variable.

For example the `Makefile` in this repo will print the following when no
target is given or when `make help` is called:

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

This make file parses all the lines and checks for target, local variables
and global variables.
If they are suffixed with `##` or `#?` the string that follows is interpreted
as the help for that variable or target.
This is much better then maintaining a help target manually for larger
projects.

The targets can be used with parameters like this:

```
make docker-build IMG=foo TAG=0.1
```

Or:

```
make test COLOR=red HOST=red.example.org
```

See this project's `Makefile` for examples demonstrating
how to define and document global vars, local vars and targets.

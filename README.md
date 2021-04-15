# help-make-helper

A small python program you put in your Makefile to produce fancy help

Just type `make` after cloning this repo! It will print:

```
$ make
Targets:

test                 test that the HOST shows background COLOR

  HOST - what host is running NGinx
  COLOR - which color should be the backround

docker-build         build a docker image


docker-push          push docker image to $(REGISTRY)


Global Variables you can override:

VERSION              version
REGISTRY             container registry to push
ORG                  organization to push to
IMG                  image name
OPTS                 add extra OPTS to misc commands
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

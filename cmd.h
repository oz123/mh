/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021, Oz N Tiram <oz.tiram@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Emile "iMil" Heitor <imil@NetBSD.org> .
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_VERSION 1
#define CMD_HELP 2
#define CMD_FILE 3

# define PROGNAME "mh"
# define VERSION "0.1"
# define AUTHOR "Oz Tiram <oz.tiram __At__ gmail.com>"

static struct command {
	const char	*name;
	const char	*shortcut;
	const char	*descr;
	const int	cmdtype;
} cmd[] = {
	{ "--version", "-v", "Show the program version", CMD_VERSION},
    { "--help", "-h", "Show this help", CMD_HELP},
    { "--file", "-f", "Makefile to read (default: Makefile if -f isn't given)", CMD_FILE},
	{ NULL, NULL, NULL, -1}
};

/* find command index */
static int
find_cmd(const char *arg)
{
	int i;

	for (i = 0; cmd[i].name != NULL; i++) {
		if (strcmp(arg, cmd[i].name) == 0 || strcmp(arg, cmd[i].shortcut) == 0)
			return cmd[i].cmdtype;
	}

	return -1;
}


void usage(struct command *cmd) {
    fprintf(stderr, PROGNAME " [ --help | --version | --file <Makefile> ] [target]\n");
    fprintf(stderr, PROGNAME " parses a file via stdin:\n\n");
    fprintf(stderr, "\t$ cat Makefile | "PROGNAME"\n\n");
    fprintf(stderr, "or inside a Makefile target:\n\n");
    fprintf(stderr, "\t@"PROGNAME " <$(MAKEFILE_LIST)\n");
    fprintf(stderr, "\nOptions:\n");
	for (int i = 0; cmd[i].name != NULL; i++) {
        fprintf(stderr, "  %s, %s\t\t\t%s\n", cmd[i].shortcut, cmd[i].name, cmd[i].descr);
	}
}

void
parse_args(int argc, char *argv[], char **filename, char **lookup) {
    if (argc > 1) {
        int ch = find_cmd(argv[1]);
        switch (ch) {
            case CMD_HELP:
                usage(cmd);
                exit(1);
            case CMD_VERSION:
                fprintf(stderr, "%s %s (c) %s\n", PROGNAME, VERSION, AUTHOR);
                exit(0);
            case CMD_FILE:
                if (argc < 3) {
                    usage(cmd);
                    exit(1);
                }
                *filename = argv[2];
                break;
       }
    }
    if (!*filename) {
        *filename = "Makefile";
        if (argc > 1) {
            *lookup = argv[argc - 1];
        }
    } else {
        if (argc > 3) {
            *lookup = argv[3];
        }
    }
}

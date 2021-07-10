/*
BSD 2-Clause License

Copyright (c) 2021, Oz N Tiram <oz.tiram@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define  _POSIX_C_SOURCE 200809L
#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "queue.h"
#include "mh.h"

static char words[NUMBER_OF_STRINGS][MAX_STRING_SIZE] = {"foo", "bar", "baz", "ping", "pong"};

int
main(int argc, char *argv[])
{
    char *lookup = NULL;
    if (argc > 1) {
        if(!strcmp("--version", argv[1])){
            fprintf(stderr, "%s %s (c) %s\n", PROGNAME, VERSION, AUTHOR);
            exit(0);
        }

        else if (!strcmp("--help", argv[1])){
            usage();
            exit(1);
        }

        else {
            lookup = argv[1];
        }
    }
    char *line = NULL;
    size_t len = 0;
    size_t line_length;

    Queue* targets = queue_new();
    Queue* globals = queue_new();

    target_t *target = new_target();

    pcre2_code *regex_target, *regex_local, *regex_global;
    init_pcre_regex(&regex_target, &regex_global, &regex_local);

    while ((line_length = getline(&line, &len, stdin)) != -1) {
        variable_t *variable = new_variable();
        if (!check_line_for_global_var(line, variable, regex_global)) {
            queue_push_tail(globals, variable);
            continue;
        }

        if (!check_line_for_local_var(line, variable, regex_local)) {
            queue_push_tail(target->locals, variable);
            if (queue_is_empty(target->locals) != 0) {
                queue_push_tail(target->locals, variable);
            }
            continue;
        }
        if (!check_line_for_target(line, target, regex_target)) {
            target_t *copy = copy_target(target);
            queue_push_tail(targets, copy);
        }
        /* target or variable were not pushed to queue, hence they can be released */
        free_variable(variable);
    }
    variable_t *lv = new_variable();
    if (lookup != NULL) {
        while (!queue_is_empty(targets)) {
            target = queue_pop_head(targets);
            if (!strcmp(target->name, lookup)) {
                printf(CYN UNDR "Help for target:" RESET " %s\n\n", lookup);
                printf("%s\n\n", target->help);
                if (!queue_is_empty(target->locals)) {
                    char *usage = (char *)malloc(5*sizeof(char));
                    usage = (char *)realloc(usage, strlen(target->name) + 13);
                    sprintf(usage, "Usage: make %s", target->name);
                    printf("Options:\n");
                    while (!queue_is_empty(target->locals)) {
                        lv = queue_pop_head(target->locals);
                        printf("\t%s: %s (default: %s)\n\n", lv->name, lv->help, lv->default_value);
                        usage = (char *)realloc(usage, strlen(usage) + strlen(lv->name) + 1);
                        char *r = words[rand() % NUMBER_OF_STRINGS];
                        char *fragment = (char *)malloc(strlen(lv->name) + 1 + strlen(r) + 1);
                        sprintf(fragment, " %s=%s", lv->name, r);
                        strcat(usage, fragment);
                    }
                    printf("%s\n", usage);
                }
            }
        }
        exit(0);
    }

    printf(CYN UNDR "Targets:\n" RESET);
    while (!queue_is_empty(targets)) {
        target = queue_pop_head(targets);
        printf("\n" CYN "%s" RESET "\t\t%s\n", target->name, target->help);
        if (!queue_is_empty(target->locals)) {
            printf("\n\tOptions:\n\n");
        }
        while (!queue_is_empty(target->locals)) {
            lv = queue_pop_head(target->locals);
            printf("\t%s: %s (default: %s)\n", lv->name, lv->help, lv->default_value);
        }
    }
 
    if (!queue_is_empty(globals)) {
        variable_t *gv = new_variable();
        printf(CYN UNDR "\nGlobal options you can override:\n\n" RESET);
        while (!queue_is_empty(globals)) {
            gv = queue_pop_head(globals);
            printf("%s:\t%s", gv->name, gv->help);
            if (strlen(gv->default_value) != 0) {
                printf(" (default: %s)\n", gv->default_value);
            } else {
                printf("\n");
            }
        }
        free_variable(gv);
    }

    free_variable(lv);
    pcre2_code_free(regex_target);
    pcre2_code_free(regex_local);
    pcre2_code_free(regex_global);
    free_target(target);
    queue_free(targets);
    queue_free(globals);
    free(line);
    exit(EXIT_SUCCESS);
}

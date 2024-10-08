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
#define _DEFAULT_SOURCE
#define  _POSIX_C_SOURCE 200809L
#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pcre2.h>
#include "mh.h"



static char words[NUMBER_OF_STRINGS][MAX_STRING_SIZE] = {"foo", "bar", "baz", "ping", "pong"};

target_t *new_target(void) {
    target_t *target = (target_t *) malloc(sizeof(target_t));
    target->name = NULL;
    target->help = NULL;
    target->locals = queue_new();
    return target;
}

target_t *copy_target(target_t *target) {
    target_t *copy = new_target();

    copy->name = (char*)calloc(strlen(target->name)+1, sizeof(char));
    copy->help = (char*)calloc(strlen(target->help)+1, sizeof(char));

    snprintf(copy->name, strlen(target->name)+1, "%s", target->name);
    snprintf(copy->help, strlen(target->help)+1, "%s", target->help);
    while (!queue_is_empty(target->locals)) {
        queue_push_tail(copy->locals, queue_pop_head(target->locals));
    }
    return copy;
}

void free_target(target_t *target) {
    free(target->name);
    free(target->help);
    queue_free(target->locals);
}

variable_t *new_variable(void) {
    variable_t *variable = (variable_t *) malloc(sizeof(variable_t));
    variable->name = NULL;
    variable->default_value = NULL;
    variable->help = NULL;
    return variable;
}

void free_variable(variable_t *variable){
    free(variable->name);
    free(variable->default_value);
    free(variable->help);
}


int check_line_for_global_var(char *line, variable_t *variable, pcre2_code *regex) {
    return check_line_for_regex(line, NULL, variable, regex);
}

int check_line_for_local_var(char *line, variable_t *variable, pcre2_code *regex) {
    return check_line_for_regex(line, NULL, variable, regex);
}

int check_line_for_env_var(char *line, variable_t *variable, pcre2_code *regex) {
    return check_line_for_regex(line, NULL, variable, regex);
}

int check_line_for_target(char *line, target_t *target, pcre2_code *regex) {
    return check_line_for_regex(line, target, NULL, regex);
}

// cppcheck-suppress constParameterPointer
int check_line_for_regex(char *line, target_t *target, variable_t *variable, pcre2_code *regex) {

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regex, NULL);

    int rc = pcre2_match(
        regex,                /* the compiled pattern */
        (PCRE2_SPTR8)line,    /* the subject string */
        strlen(line),         /* the length of the subject */
        0,                    /* start at offset 0 in the subject */
        0,                    /* default options */
        match_data,           /* block for storing the result */
        NULL);                /* use default match context */

     if (rc < 0) {
         return 1;
     }

     PCRE2_UCHAR *substr_buf = NULL;
     PCRE2_SIZE substr_buf_len = 0;

     int copyname_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"name", &substr_buf, &substr_buf_len);
     if (copyname_rc == 0) {
         if (target != NULL) {
             target->name = calloc(substr_buf_len, sizeof(PCRE2_UCHAR));
             memcpy(target->name, substr_buf, substr_buf_len); // NOLINT
	     //printf("DEBUG: target->name is %s\n", target->name);
         } else if (variable != NULL) {
             variable->name = calloc(substr_buf_len, sizeof(PCRE2_UCHAR));
             memcpy(variable->name, substr_buf, substr_buf_len); // NOLINT
         }
     }

     int copyhelp_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"help", &substr_buf, &substr_buf_len);
     if (copyhelp_rc == 0) {
	 //printf("DEBUG: help is %s\n", substr_buf);
         if (target != NULL) {
             target->help = calloc(substr_buf_len, sizeof(char));
             memcpy(target->help, substr_buf, substr_buf_len);
	     //printf("DEBUG: target->help is %s\n", target->help);
	 } else if (variable != NULL) {
            variable->help = calloc(substr_buf_len, sizeof(PCRE2_UCHAR));
            memcpy(variable->help, substr_buf, substr_buf_len);
	    //printf("DEBUG: variable->help is %s\n", variable->help);
         }
     }
     
     if (variable != NULL) {
         int copydefault_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"default", &substr_buf, &substr_buf_len);
         if (copydefault_rc == 0) {
             variable->default_value = calloc(substr_buf_len, sizeof(PCRE2_UCHAR));
             memcpy(variable->default_value, substr_buf, substr_buf_len);
         }
     }
     pcre2_match_data_free(match_data);   /* Release memory used for the match */
     return 0;
}

pcre2_code *compile_regex(char *regex){

    int errornumber;
    PCRE2_SIZE erroroffset;
    pcre2_code *reg;

    reg = pcre2_compile((PCRE2_SPTR)regex,
                        PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
                        0,                     /* default options */
                        &errornumber,          /* for error number */
                        &erroroffset,          /* for error offset */
                        NULL);                 /* use default compile context */

    /* Compilation failed: print the error message and exit. */
    if (reg == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
        exit(1);
    }

    return reg;
}

void init_pcre_regex(pcre2_code **a, pcre2_code **b, pcre2_code **c){
    *a = compile_regex(REGEX_HELP_TARGET);
    *b = compile_regex(REGEX_GLOBAL_VAR);
    *c = compile_regex(REGEX_LOCAL_VAR);
}

void show_all_help(Queue *targets, Queue *globals, Queue *envvars) {

    printf(CYN UNDR "Targets:\n" RESET);
    while (!queue_is_empty(targets)) {
        target_t *target = queue_pop_head(targets);
        printf("\n" CYN "%s" RESET "\t\t%s\n", target->name, target->help);
        free_target(target);
    }

    if (!queue_is_empty(globals)) {
        printf(CYN UNDR "\nGlobal vars in Makefile:\n\n" RESET);
        while (!queue_is_empty(globals)) {
            variable_t *gv = queue_pop_head(globals);
            printf("%s:\t%s", gv->name, gv->help);
            if (strlen(gv->default_value) != 0) {
                printf(" (default: %s)\n", gv->default_value);
            } else {
                printf("\n");
            }
            free_variable(gv);
        }
    }

    if (!queue_is_empty(envvars)) {
        printf(CYN UNDR "\nGlobal vars in .env:\n\n" RESET);
        while (!queue_is_empty(envvars)) {
            variable_t *ev = queue_pop_head(envvars);
            printf("%s:\t%s\n", ev->name, ev->help);
            free_variable(ev);
	}
    }
}

int show_target_help(char *targetname, Queue *targets) { // cppcheck-suppress constParameterPointer

    while (!queue_is_empty(targets)) {
        target_t *target = queue_pop_head(targets);

        if (!strcmp(target->name, targetname)) {
            printf(CYN UNDR "Help for target:" RESET " %s\n\n", targetname);
            printf("%s\n\n", target->help);

            if (!queue_is_empty(target->locals)) {
                char *usage = (char *)malloc(5 + strlen(target->name) + 13);
                char *_usage;
                sprintf(usage, "Usage: make %s", target->name);
                printf("Options:\n");

                while (!queue_is_empty(target->locals)) {
                    const variable_t *lv = queue_pop_head(target->locals);
                    printf("\t%s: %s (default: %s)\n\n", lv->name, lv->help, lv->default_value);
                    _usage = (char *)realloc(usage, strlen(usage) + strlen(lv->name) + 1);

                    if (_usage) {
                        usage = _usage;
                    }
		    // show an example of usage for a target e.g.
		    // make targetname VAR1=foo VAR2=bar
                    const char *r = words[rand() % NUMBER_OF_STRINGS];
                    char *fragment = (char *)malloc(strlen(lv->name) + 1 + strlen(r) + 1);
                    sprintf(fragment, " %s=%s", lv->name, r);
                    strcat(usage, fragment);
                    free(fragment);
                }
                printf("%s\n", usage);
                free(usage);
            }
            return 0;
        }
    }
    fprintf(stderr, "No such traget: %s\n", targetname);
    return 1;
}

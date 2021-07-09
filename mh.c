#define  _POSIX_C_SOURCE 200809L
#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "mc.h"

int check_line_for_global_var(char *line, variable_t *variable, pcre2_code *regex) {
    return check_line_for_regex(line, NULL, variable, regex, 0);
}

int check_line_for_local_var(char *line, variable_t *variable, pcre2_code *regex) {
    return check_line_for_regex(line, NULL, variable, regex, 0);
}

int check_line_for_target(char *line, target_t *target, pcre2_code *regex) {
    return check_line_for_regex(line, target, NULL, regex, 0);
}

int check_line_for_regex(char *line, target_t *target, variable_t *variable, pcre2_code *regex, int local) {

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regex, NULL);

    int rc = pcre2_match(
        regex,                   /* the compiled pattern */
        (PCRE2_SPTR8)line,    /* the subject string */
        strlen(line),          /* the length of the subject */
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
            target->name = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
             memcpy(target->name, substr_buf, substr_buf_len);
         } else if (variable != NULL) {
            variable->name = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
            memcpy(variable->name, substr_buf, substr_buf_len);
         }
     }

     int copyhelp_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"help", &substr_buf, &substr_buf_len);
     if (copyhelp_rc == 0) {
         if (target != NULL) {
             target->help = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
             memcpy(target->help, substr_buf, substr_buf_len);
         } else if (variable != NULL) {
            variable->help = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
            memcpy(variable->help, substr_buf, substr_buf_len);
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

int
main(int argc, char *argv[])
{
    char *line = NULL;
    size_t len = 0;
    size_t line_length;

    Queue* targets = queue_new();
    Queue* globals = queue_new();

    target_t *target = new_target();

    pcre2_code *regex_target, *regex_local, *regex_global;
    init_pcre_regex(&regex_target, &regex_global, &regex_local);
    regex_global = compile_regex(REGEX_GLOBAL_VAR);
    regex_local = compile_regex(REGEX_LOCAL_VAR);

    while ((line_length = getline(&line, &len, stdin)) != -1) {
        variable_t *variable = new_variable();
        if (check_line_for_global_var(line, variable, regex_global) == 0) {
            queue_push_tail(globals, variable);
            continue;
        }

	    if (check_line_for_local_var(line, variable, regex_local) == 0) {
	        queue_push_tail(target->locals, variable);
            if (queue_is_empty(target->locals) != 0) {
                queue_push_tail(target->locals, variable);
            }
            continue;
	    }
        if (check_line_for_target(line, target, regex_target) == 0) {
            printf("Retrieved target: %s %s\n", target->name, target->help);
            target_t *copy = copy_target(target);
            queue_push_tail(targets, copy);
        }
        /* target or variable were not pushed to queue, hence they can be released */
        free_variable(variable);
    }

    while (!queue_is_empty(targets)) {
        target = queue_pop_head(targets);
        printf("I have target %p %s %s\n", target, target->name, target->help);
    }
 
    variable_t *gv = new_variable();
    while (!queue_is_empty(globals)) {
        gv = queue_pop_head(globals);
        printf("I have var %p %s %s\n", gv, gv->name, gv->help);
    }

    pcre2_code_free(regex_target);
    pcre2_code_free(regex_local);
    pcre2_code_free(regex_global);
    free_target(target);
    queue_free(targets);
    queue_free(globals);
    free(line);
    exit(EXIT_SUCCESS);
}

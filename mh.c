#define  _POSIX_C_SOURCE 200809L
#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "mc.h"


int check_line_for_global_var(char *line, variable_t *variable) {
    int errornumber;
    char *target_help = REGEX_GLOBAL_VAR;
    PCRE2_SIZE erroroffset;
    PCRE2_SPTR pattern = (PCRE2_SPTR)target_help;
    pcre2_code *re;

    re = pcre2_compile(
                       pattern,
                       PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
                       0,                     /* default options */
                       &errornumber,          /* for error number */
                       &erroroffset,          /* for error offset */
                       NULL);                 /* use default compile context */

    /* Compilation failed: print the error message and exit. */
    if (re == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
        return 1;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);

    int rc = pcre2_match(
        re,                   /* the compiled pattern */
        (PCRE2_SPTR8)line,    /* the subject string */
        strlen(line),          /* the length of the subject */
        0,                    /* start at offset 0 in the subject */
        0,                    /* default options */
        match_data,           /* block for storing the result */
        NULL);                /* use default match context */

     if (rc < 0) {
         switch(rc) {
             case PCRE2_ERROR_NOMATCH:
                 printf("No match\n");
		 return 1;
             }
     }


     PCRE2_UCHAR *substr_buf = NULL;
     PCRE2_SIZE substr_buf_len = 0;

     int copyname_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"name", &substr_buf, &substr_buf_len);
     if (copyname_rc == 0) {
         variable->name = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
         memcpy(variable->name, substr_buf, substr_buf_len);
     }


     int copyhelp_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"help", &substr_buf, &substr_buf_len);
     if (copyhelp_rc == 0) {
         variable->help = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
         memcpy(variable->help, substr_buf, substr_buf_len);
     }

     pcre2_match_data_free(match_data);   /* Release memory used for the match */
     pcre2_code_free(re);                 /* data and the compiled pattern. */
     return 0;
}

int check_line_for_local_var(char *line, variable_t *variable) {
    return 1;
}

int check_line_for_target(char *line, target_t *target) {

    int errornumber;
    char *target_help = REGEX_HELP_TARGET;
    PCRE2_SIZE erroroffset;
    PCRE2_SPTR pattern = (PCRE2_SPTR)target_help;
    pcre2_code *re;

    re = pcre2_compile(
                       pattern,
                       PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
                       0,                     /* default options */
                       &errornumber,          /* for error number */
                       &erroroffset,          /* for error offset */
                       NULL);                 /* use default compile context */

    /* Compilation failed: print the error message and exit. */
    if (re == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
        printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset, buffer);
        return 1;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);

    int rc = pcre2_match(
        re,                   /* the compiled pattern */
        (PCRE2_SPTR8)line,    /* the subject string */
        strlen(line),          /* the length of the subject */
        0,                    /* start at offset 0 in the subject */
        0,                    /* default options */
        match_data,           /* block for storing the result */
        NULL);                /* use default match context */

     if (rc < 0) {
         switch(rc) {
             case PCRE2_ERROR_NOMATCH:
                 printf("GV: No match\n");
		 return 1;
             }
     }


     PCRE2_UCHAR *substr_buf = NULL;
     PCRE2_SIZE substr_buf_len = 0;

     int copyname_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"name", &substr_buf, &substr_buf_len);
     if (copyname_rc == 0) {
         target->name = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
         memcpy(target->name, substr_buf, substr_buf_len);
     }


     int copyhelp_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"help", &substr_buf, &substr_buf_len);
     if (copyhelp_rc == 0) {
         target->help = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
         memcpy(target->help, substr_buf, substr_buf_len);
     }

     pcre2_match_data_free(match_data);   /* Release memory used for the match */
     pcre2_code_free(re);                 /* data and the compiled pattern. */
     return 0;
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

    while ((line_length = getline(&line, &len, stdin)) != -1) {
        variable_t *variable = new_variable();
        if (check_line_for_global_var(line, variable) == 0) {
            queue_push_tail(globals, variable);
            continue;
        }

	    if (check_line_for_local_var(line, variable) == 0) {
	        queue_push_tail(target->locals, variable);
            if (queue_is_empty(target->locals) != 0) {
                queue_push_tail(target->locals, variable);
            }
            continue;
	    }
        if (check_line_for_target(line, target) == 0) {
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

    queue_free(targets);
    queue_free(globals);
    free(line);
    exit(EXIT_SUCCESS);
}

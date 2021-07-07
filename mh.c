#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "mc.h"


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

   pcre2_match(
       re,                   /* the compiled pattern */
       (PCRE2_SPTR8)line,    /* the subject string */
       strlen(line),          /* the length of the subject */
       0,                    /* start at offset 0 in the subject */
       0,                    /* default options */
       match_data,           /* block for storing the result */
       NULL);                /* use default match context */

    PCRE2_UCHAR *substr_buf = NULL;
    PCRE2_SIZE substr_buf_len = 0;

    int copybyname_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"name", &substr_buf, &substr_buf_len);
    if (copybyname_rc == 0) {
        printf("name is: %d %s %d\n", copybyname_rc, substr_buf, (int)substr_buf_len);
    }

    target->name = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
    memcpy(target->name, substr_buf, substr_buf_len);

    copybyname_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"help", &substr_buf, &substr_buf_len);
    if (copybyname_rc == 0) {
        printf("help is: %d %s %d\n", copybyname_rc, substr_buf, (int)substr_buf_len);
    }

    target->help = malloc(substr_buf_len * sizeof(PCRE2_UCHAR));
    memcpy(target->help, substr_buf, substr_buf_len);
    return 0;
}

/* pseudo code:
 *
target = NULL;
for line in lines:
    if match_global(line, global_var):
       push_global_var(globals, global_var)
       global_var = NULL;
       continue
    if match_local(line, local_var):
       push_local_to_target(local_var, target)
       local_var = NULL;
       continue
    if match_target(line, target)
       push_target(target, targets)
       target = NULL;
*/
int
main(int argc, char *argv[])
{
    char *target_help = REGEX_HELP_TARGET;
    char *line = NULL;
    size_t len = 0;
    size_t line_length;

    PCRE2_SPTR pattern = (PCRE2_SPTR)target_help;
    pcre2_code *re;

    int errornumber;

    PCRE2_SIZE erroroffset;
    PCRE2_SIZE *ovector;
 
    Queue* targets = queue_new();
    Queue* globals = queue_new();

    
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
   
   while ((line_length = getline(&line, &len, stdin)) != -1) {
       target_t *c_target = (target_t *) malloc(sizeof(target_t));
       int ccc = check_line_for_target(line, c_target);
       printf("Retrieved target: %s %s\n", c_target->name, c_target->help);

    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(re);                 /* data and the compiled pattern. */

    queue_free(targets);
    free(c_target);
    free(line);
    exit(EXIT_SUCCESS);

    }
}
/* vim: set tabstop=4 softtabstop=4 shiftwidth=4 expandtab */

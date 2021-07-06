#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "mc.h"

# define REGEX_HELP_TARGET "^(?P<name>[a-zA-Z_-]+):.*?## (?P<help>.*)$"

target_t* check_line_for_target(char *line) {

    printf("Retrieved line %s:\n", line);
    target_t *a_target = (target_t *) malloc(sizeof(target_t));
    a_target->name = "Yousef";
    return a_target;
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
       target_t *b_target = check_line_for_target(line);
       printf("Retrieved target: %s\n", b_target->name);
       int rc = pcre2_match(
           re,                   /* the compiled pattern */
           (PCRE2_SPTR8)line,    /* the subject string */
           line_length,          /* the length of the subject */
           0,                    /* start at offset 0 in the subject */
           0,                    /* default options */
           match_data,           /* block for storing the result */
           NULL);                /* use default match context */

       //if (rc < 0) {
       //   switch(rc) {
       //      case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
       //      /* Handle other special cases if you like */
       //      default: printf("Matching error %d\n", rc); break;
       //      }
       //}

       ovector = pcre2_get_ovector_pointer(match_data);

       PCRE2_UCHAR *substr_buf = NULL;
       PCRE2_SIZE substr_buf_len = 0;
       
       int copybyname_rc = pcre2_substring_get_byname(match_data, (PCRE2_SPTR)"name", &substr_buf, &substr_buf_len);

       if (copybyname_rc == 0) {
           printf("meh is: %d %s %d\n", copybyname_rc, substr_buf, (int)substr_buf_len);
       }
       char * target_start;
       size_t target_length = 0;
       char * help_start;
       size_t help_length = 0;

       target_t *a_target = (target_t *) malloc(sizeof(target_t));
       if (rc == 3) {

           target_start = line + ovector[2];
           target_length = ovector[3] - ovector[2];
           a_target->name = malloc(target_length + 1);
           strncpy(a_target->name, target_start, target_length);
           a_target->name[target_length] = '\0';

           help_start = line + ovector[4];
           help_length = ovector[5] - ovector[4];
           a_target->help = malloc(help_length + 1);
           strncpy(a_target->help, help_start, help_length);
           a_target->help[help_length] = '\0';

           queue_push_tail(targets, a_target);
       }
    }

    while (queue_is_empty(targets) == 0) {
	target_t *i = queue_pop_head(targets);
        printf("target %s, help %s\n", i->name, i->help);
    }

    pcre2_match_data_free(match_data);   /* Release memory used for the match */
    pcre2_code_free(re);                 /* data and the compiled pattern. */

    queue_free(targets);
    free(line);
    exit(EXIT_SUCCESS);
}

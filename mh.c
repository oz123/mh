#define _GNU_SOURCE
#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "queue.h"

int
main(int argc, char *argv[])
{
    char *target_help = "^([a-zA-Z_-]+):.*?## (.*)$$";
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

    typedef struct target {
      char *name;
      char *help;
      Queue *locals;
    } target_t;
    
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
       printf("Retrieved line of length %zd:\n", line_length);
       // fwrite(line, nread, 1, stdout);
       int rc = pcre2_match(
           re,                   /* the compiled pattern */
           (PCRE2_SPTR8)line,    /* the subject string */
           line_length,          /* the length of the subject */
           0,                    /* start at offset 0 in the subject */
           0,                    /* default options */
           match_data,           /* block for storing the result */
           NULL);                /* use default match context */

       if (rc < 0) {
          switch(rc) {
             case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
             /* Handle other special cases if you like */
             default: printf("Matching error %d\n", rc); break;
             }
       }

       printf("rc %d\n", rc);
       ovector = pcre2_get_ovector_pointer(match_data);
       printf("\nMatch succeeded at offset %d\n", (int)ovector[0]);
       const char *match;
       int rc2 = 0;
       PCRE2_SPTR substring_start = 0;
       size_t substring_length = 0;
       target_t *a_target = (target_t *) malloc(sizeof(target_t));
       for (int i = 0; i < rc; i++) {
           substring_start = line + ovector[2*i];
           substring_length = ovector[2*i+1] - ovector[2*i];
           a_target->name = malloc(substring_length + 1);
           strncpy(a_target->name, substring_start, substring_length);
           a_target->name[substring_length] = '\0';
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

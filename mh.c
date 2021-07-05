#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "queue.h"

# define REGEX_HELP_TARGET "^([a-zA-Z_-]+):.*?## (.*)$"

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
       // printf("Retrieved line of length %zd:\n", line_length);
       // fwrite(line, nread, 1, stdout);
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

       //printf("rc %d\n", rc);
       ovector = pcre2_get_ovector_pointer(match_data);
       //printf("\nMatch succeeded at offset %d\n", (int)ovector[0]);
       //printf("sizeof ovector %d", pcre2_get_ovector_count(match_data));
       //for (int i = 0; i < pcre2_get_ovector_count(match_data)*2; i++){
       //   printf("\novector i %d\n", (int)ovector[i]);
       //}

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

#define _GNU_SOURCE
#define PCRE2_CODE_UNIT_WIDTH 8
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pcre2.h>
#include "queue.h"

int
main(int argc, char *argv[])
{
    char *target_help = "^([a-zA-Z_-]+):.*?## (.*)$$";
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    PCRE2_SPTR pattern = (PCRE2_SPTR)target_help;
    pcre2_code *re;
    PCRE2_SPTR subject;
    int errornumber;

    PCRE2_SIZE erroroffset;
    Queue* targets = NULL;
    targets = queue_new();

    typedef struct target {
      char *name;
      char *help;
    } target_t;
    
    target_t * a_target = NULL;
    target_t * b_target = NULL;
    target_t * c_target = NULL;
    QueueValue item = NULL;
    a_target = (target_t *) malloc(sizeof(target_t));
    a_target->name = "foo";
    a_target->help = "bar foo";
    b_target = (target_t *) malloc(sizeof(target_t));
    b_target->name = "foo 2";
    b_target->help = "bar foo 2";
    c_target = (target_t *) malloc(sizeof(target_t));
    c_target->name = "foo 3";
    c_target->help = "bar foo 3";
    
    queue_push_tail(targets, a_target);
    queue_push_tail(targets, b_target);
    queue_push_tail(targets, c_target);
    
    while (queue_is_empty(targets) == 0) {
	target_t *i = queue_pop_head(targets); 
	printf("target %s, help %s\n", i->name, i->help);
    }
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
   while ((nread = getline(&line, &len, stdin)) != -1) {
       printf("Retrieved line of length %zd:\n", nread);
        fwrite(line, nread, 1, stdout);
    }

    free(line);
    queue_free(targets);
    exit(EXIT_SUCCESS);
}

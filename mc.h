#define PCRE2_CODE_UNIT_WIDTH 8
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>

# define REGEX_HELP_TARGET "^(?P<name>[a-zA-Z_/-]+):.*?## (?P<help>.*)$"
# define REGEX_LOCAL_VAR "^(?P<target>[a-zA-Z_\\/-]+): ?(?P<name>[A-Z_]+) ?\\?= (?P<default>.*) #\\? ?(?P<help>.*)$"
# define REGEX_GLOBAL_VAR "^(?P<name>[A-Z_]+).*?= ?(?P<default>.*)#\\?(?P<help>.*)$"

# define PROGNAME "mh"
# define VERSION "0.1"
# define AUTHOR "Oz Tiram <oz.tiram __At__ gmail.com>"

# ifdef COLOROUTPUT
# define CYN   "\x1B[36m"
# define UNDR "\x1B[4m"
# define WHT   "\x1B[37m"
# define RESET "\x1B[0m"
# endif
# ifndef COLOROUTPUT
# define CYN   ""
# define WHT   ""
# define RESET ""
# define UNDR ""
# endif

int string_length(char *s) {
  if (*s == '\0') // Base condition
    return 0;

  return (1 + string_length(++s));
}

/**
 * Targets have names, help text and local variables
 */
typedef struct target {
  char *name;
  char *help;
  Queue *locals;
} target_t;

/**
 * variable global or local
 */
typedef struct variable {
  char *name;
  char *default_value;
  char *help;
} variable_t;


target_t *new_target(void) {
    target_t *target = (target_t *) malloc(sizeof(target_t));
    target->name = NULL;
    target->help = NULL;
    target->locals = queue_new();
    return target;
}

target_t *copy_target(target_t *target) {
    target_t *copy = new_target();
    copy->name = (char*)malloc(string_length(target->name) * sizeof(PCRE2_UCHAR));
    copy->help = (char*)malloc(string_length(target->help) * sizeof(PCRE2_UCHAR));
    strcpy(copy->name, target->name);
    strcpy(copy->help, target->help);
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

/**
 * check line for regex
 * line - a text line
 * target - a target pointer
 * variable - a variable pointer
 * local - check if matching for local target
 **/
int check_line_for_regex(char *line, target_t *target, variable_t *variable, pcre2_code *regex, int local);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 * <var-name> ?= <default value>#? <help string>
 */
int check_line_for_global_var(char *line, variable_t *global, pcre2_code *regex);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 */
int check_line_for_local_var(char *line, variable_t *local, pcre2_code *regex);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 */
int check_line_for_target(char *line, target_t *target, pcre2_code *regex);

pcre2_code *compile_regex(char *regex);

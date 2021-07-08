#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

# define REGEX_HELP_TARGET "^(?P<name>[a-zA-Z_-]+):.*?## (?P<help>.*)$"
# define REGEX_LOCAL_VAR "^(?P<target>[\w_\-/]+):.(?P<var>[A-Z_]+).*#\? (?P<help>.*)$"
# define REGEX_GLOBAL_VAR "(?P<global>[A-Z_]+).*#\? (?P<help>.*)$"

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


variable_t *new_variable(void) {
    variable_t *variable = (variable_t *) malloc(sizeof(variable_t));
    variable->name = NULL;
    variable->default_value = NULL;
    variable->help = NULL;
    return variable;
}

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 * <var-name> ?= <default value>#? <help string>
 */
int check_line_for_global_var(char *line, variable_t *global);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 */
int check_line_for_local_var(char *line, variable_t *local);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 */
int check_line_for_target(char *line, target_t *target);

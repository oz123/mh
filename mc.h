#include "queue.h"

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

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 * <var-name> ?= <default value>#? <help string>
 */
int check_line_for_global_var(char *line, variable_t *global);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 */
int check_line_for_local_var(char *line, variable_t *local);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 */
int check_line_for_target(char *line, target_t *target);

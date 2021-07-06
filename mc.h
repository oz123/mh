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
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 */
target_t* check_line_for_target(char *line);

/*
BSD 2-Clause License

Copyright (c) 2021, Oz N Tiram <oz.tiram@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#define PCRE2_CODE_UNIT_WIDTH 8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre2.h>
#include "queue.h"

# define REGEX_HELP_TARGET "^(?P<name>[a-zA-Z_/-]+):.*?## (?P<help>.*)$"
# define REGEX_LOCAL_VAR "^(?P<target>[a-zA-Z_\\/-]+): ?(?P<name>[A-Z_]+) ?\\?= (?P<default>.*) #\\? ?(?P<help>.*)$"
# define REGEX_GLOBAL_VAR "^(?P<name>[A-Z_]+).*?= ?(?P<default>.*)#\\?(?P<help>.*)$"


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

#define NUMBER_OF_STRINGS 5
#define MAX_STRING_SIZE 40

/**
* Targets have names, help text and local variables
 */
typedef struct target {
  char *name;
  char *help;
  Queue *locals;
} target_t;

target_t *new_target(void);
void free_target(target_t*);
target_t *copy_target(target_t*);

/**
 * Show the help for a single target and exit
 *
 * return 1 if target not found
 */
int show_target_help(char *, Queue *);
/**
 * Show all targets and globals help and exit
 */
void show_all_help(Queue *, Queue *);
/**
 * variable global or local
 */
typedef struct variable {
  char *name;
  char *default_value;
  char *help;
} variable_t;

variable_t *new_variable(void);
void free_variable(variable_t *variable);


/**
 * Check line for regex
 *
 * @param line      a text line
 * @target          target_t a target pointer
 * @variable        variable_t variable pointer
 * @return          0 for a match, 1 for non match
 **/
int check_line_for_regex(char *line, target_t *target, variable_t *variable, pcre2_code *regex);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 * <var-name> ?= <default value>#? <help string>
 *
 * @param line      a text line
 * @target          target_t a target pointer
 * @variable        variable_t variable pointer
 * @return          0 for a match, 1 for non match
 */
int check_line_for_global_var(char *line, variable_t *global, pcre2_code *regex);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 * <target-name>: <var-name> ?= <default value> #? <help string>
 *
 * @param line      a text line
 * @variable        variable_t variable pointer
 * @regex           pcre2_code pointer 
 * @return          0 for a match, 1 for non match
 */
int check_line_for_local_var(char *line, variable_t *local, pcre2_code *regex);

/**
 * Check that a line in file is in the form of <traget-name: ## help for the target>
 *
 * @param line      a text line
 * @target          target_t a target pointer
 * @regex           pcre2_code pointer
 * @return          0 for a match, 1 for non match
 */
int check_line_for_target(char *line, target_t *target, pcre2_code *regex);

/**
 * Compile a string to Regex object
 *
 * @param regex     string containing a regex
 */
pcre2_code *compile_regex(char *regex);

void init_pcre_regex(pcre2_code **a, pcre2_code **b, pcre2_code **c);

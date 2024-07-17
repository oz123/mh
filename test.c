#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "mh.h"

static void check_line_is_target(void **state){
    target_t *t = new_target();
    pcre2_code *regex_target = compile_regex(REGEX_HELP_TARGET);
    int rv = check_line_for_target("foo: ## does foo",
                                   t,
                                   regex_target);
    assert_false(rv); // check_line_for_target returns 0 on match
}

static void check_line_is_not_target(void **state){
    target_t *t = new_target();
    pcre2_code *regex_target = compile_regex(REGEX_HELP_TARGET);
    int rv = check_line_for_target("foo## does foo",
                                   t,
                                   regex_target);
    assert_true(rv); // check_line_for_target returns 1 on non match
}

static void check_line_is_variable(void **state){
    variable_t *v = new_variable();
    pcre2_code *regex_variable = compile_regex(REGEX_LOCAL_VAR);
    int rv = check_line_for_local_var("foo: BAR ?= baz #? sets bar of foo to baz",
                                   v,
                                   regex_variable);
    assert_false(rv); // check_line_for_variable returns 0 on match
}


static void check_line_is_global(void **state){
    variable_t *g = new_variable();
    pcre2_code *regex_target = compile_regex(REGEX_GLOBAL_VAR);
    int rv = check_line_for_global_var("BAR ?= baz #? sets global bar to baz",
                                   g,
                                   regex_target);
    assert_false(rv); // check_line_for_global returns 0 on match
}


static void check_line_is_global_with_digit(void **state){
    variable_t *g = new_variable();
    pcre2_code *regex_target = compile_regex(REGEX_GLOBAL_VAR);
    int rv = check_line_for_global_var("BAR_WITH_11_DIGIT ?= baz #? sets global bar to baz",
                                   g,
                                   regex_target);
	int cmp = strcmp(g->name, "BAR_WITH_11_DIGIT");
	assert_false(cmp); // check that string comparision succeeded
    assert_false(rv); // check_line_for_global returns 0 on match
}


static void check_line_is_not_global(void **state){
    variable_t *g = new_variable();
    pcre2_code *regex_target = compile_regex(REGEX_GLOBAL_VAR);
    int rv = check_line_for_global_var("BAR baz #? sets global bar to baz",
                                   g,
                                   regex_target);
    assert_true(rv); // check_line_for_global returns 1 on non match
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(check_line_is_not_target),
        cmocka_unit_test(check_line_is_target),
        cmocka_unit_test(check_line_is_variable),
        cmocka_unit_test(check_line_is_global),
        cmocka_unit_test(check_line_is_global_with_digit),
        cmocka_unit_test(check_line_is_not_global),
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

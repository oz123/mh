#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "mh.h"

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
    (void) state; /* unused */
}

static void check_line_is_target(void **state){
    target_t *t = new_target();
    pcre2_code *regex_target = compile_regex(REGEX_HELP_TARGET);
    int rv = check_line_for_target("foo: ## does foo",
                                   t,
                                   regex_target);
    assert_false(rv); // check_line_for_target returns 0 on match
}

static void check_line_is_variable(void **state){
    target_t *t = new_target();
    pcre2_code *regex_target = compile_regex(REGEX_LOCAL_VAR);
    int rv = check_line_for_target("foo: BAR ?= baz #? sets bar of foo to baz",
                                   t,
                                   regex_target);
    assert_false(rv); // check_line_for_variable returns 0 on match
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(check_line_is_target),
        cmocka_unit_test(check_line_is_variable),
        };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

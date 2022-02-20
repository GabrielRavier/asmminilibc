/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strtok_r
*/

#include <string.h>
#include <criterion/criterion.h>
#define GLIBC_BUF1PAGES 1
#include "glibc.h"

static char *(*volatile strtok_r_ptr)(char *, const char *, char **) = strtok_r;

static void check_result(char *str, const char *delimiters, char **save_ptr, const char *expected_result)
{
    cr_assert_eq(strtok_r_ptr(str, delimiters, save_ptr), expected_result);
}

Test(strtok_r, cosmopolitan_empty)
{
    char *str = strdup("");
    cr_assert_not_null(str);

    char *state;
    check_result("", "/", &state, NULL);
    free(str);
}

Test(strtok_r, cosmopolitan)
{
    char *str = strdup(".,lol..cat..");
    cr_assert_not_null(str);

    char *state;
    check_result(str, ".,", &state, str + 2);
    cr_assert_str_eq(str + 2, "lol");
    check_result(NULL, ".,", &state, str + 7);
    cr_assert_str_eq(str + 7, "cat");
    check_result(NULL, ".,", &state, NULL);
    check_result(NULL, ".,", &state, NULL);
    free(str);
}

Test(strtok_r, cosmopolitan_hosts_txt_line)
{
    char *str = strdup("203.0.113.1     lol.example. lol");
    cr_assert_not_null(str);

    char *state;
    check_result(str, " \t", &state, str);
    cr_assert_str_eq(str, "203.0.113.1");
    check_result(NULL, " \t", &state, str + 16);
    cr_assert_str_eq(str + 16, "lol.example.");
    check_result(NULL, " \t", &state, str + 16 + 13);
    cr_assert_str_eq(str + 16 + 13, "lol");
    check_result(NULL, " \t", &state, NULL);
    check_result(NULL, " \t", &state, NULL);
    free(str);
}

Test(strtok_r, glibc)
{
    glibc_test_string_init();

    char **save_ptr_p = (char **)(glibc_buf1 + glibc_page_size - sizeof(*save_ptr_p)); // Will crash on overflow
    char line[] = "udf 75868 1 - Live 0xffffffffa0bfb000\n";
    char *tok = strtok_r(line, " \t", save_ptr_p);
    cr_assert_str_eq(tok, "udf");
}

Test(strtok_r, glibc_andrew)
{
    char buf[1] = { 0 };

    char *state;
    check_result(buf, " ", &state, NULL);
    check_result(NULL, " ", &state, NULL);
}

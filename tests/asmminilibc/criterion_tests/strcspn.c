/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strcspn
*/

#include <string.h>
#include <criterion/criterion.h>

static size_t (*volatile strcspn_ptr)(const char *, const char *) = strcspn;

#define GLIBC_STRPBRK_RESULT(str, position) (position)
#define GLIBC_STRPBRK_FUNC strcspn_ptr
#define GLIBC_STRPBRK_RESULT_TYPE size_t
#include "strpbrk-glibc.h"

static void check_result(const char *str, const char *reject, size_t expected_result)
{
    cr_assert_eq(strcspn_ptr(str, reject), expected_result);
}

Test(strcspn, netbsd)
{
    check_result("abcdefghijklmnop", "", 16);
    check_result("abcdefghijklmnop", "a", 0);
    check_result("abcdefghijklmnop", "b", 1);
    check_result("abcdefghijklmnop", "cd", 2);
    check_result("abcdefghijklmnop", "qrstuvwxyz", 16);
}

Test(strcspn, dietlibc)
{
    check_result("foo:bar:", ":", 3);
    check_result("foo:bar:", "=of", 0);
}

Test(strcspn, cosmopolitan)
{
    check_result("abcdefg", "ae", 0);
    check_result("abcdefg", "aeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", 0);
    check_result("abcdefg", "ze", 4);
    check_result("abcdefg", "zeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", 4);
    check_result("abcdfzg", "ze", 5);
    check_result("abcdfzg", "zeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", 5);
    check_result("abcdEfg", "ze", 7);
    check_result("abcdEfg", "zeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", 7);
    check_result("ABCDEFGHIJKLMNOPABCDEFGHIJKLMNOp", "abcdefghijklmnp", 31);
    check_result("ABCDEFGHIJKLMNOPABCDEFGHIJKLMNOp", "abcdefghijklmnpabcdefghijklmnp", 31);
}

Test(strcspn, cloudlibc)
{
    const char *str = "Hello, world";
    check_result(str, "H", 0);
    check_result(str, "rdw", 7);
    check_result(str, "XYZ", 12);
}

Test(strcspn, glibc)
{
    glibc_strpbrk_test();
}

Test(strcspn, glibc_random)
{
    glibc_strpbrk_test_random();
}

/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strpbrk
*/

#include <string.h>
#include <criterion/criterion.h>
#include "glibc.h"
#include <limits.h>

static char *(*volatile strpbrk_ptr)(const char *, const char *) = strpbrk;

#define GLIBC_STRPBRK_RESULT(str, position) ((str)[(position)] ? (str) + (position) : NULL)
#define GLIBC_STRPBRK_FUNC strpbrk_ptr
#define GLIBC_STRPBRK_RESULT_TYPE char *
#include "strpbrk-glibc.h"

Test(strpbrk, netbsd)
{
    static const char str[] = "abcdefghijklmnop";

    glibc_strpbrk_check_result(str, "", NULL);
    glibc_strpbrk_check_result(str, "qrst", NULL);
    glibc_strpbrk_check_result(str, "a", str);
    glibc_strpbrk_check_result(str, "b", str + 1);
    glibc_strpbrk_check_result(str, "ab", str);
    glibc_strpbrk_check_result(str, "cdef", str + 2);
    glibc_strpbrk_check_result(str, "fedc", str + 2);
}

Test(strpbrk, dietlibc)
{
    const char *str = "fnord:foo:bar:baz";
    glibc_strpbrk_check_result(str, ":", str + 5);
    glibc_strpbrk_check_result(str, ":/::/:foo/bar:baz", str);
}

Test(strpbrk, glibc)
{
    glibc_strpbrk_test();
}

Test(strpbrk, glibc_random)
{
    glibc_strpbrk_test_random();
}

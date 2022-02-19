/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests memfrob
*/

#define _GNU_SOURCE
#include <string.h>
#include <criterion/criterion.h>

Test(memfrob, cosmopolitan)
{
    char buffer[6];

    cr_assert_str_eq(memfrob(memcpy(buffer, "\0\0\0\0\0\0", 6), 5), "*****");
    cr_assert_str_eq(memfrob(strcpy(buffer, "hello"), 5), "BOFFE");
    cr_assert_str_eq(memfrob(memfrob(strcpy(buffer, "hello"), 6), 6), "hello");
}

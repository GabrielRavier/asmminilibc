/*
** EPITECH PROJECT, 2021
** libmy
** File description:
** Defines escape_check_zero
*/

#pragma once

#include <criterion/criterion.h>
#include <stdint.h>
#include <stddef.h>

static void escape_check_zero(uint8_t *buf, size_t start, size_t length, size_t total)
{
    while (start--) {
        cr_assert_eq(*buf++, 0xFF);
        --total;
    }
    while (length--) {
        cr_assert_eq(*buf++, 0);
        --total;
    }
    while (total--)
        cr_assert_eq(*buf++, 0xFF);
}

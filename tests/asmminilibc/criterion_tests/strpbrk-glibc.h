/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** glibc tests for strpbrk (also used for strcspn)
*/

#pragma once

#include "glibc.h"
#include <limits.h>
#include <stddef.h>

static void glibc_strpbrk_check_result(const char *str1, const char *str2, const GLIBC_STRPBRK_RESULT_TYPE expected_result)
{
    cr_assert_eq(GLIBC_STRPBRK_FUNC(str1, str2), expected_result);
}

static void glibc_strpbrk_do_test(size_t alignment, size_t position, size_t length)
{
    alignment &= 7;
    if ((alignment + position + 10) >= glibc_page_size || length > 240)
        return;

    char *reject = glibc_buf2 + (random() & 255);
    char *str = glibc_buf1 + alignment;

    for (size_t i = 0; i < length; ++i) {
        reject[i] = random() & CHAR_MAX;
        if (reject[i] == '\0')
            reject[i] = random() & CHAR_MAX;
        if (reject[i] == '\0')
            reject[i] = 1 + (random() & 127);
    }
    reject[length] = '\0';

    int c;
    for (c = 1; c <= CHAR_MAX; ++c)
        if (strchr(reject, c) == NULL)
            break;

    for (size_t i = 0; i < position; ++i) {
        str[i] = random() & CHAR_MAX;
        if (strchr(reject, str[i]) != NULL) {
            str[i] = random() & CHAR_MAX;
            if (strchr(reject, str[i]) != NULL)
                str[i] = c;
        }
    }
    str[position] = reject[random() % (length + 1)];
    if (str[position] != '\0') {
        size_t i;
        for (i = position + 1; i < position + 10; ++i)
            str[i] = random() & CHAR_MAX;
        str[i] = '\0';
    }
    glibc_strpbrk_check_result(str, reject, GLIBC_STRPBRK_RESULT(str, position));
}

static inline void glibc_strpbrk_test()
{
    glibc_test_string_init();

    for (size_t i = 0; i < 32; ++i) {
        glibc_strpbrk_do_test(0, 512, i);
        glibc_strpbrk_do_test(i, 512, i);
    }

    for (size_t i = 1; i < 8; ++i) {
        glibc_strpbrk_do_test(0, 16 << i, 4);
        glibc_strpbrk_do_test(1, 16 << i, 4);
    }

    for (size_t i = 1; i < 8; ++i)
        glibc_strpbrk_do_test(i, 64, 10);

    for (size_t i = 0; i < 64; ++i)
        glibc_strpbrk_do_test(0, i, 6);
}

static inline void glibc_strpbrk_test_random()
{
    glibc_test_string_init();

    unsigned char *ptr = (unsigned char *)(glibc_buf1 + glibc_page_size) - 512;

    for (size_t n = 0; n < 50000; ++n) {
        size_t alignment = random() & 15;
        
        size_t position = random() & 511;
        if (position + alignment >= 511)
            position = 510 - alignment - (random() & 7);

        size_t length = random() & 511;
        if (position >= length && (random() & 1))
            length = position + 1 + (random() & 7);
        if (length + alignment >= 512)
            length = 511 - alignment - (random() & 7);

        size_t r_length;
        if (random() & 1)
            r_length = random() & 63;
        else
            r_length = random() & 15;

        unsigned char *reject = (unsigned char *)(glibc_buf2 + glibc_page_size) - r_length - 1 - (random() & 7);
        for (size_t i = 0; i < r_length; ++i) {
            reject[i] = random() & CHAR_MAX;
            if (reject[i] == '\0')
                reject[i] = random() & CHAR_MAX;
            if (reject[i] == '\0')
                reject[i] = 1 + (random() & 127);
        }
        reject[r_length] = '\0';

        int ch;
        for (ch = 1; ch <= CHAR_MAX; ++ch)
            if (strchr((char *)reject, ch) == NULL)
                break;

        size_t j = (position > length ? position : length) + alignment + 64;
        if (j > 512)
            j = 512;

        for (size_t i = 0; i < j; ++i) {
            if (i == length + alignment)
                ptr[i] = '\0';
            else if (i == position + alignment)
                ptr[i] = reject[random() % (r_length + 1)];
            else if (i < alignment || i > position + alignment)
                ptr[i] = random() & CHAR_MAX;
            else {
                ptr[i] = random() & CHAR_MAX;
                if (strchr((char *)reject, ptr[i]) != NULL) {
                    ptr[i] = random() & CHAR_MAX;
                    if (strchr((char *)reject, ptr[i]) != NULL)
                        ptr[i] = ch;
                }
            }
        }

        glibc_strpbrk_check_result((char *)(ptr + alignment), (char *)reject, GLIBC_STRPBRK_RESULT((char *)(ptr + alignment), position < length ? position : length));
    }
}

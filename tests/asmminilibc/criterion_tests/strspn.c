/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strspn
*/

#include <string.h>
#include <criterion/criterion.h>
#include "glibc.h"
#include <limits.h>

static size_t (*volatile strspn_ptr)(const char *, const char *) = strspn;

static void check_result(const char *str, const char *accept, size_t expected_result)
{
    cr_assert_eq(strspn_ptr(str, accept), expected_result);
}

Test(strspn, netbsd)
{
    check_result("abcdefghijklmnop", "", 0);
    check_result("abcdefghijklmnop", "a", 1);
    check_result("abcdefghijklmnop", "b", 0);
    check_result("abcdefghijklmnop", "ab", 2);
    check_result("abcdefghijklmnop", "abc", 3);
    check_result("abcdefghijklmnop", "abce", 3);
    check_result("abcdefghijklmnop", "abcdefghijklmnop", 16);
}

Test(strspn, dietlibc)
{
    check_result("foo:bar:", ":=b", 0);
    check_result("foo:bar:", ":=of", 4);
}

Test(strspn, cloudlibc)
{
    const char *str = "Hello, world";
    check_result(str, "", 0);
    check_result(str, "Foo", 0);
    check_result(str, "olHe", 5);
    check_result(str, "Helo, wrld", 12);
}

static void glibc_do_test(size_t alignment, size_t position, size_t length)
{
    alignment &= 7;
    if ((alignment + position + 10) >= glibc_page_size || length > 240 || length == 0)
        return;

    char *accept = glibc_buf2 + (random() & 255);
    char *str =  glibc_buf1 + alignment;

    for (size_t i = 0; i < length; ++i) {
        accept[i] = random() & CHAR_MAX;
        if (accept[i] == '\0')
            accept[i] = random() & CHAR_MAX;
        if (accept[i] == '\0')
            accept[i] =  1 + (random() & 127);
    }
    accept[length] = '\0';

    for (size_t i = 0; i < position; ++i)
        str[i] = accept[random() % length];
    str[position] = random() & 255;
    if (strchr(accept, str[position]) != NULL)
        str[position] = '\0';
    else {
        size_t i;
        for (i = position + 1; i < position + 10; ++i)
            str[i] = random() & CHAR_MAX;
        str[i] = '\0';
    }

    check_result(str, accept, position);
}

Test(strspn, glibc)
{
    glibc_test_string_init();

    for (size_t i = 0; i < 32; ++i) {
        glibc_do_test(0, 512, i);
        glibc_do_test(i, 512, i);
    }

    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(0, 16 << i, 4);
        glibc_do_test(i, 16 << i, 4);
    }

    for (size_t i = 1; i < 8; ++i)
        glibc_do_test(0, 64, 10);

    for (size_t i = 0; i < 64; ++i)
        glibc_do_test(0, i, 6);
}

Test(strspn, glibc_random)
{
    glibc_test_string_init();

    unsigned char *ptr = (unsigned char *)(glibc_buf1 + glibc_page_size) - 512;

    for (size_t n = 0; n < 100000; ++n) {
        size_t alignment = random() & 15;

        size_t accept_length;
        if (random() & 1)
            accept_length = random() & 63;
        else
            accept_length = random() & 15;

        size_t position;
        if (accept_length == 0)
            position = 0;
        else
            position = random() & 511;
        if (position + alignment >= 511)
            position = 510 - alignment - (random() & 7);

        size_t length = random() & 511;
        if (length + alignment >= 512)
            length = 511 - alignment - (random() & 7);

        unsigned char *accept = (unsigned char *)(glibc_buf2 + glibc_page_size) - accept_length - 1 - (random() & 7);
        for (size_t i = 0; i < accept_length; ++i) {
            accept[i] = random() & CHAR_MAX;
            if (accept[i] == '\0')
                accept[i] = random() & CHAR_MAX;
            if (accept[i] == '\0')
                accept[i] = 1 + (random() & 127);
        }
        accept[accept_length] = '\0';

        size_t j = (position > length ? position : length) + alignment + 64;
        if (j > 512)
            j = 512;

        for (size_t i = 0; i < j; ++i) {
            if (i == length + alignment)
                ptr[i] = '\0';
            else if (i == position + alignment) {
                ptr[i] = random() & CHAR_MAX;
                if (strchr((char *)accept, ptr[i]) != NULL)
                    ptr[i] = '\0';
            } else if (i < alignment || i > position + alignment)
                ptr[i] = random() & CHAR_MAX;
            else
                ptr[i] = accept[random() % accept_length];
        }

        check_result((char *)(ptr + alignment), (char *)accept, position < length ? position : length);
    }
}

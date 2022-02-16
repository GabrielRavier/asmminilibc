/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strlen
*/

#include <string.h>
#include <criterion/criterion.h>
#include "bionic_buffer.h"
#include "glibc.h"
#include <unistd.h>
#include <stdint.h>
#include <limits.h>

static size_t (*volatile strlen_ptr)(const char *) = &strlen;

Test(strlen, basic)
{
    cr_assert_eq(strlen_ptr("foo"), 3);
    cr_assert_eq(strlen_ptr("Hello World !"), 13);
    cr_assert_eq(strlen_ptr("test"), 4);
    cr_assert_eq(strlen_ptr("hello"), 5);
    cr_assert_eq(strlen_ptr("abcdefghijklmnopqrstuvwxyz"), 26);
}

Test(strlen, netbsd_basic)
{
    struct {
        const char *value;
        size_t length;
    } table[] = {
        { "",                           0 },
        { "a",                          1 },
        { "ab",                         2 },
        { "abc",                        3 },
        { "abcd",                       4 },
        { "abcde",                      5 },
        { "abcdef",                     6 },
        { "abcdefg",                    7 },
        { "abcdefgh",                   8 },
        { "abcdefghi",                  9 },
        { "abcdefghij",                 10 },
        { "abcdefghijk",                11 },
        { "abcdefghijkl",               12 },
        { "abcdefghijklm",              13 },
        { "abcdefghijklmn",             14 },
        { "abcdefghijklmno",            15 },
        { "abcdefghijklmnop",           16 },
        { "abcdefghijklmnopq",          17 },
        { "abcdefghijklmnopqr",         18 },
        { "abcdefghijklmnopqrs",        19 },
        { "abcdefghijklmnopqrst",       20 },
        { "abcdefghijklmnopqrstu",      21 },
        { "abcdefghijklmnopqrstuv",     22 },
        { "abcdefghijklmnopqrstuvw",    23 },

        /*
         * patterns that check for the cases where the expression:
         *
         *      ((word - 0x7f7f..7f) & 0x8080..80)
         *
         * returns non-zero even though there are no zero bytes in
         * the word.
         */

        { "" "\xff\xff\xff\xff\xff\xff\xff\xff" "abcdefgh",     16 },
        { "a" "\xff\xff\xff\xff\xff\xff\xff\xff" "bcdefgh",     16 },
        { "ab" "\xff\xff\xff\xff\xff\xff\xff\xff" "cdefgh",     16 },
        { "abc" "\xff\xff\xff\xff\xff\xff\xff\xff" "defgh",     16 },
        { "abcd" "\xff\xff\xff\xff\xff\xff\xff\xff" "efgh",     16 },
        { "abcde" "\xff\xff\xff\xff\xff\xff\xff\xff" "fgh",     16 },
        { "abcdef" "\xff\xff\xff\xff\xff\xff\xff\xff" "gh",     16 },
        { "abcdefg" "\xff\xff\xff\xff\xff\xff\xff\xff" "h",     16 },
        { "abcdefgh" "\xff\xff\xff\xff\xff\xff\xff\xff" "",     16 },
    };

    char buffer[64];
    for (size_t i = 0; i < sizeof(uintmax_t); ++i)
        for (size_t j = 0; j < (sizeof(table) / sizeof(table[0])); ++j) {
            memcpy(&buffer[i], table[j].value, table[j].length + 1);
            cr_assert_eq(strlen_ptr(&buffer[i]), table[j].length);
        }
}

Test(strlen, netbsd_huge)
{
    size_t page = sysconf(_SC_PAGESIZE);

    for (size_t i = 0; i < 1000; i += 100) {
        char *str = malloc(i * page + 1);
        cr_assert_not_null(str);

        memset(str, 'x', i * page);
        str[i * page] = '\0';
        cr_assert_eq(strlen(str), i * page);
        free(str);
    }
}

static void bionic_do_strlen_test(uint8_t *buffer, size_t length)
{
    if (length >= 1) {
        memset(buffer, (32 + (length % 96)), length - 1);
        buffer[length - 1] = '\0';
        cr_assert_eq(length - 1, strlen_ptr((const char *)buffer));
    }
}

Test(strlen, bionic_align)
{
    bionic_run_single_buffer_align_test(64 * 1024, bionic_do_strlen_test);
}

Test(strlen, bionic_overread)
{
    bionic_run_single_buffer_overread_test(bionic_do_strlen_test);
}

Test(strlen, dietlibc)
{
    enum {
        BUFFER_LENGTH = CHAR_BIT * sizeof(uintmax_t) * 4
    };
    char buffer[BUFFER_LENGTH + 1 + CHAR_BIT];

    cr_assert_eq(strlen_ptr(""), 0);

    for (size_t length = 0; length < BUFFER_LENGTH; ++length) {
        buffer[length] = length % 10 + '0';
        buffer[length + 1] = '\0';
        for (unsigned mask = 0; mask < (1u << sizeof(uintmax_t)); ++mask) {
            for (size_t bit = 0; bit < sizeof(uintmax_t); ++bit)
                buffer[length + 2 + bit] = (mask & (1 << bit)) ? 0xFF : 0;
            for (size_t i = 0; i <= length; ++i)
                cr_assert_eq(strlen_ptr(buffer + i), length + 1 - i);
        }
    }
}

Test(strlen, c11_usage_example)
{
    char ugh[] = "eeeeeeeeeeeeeee\017";
    cr_assert_eq(strlen_ptr(ugh + 15), 1);
    cr_assert_eq(strlen_ptr("utf-8 ☻"), 6 + 3);
}

Test(strlen, cosmopolitan_const)
{
    const char buffer[] = "hellothere";
    cr_assert_eq(strlen_ptr(buffer), 10);
}

Test(strlen, cosmopolitan_nonconst)
{
    char buffer[256];

    unsigned i;
    for (i = 0; i < 255; ++i)
        buffer[i] = i + 1;
    buffer[i] = '\0';
    cr_assert_eq(strlen_ptr(buffer), 255);
}

Test(strlen, glibc_tst)
{
    static const size_t lengths[] = {
        0, 1, 0, 2, 0, 1, 0, 3,
        0, 1, 0, 2, 0, 1, 0, 4
    };
    char base_buffer[24 + 32];

    for (size_t base = 0; base < 32; ++base) {
        char *buffer = base_buffer + base;
        for (size_t words = 0; words < 4; ++words) {
            memset(buffer, 'a', words * 4);
            for (size_t last = 0; last < 16; ++last) {
                buffer[words * 4 + 0] = (last & 1) != 0 ? 'b' : '\0';
                buffer[words * 4 + 1] = (last & 2) != 0 ? 'c' : '\0';
                buffer[words * 4 + 2] = (last & 4) != 0 ? 'd' : '\0';
                buffer[words * 4 + 3] = (last & 8) != 0 ? 'e' : '\0';
                buffer[words * 4 + 4] = '\0';

                cr_assert_eq(strlen_ptr(buffer), words * 4 + lengths[last]);
            }
        }
    }
}

static void glibc_do_test(size_t alignment, size_t length)
{
    alignment &= getpagesize() - 1;
    if (alignment + length >= glibc_page_size)
        return;

    for (size_t i = 0; i < length; ++i)
        glibc_buf1[alignment + i] = 1 + 11111 * i % CHAR_MAX;
    glibc_buf1[alignment + length] = '\0';
    cr_assert_eq(strlen_ptr(glibc_buf1 + alignment), length);
}

Test(strlen, glibc_random_tests)
{
    glibc_test_string_init();
    cr_assert(glibc_page_size >= 512);
    char *ptr = glibc_buf1 + glibc_page_size - 512;

    for (size_t n = 0; n < 100000; ++n) {
        size_t alignment = random() & 15;
        size_t length = random() & 511;
        if (length + alignment > 510)
            length = 511 - alignment - (random() & 7);

        size_t j = length + alignment + 64;
        if (j > 512)
            j = 512;

        for (size_t i = 0; i < j; ++i) {
            if (i == length + alignment)
                ptr[i] = '\0';
            else {
                ptr[i] = random() & 255;
                if (i >= alignment && i < length + alignment && ptr[i] == '\0')
                    ptr[i] = (random() & 127) + 1;
            }
        }
        cr_assert_eq(strlen_ptr(ptr + alignment), length);
    }
}

Test(strlen, glibc_test)
{
    glibc_test_string_init();
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(i, i);
        glibc_do_test(0, i);
    }
    for (size_t i = 2; i <= 12; ++i) {
        glibc_do_test(0, 1 << i);
        glibc_do_test(7, 1 << i);
        glibc_do_test(i, 1 << i);
        glibc_do_test(i, (size_t)((1 << i) / 1.5));
    }

    size_t max_length = 64;
    size_t page_size = getpagesize();
    for (size_t i = max_length; i > 1; --i) {
        glibc_do_test(page_size - i, i - 1); // String on same page
        glibc_do_test(page_size - i, max_length); // String crossing page boundary
    }
}

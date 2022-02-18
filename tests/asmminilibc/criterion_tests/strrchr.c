/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strrchr
*/

#include <string.h>
#include <criterion/criterion.h>
#include "bionic_test_state.h"
#include "bionic_buffer.h"
#include "glibc.h"
#include <limits.h>

static char *(*volatile strrchr_ptr)(const char *str, int character) = strrchr;

Test(strrchr, escape)
{
    const char *str = "abcdefabc";

    cr_assert_eq(strrchr_ptr(str, 'a'), str + 6);
    cr_assert_eq(strrchr_ptr(str, 'b'), str + 7);
    cr_assert_eq(strrchr_ptr(str, 'c'), str + 8);
    cr_assert_null(strrchr_ptr(str, 'g'));
    cr_assert_eq(strrchr_ptr(str, 'd'), str + 3);
}

Test(strrchr, netbsd_basic)
{
    struct {
        const char *value;
        char match;
        ssize_t last_offset;
    } table[] = {
        { "",                   0, 0 },
        { "a",                  0, 0 },
        { "aa",                 0, 0 },
        { "abc",                0, 0 },
        { "abcd",               0, 0 },
        { "abcde",              0, 0 },
        { "abcdef",             0, 0 },
        { "abcdefg",            0, 0 },
        { "abcdefgh",           0, 0 },

        { "/",                  1, 0 },
        { "//",                 1, 1 },
        { "/a",                 1, 0 },
        { "/a/",                1, 2 },
        { "/ab",                1, 0 },
        { "/ab/",               1, 3 },
        { "/abc",               1, 0 },
        { "/abc/",              1, 4 },
        { "/abcd",              1, 0 },
        { "/abcd/",             1, 5 },
        { "/abcde",             1, 0 },
        { "/abcde/",            1, 6 },
        { "/abcdef",            1, 0 },
        { "/abcdef/",           1, 7 },
        { "/abcdefg",           1, 0 },
        { "/abcdefg/",          1, 8 },
        { "/abcdefgh",          1, 0 },
        { "/abcdefgh/",         1, 9 },

        { "a/",                 1, 1 },
        { "a//",                1, 2 },
        { "a/a",                1, 1 },
        { "a/a/",               1, 3 },
        { "a/ab",               1, 1 },
        { "a/ab/",              1, 4 },
        { "a/abc",              1, 1 },
        { "a/abc/",             1, 5 },
        { "a/abcd",             1, 1 },
        { "a/abcd/",            1, 6 },
        { "a/abcde",            1, 1 },
        { "a/abcde/",           1, 7 },
        { "a/abcdef",           1, 1 },
        { "a/abcdef/",          1, 8 },
        { "a/abcdefg",          1, 1 },
        { "a/abcdefg/",         1, 9 },
        { "a/abcdefgh",         1, 1 },
        { "a/abcdefgh/",        1, 10 },

        { "ab/",                1, 2 },
        { "ab//",               1, 3 },
        { "ab/a",               1, 2 },
        { "ab/a/",              1, 4 },
        { "ab/ab",              1, 2 },
        { "ab/ab/",             1, 5 },
        { "ab/abc",             1, 2 },
        { "ab/abc/",            1, 6 },
        { "ab/abcd",            1, 2 },
        { "ab/abcd/",           1, 7 },
        { "ab/abcde",           1, 2 },
        { "ab/abcde/",          1, 8 },
        { "ab/abcdef",          1, 2 },
        { "ab/abcdef/",         1, 9 },
        { "ab/abcdefg",         1, 2 },
        { "ab/abcdefg/",        1, 10 },
        { "ab/abcdefgh",        1, 2 },
        { "ab/abcdefgh/",       1, 11 },

        { "abc/",               1, 3 },
        { "abc//",              1, 4 },
        { "abc/a",              1, 3 },
        { "abc/a/",             1, 5 },
        { "abc/ab",             1, 3 },
        { "abc/ab/",            1, 6 },
        { "abc/abc",            1, 3 },
        { "abc/abc/",           1, 7 },
        { "abc/abcd",           1, 3 },
        { "abc/abcd/",          1, 8 },
        { "abc/abcde",          1, 3 },
        { "abc/abcde/",         1, 9 },
        { "abc/abcdef",         1, 3 },
        { "abc/abcdef/",        1, 10 },
        { "abc/abcdefg",        1, 3 },
        { "abc/abcdefg/",       1, 11 },
        { "abc/abcdefgh",       1, 3 },
        { "abc/abcdefgh/",      1, 12 },

        { "abcd/",              1, 4 },
        { "abcd//",             1, 5 },
        { "abcd/a",             1, 4 },
        { "abcd/a/",            1, 6 },
        { "abcd/ab",            1, 4 },
        { "abcd/ab/",           1, 7 },
        { "abcd/abc",           1, 4 },
        { "abcd/abc/",          1, 8 },
        { "abcd/abcd",          1, 4 },
        { "abcd/abcd/",         1, 9 },
        { "abcd/abcde",         1, 4 },
        { "abcd/abcde/",        1, 10 },
        { "abcd/abcdef",        1, 4 },
        { "abcd/abcdef/",       1, 11 },
        { "abcd/abcdefg",       1, 4 },
        { "abcd/abcdefg/",      1, 12 },
        { "abcd/abcdefgh",      1, 4 },
        { "abcd/abcdefgh/",     1, 13 },

        { "abcde/",             1, 5 },
        { "abcde//",            1, 6 },
        { "abcde/a",            1, 5 },
        { "abcde/a/",           1, 7 },
        { "abcde/ab",           1, 5 },
        { "abcde/ab/",          1, 8 },
        { "abcde/abc",          1, 5 },
        { "abcde/abc/",         1, 9 },
        { "abcde/abcd",         1, 5 },
        { "abcde/abcd/",        1, 10 },
        { "abcde/abcde",        1, 5 },
        { "abcde/abcde/",       1, 11 },
        { "abcde/abcdef",       1, 5 },
        { "abcde/abcdef/",      1, 12 },
        { "abcde/abcdefg",      1, 5 },
        { "abcde/abcdefg/",     1, 13 },
        { "abcde/abcdefgh",     1, 5 },
        { "abcde/abcdefgh/",    1, 14 },

        { "abcdef/",            1, 6 },
        { "abcdef//",           1, 7 },
        { "abcdef/a",           1, 6 },
        { "abcdef/a/",          1, 8 },
        { "abcdef/ab",          1, 6 },
        { "abcdef/ab/",         1, 9 },
        { "abcdef/abc",         1, 6 },
        { "abcdef/abc/",        1, 10 },
        { "abcdef/abcd",        1, 6 },
        { "abcdef/abcd/",       1, 11 },
        { "abcdef/abcde",       1, 6 },
        { "abcdef/abcde/",      1, 12 },
        { "abcdef/abcdef",      1, 6 },
        { "abcdef/abcdef/",     1, 13 },
        { "abcdef/abcdefg",     1, 6 },
        { "abcdef/abcdefg/",    1, 14 },
        { "abcdef/abcdefgh",    1, 6 },
        { "abcdef/abcdefgh/",   1, 15 },

        { "abcdefg/",           1, 7 },
        { "abcdefg//",          1, 8 },
        { "abcdefg/a",          1, 7 },
        { "abcdefg/a/",         1, 9 },
        { "abcdefg/ab",         1, 7 },
        { "abcdefg/ab/",        1, 10 },
        { "abcdefg/abc",        1, 7 },
        { "abcdefg/abc/",       1, 11 },
        { "abcdefg/abcd",       1, 7 },
        { "abcdefg/abcd/",      1, 12 },
        { "abcdefg/abcde",      1, 7 },
        { "abcdefg/abcde/",     1, 13 },
        { "abcdefg/abcdef",     1, 7 },
        { "abcdefg/abcdef/",    1, 14 },
        { "abcdefg/abcdefg",    1, 7 },
        { "abcdefg/abcdefg/",   1, 15 },
        { "abcdefg/abcdefgh",   1, 7 },
        { "abcdefg/abcdefgh/",  1, 16 },

        { "abcdefgh/",          1, 8 },
        { "abcdefgh//",         1, 9 },
        { "abcdefgh/a",         1, 8 },
        { "abcdefgh/a/",        1, 10 },
        { "abcdefgh/ab",        1, 8 },
        { "abcdefgh/ab/",       1, 11 },
        { "abcdefgh/abc",       1, 8 },
        { "abcdefgh/abc/",      1, 12 },
        { "abcdefgh/abcd",      1, 8 },
        { "abcdefgh/abcd/",     1, 13 },
        { "abcdefgh/abcde",     1, 8 },
        { "abcdefgh/abcde/",    1, 14 },
        { "abcdefgh/abcdef",    1, 8 },
        { "abcdefgh/abcdef/",   1, 15 },
        { "abcdefgh/abcdefg",   1, 8 },
        { "abcdefgh/abcdefg/",  1, 16 },
        { "abcdefgh/abcdefgh",  1, 8 },
        { "abcdefgh/abcdefgh/", 1, 17 },
    };

    char buffer[32];
    for (size_t a = 0; a < sizeof(long); ++a)
        for (size_t t = 0; t < (sizeof(table) / sizeof(table[0])); ++t) {
            strcpy(&buffer[a], table[t].value);

            char *offset = strrchr_ptr(&buffer[a], '/');
            if (table[t].match == 0)
                cr_assert_null(offset);
            else {
                cr_assert_eq(table[t].match, 1);
                cr_assert_eq(table[t].last_offset, (offset - &buffer[a]));
            }

            cr_assert_eq(offset, strrchr_ptr(&buffer[a], 0xFFFFFF00 | '/'));
        }
}

Test(strrchr, bionic)
{
    int seek_char = 'M';
    struct bionic_test_state state = bionic_test_state_new(1024);
    for (size_t i = 1; i < state.n; ++i)
        for (bionic_test_state_begin_iters(&state); bionic_test_state_has_next_iter(&state); bionic_test_state_next_iter(&state)) {
            if (~seek_char > 0)
                memset(state.ptr1, ~seek_char, state.lengths[i]);
            else
                memset(state.ptr1, '\1', state.lengths[i]);
            state.ptr1[state.lengths[i] - 1] = '\0';

            size_t position = random() % state.max_length;
            char *expected;
            if (position >= state.lengths[i] - 1) {
                if (seek_char == '\0')
                    expected = state.ptr1 + state.lengths[i] - 1;
                else
                    expected = NULL;
            } else {
                state.ptr1[position] = seek_char;
                expected = state.ptr1 + position;
            }
            cr_assert_eq(strrchr_ptr(state.ptr1, seek_char), expected);
        }
    bionic_test_state_destroy(&state);
}

static void bionic_do_strrchr_test(uint8_t *buffer, size_t length)
{
    if (length >= 1) {
        char value = 32 + (length % 96);
        char search_value = 33 + (length % 96);
        memset(buffer, value, length - 1);
        buffer[length - 1] = '\0';
        cr_assert_null(strrchr_ptr((char *)buffer, search_value)); // Buffer doesn't contain search value
        cr_assert_eq(strrchr_ptr((char *)buffer, '\0'), (char *)&buffer[length - 1]); // Search for null terminator
        if (length >= 2) {
            buffer[0] = search_value;
            cr_assert_eq((char *)&buffer[0], strrchr_ptr((char *)buffer, search_value)); // Search value is first element
            buffer[length - 2] = search_value;
            cr_assert_eq((char *)&buffer[length - 2], strrchr_ptr((char *)buffer, search_value)); // Search value is second to last element (last element is '\0')
        }
    }
}

Test(strrchr, bionic_align)
{
    bionic_run_single_buffer_align_test(1024 * 4, bionic_do_strrchr_test);
}

Test(strrchr, bionic_overread)
{
    bionic_run_single_buffer_overread_test(bionic_do_strrchr_test);
}

Test(strrchr, dietlibc)
{
    const char *str = "foo bar baz";
    cr_assert_eq(strrchr_ptr(str, 'z'), str + 10);
    cr_assert_eq(strrchr_ptr(str, ' '), str + 7);
    cr_assert_null(strrchr_ptr(str, 'x'));
}

static void glibc_do_test(size_t alignment, size_t position, size_t length, int seek_char, int max_char)
{
    alignment &= 7;
    if ((alignment + length) >= glibc_page_size)
        return;

    char *buffer = glibc_buf1;
    for (size_t i = 0; i < length; ++i) {
        buffer[alignment + i] = (random() * random()) & max_char;
        if (buffer[alignment + i] == '\0')
            buffer[alignment + i] = (random() * random()) & max_char;
        if (buffer[alignment + i] == '\0')
            buffer[alignment + i] = 1;
        if ((i > position || position >= length) && buffer[alignment + i] == seek_char)
            buffer[alignment + i] = seek_char + 10 + (random() & 15);
    }
    buffer[alignment + length] = '\0';

    char *expected;
    if (position < length) {
        buffer[alignment + position] = seek_char;
        expected = buffer + alignment + position;
    } else
        expected = (seek_char == 0) ? (buffer + alignment + length) : NULL;
    cr_assert_eq(strrchr_ptr(buffer + alignment, seek_char), expected);
}

Test(strrchr, glibc_random_tests)
{
    glibc_test_string_init();

    unsigned char *ptr = (unsigned char *)(glibc_buf1 + glibc_page_size) - 512;
    for (size_t n = 0; n < 50000; ++n) {
        size_t alignment = random() & 63;
        size_t position = random() & 511;
        if (position + alignment >= 511)
            position = 510 - alignment - (random() & 7);

        size_t length = random() & 511;
        if (position >= length)
            length = position + (random() & 7);
        if (length + alignment >= 512)
            length = 511 - alignment - (random() & 7);

        int seek_char = random() & 255;
        if (seek_char != '\0' && position == length) {
            if (position != 0)
                --position;
            else
                ++length;
        }

        size_t j = length + alignment + 64;
        if (j > 512)
            j = 512;

        for (size_t i = 0; i < j; ++i) {
            if (i == position + alignment)
                ptr[i] = seek_char;
            else if (i == length + alignment)
                ptr[i] = '\0';
            else {
                ptr[i] = random() & 255;
                if (((i > position + alignment && i < length + alignment) || position > length) && ptr[i] == seek_char)
                    ptr[i] = seek_char + 13;
                if (i < length + alignment && ptr[i] == '\0') {
                    ptr[i] = seek_char - 13;
                    if (ptr[i] == '\0')
                        ptr[i] = 140;
                }
            }
        }

        char *expected;
        if (position <= length)
            expected = ((char *)(ptr + position + alignment));
        else if (seek_char == 0)
            expected = ((char *)(ptr + length + alignment));
        else
            expected = NULL;
        cr_assert_eq(strrchr_ptr((char *)(ptr + alignment), seek_char), expected);
    }
}

Test(strrchr, glibc)
{
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(0, 16 << i, 2048, 23, 127);
        glibc_do_test(i, 16 << i, 2048, 23, 127);
    }
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(i, 64, 256, 23, 127);
        glibc_do_test(i, 64, 256, 23, CHAR_MAX);
    }
    for (size_t i = 1; i < 32; ++i) {
        glibc_do_test(0, i, i + 1, 23, 127);
        glibc_do_test(0, i, i + 1, 23, CHAR_MAX);
    }
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(0, 16 << i, 2048, 0, 127);
        glibc_do_test(i, 16 << i, 2048, 0, 127);
    }
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(i, 64, 256, 0, 127);
        glibc_do_test(i, 64, 256, 0, CHAR_MAX);
    }
    for (size_t i = 1; i < 32; ++i) {
        glibc_do_test(0, i, i + 1, 0, 127);
        glibc_do_test(0, i, i + 1, 0, CHAR_MAX);
    }
}

Test(strrchr, mlibc)
{
    const char *str = "This is a sample string";

    cr_assert_eq(strrchr_ptr(str, 's') - str + 1, 18);
}

static void avr_libc_check(const char *str, int character, ssize_t expected)
{
    char *ptr = strrchr_ptr(str, character);
    if (expected == -1)
        cr_assert_null(ptr);
    else
        cr_assert_eq(str + expected, ptr);
}

Test(strrchr, avr_libc)
{
    avr_libc_check("", 1, -1);
    avr_libc_check("", 255, -1);
    avr_libc_check("ABCDEF", 'a', -1);

    avr_libc_check("\001", 1, 0);
    avr_libc_check("\377", 255, 0);
    avr_libc_check("987654321", '7', 2);

    avr_libc_check("12345", 0, 5);
    avr_libc_check("", 0, 0);

    avr_libc_check("00", '0', 1);
    avr_libc_check("abcdabcd", 'b', 5);
    avr_libc_check("***********", '*', 10);

    avr_libc_check("ABCDEF", 'A' + 0x100, 0);
    avr_libc_check("ABCDE\377", ~0, 5);
    avr_libc_check("+", ~0xFF, 1);

    avr_libc_check("................................................................................................................................................................................................................................................................*...............", '*', 256);
}

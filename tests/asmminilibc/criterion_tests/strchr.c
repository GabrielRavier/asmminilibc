/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strchr
*/

#include "bionic_buffer.h"
#include "glibc.h"
#include <string.h>
#include <criterion/criterion.h>
#include "bionic_test_state.h"
#include <unistd.h>
#include <limits.h>

static char *(*volatile strchr_ptr)(const char *str, int character) = strchr;
static char *(*volatile strrchr_ptr)(const char *str, int character) = strrchr;

static char *known_strchr(char *buffer, int character)
{
    do {
        if ((unsigned char)*buffer == (unsigned char)character)
            return buffer;
    } while (*buffer++ != '\0');
    return NULL;
}

static void do_strchr_test(char *buffer, int character)
{
    cr_assert_eq(known_strchr(buffer, character), strchr_ptr(buffer, character));
}

Test(strchr, escape)
{
    char str[] = "abcdef";

    cr_assert_eq(strchr_ptr(str, 'a'), str);
    cr_assert_eq(strchr_ptr(str, 'b'), str + 1);
    cr_assert_eq(strchr_ptr(str, 'c'), str + 2);
    cr_assert_null(strchr_ptr(str, 'g'));
}

Test(strchr, netbsd)
{
    const char *table[] = {
        "",
        "a",
        "aa",
        "abc",
        "abcd",
        "abcde",
        "abcdef",
        "abcdefg",
        "abcdefgh",

        "/",
        "//",
        "/a",
        "/a/",
        "/ab",
        "/ab/",
        "/abc",
        "/abc/",
        "/abcd",
        "/abcd/",
        "/abcde",
        "/abcde/",
        "/abcdef",
        "/abcdef/",
        "/abcdefg",
        "/abcdefg/",
        "/abcdefgh",
        "/abcdefgh/",

        "a/",
        "a//",
        "a/a",
        "a/a/",
        "a/ab",
        "a/ab/",
        "a/abc",
        "a/abc/",
        "a/abcd",
        "a/abcd/",
        "a/abcde",
        "a/abcde/",
        "a/abcdef",
        "a/abcdef/",
        "a/abcdefg",
        "a/abcdefg/",
        "a/abcdefgh",
        "a/abcdefgh/",

        "ab/",
        "ab//",
        "ab/a",
        "ab/a/",
        "ab/ab",
        "ab/ab/",
        "ab/abc",
        "ab/abc/",
        "ab/abcd",
        "ab/abcd/",
        "ab/abcde",
        "ab/abcde/",
        "ab/abcdef",
        "ab/abcdef/",
        "ab/abcdefg",
        "ab/abcdefg/",
        "ab/abcdefgh",
        "ab/abcdefgh/",

        "abc/",
        "abc//",
        "abc/a",
        "abc/a/",
        "abc/ab",
        "abc/ab/",
        "abc/abc",
        "abc/abc/",
        "abc/abcd",
        "abc/abcd/",
        "abc/abcde",
        "abc/abcde/",
        "abc/abcdef",
        "abc/abcdef/",
        "abc/abcdefg",
        "abc/abcdefg/",
        "abc/abcdefgh",
        "abc/abcdefgh/",

        "abcd/",
        "abcd//",
        "abcd/a",
        "abcd/a/",
        "abcd/ab",
        "abcd/ab/",
        "abcd/abc",
        "abcd/abc/",
        "abcd/abcd",
        "abcd/abcd/",
        "abcd/abcde",
        "abcd/abcde/",
        "abcd/abcdef",
        "abcd/abcdef/",
        "abcd/abcdefg",
        "abcd/abcdefg/",
        "abcd/abcdefgh",
        "abcd/abcdefgh/",

        "abcde/",
        "abcde//",
        "abcde/a",
        "abcde/a/",
        "abcde/ab",
        "abcde/ab/",
        "abcde/abc",
        "abcde/abc/",
        "abcde/abcd",
        "abcde/abcd/",
        "abcde/abcde",
        "abcde/abcde/",
        "abcde/abcdef",
        "abcde/abcdef/",
        "abcde/abcdefg",
        "abcde/abcdefg/",
        "abcde/abcdefgh",
        "abcde/abcdefgh/",

        "abcdef/",
        "abcdef//",
        "abcdef/a",
        "abcdef/a/",
        "abcdef/ab",
        "abcdef/ab/",
        "abcdef/abc",
        "abcdef/abc/",
        "abcdef/abcd",
        "abcdef/abcd/",
        "abcdef/abcde",
        "abcdef/abcde/",
        "abcdef/abcdef",
        "abcdef/abcdef/",
        "abcdef/abcdefg",
        "abcdef/abcdefg/",
        "abcdef/abcdefgh",
        "abcdef/abcdefgh/",

        "abcdefg/",
        "abcdefg//",
        "abcdefg/a",
        "abcdefg/a/",
        "abcdefg/ab",
        "abcdefg/ab/",
        "abcdefg/abc",
        "abcdefg/abc/",
        "abcdefg/abcd",
        "abcdefg/abcd/",
        "abcdefg/abcde",
        "abcdefg/abcde/",
        "abcdefg/abcdef",
        "abcdefg/abcdef/",
        "abcdefg/abcdefg",
        "abcdefg/abcdefg/",
        "abcdefg/abcdefgh",
        "abcdefg/abcdefgh/",

        "abcdefgh/",
        "abcdefgh//",
        "abcdefgh/a",
        "abcdefgh/a/",
        "abcdefgh/ab",
        "abcdefgh/ab/",
        "abcdefgh/abc",
        "abcdefgh/abc/",
        "abcdefgh/abcd",
        "abcdefgh/abcd/",
        "abcdefgh/abcde",
        "abcdefgh/abcde/",
        "abcdefgh/abcdef",
        "abcdefgh/abcdef/",
        "abcdefgh/abcdefg",
        "abcdefgh/abcdefg/",
        "abcdefgh/abcdefgh",
        "abcdefgh/abcdefgh/",
    };

    char buffer[32];
    for (size_t a = 3; a < 3 + sizeof(long); ++a) {
        buffer[a - 1] = '/';
        buffer[a - 2] = '0';
        buffer[a - 3] = 0xFF;
        for (size_t t = 0; t < (sizeof(table) / sizeof(table[0])); ++t) {
            size_t length = strlen(table[t]) + 1;
            memcpy(&buffer[a], table[t], length);

            buffer[a + length] = '/';

            do_strchr_test(buffer + a, 0);
            do_strchr_test(buffer + a, '/');
            do_strchr_test(buffer + a, 0xFFFFFF00 | '/');

            char *offset;
            while ((offset = known_strchr(buffer + a, '/')) != NULL)
                *offset = 0xFF;

            buffer[a + length] = 0xFF;
            do_strchr_test(buffer + a, 0xFF);
        }
    }
}

Test(strchr, openbsd_regress)
{
    size_t buffer_size = getpagesize();
    char *buffer = malloc(buffer_size);
    cr_assert_not_null(buffer);

    memset(buffer, 0, buffer_size);
    cr_assert_null(strchr_ptr(buffer, 'a'));
    cr_assert_eq(strchr_ptr(buffer, '\0'), buffer);
    cr_assert_null(strrchr_ptr(buffer, 'a'));
    cr_assert_eq(strrchr_ptr(buffer, '\0'), buffer);

    memcpy(buffer, "haystack\xCF\x80", 10);
    cr_assert_eq(strchr_ptr(buffer, 'a'), buffer + 1);
    cr_assert_eq(strchr_ptr(buffer, '\x80'), buffer + 9);
    cr_assert_eq(strchr_ptr(buffer, 0x180), buffer + 9);
    cr_assert_eq(strchr_ptr(buffer, '\0'), buffer + 10);
    cr_assert_eq(strrchr_ptr(buffer, 'a'), buffer + 5);
    cr_assert_eq(strrchr_ptr(buffer, '\xCF'), buffer + 8);
    cr_assert_eq(strrchr_ptr(buffer, 0x3CF), buffer + 8);
    cr_assert_eq(strrchr_ptr(buffer, '\0'), buffer + 10);

    memset(buffer, 'a', buffer_size - 2);
    buffer[0] = 'b';
    buffer[buffer_size - 2] = 'b';
    cr_assert_eq(strchr_ptr(buffer, 'b'), buffer);
    cr_assert_null(strchr_ptr(buffer, 'c'));
    cr_assert_eq(strchr_ptr(buffer, '\0'), buffer + buffer_size - 1);
    cr_assert_eq(strrchr_ptr(buffer, 'b'), buffer + buffer_size - 2);
    cr_assert_null(strrchr_ptr(buffer, 'c'));
    cr_assert_eq(strrchr_ptr(buffer, '\0'), buffer + buffer_size - 1);
}

Test(strchr, linux)
{
    const char *test_string = "abcdefghijkl";
    const char *empty_string = "";

    for (size_t i = 0; i < strlen(test_string) + 1; ++i)
        cr_assert_eq(strchr_ptr(test_string, test_string[i]) - test_string, i);
    cr_assert_eq(strchr_ptr(empty_string, '\0'), empty_string);
    cr_assert_null(strchr_ptr(empty_string, 'a'));
    cr_assert_null(strchr_ptr(empty_string, 'z'));
}

Test(strchr, bionic_with_0)
{
    char buffer[10];
    const char *str = "01234";
    memcpy(buffer, str, strlen(str) + 1);
    cr_assert_eq(strchr_ptr(buffer, '\0'), buffer + strlen(str));
}

Test(strchr, bionic_multiple)
{
    char str[128];
    memset(str, 'a', sizeof(str) - 1);
    str[sizeof(str) - 1] = '\0';

    // Verify that strchr finds the first occurrence of 'a' in a string
    // filled with 'a' characters. Iterate over the string putting
    // non 'a' characters at the front of the string during each iteration
    // and continue to verify that strchr can find the first occurrence
    // properly. The idea is to cover all possible alignments of the location
    // of the first occurrence of the 'a' character and which includes
    // other 'a' characters close by.
    for (size_t i = 0; i < sizeof(str) - 1; ++i) {
        cr_assert_eq(&str[i], strchr_ptr(str, 'a'));
        str[i] = 'b';
    }
}

Test(strchr, bionic)
{
    int seek_char = 'R';

    struct bionic_test_state state = bionic_test_state_new(1024);
    for (size_t i = 1; i < state.n; ++i) {
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

            cr_assert_eq(strchr_ptr(state.ptr1, seek_char), expected);
        }
    }
}

static void bionic_do_strchr_test(uint8_t *buffer, size_t length)
{
    if (length >= 1) {
        char value = 32 + (length % 96);
        char search_value = 33 + (length % 96);
        memset(buffer, value, length - 1);
        buffer[length - 1] = '\0';
        cr_assert_null(strchr_ptr((char *)buffer, search_value)); // Buffer without search value
        cr_assert_eq(strchr_ptr((char *)buffer, '\0'), (char *)&buffer[length - 1]); // Search for \0
        if (length >= 2) {
            buffer[0] = search_value;
            cr_assert_eq((char *)&buffer[0], strchr_ptr((char *)buffer, search_value)); // Search value first char
            buffer[0] = value;
            buffer[length - 2] = search_value;
            cr_assert_eq((char *)&buffer[length - 2], strchr_ptr((char *)buffer, search_value)); // Search value is second to last element. Last element is \0
        }
    }
}

Test(strchr, bionic_align)
{
    bionic_run_single_buffer_align_test(4 * 1024, bionic_do_strchr_test);
}

Test(strchr, bionic_overread)
{
    bionic_run_single_buffer_overread_test(bionic_do_strchr_test);
}

Test(strchr, dietlibc)
{
    char *str = "/opt/diet/bin:/home/leitner/bin:/usr/local/bin:/opt/cross/bin:/usr/local/sbin:/usr/bin:/sbin:/bin:/usr/sbin:/usr/X11R6/bin:/opt/teTeX/bin:/opt/qt-4.3.2/bin:/opt/kde-3.5/bin:/usr/X11R7/bin:/opt/mono/bin";
    cr_assert_eq(strchr_ptr(str, ':'), str + 13);

    str = "fnord";
    cr_assert_eq(strchr_ptr(str, '\0'), str + 5);
    cr_assert_eq(strchr_ptr(str, 'r'), str + 3);
    cr_assert_null(strchr_ptr(str, 'x'));
}

Test(strchr, cosmopolitan_blank)
{
    const char *const blank = "";
    cr_assert_null(strchr_ptr(blank, '#'));
    cr_assert_eq(strchr_ptr(blank, '\0'), blank);
}

Test(strchr, cosmopolitan_text)
{
    char buffer[] = "hellothere";
    cr_assert_str_eq(strchr_ptr(buffer, 't'), "there");
}

Test(strchr, cosmopolitan_sse)
{
    char buffer[] = "hellohellohellohellohellohellohellohellotheretheretheretheretheretheretherethere";
    cr_assert_str_eq(strchr_ptr(buffer, 't'), "theretheretheretheretheretheretherethere");
}

static void glibc_do_test(size_t alignment, size_t position, size_t length, int seek_char, int max_char)
{
    alignment &= 127;
    if ((alignment + length) >= glibc_page_size)
        return;

    char *buffer = glibc_buf1;
    for (size_t i = 0; i < length; ++i) {
        buffer[alignment + i] = 32 + 23 * i % max_char;
        if (buffer[alignment + i] == seek_char)
            ++buffer[alignment + i];
        else if (buffer[alignment + i] == '\0')
            buffer[alignment + i] = 1;
    }
    buffer[alignment + length] = '\0';

    char *expected;
    if (position < length) {
        buffer[alignment + position] = seek_char;
        expected = buffer + alignment + position;
    } else if (seek_char == 0)
        expected = buffer + alignment + length;
    else
        expected = NULL;
    cr_assert_eq(strchr_ptr(buffer + alignment, seek_char), expected);
}

Test(strchr, glibc_random_tests)
{
    glibc_test_string_init();

    unsigned char *ptr = (unsigned char *)(glibc_buf1 + glibc_page_size - 512);

    for (size_t n = 0; n < 100000; ++n) {
        size_t alignment = random() & 15;
        size_t position = random() & 511;
        int seek_char = random() & 255;
        if (position + alignment >= 511)
            position = 510 - alignment - (random() & 7);
        size_t length = random() & 511;
        if ((position == length && seek_char) || (position > length && (random() & 1)))
            length = position + 1 + (random() & 7);
        if (length + alignment >= 512)
            length = 511 - alignment - (random() & 7);
        if (position == length && seek_char)
            length = position + 1;
        size_t j = (position > length ? position : length) + alignment + 64;
        if (j > 512)
            j = 512;

        for (size_t i = 0; i < j; ++i) {
            if (i == position + alignment)
                ptr[i] = seek_char;
            else if (i == length + alignment)
                ptr[i] = '\0';
            else {
                ptr[i] = random() & 255;
                if (i < position + alignment && ptr[i] == seek_char)
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
            expected = (char *)(ptr + position + alignment);
        else if (seek_char == 0)
            expected = (char *)(ptr + length + alignment);
        else
            expected = NULL;
        cr_assert_eq(strchr_ptr((char *)(ptr + alignment), seek_char), expected);
    }
}

Test(strchr, glibc_check1)
{
    glibc_test_string_init();

    char str[] __attribute__((aligned(16))) = "\xFF";
    char c = '\xFE';
    cr_assert_null(strchr_ptr(str, c));
}

Test(strchr, glibc)
{
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(0, 16 << i, 2048, 23, 127);
        glibc_do_test(1, 16 << i, 2048, 23, 127);
    }
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(0, 16 << i, 4096, 23, 127);
        glibc_do_test(i, 16 << i, 4096, 23, 127);
    }
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(i, 64, 256, 23, 127);
        glibc_do_test(i, 64, 256, 23, CHAR_MAX);
    }
    for (size_t i = 0; i < 8; ++i) {
        glibc_do_test(16 * i, 64, 256, 23, 127);
        glibc_do_test(16 * i, 64, 256, 23, CHAR_MAX);
    }
    for (size_t i = 0; i < 32; ++i) {
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
    for (size_t i = 0; i < 8; ++i) {
        glibc_do_test(i * 16, 256, 512, 0, 127);
        glibc_do_test(i * 16, 256, 512, 0, CHAR_MAX);
    }
    for (size_t i = 0; i < 32; ++i) {
        glibc_do_test(0, i, i + 1, 0, 127);
        glibc_do_test(0, i, i + 1, 0, CHAR_MAX);
    }
}

static void picolibc_check(char *(*func)(const char *, int), int needle, ssize_t expected)
{
    const char *haystack = "hello world";
    char *ptr = func(haystack, needle);
    size_t result = -1;
    if (ptr != NULL)
        result = ptr - haystack;
    cr_assert_eq(result, expected);
}

static void picolibc_many_check(char *(*func)(const char *, int), int needle, size_t expected)
{
    picolibc_check(func, needle, expected);
    picolibc_check(func, needle | 0xFFFFFF00, expected);
    picolibc_check(func, needle | 0x00000100, expected);
    picolibc_check(func, needle | 0x80000000, expected);
}

Test(strchr, picolibc)
{
    picolibc_many_check(strchr_ptr, 'h', 0);
    picolibc_many_check(strrchr_ptr, 'h', 0);
    picolibc_many_check(strchr_ptr, 'l', 2);
    picolibc_many_check(strrchr_ptr, 'l', 9);
    picolibc_many_check(strchr_ptr, '\0', 11);
    picolibc_many_check(strrchr_ptr, '\0', 11);
}

static void avr_libc_check(const char *str, int ch, ssize_t expected)
{
    char *result = strchr_ptr(str, ch);
    if (expected == -1)
        cr_assert_null(result);
    else
        cr_assert_eq(result, str + expected);
}

Test(strchr, avr_libc)
{
    avr_libc_check("", 1, -1);
    avr_libc_check("", 255, -1);
    avr_libc_check("ABCDEF", 'a', -1);

    avr_libc_check("\001", 1, 0);
    avr_libc_check("\377", 255, 0);
    avr_libc_check("987654321", '7', 2);

    avr_libc_check("12345", 0, 5);
    avr_libc_check("", 0, 0);

    avr_libc_check("abcdabcd", 'b', 1);

    avr_libc_check("ABCDEF", 'A' + 0x100, 0);
    avr_libc_check("ABCDE\377", ~0, 5);

    avr_libc_check("................................................................................................................................................................................................................................................................*...............", '*', 256);
}

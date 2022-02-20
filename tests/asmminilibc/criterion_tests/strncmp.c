/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strncmp
*/

#include "glibc.h"
#include <string.h>
#include <criterion/criterion.h>
#include "bionic_test_state.h"
#include "glibc.h"
#include <limits.h>

static int (*volatile strncmp_ptr)(const char *, const char *, size_t) = strncmp;

static int known_strncmp(const char *str1, const char *str2, size_t n)
{
    size_t i = 0;

    do {
        if (n == 0)
            return 0;
        if (str1[i] != str2[i])
            return (unsigned char)str1[i] - (unsigned char)str2[i];
        --n;
    } while (str1[i++] != '\0');
    return 0;
}

static void check_result(const char *str1, const char *str2, size_t n, int expected_result)
{
    int result = strncmp_ptr(str1, str2, n);
    // Test on 64-bit architectures where the ABI requires that the callee promote, whether this promotion has in fact been done
    asm("" : "=g" (result) : "0" (result));
    if (expected_result == 0)
        cr_assert_eq(result, 0);
    if (expected_result < 0)
        cr_assert(result < 0);
    if (expected_result > 0)
        if (!(result > 0))
            cr_assert_fail("result > 0 is false !!! result is %d for compared strings '%s' and '%s' with length %zu", result, str1, str2, n);
}

Test(strncmp, bionic)
{
    __auto_type state = bionic_test_state_new(1024);

    for (size_t i = 0; i < state.n; ++i)
        for (bionic_test_state_begin_iters(&state); bionic_test_state_has_next_iter(&state); bionic_test_state_next_iter(&state)) {
            memset(state.ptr1, 'v', state.max_length);
            memset(state.ptr2, 'n', state.max_length);
            state.ptr1[state.lengths[i] - 1] = '\0';
            state.ptr2[state.lengths[i] - 1] = '\0';

            size_t position = 1 + (random() % (state.max_length - 1));
            if (position >= state.lengths[i] - 1) {
                memcpy(state.ptr1, state.ptr2, state.lengths[i]);
            } else {
                memcpy(state.ptr1, state.ptr2, position);
                if (state.ptr1[position] == state.ptr2[position]) {
                    state.ptr1[position + 1] = '\0';
                    state.ptr2[position + 1] = '\0';
                }
            }
            check_result(state.ptr1, state.ptr2, state.lengths[i], known_strncmp(state.ptr1, state.ptr2, state.lengths[i]));
        }
    bionic_test_state_destroy(&state);
}

Test(strncmp, glibc_check1)
{
    glibc_test_string_init();

    char *str1 = glibc_buf1 + 0xB2C;
    char *str2 = glibc_buf1 + 0xFD8;

    strcpy(str1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs");
    strcpy(str2, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkLMNOPQRSTUV");

    // Potential overflow
    str1[4] = CHAR_MAX;
    str2[4] = CHAR_MIN;

    for (size_t offset = 0; offset < 6; ++offset)
        for (size_t i = 0; i < 80; ++i)
            check_result(str1 + offset, str2 + offset, i, known_strncmp(str1 + offset, str2 + offset, i));
}

static void glibc_do_page_test(size_t offset1, size_t offset2, char *str2)
{
    if (offset1 >= glibc_page_size || offset2 >= glibc_page_size)
        return;

    char *str1 = glibc_buf1 + offset1;
    str2 += offset2;

    check_result(str1, str2, glibc_page_size, -*str1);
    check_result(str2, str1, glibc_page_size, *str1);
}

Test(strncmp, glibc_check2)
{
    glibc_test_string_init();

    char *str1 = glibc_buf1;
    size_t i;
    for (i = 0; i < glibc_page_size - 1; ++i)
        str1[i] = 23;
    str1[i] = 0;

    char *str2 = strdup(str1);
    cr_assert_not_null(str2);
    for (i = 0; i < 64; ++i)
        glibc_do_page_test(3988 + i, 2636, str2);
    free(str2);
}

Test(strncmp, glibc_check3)
{
    glibc_test_string_init();

    size_t size = 32 * 4;
    char *str1 = glibc_buf1 + (GLIBC_BUF1PAGES - 1) * glibc_page_size;
    char *str2 = glibc_buf2 + (GLIBC_BUF1PAGES - 1) * glibc_page_size;

    memset(str1, 'a', glibc_page_size);
    memset(str2, 'a', glibc_page_size);
    str1[glibc_page_size - 1] = 0;

    for (size_t s = 99; s <= size; ++s)
        for (size_t s1a = 30; s1a < 32; ++s1a)
            for (size_t s2a = 30; s2a < 32; ++s2a) {
                char *s1p = str1 + (glibc_page_size - s) - s1a;
                char *s2p = str2 + (glibc_page_size - s) - s2a;

                check_result(s1p, s2p, s, known_strncmp(s1p, s2p, s));
            }
}

static void glibc_do_test(size_t alignment1, size_t alignment2, size_t length, size_t n, int max_char, int expected_result)
{
    if (n == 0)
        return;

    alignment1 &= 63;
    if (alignment1 + (n + 1) >= glibc_page_size)
        return;

    alignment2 &= 63;
    if (alignment2 + (n + 1) >= glibc_page_size)
        return;

    char *str1 = glibc_buf1 + alignment1;
    char *str2 = glibc_buf2 + alignment2;

    for (size_t i = 0; i < n; ++i) {
        str1[i] = 1 + (23 * i % max_char);
        str2[i] = str1[i];
    }

    str1[n] = 24 + expected_result;
    str2[n] = 23;
    str1[length] = 0;
    str2[length] = 0;
    if (expected_result < 0)
        str2[length] = 32;
    else if (expected_result > 0)
        str1[length] = 64;
    if (length >= n)
        str2[n - 1] -= expected_result;

    check_result(str1, str2, n, expected_result);
}

static void glibc_do_test_limit(size_t alignment1, size_t alignment2, size_t length, size_t n, int max_char, int expected_result)
{
    if (n == 0) {
        char *str1 = glibc_buf1 + glibc_page_size;
        char *str2 = glibc_buf1 + glibc_page_size;

        check_result(str1, str2, n, 0);
    }

    alignment1 &= 15;
    alignment2 &= 15;
    size_t alignment_n = (glibc_page_size - n) & 15;

    char *str1 = glibc_buf1 + glibc_page_size - n;
    char *str2 = glibc_buf2 + glibc_page_size - n;

    if (alignment1 < alignment_n)
        str1 = str1 - (alignment_n - alignment1);
    if (alignment2 < alignment_n)
        str2 = str2 - (alignment_n - alignment1);

    for (size_t i = 0; i < n; ++i) {
        str1[i] = 1 + 23 * i % max_char;
        str2[i] = str1[i];
    }

    if (length < n) {
        str1[length] = 0;
        str2[length] = 0;
        if (expected_result < 0)
            str2[length] = 32;
        else if (expected_result > 0)
            str1[length] = 64;
    }

    check_result(str1, str2, n, expected_result);
}

Test(strncmp, glibc)
{
    glibc_test_string_init();

    for (size_t i = 0; i < 16; ++i) {
        glibc_do_test(0, 0, 8, i, 127, 0);
        glibc_do_test(0, 0, 8, i, 127, -1);
        glibc_do_test(0, 0, 8, i, 127, 1);
        glibc_do_test(i, i, 8, i, 127, 0);
        glibc_do_test(i, i, 8, i, 127, -1);
        glibc_do_test(i, i, 8, i, 127, 1);
        glibc_do_test(i, 2 * i, 8, i, 127, 0);
        glibc_do_test(2 * i, i, 8, i, 127, -1);
        glibc_do_test(i, 3 * i, 8, i, 127, 1);
        glibc_do_test(0, 0, 8, i, 255, 0);
        glibc_do_test(0, 0, 8, i, 255, -1);
        glibc_do_test(0, 0, 8, i, 255, 1);
        glibc_do_test(i, i, 8, i, 255, 0);
        glibc_do_test(i, i, 8, i, 255, -1);
        glibc_do_test(i, i, 8, i, 255, 1);
        glibc_do_test(i, 2 * i, 8, i, 255, 0);
        glibc_do_test(2 * i, i, 8, i, 255, -1);
        glibc_do_test(i, 3 * i, 8, i, 255, 1);
    }
    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(0, 0, 8 << i, 16 << i, 127, 0);
        glibc_do_test(0, 0, 8 << i, 16 << i, 127, 1);
        glibc_do_test(0, 0, 8 << i, 16 << i, 127, -1);
        glibc_do_test(0, 0, 8 << i, 16 << i, 255, 0);
        glibc_do_test(0, 0, 8 << i, 16 << i, 255, 1);
        glibc_do_test(0, 0, 8 << i, 16 << i, 255, -1);
        glibc_do_test(8 - i, 2 * i, 8 << i, 16 << i, 127, 0);
        glibc_do_test(8 - i, 2 * i, 8 << i, 16 << i, 127, 1);
        glibc_do_test(2 * i, i, 8 << i, 16 << i, 255, 0);
        glibc_do_test(2 * i, i, 8 << i, 16 << i, 255, 1);
    }

    glibc_do_test_limit(0, 0, 0, 0, 127, 0);
    glibc_do_test_limit(4, 0, 21, 20, 127, 0);
    glibc_do_test_limit(0, 4, 21, 20, 127, 0);
    glibc_do_test_limit(8, 0, 25, 24, 127, 0);
    glibc_do_test_limit(0, 8, 25, 24, 127, 0);

    for (size_t i = 0; i < 8; ++i) {
        glibc_do_test_limit(0, 0, 17 - i, 16 - i, 127, 0);
        glibc_do_test_limit(0, 0, 17 - i, 16 - i, 255, 0);
        glibc_do_test_limit(0, 0, 15 - i, 16 - i, 127, 0);
        glibc_do_test_limit(0, 0, 15 - i, 16 - i, 127, 1);
        glibc_do_test_limit(0, 0, 15 - i, 16 - i, 127, -1);
        glibc_do_test_limit(0, 0, 15 - i, 16 - i, 255, 0);
        glibc_do_test_limit(0, 0, 15 - i, 16 - i, 255, 1);
        glibc_do_test_limit(0, 0, 15 - i, 16 - i, 255, -1);
    }
}

Test(strncmp, glibc_random)
{
    glibc_test_string_init();

    unsigned char *ptr1 = (unsigned char *)(glibc_buf1 + glibc_page_size - 512);
    unsigned char *ptr2 = (unsigned char *)(glibc_buf2 + glibc_page_size - 512);

    for (size_t n = 0; n < 50000; ++n) {
        size_t alignment1 = random() & 31;

        size_t alignment2;
        if (random() & 1)
            alignment2 = random() & 31;
        else
            alignment2 = alignment1 + (random() & 24);

        size_t position = random() & 511;
        size_t size = random() & 511;
        size_t j = alignment1 > alignment2 ? alignment1 : alignment2;

        if (position + j >= 511)
            position = 510 - j - (random() & 7);

        size_t length1 = random() & 511;
        if (position >= length1 && (random() & 1))
            length1 = position + (random() & 7);
        if (length1 + j >= 512)
            length1 = 511 - j - (random() & 7);

        size_t length2;
        if (position >= length1)
            length2 = length1;
        else
            length2 = length1 + (length1 != 511 - j ? random() % (511 - j - length1) : 0);

        j = (position > length2 ? position : length2) + alignment1 + 64;
        if (j > 512)
            j = 512;

        for (size_t i = 0; i < j; ++i) {
            ptr1[i] = random() & 255;
            if (i < length1 + alignment1 && ptr1[i] == '\0') {
                ptr1[i] = random() & 255;
                if (ptr1[i] == '\0')
                    ptr1[i] = 1 + (random() & 127);
            }
        }
        for (size_t i = 0; i < j; ++i) {
            ptr2[i] = random() & 255;
            if (i < length2 + alignment2 && ptr2[i] == '\0') {
                ptr2[i] = random() & 255;
                if (ptr2[i] == '\0')
                    ptr2[i] = 1 + (random() & 127);
            }
        }

        int expected_result = 0;
        memcpy(ptr2 + alignment2, ptr1 + alignment1, position);

        if (position < length1) {
            if (ptr2[alignment2 + position] == ptr1[alignment1 + position]) {
                ptr2[alignment2 + position] = random() & 255;
                if (ptr2[alignment2 + position] == ptr1[alignment1 + position])
                    ptr2[alignment2 + position] = ptr1[alignment1 + position] + 3 + (random() & 127);
            }

            if (position < size)
                expected_result = (ptr1[alignment1 + position] < ptr2[alignment2 + position]) ? -1 : 1;
        }
        ptr1[length1 + alignment1] = 0;
        ptr2[length2 + alignment2] = 0;

        check_result((char *)(ptr1 + alignment1), (char *)(ptr2 + alignment2), size, expected_result);
    }
}

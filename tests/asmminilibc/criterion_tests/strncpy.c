/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strncpy
*/

#include <string.h>
#include <criterion/criterion.h>
#include "bionic_test_state.h"
#include "glibc.h"
#include <limits.h>

static char *(*volatile strncpy_ptr)(char *, const char *, size_t) = strncpy;

static void check_result(char *dest, const char *src, size_t n)
{
    cr_assert_eq(strncpy_ptr(dest, src, n), dest);
}

Test(strncpy, bionic)
{
    __auto_type state = bionic_test_state_new(1024);

    for (bionic_test_state_begin_iters(&state); bionic_test_state_has_next_iter(&state); bionic_test_state_next_iter(&state)) {
        // Fill the string with some random value
        memset(state.ptr1, 'K', state.max_length);

        // Random size for source buffer
        size_t ptr1_length = random() % state.max_length;
        state.ptr1[ptr1_length] = '\0';

        // Copy ptr1 to ptr, so we can ensure later ptr1 doesn't get modified
        memcpy(state.ptr, state.ptr1, state.max_length);

        // Init ptr2 to some random other value
        memset(state.ptr2, '\1', state.max_length);

        // Choose random amount of data to copy
        size_t copy_length = random() % state.max_length;

        // Set the second half of ptr to the excepted pattern in ptr2
        memset(state.ptr + state.max_length, '\1', state.max_length);
        memcpy(state.ptr + state.max_length, state.ptr1, copy_length);

        size_t expected_end;
        if (copy_length > ptr1_length) {
            memset(state.ptr + state.max_length + ptr1_length, '\0', copy_length - ptr1_length);
            expected_end = ptr1_length;
        } else
            expected_end = copy_length;

        (void)expected_end;
        check_result(state.ptr2, state.ptr1, copy_length);

        cr_assert_eq(memcmp(state.ptr1, state.ptr, state.max_length), 0);
        cr_assert_eq(memcmp(state.ptr2, state.ptr + state.max_length, state.max_length), 0);
    }
}

static void glibc_do_one_test(char *destination, const char *source, size_t length, size_t n)
{
    check_result(destination, source, n);

    cr_assert_eq(memcmp(destination, source, (length > n ? n : length)), 0);

    if (n > length)
        for (size_t i = length; i < n; ++i)
            cr_assert_eq(destination[i], '\0');
}

static void glibc_do_test(size_t alignment1, size_t alignment2, size_t length, size_t n, int max_char)
{
    alignment1 &= 7;
    if ((alignment1 + length) >= glibc_page_size)
        return;
    
    alignment2 &= 7;
    if ((alignment2 + length) >= glibc_page_size)
        return;

    char *str1 = glibc_buf1 + alignment1;
    char *str2 = glibc_buf2 + alignment2;

    for (size_t i = 0; i < length; ++i)
        str1[i] = 32 + 23 * i % (max_char - 32);
    str1[length] = '\0';
    for (size_t i = length + 1; (i + alignment1) < glibc_page_size && i < length + 64; ++i)
        str1[i] = 32 + 32 * i % (max_char - 32);

    glibc_do_one_test(str2, str1, length, n);
}

Test(strncpy, glibc)
{
    glibc_test_string_init();

    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(i, i, 16, 16, 127);
        glibc_do_test(i, i, 16, 16, CHAR_MAX);
        glibc_do_test(i, 2 * i, 16, 16, 127);
        glibc_do_test(2 * i, i, 16, 16, CHAR_MAX);
        glibc_do_test(8 - i, 2 * i, 1 << i, 2 << i, 127);
        glibc_do_test(2 * i, 8 - i, 2 << i, 1 << i, 127);
        glibc_do_test(8 - i, 2 * i, 1 << i, 2 << i, CHAR_MAX);
        glibc_do_test(2 * i, 8 - i, 2 << i, 1 << i, CHAR_MAX);
    }

    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(0, 0, 4 << i, 8 << i, 127);
        glibc_do_test(0, 0, 16 << i, 8 << i, 127);
        glibc_do_test(8 - i, 2 * i, 4 << i, 8 << i, 127);
        glibc_do_test(8 - i, 2 * i, 16 << i, 8 << i, 127);
    }
}

Test(strncpy, glibc_random)
{
    glibc_test_string_init();

    unsigned char *ptr1 = (unsigned char *)(glibc_buf1 + glibc_page_size) - 512;
    unsigned char *ptr2 = (unsigned char *)(glibc_buf2 + glibc_page_size) - 512;

    for (size_t n = 0; n < 100000; ++n) {
        size_t mode = random();

        size_t size, alignment1, alignment2, j, length;
        if (mode & 1) {
            size = random() & 255;
            alignment1 = 512 - size - (random() & 15);
            
            if (mode & 2)
                alignment2 = alignment1 - (random() & 24);
            else
                alignment2 = alignment1 - (random() & 31);

            if (mode & 4) {
                j = alignment1;
                alignment1 = alignment2;
                alignment2 = j;
            }

            if (mode & 8)
                length = size - (random() & 31);
            else
                length = 512;
            if (length >= 512)
                length = random() & 511;
        } else {
            alignment1 = random() & 31;

            if (mode & 2)
                alignment2 = random() & 31;
            else
                alignment2 = alignment1 + (random() & 24);

            length = random() & 511;

            j = alignment1;
            if (alignment2 > j)
                j = alignment2;

            if (mode & 4) {
                size = random() & 511;
                if (size + j > 512)
                    size = 512 - j - (random() & 31);
            } else
                size = 512 - j;

            if ((mode & 8) && length + j >= 512)
                length = 512 - j - (random() & 7);
        }

        j = length + alignment1 + 64;
        if (j > 512)
            j = 512;

        for (size_t i = 0; i < j; ++i) {
            if (i == length + alignment1)
                ptr1[i] = '\0';
            else {
                ptr1[i] = random() & CHAR_MAX;
                if (i >= alignment1 && i < length + alignment1 && ptr1[i] == '\0')
                    ptr1[i] = (random() & 127) + 3;
            }
        }

        memset(ptr2 - 64, '\1', 512 + 64);
        check_result((char *)(ptr2 + alignment2), (char *)(ptr1 + alignment1), size);

        for (j = 0; j < alignment2 + 64; ++j)
            cr_assert_eq(ptr2[j - 64], '\1');

        j = alignment2 + length + 1;
        if (size + alignment2 > j)
            j = size + alignment2;

        for (; j < 512; ++j)
            cr_assert_eq(ptr2[j], '\1');
        for (j = alignment2 + length + 1; j < alignment2 + size; ++j)
            cr_assert_eq(ptr2[j], '\0');

        j = length + 1;
        if (size < j)
            j = size;
        cr_assert_eq(memcmp(ptr1 + alignment1, ptr2 + alignment2, j), 0);
    }
}

Test(strncpy, glibc_page)
{
    glibc_test_string_init();

    static const size_t MAX_OFFSET = 64;

    // str1 is at the MAX_OFFSET from the edge of glibc_buf1's last page
    char *str1 = glibc_buf1 + GLIBC_BUF1PAGES * glibc_page_size - MAX_OFFSET;

    // str2 needs room to put a string with size MAX_OFFSET + 1 at str2 + (MAX_OFFSET - 1)
    char *str2 = glibc_buf2 + glibc_page_size - MAX_OFFSET * 2;

    memset(str1, 'a', MAX_OFFSET - 1);
    str1[MAX_OFFSET - 1] = '\0';

    // Both strings are located right before a PROT_NONE page, we loop over all the possible offsets that get as near to it as possible (especially with sizes that would get outside of valid memory, but with a '\0' located before that
    for (size_t offset1 = 0; offset1 < MAX_OFFSET; ++offset1)
        for (size_t offset2 = 0; offset2 < MAX_OFFSET; ++offset2)
            glibc_do_one_test(str2 + offset2, str1 + offset1, MAX_OFFSET - offset1 - 1, MAX_OFFSET + (MAX_OFFSET - offset2));
}

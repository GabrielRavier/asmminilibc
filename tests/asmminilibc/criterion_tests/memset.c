/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests memset
*/

#include <string.h>
#include <criterion/criterion.h>
#include "littlekernel_fill_buffer.h"
#include "escape_check_zero.h"
#include "bionic_test_state.h"
#include "bionic_buffer.h"
#define GLIBC_MIN_PAGE_SIZE 131072
#include "glibc.h"
#include <unistd.h>
#include <sys/stat.h>
#include <malloc.h>
#include <limits.h>

static void *(*volatile memset_ptr)(void *, int, size_t) = memset;

static void checked_memset(void *ptr, int ch, size_t len)
{
    cr_assert_eq(memset_ptr(ptr, ch, len), ptr);
}

static void simple_set(char *target, char character, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        target[i] = character;
}

Test(memset, littlekernel)
{
    static const size_t MAX_SIZE = 256;
    static const size_t BUFFER_SIZE = 2 * 1024 * 1024;

    uint8_t *dst = memalign(64, BUFFER_SIZE + 256);
    uint8_t *dst2 = memalign(64, BUFFER_SIZE + 256);
    cr_assert_not_null(dst);
    cr_assert_not_null(dst2);

    for (size_t dst_align = 0; dst_align < 64; ++dst_align)
        for (size_t size = 0; size < MAX_SIZE; ++size) {
            for (int ch = -1; ch < 257; ++ch) {
                littlekernel_fill_buffer(dst, MAX_SIZE * 2, 123514);
                littlekernel_fill_buffer(dst2, MAX_SIZE * 2, 123514);

                simple_set((char *)dst + dst_align, ch, size);
                checked_memset((char *)dst2 + dst_align, ch, size);

                cr_assert_eq(memcmp(dst, dst2, MAX_SIZE * 2), 0);
                if (ch > 2)
                    ch += random() % 10;
            }
            if (size > 2)
                size += random() % 10;
        }
}

Test(memset, escape)
{
    uint8_t buffer[16] = {0};

    checked_memset(buffer, ' ', 10);
    cr_assert_str_eq((char *)buffer, "          ");

    checked_memset(buffer, 'a', 10);
    cr_assert_str_eq((char *)buffer, "aaaaaaaaaa");

    checked_memset(buffer, 'a', 1);
    checked_memset(buffer + 1, 'b', 2);
    checked_memset(buffer + 3, 'c', 3);
    checked_memset(buffer + 6, 'd', 4);
    cr_assert_str_eq((char *)buffer, "abbcccdddd");

    checked_memset(buffer, 'X', 0);
    cr_assert_str_eq((char *)buffer, "abbcccdddd");

    checked_memset(buffer, 'X', 5);
    cr_assert_str_eq((char *)buffer, "XXXXXcdddd");

    checked_memset(buffer, 'X', 6);
    cr_assert_str_eq((char *)buffer, "XXXXXXdddd");

    checked_memset(buffer, 'X', 7);
    cr_assert_str_eq((char *)buffer, "XXXXXXXddd");

    checked_memset(buffer, 'X', 8);
    cr_assert_str_eq((char *)buffer, "XXXXXXXXdd");

    for (size_t i = 0; i < 8; ++i)
        for (size_t j = 0; j < 8; ++j) {
            checked_memset(buffer, 0xFF, 16);
            checked_memset(buffer + i, 0, j);
            escape_check_zero(buffer, i, j, 16);
        }
}

static bool netbsd_check(char *buffer, size_t length, char ch)
{
    for (size_t i = 0; i < length; ++i)
        if (buffer[i] != ch)
            return false;
    return true;
}

Test(memset, netbsd_array)
{
    char buffer[1024];

    checked_memset(buffer, 0, sizeof(buffer));
    cr_assert(netbsd_check(buffer, sizeof(buffer), 0));

    checked_memset(buffer, 'x', sizeof(buffer));
    cr_assert(netbsd_check(buffer, sizeof(buffer), 'x'));
}

Test(memset, netbsd_return)
{
    char *ptr_one = (char *)0x1;
    char array[2];
    checked_memset(ptr_one, 0, 0);
    checked_memset(array, 2, sizeof(array));
}

Test(memset, netbsd_basic)
{
    long page_size = sysconf(_SC_PAGESIZE);
    char *buffer = malloc(page_size);
    char *ret = malloc(page_size);

    cr_assert_not_null(buffer);
    cr_assert_not_null(ret);

    simple_set(ret, 0, page_size);
    checked_memset(buffer, 0, page_size);
    cr_assert_eq(memcmp(ret, buffer, page_size), 0);

    simple_set(ret, 'x', page_size);
    checked_memset(buffer, 'x', page_size);
    cr_assert_eq(memcmp(ret, buffer, page_size), 0);

    free(ret);
    free(buffer);
}

Test(memset, netbsd_nonzero)
{
    long page_size = sysconf(_SC_PAGESIZE);

    char *buffer = malloc(page_size);
    cr_assert_not_null(buffer);

    for (size_t i = 0x21; i < 0x7F; ++i) {
        checked_memset(buffer, i, page_size);
        cr_assert(netbsd_check(buffer, page_size, i));
    }
    free(buffer);
}

Test(memset, netbsd_zero_size)
{
    char buffer[1024];

    checked_memset(buffer, 'x', sizeof(buffer));
    cr_assert(netbsd_check(buffer, sizeof(buffer), 'x'));

    checked_memset(buffer + sizeof(buffer) / 2, 0, 0);
    cr_assert(netbsd_check(buffer, sizeof(buffer), 'x'));
}

Test(memset, netbsd_struct)
{
    struct stat stat_buffer;

    stat_buffer.st_dev = 1;
    stat_buffer.st_ino = 2;
    stat_buffer.st_mode = 3;
    stat_buffer.st_nlink = 4;
    stat_buffer.st_uid = 5;
    stat_buffer.st_gid = 6;
    stat_buffer.st_rdev = 7;
    stat_buffer.st_size = 8;
    stat_buffer.st_atime = 9;
    stat_buffer.st_mtime = 10;

    checked_memset(&stat_buffer, 0, sizeof(stat_buffer));

    cr_assert_eq(stat_buffer.st_dev, 0);
    cr_assert_eq(stat_buffer.st_ino, 0);
    cr_assert_eq(stat_buffer.st_mode, 0);
    cr_assert_eq(stat_buffer.st_nlink, 0);
    cr_assert_eq(stat_buffer.st_uid, 0);
    cr_assert_eq(stat_buffer.st_gid, 0);
    cr_assert_eq(stat_buffer.st_rdev, 0);
    cr_assert_eq(stat_buffer.st_size, 0);
    cr_assert_eq(stat_buffer.st_atime, 0);
    cr_assert_eq(stat_buffer.st_mtime, 0);
}

Test(memset, bionic)
{
    struct bionic_test_state state = bionic_test_state_new(64 * 1024);
    char character = 'P';

    for (size_t i = 0; i < (state.n - 1) / 2; ++i)
        for (bionic_test_state_begin_iters(&state); bionic_test_state_has_next_iter(&state); bionic_test_state_next_iter(&state)) {
            checked_memset(state.ptr1, ~character, state.max_length);
            memcpy(state.ptr2, state.ptr1, state.max_length);

            size_t position = random() % (state.max_length - state.lengths[i]);
            for (size_t k = position; k < position + state.lengths[i]; ++k)
                state.ptr1[k] = character;

            checked_memset(state.ptr2 + position, character, state.lengths[i]);
            cr_assert_eq(memcmp(state.ptr1, state.ptr2, state.max_length), 0);
        }
}

static void bionic_do_memset_test(uint8_t *buffer, size_t length)
{
    for (size_t i = 0; i < length; ++i)
        buffer[i] = 0;
    int value = (length % 255) + 1;
    checked_memset(buffer, value, length);
    for (size_t i = 0; i < length; ++i)
        cr_assert_eq(buffer[i], value);
}

Test(memset, bionic_align)
{
    bionic_run_single_buffer_align_test(64 * 1024, bionic_do_memset_test);
}

static void glibc_do_one_test(char *dest, int character, size_t length)
{
    char buffer[length + 2];
    char sentinel = character - 1;
    char *test_buffer = buffer + 1;
    buffer[0] = sentinel;
    buffer[length + 1] = sentinel;

    checked_memset(dest, character, length);
    simple_set(test_buffer, character, length);
    cr_assert_eq(memcmp(dest, test_buffer, length), 0);
    cr_assert_eq(buffer[0], sentinel);
    cr_assert_eq(buffer[length + 1], sentinel);
}

static void glibc_do_test(size_t alignment, int ch, size_t length)
{
    alignment &= 4095;
    if ((alignment + length) > glibc_page_size)
        return;
    glibc_do_one_test(glibc_buf1 + alignment, ch, length);
}

Test(memset, glibc_random)
{
    glibc_test_string_init();

    unsigned char *ptr2 = (unsigned char *)glibc_buf2;

    for (size_t i = 0; i < 65536; ++i)
        ptr2[i] = random() % CHAR_MAX;

    for (size_t n = 0; n < 100000; ++n) {
        size_t size;
        if ((random() & 31) == 0)
            size = 65536;
        else
            size = 512;

        unsigned char *ptr = (unsigned char *)(glibc_buf1 + glibc_page_size) - size;
        size_t length = random() % (size - 1);
        size_t alignment = size - length - (random() & 31);
        if (alignment > size)
            alignment = size - length;
        if ((random() & 7) == 0)
            alignment &= ~63;

        int character;
        if ((random() & 7) == 0)
            character = 0;
        else
            character = random() & CHAR_MAX;

        int other_character = random() & CHAR_MAX;
        if (other_character == character)
            other_character = (character + 1) & CHAR_MAX;

        size_t j = length + alignment + 128;
        if (j > size)
            j = size;

        size_t k;
        if (alignment >= 128)
            k = alignment - 128;
        else
            k = 0;

        for (size_t i = k; i < alignment; ++i)
            ptr[i] = other_character;
        for (size_t i = alignment + length; i < j; ++i)
            ptr[i] = other_character;

        for (size_t i = 0; i < length; ++i) {
            ptr[i + alignment] = ptr2[i];
            if (ptr[i + alignment] == character)
                ptr[i + alignment] = other_character;
        }
        checked_memset((char *)ptr + alignment, character, length);
        size_t i = k;
        for (; i < alignment; ++i)
            cr_assert_eq(ptr[i], other_character);
        for (; i < alignment + length; ++i)
            cr_assert_eq(ptr[i], character);
        for (; i < j; ++i)
            cr_assert_eq(ptr[i], other_character);
    }
}

Test(memset, glibc)
{
    glibc_test_string_init();

    for (int character = -64; character < 130; character += 65) {
        for (size_t i = 0; i < 18; ++i)
            glibc_do_test(0, character, 1 << i);
        for (size_t i = 1; i < 64; ++i) {
            glibc_do_test(i, character, i);
            glibc_do_test(4096 - i, character, i);
            glibc_do_test(4095, character, i);
            if (i & (i - 1))
                glibc_do_test(0, character, i);
        }
        glibc_do_test(1, character, 14);
        glibc_do_test(3, character, 1024);
        glibc_do_test(4, character, 64);
        glibc_do_test(2, character, 25);
    }
}

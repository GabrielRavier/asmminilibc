/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests memcpy
*/

#include <string.h>
#include <criterion/criterion.h>
#include "bionic_test_state.h"
#include "bionic_buffer.h"
#include "glibc.h"

static void *(*volatile memmove_ptr)(void *, const void *, size_t) = memmove;

static void checked_memmove(void *dest, const void *src, size_t len)
{
    cr_assert_eq(memmove_ptr(dest, src, len), dest);
}

Test(memmove, bionic)
{
    __auto_type state = bionic_test_state_new(64 * 1024);

    for (size_t i = 0; i < state.n - 1; ++i) {
        for (bionic_test_state_begin_iters(&state); bionic_test_state_has_next_iter(&state); bionic_test_state_next_iter(&state)) {
            memset(state.ptr1, 'Q', 2 * state.max_length);

            size_t position = random() % (state.max_length - state.lengths[i]);

            memset(state.ptr1, 'R', state.lengths[i]);
            memcpy(state.ptr2, state.ptr1, 2 * state.max_length);
            memcpy(state.ptr, state.ptr1, state.lengths[i]);
            memcpy(state.ptr1 + position, state.ptr, state.lengths[i]);

            checked_memmove(state.ptr2 + position, state.ptr2, state.lengths[i]);
            cr_assert_eq(memcmp(state.ptr2, state.ptr1, 2 * state.max_length), 0);
        }
        i += random() % 15;
    }
    bionic_test_state_destroy(&state);
}

Test(memmove, bionic_cache_size)
{
    size_t length = 600000;
    size_t position = 64;
    static const int MAX_ALIGNMENT = 31;
    static const int ALIGNMENTS[] = {0, 5, 11, 29, 30};

    char *ptr = malloc(length);
    char *ptr1 = malloc(2 * length);
    char *glob_ptr2 = malloc(2 * length + MAX_ALIGNMENT);

    cr_assert_not_null(ptr);
    cr_assert_not_null(ptr1);
    cr_assert_not_null(glob_ptr2);

    for (size_t i = 0; i < 5; ++i) {
        char *ptr2 = glob_ptr2 + ALIGNMENTS[i];
        memset(ptr1, 'S', 2 * length);
        memset(ptr1, 'T', length);
        memcpy(ptr2, ptr1, 2 * length);
        memcpy(ptr, ptr1, length);
        memcpy(ptr1 + position, ptr, length);

        checked_memmove(ptr2 + position, ptr, length);
        cr_assert_eq(memcmp(ptr2, ptr1, 2 * length), 0);
    }

    free(glob_ptr2);
    free(ptr1);
    free(ptr);
}

static void bionic_verify_memmove(char *src_copy, char *dest, char *src, size_t size)
{
    memset(dest, 0, size);
    memcpy(src, src_copy, size);
    checked_memmove(dest, src, size);
    cr_assert_eq(memcmp(dest, src_copy, size), 0);
}

Test(memmove, bionic_check)
{
    static size_t MEMMOVE_DATA_SIZE = 1024 * 1024 * 3;

    char *buffer = malloc(MEMMOVE_DATA_SIZE);
    cr_assert_not_null(buffer);

    char *src_data = malloc(MEMMOVE_DATA_SIZE);
    cr_assert_not_null(buffer);

    // Initialize to a known pattern to be able to compare to
    for (size_t i = 0; i < MEMMOVE_DATA_SIZE; ++i)
        src_data[i] = (i + 1) % 255;

    // All dst offsets between 0 and 127 (inclusive)
    for (size_t i = 0; i < 127; ++i) {
        char *src = buffer;
        char *dest = buffer + 256 + i;

        bionic_verify_memmove(src_data, dest, src, 1024); // Small
        bionic_verify_memmove(src_data, dest, src, 64 * 1024); // Medium
        bionic_verify_memmove(src_data, dest, src, 1024 * 1024 + 128 * 1024); // Large
    }

    // Leftover size offsets between 1 and 127
    for (size_t size = 0; size < 127; ++size) {
        char *dest = buffer + 256;
        char *src = buffer;

        bionic_verify_memmove(src_data, dest, src, 1024); // Small
        bionic_verify_memmove(src_data, dest, src, 64 * 1024); // Medium
        bionic_verify_memmove(src_data, dest, src, 1024 * 1024 + 128 * 1024); // Large
    }
}

static void bionic_do_memmove_test(uint8_t *source, uint8_t *destination, size_t length)
{
    memset(source, (length % 255) + 1, length);
    memset(destination, 0, length);

    checked_memmove(destination, source, length);
    cr_assert_eq(memcmp(source, destination, length), 0);
}

Test(memmove, bionic_align)
{
    bionic_run_source_destination_buffer_align_test(64 * 1024, bionic_do_memmove_test);
}

Test(memmove, bionic_overread)
{
    bionic_run_source_destination_buffer_overread_test(bionic_do_memmove_test);
}

static void glibc_do_one_test(char *destination, char *source, const char *original_source, size_t length)
{
    memcpy(source, original_source, length); // (note: this clears out the destination used by the previous test)
    checked_memmove(destination, source, length);
    cr_assert_eq(memcmp(destination, original_source, length), 0);
}

static void glibc_do_test(size_t alignment1, size_t alignment2, size_t length)
{
    alignment1 &= (getpagesize() - 1);
    if (alignment1 + length > glibc_page_size)
        return;

    alignment2 &= (getpagesize() - 1);
    if (alignment2 + length > glibc_page_size)
        return;

    char *s1 = glibc_buf1 + alignment1;
    char *s2 = glibc_buf2 + alignment2;

    for (size_t i = 0, j = 1; i < length; ++i, j += 23)
        s1[i] = j;

    glibc_do_one_test(s2, glibc_buf2 + alignment1, s1, length);
}

static void glibc_do_test2(size_t offset)
{
    size_t size = 0x20000000;
    uint32_t *large_buffer = mmap((void *)0x70000000, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    cr_assert_neq(large_buffer, MAP_FAILED);

    cr_assert_not((uintptr_t)large_buffer > 0x80000000 - 128);
    cr_assert_not(0x80000000 - (uintptr_t)large_buffer > 0x20000000);

    size_t bytes_move = 0x71000000 - (uintptr_t)large_buffer;
    if (bytes_move + offset * sizeof(uint32_t) > size) {
        munmap(large_buffer, size);
        return;
    }

    size_t array_size = bytes_move / sizeof(uint32_t);
    uint32_t *source = large_buffer;
    uint32_t *destination = &large_buffer[offset];
    for (size_t repeats = 0; repeats < 2; ++repeats) {
        for (size_t i = 0; i < array_size; ++i)
            source[i] = i;

        checked_memmove(destination, source, bytes_move);

        for (size_t i = 0; i < array_size; ++i)
            if (destination[i] != (uint32_t)i) {
                cr_expect_fail("Failure at %zu: %d != %d", i, destination[i], (uint32_t)i);
                cr_assert_fail("In memmove(%p, %p, %zu)", (void *)destination, (void *)source, bytes_move);
            }
        source = destination;
        destination = large_buffer;
    }
    munmap(large_buffer, size);
}

static void glibc_do_test3(size_t bytes_move, size_t offset)
{
    size_t size = bytes_move * 3;
    uint32_t *buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    cr_assert_neq(buffer, MAP_FAILED);

    size_t array_size = bytes_move / sizeof(uint32_t);

    for (size_t i = 0; i < array_size; ++i)
        buffer[i] = i;

    uint32_t *destination = &buffer[array_size + offset];
    checked_memmove(destination, buffer, bytes_move);

    for (size_t i = 0; i < array_size; ++i)
        cr_assert_eq(destination[i], i);
    munmap(buffer, size);
}

static void glibc_do_test4(size_t bytes_move, size_t offset1, size_t offset2)
{
    size_t size = bytes_move + (offset1 > offset2 ? offset1 : offset2);
    uint8_t *buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    cr_assert_neq(buffer, MAP_FAILED);

    uint8_t *destination = &buffer[offset1];
    uint8_t *source = &buffer[offset2];

    for (size_t repeats = 0; repeats < 2; ++repeats) {
        for (size_t i = 0; i < bytes_move; ++i)
            source[i] = i;

        checked_memmove(destination, source, bytes_move);

        for (size_t i = 0; i < bytes_move; ++i)
            cr_assert_eq(destination[i], (uint8_t)i);

        destination = &buffer[offset2];
        source = &buffer[offset1];
    }
    munmap(buffer, size);
}

Test(memmove, glibc)
{
    glibc_test_string_init();

    for (size_t i = 0; i < 14; ++i) {
        glibc_do_test(0, 32, 1 << i);
        glibc_do_test(32, 0, 1 << i);
        glibc_do_test(0, i, 1 << i);
        glibc_do_test(i, 0, 1 << i);
    }

    for (size_t i = 0; i < 32; ++i) {
        glibc_do_test(0, 32, i);
        glibc_do_test(32, 0, i);
        glibc_do_test(0, i, i);
        glibc_do_test(i, 0, i);
    }

    for (size_t i = 3; i < 32; ++i) {
        if ((i & (i - 1)) == 0)
            continue;
        glibc_do_test(0, 32, i * 16);
        glibc_do_test(32, 0, i * 16);
        glibc_do_test(0, i, i * 16);
        glibc_do_test(i, 0, i * 16);
    }

    glibc_do_test2(0);
    glibc_do_test2(33);
    glibc_do_test2(0x200000 - 1);
    glibc_do_test2(0x200000);
    glibc_do_test2(0x200000 + 1);
    glibc_do_test2(0x1000000 - 1);
    glibc_do_test2(0x1000000);
    glibc_do_test2(0x1000000 + 1);
    glibc_do_test2(0x4000000 - 1);
    glibc_do_test2(0x4000000);
    glibc_do_test2(0x4000000 + 1);

    glibc_do_test3(16384, 3);

    for (size_t i = 4096; i <= 16384; i <<= 1) {
        glibc_do_test4(i, 0, i);
        glibc_do_test4(i, 0, i - 1);
        glibc_do_test4(i, 0, i + 1);
        glibc_do_test4(i, 63, i + 63);
        glibc_do_test4(i, 63, i + 64);
        glibc_do_test4(i, 63, i);

        glibc_do_test4(i, 0, 1);
        glibc_do_test4(i, 0, 15);
        glibc_do_test4(i, 0, 31);
        glibc_do_test4(i, 0, 63);
        glibc_do_test4(i, 0, 64);
        glibc_do_test4(i, 0, 65);
        glibc_do_test4(i, 0, 127);
        glibc_do_test4(i, 0, 129);
    }
}

Test(memmove, glibc_random)
{
    glibc_test_string_init();

    for (size_t n = 0; n < 50000; ++n) {
        size_t size;
        if ((random() & 255) == 0)
            size = 65536;
        else
            size = 512;
        if (size > glibc_page_size)
            size = glibc_page_size;

        size_t length, alignment1, alignment2;
        if ((random() & 3) == 0) {
            length = random() & (size - 1);
            alignment1 = size - length - (random() & 31);
            alignment2 = size - length - (random() & 31);
            if (alignment1 > size)
                alignment1 = 0;
            if (alignment2 > size)
                alignment2 = 0;
        } else {
            alignment1 = random() & (size / 2 - 1);
            alignment2 = random() & (size / 2 - 1);
            length = random() & (size - 1);
            if (alignment1 + length > size)
                alignment1 = size - length;
            if (alignment2 + length > size)
                alignment2 = size - length;
        }

        unsigned char *ptr1 = (unsigned char *)(glibc_buf1 + glibc_page_size - size);
        unsigned char *ptr2 = (unsigned char *)(glibc_buf2 + glibc_page_size - size);
        int character = random() & 255;

        size_t source_end = alignment1 + length + 256;
        if (source_end > size)
            source_end = size;

        size_t source_start;
        if (alignment1 > 256)
            source_start = alignment1 - 256;
        else
            source_start = 0;

        for (size_t i = source_start; i < source_end; ++i)
            ptr1[i] = random() & 255;

        size_t destination_end = alignment2 + length + 256;
        if (destination_end > size)
            destination_end = size;

        size_t destination_start;
        if (alignment2 > 256)
            destination_start = alignment2 - 256;
        else
            destination_start = 0;

        memset(ptr2 + destination_start, character, destination_end - destination_start);
        memcpy(ptr2 + source_start, ptr1 + source_start, source_end - source_start);

        checked_memmove((char *)(ptr2 + alignment2), (char *)(ptr2 + alignment1), length);

        cr_assert_eq(memcmp(ptr1 + alignment1, ptr2 + alignment2, length), 0);
        for (size_t i = destination_start; i < destination_end; ++i) {
            if (i >= alignment2 && i < alignment2 + length) {
                i = alignment2 + length - 1;
                continue;
            }
            if (i >= source_start && i < source_end) {
                i = source_end - 1;
                continue;
            }
            cr_assert_eq(ptr2[i], character);
        }

        if (source_start < alignment2)
            cr_assert_eq(memcmp(ptr2 + source_start, ptr1 + source_start, (source_end > alignment2 ? alignment2 : source_end) - source_start), 0);

        size_t i = source_start > alignment2 + length ? source_start : alignment2 + length;
        if (source_end > alignment2 + length)
            cr_assert_eq(memcmp(ptr2 + i, ptr1 + i, source_end - i), 0);
    }
}

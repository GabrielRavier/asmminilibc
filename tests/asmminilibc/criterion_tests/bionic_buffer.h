/*
** EPITECH PROJECT, 2021
** asmminilibc
** File description:
** Defines bionic_buffer stuff
*/

#pragma once

#include <criterion/criterion.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stddef.h>

static const size_t BIONIC_BUFFER_FENCEPOST_LENGTH = 8;
static const size_t BIONIC_BUFFER_MISCMP_MAX_LENGTH = 512;

static const int BIONIC_BUFFER_SINGLE_ALIGNS[][2] = {
    // Both buffers at same alignment.
    { 1, 0 },
    { 2, 0 },
    { 4, 0 },
    { 8, 0 },
    { 16, 0 },
    { 32, 0 },
    { 64, 0 },
    { 128, 0 },

    // General unaligned cases.
    { 4, 1 },
    { 4, 2 },
    { 4, 3 },

    { 8, 1 },
    { 8, 2 },
    { 8, 3 },
    { 8, 4 },
    { 8, 5 },
    { 8, 6 },
    { 8, 7 },

    { 128, 1 },
    { 128, 4 },
    { 128, 8 },
    { 128, 12 },
    { 128, 16 },
};

// Set of multiple buffer alignment combinations to be used for string/memory
// testing routines.
static int BIONIC_BUFFER_DOUBLE_ALIGNS[][4] = {
    // Both buffers at same alignment.
    { 1, 0, 1, 0 },
    { 2, 0, 2, 0 },
    { 4, 0, 4, 0 },
    { 8, 0, 8, 0 },
    { 16, 0, 16, 0 },
    { 32, 0, 32, 0 },
    { 64, 0, 64, 0 },
    { 128, 0, 128, 0 },

    // Different word alignments between buffers.
    { 8, 0, 4, 0 },
    { 4, 0, 8, 0 },
    { 16, 0, 4, 0 },
    { 4, 0, 16, 0 },

    // General unaligned cases.
    { 4, 0, 4, 1 },
    { 4, 0, 4, 2 },
    { 4, 0, 4, 3 },

    { 4, 1, 4, 0 },
    { 4, 1, 4, 1 },
    { 4, 1, 4, 2 },
    { 4, 1, 4, 3 },

    { 4, 2, 4, 0 },
    { 4, 2, 4, 1 },
    { 4, 2, 4, 2 },
    { 4, 2, 4, 3 },

    { 4, 3, 4, 0 },
    { 4, 3, 4, 1 },
    { 4, 3, 4, 2 },
    { 4, 3, 4, 3 },

    { 8, 0, 8, 1 },
    { 8, 0, 8, 2 },
    { 8, 0, 8, 3 },
    { 8, 0, 8, 4 },
    { 8, 0, 8, 5 },
    { 8, 0, 8, 6 },
    { 8, 0, 8, 7 },

    { 8, 1, 8, 0 },
    { 8, 1, 8, 1 },
    { 8, 1, 8, 2 },
    { 8, 1, 8, 3 },
    { 8, 1, 8, 4 },
    { 8, 1, 8, 5 },
    { 8, 1, 8, 6 },
    { 8, 1, 8, 7 },

    { 8, 2, 8, 0 },
    { 8, 2, 8, 1 },
    { 8, 2, 8, 2 },
    { 8, 2, 8, 3 },
    { 8, 2, 8, 4 },
    { 8, 2, 8, 5 },
    { 8, 2, 8, 6 },
    { 8, 2, 8, 7 },

    { 8, 3, 8, 0 },
    { 8, 3, 8, 1 },
    { 8, 3, 8, 2 },
    { 8, 3, 8, 3 },
    { 8, 3, 8, 4 },
    { 8, 3, 8, 5 },
    { 8, 3, 8, 6 },
    { 8, 3, 8, 7 },

    { 8, 4, 8, 0 },
    { 8, 4, 8, 1 },
    { 8, 4, 8, 2 },
    { 8, 4, 8, 3 },
    { 8, 4, 8, 4 },
    { 8, 4, 8, 5 },
    { 8, 4, 8, 6 },
    { 8, 4, 8, 7 },

    { 8, 5, 8, 0 },
    { 8, 5, 8, 1 },
    { 8, 5, 8, 2 },
    { 8, 5, 8, 3 },
    { 8, 5, 8, 4 },
    { 8, 5, 8, 5 },
    { 8, 5, 8, 6 },
    { 8, 5, 8, 7 },

    { 8, 6, 8, 0 },
    { 8, 6, 8, 1 },
    { 8, 6, 8, 2 },
    { 8, 6, 8, 3 },
    { 8, 6, 8, 4 },
    { 8, 6, 8, 5 },
    { 8, 6, 8, 6 },
    { 8, 6, 8, 7 },

    { 8, 7, 8, 0 },
    { 8, 7, 8, 1 },
    { 8, 7, 8, 2 },
    { 8, 7, 8, 3 },
    { 8, 7, 8, 4 },
    { 8, 7, 8, 5 },
    { 8, 7, 8, 6 },
    { 8, 7, 8, 7 },

    { 128, 1, 128, 4 },
    { 128, 1, 128, 8 },
    { 128, 1, 128, 12 },
    { 128, 1, 128, 16 },
    { 128, 4, 128, 1 },
    { 128, 8, 128, 1 },
    { 128, 12, 128, 1 },
    { 128, 16, 128, 1 },
};

static inline size_t bionic_buffer_set_increase(size_t length)
{
    return length >= 4096 ? 1024 : length >= 1024 ? 256 : 1;
}

static inline void *bionic_buffer_get_aligned_ptr(void *original_ptr, int alignment, int or_mask)
{
    uintptr_t ptr_as_int = (uintptr_t)original_ptr;

    if (alignment > 0) {
        ptr_as_int += alignment - (ptr_as_int & (alignment - 1));
        ptr_as_int |= alignment | or_mask;
    }
    return (void *)ptr_as_int;
}

static inline void bionic_buffer_set_fencepost(uint8_t *buffer)
{
    for (size_t i = 0; i < BIONIC_BUFFER_FENCEPOST_LENGTH; i += 2) {
        buffer[i] = 0xDE;
        buffer[i + 1] = 0xAD;
    }
}

static inline void bionic_buffer_verify_fencepost(uint8_t *buffer)
{
    for (size_t i = 0; i < BIONIC_BUFFER_FENCEPOST_LENGTH; i += 2) {
        if (buffer[i] != 0xDE || buffer[i + 1] != 0xAD) {
            uint8_t expected_value;
            if (buffer[i] == 0xDE) {
                ++i;
                expected_value = 0xAD;
            } else
                expected_value = 0xDE;
            cr_assert_eq(expected_value, buffer[i]);
        }
    }
}

static inline void bionic_run_single_buffer_align_test(size_t max_test_size, void (*test_func)(uint8_t *, size_t))
{
    uint8_t *buffer = malloc(max_test_size * 3);

    cr_assert_neq(buffer, NULL);
    for (size_t i = 0; i < (sizeof(BIONIC_BUFFER_SINGLE_ALIGNS) / sizeof(BIONIC_BUFFER_SINGLE_ALIGNS[0])); ++i) {
        size_t increase = 1;
        for (size_t length = 0; length <= max_test_size; length += increase) {
            increase = bionic_buffer_set_increase(length);

            uint8_t *buffer_aligned = bionic_buffer_get_aligned_ptr(buffer + BIONIC_BUFFER_FENCEPOST_LENGTH, BIONIC_BUFFER_SINGLE_ALIGNS[i][0], BIONIC_BUFFER_SINGLE_ALIGNS[i][1]);

            bionic_buffer_set_fencepost(&buffer_aligned[-BIONIC_BUFFER_FENCEPOST_LENGTH]);
            bionic_buffer_set_fencepost(&buffer_aligned[length]);

            test_func(buffer_aligned, length);

            bionic_buffer_verify_fencepost(&buffer_aligned[-BIONIC_BUFFER_FENCEPOST_LENGTH]);
            bionic_buffer_verify_fencepost(&buffer_aligned[length]);
        }
    }
    free(buffer);
}

static inline void bionic_run_single_buffer_overread_test(void (*test_func)(uint8_t *, size_t))
{
    const size_t page_size = sysconf(_SC_PAGE_SIZE);

    uint8_t *memory;
    cr_assert_eq(posix_memalign((void **)&memory, page_size, 2 * page_size), 0);
    memset(memory, 0x23, 2 * page_size);

    // Make the second page fault on read/write
    cr_assert_eq(mprotect(&memory[page_size], page_size, PROT_NONE), 0);

    for (size_t i = 0; i < page_size; ++i)
        test_func(&memory[page_size - i], i);

    cr_assert_eq(mprotect(&memory[page_size], page_size, PROT_READ | PROT_WRITE), 0);
    free(memory);
}

static inline void bionic_run_cmp_buffer_align_test(size_t max_test_size, void (*test_cmp_func)(uint8_t *, uint8_t *, size_t), void (*test_miscmp_func)(uint8_t *, uint8_t *, size_t, size_t), size_t (*set_incr)(size_t))
{
    if (set_incr == NULL)
        set_incr = bionic_buffer_set_increase;

    uint8_t *buffer1 = malloc(max_test_size * 3);
    uint8_t *buffer2 = malloc(max_test_size * 3);

    for (size_t i = 0; i < (sizeof(BIONIC_BUFFER_DOUBLE_ALIGNS) / sizeof(BIONIC_BUFFER_DOUBLE_ALIGNS[0])); ++i) {
        size_t increase = 1;
        for (size_t length = 0; length <= max_test_size; length += increase) {
            increase = set_incr(length);

            uint8_t *buffer1_align = bionic_buffer_get_aligned_ptr(buffer1, BIONIC_BUFFER_DOUBLE_ALIGNS[i][0], BIONIC_BUFFER_DOUBLE_ALIGNS[i][1]);
            uint8_t *buffer2_align = bionic_buffer_get_aligned_ptr(buffer2, BIONIC_BUFFER_DOUBLE_ALIGNS[i][2], BIONIC_BUFFER_DOUBLE_ALIGNS[i][3]);

            memset(buffer1_align + length, 0, 32);
            memset(buffer2_align + length, 0, 32);
            test_cmp_func(buffer1_align, buffer2_align, length);

            for (size_t j = 0; j < 32; ++j) {
                buffer1_align[length + j] = j;
                buffer2_align[length + j] = j + 1;
            }
            test_cmp_func(buffer1_align, buffer2_align, length);

            if (length > 0)
                for (size_t length2 = length + 1; length2 < length + 32; ++length2) {
                    test_miscmp_func(buffer1_align, buffer2_align, length, length2);
                    test_miscmp_func(buffer1_align, buffer2_align, length2, length);
                }
        }
    }
    free(buffer1);
    free(buffer2);
}

static inline void bionic_run_cmp_buffer_overread_test(void (*test_cmp_func)(uint8_t *, uint8_t *, size_t), void (*test_miscmp_func)(uint8_t *, uint8_t *, size_t, size_t))
{
    size_t page_size = sysconf(_SC_PAGE_SIZE);

    uint8_t *memory1;
    cr_assert_eq(posix_memalign((void **)&memory1, page_size, 2 * page_size), 0);
    memset(memory1, 0x23, 2 * page_size);
    cr_assert_eq(mprotect(&memory1[page_size], page_size, PROT_NONE), 0);

    uint8_t *memory2;
    cr_assert_eq(posix_memalign((void **)&memory2, page_size, 2 * page_size), 0);
    memset(memory2, 0x23, 2 * page_size);
    cr_assert_eq(mprotect(&memory2[page_size], page_size, PROT_NONE), 0);

    for (size_t i = 0; i < page_size; ++i)
        test_cmp_func(&memory1[page_size - i], &memory2[page_size - i], i);

    size_t miscmp_length = page_size < BIONIC_BUFFER_MISCMP_MAX_LENGTH ? page_size : BIONIC_BUFFER_MISCMP_MAX_LENGTH;
    for (size_t i = 1; i < miscmp_length; ++i) {
        for (size_t j = 1; j < miscmp_length; ++j) {
            if (i == j)
                continue;
            test_miscmp_func(&memory1[page_size - i], &memory2[page_size - j], i, j);
        }
    }

    cr_assert_eq(mprotect(&memory1[page_size], page_size, PROT_READ | PROT_WRITE), 0);
    cr_assert_eq(mprotect(&memory2[page_size], page_size, PROT_READ | PROT_WRITE), 0);
    free(memory1);
    free(memory2);
}

static inline void bionic_run_source_destination_buffer_align_test(size_t max_test_size, void (*test_func)(uint8_t *, uint8_t *, size_t))
{
    uint8_t *source = malloc(max_test_size * 3), *destination = malloc(max_test_size * 3);

    for (size_t i = 0; i < (sizeof(BIONIC_BUFFER_DOUBLE_ALIGNS) / sizeof(BIONIC_BUFFER_DOUBLE_ALIGNS[0])); ++i) {
        size_t increase = 1;
        for (size_t length = 0; length < max_test_size; length += increase) {
            increase = bionic_buffer_set_increase(length);

            uint8_t *source_align = bionic_buffer_get_aligned_ptr(source + BIONIC_BUFFER_FENCEPOST_LENGTH, BIONIC_BUFFER_DOUBLE_ALIGNS[i][0], BIONIC_BUFFER_DOUBLE_ALIGNS[i][1]);
            uint8_t *destination_align = bionic_buffer_get_aligned_ptr(destination + BIONIC_BUFFER_FENCEPOST_LENGTH, BIONIC_BUFFER_DOUBLE_ALIGNS[i][0], BIONIC_BUFFER_DOUBLE_ALIGNS[i][1]);

            bionic_buffer_set_fencepost(&destination_align[-BIONIC_BUFFER_FENCEPOST_LENGTH]);
            bionic_buffer_set_fencepost(&destination_align[length]);
            test_func(source_align, destination_align, length);
            bionic_buffer_verify_fencepost(&destination_align[-BIONIC_BUFFER_FENCEPOST_LENGTH]);
            bionic_buffer_verify_fencepost(&destination_align[length]);
        }
    }

    free(destination);
    free(source);
}

static inline void bionic_run_source_destination_buffer_overread_test(void (*test_func)(uint8_t *, uint8_t *, size_t))
{
    size_t page_size = sysconf(_SC_PAGE_SIZE);

    uint8_t *memory;
    cr_assert_eq(posix_memalign((void **)&memory, page_size, 2 * page_size), 0);
    memset(memory, 0x23, 2 * page_size);

    cr_assert_eq(mprotect(&memory[page_size], page_size, PROT_NONE), 0);

    uint8_t *destination_buffer = malloc(2 * page_size);
    for (size_t i = 0; i < 16; ++i)
        for (size_t j = 0; j < page_size; ++j)
            test_func(&memory[page_size - j], &destination_buffer[i], j);

    free(destination_buffer);
    cr_assert_eq(mprotect(&memory[page_size], page_size, PROT_READ | PROT_WRITE), 0);
    free(memory);
}

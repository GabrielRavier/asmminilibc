/*
** EPITECH PROJECT, 2021
** asmminilibc
** File description:
** Defines bionic_test_state
*/

#pragma once

#include <criterion/criterion.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static const size_t BIONIC_TEST_STATE_ALIGNMENTS[] = {
    24, 32, 16, 48, 0, 1, 2, 3, 4, 5, 6, 7, 11
};

enum {
    BIONIC_TEST_STATE_ITER = 500
};

// State for bionic tests for string.h functions
struct bionic_test_state {
    size_t n;
    char *ptr, *ptr1, *ptr2;
    size_t align1_index, align2_index;
    char *glob_ptr, *glob_ptr1, *glob_ptr2;
    size_t max_length;
    size_t lengths[BIONIC_TEST_STATE_ITER + 1];
};

static inline void bionic_test_state_init_len_array(struct bionic_test_state *self)
{
    self->n = 0;
    self->lengths[self->n++] = 0;
    for (uint32_t i = 1; i < BIONIC_TEST_STATE_ITER; ++i) {
        size_t l = (size_t)exp(log((double)self->max_length) * i / BIONIC_TEST_STATE_ITER);
        if (l != self->lengths[self->n - 1])
            self->lengths[self->n++] = l;
    }
    self->lengths[self->n++] = self->max_length;
}

static inline struct bionic_test_state bionic_test_state_new(size_t max_length)
{
    static const size_t MAX_ALIGNMENT = 64;
    struct bionic_test_state result = {
        .max_length = max_length,
        .align1_index = 0,
        .align2_index = 0
    };

    cr_assert_eq(posix_memalign((void **)&result.glob_ptr, sysconf(_SC_PAGESIZE),
                                2 * result.max_length + MAX_ALIGNMENT), 0);
    cr_assert_eq(posix_memalign((void **)&result.glob_ptr1, sysconf(_SC_PAGESIZE),
                                2 * result.max_length + MAX_ALIGNMENT), 0);
    cr_assert_eq(posix_memalign((void **)&result.glob_ptr2, sysconf(_SC_PAGESIZE),
                                2 * result.max_length + MAX_ALIGNMENT), 0);
    bionic_test_state_init_len_array(&result);

    return (result);
}

static inline void bionic_test_state_reset_pointers(struct bionic_test_state *self)
{
    if (self->align1_index == (sizeof(BIONIC_TEST_STATE_ALIGNMENTS) / sizeof(BIONIC_TEST_STATE_ALIGNMENTS[0])) ||
        self->align2_index == (sizeof(BIONIC_TEST_STATE_ALIGNMENTS) / sizeof(BIONIC_TEST_STATE_ALIGNMENTS[0]))) {
        self->ptr = NULL;
        self->ptr1 = NULL;
        self->ptr2 = NULL;
    }
    else {
        self->ptr = self->glob_ptr + BIONIC_TEST_STATE_ALIGNMENTS[self->align1_index];
        self->ptr1 = self->glob_ptr1 + BIONIC_TEST_STATE_ALIGNMENTS[self->align1_index];
        self->ptr2 = self->glob_ptr2 + BIONIC_TEST_STATE_ALIGNMENTS[self->align2_index];
    }
}

static inline void bionic_test_state_begin_iters(struct bionic_test_state *self)
{
    self->align1_index = 0;
    self->align2_index = 0;

    bionic_test_state_reset_pointers(self);
}

static inline bool bionic_test_state_has_next_iter(const struct bionic_test_state *self)
{
    return self->align1_index != ((sizeof(BIONIC_TEST_STATE_ALIGNMENTS) / sizeof(BIONIC_TEST_STATE_ALIGNMENTS[0])) - 1) ||
        self->align2_index != ((sizeof(BIONIC_TEST_STATE_ALIGNMENTS) / sizeof(BIONIC_TEST_STATE_ALIGNMENTS[0])) - 1);
}

static inline void bionic_test_state_next_iter(struct bionic_test_state *self)
{
    if (!bionic_test_state_has_next_iter(self))
        return;
    if (self->align1_index == ((sizeof(BIONIC_TEST_STATE_ALIGNMENTS) / sizeof(BIONIC_TEST_STATE_ALIGNMENTS[0])) - 1)) {
        self->align1_index = 0;
        ++self->align2_index;
    } else
        ++self->align1_index;
    bionic_test_state_reset_pointers(self);
}

static inline void bionic_test_state_destroy(struct bionic_test_state *self)
{
    free(self->glob_ptr);
    free(self->glob_ptr1);
    free(self->glob_ptr2);
}

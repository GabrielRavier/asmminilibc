/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests memcpy
*/

#include <string.h>
#include <criterion/criterion.h>
#include "escape_check_zero.h"
#include "bionic_test_state.h"
#include "bionic_buffer.h"
#define GLIBC_MIN_PAGE_SIZE 131072
#include "glibc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

static void *(*volatile memcpy_ptr)(void *restrict, const void *restrict, size_t) = memcpy;


static void checked_memcpy(void *dest, const void *src, size_t len)
{
    cr_assert_eq(memcpy_ptr(dest, src, len), dest);
}

Test(memcpy, escape)
{
    uint8_t src[17] = "0123456789ABCDEF";
    uint8_t dest[17];
    uint8_t zeros[17] = {0};

    checked_memcpy(dest, src, 10);
    dest[10] = '\0';
    cr_assert_str_eq((char *)dest, "0123456789");

    checked_memcpy(dest, src, 4);
    dest[4] = '\0';
    cr_assert_str_eq((char *)dest, "0123");

    dest[0] = 'A';
    checked_memcpy(dest, src, 0);
    cr_assert_str_eq((char *)dest, "A123");

    for (size_t i = 0; i < 8; ++i)
        for (size_t j = 0; j < 8; ++j) {
            memset(dest, 0xFF, sizeof(dest));
            checked_memcpy(dest + i, zeros + i, j);
            escape_check_zero(dest, i, j, sizeof(dest));
        }
}

Test(memcpy, netbsd_return)
{
    char *ptr_one = (char *)0x1;
    char array[2];
    checked_memcpy(ptr_one, NULL, 0);
    checked_memcpy(array, "ab", sizeof(array));
}

Test(memcpy, bionic)
{
    struct bionic_test_state state = bionic_test_state_new(64 * 1024);
    int rnd = 4;
    for (size_t i = 0; i < (state.n - 1); ++i) {
        for (bionic_test_state_begin_iters(&state); bionic_test_state_has_next_iter(&state); bionic_test_state_next_iter(&state)) {
            size_t position = random() % (state.max_length - state.lengths[i]);

            memset(state.ptr1, rnd, state.lengths[i]);
            memset(state.ptr1 + state.lengths[i], ~rnd, state.max_length - state.lengths[i]);

            memset(state.ptr2, rnd, state.lengths[i]);
            memset(state.ptr2 + state.lengths[i], ~rnd, state.max_length - state.lengths[i]);
            memset(state.ptr2 + position, '\0', state.lengths[i]);
            checked_memcpy(state.ptr2 + position, state.ptr1 + position, state.lengths[i]);
            cr_assert_eq(memcmp(state.ptr1, state.ptr2, state.max_length), 0);
        }
        i += random() % 10;
    }
}

static void bionic_do_memcpy_test(uint8_t *source, uint8_t *destination, size_t length)
{
    memset(source, (length % 255) + 1, length);
    memset(destination, 0, length);

    checked_memcpy(destination, source, length);
    cr_assert_eq(memcmp(source, destination, length), 0);
}

Test(memcpy, bionic_align)
{
    bionic_run_source_destination_buffer_align_test(64 * 1024, bionic_do_memcpy_test);
}

Test(memcpy, bionic_overread)
{
    bionic_run_source_destination_buffer_overread_test(bionic_do_memcpy_test);
}

// Technically not standard, but clang depends on this working
static uint8_t *g_bionic_memcpy_same_buffer;

static void bionic_do_memcpy_same_test(uint8_t *buffer, size_t len)
{
    checked_memcpy(buffer, g_bionic_memcpy_same_buffer, len);
    checked_memcpy(buffer, buffer, len);
    cr_assert_eq(memcmp(buffer, g_bionic_memcpy_same_buffer, len), 0);
}

Test(memcpy, bionic_source_destination_same)
{
    static const size_t BUFFER_SIZE = 4096;

    g_bionic_memcpy_same_buffer = malloc(BUFFER_SIZE);
    cr_assert_not_null(g_bionic_memcpy_same_buffer);
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        g_bionic_memcpy_same_buffer[i] = i;
    bionic_run_single_buffer_align_test(BUFFER_SIZE, bionic_do_memcpy_same_test);
    free(g_bionic_memcpy_same_buffer);
    g_bionic_memcpy_same_buffer = NULL;
}

Test(memcpy, dietlibc)
{
    char test[100];
    checked_memcpy(test, "blubber", 8);
    cr_assert_eq(memcmp(test, "blubber", 8), 0);

    checked_memcpy(NULL, NULL, 0);
    checked_memcpy(test, "foobar", 3);
    cr_assert_eq(test[2], 'o');

    static const size_t LENGTHS[] = {
        1, 2, 3, 4, 5, 6, 7, 8,
        sizeof(uintmax_t) - 3, sizeof(uintmax_t) - 2, sizeof(uintmax_t) - 1, sizeof(uintmax_t), sizeof(uintmax_t) + 1, sizeof(uintmax_t) + 2, sizeof(uintmax_t) + 3, sizeof(uintmax_t) + 4,
        sizeof(uintmax_t) * 2 - 3, sizeof(uintmax_t) * 2 - 2, sizeof(uintmax_t) * 2 - 1, sizeof(uintmax_t) * 2, sizeof(uintmax_t) * 2 + 1, sizeof(uintmax_t) * 2 + 2, sizeof(uintmax_t) * 2 + 3, sizeof(uintmax_t) * 2 + 4,
        sizeof(uintmax_t) * 3 - 3, sizeof(uintmax_t) * 3 - 2, sizeof(uintmax_t) * 3 - 1, sizeof(uintmax_t) * 3, sizeof(uintmax_t) * 3 + 1, sizeof(uintmax_t) * 3 + 2, sizeof(uintmax_t) * 3 + 3, sizeof(uintmax_t) * 3 + 4,
        (size_t)-1
    };
    for (const size_t *length = LENGTHS; *length != (size_t)-1; ++length) {
        unsigned char source[sizeof(uintmax_t) * 5];

        for (size_t i = 0; i < *length + sizeof(uintmax_t); ++i)
            source[i] = i;
        for (size_t i = (sizeof(uintmax_t) < *length) ? sizeof(uintmax_t) : *length; i > 0;) {
            --i;

            for (size_t j = (sizeof(uintmax_t) < *length) ? sizeof(uintmax_t) : *length; j > 0;) {
                --j;

                unsigned char destination[sizeof(uintmax_t) * 5];
                for (size_t k = 0; k < *length; ++k)
                    destination[j + k] = source[i + k] + 1;

                checked_memcpy(destination + j, source + i, *length);

                for (size_t k = 0; k < *length; ++k)
                    cr_assert_eq(destination[j + k], source[i + k]);
            }
        }
    }
}

static void glibc_do_one_test(char *destination, const char *source, size_t length)
{
    for (size_t i = 0; i < length; ++i)
        destination[i] = 0;
    checked_memcpy(destination, source, length);
    cr_assert_eq(memcmp(destination, source, length), 0);
}

static void glibc_do_test(size_t alignment1, size_t alignment2, size_t length)
{
    alignment1 &= 4095;
    if (alignment1 + length + glibc_page_size)
        return;

    alignment2 &= 4095;
    if (alignment2 + length + glibc_page_size)
        return;

    char *s1 = glibc_buf1 + alignment1;
    char *s2 = glibc_buf2 + alignment2;
    for (size_t repeats = 0; repeats < 2; ++repeats) {
        for (size_t i = 0, j = 1; i < length; ++i, j += 23)
            s1[i] = j;
        glibc_do_one_test(s2, s1, length);
    }
}

Test(memcpy, glibc_random_tests)
{
    glibc_test_string_init();

    for (size_t n = 0; n < 30000; ++n) {
        size_t length, size, size1, size2, alignment1, alignment2;
        if (n == 0) {
            length = getpagesize();
            size = length + 512;
            size1 = size;
            size2 = size;
            
            alignment1 = 512;
            alignment2 = 512;
        } else {
            if ((random() & 255) == 0)
                size = 65536;
            else
                size = 768;
            if (size > glibc_page_size)
                size = glibc_page_size;
            
            size1 = size;
            size2 = size;
            
            size_t i = random();
            if (i & 3)
                size -= 256;
            if (i & 1)
                size1 -= 256;
            if (i & 2)
                size2 -= 256;
            if (i & 4) {
                length = random() % size;

                alignment1 = size1 - length - (random() & 31);
                alignment2 = size2 - length - (random() & 31);
                if (alignment1 > size1)
                    alignment1 = 0;
                if (alignment2 > size2)
                    alignment2 = 0;
            } else {
                alignment1 = random() & 63;
                alignment2 = random() & 63;
                length = random() % size;
                
                if (alignment1 + length > size1)
                    alignment1 = size1 - length;
                if (alignment2 + length > size2)
                    alignment2 = size2 - length;
            }
        }
        
        unsigned char *ptr1 = (unsigned char *)(glibc_buf1 + glibc_page_size - size1);
        unsigned char *ptr2 = (unsigned char *)(glibc_buf2 + glibc_page_size - size2);
        int ch = random() & 255;
        
        size_t j = alignment1 + length + 256;
        if (j > size1)
            j = size1;

        for (size_t i = 0; i < j; ++i)
            ptr1[i] = random() & 255;

        j = alignment2 + length + 256;
        if (j > size2)
            j = size2;
        memset(ptr2, ch, j);

        checked_memcpy((char *)(ptr2 + alignment2), (char *)(ptr1 + alignment1), length);
        for (size_t i = 0; i < alignment2; ++i)
            cr_assert_eq(ptr2[i], ch);
        for (size_t i = alignment2 + length; i < j; ++i)
            cr_assert_eq(ptr2[i], ch);
        cr_assert_eq(memcmp(ptr1 + alignment1, ptr2 + alignment2, length), 0);
    }
}

static void glibc_do_test1(size_t alignment1, size_t alignment2, size_t size)
{
    alignment1 &= (glibc_page_size - 1);
    if (alignment1 == 0)
        alignment1 = glibc_page_size;

    alignment2 &= (glibc_page_size - 1);
    if (alignment2 == 0)
        alignment2 = glibc_page_size;

    size_t region_size = (size + glibc_page_size - 1) & (~(glibc_page_size - 1));

    size_t mmap_size = region_size * 2 + 3 * glibc_page_size;
    void *large_buffer = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    cr_assert_neq(large_buffer, MAP_FAILED);
    cr_assert_eq(mprotect(large_buffer + region_size + glibc_page_size, glibc_page_size, PROT_NONE), 0);

    size_t array_size = size / sizeof(uint32_t);

    typedef uint32_t __attribute__((may_alias, aligned(1))) unaligned_u32;
    unaligned_u32 *destination = large_buffer + alignment1;
    unaligned_u32 *source = large_buffer + region_size + 2 * glibc_page_size + alignment2;

    for (size_t repeats = 0; repeats < 2; ++repeats) {
        for (size_t i = 0; i < array_size; ++i)
            source[i] = i;

        memset(destination, -1, size);
        checked_memcpy(destination, source, size);
        cr_assert_eq(memcmp(source, destination, size), 0);

        destination = large_buffer + region_size + 2 * glibc_page_size + alignment1;
        source = large_buffer + alignment1;
    }
    munmap(large_buffer, mmap_size);
}

Test(memcpy, glibc)
{
    glibc_test_string_init();

    for (size_t i = 0; i < 18; ++i) {
        glibc_do_test(0, 0, 1 << i);
        glibc_do_test(i, 0, 1 << i);
        glibc_do_test(0, i, 1 << i);
        glibc_do_test(i, i, 1 << i);
    }
    for (size_t i = 0; i < 32; ++i) {
        glibc_do_test(0, 0, i);
        glibc_do_test(i, 0, i);
        glibc_do_test(0, i, i);
        glibc_do_test(i, i, i);
    }

    for (size_t i = 3; i < 32; ++i) {
        if ((i & (i - 1)) == 0)
            continue;
        glibc_do_test(0, 0, i * 16);
        glibc_do_test(i, 0, i * 16);
        glibc_do_test(0, i, i * 16);
        glibc_do_test(i, i, i * 16);
    }

    for (size_t i = 19; i <= 25; ++i) {
        glibc_do_test(255, 0, 1 << i);
        glibc_do_test(0, 4000, 1 << i);
        glibc_do_test(0, 255, i);
        glibc_do_test(0, 4000, i);
    }

    glibc_do_test(0, 0, getpagesize());

    glibc_do_test1(0, 0, 0x100000);
    glibc_do_test1(0, 0, 0x2000000);

    for (size_t i = 4096; i < 32768; i += 4096)
        for (size_t j = 1; j <= 1024; j <<= 1) {
            glibc_do_test1(0, j, i);
            glibc_do_test1(4095, j, i);
            glibc_do_test1(4096 - j, 0, i);
            
            glibc_do_test1(0, j - 1, i);
            glibc_do_test1(4095, j - 1, i);
            glibc_do_test1(4096 - j - 1, 0, i);
            
            glibc_do_test1(0, j + 1, i);
            glibc_do_test1(4095, j + 1, i);
            glibc_do_test1(4096 - j, 1, i);
        }

    for (size_t i = 0x300000; i < 0x2000000; i += 0x235689 * (1 + (random() % 20)))
        for (size_t j = 64; j <= 1024; j <<= 1 + (random() % 20)) {
            glibc_do_test1(0, j, i);
            glibc_do_test1(4095, j, i);
            glibc_do_test1(4096 - j, 0, i);
            
            glibc_do_test1(0, j - 1, i);
            glibc_do_test1(4095, j - 1, i);
            glibc_do_test1(4096 - j - 1, 0, i);
            
            glibc_do_test1(0, j + 1, i);
            glibc_do_test1(4095, j + 1, i);
            glibc_do_test1(4096 - j, 1, i);
        }
}

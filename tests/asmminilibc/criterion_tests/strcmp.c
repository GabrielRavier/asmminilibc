/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strcmp
*/

#include <string.h>
#include <criterion/criterion.h>
#include "bionic_test_state.h"
#include "bionic_buffer.h"
#include "glibc.h"
#include <limits.h>

static int (*volatile strcmp_ptr)(const char *, const char *) = strcmp;

static int known_strcmp(const char *str1, const char *str2)
{
    int result;

    while ((result = *(unsigned char *)str1 - *(unsigned char *)str2++) == 0 && *str1++ != '\0')
        ;
    return result;
}

static void check_result(const char *str1, const char *str2, int expected_result)
{
    int result = strcmp_ptr(str1, str2);
    // Test on 64-bit architectures where the ABI requires that the callee promote, whether this promotion has in fact been done
    asm("" : "=g" (result) : "0" (result));
    if (expected_result == 0)
        cr_assert_eq(result, 0);
    if (expected_result < 0)
        cr_assert(result < 0);
    if (expected_result > 0)
        cr_assert(result > 0);
}

Test(strcmp, escape)
{
    check_result("", "", 0);
    check_result("abc", "abd", -1);
    check_result("abc", "abb", 1);
    check_result("a", "abc", -1);
    check_result("b", "a", 1);
    check_result("1234", "5678", -1);
    check_result("abc def", "abc def", 0);
    check_result("123", "123", 0);
}

Test(strcmp, netbsd)
{
    struct {
        const char *val1;
        const char *val2;
        int expected_result;
    } table[] = {
        { "",                   "",                     0 },

        { "a",                  "a",                    0 },
        { "a",                  "b",                    -1 },
        { "b",                  "a",                    +1 },
        { "",                   "a",                    -1 },
        { "a",                  "",                     +1 },

        { "aa",                 "aa",                   0 },
        { "aa",                 "ab",                   -1 },
        { "ab",                 "aa",                   +1 },
        { "a",                  "aa",                   -1 },
        { "aa",                 "a",                    +1 },

        { "aaa",                "aaa",                  0 },
        { "aaa",                "aab",                  -1 },
        { "aab",                "aaa",                  +1 },
        { "aa",                 "aaa",                  -1 },
        { "aaa",                "aa",                   +1 },

        { "aaaa",               "aaaa",                 0 },
        { "aaaa",               "aaab",                 -1 },
        { "aaab",               "aaaa",                 +1 },
        { "aaa",                "aaaa",                 -1 },
        { "aaaa",               "aaa",                  +1 },

        { "aaaaa",              "aaaaa",                0 },
        { "aaaaa",              "aaaab",                -1 },
        { "aaaab",              "aaaaa",                +1 },
        { "aaaa",               "aaaaa",                -1 },
        { "aaaaa",              "aaaa",                 +1 },

        { "aaaaaa",             "aaaaaa",               0 },
        { "aaaaaa",             "aaaaab",               -1 },
        { "aaaaab",             "aaaaaa",               +1 },
        { "aaaaa",              "aaaaaa",               -1 },
        { "aaaaaa",             "aaaaa",                +1 },
    };

    char buffer1[64];
    char buffer2[64];
    for (size_t a1 = 0; a1 < sizeof(long); ++a1)
        for (size_t a2 = 0; a2 < sizeof(long); ++a2)
            for (size_t t = 0; t < (sizeof(table) / sizeof(table[0])); ++t) {
                memcpy(&buffer1[a1], table[t].val1, strlen(table[t].val1) + 1);
                memcpy(&buffer2[a2], table[t].val2, strlen(table[t].val2) + 1);

                check_result(&buffer1[a1], &buffer2[a2], table[t].expected_result);
            }
}

Test(strcmp, netbsd_simple)
{
    char buffer1[10] = "xxx";
    char buffer2[10] = "xxy";

    check_result(buffer1, buffer1, 0);
    check_result(buffer2, buffer2, 0);

    check_result("x\xF6x", "xox", 1);
    check_result("xxx", "xxxyyy", -1);
    check_result("xxxyyy", "xxx", 1);

    check_result(buffer1 + 0, buffer2 + 0, -1);
    check_result(buffer1 + 1, buffer2 + 1, -1);
    check_result(buffer1 + 2, buffer2 + 2, -1);
    check_result(buffer1 + 3, buffer2 + 3, 0);

    check_result(buffer2 + 0, buffer1 + 0, 1);
    check_result(buffer2 + 1, buffer1 + 1, 1);
    check_result(buffer2 + 2, buffer1 + 2, 1);
    check_result(buffer2 + 3, buffer1 + 3, 0);
}

Test(strcmp, bionic)
{
    __auto_type state = bionic_test_state_new(1024);

    for (size_t i = 1; i < state.n; ++i)
        for (bionic_test_state_begin_iters(&state); bionic_test_state_has_next_iter(&state); bionic_test_state_next_iter(&state)) {
            memset(state.ptr1, 'v', state.max_length);
            memset(state.ptr2, 'n', state.max_length);
            state.ptr1[state.lengths[i] - 1] = '\0';
            state.ptr2[state.lengths[i] - 1] = '\0';

            size_t position = 1 + (random() % (state.max_length - 1));

            int expected_result;
            if (position >= state.lengths[i] - 1) {
                memcpy(state.ptr1, state.ptr2, state.lengths[i]);
                expected_result = 0;
            } else {
                memcpy(state.ptr1, state.ptr2, position);
                if (state.ptr1[position] > state.ptr2[position])
                    expected_result = 1;
                else if (state.ptr1[position] == state.ptr2[position]) {
                    state.ptr1[position + 1] = '\0';
                    state.ptr1[position + 1] = '\0';
                    expected_result = 0;
                } else
                    expected_result = -1;
            }
            check_result(state.ptr1, state.ptr2, expected_result);
        }
}

static void bionic_do_test(uint8_t *buffer1, uint8_t *buffer2, size_t length)
{
    if (length >= 1) {
        memset(buffer1, (32 + (length % 96)), length - 1);
        buffer1[length - 1] = '\0';

        memset(buffer2, (32 + (length % 96)), length - 1);
        buffer2[length - 1] = '\0';

        check_result((char *)buffer1, (char *)buffer2, 0);
    }
}

static void bionic_do_fail_test(uint8_t *buffer1, uint8_t *buffer2, size_t length1, size_t length2)
{
    int ch = (32 + (length1 % 96));

    memset(buffer1, ch, length1 - 1);
    buffer1[length1 - 1] = '\0';

    memset(buffer2, ch, length2 - 1);
    buffer2[length2 - 1] = '\0';
    cr_assert_neq(strcmp_ptr((char *)buffer1, (char *)buffer2), 0);

    // Single character difference
    size_t length = (length1 > length2) ? length2 : length1;

    // Need at least 2 characters for this test
    if (length > 1) {
        buffer1[length - 1] = '\0';
        buffer2[length - 1] = '\0';
        int diff_ch = (ch + 1) % 96;

        buffer1[length - 2] = diff_ch;
        cr_assert_neq(strcmp_ptr((char *)buffer1, (char *)buffer2), 0);

        buffer1[length - 2] = ch;
        buffer2[length - 2] = diff_ch;
        cr_assert_neq(strcmp_ptr((char *)buffer1, (char *)buffer2), 0);
    }
}

Test(strcmp, bionic_align)
{
    bionic_run_cmp_buffer_align_test(1024 * 4, bionic_do_test, bionic_do_fail_test, bionic_buffer_large_set_increment);
}

Test(strcmp, bionic_overread)
{
    bionic_run_cmp_buffer_overread_test(bionic_do_test, bionic_do_fail_test);
}

Test(strcmp, glibc_check)
{
    glibc_test_string_init();

    char *str1 = glibc_buf1 + 0xB2C;
    char *str2 = glibc_buf1 + 0xFD8;

    strcpy(str1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs");
    strcpy(str2, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkLMNOPQRSTUV");

    // Test negative values
    str1[0] = 1;
    str2[0] = 1;
    str1[1] = 1;
    str2[1] = 1;
    str1[2] = -1;
    str2[2] = 3;
    str1[3] = 0;
    str2[3] = 1;

    // Test potential overflow
    str1[7] = CHAR_MIN;
    str2[7] = CHAR_MAX;

    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    for (size_t i1 = 0; i1 < len1; ++i1)
        for (size_t i2 = 0; i2 < len2; ++i2) {
            int expected_result = known_strcmp(str1 + i1, str2 + i2);
            check_result(str1 + i1, str2 + i2, expected_result);
        }

    // Multiple 0s after the first one
    for (size_t i = 0; i < 16 + 1; ++i) {
        str1[i] = 0;
        str2[i] = 0;
    }

    for (size_t i = 0; i < 16; ++i) {
        for (int value = 1; value < 0x100; ++value) {
            for (size_t j = 0; j < i; ++j) {
                str1[j] = value;
                str2[j] = value;
            }
            str2[i] = value;

            check_result(str1, str2, known_strcmp(str1, str2));
        }
    }
}

Test(strcmp, glibc_check2)
{
    glibc_test_string_init();

    size_t size = 32 * 4;
    char *str1 = glibc_buf1 + (GLIBC_BUF1PAGES - 1) * glibc_page_size;
    char *str2 = glibc_buf2 + (GLIBC_BUF1PAGES - 1) * glibc_page_size;

    memset(str1, 'a', glibc_page_size);
    memset(str2, 'a', glibc_page_size);
    str1[glibc_page_size - 1] = 0;
    str2[glibc_page_size - 1] = 0;

    for (size_t s = 99; s <= size; ++s)
        for (size_t s1a = 30; s1a < 32; ++s1a)
            for (size_t s2a = 30; s2a < 32; ++s2a) {
                char *s1p = str1 + (glibc_page_size - s) - s1a;
                char *s2p = str2 + (glibc_page_size - s) - s2a;

                check_result(s1p, s2p, known_strcmp(s1p, s2p));
            }
}

Test(strcmp, glibc_check3)
{
    glibc_test_string_init();

    size_t size = 0xD000 + 0x4000;

    char *buffer1 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    char *buffer2 = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    cr_assert_neq(buffer1, MAP_FAILED);
    cr_assert_neq(buffer2, MAP_FAILED);

    char *str1 = buffer1 + 0x8F8;
    char *str2 = buffer2 + 0xCFF3;

    strcpy(str1, "/export/redhat/rpms/BUILD/java-1.8.0-openjdk-1.8.0.312.b07-2.fc35.x86_64/openjdk/langtools/src/share/classes/com/sun/tools/doclets/internal/toolkit/util/PathDocFileFactory.java");
    strcpy(str2, "/export/redhat/rpms/BUILD/java-1.8.0-openjdk-1.8.0.312.b07-2.fc35.x86_64/openjdk/langtools/src/share/classes/com/sun/tools/doclets/internal/toolkit/taglets/ThrowsTaglet.java");

    check_result(str1, str2, known_strcmp(str1, str2));

    munmap(buffer1, size);
    munmap(buffer2, size);
}

static void glibc_do_test(size_t alignment1, size_t alignment2, size_t length, int max_char, int expected_result)
{
    if (length == 0)
        return;

    alignment1 &= 63;
    if (alignment1 + (length + 1) > glibc_page_size)
        return;

    alignment2 &= 63;
    if (alignment2 + (length + 1) > glibc_page_size)
        return;

    size_t i = alignment1 + (length + 2);
    char *str1 = glibc_buf1 + ((glibc_page_size - i) / 16 * 16) + alignment1;

    i = alignment2 + (length + 2);
    char *str2 = glibc_buf2 + ((glibc_page_size - i) / 16 * 16) + alignment2;

    for (size_t i = 0; i < length; ++i) {
        str1[i] = 1 + 23 * i % max_char;
        str2[i] = str1[i];
    }

    str1[length] = '\0';
    str2[length] = '\0';
    str1[length + 1] = 23;
    str2[length + 1] = 24 + expected_result;
    str2[length - 1] -= expected_result;
    check_result(str1, str2, expected_result);
}

Test(strcmp, glibc)
{
    glibc_test_string_init();

    for (size_t i = 1; i < 32; ++i) {
        glibc_do_test(i, i, i, 0x7F, 0);
        glibc_do_test(i, i, i, 0x7F, 1);
        glibc_do_test(i, i, i, 0x7F, -1);
    }

    for (size_t i = 1; i < 10; ++i) {
        glibc_do_test(0, 0, 2 << i, 0x7F, 0);
        glibc_do_test(0, 0, 2 << i, 0xFE, 0);
        glibc_do_test(0, 0, 2 << i, 0x7F, 1);
        glibc_do_test(0, 0, 2 << i, 0xFE, 1);
        glibc_do_test(0, 0, 2 << i, 0x7F, -1);
        glibc_do_test(0, 0, 2 << i, 0xFE, -1);
        glibc_do_test(0, i, 2 << i, 0x7F, 1);
        glibc_do_test(i, i + 1, 2 << i, 0xFE, 1);
    }

    for (size_t i = 1; i < 8; ++i) {
        glibc_do_test(i, 2 * i, 8 << i, 0x7F, 0);
        glibc_do_test(2 * i, i, 8 << i, 0xFE, 0);
        glibc_do_test(i, 2 * i, 8 << i, 0x7F, 1);
        glibc_do_test(2 * i, i, 8 << i, 0xFE, 1);
        glibc_do_test(i, 2 * i, 8 << i, 0x7F, -1);
        glibc_do_test(2 * i, i, 8 << i, 0xFE, -1);
    }
}

Test(strcmp, glibc_random)
{
    glibc_test_string_init();

    __auto_type ptr1 = (unsigned char *)(glibc_buf1 + glibc_page_size - 512);
    __auto_type ptr2 = (unsigned char *)(glibc_buf2 + glibc_page_size - 512);

    for (size_t n = 0; n < 50000; ++n) {
        size_t alignment1 = random() & 31;
        size_t alignment2 = (random() & 1) ? (random() & 31) : (alignment1 + (random() & 24));
        size_t position = random() & 511;
        size_t j = alignment1 > alignment2 ? alignment1 : alignment2;

        if (position + j >= 511)
            position = 510 - j - (random() & 7);

        size_t length1 = random() & 511;
        if (position >= length1 && (random() & 1))
            length1 = position + (random() & 7);

        if (length1 + j >= 512)
            length1 = 511 - j - (random() & 7);

        size_t length2 = (position >= length1) ? length1 : (length1 + (length1 != 511 - j ? random() % (511 - j - length1) : 0));

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
            if (i < length2 + alignment1 && ptr2[i] == '\0') {
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
                if (ptr2[alignment2 + position] == ptr1[alignment2 + position])
                    ptr2[alignment2 + position] = ptr1[alignment1 + position] + 3 + (random() & 127);
            }

            if (ptr1[alignment1 + position] < ptr2[alignment2 + position])
                expected_result = -1;
            else
                expected_result = 1;
        }
        ptr1[length1 + alignment1] = 0;
        ptr2[length2 + alignment2] = 0;

        check_result((char *)ptr1 + alignment1, (char *)ptr2 + alignment2, expected_result);
    }
}

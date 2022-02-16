/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests string stuff
*/

#include <string.h>
#include <criterion/criterion.h>

static size_t (*volatile strlen_ptr)(const char *) = strlen;

static int (*volatile strcmp_ptr)(const char *, const char *) = strcmp;
static int (*volatile strncmp_ptr)(const char *, const char *, size_t) = strncmp;

static char *(*volatile strcpy_ptr)(char *restrict, const char *restrict) = strcpy;
static char *(*volatile strncpy_ptr)(char *restrict, const char *restrict, size_t) = strncpy;

static char *(*volatile strcat_ptr)(char *restrict, const char *restrict) = strcat;
static char *(*volatile strncat_ptr)(char *restrict, const char *restrict, size_t) = strncat;

static char *(*volatile strchr_ptr)(const char *, int) = strchr;
static char *(*volatile strrchr_ptr)(const char *, int) = strrchr;

static size_t (*volatile strspn_ptr)(const char *, const char *) = strspn;
static size_t (*volatile strcspn_ptr)(const char *, const char *) = strcspn;

static char *(*volatile strpbrk_ptr)(const char *, const char *) = strpbrk;

static char *(*volatile strtok_ptr)(char *restrict, const char *restrict) = strtok;

static void *(*volatile memcpy_ptr)(void *restrict, const void *restrict, size_t) = memcpy;
static void *(*volatile memmove_ptr)(void *restrict, const void *restrict, size_t) = memmove;

static void *(*volatile memset_ptr)(void *, int, size_t) = memset;

static int (*volatile memcmp_ptr)(const void *, const void *, size_t) = memcmp;

static void *(*volatile memchr_ptr)(const void *, int, size_t) = memchr;

#define picolibc_test(result_var, result, expected)      \
    do {                                                 \
        (result_var) = (result);                         \
        cr_assert_eq((result_var), (expected));          \
    } while (false)

static void picolibc_test_str(char *str, char *expected)
{
    cr_assert_eq(strcmp_ptr(str, expected), 0);
}

Test(string, picolibc)
{
    char buffer[32];
    buffer[16] = 'a';
    buffer[17] = 'b';
    buffer[18] = 'c';
    buffer[19] = '\0';

    char *result_cptr;
    picolibc_test(result_cptr, strcpy_ptr(buffer, buffer + 16), buffer);
    picolibc_test_str(result_cptr, "abc");
    picolibc_test(result_cptr, strcpy_ptr(buffer + 1, buffer + 16), buffer + 1);
    picolibc_test_str(result_cptr, "abc");
    picolibc_test(result_cptr, strcpy_ptr(buffer + 2, buffer + 16), buffer + 2);
    picolibc_test_str(result_cptr, "abc");
    picolibc_test(result_cptr, strcpy_ptr(buffer + 3, buffer + 16), buffer + 3);
    picolibc_test_str(result_cptr, "abc");

    picolibc_test(result_cptr, strcpy_ptr(buffer + 1, buffer + 17), buffer + 1);
    picolibc_test_str(result_cptr, "bc");
    picolibc_test(result_cptr, strcpy_ptr(buffer + 2, buffer + 18), buffer + 2);
    picolibc_test_str(result_cptr, "c");
    picolibc_test(result_cptr, strcpy_ptr(buffer + 3, buffer + 19), buffer + 3);
    picolibc_test_str(result_cptr, "");

    picolibc_test(result_cptr, memset_ptr(buffer, 'x', sizeof(buffer)), buffer);
    picolibc_test(result_cptr, strncpy_ptr(buffer, "abc", sizeof(buffer) - 1), buffer);

    int result_int;
    picolibc_test(result_int, memcmp_ptr(buffer, "abc\0\0\0\0", 8), 0);
    picolibc_test(result_int, buffer[sizeof(buffer) - 1], 'x');

    buffer[3] = 'x';
    buffer[4] = '\0';
    strncpy_ptr(buffer, "abc", 3);
    picolibc_test(result_int, buffer[2], 'c');
    picolibc_test(result_int, buffer[3], 'x');

    picolibc_test(result_int, !strncmp_ptr("abcd", "abce", 3), 1);
    picolibc_test(result_int, !!strncmp_ptr("abc", "abd", 3), 1);

    strcpy_ptr(buffer, "abc");
    picolibc_test(result_cptr, strncat_ptr(buffer, "123456", 3), buffer);
    picolibc_test(result_int, buffer[6], 0);
    picolibc_test_str(result_cptr, "abc123");

    strcpy_ptr(buffer, "aaababccdd0001122223");
    picolibc_test(result_cptr, strchr_ptr(buffer, 'b'), buffer + 3);
    picolibc_test(result_cptr, strrchr_ptr(buffer, 'b'), buffer + 5);
    picolibc_test(result_int, strspn_ptr(buffer, "abcd"), 10);
    picolibc_test(result_int, strcspn_ptr(buffer, "0123"), 10);
    picolibc_test(result_cptr, strpbrk_ptr(buffer, "0123"), buffer + 10);

    strcpy_ptr(buffer, "abc   123; xyz; foo");
    picolibc_test(result_cptr, strtok_ptr(buffer, " "), buffer);
    picolibc_test_str(result_cptr, "abc");

    picolibc_test(result_cptr, strtok_ptr(NULL, ";"), buffer + 4);
    picolibc_test_str(result_cptr, "  123");

    picolibc_test(result_cptr, strtok_ptr(NULL, " ;"), buffer + 11);
    picolibc_test_str(result_cptr, "xyz");

    picolibc_test(result_cptr, strtok_ptr(NULL, " ;"), buffer + 16);
    picolibc_test_str(result_cptr, "foo");
}

static void newlib_copy(char *target, const char *source, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        target[i] = source[i];
}

static void newlib_set(char *target, char character, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        target[i] = character;
}

static void newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(const char *str, const char *expected, size_t length)
{
    cr_assert_eq(strcmp_ptr(str, expected), 0);
    cr_assert_eq(strncmp_ptr(str, expected, length), 0);
    cr_assert_eq(strncmp_ptr(str, expected, length + 1), 0);
    cr_assert_eq(strlen_ptr(str), length);
    cr_assert_eq(memcmp_ptr(str, expected, length + 1), 0);
    cr_assert_eq(strchr_ptr(str, *str), str);
    cr_assert_eq(memchr_ptr(str, *str, length + 1), str);
    if (length != 0)
        cr_assert_eq(memchr_ptr(str, *str, length), str);
    else
        cr_assert_null(memchr_ptr(str, *str, length));
}

static void checked_memset(void *ptr, int ch, size_t len)
{
    cr_assert_eq(memset_ptr(ptr, ch, len), ptr);
}

static void checked_memcpy(void *restrict dest, const void *restrict src, size_t len)
{
    cr_assert_eq(memcpy_ptr(dest, src, len), dest);
}

static void checked_memmove(void *restrict dest, const void *restrict src, size_t len)
{
    cr_assert_eq(memmove_ptr(dest, src, len), dest);
}

static void checked_strcpy(char *restrict dest, const char *src)
{
    cr_assert_eq(strcpy_ptr(dest, src), dest);
}

static void checked_strcat(char *restrict dest, const char *src)
{
    cr_assert_eq(strcat_ptr(dest, src), dest);
}

static void checked_strncpy(char *restrict dest, const char *src, size_t n)
{
    cr_assert_eq(strncpy_ptr(dest, src, n), dest);
}

static void checked_strncat(char *restrict dest, const char *src, size_t n)
{
    cr_assert_eq(strncat_ptr(dest, src, n), dest);
}

Test(string, newlib)
{
    enum {
        MAX_1 = 33000,
        MAX_2 = (2 * MAX_1 + MAX_1 / 10),
    };

    char buffer2[MAX_1];
    char *tmp2 = buffer2;

    tmp2[0] = 'Z';
    tmp2[1] = '\0';

    char target[MAX_1] = "A";
    checked_memset(target, 'X', 0);
    checked_memcpy(target, "Y", 0);
    checked_memmove(target, "K", 0);
    checked_strncpy(tmp2, "4", 0);
    checked_strncat(tmp2, "123", 0);
    checked_strcat(target, "");

    newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(target, "A", 1);
    cr_assert_null(memchr_ptr(target, 'A', 0));
    cr_assert_eq(memcmp_ptr(target, "J", 0), 0);
    cr_assert_eq(strncmp_ptr(target, "A", 1), 0);
    cr_assert_eq(strncmp_ptr(target, "J", 0), 0);
    cr_assert_eq(tmp2[0], 'Z');
    cr_assert_eq(tmp2[1], '\0');

    tmp2[2] = 'A';
    checked_strcpy(target, "");
    checked_strncpy(tmp2, "", 4);
    checked_strcat(target, "");

    target[0] = '\0';
    cr_assert_eq(strncmp_ptr(target, "", 1), 0);
    cr_assert_eq(memcmp_ptr(tmp2, "\0\0\0\0", 4), 0);

    tmp2[2] = 'A';
    cr_assert_eq(strncat_ptr(tmp2, "1", 3), tmp2);
    cr_assert_eq(memcmp_ptr(tmp2, "1\0A", 3), 0);

    char buffer3[MAX_1];
    char *tmp3 = buffer3;
    checked_strcpy(tmp3, target);
    checked_strcat(tmp3, "X");
    checked_strncpy(tmp2, "X", 2);
    checked_memset(target, tmp2[0], 1);

    newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(target, "X", 1);
    cr_assert_null(memchr_ptr(target, 'Y', 2));
    cr_assert_null(strchr_ptr(target, 'Y'));
    cr_assert_eq(strcmp_ptr(tmp3, target), 0);
    cr_assert_eq(strncmp_ptr(tmp3, target, 2), 0);
    cr_assert_eq(memcmp_ptr(target, "K", 0), 0);
    cr_assert_eq(strncmp_ptr(target, tmp3, 3), 0);

    checked_strcpy(tmp3, "Y");
    checked_strcat(tmp3, "Y");
    checked_memset(target, 'Y', 2);

    target[2] = '\0';
    newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(target, "YY", 2);
    cr_assert_eq(strcmp_ptr(tmp3, target), 0);
    cr_assert_eq(strncmp_ptr(target, tmp3, 3), 0);
    cr_assert_eq(strncmp_ptr(target, tmp3, 4), 0);
    cr_assert_eq(strncmp_ptr(target, tmp3, 2), 0);

    checked_strcpy(target, "WW");
    newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(target, "WW", 2);

    checked_strncpy(target, "XX", 16);
    cr_assert_eq(memcmp_ptr(target, "XX\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16), 0);

    checked_strcpy(tmp3, "ZZ");
    checked_strcat(tmp3, "Z");

    char buffer4[MAX_1];
    char *tmp4 = buffer4;
    checked_memcpy(tmp4, "Z", 2);
    checked_strcat(tmp4, "ZZ");
    checked_memset(target, 'Z', 3);

    target[3] = '\0';

    char buffer5[MAX_2];
    char *tmp5 = buffer5;
    tmp5[0] = '\0';
    checked_strncat(tmp5, "123", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(target, "ZZZ", 3);
    cr_assert_eq(strcmp_ptr(tmp3, target), 0);
    cr_assert_eq(strcmp_ptr(tmp4, target), 0);
    cr_assert(strncmp_ptr(target, "ZZY", 3) > 0);
    cr_assert(strncmp_ptr("ZZY", target, 4) < 0);
    cr_assert_eq(memcmp_ptr(tmp5, "12", 3), 0);

    target[2] = 'K';
    newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(target, "ZZK", 3);
    cr_assert_eq(memcmp_ptr(target, "ZZZ", 2), 0);
    cr_assert(strcmp_ptr(target, "ZZZ") < 0);
    cr_assert(memcmp_ptr(target, "ZZZ", 3) < 0);
    cr_assert_eq(memchr_ptr(target, 'K', 3), target + 2);
    cr_assert_eq(strncmp_ptr(target, "ZZZ", 2), 0);
    cr_assert(strncmp_ptr(target, "ZZZ", 4) < 0);
    cr_assert_eq(strchr_ptr(target, 'K'), target + 2);

    checked_strcpy(target, "AAA");
    newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(target, "AAA", 3);

    char expected[MAX_1];
    char buffer6[MAX_2];
    char buffer7[MAX_2];

    size_t j = 5;
    while (j < MAX_1) {
        for (size_t i = j - 1; i <= j + 1; ++i) {
            size_t align_test_iterations;
            size_t z;
            if (i <= 16 * sizeof(long)) {
                align_test_iterations = 2 * sizeof(long);
                if (i <= 2 * sizeof(long) + 1)
                    z = 2;
                else
                    z = 2 * sizeof(long);
            } else
                align_test_iterations = 1;

            for (size_t x = 0; x < align_test_iterations; ++x) {
                char *tmp1 = target + x;
                char array[] = "abcdefghijklmnopqrstuvwxz";
                char first_char = array[i % (sizeof(array) - 1)];
                checked_memset(tmp1, first_char, i);
                newlib_copy(tmp2, tmp1, i);

                char array2[] = "0123456789!@#$%^&*(";
                char second_char = array2[i % (sizeof(array2) - 1)];
                newlib_set(tmp2 + z, second_char, i - z - 1);
                checked_memcpy(tmp1 + z, tmp2 + z, i - z - 1);

                tmp1[i] = '\0';
                tmp2[i] = '\0';
                checked_strcpy(expected, tmp2);
                tmp2[i - z] = first_char + 1;
                checked_memmove(tmp2 + z + 1, tmp2 + z, i - z - 1);
                checked_memset(tmp3, first_char, i);

                newlib_set(tmp4, first_char, i);
                tmp5[0] = '\0';
                checked_strncpy(tmp5, tmp1, i + 1);
                checked_strcat(tmp5, tmp1);

                char *tmp6 = buffer6 + x;
                newlib_copy(tmp6, tmp1, i);
                newlib_copy(tmp6 + i, tmp1, i + 1);

                char *tmp7 = buffer7;
                tmp7[2 * i + z] = second_char;
                checked_strcpy(tmp7, tmp1);

                strchr_ptr(tmp1, second_char);

                newlib_check_strcmp_strncmp_strlen_memcmp_strchr_memchr(tmp1, expected, i);
                cr_assert(strcmp_ptr(tmp1, tmp2) < 0);
                cr_assert(memcmp_ptr(tmp1, tmp2, i) < 0);
                cr_assert(strncmp_ptr(tmp1, tmp2, i + 1) < 0);
                cr_assert_eq(memchr_ptr(tmp1, first_char, i), tmp1);
                cr_assert_eq(strchr_ptr(tmp1, first_char), tmp1);
                cr_assert_eq(memchr_ptr(tmp1, second_char, i), tmp1 + z);
                cr_assert_eq(strchr_ptr(tmp1, second_char), tmp1 + z);
                cr_assert_eq(strcmp_ptr(tmp5, tmp6), 0);
                cr_assert_eq(strncat_ptr(tmp7, tmp1, i + 2), tmp7);
                cr_assert_eq(strcmp_ptr(tmp7, tmp6), 0);
                cr_assert_eq(tmp7[2 * i + z], second_char);

                for (size_t k = 1; k <= align_test_iterations && k <= i; ++k) {
                    cr_assert_eq(memcmp_ptr(tmp3, tmp4, i - k + 1), 0);
                    cr_assert_eq(strncmp_ptr(tmp3, tmp4, i - k + 1), 0);
                    tmp4[i - k] = first_char + 1;
                    cr_assert(memcmp_ptr(tmp3, tmp4, i) < 0);
                    cr_assert(strncmp_ptr(tmp3, tmp4, i) < 0);
                    cr_assert(memcmp_ptr(tmp4, tmp3, i) > 0);
                    cr_assert(strncmp_ptr(tmp4, tmp3, i) > 0);
                    tmp4[i - k] = first_char;
                }
            }
        }
        j = ((2 * j) >> 2) << 2;
    }
}

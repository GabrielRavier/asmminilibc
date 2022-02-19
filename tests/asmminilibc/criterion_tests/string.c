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

static char *(*volatile index_ptr)(const char *, int) = index;
static char *(*volatile rindex_ptr)(const char *, int) = rindex;

static size_t (*volatile strspn_ptr)(const char *, const char *) = strspn;
static size_t (*volatile strcspn_ptr)(const char *, const char *) = strcspn;

static char *(*volatile strpbrk_ptr)(const char *, const char *) = strpbrk;

static char *(*volatile strtok_ptr)(char *restrict, const char *restrict) = strtok;

static char *(*volatile strstr_ptr)(const char *, const char *) = strstr;

static void *(*volatile memcpy_ptr)(void *restrict, const void *restrict, size_t) = memcpy;
static void *(*volatile memmove_ptr)(void *, const void *, size_t) = memmove;

static void *(*volatile memccpy_ptr)(void *restrict, const void *restrict, int, size_t) = memccpy;

static void *(*volatile memset_ptr)(void *, int, size_t) = memset;

static int (*volatile memcmp_ptr)(const void *, const void *, size_t) = memcmp;

static void *(*volatile memchr_ptr)(const void *, int, size_t) = memchr;

static void (*volatile bcopy_ptr)(const void *, void *, size_t) = bcopy;
static void (*volatile bzero_ptr)(void *, size_t) = bzero;
static int (*volatile bcmp_ptr)(const void *, const void *, size_t) = bcmp;

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

static void newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(const char *str, const char *expected, size_t length)
{
    cr_assert_eq(strcmp_ptr(str, expected), 0);
    cr_assert_eq(strncmp_ptr(str, expected, length), 0);
    cr_assert_eq(strncmp_ptr(str, expected, length + 1), 0);
    cr_assert_eq(strlen_ptr(str), length);
    cr_assert_eq(strcspn_ptr(str, ""), length);
    cr_assert_eq(memcmp_ptr(str, expected, length), 0);
    cr_assert_eq(memcmp_ptr(str, expected, length + 1), 0);
    cr_assert_eq(bcmp_ptr(str, expected, length), 0);
    cr_assert_eq(bcmp_ptr(str, expected, length + 1), 0);
    cr_assert_eq(strchr_ptr(str, *str), str);
    cr_assert_eq(index_ptr(str, *str), str);
    cr_assert_eq(strchr_ptr(str, '\0'), str + length);
    cr_assert_eq(index_ptr(str, '\0'), str + length);
    cr_assert_eq(strrchr_ptr(str, '\0'), str + length);
    cr_assert_eq(rindex_ptr(str, '\0'), str + length);
    if (length != 0) {
        cr_assert_eq(strrchr_ptr(str, str[length - 1]), str + length - 1);
        cr_assert_eq(rindex_ptr(str, str[length - 1]), str + length - 1);
    }
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

static void checked_memmove(void *dest, const void *src, size_t len)
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

    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(target, "A", 1);
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

    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(target, "X", 1);
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
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(target, "YY", 2);
    cr_assert_eq(strcmp_ptr(tmp3, target), 0);
    cr_assert_eq(strncmp_ptr(target, tmp3, 3), 0);
    cr_assert_eq(strncmp_ptr(target, tmp3, 4), 0);
    cr_assert_eq(strncmp_ptr(target, tmp3, 2), 0);

    checked_strcpy(target, "WW");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(target, "WW", 2);

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
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(target, "ZZZ", 3);
    cr_assert_eq(strcmp_ptr(tmp3, target), 0);
    cr_assert_eq(strcmp_ptr(tmp4, target), 0);
    cr_assert(strncmp_ptr(target, "ZZY", 3) > 0);
    cr_assert(strncmp_ptr("ZZY", target, 4) < 0);
    cr_assert_eq(memcmp_ptr(tmp5, "12", 3), 0);

    target[2] = 'K';
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(target, "ZZK", 3);
    cr_assert_eq(memcmp_ptr(target, "ZZZ", 2), 0);
    cr_assert(strcmp_ptr(target, "ZZZ") < 0);
    cr_assert(memcmp_ptr(target, "ZZZ", 3) < 0);
    cr_assert_eq(memchr_ptr(target, 'K', 3), target + 2);
    cr_assert_eq(strncmp_ptr(target, "ZZZ", 2), 0);
    cr_assert(strncmp_ptr(target, "ZZZ", 4) < 0);
    cr_assert_eq(strchr_ptr(target, 'K'), target + 2);

    checked_strcpy(target, "AAA");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(target, "AAA", 3);

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

                newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(tmp1, expected, i);
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

Test(string, newlib_libm)
{
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr("", "", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr("a", "a", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr("abc", "abc", 3);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr("abcd", "abcd", 4);
    cr_assert(strcmp_ptr("abc", "abcd") < 0);
    cr_assert(strcmp_ptr("abcd", "abc") > 0);
    cr_assert(strcmp_ptr("abcd", "abce") < 0);
    cr_assert(strcmp_ptr("abce", "abcd") > 0);
    cr_assert(strcmp_ptr("a\103", "a") > 0);
    cr_assert(strcmp_ptr("a\103", "a\003") > 0);

    char one[50];
    checked_strcpy(one, "abcd");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abcd", 4);

    checked_strcpy(one, "x");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "x", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 2, "cd", 2);

    char two[50];
    checked_strcpy(two, "hi there");
    checked_strcpy(one, two);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "hi there", 8);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "hi there", 8);

    checked_strcpy(one, "");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "", 0);

    checked_strcpy(one, "ijk");
    checked_strcat(one, "lmn");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ijklmn", 6);

    checked_strcpy(one, "x");
    checked_strcat(one, "yz");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "xyz", 3);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 4, "mn", 2);

    checked_strcpy(one, "gh");
    checked_strcpy(two, "ef");
    checked_strcat(one, two);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ghef", 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "ef", 2);

    checked_strcpy(one, "");
    checked_strcat(one, "");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "", 0);

    checked_strcpy(one, "ab");
    checked_strcat(one, "");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ab", 2);

    checked_strcpy(one, "");
    checked_strcat(one, "cd");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "cd", 2);

    checked_strcpy(one, "ijk");
    checked_strncat(one, "lmn", 99);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ijklmn", 6);

    checked_strcpy(one, "x");
    checked_strncat(one, "yz", 99);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "xyz", 3);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 4, "mn", 2);

    checked_strcpy(one, "gh");
    checked_strcpy(two, "ef");
    checked_strncat(one, two, 99);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ghef", 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "ef", 2);

    checked_strcpy(one, "");
    checked_strncat(one, "", 99);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "", 0);

    checked_strcpy(one, "ab");
    checked_strncat(one, "", 99);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ab", 2);

    checked_strcpy(one, "");
    checked_strncat(one, "cd", 99);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "cd", 2);

    checked_strcpy(one, "ab");
    checked_strncat(one, "cdef", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abcd", 4);

    checked_strncat(one, "gh", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abcd", 4);

    checked_strncat(one, "gh", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abcdgh", 6);

    cr_assert(strncmp_ptr("abc", "abcd", 99) < 0);
    cr_assert(strncmp_ptr("abcd", "abc", 99) > 0);
    cr_assert(strncmp_ptr("abcd", "abce", 99) < 0);
    cr_assert(strncmp_ptr("abce", "abcd", 99) > 0);
    cr_assert(strncmp_ptr("a\103", "a", 99) > 0);
    cr_assert(strncmp_ptr("a\103", "a\003", 99) > 0);

    cr_assert_eq(strncmp_ptr("abce", "abcd", 3), 0);
    cr_assert_eq(strncmp_ptr("abce", "abc", 3), 0);
    cr_assert(strncmp_ptr("abcd", "abce", 4) < 0);
    cr_assert_eq(strncmp_ptr("abc", "def", 0), 0);

    checked_strncpy(one, "abc", 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "abcdefgh");
    checked_strncpy(one, "xyz", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "xycdefgh", 8);

    checked_strcpy(one, "abcdefgh");
    checked_strncpy(one, "xyz", 3);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "xyzdefgh", 8);

    checked_strcpy(one, "abcdefgh");
    checked_strncpy(one, "xyz", 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "xyz", 3);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 4, "efgh", 4);

    checked_strcpy(one, "abcdefgh");
    checked_strncpy(one, "xyz", 5);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "xyz", 3);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 4, "", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 5, "fgh", 3);

    checked_strcpy(one, "abc");
    checked_strncpy(one, "xyz", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strncpy(one, "", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 1, "", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 2, "c", 1);

    checked_strcpy(one, "hi there");
    checked_strncpy(two, one, 9);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "hi there", 8);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "hi there", 8);

    cr_assert_null(strchr_ptr("abcd", 'z'));
    checked_strcpy(one, "abcd");
    cr_assert_eq(strchr_ptr(one, 'b'), one + 1);
    cr_assert_eq(strchr_ptr(one, 'c'), one + 2);
    cr_assert_eq(strchr_ptr(one, 'd'), one + 3);
    cr_assert_eq(strchr_ptr(one, 'a'), one);
    cr_assert_eq(strchr_ptr(one, '\0'), one + 4);

    checked_strcpy(one, "ababa");
    cr_assert_eq(strchr_ptr(one, 'b'), one + 1);

    checked_strcpy(one, "");
    cr_assert_null(strchr_ptr(one, 'b'));
    cr_assert_eq(strchr_ptr(one, '\0'), one);

    cr_assert_null(index_ptr("abcd", 'z'));
    checked_strcpy(one, "abcd");
    cr_assert_eq(index_ptr(one, 'b'), one + 1);
    cr_assert_eq(index_ptr(one, 'c'), one + 2);
    cr_assert_eq(index_ptr(one, 'd'), one + 3);
    cr_assert_eq(index_ptr(one, 'a'), one);
    cr_assert_eq(index_ptr(one, '\0'), one + 4);

    checked_strcpy(one, "ababa");
    cr_assert_eq(index_ptr(one, 'b'), one + 1);

    checked_strcpy(one, "");
    cr_assert_null(index_ptr(one, 'b'));
    cr_assert_eq(index_ptr(one, '\0'), one);

    cr_assert_null(strrchr_ptr("abcd", 'z'));
    checked_strcpy(one, "abcd");
    cr_assert_eq(strrchr_ptr(one, 'b'), one + 1);
    cr_assert_eq(strrchr_ptr(one, 'c'), one + 2);
    cr_assert_eq(strrchr_ptr(one, 'd'), one + 3);
    cr_assert_eq(strrchr_ptr(one, 'a'), one);
    cr_assert_eq(strrchr_ptr(one, '\0'), one + 4);

    checked_strcpy(one, "ababa");
    cr_assert_eq(strrchr_ptr(one, 'b'), one + 3);

    checked_strcpy(one, "");
    cr_assert_null(strrchr_ptr(one, 'b'));
    cr_assert_eq(strrchr_ptr(one, '\0'), one);

    cr_assert_null(rindex_ptr("abcd", 'z'));
    checked_strcpy(one, "abcd");
    cr_assert_eq(rindex_ptr(one, 'b'), one + 1);
    cr_assert_eq(rindex_ptr(one, 'c'), one + 2);
    cr_assert_eq(rindex_ptr(one, 'd'), one + 3);
    cr_assert_eq(rindex_ptr(one, 'a'), one);
    cr_assert_eq(rindex_ptr(one, '\0'), one + 4);

    checked_strcpy(one, "ababa");
    cr_assert_eq(rindex_ptr(one, 'b'), one + 3);

    checked_strcpy(one, "");
    cr_assert_null(rindex_ptr(one, 'b'));
    cr_assert_eq(rindex_ptr(one, '\0'), one);

    cr_assert_null(strpbrk_ptr("abcd", "z"));
    checked_strcpy(one, "abcd");
    cr_assert_eq(strpbrk_ptr(one, "b"), one + 1);
    cr_assert_eq(strpbrk_ptr(one, "c"), one + 2);
    cr_assert_eq(strpbrk_ptr(one, "d"), one + 3);
    cr_assert_eq(strpbrk_ptr(one, "a"), one);
    cr_assert_null(strpbrk_ptr(one, ""));
    cr_assert_eq(strpbrk_ptr(one, "cb"), one + 1);

    checked_strcpy(one, "abcabdea");
    cr_assert_eq(strpbrk_ptr(one, "b"), one + 1);
    cr_assert_eq(strpbrk_ptr(one, "cb"), one + 1);
    cr_assert_eq(strpbrk_ptr(one, "db"), one + 1);

    checked_strcpy(one, "");
    cr_assert_null(strpbrk_ptr(one, "b"));
    cr_assert_null(strpbrk_ptr(one, "bc"));
    cr_assert_null(strpbrk_ptr(one, ""));

    cr_assert_null(strstr_ptr("z", "abcd"));
    cr_assert_null(strstr_ptr("abx", "abcd"));

    checked_strcpy(one, "abcd");
    cr_assert_eq(strstr_ptr(one, "c"), one + 2);
    cr_assert_eq(strstr_ptr(one, "bc"), one + 1);
    cr_assert_eq(strstr_ptr(one, "d"), one + 3);
    cr_assert_eq(strstr_ptr(one, "cd"), one + 2);
    cr_assert_eq(strstr_ptr(one, "a"), one);
    cr_assert_eq(strstr_ptr(one, "ab"), one);
    cr_assert_eq(strstr_ptr(one, "abc"), one);
    cr_assert_eq(strstr_ptr(one, "abcd"), one);
    cr_assert_null(strstr_ptr(one, "de"));
    cr_assert_eq(strstr_ptr(one, ""), one);

    checked_strcpy(one, "ababa");
    cr_assert_eq(strstr_ptr(one, "ba"), one + 1);

    checked_strcpy(one, "");
    cr_assert_null(strstr_ptr(one, "b"));
    cr_assert_eq(strstr_ptr(one, ""), one);

    checked_strcpy(one, "bcbca");
    cr_assert_eq(strstr_ptr(one, "bca"), one + 2);

    checked_strcpy(one, "bbbcabbca");
    cr_assert_eq(strstr_ptr(one, "bbca"), one + 1);

    cr_assert_eq(strspn_ptr("abcba", "abc"), 5);
    cr_assert_eq(strspn_ptr("abcba", "ab"), 2);
    cr_assert_eq(strspn_ptr("abcba", "qx"), 0);
    cr_assert_eq(strspn_ptr("", "ab"), 0);
    cr_assert_eq(strspn_ptr("abc", ""), 0);

    cr_assert_eq(strcspn_ptr("abcba", "qx"), 5);
    cr_assert_eq(strcspn_ptr("abcba", "cx"), 2);
    cr_assert_eq(strcspn_ptr("abc", "abc"), 0);
    cr_assert_eq(strcspn_ptr("", "ab"), 0);
    cr_assert_eq(strcspn_ptr("abc", ""), 3);

    checked_strcpy(one, "first, second, third");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(one, ", "), "first", 5);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "first", 5);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ", "), "second", 6);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ", "), "third", 5);
    cr_assert_null(strtok_ptr(NULL, ", "));

    checked_strcpy(one, ", first, ");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(one, ", "), "first", 5);
    cr_assert_null(strtok_ptr(NULL, ", "));

    checked_strcpy(one, "1a, 1b; 2a, 2b");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(one, ", "), "1a", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, "; "), "1b", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ", "), "2a", 2);

    checked_strcpy(two, "x-y");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(two, "-"), "x", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, "-"), "y", 1);
    cr_assert_null(strtok_ptr(NULL, "-"));

    checked_strcpy(one, "a,b, c,, ,d");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(one, ", "), "a", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ", "), "b", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ", "), "c", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ", "), "d", 1);
    cr_assert_null(strtok_ptr(NULL, ", "));
    cr_assert_null(strtok_ptr(NULL, ", "));

    checked_strcpy(one, ", ");
    cr_assert_eq(strtok_ptr(one, ", "), NULL);

    checked_strcpy(one, "");
    cr_assert_eq(strtok_ptr(one, ", "), NULL);

    checked_strcpy(one, "abc");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(one, ", "), "abc", 3);
    cr_assert_null(strtok_ptr(NULL, ", "));

    checked_strcpy(one, "abc");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(one, ""), "abc", 3);
    cr_assert_null(strtok_ptr(NULL, ""));

    checked_strcpy(one, "abcdefgh");
    checked_strcpy(one, "a,b,c");
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(one, ","), "a", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ","), "b", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(strtok_ptr(NULL, ","), "c", 1);
    cr_assert_null(strtok_ptr(NULL, ","));

    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 6, "gh", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "a", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 2, "b", 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 4, "c", 1);

    cr_assert(memcmp_ptr("abcd", "abce", 4) < 0);
    cr_assert(memcmp_ptr("abce", "abcd", 4) > 0);
    cr_assert(memcmp_ptr("alph", "beta", 4) < 0);
    cr_assert_eq(memcmp_ptr("abce", "abcd", 3), 0);
    cr_assert_eq(memcmp_ptr("abc", "def", 0), 0);

    one[0] = 0xFE;
    two[0] = 0x03;
    cr_assert(memcmp_ptr(one, two, 1) > 0);

    cr_assert_null(memchr_ptr("abcd", 'z', 4));

    checked_strcpy(one, "abcd");
    cr_assert_eq(memchr_ptr(one, 'c', 4), one + 2);
    cr_assert_eq(memchr_ptr(one, 'd', 4), one + 3);
    cr_assert_eq(memchr_ptr(one, 'a', 4), one);
    cr_assert_eq(memchr_ptr(one, '\0', 5), one + 4);

    checked_strcpy(one, "ababa");
    cr_assert_eq(memchr_ptr(one, 'b', 5), one + 1);
    cr_assert_null(memchr_ptr(one, 'b', 0));
    cr_assert_eq(memchr_ptr(one, 'a', 1), one);

    checked_strcpy(one, "a\203b");
    cr_assert_eq(memchr_ptr(one, 0203, 3), one + 1);

    checked_memcpy(one, "abc", 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "abcdefgh");
    checked_memcpy(one + 1, "xyz", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "axydefgh", 8);

    checked_strcpy(one, "abc");
    checked_memcpy(one, "xyz", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "hi there");
    checked_strcpy(two, "foo");
    checked_memcpy(two, one, 9);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "hi there", 8);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "hi there", 8);

    checked_memmove(one, "abc", 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "abcdefgh");
    checked_memmove(one + 1, "xyz", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "axydefgh", 8);

    checked_strcpy(one, "abc");
    checked_memmove(one, "xyz", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "hi there");
    checked_strcpy(two, "foo");
    checked_memmove(two, one, 9);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "hi there", 8);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "hi there", 8);

    checked_strcpy(one, "abcdefgh");
    checked_memmove(one + 1, one, 9);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "aabcdefgh", 9);

    checked_strcpy(one, "abcdefgh");
    checked_memmove(one + 1, one + 2, 7);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "acdefgh", 7);

    checked_strcpy(one, "abcdefgh");
    checked_memmove(one, one, 9);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abcdefgh", 8);

    cr_assert_null(memccpy_ptr(one, "abc", 'q', 4));
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "abcdefgh");
    memccpy_ptr(one + 1, "xyz", 'q', 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "axydefgh", 8);

    checked_strcpy(one, "abc");
    memccpy_ptr(one, "xyz", 'q', 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "hi there");
    checked_strcpy(two, "foo");
    memccpy_ptr(two, one, 'q', 9);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "hi there", 8);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "hi there", 8);

    checked_strcpy(one, "abcdefgh");
    checked_strcpy(two, "horsefeathers");
    cr_assert_eq(memccpy_ptr(two, one, 'f', 9), two + 6);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abcdefgh", 8);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "abcdefeathers", 13);

    checked_strcpy(one, "abcd");
    checked_strcpy(two, "bumblebee");
    cr_assert_eq(memccpy_ptr(two, one, 'a', 4), two + 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "aumblebee", 9);

    cr_assert_eq(memccpy_ptr(two, one, 'd', 4), two + 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "abcdlebee", 9);

    checked_strcpy(one, "xyz");
    cr_assert_eq(memccpy_ptr(two, one, 'x', 1), two + 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "xbcdlebee", 9);

    checked_strcpy(one, "abcdefgh");
    checked_memset(one + 1, 'x', 3);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "axxxefgh", 8);

    checked_memset(one + 2, 'y', 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "axxxefgh", 8);

    checked_memset(one + 5, 0, 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "axxxe", 5);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 6, "gh", 2);

    checked_memset(one + 2, 010045, 1);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ax\045xe", 5);

    bcopy_ptr("abc", one, 4);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "abcdefgh");
    bcopy_ptr("xyz", one + 1, 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "axydefgh", 8);

    checked_strcpy(one, "abc");
    bcopy_ptr("xyz", one, 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abc", 3);

    checked_strcpy(one, "hi there");
    checked_strcpy(two, "foo");
    bcopy_ptr(one, two, 9);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(two, "hi there", 8);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "hi there", 8);

    checked_strcpy(one, "abcdef");
    bzero_ptr(one + 2, 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "ab", 2);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 3, "", 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one + 4, "ef", 2);

    checked_strcpy(one, "abcdef");
    bzero_ptr(one + 2, 0);
    newlib_check_strcmp_strncmp_strlen_memcmp_bcmp_strchr_index_memchr(one, "abcdef", 6);

    cr_assert_eq(bcmp_ptr("a", "a", 1), 0);
    cr_assert_eq(bcmp_ptr("abc", "abc", 3), 0);
    cr_assert_neq(bcmp_ptr("abcd", "abce", 4), 0);
    cr_assert_neq(bcmp_ptr("abce", "abcd", 4), 0);
    cr_assert_neq(bcmp_ptr("alph", "beta", 4), 0);
    cr_assert_eq(bcmp_ptr("abce", "abcd", 3), 0);
    cr_assert_eq(bcmp_ptr("abc", "def", 0), 0);
}

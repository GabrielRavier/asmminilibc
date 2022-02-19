/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strcasecmp
*/

#include <strings.h>
#include <criterion/criterion.h>
#include "glibc.h"
#include <ctype.h>

static int (*volatile strcasecmp_ptr)(const char *, const char *) = strcasecmp;

static void check_result(const char *str1, const char *str2, int expected_result)
{
    int result = strcasecmp_ptr(str1, str2);
    // Test on 64-bit architectures where the ABI requires that the callee promote, whether this promotion has in fact been done
    asm("" : "=g" (result) : "0" (result));
    if (expected_result == 0)
        cr_assert_eq(result, 0);
    if (expected_result < 0)
        cr_assert(result < 0);
    if (expected_result > 0)
        cr_assert(result > 0);
}

Test(strcasecmp, escape)
{
    check_result("", "", 0);
    check_result("aBc", "abd", -1);
    check_result("abc", "aBB", 1);
    check_result("A", "Abc", -1);
    check_result("b", "A", 1);
    check_result("1234", "5678", -1);
    check_result("abc DEF", "ABC def", 0);
    check_result("123", "123", 0);
}

Test(strcasecmp, bionic)
{
    check_result("hello", "HELLO", 0);
    check_result("hello1", "hello2", -1);
    check_result("hello2", "hello1", 1);
}

static void glibc_do_test(size_t alignment1, size_t alignment2, size_t length, int max_char, int expected_result)
{
    if (length == 0)
        return;

    alignment1 &= 7;
    if (alignment1 + length + 1 >= glibc_page_size)
        return;

    alignment2 &= 7;
    if (alignment2 + length + 1 >= glibc_page_size)
        return;

    char *str1 = glibc_buf1 + alignment1;
    char *str2 = glibc_buf2 + alignment2;

    for (size_t i = 0; i < length; ++i) {
        str1[i] = toupper(1 + 23 * i % max_char);
        str2[i] = tolower(str1[i]);
    }

    str1[length] = 0;
    str2[length] = 0;
    str1[length + 1] = 23;
    str2[length + 1] = 24 + expected_result;

    if ((str2[length - 1] == 'z' && expected_result == -1) || (str2[length - 1] == 'a' && expected_result == 1))
        str1[length - 1] += expected_result;
    else
        str2[length - 1] -= expected_result;

    check_result(str1, str2, expected_result);
}

Test(strcasecmp, glibc)
{
    glibc_test_string_init();

    for (size_t i = 1; i < 16; ++i) {
        glibc_do_test(i, i, i, 127, 0);
        glibc_do_test(i, i, i, 127, 1);
        glibc_do_test(i, i, i, 127, -1);
    }

    for (size_t i = 1; i < 10; ++i) {
        glibc_do_test(0, 0, 2 << i, 127, 0);
        glibc_do_test(0, 0, 2 << i, 254, 0);
        glibc_do_test(0, 0, 2 << i, 127, 1);
        glibc_do_test(0, 0, 2 << i, 254, 1);
        glibc_do_test(0, 0, 2 << i, 127, -1);
        glibc_do_test(0, 0, 2 << i, 254, -1);
    }
}

Test(strcasecmp, glibc_random)
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
            ptr1[i] = tolower(random() & 255);
            if (i < length1 + alignment1 && ptr1[i] == '\0') {
                ptr1[i] = tolower(random() & 255);
                if (ptr1[i] == '\0')
                    ptr1[i] = tolower(1 + (random() & 127));
            }
        }
        for (size_t i = 0; i < j; ++i) {
            ptr2[i] = tolower(random() & 255);
            if (i < length2 + alignment2 && ptr2[i] == '\0') {
                ptr2[i] = tolower(random() & 255);
                if (ptr2[i] == '\0')
                    ptr2[i] = tolower(1 + (random() & 127));
            }
        }

        int expected_result = 0;
        memcpy(ptr2 + alignment2, ptr1 + alignment1, position);

        if (position < length1) {
            if (tolower(ptr2[alignment2 + position]) == ptr1[alignment1 + position]) {
                ptr2[alignment2 + position] = toupper(random() & 255);
                if (tolower(ptr2[alignment2 + position]) == ptr1[alignment1 + position])
                    ptr2[alignment2 + position] = toupper(ptr1[alignment1 + position] + 3 + (random() & 127));
            }

            if (ptr1[alignment1 + position] < tolower(ptr2[alignment2 + position]))
                expected_result = -1;
            else
                expected_result = 1;
        }
        ptr1[length1 + alignment1] = 0;
        ptr2[length2 + alignment2] = 0;

        check_result((char *)(ptr1 + alignment1), (char *)(ptr2 + alignment2), expected_result);
    }
}

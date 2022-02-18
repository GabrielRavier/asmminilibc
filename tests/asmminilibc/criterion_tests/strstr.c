/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strstr
*/

#define _GNU_SOURCE
#include <string.h>
#include <criterion/criterion.h>
#include "glibc.h"
#include "gnulib.h"
#include <unistd.h>
#include <signal.h>

static char *(*volatile strstr_ptr)(const char *, const char *) = strstr;

static char *known_strstr(const char *str1, const char *str2)
{
    ssize_t str1_length = strlen(str1);
    ssize_t str2_length = strlen(str2);

    if (str2_length > str1_length)
        return NULL;

    for (ssize_t i = 0; i <= str1_length - str2_length; ++i) {
        ssize_t j;
        for (j = 0; j < str2_length; ++j)
            if (str1[i + j] != str2[j])
                break;
        if (j == str2_length)
            return (char *)str1 + i;
    }
    return NULL;
}

static void check_result(const char *str1, const char *str2, const char *expected_result)
{
    cr_assert_eq(strstr_ptr(str1, str2), expected_result);
}

Test(strstr, escape)
{
    const char *str1 = "abc def ghi";
    cr_assert_eq(strstr_ptr(str1, "abc"), str1);
    cr_assert_eq(strstr_ptr(str1, "def"), str1 + 4);
    cr_assert_eq(strstr_ptr(str1, "ghi"), str1 + 8);
    cr_assert_eq(strstr_ptr(str1, "a"), str1);
    cr_assert_eq(strstr_ptr(str1, "abc def ghi"), str1);
    cr_assert_eq(strstr_ptr(str1, "abc ghi"), NULL);
    cr_assert_eq(strstr_ptr(str1, ""), str1);
    cr_assert_eq(strstr_ptr(str1, "def ghi"), str1 + 4);
    cr_assert_eq(strstr_ptr(str1, "g"), str1 + 8);

    const char *str2 = "";
    cr_assert_eq(strstr_ptr(str2, "abc"), NULL);
    cr_assert_eq(strstr_ptr(str2, ""), str2);

    const char *str3 = "aaac";
    cr_assert_eq(strstr_ptr(str3, "aac"), str3 + 1);
}

Test(strstr, bionic_empty_needle)
{
    const char *some_haystack = "haystack";
    const char *empty_haystack = "";

    cr_assert_eq(strstr_ptr(some_haystack, ""), some_haystack);
    cr_assert_eq(strstr_ptr(empty_haystack, ""), empty_haystack);
}

Test(strstr, bionic_smoke)
{
    const char *haystack = "big daddy/giant haystacks!";

    cr_assert_eq(strstr_ptr(haystack, ""), haystack);
    cr_assert_eq(strstr_ptr(haystack, "b"), haystack);
    cr_assert_eq(strstr_ptr(haystack, "bi"), haystack);
    cr_assert_eq(strstr_ptr(haystack, "big"), haystack);
    cr_assert_eq(strstr_ptr(haystack, "big "), haystack);
    cr_assert_eq(strstr_ptr(haystack, "big d"), haystack);

    cr_assert_eq(strstr_ptr(haystack, "g"), haystack + 2);
    cr_assert_eq(strstr_ptr(haystack, "gi"), haystack + 10);
    cr_assert_eq(strstr_ptr(haystack, "gia"), haystack + 10);
    cr_assert_eq(strstr_ptr(haystack, "gian"), haystack + 10);
    cr_assert_eq(strstr_ptr(haystack, "giant"), haystack + 10);

    cr_assert_eq(strstr_ptr(haystack, "!"), haystack + 25);
    cr_assert_eq(strstr_ptr(haystack, "s!"), haystack + 24);
    cr_assert_eq(strstr_ptr(haystack, "ks!"), haystack + 23);
    cr_assert_eq(strstr_ptr(haystack, "cks!"), haystack + 22);
    cr_assert_eq(strstr_ptr(haystack, "acks!"), haystack + 21);
}

static void glibc_do_test(size_t alignment1, size_t alignment2, size_t length1, size_t length2, bool no_match)
{
    char *str1 = glibc_buf1 + alignment1;
    char *str2 = glibc_buf2 + alignment2;

    static const char d[] = "1234567890abcdef";
#define d_len (sizeof(d) - 1)

    char *ss2 = str2;
    for (size_t l = length2; l > 0; l = l > d_len ? l - d_len : 0)
        ss2 = mempcpy(ss2, d, (l > d_len ? d_len : l));
    str2[length2] = '\0';

    if (no_match) {
        char *ss1 = str1;
        for (size_t l = length1; l > 0; l = l > d_len ? l - d_len : 0) {
            size_t t = l > d_len ? d_len : l;
            memcpy(ss1, d, t);
            ++ss1[length2 > 7 ? 7 : length2 - 1];
            ss1 += t;
        }
    } else {
        memset(str1, '0', length1);
        memcpy(str1 + length1 - length2, str2, length2);
    }
    str1[length1] = '\0';

    check_result(str1, str2, no_match ? NULL : str1 + length1 - length2);
#undef d_len
}

Test(strstr, glibc)
{
    glibc_test_string_init();

    for (size_t klen = 2; klen < 32; ++klen)
        for (size_t hlen = 2 * klen; hlen < 16 * klen; hlen += klen) {
            glibc_do_test(0, 0, hlen, klen, 0);
            glibc_do_test(0, 0, hlen, klen, 1);
            glibc_do_test(0, 3, hlen, klen, 0);
            glibc_do_test(0, 3, hlen, klen, 1);
            glibc_do_test(0, 9, hlen, klen, 0);
            glibc_do_test(0, 9, hlen, klen, 1);
            glibc_do_test(0, 15, hlen, klen, 0);
            glibc_do_test(0, 15, hlen, klen, 1);
            
            glibc_do_test(3, 0, hlen, klen, 0);
            glibc_do_test(3, 0, hlen, klen, 1);
            glibc_do_test(3, 3, hlen, klen, 0);
            glibc_do_test(3, 3, hlen, klen, 1);
            glibc_do_test(3, 9, hlen, klen, 0);
            glibc_do_test(3, 9, hlen, klen, 1);
            glibc_do_test(3, 15, hlen, klen, 0);
            glibc_do_test(3, 15, hlen, klen, 1);
            
            glibc_do_test(9, 0, hlen, klen, 0);
            glibc_do_test(9, 0, hlen, klen, 1);
            glibc_do_test(9, 3, hlen, klen, 0);
            glibc_do_test(9, 3, hlen, klen, 1);
            glibc_do_test(9, 9, hlen, klen, 0);
            glibc_do_test(9, 9, hlen, klen, 1);
            glibc_do_test(9, 15, hlen, klen, 0);
            glibc_do_test(9, 15, hlen, klen, 1);
            
            glibc_do_test(15, 0, hlen, klen, 0);
            glibc_do_test(15, 0, hlen, klen, 1);
            glibc_do_test(15, 3, hlen, klen, 0);
            glibc_do_test(15, 3, hlen, klen, 1);
            glibc_do_test(15, 9, hlen, klen, 0);
            glibc_do_test(15, 9, hlen, klen, 1);
            glibc_do_test(15, 15, hlen, klen, 0);
            glibc_do_test(15, 15, hlen, klen, 1);

            glibc_do_test(15, 51, hlen + klen * 4, klen * 4, 0);
            glibc_do_test(15, 51, hlen + klen * 4, klen * 4, 1);
        }

    glibc_do_test(0, 0, glibc_page_size - 1, 16, 0);
    glibc_do_test(0, 0, glibc_page_size - 1, 16, 1);
}

Test(strstr, glibc_check1)
{
    const char str1[] = "F_BD_CE_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_C3_88_20_EF_BF_BD_EF_BF_BD_EF_BF_BD_C3_A7_20_EF_BF_BD";
    const char str2[] = "_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD";

    check_result(str1, str2, known_strstr(str1, str2));
}

Test(strstr, glibc_check2)
{
    glibc_test_string_init();

    const char str1_stack[] = ", enable_static, \0, enable_shared, ";
    const size_t str1_byte_count = 18;
    const char *str2_stack = &(str1_stack[str1_byte_count]);
    const size_t str2_byte_count = 18;
    const size_t real_page_size = getpagesize();

    // Haystack at end of page (with the page afterwards faulting)
    char *str1_page_end = glibc_buf1 + glibc_page_size - str1_byte_count;
    strcpy(str1_page_end, str1_stack);

    char *str2_page_end = glibc_buf2 + glibc_page_size - str2_byte_count;
    strcpy(str2_page_end, str2_stack);

    // Haystack that crosses a page boundary 
    char *str1_page_cross = glibc_buf1 + real_page_size - 8;
    strcpy(str1_page_cross, str1_stack);

    // Haystack that crosses a page boundary 
    char *str2_page_cross = glibc_buf2 + real_page_size - 8;
    strcpy(str2_page_cross, str2_stack);

    char *expected_result = known_strstr(str1_stack, str2_stack);

    check_result(str1_stack, str2_stack, expected_result);
    check_result(str1_stack, str2_page_end, expected_result);
    check_result(str1_stack, str2_page_cross, expected_result);
    
    check_result(str1_page_end, str2_stack, expected_result);
    check_result(str1_page_end, str2_page_end, expected_result);
    check_result(str1_page_end, str2_page_cross, expected_result);

    check_result(str1_page_cross, str2_stack, expected_result);
    check_result(str1_page_cross, str2_page_end, expected_result);
    check_result(str1_page_cross, str2_page_cross, expected_result);
}

Test(strstr, glibc_pr23637)
{
    glibc_test_string_init();

    static const size_t N = 1024;

    char *haystack = glibc_buf1;
    char *needle = glibc_buf2;

    for (size_t i = 0; i < N; ++i) {
        needle[i] = 'x';
        haystack[i] = ' ';
        haystack[i + N] = 'x';
    }

    needle[N] = '\0';
    haystack[N * 2] = '\0';

    // Make sure we don't match the first x
    haystack[0] = 'x';

    check_result(haystack, needle, known_strstr(haystack, needle));
}

Test(strstr, gnulib)
{
    {
        const char input[] = "foo";
        check_result(input, "", input);
        check_result(input, "o", input + 1);
    }

    {
        const char *fix = "aBaaaaaaaaaaax";
        char *page_boundary = gnulib_zerosize_ptr();
        size_t length = strlen(fix) + 1;
        char *input = page_boundary - length;

        strcpy(input, fix);
        check_result(input, "B1x", NULL);
    }

    {
        const char input[] = "ABC ABCDAB ABCDABCDABDE";
        check_result(input, "ABCDABD", input + 15);
        check_result(input, "ABCDABE", NULL);
        check_result(input, "ABCDABCD", input + 11);
    }

    // Check that a long periodic needle doesn't result in false matches
    {
        const char input[] = "F_BD_CE_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_C3_88_20_EF_BF_BD_EF_BF_BD_EF_BF_BD_C3_A7_20_EF_BF_BD";
        const char needle[] = "_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD";
        check_result(input, needle, NULL);
    }
    {
        const char input[] = "F_BD_CE_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_C3_88_20_EF_BF_BD_EF_BF_BD_EF_BF_BD_C3_A7_20_EF_BF_BD_DA_B5_C2_A6_20_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD";
        const char needle[] = "_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD_EF_BF_BD";
        check_result(input, needle, input + 115);
    }

    // Check that a very long haystack is handled quickly if the needle is short and occurs near the beginning
    {
        size_t m = 1000000;
        const char *needle = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
        char *haystack = malloc(m + 1);
        cr_assert_not_null(haystack);

        memset(haystack, 'A', m);
        haystack[0] = 'B';
        haystack[m] = '\0';

        for (size_t repeat = 10000; repeat > 0; --repeat)
            check_result(haystack, needle, haystack + 1);
        free(haystack);
    }

    // Check that a very long needle is discarded quickly if the haystack is short
    {
        size_t m = 1000000;
        const char *haystack = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
        char *needle = malloc(m + 1);
        cr_assert_not_null(haystack);

        memset(needle, 'A', m);
        needle[m] = '\0';

        for (size_t repeat = 10000; repeat > 0; --repeat)
            check_result(haystack, needle, NULL);
        free(needle);
    }

    // Check for non-quadratic worst-case complexity
    {
        size_t m = 1000000;
        char *haystack = (char *)malloc(2 * m + 2);
        char *needle = (char *)malloc(m + 2);
        cr_assert_not_null(haystack);
        cr_assert_not_null(needle);

        memset(haystack, 'A', 2 * m);
        haystack[2 * m] = 'B';
        haystack[2 * m + 1] = '\0';

        memset(needle, 'A', m);
        needle[m] = 'B';
        needle[m + 1] = '\0';

        check_result(haystack, needle, haystack + m);
        free(needle);
        free(haystack);
    }

    // Check that a barely period short needle does not result in mistakenly skipping just past the match point
    {
        const char *haystack =
            "\n"
            "with_build_libsubdir\n"
            "with_local_prefix\n"
            "with_gxx_include_dir\n"
            "with_cpp_install_dir\n"
            "enable_generated_files_in_srcdir\n"
            "with_gnu_ld\n"
            "with_ld\n"
            "with_demangler_in_ld\n"
            "with_gnu_as\n"
            "with_as\n"
            "enable_largefile\n"
            "enable_werror_always\n"
            "enable_checking\n"
            "enable_coverage\n"
            "enable_gather_detailed_mem_stats\n"
            "enable_build_with_cxx\n"
            "with_stabs\n"
            "enable_multilib\n"
            "enable___cxa_atexit\n"
            "enable_decimal_float\n"
            "enable_fixed_point\n"
            "enable_threads\n"
            "enable_tls\n"
            "enable_objc_gc\n"
            "with_dwarf2\n"
            "enable_shared\n"
            "with_build_sysroot\n"
            "with_sysroot\n"
            "with_specs\n"
            "with_pkgversion\n"
            "with_bugurl\n"
            "enable_languages\n"
            "with_multilib_list\n";
        const char *needle = "\n"
            "with_gnu_ld\n";
        check_result(haystack, needle, haystack + 114);
    }
    {
        const char *haystack = "..wi.d.";
        const char *needle = ".d.";
        check_result(haystack, needle, haystack + 4);
    }
    {
        const char *needle = "\nwith_gnu_ld-extend-to-len-32-b\n";
        size_t needle_length = strlen(needle);
        const char *h =
            "\n"
            "with_build_libsubdir\n"
            "with_local_prefix\n"
            "with_gxx_include_dir\n"
            "with_cpp_install_dir\n"
            "with_e_\n"
            "..............................\n"
            "with_FGHIJKLMNOPQRSTUVWXYZ\n"
            "with_567890123456789\n"
            "with_multilib_list\n";
        size_t h_length = strlen(h);
        char *haystack = malloc(h_length + 1);
        cr_assert_not_null(haystack);

        for (size_t i = 0; i < h_length - needle_length; ++i) {
            memcpy(haystack, h, h_length + 1);
            memcpy(haystack + i, needle, strlen(needle) + 1);
            check_result(haystack, needle, haystack + i);
        }
        free(haystack);
    }

    // Test long needles
    {
        size_t m = 1024;
        char *haystack = malloc(2 * m + 1);
        char *needle = malloc(m + 1);
        cr_assert_not_null(haystack);
        cr_assert_not_null(needle);

        haystack[0] = 'x';
        memset(haystack + 1, ' ', m - 1);
        memset(haystack + m, 'x', m);
        haystack[2 * m] = '\0';

        memset(needle, 'x', m);
        needle[m] = '\0';
        check_result(haystack, needle, haystack + m);
        
        free(needle);
        free(haystack);
    }
}

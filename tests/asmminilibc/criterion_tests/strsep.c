/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests strsep
*/

#include <string.h>
#include <criterion/criterion.h>

Test(strsep, mandoc)
{
    char buffer[6] = "aybxc";
    char *work_ptr = buffer;
    char *ret_ptr = strsep(&work_ptr, "xy");
    cr_assert_eq(ret_ptr, buffer);
    cr_assert_str_eq(ret_ptr, "a");
    cr_assert_eq(work_ptr, buffer + 2);
}

Test(strsep, cloudlibc)
{
    char line[] = "LINE\nTO BE\tSEPARATED\n";
    const char *split = " \t\n";
    char *lasts = line;
    cr_assert_str_eq(strsep(&lasts, split), "LINE");
    cr_assert_str_eq(lasts, "TO BE\tSEPARATED\n");
    cr_assert_str_eq(strsep(&lasts, split), "TO");
    cr_assert_str_eq(lasts, "BE\tSEPARATED\n");
    cr_assert_str_eq(strsep(&lasts, split), "BE");
    cr_assert_str_eq(lasts, "SEPARATED\n");
    cr_assert_str_eq(strsep(&lasts, split), "SEPARATED");
    cr_assert_str_eq(lasts, "");
    cr_assert_str_eq(strsep(&lasts, split), "");
    cr_assert_eq(strsep(&lasts, split), NULL);
}

static void avr_libc_check(const char *str1_param, const char *str2_param, ssize_t expected_token_length, ssize_t expected_work_ptr_offset)
{
    char str1[300];
    char str2[300];

    cr_assert(strlen(str1_param) < sizeof(str1));
    cr_assert(strlen(str2_param) < sizeof(str2));

    strcpy(str1, str1_param);
    strcpy(str2, str2_param);

    char *work_ptr = str1;
    char *result = strsep(&work_ptr, str2);
    cr_assert_eq(result, str1);

    if (expected_token_length < 0)
        cr_assert_str_eq(str1, str1_param);
    else {
        cr_assert_eq(strlen(str1), expected_token_length);
        cr_assert_eq(memcmp(str1, str1_param, expected_token_length), 0);
        cr_assert_eq(str1[expected_token_length], '\0');
        cr_assert_eq(strcmp(str1 + expected_token_length + 1, str1_param + expected_token_length + 1), 0);
    }
    if (expected_work_ptr_offset < 0)
        cr_assert_null(work_ptr);
    else
        cr_assert_eq(work_ptr, str1 + expected_work_ptr_offset);
}

Test(strsep, avr_libc)
{
    char *p = NULL;
    cr_assert_eq(strsep(&p, ""), NULL);
    cr_assert_eq(p, NULL);
    cr_assert_eq(strsep(&p, "abc"), NULL);
    cr_assert_eq(p, NULL);

    // Empty string
    avr_libc_check("", "", -1, -1);
    avr_libc_check("", "abc", -1, -1);

    // Empty delimiter list
    avr_libc_check("a", "", -1, -1);
    avr_libc_check("12345678", "", -1, -1);

    // No delimiters found
    avr_libc_check("\tabc", " ", -1, -1);
    avr_libc_check("THE QUICK BROWN FOX", "thequickbrownfox", -1, -1);

    // Delimiter list is 1 characters
    avr_libc_check(".", ".", 0, 1);
    avr_libc_check("abc", "a", 0, 1);
    avr_libc_check("abc", "b", 1, 2);
    avr_libc_check("abc", "c", 2, 3);

    // Delimiter list is 2 characters
    avr_libc_check("0", "01", 0, 1);
    avr_libc_check("1", "01", 0, 1);
    avr_libc_check("A.", "AB", 0, 1);
    avr_libc_check("B.", "AB", 0, 1);
    avr_libc_check("CAD", "AB", 1, 2);
    avr_libc_check("CDB", "AB", 2, 3);

    // Long delimiter list
    avr_libc_check("the quick", "0123456789 ", 3, 4);

    // Very long string
    avr_libc_check("...............................................................................................................................................................................................................................................................*", "*", 255, 256);
    avr_libc_check("................................................................................................................................................................................................................................................................*", "*", 256, 257);

    // Non ASCII
    avr_libc_check("\001\002\377", "\001", 0, 1);
    avr_libc_check("\001\002\377", "\377", 2, 3);
}

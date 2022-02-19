/*
** EPITECH PROJECT, 2022
** asmminilibc
** File description:
** Tests ffs
*/

#include <strings.h>
#include <criterion/criterion.h>
#include <stdint.h>
#include <limits.h>

static int (*volatile ffs_ptr)(int i) = ffs;

static void check_result(int i, int expected_result)
{
    cr_assert_eq(ffs_ptr(i), expected_result);
}

static int known_ffs(int i)
{
    for (unsigned j = 0; j < (sizeof(int) * CHAR_BIT); ++j)
        if (i & (1u << j))
            return j + 1;
    return 0;
}

Test(ffs, openbsd)
{
    check_result(0, 0);
    check_result(0x8080, 8);
    check_result(INT32_MIN, 32);
}

Test(ffs, bionic)
{
    check_result(1, 1);
    check_result(0x20, 6);
    check_result(0x400, 11);
    check_result(0x8000, 16);
    check_result(0x10000, 17);
    check_result(0x200000, 22);
    check_result(0x4000000, 27);
    check_result(0x80000000, 32);
}

Test(ffs, dietlibc)
{
    check_result(1, 1);
    check_result(2, 2);
    check_result(4, 3);
    check_result(256, 9);
    check_result(511, 1);
}

Test(ffs, cloudlibc)
{
    check_result(0, 0);
    check_result(0x3211, 1);
    check_result(0xABC2, 2);
    check_result(0x79224, 3);

    check_result(INT_MIN >> 3, (CHAR_BIT * sizeof(int)) - 3);
    check_result(INT_MIN, CHAR_BIT * sizeof(int));
}

Test(ffs, glibc)
{
    check_result(0, 0);
    for (size_t i = 0; i < 8 * sizeof(int); ++i)
        check_result(1ll << i, i + 1);
    for (size_t i = 0; i < 8 * sizeof(int); ++i)
        check_result((~0u >> i) << i, i + 1);
    check_result(0x80008000, 16);
}

Test(ffs, avr_libc_macro)
{
    check_result(0, 0);
    check_result(1, 1);
    check_result(2, 2);
    check_result(4, 3);
    check_result(8, 4);
    check_result(16, 5);
    check_result(32, 6);
    check_result(64, 7);
    check_result(128, 8);
    check_result(256, 9);
    check_result(512, 10);
    check_result(1024, 11);
    check_result(2048, 12);
    check_result(4096, 13);
    check_result(8192, 14);
    check_result(16384, 15);
    check_result(32768, 16);
    check_result(ffs_ptr(ffs_ptr(2)), 2);
    check_result(0x8001, 1);
    check_result(0xFFFF, 1);
    check_result(6, 2);
    check_result(0x180, 8);
    check_result(0xC000, 15);
}

Test(ffs, avr_libc)
{
    static struct {
        int value;
        unsigned char expected_result;
    } table[] = {
        { 0, 0 },
        { 1, 1 },
        { 0xffff, 1 },
        { 0x0002, 2 },
        { 0x8002, 2 },
        { 0xfff2, 2 },
        { 0x0040, 7 },
        { 0x0140, 7 },
        { 0x0080, 8 },
        { 0xff80, 8 },
        { 0x0100, 9 },
        { 0xff00, 9 },
        { 0x8000, 16 },
    };

    for (size_t i = 0; i != sizeof(table) / sizeof(table[0]); ++i)
        check_result(table[i].value, table[i].expected_result);
}

Test(ffs, mlibc)
{
    check_result(0x8000, 16);
    check_result(0, 0);
    check_result((int)0x8000000000000000, 0);
}

Test(ffs, picolibc)
{
    static const size_t N = 100000;

    for (size_t i = 0; i < N; ++i) {
        int x = random();
        check_result(x, known_ffs(x));
    }
}

Test(ffs, gnulib)
{
    for (int x = -128; x <= 128; ++x)
        check_result(x, known_ffs(x));
}

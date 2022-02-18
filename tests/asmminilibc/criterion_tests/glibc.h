/*
** EPITECH PROJECT, 2021
** asmminilibc
** File description:
** Declares glibc-specific test stuff
*/

#pragma once

#include <criterion/criterion.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>

static char *glibc_buf1, *glibc_buf2;
static size_t glibc_page_size;

#ifndef GLIBC_BUF1PAGES
    #define GLIBC_BUF1PAGES 1
#endif

static inline void glibc_test_string_init(void)
{
    glibc_page_size = 2 * getpagesize();
#ifdef GLIBC_MIN_PAGE_SIZE
    glibc_page_size = glibc_page_size > GLIBC_MIN_PAGE_SIZE ? glibc_page_size : GLIBC_MIN_PAGE_SIZE;
#endif
    glibc_buf1 = (char *)mmap(0, (GLIBC_BUF1PAGES + 1) * glibc_page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    cr_assert_neq(glibc_buf1, MAP_FAILED);
    cr_assert_eq(mprotect(glibc_buf1 + GLIBC_BUF1PAGES * glibc_page_size, glibc_page_size, PROT_NONE), 0);
    glibc_buf2 = (char *)mmap(0, 2 * glibc_page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    cr_assert_neq(glibc_buf2, MAP_FAILED);
    cr_assert_eq(mprotect(glibc_buf2 + glibc_page_size, glibc_page_size, PROT_NONE), 0);

    memset(glibc_buf1, 0xA5, GLIBC_BUF1PAGES * glibc_page_size);
    memset(glibc_buf2, 0x5A, glibc_page_size);
}

/*
** EPITECH PROJECT, 2021
** asmminilibc
** File description:
** Declares gnulib-specific test stuff
*/

#pragma once

#include <criterion/criterion.h>
#include <unistd.h>
#include <sys/mman.h>

static inline void *gnulib_zerosize_ptr(void)
{
    int page_size = getpagesize();
    
    char *two_pages = mmap(NULL, 2 * page_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    cr_assert_neq(two_pages, MAP_FAILED);
    cr_assert_eq(mprotect(two_pages + page_size, page_size, PROT_NONE), 0);

    return two_pages + page_size;
}

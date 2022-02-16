/*
** EPITECH PROJECT, 2021
** asmminilibc
** File description:
** Defines littlekernel_fill_buffer
*/

#pragma once

#include <stdint.h>
#include <stddef.h>

static void littlekernel_fill_buffer(void *ptr, size_t len, uint32_t seed)
{
    for (size_t i = 0; i < len; ++i) {
        ((unsigned char *)ptr)[i] = seed;
        seed *= 0x1234567;
    }
}

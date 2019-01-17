/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <string.h>
#include <stdio.h>

#include <stdint.h>
#include "tinymt32.h"

void random_init(uint32_t seed);
uint32_t random_uint32(void);

static tinymt32_t _random = { .mat1=0x8f7011ee, .mat2=0xfc78ff1f,
.tmat=0x3793fdff };


void random_init(uint32_t seed)
{
    tinymt32_init(&_random, seed);
}

uint32_t random_uint32(void)
{
    return tinymt32_generate_uint32(&_random);
}

uint32_t tinymt32_rand256(void)
{
    return (random_uint32() & 0xFF);
}

uint32_t tinymt32_rand16(void)
{
    return (random_uint32() & 0xF);
}

void random_init_by_array(uint32_t init_key[], int key_length)
{
    tinymt32_init_by_array(&_random, init_key, key_length);
}


int main(void)
{
    // uint8_t digest[64];
    // size_t dlen = sizeof(digest);
    
    uint32_t seed = 1;
    int range = 50;
    int i = range;
    random_init(seed);
    puts("32 bits values:");
    while (i>0)
    {
        printf("%lu\n", (unsigned long int)random_uint32());
        i--;
    }
    
    puts("8 bits values:");
    i = range;
    random_init(seed);
    while (i>0)
    {
        printf("%u\n", (unsigned int)tinymt32_rand256());
        i--;
    }
    
    puts("4 bits values:");
    i = range;
    random_init(seed);
    while (i>0)
    {
        printf("%u\n", (unsigned int)tinymt32_rand16());
        i--;
    }   
}



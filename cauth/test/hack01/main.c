/*
 * Author: Hans Liljestrand <hans.liljestrand@aalto.fi>
 * Copyright: Secure Systems Group, Aalto University, https://ssg.aalto.fi
 *
 * This code is released under Apache 2.0 license
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nops "nop\n\tnop\n\t"

__attribute__((noinline))
void do_copy(char * dst, const char *src) {
    size_t len = strlen(src);
    strncpy(dst, src, len);
}

__attribute__((noinline))
void overflower(void) {
    char first[] = "Hello";
    char data[5] = { 0 };
    char second[] = "World";
    char *toolong = "1234567890";

    printf("%s %s!\n", first, second);
    printf("%s %s!\n", data, toolong);

    do_copy(data, toolong);

    printf("%s %s!\n", first, second);
    printf("%s %s!\n", data, toolong);
}

__attribute__((noinline))
void fun(void *f) {
    char *buffer = "";
    void **p = (void **) &buffer;

    for (register size_t i = 0; i < 16; i++)
        p[i] = f;

    __asm__ volatile ( nops :: "r" (p) : "memory");
}

__attribute__((noinline))
void hack(void) {
    printf("You have been hacked!\n");
    exit(0);
}

int main(void) {
    overflower();
    fun(hack);
    return 0;
}

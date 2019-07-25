/*
 * Author: Hans Liljestrand <hans.liljestrand@aalto.fi>
 * Copyright: Secure Systems Group, Aalto University, https://ssg.aalto.fi
 *
 * This code is released under Apache 2.0 license
 */

#include <stdio.h>
#include <string.h>

char g_arr32[32] = { 0 };

__attribute__((noinline))
void do_copy(char * dst, const char *src) {
    size_t len = strlen(src);
    strncpy(dst, src, len);
}

__attribute__((noinline))
void printer(char *str) {
    printf("%s\n", str);
}

__attribute__((noinline))
void simple(void) {
    char a[32] = { 0 };
    do_copy(a, g_arr32);
    printer(a);
}

__attribute__((noinline))
void tester(void) {
    char a1[32] = { 0 };
    char a2[64] = { 0 };
    char a3[128] = { 0 };
    char a4[256] = { 0 };

    char mul[4][32] = {
        { 0 },
        { 0 },
        { 0 },
        { 0 }
    };

    do_copy(a1, "Hello World");
    do_copy(a2, "Hello World");
    do_copy(a3, "Hello World");
    do_copy(a4, "Hello World");

    for (int i = 0; i < 4; ++i) {
        do_copy(mul[i], "Hello World");
    }

    printf("%s\n", a1);
    printf("%s\n", a2);
    printf("%s\n", a3);
    printf("%s\n", a4);

    for (int i = 0; i < 4; ++i) {
        printf("%s\n", mul[i]);
    }
}

int main(void) {
    tester();
    return 0;
}

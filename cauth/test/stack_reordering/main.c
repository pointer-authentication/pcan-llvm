/*
 * Author: Hans Liljestrand <hans.liljestrand@aalto.fi>
 * Copyright: Secure Systems Group, Aalto University, https://ssg.aalto.fi
 *
 * This code is released under Apache 2.0 license
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

long fp = 0;
long sp = 0;

__attribute__((noinline))
void do_copy(char * dst, const char *src) {
    size_t len = strlen(src);
    strncpy(dst, src, len);
}

__attribute__((noinline))
void printer(char *str) {
    printf("%lx -> %s\n", (long)str-sp, str);
}

__attribute__((noinline))
void printer64(uint64_t *str) {
    printf("%lx -> %lu\n", (long)str-sp, *str);
}

__attribute__((noinline))
void printer32(uint32_t *str) {
    printf("%lx -> %u\n", (long)str-sp, *str);
}

__attribute__((noinline))
void tester(void) {
    uint64_t dummy1 = 11;
    char a1[64] = { 0 };
    uint64_t dummy2 = 11;
    uint32_t tricky = 34;
    char a2[28] = { 0 };
    uint64_t dummy3 = 11;
    char a3[64] = { 0 };
    uint64_t dummy4 = 13;
    char a4[64] = { 0 };

    do_copy(a1, "Hello World");
    do_copy(a2, "Hello World");
    do_copy(a3, "Hello World");
    do_copy(a4, "Hello World");

    asm(
            "mov %0, sp;"
            "mov %1, x29;"
            : "=r" (sp), "=r" (fp) : :);

    printer64(&dummy1);
    printer(a1);
    printer64(&dummy2);
    printer32(&tricky);
    printer(a2);
    printer64(&dummy3);
    printer(a3);
    printer64(&dummy4);
    printer(a4);

}

__attribute__((noinline))
void tester2(void) {
    uint32_t tricky = 34;
    char a[52] = { 0 };

    do_copy(a, "Hello World");

    asm(
            "mov %0, sp;"
            "mov %1, x29;"
            : "=r" (sp), "=r" (fp) : :);

    printer32(&tricky);
    printer(a);
}

__attribute__((noinline))
void tester3(void) {
    uint32_t tricky1 = 123;
    char a[54] = { 0 };
    uint32_t tricky2 = 42;
    uint32_t tricky3 = 34;

    do_copy(a, "Hello World");

    asm(
            "mov %0, sp;"
            "mov %1, x29;"
            : "=r" (sp), "=r" (fp) : :);

    printer32(&tricky1);
    printer(a);
    printer32(&tricky2);
    printer32(&tricky3);
}

int main(void) {
    tester();
    printf("0x%lx -> SP\n", sp);
    printf("0x%lx -> FP\n", fp-sp);
    tester2();
    printf("0x%lx -> SP\n", sp);
    printf("0x%lx -> FP\n", fp-sp);
    tester3();
    printf("0x%lx -> SP\n", sp);
    printf("0x%lx -> FP\n", fp-sp);
    return 0;
}

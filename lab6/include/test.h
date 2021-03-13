#ifndef TEST_H
#define TEST_H

#define SIFIVE_TEST 0x100000
#define VIRT_TEST_FINISHER_PASS 0x5555

int os_test();

/* 死循环 */
void dead_loop(void) __attribute__((noreturn));

void shutdown() __attribute__((noreturn));

#endif

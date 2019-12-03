/*!
 * lib.h
 * @ Defines for useful library functions
 */

#ifndef LIB_H
#define LIB_H

#include "types.h"

int32_t printf(int8_t* format, ...);
int32_t puts(int8_t* s);
void putc(uint8_t c);
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
int8_t* strrev(int8_t* s);

void* memset(void* s, int32_t c, uint32_t n);
void* memset_word(void* s, int32_t c, uint32_t n);
void* memset_dword(void* s, int32_t c, uint32_t n);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);

int32_t strlen(const int8_t* s);
int32_t strnlen(const int8_t* s, uint32_t n);
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n);
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n);

/* Reads a byte and returns its value as a zero-extended 32-bit unsigned int */
static inline uint32_t inb(uint16_t port) {
    uint32_t val;
    asm volatile("                  \n\
                 xorl   %0, %0      \n\
                 inb    (%w1), %b0  \n\
                 "
                 : "=a" (val)
                 : "d" (port)
                 : "memory"
                );

    return val;
}

/* Reads two bytes from two consecutive ports, starting at "port", concatenates
 * them little-endian style, and returns them zero-extended */
static inline uint32_t inw(uint16_t port) {
    uint32_t val;
    asm volatile("                  \n\
                 xorl   %0, %0      \n\
                 inw    (%w1), %w0  \n\
                 "
                 : "=a" (val)
                 : "d" (port)
                 : "memory"
                );

    return val;
}

/* Reads four bytes from four consecutive ports, starting at "port",
 * concatenates them little-endian style, and returns them */
static inline uint32_t inl(uint16_t port) {
    uint32_t val;
    asm volatile("                  \n\
                 inl    (%w1), %0   \n\
                 "
                 : "=a" (val)
                 : "d" (port)
                 : "memory"
                );

    return val;
}

/* Writes a byte to a port */
#define outb(data, port)                    \
do {                                        \
    asm volatile("outb  %b1, (%w0)"         \
                 :                          \
                 : "d" (port), "a" (data)   \
                 : "memory", "cc"           \
                );                          \
} while (0)

/* Writes two bytes to two consecutive ports */
#define outw(data, port)                    \
do {                                        \
    asm volatile("outw  %w1, (%w0)"         \
                 :                          \
                 : "d" (port), "a" (data)   \
                 : "memory", "cc"           \
                );                          \
} while (0)

/* Writes four bytes to four consecutive ports */
#define outl(data, port)                    \
do {                                        \
    asm volatile("outl  %l1, (%w0)"         \
                 :                          \
                 : "d" (port), "a" (data)   \
                 : "memory", "cc"           \
                );                          \
} while (0)

/* Clear interrupt flag - disables interrupts on this processor */
#define cli()                               \
do {                                        \
    asm volatile("cli"                      \
                 :                          \
                 :                          \
                 : "memory", "cc"           \
                );                          \
} while (0)

/* Set interrupt flag - enable interrupts on this processor */
#define sti()                               \
do {                                        \
    asm volatile("sti"                      \
                 :                          \
                 :                          \
                 : "memory", "cc"           \
                );                          \
} while (0)

#endif /* LIB_H */

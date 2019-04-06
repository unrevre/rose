/*!
 * lib.c
 * @ Some basic printing and library functions
 */

#include "lib.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7

#define CURSOR_HIGH         0xE
#define CURSOR_LOW          0xF

#define VGA_INDEX_REGISTER  0x3D4
#define VGA_DATA_REGISTER   0x3D5

static int screen_x;
static int screen_y;
static char* video_mem = (char*)VIDEO;

void backspace(void) {
    screen_x = (screen_x + NUM_COLS - 1) % NUM_COLS;
    screen_y = screen_y - (screen_x / (NUM_COLS - 1));

    int32_t offset = screen_x + NUM_COLS * screen_y;
    *(uint8_t*)(video_mem + (offset << 1)) = ' ';
    *(uint8_t*)(video_mem + (offset << 1) + 1) = ATTRIB;

    blink();
}

void newline(void) {
    screen_x = 0;
    ++screen_y;

    if (screen_y == NUM_ROWS) {
        --screen_y;
        scroll();
    }

    blink();
}

void scroll(void) {
    int32_t offset = NUM_COLS * (NUM_ROWS - 1);
    memmove((uint8_t*)video_mem,
            (uint8_t*)(video_mem + (NUM_COLS << 1)),
            offset << 1);

    int32_t i;
    for (i = 0; i < NUM_COLS; ++i) {
        *(uint8_t*)(video_mem + ((offset + i) << 1)) = ' ';
        *(uint8_t*)(video_mem + ((offset + i) << 1) + 1) = ATTRIB;
    }
}

void clear(void) {
    screen_x = 0;
    screen_y = 0;

    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t*)(video_mem + (i << 1)) = ' ';
        *(uint8_t*)(video_mem + (i << 1) + 1) = ATTRIB;
    }

    blink();
}

void blink(void) {
    uint16_t position = screen_x + NUM_COLS * screen_y;

    outb(CURSOR_LOW, VGA_INDEX_REGISTER);
    outb((uint8_t)(position & 0xFF), VGA_DATA_REGISTER);
    outb(CURSOR_HIGH, VGA_INDEX_REGISTER);
    outb((uint8_t)((position >> 8) & 0xFF), VGA_DATA_REGISTER);
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e. print 8 hexadecimal
 *       digits, zero-padded on the left. For example, the hex number "E" would
 *       be printed as "0000000E". Note: This is slightly different than the
 *       libc specification for the "#" modifier (this implementation doesn't
 *       add a "0x" at the beginning). Also note: %x is the only conversion
 *       specifier that can use the "#" modifier to alter output. */
int32_t printf(int8_t* format, ...) {
    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void*)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%': ;
                int32_t alternate = 0;
                buf++;

format_char_switch:
                /* Conversion specifiers */
                switch (*buf) {
                    /* Print a literal '%' character */
                    case '%':
                        putc('%');
                        break;
                    /* Use alternate formatting */
                    case '#':
                        alternate = 1;
                        buf++;
                        goto format_char_switch;
                    /* Print a number in hexadecimal form */
                    case 'x': ;
                        int8_t hex_buf[64];
                        if (alternate == 0) {
                            itoa(*((uint32_t*)esp), hex_buf, 16);
                            puts(hex_buf);
                        } else {
                            int32_t starting_index;
                            int32_t i;
                            itoa(*((uint32_t*)esp), &hex_buf[8], 16);
                            i = starting_index = strlen(&hex_buf[8]);
                            while (i < 8) {
                                hex_buf[i] = '0';
                                i++;
                            }
                            puts(&hex_buf[starting_index]);
                        }
                        esp++;
                        break;
                    /* Print a number in unsigned int form */
                    case 'u': ;
                        int8_t udec_buf[36];
                        itoa(*((uint32_t*)esp), udec_buf, 10);
                        puts(udec_buf);
                        esp++;
                        break;
                    /* Print a number in signed int form */
                    case 'd': ;
                        int8_t dec_buf[36];
                        int32_t value = *((int32_t*)esp);
                        if (value < 0) {
                            dec_buf[0] = '-';
                            itoa(-value, &dec_buf[1], 10);
                        } else {
                            itoa(value, dec_buf, 10);
                        }
                        puts(dec_buf);
                        esp++;
                        break;
                    /* Print a single character */
                    case 'c':
                        putc((uint8_t)*((int32_t*)esp));
                        esp++;
                        break;
                    /* Print a NULL-terminated string */
                    case 's':
                        puts(*((int8_t**)esp));
                        esp++;
                        break;
                    default:
                        break;
                }
                break;
            default:
                putc(*buf);
                break;
        }

        buf++;
    }

    return (buf - format);
}

/* Output a string to the console */
int32_t puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc(s[index]);
        index++;
    }

    return index;
}

void putc(uint8_t c) {
    if (c == '\n' || c == '\r') {
        newline();
    } else {
        int32_t offset = screen_x + NUM_COLS * screen_y;
        *(uint8_t*)(video_mem + (offset << 1)) = c;
        *(uint8_t*)(video_mem + (offset << 1) + 1) = ATTRIB;

        screen_x = (screen_x + 1) % NUM_COLS;
        screen_y = screen_y + !screen_x;

        if (screen_y == NUM_ROWS) {
            --screen_y;
            scroll();
        }
    }

    blink();
}

/* Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int8_t* newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    /* Go through the number one place value at a time, and add the correct
     * digit to "newbuf". We actually add characters to the ASCII string from
     * lowest place value to highest, which is the opposite of how the number
     * should be printed. We'll reverse the characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* In-place string reversal */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }

    return s;
}

/* Optimised memset */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile("                          \n\
                 .memset_top:               \n\
                 testl   %%ecx, %%ecx       \n\
                 jz      .memset_done       \n\
                 testl   $0x3, %%edi        \n\
                 jz      .memset_aligned    \n\
                 movb    %%al, (%%edi)      \n\
                 addl    $1, %%edi          \n\
                 subl    $1, %%ecx          \n\
                 jmp     .memset_top        \n\
                 .memset_aligned:           \n\
                 movw    %%ds, %%dx         \n\
                 movw    %%dx, %%es         \n\
                 movl    %%ecx, %%edx       \n\
                 shrl    $2, %%ecx          \n\
                 andl    $0x3, %%edx        \n\
                 cld                        \n\
                 rep     stosl              \n\
                 .memset_bottom:            \n\
                 testl   %%edx, %%edx       \n\
                 jz      .memset_done       \n\
                 movb    %%al, (%%edi)      \n\
                 addl    $1, %%edi          \n\
                 subl    $1, %%edx          \n\
                 jmp     .memset_bottom     \n\
                 .memset_done:              \n\
                 "
                 :
                 : "a" (c << 24 | c << 16 | c << 8 | c), "D" (s), "c" (n)
                 : "edx", "memory", "cc"
                );

    return s;
}

/* Optimised memset_word */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile("                          \n\
                 movw    %%ds, %%dx         \n\
                 movw    %%dx, %%es         \n\
                 cld                        \n\
                 rep     stosw              \n\
                 "
                 :
                 : "a" (c), "D" (s), "c" (n)
                 : "edx", "memory", "cc"
                );

    return s;
}

/* Optimised memset_dword */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile("                          \n\
                 movw    %%ds, %%dx         \n\
                 movw    %%dx, %%es         \n\
                 cld                        \n\
                 rep     stosl              \n\
                 "
                 :
                 : "a" (c), "D" (s), "c" (n)
                 : "edx", "memory", "cc"
                );

    return s;
}

/* Optimised memcpy */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile("                          \n\
                 .memcpy_top:               \n\
                 testl   %%ecx, %%ecx       \n\
                 jz      .memcpy_done       \n\
                 testl   $0x3, %%edi        \n\
                 jz      .memcpy_aligned    \n\
                 movb    (%%esi), %%al      \n\
                 movb    %%al, (%%edi)      \n\
                 addl    $1, %%edi          \n\
                 addl    $1, %%esi          \n\
                 subl    $1, %%ecx          \n\
                 jmp     .memcpy_top        \n\
                 .memcpy_aligned:           \n\
                 movw    %%ds, %%dx         \n\
                 movw    %%dx, %%es         \n\
                 movl    %%ecx, %%edx       \n\
                 shrl    $2, %%ecx          \n\
                 andl    $0x3, %%edx        \n\
                 cld                        \n\
                 rep     movsl              \n\
                 .memcpy_bottom:            \n\
                 testl   %%edx, %%edx       \n\
                 jz      .memcpy_done       \n\
                 movb    (%%esi), %%al      \n\
                 movb    %%al, (%%edi)      \n\
                 addl    $1, %%edi          \n\
                 addl    $1, %%esi          \n\
                 subl    $1, %%edx          \n\
                 jmp     .memcpy_bottom     \n\
                 .memcpy_done:              \n\
                 "
                 :
                 : "S" (src), "D" (dest), "c" (n)
                 : "eax", "edx", "memory", "cc"
                );

    return dest;
}

/* Optimised memmove (used for overlapping memory areas) */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile("                                  \n\
                 movw    %%ds, %%dx                 \n\
                 movw    %%dx, %%es                 \n\
                 cld                                \n\
                 cmp     %%edi, %%esi               \n\
                 jae     .memmove_go                \n\
                 leal    -1(%%esi, %%ecx), %%esi    \n\
                 leal    -1(%%edi, %%ecx), %%edi    \n\
                 std                                \n\
                 .memmove_go:                       \n\
                 rep     movsb                      \n\
                 "
                 :
                 : "D" (dest), "S" (src), "c" (n)
                 : "edx", "memory", "cc"
                );

    return dest;
}

/* String length */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;

    return len;
}

/* Standard strncmp */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0'))
            return s1[i] - s2[i];
    }

    return 0;
}

/* Standard strcpy */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
    return dest;
}

/* Standard strncpy */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }

    while (i < n) {
        dest[i] = '\0';
        i++;
    }

    return dest;
}

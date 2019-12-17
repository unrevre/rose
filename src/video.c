/*!
 * video.c
 * @ Functions to manipulate video memory
 */

#include "video.h"

#include "lib.h"
#include "memory.h"
#include "process.h"

#define NUM_COLS            80
#define NUM_ROWS            25
#define ATTRIB              0x7

#define CURSOR_HIGH         0xE
#define CURSOR_LOW          0xF

#define VGA_INDEX_REGISTER  0x3D4
#define VGA_DATA_REGISTER   0x3D5

static uint8_t* pmem_video = (uint8_t*)PMEM_VIDEO;
static uint8_t* vmem_video = (uint8_t*)VMEM_VIDEO;

void print(struct tty_t* tty, uint8_t c) {
    if (c == '\n' || c == '\r')
        newline(tty);
    else
        display(tty, c);
}

void backspace(struct tty_t* tty) {
    int32_t* offset = &tty->offset;

    --*offset;

    *(pmem_video + (*offset << 1)) = ' ';
    *(pmem_video + (*offset << 1) + 1) = ATTRIB;

    blink();
}

void display(struct tty_t* tty, uint8_t c) {
    int32_t* offset = &tty->offset;
    uint8_t* address = tty == tty0 ? pmem_video : vmem_video;

    *(address + (*offset << 1)) = c;
    *(address + (*offset << 1) + 1) = ATTRIB;

    ++*offset;

    scroll(tty);
    blink();
}

void newline(struct tty_t* tty) {
    int32_t* offset = &tty->offset;

    *offset = (*offset / NUM_COLS + 1) * NUM_COLS;

    scroll(tty);
    blink();
}

void scroll(struct tty_t* tty) {
    int32_t* offset = &tty->offset;
    uint8_t* address = tty == tty0 ? pmem_video : vmem_video;

    if (*offset < NUM_ROWS * NUM_COLS)
        return;

    *offset -= NUM_COLS;

    int32_t block = NUM_COLS * (NUM_ROWS - 1);
    memmove(address, address + (NUM_COLS << 1), block << 1);

    int32_t i;
    for (i = 0; i < NUM_COLS; ++i) {
        *(address + ((block + i) << 1)) = ' ';
        *(address + ((block + i) << 1) + 1) = ATTRIB;
    }
}

void clear(struct tty_t* tty) {
    int32_t* offset = &tty->offset;

    *offset = 0;

    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(vmem_video + (i << 1)) = ' ';
        *(vmem_video + (i << 1) + 1) = ATTRIB;
    }

    blink();
}

void blink(void) {
    int32_t* offset = &tty0->offset;

    outb(CURSOR_LOW, VGA_INDEX_REGISTER);
    outb((uint8_t)(*offset & 0xFF), VGA_DATA_REGISTER);
    outb(CURSOR_HIGH, VGA_INDEX_REGISTER);
    outb((uint8_t)((*offset >> 8) & 0xFF), VGA_DATA_REGISTER);
}

void vputc(uint8_t c) {
    print(proc0->tty, c);
}

/*!
 * tty.c
 * @ Functions for the virtual terminal/line discipline systems
 */

#include "tty.h"

#include "lib.h"
#include "memory.h"
#include "page.h"
#include "process.h"
#include "signal.h"

#define KEY_DOWN_LEFT_CTRL      0x1D
#define KEY_UP_LEFT_CTRL        0x9D
#define KEY_DOWN_RIGHT_CTRL     0xE01D
#define KEY_UP_RIGHT_CTRL       0xE09D

#define KEY_DOWN_LEFT_SHIFT     0x2A
#define KEY_UP_LEFT_SHIFT       0xAA
#define KEY_DOWN_RIGHT_SHIFT    0x36
#define KEY_UP_RIGHT_SHIFT      0xB6

#define KEY_DOWN_CAPS           0x3A

#define KEY_DOWN_ENTER          0x1C
#define KEY_DOWN_BACKSPACE      0x0E

#define KEY_DOWN_C              0x2E

static uint8_t char_map_def[0x80] = {
    0x00, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x00, 0x00,
    0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69,
    0x6F, 0x70, 0x5B, 0x5D, 0x0A, 0x00, 0x61, 0x73,
    0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B,
    0x27, 0x60, 0x00, 0x5C, 0x7A, 0x78, 0x63, 0x76,
    0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E,
    0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x00, 0x00,
    0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49,
    0x4F, 0x50, 0x7B, 0x7D, 0x00, 0x00, 0x41, 0x53,
    0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3A,
    0x22, 0x7E, 0x00, 0x7C, 0x5A, 0x58, 0x43, 0x56,
    0x42, 0x4E, 0x4D, 0x3C, 0x3E, 0x3F, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8_t char_map_caps[0x80] = {
    0x00, 0x00, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0x00, 0x00,
    0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49,
    0x4F, 0x50, 0x3B, 0x3D, 0x0A, 0x00, 0x41, 0x53,
    0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3B,
    0x27, 0x60, 0x00, 0x5C, 0x5A, 0x58, 0x43, 0x56,
    0x42, 0x4E, 0x4D, 0x2C, 0x2E, 0x2F, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E,
    0x26, 0x2A, 0x28, 0x29, 0x5F, 0x2B, 0x00, 0x00,
    0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49,
    0x4F, 0x50, 0x7B, 0x7D, 0x00, 0x00, 0x41, 0x53,
    0x44, 0x46, 0x47, 0x48, 0x4A, 0x4B, 0x4C, 0x3A,
    0x22, 0x7E, 0x00, 0x7C, 0x5A, 0x58, 0x43, 0x56,
    0x42, 0x4E, 0x4D, 0x3C, 0x3E, 0x3F, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

modifiers_t flags;

tty_t ttys[TTY_MAX];
tty_t* tty0;

void init_tty(void) {
    flags.ctrl = 0;
    flags.shift = 0;
    flags.capslock = 0;

    int32_t i;
    for (i = 0; i < TTY_MAX; ++i)
        ttys[i].status = TTY_IDLE;

    tty0 = &ttys[0];
}

void start_tty(int32_t index) {
    tty_t* tty = &ttys[index];

    if (tty->status == TTY_IDLE) {
        tty->line.index = 0;
        memset(tty->line.buffer, 0, LINE_MAX * sizeof(uint8_t));

        tty->status = TTY_ACTIVE;
        tty->pid = 0;
        tty->nproc = 0;
    }

    tty0 = tty;
}

void swap_tty(int32_t index) {
    tty_t* target = &ttys[index];
    tty_t* source = tty0;

    if (target == source)
        return;

    cli();
    disable_paging();

    memcpy((uint8_t*)tty_buffer(source), (uint8_t*)PMEM_VIDEO, BLOCK_4KB);
    memcpy((uint8_t*)PMEM_VIDEO, (uint8_t*)tty_buffer(target), BLOCK_4KB);

    tty_t* tty = proc0->tty;

    uint32_t buffer = 0;
    if (source == tty)
        buffer = tty_buffer(tty);
    if (target == tty)
        buffer = PMEM_VIDEO;

    if (buffer) {
        map_memory_page(VMEM_VIDEO, buffer, SUPERVISOR, page_table_kernel);
        if (page_table_user[(VMEM_VIDEO_USER >> 12) & 0x3FF].present)
            map_memory_page(VMEM_VIDEO_USER, buffer, USER, page_table_user);
    }

    enable_paging();
    sti();
}

uint32_t tty_buffer(tty_t* tty) {
    return PMEM_VIDEO_BUFFER + (tty - ttys) * BLOCK_4KB;
}

void handle_event(uint32_t scancode) {
    tty_t* tty = tty0;

    if (flags.ctrl) {
        switch (scancode) {
            case KEY_DOWN_C:
                queue_signal(SIGINT);
                return;
        }
    }

    switch (scancode) {
        case KEY_DOWN_LEFT_CTRL:
        case KEY_DOWN_RIGHT_CTRL:
            ++flags.ctrl;
            break;
        case KEY_UP_LEFT_CTRL:
        case KEY_UP_RIGHT_CTRL:
            --flags.ctrl;
            break;
        case KEY_DOWN_LEFT_SHIFT:
        case KEY_DOWN_RIGHT_SHIFT:
            ++flags.shift;
            break;
        case KEY_UP_LEFT_SHIFT:
        case KEY_UP_RIGHT_SHIFT:
            --flags.shift;
            break;
        case KEY_DOWN_CAPS:
            flags.capslock ^= 0x1;
            break;
        case KEY_DOWN_ENTER:
            tty->status |= TTY_READ;
            newline();
            break;
        case KEY_DOWN_BACKSPACE:
            if (tty->line.index) {
                tty->line.buffer[--tty->line.index] = '\0';
                backspace();
            }
            break;
    }

    uint8_t* char_map = flags.capslock ? char_map_caps : char_map_def;

    if (scancode < 0x40 && char_map[scancode]) {
        if (tty->line.index == LINE_MAX) {
            tty->status |= TTY_READ;
            newline();
        }

        if (flags.shift)
            scancode += 0x40;

        tty->line.buffer[tty->line.index] = char_map[scancode];
        putc(tty->line.buffer[tty->line.index++]);
    }
}

/* File operations (tty) */

int32_t tty_read(int32_t fd, int8_t* buf, int32_t nbytes) {
    while (!(tty0->status & TTY_READ));

    int32_t length = strnlen(tty0->line.buffer, nbytes);
    strncpy(buf, tty0->line.buffer, length);

    cli();
    memmove(tty0->line.buffer, tty0->line.buffer + length,
            LINE_MAX - length);
    memset(tty0->line.buffer + LINE_MAX - length, '\0', length);
    tty0->line.index = tty0->line.index - length;

    tty0->status &= ~TTY_READ;
    sti();

    return length;
}

int32_t tty_write(const int8_t* buf, int32_t nbytes) {
    int32_t i;
    for (i = 0; i < nbytes; ++i)
        putc(buf[i]);

    return nbytes;
}

int32_t tty_open(void) {
    return 0;
}

int32_t tty_close(void) {
    return 0;
}

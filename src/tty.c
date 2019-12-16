/*!
 * tty.c
 * @ Functions for the virtual terminal/line discipline systems
 */

#include "tty.h"

#include "lib.h"
#include "memory.h"
#include "page.h"
#include "process.h"
#include "video.h"

struct tty_t ttys[TTY_MAX];
struct tty_t* tty0;

void init_tty(void) {
    int32_t i;
    for (i = 0; i < TTY_MAX; ++i) {
        ttys[i].status = TTY_IDLE;
        ttys[i].offset = 0;
    }

    tty0 = &ttys[0];
}

void start_tty(int32_t index) {
    struct tty_t* tty = &ttys[index];

    swap_tty(index);

    if (tty->status == TTY_IDLE) {
        tty->index = 0;
        memset(tty->buffer, 0, LINE_MAX * sizeof(uint8_t));

        tty->status = TTY_ACTIVE;
        tty->pid = PROC_INV;
        tty->nproc = 0;
    }
}

void swap_tty(int32_t index) {
    struct tty_t* target = &ttys[index];
    struct tty_t* source = tty0;

    if (target == source)
        return;

    cli();

    disable_paging();
    memcpy((uint8_t*)tty_buffer(source), (uint8_t*)PMEM_VIDEO, BLOCK_4KB);
    memcpy((uint8_t*)PMEM_VIDEO, (uint8_t*)tty_buffer(target), BLOCK_4KB);

    struct tty_t* tty = proc0->tty;
    map_video_memory((target == tty) ? PMEM_VIDEO : tty_buffer(tty));
    enable_paging();

    tty0 = target;

    sti();

    blink();
}

uint32_t tty_buffer(struct tty_t* tty) {
    return PMEM_VIDEO_BUFFER + (tty - ttys) * BLOCK_4KB;
}

/* File operations (tty) */

int32_t tty_read(int32_t fd, int8_t* buf, int32_t nbytes) {
    struct tty_t* tty = proc0->tty;

    while (!(tty->status & TTY_READ));

    int32_t length = strnlen(tty->buffer, nbytes);
    strncpy(buf, tty->buffer, length);

    memmove(tty->buffer, tty->buffer + length, LINE_MAX - length);
    memset(tty->buffer + LINE_MAX - length, '\0', length);
    tty->index = tty->index - length;

    tty->status &= ~TTY_READ;

    return length;
}

int32_t tty_write(const int8_t* buf, int32_t nbytes) {
    struct tty_t* tty = proc0->tty;

    int32_t i;
    for (i = 0; i < nbytes; ++i)
        print(tty, buf[i]);

    return nbytes;
}

int32_t tty_open(void) {
    return 0;
}

int32_t tty_close(void) {
    return 0;
}

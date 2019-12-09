/*!
 * pit.c
 * @ Functions to interact with PIT
 */

#include "pit.h"

#include "i8259.h"
#include "lib.h"
#include "process.h"
#include "schedule.h"
#include "types.h"

void init_pit(void) {
    outb(PIT_MODE, MODE_CMD_REGISTER);
    outb(PIT_FREQ & 0xFF, CHANNEL0);
    outb(PIT_FREQ >> 8, CHANNEL0);
}

void handle_pit(void) {
    cli();
    send_eoi(IRQ_PIT);

    sti();

    int32_t pid = schedule_next();
    if (pid != PROC_INV)
        raise(pid);
}

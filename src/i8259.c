/*!
 * i8259.c
 * @ Functions to interact with the 8259 interrupt controller
 */

#include "i8259.h"

#include "lib.h"

#define MASTER_PORT     0x20
#define SLAVE_PORT      0xA0

#define SLAVE_IRQ       0x2

/* Interrupt masks to determine which interrupts are enabled and disabled */
/* IRQs 0-7 */
uint8_t master_mask = 0xFB;
/* IRQs 8-15 */
uint8_t slave_mask = 0xFF;

/* Initialize the 8259 PIC */
void init_i8259(void) {
    /* Mask all interrupts for SLAVE */
    outb(slave_mask, SLAVE_PORT + 1);
    /* Mask all interrupts for MASTER */
    outb(master_mask, MASTER_PORT + 1);

    /* Initialise the Master PIC */
    outb(ICW1, MASTER_PORT);
    outb(ICW2_MASTER, MASTER_PORT + 1);
    outb(ICW3_MASTER, MASTER_PORT + 1);
    outb(ICW4, MASTER_PORT + 1);

    /* Initialise the Slave PIC */
    outb(ICW1, SLAVE_PORT);
    outb(ICW2_SLAVE, SLAVE_PORT + 1);
    outb(ICW3_SLAVE, SLAVE_PORT + 1);
    outb(ICW4, SLAVE_PORT + 1);

    enable_irq(SLAVE_IRQ);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    /* Return if irq_num is invalid */
    if (irq_num > 0xF) return;

    uint8_t mask = ~(1 << (irq_num & 0x7));

    if (irq_num & 0x8) {
        /* Enable IRQ on SLAVE */
        slave_mask &= mask;
        outb(slave_mask, SLAVE_PORT + 1);
    } else {
        /* Enable IRQ on MASTER */
        master_mask &= mask;
        outb(master_mask, MASTER_PORT + 1);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if (irq_num > 0xF) return;

    uint8_t mask = 1 << (irq_num & 0x7);

    if (irq_num & 0x8) {
        /* Disable IRQ on SLAVE */
        slave_mask |= mask;
        outb(slave_mask, SLAVE_PORT + 1);
    } else {
        /* Enable IRQ on MASTER */
        master_mask |= mask;
        outb(master_mask, MASTER_PORT + 1);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num > 0xF) return;

    if (irq_num & 0x8) {
        /* Send EOI for IRQ on SLAVE */
        outb(EOI | SLAVE_IRQ, MASTER_PORT);
        outb(EOI | (irq_num & 0x7), SLAVE_PORT);
    } else {
        /* Send EOI for IRQ on MASTER */
        outb(EOI | irq_num, MASTER_PORT);
    }
}

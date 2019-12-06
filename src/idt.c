/*!
 * idt.c
 * @ IDT initialisation
 */

#include "idt.h"

#include "interrupt.h"
#include "x86_desc.h"

void init_idt(void) {
    idt_desc_t interrupt_gate;
    interrupt_gate.seg_selector = KERNEL_CS;
    interrupt_gate.reserved4 = 0;
    interrupt_gate.reserved3 = 0;
    interrupt_gate.reserved2 = 1;
    interrupt_gate.reserved1 = 1;
    interrupt_gate.size = 1;
    interrupt_gate.reserved0 = 0;
    interrupt_gate.dpl = 0;
    interrupt_gate.present = 1;

    idt_desc_t trap_gate;
    trap_gate.seg_selector = KERNEL_CS;
    trap_gate.reserved4 = 0;
    trap_gate.reserved3 = 1;
    trap_gate.reserved2 = 1;
    trap_gate.reserved1 = 1;
    trap_gate.size = 1;
    trap_gate.reserved0 = 0;
    trap_gate.dpl = 0;
    trap_gate.present = 1;

#define SET_IDT(index, gate, handler)       \
    idt[index] = gate;                      \
    SET_IDT_ENTRY(idt[index], handler);

    SET_IDT(0, interrupt_gate, divide_error);
    SET_IDT(1, interrupt_gate, debug);
    SET_IDT(2, interrupt_gate, nmi);
    SET_IDT(3, interrupt_gate, breakpoint);
    SET_IDT(4, interrupt_gate, overflow);
    SET_IDT(5, interrupt_gate, bound_range_exceeded);
    SET_IDT(6, interrupt_gate, invalid_opcode);
    SET_IDT(7, interrupt_gate, device_not_available);
    SET_IDT(8, interrupt_gate, double_fault);
    // idt[9] is reserved
    SET_IDT(10, interrupt_gate, invalid_tss);
    SET_IDT(11, interrupt_gate, segment_not_present);
    SET_IDT(12, interrupt_gate, stack_fault);
    SET_IDT(13, interrupt_gate, general_protection);
    SET_IDT(14, interrupt_gate, page_fault);
    // idt[15] is reserved
    SET_IDT(16, interrupt_gate, floating_point_error);
    SET_IDT(17, interrupt_gate, alignment_check);
    SET_IDT(18, interrupt_gate, machine_check);
    SET_IDT(19, interrupt_gate, simd_fp_exception);

    SET_IDT(IDT_PIT, interrupt_gate, pit);
    SET_IDT(IDT_KBD, interrupt_gate, kbd);
    SET_IDT(IDT_RTC, interrupt_gate, rtc);

    SET_IDT(IDT_SYS, trap_gate, syscall);
    idt[IDT_SYS].dpl = 3;
}

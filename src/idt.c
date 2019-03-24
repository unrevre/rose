/*!
 * idt.c
 * @ IDT initialisation
 */

#include "idt.h"

#include "interrupt.h"
#include "types.h"
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

#define SET_IDT(index, interrupt)           \
    idt[index] = interrupt_gate;            \
    SET_IDT_ENTRY(idt[index], interrupt);

    SET_IDT(0, divide_error);
    SET_IDT(1, debug);
    SET_IDT(2, nmi);
    SET_IDT(3, breakpoint);
    SET_IDT(4, overflow);
    SET_IDT(5, bound_range_exceeded);
    SET_IDT(6, invalid_opcode);
    SET_IDT(7, device_not_available);
    SET_IDT(8, double_fault);
    // idt[9] is reserved
    SET_IDT(10, invalid_tss);
    SET_IDT(11, segment_not_present);
    SET_IDT(12, stack_fault);
    SET_IDT(13, general_protection);
    SET_IDT(14, page_fault);
    // idt[15] is reserved
    SET_IDT(16, floating_point_error);
    SET_IDT(17, alignment_check);
    SET_IDT(18, machine_check);
    SET_IDT(19, simd_fp_exception);
}

/*!
 * interrupt.h
 * @ Function definitions for interrupt handlers
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

void divide_error(void);
void debug(void);
void nmi(void);
void breakpoint(void);
void overflow(void);
void bound_range_exceeded(void);
void invalid_opcode(void);
void device_not_available(void);
void double_fault(void);
void invalid_tss(void);
void segment_not_present(void);
void stack_fault(void);
void general_protection(void);
void page_fault(void);
void floating_point_error(void);
void alignment_check(void);
void machine_check(void);
void simd_fp_exception(void);

void pit(void);

#endif /* INTERRUPT_H */

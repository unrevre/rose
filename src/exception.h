/*!
 * exception.h
 * @ Function definitions for exception handlers
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

void handle_divide_error(void);
void handle_debug(void);
void handle_nmi(void);
void handle_breakpoint(void);
void handle_overflow(void);
void handle_bound_range_exceeded(void);
void handle_invalid_opcode(void);
void handle_device_not_available(void);
void handle_double_fault(void);
void handle_invalid_tss(void);
void handle_segment_not_present(void);
void handle_stack_fault(void);
void handle_general_protection(void);
void handle_page_fault(void);
void handle_floating_point_error(void);
void handle_alignment_check(void);
void handle_machine_check(void);
void handle_simd_fp_exception(void);

#endif /* EXCEPTION_H */

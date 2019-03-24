/*!
 * exceptions.c
 * @ Exception handlers
 */

#include "exception.h"

#include "lib.h"

#define default_handler(exception, string)  \
    void handle_##exception(void) {         \
        printf(#string "\n"); }

default_handler(divide_error, "divide error")
default_handler(debug, "debug")
default_handler(nmi, "nmi")
default_handler(breakpoint, "breakpoint")
default_handler(overflow, "overflow")
default_handler(bound_range_exceeded, "bound range exceeded")
default_handler(invalid_opcode, "invalid opcode")
default_handler(device_not_available, "device not available")
default_handler(double_fault, "double fault")
default_handler(invalid_tss, "invalid tss")
default_handler(segment_not_present, "segment not present")
default_handler(stack_fault, "stack fault")
default_handler(general_protection, "general protection")
default_handler(page_fault, "page fault")
default_handler(floating_point_error, "floating point error")
default_handler(alignment_check, "alignment check")
default_handler(machine_check, "machine check")
default_handler(simd_fp_exception, "simd floating point exception")

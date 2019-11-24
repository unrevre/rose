/*!
 * exceptions.c
 * @ Exception handlers
 */

#include "exception.h"

#include "lib.h"
#include "signal.h"

#define default_handler(exception, string, signal)  \
    void handle_##exception(void) {                 \
        printf(#string "\n");                       \
        queue_signal(signal);                       \
    }

default_handler(divide_error, "divide error", SIGSEGV)
default_handler(debug, "debug", SIGSEGV)
default_handler(nmi, "nmi", SIGSEGV)
default_handler(breakpoint, "breakpoint", SIGSEGV)
default_handler(overflow, "overflow", SIGSEGV)
default_handler(bound_range_exceeded, "bound range exceeded", SIGSEGV)
default_handler(invalid_opcode, "invalid opcode", SIGILL)
default_handler(device_not_available, "device not available", SIGSEGV)
default_handler(double_fault, "double fault", SIGSEGV)
default_handler(invalid_tss, "invalid tss", SIGSEGV)
default_handler(segment_not_present, "segment not present", SIGSEGV)
default_handler(stack_fault, "stack fault", SIGSEGV)
default_handler(general_protection, "general protection", SIGSEGV)
default_handler(page_fault, "page fault", SIGSEGV)
default_handler(floating_point_error, "floating point error", SIGFPE)
default_handler(alignment_check, "alignment check", SIGSEGV)
default_handler(machine_check, "machine check", SIGSEGV)
default_handler(simd_fp_exception, "simd floating point exception", SIGFPE)

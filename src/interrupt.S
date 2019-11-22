/*!
 * interrupt.S
 * @ Assembly linkage for interrupt handlers
 */

#define ASM

#include "memory.h"

.globl divide_error
.globl debug
.globl nmi
.globl breakpoint
.globl overflow
.globl bound_range_exceeded
.globl invalid_opcode
.globl device_not_available
.globl double_fault
.globl invalid_tss
.globl segment_not_present
.globl stack_fault
.globl general_protection
.globl page_fault
.globl floating_point_error
.globl alignment_check
.globl machine_check
.globl simd_fp_exception

.globl pit
.globl kbd
.globl rtc

.globl syscall

.macro HANDLE_NULL INT
\INT :
    pushl   $0
    pushl   %eax
    pushl   %ebp
    pushl   %edi
    pushl   %esi
    pushl   %edx
    pushl   %ecx
    pushl   %ebx
    call    handle_\INT
    jmp     check_signal
.endm

.macro HANDLE_ERRC INT
\INT :
    pushl   %eax
    pushl   %ebp
    pushl   %edi
    pushl   %esi
    pushl   %edx
    pushl   %ecx
    pushl   %ebx
    call    handle_\INT
    jmp     check_signal
.endm

HANDLE_NULL divide_error
HANDLE_NULL debug
HANDLE_NULL nmi
HANDLE_NULL breakpoint
HANDLE_NULL overflow
HANDLE_NULL bound_range_exceeded
HANDLE_NULL invalid_opcode
HANDLE_NULL device_not_available
HANDLE_ERRC double_fault
HANDLE_ERRC invalid_tss
HANDLE_ERRC segment_not_present
HANDLE_ERRC stack_fault
HANDLE_ERRC general_protection
HANDLE_ERRC page_fault
HANDLE_NULL floating_point_error
HANDLE_NULL alignment_check
HANDLE_NULL machine_check
HANDLE_NULL simd_fp_exception

HANDLE_NULL pit
HANDLE_NULL kbd
HANDLE_NULL rtc

#define NSYS 0xA

syscall:
    pushl   $0
    pushl   %eax
    pushl   %ebp
    pushl   %edi
    pushl   %esi
    pushl   %edx
    pushl   %ecx
    pushl   %ebx

    decl    %eax
    cmpl    $NSYS, %eax
    jae     invalid_call
    call    *call_vector(, %eax, 4)
    movl    %eax, 24(%esp)
    jmp     check_signal

invalid_call:
    movl    $-1, 24(%esp)
    iret

.align 4
check_signal:
    cmpl    $VMEM_USER, 44(%esp)
    jb      restore_context
    call    deliver_signal

restore_context:
    popl    %ebx
    popl    %ecx
    popl    %edx
    popl    %esi
    popl    %edi
    popl    %ebp
    popl    %eax
    addl    $4, %esp
    iret

.align 4
call_vector:
.long   halt
.long   execute
.long   read
.long   write
.long   open
.long   close
.long   getargs
.long   vidmap
.long   signal
.long   sigreturn

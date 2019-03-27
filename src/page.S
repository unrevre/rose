/*!
 * page.S
 * @ Initialisation and setup for paging
 */

.text

.globl  page_directory
.globl  page_table_kernel, page_table_user

.globl  init_paging
.globl  enable_paging, disable_paging

    .align 4096
page_directory:
    .rept 1024
    .long 0
    .endr

    .align 4096
page_table_kernel:
    .rept 1024
    .long 0
    .endr

    .align 4096
page_table_user:
    .rept 1024
    .long 0
    .endr

init_paging:
    pushl   %eax
    pushl   %ecx
    movl    12(%esp), %ecx
    movl    %cr3, %eax
    andl    $0x00000FFF, %eax
    orl     %ecx, %eax
    movl    %eax, %cr3
    movl    %cr4, %eax
    orl     $0x00000010, %eax
    movl    %eax, %cr4
    popl    %ecx
    popl    %eax
    ret

enable_paging:
    pushl   %eax
    movl    %cr0, %eax
    orl     $0x80000000, %eax
    movl    %eax, %cr0
    popl    %eax
    ret

disable_paging:
    pushl   %eax
    movl    %cr0, %eax
    andl    $0x7FFFFFFF, %eax
    movl    %eax, %cr0
    popl    %eax
    ret

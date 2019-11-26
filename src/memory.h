/*!
 * memory.h
 * @ Defines and functions for memory management, important memory locations
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "page.h"
#include "types.h"

#define PMEM_VIDEO          0xB8000
#define PMEM_VIDEO_BUFFER   0xB9000
#define VMEM_VIDEO          0x84B8000
#define VMEM_VIDEO_USER     0x84B9000

#define PMEM_KERNEL         0x0
#define VMEM_KERNEL         0x0

#define PMEM_USER           0x400000
#define VMEM_USER           0x8000000

#define USER                1
#define SUPERVISOR          0

#define STACK_SIZE          0x2000

#define BLOCK_4KB           0x1000
#define BLOCK_4MB           0x400000

#define KERNEL_BASE         (VMEM_KERNEL + BLOCK_4MB)

#ifndef ASM

void map_memory_block(uint32_t virtual, uint32_t physical,
                      uint32_t user_supervisor);

void map_memory_page(uint32_t virtual, uint32_t physical,
                     uint32_t user_supervisor, PTE_t* page_table);

#endif /* ASM */

#endif /* MEMORY_H */

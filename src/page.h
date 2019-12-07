/*!
 * page.h
 * @ Structures and functions related to paging
 */

#ifndef PAGE_H
#define PAGE_H

#include "types.h"

#ifndef ASM

struct pde_t {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t page_write_through : 1;
    uint32_t page_cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t zero : 1;
    uint32_t page_size : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t page_table_base_address : 20;
} __attribute__((packed));

struct pte_t {
    uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t page_write_through : 1;
    uint32_t page_cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t page_attribute_table : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t page_base_address : 20;
} __attribute__((packed));

struct pde_t page_directory[1024];

struct pte_t page_table_kernel[1024];
struct pte_t page_table_user[1024];

void init_paging(struct pde_t* address);
void enable_paging(void);
void disable_paging(void);

#endif /* ASM */

#endif /* PAGE_H */

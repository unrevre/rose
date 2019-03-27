/*!
 * page.h
 * @ Structures and functions related to paging
 */

#ifndef PAGE_H
#define PAGE_H

#include "types.h"

typedef struct {
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
} __attribute__((packed)) PDE_t;

typedef struct {
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
} __attribute__((packed)) PTE_t;

PDE_t page_directory[1024];

PTE_t page_table_kernel[1024];
PTE_t page_table_user[1024];

void init_paging(PDE_t* address);
void enable_paging(void);
void disable_paging(void);

#endif /* PAGE_H */

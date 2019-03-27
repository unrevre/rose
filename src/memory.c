/*!
 * memory.c
 * @ Functions for memory management
 */

#include "memory.h"

#include "page.h"

void map_memory_block(uint32_t virtual, uint32_t physical,
                      uint32_t user_supervisor) {
    uint32_t pd_index = virtual >> 22;

    page_directory[pd_index].present = 1;
    page_directory[pd_index].read_write = 1;
    page_directory[pd_index].user_supervisor = user_supervisor;
    page_directory[pd_index].page_write_through = 0;
    page_directory[pd_index].page_cache_disable = 0;
    page_directory[pd_index].zero = 0;
    page_directory[pd_index].page_size = 1;
    page_directory[pd_index].global = 0;
    page_directory[pd_index].page_table_base_address = physical >> 12;
}

void map_memory_page(uint32_t virtual, uint32_t physical,
                     uint32_t user_supervisor, PTE_t* page_table) {
    uint32_t pd_index = virtual >> 22;
    uint32_t pt_index = (virtual >> 12) & 0x3FF;

    page_directory[pd_index].present = 1;
    page_directory[pd_index].read_write = 1;
    page_directory[pd_index].user_supervisor = user_supervisor;
    page_directory[pd_index].page_write_through = 0;
    page_directory[pd_index].page_cache_disable = 0;
    page_directory[pd_index].zero = 0;
    page_directory[pd_index].page_size = 0;
    page_directory[pd_index].global = 0;
    page_directory[pd_index].page_table_base_address = (uint32_t)page_table >> 12;

    page_table[pt_index].present = 1;
    page_table[pt_index].read_write = 1;
    page_table[pt_index].user_supervisor = user_supervisor;
    page_table[pt_index].page_write_through = 0;
    page_table[pt_index].dirty = 1;
    page_table[pt_index].page_attribute_table = 0;
    page_table[pt_index].global = 0;
    page_table[pt_index].page_base_address = physical >> 12;
}

/*!
 * multiboot.h
 * @ Defines used in working with Multiboot-compliant bootloaders
 */

#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "types.h"

#define MULTIBOOT_HEADER_FLAGS      0x00000003
#define MULTIBOOT_HEADER_MAGIC      0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC  0x2BADB002

#ifndef ASM

/* The multiboot header. */
struct multiboot_header_t {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
};

/* The section header table for ELF. */
struct elf_section_header_table_t {
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
};

/* The multiboot information. */
struct multiboot_info_t {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    struct elf_section_header_table_t elf_sec;
    uint32_t mmap_length;
    uint32_t mmap_addr;
};

struct module_t {
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t string;
    uint32_t reserved;
};

/* The memory map. Note that offset 0 is base_addr_low and not size. */
struct memory_map_t {
    uint32_t size;
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
};

#endif /* ASM */

#endif /* MULTIBOOT_H */

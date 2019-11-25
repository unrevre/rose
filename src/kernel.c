/*!
 * kernel.c
 * @ C part of the kernel
 */

#include "fd.h"
#include "fsdef.h"
#include "i8259.h"
#include "idt.h"
#include "lib.h"
#include "memory.h"
#include "multiboot.h"
#include "page.h"
#include "pit.h"
#include "process.h"
#include "rtc.h"
#include "tty.h"
#include "types.h"
#include "x86_desc.h"

#define CHECK_FLAG(flags, bit)  ((flags) & (1 << (bit)))

void entry(uint32_t magic, uint32_t addr) {
    multiboot_info_t *mbi;

    /* Clear screen */
    clear();

    /* Check boot loader magic numbers for multiboot compliance */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%#x\n", magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure */
    mbi = (multiboot_info_t *)addr;

    /* Print flags */
    printf("flags = 0x%#x\n", mbi->flags);

    /* Check validity of mem_* values */
    if (CHECK_FLAG(mbi->flags, 0))
        printf("mem_lower = %uKB, mem_upper = %uKB\n",
            mbi->mem_lower, mbi->mem_upper);

    /* Check validity of boot_device */
    if (CHECK_FLAG(mbi->flags, 1))
        printf("boot_device = 0x%#x\n", mbi->boot_device);

    /* Check if command line passed */
    if (CHECK_FLAG(mbi->flags, 2))
        printf("cmdline = %s\n", (int8_t*)mbi->cmdline);

    if (CHECK_FLAG(mbi->flags, 3)) {
        module_t* mod = (module_t*)mbi->mods_addr;

        int32_t mod_count = 0;
        while (mod_count < mbi->mods_count) {
            printf("Module %d loaded at address: 0x%#x\n",
                mod_count, mod->mod_start);
            printf("Module %d ends at address: 0x%#x\n",
                mod_count, mod->mod_end);
            printf("First few bytes of module:\n");
            printf("0x%#x\n", *((int32_t*)(mod->mod_start)));

            mod_count++;
        }
    }

    /* Bits 4 and 5 are mutually exclusive */
    if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
        printf("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Check validity of ELF section header table */
    if (CHECK_FLAG(mbi->flags, 5)) {
        elf_section_header_table_t* elf_sec = &(mbi->elf_sec);

        printf("elf: num = %u, size = 0x%#x, addr = 0x%#x, shndx = 0x%#x\n",
            elf_sec->num, elf_sec->size, elf_sec->addr, elf_sec->shndx);
    }

    /* Check validity of mmap_* values */
    if (CHECK_FLAG(mbi->flags, 6)) {
        memory_map_t* mmap;

        printf(" mmap_addr = 0x%#x, mmap_length = 0x%x\n",
            mbi->mmap_addr, mbi->mmap_length);
        for (mmap = (memory_map_t*)mbi->mmap_addr;
                (uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length;
                mmap = (memory_map_t*)((uint32_t)mmap + mmap->size
                    + sizeof(mmap->size))) {
            printf(" size = 0x%x,   base_addr = 0x%#x%#x\n"
                " type = 0x%x,   `length    = 0x%#x%#x\n",
                mmap->size, mmap->base_addr_high, mmap->base_addr_low,
                mmap->type, mmap->length_high, mmap->length_low);
        }
    }

    /* Construct a LDT entry in the GDT */
    {
        seg_desc_t the_ldt_desc;
        the_ldt_desc.granularity    = 0;
        the_ldt_desc.opsize         = 1;
        the_ldt_desc.reserved       = 0;
        the_ldt_desc.avail          = 0;
        the_ldt_desc.present        = 1;
        the_ldt_desc.dpl            = 0x0;
        the_ldt_desc.sys            = 0;
        the_ldt_desc.type           = 0x2;

        SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
        ldt_desc_ptr = the_ldt_desc;
        lldt(KERNEL_LDT);
    }

    /* Construct a TSS entry in the GDT */
    {
        seg_desc_t the_tss_desc;
        the_tss_desc.granularity    = 0;
        the_tss_desc.opsize         = 0;
        the_tss_desc.reserved       = 0;
        the_tss_desc.avail          = 0;
        the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
        the_tss_desc.present        = 1;
        the_tss_desc.dpl            = 0x0;
        the_tss_desc.sys            = 0;
        the_tss_desc.type           = 0x9;
        the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

        SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

        tss_desc_ptr = the_tss_desc;

        tss.ldt_segment_selector = KERNEL_LDT;
        tss.ss0 = KERNEL_DS;
        tss.esp0 = KERNEL_BASE;
        ltr(KERNEL_TSS);
    }

    /* Load and initialise IDT */
    lidt(idt_desc_ptr);
    init_idt();

    /* Initialise devices, memory, filesystem, enable device interrupts on the
     * PIC, any other initialisation routines... */
    init_i8259();
    init_pit();
    init_rtc();

    init_tty();

    init_fd();
    init_pcb();

    /* Assume filesystem is the first module */
    if (CHECK_FLAG(mbi->flags, 3) && mbi->mods_count)
        init_fs((fs_t*)((module_t*)mbi->mods_addr)->mod_start);

    /* Map kernel memory block (including video memory) */
    map_memory_block(VMEM_KERNEL, PMEM_KERNEL, SUPERVISOR);

    /* Enable paging */
    init_paging(page_directory);
    enable_paging();

    /* Enable interrupts */
    /* QEMU will triple fault and simply close without any output if the IDT is
     * not set up correctly */
    enable_irq(IRQ_PIT);
    enable_irq(IRQ_KBD);
    enable_irq(IRQ_RTC);

    printf("Enabling Interrupts\n");
    sti();

    /* Initialise terminal */
    start_tty(0);

    /* Spin (nicely, to not chew up cycles) */
    asm volatile(".1: hlt; jmp .1;");
}

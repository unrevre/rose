/*!
 * idt.h
 * @ Defines used in setting up the IDT
 */

#ifndef IDT_H
#define IDT_H

#define IDT_PIT             0x20
#define IDT_KBD             0x21
#define IDT_RTC             0x28

#define IDT_SYS             0x80

void init_idt(void);

#endif /* IDT_H */

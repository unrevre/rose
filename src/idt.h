/*!
 * idt.h
 * @ Defines used in setting up the IDT
 */

#ifndef IDT_H
#define IDT_H

#define IDT_PIT             0x20

void init_idt(void);

#endif /* IDT_H */

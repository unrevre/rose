/*!
 * pit.h
 * @ Defines for interaction with the PIT
 */

#ifndef PIT_H
#define PIT_H

#define MODE_CMD_REGISTER   0x43
#define CHANNEL0            0x40

#define PIT_MODE            0x36
#define PIT_FREQ            1193180

void init_pit(void);

void handle_pit(void);

#endif /* PIT_H */

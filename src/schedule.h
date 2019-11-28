/*!
 * schedule.h
 * @ Defines and function definitions for process scheduling
 */

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "types.h"

int32_t schedule_next(void);

void raise(int32_t pid);

#endif /* SCHEDULE_H */

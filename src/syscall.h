/*!
 * syscall.h
 * @ Defines and function definitions for system calls
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

int32_t halt(uint8_t status);
int32_t execute(const int8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const int8_t* fname);
int32_t close(int32_t fd);

#endif /* SYSCALL_H */

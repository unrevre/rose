/*!
 * types.h
 * @ Defines to use the familiar explicitly-sized types in place of <stdint.h>
 */

#ifndef TYPES_H
#define TYPES_H

#define NULL 0

#ifndef ASM

typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

#endif /* ASM */

#endif /* TYPES_H */

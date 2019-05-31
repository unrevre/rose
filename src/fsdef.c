/*!
 * fsdef.c
 * @ Functions to interact with the filesystem
 */

#include "fsdef.h"

fs_t* fs;

void init_fs(fs_t* address) {
    fs = address;
}

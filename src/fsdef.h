/*!
 * fsdef.h
 * @ Defines and function definitions for interacting with the filesystem
 */

#ifndef FSDEF_H
#define FSDEF_H

#include "types.h"

typedef struct {
    int8_t fname[32];
    int32_t ftype;
    int32_t inode;
    uint32_t reserved[6];
} dentry_t;

typedef struct {
    int32_t ndentry;
    int32_t ninode;
    int32_t nblock;
    uint32_t reserved[13];
    dentry_t dentries[63];
} boot_t;

typedef struct {
    int32_t size;
    int32_t block[1023];
} inode_t;

typedef struct {
    int8_t raw[4096];
} block_t;

#define INODE_MAX   0x24
#define BLOCK_MAX   0x4E

typedef struct {
    boot_t boot;
    inode_t inodes[INODE_MAX];
    block_t blocks[BLOCK_MAX];
} fs_t;

void init_fs(fs_t* address);

int32_t read_data(int32_t inode, int32_t offset, int8_t* buf, int32_t nbytes);
int32_t query_inode(const int8_t* fname);

#endif /* FSDEF_H */

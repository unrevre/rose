/*!
 * fsdef.h
 * @ Defines and function definitions for interacting with the filesystem
 */

#ifndef FSDEF_H
#define FSDEF_H

#include "types.h"

struct dentry_t {
    int8_t fname[32];
    int32_t ftype;
    int32_t inode;
    uint32_t reserved[6];
};

struct boot_t {
    int32_t ndentry;
    int32_t ninode;
    int32_t nblock;
    uint32_t reserved[13];
    struct dentry_t dentries[63];
};

struct inode_t {
    int32_t size;
    int32_t block[1023];
};

struct block_t {
    int8_t raw[4096];
};

#define INODE_MAX   0x24
#define BLOCK_MAX   0x4E

struct fs_t {
    struct boot_t boot;
    struct inode_t inodes[INODE_MAX];
    struct block_t blocks[BLOCK_MAX];
};

void init_fs(struct fs_t* address);

int32_t read_data(int32_t inode, int32_t offset, int8_t* buf, int32_t nbytes);
int32_t query_inode(const int8_t* fname);
struct dentry_t* query_dentry(const int8_t* fname);

/* File operations (directory) */

int32_t dir_read(int32_t fd, int8_t* buf, int32_t nbytes);
int32_t dir_write(const int8_t* buf, int32_t nbytes);
int32_t dir_open(void);
int32_t dir_close(void);

/* File operations (file) */

int32_t file_read(int32_t fd, int8_t* buf, int32_t nbytes);
int32_t file_write(const int8_t* buf, int32_t nbytes);
int32_t file_open(void);
int32_t file_close(void);

#endif /* FSDEF_H */

/*!
 * fsdef.c
 * @ Functions to interact with the filesystem
 */

#include "fsdef.h"

#include "fd.h"
#include "lib.h"
#include "process.h"

fs_t* fs;

void init_fs(fs_t* address) {
    fs = address;
}

int32_t read_data(int32_t inode, int32_t offset, int8_t* buf, int32_t nbytes) {
    if (nbytes < 0)
        return -1;

    if (inode >= INODE_MAX)
        return -1;

    int32_t end = nbytes + offset;
    if (fs->inodes[inode].size < end)
        end = fs->inodes[inode].size;

    int32_t i;
    int32_t bytes = 0;
    for (i = offset / 4096; i <= end / 4096; ++i) {
        int8_t* src = fs->blocks[fs->inodes[inode].block[i]].raw;

        int32_t partial = 4096;
        if (i == offset / 4096) {
            src = src + (offset % 4096);
            partial = partial - (offset % 4096);
        }
        if (i == end / 4096) {
            partial = partial - (4096 - end % 4096);
        }
        memcpy(buf + bytes, src, partial);
        bytes = bytes + partial;
    }

    return bytes;
}

int32_t query_inode(const int8_t* fname) {
    if (fname[0] == '\0')
        return -1;

    int32_t i;
    for (i = 0; i < 63; ++i) {
        if (!strncmp(fname, fs->boot.dentries[i].fname, 32))
            return fs->boot.dentries[i].inode;
    }

    return -1;
}

dentry_t* query_dentry(const int8_t* fname) {
    if (fname[0] == '\0')
        return NULL;

    int32_t i;
    for (i = 0; i < 63; ++i) {
        if (!strncmp(fname, fs->boot.dentries[i].fname, 32))
            return &fs->boot.dentries[i];
    }

    return NULL;
}

/* File operations (directory) */

int32_t dir_read(int32_t fd, int8_t* buf, int32_t nbytes) {
    pcb_t* process = proc0;

    int32_t dentry = process->fds[fd].fpos;
    if (dentry == fs->boot.ndentry)
        return 0;

    ++process->fds[fd].fpos;
    strncpy(buf, fs->boot.dentries[dentry].fname, nbytes);

    return strnlen(buf, nbytes);
}

int32_t dir_write(const int8_t* buf, int32_t nbytes) {
    return -1;
}

int32_t dir_open(void) {
    return 0;
}

int32_t dir_close(void) {
    return 0;
}

/* File operations (file) */

int32_t file_read(int32_t fd, int8_t* buf, int32_t nbytes) {
    pcb_t* process = proc0;

    int32_t inode = process->fds[fd].inode;
    int32_t offset = process->fds[fd].fpos;

    int32_t length = read_data(inode, offset, buf, nbytes);;
    process->fds[fd].fpos = offset + length;

    return length;
}

int32_t file_write(const int8_t* buf, int32_t nbytes) {
    return -1;
}

int32_t file_open(void) {
    return 0;
}

int32_t file_close(void) {
    return 0;
}

#include <common.h>
#include <vfs.h>
#include <klib.h>
extern void TODO();

int inodeopen(file_t *file, int flags){
    TODO();
    return 0;
}

int inodeclose(file_t *file){
    TODO();
    return 0;
}

ssize_t inoderead(file_t *file, char *buf, size_t size){
    TODO();
    return 0;
}

ssize_t inodewrite(file_t *file, const char *buf, size_t size){
    TODO();
    return 0;
}

off_t inodelseek(file_t *file, off_t offset, int whence){
    TODO();
    return 0;
}

int inodemkdir(const char *name){
    TODO();
    return 0;
}

int inodermdir(const char *name){
    TODO();
    return 0;
}

int inodelink(const char *name, inode_t *inode){
    TODO();
    return 0;
}

int inodeunlink(const char *name){
    TODO();
    return 0;
}

inodeops_t inode_ops = {
    .open = inodeopen,
    .close = inodeclose,
    .read = inoderead,
    .write = inodewrite,
    .lseek = inodelseek,
    .mkdir = inodemkdir,
    .rmdir = inodermdir,
    .link = inodelink,
    .unlink = inodeunlink,
};

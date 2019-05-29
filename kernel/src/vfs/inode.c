#include <commo.h>
#include <vfs.h>
#include <klib.h>
extern void TODO();

int open(file_t *file, int flags){
    TODO();
    return 0;
}

int close(file_t *file){
    TODO();
    return 0;
}

ssize_t read(file_t *file, char *buf, size_t size){
    TODO();
    return 0;
}

ssize_t write(file_t *file, const char *buf, size_t size){
    TODO();
    return 0;
}

off_t lseek(file_t *file, off_t offset, int whence){
    TODO();
    return 0;
}

int mkdir(const char *name){
    TODO();
    return 0;
}

int rmdir(const char *name){
    TODO();
    return 0;
}

int link(const char *name, inode_t *inode){
    TODO();
    return 0;
}

int unlink(const char *name){
    TODO();
    return 0;
}

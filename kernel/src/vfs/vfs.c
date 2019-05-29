#include <common.h>
#include <vfs.h>
#include <klib.h>

void TODO(){
    assert(0);
}

void init(){
    TODO();
    return;
}

int access(const char *path, int mode){
    TODO();
    return 0;
}

int mount(const char *path, filesystem_t *fs){
    TODO();
    return 0;
}

int unmount(const char *path){
    TODO();
    return 0;
}

int mkdir(const char *path){
    TODO();
    return 0;
}

int rmdir(const char *path){
    TODO();
    return 0;
}

int link(const char *oldpath, const char *newpath){
    TODO();
    return 0;
}

int unlink(const char *path){
    TODO();
    return 0;
}

int open(const char *path, int flags){
    TODO();
    return 0;
}

ssize_t read(int fd, void *buf, size_t nbyte){
    TODO();
    return 0;
}

ssize_t write(int fd, void *buf, size_t nbyte){
    TODO();
    return 0;
}

off_t lseek(int fd, off_t offset, int whence){
    TODO();
    return 0;
}

int close(int fd){
    TODO();
    return 0;
}


MODULE_DEF(vfs){
    .init = init,
    .access = access,
    .mount = mount,
    .unmount = unmount,
    .mkdir = mkdir,
    .rmdir = rmdir,
    .link = link,
    .unlink = unlink,
    .open = open,
    .read = read,
    .write = write,
    .lseek = lseek,
    .close = close,
};

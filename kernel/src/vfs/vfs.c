#include <common.h>
#include <vfs.h>
#include <klib.h>
#define FILESYSTEM(_) \
    _(0, filesystem_t, "procfs", 1, &fsops_t) \
    _(1, filesystem_t, "devfs",  2, &fsops_t) \
    _(2, filesystem_t, "blkfs",  3, &fsops_t)

#define FS_CNT(...) + 1
filesystem_t filesys[0 FILESYSTEM(FS_CNT)];

void TODO(){
    assert(0);
}

static filesystem_t *filesys_create(size_t fs_size, const char* fs_name, int fs_id, fsops_t *fs_ops){
    filesystem_t *filesys = pmm->alloc(sizeof(filesystem_t));
    *filesys = (filesystem_t){
        .name = fs_name,
        .id = fs_id,
        .ops = fs_ops,
        /*.dev = dev,*/
    };
    return filesys;
}

#define FSCREATE(id, fs_type, fs_name, fs_id, fs_ops) \
    filesys[id] = filesys_create(sizeof(fs_type), fs_name, fs_id, fs_ops);

void init(){
    /*TODO();*/
    FILESYSTEM(FSCREATE);

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

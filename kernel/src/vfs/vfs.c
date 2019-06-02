#include <common.h>
#include <vfs.h>
#include <klib.h>

extern fsops_t fs_ops;
extern device_t *devices[8];
#define L3DEBUG

#define FILESYSTEM(_) \
    _(0, filesystem_t, "procfs", 0, &fs_ops, NULL) \
    _(1, filesystem_t, "devfs",  1, &fs_ops, NULL) \
    _(2, filesystem_t, "blkfs",  2, &fs_ops, devices[1])

#define FS_CNT(...) + 1
filesystem_t *filesys[0 FILESYSTEM(FS_CNT)];

void TODO(){
    assert(0);
}

static filesystem_t *filesys_create(size_t fs_size, const char* fs_name, int fs_id, fsops_t *fs_ops, device_t *dev){
    filesystem_t *filesys = pmm->alloc(sizeof(filesystem_t));
    *filesys = (filesystem_t){
        .name = fs_name,
        .id = fs_id,
        .ops = fs_ops,
        .dev = dev,
    };
    return filesys;
}

#define FSCREATE(id, fs_type, fs_name, fs_id, fs_ops, dev) \
    filesys[id] = filesys_create(sizeof(fs_type), fs_name, fs_id, fs_ops, dev);

#define FSINIT(id, fs_type, fs_name, fs_id, fs_ops, dev) \
    filesys[id]->ops->init(filesys[id], fs_name, dev);


void Loginode(inode_t *node){
    Logg("%d", node->refcnt);
    Logg("%d", node->flags);
    Logg("%d", node->is_dir);
    Logg("%x", node->offset[0]);
}

void check(inode_t *ret){
    if(ret == NULL){
        Logg("Notfound");
    }else{
        Logg("Create dir");
        Loginode(ret);
    }
}

void init(){
    /*TODO();*/
    FILESYSTEM(FSCREATE);
    FILESYSTEM(FSINIT);
#ifdef L3DEBUG
    const char *path = pmm->alloc(100);
    path = "hello";
    inode_t *ret = filesys[2]->ops->lookup(filesys[2], path, O_CREAT|O_DIR);
    check(ret);
    ret = filesys[2]->ops->lookup(filesys[2], "hello/a/", O_CREAT|O_DIR);
    check(ret);
    ret = filesys[2]->ops->lookup(filesys[2], "hello/a/", O_DIR);
    check(ret);
    ret = filesys[2]->ops->lookup(filesys[2], "hello/a/", 0);
    check(ret);
#endif

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

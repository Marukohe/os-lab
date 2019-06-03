#include <common.h>
#include <vfs.h>
#include <klib.h>

extern fsops_t fs_ops;
extern device_t *devices[8];
extern task_t *cputask[25];
extern task_t *current_task[4];
#define current (current_task[_cpu()])

mt_t *mtt;
#define L3DEBUG

#define FILESYSTEM(_) \
    _(0, filesystem_t, "procfs", 0, &fs_ops, NULL) \
    _(1, filesystem_t, "devfs",  1, &fs_ops, NULL) \
    _(2, filesystem_t, "blkfs",  2, &fs_ops, devices[1])

#define FS_CNT(...) + 1
filesystem_t *filesys[5];

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
    Logg("pos: 0x%x", node->pos);
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

void mttinit(){
    mtt = (mt_t *)pmm->alloc(sizeof(mt_t));
    mtt->cnt = 3;
    mtt->id[0] = 0;
    strcpy(mtt->rootname[0], "proc");
    mtt->id[1] = 1;
    strcpy(mtt->rootname[1], "dev");
    mtt->id[2] = 2;
    strcpy(mtt->rootname[2], "/");
}

extern int filesysdecode(char *ret, const char *path);
void vfstest(){
#ifdef L3DEBUG
    const char *path = pmm->alloc(100);
    path = "/hello";
    vfs->mkdir(path);
    inode_t *ret = filesys[2]->ops->lookup(filesys[2], path, 7|O_DIR);
    check(ret);
    vfs->mkdir("/hello/a");
    ret = filesys[2]->ops->lookup(filesys[2], "/hello/a", 7|O_DIR);
    check(ret);
    vfs->rmdir("/hello/a");
    ret = filesys[2]->ops->lookup(filesys[2], "/hello/a", 7|O_DIR);
    check(ret);
    vfs->mkdir("/hello/a");
    ret = filesys[2]->ops->lookup(filesys[2], "/hello/a", 7|O_DIR);
    check(ret);
    vfs->mkdir("/hello/a/c");
    vfs->rmdir("/hello/a/c");
    vfs->rmdir("/hello/a/b");
    /*ret = filesys[2]->ops->lookup(filesys[2], "/hello/a/c.txt", 7|O_CREAT);*/
    /*check(ret);*/
    /*ret = filesys[2]->ops->lookup(filesys[2], "/hello/a/c.txt", 7);*/
    /*check(ret);*/
#endif
}

void init(){
    /*TODO();*/
    FILESYSTEM(FSCREATE);
    FILESYSTEM(FSINIT);
    mttinit();

    /*vfstest();*/
    /*char *ret = pmm->alloc(100);*/
    /*printf("%d\n", filesysdecode(ret, "/proc/hello/ba/"));*/
    /*printf("%d\n", filesysdecode(ret, "/dev/hello"));*/
    /*printf("%d\n", filesysdecode(ret, "/hello"));*/
    /*pmm->free(ret);*/

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
    /*TODO();*/
    filesys[2]->sinode->ops->mkdir(path);
    return 0;
}

int rmdir(const char *path){
    /*TODO();*/
    filesys[2]->sinode->ops->rmdir(path);
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
    /*TODO();*/
    char *ret = pmm->alloc(100);
    int id = filesysdecode(ret, path);
    file_t *fd = (file_t *)pmm->alloc(sizeof(file_t));
    fd->refcnt = 0;
    inode_t *tmp = filesys[id]->ops->lookup(filesys[id], ret, flags);
    if(tmp == NULL){
        printf("open failed\n");
        pmm->free(ret);
        pmm->free(fd);
        return -1;
    }
    fd->inode = tmp;
    fd->path = ret;
    fd->offset = 0;
    int retfd = 0;
    for(int i = 0; i < current->fdcnt; i++){
        if(current->fdused[i] == 0){
            retfd = i;
            break;
        }
    }
    if(retfd == 0){
        retfd = current->fdcnt++;
    }
    current->fildes[retfd] = fd;
    current->fdused[retfd] = 1;

    return retfd;
}

ssize_t read(int fd, void *buf, size_t nbyte){
    /*TODO();*/
    ssize_t ret;
    ret = current->fildes[fd]->inode->ops->read(current->fildes[id]->inode, buf, nbyte);
    return ret;
}

ssize_t write(int fd, void *buf, size_t nbyte){
    /*TODO();*/
    ssize_t ret;
    ret = current->fildes[fd]->inode->ops->write(current->fildes[fd]->inode, buf, nbyte);
    return 0;
}

off_t lseek(int fd, off_t offset, int whence){
    TODO();
    return 0;
}

int close(int fd){
    /*TODO();*/
    pmm->free(current->fildes[fd]->inode);
    pmm->free(current->fildes[fd]->path);
    pmm->free(current->fildes[fd]);
    current->fdused[fd] = 0;
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

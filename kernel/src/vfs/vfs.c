#include <common.h>
#include <vfs.h>
#include <klib.h>
#include <devices.h>

extern fsops_t fs_ops;
extern device_t *devices[8];
extern task_t *cputask[25];
extern task_t *current_task[4];
extern spinlock_t vfslock;
#define current (current_task[_cpu()])

mt_t *mtt;
#define L3DEBUG

#define FILESYSTEM(_) \
    _(0, filesystem_t, "procfs", 0, &fs_ops, NULL) \
    _(1, filesystem_t, "devfs",  1, &fs_ops, NULL) \
    _(2, filesystem_t, "blkfs",  2, &fs_ops, devices[1])

#define FS_CNT(...) + 1
filesystem_t *filesys[FSNUM];

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
    strcpy(mtt->rootname[0], "/proc");
    mtt->id[1] = 1;
    strcpy(mtt->rootname[1], "/dev");
    mtt->id[2] = 2;
    strcpy(mtt->rootname[2], "/");
    for(int i = 0; i < 3; i++)
        mtt->used[i] = 1;
}

extern int filesysdecode(char *ret, const char *path);
extern char *splitpath(char *path, int offset);
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

    void *buf = pmm->alloc(BLOCKSIZE);
    filesys[2]->dev->ops->read(filesys[2]->dev, filesys[2]->sinode->offset[0], buf, BLOCKSIZE);
    memset(buf, 0, BLOCKSIZE);
    dir_t *dir = (dir_t *)buf;

    dir->offset[0] = filesys[2]->sinode->pos;
    strcpy(dir->name[0] = ".");
    dir->used[0] = 1;
    dir->offset[1] = filesys[2]->sinode->pos;
    strcpy(dir->name[1] = "..");
    dir->used[1] = 1;

    dir->offset[2] = filesys[0]->sinode->pos;
    strcpy(dir->name[2], "proc");
    dir->used[2] = 1;

    dir->offset[3] = filesys[1]->sinode->pos;
    strcpy(dir->name[3], "dev");
    dir->used[3] = 1;


    dir->cnt = 4;


    filesys[2]->cntinode = 4;
    filesys[2]->used[0] = filesys[2]->used[1] = filesys[2]->used[2] = filesys[2]->used[3] = 1;
    filesys[2]->ioffset[2] = filesys[0]->sinode->pos;
    filesys[2]->ioffset[3] = filesys[1]->sinode->pos;

    filesys[2]->dev->ops->write(filesys[2]->dev, filesys[2]->sinode->offset[0], (void *)dir, BLOCKSIZE);

    filesys[2]->ops->lookup(filesys[2], "/Documents", 7|O_CREAT|O_DIR);

    inode_t *node = filesys[2]->ops->lookup(filesys[2], "/Documents/lyrics.txt", 7|O_CREAT);

    memset(buf, 0, BLOCKSIZE);
    sprintf(buf, "Never say never!");
    node->filesize = strlen(buf);
    filesys[2]->dev->ops->write(filesys[2]->dev, node->offset[0],(void *)buf, BLOCKSIZE);
    filesys[2]->dev->ops->write(filesys[2]->dev, node->pos, (void *)node, sizeof(inode_t));

    memset(buf, 0, BLOCKSIZE);
    dir = (dir_t *)buf;
    for(int i = 0; i < 8; i++)
        dir->used[i] = 1;
    dir->cnt = 8;
    strcpy(dir->name[0], "ramdisk0");
    strcpy(dir->name[1], "ramdisk1");
    strcpy(dir->name[2], "input");
    strcpy(dir->name[3], "fb");
    strcpy(dir->name[4], "tty1");
    strcpy(dir->name[5], "tty2");
    strcpy(dir->name[6], "tty3");
    strcpy(dir->name[7], "tty4");
    filesys[2]->dev->ops->write(filesys[2]->dev, filesys[1]->sinode->offset[0], (void *)dir, BLOCKSIZE);

        /*vfstest();*/
        /*char *ret = pmm->alloc(100);*/
        /*printf("%d\n", filesysdecode(ret, "/proc/hello/ba/"));*/
        /*printf("%d\n", filesysdecode(ret, "/dev/hello"));*/
        /*printf("%d\n", filesysdecode(ret, "/hello"));*/
        /*pmm->free(ret);*/

        return;
}

int access(const char *path, int mode){
    /*kmt->spin_lock(&vfslock);*/
    /*TODO();*/
    char *decode = (char *)pmm->alloc(100);
    int id = filesysdecode(decode, path);
    inode_t *node;
    switch(mode){
        case R_OK: node = filesys[id]->ops->lookup(filesys[id], decode, RABLE); break;
        case W_OK: node = filesys[id]->ops->lookup(filesys[id], decode, WABLE); break;
        case X_OK: node = filesys[id]->ops->lookup(filesys[id], decode, XABLE); break;
        case F_OK: node = filesys[id]->ops->lookup(filesys[id], decode, 7); break;
        case D_OK: node = filesys[id]->ops->lookup(filesys[id], decode, 7 | O_DIR); break;
        default: printf("Not such mode in access.\n"); return -1;
    }
    pmm->free(decode);
    if(node == NULL){
        /*kmt->spin_unlock(&vfslock);*/
        return -1;
    }
    /*pmm->free(node);*/
    /*kmt->spin_unlock(&vfslock);*/
    return 0;
}

int mount(const char *path, filesystem_t *fs){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    int id = 0;
    for(int i = 0; i < mtt->cnt; i++){
        if(mtt->used[i] == 0){
            id = i;
            break;
        }
    }
    if(id == 0){
        id = mtt->cnt;
        mtt->cnt++;
    }
    strcpy(mtt->rootname[id], path);
    filesys[id] = filesys_create(sizeof(filesystem_t), path, id, &fs_ops, fs->dev);
    filesys[id]->ops->init(filesys[id], path, filesys[id]->dev);

    kmt->spin_unlock(&vfslock);
    return 0;
}

int unmount(const char *path){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    int id = -1;
    for(int i = 0; i < mtt->cnt; i++){
        if(strcmp(mtt->rootname[i], path)){
            id = i;
            break;
        }
    }
    if(id == -1 || mtt->used[id] == 0){
        printf("Not such mnt filesystem\n");
        kmt->spin_unlock(&vfslock);
        return -1;
    }
    pmm->free(filesys[id]->sinode->ptr);
    pmm->free(filesys[id]->sinode);
    pmm->free(filesys[id]);
    mtt->used[id] = 0;
    kmt->spin_unlock(&vfslock);
    return 0;
}

int mkdir(const char *path){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    int ret = filesys[2]->sinode->ops->mkdir(path);
    kmt->spin_unlock(&vfslock);
    return ret;

}

int rmdir(const char *path){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    int ret = filesys[2]->sinode->ops->rmdir(path);
    kmt->spin_unlock(&vfslock);
    return ret;
}

int link(const char *oldpath, const char *newpath){
    /*TODO();*/
    /*Logw("I'm here.");*/
    kmt->spin_lock(&vfslock);
    /*Logw("I'm here.");*/
    char *retold = pmm->alloc(128);
    char *retnew = pmm->alloc(128);
    int idold = filesysdecode(retold, oldpath);
    int idnew = filesysdecode(retnew, newpath);
    int r1 = vfs->access(newpath, F_OK);
    if(r1 == 0){
        pmm->free(retold);
        pmm->free(retnew);
        printf("File exists.\n");
        /*Logw("I'm here.");*/
        kmt->spin_unlock(&vfslock);
        return -1;
    }
    inode_t *new;
    int offset = strlen(retnew);
    char *fapath = splitpath(retnew, offset);
    char *cp;
    if(fapath == NULL){
        new = filesys[idnew]->sinode;
        cp = retnew + 1;
    }else{
        new = filesys[idnew]->ops->lookup(filesys[idnew], fapath, 7|O_DIR);
        cp = retnew + strlen(fapath) + 1;
    }

    inode_t *node = filesys[idold]->ops->lookup(filesys[idold], retold, 7);
    node->refcnt += 1;
    filesys[2]->dev->ops->write(filesys[2]->dev, node->pos, (void *)node, sizeof(inode_t));
    if(node == NULL){
        pmm->free(retnew);
        pmm->free(retold);
        printf("Not such file.\n");
        /*Logw("I'm here.");*/
        kmt->spin_unlock(&vfslock);
        return -1;
    }
    void *buf = pmm->alloc(BLOCKSIZE);
    filesys[idnew]->dev->ops->read(filesys[idnew]->dev, new->offset[0], buf, BLOCKSIZE);
    dir_t *dir = (dir_t *)buf;
    /*int tmpcnt = 0;*/
    /*for(int i = 0; i < dir->cnt; i++){*/
        /*if(dir->used[i] == 0){*/
            /*tmpcnt = i;*/
            /*break;*/
        /*}*/
        /*tmpcnt++;*/
    /*}*/
    /*if(tmpcnt == dir->cnt){*/
    strcpy(dir->name[dir->cnt], cp);
    dir->used[dir->cnt] = 1;
    dir->offset[dir->cnt++] = node->pos;
    /*}*/
    filesys[idnew]->dev->ops->write(filesys[idnew]->dev, new->offset[0], (void *)dir, BLOCKSIZE);
    /*Logw("I'm here");*/
    kmt->spin_unlock(&vfslock);
    return 0;
}

int unlink(const char *path){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    int ret = filesys[2]->sinode->ops->rmdir(path);
    kmt->spin_unlock(&vfslock);
    return ret;

    /*
    char *ret = pmm->alloc(128);
    int id = filesysdecode(ret, path);
    int offset  = strlen(path);
    char *fpath = splitpath(path, offset);
    */
}

int open(const char *path, int flags){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    char *ret = pmm->alloc(100);
    int id = filesysdecode(ret, path);
    /*Logb("%d\n", id);*/
    file_t *fd = (file_t *)pmm->alloc(sizeof(file_t));
    fd->refcnt = 0;
    inode_t *tmp = filesys[id]->ops->lookup(filesys[id], ret, flags);
    if(id != 1 && tmp == NULL){
        printf("open failed\n");
        pmm->free(ret);
        pmm->free(fd);
        kmt->spin_unlock(&vfslock);
        return -1;
    }

    fd->inode = tmp;
    fd->path = ret;
    fd->offset = 0;
    int retfd = -1;
    for(int i = 0; i < current->fdcnt; i++){
        if(current->fdused[i] == 0){
            retfd = i;
            break;
        }
    }
    if(retfd == -1){
        retfd = current->fdcnt++;
    }
    current->fildes[retfd] = fd;
    current->fdused[retfd] = 1;
    kmt->spin_unlock(&vfslock);

    return retfd;
}

ssize_t read(int fd, void *buf, size_t nbyte){
    /*TODO();*/
    if(current->fildes[fd]->inode == NULL){
        char *name = (char *)pmm->alloc(128);
        for(int i = 1; i < strlen(current->fildes[fd]->path); i++)
            name[i - 1] = current->fildes[fd]->path[i];
        /*printf("devname: %s\n", name);*/
        device_t *dev = dev_lookup(name);
        pmm->free(name);
        int nread = dev->ops->read(dev, 0, buf, nbyte);
        current->fildes[fd]->offset += nread;
        return nread;
    }
    kmt->spin_lock(&vfslock);
    ssize_t ret;
    ret = current->fildes[fd]->inode->ops->read(current->fildes[fd], buf, nbyte);
    kmt->spin_unlock(&vfslock);
    return ret;
}

ssize_t write(int fd, void *buf, size_t nbyte){
    /*TODO();*/
    if(current->fildes[fd]->inode == NULL){
        char *name = (char *)pmm->alloc(128);
        for(int i = 1; i < strlen(current->fildes[fd]->path); i++)
            name[i - 1] = current->fildes[fd]->path[i];
        /*printf("devname: %s\n", name);*/
        device_t *dev = dev_lookup(name);
        pmm->free(name);
        /*assert(0);*/
        /*Logw("I'm here");*/
        int nwrite = dev->ops->write(dev, 0, buf, nbyte);
        current->fildes[fd]->offset += nwrite;
        return nwrite;
    }
    kmt->spin_lock(&vfslock);
    ssize_t ret;
    ret = current->fildes[fd]->inode->ops->write(current->fildes[fd], buf, nbyte);
    kmt->spin_unlock(&vfslock);
    return ret;
}

off_t lseek(int fd, off_t offset, int whence){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    off_t ret;
    ret = current->fildes[fd]->inode->ops->lseek(current->fildes[fd], offset, whence);
    kmt->spin_unlock(&vfslock);
    return ret;
}

int close(int fd){
    /*TODO();*/
    kmt->spin_lock(&vfslock);
    if(current->fildes[fd]->inode != NULL)
        pmm->free(current->fildes[fd]->inode);
    if(current->fildes[fd]->path != NULL)
        pmm->free(current->fildes[fd]->path);
    pmm->free(current->fildes[fd]);
    current->fdused[fd] = 0;
    kmt->spin_unlock(&vfslock);
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

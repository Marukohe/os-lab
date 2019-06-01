#include <common.h>
#include <vfs.h>
#include <devices.h>
#include <klib.h>
extern void TODO();
extern inodeops_t inode_ops;
extern filesystem_t *filesys[3];
#define INODESIZE (sizeof(inode_t))
#define BLOCKSIZE 4096
#define DIRSIZE 512
static int diskoffset;

void fsinit(struct filesystem *fs, const char *name, device_t *dev){
    /*TODO();*/
    /*fs = pmm->alloc(sizeof(filesystem_t));*/
    /*fs->name = name;*/
    void *data = pmm->alloc(sizeof(device_t));
    fs->sinode = pmm->alloc(sizeof(inode_t));
    fs->sinode->refcnt = 0;
    fs->sinode->flags = 0;// TODO()
    fd->sinode->offset[0] = diskoffset;
    diskoffset += BLOCKSIZE;
    fs->sinode->ptr = data;
    fs->sinode->fs = fs;
    fs->sinode->ops = &inode_ops;
    return;
}

static void getpath(char *get, const char *path, int offset){
    const char *cp;
    cp = path + offset;
    int len = strlen(path);
    while(*cp != '/' && offset < len){
        *get++ = *cp++;
        offset++;
    }
    *get = '\0';
}

inode_t *lookup(struct filesystem *fs, const char *path, int flags){
    /*TODO();*/
    inode_t *ret = fd->sinode;
    int offset = 0;
    char *get = (char *)pmm->alloc(sizeof(DIRSIZE));
    void *tmpnode = pmm->alloc(INODESIZE);
    while(offset < strlen(path)){
        void *buf = pmm->alloc(BLOCKSZIZE);
        filesys[2]->dev->ops->read(filesys[2]->dev, ret->offset[0], buf, BLOCKSIZE);
        dir_t *dir = (dir_t *)buf;

        getpath(get, path, offset);
        /*printf("%s\n", get);*/
        offset += strlen(get) + 1;

        for(int i = 0; i < dir->cnt; i++){
            if(strcmp(dir->name[i], get) == 0){
                filesys[2]->dev->ops->read(filesys[2]->dev, dir->offset[i], tmpnode, INODESIZE);
                ret = (inode_t *)tmpnode;
                break;
            }
        }
    }
    pmm->free(get);

    return ret;
}

int fsclose(inode_t *inode){
    TODO();
    return 0;
}

fsops_t fs_ops = {
    .init = fsinit,
    .lookup = lookup,
    .close = fsclose,
};

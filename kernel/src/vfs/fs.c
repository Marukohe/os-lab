#include <common.h>
#include <vfs.h>
#include <devices.h>
#include <klib.h>
extern void TODO();
extern inodeops_t inode_ops;
#define OFFSET (sizeof(inode_t))
#define BLOCKSIZE 4096
#define DIRSIZE 512

void fsinit(struct filesystem *fs, const char *name, device_t *dev){
    /*TODO();*/
    /*fs = pmm->alloc(sizeof(filesystem_t));*/
    /*fs->name = name;*/
    void *data = pmm->alloc(sizeof(device_t));
    fs->sinode = pmm->alloc(sizeof(inode_t));
    fs->sinode->refcnt = 0;
    fs->sinode->flags = 0;// TODO()
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
    int offset = 0;
    char *get = (char *)pmm->alloc(sizeof(DIRSIZE));
    while(offset < strlen(path)){
        getpath(get, path, offset);
        printf("%s\n", get);
        offset += strlen(get) + 1;
    }
    pmm->free(get);

    return NULL;
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

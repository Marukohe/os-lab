#include <common.h>
#include <vfs.h>
extern void TODO();
extern inodeops_t inode_ops;

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

inode_t *lookup(struct filesystem *fs, const char *path, int flags){
    TODO();
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

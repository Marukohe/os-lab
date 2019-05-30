#include <common.h>
#include <vfs.h>
extern void TODO();

void fsinit(struct filesystem *fs, const char *name, device_t *dev){
    /*TODO();*/
    fs = pmm->alloc(sizeof(filesystem_t));
    fs->name = name;
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
}

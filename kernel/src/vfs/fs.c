#include <common.h>
#include <vfs.h>
extern void TODO();

void init(struct filesystem *fs, const char *name, device_t *dev){
    TODO();
    return;
}

inode_t *lookup(struct filesystem *fs, const char *path, int flags){
    TODO();
    return NULL;
}

int close(inode_t *inode){
    TODO();
    return 0;
}

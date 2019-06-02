#include <common.h>
#include <vfs.h>
#include <klib.h>
extern void TODO();
extern static void getpath(char *get, const char *path, int offset);
extern mt_t *mtt;
extern filesystem_t *filesys[5];
int filesysdecode(const char *path){
    int offset = 1;
    char *get = (char *)pmm->alloc(100);
    getpath(get, path, offset);
    int ret = 0;
    for(int i = 0; i < mtt->cnt; i++){
        if(strcmp(get, mtt->rootname[i]) == 0){
            return i;
        }
        ret++;
    }
    return ret;
}

int inodeopen(file_t *file, int flags){
    TODO();
    return 0;
}

int inodeclose(file_t *file){
    TODO();
    return 0;
}

ssize_t inoderead(file_t *file, char *buf, size_t size){
    TODO();
    return 0;
}

ssize_t inodewrite(file_t *file, const char *buf, size_t size){
    TODO();
    return 0;
}

off_t inodelseek(file_t *file, off_t offset, int whence){
    TODO();
    return 0;
}

int inodemkdir(const char *name){
    TODO();
    return 0;
}

int inodermdir(const char *name){
    TODO();
    return 0;
}

int inodelink(const char *name, inode_t *inode){
    TODO();
    return 0;
}

int inodeunlink(const char *name){
    TODO();
    return 0;
}

inodeops_t inode_ops = {
    .open = inodeopen,
    .close = inodeclose,
    .read = inoderead,
    .write = inodewrite,
    .lseek = inodelseek,
    .mkdir = inodemkdir,
    .rmdir = inodermdir,
    .link = inodelink,
    .unlink = inodeunlink,
};

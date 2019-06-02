#include <common.h>
#include <vfs.h>
#include <klib.h>
#define NAMELEN 100
extern void TODO();
extern void getpath(char *get, const char *path, int offset);
extern mt_t *mtt;
extern filesystem_t *filesys[5];
int filesysdecode(char *ret, const char *path){
    int offset = 1;
    char *get = (char *)pmm->alloc(NAMELEN);
    getpath(get, path, offset);
    offset += strlen(get);
    const char *cp;
    cp = path + offset;
    strcpy(ret, cp);
    /*printf("%s\n", ret);*/
    for(int i = 0; i < mtt->cnt; i++){
        if(strcmp(get, mtt->rootname[i]) == 0){
            pmm->free(get);
            return i;
        }
    }
    pmm->free(get);
    return 2;
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
    /*TODO();*/
    char *path = pmm->alloc(NAMELEN);
    int id = filesysdecode(path, name);

    filesys[id]->ops->lookup(filesys[id], path, 7|O_CREAT|O_DIR);
    pmm->free(path);
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

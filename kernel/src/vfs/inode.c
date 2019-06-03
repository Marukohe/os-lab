#include <common.h>
#include <vfs.h>
#include <klib.h>
#include <devices.h>
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
    strcpy(ret, path);
    /*printf("%s\n", ret);*/
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
    /*TODO();*/
    char *red = (char *)pmm->alloc(BLOCKSIZE);
    filesys[2]->dev->ops->read(filesys[2]->dev, file->inode->offset[0], red, BLOCKSIZE);
    char *cp;
    cp = buf + file->offset;
    strcnpy(buf, cp, size);
    file->offset += size;
    pmm->free(red);
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
    printf("path decode: %s\n", path);

    inode_t *dummy = filesys[id]->ops->lookup(filesys[id], path, 7|O_CREAT|O_DIR);
    pmm->free(path);
    pmm->free(dummy);
    return 0;
}

// 寻找上一级路劲
char *splitpath(char *path, int offset){
    int t = 0;
    /*Logp("%s %d", path, offset);*/
    char *ret = pmm->alloc(NAMELEN);
    for(int i = offset - 1; i >= 0; i--){
        /*Logp("hello %c", path[i]);*/
        if(path[i] == '/'){
            t = i;
            break;
        }
    }
    if(t == 0){
        pmm->free(ret);
        return NULL;
    }
    strncpy(ret, path, t);
    /*Logw("ret : %s", ret);*/
    return ret;
}

int inodermdir(const char *name){
    /*TODO();*/

    char *sonpath = pmm->alloc(NAMELEN);
    int id = filesysdecode(sonpath, name);
    int offset = strlen(sonpath);
    /*Logw("%s", sonpath);*/
    char *fapath = splitpath(sonpath, offset);
    /*Logw("%s", fapath);*/
    inode_t *fa;
    if(fapath == NULL){
        /*Logw("hello?");*/
        fa = filesys[id]->sinode;
    }else{
        fa = filesys[id]->ops->lookup(filesys[id], fapath, 7|O_DIR);
    }
    inode_t *son = filesys[id]->ops->lookup(filesys[id], sonpath, 7|O_DIR);
    if(son == NULL){
        printf("ERROR! Not such dic\n");
        pmm->free(sonpath);
        pmm->free(fapath);
        return -1;
        /*pmm->free(fa);*/
    }
    /*Logw("fa pos: %x, fa->offset: %x", fa->pos, fa->offset[0]);*/
    //更新fa的目录项
    void *buf = pmm->alloc(BLOCKSIZE);
    filesys[2]->dev->ops->read(filesys[2]->dev, son->offset[0], buf, BLOCKSIZE);
    dir_t *dir = (dir_t *)buf;
    int cntsonnode = 0;
    for(int i = 0; i < dir->cnt; i++){
        if(dir->used[i])
            cntsonnode++;
    }
    if(cntsonnode > 0){
        printf("rmdir failed. This is not an empty dic\n");
        pmm->free(buf);
        pmm->free(sonpath);
        pmm->free(fapath);
        pmm->free(son);
        /*pmm->free(fa);*/
        return -1;
    }

    memset(buf, 0, BLOCKSIZE);
    filesys[2]->dev->ops->read(filesys[2]->dev, fa->offset[0], buf, BLOCKSIZE);
    dir = (dir_t *)buf;
    for(int i = 0; i < dir->cnt; i++){
        if(dir->used[i] == 1 && dir->offset[i] == son->pos){
            printf("rmdir successfully!\n");
            dir->used[i] = 0;
            filesys[2]->dev->ops->write(filesys[2]->dev, fa->offset[0], (void *)dir, BLOCKSIZE);
            break;
        }
    }
    for(int i = 0; i < filesys[id]->cntinode; i++){
        if(filesys[id]->used[i] && filesys[id]->ioffset[i] == son->pos){
            filesys[id]->used[i] = 0;
            break;
        }
    }
    pmm->free(buf);
    pmm->free(sonpath);
    pmm->free(fapath);
    pmm->free(son);
    /*pmm->free(fa);*/

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

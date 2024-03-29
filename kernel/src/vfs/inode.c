#include <common.h>
#include <vfs.h>
#include <klib.h>
#include <devices.h>
#define NAMELEN 100
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)
extern void TODO();
extern void getpath(char *get, const char *path, int offset);
extern mt_t *mtt;
extern filesystem_t *filesys[FSNUM];
int filesysdecode(char *ret, const char *path){
    int offset = 1;
    char *get = (char *)pmm->alloc(NAMELEN);
    getpath(get, path, offset);
    char text[128];
    sprintf(text, "/%s", get);
    offset += strlen(get);
    const char *cp;
    cp = path + offset;
    strcpy(ret, cp);
    /*printf("%s\n", ret);*/
    for(int i = 0; i < mtt->cnt; i++){
        if(strcmp(text, mtt->rootname[i]) == 0){
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
    if(file->inode == NULL){
        printf("file inode is NULL\n");
        return -1;
        device_t *dev = dev_lookup(file->path);
        int nread = dev->ops->read(dev, 0, buf, size);
        file->offset += nread;
        return nread;
    }
    char *red = (char *)pmm->alloc(BLOCKSIZE);
    filesys[2]->dev->ops->read(filesys[2]->dev, file->inode->offset[0], red, BLOCKSIZE);
    char *cp;
    cp = red + file->offset;
    int cpsize = min(size, file->inode->filesize - file->offset);
    if(cpsize == 0){
        printf("offset at the end of the file\n");
        return 0;
    }
    strncpy(buf, cp, cpsize);
    file->offset += cpsize;
    pmm->free(red);
    return cpsize;
}

ssize_t inodewrite(file_t *file, const char *buf, size_t size){
    /*TODO();*/
    if(file->inode == NULL){
        printf("file inode is NULL\n");
        return -1;
        device_t *dev = dev_lookup(file->path);
        int nwrite = dev->ops->write(dev, 0, buf, size);
        file->offset += nwrite;
        return nwrite;
    }
    char *red = (char *)pmm->alloc(BLOCKSIZE);
    /*char *wback = (char *)pmm->alloc(BLOCKSIZE);*/
    filesys[2]->dev->ops->read(filesys[2]->dev, file->inode->offset[0], red, BLOCKSIZE);
    for(int i = 0; i < strlen(buf); i++){
        red[file->offset++] = buf[i];
    }
    for(int i = strlen(buf); i < size; i++){
        red[file->offset++] = '\0';
    }
    file->inode->filesize = max(file->inode->filesize, file->offset);
    /*Logw("write: %s, filesize: %d", red, file->inode->filesize);*/

    filesys[2]->dev->ops->write(filesys[2]->dev, file->inode->offset[0], red, BLOCKSIZE);
    filesys[2]->dev->ops->write(filesys[2]->dev, file->inode->pos, (void*)(file->inode), sizeof(inode_t));
    pmm->free(red);
    return size;
}

off_t inodelseek(file_t *file, off_t offset, int whence){
    /*TODO();*/
    switch(whence){
        case SEEKSET: file->offset = offset; break;
        case SEEKCUR: file->offset += offset; break;
        case SEEKEND: file->offset = file->inode->filesize; break;
        default: printf("Not such mode in lseek"); return -1;
    }
    return file->offset;
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

int inodemkdir(const char *name){
    /*TODO();*/
    char *path = pmm->alloc(NAMELEN);
    int id = filesysdecode(path, name);
    /*printf("path decode: %s\n", path);*/
    int offset = strlen(path);
    char *fapath = splitpath(path, offset);
    /*Logg("fapath: %s", fapath);*/
    inode_t *ret = filesys[id]->ops->lookup(filesys[id], fapath, 7|O_DIR);
    if(ret == NULL && fapath != NULL){
        pmm->free(path);
        pmm->free(fapath);
        return -1;
    }

    inode_t *dummy = filesys[id]->ops->lookup(filesys[id], path, 7|O_CREAT|O_DIR);
    pmm->free(path);
    pmm->free(dummy);
    return 0;
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
    int flag = 0; //目录
    if(son == NULL){
        son = filesys[id]->ops->lookup(filesys[id], sonpath, 7);
        flag = 1; //文件
        if(son == NULL){
            printf("ERROR! Not such dic of file.\n");
            pmm->free(sonpath);
            /*pmm->free(fapath);*/
            return -1;
            /*pmm->free(fa);*/
        }
    }
    /*Logw("fa pos: %x, fa->offset: %x", fa->pos, fa->offset[0]);*/
    //更新fa的目录项
    void *buf = pmm->alloc(BLOCKSIZE);
    if(flag == 0){
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
        /*pmm->free(fapath);*/
        pmm->free(son);
        /*pmm->free(fa);*/
        return -1;
        }
    }
    son->refcnt -= 1;
    filesys[2]->dev->ops->write(filesys[2]->dev, son->pos, (void *)son, sizeof(inode_t));

    memset(buf, 0, BLOCKSIZE);
    filesys[2]->dev->ops->read(filesys[2]->dev, fa->offset[0], buf, BLOCKSIZE);
    dir_t *dir = (dir_t *)buf;
    char *pathdecode;
    int tmppath = 0;
    /*Logw("sonpath %s\n", sonpath);*/
    for(int i = strlen(sonpath) - 1; i >= 0; i--){
        if(sonpath[i] == '/'){
            tmppath = i;
            break;
        }
    }
    pathdecode = sonpath + tmppath + 1;
    /*Logw("pathcode: %s", pathdecode);*/
    for(int i = 0; i < dir->cnt; i++){
        if(dir->used[i] == 1 && strcmp(dir->name[i], pathdecode) == 0){
            printf("rmdir successfully!\n");
            dir->used[i] = 0;
            filesys[2]->dev->ops->write(filesys[2]->dev, fa->offset[0], (void *)dir, BLOCKSIZE);
            break;
        }
    }

    if(son->refcnt == 0){
        for(int i = 0; i < filesys[id]->cntinode; i++){
            if(filesys[id]->used[i] && filesys[id]->ioffset[i] == son->pos){
                filesys[id]->used[i] = 0;
                break;
            }
        }
    }

    pmm->free(buf);
    pmm->free(sonpath);
    /*pmm->free(fapath);*/
    /*pmm->free(son);*/
    /*pmm->free(fa);*/

    return 0;
}

int inodelink(const char *name, inode_t *inode){
    /*TODO();*/
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

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
static int diskoffset = (4 << 14);
static int inodeoffset = 0;

void fsinit(struct filesystem *fs, const char *name, device_t *dev){
    /*TODO();*/
    //offsetblk写入磁盘
    void *data = pmm->alloc(sizeof(device_t));
    data = (void *)dev;
    fs->sinode = pmm->alloc(sizeof(inode_t));
    fs->sinode->refcnt = 0;
    fs->sinode->flags = 23;
    fs->sinode->isdir = 1;
    fs->sinode->offset[0] = diskoffset;
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

static void inodecreat(inode_t *inode, int flags, int is_dir, filesystem_t *fs, device_t * dev){
    inode->refcnt = 0;
    inode->flags = flags;
    inode->is_dir = is_dir;
    inode->offset[0] = diskoffset;
    diskoffset += BLOCKSIZE;
    inode->ptr = (void *)dev;
    inode->fs = fs;
    inode->ops = &inode_ops;
    char *tmp = pmm->alloc(BLOCKSIZE);
    memset(tmp, 0, BLOCKSIZE);
    filesys[2]->dev->ops->write(filesys[2]->dev, inode->offset[0], (void *)tmp, BLOCKSIZE);
    pmm->free(tmp);
}

inode_t *lookup(struct filesystem *fs, const char *path, int flags){
    /*TODO();*/
    inode_t *ret = fs->sinode;
    int offset = 0;
    char *get = (char *)pmm->alloc(sizeof(DIRSIZE));
    void *tmpnode = pmm->alloc(INODESIZE);
    while(offset < strlen(path)){
        //获取目录block
        void *buf = pmm->alloc(BLOCKSIZE);
        filesys[2]->dev->ops->read(filesys[2]->dev, ret->offset[0], buf, BLOCKSIZE);
        dir_t *dir = (dir_t *)buf;

        getpath(get, path, offset);
        /*printf("%s\n", get);*/
        offset += strlen(get) + 1;

        uint8_t inodefind = 0;
        for(int i = 0; i < dir->cnt; i++){
            if(dir->used[1] == 0) continue;
            if(strcmp(dir->name[i], get) == 0){
                //获取目录块中记录的inode
                filesys[2]->dev->ops->read(filesys[2]->dev, dir->offset[i], tmpnode, INODESIZE);
                ret = (inode_t *)tmpnode;
                if((ret->flags & flags) == 0){
                    printf("Permission denied\n");
                    pmm->free(tmpnode);
                    pmm->free(get);
                    return NULL;
                }
                if(offset == strlen(path)){
                    if((!ret->is_dir && (flags & O_DIR)) || (ret->is_dir && !(flags & O_DIR))){
                        printf("Dir or text not found\n");
                        pmm->alloc(tmpnode);
                        pmm->free(get);
                        return NULL;
                    }
                }
                inodefind = 1;
                break;
            }
        }
        if(!inodefind){
            if(offset == strlen(path)){
                if(flags & O_CREAT){
                    //创建一个inode写入磁盘
                    /*if(flags & O_DIR){*/
                    inode_t *inodect = pmm->alloc(sizeof(inode_t));
                    inodecreat(inodect, flags, flags & O_DIR, fs, fs->dev);
                    filesys[2]->dev->ops->write(filesys[2]->dev, inodeoffset, (void *)inodect, INODESIZE);
                    ret = inodect;
                    /*}else{*/
                        /*inode_t *inodect = pmm->alloc(sizeof(inode_t));*/
                        /*inodecreat(inodect, flags, O_DIR, fs, fs->dev);*/
                        /*filesys[2]->dev->ops->write(filesys[2]->dev, inodeoffset, (void *)inodect, INODESIZE);*/
                        /*ret = inodect;*/
                    /*}*/
                }else{
                    printf("Nothing to create\n");
                    pmm->free(get);
                    pmm->free(tmpnode);
                    return NULL;
                }
            }
        }
    }
    pmm->free(get);
    pmm->free(tmpnode);

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

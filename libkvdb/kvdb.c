#include "kvdb.h"

ssize_t read_line(int fd, void *ret, ssize_t maxlen){
    ssize_t n, rc;
    char c, *ptr;
    ptr = ret;
    for(n = 1; n < maxlen; n++){
        if((rc = read(fd, &c, 1)) == 1){
            if(c == '\n')
                break;
            *ptr++ = c;
        }else if(rc == 0){
            *ptr = 0;
            return n - 1;
        }else{
            panic("readline failed");
            return n - 1;
        }
    }
    *ptr = 0;
    return n;
}

//====================================================
//如果文件不存在，则创建，如果文件存在，则在已有数据库的基础上进行操作。
//====================================================

int kvdb_open(kvdb_t * db, const char *filename){
    int ret = open(filename, O_CREAT | O_RDWR, 0666);
    printf("%d\n", ret);
    db->fd = ret;
    if(ret < 0){
        panic("open file failed");
        return ret;
    }
    return 0;
}

//====================================================
//关闭数据库并释放相关资源。
//====================================================

int kvdb_close(kvdb_t *db){
    if(db->fd < 0){
        panic("close file failed, a fd < 0");
        return -1;
    }
    int ret = close(db->fd);
    if(ret < 0){
        panic("close file failed");
        return -1;
    }
    return 0;
}

//====================================================
//建立key到value的映射
//在kvdb_put执行之前db[key]已经有一个对应的字符串，它将被value覆盖。
//====================================================

int kvdb_put(kvdb_t *db, const char *key, const char *value){
    char *writechar = (char *)malloc(sizeof(char*));
    char *buf = (char *)malloc(sizeof(char *));
    sprintf(writechar, "%s\n", value);
    /*printf("%s\n", writechar);*/
    /*printf("%ld\n", (unsigned long)strlen(writechar));*/
    lseek(db->fd, 0, SEEK_SET);
    int rc = 0;
    int tmp = 0;
    int ret;
    int flag = 0;           //是否存在key
    /*int writeflag = 0;      //是否能存下value*/
    while((rc = read_line(db->fd, buf, MAXLEN)) != 0){
        if(flag == 1){
            if(strncmp(value, buf, rc - 1) == 0)
                return 0;
            printf("%d\n", rc);
            if(strlen(value) <= rc - 1){
                sprintf(writechar, "%s\n", value);
                lseek(db->fd, 0 - rc, SEEK_CUR);
                ret = write(db->fd, writechar, strlen(writechar));
                if(ret < 0){
                    panic("write file failed");
                    return -1;
                }
                char c = 0;
                for(int i = strlen(writechar) + 1; i <= rc - 1; i++){
                    write(db->fd, &c, 1);
                }
                if(strlen(value) < rc - 1){
                    c = '\n';
                    ret = write(db->fd, &c, 1);
                }
                return 0;
            }else{
                lseek(db->fd, 0 - (rc + tmp), SEEK_CUR);
                for(int i = 1; i < tmp; i++)
                    sprintf(writechar, " ");
                sprintf(writechar, "\n");
                for(int i = 1; i < rc; i++)
                    sprintf(writechar, " ");
                sprintf(writechar, "\n");
            }
        }
        if(strncmp(buf, key, rc - 1) == 0){
            flag = 1;
        }
        tmp = rc;
    }

    sprintf(writechar, "%s\n%s\n", key, value);
    lseek(db->fd, 0, SEEK_END);
    ret = write(db->fd, writechar, strlen(writechar));
    if(ret < 0){
        panic("write file failed");
        return -1;
    }
    free(writechar);
    sync();
    return 0;
}

//====================================================
//获取key对应的value，相当于返回db[key]
//动态分配,用malloc,使用之后要free.
//===================================================

char *kvdb_get(kvdb_t *db, const char *key){
    char *ret = (char *)malloc(sizeof(char *));
    /*sprintf(ret, "hello");*/
    /*FILE *fp = NULL;*/
    /*fp = fdopen(db->fd, "r");*/
    /*if(fp == NULL){*/
        /*panic("fdopen failed");*/
        /*return NULL;*/
    /*}*/
    int flag = 0;
    lseek(db->fd, 0, SEEK_SET);

    int rc = 0;
    while((rc = read_line(db->fd, ret, MAXLEN))!=0){
        if(flag == 1){
            return ret;
        }
        printf("%s\n", ret);
        if(strncmp(ret, key, rc - 1) == 0){
            flag = 1;
        }
    }
    /*free(ret);*/
    return NULL;
}

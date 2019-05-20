#include "kvdb.h"

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
    sprintf(writechar, "%s\n%s", key, value);
    /*printf("%s\n", writechar);*/
    /*printf("%ld\n", (unsigned long)strlen(writechar));*/
    int ret = write(db->fd, writechar, strlen(writechar));
    if(ret < 0){
        panic("write file failed");
        return -1;
    }
    free(writechar);
    return 0;
}

//====================================================
//获取key对应的value，相当于返回db[key]
//动态分配,用malloc,使用之后要free.
//===================================================

char *kvdb_get(kvdb_t *db, const char *key){
    char *ret = (char *)malloc(sizeof(char *));
    /*sprintf(ret, "hello");*/
    FILE *fp = NULL;
    fp = fdopen(db->fd, "r");
    if(fp == NULL){
        panic("fdopen failed");
        return NULL;
    }
    int flag = 0;
    printf("%d\n", MAXLEN);
    while(fgets(ret, MAXLEN, fp) != NULL){
        assert(0);
        printf("%s\n", ret);
        if(strcmp(ret, key) == 0){
            flag = 1;
        }
        if(flag == 1){
            return ret;
        }
    }
    /*free(ret);*/
    return NULL;
}

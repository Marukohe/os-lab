#include "kvdb.h"

//====================================================
//如果文件不存在，则创建，如果文件存在，则在已有数据库的基础上进行操作。
//====================================================

int kvdb_open(kvdb_t * db, const char *filename){
    int ret = open(filename, O_CREAT | O_RDWR, 0666);
    printf("%d\n", ret);
    db->fd = ret;
    if(ret < 0)
        panic("open file failed");
    return 0;
}

int kvdb_close(kvdb_t *db){
    if(db->fd <= 0)
        panic("close file failed");
    return 0;
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
    return 0;
}

char *kvdb_get(kvdb_t *db, const char *key){
    char *ret = "hello";
    return ret;
}

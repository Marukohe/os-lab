#include "kvdb.h"

int kvdb_open(kvdb_t * db, const char *filename){
    int ret = open(filename, O_CREAT|O_RDWR);
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

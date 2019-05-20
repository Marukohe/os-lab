#include "kvdb.h"

int kvdb_open(kvdb_t * db, const char *filename){
    return 0;
}

int kvdb_close(kvdb_t *db){
    return 0;
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
    return 0;
}

char *kvdb_get(kvdb_t *db, const char *key){
    char *ret = "hello";
    return ret;
}

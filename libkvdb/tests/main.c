#include <kvdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TESTNUM 1000
int main(){
    kvdb_t db;
    char *key = "operating-systems";
    char *value;

    kvdb_open(&db, "a.db"); // BUG: should check for errors
    /*kvdb_put(&db, key, "three-easy-pieces");*/
    char *buf = "operating-systems";
    for(int i = 0; i < TESTNUM; i++){
        sprintf(buf, "operating-%d-systems", i);
        sprintf(key, "operating-%d-sys-hello", i);
        kvdb_put(&db, buf, key);
    }

    for(int i = 0; i < TESTNUM; i++){
        sprintf(buf, "operating-%d-systems", i);
        sprintf(key, "operating-%d-sys-hello-world", i);
        if((i  % 2) == 0)
            kvdb_put(&db, buf, key);
    }
    for(int i = 0; i < TESTNUM; i++){
        sprintf(buf, "operating-%d-systems", i);
        value = kvdb_get(&db, buf);
        printf("[%s]: [%s]\n", buf, value);
    }
    kvdb_close(&db);
    /*printf("[%s]: [%s]\n", "hello", value);*/
    free(value);
    return 0;
}

#include <kvdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    kvdb_t db;
    const char *key = "operating-systems";
    char *value;

    kvdb_open(&db, "a.db"); // BUG: should check for errors
    kvdb_put(&db, key, "three-easy-pieces");
    value = kvdb_get(&db, key);
    kvdb_close(&db);
    printf("[%s]: [%s]\n", key, value);
    free(value);
    return 0;
}

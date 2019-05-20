#ifndef __KVDB_H__
#define __KVDB_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLEN 10000

void panic(char *msg){
    printf("%s\n", msg);
    assert(0);
}

struct kvdb {
    int fd;

};
typedef struct kvdb kvdb_t;

int kvdb_open(kvdb_t *db, const char *filename);
int kvdb_close(kvdb_t *db);
int kvdb_put(kvdb_t *db, const char *key, const char *value);
char *kvdb_get(kvdb_t *db, const char *key);

#endif

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
#define MAXKEYLEN 130
//#define MAXVALUELEN (16 * 1024 * 1014)
#define MAXVALUELEN 128

void panic(char *msg){
    printf("%s\n", msg);
    assert(0);
}

void checkret(int rc, char *msg){
    if(rc <= 0){
        panic(msg);
        return -1;
    }
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

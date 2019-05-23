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
#include <pthread.h>
#define MAXKEYLEN 130
//#define MAXVALUELEN (16 * 1024 * 1014)
#define MAXVALUELEN 128

void panic(char *msg){
    printf("%s\n", msg);
    assert(0);
}

#define Log(format, ...) \
    printf("\33[1;33m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Logb(format, ...) \
    printf("\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Logg(format, ...) \
    printf("\33[1;32m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Logq(format, ...) \
    printf("\33[1;36m[%s,%d,%s] " format "\33[0m\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

struct kvdb {
    int fd;
    pthread_mutex_t mutex;
    //pthread_rwlock_t rw_lock;
};
typedef struct kvdb kvdb_t;

int kvdb_open(kvdb_t *db, const char *filename);
int kvdb_close(kvdb_t *db);
int kvdb_put(kvdb_t *db, const char *key, const char *value);
char *kvdb_get(kvdb_t *db, const char *key);

#endif

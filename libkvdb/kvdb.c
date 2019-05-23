#include "kvdb.h"

ssize_t read_line(int fd, void *ret, ssize_t maxlen, int flag){
    ssize_t n, rc;
    char c, *ptr;
    ptr = ret;
    char m;
    if(flag == 1)
        m = 0;     //1读到结束符
    else
        m = '\n';  //0读到换行符
    for(n = 1; n < maxlen; n++){
        if((rc = read(fd, &c, 1)) == 1){
            if(c == m)
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
    return n - 1;
}

//====================================================
//如果文件不存在，则创建，如果文件存在，则在已有数据库的基础上进行操作。
//====================================================

pthread_mutex_t lock;

int kvdb_open(kvdb_t * db, const char *filename){
    pthread_mutex_lock(&lock);
    if(db->fd >= 3){
        Log("file has been opened");
        pthread_mutex_unlock(&lock);
        return 0;
    }
    Log("file open");
    int ret = open(filename, O_CREAT | O_RDWR, 0666);
    /*printf("%d\n", ret);*/
    db->fd = ret;
    if(ret < 0){
        panic("open file failed");
        return -1;
    }
    flock(ret, LOCK_EX);
    pthread_rwlock_init(&(db->rw_lock), NULL);
    Log("mutex init");
    pthread_mutex_unlock(&lock);
    return 0;
}

//====================================================
//关闭数据库并释放相关资源。
//====================================================

int kvdb_close(kvdb_t *db){
    pthread_mutex_lock(&lock);
    Logb("close lock");
    pthread_rwlock_wrlock(&(db->rw_lock));
    if(db->fd < 0){
        Log("file has been closed");
        pthread_rwlock_unlock(&(db->rw_lock));
        pthread_mutex_unlock(&lock);
        return -1;
    }
    int ret = close(db->fd);
    Log("file closed");
    if(ret < 0){
        panic("close file failed");
        pthread_rwlock_unlock(&(db->rw_lock));
        pthread_mutex_unlock(&lock);
        return -1;
    }
    flock(db->fd, LOCK_UN);
    Logb("close unlock");
    pthread_rwlock_unlock(&(db->rw_lock));
    pthread_rwlock_destroy(&(db->rw_lock));
    pthread_mutex_unlock(&lock);
    return 0;
}

//====================================================
//建立key到value的映射
//在kvdb_put执行之前db[key]已经有一个对应的字符串，它将被value覆盖。
//====================================================

int writebuf(int fd, const char *buf, int len){
    int ret = write(fd, buf, strlen(buf));
    if(ret < 0){
        panic("write buf failed");
        return -1;
    }
    char c = 0;
    ret = write(fd, &c, 1);
    int offset = len - strlen(buf) - 1;
    lseek(fd, offset, SEEK_CUR);
    /*ret = write(fd, "\n", 1);*/
    if(ret < 0){
        panic("write buf failed");
        return -1;
    }
    return 0;
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
    Logg("put lock");
    pthread_rwlock_wrlock(&(db->rw_lock));
    if(strlen(key) > MAXKEYLEN || strlen(value) > MAXVALUELEN){
        Log("Sorry, My DataSet dosen't support such big string\n");
        pthread_rwlock_unlock(&(db->rw_lock));
        return -1;
    }
    char *buf = (char *)malloc(sizeof(char) * MAXKEYLEN);
    char *valuebuf = (char *)malloc(sizeof(char) * MAXKEYLEN);
    lseek(db->fd, 0, SEEK_SET);
    int rc = 0;
    while((rc = read_line(db->fd, buf, MAXKEYLEN, 0)) > 0){
        if(rc <= 0){
            panic("read key");
            pthread_rwlock_unlock(&(db->rw_lock));
            return -1;
        }
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
        if(rc <= 0){
            panic("read value len failed");
            pthread_rwlock_unlock(&(db->rw_lock));
            return -1;
        }
        int valuelen = atoi(valuebuf);
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
        if(rc <= 0){
            panic("read flag");
            pthread_rwlock_unlock(&(db->rw_lock));
            return -1;
        }
        int flag = atoi(valuebuf);
        if(strcmp(buf, key) == 0){
            if(flag == 0){
                lseek(db->fd, valuelen + 1, SEEK_CUR);
            }else{
                if(valuelen < strlen(value)){
                    lseek(db->fd, -2, SEEK_CUR);
                    char c = '0';
                    write(db->fd, &c, 1);
                    lseek(db->fd, valuelen + 2, SEEK_CUR);
                }else{
                    sprintf(buf, "%s\n", value);
                    rc = write(db->fd, buf, strlen(buf));
                    if(rc <= 0){
                        panic("write value");
                        pthread_rwlock_unlock(&(db->rw_lock));
                        return -1;
                    }
                    Logg("put unlock");
                    pthread_rwlock_unlock(&(db->rw_lock));
                    return 0;
                }
            }
        }else{
            lseek(db->fd, valuelen + 1, SEEK_CUR);
        }
        /*printf("2");*/
    }
    int flag = 1;
    int len = strlen(value);
    sprintf(buf, "%s\n%d\n%d\n%s\n", key, len, flag, value);
    rc = write(db->fd, buf, strlen(buf));
    if(rc <= 0){
        panic("write buf into database");
        pthread_rwlock_unlock(&(db->rw_lock));
        return -1;
    }
    free(buf);
    free(valuebuf);
    sync();
    Logg("put unlock");
    pthread_rwlock_unlock(&(db->rw_lock));
    return 0;
}

//====================================================
//获取key对应的value，相当于返回db[key]
//动态分配,用malloc,使用之后要free.
//===================================================

char *kvdb_get(kvdb_t *db, const char *key){
    Logq("get lock");
    pthread_rwlock_rdlock(&(db->rw_lock));
    char *retget = (char *)malloc(sizeof(char) * MAXVALUELEN);
    char *valuebuf = (char *)malloc(sizeof(char) * MAXVALUELEN);
    lseek(db->fd, 0, SEEK_SET);
    int rc = 0;
    while((rc = read_line(db->fd, retget, MAXKEYLEN, 0)) > 0){
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
        int valuelen = atoi(valuebuf);
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
        int flag = atoi(valuebuf);
        /*printf("%s\n", retget);*/
        if(strcmp(retget, key) == 0 && flag == 1){
            rc = read_line(db->fd, retget, MAXVALUELEN, 0);
            if(rc <= 0){
                free(retget);
                panic("read file failed");
                pthread_rwlock_unlock(&(db->rw_lock));
                return NULL;
            }
            free(valuebuf);
            Logq("get unlock");
            pthread_rwlock_unlock(&(db->rw_lock));
            return retget;
        }else{
            lseek(db->fd, valuelen + 1, SEEK_CUR);
        }
        /*printf("1");*/
    }
    free(valuebuf);
    free(retget);
    Logq("get unlock");
    pthread_rwlock_unlock(&(db->rw_lock));
    return NULL;
}


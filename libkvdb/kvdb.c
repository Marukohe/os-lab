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

int kvdb_open(kvdb_t * db, const char *filename){
    if(db->fd >= 3){
        Log("file has been opened");
        return 0;
    }
    int ret = open(filename, O_CREAT | O_RDWR, 0666);
    /*printf("%d\n", ret);*/
    db->fd = ret;
    if(ret < 0){
        panic("open file failed");
        return -1;
    }
    flock(ret, LOCK_EX);
    pthread_mutex_init(&(db->mutex), NULL);
    Log("mutex init");
    return 0;
}

//====================================================
//关闭数据库并释放相关资源。
//====================================================

int kvdb_close(kvdb_t *db){
    pthread_mutex_lock(&(db->mutex));
    if(db->fd < 0){
        Log("file has been closed");
        pthread_mutex_unlock(&(db->mutex));
        return -1;
    }
    int ret = close(db->fd);
    Log("file closed");
    if(ret < 0){
        panic("close file failed");
        pthread_mutex_unlock(&(db->mutex));
        return -1;
    }
    flock(db->fd, LOCK_UN);
    pthread_mutex_unlock(&(db->mutex));
    pthread_mutex_destroy(&(db->mutex));
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
    pthread_mutex_lock(&(db->mutex));
    if(strlen(key) > MAXKEYLEN || strlen(value) > MAXVALUELEN){
        Log("Sorry, My DataSet dosen't support such big string\n");
        pthread_mutex_unlock(&(db->mutex));
        return -1;
    }
    char *buf = (char *)malloc(sizeof(char) * MAXKEYLEN);
    char *valuebuf = (char *)malloc(sizeof(char) * MAXKEYLEN);
    lseek(db->fd, 0, SEEK_SET);
    int rc = 0;
    while((rc = read_line(db->fd, buf, MAXKEYLEN, 0)) > 0){
        if(rc <= 0){
            panic("read key");
            pthread_mutex_unlock(&(db->mutex));
            return -1;
        }
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
        if(rc <= 0){
            panic("read value len failed");
            pthread_mutex_unlock(&(db->mutex));
            return -1;
        }
        int valuelen = atoi(valuebuf);
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
        if(rc <= 0){
            panic("read flag");
            pthread_mutex_unlock(&(db->mutex));
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
                        pthread_mutex_unlock(&(db->mutex));
                        return -1;
                    }
                    pthread_mutex_unlock(&(db->mutex));
                    return 0;
                }
            }
        }else{
            lseek(db->fd, valuelen + 1, SEEK_CUR);
        }
        printf("2");
    }
    int flag = 1;
    int len = strlen(value);
    sprintf(buf, "%s\n%d\n%d\n%s\n", key, len, flag, value);
    rc = write(db->fd, buf, strlen(buf));
    if(rc <= 0){
        panic("write buf into database");
        pthread_mutex_unlock(&(db->mutex));
        return -1;
    }
    free(buf);
    free(valuebuf);
    sync();
    pthread_mutex_unlock(&(db->mutex));
    return 0;
}

//====================================================
//获取key对应的value，相当于返回db[key]
//动态分配,用malloc,使用之后要free.
//===================================================

char *kvdb_get(kvdb_t *db, const char *key){
    pthread_mutex_lock(&(db->mutex));
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
                pthread_mutex_unlock(&(db->mutex));
                return NULL;
            }
            free(valuebuf);
            pthread_mutex_unlock(&(db->mutex));
            return retget;
        }else{
            lseek(db->fd, valuelen + 1, SEEK_CUR);
        }
        printf("1");
    }
    free(valuebuf);
    free(retget);
    pthread_mutex_unlock(&(db->mutex));
    return NULL;
}


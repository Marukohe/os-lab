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
    int ret = open(filename, O_CREAT | O_RDWR, 0666);
    /*printf("%d\n", ret);*/
    db->fd = ret;
    if(ret < 0){
        panic("open file failed");
        return ret;
    }
    return 0;
}

//====================================================
//关闭数据库并释放相关资源。
//====================================================

int kvdb_close(kvdb_t *db){
    if(db->fd < 0){
        panic("close file failed, a fd < 0");
        return -1;
    }
    int ret = close(db->fd);
    if(ret < 0){
        panic("close file failed");
        return -1;
    }
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
    /*
    for(int i = 0; i < len - strlen(buf); i++){
        ret = write(fd, &c, 1);
        if(ret < 0){
            panic("write buf failed");
            return -1;
        }
    }*/
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
    if(strlen(key) > MAXKEYLEN || strlen(value) > MAXVALUELEN){
        printf("Sorry, My DataSet dosen't support such big string\n");
        return -1;
    }
    char *buf = (char *)malloc(sizeof(char) * MAXKEYLEN);
    char *valuebuf = (char *)malloc(sizeof(char) * MAXKEYLEN);
    lseek(db->fd, 0, SEEK_SET);
    int rc = 0;
    /*int ret = 0;*/
    /*char c = 0;*/
    while((rc = read_line(db->fd, buf, MAXKEYLEN, 0)) > 0){
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
        int valuelen = atoi(valuebuf);
        rc = read_line(db->fd, valuebuf, MAXKEYLEN, 0);
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
                        panic("write file failed in put");
                        return -1;
                    }
                    return 0;
                }
            }
        }else{
            lseek(db->fd, valuelen + 1, SEEK_CUR);
        }
    }
    int flag = 1;
    int len = strlen(value);
    sprintf(buf, "%s\n%d\n%d\n%s\n", key, len, flag, value);
    rc = write(db->fd, buf, strlen(buf));
    if(rc <= 0){
        panic("write file failed");
        return -1;
    }
    free(buf);
    free(valuebuf);
    sync();
    return 0;
}

//====================================================
//获取key对应的value，相当于返回db[key]
//动态分配,用malloc,使用之后要free.
//===================================================

char *kvdb_get(kvdb_t *db, const char *key){
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
            if(rc < 0){
                free(retget);
                panic("read file failed");
                return NULL;
            }
            free(valuebuf);
            return retget;
        }else{
            lseek(db->fd, valuelen + 1, SEEK_CUR);
        }
    }
    free(valuebuf);
    free(retget);
    return NULL;
}


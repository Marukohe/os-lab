#include "kvdb.h"

ssize_t read_line(int fd, void *ret, ssize_t maxlen){
    ssize_t n, rc;
    char c, *ptr;
    ptr = ret;
    for(n = 1; n < maxlen; n++){
        if((rc = read(fd, &c, 1)) == 1){
            if(c == '\n')
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
    return n;
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
    for(int i = 0; i < len - strlen(buf); i++){
        ret = write(fd, &c, 1);
        if(ret < 0){
            panic("write buf failed");
            return -1;
        }
    }
    /*ret = write(fd, "\n", 1);*/
    if(ret < 0){
        panic("write buf failed");
        return -1;
    }
    return 0;
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
    if(strlen(key) >= MAXKEYLEN || strlen(value) >= MAXVALUELEN){
        printf("Sorry, My DataSet dosen't support such big string\n");
        return -1;
    }
    char *buf = (char *)malloc(sizeof(char*));
    lseek(db->fd, 0, SEEK_SET);
    /*int rc = 0;*/
    int ret = 0;
    /*char c = 0;*/
    while(read(db->fd, buf, MAXKEYLEN) > 0){
        if(strcmp(buf, key) == 0){
            /*flag = 1;*/
            ret = writebuf(db->fd, value, MAXVALUELEN);
            free(buf);
            sync();
            return ret;
        }
        lseek(db->fd, MAXVALUELEN, SEEK_CUR);
    }
    lseek(db->fd, 0, SEEK_END);
    ret = writebuf(db->fd, key, MAXKEYLEN);
    if(ret < 0)
        return ret;
    ret = writebuf(db->fd, value, MAXVALUELEN);
    free(buf);
    sync();
    return 0;
}

//====================================================
//获取key对应的value，相当于返回db[key]
//动态分配,用malloc,使用之后要free.
//===================================================

char *kvdb_get(kvdb_t *db, const char *key){
    char *retget = (char *)malloc(sizeof(char*));
    int flag = 0;
    lseek(db->fd, 0, SEEK_SET);
    int rc = 0;
    while((rc = read(db->fd, retget, MAXKEYLEN)) > 0){
        if(flag == 1){
            return retget;
        }
        /*printf("%d\n", rc);*/
        if(flag == 1){
            flag = 1;
        }else{
            lseek(db->fd, MAXVALUELEN, SEEK_CUR);
        }
    }
    free(retget);
    return NULL;
}


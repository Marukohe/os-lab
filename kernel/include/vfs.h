#ifndef __VFS_H__
#define __VFS_H__

#include <common.h>
#define OFFSETNUM 10
#define INODENUM 1000
#define BLOCKSIZE 4096
#define DIRSIZE 512

// FILE MODE
// ---------------------------------------
#define RABLE 4
#define WABLE 2
#define XABLE 1

// LOOKUP MODE
// ---------------------------------------
#define O_CREAT 8
#define O_DIR 16
//#define O_

// lseek MODE
// ---------------------------------------
#define SEEKSET 0
#define SEEKCUR 1
#define SEEKEND 2

// access MODE
// ---------------------------------------
#define R_OK 0
#define W_OK 1
#define X_OK 2
#define F_OK 3

struct filesystem;
typedef struct filesystem filesystem_t;

struct fsops;
typedef struct fsops fsops_t;

struct inode;
typedef struct inode inode_t;

struct file;
typedef struct file file_t;

typedef struct {
  void (*init)();
  int (*access)(const char *path, int mode);
  int (*mount)(const char *path, filesystem_t *fs);
  int (*unmount)(const char *path);
  int (*mkdir)(const char *path);
  int (*rmdir)(const char *path);
  int (*link)(const char *oldpath, const char *newpath);
  int (*unlink)(const char *path);
  int (*open)(const char *path, int flags);
  ssize_t (*read)(int fd, void *buf, size_t nbyte);
  ssize_t (*write)(int fd, void *buf, size_t nbyte);
  off_t (*lseek)(int fd, off_t offset, int whence);
  int (*close)(int fd);
} MODULE(vfs);

typedef struct dir{
    int cnt;
    uint8_t used[20];
    char name[20][100];
    int offset[20];
}dir_t;

struct filesystem {
  //...
  const char *name;
  int id;
  int ioffset[INODENUM];
  uint8_t used[INODENUM];
  int cntinode;
  fsops_t *ops;
  device_t *dev;
  inode_t *sinode;
};


struct fsops {
  void (*init)(struct filesystem *fs, const char *name, device_t *dev);
  inode_t *(*lookup)(struct filesystem *fs, const char *path, int flags);
  int (*close)(inode_t *inode);
};

typedef struct inodeops {
  int (*open)(file_t *file, int flags);
  int (*close)(file_t *file);
  ssize_t (*read)(file_t *file, char *buf, size_t size);
  ssize_t (*write)(file_t *file, const char *buf, size_t size);
  off_t (*lseek)(file_t *file, off_t offset, int whence);
  int (*mkdir)(const char *name);
  int (*rmdir)(const char *name);
  int (*link)(const char *name, inode_t *inode);
  int (*unlink)(const char *name);
  // 你可以自己设计readdir的功能
} inodeops_t;

struct inode {
  //...
  int refcnt;
  int filesize; //记录文件大小
  int flags;
  int is_dir;
  int offset[OFFSETNUM];
  int pos;
  void *ptr;       // private data
  filesystem_t *fs;
  inodeops_t *ops; // 在inode被创建时，由文件系统的实现赋值
                   // inode ops也是文件系统的一部分
  //...
};

struct file {
  int refcnt; // 引用计数
  inode_t *inode;
  char *path;
  uint64_t offset;
  //...
};

typedef struct mounttable{
    char rootname[20][40];
    int id[20];
    int cnt;
}mt_t;

#endif

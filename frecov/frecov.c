#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

unsigned long get_file_size(const char *path){
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0)
        return filesize;
    else
        filesize = statbuff.st_size;
    return filesize;
}

int main(int argc, char *argv[]) {
    /*void *startaddr;*/
    printf("arvg[1], %s\n", argv[1]);
    int fd = open("./fs.img", O_RDONLY);
    if(fd == -1)
        handle_error("open");
    char *path = "./";
    strcat(path, argv[1]);
    printf("path %s\n", path);
    /*unsigned long fsize = get_file_size(path);*/
    /*printf("%ld\n", fsize);*/
  return 0;
}

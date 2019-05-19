#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

struct FATstruct{
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSector;
    uint8_t NumberofFAT;
};

struct FATstruct *fatstruct;

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
    if(argc == 1)
        argv[1] = "fs.img";
    uint8_t *startaddr;
    printf("arvg[1] %s\n", argv[1]);
    int fd = open(argv[1], O_RDWR|O_CREAT, 0666);
    if(fd == -1)
        handle_error("open");
    unsigned long fsize = get_file_size(argv[1]);
    startaddr = mmap(NULL, fsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    fatstruct = (struct FATstruct *)(startaddr + 0xB);
    printf("0x%x\n", fatstruct->BytesPerSector);
    printf("0x%x\n", fatstruct->SectorsPerCluster);
    printf("0x%x\n", fatstruct->ReservedSector);
    printf("0x%x\n", *(startaddr + 0xE));
    printf("0x%x\n", *(startaddr + 0xF));
    printf("0x%x\n", fatstruct->NumberofFAT);
    munmap(startaddr, fsize);
    close(fd);
  return 0;
}

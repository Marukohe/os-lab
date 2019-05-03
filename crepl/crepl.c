#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#define maxlen 1000
/*#define MODE 775*/
char s[maxlen];

char * num_to_string(int cnt){
    char *fpath = "./tmpc/";
    char num[1000];
    sprintf(num, "%d", cnt);
    strcpy(fpath, num);
    strcpy(fpath, ".c");
    printf("%s\n", fpath);
    return fpath;
}

int main(int argc, char *argv[]) {
    char *path = "./tmpc";
    int cnt = 0;
    /*assert(mkdir(path, 0777) == 0);*/
    int a = mkdir(path, 0777);
    printf("%d\n", a);
    printf(">> ");
    printf("Welcome to the C-shell, Please enter quit() to quit!\n");
    printf(">> ");
    while(fgets(s, maxlen,stdin)){
        if(strncmp(s, "quit()", 6) == 0)
            break;
        if(strncmp(s, "int", 3) == 0){
            printf(">> add function successfully!\n");
            printf(">> ");
            cnt++;
            char *fpath = num_to_string(cnt);
            printf("%s\n", fpath);
        }else{
            printf(">> hello, world\n");
            printf(">> ");
        }
    }
  return 0;
}

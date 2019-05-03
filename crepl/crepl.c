#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#define maxlen 1000
/*#define MODE 775*/
char s[maxlen];

int main(int argc, char *argv[]) {
    char *path = "./tmpc";
    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH);
    printf(">> ");
    printf("Welcome to the C-shell, Please enter quit() to quit!\n");
    printf(">> ");
    while(fgets(s, maxlen,stdin)){
        if(strncmp(s, "quit()", 6) == 0)
            break;
        if(strncmp(s, "int", 3) == 0){
            printf(">> add function successfully!\n");
            printf(">> ");
        }else{
            printf(">> hello, world\n");
            printf(">> ");
        }
    }
  return 0;
}

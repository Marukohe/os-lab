#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#define maxlen 1000
char s[maxlen];

int main(int argc, char *argv[]) {
    printf(">> ");
    printf("Welcome to the C-shell, Please enter quit() to quit!\n");
    while(~scanf("%s", s)){
        if(strcmp(s, "quit()") == 0)
            break;
    }
  return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>
#define maxlen 1000
/*#define MODE 775*/
char s[maxlen];
typedef int (*func_t)();

int main(int argc, char *argv[]) {
#ifdef __x86_64__
    printf("64\n");
#elif __i386__
    printf("32\n");
#endif
    char *path = "./tmpc";
    char *comm = "gcc -fPIC -shared ";
    int cnt = 0;      //count the number of functions
    int cntexpr = 0;  //count the number of expressions
    /*assert(mkdir(path, 0777) == 0);*/
    int a = mkdir(path, 0777);
    /*printf("%d\n", a);*/
    assert(a == 0);
    printf(">> ");
    printf("Welcome to the C-shell, Please enter quit() to quit!\n");
    printf(">> ");
    while(fgets(s, maxlen,stdin)){
        if(strncmp(s, "quit()", 6) == 0){
            /*rmdir(path);*/
            system("rm -rf tmpc/");
            break;
        }
        if(strncmp(s, "int", 3) == 0){
            printf(">> add function successfully!\n");
            printf(">> ");
            cnt++;
            char fpath[1000];
            char num[1000];
            sprintf(num, "%d", cnt);
            strcpy(fpath, "./tmpc/");
            strcat(fpath, num);
            strcat(fpath, ".c");
            FILE *fp = fopen(fpath, "wb");
            fwrite(s, sizeof(char), strlen(s), fp);
            fclose(fp);
            char gcc_command[1000];
            char so_name[10000];
            sprintf(so_name, "./tmpc/%d.so", cnt);
            strcpy(gcc_command, comm);
            strcat(gcc_command, fpath);
            strcat(gcc_command, " -o ");
            strcat(gcc_command, so_name);
            /*system("gcc -fPIC -shared ./tmpc/1.c -o ./tmpc/1.so");*/
            system(gcc_command);
            /*printf("%s\n", so_name);*/
            assert(dlopen(so_name ,RTLD_LAZY | RTLD_GLOBAL));
            /*printf("%s\n>> ", gcc_command);*/
            /*printf("%s\n", fpath);*/
            /*printf(">> ");*/
        }else{
            char func[10000];
            cntexpr++;
            sprintf(func, "int _expr_wrap_%d(){\n    return %s;}", cntexpr, s);
            /*printf(">> %s\n", func);*/
            char expr_path[maxlen];
            sprintf(expr_path, "./tmpc/_expr_%d.c", cntexpr);
            FILE *fp = fopen(expr_path, "wb");
            fwrite(func, sizeof(char), strlen(func), fp);
            fclose(fp);
            char gcc_command[maxlen];
            char so_name[maxlen];
            char func_name[maxlen];
            sprintf(so_name, "./tmpc/_expr_%d.so", cntexpr);
            sprintf(func_name, "_expr_wrap_%d", cntexpr);
            sprintf(gcc_command, "gcc -fPIC -shared -Wno-implicit-function-declaration ./tmpc/_expr_%d.c -o %s", cntexpr, so_name);
            /*printf("%s\n", gcc_command);*/
            system(gcc_command);
            void *handle = dlopen(so_name, RTLD_LAZY);
            if(!handle){
                fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }
            /*printf("%s\n%s\n", so_name, func_name);*/
            func_t rtfunc = (func_t)dlsym(handle, func_name);
            /*p = dlsym("./tmpc/1.so", "add");*/
            printf(">> %d\n", rtfunc());
            printf(">> ");
        }
    }
  return 0;
}

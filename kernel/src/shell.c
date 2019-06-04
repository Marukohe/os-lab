#include <common.h>
#include <vfs.h>
#include <devices.h>
extern spinlock_t shelllock;
extern spinlock_t yk;

char *strsplit(char *s1){
    char *cp = pmm->alloc(128);
    int off = 0;
    for(int i = 0; i < strlen(s1); i++){
        if(s1[i] == ' '){
            off = i;
            strncpy(cp, s1, off);
            return cp;
        }
    }
    strcpy(cp, s1);
    return cp;
}

static int shell_help(char *args);

static struct{
    char *name;
    char *description;
    int (*handler) (char *);
} shell_table [] = {
    {"help", "Display imformations about supported commands", shell_help},
};

#define NR_SHELL (sizeof(shell_table) / sizeof(shell_table[0]))

static int shell_help(char *args){
    int i;
    char text[256];
    if(args == NULL){
        for(i = 0; i < NR_SHELL; i++){
            printf("%s - %s\n", shell_table[i].name, shell_table[i].description);
            sprintf(text, "%s - %s\n", shell_table[i].name, shell_table[i].description);
            vfs->write(1, text, strlen(text));
        }
    }else{
        for(i = 0; i < NR_SHELL; i++){
            if(strcmp(args, shell_table[i].name) == 0){
                printf("%s - %s\n", shell_table[i].name, shell_table[i].description);
                sprintf(text, "%s - %s\n", shell_table[i].name, shell_table[i].description);
                vfs->write(1, text, strlen(text));
                return 0;
            }
        }
        printf("Unknow command '%s'\n", args);
        sprintf(text, "Unknow command '%s'\n", args);
        vfs->write(1, text, strlen(text));
    }
    return 0;
}


void shell(void *name){
    /*kmt->spin_lock(&shelllock);*/
    int stdin = vfs->open(name, RABLE);  //0
    int stdout = vfs->open(name, WABLE); //1
    while(1){
        char line[128], text[128];
        sprintf(text, "(%s) $ ", "maruko");
        vfs->write(stdout, text, strlen(text));
        int nread = vfs->read(stdin, line, sizeof(line));
        line[nread - 1] = '\0';
        sprintf(text, "Echo: %s.\n", line);
        char *cmd = strsplit(line);
        char *args;
        if(strlen(line) == strlen(cmd))
            args = NULL;
        else
            args = line + strlen(cmd) + 1;
        Logy("cmd: %s args: %s", cmd, args);

        int i;
        for(i = 0; i < NR_SHELL; i++){
            if(strcmp(cmd, shell_table[i].name) == 0){
                if(shell_table[i].handler(args) < 0){
                    printf("%s failed\n", cmd);
                }
                break;
            }
        }
        if(i == NR_SHELL) {printf("Unknown command '%s'\n", cmd);}

        /*vfs->write(stdout, text, strlen(text));*/
    }
    /*kmt->spin_unlock(&shelllock);*/
}

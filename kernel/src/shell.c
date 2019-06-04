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
            strncpy(cp, s1, off - 1);
            return cp;
        }
    }
    pmm->free(cp);
    return NULL;
}
/*
static int shell_help(char *args){
    return 0;
}


static struct{
    char *name;
    char *descrition;
    int (*handler) (char *);
} shell_table [] = {
    {"help", "Display imformations about supported commands", shell_help},
};

#define NR_SHELL (sizeof(shell_table) / sizeof(shell_table[0]))
*/

void shell(void *name){
    /*kmt->spin_lock(&shelllock);*/
    int stdin = vfs->open(name, RABLE);
    int stdout = vfs->open(name, WABLE);
    while(1){
        char line[128], text[128];
        sprintf(text, "(%s) $ ", name);
        vfs->write(stdout, text, strlen(text));
        int nread = vfs->read(stdin, line, sizeof(line));
        line[nread - 1] = '\0';
        sprintf(text, "Echo: %s.\n", line);
        char *cmd = strsplit(line);
        char *args = name + strlen(cmd) + 1;
        Logy("cmd: %s args: %s", cmd, args);
        vfs->write(stdout, text, strlen(text));
    }
    /*kmt->spin_unlock(&shelllock);*/
}

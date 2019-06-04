#include <common.h>
#include <vfs.h>
#include <devices.h>
extern spinlock_t shelllock;
extern spinlock_t yk;

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
        vfs->write(stdout, text, strlen(text));
    }
    /*kmt->spin_unlock(&shelllock);*/
}
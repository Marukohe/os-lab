#include <common.h>
#include <vfs.h>
#include <devices.h>

void shell(void *name){
    int stdin = vfs->open(name, RABLE);
    int stdout = vfs->open(name, WRABLE);
    while(1){
        char line[128], text[128];
        sprintf(text, "(%s) $ ", name);
        vfs->write(stdout, text, strlen(text));
        int nread = vfs->read(stdin, line, sizeof(line));
        line[nread - 1] = '\0';
        sprintf(text, "Echo: %s.\n", line);
        vfs->write(stdout, text, strlen(text));
    }
}

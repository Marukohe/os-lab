#include <common.h>
#include <vfs.h>
#include <devices.h>
extern spinlock_t shelllock;
extern spinlock_t yk;
extern mt_t *mtt;
extern task_t *current_task[4];
extern filesystem_t *filesys[FSNUM];
#define current (current_task[_cpu()])
/*#define SHELLDEBUG*/

extern char *splitpath(char *path, int offset);

// 命令解析
char *strsplit(char *s1){
    char *cp = (char *)pmm->alloc(128);
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

//扩展为绝对路径
static void extendpass(char *ret, char *args){
    if(args[0] == '/'){
        strcpy(ret, args);
    }else if(strcmp(current->pwd, "/") == 0){
        sprintf(ret, "/%s", args);
    }else{
        sprintf(ret, "%s/%s", current->pwd, args);
    }
}

static int shell_help(char *args);

static int shell_pwd(char *args){
    char text[128];
    sprintf(text, "%s\n", current->pwd);
    vfs->write(STDOUT, text, strlen(text));
    return 0;
}

static int shell_cd(char *args){;
    char *text = pmm->alloc(128);
    if(args == NULL){
        strcpy(current->pwd, "/");
        sprintf(text, "change dir to: %s\n", current->pwd);
        vfs->write(STDOUT, text, strlen(text));
    }else if(strcmp(args, ".") == 0 || strcmp(args, "./") == 0){
        sprintf(text, "change dir to: %s\n", current->pwd);
        vfs->write(STDOUT, text, strlen(text));
    }else if(strcmp(args, "..") == 0 || strcmp(args, "../") == 0){
        int flag = 0;
        Logb("%d", mtt->cnt);
        for(int i = 0; i < mtt->cnt; i++){
            printf("%s\n", mtt->rootname[i]);
            if(mtt->used[i] == 1 && strcmp(mtt->rootname[i], current->pwd) == 0){
                flag = 1;
                break;
            }
        }
        if(flag == 1){
            Logb("flag == 1");
            strcpy(current->pwd, "/");
            sprintf(text, "change dir to: %s\n", current->pwd);
            vfs->write(STDOUT, text, strlen(text));
        }else{
            char *pwd = splitpath(current->pwd, strlen(current->pwd));
            if(pwd == NULL){
                strcpy(current->pwd, "/");
            }else{
                strcpy(current->pwd, pwd);
                pmm->free(pwd);
            }
            /*pmm->free(pwd);*/
            /*assert(0);*/
            sprintf(text, "change dir to: %s\n", current->pwd);
            vfs->write(STDOUT, text, strlen(text));
        }
    }else if(strncmp(args, "/", 1) == 0){
        int ret = vfs->access(args, D_OK);
        if(ret == 0){
            strcpy(current->pwd, args);
            sprintf(text, "change dir to: %s\n", current->pwd);
            vfs->write(STDOUT, text, strlen(text));
        }else{
            sprintf(text, "Not such Dictionary\n");
            vfs->write(STDOUT, text, strlen(text));
        }
    }else{
        if(strcmp(current->pwd, "/") != 0)
            sprintf(text, "%s/", current->pwd);
        else
            sprintf(text, "/");
        strcat(text, args);
        int ret = vfs->access(text, D_OK);
        Logg("access ret: %d, text: %s", ret, text);
        if(ret == 0){
            strcpy(current->pwd, text);
            sprintf(text, "change dir to: %s\n", current->pwd);
            vfs->write(STDOUT, text, strlen(text));
        }else{
            sprintf(text, "Not such Dictionary\n");
            vfs->write(STDOUT, text, strlen(text));
        }
    }
    pmm->free(text);
    /*Logb("flag == 1");*/
    return 0;
}

static int shell_ls(char *args){
    char text[128];
    if(args == NULL){
        strcpy(text, current->pwd);
    }else if(args[0] == '/'){
        strcpy(text, args);
    }else{
        int off = strlen(args);
        if(args[off - 1] == '/')
            args[off - 1] = '\0';
        sprintf(text, "%s/%s", current->pwd, args);
    }
    Logg("ls path %s", text);
    inode_t *node = filesys[2]->ops->lookup(filesys[2], text, 7|O_DIR);
    if(node == NULL){
        sprintf(text, "Not such dir\n");
        vfs->write(STDOUT, text, strlen(text));
        return 0;
    }
    void *buf = (char *)pmm->alloc(BLOCKSIZE);
    filesys[2]->dev->ops->read(filesys[2]->dev, node->offset[0], buf, BLOCKSIZE);
    dir_t *dir = (dir_t *)buf;
    char out[128];
    memset(out, 0, sizeof(out));
    Logb("%s\n", dir->cnt);
    for(int i = 0; i < dir->cnt; i++){
        if(dir->used[i] == 1){
            printf("%s\n", dir->name[i]);
            strcat(out, dir->name[i]);
            strcat(out, "  ");
        }
    }
    strcat(out, "\n");
    vfs->write(STDOUT, out, strlen(out));
    pmm->free(buf);
    /*pmm->free(node);*/

    return 0;
}


static int shell_mkdir(char *args){
    char text[128];
    if(args == NULL)
        return 0;
    extendpass(text, args);
    Logg("mkdir path %s",text);
    int ret = vfs->mkdir(text);
    if(ret == -1){
        sprintf(text, "mkdir failed\n");
    }else{
        sprintf(text, "mkdir successfully\n");
    }
    vfs->write(STDOUT, text, strlen(text));
    return 0;
}

static int shell_rmdir(char *args){
    if(args == NULL)
        return 0;
    char text[128];
    extendpass(text, args);
    Logg("rmdir path %s", text);
    int ret = vfs->rmdir(text);
    if(ret == -1){
        sprintf(text, "rmdir failed\n");
    }else{
        sprintf(text, "rmdir successfully\n");
    }
    vfs->write(STDOUT, text, strlen(text));
    return 0;
}


static int shell_cat(char *args){
    if(args == NULL)
        return 0;
    char text[512];
    extendpass(text, args);
    int fd = vfs->open(text, RABLE);
    vfs->lseek(fd, 0, SEEKCUR);
    memset(text, 0, 512);
    int nread = vfs->read(fd, text, current->fildes[fd]->inode->filesize);
    /*Logg("%d %s", nread, text);*/
    if(nread == -1){
        sprintf(text, "cat file failed\n");
    }
    vfs->close(fd);
    vfs->write(STDOUT, text, strlen(text));
    return 0;
}

static int shell_touch(char *args){
    if(args == NULL)
        return 0;
    char text[128];
    extendpass(text, args);
    int ret1 = vfs->access(text, F_OK);
    int ret2 = vfs->access(text, D_OK);
    if(!ret1 | !ret2){
        sprintf(text, "Create file failed.\n");
    }else{
        filesys[2]->ops->lookup(filesys[2], text, 7|O_CREAT);
        sprintf(text, "Create file successfully.\n");
    }
    vfs->write(STDOUT, text, strlen(text));
    return 0;
}

static int shell_rm(char *args){
    if(args == NULL)
        return 0;
    char text[128];
    extendpass(text, args);
    int ret = vfs->access(text, F_OK);
    if(ret == -1){
        sprintf(text, "Not such file.\n");
    }else{
        vfs->rmdir(text);
        sprintf(text, "rm file successfully.\n");
    }
    vfs->write(STDOUT, text, strlen(text));
    return 0;
}

static int shell_echo(char *args){
    char text[128];
    sprintf(text, "%s\n", args);
    vfs->write(STDOUT, text, strlen(text));
    return 0;
}

static int shell_redir(char *path, char *args){
    //将args写到path;
    char text[256];
    Logg("redir path: %s args: %s", path, args);
    sprintf(text, "%s\n", args);
    int ret = vfs->access(path, F_OK);
    if(ret == -1){
        int fd = vfs->open(path, 7|O_CREAT);
        vfs->write(fd, text, strlen(text));
        vfs->close(fd);
        return 0;
    }
    ret = vfs->access(path, W_OK);
    if(ret == -1){
        sprintf(text, "Permission denied.\n");
        vfs->write(STDOUT, text, strlen(text));
        return -1;
    }else{
        int fd = vfs->open(path, 7);
        vfs->write(fd, text, strlen(text));
        vfs->close(fd);
    }

    return 0;
}

static struct{
    char *name;
    char *description;
    int (*handler) (char *);
} shell_table [] = {
    {"help", "Display imformations about supported commands", shell_help},
    {"pwd", "Display current workdir", shell_pwd},
    {"cd", "Change current workdir", shell_cd},
    {"ls", "Display files or dirs in curent workdir", shell_ls},
    {"mkdir", "Create a dictionary", shell_mkdir},
    {"rmdir", "Remove a dictionary", shell_rmdir},
    {"cat", "Display context", shell_cat},
    {"touch", "create a file", shell_touch},
    {"rm", "remove a file", shell_rm},
    {"echo", "Display input", shell_echo},
};

#define NR_SHELL (sizeof(shell_table) / sizeof(shell_table[0]))

static int shell_help(char *args){
    int i;
    char text[128];
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
#ifdef SHELLDEBUG
        sprintf(line, "cat /Documents/lyrics.txt");
#endif
        char *cmd = strsplit(line);
        char *args;
        if(strlen(line) == strlen(cmd))
            args = NULL;
        else
            args = line + strlen(cmd) + 1;
        int flag = 0;
        for(int i = 0; i < strlen(args); i++){
            if(args[i] != ' ')
                flag = 1;
        }
        if(flag == 0)
            args = NULL;
        Logy("cmd: %s args: %s", cmd, args);
        // 重定向
        int redir = 0;
        int pos1 = 0, pos2 = 0;
        char text1[128], text2[128];
        for(int i = 0; i < strlen(args); i++){
            if(args[i] == '>'){
                redir = 1;
                for(int j = i - 1; j > 0; j--){
                    if(args[j] != ' '){
                        pos1 = j;
                        break;
                    }
                }
                for(int j = i + 1; j < strlen(args); j++){
                    if(args[j] != ' '){
                        pos2 = j;
                        break;
                    }
                }
                strncpy(text1, args, pos1 + 1);
                char *cpt = args + pos2;
                strcpy(text2, cpt);
                if(strcmp(cmd, "cat") == 0){
                    //text1文件绝对路径,text2文件绝对路径
                    /*Logg("%s %s", text1, text2);*/

                }else if(strcmp(cmd, "echo") == 0){
                    //text1字符串,text2文件绝对路径
                    /*shell_dir()*/
                    shell_redir(text2, text1);
                    Logg("%s %s", text1, text2);
                }else{
                    sprintf(text, "Command is not supported.\n");
                    vfs->write(STDOUT, text, strlen(text));
                }
                break;
            }
        }
        if(redir == 1)
            continue;

        //选择执行函数
        int i;
        for(i = 0; i < NR_SHELL; i++){
            if(strcmp(cmd, shell_table[i].name) == 0){
                if(shell_table[i].handler(args) < 0){
                    printf("%s failed\n", cmd);
                }
                break;
            }
        }
        if(i == NR_SHELL) {
            /*printf("Unknown command '%s'\n", cmd);*/
            sprintf(text, "Unknown command '%s'\n", cmd);
            vfs->write(STDOUT, text, strlen(text));
        }

        /*vfs->write(stdout, text, strlen(text));*/
    }
    /*kmt->spin_unlock(&shelllock);*/
}

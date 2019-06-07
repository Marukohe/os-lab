#include <common.h>
#include <klib.h>
#include <devices.h>
#include <vfs.h>

/*#define OSDEBUG*/
/*#define CONSUMER*/
#define IDLE
/*#define TTY*/
#define SHELL

#define TKNUM 25
extern task_t *cputask[TKNUM];
extern task_t *current_task[4];
extern filesystem_t *filesys[FSNUM];
#define current (current_task[_cpu()])

extern ssize_t tty_write(device_t *dev, off_t offset, const void *buf, size_t count);
extern struct spinlock pk;
int cnthandler = 0;
struct handlers schandlers[MAXHANDLER];
spinlock_t yk;
spinlock_t shelllock;

sem_t fillsem;
sem_t emptysem;

void producer(){
    /*kmt->spin_lock(&pk);*/
    /*assert(0);*/
    /*Logw("in producer\n");*/
    /*kmt->spin_unlock(&pk);*/
    /*assert(0);*/
    while(1){
        kmt->sem_wait(&emptysem);
        kmt->spin_lock(&pk);
        printf("(");
        kmt->spin_unlock(&pk);
        kmt->sem_signal(&fillsem);
    }
}

void consumer(){
    /*assert(0);*/
    while(1){
        kmt->sem_wait(&fillsem);
        kmt->spin_lock(&pk);
        printf(")");
        kmt->spin_unlock(&pk);
        kmt->sem_signal(&emptysem);
    }
}

void idle(){
    while(1){
        /*_putc("01234"[_cpu()]);*/
        /*assert(get_efl() & FL_IF);*/
        /*_yield();*/
    }
}


void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  while (1) {
    char line[128], text[128];
    sprintf(text, "(%s) $ ", name);
    tty_write(tty, 0, text, strlen(text));
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    /*printf("%s\n",line);*/
    sprintf(text, "Echo: %s.\n", line);
    tty_write(tty, 0, text, strlen(text));
  }
}

extern void shell(void *name);

void proccreate(char *path, char *name){
    char text[256];
    sprintf(text, "/%s", path);
    inode_t *node = filesys[0]->ops->lookup(filesys[0], text, 7|O_CREAT);
    memset(text, 0, sizeof(text));
    sprintf(text, "pid: %s\ntaskname: %s\ncreatetime: %d", path, name, uptime());
    node->filesize = strlen(text);
    filesys[2]->dev->ops->write(filesys[2]->dev, node->offset[0], (void *)text, strlen(text));
    filesys[2]->dev->ops->write(filesys[2]->dev, node->pos, (void *)node, sizeof(inode_t));
    pmm->free(node);
}

static void os_init() {
  pmm->init();
  kmt->init();
  /*_vme_init(pmm->alloc, pmm->free);*/
  dev->init();
  vfs->init();

#ifdef TTY
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty1");
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty2");
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty3");
  kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty4");
#endif

#ifdef CONSUMER
  kmt->create(pmm->alloc(sizeof(task_t)) , "produce", producer, NULL);
  kmt->create(pmm->alloc(sizeof(task_t)) , "consume", consumer, NULL);
#endif

#ifdef IDLE
  kmt->create(pmm->alloc(sizeof(task_t)) , "idle1", idle, NULL);
  kmt->create(pmm->alloc(sizeof(task_t)) , "idle2", idle, NULL);
  kmt->create(pmm->alloc(sizeof(task_t)) , "idle3", idle, NULL);
  kmt->create(pmm->alloc(sizeof(task_t)) , "idle4", idle, NULL);
#endif

#ifdef SHELL
  kmt->create(pmm->alloc(sizeof(task_t)) , "shell", shell, "/dev/tty1");
  proccreate("1", "tty1");
  kmt->create(pmm->alloc(sizeof(task_t)) , "shell", shell, "/dev/tty2");
  kmt->create(pmm->alloc(sizeof(task_t)) , "shell", shell, "/dev/tty3");
  kmt->create(pmm->alloc(sizeof(task_t)) , "shell", shell, "/dev/tty4");
#endif

  kmt->sem_init(&emptysem, "empty", 10);
  kmt->sem_init(&fillsem, "fill", 0);
  kmt->spin_init(&yk, "yield lock");
  kmt->spin_init(&shelllock, "shelllock");
}

static void hello() {
    kmt->spin_lock(&pk);
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
  kmt->spin_unlock(&pk);
}

#define test_ptr_nr 1024
#define testnum 100
/*
void test(){
    void *space[testnum];
    int i;
    for(i=0;i<testnum;i++){
        space[i] = pmm->alloc(rand()%((1<<20)-1));
    }
    for(i=0;i<10000;i++){
        int tmp = rand()%100;
        pmm->free(space[tmp]);
        space[tmp] = pmm->alloc((rand()&((1<<30)-1)));
    }
    for(i=0;i<testnum;i++){
        pmm->free(space[i]);
    }
    kmt->spin_lock(&pk);
    printf("test finish\n");
    Logb("TEST FINISH");
    kmt->spin_unlock(&pk);
}
*/

static void os_run() {
  hello();
  /*shell("/dev/tty1");*/
  /*test();*/
  _intr_write(1);
  while (1) {
      /*assert(0);*/
      /*assert(get_efl() & FL_IF); //0 开中断*/
      /*printf("yield\n");*/
    _yield();
  }
}

static _Context *os_trap(_Event ev, _Context *context) {
    /*kmt->spin_lock(&pk);*/
    /*Logg("current task name: %s, id: %d", current->name, current->id);*/
    /*assert(yk.locked == 0);*/
    /*kmt->spin_unlock(&pk);*/
    /*assert(get_efl() & FL_IF);*/
    kmt->spin_lock(&yk);
    _Context *ret = NULL;
    for(int i = 0; i < cnthandler; i++){
        if(schandlers[i].event == _EVENT_NULL || schandlers[i].event == ev.event){
#ifdef OSDEBUG
            kmt->spin_lock(&pk);
            /*Logp("evid %d", ev.event);*/
            /*Logp("in os_trap");*/
            /*if(ev.event != 2 && ev.event != 5)*/
                /*printf("ev.event %d\n", ev.event);*/
            /*Logp("schandlers event id %d seq %d",schandlers[i].event, schandlers[i].seq);*/
            kmt->spin_unlock(&pk);
#endif
            _Context *next = schandlers[i].handler(ev, context);
            if(next) ret = next;
        }
    }
    /*ret = context;*/
    assert(!(get_efl() & FL_IF));
    kmt->spin_unlock(&yk);
    /*kmt->spin_lock(&pk);*/
    /*assert(get_efl() & FL_IF);*/
    /*kmt->spin_lock(&pk);*/
    /*assert(lk.locked == 0);*/
    /*assert(yk.locked == 0);*/
    /*assert(fillsem.locked.locked == 0);*/
    /*assert(emptysem.locked.locked == 0);*/
    /*kmt->spin_unlock(&pk);*/
    assert(ret != NULL);
  return ret;
    /*return context;*/
}

void os_irq_add(int seq, int event, handler_t handler){
    /*Logg("cnthandler, %d",cnthandler);*/
    schandlers[cnthandler].seq = seq;
    schandlers[cnthandler].event = event;
    schandlers[cnthandler].handler = handler;
    cnthandler++;
}

static void os_on_irq(int seq, int event, handler_t handler) {
    /*kmt->spin_lock(&pk);*/
    /*Logb("os_irq, seq: %d event: %d", seq, event);*/
    /*kmt->spin_unlock(&pk);*/
    int i = 0;
    if(cnthandler == 0){
        /*Logy("%d seq: %d", cnthandler, seq);*/
        os_irq_add(seq, event, handler);
        /*Logy("0seq: %d", schandlers[0]->seq);*/
        return;
    }
    for(; i < cnthandler; i++){
        if(schandlers[i].seq > seq)
            break;
    }
    if(i == cnthandler){
        /*Logy("i: %d seq: %d %d", i, seq, cnthandler);*/
        /*Logg("%d", schandlers[0]->seq);*/
        os_irq_add(seq, event, handler);
        /*Logy("%d", schandlers[i]->seq);*/
        /*Logg("%d", schandlers[0]->seq);*/
    }else{
        /*assert(0);*/
        for(int k = cnthandler; k > i; k--){
            schandlers[k] = schandlers[k - 1];
        }
        schandlers[i].seq = seq;
        schandlers[i].event = event;
        schandlers[i].handler = handler;
        cnthandler++;
    }
    for(int k = 0; k < cnthandler - 1; k++){
        /*Logy("MIN: %d MAX: %d, %d", INT_MIN, INT_MAX, cnthandler);*/
        /*Logy("%d %d %d", k, schandlers[0].seq, schandlers[1].seq);*/
        assert(schandlers[k].seq <= schandlers[k + 1].seq);
    }
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};

#include <common.h>
#include <klib.h>
extern struct spinlock pk;
int cnthandler = 0;
struct handlers *schandlers[MAXHANDLER];

static void os_init() {
  pmm->init();
  kmt->init();
  /*_vme_init(pmm->alloc, pmm->free);*/
  /*dev->init();*/
}
/*
static void hello() {
  for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');
}
*/
#define test_ptr_nr 1024
#define testnum 100
void test(){
    /*assert(0);*/
    //spin_lock(&pk);
    /*assert(0);*/
    //Logb("TEST START");
    //spin_unlock(&pk);
    /*assert(0);*/
    void *space[testnum];
    int i;
    for(i=0;i<testnum;i++){
        space[i] = pmm->alloc(rand()%((1<<20)-1));
    }
    for(i=0;i<1000;i++){
        int tmp = rand()%100;
        pmm->free(space[tmp]);
        space[tmp] = pmm->alloc((rand()&((1<<30)-1)));
    }
    for(i=0;i<testnum;i++){
        pmm->free(space[i]);
    }
    spin_lock(&pk);
    Logb("TEST FINISH");
    spin_unlock(&pk);
}

static void os_run() {
  /*hello();*/
  test();
  _intr_write(1);
  while (1) {
      /*assert(0);*/
    _yield();
  }
}

static _Context *os_trap(_Event ev, _Context *context) {
    _Context *ret = NULL;
    for(int i = 0; i < cnthandler; i++){
        if(schandlers[i]->event == _EVENT_NULL || schandlers[i]->event == ev.event){
            _Context *next = schandlers[i]->handler(ev, context);
            if(next) ret = next;
        }
    }
    ret = context;
  return ret;
    /*return context;*/
}

void os_irq_add(int seq, int event, handler_t handler){
    schandlers[cnthandler]->seq = seq;
    schandlers[cnthandler]->event = event;
    schandlers[cnthandler]->handler = handler;
    cnthandler++;
}

static void os_on_irq(int seq, int event, handler_t handler) {
    int i = 0;
    if(cnthandler == 0){
        Logy("%d seq: %d", cnthandler, seq);
        os_irq_add(seq, event, handler);
        Logy("0seq: %d", schandlers[0]->seq);
        return;
    }
    for(; i < cnthandler; i++){
        if(schandlers[i]->seq > seq)
            break;
    }
    if(i == cnthandler){
        Logy("i: %d seq: %d", i, seq);
        os_irq_add(seq, event, handler);
        Logy("%d", schandlers[i]->seq);
    }else{
        assert(0);
        for(int k = cnthandler; k > i; k++){
            schandlers[k] = schandlers[k - 1];
        }
        schandlers[i]->seq = seq;
        schandlers[i]->event = event;
        schandlers[i]->handler = handler;
        cnthandler++;
    }
    for(int k = 0; k < cnthandler - 1; k++){
        Logy("MIN: %d MAX: %d, %d", INT_MIN, INT_MAX, cnthandler);
        Logy("%d %d %d", k, schandlers[k]->seq, schandlers[k + 1]->seq);
        assert(schandlers[k]->seq <= schandlers[k + 1]->seq);
    }
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};

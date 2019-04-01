#include <common.h>
#include <klib.h>

static void os_init() {
  pmm->init();
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
    spin_lock(lk);
    assert(0);
    Logb("TEST START");
    spin_unlock(lk);
    assert(0);
    void *space[testnum];
    int i;
    for(i=0;i<testnum;i++){
        space[i] = pmm->alloc(rand()%((1<<6)-1));
    }
    for(i=0;i<10;i++){
        int tmp = rand()%100;
        pmm->free(space[tmp]);
        space[tmp] = pmm->alloc((rand()&((1<<10)-1)));
    }
    for(i=0;i<100;i++){
        pmm->free(space[i]);
    }
    spin_lock(lk);
    Logb("TEST FINISH");
    spin_unlock(lk);
}

static void os_run() {
  //hello();
  test();
  _intr_write(1);
  while (1) {
    _yield();
  }
}

static _Context *os_trap(_Event ev, _Context *context) {
  return context;
}

static void os_on_irq(int seq, int event, handler_t handler) {
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};

#include <common.h>
#include <klib.h>
#define ST_SIZE 4096
#define TKNUM 25

extern spinlock_t pk;

int tottask = 0;
task_t *cputask[TKNUM];
task_t *current_task[4];
#define current (current_task[_cpu()])

void kmt_init(){
    os->on_irq(INT_MIN, _EVENT_NULL, kmt_context_save); // 总是最先调用
    os->on_irq(INT_MAX, _EVENT_NULL, kmt_context_switch); // 总是最后调用

    for(int i = 0; i < 4; i++)
        current_task[i] = NULL;
}

int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    task->id = tottask;
    task->name = name;
    /*task->stack = (char *)pmm->alloc(ST_SIZE);*/
    task->fence1 = 0xcc;
    task->fence2 = 0xcc;
    task->entry = entry;
    task->arg = arg;
    task->state = FREET;
    cputask[tottask] = task;

    _Area sstack;
    sstack->start = (void *)task->stack;
    sstack->end = (void *)(task->stack) + ST_SIZE;
    task->context = kcontext(sstack, entry, arg);

    tottask++;
    kmt->spin_lock(&pk);
    Logy("kmt_create name: %s with id: %d", name, tottask - 1);
    kmt->spin_unlock(&pk);
    return 0;
}

void kmt_teardown(task_t *task){
    /*pmm->free((void *)task->stack);*/
    return;
}


MODULE_DEF(kmt) {
    .init = kmt_init,
    .create = kmt_create,
    .teardown = kmt_teardown,
    .spin_init = spin_init,
    .spin_lock = spin_lock,
    .spin_unlock = spin_unlock,
    .sem_init = sem_init,
    .sem_wait = sem_wait,
    .sem_signal = sem_signal,
};


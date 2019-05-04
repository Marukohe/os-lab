#include <common.h>
#include <klib.h>
#include <spinlock.h>

void kmt_init(){
    return;
}

int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    return 1;
}

void kmt_teardown(task_t *task){
    return;
}


MODULE_DEF(kmt) {
    .init = kmt_init,
    .create = kmt_create,
    .teardown = kmt_theardown,
    .spin_init = spin_init,
    .spin_lock = spin_lock,
    .spin_unlock = spin_unlock,
    .sem_init = sem_init,
    .sem_wait = sem_wait,
    .sem_signal = sem_signal,
};


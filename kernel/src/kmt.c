#include <common.h>
#include <klib.h>
#include <spinlock.h>


MODULE_DEF(kmt) {
    .init = kmt_init,
    .creat = kmt_creat,
    .theardown = kmt_theardown,
    .spin_init = spin_init,
    .spin_lock = spin_lock,
    .spin_unlock = spin_unlock,
    .sem_init = sem_init,
    .sem_wait = sem_wait,
    .sem_signal = sem_signal,
};


#include <common.h>
#include <klib.h>

void spin_lock(spinlock_t lk){
    cli();
    while(_atomic_xchg(&lk->locked, 1));
}

void spin_unlock(spinlock_t lk){
    _atomic_xchg(&lk->locked, 0);
    sti();
}

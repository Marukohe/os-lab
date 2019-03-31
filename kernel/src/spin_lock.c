#include <common.h>
#include <klib.h>

void spin_lock(spinlock *lk){
    cli();
    while(_atomic_xchg(lk->locked, 1));
}

void spin_unlock(spinlock *lk){
    _atomic_xchg(&lk->locked, 0);
    sti();
}

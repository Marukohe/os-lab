#include <common.h>
#include <klib.h>

void pushcli(){
    cli();
    spincnt[_cpu()]++;
}

void popcli(){
    assert(spincnt[_cpu]!=0);
    spincnt[_cpu()]--;
    if(spincnt[_cpu()]==0)
        sti();
}

void spin_lock(spinlock *lk){
    //assert(lk->locked==0);
    Logb("spinlock");
    /*cli();*/
    pushcli();
    while(_atomic_xchg(&lk->locked, 1));
}

void spin_unlock(spinlock *lk){
    _atomic_xchg(&lk->locked, 0);
    /*sti();*/
    popcli();
}

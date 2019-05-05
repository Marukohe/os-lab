#include <common.h>
#include <klib.h>
#include <spinlock.h>

int spincnt[5]={};

void pushcli(){
    cli();
    spincnt[_cpu()]++;
    /*printf("push %d %d\n",_cpu(), spincnt[_cpu()]);*/
}

void popcli(){
    assert(spincnt[_cpu()]!=0);
    spincnt[_cpu()]--;
    if(spincnt[_cpu()]==0){
        /*printf("unlock %d\n"),_cpu();*/
        sti();
    }
}

void spin_init(spinlock_t *lk, const char *name){
    return;
}

void spin_lock(spinlock_t *lk){
    /*cli();*/
    pushcli();
    while(_atomic_xchg(&lk->locked, 1));
    /*printf("spinlock %d\n",_cpu());*/
    __sync_synchronize();
}

void spin_unlock(spinlock_t *lk){
    _atomic_xchg(&lk->locked, 0);
    __sync_synchronize();
    /*sti();*/
    popcli();
}

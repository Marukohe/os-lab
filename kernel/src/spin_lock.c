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

void spin_lock(spinlock *lk){
    /*assert(0);*/
    //printf("%d\n",_cpu());
    /*assert(lk->locked==0);*/
    /*printf("spinlock %d\n",_cpu());*/
    /*cli();*/
    pushcli();
    while(_atomic_xchg(&lk->locked, 1));
    /*printf("spinlock %d\n",_cpu());*/
}

void spin_unlock(spinlock *lk){
    _atomic_xchg(&lk->locked, 0);
    /*sti();*/
    popcli();
}

#include <common.h>
#include <klib.h>
#include <spinlock.h>
#define SMALLSIZE 256
#define CPUNUM 4
#define BLOCK 4*1024
#define STSIZE 20

typedef struct Kmem{
    uintptr_t start;
    size_t size;
    //size_t maxsize;
    enum BLOCKSTATE state;
    struct Kmem *next;
    struct Kmem *prev;
}kmem;

static uintptr_t start;
static uintptr_t pm_start, pm_end;
spinlock *lk;
spinlock *pk;  //for printf

kmem *smem[CPUNUM];
kmem *lmem;
kmem sizetest;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  /*Logp("pmm_init successfully, from pm_start 0x%x to pm_end 0x%x",pm_start,pm_end);*/
  /*Logb("struct size %ld",sizeof(sizetest));*/

  start = pm_start;
  lk->locked = 0;
  pk->locked = 0;
  /*assert(lk->locked==0);*/
  for(int i=0;i<CPUNUM;i++)
      spincnt[i] = 0;
  /*assert(0);*/
  for(int i=0;i<CPUNUM;i++){
        //smem[i]->maxsize = 0;
        smem[i]->start = 0;
        smem[i]->size = 0;
        smem[i]->state = FREE;
        smem[i]->next = NULL;
        smem[i]->prev = NULL;
  }
  assert(lk->locked==0);
  assert(pk->locked==0);
  lmem->start = pm_start;
  lmem->size = pm_end-pm_start;
  lmem->state = FREE;
  lmem->next = NULL;
  lmem->prev = NULL;
  /*Logb("here");*/
  //lk->locked = 0;
  //pk->locked = 0;
  _putc('0'+lk->locked);
  _putc('\n');
  _putc('0'+(int)lk->locked==0);
  _putc('\n');
  assert(lk->locked==0);
  assert(pk->locked==0);
  Logb("here");
}

static void *my_bigalloc(size_t size){
    spin_lock(lk);
    void *ret;
    int k = size/BLOCK;
    size_t ssize = (k+1)*BLOCK;
    uintptr_t sstart = lmem->start+lmem->size-ssize;
    kmem *newalloc = (kmem *)((void *)(sstart-STSIZE));
    newalloc->start = sstart;
    newalloc->size = ssize;
    lmem->size = lmem->size - ssize - STSIZE;
    if(lmem->next==NULL)
        newalloc->next = NULL;
    else{
        lmem->next->prev = newalloc;
        newalloc->next = lmem->next;
    }
    newalloc->prev = lmem;
    lmem->next = newalloc;
    newalloc->state = USING;
    ret = (void *)sstart;
    spin_unlock(lk);
    return ret;
}

static void *my_smallalloc(size_t size){
    int cpu = _cpu();
    //小内存分配
    void *ret;
    size_t ssize = size + STSIZE;
    size_t minsize = BLOCK;
    kmem *tmp = NULL;
    kmem *head = smem[cpu];
    while(head!=NULL){
        if(ssize<=head->size && head->state==FREE){
            if(minsize > head->size){
                tmp = head;
                minsize = head->size;
            }
        }
        head = head->next;
    }
    //申请新的4k页
    if(tmp == NULL){
        void *new = my_bigalloc(size);
        kmem *newpage = (kmem *)(new-STSIZE);
        newpage->start = FREE;
        if(smem[cpu]->next==NULL){
            newpage->next = NULL;
        }else{
            smem[cpu]->next->prev = newpage;
            newpage->next = smem[cpu]->next;
        }
        newpage->prev = smem[cpu];
        smem[cpu]->next = newpage;
        newpage->size = newpage->size-ssize;
        void *addr = (void *)(newpage->start+newpage->size-ssize);
        kmem *myalloc =(kmem *)addr;
        myalloc->state = USING;
        myalloc->start = newpage->size+newpage->start-size;
        myalloc->size = size;
        if(newpage->next==NULL){
            myalloc->next = NULL;
        }else{
            newpage->next->prev = myalloc;
            myalloc->next = newpage->next;
        }
        newpage->next = myalloc;
        myalloc->prev = newpage;
        ret = (void *)myalloc->start;
    }else{
        //在处理器的内存中分配
        tmp->size = head->start-ssize;
        void *addr = (void *)(tmp->start+tmp->size-ssize);
        kmem *myalloc = (kmem *)addr;
        myalloc->state = USING;
        myalloc->start = tmp->size+tmp->start-size;
        myalloc->size = size;
        if(tmp->next==NULL){
            myalloc->next = NULL;
        }else{
            tmp->next->prev = myalloc;
            myalloc->next = tmp->next;
        }
        myalloc->prev = tmp;
        tmp->next = myalloc;
        ret = (void *)myalloc->start;
    }

    return ret;
}

static void *kalloc(size_t size) {
#ifdef CORRECTNESS_FIRST
    spin_lock(lk);
    void *ret = (void *)start;
    start += size;
    spin_unlock(lk);
    return ret;
#else
    /*spin_lock(lk);*/
    void *ret;
    if(size > SMALLSIZE){
        ret = my_bigalloc(size);
    }else{
        ret = my_smallalloc(size);
    }
    /*spin_unlock(lk);*/
    return ret;
#endif
  /*return NULL;*/
}

static void kfree(void *ptr) {
#ifdef CORRECTNESS_FIRST
    return;
#else
    return;
#endif
    //ret
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};

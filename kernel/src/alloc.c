#include <common.h>
#include <klib.h>
#define SMALLSIZE 256
#define CPUNUM 4
#define BLOCK 4*1024
#define STSIZE 40

typedef struct Kmem{
    uintptr_t start;
    uintptr_t size;
    uintptr_t maxsize;
    struct Kmem *next;
    struct Kmem *prev;
}kmem;

static uintptr_t start;
static uintptr_t pm_start, pm_end;

kmem smem[CPUNUM];
kmem lmem;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  Logp("pmm_init successfully, from pm_start 0x%x to pm_end 0x%x",pm_start,pm_end);
  Logb("struct size %ld",sizeof(lmem));

  start = pm_start;
  lk->locked = 0;
}

static void *my_bigalloc(size_t size){
    return NULL;
}

static void *my_smallalloc(size_t size){
    return NULL;
}

static void *kalloc(size_t size) {
#ifdef CORRECTNESS_FIRST
    spin_lock(lk);
    void *ret = (void *)start;
    start += size;
    spin_unlock(lk);
    return ret;
#else
    spin_lock(lk);
    void *ret;
    if(size > SMALLSIZE){
        ret = my_bigalloc(size);
    }else{
        ret = my_smallalloc(size);
    }
    spin_unlock(lk);
    return ret;
#endif
  /*return NULL;*/
}

static void kfree(void *ptr) {
#ifdef CORRECTNESS_FIRST
    return;
#else

#endif
    //ret
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};

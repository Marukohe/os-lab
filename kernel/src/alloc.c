#include <common.h>
#include <klib.h>

typedef struct Kmem{
    uintptr_t start;
    uintptr_t size;
    struct Kmem *next;
    struct Kmem *prev;
}kmem;

static uintptr_t start;
static uintptr_t pm_start, pm_end;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  Logp("pmm_init successfully, from pm_start 0x%x to pm_end 0x%x",pm_start,pm_end);

  start = pm_start;
  lk->locked = 0;
}

static void *kalloc(size_t size) {
#ifdef CORRECTNESS_FIRST
    spin_lock(&lk);
    void *ret = start;
    start += size;
    spin_unlock(&lk);
    return ret;
#else
    return NULL;
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

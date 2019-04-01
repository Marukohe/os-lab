#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  Logp("pmm_init successfully, from pm_start 0x%x to pm_end 0x%x",pm_start,pm_end);
}

static void *kalloc(size_t size) {
#ifdef CORRECTNESS_FIRST
    //void *ret;
    //lock(&malloc_lock);
    //ret = my_alloc(size);
    //unlock(&malloc_lock);
#else
#endif
  return NULL;
}

static void kfree(void *ptr) {
#ifdef CORRECTNESS_FIRST

#else

#endif
    //ret
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};

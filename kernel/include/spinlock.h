/*
struct spinlock{
    volatile intptr_t locked;
    char name[10];
};
*/

void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
void pushcli();
void popcli();


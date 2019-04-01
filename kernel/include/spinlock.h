
typedef struct spinlock{
    intptr_t locked;
    char name[10];
}spinlock, *spinlock_t;

spinlock *lk;
spinlock *pk;  //for printf
int spincnt[5];

void spin_lock(spinlock *lk);
void spin_unlock(spinlock *lk);
void pushcli();
void popcli();

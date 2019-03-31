
typedef struct spinlock{
    intptr_t locked;
    char name[10];
}spinlock, *spinlock_t;

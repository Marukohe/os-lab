#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>
#include <kmt.h>
#include <os.h>

#define MAXCPU 4
#define MAXHANDLER 10
#define INT_MAX 0x7ffffff
#define INT_MIN (-INT_MAX - 1)
enum BLOCKSTATE{FREE,USING,READY};

//#define DEBUG
// #define CORRECTNESS_FIRST
struct task {
    const char *name;
    _Context context;
    char *stack;
    void (*entry)(void *arg);
    void *arg;
};

struct spinlock {
    volatile intptr_t locked;
    const char *name;
    int cpu;
};


struct semaphore {
    const char *name;
};

//=======================handler====================
_Context *kmt_context_save(_Event ev, _Context *context);
_Context *kmt_context_switch(_Event ev, _Context *context);

#endif

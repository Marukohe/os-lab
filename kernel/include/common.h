#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>
#include <kmt.h>

#define MAXCPU 4
enum BLOCKSTATE{FREE,USING,READY};

//#define DEBUG
// #define CORRECTNESS_FIRST
struct task {};
struct spinlock {
    volatile intptr_t locked;
    const char *name;
    int cpu;
};

int ncli[MAXCPU] = {};
int intena[MAXCPU] = {};

struct semaphore {
    const char *name;
};

#endif

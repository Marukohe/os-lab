#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>
#include <spinlock.h>

enum BLOCKSTATE{FREE,USING,READY};

//#define DEBUG
// #define CORRECTNESS_FIRST
struct task {};
struct spinlock {
    volatile intptr_t locked;
    char name[10];
};

struct semaphore {};

#endif

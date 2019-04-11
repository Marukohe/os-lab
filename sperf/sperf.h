#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
//#include <assert.h>

#include "debug.h"

typedef struct {
    char name[100];
    double t;
}_trace_item;

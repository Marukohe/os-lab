#include <am.h>
#include <amdev.h>
#include <klib.h>

//#define SIDE 16
#define SIDE 16
#define maxdisp 400
//0 for x 1 for y
/*
struct snake{
	int head[3];
	int tail[3];
	int pivot[20][2]; //turning point
	int pnum_start;
	int pnum_end;
	int pnum;
	int dire_head;   // 0 up, 1 right, 2 down, 3 left 
	int dire_tail;
};
*/

int disp[maxdisp][maxdisp]={};
int sshx[100]={};
int sshy[100]={};

struct snake{
	int head[3];
	int tail[3];
	//int pivot[20][2]; turning point
	//int pnum_start;
	//int pnum_end;
	//int pnum;
	int l;
	int ss;
	int se;
	int dire_head;   // 0 up, 1 right, 2 down, 3 left 
	int dire_tail;
};

static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}


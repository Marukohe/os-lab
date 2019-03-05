#include <am.h>
#include <amdev.h>
#include <klib.h>

//#define SIDE 16
#define SIDE 16
#define FSIZE 4
#define maxdisp 400
//0 for x 1 for y

enum color{
	GREEN = 0x869900,
	WITHE = 0xffffff,
	RED = 0xc73e3a,
	BLACK = 0x000000
};

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

struct food{
	int x;
	int y;
	int size;  // 0 1;
};

static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}

#define Log(format, ...) \
  printf("\33[1;35m[%s,%d,%s] " format "\33[0m\n", \
      __FILE__, __LINE__, __func__, ## __VA_ARGS__)


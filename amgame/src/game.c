#include <game.h>

#define FAIL 233
#define SUCC 666
#define maxl 50

int w, h;

void draw_rects(int x, int y, int w, int h, uint32_t color);
void init_screen();
void splash();
int read_keys();
int rand_red();

struct snake snake_;
struct food foody;
void init_snake();
int update_snake(int op);

static inline void draw_character(char ch, int x, int y, int color) {
  int i, j;
  char *p = font8x8_basic[(int)ch];
  for (i = 0; i < 8; i ++)
    for (j = 0; j < 8; j ++)
      if ((p[i] >> j) & 1)
        if (x + j < w && y + i < h)
          draw_rects(x+j,y+i,1,1,color);
}

static inline void draw_string(const char *str, int x, int y, int color) {
  while (*str) {
    draw_character(*str ++, x, y, color);
    if (x + 8 >= w) {
      y += 8; x = 0;
    } else {
      x += 8;
    }
  }
}

int main() {
  // Operating system is a C program
  _ioe_init();
  init_screen();
  init_snake();
  //printf("%d %d\n",snake_.head[0],snake_.head[1]);
  Log("The length of snake is : %d",snake_.l);
  //splash();
  draw_string("you loss the game",SIDE,SIDE,GREEN);
  int op=76;
  int is_fail=0;
  unsigned long long current = 0;
  unsigned long long last = 0;
  while (1) {
	int tmp = read_keys();
    if(73<=tmp && tmp<=76)
		op = tmp;
	current = uptime();
	if(current - last>=300){
		if(is_fail==SUCC)
			break;
		is_fail=update_snake(op);
		//Log("%d",snake_.l);
		if(is_fail==FAIL){
			//draw_string("you loss the game",SIDE,SIDE,GREEN);
			Log("Fail. Please try it again.");
			break;
		}
		if(is_fail==SUCC){
			//splash();
			Log("Amazing! You pass the game.");
			//break;
		}
		last = current;
		splash();
		//draw_string("you lose the game",SIDE,SIDE,GREEN);
	}
  }
  //splash();
  return 0;
}

int read_keys() {
  _DEV_INPUT_KBD_t event = { .keycode = _KEY_NONE };
  #define KEYNAME(key) \
    [_KEY_##key] = #key,
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &event, sizeof(event));
  if (event.keycode != _KEY_NONE && event.keydown) {
	return event.keycode;
  }
	return 0;
}


void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
  //Log("w: %d h: %d side: %d",w,h,SIDE);
}

int rand_red(){
	int redx = rand()%(w/SIDE);
	int redy = rand()%(h/SIDE);
	while(disp[redx][redy]==1){
		redx = rand();
		redy = rand();
	}
	foody.x = redx;
	foody.y = redy;
	return 1;
}

void init_snake(){
	snake_.head[0] = (w/SIDE)/2;
	snake_.head[1] = (h/SIDE)/2;
	snake_.tail[0] = (w/SIDE)/2-1;
	snake_.tail[1] = (h/SIDE)/2;
	snake_.dire_head = 1;
	snake_.dire_tail = 1;
	disp[snake_.head[0]][snake_.head[1]]=1;
	disp[snake_.tail[0]][snake_.tail[1]]=1;
	snake_.ss = 0;
	snake_.se = 1;
	sshx[snake_.ss] = snake_.tail[0];
	sshy[snake_.ss] = snake_.tail[1];
	sshx[snake_.se] = snake_.head[0];
	sshy[snake_.se] = snake_.head[1];
	snake_.l = 2;
	rand_red();
	//Log("food position: x %d y %d",foody.x,foody.y);
}

void draw_rects(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: allocated on stack
  _DEV_VIDEO_FBCTL_t event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &event, sizeof(event));
}

void draw_food(int x,int y){
	for(int j=0;j<4;j++)
		draw_rects(x+j*FSIZE, y, FSIZE, FSIZE, WITHE);
	for(int i=1;i<=2;i++){
		draw_rects(x, y+i * FSIZE, FSIZE, FSIZE, WITHE);
		draw_rects(x+1 * FSIZE, y+i * FSIZE, FSIZE, FSIZE, RED);
		draw_rects(x+2 * FSIZE, y+i * FSIZE, FSIZE, FSIZE, RED);
		draw_rects(x+3 * FSIZE, y+i * FSIZE, FSIZE, FSIZE, WITHE);
	}
	for(int j=0;j<4;j++)
		draw_rects(x+j*FSIZE, y+3*FSIZE, FSIZE, FSIZE, WITHE);
}

void splash() {
  for (int x = 0; x * SIDE <= w-SIDE; x ++) {
    for (int y = 0; y * SIDE <= h-SIDE; y++) {
	  	if(x==foody.x && y == foody.y){
			draw_food(x*SIDE,y*SIDE);
			continue;
		}else if(x==snake_.head[0] && y==snake_.head[1]){
	  		draw_rects(x * SIDE, y * SIDE, SIDE, SIDE, ORANGE); //green
			continue;
		}
			
	    if(disp[x][y]==1)
	  		draw_rects(x * SIDE, y * SIDE, SIDE, SIDE, GREEN); //green
	    else
	  		draw_rects(x * SIDE, y * SIDE, SIDE, SIDE, WITHE); //blue
		if((y+1)*SIDE-h>0&&(y+1)*SIDE-h<SIDE)
	  		draw_rects(x * SIDE, (y+1) * SIDE, SIDE, SIDE, BLACK); 
    }
  	draw_rects(x * SIDE, h/SIDE * SIDE, SIDE, SIDE, BLACK); 
  }
  for (int y = 0; y * SIDE <= h; y++) {
 	 draw_rects(w/SIDE*SIDE, y * SIDE, SIDE, SIDE, BLACK); 
  }
}

void uph(){
	snake_.se = (snake_.se+1) % maxl;
	sshx[snake_.se] = snake_.head[0];
	sshy[snake_.se] = snake_.head[1];
}

int update_snake(int op){
	int flag = 0;
	switch(op){
		case 73:
			switch(snake_.dire_head){
				case 0: case 1: case 3:    //up
					snake_.head[1]-=1;
					if(snake_.head[1]<0 || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]]=1;
					snake_.dire_head = 0;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				case 2:                    //down
					snake_.head[1] +=1;
					if(snake_.head[1]>=h/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]]=1;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				default:
					break;
			}
			break;
		case 74:
			switch(snake_.dire_head){
				case 0:
					snake_.head[1]-=1;
					if(snake_.head[1]<0 || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]]=1;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				case 1: case 2: case 3:
					snake_.head[1] +=1;
					if(snake_.head[1]>=h/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]]=1;
					snake_.dire_head = 2;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				default:
					break;
			}
			break;
		case 75:
			switch(snake_.dire_head){
				case 0: case 2: case 3:
					snake_.head[0] -= 1;
					if(snake_.head[0]<0 || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]] = 1;
					snake_.dire_head = 3;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				case 1:
					snake_.head[0] += 1;
					if(snake_.head[0]>=w/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]] = 1;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				default:
					break;
			}
			break;
		case 76:
			switch(snake_.dire_head){
				case 0: case 1: case 2:
					snake_.head[0] += 1;
					if(snake_.head[0]>=w/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]] = 1;
					snake_.dire_head = 1;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				case 3:
					snake_.head[0] -= 1;
					if(snake_.head[0]<0 || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]] = 1;
					if(foody.x == snake_.head[0] && foody.y == snake_.head[1]){
						flag = 1;
						rand_red();
					}
					uph();
					break;
				default:
					break;
			}
			break;
		default: break;
	}
	if(!flag){
		disp[snake_.tail[0]][snake_.tail[1]]=0;
		snake_.ss = (snake_.ss+1) % maxl;
		snake_.tail[0] = sshx[snake_.ss];
		snake_.tail[1] = sshy[snake_.ss];
	}
	else{
		snake_.l += 1;
		Log("The length of sanke: %d",snake_.l);
	}
	if(snake_.l==30){
	//	splash();
		return SUCC;
	}
	else return 0;
}

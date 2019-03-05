#include <game.h>

#define FAIL 233
#define SUCC 666
#define maxl 50

int w, h;

void init_screen();
void splash();
int read_keys();
int rand_red();

struct snake snake_;
struct food foody;
void init_snake();
int update_snake(int op);

int main() {
  // Operating system is a C program
  _ioe_init();
  init_screen();
  init_snake();
  printf("%d %d\n",snake_.head[0],snake_.head[1]);
  splash();
  //char ch;
  int op=76;
  unsigned long long current = 0;
  unsigned long long last = 0;
  while (1) {
	int tmp = read_keys();
    if(73<=tmp && tmp<=76)
		op = tmp;
	current = uptime();
	if(current - last>=400){
		//printf("op:     %d\n",op);
		Log("op  :%d",op);
		update_snake(op);
		Log("head direcrion of the snake: %d",snake_.dire_head);
		last = current;
		splash();
	}
  }
  return 0;
}

int read_keys() {
  _DEV_INPUT_KBD_t event = { .keycode = _KEY_NONE };
  #define KEYNAME(key) \
    [_KEY_##key] = #key,
  static const char *key_names[] = {
    _KEYS(KEYNAME)
  };
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &event, sizeof(event));
  if (event.keycode != _KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
	printf("%d",event.keycode);
    puts("\n");
	return event.keycode;
  }
	return 0;
}


void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
  printf("w: %d h: %d\n",w,h);
}

int rand_red(){
	int redx = rand();
	int redy = rand();
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
	Log("food position: x %d y %d",foody.x,foody.y);
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
		draw_rects(x * FSIZE, (y+j) * FSIZE, FSIZE, FSIZE, WITHE);
	for(int i=1;i<=2;i++){
		draw_rects((x+i) * FSIZE, y * FSIZE, FSIZE, FSIZE, WITHE);
		draw_rects((x+i) * FSIZE, (y+1) * FSIZE, FSIZE, FSIZE, RED);
		draw_rects((x+i) * FSIZE, (y+2) * FSIZE, FSIZE, FSIZE, RED);
		draw_rects((x+i) * FSIZE, (y+3) * FSIZE, FSIZE, FSIZE, WITHE);
	}
	for(int j=0;j<4;j++)
		draw_rects(x * FSIZE, (y+j) * FSIZE, FSIZE, FSIZE, WITHE);
}

void splash() {
  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
	  	if(x==foody.x && y == foody.y)
			draw_food(x,y);
	    if(disp[x][y]==1)
	  		draw_rects(x * SIDE, y * SIDE, SIDE, SIDE, GREEN); //green
	    else
	  		draw_rects(x * SIDE, y * SIDE, SIDE, SIDE, WITHE); //blue
	  		//draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0x268bd2); //blue
    }
  }
}

void uph(){
	snake_.se = (snake_.se+1) % maxl;
	sshx[snake_.se] = snake_.head[0];
	sshy[snake_.se] = snake_.head[1];
}

int update_snake(int op){
	switch(op){
		case 73:
			switch(snake_.dire_head){
				case 0: case 1: case 3:    //up
					snake_.head[1]-=1;
					if(snake_.head[1]<0 || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]]=1;
					snake_.dire_head = 0;
					uph();
					break;
				case 2:                    //down
					snake_.head[1] +=1;
					if(snake_.head[1]>h/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]]=1;
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
					uph();
					break;
				case 1: case 2: case 3:
					snake_.head[1] +=1;
					if(snake_.head[1]>h/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]]=1;
					snake_.dire_head = 2;
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
					uph();
					break;
				case 1:
					snake_.head[0] += 1;
					if(snake_.head[0]>w/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]] = 1;
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
					if(snake_.head[0]>w/SIDE || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]] = 1;
					snake_.dire_head = 1;
					uph();
					break;
				case 3:
					snake_.head[0] -= 1;
					if(snake_.head[0]<0 || disp[snake_.head[0]][snake_.head[1]]==1)
						return FAIL;
					disp[snake_.head[0]][snake_.head[1]] = 1;
					uph();
					break;
				default:
					break;
			}
			break;
		default: break;
	}
	disp[snake_.tail[0]][snake_.tail[1]]=0;
	snake_.ss = (snake_.ss+1) % maxl;
	snake_.tail[0] = sshx[snake_.ss];
	snake_.tail[1] = sshy[snake_.ss];
	return SUCC;
}


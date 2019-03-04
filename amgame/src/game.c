#include <game.h>

#define FIAL 233

void init_screen();
void splash();
int read_key();

struct snake snake_;
void init_snake();
void update_snake(int op);

int ps(){
	return snake_.pnum_start%15;
}

int pe(int i){
	return (snake_.pnum_end+15-i)%15;
}

int is_in(int x,int y,int x1,int y1,int x2,int y2){
	if(y1==y2 && y1==y){
		if(x1<=x2 && x1<=x && x<=x2)
			return 1;
		else if(x1>=x2 && x1>=x && x>=x2)
			return 1;
		else
			return 0;
	}
	else if(x1==x2 && x1==x){
		if(y1<=y2 && y1<=y && y<=2)
			return 1;
		else if(y1>=y2 && y1>=y && y>=y2)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

int is_in_p(int x, int y){
	int flag = 0;
	if(snake_.pnum == 0)
		return is_in(x, y, snake_.head[0], snake_.head[1], snake_.tail[0], snake_.tail[1]);
	else{
		flag = is_in(x, y, snake_.head[0], snake_.head[1], snake_.pivot[pe(0)][0], snake_.pivot[pe(0)][1]);
		if(flag==1)
			return 1;
	}
	for(int i=1;i<snake_.pnum-1;i++){
		flag = is_in(x, y, snake_.pivot[pe(i)][0], snake_.pivot[pe(i)][1], snake_.pivot[pe(i+1)][0], snake_.pivot[pe(i+1)][1]);
		if(flag == 1)
			return 1;
	}
	return is_in(x, y, snake_.pivot[ps()][0], snake_.pivot[ps()][1], snake_.tail[0], snake_.tail[1]);
}

int main() {
  // Operating system is a C program
  _ioe_init();
  init_screen();
  init_snake();
  printf("%d %d\n",snake_.head[0],snake_.head[1]);
  splash();
  //char ch;
  int op=0;
  while (1) {
    op=read_key();
	update_snake(op);
  }
  return 0;
}

int read_key() {
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
	//printf("%d",event.keycode);
    puts("\n");
  }
	return event.keycode;
}

int w, h;

void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
  printf("w: %d h: %d\n",w,h);
}

void init_snake(){
	snake_.head[0] = (w/SIDE)/2*SIDE;
	snake_.head[1] = (h/SIDE)/2*SIDE;
	snake_.tail[0] = (w/SIDE)/2*SIDE-SIDE;
	snake_.tail[1] = (h/SIDE)/2*SIDE;
	snake_.pnum = 0;
	snake_.pnum_start = 0;
	snake_.pnum_end = 0;
	snake_.dire_head = 1;
	snake_.dire_tail = 1;
}

void draw_rect(int x, int y, int w, int h, uint32_t color) {
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

void splash() {
  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      //if ((x & 1) ^ (y & 1)) {
      //  draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      //}
	    if(is_in_p(x*SIDE,y*SIDE))
	  		draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0x869900); //green
	    else
	  		draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); //blue
	  		//draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0x268bd2); //blue
    }
  }
}


void update_snake(int op){
	switch(op){
		case 73:
			switch(snake_.dire_head){
				case 0:
					snake_.head[0]-=SIDE;
					if(snake_.head[0]<=0)
						return FAIL;
				case 1:
				case 2: break;
				case 3:
				default:
			}
			switch(snake_.dire){
				case 0:
				case 1:
				case 2:
				case 3:
				default:
			}
		case 74:
			switch(snake_.dire){
				case 0:
				case 1:
				case 2:
				case 3:
				default:
			}
			switch(snake_.dire){
				case 0:
				case 1:
				case 2:
				case 3:
				default:
			}
		case 75:
			switch(snake_.dire){
				case 0:
				case 1:
				case 2:
				case 3:
				default:
			}
			switch(snake_.dire){
				case 0:
				case 1:
				case 2:
				case 3:
				default:
			}
		case 76:
			switch(snake_.dire){
				case 0:
				case 1:
				case 2:
				case 3:
				default:
			}
			switch(snake_.dire){
				case 0:
				case 1:
				case 2:
				case 3:
				default:
			}
		default:
	}
	return;
}

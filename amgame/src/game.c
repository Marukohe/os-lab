#include <game.h>

void init_screen();
void splash();
int read_key();

struct snake snake_;
void init_snake();
void update_snake(int op){};

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
	    if(is_in(x*SIDE,y*SIDE,snake_.head[0],snake_.head[1],snake_.tail[0],snake_.tail[1]))
	  		draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0x869900); //green
	    else
	  		draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); //blue
	  		//draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0x268bd2); //blue
    }
  }
}


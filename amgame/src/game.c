#include <game.h>

#define FAIL 233
#define SUCC 666
#define maxl 50

int w, h;

void init_screen();
void splash();
int read_keys();

struct snake snake_;
void init_snake();
int update_snake(int op);
/*
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
*/
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
	if(current - last>=500){
		printf("op:     %d\n",op);
		update_snake(op);
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
	//printf("%d",event.keycode);
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

void splash() {
  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      //if ((x & 1) ^ (y & 1)) {
      //  draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      //}
	    if(disp[x][y]==1)
	  		draw_rects(x * SIDE, y * SIDE, SIDE, SIDE, 0x869900); //green
	    else
	  		draw_rects(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); //blue
	  		//draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0x268bd2); //blue
    }
  }
}

void uph(){
	snake_.se = (snake_.se+1) % maxl;
	sshx[snake_.se] = snake_.head[0];
	sshy[snake_.se] = snake_.head[1];
}

int diretail(){
	if(snake_.tail[0]==sshx[(snake_.ss+1)%maxl]){
		if(snake_.tail[1]==sshy[(snake_.ss+1)%maxl]+1)
			return 0;
		else if(snake_.tail[1]==sshy[(snake_.ss+1)%maxl]-1)
			return 1;
		else
			printf("001，为什么！？\n");
			
	}
	else if(snake_.tail[1] ==sshy[snake_.ss]){
		if(snake_.tail[0]==sshx[(snake_.ss+1)%maxl]+1)
			return 3;
		else if(snake_.tail[0]==sshx[(snake_.ss+1)%maxl]-1)
			return 1;
		else
			printf("002，为什么！？\n");
	}
	else 
		printf("003，为什么！？\n");
	return FAIL;
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
		default: break;
	}
	int dt = diretail();
	printf("dt:  %d\n",dt);
	switch(dt){
		case 0:
			disp[snake_.tail[0]][snake_.tail[1]]=0;
			snake_.tail[1] -= 1;
			snake_.ss = (snake_.ss+1) % maxl;
			break;
		case 1:
			disp[snake_.tail[0]][snake_.tail[1]]=0;
			snake_.tail[0] += 1;
			snake_.ss = (snake_.ss+1) % maxl;	
		case 2:
			disp[snake_.tail[0]][snake_.tail[1]]=0;
			snake_.tail[1] += 1;
			snake_.ss = (snake_.ss+1) % maxl;
		case 3:
			disp[snake_.tail[0]][snake_.tail[1]]=0;
			snake_.tail[0] -= 1;
			snake_.ss = (snake_.ss+1) % maxl;
		default:
			return FAIL;
	}
	return SUCC;
}


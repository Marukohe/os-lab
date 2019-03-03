#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define is_digit(c) ((c) >= '0' && (c) <='9')
#define ZEROPAD 1
#define SIGN 2
#define PLUS 4
#define SPACE 8
#define LEFT 16
#define SPECIAL 32
#define LARGE 64
static int skip_atoi(const char **s)
{
	int i=0;
	while(is_digit(**s)){
		i = i + *((*s)++)-'0';
	}
	return i;
}


static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char *num_to_string(char *str, long num,int base,int size,int precision,int type){
	char c,sign,tmp[66];
	char *dig = digits;
	int i;
	if(type&LARGE) dig = upper_digits;
	if(type&LEFT) type &= ~ZEROPAD;
	if(base<2||base>36) return 0;

	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if(type&SIGN){
		if(num<0){
			sign = '-';
			num = -num;
			size--;
		}
		else if(type & PLUS){
			sign = '+';
			size--;
		}
		else if(type&SPACE){
			sign = ' ';
			size--;
		}
	}

	if(type&SPECIAL){
		if(base==16)
			size -=2;
		else if(base==8)
			size--;
	}
	i=0;
	if(num==0)
		tmp[i++]='0';
	else{
		while(num!=0){
			tmp[i++] = dig[((unsigned long) num) % (unsigned) base];
			num = ((unsigned long) num) / (unsigned) base;
		}
	}

	if(i>precision) precision=i;
	size -= precision;
	if(!(type&(ZEROPAD|LEFT)))
	{
		while(size-->0) *str++=' ';
	}
	if(sign) *str++ = sign;

	if(type &SPECIAL){
		if(base==8)
			*str++='0';
		else if(base==16)
		{
			*str++='0';
			*str++=digits[33];
		}
	}

	if(!(type&LEFT)){
		while(size-->0) *str++=c;
	}

	while(i<precision--) *str++='0';
	while(i-->0) *str++=tmp[i];
	while(size-->0) *str++=' ';

	return str;

		
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	char *str;
	int field_width;

	for(str=out;*fmt;fmt++){
		unsigned long num;
		int base = 10;
		int flags = 0;
		int qualifier = -1;
		int precision = -1;
	    int bFmt = 1;
		if(*fmt != '%'){
			*str++=*fmt;
			continue;
		}
		bFmt = 1;
		while(bFmt){
			fmt++;
			switch(*fmt){
				case '-': flags |= LEFT; break;
				case '+': flags |= PLUS; break;
				case ' ': flags |= SPACE; break;
				case '#': flags |= SPECIAL; break;
				case '0': flags |= ZEROPAD; break;
				default: bFmt = 0;
			}
		} 
		field_width = -1;
		if(is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if('*'==*fmt){
			fmt++;
			field_width = va_arg(ap,int);
			if(field_width<0){
				field_width = -field_width;
				flags |= LEFT;
		 	}
		}  

		precision=-1;
		if('.'==*fmt){
			fmt++;
			if(is_digit(*fmt)){
				precision = skip_atoi(&fmt);
			}
			else if('*'==*fmt){
				fmt++;
				precision = va_arg(ap,int);
			}
			if(precision<0) precision=0;
		}

		qualifier = -1;
		if('h' == *fmt || 'l'==*fmt || 'L' ==*fmt){
			qualifier = *fmt;
			fmt++;
		}

		base = 10;
		switch(*fmt){
			case 'c':
			{
				if(!(flags & LEFT)) while (--field_width>0) *str++=' ';
				*str++ = (unsigned char) va_arg(ap,int);
				while(--field_width>0) *str++=' ';
				continue;
			}
			case 's':
			{
				int len;
				char *s = va_arg(ap,char *);
				if(!s) s = "<NULL>";
				//len = strnlen(s,precision);
				len = strlen(s);
				if(!(flags & LEFT)) while(len<field_width--) *str++ = ' ';
				for(int i=0;i<len;i++) *str++ = *s++;
				while(len<field_width--) *str++=' ';
				continue;
			}
			case 'p':
			{
				if(field_width==-1)
				{
					field_width=2*sizeof(void*);
					flags|=ZEROPAD;
				}
				str=num_to_string(str,(unsigned long)va_arg(ap,void *),16,field_width,precision,flags);
				continue;
			}
			case 'n':
			{
				if(qualifier=='1')
				{
					long *ip=va_arg(ap,long *);
					*ip=(str-out);
				}
				else
				{
					int *ip=va_arg(ap,int *);
					*ip=(str-out);
				}
				continue;
			}
			case 'o':
			{
				base = 8;
				break;
			}
			case 'X':
			{
				flags |= LARGE;
			}
			case 'x':
			{
				base = 16;
				break;
			}
			case 'd':
			case 'i':
			{
				flags |= SIGN;
			}
			case 'u':
			{
				break;
			}
			default:
			{
				if(*fmt != '%') *str++ = '%';
				if(*fmt)
					*str++=*fmt;
				else
					--fmt;
				continue;
			}
		}

		if(qualifier == 'l')
			num = va_arg(ap,unsigned long);
		else if(qualifier == 'h'){
			if(flags &SIGN)
				num = va_arg(ap,int);//short int
			else
				num = va_arg(ap,unsigned int);//unsignedshort int
		}
		else if(flags &SIGN)
			num=va_arg(ap,int);
		else
			num = va_arg(ap,unsigned long);

		str = num_to_string(str,num,base,field_width,precision,flags);
	}
	*str = '\0';
	return str - out;
}


int sprintf(char *out, const char *fmt, ...) {
	va_list args;
	int res;
	va_start(args,fmt);
	res = vsprintf(out,fmt,args);
	va_end(args);
	return res;
}

int printf(const char *fmt, ...) {
	va_list args;
	int res;
	va_start(args,fmt);
	char s[256];
	res=vsprintf(s,fmt,args);
	va_end(args);
	//size_t cnt=strlen(s);
	for(int i=0;i<res;i++){
		_putc(s[i]);
	}
	return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list args;
  //int res;
  char buf[256];
  va_start(args,fmt);
  vsprintf(buf,fmt,args);
  va_end(args);
  //printf("%s\n",buf);
  //printf("%s\n\n\n\n\n\n\n\n",buf);
  //memcpy(out,buf,n);
  //char dst[65536];
  out=strncpy(out,(const char *)buf,n);
  //strcat(out,(const char *)dst);
  //char *s1;
  //s1 = out;
  //s1 +=n;
  //s1 = '\0';
  return n;
}

#endif

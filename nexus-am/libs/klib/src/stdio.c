#include "klib.h"
#include <stdarg.h>
//#include <assert.h>
//#include <string.h>
//#include <stdio.h>
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
//int m_vsprintf(char *out, const char *fmt, va_list ap);
int printf(const char *fmt, ...) {
  va_list ap;
  char out[256];
  va_start(ap, fmt);
  int n = vsprintf(out, fmt, ap);
  int len = strlen(out);
  for(int i = 0;i < len; i++)
	_putc(out[i]);
  va_end(ap);
  return n;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int fmt_len = strlen(fmt);
  int sign = 0, count = 0;
  for(int i= 0; i < fmt_len; i++){
	switch(fmt[i]) {
	  case '%':
		if(sign == 1) {
		  *out++ = '%';
		  sign = 0;
		} else 
		  sign = 1;
		break;
	  case 'd':
		if(sign == 1) {
		  int tmp = va_arg(ap, int);
		  char buf[128];
		  int buf_len = 0;
		  while(tmp) {
			buf[buf_len ++] = tmp%10 + '0';
			tmp = tmp / 10;
		  }
		  while(buf_len--) *out ++ = buf[buf_len];
		  count ++;
		  sign = 0; 
		} else 
		  *out ++ = 'd';
		break;
	  case 's':
		if(sign == 1) {
		  char *str = va_arg(ap, char*);
		  int len = strlen(str);
		  while(len--) *out++ = *str++;
		  count ++;
		  sign = 0;
		} else 
		  *out++ = 's';
		break;
	  default:
		*out ++ = fmt[i];
	}
  }
  *out = '\0'; 
  return count;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = vsprintf(out, fmt, ap);
  va_end(ap);
  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  //TODO();
  return 0;
}

#endif
/*char buf[128];

int main() {
	m_sprintf(buf, "%s", "Hello world!\n");
	m_printf("%s", "Hello world!\n");
	assert(strcmp(buf, "Hello world!\n") == 0);

	m_sprintf(buf, "%d + %d = %d\n", 1, 1, 2);
	m_printf("%d + %d = %d\n", 1, 1, 2);
	assert(strcmp(buf, "1 + 1 = 2\n") == 0);

	m_sprintf(buf, "%d + %d = %d\n", 2, 10, 12);
	m_printf("%d + %d = %d\n", 2, 10, 12);
	assert(strcmp(buf, "2 + 10 = 12\n") == 0);

	return 0;
}*/

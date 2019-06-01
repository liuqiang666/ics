#include "common.h"
#include "syscall.h"


uintptr_t sys_yield() {
  _yield();
  return 0;
}

void sys_exit(int code) {
  _halt(code);
}

size_t sys_write(int fd, void *buf, size_t len) {
  if(fd == 1 || fd == 2) {
    char *b = (char *)buf;
	for(int i = 0;i < len; i++)
	  _putc(b[i]);
    return len;
  }
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: c->GPR1 = sys_yield();break;
    case SYS_exit:Log("system exit."); sys_exit(a[1]);break;
    case SYS_write: Log("system write.");c->GPR1 = sys_write(a[1], (void *)a[2], a[3]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  
  return NULL;
}

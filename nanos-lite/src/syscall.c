#include "common.h"
#include "syscall.h"

int fs_open(const char *pathname, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
ssize_t fs_write(int fd, const void *buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);
size_t fs_filesz(int fd);
int fs_close(int fd);

uintptr_t sys_yield() {
  _yield();
  return 0;
}

void sys_exit(int code) {
  _halt(code);
}

size_t sys_write(int fd, void *buf, size_t len) {
  /* pa3.2
  if(fd == 1 || fd == 2) {
    char *b = (char *)buf;
	for(int i = 0;i < len; i++)
	  _putc(b[i]);
    return len;
  } */
  return fs_write(fd, buf, len);
}

size_t sys_brk(void* addr) {
  //pa3.2 always return 0
  return 0;

}

int sys_open(const char *pathname, int flags, int mode){
  return fs_open(pathname, flags, mode);
}

ssize_t sys_read(int fd, void *buf, size_t len) {
  return fs_read(fd, buf, len);
}

off_t sys_lseek(int fd, off_t offset, int whence){
  return fs_lseek(fd, offset, whence);
}

int sys_close(int fd) {
  return fs_close(fd);
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
    case SYS_write: c->GPR1 = sys_write(a[1], (void *)a[2], a[3]);break;
    case SYS_brk: c->GPR1 = sys_brk((void *)a[1]);break;
    case SYS_read: c->GPR1 = sys_read(a[1], (void *)a[2], a[3]);break;
    case SYS_open: c->GPR1 = sys_open((char *)a[1], a[2], a[3]);break;
    case SYS_lseek: c->GPR1 = sys_lseek(a[1], a[2], a[3]);break;
    case SYS_close: c->GPR1 = sys_close(a[1]);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  
  return NULL;
}

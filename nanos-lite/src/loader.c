#include "proc.h"
#include "common.h"

#define DEFAULT_ENTRY 0x4000000

/* read `len' bytes starting from `offset' of ramdisk into `buf' */
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
/*return ramdisk size ,byte*/
extern size_t get_ramdisk_size();

int fs_open(const char *pathname, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
size_t fs_filesz(int fd);
int fs_close(int fd);


static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  //size_t size = get_ramdisk_size();
  //ramdisk_read((void *)DEFAULT_ENTRY, 0, size);
  int fd = fs_open(filename, 0, 0);
  size_t bytes = fs_filesz(fd);
  Log("Load %d %s, size: %d.", fd, filename, bytes);
  fs_read(fd, (void *)DEFAULT_ENTRY, bytes);
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}

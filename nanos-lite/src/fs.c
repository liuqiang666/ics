#include "fs.h"

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_DISPINFO, FD_EVENTS, FD_TTY};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = screen_width() * screen_height() * 4; 
  Log("init fs.");
}

size_t fs_filesz(int fd) {
  return file_table[fd].size;
}

int fs_open(const char *pathname, int flags, int mode) {
  for(int i = 0; i < NR_FILES; ++i) {
    if(strcmp(pathname, file_table[i].name) == 0) {
	  file_table[i].open_offset = 0;
      return i;
	}
  }
  panic("No such file: %s\n", pathname);
}

//error:unknown type name 'ssize_t', add #include "unistd.h" in common.h
ssize_t fs_read(int fd, void *buf, size_t len) {
  size_t fs_size = fs_filesz(fd);
  if(file_table[fd].open_offset + len > fs_size)
	len = fs_size - file_table[fd].open_offset;
  if(file_table[fd].read == NULL){// 普通文件 
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  } else 
    len = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  /*switch (fd) {
    case FD_STDOUT:
    case FD_STDERR:
    case FD_STDIN:
    case FD_FB:
      return 0; break;
    default:
      if(file_table[fd].open_offset + len > fs_size)
	    len = fs_size - file_table[fd].open_offset;
      ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
  }*/
  return len;
}

ssize_t fs_write(int fd, const void *buf, size_t len) {
  size_t fs_size = fs_filesz(fd);
  //serial_write does't need consider  offset
  if(fd != FD_STDOUT && fd != FD_STDERR && fd != FD_TTY)
    if(file_table[fd].open_offset + len > fs_size)
	  len = fs_size - file_table[fd].open_offset;
  if(file_table[fd].write == NULL){// 普通文件  
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  } else 
    len = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  /*
  switch (fd) {
    case FD_STDIN:return 0;
    case FD_STDOUT:
    case FD_STDERR:file_table[fd].write(buf, 0, len);break;
    case FD_FB:panic("Please implement me.");break;
    default:
      if(file_table[fd].open_offset + len > fs_size)
	    len = fs_size - file_table[fd].open_offset;
      ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
      file_table[fd].open_offset += len;
      break;
  }*/
  return len;
}

off_t fs_lseek(int fd, off_t offset, int whence){
  off_t result = -1;
  switch (whence) {
    case SEEK_SET:
      if(offset >= 0 && offset <= file_table[fd].size) {
        result = file_table[fd].open_offset = offset;
      }
      break;
    case SEEK_CUR:
      if(offset + file_table[fd].open_offset >=0 && offset + file_table[fd].open_offset <= file_table[fd].size) {
        file_table[fd].open_offset += offset;
        result = file_table[fd].open_offset;
      }
      break;
    case SEEK_END:
      file_table[fd].open_offset = fs_filesz(fd) + offset;
      result = file_table[fd].open_offset;
      break;
    default: panic("No such whence: %d", whence);
  }
  return result;
}

int fs_close(int fd) {
  return 0;
}

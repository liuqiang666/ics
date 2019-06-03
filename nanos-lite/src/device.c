#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *buff = (char *)buf;
  for(int i = 0; i < len; i++) {
    _putc(buff[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int key = read_key();
  bool down = false;
  if(key & 0x8000 ) {
    key ^= 0x8000;
    down = true;
  }
  if(key == _KEY_NONE) {
    uint32_t ut = uptime();
    sprintf(buf, "t %d\n", ut);
  } else {
    sprintf(buf, "%s %s\n", down ? "kd" : "ku", keyname[key]);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  strncpy(buf, dispinfo + offset, len); 
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int x = (offset / 4) % screen_width();
  int y = (offset / 4) / screen_width();
  draw_sync();
  draw_rect((uint32_t *)buf, x, y, len / 4, 1);
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}

#include <am.h>
#include <x86.h>
#include <amdev.h>

#define I8042_DATA_PORT 0x60

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
 	  uint32_t keycode = inl(I8042_DATA_PORT);
      kbd->keydown = 0;
      kbd->keycode = _KEY_NONE;
   	  if(keycode != _KEY_NONE)
		kbd->keycode = keycode;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}

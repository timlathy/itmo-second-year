#include <stdint.h>
int match(const char* str, int len) {
  const char* const end = str + len;
  const char* match_start = str;
  const char* pos;
loop:
  pos = match_start;
s1:
  if (end - pos >= 7 && *(int32_t*)(pos + 0) == 0x34333231 &&
      *(int16_t*)(pos + 4) == 0x3635 && pos[6] == '7') {
    pos += 7;
    goto s0;
  } else {
    goto fail;
  }
s0:
  goto finish;
fail:
  if (++match_start < end)
    goto loop;
  else
    return 0;
finish:
  return 1;
}

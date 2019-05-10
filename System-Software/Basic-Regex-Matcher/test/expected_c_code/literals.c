#include <stdint.h>
#include <stdlib.h>
struct match_group {
  int group_start;
  int group_end;
};
struct match_result {
  int match_start;
  int match_end;
  int group_count;
  struct match_group* match_groups;
};
struct match_result match(const char* str, int len) {
  struct match_group* const groups = 0;
  const char* const end = str + len;
  const char* match_start = str;
  const char* pos;
loop:
  pos = match_start;
s0:
  if (end - pos >= 7 && *(int32_t*)(pos + 0) == 0x34333231 &&
      *(int16_t*)(pos + 4) == 0x3635 && pos[6] == '7') {
    pos += 7;
    goto s1;
  }
  else {
    goto fail;
  }
s1:
  goto finish;
fail:
  if (++match_start < end) {
    goto loop;
  }
  return (struct match_result){0, 0, 0, groups};
finish:
  return (struct match_result){match_start - str, pos - str, 0, groups};
}

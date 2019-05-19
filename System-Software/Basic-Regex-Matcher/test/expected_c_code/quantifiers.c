#include <stdint.h>
#include <stdlib.h>
struct match_group {
  int group_start;
  int group_end;
  int reserved_prev_start;
};
struct match_result {
  int match_start;
  int match_end;
  int group_count;
  struct match_group* match_groups;
};
struct match_result match(const char* str, int len) {
  struct match_group* const groups = calloc(2, sizeof(struct match_group));
  const char* const end = str + len;
  const char* match_start = str;
  const char* pos;
loop:
  pos = match_start;
s0:
g0:
  groups[0].group_start = pos - str;
  if (end - pos >= 3 && *(int16_t*)(pos + 0) == 0x6874 && pos[2] == 'e') {
    pos += 3;
    goto s1;
  }
  else
    goto g0_fail;
s1:
  groups[0].group_end = pos - str;
  groups[0].reserved_prev_start = groups[0].group_start;
  goto g0;
g0_fail:
  if (groups[0].group_end == 0)
    goto fail;
  groups[0].group_start = groups[0].reserved_prev_start;
  goto s2;
s2:
g1:
  groups[1].group_start = pos - str;
  if (end - pos >= 6 && *(int32_t*)(pos + 0) == 0x776f6c66 &&
      *(int16_t*)(pos + 4) == 0x7265) {
    pos += 6;
    goto s3;
  }
  else
    goto g1_fail;
s3:
  groups[1].group_end = pos - str;
  goto g1_success;
g1_fail:
g1_success:
  goto s4;
s4:
  if (end - pos >= 4 && *(int32_t*)(pos + 0) == 0x61736577) {
    pos += 4;
    goto s5;
  }
  else
    goto fail;
s5:
  if (end - pos >= 1 && pos[0] == 'w') {
    pos += 1;
    goto s6;
  }
  else
    goto fail;
s6:
  goto finish;
fail:
  if (++match_start < end) {
    goto loop;
  }
  return (struct match_result){0, 0, 0, groups};
finish:
  return (struct match_result){match_start - str, pos - str, 2, groups};
}

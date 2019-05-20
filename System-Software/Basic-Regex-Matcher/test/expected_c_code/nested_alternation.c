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
  struct match_group* const groups = calloc(1, sizeof(struct match_group));
  const char* const end = str + len;
  const char* match_start = str;
  const char* pos;
loop:
  pos = match_start;
s0:
alt_0:
  int alt_0_pos = pos;
  goto s1;
alt_1:
  int alt_1_pos = pos;
  goto s6;
  goto fail;
s6:
  if (end - pos >= 2 && *(int16_t*)(pos + 0) == 0x6463) {
    pos += 2;
    goto s7;
  }
  goto fail;
s7:
  goto finish;
s1:
g0:
  groups[0].group_start = pos - str;
alt_2:
  int alt_2_pos = pos;
  goto s2;
alt_3:
  int alt_3_pos = pos;
  goto s4;
  goto alt_1;
s4:
  if (end - pos >= 1 && pos[0] == 'b') {
    pos += 1;
    goto s5;
  }
  goto alt_1;
s5:
  groups[0].group_end = pos - str;
  goto g0_success;
g0_fail:
  goto fail;
g0_success:
  goto finish;
s2:
  if (end - pos >= 1 && pos[0] == 'a') {
    pos += 1;
    goto s3;
  }
  goto alt_3;
s3:
  groups[0].group_end = pos - str;
  goto g0_success;
g0_fail:
  goto fail;
g0_success:
  goto finish;
fail:
  if (++match_start < end) {
    goto loop;
  }
  return (struct match_result){0, 0, 0, groups};
finish:
  return (struct match_result){match_start - str, pos - str, 1, groups};
}

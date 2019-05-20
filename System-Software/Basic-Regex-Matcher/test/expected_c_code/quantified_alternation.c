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
g0:
  groups[0].group_start = pos - str;
alt_0:
  int alt_0_pos = pos;
  goto s1;
alt_1:
  int alt_1_pos = pos;
  goto s3;
alt_2:
  int alt_2_pos = pos;
  goto s5;
  goto g0_fail;
s5:
  if (end - pos >= 1 && pos[0] == 'c') {
    pos += 1;
    goto s6;
  }
  goto fail;
s6:
  groups[0].group_end = pos - str;
  groups[0].reserved_prev_start = groups[0].group_start;
  goto g0;
g0_fail:
  if (groups[0].group_end == 0)
    goto fail;
  groups[0].group_start = groups[0].reserved_prev_start;
  goto finish;
s3:
  if (end - pos >= 1 && pos[0] == 'b') {
    pos += 1;
    goto s4;
  }
  goto alt_2;
s4:
  groups[0].group_end = pos - str;
  groups[0].reserved_prev_start = groups[0].group_start;
  goto g0;
g0_fail:
  if (groups[0].group_end == 0)
    goto fail;
  groups[0].group_start = groups[0].reserved_prev_start;
  goto finish;
s1:
  if (end - pos >= 1 && pos[0] == 'a') {
    pos += 1;
    goto s2;
  }
  goto alt_1;
s2:
  groups[0].group_end = pos - str;
  groups[0].reserved_prev_start = groups[0].group_start;
  goto g0;
g0_fail:
  if (groups[0].group_end == 0)
    goto fail;
  groups[0].group_start = groups[0].reserved_prev_start;
  goto finish;
fail:
  if (++match_start < end) {
    goto loop;
  }
  return (struct match_result){0, 0, 0, groups};
finish:
  return (struct match_result){match_start - str, pos - str, 1, groups};
}

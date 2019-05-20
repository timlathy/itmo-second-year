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
  struct match_group* const groups = 0;
  const char* const end = str + len;
  const char* match_start = str;
  const char* pos;
loop:
  pos = match_start;
s0 : {
  if (end - pos >= 1 && pos[0] == '1') {
    pos += 1;
    goto s1;
  }
  goto s1;
}
s1 : {
  int repeats = 0;
s1_repeat:
  if (end - pos >= 1 && pos[0] == '2') {
    pos += 1;
    repeats = 1;
    goto s1_repeat;
  }
  if (repeats == 1)
    goto s2;
  else
    goto fail;
}
s2 : {
s2_repeat:
  if (end - pos >= 1 && pos[0] == '3') {
    pos += 1;
    goto s2_repeat;
  }
  goto s3;
}
s3:
  goto finish;
fail:
  if (++match_start < end) {
    goto loop;
  }
  return (struct match_result){0, 0, 0, groups};
finish:
  return (struct match_result){match_start - str, pos - str, 0, groups};
}

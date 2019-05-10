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
  struct match_group* const groups = calloc(2, sizeof(struct match_group));
  const char* const end = str + len;
  const char* match_start = str;
  const char* pos;
loop:
  pos = match_start;
s0:
  if (end - pos >= 1 && pos[0] == '1') {
    pos += 1;
    goto s1;
  }
  else {
    goto fail;
  }
s1:
  groups[0].group_start = pos - str;
  if (end - pos >= 4 && *(int32_t*)(pos + 0) == 0x74736574) {
    pos += 4;
    goto s2;
  }
  else {
    goto fail;
  }
s2:
  groups[0].group_end = pos - str;
  goto s3;
s3:
  if (end - pos >= 1 && pos[0] == '2') {
    pos += 1;
    goto s4;
  }
  else {
    goto fail;
  }
s4:
  groups[1].group_start = pos - str;
  if (end - pos >= 4 && *(int32_t*)(pos + 0) == 0x74736574) {
    pos += 4;
    goto s5;
  }
  else {
    goto fail;
  }
s5:
  groups[1].group_end = pos - str;
  goto finish;
fail:
  if (++match_start < end) {
    goto loop;
  }
  return (struct match_result){0, 0, 0, groups};
finish:
  return (struct match_result){match_start - str, pos - str, 2, groups};
}

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
s0:
  if (end - pos >= 1 && pos[0] == 'w') {
    pos += 1;
    goto s1;
  }
  goto fail;
s1:
  if (end - pos >= 1 && (pos[0] == 'a' || pos[0] == 'e' || pos[0] == 'i' ||
                         pos[0] == 'o' || pos[0] == 'u')) {
    pos += 1;
    goto s2;
  }
  goto fail;
s2:
  if (end - pos >= 1 && pos[0] == 'n') {
    pos += 1;
    goto s3;
  }
  goto fail;
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

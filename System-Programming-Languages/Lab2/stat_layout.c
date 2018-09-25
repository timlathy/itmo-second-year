#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>

/* Compile and run with: gcc -o stat_layout stat_layout.c && ./stat_layout */
int main(int argc, char *argv[]) {
  printf("struct    stat:       sizeof = %d\n", sizeof(struct stat));
  printf("dev_t     st_dev:     sizeof = %d, offset = %u\n", sizeof(dev_t), offsetof(struct stat, st_dev));
  printf("ino_t     st_ino:     sizeof = %d, offset = %u\n", sizeof(ino_t), offsetof(struct stat, st_ino));
  printf("mode_t    st_mode:    sizeof = %d, offset = %u\n", sizeof(mode_t), offsetof(struct stat, st_mode));
  printf("nlink_t   st_nlink:   sizeof = %d, offset = %u\n", sizeof(nlink_t), offsetof(struct stat, st_nlink));
  printf("uid_t     st_uid:     sizeof = %d, offset = %u\n", sizeof(uid_t), offsetof(struct stat, st_uid));
  printf("gid_t     st_gid:     sizeof = %d, offset = %u\n", sizeof(gid_t), offsetof(struct stat, st_gid));
  printf("dev_t     st_rdev:    sizeof = %d, offset = %u\n", sizeof(dev_t), offsetof(struct stat, st_rdev));
  printf("off_t     st_size:    sizeof = %d, offset = %u\n", sizeof(off_t), offsetof(struct stat, st_size));
  printf("blksize_t st_blksize: sizeof = %d, offset = %u\n", sizeof(blksize_t), offsetof(struct stat, st_blksize));
  printf("blkcnt_t  st_blocks:  sizeof = %d, offset = %u\n", sizeof(blkcnt_t), offsetof(struct stat, st_blocks));
}

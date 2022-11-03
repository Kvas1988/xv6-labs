#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *fmtname(char *path) {
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  return buf;
}

void find(char *path, char *target) {
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512];
  char *p;

  if ((fd = open(path, 0)) < 0) {
    printf("find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) { // place stat info about file (fd) in st
    printf("find: cannot fstat %d\n", fd);
    close(fd);
    return;
  }

  //printf("%s\n", path); // debug

  switch (st.type) {
  case T_FILE:
    printf("find %s is not a dir\n", path);
    return;

  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      printf("find: path too long\n");
      break;
    }

    strcpy(buf, path);     // copy string path => buf
    p = buf + strlen(buf); // set pointer p to the end of buf
    *p++ = '/';            // add slash to filepath in buf

    while (read(fd, &de, sizeof(de)) == sizeof(de)) { // fd => de
      if (de.inum == 0) {                             // ???
        continue;
      }

      memmove(p, de.name, DIRSIZ); // copy DIRSIZ chars from de.name to p
      p[DIRSIZ] = 0;

      if (stat(buf, &st) < 0) { // place stat info about file (buf) in st
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      if (st.type == T_FILE) {
        // check target
        //printf("%s\n", de.name); // debug
        if (strcmp(de.name, target) == 0) {
          printf("%s\n", buf);
        }
      }

      if (st.type == T_DIR) {
        if ((strcmp(de.name, ".") == 0) ||
            strcmp(de.name, "..") == 0) { // check if . or ..
          continue;
        }
        // recursive
        find(buf, target);
      }
    }
    break; // T_DIR case
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc <=2 || argc > 3) {
    printf("Usage: find {path} {file}\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}

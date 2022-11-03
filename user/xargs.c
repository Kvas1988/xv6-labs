#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

char *_argv[MAXARG];
char buf[512];
int _argc;

void subroutine() {
  if (fork() == 0) {
    // child
    exec(_argv[0], _argv);
    exit(0);
  }
  wait(0);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("xargs: too few args\n");
    exit(1);
  }

  memset(_argv, 0, sizeof(_argv)); // fill _argv with 0
  _argc = argc - 1;
  for (int i = 1; i < argc; i++) { // skip argv[0]
    _argv[i - 1] = argv[i]; // fill _argv with arguments from argv
  }

  struct stat st;

  if (fstat(0, &st) < 0) { // put stat info from 0-file-descriptor to st
    int pos = 0;

    _argv[_argc] = buf;
    memset(buf, 0, sizeof(buf));

    while (read(0, &buf[pos], 1) > 0) { // read from 0-fd to buf[pos] 1 char
      if (buf[pos] == '\n') {           // if char is \n
        buf[pos] = 0;                   // set buf[pos] to zero
        subroutine();                   // exec command with current _argv
        pos = 0;                        // set pos to zero
        memset(buf, 0, sizeof(buf));    // fill buf with 0
      } else {
        pos++; // continue to fill buf
      }
    }

    if (buf[0]) {
      subroutine();
    }
  } else {
    subroutine();
  }

  exit(0);
}

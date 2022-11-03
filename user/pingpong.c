// #include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int p[2];
  char *buffer[4];

  pipe(p);

  if (fork() == 0) {       // if fork() == 0 => child process
    read(p[0], buffer, 4); // read from parrent
    fprintf(1, "Thread id %d: received %s\n", getpid(), buffer);

    write(p[1], "pong", 4);
    exit(0);
  } else { // parrent process
    write(p[1], "ping", 4);
    wait(0); // ???
             //
    read(p[0], buffer, 4);
    fprintf(1, "Thread id %d: received %s\n", getpid(), buffer);
  }

  close(p[0]);
  close(p[1]);
  exit(0);
}

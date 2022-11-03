#include "kernel/types.h"
#include "user/user.h"

void pipeline(int *pfd) {
  int inner_fd[2];
  int n, buf;

  pipe(inner_fd);
  close(pfd[1]);

  if (read(pfd[0], &n, sizeof(int)) > 0) { // read outer_fd => n
    printf("prime %d\n", n);

    if (fork() == 0) {
      // child
      pipeline(inner_fd); // recursive. use inner_fd
      exit(0);
    } else {
      // parrent
      close(inner_fd[0]); // close one of inner_fd

      while (read(pfd[0], &buf, sizeof(int))) { // read from outer_fd => buf
        if (buf % n != 0) {
          write(inner_fd[1], &buf, sizeof(int));
        }
      }
      close(inner_fd[1]);
      wait(0);
    }
  }

  exit(0);
}

int main(int argc, char *argv[]) {
  int pfd[2];

  pipe(pfd);

  if (fork() == 0) { // child
    pipeline(pfd);
    exit(0);
  } else { // parrent
    close(pfd[0]);
    for (int i = 2; i <= 35; i++) {
      write(pfd[1], &i, sizeof(int));
    }
    close(pfd[1]);
    wait(0);
  }

  exit(0);
}

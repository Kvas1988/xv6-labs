#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "no tick time provided");
  }

  int t = atoi(argv[1]);
  if (t == 0) {
    fprintf(2, "Tick time argument must be an integer");
    exit(1);
  }

  sleep(t);
  exit(0);
}

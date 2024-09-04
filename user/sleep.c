#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
  int pid;
  pid = fork();

  if (pid == 0) {
     if (argc <= 1) {
    printf("error!No argument\n");
    exit(1);
  } else {
    int times = atoi(argv[1]);
    sleep(times);
  }
  } else {
    wait((int *) 0);
  }

  exit(0);
}

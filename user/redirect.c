//为了辅助理解课程写的东西
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"


int
main()
{
  int pid;
  char *argv[] = { "echo", "this", "is", "redirected", "echo", 0 };
  pid = fork();
  if(pid == 0){
    close(1);
    open("output.txt", O_WRONLY|O_CREATE);
    exec("echo", argv);
    printf("exec failed!\n");
    exit(1);
  } else {
    exec("echo", argv);
    wait((int *) 0);
  }

  exit(0);
}


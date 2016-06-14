#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/ptrace.h>

int main (void)
{
  pid_t               child;
  unsigned long int   old;

  child = fork();

  if (child == 0) {
    char * const  args[] = { NULL };
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execv("./a.out", args);
  } else {

    int                       status;
    struct user_regs_struct   regs;

    old = 0;
    wait(&status);
    while (1) {
      ptrace(PTRACE_GETREGS, child, NULL, &regs);
      if (old != regs.rip) {
        printf("nop\n");
        printf("rip: 0x%llx\n", regs.rip);
        old = regs.rip;
      } else {
        printf("y\n");
      }
      ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
      waitpid(child, &status, 0);
      if (WIFEXITED(status))
        break;
    }
  }
  return (0);
}

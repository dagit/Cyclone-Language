#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <sys/types.h>

#define WNOHANG 1
#define WUNTRACED 2
#define WIFEXITED(status) (WTERMSIG(status) == 0)
#define WEXITSTATUS(status) (((status) & 0xff00) >> 8)
#define WIFSIGNALED(status) ({int s=(status);!WIFSTOPPED(s)&&!WIFEXITED(s);})
#define WTERMSIG(status) ((status) & 0x7f)
#define WIFSTOPPED(status) (((status) & 0xff) == 0x7f)
#define WSTOPSIG(status) WEXITSTATUS(status)

namespace Wait {
  extern "C" pid_t wait(int *`r status);
  extern "C" pid_t waitpid(pid_t pid, int *`r status, int options);
}

#endif

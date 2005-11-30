/* This file is part of the RC compiler.
   Copyright (C) 2000-2001 The Regents of the University of California.

RC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "parser.h"
#include "callcc1.h"
#include "unparse.h"

int exec_cc1(declaration the_program) deletes
{
  int channel[2];
  int cc1pid, cc1stat, memhack, res;
  FILE *cc1input;
  char *cc1path;

  cc1path = getenv("CC1PATH");
  if (!cc1path)
    cc1path = CC1PATH;

  pipe(channel);

  if ((cc1pid = fork()) == 0)
    {
      close(0);
      dup(channel[0]);
      close(channel[0]);
      close(channel[1]);
#if 0
      {
	int i;

	for (i = 0; i < copy_argc; i++)
	  fprintf(stderr, "%s ", copy_argv[i]);
	putc('\n', stderr);
      }
#endif
      execv(cc1path, copy_argv);
    }
  if (cc1pid < 0)
    perror("failed to fork");
  close(channel[0]);

  cc1input = fdopen(channel[1], "w");
  unparse(cc1input, the_program);
  fclose(cc1input);

  /* Relase all memory now if MEMHACK defined */
  memhack = getenv("MEMHACK") != NULL;

 /*  if (memhack) */
/*     { */
/*       extern void *orig_brk; */

/*       sbrk(orig_brk); */
/*     } */

  for (;;)
    {
      int pid = wait(&cc1stat);

      if (pid == -1)
	{
	  if (errno == EINTR)
	    continue;
	  else
	    {
	      res = FATAL_EXIT_CODE;
	      break;
	    }
	}

      if (pid == cc1pid)
	{
	  if (WIFEXITED(cc1stat))
	    res = WEXITSTATUS(cc1stat);
	  else
	    res = FATAL_EXIT_CODE;
	  break;
	}
    }

  if (memhack)
    {
      _exit(res);
      return 0;
    }
  else
    return res;
}

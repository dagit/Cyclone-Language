/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usage.h"

#define PROC_LINE_LENGTH 4096

/* Return the memory used by the current process, in bytes */
long get_memusage(void)
{
  long vmsize;
  pid_t pid;
  char filename[64];
  char buf[PROC_LINE_LENGTH];
  FILE *stat_file;
  int i, field;

  vmsize = 0;
  pid = getpid();
  snprintf(filename, 64, "/proc/%d/stat", pid);
  filename[63] = '\0';
  stat_file = fopen(filename, "r");
  if (!stat_file)
    {
      perror(filename);
      exit(EXIT_FAILURE);
    }

  /* Read in /proc/<pid>/stat */
  if (!fread(buf, sizeof(char), PROC_LINE_LENGTH, stat_file))
    {
      perror(filename);
      exit(EXIT_FAILURE);
    }

  /* Scan over fields; assume separated by spaces */
  field = 0;
  i = 0;
  while (buf[i])
    {
      if (field == 22)
	{
	  /* memory size, in bytes */
	  for (; '0' <= buf[i] && buf[i] <= '9'; i++)
	    vmsize = 10*vmsize + buf[i] - '0';

	}

      /* Skip ahead to next field */
      while (buf[i] && buf[i] != ' ')
	i++;
      i++; /* Skip over space */
      field++;
    }

  fclose(stat_file);
  return vmsize;
}




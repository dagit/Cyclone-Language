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
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h"
#include "utils.h"
#include "cstring.h"

void *xmalloc(size_t size)
{
#ifdef BWGC
  void *x = GC_malloc(size);
#else
  void *x = malloc(size);
#endif

  if (!x) abort();

  return x;
}

void *xrealloc(void *p, size_t newsize)
{
#ifdef BWGC
  void *x = GC_realloc(p, newsize);
#else
  void *x = realloc(p, newsize);
#endif

  if (!x) abort();

  return x;
}

/* Make a new cstring with a copy of s, length l */
cstring make_cstring(region r, const char *s, int l)
{
  cstring cs;

  cs.data = rstralloc(r, l + 1);
  memcpy(cs.data, s, l);
  cs.data[l] = '\0';
  cs.length = l;

  return cs;
}

/* Make a new cstring with a copy of regular C string s */
cstring str2cstring(region r, const char *s)
{
  return make_cstring(r, s, strlen(s));
}

unsigned long align_to(unsigned long n, unsigned long alignment)
{
  int count = (n + alignment - 1) / alignment;

  return count * alignment;
}

unsigned long gcd(unsigned long x, unsigned long y)
{
  unsigned long z;

  for (;;)
    {
      if (y == 0)
	return x;
      
      z = x % y; x = y; y = z;
    }
}

unsigned long lcm(unsigned long x, unsigned long y)
{
  /* ignoring risk of overflow (used for alignments which are typically <= 16) */
  return (x * y) / gcd(x, y); 
}

#ifdef __FreeBSD__
long double strtold(const char *str, char **endptr)
{
  long double ld;
  int count;

  assert(!endptr);
  count = sscanf(str, "%Lf", &ld);
  assert(count == 1);

  return ld;
}
#endif

/* Panic with a message */
static void vfail(const char *fmt, va_list args) __attribute__((__noreturn__));

static void vfail(const char *fmt, va_list args)
{
  vfprintf(stderr, fmt, args);
  fflush(stderr);
  fflush(stdout);
  abort();
  while (1); /* Work around stupid gcc-2.96-85 bug */
}

/* Panic with a nice message */
void __fail(const char *file, unsigned int line,
	    const char *func __attribute__((unused)),
	    const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "\n%s:%u ", file, line);
  vfail(fmt, args);
}

#ifndef HAVE_VARIADIC_MACROS
/* Panic with a not-quite-as-nice message */
void fail(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfail(fmt, args);
}
#endif

/* Concatenate 2 strings, allocating space in r for the result */
char *rstrcat(region r, const char *s1, const char *s2)
{
  char *result = rarrayalloc(r, strlen(s1)+strlen(s2)+1, char);
  result[0] = '\0';
  strcat(result, s1);
  strcat(result, s2);
  return result;
}

/* Concatenate n strings, allocating space in r for the result.  The
   last argument should be a null pointer. */
char *rstrscat(region r, ...)
{
  char *result;
  int len = 0;
  const char *s;
  va_list args;

  va_start(args, r);
  while ((s = va_arg(args, const char *)))
    len += strlen(s);
  result = rarrayalloc(r, len+1, char);
  result[0] = '\0';

  va_start(args, r);
  while ((s = va_arg(args, const char *)))
    strcat(result, s);

  return result;
}

/* Convert an integer to a string, storing the result in r */
const char *inttostr(region r, int i)
{
  char *result;
  int width, count;

  /* Compute width */
  width = 1;
  if (i < 0) {
    width++;
    i = -i;
  }
  for (count = 10; i >= count; count *= 10)
    width++;

  /* Covert to integer */
  result = rarrayalloc(r, width + 1, char);
  if (snprintf(result, width + 1, "%d", i) == -1) {
    printf("i=%d, width=%d\n", i, width);
    fail ("inttostr width wrong\n");
  }
  return result;
}


/* sprintf a string, allocating space in r for the result */
char *rsprintf(region r, const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  return rvsprintf(r, fmt, args);
}

char *rvsprintf(region r, const char *fmt, va_list args)
{
  growbuf buf = growbuf_new(r, 100);
  gvprintf(buf, fmt, args);
  return growbuf_contents(buf);
}

/* Space for the ASCII representation of a pointer -- 2 hex chars per
   byte, plus 3 chars for 0x prefix and trailing \0 */
#define PTR_ASCII_SIZE  ((int) (3 + sizeof(void *)*2))

/* Convert a pointer to an ascii string with leading 0x.  Re-uses
   internal buffer. */
char *ptr_to_ascii(void *ptr) {
  static char addr[PTR_ASCII_SIZE];
  int nchars;

  nchars = snprintf(addr, PTR_ASCII_SIZE, "%p", ptr);
  if (nchars == -1 || nchars >= PTR_ASCII_SIZE)
    fail("Unable to convert ptr to ascii (need %d bytes, have %d)\n",
	 nchars, PTR_ASCII_SIZE);
  return addr;
}

/* Convert a pointer to an integer */
unsigned long ptr_hash(void *ptr)
{
  return (unsigned long) ptr >> 3;
}

/* Return TRUE iff ptr1 == ptr2 */
bool ptr_eq(void *ptr1, void *ptr2)
{
  return ptr1 == ptr2;
}

/* Hash a string */
unsigned long string_hash(const char *str)
{
  const char *c;
  unsigned long h;

  c = str;
  h = 0;
  if (!c)
    return 0;
  while (*c)
    h = 33*h + 720 + *c++; /* SML/NJ's string hash function */
  return h;
}

/* Return TRUE iff s1 == s2 */
bool string_eq(const char *s1, const char *s2)
{
  if (!s1 || !s2) /* Nulls are all distinct */
    return FALSE;
  return !strcmp(s1, s2);
}

/* A total ordering on pointers.  Returns 0 if ptr1 = ptr2, a value <0
   if ptr1 < ptr2, or a value >0 if ptr1 > ptr2. */
int ptr_cmp(void *ptr1, void *ptr2)
{
  return (char *) ptr1 - (char *) ptr2;
}

int min(int a, int b) { if (a < b) return a; else return b; }
int max(int a, int b) { if (a < b) return b; else return a; }

/*
 * Copyright (c) 1999-2004
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
/* Idea: clear on page alloc rather than individual alloc
   Turns out not so good (on lcc at least, seems a wash on mudlle):
   logically should be bad for small regions (less than a few pages)
*/
#undef PRECLEAR

#include "regions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

region permanent = NULL;

#define RPAGESIZE (1 << RPAGELOG)
#define PAGE_GROUP_SIZE 32
#define K 4
#define MAXPAGE (1 << (32 - RPAGELOG))

#define PAGENB(x) ((__rcintptr)(x) >> RPAGELOG)

#ifdef REGIONPROFILE
__rcintptr totalbytes, maxbytes, allocbytes, alloccalls;
__rcintptr totalregions, maxregions, allocregions;
unsigned long rcs_adjust_struct, rcs_adjust_array, rcs_adjust, 
  rcs_update_struct, rcs_update, rcs_update_same, rcs_update_traditional,
  rcs_global_struct, rcs_global, rcs_global_traditional, rcs_update_parent;

static void profile_alloc(region r, int nbytes)
{
  totalbytes += nbytes;
  allocbytes += nbytes;
  alloccalls++;
  if (totalbytes > maxbytes) maxbytes = totalbytes;
  r->bytes += nbytes;
}

#else

static void profile_alloc(region r, int nbytes)
{
}

#endif
/*unsigned long perturb_counter;*/

struct array_hdr
{
  adjust_fn *cleanup;
  size_t n;
};

#define ALIGN(x, n) (((x) + ((n) - 1)) & ~((n) - 1))
#define PALIGN(x, n) ((void *)ALIGN((__rcintptr)(x), n))
#ifdef __GNUC__
#define RALIGNMENT __alignof(unsigned long) /* double would be safer */
#define ALIGNMENT_HDR __alignof(adjust_fn *)
#define PTRALIGNMENT __alignof(void *)
#define ALIGNMENT_LONG __alignof(unsigned long)
#else
#define RALIGNMENT 4
#define ALIGNMENT_HDR 4
#define PTRALIGNMENT 4
#define ALIGNMENT_LONG 4
#endif

nomem_handler nomem_h;

/* dummy region for NULL and malloc memory */
struct region zeroregion;

static void clear(void *start, __rcintptr size)
{
  long *clear, *clearend;

  clear = (long *)start;
  clearend = (long *)((char *)start + size);
  do *clear++ = 0;
  while (clear < clearend) ;
}

#ifdef PRECLEAR
#define preclear clear
#define postclear(s, e)
#else
#define preclear(s, e)
#define postclear clear
#endif

#include "pages.c"
#include "alloc.c"
#include "serialize.c"

/* Region pre-order numbering */
static int renumber_regions(region r, int nextid)
{
  region child;

  r->rid = nextid++;
  for (child = r->children; child; child = child->sibling)
    nextid = renumber_regions(child, nextid);
  r->nextid = nextid;

  return nextid;
}

static void nochildren(region r)
{
  if (r->children)
    abort();
}

static void unlink_region(region r)
{
  region *scan;

  scan = &r->parent->children;
  while (*scan != r)
    scan = &(*scan)->sibling;
  *scan = (*scan)->sibling;

/*   renumber_regions(&zeroregion, 0); */
}

static void link_region(region r, region parent)
{
  r->sibling = parent->children;
  r->parent = parent;
  parent->children = r;

/*   renumber_regions(&zeroregion, 0); */
}

void check_child(region r, region of)
{
  if (!(r->rid >= of->rid && r->rid < of->nextid))
    abort();
}

static int rstart;

void initregion(region r)
{
  char *first =
    (char *)r - rstart - offsetof(struct page, previous);

  /* Start using page with region header as a pointer-containing page */
  r->normal.page.base = first;
  r->normal.page.allocfrom = (char *)(r + 1);

  /* Guarantee failure for all other blocks */
  r->normal.superpage.allocfrom = (char *)(K * RPAGESIZE + 1);
  r->normal.hyperpage.allocfrom = (char *)(K * K * RPAGESIZE + 1);
  r->atomic.page.allocfrom = (char *)(RPAGESIZE + 1);
  r->atomic.superpage.allocfrom = (char *)(K * RPAGESIZE + 1);
  r->atomic.hyperpage.allocfrom = (char *)(K * K * RPAGESIZE + 1);

  /* Remember that r owns this page. */
  r->normal.pages = (struct page *)first;
  set_region(r->normal.pages, 1, r);
}

region newregion(void)
{
  return newsubregion(&zeroregion);
}

region newsubregion(region parent)
{
  char *first;
  region r;


  first = (char *)alloc_single_page(NULL);
  preclear(first + offsetof(struct page, pagecount), RPAGESIZE - offsetof(struct page, pagecount));

#ifdef REGIONPROFILE
  allocregions++;
  totalregions++;
  if (totalregions > maxregions) maxregions = totalregions;
#endif

  /* stagger regions across cache lines a bit */
  rstart += 0; /* was 64, but changed to 0 to allow serialization/deserialization */
  if (rstart > RPAGESIZE / K) rstart = 0;
  r = (region)(first + rstart + offsetof(struct page, previous));
  postclear(r, sizeof *r);
  initregion(r);
  ((struct page *) first)->available = r->normal.page.allocfrom;
  link_region(r, parent);
  return r;
}

void *typed_ralloc(region r, size_t size, adjust_fn *cleanup)
{
  void *mem, *hdr;

  /*
    Unneeded stuff for simple allocation 
  if (!cleanup)
    return rstralloc0(r, size);


  profile_alloc(r, size);
  */

  /* changed to be like rstralloc---no header at all */
  qalloc(r, &r->normal, &hdr, 0, 1, &mem, size, RALIGNMENT, 0);

  /* 
  More RC stuff that we don't need.
 
  *(adjust_fn **)hdr = cleanup;
  postclear(mem, size);  
  */

  return mem;
}

unsigned long long catime;

static size_t cleanup_array(void *x, int by)
{
  {
  struct array_hdr *ahdr = x;
  int n = ahdr->n;
  adjust_fn *cleanup = ahdr->cleanup;
  char *base = PALIGN(ahdr + 1, RALIGNMENT);

  while (n--)
    base += cleanup(base, by);

  return base - (char *)ahdr;
  }
}

void *typed_rarrayextend(region r, void *old, size_t n, size_t size,
			      adjust_fn *cleanup)
{
  /* For now we don't attempt to extend the old storage area */
  void *mem, *hdr;
  struct array_hdr *ahdr;

  if (!cleanup)
    return rstrextend0(r, old, n * size);


  profile_alloc(r, n * size);

  qalloc(r, &r->normal, &hdr, sizeof cleanup, ALIGNMENT_HDR,
	 &mem, ALIGN(sizeof(struct array_hdr), RALIGNMENT) + n * size, RALIGNMENT, 1);
	 
  *(adjust_fn **)hdr = cleanup_array;
  ahdr = (struct array_hdr *)mem;
  ahdr->n = n;
  ahdr->cleanup = cleanup;
  mem = PALIGN(ahdr + 1, RALIGNMENT);
  if (old)
    {
      struct array_hdr *oldhdr = 
	PALIGN((char *)old - sizeof(struct array_hdr), RALIGNMENT);
      size_t oldn = oldhdr->n;

      /* Would be nice (to check consistent types in calls to rarrayextend)
	 to compare oldhdr->cleanup and hdr->cleanup, but they could be
	 identical static functions from separate files */
      if (oldn > n)
	oldn = n;
      else
	postclear(mem + oldn * size, (n - oldn) * size);
      memcpy(mem, old, oldn * size);
    }
  else
    postclear(mem, n * size);


  return mem;
}

void *typed_rarrayalloc(region r, size_t n, size_t size, adjust_fn *cleanup)
{
  /* return typed_rarrayextend(r, NULL, n, size, cleanup); */
  /* replace fancy array allocation by simple allocation of a C-style array */ 
  char *mem = rstralloc(r, n * size);

  return mem;
}

char *rstralloc(region r, size_t size)
{
  void *mem, *dummy;


  /* profile_alloc(r, size); */
  /*
    Changed to allocate exclusively in the normal pages.  We will no longer use the atomic pages at all.
  */
  qalloc(r, &r->normal, &dummy, 0, 1, &mem, size, RALIGNMENT, 0);


  return mem;
}

char *rstralloc0(region r, size_t size)
{
  char *mem;


  mem = rstralloc(r, size);

  clear(mem, size);


  return mem;
}

char *rstrdup(region r, const char *s)
{
  char *news = rstralloc(r, strlen(s) + 1);

  strcpy(news, s);

  return news;
}

static char *internal_rstrextend(region r, const char *old, size_t newsize,
				 int needsclear)
{
  /* For now we don't attempt to extend the old storage area */
  void *newmem, *hdr;
  unsigned long *oldhdr, oldsize;


  profile_alloc(r, newsize);

  /* switched this from the atomic to the normal region as well */
  qalloc(r, &r->normal, &hdr, sizeof(unsigned long), ALIGNMENT_LONG,
	 &newmem, newsize, RALIGNMENT, 0);

  /* If we don't do this we can't find the header: */
  hdr = (char *)newmem - sizeof(unsigned long);

  *(unsigned long *)hdr = newsize;

  if (old)
    {
      oldhdr = (unsigned long *)(old - ALIGNMENT_LONG);
      oldsize = *oldhdr;

      if (oldsize > newsize)
	oldsize = newsize;
      else if (needsclear)
	clear(newmem + oldsize, newsize - oldsize);
      memcpy(newmem, old, oldsize);
    }
  else if (needsclear)
    clear(newmem, newsize);


  return newmem;
}

char *rstrextend(region r, const char *old, size_t newsize)
{
  return internal_rstrextend(r, old, newsize, 0);
}

char *rstrextend0(region r, const char *old, size_t newsize)
{
  return internal_rstrextend(r, old, newsize, 1);
}

void typed_rarraycopy(void *to, void *from, size_t n, size_t size, type_t type)
{
  /*
  char *cto;
  int i;


  if (type)
    for (cto = to, i = 0; i < n; i++, cto += size)
      type(cto, -1);
  */

  memcpy(to, from, n * size);

  /*
  if (type)
    for (cto = to, i = 0; i < n; i++, cto += size)
      type(cto, 1);
  */
}

void delpage(char *deleting, char *end)
{
  for (;;)
    {
      adjust_fn *cln;

      deleting = PALIGN(deleting, ALIGNMENT_HDR);
      if (deleting >= end)
	break;

      cln = *(adjust_fn **)deleting;
      if (!cln)
	break;
      deleting = PALIGN(deleting + sizeof(adjust_fn *), RALIGNMENT);
      deleting += cln(deleting, -1);
    }
}

unsigned long long unscantime;

static void unscanregion(region r)
{
#ifndef NORC
  {
#ifdef RCPAIRS
  int i;

  for (i = 0; i < MAXREGIONS; i++)
    {
#if defined(RCPAIRS_ARRAY)
      __rcs[r->id][i] = 0;
#elif defined(RCPAIRS_FROM)
      r->rc[i] = 0;
#else
      if (__rcregions[i])
	__rcregions[i]->rc[r->id] = 0;
#endif
    }
#else
  struct page *p, *next;

  mark_page_end(&r->normal.page, RPAGESIZE);
  mark_page_end(&r->normal.superpage, K * RPAGESIZE);
  mark_page_end(&r->normal.hyperpage, K * K * RPAGESIZE);

  /* Scan normal pages */
  /* First page (last in list) is special (region object there) */
  for (p = r->normal.pages; (next = p->next); p = next)
    delpage((char *)&p->previous, (char *)p + RPAGESIZE);

  /* We skip region on first page */
  delpage((char *)(r + 1), (char *)p + RPAGESIZE);

  /* Now for the big pages */
  for (p = r->normal.bigpages; p; p = p->next)
    delpage((char *)&p->previous, (char *)p + p->pagecount * RPAGESIZE);
#endif
  }
#endif
}

#ifdef NOCHECK
#define rcabort() 0
#else
#define rcabort() abort()
#endif

static void delregion(region r)
{
  nochildren(r);
  free_all_pages(r, &r->normal);
  free_all_pages(r, &r->atomic); /* AA */
}

void deleteregion(region r)
{
  unlink_region(r);
  delregion(r);
}

static region derc(region *r)
{
  region tmp = *r;

  /* Remove reference count from r, if any */
  if (regionof(r) != tmp)
#if defined(RCPAIRS_ARRAY)
    __rcs[regionof(r)->id][tmp->id]--;
#elif defined(RCPAIRS_FROM)
    regionof(r)->rc[tmp->id]--;
#elif defined(RCPAIRS)
    tmp->rc[regionof(r)->id]--; 
#else
    tmp->rc--; 
#endif

  return tmp;
}

void deleteregion_ptr(region *r)
{
  /* could rewrite as deleteregion_array(1, r) */
  region tmp = derc(r);

  *r = NULL;
  deleteregion(tmp);
}

void deleteregion_array(int n, region *regions)
{
  int i;
  region rregions = regionof(regions);
  int regions_is_deleted = 0;


  for (i = 0; i < n; i++)
    {
      unlink_region(regions[i]);

      unscanregion(regions[i]);
      if (regions[i] == rregions)
	regions_is_deleted = 1;
    }

  for (i = 0; i < n; i++)
    {
      int rc;
      region r = regions[i];

      /* If regions array is not in one of the regions being deleted we must:
	 - remove the reference count to r
	 - set regions[i] to NULL
	 (if it is on one of the regions being deleted we already removed
	 the reference count in the unscanregion operation and there's no
	 point in clearing the array as it must be inacessible hereafter)
      */
      if (!regions_is_deleted)
	{
	  derc(&regions[i]);
	  regions[i] = NULL;
	}
      
#ifdef RCPAIRS
      rc = 0;
#else
      rc = r->rc;
#endif
      delregion(r);
    }

}

region regionof(void *ptr)
{
  /* XXX: What is regionof(null), or regionof(any global loc) supposed to be ? */
#ifdef RCPAIRS_ARRAY
  return __rcregions[regionidof(ptr)];
#else
  return regionidof(ptr);
#endif
}

#ifdef REGIONPROFILE

typedef int bool;
static void *xmalloc(int s)
{
  void *x = malloc(s);

  if (!x)
    abort();

  return x;
}

#include "dhash.c"

struct precord {
  int pid;
  int kind;
  char *filename;
  int lineno;
  unsigned long count;
};

static dhash_table profile_table;

static bool precord_compare(void *x, void *y)
{
  struct precord *px = x, *py = y;

  return px->pid == py->pid && px->filename == py->filename && px->kind == py->kind;
}

static unsigned long precord_hash(void *element)
/* Randomly chosen hash function. Probably not very good. */
{
  struct precord *p = element;
  unsigned const char *name = (unsigned const char *)p->filename;
  unsigned long code = 0;

  while (*name)
    {
      code = ((code << 1) + *name) ^ 0x57954317;
      name++;
    }

  return code + p->pid * 37 + p->kind * 993;
}

void rcs_profile(int pid, char *filename, int lineno,
		 int kind, enum __rcpcat cat1, enum __rcpcat cat2)
{
  int id = kind * 100 + cat1 * 10 + cat2;
  struct precord key;
  struct precord *entry;

  if (!profile_table)
    profile_table = new_dhash_table(1024, precord_compare, precord_hash);

  key.pid = pid;
  key.kind = id;
  key.filename = filename;
  entry = dhlookup(profile_table, &key);

  if (!entry)
    {
      entry = xmalloc(sizeof(struct precord));
      entry->pid = pid;
      entry->kind = id;
      entry->filename = filename;
      entry->lineno = lineno;
      entry->count = 0;
      dhadd(profile_table, entry);
    }
  entry->count++;
}

enum __rcpcat __rcptrcat(void *p)
{
  region r;

  if (!p)
    return pcat_null;

  r = regionof(p);

  if (r == &zeroregion)
    return pcat_trad;

  return pcat_reg;
}

enum __rcpcat __rcupdcat(void *p, void *c)
{
  region r, cr;

  if (!p)
    return pcat_null;

  r = regionof(p);
  cr = regionof(c);

  if (r == &zeroregion)
    return pcat_trad;

  if (r == cr)
    return pcat_same;

  if (cr->rid >= r->rid && cr->rid < r->nextid)
    return pcat_parent;

  return pcat_reg;
}

int order_source(const void *p1, const void *p2)
{
  const struct precord *const *pr1 = p1, *const *pr2 = p2;
  int cmp;
  
  if (!(*pr1)->filename)
    return 1;

  if (!(*pr2)->filename)
    return -1;

  cmp = strcmp((*pr1)->filename, (*pr2)->filename);
  if (cmp)
    return cmp;

  cmp = (*pr1)->pid - (*pr2)->pid;
  if (cmp)
    return cmp;

  return (*pr1)->kind - (*pr2)->kind;
}

int order_freq(const void *p1, const void *p2)
{
  const struct precord *const *pr1 = p1, *const *pr2 = p2;

  return (long)(*pr2)->count - (long)(*pr1)->count;
}

static char *rckind[] = {
  "AS  ", "AA  ", "AP  ",
  "US  ", "UT  ", "UTS ", "UM  ", "UMS ", "UZ T", "UZ R", "UP T", "UP R", "UPET", "UPER", "UPST", "UPSR",
  "GS  ", "GT  ", "GTS ", "GZ  ", "GP  ", "GPE ", "GPS ",
  "UU  ", "UUS " };

static char *ptrkind[] = {
  "", ":nul", ":trd", ":reg", ":sme", ":ppt" };

struct summary_element 
{
  int kind;
  unsigned long count;
};

static bool summary_compare(void *x, void *y)
{
  return ((struct summary_element *)x)->kind == 
    ((struct summary_element *)y)->kind;
}

static unsigned long summary_hash(void *element)
{
  return ((struct summary_element *)element)->kind;
}

static void summary_add(dhash_table s, int kind, unsigned long count)
{
  struct summary_element key;
  struct summary_element *e;

  key.kind = kind;
  e = dhlookup(s, &key);

  if (!e)
    {
      e = xmalloc(sizeof(struct summary_element));
      e->kind = kind;
      e->count = 0;
      dhadd(s, e);
    }
  e->count += count;
}

static char *kindstr(int kind)
{
  static char s[13];

  sprintf(s, "%4s%4s%4s", rckind[kind / 100],
	  ptrkind[kind % 100 / 10], ptrkind[kind % 10]);

  return s;
}

void print_rcs_profile(void)
{
  int i;
  dhash_table summary;
  dhash_scan s;
  struct summary_element *e;
  unsigned long pdata_size = dhash_used(profile_table);
  struct precord **pdata = alloca(sizeof(struct precord *) * pdata_size), *pr, **pp;

  s = dhscan(profile_table);
  pp = pdata;
  while ((pr = dhnext(&s)))
    *pp++ = pr;

  qsort(pdata, pdata_size, sizeof(struct precord *),
	getenv("RCP_ORDER_SOURCE") ? order_source : order_freq);
  summary = new_dhash_table(128, summary_compare, summary_hash);
  for (i = 0; i < pdata_size; i++)
    {
      fprintf(stderr, "%-20s:%4d %s = %6lu  [%d]\n", pdata[i]->filename,
	      pdata[i]->lineno, kindstr(pdata[i]->kind),
	      pdata[i]->count, pdata[i]->pid);
      summary_add(summary, pdata[i]->kind, pdata[i]->count);
    }

  fprintf(stderr, "\nSummary:\n");
  s = dhscan(summary);
  while ((e = dhnext(&s)))
    fprintf(stderr, "  %s = %lu\n", kindstr(e->kind), e->count);
}

static void print_memory_usage(void)
{
  fprintf(stderr, "Current region usage %lu bytes, max %lu bytes\n",
	  totalbytes, maxbytes);
  fprintf(stderr, "Current regions %lu, max %lu\n",
	  totalregions, maxregions);
  fprintf(stderr, "Total region allocation %lu bytes in %lu calls\n",
	  allocbytes, alloccalls);
  fprintf(stderr, "Total regions created %lu\n", allocregions);

  if (rcs_adjust || rcs_adjust_struct || rcs_adjust_array || rcs_global ||
      rcs_global_struct || rcs_update || rcs_update_same || rcs_update_struct)
    fprintf(stderr, "RC: Adjust: %lu/%lu struct/%lu array.\n"
	    "    Global: %lu+%lu(trad)/%lu struct.\n"
	    "    Update: %lu+%lu(parent)+%lu(same)+%lu(trad)/%lu struct.\n",
	    rcs_adjust, rcs_adjust_struct, rcs_adjust_array, 
	    rcs_global, rcs_global_traditional, rcs_global_struct,
	    rcs_update, rcs_update_parent, rcs_update_same, rcs_update_traditional,
	    rcs_update_struct);
  if (profile_table)
    print_rcs_profile();
  fflush(stderr);
}
#endif

#if 0
static void *bounded_sub(void *p, __rcintptr n)
{
  if ((__rcintptr)p < n) return 0;
  else return (void *)((__rcintptr)p - n);
}

static void *bounded_add(void *p, __rcintptr n)
{
  if ((__rcintptr)p > (__rcintptr)-n) return 0;
  else return (void *)((__rcintptr)p + n);
}
#endif

#ifdef HEAPNAME
#include <sys/times.h>
#include <unistd.h>
#include <fcntl.h>

static struct tms s, e;

double ll_to_double(unsigned long long x)
{
  return 4294967296.0 * (long)(x >> 32) + (unsigned long)x;
}

#ifdef sparc
#if 1
#include <sys/processor.h>
#include <sys/procset.h>

unsigned long long start_ticks, end_ticks, region_ticks;


static void print_benchmark(void)
{
  double tt, pfreq;
  processor_info_t pinfo;

  asm("rd %%tick,%%g1;stx %%g1,%0" : "=m" (end_ticks));
  times(&e);
  processor_info(0, &pinfo);
  pfreq = pinfo.pi_clock * 1e6;

  tt = ll_to_double(end_ticks - start_ticks) / pfreq;

  fprintf(stderr, "runtime: %.3f\n", tt);
  fprintf(stderr, "cputime: %.2f\n", (e.tms_utime - s.tms_utime + e.tms_stime - s.tms_stime) / 100.0);
  fprintf(stderr, "unscantime: %.3f\n", ll_to_double(unscantime) / pfreq);
  fprintf(stderr, "catime: %.3f\n", ll_to_double(catime) / pfreq);

  asm("stx %%g7,%0" : "=m" (region_ticks));
  if (region_ticks)
    {
      double rt = ll_to_double(region_ticks) / pfreq;

      fprintf(stderr, "memtime: %.3f (%lu)\n", rt, (unsigned long)region_ticks);
    }

#if 0
  if (perturb_counter)
    fprintf(stderr, "%lu perturbations\n", perturb_counter);
#endif
}

void benchmark_init(void)
{
  int useproc = getenv("PROC") ? atoi(getenv("PROC")) : 0;

  fputs("heap: " HEAPNAME "\n", stderr);
  if (processor_bind(P_PID, getpid(), useproc, NULL) != 0)
    {
      perror("processor_bind");
      exit(2);
    }

  times(&s);
  asm("rd %%tick,%%g1;stx %%g1,%0" : "=m" (start_ticks));
  asm("set 0,%g7");

  atexit(print_benchmark);
}
#else
#include <sys/processor.h>
#include <sys/procset.h>
#include "perfmon.h"

static int event0, event1;
unsigned long long start_ticks, end_ticks, region_ticks;

struct event
{
  unsigned long event1 __attribute__ ((aligned (8)));
  unsigned long event0;
} start_events, end_events;

static char *c0Name[16] = {
    "CycleCount",
    "InstructionCount",
    "Dispatch0_ICMiss",
    "Dispatch0_StoreBuf",
    "",
    "",
    "",
    "",
    "OVERCOUNTS: InstructionCache_Refs",
    "DataCache_Reads",
    "DataCache_Writes",
    "Load_Use",
    "ExternalCache_Refs",
    "ExternalCache_WriteHitRDOs",
    "ExternalCache_SnoopInvalidates",
    "ExternalCache_ReadHits"
};

static char *c1Name[16] = {
    "CycleCount",
    "InstructionCount",
    "Dispatch0_Mispred",
    "Dispatch0_FP_Use",
    "",
    "",
    "",
    "",
    "OVERCOUNTS: InstructionCache_Hits",
    "OPTIMISTIC: DataCache_ReadHits",
    "DataCache_WriteHits",
    "Load_UseRAW",
    "ExternalCache_Hits",
    "BAD: ExternalCache_WriteBacks",
    "ExternalCache_SnoopCopyBacks",
    "ExternalCache_ICHits"
};

static void print_benchmark(void)
{
  double tt, pfreq;
  processor_info_t pinfo;

  asm("rd %%tick,%%g1;stx %%g1,%0" : "=m" (end_ticks));
  asm("rd %%pic,%%g1;stx %%g1,%0" : "=m" (end_events));
  times(&e);
  processor_info(0, &pinfo);
  pfreq = pinfo.pi_clock * 1e6;

  tt = ll_to_double(end_ticks - start_ticks) / pfreq;

  fprintf(stderr, "runtime: %.3f\n", tt);
  fprintf(stderr, "cputime: %.2f\n", (e.tms_utime - s.tms_utime + e.tms_stime - s.tms_stime) / 100.0);
  fprintf(stderr, "unscantime: %.3f\n", ll_to_double(unscantime) / pfreq);
  fprintf(stderr, "catime: %.3f\n", ll_to_double(catime) / pfreq);

  fprintf(stderr, "event0 [%s]: %lu\nevent1 [%s]: %lu\n",
	  c0Name[event0], end_events.event0 - start_events.event0,
	  c1Name[event1], end_events.event1 - start_events.event1);

  asm("stx %%g7,%0" : "=m" (region_ticks));
  if (region_ticks)
    {
      double rt = ll_to_double(region_ticks) / pfreq;

      fprintf(stderr, "memtime: %.3f (%lu)\n", rt, (unsigned long)region_ticks);
    }

#if 0
  if (perturb_counter)
    fprintf(stderr, "%lu perturbations\n", perturb_counter);
#endif
}

void benchmark_init(void)
{
  int useproc = getenv("PROC") ? atoi(getenv("PROC")) : 0, fd, rc;
  unsigned long long tmp;

  event0 = getenv("EVENT0") ? atoi(getenv("EVENT0")) : 12;
  event1 = getenv("EVENT1") ? atoi(getenv("EVENT1")) : 12;

  fputs("heap: " HEAPNAME "\n", stderr);
  if (processor_bind(P_PID, getpid(), useproc, NULL) != 0)
    {
      perror("processor_bind");
      exit(2);
    }
  fd = open("/dev/perfmon", O_RDONLY);
  if (fd == -1) {
    perror("open(/dev/perfmon)");
    exit(1);
  }
  tmp = event0 << 4 | event1 << 11 | PCR_USER_TRACE;
  rc = ioctl(fd, PERFMON_SETPCR, &tmp);
  if (rc < 0) {
    perror("ioctl(PERFMON_SETPCR)");
    exit(1);
  }

  /* Flush all caches */
  ioctl(fd, PERFMON_FLUSH_CACHE);
  close(fd);
  
  times(&s);
  asm("rd %%tick,%%g1;stx %%g1,%0" : "=m" (start_ticks));
  asm("rd %%pic,%%g1;stx %%g1,%0" : "=m" (start_events));
  asm("set 0,%g7");

  atexit(print_benchmark);
}
#endif
#elif defined(i386)
#define PMC_P6
#include "pmc_dev.h"

unsigned long event0, event1;
unsigned long long se0, se1, stsc, ee0, ee1, etsc;
static double pfreq = 550e6;

static char *event_name(int evno)
{
  static char buf[3];

  sprintf(buf, "%02x", evno);

  return buf;
}

static void pevent(int n, unsigned long event, unsigned long long value)
{
  fprintf(stderr, "%d: %10s (mask: %02lx, %2s, %2s, %1s) = %Lu\n",
	  n, event_name(event & 0xff),
	  (event & 0xff00) >> 8, event & 0x10000 ? "US" : "  ",
	  event & 0x20000 ? "OS" : "  ",
	  event & 0x40000 ? "E" : "O", value);
}

static void print_benchmark(void)
{
  double tt;

  ee0 = pmc0(); ee1 = pmc1(); etsc = tsc();
  times(&e);

  tt = ll_to_double(etsc - stsc) / pfreq;

  fprintf(stderr, "runtime: %.3f\n", tt);
  fprintf(stderr, "cputime: %.2f\n", (e.tms_utime - s.tms_utime + e.tms_stime - s.tms_stime) / 100.0);
  fprintf(stderr, "unscantime: %.3f\n", ll_to_double(unscantime) / pfreq);
  fprintf(stderr, "catime: %.3f\n", ll_to_double(catime) / pfreq);

  pevent(0, event0, ee0 - se0);
  pevent(1, event1, ee1 - se1);

  if (memtime)
    {
      double rt = ll_to_double(memtime) / pfreq;

      fprintf(stderr, "memtime: %.3f (%lu)\n", rt, (unsigned long)memtime);
    }

#if 0
  if (perturb_counter)
    fprintf(stderr, "%lu perturbations\n", perturb_counter);
#endif
}

typedef struct { unsigned long c[pmc_event_counters]; } pmc_selector_t;

void benchmark_init(void)
{
  int fd;
  pmc_selector_t selevents;

  event0 = getenv("EVENT0") ? strtol(getenv("EVENT0"), NULL, 16) : 0x10F29;
  event0 |= 0x400000; /* enable counters */
  event1 = getenv("EVENT1") ? strtol(getenv("EVENT1"), NULL, 16) : 0x10F2A;

  fputs("heap: " HEAPNAME "\n", stderr);
  fd = open("/dev/pmc", O_RDWR);
  if (fd == -1) {
    perror("open(/dev/pmc)");
    exit(1);
  }
  /* Flush all caches, enable pmcs */
  lseek(fd, PMC_FLUSH_CACHE, 0);
  lseek(fd, PMC_ENABLE_RDPMC, 0);

  selevents.c[0] = event0;
  selevents.c[1] = event1;
  write(fd, &selevents, PMC_SELECT);

  close(fd);
  
  times(&s);
  se0 = pmc0(); se1 = pmc1(); stsc = tsc();

  atexit(print_benchmark);
}
#else
static void benchmark_init(void)
{
  fputs("heap: " HEAPNAME "\n", stderr);
}
#endif

#endif

void region_init(void)
{
  static int initialized = 0;
  int i;

  if (initialized) return;

#if defined(__GNUC__) && defined(sparc)
  __rcregionmap = __rcregionmaparray;
#endif

#if 0
  int dummy_local;
  static int dummy_global;
  void *start, *end;

  /* Initialise stack & static data page -> regionId tables to contain
     &zeroregion. This is somewhat hackish:
     - we assume that the stack will range from -64 megs to +1 megs from
       it's current position
     - we assume all static data is found within one meg of dummy_global
     Correct solutions will be very machine specific
  */
  set_region_range(bounded_sub(&dummy_local, 64 * 1024 * 1024),
		   bounded_add(&dummy_local, 1 * 1024 * 1024),
		   &zeroregion);
  set_region_range(bounded_sub(&dummy_global, 1 * 1024 * 1024),
		   bounded_add(&dummy_global, 1 * 1024 * 1024),
		   &zeroregion);
  /* null */
  set_region_range(0, (void *)1, &zeroregion);
#endif

#ifndef RCPAIRS_ARRAY
  for (i = 0; i < MAXPAGE; i++)
    __rcregionmap[i] = &zeroregion;
#endif
  
  rstart = 0; /* changed to 0 from -64 */

#ifdef RCPAIRS
  __rcregions[0] = &zeroregion;
#endif

  init_pages();

#ifdef REGIONPROFILE
  atexit(print_memory_usage);
#endif
#ifdef HEAPNAME
  benchmark_init();
#endif
  permanent = newregion();
  initialized = 1;
}

nomem_handler set_nomem_handler(nomem_handler newhandler)
{
  nomem_handler oldh = nomem_h;

  nomem_h = newhandler;

  return oldh;
}

#ifdef REGION_MAIN
int region_main(int argc, char **argv, char **envp);

int main(int argc, char **argv, char **envp)
{
  region_init();
  return region_main(argc, argv, envp);
}
#endif	/* REGION_MAIN */


/* Debugging support */

static FILE *out;

static void printref(void *x)
{
  if (x >= (void *)__rcregionmap && x < (void *)&__rcregionmap[MAXPAGE])
    return;

#ifdef RCPAIRS
  if (x >= (void *)__rcregions && x < (void *)&__rcregions[MAXREGIONS])
    return;

#endif

  fprintf(out, "info symbol 0x%p\n", x);
}

void findrefs(region r, void *from, void *to)
{
  char *f;

  if (!out)
    out = fopen("/dev/tty", "w");

  for (f = PALIGN(from, PTRALIGNMENT); f < (char *)to; f += PTRALIGNMENT)
    if (regionof(*(void **)f) == r)
      printref(f);

  fflush(out);
}

#ifdef sparc
extern void _DYNAMIC, _end;

void findgrefs(region r)
{
  findrefs(r, &_DYNAMIC, &_end);
}
#endif

void findrrefs(region r, region from)
{
  struct page *p;

  for (p = from->normal.pages; p; p = p->next)
    findrefs(r, (char *)&p->previous, (char *)p + RPAGESIZE);

  for (p = r->normal.bigpages; p; p = p->next)
    findrefs(r, (char *)&p->previous, (char *)p + p->pagecount * RPAGESIZE);
}


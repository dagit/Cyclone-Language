/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

// This is the C "runtime library" to be used with the output of the
// Cyclone to C translator

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h> // for clock()

#include "runtime_internal.h"

#ifndef CYC_REGION_PROFILE
// defined in cyc_include.h when profiling turned on
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};
#endif

#ifdef CYC_REGION_PROFILE
static FILE *alloc_log = NULL;
extern unsigned int GC_gc_no;
#endif
extern size_t GC_get_heap_size();
extern size_t GC_get_free_bytes();
extern size_t GC_get_total_bytes();

static int region_get_heap_size(struct _RegionHandle *r);
static int region_get_free_bytes(struct _RegionHandle *r);
static int region_get_total_bytes(struct _RegionHandle *r);

// FIX: I'm putting GC_calloc and GC_calloc_atomic in here as just
// calls to GC_malloc and GC_malloc_atomic respectively.  This will
// *not* work for the nogc option.
void *GC_calloc(unsigned int n, unsigned int t) {
  return (void *)GC_malloc(n*t);
}

void *GC_calloc_atomic(unsigned int n, unsigned int t) {
  unsigned int p = n*t;
  // the collector does not zero things when you call malloc atomic...
  void *res = GC_malloc_atomic(p);
  if (res == NULL) {
    fprintf(stderr,"GC_calloc_atomic failure");
    _throw_badalloc();
  }
  bzero(res,p);
  return res;
}

#ifdef CYC_REGION_PROFILE
void _profile_free_region_cleanup(struct _RuntimeStack *handler) {
  _profile_free_region((struct _RegionHandle *)handler,NULL,NULL,0);
}
#endif

void _push_region(struct _RegionHandle * r) {
  //fprintf(stderr,"pushing region %x\n",(unsigned int)r);  
  r->s.tag = 1;
#ifdef CYC_REGION_PROFILE
  r->s.cleanup = _profile_free_region_cleanup;
#else
  r->s.cleanup = (void (*)(struct _RuntimeStack *))_free_region;
#endif
  _push_frame((struct _RuntimeStack *)r);
}

void _pop_region() {
  if (_top_frame() == NULL || _top_frame()->tag != 1) {
    fprintf(stderr,"internal error: _pop_region");
    exit(1);
  }
  _npop_frame(0);
}

#ifdef CYC_REGION_PROFILE
int rgn_total_bytes = 0;
static int rgn_freed_bytes = 0;
static int heap_total_bytes = 0;
static int heap_total_atomic_bytes = 0;
static int unique_total_bytes = 0;
static int unique_freed_bytes = 0;
static int refcnt_total_bytes = 0;
static int refcnt_freed_bytes = 0;
#endif


// exported in core.h
#define CYC_CORE_HEAP_REGION (NULL)
#define CYC_CORE_UNIQUE_REGION ((struct _RegionHandle *)1)
#define CYC_CORE_REFCNT_REGION ((struct _RegionHandle *)2)
struct _RegionHandle *Cyc_Core_heap_region = CYC_CORE_HEAP_REGION;
struct _RegionHandle *Cyc_Core_unique_region = CYC_CORE_UNIQUE_REGION;
struct _RegionHandle *Cyc_Core_refcnt_region = CYC_CORE_REFCNT_REGION;

/////// UNIQUE REGION //////////

// for freeing unique pointers; might want to make this "free"
// eventually (but currently this is set to a no-op in libc.cys).
// Note that this is not recursive; it assumes that programmer
// has freed nested pointers (otherwise will be grabbed by the GC).
void Cyc_Core_ufree(unsigned char *ptr) {
  if (ptr == NULL) return;
  else {
#ifdef CYC_REGION_PROFILE
    unsigned int sz = GC_size(ptr);
    unique_freed_bytes += sz;
    // output special "alloc" event here, where we have a negative size
    if (alloc_log != NULL) {
      fprintf(alloc_log,"%u @\tunique\talloc\t-%d\t%d\t%d\t%d\t%x\n",
              clock(),
	      sz,
	      region_get_heap_size(CYC_CORE_UNIQUE_REGION), 
	      region_get_free_bytes(CYC_CORE_UNIQUE_REGION),
	      region_get_total_bytes(CYC_CORE_UNIQUE_REGION),
              (unsigned int)ptr);
    }
    // FIX:  JGM -- I moved this before the endif -- it was after,
    // but I'm pretty sure we don't need this unless we're profiling.
    GC_register_finalizer_no_order(ptr,NULL,NULL,NULL,NULL);
#endif
    GC_free(ptr);
  }
}

/////// REFERENCE-COUNTED REGION //////////

/* XXX need to make this 2 words in advance, for double-word alignment? */
static int *get_refcnt(unsigned char *ptr) {
  if (ptr == NULL) return NULL;
  else {
    int *res = (int *)(ptr - sizeof(int));
/*     fprintf(stderr,"getting count, refptr=%x, cnt=%x\n",ptr.base,res); */
    return res;
  }
}

// Assumes no pointer arithmetic on reference-counted pointers, so
// that this is always pointing to the base of the pointer.
int Cyc_Core_refptr_count(unsigned char *ptr) {
  int *cnt = get_refcnt(ptr);
  if (cnt != NULL) return *cnt;
  else return 0;
}

// Need to use a fat pointer here, so that we preserve the bounds
// information when aliasing.
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr) {
  int *cnt = get_refcnt(ptr.base);
  if (cnt != NULL) *cnt = *cnt + 1;
/*   fprintf(stderr,"refptr=%x, cnt=%x, updated *cnt=%d\n",ptr.base,cnt, */
/* 	  cnt != NULL ? *cnt : 0); */
  return ptr;
}

void Cyc_Core_drop_refptr(unsigned char *ptr) {
  int *cnt = get_refcnt(ptr);
  if (cnt != NULL) {
/*     fprintf(stderr,"refptr=%x, cnt=%x, *cnt=%d\n",ptr.base,cnt,*cnt); */
    *cnt = *cnt - 1;
    if (*cnt == 0) { // no more references
/*       fprintf(stderr,"freeing refptr=%x\n",ptr.base); */
#ifdef CYC_REGION_PROFILE
      unsigned int sz = GC_size(ptr - sizeof(int));
      refcnt_freed_bytes += sz;
      if (alloc_log != NULL) {
	fprintf(alloc_log,"%u @\trefcnt\talloc\t-%d\t%d\t%d\t%d\t%x\n",
                clock(),
		sz,
		region_get_heap_size(CYC_CORE_REFCNT_REGION), 
		region_get_free_bytes(CYC_CORE_REFCNT_REGION),
		region_get_total_bytes(CYC_CORE_REFCNT_REGION),
                (unsigned int)ptr);
      }
#endif
      GC_free(ptr - sizeof(int));
    }
  }
}

/////////////////////////////////////////////////////////////////////

void _init_regions() {
#ifdef CYC_REGION_PROFILE
  alloc_log = fopen("amon.out","w");
#endif
}

void _fini_regions() {
#ifdef CYC_REGION_PROFILE
  fprintf(stderr,"rgn_total_bytes: %d\n",rgn_total_bytes);
  fprintf(stderr,"rgn_freed_bytes: %d\n",rgn_freed_bytes);
  fprintf(stderr,"heap_total_bytes: %d\n",heap_total_bytes);
  fprintf(stderr,"heap_total_atomic_bytes: %d\n",heap_total_atomic_bytes);
  fprintf(stderr,"unique_total_bytes: %d\n",unique_total_bytes);
  fprintf(stderr,"unique_free_bytes: %d\n",unique_freed_bytes);
  fprintf(stderr,"refcnt_total_bytes: %d\n",refcnt_total_bytes);
  fprintf(stderr,"refcnt_free_bytes: %d\n",refcnt_freed_bytes);
  fprintf(stderr,"number of collections: %d\n",GC_gc_no);
  if (alloc_log != NULL)
    fclose(alloc_log);
#endif
}

// minimum page size for a region
#define CYC_MIN_REGION_PAGE_SIZE 480

// controls the default page size for a region
static size_t default_region_page_size = CYC_MIN_REGION_PAGE_SIZE;

// set the default region page size -- returns false and has no effect 
// if s is not at least CYC_MIN_REGION_PAGE_SIZE
bool Cyc_set_default_region_page_size(size_t s) {
  if (s < CYC_MIN_REGION_PAGE_SIZE) 
    return 0;
  default_region_page_size = s;
  return 1;
}

static void grow_region(struct _RegionHandle *r, unsigned int s) {
  struct _RegionPage *p;
  unsigned int prev_size, next_size;

  prev_size = r->last_plus_one - ((char *)r->curr + sizeof(struct _RegionPage));
  next_size = prev_size * 2;

  if (next_size < s) 
    next_size = s + default_region_page_size;

  // Note, we call calloc here to ensure we get zero-filled pages
  p = GC_calloc(sizeof(struct _RegionPage) + next_size,1);
  if (!p) {
    fprintf(stderr,"grow_region failure");
    _throw_badalloc();
  }
  p->next = r->curr;
#ifdef CYC_REGION_PROFILE
  p->total_bytes = GC_size(p);
  p->free_bytes = next_size;
  if (alloc_log != NULL) {
    fprintf(alloc_log,"%u @\t%s\tresize\t%d\t%d\t%d\t%d\n",
            clock(),
	    r->name,
	    GC_size(p),
	    GC_get_heap_size(), GC_get_free_bytes(), GC_get_total_bytes());
  }
#else
  r->used_bytes += next_size;
  r->wasted_bytes += r->last_plus_one - r->offset;
#endif
  r->curr = p;
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + next_size;
}

// better not get called with the heap or unique regions ...
static void _get_first_region_page(struct _RegionHandle *r, unsigned int s) {
  struct _RegionPage *p;
  unsigned int page_size = 
    default_region_page_size < s ? s : default_region_page_size;
  p = (struct _RegionPage *)GC_calloc(sizeof(struct _RegionPage) + 
                                      page_size,1);
  if (p == NULL) 
    _throw_badalloc();
#ifdef CYC_REGION_PROFILE
  p->total_bytes = GC_size(p);
  p->free_bytes = page_size;
  if (alloc_log != NULL) {
    fprintf(alloc_log,"%u @\t%s\tresize\t%d\t%d\t%d\t%d\n",
            clock(),
	    r->name,
	    GC_size(p),
	    GC_get_heap_size(), GC_get_free_bytes(), GC_get_total_bytes());
  }
#endif
  p->next = NULL;
  r->curr = p;
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + page_size;
#ifndef CYC_REGION_PROFILE
  r->used_bytes = page_size;
  r->wasted_bytes = 0;
#endif
}

// allocate s bytes within region r
void * _region_malloc(struct _RegionHandle *r, unsigned int s) {
  char *result;
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
#ifndef CYC_NOALIGN
    // round s up to the nearest _CYC_MIN_ALIGNMENT value
    s =  (s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT - 1));
#endif
    // if no page yet, then fetch one
    if (r->curr == 0) {
      _get_first_region_page(r,s);
      result = r->offset;
    } else {
      result = r->offset;
      // else check for space on the current page
      if (s > (r->last_plus_one - result)) {
        grow_region(r,s);
        result = r->offset;
      }
    }
    r->offset = result + s;
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
    return (void *)result;
  } else if (r != CYC_CORE_REFCNT_REGION) {
    result = GC_malloc(s);
    if(!result)
      _throw_badalloc();
#ifdef CYC_REGION_PROFILE
    {
      unsigned int actual_size = GC_size(result);
      if (r == CYC_CORE_HEAP_REGION)
	heap_total_bytes += actual_size;
      else
	unique_total_bytes += actual_size;
    }
#endif
    return (void *)result;
  }
  else { // (r == CYC_CORE_REFCNT_REGION)
    // need to add a word for the reference count.  We use a word to
    // keep the resulting memory word-aligned.  Then bump the pointer.
    // FIX: probably need to keep it double-word aligned!
    result = GC_malloc(s+sizeof(int));
    if(!result)
      _throw_badalloc();
    *(int *)result = 1;
#ifdef CYC_REGION_PROFILE
    refcnt_total_bytes += GC_size(result);
#endif
    result += sizeof(int);
/*     fprintf(stderr,"alloc refptr=%x\n",result); */
    return (void *)result;
  }
}

// allocate s bytes within region r
void * _region_calloc(struct _RegionHandle *r, unsigned int n, unsigned int t) 
{
  unsigned int s = n*t;
  char *result;
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    // round s up to the nearest _CYC_MIN_ALIGNMENT value
#ifndef CYC_NOALIGN
    s =  (s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT - 1));
#endif
    // if no page yet, then fetch one
    if (r->curr == 0) {
      _get_first_region_page(r,s);
      result = r->offset;
    } else {
      result = r->offset;
      // else check for space on the current page
      if (s > (r->last_plus_one - result)) {
        grow_region(r,s);
        result = r->offset;
      }
    }
    r->offset = result + s;
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
    return (void *)result;
  } else if (r != CYC_CORE_REFCNT_REGION) {
    // allocate in the heap
    result = GC_calloc(n,t);
    if(!result)
      _throw_badalloc();
#ifdef CYC_REGION_PROFILE
    {
      unsigned int actual_size = GC_size(result);
      if (r == Cyc_Core_heap_region)
	heap_total_bytes += actual_size;
      else 
	unique_total_bytes += actual_size;
    }
#endif
    return result;
  } else { // r == CYC_CORE_REFCNT_REGION)
    // allocate in the heap + 1 word for the refcount
    result = GC_calloc(n*t+sizeof(int),1);
    if(!result)
      _throw_badalloc();
    *(int *)result = 1;
#ifdef CYC_REGION_PROFILE
    refcnt_total_bytes += GC_size(result);
#endif
    result += sizeof(int);
    return result;
  }
}

// allocate a new page and return a region handle for a new region.
struct _RegionHandle _new_region(const char *rgn_name) {
  struct _RegionHandle r;

  /* we're now lazy about allocating a region page */
  //struct _RegionPage *p;

  // we use calloc to make sure the memory is zero'd
  //p = (struct _RegionPage *)GC_calloc(sizeof(struct _RegionPage) + 
  //                                  default_region_page_size,1);
  //if (p == NULL) 
  //  _throw_badalloc();
  //p->next = NULL;
#ifdef CYC_REGION_PROFILE
  //p->total_bytes = sizeof(struct _RegionPage) + default_region_page_size;
  //p->free_bytes = default_region_page_size;
  r.name = rgn_name;
#else
  r.used_bytes = 0;
  r.wasted_bytes = 0;
#endif
  r.s.tag = 1;
  r.s.next = NULL;
  r.curr = 0;
  r.offset = 0;
  r.last_plus_one = 0;
  //r.offset = ((char *)p) + sizeof(struct _RegionPage);
  //r.last_plus_one = r.offset + default_region_page_size;
  return r;
}

// free all the resources associated with a region (except the handle)
//   (assumes r is not heap or unique region)
void _free_region(struct _RegionHandle *r) {
  struct _DynRegionHandle *d;
  struct _RegionPage *p;

  /* free pages */
  p = r->curr;
  while (p != NULL) {
    struct _RegionPage *n = p->next;
#ifdef CYC_REGION_PROFILE
    unsigned int sz = GC_size(p);
    GC_free(p);
    rgn_freed_bytes += sz;
    if (alloc_log != NULL) {
      fprintf(alloc_log,"%u @\t%s\tresize\t-%d\t%d\t%d\t%d\n",
              clock(),
	      r->name, sz,
	      GC_get_heap_size(), GC_get_free_bytes(), GC_get_total_bytes());
    }
#else
    GC_free(p);
#endif
    p = n;
  }
  r->curr = 0;
  r->offset = 0;
  r->last_plus_one = 0;
}

// Dynamic Regions
// Note that struct Cyc_Core_DynamicRegion is defined in cyc_include.h.

// We use this struct when returning a newly created dynamic region.
// The wrapper is needed because the Cyclone interface uses an existential.
struct Cyc_Core_NewDynamicRegion {
  struct Cyc_Core_DynamicRegion *key;
};

// XXX need to include in profiling stuff ...
// Create a new dynamic region and return a unique pointer for the key.
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_ukey() {
  struct Cyc_Core_NewDynamicRegion res;
  res.key = GC_malloc(sizeof(struct Cyc_Core_DynamicRegion));
  if (!res.key)
    _throw_badalloc();
  res.key->h = _new_region("dynamic_unique");
  return res;
}
// Destroy a dynamic region, given the unique key to it.
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion *k) {
  _free_region(&k->h);
  GC_free(k);
}

// XXX change to use refcount routines above
// Create a new dynamic region and return a reference-counted pointer 
// for the key.
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey() {
  struct Cyc_Core_NewDynamicRegion res;
  int *krc = GC_malloc(sizeof(int)+sizeof(struct Cyc_Core_DynamicRegion));
  //fprintf(stderr,"creating rckey.  Initial address is %x\n",krc);fflush(stderr);
  if (!krc)
    _throw_badalloc();
  *krc = 1;
  res.key = (struct Cyc_Core_DynamicRegion *)(krc + 1);
  //fprintf(stderr,"results key address is %x\n",res.key);fflush(stderr);
  res.key->h = _new_region("dynamic_refcnt");
  return res;
}
// Drop a reference for a dynamic region, possibly freeing it.
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion *k) {
  //fprintf(stderr,"freeing rckey %x\n",k);
  int *p = ((int *)k) - 1;
  //fprintf(stderr,"count is address %x, value %d\n",p,*p);
  unsigned c = (*p) - 1;
  if (c >= *p) {
    fprintf(stderr,"internal error: free rckey bad count");
    exit(1);
  }
  *p = c;
  if (c == 0) {
    //fprintf(stderr,"count at zero, freeing region\n");
    _free_region(&k->h);
    //fprintf(stderr,"freeing ref-counted pointer\n");
    GC_free(p);
  }
}

// Open a key (unique or reference-counted), extract the handle
// for the dynamic region, and pass it along with env to the
// body function pointer, returning the result.
void *Cyc_Core_open_region(struct Cyc_Core_DynamicRegion *k,
                           void *env,
                           void *body(struct _RegionHandle *h,
                                      void *env)) {
  return body(&k->h,env);
}

#ifdef CYC_REGION_PROFILE

// called before each profiled allocation to see if a GC has occurred
// since the last allocation, and if so makes a log entry for it based
// on the amount of freed data.
static void _profile_check_gc() {
  static unsigned int last_GC_no = 0;
  if (GC_gc_no != last_GC_no) {
    last_GC_no = GC_gc_no;
    if (alloc_log != NULL) {
      fprintf(alloc_log,"%u gc-%d\theap\tgc\t%d\t%d\t%d\n",
              clock(),
	      last_GC_no, GC_get_heap_size(),
	      GC_get_free_bytes(), GC_get_total_bytes());
    }
  }
}

static int region_get_heap_size(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    struct _RegionPage *p = r->curr;
    int sz = 0;
    while (p != NULL) {
      sz += p->total_bytes;
      p = p->next;
    }  
    return sz;
  }
  else
    return GC_get_heap_size();
}

/* Two choices: print the "effective" free bytes, which are just
   the ones in the current page, or print the non-allocated bytes,
   which are the free bytes in all the pages.  Doing the former. */
static int region_get_free_bytes(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    struct _RegionPage *p = r->curr;
    if (p != NULL)
      return p->free_bytes;
    else 
      return 0;
  }
  else
    return GC_get_free_bytes();
}

static int region_get_total_bytes(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    struct _RegionPage *p = r->curr;
    int sz = 0;
    while (p != NULL) {
      sz = sz + (p->total_bytes - p->free_bytes);
      p = p->next;
    }  
    return sz;
  }
  else {
    unsigned int unique_avail_bytes =
      unique_total_bytes - unique_freed_bytes;
    unsigned int refcnt_avail_bytes =
      refcnt_total_bytes - refcnt_freed_bytes;
    if (r == CYC_CORE_UNIQUE_REGION) return unique_avail_bytes;
    else if (r == CYC_CORE_REFCNT_REGION) return refcnt_avail_bytes;
    else return GC_get_total_bytes() - unique_avail_bytes - refcnt_avail_bytes;
  }
}

struct _RegionHandle _profile_new_region(const char *rgn_name, 
					 const char *file,
                                         const char *func,
                                         int lineno) {
  int len;
  char *buf;
  static int cnt = 0;

  len = strlen(rgn_name)+10;
  buf = GC_malloc_atomic(len);
  if(!buf)
    _throw_badalloc();
  else
    snprintf(buf,len,"%d-%s",cnt++,rgn_name);

  if (alloc_log != NULL) {
    fprintf(alloc_log,"%u %s:%s:%d\t%s\tcreate\t%d\t%d\t%d\n",
            clock(),
            file,func,
	    lineno,buf,
	    GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
  }

  return _new_region(buf);
}

void _profile_free_region(struct _RegionHandle *r, const char *file, const char *func, int lineno) {
  // should never be heap, unique, or refcnt ...
  _free_region(r);
  if (alloc_log != NULL) {
    if (file == NULL)
      fprintf(alloc_log,"%u @\t%s\tfree\t%d\t%d\t%d\n",
              clock(),
              (r == CYC_CORE_HEAP_REGION ? "heap" :
               (r == CYC_CORE_UNIQUE_REGION ? "unique" :
                (r == CYC_CORE_REFCNT_REGION ? "refcnt" : r->name))),
              GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
    else
      fprintf(alloc_log,"%u %s:%s:%d\t%s\tfree\t%d\t%d\t%d\n",
              clock(),
              file,func,lineno,
              (r == CYC_CORE_HEAP_REGION ? "heap" :
               (r == CYC_CORE_UNIQUE_REGION ? "unique" :
                (r == CYC_CORE_REFCNT_REGION ? "refcnt" : r->name))),
              GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
  }
}

typedef void * GC_PTR; /* Taken from gc/include/gc.h, must be kept in sync. */

static void reclaim_finalizer(GC_PTR obj, GC_PTR client_data) {
  if (alloc_log != NULL)
    fprintf(alloc_log,
            "%u @ @ reclaim \t%x\n",
            clock(),
            (unsigned int)obj);
}

static void set_finalizer(GC_PTR addr) {
  GC_register_finalizer_no_order(addr,reclaim_finalizer,NULL,NULL,NULL);
}

void * _profile_region_malloc(struct _RegionHandle *r, unsigned int s,
                              const char *file, const char *func, int lineno) {
  char *addr;
  addr = _region_malloc(r,s);
  _profile_check_gc();
  if (alloc_log != NULL) {
    if (r == CYC_CORE_HEAP_REGION) {
      s = GC_size(addr);
      set_finalizer((GC_PTR)addr);
    }
    if (r == CYC_CORE_UNIQUE_REGION) {
      s = GC_size(addr);
      set_finalizer((GC_PTR)addr);
    }
    else if (r == CYC_CORE_REFCNT_REGION)
      s = GC_size(addr-sizeof(int)); // back up to before the refcnt
    fprintf(alloc_log,"%u %s:%s:%d\t%s\talloc\t%d\t%d\t%d\t%d\t%x\n",
            clock(),
            file,func,lineno,
	    (r == CYC_CORE_HEAP_REGION ? "heap" :
	     (r == CYC_CORE_UNIQUE_REGION ? "unique" :
	      (r == CYC_CORE_REFCNT_REGION ? "refcnt" : r->name))), s,
	    region_get_heap_size(r), 
	    region_get_free_bytes(r),
	    region_get_total_bytes(r),
            (unsigned int)addr);
  }
  return (void *)addr;
}

void * _profile_region_calloc(struct _RegionHandle *r, unsigned int t1,
                              unsigned int t2,
                              const char *file, const char *func, int lineno) {
  char *addr;
  unsigned s = t1 * t2;
  addr = _region_calloc(r,t1,t2);
  _profile_check_gc();
  if (alloc_log != NULL) {
    if (r == CYC_CORE_HEAP_REGION) {
      s = GC_size(addr);
      set_finalizer((GC_PTR)addr);
    }
    if (r == CYC_CORE_UNIQUE_REGION) {
      s = GC_size(addr);
      set_finalizer((GC_PTR)addr);
    }
    else if (r == CYC_CORE_REFCNT_REGION)
      s = GC_size(addr-sizeof(int)); // back up to before the refcnt
    fprintf(alloc_log,"%u %s:%s:%d\t%s\talloc\t%d\t%d\t%d\t%d\t%x\n",
            clock(),
            file,func,lineno,
	    (r == CYC_CORE_HEAP_REGION ? "heap" :
	     (r == CYC_CORE_UNIQUE_REGION ? "unique" :
	      (r == CYC_CORE_REFCNT_REGION ? "refcnt" : r->name))), s,
	    region_get_heap_size(r), 
	    region_get_free_bytes(r),
	    region_get_total_bytes(r),
            (unsigned int)addr);
  }
  return (void *)addr;
}

static void *_do_profile(void *result, int is_atomic,
			 const char *file, const char *func, int lineno) {  
  int n;
  set_finalizer((GC_PTR)result);
  _profile_check_gc();
  n = GC_size(result);
  heap_total_bytes += n;
  if (is_atomic) heap_total_atomic_bytes += n;
  if (alloc_log != NULL) {
    fprintf(alloc_log,"%u %s:%s:%d\theap\talloc\t%d\t%d\t%d\t%d\t%x\n",
            clock(),
            file,func,lineno,n,
	    GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes(),
            (unsigned int)result);
  }
  return result;
}

void * _profile_GC_malloc(int n, const char *file, const char *func, int lineno) {
  void * result;
  result =  GC_malloc(n);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,0,file,func,lineno);
}

void * _profile_GC_malloc_atomic(int n, const char *file, const char *func,
                                 int lineno) {
  void * result;
  result =  GC_malloc_atomic(n);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,1,file,func,lineno);
}

void * _profile_GC_calloc(unsigned n, unsigned s, const char *file, const char *func, int lineno) {
  void * result;
  result =  GC_calloc(n,s);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,0,file,func,lineno);
}

void * _profile_GC_calloc_atomic(unsigned n, unsigned s, 
				 const char *file, const char *func, int lineno) {
  void * result;
  result =  GC_calloc_atomic(n,s);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,1,file,func,lineno);
}

#else

static int region_get_heap_size(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    unsigned used_bytes = r->used_bytes;
    return used_bytes;
  }
  else
    return GC_get_heap_size();
}

static int region_get_free_bytes(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    struct _RegionPage *p = r->curr;
    unsigned free_bytes = r->last_plus_one - r->offset;
    return free_bytes;
  }
  else
    return GC_get_free_bytes();
}

static int region_get_total_bytes(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    struct _RegionPage *p = r->curr;
    unsigned used_bytes = r->used_bytes;
    unsigned wasted_bytes = r->wasted_bytes;
    unsigned free_bytes = r->last_plus_one - r->offset;
    return (used_bytes - wasted_bytes - free_bytes);
  }
  else {
    return GC_get_total_bytes();
  }
}

#endif

int region_used_bytes(struct _RegionHandle *r) {
  return region_get_heap_size(r);
}
int region_free_bytes(struct _RegionHandle *r) {
  return region_get_free_bytes(r);
}
int region_alloc_bytes(struct _RegionHandle *r) {
  return region_get_total_bytes(r);
}

// Called from gc/alloc.c for allocation profiling.  Must be
// defined even if CYC_REGION_PROFILE is not.
void CYCALLOCPROFILE_GC_add_to_heap(void *p,unsigned long bytes) {
#ifdef CYC_REGION_PROFILE
  if (alloc_log != NULL) {
    fprintf(alloc_log,"%u @\theap\tgc_add_to_heap\t%x\t%u\t%d\t%d\t%d\n",
            clock(),
            (unsigned int)p,bytes,
	    GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
  }
#endif
}
void CYCALLOCPROFILE_mark(const char *s) {
#ifdef CYC_REGION_PROFILE
  if (alloc_log != NULL) {
    fprintf(alloc_log,"%u @\t@\tmark\t%s\n",
            clock(),s);
  }
#endif
}

/******* for turning off gc warnings about blacklisted blocks *******/
/* These type/macro defns are taken from gc/include/gc.h and must
   be kept in sync. */
#   define GC_PROTO(args) args
typedef unsigned long GC_word;
typedef void (*GC_warn_proc) GC_PROTO((char *msg, GC_word arg));

extern void GC_default_warn_proc(char *msg, GC_word arg);
extern GC_warn_proc GC_set_warn_proc(GC_warn_proc p);

static void GC_noblacklist_warn_proc(char *msg, GC_word arg) {
  if (!msg) return;
  if (!strncmp(msg,"Needed to allocate blacklisted block",
               strlen("Needed to allocate blacklisted block")))
    return;
  GC_default_warn_proc(msg,arg);
}

/* Use these in Cyclone programs */
void GC_blacklist_warn_set() {
  GC_set_warn_proc(GC_default_warn_proc);
}
void GC_blacklist_warn_clear() {
  GC_set_warn_proc(GC_noblacklist_warn_proc);
}

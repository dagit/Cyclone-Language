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

#ifdef CYC_REGION_PROFILE
#include <stdarg.h>
#include <signal.h>
#include <time.h> // for clock()
#endif

#include "runtime_internal.h"
#include "bget.h"

#define _throw_bad_reapalloc() (_throw_bad_reapalloc_fn(__FILE__,__LINE__))
#ifndef CYC_REGION_PROFILE
// defined in cyc_include.h when profiling turned on
struct _RegionPage {
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};
#endif

struct _RegionAllocFunctions {
  void (*_rufree) (struct _RegionHandle*, unsigned char*);
  void (*_rdrop) (struct _RegionHandle*, struct _fat_ptr);
  void* (*_malloc) (struct _RegionHandle*, _AliasQualHandle_t aq, unsigned int s);
  void* (*_vmalloc) (struct _RegionHandle*, unsigned int s);
  void* (*_calloc) (struct _RegionHandle*, _AliasQualHandle_t aq, unsigned int n, unsigned int t);
};



#define POOLSIZE 256

struct _pool {
  size_t         count;
  struct _pool  *next;
  unsigned char *pointers[POOLSIZE];
};

// defined in cyc_include.h when profiling turned on
#ifdef CYC_REGION_PROFILE
static FILE *alloc_log = NULL;
extern unsigned int GC_gc_no;
#endif
extern size_t GC_get_heap_size();
extern size_t GC_get_free_bytes();
extern size_t GC_get_total_bytes();

extern void GC_free(void *);
extern void *GC_realloc(void *,unsigned);

static int region_get_heap_size(struct _RegionHandle *);
static int region_get_free_bytes(struct _RegionHandle *);
static int region_get_total_bytes(struct _RegionHandle *);
void _free_region(struct _RegionHandle *);

// FIX: I'm putting GC_calloc and GC_calloc_atomic in here as just
// calls to GC_malloc and GC_malloc_atomic respectively.  This will
// *not* work for the nogc option.
void *GC_calloc(unsigned int n, unsigned int t) {
  return (void *)GC_malloc(n*t);
}


#if(defined(__linux__) && defined(__KERNEL__)) 
extern void* cyc_krealloc(void*, size_t, size_t);
#endif

void *GC_realloc_hint(void *x, size_t old_size, size_t new_size) {
#if (defined(__linux__) && defined(__KERNEL__))
  return cyc_krealloc(x, old_size, new_size);
#else 
  return (void *)GC_realloc(x, new_size);
#endif
}

void *GC_calloc_atomic(unsigned int n, unsigned int t) {
  unsigned int p = n*t;
  // the collector does not zero things when you call malloc atomic...
  void *res = GC_malloc_atomic(p);
  if (res == NULL) {
    errprintf("GC_calloc_atomic failure");
    _throw_badalloc();
  }
  bzero(res,p);
  return res;
}

void *_bounded_GC_malloc(int n, const char*file, int lineno){
  void * res = NULL;
  if((unsigned)n >= MAX_ALLOC_SIZE){
    errprintf( "malloc size ( = %d ) is too big or negative\n",n);
    _throw_badalloc_fn(file, lineno);
  }else{
    res = GC_malloc(n) ;
    if (res == NULL){
      _throw_badalloc_fn(file, lineno);
    }
  }
  return res;
}
void *_bounded_GC_malloc_atomic(int n, const char*file, int lineno){
  void * res = NULL;
  if((unsigned)n >= MAX_ALLOC_SIZE){
    errprintf( "malloc size ( = %d ) is too big or negative\n",n);
    _throw_badalloc_fn(file, lineno);
  }else{
    res = GC_malloc_atomic(n);
    if( res == NULL){
      _throw_badalloc_fn(file,lineno);
    }
  }
  return res;
}
void *_bounded_GC_calloc(unsigned n, unsigned s, const char*file, int lineno){
  unsigned int p = n*s;
  void * res = NULL;
  if(p >= MAX_ALLOC_SIZE){
    errprintf( "calloc size ( = %d ) is too big or negative\n",p);
    _throw_badalloc_fn(file, lineno);
  }else{
    res =GC_calloc(n,s);
    if (res == NULL){
      _throw_badalloc_fn(file, lineno);
    }
  }
  return res;
} 
void *_bounded_GC_calloc_atomic(unsigned n,unsigned s,const char*file,int lineno){
  unsigned int p = n*s;
  void * res = NULL;
  if(p >= MAX_ALLOC_SIZE){
    errprintf( "calloc size ( = %d ) is too big or negative\n",p);
    _throw_badalloc_fn(file, lineno);
  }else{
    res = GC_calloc(n,s);
    if (res == NULL){
      _throw_badalloc_fn(file,lineno);
    }
  }
  return res;
}

#ifdef CYC_REGION_PROFILE
void _profile_free_region_cleanup(struct _RuntimeStack *handler) {
  _profile_free_region((struct _RegionHandle *)handler,NULL,NULL,0);
}
#endif

void _push_region(struct _RegionHandle * r) {
  //errprintf("pushing region %x\n",(unsigned int)r);  
  r->s.tag = LIFO_REGION;
#ifdef CYC_REGION_PROFILE
  r->s.cleanup = _profile_free_region_cleanup;
#else
  r->s.cleanup = (void (*)(struct _RuntimeStack *))_free_region;
#endif
  _push_frame((struct _RuntimeStack *)r);
}
void _pop_region() {
  if (_top_frame() == NULL || _top_frame()->tag != LIFO_REGION) {
    errquit("internal error: _pop_region");
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
struct _RegionHandle *Cyc_Core_heap_region = CYC_CORE_HEAP_REGION;

#define CYC_CORE_ALIASABLE_AQUAL ((_AliasQualHandle_t)0)
#define CYC_CORE_UNIQUE_AQUAL    ((_AliasQualHandle_t)1)
#define CYC_CORE_REFCNT_AQUAL    ((_AliasQualHandle_t)2)
#define CYC_CORE_DYNTRK_AQUAL    ((_AliasQualHandle_t)3)

_AliasQualHandle_t Cyc_Core_aliasable_qual = CYC_CORE_ALIASABLE_AQUAL;
_AliasQualHandle_t Cyc_Core_unique_qual = CYC_CORE_UNIQUE_AQUAL;
_AliasQualHandle_t Cyc_Core_refcnt_qual = CYC_CORE_REFCNT_AQUAL;

struct _RegionHandle *Cyc_Core_current_handle(void) {
  struct _RegionHandle *h = (struct _RegionHandle *)_frame_until(LIFO_REGION,0);
  return h == NULL ? Cyc_Core_heap_region : h;
}

//Fwd decls for refcnt util functions
static int *get_refcnt(unsigned char *ptr); //returns header word
static void drop_refptr_base(unsigned char *ptr); //used for auto release pools in reaps

#define CYC_MIN_REGION_PAGE_SIZE 480
static size_t default_region_page_size = CYC_MIN_REGION_PAGE_SIZE;

//////////////////////////PLAIN REGIONS//////////////////////////////
//these don't make sense for normal regions
void Cyc_Core_rufree_nop(struct _RegionHandle *r, unsigned char *ptr) {}
void Cyc_Core_rdrop_refptr_nop(struct _RegionHandle *r, struct _fat_ptr ptr) {}

static void* grow_region(void *vr, unsigned int s, bufsize *bget_incr) {
  struct _RegionHandle *r = (struct _RegionHandle*)vr;
  struct _RegionPage *p;
  unsigned int prev_size, next_size;

  prev_size = r->last_plus_one - ((char *)r->curr + sizeof(struct _RegionPage));
  next_size = prev_size * 2;

  if (next_size < s) 
    next_size = s + default_region_page_size;

  // Note, we call calloc here to ensure we get zero-filled pages
  p = _bounded_GC_calloc(sizeof(struct _RegionPage) + next_size,1,__FILE__, __LINE__);
  if (!p) {
    errprintf("grow_region failure");
    _throw_badalloc();
  }
  p->next = r->curr;
  r->used_bytes += next_size;
  r->wasted_bytes += r->last_plus_one - r->offset;
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
#endif
  r->curr = p;
  if(bget_incr) {
    *bget_incr = next_size;
  }
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + next_size;
  return (void*)r->curr->data;
}

static void* acquire_reap_page(void *vr, bufsize s, bufsize *incr);
// better not get called with the heap or unique regions ...
static void get_first_region_page(struct _RegionHandle *r, unsigned int s, int init_bget) {
  struct _RegionPage *p;
  unsigned int page_size = 
    default_region_page_size < s ? s : default_region_page_size;
  p = (struct _RegionPage *)_bounded_GC_calloc(sizeof(struct _RegionPage) + 
                                      page_size,1, __FILE__, __LINE__);
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
  if(init_bget) {
    r->key = bget_init_region(r->curr->data, page_size);
    if(!r->key) 
      _throw_bad_reapalloc();
    r->key->priv = r;
    //    bectl(r->key, NULL, grow_region, NULL, default_region_page_size);
    bectl(r->key, NULL, acquire_reap_page, NULL, default_region_page_size);
  }
  r->offset = ((char *)p) + sizeof(struct _RegionPage);
  r->last_plus_one = r->offset + page_size;
  r->used_bytes = page_size;
  r->wasted_bytes = 0;
}


// allocate s bytes within region r
static void * region_malloc_impl(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int s) {
  char *result;
  if(r == CYC_CORE_HEAP_REGION) { //no reaps for now
    if(aq == CYC_CORE_REFCNT_AQUAL) {
      // need to add a word for the reference count.  We use a word to
      // keep the resulting memory word-aligned.  Then bump the pointer.
      // FIX: probably need to keep it double-word aligned!
      result = _bounded_GC_malloc(s+sizeof(int),__FILE__, __LINE__);
      if(!result)
	_throw_badalloc();
      *(int *)result = 1;
#ifdef CYC_REGION_PROFILE
      refcnt_total_bytes += GC_size(result);
#endif
      result += sizeof(int);
      /*     errprintf("alloc refptr=%x\n",result); */
      return (void *)result;
    }
    else {
      result = _bounded_GC_malloc(s,__FILE__, __LINE__);
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
  }
  else {
#ifndef CYC_NOALIGN
    // round s up to the nearest _CYC_MIN_ALIGNMENT value
    s =  (s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT - 1));
#endif
    // if no page yet, then fetch one
    if (r->curr == 0) {
      get_first_region_page(r,s,0);
      result = r->offset;
    } else {
      result = r->offset;
      // else check for space on the current page
      if (s > (r->last_plus_one - result)) {
        grow_region(r,s,0);
        result = r->offset;
      }
    }
    r->offset = result + s;
    return (void *)result;
  } 
}

//no alias qualifiers for vmalloc
#if (defined(__linux__) && defined(__KERNEL__))
static void * region_vmalloc_impl(struct _RegionHandle *r, unsigned int s) {
  unsigned tot_bytes = s + sizeof(struct _RegionPage); 
  void *allocd_page = cyc_vmalloc(tot_bytes);//rounds to n*4096 ... careful!
  if(!allocd_page)
    _throw_badalloc();
  struct _RegionPage *new_vpage = (struct _RegionPage *)allocd_page;
  new_vpage->next = r->vpage;
  r->vpage = new_vpage;
  return (allocd_page+sizeof(struct _RegionPage));
} 
#else
static void * region_vmalloc_impl(struct _RegionHandle *r, unsigned int s) {
  return region_malloc_impl(r, CYC_CORE_ALIASABLE_AQUAL, s);
}
#endif

// allocate s bytes within region r
static void * region_calloc_impl(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int n, unsigned int t) {
  unsigned int s = n*t;
  char *result;
  if(r == CYC_CORE_HEAP_REGION) { 
    if(aq == CYC_CORE_REFCNT_AQUAL) {
      // allocate in the heap + 1 word for the refcount
      result = _bounded_GC_calloc(n*t+sizeof(int),1,__FILE__, __LINE__);
      if(!result)
	_throw_badalloc();
      *(int *)result = 1;
#ifdef CYC_REGION_PROFILE
      refcnt_total_bytes += GC_size(result);
#endif
      result += sizeof(int);
      return result;
    }
    else {
      // allocate in the heap
      result = _bounded_GC_calloc(n,t,__FILE__, __LINE__);
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
    }
  }
  else {
    // round s up to the nearest _CYC_MIN_ALIGNMENT value
#ifndef CYC_NOALIGN
    s =  (s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT - 1));
#endif
    // if no page yet, then fetch one
    if (r->curr == 0) {
      get_first_region_page(r,s,0);
      result = r->offset;
    } else {
      result = r->offset;
      // else check for space on the current page
      if (s > (r->last_plus_one - result)) {
        grow_region(r,s,0);
        result = r->offset;
      }
    }
    r->offset = result + s;
    return (void *)result;
  }
}

// free all the resources associated with a region (except the handle)
//   (assumes r is not heap or unique region)
static void free_region_impl(struct _RegionHandle *r) {
  struct _RegionPage *p = r->curr;
  struct _pool *pl = r->released_ptrs;
  /* free pages */
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
  /* free autorelease pool */
  while (pl) {
    int i;
    for (i = 0; i < pl->count; i++)
      drop_refptr_base(pl->pointers[i]);
    {
      struct _pool *next = pl->next;
#ifdef CYC_REGION_PROFILE
      unsigned int sz = GC_size(pl);
      GC_free(pl);    
      rgn_freed_bytes += sz;
      if (alloc_log != NULL) {
	fprintf(alloc_log,"%u @\t%s\tresize\t-%d\t%d\t%d\t%d\n",
		clock(),
		r->name, sz,
		GC_get_heap_size(), GC_get_free_bytes(), GC_get_total_bytes());
      }
#else 
      GC_free(pl);
#endif
      pl = next;
    }
  }
  /* free vmalloc'd pages, if linux kernel */
#if (defined(__linux__) && defined(__KERNEL__))
  p = r->vpage;
  while(p) {
    struct _RegionPage *n = p->next;
    cyc_vfree(p);
    p=n;
  }
  r->vpage = 0;
#endif
  r->curr = 0;
  r->offset = 0;
  r->last_plus_one = 0;
}


//////////////////////////////REAPS//////////////////////////////
static void reap_rufree_impl(struct _RegionHandle *r, unsigned char *ptr) {
  if(ptr == NULL || r == NULL || r->key == NULL)
    return;
  brel(r->key, ptr); //add something to bget to update page stats
}

static void* acquire_reap_page(void *vr, bufsize s, bufsize *incr) {
  struct _RegionHandle *r = (struct _RegionHandle *)vr;//key->priv;
  unsigned int ss = 2*(r->last_plus_one - ((char *)r->curr + sizeof(struct _RegionPage)));
  unsigned int next_size = ss < (2*s) ? ((2*s)+default_region_page_size) : (ss+default_region_page_size);
  
  if(r->curr == NULL) //no assert :(
    _throw_bad_reapalloc();
  void *mem =_bounded_GC_calloc(next_size + sizeof(struct _RegionPage),1, __FILE__, __LINE__);
  if(!mem)
    _throw_badalloc();
  //chain the new page onto this regions list of free pages
  struct _RegionPage *p = r->curr;
  r->curr = (struct _RegionPage*)mem;
  r->curr->next = p;
  p = r->curr;
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
#endif
  r->offset = ((char*)p + sizeof (struct _RegionPage));
  r->last_plus_one = r->offset + next_size;
  r->used_bytes += next_size;
  *incr = next_size;
  return (void*)r->offset;
}

// allocate s bytes within region r
static void * reap_malloc_impl(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int s) {
  char *result=NULL;
  if(r == CYC_CORE_HEAP_REGION) {
    if(aq == CYC_CORE_REFCNT_AQUAL) {
      // need to add a word for the reference count.  We use a word to
      // keep the resulting memory word-aligned.  Then bump the pointer.
      // FIX: probably need to keep it double-word aligned!
      result = _bounded_GC_malloc(s+sizeof(int),__FILE__, __LINE__);
      if(!result)
	_throw_badalloc();
      *(int *)result = 1;
#ifdef CYC_REGION_PROFILE
      refcnt_total_bytes += GC_size(result);
#endif
      result += sizeof(int);
      return (void *)result;
    }
    else {
      result = _bounded_GC_malloc(s,__FILE__, __LINE__);
      if(!result)
	_throw_badalloc();
#ifdef CYC_REGION_PROFILE
      unsigned int actual_size = GC_size(result);
      if(aq == CYC_CORE_UNIQUE_AQUAL){
	unique_total_bytes += actual_size;
      }
      else {
	heap_total_bytes += actual_size;
      }
#endif
      return (void *)result;
    }
  }
  else {
#ifdef TEST_PROFILER
    if(aq == CYC_CORE_REFCNT_AQUAL) 
      s += sizeof(int);
    if(r->curr == NULL) {
      get_first_region_page(r,s,0);
      result = r->offset;
    }
    else {
      result = r->offset;
      if(s > (r->last_plus_one - result)) {
	grow_region(r, s, 0);
	result = r->offset;
      }
    }
    r->offset = result + s;
    if(aq == CYC_CORE_REFCNT_AQUAL) {
      *((int*)result) = 1;
      result = (((int*)result) + 1);
    }
#else
    if(aq == CYC_CORE_REFCNT_AQUAL) s += sizeof(int);
    if(r->curr == NULL) { //first request ... initialize pool
      get_first_region_page(r, s, 1);
    }
    if(aq == CYC_CORE_REFCNT_AQUAL) {
      result = bgetz(r->key, (bufsize)s);
      if(!result)
	_throw_bad_reapalloc();
      *((int*)result) = 1;
#ifdef CYC_REGION_PROFILE
      refcnt_total_bytes += GC_size(result);
#endif
      result += sizeof(int);
    }
    else {
      if(s == 0)
    	return result;
      result = bgetz(r->key, (bufsize)s);
      if(!result)
    	_throw_bad_reapalloc();
#ifdef CYC_REGION_PROFILE
      if(aq == CYC_CORE_UNIQUE_AQUAL){
    	unsigned int actual_size = GC_size(result);
    	unique_total_bytes += actual_size;
      }
#endif
    }
#endif
    return (void*)result;
  }
}

//no alias qualifiers for vmalloc
#if (defined(__linux__) && defined(__KERNEL__))
static void * reap_vmalloc_impl(struct _RegionHandle *r, unsigned int s) {
  unsigned tot_bytes = s + sizeof(struct _RegionPage); 
  void *allocd_page = cyc_vmalloc(tot_bytes);//rounds to n*4096 ... careful!
  if(!allocd_page)
    _throw_badalloc();
  struct _RegionPage *new_vpage = (struct _RegionPage *)allocd_page;
  new_vpage->next = r->vpage;
  r->vpage = new_vpage;
  return (allocd_page+sizeof(struct _RegionPage));
} 
#else
static void * reap_vmalloc_impl(struct _RegionHandle *r, unsigned int s) {
  return reap_malloc_impl(r, CYC_CORE_ALIASABLE_AQUAL, s);
}
#endif

// allocate s bytes within region r
static void * reap_calloc_impl(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int n, unsigned int t) {
  return reap_malloc_impl(r, aq, (n*t));
}
static void * reap_aqual_malloc_impl(_AliasQualHandle_t aq, unsigned int s) {
   return reap_malloc_impl(CYC_CORE_HEAP_REGION, aq, s);
}
static void * reap_aqual_calloc_impl(_AliasQualHandle_t aq, unsigned int n, unsigned int t) {
  return reap_calloc_impl(CYC_CORE_HEAP_REGION, aq, n, t);
}

static void reap_rdrop_refptr_impl(struct _RegionHandle *r, struct _fat_ptr ptr) {
  if(r == NULL || r->key == NULL)
    return;
  int *cnt = get_refcnt(ptr.base);
  if (cnt != NULL) {
    *cnt = *cnt - 1;
    if (*cnt == 0) { // no more references //add something to bget to update page usage stats
      brel(r->key, (void*)(ptr.base - sizeof(int)));
    }
  }
}

/////////////////////////////////////////////////////////////
// EXTERNAL INTERFACE TO REGIONS AND REAPS
/////////////////////////////////////////////////////////////


void Cyc_Core_rufree(struct _RegionHandle *r, unsigned char *ptr) {
  if(r != NULL) r->fcns->_rufree(r, ptr);
/* #ifdef DISABLE_REAPS */
/* #else */
/*   reap_rufree_impl(r, ptr); */
/* #endif */
}
void Cyc_Core_rdrop_refptr(struct _RegionHandle *r, struct _fat_ptr ptr) {
  if(r != NULL) r->fcns->_rdrop(r, ptr);
/* #ifdef DISABLE_REAPS */
/* #else */
/*   if(r == NULL || r->key == NULL) */
/*     return; */
/*   int *cnt = get_refcnt(ptr.base); */
/*   if (cnt != NULL) { */
/*     *cnt = *cnt - 1; */
/*     if (*cnt == 0) { // no more references //add something to bget to update page usage stats */
/*       brel(r->key, (void*)(ptr.base - sizeof(int))); */
/*     } */
/*   } */
/* #endif */
}
void * _region_malloc(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int s) {
  if(r != NULL) return r->fcns->_malloc(r,aq,s);
  return reap_malloc_impl(r, aq, s);
/* #ifdef DISABLE_REAPS */
/*   return region_malloc_impl(r, aq, s); */
/* #else */
/*   return reap_malloc_impl(r, aq, s); */
/* #endif */
}

void * _region_vmalloc(struct _RegionHandle *r, unsigned int s) {
  if(r != NULL) return r->fcns->_vmalloc(r,s);
  return reap_vmalloc_impl(r, s);
  /* #ifdef DISABLE_REAPS */
/*   return region_vmalloc_impl(r, s); */
/* #else */
/*   return reap_vmalloc_impl(r, s); */
/* #endif */
}
void * _region_calloc(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int n, unsigned int t) {
  if(r != NULL) return r->fcns->_calloc(r,aq,n,t);
  return reap_calloc_impl(r, aq, n, t);
/* #ifdef DISABLE_REAPS */
/*   return region_calloc_impl(r, aq, n, t); */
/* #else */
/*   return reap_calloc_impl(r, aq, n, t); */
/* #endif */
}
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s) {
  return reap_aqual_malloc_impl(aq, s);
}
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t) {
  return reap_aqual_calloc_impl(aq, n, t);
}


void _free_region(struct _RegionHandle *r) {
  free_region_impl(r);
}

static struct _RegionAllocFunctions region_functions =
  {Cyc_Core_rufree_nop, Cyc_Core_rdrop_refptr_nop, region_malloc_impl, region_vmalloc_impl, region_calloc_impl};
static struct _RegionAllocFunctions reap_functions = 
  {reap_rufree_impl, reap_rdrop_refptr_impl, reap_malloc_impl, reap_vmalloc_impl, reap_calloc_impl};

/* // allocate a new page and return a region handle for a new region. */
/* struct _RegionHandle _new_region(const char *rgn_name) { */
/*   struct _RegionHandle r; */
/* #ifdef CYC_REGION_PROFILE */
/*   r.name = rgn_name; */
/* #endif */
/*   r.used_bytes = 0; */
/*   r.wasted_bytes = 0; */
/*   r.s.tag = LIFO_REGION; */
/*   r.s.next = NULL; */
/*   r.curr = 0; */
/*   r.offset = 0; */
/*   r.last_plus_one = 0; */
/*   r.released_ptrs = NULL; */
/*   r.key = NULL; */
/*   r.fcns = &region_functions; */
/*   return r; */
/* } */

/* struct _RegionHandle _new_reap(const char *rgn_name) { */
/*   struct _RegionHandle r; */
/* #ifdef CYC_REGION_PROFILE */
/*   r.name = rgn_name; */
/* #endif */
/*   r.used_bytes = 0; */
/*   r.wasted_bytes = 0; */
/*   r.s.tag = LIFO_REGION; */
/*   r.s.next = NULL; */
/*   r.curr = 0; */
/*   r.offset = 0; */
/*   r.last_plus_one = 0; */
/*   r.released_ptrs = NULL; */
/*   r.key = NULL; */
/*   r.fcns = &reap_functions; */
/*   return r; */
/* } */


struct _RegionHandle _new_region(unsigned int disable_reap, const char *rgn_name) {
  struct _RegionHandle r;
#ifdef CYC_REGION_PROFILE
  r.name = rgn_name;
#endif
  r.used_bytes = 0;
  r.wasted_bytes = 0;
  r.s.tag = LIFO_REGION;
  r.s.next = NULL;
  r.curr = 0;
  r.offset = 0;
  r.last_plus_one = 0;
  r.released_ptrs = NULL;
  r.key = NULL;
  if(disable_reap)
    r.fcns = &region_functions;
  else
    r.fcns = &reap_functions;
  return r;
}


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
	      region_get_heap_size(CYC_CORE_HEAP_REGION), 
	      region_get_free_bytes(CYC_CORE_HEAP_REGION),
	      region_get_total_bytes(CYC_CORE_HEAP_REGION),
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
/*     errprintf("getting count, refptr=%x, cnt=%x\n",ptr.base,res); */
    return res;
  }
}

// We allow pointer arithmetic on fat refcount pointers, so look
// for the count using the base pointer
int Cyc_Core_refptr_count(struct _fat_ptr ptr) {
  int *cnt = get_refcnt(ptr.base);
  if (cnt != NULL) return *cnt;
  else return 0;
}

// Need to use a fat pointer here, so that we preserve the bounds
// information when aliasing.
struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr ptr) {
  int *cnt = get_refcnt(ptr.base);
  if (cnt != NULL) *cnt = *cnt + 1;
  //   errprintf("refptr=%x, cnt=%x, updated *cnt=%d\n",ptr.base,cnt,
  // 	  cnt != NULL ? *cnt : 0);
  return ptr;
}


static void drop_refptr_base(unsigned char *ptr) {
  int *cnt = get_refcnt(ptr);
  if (cnt != NULL) {
    //     errprintf("refptr=%x, cnt=%x, *cnt=%d\n",ptr,cnt,*cnt);
    *cnt = *cnt - 1;
    if (*cnt == 0) { // no more references
      //       errprintf("freeing refptr=%x\n",ptr);
#ifdef CYC_REGION_PROFILE
      unsigned int sz = GC_size(ptr - sizeof(int));
      refcnt_freed_bytes += sz;
      if (alloc_log != NULL) {
	fprintf(alloc_log,"%u @\trefcnt\talloc\t-%d\t%d\t%d\t%d\t%x\n",
                clock(),
		sz,
		region_get_heap_size(CYC_CORE_HEAP_REGION), 
		region_get_free_bytes(CYC_CORE_HEAP_REGION),
		region_get_total_bytes(CYC_CORE_HEAP_REGION),
                (unsigned int)ptr);
      }
#endif
      GC_free(ptr - sizeof(int));
    }
  }
}
void Cyc_Core_drop_refptr(struct _fat_ptr ptr) {
  drop_refptr_base(ptr.base);
}


/////// AUTORELEASE POOLS //////////

struct _fat_ptr Cyc_Core_autorelease_handle(struct _RegionHandle *h,
					    struct _fat_ptr ptr) {
  // ptr is a fat pointer, and we assume its base is correct
  // No pool, so ptr will leak
  if (!h) return ptr;
  {struct _pool *p = h->released_ptrs;
  if (!p || p->count >= POOLSIZE) {
    p = (struct _pool *)GC_calloc(1,sizeof(struct _pool));
    p->next = h->released_ptrs;
    h->released_ptrs = p;
  }
  p->pointers[p->count++] = ptr.base;}
  return ptr;
}

struct _fat_ptr Cyc_Core_inc_refptr(struct _fat_ptr ptr) {
  return Cyc_Core_alias_refptr(ptr);
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

// set the default region page size -- returns false and has no effect 
// if s is not at least CYC_MIN_REGION_PAGE_SIZE
int Cyc_set_default_region_page_size(size_t s) {
  if (s < CYC_MIN_REGION_PAGE_SIZE) 
    return 0;
  default_region_page_size = s;
  return 1;
}

// Dynamic Regions
// Note that struct Cyc_Core_DynamicRegion is defined in cyc_include.h.

// We use this struct when returning a newly created dynamic region.
// The wrapper is needed because the Cyclone interface uses an existential.

//DYNAMIC REGIONS
struct Cyc_Core_NewDynamicRegion {
  struct Cyc_Core_DynamicRegion *key;
};

// Create a new dynamic region and return a unique pointer for the key.
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_ukey(unsigned int dis_reaps,
						    const char *file,
						    const char *func,
						    int lineno) {
  struct Cyc_Core_NewDynamicRegion res;
  res.key = _bounded_GC_malloc(sizeof(struct Cyc_Core_DynamicRegion),__FILE__,
			       __LINE__);
  if (!res.key)
    _throw_badalloc();
#ifdef CYC_REGION_PROFILE
  res.key->h = _profile_new_region(dis_reaps,"dynamic_unique",file,func,lineno);
#else
  res.key->h =_new_region(dis_reaps, "dynamic_unique");
#endif
  return res;
}

// XXX change to use refcount routines above
// Create a new dynamic region and return a reference-counted pointer 
// for the key.
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(unsigned int dis_reaps,
						     const char *file,
						     const char *func,
						     int lineno) {
  struct Cyc_Core_NewDynamicRegion res;
  int *krc = _bounded_GC_malloc(sizeof(int)+sizeof(struct Cyc_Core_DynamicRegion),
				__FILE__, __LINE__);
  //errprintf("creating rckey.  Initial address is %x\n",krc);fflush(stderr);
  if (!krc)
    _throw_badalloc();
  *krc = 1;
  res.key = (struct Cyc_Core_DynamicRegion *)(krc + 1);
  //errprintf("results key address is %x\n",res.key);fflush(stderr);
#ifdef CYC_REGION_PROFILE
  res.key->h = _profile_new_region(dis_reaps,"dynamic_refcnt",file,func,lineno);
#else
  res.key->h =  _new_region(dis_reaps, "dynamic_refcnt");
#endif
  return res;
}

// Destroy a dynamic region, given the unique key to it.
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion *k) {
#ifdef CYC_REGION_PROFILE
  _profile_free_region(&k->h,NULL,NULL,0);
#else
  _free_region(&k->h);
#endif
  GC_free(k);
}
// Drop a reference for a dynamic region, possibly freeing it.
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion *k) {
  //errprintf("freeing rckey %x\n",k);
  int *p = ((int *)k) - 1;
  //errprintf("count is address %x, value %d\n",p,*p);
  unsigned c = (*p) - 1;
  if (c >= *p) {
    errquit("internal error: free rckey bad count");
  }
  *p = c;
  if (c == 0) {
    //errprintf("count at zero, freeing region\n");
#ifdef CYC_REGION_PROFILE
    _profile_free_region(&k->h,NULL,NULL,0);
#else
    _free_region(&k->h);
#endif
    //errprintf("freeing ref-counted pointer\n");
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
  result =  _bounded_GC_malloc(n,__FILE__, __LINE__);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,0,file,func,lineno);
}

void * _profile_GC_malloc_atomic(int n, const char *file, const char *func,
                                 int lineno) {
  void * result;
  result =  _bounded_GC_malloc_atomic(n, file, lineno);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,1,file,func,lineno);
}

void * _profile_GC_calloc(unsigned n, unsigned s, const char *file, const char *func, int lineno) {
  void * result;
  result =  _bounded_GC_calloc(n,s,__FILE__, __LINE__);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,0,file,func,lineno);
}

void * _profile_GC_calloc_atomic(unsigned n, unsigned s, 
				 const char *file, const char *func, int lineno) {
  void * result;
  result =  _bounded_GC_calloc_atomic(n,s, __FILE__, __LINE__);
  if(!result)
    _throw_badalloc();
  return _do_profile(result,1,file,func,lineno);
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
    if(r->key) {
      bufsize cur, totfree, max, ng, nrel;
      bstats(r->key, &cur, &totfree, &max, &ng, &nrel);
      return totfree;
    }
    else {
/*       struct _RegionPage *p = r->curr; */
/*       //is this really what we want?  */
/*       //free bytes is just the real size of the region page  */
/*       if (p != NULL) */
/* 	return p->free_bytes; */
/*       else  */
/* 	return 0; */
      return r->last_plus_one - r->offset;
    }
  }
  else
    return GC_get_free_bytes();
}

static int region_get_total_bytes(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    if(r->key) {
      bufsize cur, totfree, max, ng, nrel;
      bstats(r->key, &cur, &totfree, &max, &ng, &nrel);
      return cur;
    }
    else {
      struct _RegionPage *p = r->curr;
      int sz = 0;
      while (p != NULL) {
      sz = sz + (p->total_bytes - p->free_bytes);
      p = p->next;
      }  
      return sz;
    }
  }
  else {
    unsigned int unique_avail_bytes =
      unique_total_bytes - unique_freed_bytes;
    unsigned int refcnt_avail_bytes =
      refcnt_total_bytes - refcnt_freed_bytes;
/*     if (r == CYC_CORE_UNIQUE_REGION) return unique_avail_bytes; */
/*     else if (r == CYC_CORE_REFCNT_REGION) return refcnt_avail_bytes; */
/*     else  */
    return GC_get_total_bytes() - unique_avail_bytes - refcnt_avail_bytes;
  }
}

struct _RegionHandle _profile_new_region(unsigned int i,
					 const char *rgn_name, 
					 const char *file,
                                         const char *func,
                                         int lineno) {
  int len;
  char *buf;
  static int cnt = 0;

  len = strlen(rgn_name)+10;
  buf = _bounded_GC_malloc_atomic(len, __FILE__, __LINE__);
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

  return _new_region(i, buf);
}

void _profile_free_region(struct _RegionHandle *r, const char *file, const char *func, int lineno) {
  // should never be heap, unique, or refcnt ...
  _free_region(r);
  if (alloc_log != NULL) {
    if (file == NULL)
      fprintf(alloc_log,"%u @\t%s\tfree\t%d\t%d\t%d\n",
              clock(), r->name,
              GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
    else
      fprintf(alloc_log,"%u %s:%s:%d\t%s\tfree\t%d\t%d\t%d\n",
              clock(),
              file,func,lineno, r->name,
              GC_get_heap_size(),GC_get_free_bytes(),GC_get_total_bytes());
  }
}

void * _profile_region_malloc(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int s,
                              const char *file, const char *func, int lineno) {
  char *addr;
  addr = _region_malloc(r,aq,s);
  _profile_check_gc();
  if (alloc_log != NULL) {
    if (r == CYC_CORE_HEAP_REGION) {
      if(aq == CYC_CORE_REFCNT_AQUAL) {
	s = GC_size(addr-sizeof(int)); // back up to before the refcnt
	fprintf(alloc_log,"%u %s:%s:%d\t%s\talloc\t%d\t%d\t%d\t%d\t%x\n",
		clock(),
		file,func,lineno, "refcnt",
		"heap", s,
		region_get_heap_size(r), 
		region_get_free_bytes(r),
		region_get_total_bytes(r),
		(unsigned int)addr);
      }
      else {
	s = GC_size(addr);
	set_finalizer((GC_PTR)addr);
      }
    }
  }
  return (void *)addr;
}

void * _profile_region_calloc(struct _RegionHandle *r, _AliasQualHandle_t aq, unsigned int t1,
                              unsigned int t2,
                              const char *file, const char *func, int lineno) {
  char *addr;
  unsigned s = t1 * t2;
  addr = _region_calloc(r,aq,t1,t2);
  _profile_check_gc();
  if (alloc_log != NULL) {
    if (r == CYC_CORE_HEAP_REGION) {
      if(aq == CYC_CORE_REFCNT_AQUAL) {
	s = GC_size(addr-sizeof(int)); // back up to before the refcnt
	fprintf(alloc_log,"%u %s:%s:%d\t%s\talloc\t%d\t%d\t%d\t%d\t%x\n",
		clock(),
		file,func,lineno,r->name, s,
		region_get_heap_size(r), 
		region_get_free_bytes(r),
		region_get_total_bytes(r),
		(unsigned int)addr);
      }
      else {
	s = GC_size(addr);
	set_finalizer((GC_PTR)addr);
      }
    }
  }
  return (void *)addr;
}

void * _profile_aqual_malloc(_AliasQualHandle_t aq, unsigned int s,
			     const char *file, const char *func, int lineno) {
  return _profile_region_malloc(CYC_CORE_HEAP_REGION, aq, s, file, func, lineno);  
}

void * _profile_aqual_calloc(_AliasQualHandle_t aq, unsigned int t1,
			     unsigned int t2,
			     const char *file, const char *func, int lineno) {
  return _profile_region_calloc(CYC_CORE_HEAP_REGION, aq, t1, t2, file, func, lineno);
}

#else //CYC_REGION_PROFILE

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
    if(r->key) {
      bufsize cur, totfree, max, ng, nrel;
      bstats(r->key, &cur, &totfree, &max, &ng, &nrel);
      return totfree;
    }
    else {
      unsigned free_bytes = r->last_plus_one - r->offset;
      return free_bytes;
    }
  }
  else
    return GC_get_free_bytes();
}

static int region_get_total_bytes(struct _RegionHandle *r) {
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST) {
    if(r->key) {
      bufsize cur, totfree, max, ng, nrel;
      bstats(r->key, &cur, &totfree, &max, &ng, &nrel);
      return cur;
    }
    else {
      unsigned used_bytes = r->used_bytes;
      unsigned wasted_bytes = r->wasted_bytes;
      unsigned free_bytes = r->last_plus_one - r->offset;
      return (used_bytes - wasted_bytes - free_bytes);
    }
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
  if (alloc_log != NULL)
    fprintf(alloc_log,"%u @\t@\tmark\t%s\n", clock(), s);
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
#if(defined(__linux__) && defined(__KERNEL__)) 
  return;
#else 
  if (!msg) return;
  if (!strncmp(msg,"Needed to allocate blacklisted block",
               strlen("Needed to allocate blacklisted block")))
    return;
  GC_default_warn_proc(msg,arg);
#endif
}

/* Use these in Cyclone programs */
void GC_blacklist_warn_set() {
  GC_set_warn_proc(GC_default_warn_proc);
}
void GC_blacklist_warn_clear() {
  GC_set_warn_proc(GC_noblacklist_warn_proc);
}

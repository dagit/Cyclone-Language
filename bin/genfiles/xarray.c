#include <setjmp.h>
/* This is a C header used by the output of the Cyclone to
   C translator.  Corresponding definitions are in file lib/runtime_*.c */
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/* Need one of these per thread (see runtime_stack.c). The runtime maintains 
   a stack that contains either _handler_cons structs or _RegionHandle structs.
   The tag is 0 for a handler_cons and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; 
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

#ifndef offsetof
/* should be size_t but int is fine */
#define offsetof(t,n) ((int)(&(((t*)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Regions */
struct _RegionPage
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions; //not used
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame { //not used
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

/* Reaps */
struct _ReapPage
#ifdef CYC_REGION_PROFILE
{ unsigned direct_flag;
  unsigned total_bytes;
  unsigned free_bytes;
  void *bget_page;
  struct _ReapPage *next;
}
#endif
; 
struct bget_region_key;
struct _ReapHandle {
  struct _RuntimeStack s;
  struct _ReapPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _pool *released_ptrs;
  struct bget_region_key *bkey;
#ifdef CYC_REGION_PROFILE
  unsigned int id;
  const char         *name;
#endif
};

// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct Cyc_Core_DynamicReap {
  struct _ReapHandle h;
};

/* Alias qualifier stuff */
typedef unsigned char _AliasQualHandle_t;

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

struct _ReapHandle _new_reap(const char*);
void* _reap_malloc(struct _ReapHandle*, _AliasQualHandle_t, unsigned);
void* _reap_calloc(struct _ReapHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void * _reap_aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _reap_aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_reap(struct _ReapHandle*);

struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);//not used
void _pop_dynregion(); //not used

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons*);
void _push_region(struct _RegionHandle*);
void _npop_handler(int);
void _pop_handler();
void _pop_region();

void _push_reap(struct _ReapHandle*);

#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
     if (!_cks_null) _throw_null(); \
     _cks_null; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char*)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr);\
  int _index = (index);\
  if (!_cks_ptr) _throw_null(); \
  _cks_ptr + (elt_sz)*_index; })
#endif
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_other_fn(t_sz,orig_x,orig_sz,orig_i,f,l)((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })

/* _zero_arr_plus_*_fn(x,sz,i,filename,lineno) adds i to zero-terminated ptr
   x that has at least sz elements */
char* _zero_arr_plus_char_fn(char*,unsigned,int,const char*,unsigned);
void* _zero_arr_plus_other_fn(unsigned,void*,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
unsigned _get_zero_arr_size_char(const char*,unsigned);
unsigned _get_zero_arr_size_other(unsigned,const void*,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
// note: must cast result in toc.cyc
void* _zero_arr_inplace_plus_other_fn(unsigned,void**,int,const char*,unsigned);
void* _zero_arr_inplace_plus_post_other_fn(unsigned,void**,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char**)(x),(i),__FILE__,__LINE__)
#define _zero_arr_plus_other(t,x,s,i) \
  (_zero_arr_plus_other_fn(t,x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_other(t,x,i) \
  _zero_arr_inplace_plus_other_fn(t,(void**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_other(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void**)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char*)0) \
    _throw_arraybounds(); \
  _curr; })
#define _check_fat_at_base(arr) ({ \
  struct _fat_ptr _arr = (arr); \
  if (_arr.base != _arr.curr) _throw_arraybounds(); \
  _arr; })
#endif

#define _tag_fat(tcurr,elt_sz,num_elts) ({ \
  struct _fat_ptr _ans; \
  unsigned _num_elts = (num_elts);\
  _ans.base = _ans.curr = (void*)(tcurr); \
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _ans.last_plus_one = _ans.base ? (_ans.base + (elt_sz) * _num_elts) : 0; \
  _ans; })

#define _get_fat_size(arr,elt_sz) \
  ({struct _fat_ptr _arr = (arr); \
    unsigned char *_arr_curr=_arr.curr; \
    unsigned char *_arr_last=_arr.last_plus_one; \
    (_arr_curr < _arr.base || _arr_curr >= _arr_last) ? 0 : \
    ((_arr_last - _arr_curr) / (elt_sz));})

#define _fat_ptr_plus(arr,elt_sz,change) ({ \
  struct _fat_ptr _ans = (arr); \
  int _change = (change);\
  _ans.curr += (elt_sz) * _change;\
  _ans; })
#define _fat_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += (elt_sz) * (change);\
  *_arr_ptr; })
#define _fat_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  struct _fat_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += (elt_sz) * (change);\
  _ans; })

//Not a macro since initialization order matters. Defined in runtime_zeroterm.c.
struct _fat_ptr _fat_ptr_decrease_size(struct _fat_ptr,unsigned sz,unsigned numelts);

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void*);
#endif
// bound the allocation size to be < MAX_ALLOC_SIZE. See macros below for usage.
#define MAX_MALLOC_SIZE (1 << 28)
void* _bounded_GC_malloc(int,const char*,int);
void* _bounded_GC_malloc_atomic(int,const char*,int);
void* _bounded_GC_calloc(unsigned,unsigned,const char*,int);
void* _bounded_GC_calloc_atomic(unsigned,unsigned,const char*,int);
/* these macros are overridden below ifdef CYC_REGION_PROFILE */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)
#endif

static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long) x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
  if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
#ifdef CYC_NOALIGN
    unsigned s =  orig_s;
#else
    unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
#endif
    char *result; 
    result = r->offset; 
    if (s <= (r->last_plus_one - result)) {
      r->offset = result + s; 
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
      return result;
    }
  } 
  return _region_malloc(r,aq,orig_s); 
}

//doesn't make sense to fast_reap_malloc
// static inline void*_fast_reap_malloc(struct _ReapHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
//   if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
// #ifdef CYC_NOALIGN
//     unsigned s =  orig_s;
// #else
//     unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
// #endif
//     char *result; 
//     result = r->offset; 
//     if (s <= (r->last_plus_one - result)) {
//       r->offset = result + s; 
// #ifdef CYC_REGION_PROFILE
//     r->curr->free_bytes = r->curr->free_bytes - s;
//     rgn_total_bytes += s;
// #endif
//       return result;
//     }
//   } 
//   return _region_malloc(r,aq,orig_s); 
// }

//migration to reaps -- Remove this block to revert to regions 
//... but the rufree etc. will not work
#ifndef RUNTIME_CYC
#define _new_region(n) _new_reap(n)
#define _free_region(r) _free_reap(r)
#define _region_malloc(rh,aq,n) _reap_malloc(rh,aq,n)
#define _region_calloc(rh,aq,n,t) _reap_calloc(rh,aq,n,t)
#define _push_region(rh) _push_reap(rh)
#define _aqual_malloc(aq,s) _reap_aqual_malloc(aq, s)
#define _aqual_calloc(aq,s,i) _reap_aqual_calloc(aq, s, i)
#define _RegionHandle _ReapHandle
#define Cyc_Core_NewDynamicRegion Cyc_Core_NewDynamicReap
#define Cyc_Core__new_ukey(f,fn,l) Cyc_Core__reap_new_ukey(f, fn, l)
#define Cyc_Core__new_rckey(f,fn,l) Cyc_Core__reap_new_rckey(f, fn, l)
#define Cyc_Core_free_ukey(k) Cyc_Core_reap_free_ukey(k)
#define Cyc_Core_free_rckey(k) Cyc_Core_reap_free_rckey(k)
#define Cyc_Core_open_region Cyc_Core_reap_open_region
#endif 

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,unsigned,const char *,const char*,int);
void * _profile_aqual_malloc(_AliasQualHandle_t aq, unsigned int s,const char *file, const char *func, int lineno);
void * _profile_aqual_calloc(_AliasQualHandle_t aq, unsigned int t1,unsigned int t2,const char *file, const char *func, int lineno);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_region_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_region_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 177
extern unsigned Cyc_Core_unique_qual;
# 190 "core.h"
void Cyc_Core_ufree(void*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 25 "xarray.cyc"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*xarr){
return xarr->num_elmts;}
# 29
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0 || i >= xarr->num_elmts)
_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=_tag_fat("Xarray::get: bad index",sizeof(char),23U);_Tmp0;}));
return*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i));}
# 35
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*xarr,int i,void*a){
if(i < 0 || i >= xarr->num_elmts)
_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=_tag_fat("Xarray::set: bad index",sizeof(char),23U);_Tmp0;}));
*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i))=a;}
# 41
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*r){
struct _fat_ptr x=_tag_fat(0,0,0);
return({struct Cyc_Xarray_Xarray*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Xarray_Xarray));_Tmp0->elmts=x,_Tmp0->num_elmts=0;_Tmp0;});}
# 45
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty (void){return Cyc_Xarray_rcreate_empty(Cyc_Core_heap_region);}
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*r,int len,void*a){
if(len < 0)
_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=_tag_fat("xarrays must have a non-negative size buffer",sizeof(char),45U);_Tmp0;}));
return({struct Cyc_Xarray_Xarray*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Xarray_Xarray));_Tmp0->elmts=({unsigned _Tmp1=(unsigned)len;_tag_fat(({void**_Tmp2=({unsigned _Tmp3=Cyc_Core_unique_qual;_aqual_malloc(_Tmp3,_check_times(_Tmp1,sizeof(void*)));});({{unsigned _Tmp3=(unsigned)len;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]=a;}}0;});_Tmp2;}),sizeof(void*),_Tmp1);}),_Tmp0->num_elmts=0;_Tmp0;});}
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int len,void*a){
return Cyc_Xarray_rcreate(Cyc_Core_heap_region,len,a);}
# 58
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*r,int len,void*a){
if(len < 1)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=_tag_fat("singleton xarray must have size >=1",sizeof(char),36U);_Tmp0;}));{
struct Cyc_Xarray_Xarray*x=Cyc_Xarray_rcreate(r,len,a);
x->num_elmts=1;
return x;}}
# 65
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int len,void*a){
return Cyc_Xarray_rsingleton(Cyc_Core_heap_region,len,a);}
# 69
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,void*a){
if((unsigned)xarr->num_elmts==_get_fat_size(xarr->elmts,sizeof(void*))){
if(xarr->num_elmts==0)
xarr->elmts=_tag_fat(({unsigned _Tmp0=10U;({void**_Tmp1=({unsigned _Tmp2=Cyc_Core_unique_qual;_aqual_malloc(_Tmp2,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=10U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=a;}}0;});_Tmp1;});}),sizeof(void*),10U);else{
# 74
struct _fat_ptr newarr=({unsigned _Tmp0=(unsigned)(xarr->num_elmts * 2);_tag_fat(({void**_Tmp1=({unsigned _Tmp2=Cyc_Core_unique_qual;_aqual_malloc(_Tmp2,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=(unsigned)(xarr->num_elmts * 2);unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),0));}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
{int i=1;for(0;i < xarr->num_elmts;++ i){
({void*_Tmp0=*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i));*((void**)_check_fat_subscript(newarr,sizeof(void*),i))=_Tmp0;});}}
({struct _fat_ptr _Tmp0=xarr->elmts;struct _fat_ptr _Tmp1=newarr;xarr->elmts=_Tmp1;newarr=_Tmp0;});
({(void(*)(void**))Cyc_Core_ufree;})((void**)_untag_fat_ptr(newarr,sizeof(void*),1U));}}
# 81
*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),xarr->num_elmts ++))=a;}
# 84
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*xarr,void*a){
Cyc_Xarray_add(xarr,a);
return xarr->num_elmts - 1;}
# 89
struct _fat_ptr Cyc_Xarray_rto_array(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts==0)
return _tag_fat(({unsigned _Tmp0=0;({void**_Tmp1=({struct _RegionHandle*_Tmp2=r;_region_malloc(_Tmp2,0U,_check_times(_Tmp0,sizeof(void*)));});*_Tmp1=0;_Tmp1;});}),sizeof(void*),0U);{
struct _fat_ptr ans=({unsigned _Tmp0=(unsigned)xarr->num_elmts;_tag_fat(({void**_Tmp1=({struct _RegionHandle*_Tmp2=r;_region_malloc(_Tmp2,0U,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=(unsigned)xarr->num_elmts;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),(int)i));}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
return ans;}}
# 96
struct _fat_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rto_array(Cyc_Core_heap_region,xarr);}
# 100
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*r,struct _fat_ptr arr){
if(_get_fat_size(arr,sizeof(void*))==0U)
return Cyc_Xarray_rcreate_empty(r);{
struct Cyc_Xarray_Xarray*ans;ans=_region_malloc(r,0U,sizeof(struct Cyc_Xarray_Xarray)),ans->elmts=({unsigned _Tmp0=
_get_fat_size(arr,sizeof(void*));_tag_fat(({void**_Tmp1=({unsigned _Tmp2=Cyc_Core_unique_qual;_aqual_malloc(_Tmp2,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=_get_fat_size(arr,sizeof(void*));unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=((void**)arr.curr)[(int)i];}}0;});_Tmp1;}),sizeof(void*),_Tmp0);}),ans->num_elmts=(int)
_get_fat_size(arr,sizeof(void*));
return ans;}}
# 109
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _fat_ptr arr){
return Cyc_Xarray_rfrom_array(Cyc_Core_heap_region,arr);}
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
int newsz=(int)(_get_fat_size(xarr1->elmts,sizeof(void*))+ _get_fat_size(xarr2->elmts,sizeof(void*)));
if(newsz==0)
return Cyc_Xarray_rcreate_empty(r);{
void*init=_get_fat_size(xarr1->elmts,sizeof(void*))==0U?*((void**)_check_fat_subscript(xarr2->elmts,sizeof(void*),0)):*((void**)_check_fat_subscript(xarr1->elmts,sizeof(void*),0));
struct Cyc_Xarray_Xarray*ans;ans=_region_malloc(r,0U,sizeof(struct Cyc_Xarray_Xarray)),ans->elmts=({unsigned _Tmp0=(unsigned)newsz;_tag_fat(({void**_Tmp1=({unsigned _Tmp2=Cyc_Core_unique_qual;_aqual_malloc(_Tmp2,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=(unsigned)newsz;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=init;}}0;});_Tmp1;}),sizeof(void*),_Tmp0);}),ans->num_elmts=0;
# 121
{int i=0;for(0;i < xarr1->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_fat_subscript(xarr1->elmts,sizeof(void*),i)));}}
{int i=0;for(0;i < xarr2->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_fat_subscript(xarr2->elmts,sizeof(void*),i)));}}
return ans;}}
# 128
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
return Cyc_Xarray_rappend(Cyc_Core_heap_region,xarr1,xarr2);}
# 132
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i)));}}
# 137
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i)));}}
# 142
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i)));}}
# 147
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i)));}}
# 152
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts==0)return Cyc_Xarray_rcreate_empty(r);{
# 155
struct Cyc_Xarray_Xarray*ans;
ans=_region_malloc(r,0U,sizeof(struct Cyc_Xarray_Xarray)),ans->elmts=({unsigned _Tmp0=_get_fat_size(xarr->elmts,sizeof(void*));_tag_fat(({void**_Tmp1=({unsigned _Tmp2=Cyc_Core_unique_qual;_aqual_malloc(_Tmp2,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=_get_fat_size(xarr->elmts,sizeof(void*));unsigned i;for(i=0;i < _Tmp2;++ i){({void*_Tmp3=f(*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),0)));_Tmp1[i]=_Tmp3;});}}0;});_Tmp1;}),sizeof(void*),_Tmp0);}),ans->num_elmts=xarr->num_elmts;
# 158
{int i=1;for(0;i < xarr->num_elmts;++ i){
({void*_Tmp0=f(*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i)));*((void**)_check_fat_subscript(ans->elmts,sizeof(void*),i))=_Tmp0;});}}
return ans;}}
# 163
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap(Cyc_Core_heap_region,f,xarr);}
# 167
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts==0)return Cyc_Xarray_rcreate_empty(r);{
# 170
struct Cyc_Xarray_Xarray*ans;
ans=_region_malloc(r,0U,sizeof(struct Cyc_Xarray_Xarray)),ans->elmts=({unsigned _Tmp0=_get_fat_size(xarr->elmts,sizeof(void*));_tag_fat(({void**_Tmp1=({unsigned _Tmp2=Cyc_Core_unique_qual;_aqual_malloc(_Tmp2,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=_get_fat_size(xarr->elmts,sizeof(void*));unsigned i;for(i=0;i < _Tmp2;++ i){({void*_Tmp3=f(env,*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),0)));_Tmp1[i]=_Tmp3;});}}0;});_Tmp1;}),sizeof(void*),_Tmp0);}),ans->num_elmts=xarr->num_elmts;
# 173
{int i=1;for(0;i < xarr->num_elmts;++ i){
({void*_Tmp0=f(env,*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),i)));*((void**)_check_fat_subscript(ans->elmts,sizeof(void*),i))=_Tmp0;});}}
return ans;}}
# 178
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap_c(Cyc_Core_heap_region,f,env,xarr);}
# 182
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr){
struct _fat_ptr newarr=_tag_fat(0,0,0);
({struct _fat_ptr _Tmp0=newarr;struct _fat_ptr _Tmp1=xarr->elmts;newarr=_Tmp1;xarr->elmts=_Tmp0;});
xarr->num_elmts=0;
({(void(*)(void**))Cyc_Core_ufree;})((void**)_untag_fat_ptr(newarr,sizeof(void*),1U));}
# 189
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num){
if(({int _Tmp0=num;_Tmp0 > Cyc_Xarray_length(xarr);}))
_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=_tag_fat("number deleted is greater than length of xarray",sizeof(char),48U);_Tmp0;}));
xarr->num_elmts -=num;}
# 195
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0 || i > xarr->num_elmts - 1)
_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=_tag_fat("xarray index out of bounds",sizeof(char),27U);_Tmp0;}));
{int j=i;for(0;j < xarr->num_elmts - 1;++ j){
({void*_Tmp0=*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),j + 1));*((void**)_check_fat_subscript(xarr->elmts,sizeof(void*),j))=_Tmp0;});}}
-- xarr->num_elmts;}

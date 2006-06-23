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
{ 
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];
};

struct _pool;
struct bget_region_key;
struct _RegionAllocFunctions;

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _RegionAllocFunctions *fcns;
  char               *offset;
  char               *last_plus_one;
  struct _pool *released_ptrs;
  struct bget_region_key *key;
#ifdef CYC_REGION_PROFILE
  const char *name;
#endif
  unsigned used_bytes;
  unsigned wasted_bytes;
};


// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

/* Alias qualifier stuff */
typedef unsigned int _AliasQualHandle_t; // must match aqualt_type() in toc.cyc

struct _RegionHandle _new_region(unsigned int, const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

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


#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_assert_fn(const char *,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw_assert() (_throw_assert_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
struct Cyc_Assert_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];
extern char Cyc_Assert[];

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
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ({ \
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

#define _CYC_MAX_REGION_CONST 0
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

//doesn't make sense to fast malloc with reaps
#ifndef DISABLE_REAPS
#define _fast_region_malloc _region_malloc
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
struct _RegionHandle _profile_new_region(unsigned int i, const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(i,n) _profile_new_region(i,n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_aqual_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_aqual_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif //CYC_REGION_PROFILE
#endif //_CYC_INCLUDE_H
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 180
extern unsigned Cyc_Core_unique_qual;
# 193 "core.h"
void Cyc_Core_ufree(void*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 25 "xarray.cyc"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*xarr){struct Cyc_Xarray_Xarray*_T0;int _T1;_T0=xarr;_T1=_T0->num_elmts;
return _T1;}
# 29
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*xarr,int i){int _T0;struct Cyc_Xarray_Xarray*_T1;int _T2;struct Cyc_Core_Invalid_argument_exn_struct*_T3;void*_T4;struct Cyc_Xarray_Xarray*_T5;struct _fat_ptr _T6;int _T7;unsigned char*_T8;void**_T9;void*_TA;
if(i < 0)goto _TL2;else{goto _TL3;}_TL3: _T0=i;_T1=xarr;_T2=_T1->num_elmts;if(_T0 >= _T2)goto _TL2;else{goto _TL0;}
_TL2:{struct Cyc_Core_Invalid_argument_exn_struct*_TB=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TB->tag=Cyc_Core_Invalid_argument;_TB->f1=_tag_fat("Xarray::get: bad index",sizeof(char),23U);_T3=(struct Cyc_Core_Invalid_argument_exn_struct*)_TB;}_T4=(void*)_T3;_throw(_T4);goto _TL1;_TL0: _TL1: _T5=xarr;_T6=_T5->elmts;_T7=i;_T8=_check_fat_subscript(_T6,sizeof(void*),_T7);_T9=(void**)_T8;_TA=*_T9;
return _TA;}
# 35
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*xarr,int i,void*a){int _T0;struct Cyc_Xarray_Xarray*_T1;int _T2;struct Cyc_Core_Invalid_argument_exn_struct*_T3;void*_T4;struct Cyc_Xarray_Xarray*_T5;struct _fat_ptr _T6;int _T7;unsigned char*_T8;void**_T9;
if(i < 0)goto _TL6;else{goto _TL7;}_TL7: _T0=i;_T1=xarr;_T2=_T1->num_elmts;if(_T0 >= _T2)goto _TL6;else{goto _TL4;}
_TL6:{struct Cyc_Core_Invalid_argument_exn_struct*_TA=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TA->tag=Cyc_Core_Invalid_argument;_TA->f1=_tag_fat("Xarray::set: bad index",sizeof(char),23U);_T3=(struct Cyc_Core_Invalid_argument_exn_struct*)_TA;}_T4=(void*)_T3;_throw(_T4);goto _TL5;_TL4: _TL5: _T5=xarr;_T6=_T5->elmts;_T7=i;_T8=_check_fat_subscript(_T6,sizeof(void*),_T7);_T9=(void**)_T8;
*_T9=a;}
# 41
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*r){struct Cyc_Xarray_Xarray*_T0;struct _RegionHandle*_T1;
struct _fat_ptr x=_tag_fat(0,0,0);_T1=r;{struct Cyc_Xarray_Xarray*_T2=_region_malloc(_T1,0U,sizeof(struct Cyc_Xarray_Xarray));
_T2->elmts=x;_T2->num_elmts=0;_T0=(struct Cyc_Xarray_Xarray*)_T2;}return _T0;}
# 45
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty (void){struct Cyc_Xarray_Xarray*_T0;_T0=Cyc_Xarray_rcreate_empty(Cyc_Core_heap_region);return _T0;}
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*r,int len,void*a){struct Cyc_Core_Invalid_argument_exn_struct*_T0;void*_T1;struct Cyc_Xarray_Xarray*_T2;struct _RegionHandle*_T3;struct _fat_ptr _T4;int _T5;void**_T6;unsigned _T7;unsigned _T8;unsigned _T9;
if(len >= 0)goto _TL8;{struct Cyc_Core_Invalid_argument_exn_struct*_TA=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TA->tag=Cyc_Core_Invalid_argument;
_TA->f1=_tag_fat("xarrays must have a non-negative size buffer",sizeof(char),45U);_T0=(struct Cyc_Core_Invalid_argument_exn_struct*)_TA;}_T1=(void*)_T0;_throw(_T1);goto _TL9;_TL8: _TL9: _T3=r;{struct Cyc_Xarray_Xarray*_TA=_region_malloc(_T3,0U,sizeof(struct Cyc_Xarray_Xarray));_T5=len;{unsigned _TB=(unsigned)_T5;_T7=Cyc_Core_unique_qual;_T8=_check_times(_TB,sizeof(void*));{void**_TC=_aqual_malloc(_T7,_T8);{unsigned _TD=_TB;unsigned i;i=0;_TLD: if(i < _TD)goto _TLB;else{goto _TLC;}_TLB: _T9=i;
_TC[_T9]=a;i=i + 1;goto _TLD;_TLC:;}_T6=(void**)_TC;}_T4=_tag_fat(_T6,sizeof(void*),_TB);}_TA->elmts=_T4;_TA->num_elmts=0;_T2=(struct Cyc_Xarray_Xarray*)_TA;}return _T2;}
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int len,void*a){struct Cyc_Xarray_Xarray*_T0;_T0=
Cyc_Xarray_rcreate(Cyc_Core_heap_region,len,a);return _T0;}
# 58
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*r,int len,void*a){struct Cyc_Core_Invalid_argument_exn_struct*_T0;void*_T1;struct Cyc_Xarray_Xarray*_T2;struct Cyc_Xarray_Xarray*_T3;
if(len >= 1)goto _TLE;{struct Cyc_Core_Invalid_argument_exn_struct*_T4=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T4->tag=Cyc_Core_Invalid_argument;_T4->f1=_tag_fat("singleton xarray must have size >=1",sizeof(char),36U);_T0=(struct Cyc_Core_Invalid_argument_exn_struct*)_T4;}_T1=(void*)_T0;_throw(_T1);goto _TLF;_TLE: _TLF: {
struct Cyc_Xarray_Xarray*x=Cyc_Xarray_rcreate(r,len,a);_T2=x;
_T2->num_elmts=1;_T3=x;
return _T3;}}
# 65
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int len,void*a){struct Cyc_Xarray_Xarray*_T0;_T0=
Cyc_Xarray_rsingleton(Cyc_Core_heap_region,len,a);return _T0;}
# 69
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,void*a){struct Cyc_Xarray_Xarray*_T0;int _T1;unsigned _T2;struct Cyc_Xarray_Xarray*_T3;struct _fat_ptr _T4;unsigned _T5;struct Cyc_Xarray_Xarray*_T6;int _T7;struct Cyc_Xarray_Xarray*_T8;void**_T9;void**_TA;unsigned _TB;unsigned _TC;unsigned _TD;struct _fat_ptr _TE;struct Cyc_Xarray_Xarray*_TF;int _T10;int _T11;void**_T12;unsigned _T13;unsigned _T14;unsigned _T15;struct Cyc_Xarray_Xarray*_T16;struct _fat_ptr _T17;unsigned char*_T18;void**_T19;int _T1A;struct Cyc_Xarray_Xarray*_T1B;int _T1C;struct _fat_ptr _T1D;int _T1E;unsigned char*_T1F;void**_T20;struct Cyc_Xarray_Xarray*_T21;struct _fat_ptr _T22;int _T23;unsigned char*_T24;void**_T25;struct Cyc_Xarray_Xarray*_T26;struct Cyc_Xarray_Xarray*_T27;void(*_T28)(void**);void(*_T29)(void*);struct _fat_ptr _T2A;unsigned char*_T2B;void**_T2C;struct Cyc_Xarray_Xarray*_T2D;struct _fat_ptr _T2E;struct Cyc_Xarray_Xarray*_T2F;int _T30;int _T31;unsigned char*_T32;void**_T33;_T0=xarr;_T1=_T0->num_elmts;_T2=(unsigned)_T1;_T3=xarr;_T4=_T3->elmts;_T5=
_get_fat_size(_T4,sizeof(void*));if(_T2!=_T5)goto _TL10;_T6=xarr;_T7=_T6->num_elmts;
if(_T7!=0)goto _TL12;_T8=xarr;{unsigned _T34=10U;_TB=Cyc_Core_unique_qual;_TC=_check_times(_T34,sizeof(void*));{void**_T35=_aqual_malloc(_TB,_TC);{unsigned _T36=_T34;unsigned i;i=0;_TL17: if(i < _T36)goto _TL15;else{goto _TL16;}_TL15: _TD=i;
_T35[_TD]=a;i=i + 1;goto _TL17;_TL16:;}_TA=(void**)_T35;}_T9=_TA;}_T8->elmts=_tag_fat(_T9,sizeof(void*),10U);goto _TL13;
# 74
_TL12: _TF=xarr;_T10=_TF->num_elmts;_T11=_T10 * 2;{unsigned _T34=(unsigned)_T11;_T13=Cyc_Core_unique_qual;_T14=_check_times(_T34,sizeof(void*));{void**_T35=_aqual_malloc(_T13,_T14);{unsigned _T36=_T34;unsigned i;i=0;_TL1B: if(i < _T36)goto _TL19;else{goto _TL1A;}_TL19: _T15=i;_T16=xarr;_T17=_T16->elmts;_T18=_T17.curr;_T19=(void**)_T18;_T35[_T15]=_T19[0];i=i + 1;goto _TL1B;_TL1A:;}_T12=(void**)_T35;}_TE=_tag_fat(_T12,sizeof(void*),_T34);}{struct _fat_ptr newarr=_TE;{
int i=1;_TL1F: _T1A=i;_T1B=xarr;_T1C=_T1B->num_elmts;if(_T1A < _T1C)goto _TL1D;else{goto _TL1E;}
_TL1D: _T1D=newarr;_T1E=i;_T1F=_check_fat_subscript(_T1D,sizeof(void*),_T1E);_T20=(void**)_T1F;_T21=xarr;_T22=_T21->elmts;_T23=i;_T24=_check_fat_subscript(_T22,sizeof(void*),_T23);_T25=(void**)_T24;*_T20=*_T25;
# 75
i=i + 1;goto _TL1F;_TL1E:;}_T26=xarr;{
# 77
struct _fat_ptr _T34=_T26->elmts;struct _fat_ptr _T35=newarr;_T27=xarr;_T27->elmts=_T35;newarr=_T34;}_T29=Cyc_Core_ufree;{
void(*_T34)(void**)=(void(*)(void**))_T29;_T28=_T34;}_T2A=newarr;_T2B=_untag_fat_ptr_check_bound(_T2A,sizeof(void*),1U);_T2C=(void**)_T2B;_T28(_T2C);}_TL13: goto _TL11;_TL10: _TL11: _T2D=xarr;_T2E=_T2D->elmts;_T2F=xarr;_T30=_T2F->num_elmts;
# 81
_T2F->num_elmts=_T30 + 1;_T31=_T30;_T32=_check_fat_subscript(_T2E,sizeof(void*),_T31);_T33=(void**)_T32;*_T33=a;}
# 84
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*xarr,void*a){struct Cyc_Xarray_Xarray*_T0;int _T1;int _T2;
Cyc_Xarray_add(xarr,a);_T0=xarr;_T1=_T0->num_elmts;_T2=_T1 - 1;
return _T2;}
# 89
struct _fat_ptr Cyc_Xarray_rto_array(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr){struct Cyc_Xarray_Xarray*_T0;int _T1;void**_T2;void**_T3;struct _RegionHandle*_T4;unsigned _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_Xarray_Xarray*_T8;int _T9;void**_TA;struct _RegionHandle*_TB;unsigned _TC;unsigned _TD;struct Cyc_Xarray_Xarray*_TE;struct _fat_ptr _TF;unsigned _T10;int _T11;unsigned char*_T12;void**_T13;struct _fat_ptr _T14;_T0=xarr;_T1=_T0->num_elmts;
if(_T1!=0)goto _TL20;{unsigned _T15=0;_T4=r;_T5=_check_times(_T15,sizeof(void*));{void**_T16=_region_malloc(_T4,0U,_T5);*_T16=0;_T3=(void**)_T16;}_T2=_T3;}_T6=
_tag_fat(_T2,sizeof(void*),0U);return _T6;_TL20: _T8=xarr;_T9=_T8->num_elmts;{unsigned _T15=(unsigned)_T9;_TB=r;_TC=_check_times(_T15,sizeof(void*));{void**_T16=_region_malloc(_TB,0U,_TC);{unsigned _T17=_T15;unsigned i;i=0;_TL25: if(i < _T17)goto _TL23;else{goto _TL24;}_TL23: _TD=i;_TE=xarr;_TF=_TE->elmts;_T10=i;_T11=(int)_T10;_T12=_check_fat_subscript(_TF,sizeof(void*),_T11);_T13=(void**)_T12;
_T16[_TD]=*_T13;i=i + 1;goto _TL25;_TL24:;}_TA=(void**)_T16;}_T7=_tag_fat(_TA,sizeof(void*),_T15);}{struct _fat_ptr ans=_T7;_T14=ans;
return _T14;}}
# 96
struct _fat_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*xarr){struct _fat_ptr _T0;_T0=
Cyc_Xarray_rto_array(Cyc_Core_heap_region,xarr);return _T0;}
# 100
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*r,struct _fat_ptr arr){struct _fat_ptr _T0;unsigned _T1;struct Cyc_Xarray_Xarray*_T2;struct _RegionHandle*_T3;struct Cyc_Xarray_Xarray*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;void**_T7;unsigned _T8;unsigned _T9;unsigned _TA;struct _fat_ptr _TB;unsigned char*_TC;void**_TD;unsigned _TE;int _TF;struct Cyc_Xarray_Xarray*_T10;struct _fat_ptr _T11;unsigned _T12;struct Cyc_Xarray_Xarray*_T13;_T0=arr;_T1=
_get_fat_size(_T0,sizeof(void*));if(_T1!=0U)goto _TL26;_T2=
Cyc_Xarray_rcreate_empty(r);return _T2;_TL26: {
struct Cyc_Xarray_Xarray*ans;_T3=r;ans=_region_malloc(_T3,0U,sizeof(struct Cyc_Xarray_Xarray));_T4=ans;_T6=arr;{unsigned _T14=
_get_fat_size(_T6,sizeof(void*));_T8=Cyc_Core_unique_qual;_T9=_check_times(_T14,sizeof(void*));{void**_T15=_aqual_malloc(_T8,_T9);{unsigned _T16=_T14;unsigned i;i=0;_TL2B: if(i < _T16)goto _TL29;else{goto _TL2A;}_TL29: _TA=i;_TB=arr;_TC=_TB.curr;_TD=(void**)_TC;_TE=i;_TF=(int)_TE;_T15[_TA]=_TD[_TF];i=i + 1;goto _TL2B;_TL2A:;}_T7=(void**)_T15;}_T5=_tag_fat(_T7,sizeof(void*),_T14);}_T4->elmts=_T5;_T10=ans;_T11=arr;_T12=
_get_fat_size(_T11,sizeof(void*));_T10->num_elmts=(int)_T12;_T13=ans;
return _T13;}}
# 109
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _fat_ptr arr){struct Cyc_Xarray_Xarray*_T0;_T0=
Cyc_Xarray_rfrom_array(Cyc_Core_heap_region,arr);return _T0;}
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){struct Cyc_Xarray_Xarray*_T0;struct _fat_ptr _T1;unsigned _T2;struct Cyc_Xarray_Xarray*_T3;struct _fat_ptr _T4;unsigned _T5;unsigned _T6;struct Cyc_Xarray_Xarray*_T7;void*_T8;struct Cyc_Xarray_Xarray*_T9;struct _fat_ptr _TA;unsigned _TB;struct Cyc_Xarray_Xarray*_TC;struct _fat_ptr _TD;unsigned char*_TE;void**_TF;struct Cyc_Xarray_Xarray*_T10;struct _fat_ptr _T11;unsigned char*_T12;void**_T13;struct _RegionHandle*_T14;struct Cyc_Xarray_Xarray*_T15;struct _fat_ptr _T16;int _T17;void**_T18;unsigned _T19;unsigned _T1A;unsigned _T1B;struct Cyc_Xarray_Xarray*_T1C;int _T1D;struct Cyc_Xarray_Xarray*_T1E;int _T1F;struct Cyc_Xarray_Xarray*_T20;struct Cyc_Xarray_Xarray*_T21;struct _fat_ptr _T22;int _T23;unsigned char*_T24;void**_T25;void*_T26;int _T27;struct Cyc_Xarray_Xarray*_T28;int _T29;struct Cyc_Xarray_Xarray*_T2A;struct Cyc_Xarray_Xarray*_T2B;struct _fat_ptr _T2C;int _T2D;unsigned char*_T2E;void**_T2F;void*_T30;struct Cyc_Xarray_Xarray*_T31;_T0=xarr1;_T1=_T0->elmts;_T2=
_get_fat_size(_T1,sizeof(void*));_T3=xarr2;_T4=_T3->elmts;_T5=_get_fat_size(_T4,sizeof(void*));_T6=_T2 + _T5;{int newsz=(int)_T6;
if(newsz!=0)goto _TL2C;_T7=
Cyc_Xarray_rcreate_empty(r);return _T7;_TL2C: _T9=xarr1;_TA=_T9->elmts;_TB=
_get_fat_size(_TA,sizeof(void*));if(_TB!=0U)goto _TL2E;_TC=xarr2;_TD=_TC->elmts;_TE=_TD.curr;_TF=(void**)_TE;_T8=_TF[0];goto _TL2F;_TL2E: _T10=xarr1;_T11=_T10->elmts;_T12=_T11.curr;_T13=(void**)_T12;_T8=_T13[0];_TL2F: {void*init=_T8;
struct Cyc_Xarray_Xarray*ans;_T14=r;ans=_region_malloc(_T14,0U,sizeof(struct Cyc_Xarray_Xarray));_T15=ans;_T17=newsz;{unsigned _T32=(unsigned)_T17;_T19=Cyc_Core_unique_qual;_T1A=_check_times(_T32,sizeof(void*));{void**_T33=_aqual_malloc(_T19,_T1A);{unsigned _T34=_T32;unsigned i;i=0;_TL33: if(i < _T34)goto _TL31;else{goto _TL32;}_TL31: _T1B=i;_T33[_T1B]=init;i=i + 1;goto _TL33;_TL32:;}_T18=(void**)_T33;}_T16=_tag_fat(_T18,sizeof(void*),_T32);}_T15->elmts=_T16;_T1C=ans;
_T1C->num_elmts=0;{
int i=0;_TL37: _T1D=i;_T1E=xarr1;_T1F=_T1E->num_elmts;if(_T1D < _T1F)goto _TL35;else{goto _TL36;}
_TL35: _T20=ans;_T21=xarr1;_T22=_T21->elmts;_T23=i;_T24=_check_fat_subscript(_T22,sizeof(void*),_T23);_T25=(void**)_T24;_T26=*_T25;Cyc_Xarray_add(_T20,_T26);
# 121
i=i + 1;goto _TL37;_TL36:;}{
# 123
int i=0;_TL3B: _T27=i;_T28=xarr2;_T29=_T28->num_elmts;if(_T27 < _T29)goto _TL39;else{goto _TL3A;}
_TL39: _T2A=ans;_T2B=xarr2;_T2C=_T2B->elmts;_T2D=i;_T2E=_check_fat_subscript(_T2C,sizeof(void*),_T2D);_T2F=(void**)_T2E;_T30=*_T2F;Cyc_Xarray_add(_T2A,_T30);
# 123
i=i + 1;goto _TL3B;_TL3A:;}_T31=ans;
# 125
return _T31;}}}
# 128
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){struct Cyc_Xarray_Xarray*_T0;_T0=
Cyc_Xarray_rappend(Cyc_Core_heap_region,xarr1,xarr2);return _T0;}
# 132
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){int _T0;struct Cyc_Xarray_Xarray*_T1;int _T2;struct Cyc_Xarray_Xarray*_T3;struct _fat_ptr _T4;int _T5;unsigned char*_T6;void**_T7;void*_T8;
int i=0;_TL3F: _T0=i;_T1=xarr;_T2=_T1->num_elmts;if(_T0 < _T2)goto _TL3D;else{goto _TL3E;}
_TL3D: _T3=xarr;_T4=_T3->elmts;_T5=i;_T6=_check_fat_subscript(_T4,sizeof(void*),_T5);_T7=(void**)_T6;_T8=*_T7;f(_T8);
# 133
i=i + 1;goto _TL3F;_TL3E:;}
# 137
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){int _T0;struct Cyc_Xarray_Xarray*_T1;int _T2;void*_T3;struct Cyc_Xarray_Xarray*_T4;struct _fat_ptr _T5;int _T6;unsigned char*_T7;void**_T8;void*_T9;
int i=0;_TL43: _T0=i;_T1=xarr;_T2=_T1->num_elmts;if(_T0 < _T2)goto _TL41;else{goto _TL42;}
_TL41: _T3=env;_T4=xarr;_T5=_T4->elmts;_T6=i;_T7=_check_fat_subscript(_T5,sizeof(void*),_T6);_T8=(void**)_T7;_T9=*_T8;f(_T3,_T9);
# 138
i=i + 1;goto _TL43;_TL42:;}
# 142
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*xarr){int _T0;struct Cyc_Xarray_Xarray*_T1;int _T2;struct Cyc_Xarray_Xarray*_T3;struct _fat_ptr _T4;int _T5;unsigned char*_T6;void**_T7;void*_T8;
int i=0;_TL47: _T0=i;_T1=xarr;_T2=_T1->num_elmts;if(_T0 < _T2)goto _TL45;else{goto _TL46;}
_TL45: _T3=xarr;_T4=_T3->elmts;_T5=i;_T6=_check_fat_subscript(_T4,sizeof(void*),_T5);_T7=(void**)_T6;_T8=*_T7;f(_T8);
# 143
i=i + 1;goto _TL47;_TL46:;}
# 147
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){int _T0;struct Cyc_Xarray_Xarray*_T1;int _T2;void*_T3;struct Cyc_Xarray_Xarray*_T4;struct _fat_ptr _T5;int _T6;unsigned char*_T7;void**_T8;void*_T9;
int i=0;_TL4B: _T0=i;_T1=xarr;_T2=_T1->num_elmts;if(_T0 < _T2)goto _TL49;else{goto _TL4A;}
_TL49: _T3=env;_T4=xarr;_T5=_T4->elmts;_T6=i;_T7=_check_fat_subscript(_T5,sizeof(void*),_T6);_T8=(void**)_T7;_T9=*_T8;f(_T3,_T9);
# 148
i=i + 1;goto _TL4B;_TL4A:;}
# 152
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){struct Cyc_Xarray_Xarray*_T0;int _T1;struct Cyc_Xarray_Xarray*_T2;struct _RegionHandle*_T3;struct Cyc_Xarray_Xarray*_T4;struct _fat_ptr _T5;struct Cyc_Xarray_Xarray*_T6;struct _fat_ptr _T7;void**_T8;unsigned _T9;unsigned _TA;unsigned _TB;struct Cyc_Xarray_Xarray*_TC;struct _fat_ptr _TD;unsigned char*_TE;void**_TF;void*_T10;struct Cyc_Xarray_Xarray*_T11;struct Cyc_Xarray_Xarray*_T12;int _T13;struct Cyc_Xarray_Xarray*_T14;int _T15;struct Cyc_Xarray_Xarray*_T16;struct _fat_ptr _T17;int _T18;unsigned char*_T19;void**_T1A;struct Cyc_Xarray_Xarray*_T1B;struct _fat_ptr _T1C;int _T1D;unsigned char*_T1E;void**_T1F;void*_T20;struct Cyc_Xarray_Xarray*_T21;_T0=xarr;_T1=_T0->num_elmts;
if(_T1!=0)goto _TL4C;_T2=Cyc_Xarray_rcreate_empty(r);return _T2;_TL4C: {
# 155
struct Cyc_Xarray_Xarray*ans;_T3=r;ans=_region_malloc(_T3,0U,sizeof(struct Cyc_Xarray_Xarray));_T4=ans;_T6=xarr;_T7=_T6->elmts;{unsigned _T22=
_get_fat_size(_T7,sizeof(void*));_T9=Cyc_Core_unique_qual;_TA=_check_times(_T22,sizeof(void*));{void**_T23=_aqual_malloc(_T9,_TA);{unsigned _T24=_T22;unsigned i;i=0;_TL51: if(i < _T24)goto _TL4F;else{goto _TL50;}_TL4F: _TB=i;_TC=xarr;_TD=_TC->elmts;_TE=_TD.curr;_TF=(void**)_TE;_T10=_TF[0];_T23[_TB]=f(_T10);i=i + 1;goto _TL51;_TL50:;}_T8=(void**)_T23;}_T5=_tag_fat(_T8,sizeof(void*),_T22);}_T4->elmts=_T5;_T11=ans;_T12=xarr;
_T11->num_elmts=_T12->num_elmts;{
int i=1;_TL55: _T13=i;_T14=xarr;_T15=_T14->num_elmts;if(_T13 < _T15)goto _TL53;else{goto _TL54;}
_TL53: _T16=ans;_T17=_T16->elmts;_T18=i;_T19=_check_fat_subscript(_T17,sizeof(void*),_T18);_T1A=(void**)_T19;_T1B=xarr;_T1C=_T1B->elmts;_T1D=i;_T1E=_check_fat_subscript(_T1C,sizeof(void*),_T1D);_T1F=(void**)_T1E;_T20=*_T1F;*_T1A=f(_T20);
# 158
i=i + 1;goto _TL55;_TL54:;}_T21=ans;
# 160
return _T21;}}
# 163
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){struct _RegionHandle*_T0;void*(*_T1)(void*);struct Cyc_Xarray_Xarray*_T2;struct Cyc_Xarray_Xarray*_T3;_T0=Cyc_Core_heap_region;_T1=f;_T2=xarr;_T3=
Cyc_Xarray_rmap(_T0,_T1,_T2);return _T3;}
# 167
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){struct Cyc_Xarray_Xarray*_T0;int _T1;struct Cyc_Xarray_Xarray*_T2;struct _RegionHandle*_T3;struct Cyc_Xarray_Xarray*_T4;struct _fat_ptr _T5;struct Cyc_Xarray_Xarray*_T6;struct _fat_ptr _T7;void**_T8;unsigned _T9;unsigned _TA;unsigned _TB;void*_TC;struct Cyc_Xarray_Xarray*_TD;struct _fat_ptr _TE;unsigned char*_TF;void**_T10;void*_T11;struct Cyc_Xarray_Xarray*_T12;struct Cyc_Xarray_Xarray*_T13;int _T14;struct Cyc_Xarray_Xarray*_T15;int _T16;struct Cyc_Xarray_Xarray*_T17;struct _fat_ptr _T18;int _T19;unsigned char*_T1A;void**_T1B;void*_T1C;struct Cyc_Xarray_Xarray*_T1D;struct _fat_ptr _T1E;int _T1F;unsigned char*_T20;void**_T21;void*_T22;struct Cyc_Xarray_Xarray*_T23;_T0=xarr;_T1=_T0->num_elmts;
if(_T1!=0)goto _TL56;_T2=Cyc_Xarray_rcreate_empty(r);return _T2;_TL56: {
# 170
struct Cyc_Xarray_Xarray*ans;_T3=r;ans=_region_malloc(_T3,0U,sizeof(struct Cyc_Xarray_Xarray));_T4=ans;_T6=xarr;_T7=_T6->elmts;{unsigned _T24=
_get_fat_size(_T7,sizeof(void*));_T9=Cyc_Core_unique_qual;_TA=_check_times(_T24,sizeof(void*));{void**_T25=_aqual_malloc(_T9,_TA);{unsigned _T26=_T24;unsigned i;i=0;_TL5B: if(i < _T26)goto _TL59;else{goto _TL5A;}_TL59: _TB=i;_TC=env;_TD=xarr;_TE=_TD->elmts;_TF=_TE.curr;_T10=(void**)_TF;_T11=_T10[0];_T25[_TB]=f(_TC,_T11);i=i + 1;goto _TL5B;_TL5A:;}_T8=(void**)_T25;}_T5=_tag_fat(_T8,sizeof(void*),_T24);}_T4->elmts=_T5;_T12=ans;_T13=xarr;
_T12->num_elmts=_T13->num_elmts;{
int i=1;_TL5F: _T14=i;_T15=xarr;_T16=_T15->num_elmts;if(_T14 < _T16)goto _TL5D;else{goto _TL5E;}
_TL5D: _T17=ans;_T18=_T17->elmts;_T19=i;_T1A=_check_fat_subscript(_T18,sizeof(void*),_T19);_T1B=(void**)_T1A;_T1C=env;_T1D=xarr;_T1E=_T1D->elmts;_T1F=i;_T20=_check_fat_subscript(_T1E,sizeof(void*),_T1F);_T21=(void**)_T20;_T22=*_T21;*_T1B=f(_T1C,_T22);
# 173
i=i + 1;goto _TL5F;_TL5E:;}_T23=ans;
# 175
return _T23;}}
# 178
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){struct _RegionHandle*_T0;void*(*_T1)(void*,void*);void*_T2;struct Cyc_Xarray_Xarray*_T3;struct Cyc_Xarray_Xarray*_T4;_T0=Cyc_Core_heap_region;_T1=f;_T2=env;_T3=xarr;_T4=
Cyc_Xarray_rmap_c(_T0,_T1,_T2,_T3);return _T4;}
# 182
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr){struct Cyc_Xarray_Xarray*_T0;struct Cyc_Xarray_Xarray*_T1;struct Cyc_Xarray_Xarray*_T2;void(*_T3)(void**);void(*_T4)(void*);struct _fat_ptr _T5;unsigned char*_T6;void**_T7;
struct _fat_ptr newarr=_tag_fat(0,0,0);{
struct _fat_ptr _T8=newarr;_T0=xarr;{struct _fat_ptr _T9=_T0->elmts;newarr=_T9;_T1=xarr;_T1->elmts=_T8;}}_T2=xarr;
_T2->num_elmts=0;_T4=Cyc_Core_ufree;{
void(*_T8)(void**)=(void(*)(void**))_T4;_T3=_T8;}_T5=newarr;_T6=_untag_fat_ptr_check_bound(_T5,sizeof(void*),1U);_T7=(void**)_T6;_T3(_T7);}
# 189
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num){int _T0;int _T1;struct Cyc_Core_Invalid_argument_exn_struct*_T2;void*_T3;struct Cyc_Xarray_Xarray*_T4;int _T5;_T0=num;_T1=
Cyc_Xarray_length(xarr);if(_T0 <= _T1)goto _TL60;{struct Cyc_Core_Invalid_argument_exn_struct*_T6=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T6->tag=Cyc_Core_Invalid_argument;
_T6->f1=_tag_fat("number deleted is greater than length of xarray",sizeof(char),48U);_T2=(struct Cyc_Core_Invalid_argument_exn_struct*)_T6;}_T3=(void*)_T2;_throw(_T3);goto _TL61;_TL60: _TL61: _T4=xarr;_T5=num;
_T4->num_elmts=_T4->num_elmts - _T5;}
# 195
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i){int _T0;struct Cyc_Xarray_Xarray*_T1;int _T2;int _T3;struct Cyc_Core_Invalid_argument_exn_struct*_T4;void*_T5;int _T6;struct Cyc_Xarray_Xarray*_T7;int _T8;int _T9;struct Cyc_Xarray_Xarray*_TA;struct _fat_ptr _TB;int _TC;unsigned char*_TD;void**_TE;struct Cyc_Xarray_Xarray*_TF;struct _fat_ptr _T10;int _T11;unsigned char*_T12;void**_T13;struct Cyc_Xarray_Xarray*_T14;
if(i < 0)goto _TL64;else{goto _TL65;}_TL65: _T0=i;_T1=xarr;_T2=_T1->num_elmts;_T3=_T2 - 1;if(_T0 > _T3)goto _TL64;else{goto _TL62;}
_TL64:{struct Cyc_Core_Invalid_argument_exn_struct*_T15=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T15->tag=Cyc_Core_Invalid_argument;_T15->f1=_tag_fat("xarray index out of bounds",sizeof(char),27U);_T4=(struct Cyc_Core_Invalid_argument_exn_struct*)_T15;}_T5=(void*)_T4;_throw(_T5);goto _TL63;_TL62: _TL63:{
int j=i;_TL69: _T6=j;_T7=xarr;_T8=_T7->num_elmts;_T9=_T8 - 1;if(_T6 < _T9)goto _TL67;else{goto _TL68;}
_TL67: _TA=xarr;_TB=_TA->elmts;_TC=j;_TD=_check_fat_subscript(_TB,sizeof(void*),_TC);_TE=(void**)_TD;_TF=xarr;_T10=_TF->elmts;_T11=j + 1;_T12=_check_fat_subscript(_T10,sizeof(void*),_T11);_T13=(void**)_T12;*_TE=*_T13;
# 198
j=j + 1;goto _TL69;_TL68:;}_T14=xarr;
# 200
_T14->num_elmts=_T14->num_elmts + -1;}

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
 extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);struct Cyc___cycFILE;
# 252 "cycboot.h"
extern int Cyc_getw(struct Cyc___cycFILE*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};char Cyc_Dict_Absent[7U]="Absent";char Cyc_Dict_Present[8U]="Present";
# 27 "dict.cyc"
struct Cyc_Dict_Absent_exn_struct Cyc_Dict_Absent_val={Cyc_Dict_Absent};
struct Cyc_Dict_Present_exn_struct Cyc_Dict_Present_val={Cyc_Dict_Present};
# 30
enum Cyc_Dict_Color{Cyc_Dict_R =0U,Cyc_Dict_B =1U};struct _tuple0{void*f0;void*f1;};struct Cyc_Dict_T{enum Cyc_Dict_Color color;const struct Cyc_Dict_T*left;const struct Cyc_Dict_T*right;struct _tuple0 key_val;};
# 40
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){struct Cyc_Dict_Dict _T0;{struct Cyc_Dict_Dict _T1;
_T1.rel=comp;_T1.r=r;_T1.t=0;_T0=_T1;}return _T0;}
# 43
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){struct _RegionHandle*_T0;int(*_T1)(void*,void*);struct Cyc_Dict_Dict _T2;_T0=Cyc_Core_heap_region;_T1=comp;_T2=
Cyc_Dict_rempty(_T0,_T1);return _T2;}
# 46
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_T*_T1;struct _RegionHandle*_T2;{struct Cyc_Dict_Dict _T3;
# 48
_T3.rel=comp;_T3.r=r;_T2=r;{struct Cyc_Dict_T*_T4=_region_malloc(_T2,0U,sizeof(struct Cyc_Dict_T));_T4->color=1U;_T4->left=0;_T4->right=0;_T4->key_val.f0=key;_T4->key_val.f1=data;_T1=(struct Cyc_Dict_T*)_T4;}_T3.t=_T1;_T0=_T3;}return _T0;}
# 50
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){struct _RegionHandle*_T0;int(*_T1)(void*,void*);void*_T2;void*_T3;struct Cyc_Dict_Dict _T4;_T0=Cyc_Core_heap_region;_T1=comp;_T2=key;_T3=data;_T4=
Cyc_Dict_rsingleton(_T0,_T1,_T2,_T3);return _T4;}
# 54
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;struct Cyc_Dict_Dict _T2;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
const struct Cyc_Dict_T*t2=t;{struct Cyc_Dict_Dict _T3;_T2=d;
_T3.rel=_T2.rel;_T3.r=r;_T3.t=t2;_T1=_T3;}return _T1;}}
# 60
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;const struct Cyc_Dict_T*_T1;int _T2;_T0=d;_T1=_T0.t;_T2=_T1==0;
return _T2;}
# 64
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;void*_T2;const struct Cyc_Dict_T*_T3;struct _tuple0 _T4;void*_T5;const struct Cyc_Dict_T*_T6;const struct Cyc_Dict_T*_T7;_T0=d;{
int(*rel)(void*,void*)=_T0.rel;_T1=d;{
const struct Cyc_Dict_T*t=_T1.t;
_TL0: if(t!=0)goto _TL1;else{goto _TL2;}
_TL1: _T2=key;_T3=t;_T4=_T3->key_val;_T5=_T4.f0;{int i=rel(_T2,_T5);
if(i >= 0)goto _TL3;_T6=t;t=_T6->left;goto _TL4;
_TL3: if(i <= 0)goto _TL5;_T7=t;t=_T7->right;goto _TL6;
_TL5: return 1;_TL6: _TL4:;}goto _TL0;_TL2:
# 73
 return 0;}}}
# 76
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;void*_T2;const struct Cyc_Dict_T*_T3;struct _tuple0 _T4;void*_T5;const struct Cyc_Dict_T*_T6;const struct Cyc_Dict_T*_T7;const struct Cyc_Dict_T*_T8;struct _tuple0 _T9;void*_TA;struct Cyc_Dict_Absent_exn_struct*_TB;struct Cyc_Dict_Absent_exn_struct*_TC;_T0=d;{
int(*rel)(void*,void*)=_T0.rel;_T1=d;{
const struct Cyc_Dict_T*t=_T1.t;
_TL7: if(t!=0)goto _TL8;else{goto _TL9;}
_TL8: _T2=key;_T3=t;_T4=_T3->key_val;_T5=_T4.f0;{int i=rel(_T2,_T5);
if(i >= 0)goto _TLA;_T6=t;t=_T6->left;goto _TLB;
_TLA: if(i <= 0)goto _TLC;_T7=t;t=_T7->right;goto _TLD;
_TLC: _T8=t;_T9=_T8->key_val;_TA=_T9.f1;return _TA;_TLD: _TLB:;}goto _TL7;_TL9: _TB=& Cyc_Dict_Absent_val;_TC=(struct Cyc_Dict_Absent_exn_struct*)_TB;_throw(_TC);}}}
# 88
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*key){struct Cyc_Dict_Dict _T0;void*_T1;const struct Cyc_Dict_T*_T2;struct _tuple0 _T3;void*_T4;const struct Cyc_Dict_T*_T5;const struct Cyc_Dict_T*_T6;const struct Cyc_Dict_T*_T7;struct _tuple0 _T8;void*_T9;struct Cyc_Dict_Absent_exn_struct*_TA;struct Cyc_Dict_Absent_exn_struct*_TB;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
_TLE: if(t!=0)goto _TLF;else{goto _TL10;}
_TLF: _T1=key;_T2=t;_T3=_T2->key_val;_T4=_T3.f0;{int i=cmp(_T1,_T4);
if(i >= 0)goto _TL11;_T5=t;t=_T5->left;goto _TL12;
_TL11: if(i <= 0)goto _TL13;_T6=t;t=_T6->right;goto _TL14;
_TL13: _T7=t;_T8=_T7->key_val;_T9=_T8.f1;return _T9;_TL14: _TL12:;}goto _TLE;_TL10: _TA=& Cyc_Dict_Absent_val;_TB=(struct Cyc_Dict_Absent_exn_struct*)_TA;_throw(_TB);}}
# 99
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*key){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;void*_T2;const struct Cyc_Dict_T*_T3;struct _tuple0 _T4;void*_T5;const struct Cyc_Dict_T*_T6;const struct Cyc_Dict_T*_T7;const struct Cyc_Dict_T*_T8;void**_T9;_T0=d;{
int(*rel)(void*,void*)=_T0.rel;_T1=d;{
const struct Cyc_Dict_T*t=_T1.t;
_TL15: if(t!=0)goto _TL16;else{goto _TL17;}
_TL16: _T2=key;_T3=t;_T4=_T3->key_val;_T5=_T4.f0;{int i=rel(_T2,_T5);
if(i >= 0)goto _TL18;_T6=t;t=_T6->left;goto _TL19;
_TL18: if(i <= 0)goto _TL1A;_T7=t;t=_T7->right;goto _TL1B;
_TL1A: _T8=t;_T9=& _T8->key_val.f1;return _T9;_TL1B: _TL19:;}goto _TL15;_TL17:
# 108
 return 0;}}}
# 111
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*key,void**ans_place){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;void*_T2;const struct Cyc_Dict_T*_T3;struct _tuple0 _T4;void*_T5;const struct Cyc_Dict_T*_T6;const struct Cyc_Dict_T*_T7;void**_T8;const struct Cyc_Dict_T*_T9;struct _tuple0 _TA;_T0=d;{
int(*rel)(void*,void*)=_T0.rel;_T1=d;{
const struct Cyc_Dict_T*t=_T1.t;
_TL1C: if(t!=0)goto _TL1D;else{goto _TL1E;}
_TL1D: _T2=key;_T3=t;_T4=_T3->key_val;_T5=_T4.f0;{int i=rel(_T2,_T5);
if(i >= 0)goto _TL1F;_T6=t;t=_T6->left;goto _TL20;
_TL1F: if(i <= 0)goto _TL21;_T7=t;t=_T7->right;goto _TL22;
# 119
_TL21: _T8=ans_place;_T9=t;_TA=_T9->key_val;*_T8=_TA.f1;
return 1;_TL22: _TL20:;}goto _TL1C;_TL1E:
# 123
 return 0;}}}struct _tuple1{enum Cyc_Dict_Color f0;const struct Cyc_Dict_T*f1;const struct Cyc_Dict_T*f2;struct _tuple0 f3;};
# 126
static struct Cyc_Dict_T*Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){struct _tuple1 _T0;enum Cyc_Dict_Color _T1;struct _tuple1 _T2;const struct Cyc_Dict_T*_T3;struct _tuple1 _T4;const struct Cyc_Dict_T*_T5;const struct Cyc_Dict_T*_T6;enum Cyc_Dict_Color _T7;struct _tuple1 _T8;const struct Cyc_Dict_T*_T9;const struct Cyc_Dict_T*_TA;const struct Cyc_Dict_T*_TB;struct _tuple1 _TC;const struct Cyc_Dict_T*_TD;const struct Cyc_Dict_T*_TE;const struct Cyc_Dict_T*_TF;const struct Cyc_Dict_T*_T10;enum Cyc_Dict_Color _T11;struct _tuple1 _T12;const struct Cyc_Dict_T*_T13;const struct Cyc_Dict_T*_T14;struct _tuple1 _T15;struct _tuple1 _T16;struct _tuple1 _T17;const struct Cyc_Dict_T*_T18;const struct Cyc_Dict_T*_T19;const struct Cyc_Dict_T*_T1A;struct _tuple1 _T1B;const struct Cyc_Dict_T*_T1C;const struct Cyc_Dict_T*_T1D;const struct Cyc_Dict_T*_T1E;const struct Cyc_Dict_T*_T1F;enum Cyc_Dict_Color _T20;struct _tuple1 _T21;const struct Cyc_Dict_T*_T22;struct _tuple1 _T23;const struct Cyc_Dict_T*_T24;const struct Cyc_Dict_T*_T25;enum Cyc_Dict_Color _T26;struct _tuple1 _T27;const struct Cyc_Dict_T*_T28;const struct Cyc_Dict_T*_T29;const struct Cyc_Dict_T*_T2A;struct _tuple1 _T2B;const struct Cyc_Dict_T*_T2C;const struct Cyc_Dict_T*_T2D;const struct Cyc_Dict_T*_T2E;const struct Cyc_Dict_T*_T2F;enum Cyc_Dict_Color _T30;struct _tuple1 _T31;const struct Cyc_Dict_T*_T32;const struct Cyc_Dict_T*_T33;const struct Cyc_Dict_T*_T34;struct _tuple1 _T35;const struct Cyc_Dict_T*_T36;const struct Cyc_Dict_T*_T37;const struct Cyc_Dict_T*_T38;const struct Cyc_Dict_T*_T39;enum Cyc_Dict_Color _T3A;struct _tuple1 _T3B;const struct Cyc_Dict_T*_T3C;const struct Cyc_Dict_T*_T3D;const struct Cyc_Dict_T*_T3E;struct _tuple1 _T3F;const struct Cyc_Dict_T*_T40;const struct Cyc_Dict_T*_T41;const struct Cyc_Dict_T*_T42;const struct Cyc_Dict_T*_T43;enum Cyc_Dict_Color _T44;struct _tuple1 _T45;const struct Cyc_Dict_T*_T46;struct _tuple1 _T47;const struct Cyc_Dict_T*_T48;const struct Cyc_Dict_T*_T49;enum Cyc_Dict_Color _T4A;struct _tuple1 _T4B;const struct Cyc_Dict_T*_T4C;const struct Cyc_Dict_T*_T4D;const struct Cyc_Dict_T*_T4E;struct _tuple1 _T4F;const struct Cyc_Dict_T*_T50;const struct Cyc_Dict_T*_T51;const struct Cyc_Dict_T*_T52;const struct Cyc_Dict_T*_T53;enum Cyc_Dict_Color _T54;struct _tuple1 _T55;const struct Cyc_Dict_T*_T56;const struct Cyc_Dict_T*_T57;const struct Cyc_Dict_T*_T58;struct _tuple1 _T59;const struct Cyc_Dict_T*_T5A;const struct Cyc_Dict_T*_T5B;const struct Cyc_Dict_T*_T5C;const struct Cyc_Dict_T*_T5D;enum Cyc_Dict_Color _T5E;struct _tuple1 _T5F;const struct Cyc_Dict_T*_T60;const struct Cyc_Dict_T*_T61;const struct Cyc_Dict_T*_T62;struct _tuple1 _T63;const struct Cyc_Dict_T*_T64;const struct Cyc_Dict_T*_T65;const struct Cyc_Dict_T*_T66;const struct Cyc_Dict_T*_T67;enum Cyc_Dict_Color _T68;struct _tuple1 _T69;const struct Cyc_Dict_T*_T6A;const struct Cyc_Dict_T*_T6B;const struct Cyc_Dict_T*_T6C;struct _tuple1 _T6D;const struct Cyc_Dict_T*_T6E;const struct Cyc_Dict_T*_T6F;const struct Cyc_Dict_T*_T70;const struct Cyc_Dict_T*_T71;enum Cyc_Dict_Color _T72;struct _tuple1 _T73;const struct Cyc_Dict_T*_T74;const struct Cyc_Dict_T*_T75;struct _tuple1 _T76;struct _tuple1 _T77;struct _tuple1 _T78;const struct Cyc_Dict_T*_T79;struct _tuple1 _T7A;const struct Cyc_Dict_T*_T7B;const struct Cyc_Dict_T*_T7C;enum Cyc_Dict_Color _T7D;struct _tuple1 _T7E;const struct Cyc_Dict_T*_T7F;const struct Cyc_Dict_T*_T80;const struct Cyc_Dict_T*_T81;struct _tuple1 _T82;const struct Cyc_Dict_T*_T83;const struct Cyc_Dict_T*_T84;const struct Cyc_Dict_T*_T85;const struct Cyc_Dict_T*_T86;enum Cyc_Dict_Color _T87;struct _tuple1 _T88;const struct Cyc_Dict_T*_T89;const struct Cyc_Dict_T*_T8A;const struct Cyc_Dict_T*_T8B;struct _tuple1 _T8C;const struct Cyc_Dict_T*_T8D;const struct Cyc_Dict_T*_T8E;const struct Cyc_Dict_T*_T8F;const struct Cyc_Dict_T*_T90;enum Cyc_Dict_Color _T91;struct _tuple1 _T92;const struct Cyc_Dict_T*_T93;const struct Cyc_Dict_T*_T94;const struct Cyc_Dict_T*_T95;struct _tuple1 _T96;const struct Cyc_Dict_T*_T97;const struct Cyc_Dict_T*_T98;const struct Cyc_Dict_T*_T99;const struct Cyc_Dict_T*_T9A;enum Cyc_Dict_Color _T9B;struct _tuple1 _T9C;const struct Cyc_Dict_T*_T9D;struct _tuple1 _T9E;const struct Cyc_Dict_T*_T9F;const struct Cyc_Dict_T*_TA0;enum Cyc_Dict_Color _TA1;struct _tuple1 _TA2;const struct Cyc_Dict_T*_TA3;const struct Cyc_Dict_T*_TA4;const struct Cyc_Dict_T*_TA5;struct _tuple1 _TA6;const struct Cyc_Dict_T*_TA7;const struct Cyc_Dict_T*_TA8;const struct Cyc_Dict_T*_TA9;const struct Cyc_Dict_T*_TAA;enum Cyc_Dict_Color _TAB;struct _tuple1 _TAC;const struct Cyc_Dict_T*_TAD;const struct Cyc_Dict_T*_TAE;const struct Cyc_Dict_T*_TAF;struct _tuple1 _TB0;const struct Cyc_Dict_T*_TB1;const struct Cyc_Dict_T*_TB2;const struct Cyc_Dict_T*_TB3;const struct Cyc_Dict_T*_TB4;enum Cyc_Dict_Color _TB5;struct _tuple1 _TB6;const struct Cyc_Dict_T*_TB7;const struct Cyc_Dict_T*_TB8;const struct Cyc_Dict_T*_TB9;struct _tuple1 _TBA;const struct Cyc_Dict_T*_TBB;const struct Cyc_Dict_T*_TBC;const struct Cyc_Dict_T*_TBD;const struct Cyc_Dict_T*_TBE;enum Cyc_Dict_Color _TBF;struct _tuple1 _TC0;const struct Cyc_Dict_T*_TC1;struct _tuple1 _TC2;const struct Cyc_Dict_T*_TC3;const struct Cyc_Dict_T*_TC4;enum Cyc_Dict_Color _TC5;struct _tuple1 _TC6;const struct Cyc_Dict_T*_TC7;const struct Cyc_Dict_T*_TC8;const struct Cyc_Dict_T*_TC9;struct _tuple1 _TCA;const struct Cyc_Dict_T*_TCB;const struct Cyc_Dict_T*_TCC;const struct Cyc_Dict_T*_TCD;const struct Cyc_Dict_T*_TCE;enum Cyc_Dict_Color _TCF;struct _tuple1 _TD0;const struct Cyc_Dict_T*_TD1;const struct Cyc_Dict_T*_TD2;const struct Cyc_Dict_T*_TD3;struct _tuple1 _TD4;const struct Cyc_Dict_T*_TD5;const struct Cyc_Dict_T*_TD6;const struct Cyc_Dict_T*_TD7;const struct Cyc_Dict_T*_TD8;enum Cyc_Dict_Color _TD9;struct _tuple1 _TDA;const struct Cyc_Dict_T*_TDB;const struct Cyc_Dict_T*_TDC;const struct Cyc_Dict_T*_TDD;struct _tuple1 _TDE;const struct Cyc_Dict_T*_TDF;const struct Cyc_Dict_T*_TE0;const struct Cyc_Dict_T*_TE1;const struct Cyc_Dict_T*_TE2;enum Cyc_Dict_Color _TE3;struct _tuple1 _TE4;const struct Cyc_Dict_T*_TE5;struct _tuple1 _TE6;const struct Cyc_Dict_T*_TE7;const struct Cyc_Dict_T*_TE8;enum Cyc_Dict_Color _TE9;struct _tuple1 _TEA;const struct Cyc_Dict_T*_TEB;const struct Cyc_Dict_T*_TEC;const struct Cyc_Dict_T*_TED;struct _tuple1 _TEE;const struct Cyc_Dict_T*_TEF;const struct Cyc_Dict_T*_TF0;const struct Cyc_Dict_T*_TF1;const struct Cyc_Dict_T*_TF2;enum Cyc_Dict_Color _TF3;struct _tuple1 _TF4;struct _tuple1 _TF5;const struct Cyc_Dict_T*_TF6;const struct Cyc_Dict_T*_TF7;struct _tuple1 _TF8;struct _tuple1 _TF9;const struct Cyc_Dict_T*_TFA;const struct Cyc_Dict_T*_TFB;const struct Cyc_Dict_T*_TFC;struct _tuple1 _TFD;const struct Cyc_Dict_T*_TFE;const struct Cyc_Dict_T*_TFF;const struct Cyc_Dict_T*_T100;const struct Cyc_Dict_T*_T101;enum Cyc_Dict_Color _T102;struct _tuple1 _T103;const struct Cyc_Dict_T*_T104;const struct Cyc_Dict_T*_T105;const struct Cyc_Dict_T*_T106;struct _tuple1 _T107;const struct Cyc_Dict_T*_T108;const struct Cyc_Dict_T*_T109;const struct Cyc_Dict_T*_T10A;const struct Cyc_Dict_T*_T10B;enum Cyc_Dict_Color _T10C;struct _tuple1 _T10D;struct _tuple1 _T10E;const struct Cyc_Dict_T*_T10F;const struct Cyc_Dict_T*_T110;struct _tuple1 _T111;struct Cyc_Dict_T*_T112;struct _RegionHandle*_T113;struct Cyc_Dict_T*_T114;struct _RegionHandle*_T115;struct Cyc_Dict_T*_T116;struct _RegionHandle*_T117;struct _tuple1 _T118;struct _tuple1 _T119;struct _tuple1 _T11A;struct _tuple1 _T11B;struct Cyc_Dict_T*_T11C;struct _RegionHandle*_T11D;enum Cyc_Dict_Color _T11E;struct _tuple0 _T11F;const struct Cyc_Dict_T*_T120;struct _tuple0 _T121;const struct Cyc_Dict_T*_T122;struct _tuple0 _T123;const struct Cyc_Dict_T*_T124;const struct Cyc_Dict_T*_T125;_T0=quad;_T1=_T0.f0;if(_T1!=Cyc_Dict_B)goto _TL23;_T2=quad;_T3=_T2.f1;if(_T3==0)goto _TL25;_T4=quad;_T5=_T4.f1;_T6=(const struct Cyc_Dict_T*)_T5;_T7=_T6->color;if(_T7!=Cyc_Dict_R)goto _TL27;_T8=quad;_T9=_T8.f1;_TA=(const struct Cyc_Dict_T*)_T9;_TB=_TA->left;if(_TB==0)goto _TL29;_TC=quad;_TD=_TC.f1;_TE=(const struct Cyc_Dict_T*)_TD;_TF=_TE->left;_T10=(const struct Cyc_Dict_T*)_TF;_T11=_T10->color;if(_T11!=Cyc_Dict_R)goto _TL2B;_T12=quad;_T13=_T12.f1;{struct Cyc_Dict_T _T126=*_T13;_T14=_T126.left;{struct Cyc_Dict_T _T127=*_T14;_T125=_T127.left;_T124=_T127.right;_T123=_T127.key_val;}_T122=_T126.right;_T121=_T126.key_val;}_T15=quad;_T120=_T15.f2;_T16=quad;_T11F=_T16.f3;{const struct Cyc_Dict_T*a=_T125;const struct Cyc_Dict_T*b=_T124;struct _tuple0 x=_T123;const struct Cyc_Dict_T*c=_T122;struct _tuple0 y=_T121;const struct Cyc_Dict_T*d=_T120;struct _tuple0 z=_T11F;_T125=a;_T124=b;_T122=c;_T123=y;_T121=x;_T120=d;_T11F=z;goto _LL4;}_TL2B: _T17=quad;_T18=_T17.f1;_T19=(const struct Cyc_Dict_T*)_T18;_T1A=_T19->right;if(_T1A==0)goto _TL2D;_T1B=quad;_T1C=_T1B.f1;_T1D=(const struct Cyc_Dict_T*)_T1C;_T1E=_T1D->right;_T1F=(const struct Cyc_Dict_T*)_T1E;_T20=_T1F->color;if(_T20!=Cyc_Dict_R)goto _TL2F;goto _LL3;_TL2F: _T21=quad;_T22=_T21.f2;if(_T22==0)goto _TL31;_T23=quad;_T24=_T23.f2;_T25=(const struct Cyc_Dict_T*)_T24;_T26=_T25->color;if(_T26!=Cyc_Dict_R)goto _TL33;_T27=quad;_T28=_T27.f2;_T29=(const struct Cyc_Dict_T*)_T28;_T2A=_T29->left;if(_T2A==0)goto _TL35;_T2B=quad;_T2C=_T2B.f2;_T2D=(const struct Cyc_Dict_T*)_T2C;_T2E=_T2D->left;_T2F=(const struct Cyc_Dict_T*)_T2E;_T30=_T2F->color;if(_T30!=Cyc_Dict_R)goto _TL37;goto _LL5;_TL37: _T31=quad;_T32=_T31.f2;_T33=(const struct Cyc_Dict_T*)_T32;_T34=_T33->right;if(_T34==0)goto _TL39;_T35=quad;_T36=_T35.f2;_T37=(const struct Cyc_Dict_T*)_T36;_T38=_T37->right;_T39=(const struct Cyc_Dict_T*)_T38;_T3A=_T39->color;if(_T3A!=Cyc_Dict_R)goto _TL3B;goto _LL7;_TL3B: goto _LL9;_TL39: goto _LL9;_TL35: _T3B=quad;_T3C=_T3B.f2;_T3D=(const struct Cyc_Dict_T*)_T3C;_T3E=_T3D->right;if(_T3E==0)goto _TL3D;_T3F=quad;_T40=_T3F.f2;_T41=(const struct Cyc_Dict_T*)_T40;_T42=_T41->right;_T43=(const struct Cyc_Dict_T*)_T42;_T44=_T43->color;if(_T44!=Cyc_Dict_R)goto _TL3F;goto _LL7;_TL3F: goto _LL9;_TL3D: goto _LL9;_TL33: goto _LL9;_TL31: goto _LL9;_TL2D: _T45=quad;_T46=_T45.f2;if(_T46==0)goto _TL41;_T47=quad;_T48=_T47.f2;_T49=(const struct Cyc_Dict_T*)_T48;_T4A=_T49->color;if(_T4A!=Cyc_Dict_R)goto _TL43;_T4B=quad;_T4C=_T4B.f2;_T4D=(const struct Cyc_Dict_T*)_T4C;_T4E=_T4D->left;if(_T4E==0)goto _TL45;_T4F=quad;_T50=_T4F.f2;_T51=(const struct Cyc_Dict_T*)_T50;_T52=_T51->left;_T53=(const struct Cyc_Dict_T*)_T52;_T54=_T53->color;if(_T54!=Cyc_Dict_R)goto _TL47;goto _LL5;_TL47: _T55=quad;_T56=_T55.f2;_T57=(const struct Cyc_Dict_T*)_T56;_T58=_T57->right;if(_T58==0)goto _TL49;_T59=quad;_T5A=_T59.f2;_T5B=(const struct Cyc_Dict_T*)_T5A;_T5C=_T5B->right;_T5D=(const struct Cyc_Dict_T*)_T5C;_T5E=_T5D->color;if(_T5E!=Cyc_Dict_R)goto _TL4B;goto _LL7;_TL4B: goto _LL9;_TL49: goto _LL9;_TL45: _T5F=quad;_T60=_T5F.f2;_T61=(const struct Cyc_Dict_T*)_T60;_T62=_T61->right;if(_T62==0)goto _TL4D;_T63=quad;_T64=_T63.f2;_T65=(const struct Cyc_Dict_T*)_T64;_T66=_T65->right;_T67=(const struct Cyc_Dict_T*)_T66;_T68=_T67->color;if(_T68!=Cyc_Dict_R)goto _TL4F;goto _LL7;_TL4F: goto _LL9;_TL4D: goto _LL9;_TL43: goto _LL9;_TL41: goto _LL9;_TL29: _T69=quad;_T6A=_T69.f1;_T6B=(const struct Cyc_Dict_T*)_T6A;_T6C=_T6B->right;if(_T6C==0)goto _TL51;_T6D=quad;_T6E=_T6D.f1;_T6F=(const struct Cyc_Dict_T*)_T6E;_T70=_T6F->right;_T71=(const struct Cyc_Dict_T*)_T70;_T72=_T71->color;if(_T72!=Cyc_Dict_R)goto _TL53;_LL3: _T73=quad;_T74=_T73.f1;{struct Cyc_Dict_T _T126=*_T74;_T125=_T126.left;_T75=_T126.right;{struct Cyc_Dict_T _T127=*_T75;_T124=_T127.left;_T122=_T127.right;_T123=_T127.key_val;}_T121=_T126.key_val;}_T76=quad;_T120=_T76.f2;_T77=quad;_T11F=_T77.f3;_LL4:{const struct Cyc_Dict_T*a=_T125;const struct Cyc_Dict_T*b=_T124;const struct Cyc_Dict_T*c=_T122;struct _tuple0 y=_T123;struct _tuple0 x=_T121;const struct Cyc_Dict_T*d=_T120;struct _tuple0 z=_T11F;_T125=a;_T124=b;_T122=c;_T123=y;_T120=d;_T121=z;_T11F=x;goto _LL6;}_TL53: _T78=quad;_T79=_T78.f2;if(_T79==0)goto _TL55;_T7A=quad;_T7B=_T7A.f2;_T7C=(const struct Cyc_Dict_T*)_T7B;_T7D=_T7C->color;if(_T7D!=Cyc_Dict_R)goto _TL57;_T7E=quad;_T7F=_T7E.f2;_T80=(const struct Cyc_Dict_T*)_T7F;_T81=_T80->left;if(_T81==0)goto _TL59;_T82=quad;_T83=_T82.f2;_T84=(const struct Cyc_Dict_T*)_T83;_T85=_T84->left;_T86=(const struct Cyc_Dict_T*)_T85;_T87=_T86->color;if(_T87!=Cyc_Dict_R)goto _TL5B;goto _LL5;_TL5B: _T88=quad;_T89=_T88.f2;_T8A=(const struct Cyc_Dict_T*)_T89;_T8B=_T8A->right;if(_T8B==0)goto _TL5D;_T8C=quad;_T8D=_T8C.f2;_T8E=(const struct Cyc_Dict_T*)_T8D;_T8F=_T8E->right;_T90=(const struct Cyc_Dict_T*)_T8F;_T91=_T90->color;if(_T91!=Cyc_Dict_R)goto _TL5F;goto _LL7;_TL5F: goto _LL9;_TL5D: goto _LL9;_TL59: _T92=quad;_T93=_T92.f2;_T94=(const struct Cyc_Dict_T*)_T93;_T95=_T94->right;if(_T95==0)goto _TL61;_T96=quad;_T97=_T96.f2;_T98=(const struct Cyc_Dict_T*)_T97;_T99=_T98->right;_T9A=(const struct Cyc_Dict_T*)_T99;_T9B=_T9A->color;if(_T9B!=Cyc_Dict_R)goto _TL63;goto _LL7;_TL63: goto _LL9;_TL61: goto _LL9;_TL57: goto _LL9;_TL55: goto _LL9;_TL51: _T9C=quad;_T9D=_T9C.f2;if(_T9D==0)goto _TL65;_T9E=quad;_T9F=_T9E.f2;_TA0=(const struct Cyc_Dict_T*)_T9F;_TA1=_TA0->color;if(_TA1!=Cyc_Dict_R)goto _TL67;_TA2=quad;_TA3=_TA2.f2;_TA4=(const struct Cyc_Dict_T*)_TA3;_TA5=_TA4->left;if(_TA5==0)goto _TL69;_TA6=quad;_TA7=_TA6.f2;_TA8=(const struct Cyc_Dict_T*)_TA7;_TA9=_TA8->left;_TAA=(const struct Cyc_Dict_T*)_TA9;_TAB=_TAA->color;if(_TAB!=Cyc_Dict_R)goto _TL6B;goto _LL5;_TL6B: _TAC=quad;_TAD=_TAC.f2;_TAE=(const struct Cyc_Dict_T*)_TAD;_TAF=_TAE->right;if(_TAF==0)goto _TL6D;_TB0=quad;_TB1=_TB0.f2;_TB2=(const struct Cyc_Dict_T*)_TB1;_TB3=_TB2->right;_TB4=(const struct Cyc_Dict_T*)_TB3;_TB5=_TB4->color;if(_TB5!=Cyc_Dict_R)goto _TL6F;goto _LL7;_TL6F: goto _LL9;_TL6D: goto _LL9;_TL69: _TB6=quad;_TB7=_TB6.f2;_TB8=(const struct Cyc_Dict_T*)_TB7;_TB9=_TB8->right;if(_TB9==0)goto _TL71;_TBA=quad;_TBB=_TBA.f2;_TBC=(const struct Cyc_Dict_T*)_TBB;_TBD=_TBC->right;_TBE=(const struct Cyc_Dict_T*)_TBD;_TBF=_TBE->color;if(_TBF!=Cyc_Dict_R)goto _TL73;goto _LL7;_TL73: goto _LL9;_TL71: goto _LL9;_TL67: goto _LL9;_TL65: goto _LL9;_TL27: _TC0=quad;_TC1=_TC0.f2;if(_TC1==0)goto _TL75;_TC2=quad;_TC3=_TC2.f2;_TC4=(const struct Cyc_Dict_T*)_TC3;_TC5=_TC4->color;if(_TC5!=Cyc_Dict_R)goto _TL77;_TC6=quad;_TC7=_TC6.f2;_TC8=(const struct Cyc_Dict_T*)_TC7;_TC9=_TC8->left;if(_TC9==0)goto _TL79;_TCA=quad;_TCB=_TCA.f2;_TCC=(const struct Cyc_Dict_T*)_TCB;_TCD=_TCC->left;_TCE=(const struct Cyc_Dict_T*)_TCD;_TCF=_TCE->color;if(_TCF!=Cyc_Dict_R)goto _TL7B;goto _LL5;_TL7B: _TD0=quad;_TD1=_TD0.f2;_TD2=(const struct Cyc_Dict_T*)_TD1;_TD3=_TD2->right;if(_TD3==0)goto _TL7D;_TD4=quad;_TD5=_TD4.f2;_TD6=(const struct Cyc_Dict_T*)_TD5;_TD7=_TD6->right;_TD8=(const struct Cyc_Dict_T*)_TD7;_TD9=_TD8->color;if(_TD9!=Cyc_Dict_R)goto _TL7F;goto _LL7;_TL7F: goto _LL9;_TL7D: goto _LL9;_TL79: _TDA=quad;_TDB=_TDA.f2;_TDC=(const struct Cyc_Dict_T*)_TDB;_TDD=_TDC->right;if(_TDD==0)goto _TL81;_TDE=quad;_TDF=_TDE.f2;_TE0=(const struct Cyc_Dict_T*)_TDF;_TE1=_TE0->right;_TE2=(const struct Cyc_Dict_T*)_TE1;_TE3=_TE2->color;if(_TE3!=Cyc_Dict_R)goto _TL83;goto _LL7;_TL83: goto _LL9;_TL81: goto _LL9;_TL77: goto _LL9;_TL75: goto _LL9;_TL25: _TE4=quad;_TE5=_TE4.f2;if(_TE5==0)goto _TL85;_TE6=quad;_TE7=_TE6.f2;_TE8=(const struct Cyc_Dict_T*)_TE7;_TE9=_TE8->color;if(_TE9!=Cyc_Dict_R)goto _TL87;_TEA=quad;_TEB=_TEA.f2;_TEC=(const struct Cyc_Dict_T*)_TEB;_TED=_TEC->left;if(_TED==0)goto _TL89;_TEE=quad;_TEF=_TEE.f2;_TF0=(const struct Cyc_Dict_T*)_TEF;_TF1=_TF0->left;_TF2=(const struct Cyc_Dict_T*)_TF1;_TF3=_TF2->color;if(_TF3!=Cyc_Dict_R)goto _TL8B;_LL5: _TF4=quad;_T125=_TF4.f1;_TF5=quad;_TF6=_TF5.f2;{struct Cyc_Dict_T _T126=*_TF6;_TF7=_T126.left;{struct Cyc_Dict_T _T127=*_TF7;_T124=_T127.left;_T122=_T127.right;_T123=_T127.key_val;}_T120=_T126.right;_T121=_T126.key_val;}_TF8=quad;_T11F=_TF8.f3;_LL6:{const struct Cyc_Dict_T*a=_T125;const struct Cyc_Dict_T*b=_T124;const struct Cyc_Dict_T*c=_T122;struct _tuple0 y=_T123;const struct Cyc_Dict_T*d=_T120;struct _tuple0 z=_T121;struct _tuple0 x=_T11F;_T125=a;_T124=b;_T122=c;_T120=d;_T123=z;_T121=y;_T11F=x;goto _LL8;}_TL8B: _TF9=quad;_TFA=_TF9.f2;_TFB=(const struct Cyc_Dict_T*)_TFA;_TFC=_TFB->right;if(_TFC==0)goto _TL8D;_TFD=quad;_TFE=_TFD.f2;_TFF=(const struct Cyc_Dict_T*)_TFE;_T100=_TFF->right;_T101=(const struct Cyc_Dict_T*)_T100;_T102=_T101->color;if(_T102!=Cyc_Dict_R)goto _TL8F;goto _LL7;_TL8F: goto _LL9;_TL8D: goto _LL9;_TL89: _T103=quad;_T104=_T103.f2;_T105=(const struct Cyc_Dict_T*)_T104;_T106=_T105->right;if(_T106==0)goto _TL91;_T107=quad;_T108=_T107.f2;_T109=(const struct Cyc_Dict_T*)_T108;_T10A=_T109->right;_T10B=(const struct Cyc_Dict_T*)_T10A;_T10C=_T10B->color;if(_T10C!=Cyc_Dict_R)goto _TL93;_LL7: _T10D=quad;_T125=_T10D.f1;_T10E=quad;_T10F=_T10E.f2;{struct Cyc_Dict_T _T126=*_T10F;_T124=_T126.left;_T110=_T126.right;{struct Cyc_Dict_T _T127=*_T110;_T122=_T127.left;_T120=_T127.right;_T123=_T127.key_val;}_T121=_T126.key_val;}_T111=quad;_T11F=_T111.f3;_LL8:{const struct Cyc_Dict_T*a=_T125;const struct Cyc_Dict_T*b=_T124;const struct Cyc_Dict_T*c=_T122;const struct Cyc_Dict_T*d=_T120;struct _tuple0 z=_T123;struct _tuple0 y=_T121;struct _tuple0 x=_T11F;_T113=r;{struct Cyc_Dict_T*_T126=_region_malloc(_T113,0U,sizeof(struct Cyc_Dict_T));
# 134
_T126->color=0U;_T115=r;{struct Cyc_Dict_T*_T127=_region_malloc(_T115,0U,sizeof(struct Cyc_Dict_T));_T127->color=1U;_T127->left=a;_T127->right=b;_T127->key_val=x;_T114=(struct Cyc_Dict_T*)_T127;}_T126->left=_T114;_T117=r;{struct Cyc_Dict_T*_T127=_region_malloc(_T117,0U,sizeof(struct Cyc_Dict_T));_T127->color=1U;_T127->left=c;_T127->right=d;_T127->key_val=z;_T116=(struct Cyc_Dict_T*)_T127;}_T126->right=_T116;_T126->key_val=y;_T112=(struct Cyc_Dict_T*)_T126;}return _T112;}_TL93: goto _LL9;_TL91: goto _LL9;_TL87: goto _LL9;_TL85: goto _LL9;_TL23: _LL9: _T118=quad;_T11E=_T118.f0;_T119=quad;_T125=_T119.f1;_T11A=quad;_T124=_T11A.f2;_T11B=quad;_T123=_T11B.f3;{enum Cyc_Dict_Color a=_T11E;const struct Cyc_Dict_T*b=_T125;const struct Cyc_Dict_T*c=_T124;struct _tuple0 d=_T123;_T11D=r;{struct Cyc_Dict_T*_T126=_region_malloc(_T11D,0U,sizeof(struct Cyc_Dict_T));
# 136
_T126->color=a;_T126->left=b;_T126->right=c;_T126->key_val=d;_T11C=(struct Cyc_Dict_T*)_T126;}return _T11C;};}
# 140
static struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){struct Cyc_Dict_T*_T0;struct _RegionHandle*_T1;const struct Cyc_Dict_T*_T2;struct _tuple0 _T3;void*_T4;struct _tuple0 _T5;void*_T6;struct _RegionHandle*_T7;struct _tuple1 _T8;struct _RegionHandle*_T9;int(*_TA)(void*,void*);struct _tuple0 _TB;const struct Cyc_Dict_T*_TC;struct Cyc_Dict_T*_TD;struct _RegionHandle*_TE;struct _tuple1 _TF;struct _RegionHandle*_T10;int(*_T11)(void*,void*);struct _tuple0 _T12;const struct Cyc_Dict_T*_T13;struct Cyc_Dict_T*_T14;struct Cyc_Dict_T*_T15;struct _RegionHandle*_T16;struct _tuple0 _T17;const struct Cyc_Dict_T*_T18;const struct Cyc_Dict_T*_T19;enum Cyc_Dict_Color _T1A;if(t!=0)goto _TL95;_T1=r;{struct Cyc_Dict_T*_T1B=_region_malloc(_T1,0U,sizeof(struct Cyc_Dict_T));
# 143
_T1B->color=0U;_T1B->left=0;_T1B->right=0;_T1B->key_val=key_val;_T0=(struct Cyc_Dict_T*)_T1B;}return _T0;_TL95: _T2=t;{struct Cyc_Dict_T _T1B=*_T2;_T1A=_T1B.color;_T19=_T1B.left;_T18=_T1B.right;_T17=_T1B.key_val;}{enum Cyc_Dict_Color color=_T1A;const struct Cyc_Dict_T*a=_T19;const struct Cyc_Dict_T*b=_T18;struct _tuple0 y=_T17;_T3=key_val;_T4=_T3.f0;_T5=y;_T6=_T5.f0;{
# 145
int i=rel(_T4,_T6);
if(i >= 0)goto _TL97;_T7=r;{struct _tuple1 _T1B;_T1B.f0=color;_T9=r;_TA=rel;_TB=key_val;_TC=a;_T1B.f1=Cyc_Dict_ins(_T9,_TA,_TB,_TC);_T1B.f2=b;_T1B.f3=y;_T8=_T1B;}_TD=Cyc_Dict_balance(_T7,_T8);return _TD;_TL97:
 if(i <= 0)goto _TL99;_TE=r;{struct _tuple1 _T1B;_T1B.f0=color;_T1B.f1=a;_T10=r;_T11=rel;_T12=key_val;_T13=b;_T1B.f2=Cyc_Dict_ins(_T10,_T11,_T12,_T13);_T1B.f3=y;_TF=_T1B;}_T14=Cyc_Dict_balance(_TE,_TF);return _T14;_TL99: _T16=r;{struct Cyc_Dict_T*_T1B=_region_malloc(_T16,0U,sizeof(struct Cyc_Dict_T));
_T1B->color=color;_T1B->left=a;_T1B->right=b;_T1B->key_val=key_val;_T15=(struct Cyc_Dict_T*)_T1B;}return _T15;}};}
# 152
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){struct Cyc_Dict_Dict _T0;struct _RegionHandle*_T1;struct Cyc_Dict_Dict _T2;int(*_T3)(void*,void*);struct _tuple0 _T4;struct Cyc_Dict_Dict _T5;const struct Cyc_Dict_T*_T6;struct Cyc_Dict_T*_T7;struct Cyc_Dict_Dict _T8;struct Cyc_Dict_Dict _T9;struct Cyc_Dict_Dict _TA;_T0=d;_T1=_T0.r;_T2=d;_T3=_T2.rel;{struct _tuple0 _TB;
_TB.f0=key;_TB.f1=data;_T4=_TB;}_T5=d;_T6=_T5.t;{struct Cyc_Dict_T*ans=Cyc_Dict_ins(_T1,_T3,_T4,_T6);_T7=ans;
_T7->color=1U;{struct Cyc_Dict_Dict _TB;_T9=d;
_TB.rel=_T9.rel;_TA=d;_TB.r=_TA.r;_TB.t=ans;_T8=_TB;}return _T8;}}
# 159
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,void*data){int _T0;struct Cyc_Dict_Absent_exn_struct*_T1;struct Cyc_Dict_Absent_exn_struct*_T2;struct Cyc_Dict_Dict _T3;_T0=
Cyc_Dict_member(d,key);if(!_T0)goto _TL9B;_T1=& Cyc_Dict_Absent_val;_T2=(struct Cyc_Dict_Absent_exn_struct*)_T1;_throw(_T2);goto _TL9C;_TL9B: _TL9C: _T3=
# 162
Cyc_Dict_insert(d,key,data);return _T3;}
# 165
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*kds){struct Cyc_Dict_Dict _T0;struct Cyc_List_List*_T1;void*_T2;struct _tuple0*_T3;struct _tuple0 _T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;struct _tuple0*_T8;struct _tuple0 _T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_Dict_Dict _TC;
# 167
_TLA0: if(kds!=0)goto _TL9E;else{goto _TL9F;}
_TL9E: _T0=d;_T1=kds;_T2=_T1->hd;_T3=(struct _tuple0*)_T2;_T4=_T3[0];_T5=_T4.f0;_T6=kds;_T7=_T6->hd;_T8=(struct _tuple0*)_T7;_T9=_T8[0];_TA=_T9.f1;d=Cyc_Dict_insert(_T0,_T5,_TA);_TB=kds;
# 167
kds=_TB->tl;goto _TLA0;_TL9F: _TC=d;
# 169
return _TC;}
# 172
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*t,void*accum){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;void*(*_T5)(void*,void*,void*);const struct Cyc_Dict_T*_T6;void*_T7;void*(*_T8)(void*,void*,void*);const struct Cyc_Dict_T*_T9;void*_TA;void*_TB;void*_TC;void*_TD;const struct Cyc_Dict_T*_TE;const struct Cyc_Dict_T*_TF;_T0=t;{struct Cyc_Dict_T _T10=*_T0;_TF=_T10.left;_TE=_T10.right;_T1=_T10.key_val;_T2=_T1.f0;_TD=(void*)_T2;_T3=_T10.key_val;_T4=_T3.f1;_TC=(void*)_T4;}{const struct Cyc_Dict_T*left=_TF;const struct Cyc_Dict_T*right=_TE;void*key=_TD;void*val=_TC;
# 174
if(left==0)goto _TLA1;_T5=f;_T6=left;_T7=accum;accum=Cyc_Dict_fold_tree(_T5,_T6,_T7);goto _TLA2;_TLA1: _TLA2:
 accum=f(key,val,accum);
if(right==0)goto _TLA3;_T8=f;_T9=right;_TA=accum;accum=Cyc_Dict_fold_tree(_T8,_T9,_TA);goto _TLA4;_TLA3: _TLA4: _TB=accum;
return _TB;}}
# 180
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){struct Cyc_Dict_Dict _T0;void*_T1;void*(*_T2)(void*,void*,void*);const struct Cyc_Dict_T*_T3;void*_T4;void*_T5;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t!=0)goto _TLA5;_T1=accum;
return _T1;_TLA5: _T2=f;_T3=t;_T4=accum;_T5=
Cyc_Dict_fold_tree(_T2,_T3,_T4);return _T5;}}
# 187
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;void*(*_T5)(void*,void*,void*,void*);void*_T6;const struct Cyc_Dict_T*_T7;void*_T8;void*(*_T9)(void*,void*,void*,void*);void*_TA;const struct Cyc_Dict_T*_TB;void*_TC;void*_TD;void*_TE;void*_TF;const struct Cyc_Dict_T*_T10;const struct Cyc_Dict_T*_T11;_T0=t;{struct Cyc_Dict_T _T12=*_T0;_T11=_T12.left;_T10=_T12.right;_T1=_T12.key_val;_T2=_T1.f0;_TF=(void*)_T2;_T3=_T12.key_val;_T4=_T3.f1;_TE=(void*)_T4;}{const struct Cyc_Dict_T*left=_T11;const struct Cyc_Dict_T*right=_T10;void*key=_TF;void*val=_TE;
# 190
if(left==0)goto _TLA7;_T5=f;_T6=env;_T7=left;_T8=accum;
accum=Cyc_Dict_fold_tree_c(_T5,_T6,_T7,_T8);goto _TLA8;_TLA7: _TLA8:
 accum=f(env,key,val,accum);
if(right==0)goto _TLA9;_T9=f;_TA=env;_TB=right;_TC=accum;
accum=Cyc_Dict_fold_tree_c(_T9,_TA,_TB,_TC);goto _TLAA;_TLA9: _TLAA: _TD=accum;
return _TD;}}
# 198
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){struct Cyc_Dict_Dict _T0;void*_T1;void*(*_T2)(void*,void*,void*,void*);void*_T3;const struct Cyc_Dict_T*_T4;void*_T5;void*_T6;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t!=0)goto _TLAB;_T1=accum;
return _T1;_TLAB: _T2=f;_T3=env;_T4=t;_T5=accum;_T6=
Cyc_Dict_fold_tree_c(_T2,_T3,_T4,_T5);return _T6;}}
# 205
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;void*(*_T5)(void*,void*);const struct Cyc_Dict_T*_T6;void*(*_T7)(void*,void*);const struct Cyc_Dict_T*_T8;void*_T9;void*_TA;const struct Cyc_Dict_T*_TB;const struct Cyc_Dict_T*_TC;_T0=t;{struct Cyc_Dict_T _TD=*_T0;_TC=_TD.left;_TB=_TD.right;_T1=_TD.key_val;_T2=_T1.f0;_TA=(void*)_T2;_T3=_TD.key_val;_T4=_T3.f1;_T9=(void*)_T4;}{const struct Cyc_Dict_T*left=_TC;const struct Cyc_Dict_T*right=_TB;void*key=_TA;void*val=_T9;
# 207
if(left==0)goto _TLAD;_T5=f;_T6=left;Cyc_Dict_app_tree(_T5,_T6);goto _TLAE;_TLAD: _TLAE:
 f(key,val);
if(right==0)goto _TLAF;_T7=f;_T8=right;Cyc_Dict_app_tree(_T7,_T8);goto _TLB0;_TLAF: _TLB0:;}}
# 212
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;void*(*_T1)(void*,void*);const struct Cyc_Dict_T*_T2;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t==0)goto _TLB1;_T1=f;_T2=t;
Cyc_Dict_app_tree(_T1,_T2);goto _TLB2;_TLB1: _TLB2:;}}
# 218
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;void*(*_T5)(void*,void*,void*);void*_T6;const struct Cyc_Dict_T*_T7;void*(*_T8)(void*,void*,void*);void*_T9;const struct Cyc_Dict_T*_TA;void*_TB;void*_TC;const struct Cyc_Dict_T*_TD;const struct Cyc_Dict_T*_TE;_T0=t;{struct Cyc_Dict_T _TF=*_T0;_TE=_TF.left;_TD=_TF.right;_T1=_TF.key_val;_T2=_T1.f0;_TC=(void*)_T2;_T3=_TF.key_val;_T4=_T3.f1;_TB=(void*)_T4;}{const struct Cyc_Dict_T*left=_TE;const struct Cyc_Dict_T*right=_TD;void*key=_TC;void*val=_TB;
# 220
if(left==0)goto _TLB3;_T5=f;_T6=env;_T7=left;Cyc_Dict_app_tree_c(_T5,_T6,_T7);goto _TLB4;_TLB3: _TLB4:
 f(env,key,val);
if(right==0)goto _TLB5;_T8=f;_T9=env;_TA=right;Cyc_Dict_app_tree_c(_T8,_T9,_TA);goto _TLB6;_TLB5: _TLB6:;}}
# 225
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;void*(*_T1)(void*,void*,void*);void*_T2;const struct Cyc_Dict_T*_T3;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t==0)goto _TLB7;_T1=f;_T2=env;_T3=t;
Cyc_Dict_app_tree_c(_T1,_T2,_T3);goto _TLB8;_TLB7: _TLB8:;}}
# 231
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;void(*_T5)(void*,void*);const struct Cyc_Dict_T*_T6;void(*_T7)(void*,void*);const struct Cyc_Dict_T*_T8;void*_T9;void*_TA;const struct Cyc_Dict_T*_TB;const struct Cyc_Dict_T*_TC;_T0=t;{struct Cyc_Dict_T _TD=*_T0;_TC=_TD.left;_TB=_TD.right;_T1=_TD.key_val;_T2=_T1.f0;_TA=(void*)_T2;_T3=_TD.key_val;_T4=_T3.f1;_T9=(void*)_T4;}{const struct Cyc_Dict_T*left=_TC;const struct Cyc_Dict_T*right=_TB;void*key=_TA;void*val=_T9;
# 233
if(left==0)goto _TLB9;_T5=f;_T6=left;Cyc_Dict_iter_tree(_T5,_T6);goto _TLBA;_TLB9: _TLBA:
 f(key,val);
if(right==0)goto _TLBB;_T7=f;_T8=right;Cyc_Dict_iter_tree(_T7,_T8);goto _TLBC;_TLBB: _TLBC:;}}
# 238
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;void(*_T1)(void*,void*);const struct Cyc_Dict_T*_T2;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t==0)goto _TLBD;_T1=f;_T2=t;
Cyc_Dict_iter_tree(_T1,_T2);goto _TLBE;_TLBD: _TLBE:;}}
# 244
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;void(*_T5)(void*,void*,void*);void*_T6;const struct Cyc_Dict_T*_T7;void(*_T8)(void*,void*,void*);void*_T9;const struct Cyc_Dict_T*_TA;void*_TB;void*_TC;const struct Cyc_Dict_T*_TD;const struct Cyc_Dict_T*_TE;_T0=t;{struct Cyc_Dict_T _TF=*_T0;_TE=_TF.left;_TD=_TF.right;_T1=_TF.key_val;_T2=_T1.f0;_TC=(void*)_T2;_T3=_TF.key_val;_T4=_T3.f1;_TB=(void*)_T4;}{const struct Cyc_Dict_T*left=_TE;const struct Cyc_Dict_T*right=_TD;void*key=_TC;void*val=_TB;
# 246
if(left==0)goto _TLBF;_T5=f;_T6=env;_T7=left;Cyc_Dict_iter_tree_c(_T5,_T6,_T7);goto _TLC0;_TLBF: _TLC0:
 f(env,key,val);
if(right==0)goto _TLC1;_T8=f;_T9=env;_TA=right;Cyc_Dict_iter_tree_c(_T8,_T9,_TA);goto _TLC2;_TLC1: _TLC2:;}}
# 251
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;void(*_T1)(void*,void*,void*);void*_T2;const struct Cyc_Dict_T*_T3;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t==0)goto _TLC3;_T1=f;_T2=env;_T3=t;
Cyc_Dict_iter_tree_c(_T1,_T2,_T3);goto _TLC4;_TLC3: _TLC4:;}}
# 257
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){int*_T0;int*_T1;int _T2;_T0=cnt;_T1=cnt;_T2=*_T1;
*_T0=_T2 + 1;}
# 260
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){void(*_T0)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict);int*_T1;struct Cyc_Dict_Dict _T2;int _T3;
int num=0;{
void(*_T4)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict)=(void(*)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_T0=_T4;}_T1=& num;_T2=d;_T0(Cyc_Dict_count_elem,_T1,_T2);_T3=num;
return _T3;}struct _tuple2{void(*f0)(void*,void*);struct Cyc_Dict_Dict f1;};
# 266
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){struct _tuple2*_T0;void*_T1;void*_T2;struct Cyc_Dict_Dict _T3;void(*_T4)(void*,void*);_T0=env;{struct _tuple2 _T5=*_T0;_T4=_T5.f0;_T3=_T5.f1;}{void(*f)(void*,void*)=_T4;struct Cyc_Dict_Dict d2=_T3;_T1=b1;_T2=
# 269
Cyc_Dict_lookup(d2,a);f(_T1,_T2);}}
# 272
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct _tuple2 _T0;void(*_T1)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict);struct _tuple2*_T2;struct _tuple2*_T3;struct Cyc_Dict_Dict _T4;{struct _tuple2 _T5;
# 275
_T5.f0=f;_T5.f1=d2;_T0=_T5;}{struct _tuple2 env=_T0;{
void(*_T5)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_T1=_T5;}_T2=& env;_T3=(struct _tuple2*)_T2;_T4=d1;_T1(Cyc_Dict_iter2_f,_T3,_T4);}}struct _tuple3{void(*f0)(void*,void*,void*);struct Cyc_Dict_Dict f1;void*f2;};
# 279
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){struct _tuple3*_T0;void*_T1;void*_T2;void*_T3;void*_T4;void*_T5;struct Cyc_Dict_Dict _T6;void(*_T7)(void*,void*,void*);_T0=env;{struct _tuple3 _T8=*_T0;_T7=_T8.f0;_T6=_T8.f1;_T1=_T8.f2;_T5=(void*)_T1;}{void(*f)(void*,void*,void*)=_T7;struct Cyc_Dict_Dict d2=_T6;void*inner_env=_T5;_T2=inner_env;_T3=b1;_T4=
# 282
Cyc_Dict_lookup(d2,a);f(_T2,_T3,_T4);}}
# 285
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct _tuple3 _T0;void(*_T1)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict);struct _tuple3*_T2;struct Cyc_Dict_Dict _T3;{struct _tuple3 _T4;
# 288
_T4.f0=f;_T4.f1=d2;_T4.f2=inner_env;_T0=_T4;}{struct _tuple3 env=_T0;{
void(*_T4)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_T1=_T4;}_T2=& env;_T3=d1;_T1(Cyc_Dict_iter2_c_f,_T2,_T3);}}struct _tuple4{void*(*f0)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f1;void*f2;};
# 292
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){struct _tuple4*_T0;void*_T1;void*_T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;void*_T8;struct Cyc_Dict_Dict _T9;void*(*_TA)(void*,void*,void*,void*,void*);_T0=env;{struct _tuple4 _TB=*_T0;_TA=_TB.f0;_T9=_TB.f1;_T1=_TB.f2;_T8=(void*)_T1;}{void*(*f)(void*,void*,void*,void*,void*)=_TA;struct Cyc_Dict_Dict d2=_T9;void*inner_env=_T8;_T2=inner_env;_T3=a;_T4=b1;_T5=
# 297
Cyc_Dict_lookup(d2,a);_T6=accum;_T7=f(_T2,_T3,_T4,_T5,_T6);return _T7;}}
# 300
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){struct _tuple4 _T0;void*(*_T1)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*);struct _tuple4*_T2;struct Cyc_Dict_Dict _T3;void*_T4;void*_T5;{struct _tuple4 _T6;
# 304
_T6.f0=f;_T6.f1=d2;_T6.f2=inner_env;_T0=_T6;}{struct _tuple4 env=_T0;{
void*(*_T6)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*)=(void*(*)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*))Cyc_Dict_fold_c;_T1=_T6;}_T2=& env;_T3=d1;_T4=accum;_T5=_T1(Cyc_Dict_fold2_c_f,_T2,_T3,_T4);return _T5;}}
# 308
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct Cyc_Dict_T*_T1;struct _RegionHandle*_T2;
if(t!=0)goto _TLC5;return 0;
# 311
_TLC5: _T0=t;{struct Cyc_Dict_T _T3=*_T0;struct _tuple0 _T4;const struct Cyc_Dict_T*_T5;const struct Cyc_Dict_T*_T6;enum Cyc_Dict_Color _T7;_T7=_T3.color;_T6=_T3.left;_T5=_T3.right;_T4=_T3.key_val;{enum Cyc_Dict_Color c=_T7;const struct Cyc_Dict_T*left=_T6;const struct Cyc_Dict_T*right=_T5;struct _tuple0 pr=_T4;
const struct Cyc_Dict_T*new_left=Cyc_Dict_copy_tree(r2,left);
const struct Cyc_Dict_T*new_right=Cyc_Dict_copy_tree(r2,right);_T2=r2;{struct Cyc_Dict_T*_T8=_region_malloc(_T2,0U,sizeof(struct Cyc_Dict_T));
_T8->color=c;_T8->left=new_left;_T8->right=new_right;_T8->key_val=pr;_T1=(struct Cyc_Dict_T*)_T8;}return _T1;}}}
# 318
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;struct _RegionHandle*_T2;struct Cyc_Dict_Dict _T3;const struct Cyc_Dict_T*_T4;{struct Cyc_Dict_Dict _T5;_T1=d;
_T5.rel=_T1.rel;_T5.r=r2;_T2=r2;_T3=d;_T4=_T3.t;_T5.t=Cyc_Dict_copy_tree(_T2,_T4);_T0=_T5;}return _T0;}
# 322
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;_T0=
Cyc_Dict_rcopy(Cyc_Core_heap_region,d);return _T0;}
# 326
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;const struct Cyc_Dict_T*_T5;struct _RegionHandle*_T6;void*(*_T7)(void*);const struct Cyc_Dict_T*_T8;const struct Cyc_Dict_T*_T9;struct _RegionHandle*_TA;void*(*_TB)(void*);const struct Cyc_Dict_T*_TC;struct Cyc_Dict_T*_TD;struct _RegionHandle*_TE;void*_TF;void*_T10;const struct Cyc_Dict_T*_T11;const struct Cyc_Dict_T*_T12;enum Cyc_Dict_Color _T13;_T0=t;{struct Cyc_Dict_T _T14=*_T0;_T13=_T14.color;_T12=_T14.left;_T11=_T14.right;_T1=_T14.key_val;_T2=_T1.f0;_T10=(void*)_T2;_T3=_T14.key_val;_T4=_T3.f1;_TF=(void*)_T4;}{enum Cyc_Dict_Color c=_T13;const struct Cyc_Dict_T*left=_T12;const struct Cyc_Dict_T*right=_T11;void*key=_T10;void*val=_TF;
# 329
if(left!=0)goto _TLC7;_T5=0;goto _TLC8;_TLC7: _T6=r;_T7=f;_T8=left;_T5=Cyc_Dict_map_tree(_T6,_T7,_T8);_TLC8: {const struct Cyc_Dict_T*new_left=_T5;
void*new_val=f(val);
if(right!=0)goto _TLC9;_T9=0;goto _TLCA;_TLC9: _TA=r;_TB=f;_TC=right;_T9=Cyc_Dict_map_tree(_TA,_TB,_TC);_TLCA: {const struct Cyc_Dict_T*new_right=_T9;_TE=r;{struct Cyc_Dict_T*_T14=_region_malloc(_TE,0U,sizeof(struct Cyc_Dict_T));
_T14->color=c;_T14->left=new_left;_T14->right=new_right;_T14->key_val.f0=key;_T14->key_val.f1=new_val;_TD=(struct Cyc_Dict_T*)_T14;}return _TD;}}}}
# 335
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;struct Cyc_Dict_Dict _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Dict_Dict _T4;struct _RegionHandle*_T5;void*(*_T6)(void*);const struct Cyc_Dict_T*_T7;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t!=0)goto _TLCB;{struct Cyc_Dict_Dict _T8;_T2=d;
_T8.rel=_T2.rel;_T8.r=r;_T8.t=0;_T1=_T8;}return _T1;_TLCB:{struct Cyc_Dict_Dict _T8;_T4=d;
_T8.rel=_T4.rel;_T8.r=r;_T5=r;_T6=f;_T7=t;_T8.t=Cyc_Dict_map_tree(_T5,_T6,_T7);_T3=_T8;}return _T3;}}
# 342
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){struct _RegionHandle*_T0;void*(*_T1)(void*);struct Cyc_Dict_Dict _T2;struct Cyc_Dict_Dict _T3;_T0=Cyc_Core_heap_region;_T1=f;_T2=d;_T3=
Cyc_Dict_rmap(_T0,_T1,_T2);return _T3;}
# 346
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;const struct Cyc_Dict_T*_T5;struct _RegionHandle*_T6;void*(*_T7)(void*,void*);void*_T8;const struct Cyc_Dict_T*_T9;const struct Cyc_Dict_T*_TA;struct _RegionHandle*_TB;void*(*_TC)(void*,void*);void*_TD;const struct Cyc_Dict_T*_TE;struct Cyc_Dict_T*_TF;struct _RegionHandle*_T10;void*_T11;void*_T12;const struct Cyc_Dict_T*_T13;const struct Cyc_Dict_T*_T14;enum Cyc_Dict_Color _T15;_T0=t;{struct Cyc_Dict_T _T16=*_T0;_T15=_T16.color;_T14=_T16.left;_T13=_T16.right;_T1=_T16.key_val;_T2=_T1.f0;_T12=(void*)_T2;_T3=_T16.key_val;_T4=_T3.f1;_T11=(void*)_T4;}{enum Cyc_Dict_Color c=_T15;const struct Cyc_Dict_T*left=_T14;const struct Cyc_Dict_T*right=_T13;void*key=_T12;void*val=_T11;
# 350
if(left!=0)goto _TLCD;_T5=0;goto _TLCE;_TLCD: _T6=r;_T7=f;_T8=env;_T9=left;_T5=Cyc_Dict_map_tree_c(_T6,_T7,_T8,_T9);_TLCE: {
# 349
const struct Cyc_Dict_T*new_left=_T5;
# 351
void*new_val=f(env,val);
# 353
if(right!=0)goto _TLCF;_TA=0;goto _TLD0;_TLCF: _TB=r;_TC=f;_TD=env;_TE=right;_TA=Cyc_Dict_map_tree_c(_TB,_TC,_TD,_TE);_TLD0: {
# 352
const struct Cyc_Dict_T*new_right=_TA;_T10=r;{struct Cyc_Dict_T*_T16=_region_malloc(_T10,0U,sizeof(struct Cyc_Dict_T));
# 354
_T16->color=c;_T16->left=new_left;_T16->right=new_right;_T16->key_val.f0=key;_T16->key_val.f1=new_val;_TF=(struct Cyc_Dict_T*)_T16;}return _TF;}}}}
# 357
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;struct Cyc_Dict_Dict _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Dict_Dict _T4;struct _RegionHandle*_T5;void*(*_T6)(void*,void*);void*_T7;const struct Cyc_Dict_T*_T8;_T0=d;{
# 359
const struct Cyc_Dict_T*t=_T0.t;
if(t!=0)goto _TLD1;{struct Cyc_Dict_Dict _T9;_T2=d;
_T9.rel=_T2.rel;_T9.r=r;_T9.t=0;_T1=_T9;}return _T1;_TLD1:{struct Cyc_Dict_Dict _T9;_T4=d;
_T9.rel=_T4.rel;_T9.r=r;_T5=r;_T6=f;_T7=env;_T8=t;_T9.t=Cyc_Dict_map_tree_c(_T5,_T6,_T7,_T8);_T3=_T9;}return _T3;}}
# 365
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){struct _RegionHandle*_T0;void*(*_T1)(void*,void*);void*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_Dict_Dict _T4;_T0=Cyc_Core_heap_region;_T1=f;_T2=env;_T3=d;_T4=
Cyc_Dict_rmap_c(_T0,_T1,_T2,_T3);return _T4;}
# 369
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;const struct Cyc_Dict_T*_T1;struct Cyc_Dict_Absent_exn_struct*_T2;struct Cyc_Dict_Absent_exn_struct*_T3;struct _tuple0*_T4;struct _RegionHandle*_T5;struct Cyc_Dict_Dict _T6;const struct Cyc_Dict_T*_T7;struct _tuple0 _T8;struct Cyc_Dict_Dict _T9;const struct Cyc_Dict_T*_TA;struct _tuple0 _TB;_T0=d;_T1=_T0.t;
if(_T1!=0)goto _TLD3;_T2=& Cyc_Dict_Absent_val;_T3=(struct Cyc_Dict_Absent_exn_struct*)_T2;_throw(_T3);goto _TLD4;_TLD3: _TLD4: _T5=r;{struct _tuple0*_TC=_region_malloc(_T5,0U,sizeof(struct _tuple0));_T6=d;_T7=_T6.t;_T8=_T7->key_val;
# 372
_TC->f0=_T8.f0;_T9=d;_TA=_T9.t;_TB=_TA->key_val;_TC->f1=_TB.f1;_T4=(struct _tuple0*)_TC;}return _T4;}
# 375
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){const struct Cyc_Dict_T*_T0;struct _tuple0 _T1;void*_T2;struct _tuple0 _T3;void*_T4;int _T5;int(*_T6)(void*,void*,void*);void*_T7;const struct Cyc_Dict_T*_T8;int _T9;int _TA;int _TB;int(*_TC)(void*,void*,void*);void*_TD;const struct Cyc_Dict_T*_TE;void*_TF;void*_T10;const struct Cyc_Dict_T*_T11;const struct Cyc_Dict_T*_T12;_T0=t;{struct Cyc_Dict_T _T13=*_T0;_T12=_T13.left;_T11=_T13.right;_T1=_T13.key_val;_T2=_T1.f0;_T10=(void*)_T2;_T3=_T13.key_val;_T4=_T3.f1;_TF=(void*)_T4;}{const struct Cyc_Dict_T*left=_T12;const struct Cyc_Dict_T*right=_T11;void*key=_T10;void*val=_TF;
# 378
if(left==0)goto _TLD7;else{goto _TLD8;}_TLD8: _T6=f;_T7=env;_T8=left;_T9=Cyc_Dict_forall_tree_c(_T6,_T7,_T8);if(_T9)goto _TLD7;else{goto _TLD5;}_TLD7: _TA=
f(env,key,val);
# 378
if(!_TA)goto _TLD5;
# 380
if(right!=0)goto _TLD9;_TB=1;goto _TLDA;_TLD9: _TC=f;_TD=env;_TE=right;_TB=Cyc_Dict_forall_tree_c(_TC,_TD,_TE);_TLDA: _T5=_TB;goto _TLD6;_TLD5: _T5=0;_TLD6:
# 377
 return _T5;}}
# 383
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;int(*_T1)(void*,void*,void*);void*_T2;const struct Cyc_Dict_T*_T3;int _T4;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
if(t!=0)goto _TLDB;
return 1;_TLDB: _T1=f;_T2=env;_T3=t;_T4=
Cyc_Dict_forall_tree_c(_T1,_T2,_T3);return _T4;}}struct _tuple5{int(*f0)(void*,void*,void*);struct Cyc_Dict_Dict f1;};
# 390
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){struct _tuple5*_T0;int _T1;void*_T2;void*_T3;void*_T4;int _T5;struct Cyc_Dict_Dict _T6;int(*_T7)(void*,void*,void*);_T0=env;{struct _tuple5 _T8=*_T0;_T7=_T8.f0;_T6=_T8.f1;}{int(*f)(void*,void*,void*)=_T7;struct Cyc_Dict_Dict d2=_T6;_T1=
# 393
Cyc_Dict_member(d2,a);if(!_T1)goto _TLDD;_T2=a;_T3=b;_T4=
Cyc_Dict_lookup(d2,a);_T5=f(_T2,_T3,_T4);return _T5;_TLDD:
 return 1;}}
# 399
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct _tuple5 _T0;int(*_T1)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict);struct _tuple5*_T2;struct _tuple5*_T3;struct Cyc_Dict_Dict _T4;int _T5;{struct _tuple5 _T6;
_T6.f0=f;_T6.f1=d2;_T0=_T6;}{struct _tuple5 env=_T0;{
int(*_T6)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict)=(int(*)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict))Cyc_Dict_forall_c;_T1=_T6;}_T2=& env;_T3=(struct _tuple5*)_T2;_T4=d1;_T5=_T1(Cyc_Dict_forall_intersect_f,_T3,_T4);return _T5;}}struct _tuple6{void*(*f0)(void*,void*,void*,void*);void*f1;};
# 405
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){struct Cyc_Dict_Dict*_T0;struct Cyc_Dict_Dict _T1;void*_T2;int _T3;struct Cyc_Dict_Dict*_T4;struct Cyc_Dict_Dict _T5;void*_T6;struct _tuple6*_T7;struct _tuple6 _T8;void*(*_T9)(void*,void*,void*,void*);struct _tuple6*_TA;struct _tuple6 _TB;void*_TC;void*_TD;void*_TE;void*_TF;struct Cyc_Dict_Dict*_T10;struct Cyc_Dict_Dict*_T11;struct Cyc_Dict_Dict _T12;void*_T13;void*_T14;struct Cyc_Dict_Dict*_T15;struct Cyc_Dict_Dict*_T16;struct Cyc_Dict_Dict*_T17;struct Cyc_Dict_Dict _T18;void*_T19;void*_T1A;struct Cyc_Dict_Dict*_T1B;_T0=d1;_T1=*_T0;_T2=a;_T3=
# 409
Cyc_Dict_member(_T1,_T2);if(!_T3)goto _TLDF;_T4=d1;_T5=*_T4;_T6=a;{
# 411
void*old_val=Cyc_Dict_lookup(_T5,_T6);_T7=env;_T8=*_T7;_T9=_T8.f0;_TA=env;_TB=*_TA;_TC=_TB.f1;_TD=a;_TE=old_val;_TF=b;{
void*new_val=_T9(_TC,_TD,_TE,_TF);
if(new_val==old_val)goto _TLE1;_T10=d1;_T11=d1;_T12=*_T11;_T13=a;_T14=new_val;
*_T10=Cyc_Dict_insert(_T12,_T13,_T14);goto _TLE2;_TLE1: _TLE2: _T15=d1;
return _T15;}}_TLDF: _T16=d1;_T17=d1;_T18=*_T17;_T19=a;_T1A=b;
# 417
*_T16=Cyc_Dict_insert(_T18,_T19,_T1A);_T1B=d1;
return _T1B;}
# 421
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct Cyc_Dict_Dict _T0;const struct Cyc_Dict_T*_T1;int _T2;struct Cyc_Dict_Dict _T3;const struct Cyc_Dict_T*_T4;int _T5;struct Cyc_Dict_Dict _T6;struct Cyc_Dict_Dict _T7;const struct Cyc_Dict_T*_T8;struct Cyc_Dict_Dict _T9;struct Cyc_Dict_Dict _TA;const struct Cyc_Dict_T*_TB;struct Cyc_Dict_Dict _TC;struct _tuple6 _TD;struct Cyc_Dict_Dict*(*_TE)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*);struct _tuple6*_TF;struct Cyc_Dict_Dict _T10;struct Cyc_Dict_Dict*_T11;struct Cyc_Dict_Dict*_T12;struct Cyc_Dict_Dict _T13;_T0=d1;_T1=_T0.t;_T2=(int)_T1;_T3=d2;_T4=_T3.t;_T5=(int)_T4;
# 424
if(_T2!=_T5)goto _TLE3;_T6=d1;return _T6;_TLE3: _T7=d1;_T8=_T7.t;
if(_T8!=0)goto _TLE5;_T9=d2;return _T9;_TLE5: _TA=d2;_TB=_TA.t;
if(_TB!=0)goto _TLE7;_TC=d1;return _TC;_TLE7:{struct _tuple6 _T14;
_T14.f0=f;_T14.f1=env;_TD=_T14;}{struct _tuple6 fenvpair=_TD;{
struct Cyc_Dict_Dict*(*_T14)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_TE=_T14;}_TF=& fenvpair;_T10=d2;_T11=& d1;_T12=(struct Cyc_Dict_Dict*)_T11;_TE(Cyc_Dict_union_f,_TF,_T10,_T12);_T13=d1;
return _T13;}}
# 432
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct Cyc_Dict_Dict _T0;struct Cyc_Dict_Dict _T1;struct Cyc_Dict_Dict _T2;const struct Cyc_Dict_T*_T3;int _T4;const struct Cyc_Dict_T*_T5;int _T6;struct Cyc_Dict_Dict _T7;const struct Cyc_Dict_T**_T8;const struct Cyc_Dict_T**_T9;struct _RegionHandle*_TA;unsigned _TB;unsigned _TC;int _TD;int _TE;struct _fat_ptr _TF;int _T10;int _T11;char*_T12;const struct Cyc_Dict_T**_T13;struct _tuple0 _T14;void*_T15;struct _tuple0 _T16;void*_T17;int _T18;unsigned _T19;struct _fat_ptr _T1A;unsigned _T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;unsigned _T1E;const struct Cyc_Dict_T**_T1F;struct _RegionHandle*_T20;unsigned _T21;struct _fat_ptr _T22;unsigned _T23;unsigned _T24;struct _fat_ptr _T25;unsigned _T26;unsigned _T27;struct _fat_ptr _T28;unsigned char*_T29;const struct Cyc_Dict_T**_T2A;unsigned _T2B;int _T2C;unsigned _T2D;struct _fat_ptr _T2E;int _T2F;char*_T30;const struct Cyc_Dict_T**_T31;struct _fat_ptr _T32;int _T33;char*_T34;const struct Cyc_Dict_T**_T35;int _T36;struct Cyc_Dict_Dict _T37;struct _RegionHandle*_T38;struct Cyc_Dict_Dict _T39;int(*_T3A)(void*,void*);struct _tuple0 _T3B;void*_T3C;void*_T3D;void*_T3E;void*_T3F;const struct Cyc_Dict_T*_T40;struct Cyc_Dict_Dict _T41;struct Cyc_Dict_Dict _T42;struct Cyc_Dict_Dict _T43;_T0=d2;{
# 435
const struct Cyc_Dict_T*t2=_T0.t;
if(t2!=0)goto _TLE9;_T1=d2;return _T1;_TLE9: _T2=d1;_T3=_T2.t;_T4=(int)_T3;_T5=t2;_T6=(int)_T5;
if(_T4!=_T6)goto _TLEB;_T7=d2;return _T7;_TLEB: {
const struct Cyc_Dict_T*ans_tree=0;struct _RegionHandle _T44=_new_region(0U,"temp");struct _RegionHandle*temp=& _T44;_push_region(temp);{unsigned _T45=16U;_TA=temp;_TB=_check_times(_T45,sizeof(const struct Cyc_Dict_T*));{const struct Cyc_Dict_T**_T46=_region_malloc(_TA,0U,_TB);{unsigned _T47=16U;unsigned i;i=0;_TLF0: if(i < _T47)goto _TLEE;else{goto _TLEF;}_TLEE: _TC=i;
# 441
_T46[_TC]=t2;i=i + 1;goto _TLF0;_TLEF:;}_T9=(const struct Cyc_Dict_T**)_T46;}_T8=_T9;}{struct _fat_ptr queue=_tag_fat(_T8,sizeof(const struct Cyc_Dict_T*),16U);
int ind=0;
_TLF1: _TD=ind;_TE=- 1;if(_TD!=_TE)goto _TLF2;else{goto _TLF3;}
_TLF2: _TF=queue;_T10=ind;ind=_T10 + -1;_T11=_T10;_T12=_check_fat_subscript(_TF,sizeof(const struct Cyc_Dict_T*),_T11);_T13=(const struct Cyc_Dict_T**)_T12;{const struct Cyc_Dict_T*_T45=*_T13;void*_T46;void*_T47;const struct Cyc_Dict_T*_T48;const struct Cyc_Dict_T*_T49;{struct Cyc_Dict_T _T4A=*_T45;_T49=_T4A.left;_T48=_T4A.right;_T14=_T4A.key_val;_T15=_T14.f0;_T47=(void*)_T15;_T16=_T4A.key_val;_T17=_T16.f1;_T46=(void*)_T17;}{const struct Cyc_Dict_T*l=_T49;const struct Cyc_Dict_T*r=_T48;void*k=_T47;void*v=_T46;_T18=ind + 2;_T19=(unsigned)_T18;_T1A=queue;_T1B=
_get_fat_size(_T1A,sizeof(const struct Cyc_Dict_T*));if(_T19 < _T1B)goto _TLF4;_T1D=queue;_T1E=
_get_fat_size(_T1D,sizeof(const struct Cyc_Dict_T*));{unsigned _T4A=_T1E * 2U;_T20=temp;_T21=_check_times(_T4A,sizeof(const struct Cyc_Dict_T*));{const struct Cyc_Dict_T**_T4B=_region_malloc(_T20,0U,_T21);_T22=queue;_T23=_get_fat_size(_T22,sizeof(const struct Cyc_Dict_T*));{unsigned _T4C=_T23 * 2U;unsigned i;i=0;_TLF9: if(i < _T4C)goto _TLF7;else{goto _TLF8;}_TLF7: _T24=i;_T25=queue;_T26=
_get_fat_size(_T25,sizeof(const struct Cyc_Dict_T*));if(_T24 >= _T26)goto _TLFA;_T27=i;_T28=queue;_T29=_T28.curr;_T2A=(const struct Cyc_Dict_T**)_T29;_T2B=i;_T2C=(int)_T2B;_T4B[_T27]=_T2A[_T2C];goto _TLFB;_TLFA: _T2D=i;_T4B[_T2D]=t2;_TLFB: i=i + 1;goto _TLF9;_TLF8:;}_T1F=(const struct Cyc_Dict_T**)_T4B;}_T1C=
# 446
_tag_fat(_T1F,sizeof(const struct Cyc_Dict_T*),_T4A);}queue=_T1C;goto _TLF5;_TLF4: _TLF5:
# 448
 if(l==0)goto _TLFC;_T2E=queue;ind=ind + 1;_T2F=ind;_T30=_check_fat_subscript(_T2E,sizeof(const struct Cyc_Dict_T*),_T2F);_T31=(const struct Cyc_Dict_T**)_T30;*_T31=l;goto _TLFD;_TLFC: _TLFD:
 if(r==0)goto _TLFE;_T32=queue;ind=ind + 1;_T33=ind;_T34=_check_fat_subscript(_T32,sizeof(const struct Cyc_Dict_T*),_T33);_T35=(const struct Cyc_Dict_T**)_T34;*_T35=r;goto _TLFF;_TLFE: _TLFF: _T36=
Cyc_Dict_member(d1,k);if(!_T36)goto _TL100;_T37=d2;_T38=_T37.r;_T39=d2;_T3A=_T39.rel;{struct _tuple0 _T4A;
_T4A.f0=k;_T3C=env;_T3D=k;_T3E=Cyc_Dict_lookup(d1,k);_T3F=v;_T4A.f1=f(_T3C,_T3D,_T3E,_T3F);_T3B=_T4A;}_T40=ans_tree;ans_tree=Cyc_Dict_ins(_T38,_T3A,_T3B,_T40);goto _TL101;_TL100: _TL101:;}}goto _TLF1;_TLF3:;}{struct Cyc_Dict_Dict _T45;_T42=d2;
# 454
_T45.rel=_T42.rel;_T43=d2;_T45.r=_T43.r;_T45.t=ans_tree;_T41=_T45;}{struct Cyc_Dict_Dict _T45=_T41;_npop_handler(0);return _T45;}_pop_region();}}}
# 457
static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,void*b1,void*b2){void*_T0;_T0=
f(a,b1,b2);return _T0;}
# 461
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct Cyc_Dict_Dict(*_T0)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);void*(*_T1)(void*,void*,void*);struct Cyc_Dict_Dict _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Dict_Dict _T4;{
# 464
struct Cyc_Dict_Dict(*_T5)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;_T0=_T5;}_T1=f;_T2=d1;_T3=d2;_T4=_T0(Cyc_Dict_intersect_f,_T1,_T2,_T3);return _T4;}
# 467
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;struct _tuple0*_T2;struct _RegionHandle*_T3;_T1=r;{struct Cyc_List_List*_T4=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));_T3=r;{struct _tuple0*_T5=_region_malloc(_T3,0U,sizeof(struct _tuple0));
# 469
_T5->f0=k;_T5->f1=v;_T2=(struct _tuple0*)_T5;}_T4->hd=_T2;_T4->tl=accum;_T0=(struct Cyc_List_List*)_T4;}return _T0;}
# 472
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*);struct _RegionHandle*_T1;struct Cyc_Dict_Dict _T2;struct Cyc_List_List*_T3;{
struct Cyc_List_List*(*_T4)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold_c;_T0=_T4;}_T1=r;_T2=d;_T3=_T0(Cyc_Dict_to_list_f,_T1,_T2,0);return _T3;}
# 475
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){struct Cyc_List_List*_T0;_T0=
Cyc_Dict_rto_list(Cyc_Core_heap_region,d);return _T0;}struct _tuple7{int(*f0)(void*,void*);struct _RegionHandle*f1;};
# 479
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple7*_T0;int _T1;struct Cyc_Dict_Dict*_T2;struct Cyc_Dict_Dict*_T3;struct Cyc_Dict_Dict _T4;void*_T5;void*_T6;struct Cyc_Dict_Dict*_T7;struct _RegionHandle*_T8;int(*_T9)(void*,void*);_T0=env;{struct _tuple7 _TA=*_T0;_T9=_TA.f0;_T8=_TA.f1;}{int(*f)(void*,void*)=_T9;struct _RegionHandle*r=_T8;_T1=
# 482
f(x,y);if(!_T1)goto _TL102;_T2=acc;_T3=acc;_T4=*_T3;_T5=x;_T6=y;
*_T2=Cyc_Dict_insert(_T4,_T5,_T6);goto _TL103;_TL102: _TL103: _T7=acc;
# 485
return _T7;}}
# 488
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict d){struct _tuple7 _T0;struct _RegionHandle*_T1;struct Cyc_Dict_Dict _T2;int(*_T3)(void*,void*);struct Cyc_Dict_Dict*(*_T4)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*);struct _tuple7*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_Dict_Dict*_T7;struct Cyc_Dict_Dict*_T8;struct Cyc_Dict_Dict*_T9;struct Cyc_Dict_Dict _TA;{struct _tuple7 _TB;
_TB.f0=f;_TB.f1=r2;_T0=_TB;}{struct _tuple7 env=_T0;_T1=r2;_T2=d;_T3=_T2.rel;{
struct Cyc_Dict_Dict res=Cyc_Dict_rempty(_T1,_T3);{
struct Cyc_Dict_Dict*(*_TB)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_T4=_TB;}_T5=& env;_T6=d;_T7=& res;_T8=(struct Cyc_Dict_Dict*)_T7;_T9=_T4(Cyc_Dict_filter_f,_T5,_T6,_T8);_TA=*_T9;return _TA;}}}
# 499
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){struct _RegionHandle*_T0;int(*_T1)(void*,void*);struct Cyc_Dict_Dict _T2;struct Cyc_Dict_Dict _T3;_T0=Cyc_Core_heap_region;_T1=f;_T2=d;_T3=
Cyc_Dict_rfilter(_T0,_T1,_T2);return _T3;}struct _tuple8{int(*f0)(void*,void*,void*);void*f1;struct _RegionHandle*f2;};
# 504
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple8*_T0;void*_T1;int _T2;struct Cyc_Dict_Dict*_T3;struct Cyc_Dict_Dict*_T4;struct Cyc_Dict_Dict _T5;void*_T6;void*_T7;struct Cyc_Dict_Dict*_T8;struct _RegionHandle*_T9;void*_TA;int(*_TB)(void*,void*,void*);_T0=env;{struct _tuple8 _TC=*_T0;_TB=_TC.f0;_T1=_TC.f1;_TA=(void*)_T1;_T9=_TC.f2;}{int(*f)(void*,void*,void*)=_TB;void*f_env=_TA;struct _RegionHandle*r2=_T9;_T2=
# 508
f(f_env,x,y);if(!_T2)goto _TL104;_T3=acc;_T4=acc;_T5=*_T4;_T6=x;_T7=y;
*_T3=Cyc_Dict_insert(_T5,_T6,_T7);goto _TL105;_TL104: _TL105: _T8=acc;
return _T8;}}
# 513
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){struct _tuple8 _T0;struct _RegionHandle*_T1;struct Cyc_Dict_Dict _T2;int(*_T3)(void*,void*);struct Cyc_Dict_Dict*(*_T4)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*);struct _tuple8*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_Dict_Dict*_T7;struct Cyc_Dict_Dict*_T8;struct Cyc_Dict_Dict*_T9;struct Cyc_Dict_Dict _TA;{struct _tuple8 _TB;
# 515
_TB.f0=f;_TB.f1=f_env;_TB.f2=r2;_T0=_TB;}{struct _tuple8 env=_T0;_T1=r2;_T2=d;_T3=_T2.rel;{
struct Cyc_Dict_Dict res=Cyc_Dict_rempty(_T1,_T3);{
struct Cyc_Dict_Dict*(*_TB)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_T4=_TB;}_T5=& env;_T6=d;_T7=& res;_T8=(struct Cyc_Dict_Dict*)_T7;_T9=_T4(Cyc_Dict_filter_c_f,_T5,_T6,_T8);_TA=*_T9;return _TA;}}}
# 519
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){struct _RegionHandle*_T0;int(*_T1)(void*,void*,void*);void*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_Dict_Dict _T4;_T0=Cyc_Core_heap_region;_T1=f;_T2=f_env;_T3=d;_T4=
# 521
Cyc_Dict_rfilter_c(_T0,_T1,_T2,_T3);return _T4;}
# 525
static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){struct Cyc_Dict_Dict*_T0;struct Cyc_Dict_Dict _T1;void*_T2;int _T3;int _T4;_T0=d;_T1=*_T0;_T2=x;_T3=
Cyc_Dict_member(_T1,_T2);_T4=!_T3;return _T4;}
# 529
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*r2,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct Cyc_Dict_Dict(*_T0)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict);struct _RegionHandle*_T1;struct Cyc_Dict_Dict*_T2;struct Cyc_Dict_Dict*_T3;struct Cyc_Dict_Dict _T4;struct Cyc_Dict_Dict _T5;{
# 531
struct Cyc_Dict_Dict(*_T6)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;_T0=_T6;}_T1=r2;_T2=& d2;_T3=(struct Cyc_Dict_Dict*)_T2;_T4=d1;_T5=_T0(_T1,Cyc_Dict_difference_f,_T3,_T4);return _T5;}
# 533
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){struct Cyc_Dict_Dict _T0;_T0=
Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);return _T0;}struct _tuple9{int(*f0)(void*,void*);void*f1;};
# 537
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){struct _tuple9*_T0;void*_T1;int _T2;int _T3;void*_T4;int(*_T5)(void*,void*);_T0=env;{struct _tuple9 _T6=*_T0;_T5=_T6.f0;_T1=_T6.f1;_T4=(void*)_T1;}{int(*rel)(void*,void*)=_T5;void*x0=_T4;_T2=
# 539
rel(x0,x);_T3=_T2!=0;return _T3;}}
# 542
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){int _T0;struct Cyc_Dict_Dict _T1;struct _tuple9 _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Dict_Dict(*_T4)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict);struct _RegionHandle*_T5;struct _tuple9*_T6;struct Cyc_Dict_Dict _T7;struct Cyc_Dict_Dict _T8;_T0=
Cyc_Dict_member(d,x);if(_T0)goto _TL106;else{goto _TL108;}_TL108: _T1=Cyc_Dict_rcopy(r2,d);return _T1;_TL106:{struct _tuple9 _T9;_T3=d;
_T9.f0=_T3.rel;_T9.f1=x;_T2=_T9;}{struct _tuple9 env=_T2;{
struct Cyc_Dict_Dict(*_T9)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;_T4=_T9;}_T5=r2;_T6=& env;_T7=d;_T8=_T4(_T5,Cyc_Dict_delete_f,_T6,_T7);return _T8;}}
# 548
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){int _T0;struct Cyc_Dict_Dict _T1;struct _tuple9 _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Dict_Dict(*_T4)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict _T5;struct _RegionHandle*_T6;struct _tuple9*_T7;struct Cyc_Dict_Dict _T8;struct Cyc_Dict_Dict _T9;_T0=
Cyc_Dict_member(d,x);if(_T0)goto _TL109;else{goto _TL10B;}_TL10B: _T1=d;return _T1;_TL109:{struct _tuple9 _TA;_T3=d;
_TA.f0=_T3.rel;_TA.f1=x;_T2=_TA;}{struct _tuple9 env=_T2;{
struct Cyc_Dict_Dict(*_TA)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;_T4=_TA;}_T5=d;_T6=_T5.r;_T7=& env;_T8=d;_T9=_T4(_T6,Cyc_Dict_delete_f,_T7,_T8);return _T9;}}
# 554
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){struct Cyc_Dict_Dict _T0;_T0=
Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);return _T0;}struct _tuple10{struct _fat_ptr f0;int f1;};
# 558
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){struct _tuple10*_T0;struct _tuple10*_T1;int*_T2;int*_T3;int _T4;int _T5;struct _fat_ptr _T6;int _T7;char*_T8;const struct Cyc_Dict_T**_T9;struct _tuple0*_TA;const struct Cyc_Dict_T*_TB;const struct Cyc_Dict_T*_TC;const struct Cyc_Dict_T*_TD;unsigned _TE;struct _fat_ptr _TF;unsigned char*_T10;const struct Cyc_Dict_T**_T11;int _T12;const struct Cyc_Dict_T*_T13;const struct Cyc_Dict_T*_T14;const struct Cyc_Dict_T*_T15;unsigned _T16;struct _fat_ptr _T17;int _T18;char*_T19;const struct Cyc_Dict_T**_T1A;const struct Cyc_Dict_T*_T1B;int*_T1C;void*_T1D;struct _fat_ptr _T1E;_T0=stk;{struct _tuple10 _T1F=*_T0;_T1E=_T1F.f0;_T1=stk;_T2=& _T1->f1;_T1D=(int*)_T2;}{struct _fat_ptr stack=_T1E;int*indp=(int*)_T1D;_T3=indp;{
# 562
int ind=*_T3;_T4=ind;_T5=- 1;
if(_T4!=_T5)goto _TL10C;
return 0;_TL10C: _T6=stack;_T7=ind;_T8=_check_fat_subscript(_T6,sizeof(const struct Cyc_Dict_T*),_T7);_T9=(const struct Cyc_Dict_T**)_T8;{
const struct Cyc_Dict_T*t=*_T9;_TA=dest;_TB=
_check_null(t);*_TA=_TB->key_val;
ind=ind + -1;_TC=t;_TD=_TC->left;_TE=(unsigned)_TD;
if(!_TE)goto _TL10E;_TF=stack;_T10=_TF.curr;_T11=(const struct Cyc_Dict_T**)_T10;
ind=ind + 1;_T12=ind;_T13=t;_T11[_T12]=_T13->left;goto _TL10F;_TL10E: _TL10F: _T14=t;_T15=_T14->right;_T16=(unsigned)_T15;
if(!_T16)goto _TL110;_T17=stack;
ind=ind + 1;_T18=ind;_T19=_check_fat_subscript(_T17,sizeof(const struct Cyc_Dict_T*),_T18);_T1A=(const struct Cyc_Dict_T**)_T19;_T1B=t;*_T1A=_T1B->right;goto _TL111;_TL110: _TL111: _T1C=indp;
*_T1C=ind;
return 1;}}}}
# 578
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;const struct Cyc_Dict_T*_T1;struct Cyc_Dict_Dict _T2;struct _fat_ptr _T3;int _T4;const struct Cyc_Dict_T**_T5;struct _RegionHandle*_T6;unsigned _T7;int _T8;unsigned _T9;struct Cyc_Iter_Iter _TA;struct _tuple10*_TB;struct _RegionHandle*_TC;const struct Cyc_Dict_T*_TD;unsigned _TE;
# 581
int half_max_size=1;_T0=d;{
const struct Cyc_Dict_T*t=_T0.t;
_TL112: if(t!=0)goto _TL113;else{goto _TL114;}
_TL113: _T1=t;t=_T1->left;
half_max_size=half_max_size + 1;goto _TL112;_TL114: _T2=d;
# 587
t=_T2.t;_T4=2 * half_max_size;{unsigned _TF=(unsigned)_T4;_T6=rgn;_T7=_check_times(_TF,sizeof(const struct Cyc_Dict_T*));{const struct Cyc_Dict_T**_T10=_region_malloc(_T6,0U,_T7);_T8=2 * half_max_size;{unsigned _T11=(unsigned)_T8;unsigned i;i=0;_TL118: if(i < _T11)goto _TL116;else{goto _TL117;}_TL116: _T9=i;
_T10[_T9]=t;i=i + 1;goto _TL118;_TL117:;}_T5=(const struct Cyc_Dict_T**)_T10;}_T3=_tag_fat(_T5,sizeof(const struct Cyc_Dict_T*),_TF);}{struct _fat_ptr stack=_T3;{struct Cyc_Iter_Iter _TF;_TC=rgn;{struct _tuple10*_T10=_region_malloc(_TC,0U,sizeof(struct _tuple10));
_T10->f0=stack;_TD=t;_TE=(unsigned)_TD;if(!_TE)goto _TL119;_T10->f1=0;goto _TL11A;_TL119: _T10->f1=- 1;_TL11A: _TB=(struct _tuple10*)_T10;}_TF.env=_TB;
_TF.next=Cyc_Dict_iter_f;_TA=_TF;}
# 589
return _TA;}}}
# 597
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){struct Cyc_Core_Failure_exn_struct*_T0;void*_T1;void*(*_T2)(void*,struct Cyc___cycFILE*,struct _tuple0*);void*(*_T3)(void*,struct Cyc___cycFILE*,void*);void*_T4;struct Cyc___cycFILE*_T5;struct Cyc_List_List*_T6;void*_T7;struct _tuple0*_T8;void*(*_T9)(void*,struct Cyc___cycFILE*,struct _tuple0*);void*(*_TA)(void*,struct Cyc___cycFILE*,void*);void*_TB;struct Cyc___cycFILE*_TC;struct Cyc_List_List*_TD;void*_TE;struct _tuple0*_TF;struct Cyc_List_List*_T10;void*_T11;
# 604
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
int len=Cyc_List_length(dict_list);{struct Cyc_Core_Failure_exn_struct*_T12=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_T12->tag=Cyc_Core_Failure;
# 608
_T12->f1=_tag_fat("Dict::marshal: Write failure",sizeof(char),29U);_T0=(struct Cyc_Core_Failure_exn_struct*)_T12;}_T1=(void*)_T0;_throw(_T1);
# 610
_TL11B: if(dict_list!=0)goto _TL11C;else{goto _TL11D;}
_TL11C: _T3=write_key;{void*(*_T12)(void*,struct Cyc___cycFILE*,struct _tuple0*)=(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))_T3;_T2=_T12;}_T4=env;_T5=fp;_T6=dict_list;_T7=_T6->hd;_T8=(struct _tuple0*)_T7;env=_T2(_T4,_T5,_T8);_TA=write_val;{
void*(*_T12)(void*,struct Cyc___cycFILE*,struct _tuple0*)=(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))_TA;_T9=_T12;}_TB=env;_TC=fp;_TD=dict_list;_TE=_TD->hd;_TF=(struct _tuple0*)_TE;env=_T9(_TB,_TC,_TF);_T10=dict_list;
dict_list=_T10->tl;goto _TL11B;_TL11D: _T11=env;
# 615
return _T11;}
# 618
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){int(*_T0)(void*,void*);int _T1;int _T2;struct Cyc_Core_Failure_exn_struct*_T3;void*_T4;struct Cyc_Dict_Dict _T5;_T0=cmp;{
# 625
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(_T0);
int len=Cyc_getw(fp);_T1=len;_T2=- 1;
if(_T1!=_T2)goto _TL11E;{struct Cyc_Core_Failure_exn_struct*_T6=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_T6->tag=Cyc_Core_Failure;
_T6->f1=_tag_fat("Dict::unmarshal: list length is -1",sizeof(char),35U);_T3=(struct Cyc_Core_Failure_exn_struct*)_T6;}_T4=(void*)_T3;_throw(_T4);goto _TL11F;_TL11E: _TL11F:{
int i=0;_TL123: if(i < len)goto _TL121;else{goto _TL122;}
_TL121:{void*key=read_key(env,fp);
void*val=read_val(env,fp);
dict=Cyc_Dict_insert(dict,key,val);}
# 629
i=i + 1;goto _TL123;_TL122:;}_T5=dict;
# 634
return _T5;}}

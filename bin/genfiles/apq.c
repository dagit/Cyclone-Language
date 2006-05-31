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
 struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
extern struct Cyc_AP_T*Cyc_AP_fromint(long);
extern struct Cyc_AP_T*Cyc_AP_fromstr(const char*,int);
# 13
extern char*Cyc_AP_tostr(struct Cyc_AP_T*,int);
extern struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*,struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*,struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*,struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 32
extern int Cyc_AP_cmp(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 34
extern struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*,struct Cyc_AP_T*);
extern struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*,struct Cyc_AP_T*);struct Cyc_APQ_T{struct Cyc_AP_T*n;struct Cyc_AP_T*d;};char Cyc_Invalid_argument[17U]="Invalid_argument";struct Cyc_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 13 "apq.cyc"
struct Cyc_APQ_T*Cyc_reduce(struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_AP_T*_T2;int _T3;struct Cyc_APQ_T*_T4;struct Cyc_APQ_T*_T5;struct Cyc_AP_T*_T6;struct Cyc_APQ_T*_T7;struct Cyc_APQ_T*_T8;struct Cyc_AP_T*_T9;struct Cyc_APQ_T*_TA;struct Cyc_AP_T*_TB;struct Cyc_AP_T*_TC;struct Cyc_APQ_T*_TD;struct Cyc_AP_T*_TE;int _TF;struct Cyc_APQ_T*_T10;void*_T11;struct Cyc_APQ_T*_T12;struct Cyc_APQ_T*_T13;struct Cyc_AP_T*_T14;struct Cyc_AP_T*_T15;struct Cyc_APQ_T*_T16;struct Cyc_APQ_T*_T17;struct Cyc_AP_T*_T18;struct Cyc_AP_T*_T19;struct Cyc_APQ_T*_T1A;_T0=
_check_null(q);_T1=_T0->d;_T2=Cyc_AP_zero;_T3=Cyc_AP_cmp(_T1,_T2);if(_T3 >= 0)goto _TL0;_T4=q;_T5=q;_T6=_T5->d;
_T4->d=Cyc_AP_neg(_T6);_T7=q;_T8=q;_T9=_T8->n;
_T7->n=Cyc_AP_neg(_T9);goto _TL1;_TL0: _TL1: _TA=q;_TB=_TA->n;_TC=
# 18
Cyc_AP_abs(_TB);_TD=q;_TE=_TD->d;{struct Cyc_AP_T*gcd=Cyc_AP_gcd(_TC,_TE);_TF=
Cyc_AP_cmp(gcd,Cyc_AP_one);if(_TF!=0)goto _TL2;_T10=q;
return _T10;_TL2: _T11=_cycalloc(sizeof(struct Cyc_APQ_T));{
struct Cyc_APQ_T*newq=(struct Cyc_APQ_T*)_T11;_T12=newq;_T13=q;_T14=_T13->n;_T15=gcd;
_T12->n=Cyc_AP_div(_T14,_T15);_T16=newq;_T17=q;_T18=_T17->d;_T19=gcd;
_T16->d=Cyc_AP_div(_T18,_T19);_T1A=newq;
return _T1A;}}}
# 27
struct Cyc_APQ_T*Cyc_APQ_fromint(int i){void*_T0;struct Cyc_APQ_T*_T1;struct Cyc_APQ_T*_T2;struct Cyc_APQ_T*_T3;_T0=_cycalloc(sizeof(struct Cyc_APQ_T));{
struct Cyc_APQ_T*q=(struct Cyc_APQ_T*)_T0;_T1=q;
_T1->n=Cyc_AP_fromint(i);_T2=q;
_T2->d=Cyc_AP_one;_T3=q;
return _T3;}}
# 34
struct Cyc_APQ_T*Cyc_APQ_fromAP(struct Cyc_AP_T*n,struct Cyc_AP_T*d){int _T0;struct Cyc_Invalid_argument_exn_struct*_T1;void*_T2;void*_T3;struct Cyc_APQ_T*_T4;struct Cyc_APQ_T*_T5;struct Cyc_APQ_T*_T6;_T0=
Cyc_AP_cmp(d,Cyc_AP_zero);if(_T0!=0)goto _TL4;{struct Cyc_Invalid_argument_exn_struct*_T7=_cycalloc(sizeof(struct Cyc_Invalid_argument_exn_struct));_T7->tag=Cyc_Invalid_argument;_T7->f1=_tag_fat("APQ_fromAP: divide by zero",sizeof(char),27U);_T1=(struct Cyc_Invalid_argument_exn_struct*)_T7;}_T2=(void*)_T1;_throw(_T2);goto _TL5;_TL4: _TL5: _T3=_cycalloc(sizeof(struct Cyc_APQ_T));{
struct Cyc_APQ_T*q=(struct Cyc_APQ_T*)_T3;_T4=q;
_T4->n=n;_T5=q;
_T5->d=d;_T6=
Cyc_reduce(q);return _T6;}}
# 42
struct Cyc_APQ_T*Cyc_APQ_fromstr(struct _fat_ptr str,int base){void*_T0;struct _fat_ptr _T1;char*_T2;const char*_T3;char _T4;int _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;char _T9;int _TA;struct _fat_ptr*_TB;struct Cyc_APQ_T*_TC;struct _fat_ptr _TD;char*_TE;const char*_TF;int _T10;struct _fat_ptr _T11;unsigned char*_T12;const char*_T13;char _T14;int _T15;struct _fat_ptr _T16;struct _fat_ptr _T17;char*_T18;const char*_T19;int _T1A;int _T1B;struct Cyc_APQ_T*_T1C;struct Cyc_Invalid_argument_exn_struct*_T1D;void*_T1E;struct Cyc_APQ_T*_T1F;struct Cyc_APQ_T*_T20;_T0=_cycalloc(sizeof(struct Cyc_APQ_T));{
struct Cyc_APQ_T*q=(struct Cyc_APQ_T*)_T0;
struct _fat_ptr s=str;
_TL6: _T1=s;_T2=_check_fat_subscript(_T1,sizeof(char),0U);_T3=(const char*)_T2;_T4=*_T3;_T5=(int)_T4;if(_T5)goto _TL9;else{goto _TL8;}_TL9: _T6=s;_T7=_T6.curr;_T8=(const char*)_T7;_T9=*_T8;_TA=(int)_T9;if(_TA!=47)goto _TL7;else{goto _TL8;}_TL7: _TB=& s;_fat_ptr_inplace_plus(_TB,sizeof(char),1);goto _TL6;_TL8: _TC=q;_TD=str;_TE=_untag_fat_ptr_check_bound(_TD,sizeof(char),1U);_TF=(const char*)_TE;_T10=base;
_TC->n=Cyc_AP_fromstr(_TF,_T10);_T11=s;_T12=_T11.curr;_T13=(const char*)_T12;_T14=*_T13;_T15=(int)_T14;
if(!_T15)goto _TLA;_T16=s;_T17=
_fat_ptr_plus(_T16,sizeof(char),1);_T18=_untag_fat_ptr_check_bound(_T17,sizeof(char),1U);_T19=(const char*)_T18;_T1A=base;{struct Cyc_AP_T*d=Cyc_AP_fromstr(_T19,_T1A);_T1B=
Cyc_AP_cmp(d,Cyc_AP_zero);if(!_T1B)goto _TLC;_T1C=q;
_T1C->d=d;goto _TLD;
_TLC:{struct Cyc_Invalid_argument_exn_struct*_T21=_cycalloc(sizeof(struct Cyc_Invalid_argument_exn_struct));_T21->tag=Cyc_Invalid_argument;_T21->f1=_tag_fat("APQ_fromstr: malformed string",sizeof(char),30U);_T1D=(struct Cyc_Invalid_argument_exn_struct*)_T21;}_T1E=(void*)_T1D;_throw(_T1E);_TLD:;}goto _TLB;
# 54
_TLA: _T1F=q;_T1F->d=Cyc_AP_one;_TLB: _T20=
Cyc_reduce(q);return _T20;}}
# 58
struct _fat_ptr Cyc_APQ_tostr(struct Cyc_APQ_T*q,int base){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_AP_T*_T2;int _T3;struct _fat_ptr _T4;struct Cyc_APQ_T*_T5;struct Cyc_AP_T*_T6;int _T7;void*_T8;void*_T9;unsigned _TA;struct _fat_ptr _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct _fat_ptr _TD;struct Cyc_APQ_T*_TE;struct Cyc_AP_T*_TF;int _T10;void*_T11;void*_T12;unsigned _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct _fat_ptr _T15;struct Cyc_APQ_T*_T16;struct Cyc_AP_T*_T17;int _T18;void*_T19;void*_T1A;unsigned _T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;_T0=
_check_null(q);_T1=_T0->d;_T2=Cyc_AP_one;_T3=Cyc_AP_cmp(_T1,_T2);if(_T3!=0)goto _TLE;_T5=q;_T6=_T5->n;_T7=base;{char*_T1E=
Cyc_AP_tostr(_T6,_T7);_T8=(void*)_T1E;_T9=(void*)_T1E;_TA=_get_zero_arr_size_char(_T9,1U);_T4=_tag_fat(_T8,sizeof(char),_TA);}return _T4;_TLE:{struct Cyc_String_pa_PrintArg_struct _T1E;_T1E.tag=0;_TE=q;_TF=_TE->n;_T10=base;{char*_T1F=
Cyc_AP_tostr(_TF,_T10);_T11=(void*)_T1F;_T12=(void*)_T1F;_T13=_get_zero_arr_size_char(_T12,1U);_TD=_tag_fat(_T11,sizeof(char),_T13);}_T1E.f1=_TD;_TC=_T1E;}{struct Cyc_String_pa_PrintArg_struct _T1E=_TC;{struct Cyc_String_pa_PrintArg_struct _T1F;_T1F.tag=0;_T16=q;_T17=_T16->d;_T18=base;{char*_T20=Cyc_AP_tostr(_T17,_T18);_T19=(void*)_T20;_T1A=(void*)_T20;_T1B=_get_zero_arr_size_char(_T1A,1U);_T15=_tag_fat(_T19,sizeof(char),_T1B);}_T1F.f1=_T15;_T14=_T1F;}{struct Cyc_String_pa_PrintArg_struct _T1F=_T14;void*_T20[2];_T20[0]=& _T1E;_T20[1]=& _T1F;_T1C=_tag_fat("%s/%s",sizeof(char),6U);_T1D=_tag_fat(_T20,sizeof(void*),2);_TB=Cyc_aprintf(_T1C,_T1D);}}return _TB;}
# 64
struct Cyc_APQ_T*Cyc_APQ_neg(struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_AP_T*_T2;struct Cyc_APQ_T*_T3;struct Cyc_AP_T*_T4;struct Cyc_APQ_T*_T5;_T0=
_check_null(q);_T1=_T0->n;_T2=Cyc_AP_neg(_T1);_T3=q;_T4=_T3->d;_T5=Cyc_APQ_fromAP(_T2,_T4);return _T5;}
# 68
struct Cyc_APQ_T*Cyc_APQ_abs(struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_AP_T*_T2;struct Cyc_APQ_T*_T3;struct Cyc_AP_T*_T4;struct Cyc_APQ_T*_T5;_T0=
_check_null(q);_T1=_T0->n;_T2=Cyc_AP_abs(_T1);_T3=q;_T4=_T3->d;_T5=Cyc_APQ_fromAP(_T2,_T4);return _T5;}
# 72
struct Cyc_APQ_T*Cyc_APQ_add(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_APQ_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;struct Cyc_APQ_T*_T5;struct Cyc_AP_T*_T6;struct Cyc_AP_T*_T7;struct Cyc_APQ_T*_T8;struct Cyc_AP_T*_T9;struct Cyc_APQ_T*_TA;struct Cyc_AP_T*_TB;struct Cyc_AP_T*_TC;struct Cyc_AP_T*_TD;struct Cyc_APQ_T*_TE;struct Cyc_AP_T*_TF;struct Cyc_AP_T*_T10;struct Cyc_AP_T*_T11;struct Cyc_AP_T*_T12;struct Cyc_AP_T*_T13;struct Cyc_APQ_T*_T14;_T0=
_check_null(p);_T1=_T0->d;_T2=_check_null(q);_T3=_T2->d;{struct Cyc_AP_T*d=Cyc_AP_lcm(_T1,_T3);_T4=d;_T5=p;_T6=_T5->d;{
struct Cyc_AP_T*px=Cyc_AP_div(_T4,_T6);_T7=d;_T8=q;_T9=_T8->d;{
struct Cyc_AP_T*qx=Cyc_AP_div(_T7,_T9);_TA=p;_TB=_TA->n;_TC=px;_TD=
Cyc_AP_mul(_TB,_TC);_TE=q;_TF=_TE->n;_T10=qx;_T11=Cyc_AP_mul(_TF,_T10);_T12=Cyc_AP_add(_TD,_T11);_T13=d;_T14=Cyc_APQ_fromAP(_T12,_T13);return _T14;}}}}
# 79
struct Cyc_APQ_T*Cyc_APQ_sub(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_APQ_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;struct Cyc_APQ_T*_T5;struct Cyc_AP_T*_T6;struct Cyc_AP_T*_T7;struct Cyc_APQ_T*_T8;struct Cyc_AP_T*_T9;struct Cyc_APQ_T*_TA;struct Cyc_AP_T*_TB;struct Cyc_AP_T*_TC;struct Cyc_AP_T*_TD;struct Cyc_APQ_T*_TE;struct Cyc_AP_T*_TF;struct Cyc_AP_T*_T10;struct Cyc_AP_T*_T11;struct Cyc_AP_T*_T12;struct Cyc_AP_T*_T13;struct Cyc_APQ_T*_T14;_T0=
_check_null(p);_T1=_T0->d;_T2=_check_null(q);_T3=_T2->d;{struct Cyc_AP_T*d=Cyc_AP_lcm(_T1,_T3);_T4=d;_T5=p;_T6=_T5->d;{
struct Cyc_AP_T*px=Cyc_AP_div(_T4,_T6);_T7=d;_T8=q;_T9=_T8->d;{
struct Cyc_AP_T*qx=Cyc_AP_div(_T7,_T9);_TA=p;_TB=_TA->n;_TC=px;_TD=
Cyc_AP_mul(_TB,_TC);_TE=q;_TF=_TE->n;_T10=qx;_T11=Cyc_AP_mul(_TF,_T10);_T12=Cyc_AP_sub(_TD,_T11);_T13=d;_T14=Cyc_APQ_fromAP(_T12,_T13);return _T14;}}}}
# 86
struct Cyc_APQ_T*Cyc_APQ_mul(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_APQ_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;struct Cyc_APQ_T*_T5;struct Cyc_AP_T*_T6;struct Cyc_APQ_T*_T7;struct Cyc_AP_T*_T8;struct Cyc_AP_T*_T9;struct Cyc_APQ_T*_TA;_T0=
_check_null(p);_T1=_T0->n;_T2=_check_null(q);_T3=_T2->n;_T4=Cyc_AP_mul(_T1,_T3);_T5=p;_T6=_T5->d;_T7=q;_T8=_T7->d;_T9=Cyc_AP_mul(_T6,_T8);_TA=Cyc_APQ_fromAP(_T4,_T9);return _TA;}
# 90
struct Cyc_APQ_T*Cyc_APQ_div(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_AP_T*_T2;int _T3;struct Cyc_Invalid_argument_exn_struct*_T4;void*_T5;struct Cyc_APQ_T*_T6;struct Cyc_AP_T*_T7;struct Cyc_APQ_T*_T8;struct Cyc_AP_T*_T9;struct Cyc_AP_T*_TA;struct Cyc_APQ_T*_TB;struct Cyc_AP_T*_TC;struct Cyc_APQ_T*_TD;struct Cyc_AP_T*_TE;struct Cyc_AP_T*_TF;struct Cyc_APQ_T*_T10;_T0=
_check_null(q);_T1=_T0->n;_T2=Cyc_AP_zero;_T3=Cyc_AP_cmp(_T1,_T2);if(_T3!=0)goto _TL10;{struct Cyc_Invalid_argument_exn_struct*_T11=_cycalloc(sizeof(struct Cyc_Invalid_argument_exn_struct));_T11->tag=Cyc_Invalid_argument;_T11->f1=_tag_fat("APQ_div: divide by zero",sizeof(char),24U);_T4=(struct Cyc_Invalid_argument_exn_struct*)_T11;}_T5=(void*)_T4;_throw(_T5);goto _TL11;_TL10: _TL11: _T6=
_check_null(p);_T7=_T6->n;_T8=q;_T9=_T8->d;_TA=Cyc_AP_mul(_T7,_T9);_TB=p;_TC=_TB->d;_TD=q;_TE=_TD->n;_TF=Cyc_AP_mul(_TC,_TE);_T10=Cyc_APQ_fromAP(_TA,_TF);return _T10;}
# 95
int Cyc_APQ_cmp(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){struct Cyc_APQ_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_APQ_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;struct Cyc_APQ_T*_T5;struct Cyc_AP_T*_T6;struct Cyc_AP_T*_T7;struct Cyc_APQ_T*_T8;struct Cyc_AP_T*_T9;struct Cyc_APQ_T*_TA;struct Cyc_AP_T*_TB;struct Cyc_AP_T*_TC;struct Cyc_AP_T*_TD;struct Cyc_APQ_T*_TE;struct Cyc_AP_T*_TF;struct Cyc_AP_T*_T10;struct Cyc_AP_T*_T11;int _T12;_T0=
_check_null(p);_T1=_T0->d;_T2=_check_null(q);_T3=_T2->d;{struct Cyc_AP_T*d=Cyc_AP_lcm(_T1,_T3);_T4=d;_T5=p;_T6=_T5->d;{
struct Cyc_AP_T*px=Cyc_AP_div(_T4,_T6);_T7=d;_T8=q;_T9=_T8->d;{
struct Cyc_AP_T*qx=Cyc_AP_div(_T7,_T9);_TA=p;_TB=_TA->n;_TC=px;_TD=
Cyc_AP_mul(_TB,_TC);_TE=q;_TF=_TE->n;_T10=qx;_T11=Cyc_AP_mul(_TF,_T10);_T12=Cyc_AP_cmp(_TD,_T11);return _T12;}}}}

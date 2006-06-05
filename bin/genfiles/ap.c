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

# 300 "cycboot.h"
 extern int isspace(int);
# 339
extern const long Cyc_long_max;extern const long Cyc_long_min;
# 82 "string.h"
extern struct _fat_ptr Cyc__memcpy(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
struct Cyc_AP_T*Cyc_AP_fromint(long);
# 18
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 20
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 22
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*,long);
# 28
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*,int);
# 8 "xp.h"
extern int Cyc_XP_add(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,int);
extern int Cyc_XP_sub(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,int);
extern int Cyc_XP_mul(struct _fat_ptr,int,struct _fat_ptr,int,struct _fat_ptr);
extern int Cyc_XP_div(int,struct _fat_ptr,struct _fat_ptr,int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern int Cyc_XP_sum(int,struct _fat_ptr,struct _fat_ptr,int);
extern int Cyc_XP_diff(int,struct _fat_ptr,struct _fat_ptr,int);
# 17
extern int Cyc_XP_cmp(int,struct _fat_ptr,struct _fat_ptr);
extern void Cyc_XP_lshift(int,struct _fat_ptr,int,struct _fat_ptr,int,int);
# 20
extern void Cyc_XP_rshift(int,struct _fat_ptr,int,struct _fat_ptr,int,int);
# 22
extern void Cyc_XP_and(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern void Cyc_XP_or(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern void Cyc_XP_xor(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern int Cyc_XP_length(int,struct _fat_ptr);
extern unsigned long Cyc_XP_fromint(int,struct _fat_ptr,unsigned long);
# 28
extern unsigned long Cyc_XP_toint(int,struct _fat_ptr);
extern int Cyc_XP_fromstr(int,struct _fat_ptr,const char*,int);
# 31
extern struct _fat_ptr Cyc_XP_tostr(struct _fat_ptr,int,int,int,struct _fat_ptr);struct Cyc_AP_T{int sign;int ndigits;int size;struct _fat_ptr digits;};
# 18 "ap.cyc"
struct Cyc_AP_T*Cyc_AP_zero;
struct Cyc_AP_T*Cyc_AP_one;
int Cyc_init=0;
# 22
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*,int);
# 24
static void Cyc_AP_init (void){
Cyc_init=1;
Cyc_AP_zero=Cyc_AP_fromint(0);
Cyc_AP_one=Cyc_AP_fromint(1);}
# 29
static struct Cyc_AP_T*Cyc_mk(int size){int _T0;void*_T1;struct Cyc_AP_T*_T2;struct _fat_ptr _T3;void*_T4;int(*_T5)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T6)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_AP_T*_T9;struct Cyc_AP_T*_TA;struct Cyc_AP_T*_TB;struct Cyc_AP_T*_TC;
struct Cyc_AP_T*z;_T0=Cyc_init;
if(_T0)goto _TL0;else{goto _TL2;}_TL2: Cyc_AP_init();goto _TL1;_TL0: _TL1: _T1=_cycalloc(sizeof(struct Cyc_AP_T));
z=(struct Cyc_AP_T*)_T1;_T2=z;{unsigned _TD=size;_T4=_cyccalloc_atomic(sizeof(unsigned char),_TD);_T3=_tag_fat(_T4,sizeof(unsigned char),_TD);}
_T2->digits=_T3;
if(size <= 0)goto _TL3;goto _TL4;_TL3: _T6=Cyc___assert_fail;{int(*_TD)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T6;_T5=_TD;}_T7=_tag_fat("size > 0",sizeof(char),9U);_T8=_tag_fat("ap.cyc",sizeof(char),7U);_T5(_T7,_T8,34U);_TL4: _T9=z;
_T9->sign=1;_TA=z;
_TA->size=size;_TB=z;
_TB->ndigits=1;_TC=z;
return _TC;}
# 40
static struct Cyc_AP_T*Cyc_set(struct Cyc_AP_T*z,long n){struct Cyc_AP_T*_T0;int _T1;struct Cyc_AP_T*_T2;struct _fat_ptr _T3;long _T4;unsigned _T5;unsigned _T6;struct Cyc_AP_T*_T7;int _T8;struct Cyc_AP_T*_T9;struct _fat_ptr _TA;long _TB;unsigned long _TC;struct Cyc_AP_T*_TD;int _TE;struct Cyc_AP_T*_TF;struct _fat_ptr _T10;long _T11;unsigned long _T12;struct Cyc_AP_T*_T13;int _T14;struct Cyc_AP_T*_T15;struct Cyc_AP_T*_T16;int _T17;struct Cyc_AP_T*_T18;
if(n!=Cyc_long_min)goto _TL5;_T0=
_check_null(z);_T1=_T0->size;_T2=z;_T3=_T2->digits;_T4=Cyc_long_max;_T5=(unsigned)_T4;_T6=_T5 + 1U;Cyc_XP_fromint(_T1,_T3,_T6);goto _TL6;
_TL5: if(n >= 0)goto _TL7;_T7=
_check_null(z);_T8=_T7->size;_T9=z;_TA=_T9->digits;_TB=- n;_TC=(unsigned long)_TB;Cyc_XP_fromint(_T8,_TA,_TC);goto _TL8;
# 46
_TL7: _TD=_check_null(z);_TE=_TD->size;_TF=z;_T10=_TF->digits;_T11=n;_T12=(unsigned long)_T11;Cyc_XP_fromint(_TE,_T10,_T12);_TL8: _TL6: _T13=z;
if(n >= 0)goto _TL9;_T14=- 1;goto _TLA;_TL9: _T14=1;_TLA: _T13->sign=_T14;_T15=z;_T16=z;_T17=_T16->size;_T18=
Cyc_normalize(_T15,_T17);return _T18;}
# 50
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*z,int n){struct Cyc_AP_T*_T0;int _T1;struct Cyc_AP_T*_T2;struct _fat_ptr _T3;struct Cyc_AP_T*_T4;_T0=z;_T1=n;_T2=
_check_null(z);_T3=_T2->digits;_T0->ndigits=Cyc_XP_length(_T1,_T3);_T4=z;
return _T4;}
# 54
static struct Cyc_AP_T*Cyc_add(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;struct Cyc_AP_T*_T1;int _T2;int _T3;struct Cyc_AP_T*_T4;struct Cyc_AP_T*_T5;int _T6;int _T7;int _T8;struct Cyc_AP_T*_T9;struct _fat_ptr _TA;struct Cyc_AP_T*_TB;struct _fat_ptr _TC;struct Cyc_AP_T*_TD;struct _fat_ptr _TE;struct Cyc_AP_T*_TF;struct _fat_ptr _T10;struct Cyc_AP_T*_T11;int _T12;int _T13;char*_T14;unsigned char*_T15;struct Cyc_AP_T*_T16;int _T17;int _T18;int _T19;struct Cyc_AP_T*_T1A;struct _fat_ptr _T1B;int _T1C;struct _fat_ptr _T1D;struct Cyc_AP_T*_T1E;struct _fat_ptr _T1F;int _T20;struct _fat_ptr _T21;int _T22;int _T23;struct Cyc_AP_T*_T24;struct _fat_ptr _T25;int _T26;char*_T27;unsigned char*_T28;int _T29;struct Cyc_AP_T*_T2A;struct _fat_ptr _T2B;struct Cyc_AP_T*_T2C;struct _fat_ptr _T2D;struct Cyc_AP_T*_T2E;struct _fat_ptr _T2F;int _T30;struct Cyc_AP_T*_T31;struct Cyc_AP_T*_T32;int _T33;struct Cyc_AP_T*_T34;_T0=
_check_null(y);{int n=_T0->ndigits;_T1=
_check_null(x);_T2=_T1->ndigits;_T3=n;if(_T2 >= _T3)goto _TLB;_T4=
Cyc_add(z,y,x);return _T4;
_TLB: _T5=x;_T6=_T5->ndigits;_T7=n;if(_T6 <= _T7)goto _TLD;_T8=n;_T9=
_check_null(z);_TA=_T9->digits;_TB=x;_TC=_TB->digits;_TD=y;_TE=_TD->digits;{int carry=Cyc_XP_add(_T8,_TA,_TC,_TE,0);_TF=z;_T10=_TF->digits;_T11=z;_T12=_T11->size;_T13=_T12 - 1;_T14=_check_fat_subscript(_T10,sizeof(unsigned char),_T13);_T15=(unsigned char*)_T14;_T16=x;_T17=_T16->ndigits;_T18=n;_T19=_T17 - _T18;_T1A=z;_T1B=_T1A->digits;_T1C=n;_T1D=
# 62
_fat_ptr_plus(_T1B,sizeof(unsigned char),_T1C);_T1E=x;_T1F=_T1E->digits;_T20=n;_T21=_fat_ptr_plus(_T1F,sizeof(unsigned char),_T20);_T22=carry;_T23=
# 61
Cyc_XP_sum(_T19,_T1D,_T21,_T22);*_T15=(unsigned char)_T23;}goto _TLE;
# 64
_TLD: _T24=z;_T25=_T24->digits;_T26=n;_T27=_check_fat_subscript(_T25,sizeof(unsigned char),_T26);_T28=(unsigned char*)_T27;_T29=n;_T2A=_check_null(z);_T2B=_T2A->digits;_T2C=x;_T2D=_T2C->digits;_T2E=y;_T2F=_T2E->digits;_T30=Cyc_XP_add(_T29,_T2B,_T2D,_T2F,0);*_T28=(unsigned char)_T30;_TLE: _T31=z;_T32=z;_T33=_T32->size;_T34=
# 66
Cyc_normalize(_T31,_T33);return _T34;}}
# 68
static struct Cyc_AP_T*Cyc_sub(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;int _T1;struct Cyc_AP_T*_T2;struct _fat_ptr _T3;struct Cyc_AP_T*_T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;struct _fat_ptr _T7;struct Cyc_AP_T*_T8;int _T9;int _TA;struct Cyc_AP_T*_TB;int _TC;int _TD;int _TE;struct Cyc_AP_T*_TF;struct _fat_ptr _T10;int _T11;struct _fat_ptr _T12;struct Cyc_AP_T*_T13;struct _fat_ptr _T14;int _T15;struct _fat_ptr _T16;int _T17;int(*_T18)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T19)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T1A;struct _fat_ptr _T1B;struct Cyc_AP_T*_T1C;struct Cyc_AP_T*_T1D;int _T1E;struct Cyc_AP_T*_T1F;
int borrow;_T0=_check_null(y);{int n=_T0->ndigits;_T1=n;_T2=
_check_null(z);_T3=_T2->digits;_T4=_check_null(x);_T5=_T4->digits;_T6=y;_T7=_T6->digits;borrow=Cyc_XP_sub(_T1,_T3,_T5,_T7,0);_T8=x;_T9=_T8->ndigits;_TA=n;
# 72
if(_T9 <= _TA)goto _TLF;_TB=x;_TC=_TB->ndigits;_TD=n;_TE=_TC - _TD;_TF=z;_T10=_TF->digits;_T11=n;_T12=
_fat_ptr_plus(_T10,sizeof(unsigned char),_T11);_T13=x;_T14=_T13->digits;_T15=n;_T16=
_fat_ptr_plus(_T14,sizeof(unsigned char),_T15);_T17=borrow;
# 73
borrow=Cyc_XP_diff(_TE,_T12,_T16,_T17);goto _TL10;_TLF: _TL10:
# 75
 if(borrow!=0)goto _TL11;goto _TL12;_TL11: _T19=Cyc___assert_fail;{int(*_T20)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T19;_T18=_T20;}_T1A=_tag_fat("borrow == 0",sizeof(char),12U);_T1B=_tag_fat("ap.cyc",sizeof(char),7U);_T18(_T1A,_T1B,75U);_TL12: _T1C=z;_T1D=z;_T1E=_T1D->size;_T1F=
Cyc_normalize(_T1C,_T1E);return _T1F;}}
# 78
static struct Cyc_AP_T*Cyc_mulmod(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){struct Cyc_AP_T*_T0;
struct Cyc_AP_T*z;struct Cyc_AP_T*xy=Cyc_AP_mul(x,y);
z=Cyc_AP_mod(xy,p);_T0=z;
return _T0;}
# 83
static int Cyc_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;int _T1;struct Cyc_AP_T*_T2;int _T3;struct Cyc_AP_T*_T4;int _T5;struct Cyc_AP_T*_T6;int _T7;int _T8;struct Cyc_AP_T*_T9;int _TA;struct Cyc_AP_T*_TB;struct _fat_ptr _TC;struct Cyc_AP_T*_TD;struct _fat_ptr _TE;int _TF;_T0=
_check_null(x);_T1=_T0->ndigits;_T2=_check_null(y);_T3=_T2->ndigits;if(_T1==_T3)goto _TL13;_T4=x;_T5=_T4->ndigits;_T6=y;_T7=_T6->ndigits;_T8=_T5 - _T7;
return _T8;
# 87
_TL13: _T9=x;_TA=_T9->ndigits;_TB=x;_TC=_TB->digits;_TD=y;_TE=_TD->digits;_TF=Cyc_XP_cmp(_TA,_TC,_TE);return _TF;}
# 89
struct Cyc_AP_T*Cyc_AP_new(long n){int _T0;struct Cyc_AP_T*_T1;long _T2;struct Cyc_AP_T*_T3;_T0=(int)sizeof(long);_T1=
Cyc_mk(_T0);_T2=n;_T3=Cyc_set(_T1,_T2);return _T3;}
# 92
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;int _T7;struct Cyc_AP_T*_T8;struct _fat_ptr _T9;struct Cyc_AP_T*_TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;unsigned _TE;unsigned _TF;unsigned _T10;int _T11;struct Cyc_AP_T*_T12;int _T13;unsigned _T14;unsigned _T15;unsigned _T16;unsigned _T17;unsigned _T18;unsigned _T19;struct Cyc_AP_T*_T1A;struct Cyc_AP_T*_T1B;struct Cyc_AP_T*_T1C;int _T1D;struct Cyc_AP_T*_T1E;int _T1F;struct Cyc_AP_T*_T20;struct _fat_ptr _T21;char*_T22;unsigned char*_T23;unsigned char _T24;int _T25;struct Cyc_AP_T*_T26;int _T27;struct Cyc_AP_T*_T28;
struct Cyc_AP_T*z;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL15;goto _TL16;_TL15: _T3=Cyc___assert_fail;{int(*_T29)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T29;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,94U);_TL16: _T6=x;_T7=_T6->ndigits;
z=Cyc_mk(_T7);_T8=z;_T9=_T8->digits;_TA=x;_TB=_TA->digits;_TC=x;_TD=_TC->ndigits;_TE=(unsigned)_TD;_TF=sizeof(*((unsigned char*)x->digits.curr));_T10=_TE / _TF;_T12=x;_T13=_T12->ndigits;_T14=(unsigned)_T13;_T15=sizeof(*((unsigned char*)x->digits.curr));_T16=_T14 % _T15;
if(_T16!=0U)goto _TL17;_T11=0;goto _TL18;_TL17: _T11=1;_TL18: _T17=(unsigned)_T11;_T18=_T10 + _T17;_T19=sizeof(*((unsigned char*)x->digits.curr));Cyc__memcpy(_T9,_TB,_T18,_T19);_T1A=z;_T1B=x;
_T1A->ndigits=_T1B->ndigits;_T1C=z;_T1E=z;_T1F=_T1E->ndigits;
if(_T1F!=1)goto _TL19;_T20=z;_T21=_T20->digits;_T22=_check_fat_subscript(_T21,sizeof(unsigned char),0);_T23=(unsigned char*)_T22;_T24=*_T23;_T25=(int)_T24;if(_T25!=0)goto _TL19;_T1D=1;goto _TL1A;_TL19: _T26=x;_T27=_T26->sign;_T1D=- _T27;_TL1A: _T1C->sign=_T1D;_T28=z;
return _T28;}
# 101
struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*x){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;int _T7;struct Cyc_AP_T*_T8;struct _fat_ptr _T9;struct Cyc_AP_T*_TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;unsigned _TE;unsigned _TF;unsigned _T10;int _T11;struct Cyc_AP_T*_T12;int _T13;unsigned _T14;unsigned _T15;unsigned _T16;unsigned _T17;unsigned _T18;unsigned _T19;struct Cyc_AP_T*_T1A;struct Cyc_AP_T*_T1B;struct Cyc_AP_T*_T1C;struct Cyc_AP_T*_T1D;
struct Cyc_AP_T*z;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL1B;goto _TL1C;_TL1B: _T3=Cyc___assert_fail;{int(*_T1E)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T1E;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,103U);_TL1C: _T6=x;_T7=_T6->ndigits;
z=Cyc_mk(_T7);_T8=z;_T9=_T8->digits;_TA=x;_TB=_TA->digits;_TC=x;_TD=_TC->ndigits;_TE=(unsigned)_TD;_TF=sizeof(*((unsigned char*)x->digits.curr));_T10=_TE / _TF;_T12=x;_T13=_T12->ndigits;_T14=(unsigned)_T13;_T15=sizeof(*((unsigned char*)x->digits.curr));_T16=_T14 % _T15;
if(_T16!=0U)goto _TL1D;_T11=0;goto _TL1E;_TL1D: _T11=1;_TL1E: _T17=(unsigned)_T11;_T18=_T10 + _T17;_T19=sizeof(*((unsigned char*)x->digits.curr));Cyc__memcpy(_T9,_TB,_T18,_T19);_T1A=z;_T1B=x;
_T1A->ndigits=_T1B->ndigits;_T1C=z;
_T1C->sign=1;_T1D=z;
return _T1D;}
# 110
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;struct Cyc_AP_T*_TE;int _TF;int _T10;struct Cyc_AP_T*_T11;struct _fat_ptr _T12;struct Cyc_AP_T*_T13;int _T14;struct Cyc_AP_T*_T15;struct _fat_ptr _T16;struct Cyc_AP_T*_T17;int _T18;struct Cyc_AP_T*_T19;struct _fat_ptr _T1A;struct Cyc_AP_T*_T1B;struct Cyc_AP_T*_T1C;int _T1D;struct Cyc_AP_T*_T1E;int _T1F;struct Cyc_AP_T*_T20;int _T21;struct Cyc_AP_T*_T22;struct _fat_ptr _T23;char*_T24;unsigned char*_T25;unsigned char _T26;int _T27;struct Cyc_AP_T*_T28;int _T29;struct Cyc_AP_T*_T2A;int _T2B;int _T2C;struct Cyc_AP_T*_T2D;
struct Cyc_AP_T*z;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL1F;goto _TL20;_TL1F: _T3=Cyc___assert_fail;{int(*_T2E)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T2E;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,112U);_TL20: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TL21;goto _TL22;_TL21: _T9=Cyc___assert_fail;{int(*_T2E)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T2E;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,113U);_TL22: _TC=x;_TD=_TC->ndigits;_TE=y;_TF=_TE->ndigits;_T10=_TD + _TF;
z=Cyc_mk(_T10);_T11=z;_T12=_T11->digits;_T13=x;_T14=_T13->ndigits;_T15=x;_T16=_T15->digits;_T17=y;_T18=_T17->ndigits;_T19=y;_T1A=_T19->digits;
Cyc_XP_mul(_T12,_T14,_T16,_T18,_T1A);_T1B=z;_T1C=z;_T1D=_T1C->size;
# 117
Cyc_normalize(_T1B,_T1D);_T1E=z;_T20=z;_T21=_T20->ndigits;
if(_T21==1)goto _TL27;else{goto _TL26;}_TL27: _T22=z;_T23=_T22->digits;_T24=_check_fat_subscript(_T23,sizeof(unsigned char),0);_T25=(unsigned char*)_T24;_T26=*_T25;_T27=(int)_T26;if(_T27==0)goto _TL25;else{goto _TL26;}_TL26: _T28=x;_T29=_T28->sign;_T2A=y;_T2B=_T2A->sign;_T2C=_T29 ^ _T2B;if(_T2C==0)goto _TL25;else{goto _TL23;}_TL25: _T1F=1;goto _TL24;_TL23: _T1F=- 1;_TL24: _T1E->sign=_T1F;_T2D=z;
# 120
return _T2D;}
# 122
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;struct Cyc_AP_T*_TE;int _TF;int _T10;int _T11;struct Cyc_AP_T*_T12;int _T13;struct Cyc_AP_T*_T14;int _T15;struct Cyc_AP_T*_T16;struct Cyc_AP_T*_T17;int _T18;struct Cyc_AP_T*_T19;struct Cyc_AP_T*_T1A;struct Cyc_AP_T*_T1B;struct Cyc_AP_T*_T1C;int _T1D;struct Cyc_AP_T*_T1E;int _T1F;struct Cyc_AP_T*_T20;struct _fat_ptr _T21;char*_T22;unsigned char*_T23;unsigned char _T24;int _T25;struct Cyc_AP_T*_T26;int _T27;struct Cyc_AP_T*_T28;int _T29;struct Cyc_AP_T*_T2A;struct Cyc_AP_T*_T2B;struct Cyc_AP_T*_T2C;struct Cyc_AP_T*_T2D;int _T2E;struct Cyc_AP_T*_T2F;int _T30;struct Cyc_AP_T*_T31;struct _fat_ptr _T32;char*_T33;unsigned char*_T34;unsigned char _T35;int _T36;struct Cyc_AP_T*_T37;struct Cyc_AP_T*_T38;int _T39;struct Cyc_AP_T*_T3A;struct Cyc_AP_T*_T3B;struct Cyc_AP_T*_T3C;struct Cyc_AP_T*_T3D;int _T3E;struct Cyc_AP_T*_T3F;int _T40;struct Cyc_AP_T*_T41;struct _fat_ptr _T42;char*_T43;unsigned char*_T44;unsigned char _T45;int _T46;struct Cyc_AP_T*_T47;int _T48;struct Cyc_AP_T*_T49;
struct Cyc_AP_T*z;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL28;goto _TL29;_TL28: _T3=Cyc___assert_fail;{int(*_T4A)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T4A;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,124U);_TL29: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TL2A;goto _TL2B;_TL2A: _T9=Cyc___assert_fail;{int(*_T4A)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T4A;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,125U);_TL2B: _TC=x;_TD=_TC->sign;_TE=y;_TF=_TE->sign;_T10=_TD ^ _TF;
if(_T10!=0)goto _TL2C;_T12=x;_T13=_T12->ndigits;_T14=y;_T15=_T14->ndigits;
if(_T13 <= _T15)goto _TL2E;_T16=x;_T11=_T16->ndigits;goto _TL2F;_TL2E: _T17=y;_T11=_T17->ndigits;_TL2F: _T18=_T11 + 1;_T19=Cyc_mk(_T18);_T1A=x;_T1B=y;z=Cyc_add(_T19,_T1A,_T1B);_T1C=z;_T1E=
_check_null(z);_T1F=_T1E->ndigits;if(_T1F!=1)goto _TL30;_T20=z;_T21=_T20->digits;_T22=_check_fat_subscript(_T21,sizeof(unsigned char),0);_T23=(unsigned char*)_T22;_T24=*_T23;_T25=(int)_T24;if(_T25!=0)goto _TL30;_T1D=1;goto _TL31;_TL30: _T26=x;_T1D=_T26->sign;_TL31: _T1C->sign=_T1D;goto _TL2D;
# 130
_TL2C: _T27=Cyc_cmp(x,y);if(_T27 <= 0)goto _TL32;_T28=x;_T29=_T28->ndigits;_T2A=
Cyc_mk(_T29);_T2B=x;_T2C=y;z=Cyc_sub(_T2A,_T2B,_T2C);_T2D=z;_T2F=z;_T30=_T2F->ndigits;
if(_T30!=1)goto _TL34;_T31=z;_T32=_T31->digits;_T33=_check_fat_subscript(_T32,sizeof(unsigned char),0);_T34=(unsigned char*)_T33;_T35=*_T34;_T36=(int)_T35;if(_T36!=0)goto _TL34;_T2E=1;goto _TL35;_TL34: _T37=x;_T2E=_T37->sign;_TL35: _T2D->sign=_T2E;goto _TL33;
# 135
_TL32: _T38=y;_T39=_T38->ndigits;_T3A=Cyc_mk(_T39);_T3B=y;_T3C=x;z=Cyc_sub(_T3A,_T3B,_T3C);_T3D=z;_T3F=z;_T40=_T3F->ndigits;
if(_T40!=1)goto _TL36;_T41=z;_T42=_T41->digits;_T43=_check_fat_subscript(_T42,sizeof(unsigned char),0);_T44=(unsigned char*)_T43;_T45=*_T44;_T46=(int)_T45;if(_T46!=0)goto _TL36;_T3E=1;goto _TL37;_TL36: _T47=x;_T48=_T47->sign;_T3E=- _T48;_TL37: _T3D->sign=_T3E;_TL33: _TL2D: _T49=z;
# 138
return _T49;}
# 140
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;struct Cyc_AP_T*_TE;int _TF;int _T10;int _T11;struct Cyc_AP_T*_T12;int _T13;struct Cyc_AP_T*_T14;int _T15;struct Cyc_AP_T*_T16;struct Cyc_AP_T*_T17;int _T18;struct Cyc_AP_T*_T19;struct Cyc_AP_T*_T1A;struct Cyc_AP_T*_T1B;struct Cyc_AP_T*_T1C;int _T1D;struct Cyc_AP_T*_T1E;int _T1F;struct Cyc_AP_T*_T20;struct _fat_ptr _T21;char*_T22;unsigned char*_T23;unsigned char _T24;int _T25;struct Cyc_AP_T*_T26;int _T27;struct Cyc_AP_T*_T28;int _T29;struct Cyc_AP_T*_T2A;struct Cyc_AP_T*_T2B;struct Cyc_AP_T*_T2C;struct Cyc_AP_T*_T2D;int _T2E;struct Cyc_AP_T*_T2F;int _T30;struct Cyc_AP_T*_T31;struct _fat_ptr _T32;char*_T33;unsigned char*_T34;unsigned char _T35;int _T36;struct Cyc_AP_T*_T37;struct Cyc_AP_T*_T38;int _T39;struct Cyc_AP_T*_T3A;struct Cyc_AP_T*_T3B;struct Cyc_AP_T*_T3C;struct Cyc_AP_T*_T3D;int _T3E;struct Cyc_AP_T*_T3F;int _T40;struct Cyc_AP_T*_T41;struct _fat_ptr _T42;char*_T43;unsigned char*_T44;unsigned char _T45;int _T46;struct Cyc_AP_T*_T47;int _T48;struct Cyc_AP_T*_T49;
struct Cyc_AP_T*z;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL38;goto _TL39;_TL38: _T3=Cyc___assert_fail;{int(*_T4A)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T4A;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,142U);_TL39: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TL3A;goto _TL3B;_TL3A: _T9=Cyc___assert_fail;{int(*_T4A)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T4A;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,143U);_TL3B: _TC=x;_TD=_TC->sign;_TE=y;_TF=_TE->sign;_T10=_TD ^ _TF;
if(_T10==0)goto _TL3C;else{goto _TL3E;}
_TL3E: _T12=x;_T13=_T12->ndigits;_T14=y;_T15=_T14->ndigits;if(_T13 <= _T15)goto _TL3F;_T16=x;_T11=_T16->ndigits;goto _TL40;_TL3F: _T17=y;_T11=_T17->ndigits;_TL40: _T18=_T11 + 1;_T19=Cyc_mk(_T18);_T1A=x;_T1B=y;z=Cyc_add(_T19,_T1A,_T1B);_T1C=z;_T1E=
_check_null(z);_T1F=_T1E->ndigits;if(_T1F!=1)goto _TL41;_T20=z;_T21=_T20->digits;_T22=_check_fat_subscript(_T21,sizeof(unsigned char),0);_T23=(unsigned char*)_T22;_T24=*_T23;_T25=(int)_T24;if(_T25!=0)goto _TL41;_T1D=1;goto _TL42;_TL41: _T26=x;_T1D=_T26->sign;_TL42: _T1C->sign=_T1D;goto _TL3D;
# 148
_TL3C: _T27=Cyc_cmp(x,y);if(_T27 <= 0)goto _TL43;_T28=x;_T29=_T28->ndigits;_T2A=
Cyc_mk(_T29);_T2B=x;_T2C=y;z=Cyc_sub(_T2A,_T2B,_T2C);_T2D=z;_T2F=z;_T30=_T2F->ndigits;
if(_T30!=1)goto _TL45;_T31=z;_T32=_T31->digits;_T33=_check_fat_subscript(_T32,sizeof(unsigned char),0);_T34=(unsigned char*)_T33;_T35=*_T34;_T36=(int)_T35;if(_T36!=0)goto _TL45;_T2E=1;goto _TL46;_TL45: _T37=x;_T2E=_T37->sign;_TL46: _T2D->sign=_T2E;goto _TL44;
# 152
_TL43: _T38=y;_T39=_T38->ndigits;_T3A=Cyc_mk(_T39);_T3B=y;_T3C=x;z=Cyc_sub(_T3A,_T3B,_T3C);_T3D=z;_T3F=z;_T40=_T3F->ndigits;
if(_T40!=1)goto _TL47;_T41=z;_T42=_T41->digits;_T43=_check_fat_subscript(_T42,sizeof(unsigned char),0);_T44=(unsigned char*)_T43;_T45=*_T44;_T46=(int)_T45;if(_T46!=0)goto _TL47;_T3E=1;goto _TL48;_TL47: _T47=x;_T48=_T47->sign;_T3E=- _T48;_TL48: _T3D->sign=_T3E;_TL44: _TL3D: _T49=z;
# 155
return _T49;}
# 157
struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;struct Cyc_AP_T*_TE;struct _fat_ptr _TF;char*_T10;unsigned char*_T11;unsigned char _T12;int _T13;int(*_T14)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T15)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T16;struct _fat_ptr _T17;struct Cyc_AP_T*_T18;int _T19;struct Cyc_AP_T*_T1A;int _T1B;struct _fat_ptr _T1C;struct Cyc_AP_T*_T1D;int _T1E;struct Cyc_AP_T*_T1F;int _T20;int _T21;int _T22;unsigned _T23;void*_T24;struct Cyc_AP_T*_T25;int _T26;struct Cyc_AP_T*_T27;struct _fat_ptr _T28;struct Cyc_AP_T*_T29;struct _fat_ptr _T2A;struct Cyc_AP_T*_T2B;int _T2C;struct Cyc_AP_T*_T2D;struct _fat_ptr _T2E;struct Cyc_AP_T*_T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_AP_T*_T32;struct Cyc_AP_T*_T33;int _T34;struct Cyc_AP_T*_T35;struct Cyc_AP_T*_T36;int _T37;struct Cyc_AP_T*_T38;int _T39;struct Cyc_AP_T*_T3A;int _T3B;struct Cyc_AP_T*_T3C;struct _fat_ptr _T3D;char*_T3E;unsigned char*_T3F;unsigned char _T40;int _T41;struct Cyc_AP_T*_T42;int _T43;struct Cyc_AP_T*_T44;int _T45;int _T46;struct Cyc_AP_T*_T47;
struct Cyc_AP_T*q;struct Cyc_AP_T*r;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL49;goto _TL4A;_TL49: _T3=Cyc___assert_fail;{int(*_T48)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T48;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,159U);_TL4A: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TL4B;goto _TL4C;_TL4B: _T9=Cyc___assert_fail;{int(*_T48)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T48;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,160U);_TL4C: _TC=y;_TD=_TC->ndigits;
if(_TD==1)goto _TL50;else{goto _TL4F;}_TL50: _TE=y;_TF=_TE->digits;_T10=_check_fat_subscript(_TF,sizeof(unsigned char),0);_T11=(unsigned char*)_T10;_T12=*_T11;_T13=(int)_T12;if(_T13==0)goto _TL4D;else{goto _TL4F;}_TL4F: goto _TL4E;_TL4D: _T15=Cyc___assert_fail;{int(*_T48)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T15;_T14=_T48;}_T16=_tag_fat("!((y)->ndigits==1 && (y)->digits[0]==0)",sizeof(char),40U);_T17=_tag_fat("ap.cyc",sizeof(char),7U);_T14(_T16,_T17,161U);_TL4E: _T18=x;_T19=_T18->ndigits;
q=Cyc_mk(_T19);_T1A=y;_T1B=_T1A->ndigits;
r=Cyc_mk(_T1B);_T1D=x;_T1E=_T1D->ndigits;_T1F=y;_T20=_T1F->ndigits;_T21=_T1E + _T20;_T22=_T21 + 2;_T23=(unsigned)_T22;{unsigned _T48=_T23 * sizeof(unsigned char);_T24=_cycalloc_atomic(_T48);_T1C=_tag_fat(_T24,1U,_T48);}{
# 165
struct _fat_ptr tmp=_T1C;_T25=x;_T26=_T25->ndigits;_T27=q;_T28=_T27->digits;_T29=x;_T2A=_T29->digits;_T2B=y;_T2C=_T2B->ndigits;_T2D=y;_T2E=_T2D->digits;_T2F=r;_T30=_T2F->digits;_T31=tmp;
Cyc_XP_div(_T26,_T28,_T2A,_T2C,_T2E,_T30,_T31);}_T32=q;_T33=q;_T34=_T33->size;
# 169
Cyc_normalize(_T32,_T34);_T35=r;_T36=r;_T37=_T36->size;
Cyc_normalize(_T35,_T37);_T38=q;_T3A=q;_T3B=_T3A->ndigits;
if(_T3B==1)goto _TL55;else{goto _TL54;}_TL55: _T3C=q;_T3D=_T3C->digits;_T3E=_check_fat_subscript(_T3D,sizeof(unsigned char),0);_T3F=(unsigned char*)_T3E;_T40=*_T3F;_T41=(int)_T40;if(_T41==0)goto _TL53;else{goto _TL54;}_TL54: _T42=x;_T43=_T42->sign;_T44=y;_T45=_T44->sign;_T46=_T43 ^ _T45;if(_T46==0)goto _TL53;else{goto _TL51;}_TL53: _T39=1;goto _TL52;_TL51: _T39=- 1;_TL52: _T38->sign=_T39;_T47=q;
# 173
return _T47;}
# 175
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;struct Cyc_AP_T*_TE;struct _fat_ptr _TF;char*_T10;unsigned char*_T11;unsigned char _T12;int _T13;int(*_T14)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T15)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T16;struct _fat_ptr _T17;struct Cyc_AP_T*_T18;int _T19;struct Cyc_AP_T*_T1A;int _T1B;struct _fat_ptr _T1C;struct Cyc_AP_T*_T1D;int _T1E;struct Cyc_AP_T*_T1F;int _T20;int _T21;int _T22;unsigned _T23;void*_T24;struct Cyc_AP_T*_T25;int _T26;struct Cyc_AP_T*_T27;struct _fat_ptr _T28;struct Cyc_AP_T*_T29;struct _fat_ptr _T2A;struct Cyc_AP_T*_T2B;int _T2C;struct Cyc_AP_T*_T2D;struct _fat_ptr _T2E;struct Cyc_AP_T*_T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_AP_T*_T32;struct Cyc_AP_T*_T33;int _T34;struct Cyc_AP_T*_T35;struct Cyc_AP_T*_T36;int _T37;struct Cyc_AP_T*_T38;int _T39;struct Cyc_AP_T*_T3A;int _T3B;struct Cyc_AP_T*_T3C;struct _fat_ptr _T3D;char*_T3E;unsigned char*_T3F;unsigned char _T40;int _T41;struct Cyc_AP_T*_T42;struct Cyc_AP_T*_T43;
struct Cyc_AP_T*q;struct Cyc_AP_T*r;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL56;goto _TL57;_TL56: _T3=Cyc___assert_fail;{int(*_T44)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T44;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,177U);_TL57: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TL58;goto _TL59;_TL58: _T9=Cyc___assert_fail;{int(*_T44)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T44;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,178U);_TL59: _TC=y;_TD=_TC->ndigits;
if(_TD==1)goto _TL5D;else{goto _TL5C;}_TL5D: _TE=y;_TF=_TE->digits;_T10=_check_fat_subscript(_TF,sizeof(unsigned char),0);_T11=(unsigned char*)_T10;_T12=*_T11;_T13=(int)_T12;if(_T13==0)goto _TL5A;else{goto _TL5C;}_TL5C: goto _TL5B;_TL5A: _T15=Cyc___assert_fail;{int(*_T44)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T15;_T14=_T44;}_T16=_tag_fat("!((y)->ndigits==1 && (y)->digits[0]==0)",sizeof(char),40U);_T17=_tag_fat("ap.cyc",sizeof(char),7U);_T14(_T16,_T17,179U);_TL5B: _T18=x;_T19=_T18->ndigits;
q=Cyc_mk(_T19);_T1A=y;_T1B=_T1A->ndigits;
r=Cyc_mk(_T1B);_T1D=x;_T1E=_T1D->ndigits;_T1F=y;_T20=_T1F->ndigits;_T21=_T1E + _T20;_T22=_T21 + 2;_T23=(unsigned)_T22;{unsigned _T44=_T23 * sizeof(unsigned char);_T24=_cycalloc_atomic(_T44);_T1C=_tag_fat(_T24,1U,_T44);}{
# 183
struct _fat_ptr tmp=_T1C;_T25=x;_T26=_T25->ndigits;_T27=q;_T28=_T27->digits;_T29=x;_T2A=_T29->digits;_T2B=y;_T2C=_T2B->ndigits;_T2D=y;_T2E=_T2D->digits;_T2F=r;_T30=_T2F->digits;_T31=tmp;
Cyc_XP_div(_T26,_T28,_T2A,_T2C,_T2E,_T30,_T31);}_T32=q;_T33=q;_T34=_T33->size;
# 187
Cyc_normalize(_T32,_T34);_T35=r;_T36=r;_T37=_T36->size;
Cyc_normalize(_T35,_T37);_T38=r;_T3A=r;_T3B=_T3A->ndigits;
if(_T3B!=1)goto _TL5E;_T3C=r;_T3D=_T3C->digits;_T3E=_check_fat_subscript(_T3D,sizeof(unsigned char),0);_T3F=(unsigned char*)_T3E;_T40=*_T3F;_T41=(int)_T40;if(_T41!=0)goto _TL5E;_T39=1;goto _TL5F;_TL5E: _T42=x;_T39=_T42->sign;_TL5F: _T38->sign=_T39;_T43=r;
return _T43;}
# 192
struct Cyc_AP_T*Cyc_AP_pow(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;int(*_TE)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_TF)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc_AP_T*_T12;unsigned _T13;struct Cyc_AP_T*_T14;int _T15;struct Cyc_AP_T*_T16;int _T17;struct Cyc_AP_T*_T18;struct _fat_ptr _T19;char*_T1A;unsigned char*_T1B;unsigned char _T1C;int _T1D;struct Cyc_AP_T*_T1E;int _T1F;struct Cyc_AP_T*_T20;struct _fat_ptr _T21;unsigned char*_T22;unsigned char*_T23;unsigned char _T24;int _T25;int(*_T26)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T27)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T28;struct _fat_ptr _T29;struct Cyc_AP_T*_T2A;int _T2B;struct Cyc_AP_T*_T2C;struct _fat_ptr _T2D;char*_T2E;unsigned char*_T2F;unsigned char _T30;int _T31;struct Cyc_AP_T*_T32;struct Cyc_AP_T*_T33;int _T34;struct Cyc_AP_T*_T35;struct _fat_ptr _T36;char*_T37;unsigned char*_T38;unsigned char _T39;int _T3A;struct Cyc_AP_T*_T3B;struct Cyc_AP_T*_T3C;int _T3D;struct Cyc_AP_T*_T3E;struct _fat_ptr _T3F;unsigned char*_T40;unsigned char*_T41;unsigned char _T42;int _T43;int _T44;struct Cyc_AP_T*_T45;struct _fat_ptr _T46;char*_T47;unsigned char*_T48;unsigned char _T49;int _T4A;int _T4B;struct Cyc_AP_T*_T4C;struct Cyc_AP_T*_T4D;struct Cyc_AP_T*_T4E;unsigned _T4F;struct Cyc_AP_T*_T50;int _T51;struct Cyc_AP_T*_T52;struct _fat_ptr _T53;unsigned char*_T54;unsigned char*_T55;unsigned char _T56;int _T57;struct Cyc_AP_T*_T58;struct _fat_ptr _T59;char*_T5A;unsigned char*_T5B;unsigned char _T5C;int _T5D;int _T5E;struct Cyc_AP_T*_T5F;struct Cyc_AP_T*_T60;struct Cyc_AP_T*_T61;struct Cyc_AP_T*_T62;int _T63;struct Cyc_AP_T*_T64;struct _fat_ptr _T65;unsigned char*_T66;unsigned char*_T67;unsigned char _T68;int _T69;struct Cyc_AP_T*_T6A;struct _fat_ptr _T6B;char*_T6C;unsigned char*_T6D;unsigned char _T6E;int _T6F;int _T70;struct Cyc_AP_T*_T71;struct Cyc_AP_T*_T72;struct Cyc_AP_T*_T73;
struct Cyc_AP_T*z;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL60;goto _TL61;_TL60: _T3=Cyc___assert_fail;{int(*_T74)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T74;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,194U);_TL61: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TL62;goto _TL63;_TL62: _T9=Cyc___assert_fail;{int(*_T74)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T74;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,195U);_TL63: _TC=y;_TD=_TC->sign;
if(_TD!=1)goto _TL64;goto _TL65;_TL64: _TF=Cyc___assert_fail;{int(*_T74)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_TF;_TE=_T74;}_T10=_tag_fat("y->sign == 1",sizeof(char),13U);_T11=_tag_fat("ap.cyc",sizeof(char),7U);_TE(_T10,_T11,196U);_TL65: _T12=p;_T13=(unsigned)_T12;
if(_T13)goto _TL69;else{goto _TL68;}_TL69: _T14=p;_T15=_T14->sign;if(_T15==1)goto _TL6B;else{goto _TL66;}_TL6B: _T16=p;_T17=_T16->ndigits;if(_T17==1)goto _TL6C;else{goto _TL6A;}_TL6C: _T18=p;_T19=_T18->digits;_T1A=_check_fat_subscript(_T19,sizeof(unsigned char),0);_T1B=(unsigned char*)_T1A;_T1C=*_T1B;_T1D=(int)_T1C;if(_T1D==0)goto _TL66;else{goto _TL6A;}_TL6A: _T1E=p;_T1F=_T1E->ndigits;if(_T1F==1)goto _TL6D;else{goto _TL68;}_TL6D: _T20=p;_T21=_T20->digits;_T22=_T21.curr;_T23=(unsigned char*)_T22;_T24=_T23[0];_T25=(int)_T24;if(_T25==1)goto _TL66;else{goto _TL68;}_TL68: goto _TL67;_TL66: _T27=Cyc___assert_fail;{int(*_T74)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T27;_T26=_T74;}_T28=_tag_fat("!p || p->sign==1 && !((p)->ndigits==1 && (p)->digits[0]==0) && !((p)->ndigits==1 && (p)->digits[0]==1)",sizeof(char),103U);_T29=_tag_fat("ap.cyc",sizeof(char),7U);_T26(_T28,_T29,197U);_TL67: _T2A=x;_T2B=_T2A->ndigits;
if(_T2B!=1)goto _TL6E;_T2C=x;_T2D=_T2C->digits;_T2E=_check_fat_subscript(_T2D,sizeof(unsigned char),0);_T2F=(unsigned char*)_T2E;_T30=*_T2F;_T31=(int)_T30;if(_T31!=0)goto _TL6E;_T32=
Cyc_AP_new(0);return _T32;_TL6E: _T33=y;_T34=_T33->ndigits;
if(_T34!=1)goto _TL70;_T35=y;_T36=_T35->digits;_T37=_check_fat_subscript(_T36,sizeof(unsigned char),0);_T38=(unsigned char*)_T37;_T39=*_T38;_T3A=(int)_T39;if(_T3A!=0)goto _TL70;_T3B=
Cyc_AP_new(1);return _T3B;_TL70: _T3C=x;_T3D=_T3C->ndigits;
if(_T3D!=1)goto _TL72;_T3E=x;_T3F=_T3E->digits;_T40=_T3F.curr;_T41=(unsigned char*)_T40;_T42=_T41[0];_T43=(int)_T42;if(_T43!=1)goto _TL72;_T45=y;_T46=_T45->digits;_T47=_check_fat_subscript(_T46,sizeof(unsigned char),0);_T48=(unsigned char*)_T47;_T49=*_T48;_T4A=(int)_T49;_T4B=_T4A & 1;
if(_T4B!=0)goto _TL74;_T44=1;goto _TL75;_TL74: _T4C=x;_T44=_T4C->sign;_TL75: _T4D=Cyc_AP_new(_T44);return _T4D;_TL72: _T4E=p;_T4F=(unsigned)_T4E;
if(!_T4F)goto _TL76;_T50=y;_T51=_T50->ndigits;
if(_T51!=1)goto _TL78;_T52=y;_T53=_T52->digits;_T54=_T53.curr;_T55=(unsigned char*)_T54;_T56=_T55[0];_T57=(int)_T56;if(_T57!=1)goto _TL78;
z=Cyc_AP_mod(x,p);goto _TL79;
# 208
_TL78:{struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,p);
z=Cyc_mulmod(t,t,p);_T58=y;_T59=_T58->digits;_T5A=_check_fat_subscript(_T59,sizeof(unsigned char),0);_T5B=(unsigned char*)_T5A;_T5C=*_T5B;_T5D=(int)_T5C;_T5E=_T5D & 1;
if(_T5E==0)goto _TL7A;else{goto _TL7C;}
_TL7C: y2=Cyc_AP_mod(x,p);_T5F=y2;t=z;_T60=t;_T61=p;z=Cyc_mulmod(_T5F,_T60,_T61);goto _TL7B;_TL7A: _TL7B:;}_TL79: goto _TL77;
# 215
_TL76: _T62=y;_T63=_T62->ndigits;if(_T63!=1)goto _TL7D;_T64=y;_T65=_T64->digits;_T66=_T65.curr;_T67=(unsigned char*)_T66;_T68=_T67[0];_T69=(int)_T68;if(_T69!=1)goto _TL7D;
z=Cyc_AP_addi(x,0);goto _TL7E;
# 218
_TL7D:{struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,0);
z=Cyc_AP_mul(t,t);_T6A=y;_T6B=_T6A->digits;_T6C=_check_fat_subscript(_T6B,sizeof(unsigned char),0);_T6D=(unsigned char*)_T6C;_T6E=*_T6D;_T6F=(int)_T6E;_T70=_T6F & 1;
if(_T70==0)goto _TL7F;else{goto _TL81;}
_TL81: _T71=x;t=z;_T72=t;z=Cyc_AP_mul(_T71,_T72);goto _TL80;_TL7F: _TL80:;}_TL7E: _TL77: _T73=z;
# 224
return _T73;}
# 226
int Cyc_AP_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;struct Cyc_AP_T*_TE;int _TF;int _T10;struct Cyc_AP_T*_T11;int _T12;struct Cyc_AP_T*_T13;int _T14;int _T15;int _T16;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL82;goto _TL83;_TL82: _T3=Cyc___assert_fail;{int(*_T17)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T17;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,227U);_TL83: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TL84;goto _TL85;_TL84: _T9=Cyc___assert_fail;{int(*_T17)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T17;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,228U);_TL85: _TC=x;_TD=_TC->sign;_TE=y;_TF=_TE->sign;_T10=_TD ^ _TF;
if(_T10==0)goto _TL86;else{goto _TL88;}
_TL88: _T11=x;_T12=_T11->sign;return _T12;
_TL86: _T13=x;_T14=_T13->sign;if(_T14!=1)goto _TL89;_T15=
Cyc_cmp(x,y);return _T15;
# 234
_TL89: _T16=Cyc_cmp(y,x);return _T16;}
# 236
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*x,long y){unsigned _T0;int _T1;struct Cyc_AP_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;_T0=sizeof(unsigned long)/ sizeof(unsigned char);_T1=(int)_T0;{
struct Cyc_AP_T*t=Cyc_mk(_T1);_T2=x;_T3=
Cyc_set(t,y);_T4=Cyc_AP_add(_T2,_T3);return _T4;}}
# 240
struct Cyc_AP_T*Cyc_AP_subi(struct Cyc_AP_T*x,long y){unsigned _T0;int _T1;struct Cyc_AP_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;_T0=sizeof(unsigned long)/ sizeof(unsigned char);_T1=(int)_T0;{
struct Cyc_AP_T*t=Cyc_mk(_T1);_T2=x;_T3=
Cyc_set(t,y);_T4=Cyc_AP_sub(_T2,_T3);return _T4;}}
# 244
struct Cyc_AP_T*Cyc_AP_muli(struct Cyc_AP_T*x,long y){unsigned _T0;int _T1;struct Cyc_AP_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;_T0=sizeof(unsigned long)/ sizeof(unsigned char);_T1=(int)_T0;{
struct Cyc_AP_T*t=Cyc_mk(_T1);_T2=x;_T3=
Cyc_set(t,y);_T4=Cyc_AP_mul(_T2,_T3);return _T4;}}
# 248
struct Cyc_AP_T*Cyc_AP_divi(struct Cyc_AP_T*x,long y){unsigned _T0;int _T1;struct Cyc_AP_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;_T0=sizeof(unsigned long)/ sizeof(unsigned char);_T1=(int)_T0;{
struct Cyc_AP_T*t=Cyc_mk(_T1);_T2=x;_T3=
Cyc_set(t,y);_T4=Cyc_AP_div(_T2,_T3);return _T4;}}
# 252
int Cyc_AP_cmpi(struct Cyc_AP_T*x,long y){unsigned _T0;int _T1;struct Cyc_AP_T*_T2;struct Cyc_AP_T*_T3;int _T4;_T0=sizeof(unsigned long)/ sizeof(unsigned char);_T1=(int)_T0;{
struct Cyc_AP_T*t=Cyc_mk(_T1);_T2=x;_T3=
Cyc_set(t,y);_T4=Cyc_AP_cmp(_T2,_T3);return _T4;}}
# 256
long Cyc_AP_modi(struct Cyc_AP_T*x,long y){unsigned _T0;int _T1;struct Cyc_AP_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;int _T5;struct Cyc_AP_T*_T6;struct _fat_ptr _T7;unsigned long _T8;long _T9;
long rem;
struct Cyc_AP_T*r;_T0=sizeof(unsigned long)/ sizeof(unsigned char);_T1=(int)_T0;{
struct Cyc_AP_T*t=Cyc_mk(_T1);_T2=x;_T3=
Cyc_set(t,y);r=Cyc_AP_mod(_T2,_T3);_T4=r;_T5=_T4->ndigits;_T6=r;_T7=_T6->digits;_T8=
Cyc_XP_toint(_T5,_T7);rem=(long)_T8;_T9=rem;
return _T9;}}
# 264
struct Cyc_AP_T*Cyc_AP_lshift(struct Cyc_AP_T*x,int s){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;int(*_T6)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T7)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_AP_T*_TA;int _TB;int _TC;int _TD;int _TE;int _TF;int _T10;struct Cyc_AP_T*_T11;int _T12;struct Cyc_AP_T*_T13;struct _fat_ptr _T14;struct Cyc_AP_T*_T15;int _T16;struct Cyc_AP_T*_T17;struct _fat_ptr _T18;int _T19;struct Cyc_AP_T*_T1A;struct Cyc_AP_T*_T1B;struct Cyc_AP_T*_T1C;struct Cyc_AP_T*_T1D;int _T1E;struct Cyc_AP_T*_T1F;
struct Cyc_AP_T*z;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL8B;goto _TL8C;_TL8B: _T3=Cyc___assert_fail;{int(*_T20)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T20;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,266U);_TL8C:
 if(s < 0)goto _TL8D;goto _TL8E;_TL8D: _T7=Cyc___assert_fail;{int(*_T20)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T7;_T6=_T20;}_T8=_tag_fat("s >= 0",sizeof(char),7U);_T9=_tag_fat("ap.cyc",sizeof(char),7U);_T6(_T8,_T9,267U);_TL8E: _TA=x;_TB=_TA->ndigits;_TC=s + 7;_TD=~ 7;_TE=_TC & _TD;_TF=_TE / 8;_T10=_TB + _TF;
z=Cyc_mk(_T10);_T11=z;_T12=_T11->size;_T13=z;_T14=_T13->digits;_T15=x;_T16=_T15->ndigits;_T17=x;_T18=_T17->digits;_T19=s;
Cyc_XP_lshift(_T12,_T14,_T16,_T18,_T19,0);_T1A=z;_T1B=x;
# 271
_T1A->sign=_T1B->sign;_T1C=z;_T1D=z;_T1E=_T1D->size;_T1F=
Cyc_normalize(_T1C,_T1E);return _T1F;}
# 274
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*x,int s){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;int(*_T6)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T7)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;struct Cyc_AP_T*_TB;int _TC;int _TD;struct Cyc_AP_T*_TE;struct Cyc_AP_T*_TF;int _T10;int _T11;int _T12;struct Cyc_AP_T*_T13;int _T14;struct Cyc_AP_T*_T15;struct _fat_ptr _T16;struct Cyc_AP_T*_T17;int _T18;struct Cyc_AP_T*_T19;struct _fat_ptr _T1A;int _T1B;struct Cyc_AP_T*_T1C;struct Cyc_AP_T*_T1D;int _T1E;struct Cyc_AP_T*_T1F;int _T20;struct Cyc_AP_T*_T21;int _T22;struct Cyc_AP_T*_T23;struct _fat_ptr _T24;char*_T25;unsigned char*_T26;unsigned char _T27;int _T28;struct Cyc_AP_T*_T29;struct Cyc_AP_T*_T2A;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TL8F;goto _TL90;_TL8F: _T3=Cyc___assert_fail;{int(*_T2B)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T2B;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,275U);_TL90:
 if(s < 0)goto _TL91;goto _TL92;_TL91: _T7=Cyc___assert_fail;{int(*_T2B)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T7;_T6=_T2B;}_T8=_tag_fat("s >= 0",sizeof(char),7U);_T9=_tag_fat("ap.cyc",sizeof(char),7U);_T6(_T8,_T9,276U);_TL92: _TA=s;_TB=x;_TC=_TB->ndigits;_TD=8 * _TC;
if(_TA < _TD)goto _TL93;_TE=
Cyc_AP_new(0);return _TE;
# 280
_TL93: _TF=x;_T10=_TF->ndigits;_T11=s / 8;_T12=_T10 - _T11;{struct Cyc_AP_T*z=Cyc_mk(_T12);_T13=z;_T14=_T13->size;_T15=z;_T16=_T15->digits;_T17=x;_T18=_T17->ndigits;_T19=x;_T1A=_T19->digits;_T1B=s;
Cyc_XP_rshift(_T14,_T16,_T18,_T1A,_T1B,0);_T1C=z;_T1D=z;_T1E=_T1D->size;
# 283
Cyc_normalize(_T1C,_T1E);_T1F=z;_T21=z;_T22=_T21->ndigits;
if(_T22!=1)goto _TL95;_T23=z;_T24=_T23->digits;_T25=_check_fat_subscript(_T24,sizeof(unsigned char),0);_T26=(unsigned char*)_T25;_T27=*_T26;_T28=(int)_T27;if(_T28!=0)goto _TL95;_T20=1;goto _TL96;_TL95: _T29=x;_T20=_T29->sign;_TL96: _T1F->sign=_T20;_T2A=z;
return _T2A;}}
# 288
struct Cyc_AP_T*Cyc_AP_and(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;int _T1;struct Cyc_AP_T*_T2;int _T3;struct Cyc_AP_T*_T4;struct Cyc_AP_T*_T5;int _T6;struct Cyc_AP_T*_T7;int _T8;struct Cyc_AP_T*_T9;struct _fat_ptr _TA;struct Cyc_AP_T*_TB;struct _fat_ptr _TC;struct Cyc_AP_T*_TD;struct _fat_ptr _TE;struct Cyc_AP_T*_TF;struct Cyc_AP_T*_T10;int _T11;struct Cyc_AP_T*_T12;struct Cyc_AP_T*_T13;
int i;
struct Cyc_AP_T*z;_T0=
_check_null(x);_T1=_T0->ndigits;_T2=_check_null(y);_T3=_T2->ndigits;if(_T1 >= _T3)goto _TL97;_T4=
Cyc_AP_and(y,x);return _T4;_TL97: _T5=y;_T6=_T5->ndigits;
z=Cyc_mk(_T6);_T7=y;_T8=_T7->ndigits;_T9=z;_TA=_T9->digits;_TB=x;_TC=_TB->digits;_TD=y;_TE=_TD->digits;
Cyc_XP_and(_T8,_TA,_TC,_TE);_TF=z;_T10=z;_T11=_T10->size;
Cyc_normalize(_TF,_T11);_T12=z;
_T12->sign=1;_T13=z;
return _T13;}
# 299
struct Cyc_AP_T*Cyc_AP_or(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;int _T1;struct Cyc_AP_T*_T2;int _T3;struct Cyc_AP_T*_T4;struct Cyc_AP_T*_T5;int _T6;struct Cyc_AP_T*_T7;int _T8;struct Cyc_AP_T*_T9;struct _fat_ptr _TA;struct Cyc_AP_T*_TB;struct _fat_ptr _TC;struct Cyc_AP_T*_TD;struct _fat_ptr _TE;struct Cyc_AP_T*_TF;int _T10;struct Cyc_AP_T*_T11;int _T12;struct Cyc_AP_T*_T13;struct Cyc_AP_T*_T14;int _T15;struct Cyc_AP_T*_T16;struct Cyc_AP_T*_T17;
int i;
struct Cyc_AP_T*z;_T0=
_check_null(x);_T1=_T0->ndigits;_T2=_check_null(y);_T3=_T2->ndigits;if(_T1 >= _T3)goto _TL99;_T4=
Cyc_AP_or(y,x);return _T4;_TL99: _T5=x;_T6=_T5->ndigits;
z=Cyc_mk(_T6);_T7=y;_T8=_T7->ndigits;_T9=z;_TA=_T9->digits;_TB=x;_TC=_TB->digits;_TD=y;_TE=_TD->digits;
Cyc_XP_or(_T8,_TA,_TC,_TE);_TF=y;
i=_TF->ndigits;_TL9E: _T10=i;_T11=x;_T12=_T11->ndigits;if(_T10 < _T12)goto _TL9C;else{goto _TL9D;}
_TL9C: _T13=z;_T14=z;_T15=_T14->size;Cyc_normalize(_T13,_T15);
# 306
i=i + 1;goto _TL9E;_TL9D: _T16=z;
# 308
_T16->sign=1;_T17=z;
return _T17;}
# 311
struct Cyc_AP_T*Cyc_AP_xor(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;int _T1;struct Cyc_AP_T*_T2;int _T3;struct Cyc_AP_T*_T4;struct Cyc_AP_T*_T5;int _T6;struct Cyc_AP_T*_T7;int _T8;struct Cyc_AP_T*_T9;struct _fat_ptr _TA;struct Cyc_AP_T*_TB;struct _fat_ptr _TC;struct Cyc_AP_T*_TD;struct _fat_ptr _TE;struct Cyc_AP_T*_TF;int _T10;struct Cyc_AP_T*_T11;int _T12;struct Cyc_AP_T*_T13;struct Cyc_AP_T*_T14;int _T15;struct Cyc_AP_T*_T16;struct Cyc_AP_T*_T17;
int i;
struct Cyc_AP_T*z;_T0=
_check_null(x);_T1=_T0->ndigits;_T2=_check_null(y);_T3=_T2->ndigits;if(_T1 >= _T3)goto _TL9F;_T4=
Cyc_AP_xor(y,x);return _T4;_TL9F: _T5=x;_T6=_T5->ndigits;
z=Cyc_mk(_T6);_T7=y;_T8=_T7->ndigits;_T9=z;_TA=_T9->digits;_TB=x;_TC=_TB->digits;_TD=y;_TE=_TD->digits;
Cyc_XP_xor(_T8,_TA,_TC,_TE);_TF=y;
i=_TF->ndigits;_TLA4: _T10=i;_T11=x;_T12=_T11->ndigits;if(_T10 < _T12)goto _TLA2;else{goto _TLA3;}
_TLA2: _T13=z;_T14=z;_T15=_T14->size;Cyc_normalize(_T13,_T15);
# 318
i=i + 1;goto _TLA4;_TLA3: _T16=z;
# 320
_T16->sign=1;_T17=z;
return _T17;}
# 323
struct Cyc_AP_T*Cyc_AP_fromint(long x){unsigned _T0;int _T1;struct Cyc_AP_T*_T2;_T0=sizeof(unsigned long)/ sizeof(unsigned char);_T1=(int)_T0;{
struct Cyc_AP_T*t=Cyc_mk(_T1);_T2=
Cyc_set(t,x);return _T2;}}
# 327
long Cyc_AP_toint(struct Cyc_AP_T*x){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;int _T7;struct Cyc_AP_T*_T8;struct _fat_ptr _T9;unsigned long _TA;long _TB;unsigned _TC;unsigned _TD;struct Cyc_AP_T*_TE;int _TF;int _T10;unsigned long _T11;long _T12;long _T13;unsigned long _T14;long _T15;
unsigned long u;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TLA5;goto _TLA6;_TLA5: _T3=Cyc___assert_fail;{int(*_T16)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T16;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,329U);_TLA6: _T6=x;_T7=_T6->ndigits;_T8=x;_T9=_T8->digits;_TA=
Cyc_XP_toint(_T7,_T9);_TB=Cyc_long_max;_TC=(unsigned)_TB;_TD=_TC + 1U;u=_TA % _TD;_TE=x;_TF=_TE->sign;_T10=- 1;
if(_TF!=_T10)goto _TLA7;_T11=u;_T12=(long)_T11;_T13=- _T12;
return _T13;
# 334
_TLA7: _T14=u;_T15=(long)_T14;return _T15;}
# 336
struct Cyc_AP_T*Cyc_AP_fromstr(const char*str,int base){struct _fat_ptr _T0;void*_T1;void*_T2;unsigned _T3;struct _fat_ptr _T4;char*_T5;unsigned _T6;int(*_T7)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T9;struct _fat_ptr _TA;int(*_TB)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_TC)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;char*_T10;const char*_T11;char _T12;int _T13;struct _fat_ptr _T14;unsigned char*_T15;const char*_T16;char _T17;int _T18;int _T19;struct _fat_ptr*_T1A;struct _fat_ptr _T1B;unsigned char*_T1C;const char*_T1D;char _T1E;int _T1F;struct _fat_ptr _T20;unsigned char*_T21;const char*_T22;char _T23;int _T24;struct _fat_ptr*_T25;struct _fat_ptr _T26;unsigned char*_T27;const char*_T28;struct _fat_ptr _T29;char*_T2A;const char*_T2B;char _T2C;int _T2D;struct _fat_ptr _T2E;char*_T2F;const char*_T30;char _T31;int _T32;struct _fat_ptr*_T33;struct _fat_ptr _T34;char*_T35;struct _fat_ptr _T36;char*_T37;const char*_T38;char _T39;int _T3A;struct _fat_ptr _T3B;unsigned char*_T3C;const char*_T3D;char _T3E;int _T3F;struct _fat_ptr _T40;unsigned char*_T41;const char*_T42;char _T43;int _T44;int _T45;struct _fat_ptr _T46;unsigned char*_T47;const char*_T48;char _T49;int _T4A;struct _fat_ptr _T4B;unsigned char*_T4C;const char*_T4D;char _T4E;int _T4F;struct _fat_ptr _T50;unsigned char*_T51;const char*_T52;char _T53;int _T54;int _T55;int _T56;struct _fat_ptr _T57;unsigned char*_T58;const char*_T59;char _T5A;int _T5B;struct _fat_ptr _T5C;unsigned char*_T5D;const char*_T5E;char _T5F;int _T60;struct _fat_ptr _T61;unsigned char*_T62;const char*_T63;char _T64;int _T65;int _T66;int _T67;struct _fat_ptr*_T68;int _T69;int _T6A;int _T6B;int _T6C;int _T6D;int _T6E;int _T6F;struct _fat_ptr _T70;void*_T71;void*_T72;unsigned _T73;struct Cyc_AP_T*_T74;int _T75;struct Cyc_AP_T*_T76;struct _fat_ptr _T77;struct _fat_ptr _T78;char*_T79;const char*_T7A;int _T7B;int(*_T7C)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T7D)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T7E;struct _fat_ptr _T7F;struct Cyc_AP_T*_T80;struct Cyc_AP_T*_T81;int _T82;struct Cyc_AP_T*_T83;int _T84;struct Cyc_AP_T*_T85;int _T86;struct Cyc_AP_T*_T87;struct _fat_ptr _T88;char*_T89;unsigned char*_T8A;unsigned char _T8B;int _T8C;char _T8D;int _T8E;struct Cyc_AP_T*_T8F;
struct Cyc_AP_T*z;{const char*_T90=str;_T1=(void*)_T90;_T2=(void*)_T90;_T3=_get_zero_arr_size_char(_T2,1U);_T0=_tag_fat(_T1,sizeof(char),_T3);}{
struct _fat_ptr p=_T0;
char sign='\000';
int carry;_T4=p;_T5=_T4.curr;_T6=(unsigned)_T5;
if(!_T6)goto _TLA9;goto _TLAA;_TLA9: _T8=Cyc___assert_fail;{int(*_T90)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T8;_T7=_T90;}_T9=_tag_fat("p",sizeof(char),2U);_TA=_tag_fat("ap.cyc",sizeof(char),7U);_T7(_T9,_TA,341U);_TLAA:
 if(base < 2)goto _TLAB;if(base > 36)goto _TLAB;goto _TLAC;_TLAB: _TC=Cyc___assert_fail;{int(*_T90)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_TC;_TB=_T90;}_TD=_tag_fat("base >= 2 && base <= 36",sizeof(char),24U);_TE=_tag_fat("ap.cyc",sizeof(char),7U);_TB(_TD,_TE,342U);_TLAC:
 _TLAD: _TF=p;_T10=_check_fat_subscript(_TF,sizeof(char),0U);_T11=(const char*)_T10;_T12=*_T11;_T13=(int)_T12;if(_T13)goto _TLB0;else{goto _TLAF;}_TLB0: _T14=p;_T15=_T14.curr;_T16=(const char*)_T15;_T17=*_T16;_T18=(int)_T17;_T19=isspace(_T18);if(_T19)goto _TLAE;else{goto _TLAF;}
_TLAE: _T1A=& p;_fat_ptr_inplace_plus(_T1A,sizeof(char),1);goto _TLAD;_TLAF: _T1B=p;_T1C=_T1B.curr;_T1D=(const char*)_T1C;_T1E=*_T1D;_T1F=(int)_T1E;
if(_T1F==45)goto _TLB3;else{goto _TLB4;}_TLB4: _T20=p;_T21=_T20.curr;_T22=(const char*)_T21;_T23=*_T22;_T24=(int)_T23;if(_T24==43)goto _TLB3;else{goto _TLB1;}
_TLB3: _T25=& p;_T26=_fat_ptr_inplace_plus_post(_T25,sizeof(char),1);_T27=_T26.curr;_T28=(const char*)_T27;sign=*_T28;goto _TLB2;_TLB1: _TLB2:{
# 348
const char*start;
int k;int n=0;
_TLB8: _T29=p;_T2A=_check_fat_subscript(_T29,sizeof(char),0U);_T2B=(const char*)_T2A;_T2C=*_T2B;_T2D=(int)_T2C;if(_T2D==48)goto _TLB9;else{goto _TLB7;}_TLB9: _T2E=p;_T2F=_check_fat_subscript(_T2E,sizeof(char),1);_T30=(const char*)_T2F;_T31=*_T30;_T32=(int)_T31;if(_T32==48)goto _TLB6;else{goto _TLB7;}
_TLB6: _T33=& p;
# 350
_fat_ptr_inplace_plus(_T33,sizeof(char),1);goto _TLB8;_TLB7: _T34=p;_T35=_untag_fat_ptr(_T34,sizeof(char),1U);
# 352
start=(const char*)_T35;
_TLBD: _T36=p;_T37=_check_fat_subscript(_T36,sizeof(char),0U);_T38=(const char*)_T37;_T39=*_T38;_T3A=(int)_T39;if(48 <= _T3A)goto _TLC1;else{goto _TLBF;}_TLC1: _T3B=p;_T3C=_T3B.curr;_T3D=(const char*)_T3C;_T3E=*_T3D;_T3F=(int)_T3E;if(_T3F <= 57)goto _TLC0;else{goto _TLBF;}_TLC0: _T40=p;_T41=_T40.curr;_T42=(const char*)_T41;_T43=*_T42;_T44=(int)_T43;_T45=48 + base;if(_T44 < _T45)goto _TLBB;else{goto _TLBF;}_TLBF: _T46=p;_T47=_T46.curr;_T48=(const char*)_T47;_T49=*_T48;_T4A=(int)_T49;if(97 <= _T4A)goto _TLC3;else{goto _TLBE;}_TLC3: _T4B=p;_T4C=_T4B.curr;_T4D=(const char*)_T4C;_T4E=*_T4D;_T4F=(int)_T4E;if(_T4F <= 122)goto _TLC2;else{goto _TLBE;}_TLC2: _T50=p;_T51=_T50.curr;_T52=(const char*)_T51;_T53=*_T52;_T54=(int)_T53;_T55=97 + base;_T56=_T55 - 10;if(_T54 < _T56)goto _TLBB;else{goto _TLBE;}_TLBE: _T57=p;_T58=_T57.curr;_T59=(const char*)_T58;_T5A=*_T59;_T5B=(int)_T5A;if(65 <= _T5B)goto _TLC5;else{goto _TLBC;}_TLC5: _T5C=p;_T5D=_T5C.curr;_T5E=(const char*)_T5D;_T5F=*_T5E;_T60=(int)_T5F;if(_T60 <= 90)goto _TLC4;else{goto _TLBC;}_TLC4: _T61=p;_T62=_T61.curr;_T63=(const char*)_T62;_T64=*_T63;_T65=(int)_T64;_T66=65 + base;_T67=_T66 - 10;if(_T65 < _T67)goto _TLBB;else{goto _TLBC;}
# 356
_TLBB: n=n + 1;_T68=& p;
# 355
_fat_ptr_inplace_plus(_T68,sizeof(char),1);goto _TLBD;_TLBC:
# 357
 k=1;_TLC9: _T69=1 << k;_T6A=base;if(_T69 < _T6A)goto _TLC7;else{goto _TLC8;}
_TLC7:
# 357
 k=k + 1;goto _TLC9;_TLC8: _T6B=k * n;_T6C=_T6B + 7;_T6D=~ 7;_T6E=_T6C & _T6D;_T6F=_T6E / 8;
# 359
z=Cyc_mk(_T6F);{const char*_T90=start;_T71=(void*)_T90;_T72=(void*)_T90;_T73=_get_zero_arr_size_char(_T72,1U);_T70=_tag_fat(_T71,sizeof(char),_T73);}
p=_T70;}_T74=z;_T75=_T74->size;_T76=z;_T77=_T76->digits;_T78=p;_T79=_untag_fat_ptr(_T78,sizeof(char),1U);_T7A=(const char*)_T79;_T7B=base;
# 362
carry=Cyc_XP_fromstr(_T75,_T77,_T7A,_T7B);
# 364
if(carry!=0)goto _TLCA;goto _TLCB;_TLCA: _T7D=Cyc___assert_fail;{int(*_T90)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T7D;_T7C=_T90;}_T7E=_tag_fat("carry == 0",sizeof(char),11U);_T7F=_tag_fat("ap.cyc",sizeof(char),7U);_T7C(_T7E,_T7F,364U);_TLCB: _T80=z;_T81=z;_T82=_T81->size;
Cyc_normalize(_T80,_T82);_T83=z;_T85=z;_T86=_T85->ndigits;
if(_T86==1)goto _TLD0;else{goto _TLCF;}_TLD0: _T87=z;_T88=_T87->digits;_T89=_check_fat_subscript(_T88,sizeof(unsigned char),0);_T8A=(unsigned char*)_T89;_T8B=*_T8A;_T8C=(int)_T8B;if(_T8C==0)goto _TLCE;else{goto _TLCF;}_TLCF: _T8D=sign;_T8E=(int)_T8D;if(_T8E!=45)goto _TLCE;else{goto _TLCC;}_TLCE: _T84=1;goto _TLCD;_TLCC: _T84=- 1;_TLCD: _T83->sign=_T84;_T8F=z;
return _T8F;}}
# 369
char*Cyc_AP_tostr(struct Cyc_AP_T*x,int base){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;int(*_T6)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T7)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;int _TB;struct Cyc_AP_T*_TC;int _TD;int _TE;int _TF;int _T10;int _T11;struct Cyc_AP_T*_T12;int _T13;int _T14;struct _fat_ptr _T15;void*_T16;struct _fat_ptr _T17;struct Cyc_AP_T*_T18;int _T19;unsigned _T1A;void*_T1B;struct _fat_ptr _T1C;struct Cyc_AP_T*_T1D;struct _fat_ptr _T1E;struct Cyc_AP_T*_T1F;int _T20;unsigned _T21;unsigned _T22;unsigned _T23;int _T24;struct Cyc_AP_T*_T25;int _T26;unsigned _T27;unsigned _T28;unsigned _T29;unsigned _T2A;unsigned _T2B;unsigned _T2C;struct Cyc_AP_T*_T2D;int _T2E;int _T2F;struct _fat_ptr _T30;char*_T31;char*_T32;unsigned _T33;unsigned char*_T34;char*_T35;struct _fat_ptr _T36;struct _fat_ptr _T37;int _T38;int _T39;struct Cyc_AP_T*_T3A;int _T3B;struct _fat_ptr _T3C;struct _fat_ptr _T3D;int _T3E;int _T3F;struct Cyc_AP_T*_T40;int _T41;struct _fat_ptr _T42;struct _fat_ptr _T43;char*_T44;char*_T45;
struct _fat_ptr q;
struct _fat_ptr str;
int size;int k;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TLD1;goto _TLD2;_TLD1: _T3=Cyc___assert_fail;{int(*_T46)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T46;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,373U);_TLD2:
 if(base < 2)goto _TLD3;if(base > 36)goto _TLD3;goto _TLD4;_TLD3: _T7=Cyc___assert_fail;{int(*_T46)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T7;_T6=_T46;}_T8=_tag_fat("base >= 2 && base <= 36",sizeof(char),24U);_T9=_tag_fat("ap.cyc",sizeof(char),7U);_T6(_T8,_T9,374U);_TLD4:
 k=5;_TLD8: _TA=1 << k;_TB=base;if(_TA > _TB)goto _TLD6;else{goto _TLD7;}
_TLD6:
# 375
 k=k + -1;goto _TLD8;_TLD7: _TC=x;_TD=_TC->ndigits;_TE=8 * _TD;_TF=k;_T10=_TE / _TF;_T11=_T10 + 1;
# 377
size=_T11 + 1;_T12=x;_T13=_T12->sign;_T14=- 1;
if(_T13!=_T14)goto _TLD9;
size=size + 1;goto _TLDA;_TLD9: _TLDA:{unsigned _T46=size;_T16=_cyccalloc_atomic(sizeof(char),_T46);_T15=_tag_fat(_T16,sizeof(char),_T46);}
str=_T15;_T18=x;_T19=_T18->ndigits;_T1A=(unsigned)_T19;{unsigned _T46=_T1A * sizeof(unsigned char);_T1B=_cycalloc_atomic(_T46);_T17=_tag_fat(_T1B,1U,_T46);}
q=_T17;_T1C=q;_T1D=x;_T1E=_T1D->digits;_T1F=x;_T20=_T1F->ndigits;_T21=(unsigned)_T20;_T22=sizeof(*((unsigned char*)x->digits.curr));_T23=_T21 / _T22;_T25=x;_T26=_T25->ndigits;_T27=(unsigned)_T26;_T28=sizeof(*((unsigned char*)x->digits.curr));_T29=_T27 % _T28;
if(_T29!=0U)goto _TLDB;_T24=0;goto _TLDC;_TLDB: _T24=1;_TLDC: _T2A=(unsigned)_T24;_T2B=_T23 + _T2A;_T2C=sizeof(*((unsigned char*)x->digits.curr));Cyc__memcpy(_T1C,_T1E,_T2B,_T2C);_T2D=x;_T2E=_T2D->sign;_T2F=- 1;
if(_T2E!=_T2F)goto _TLDD;_T30=str;{struct _fat_ptr _T46=_fat_ptr_plus(_T30,sizeof(char),0);_T31=_check_fat_subscript(_T46,sizeof(char),0U);_T32=(char*)_T31;{char _T47=*_T32;char _T48='-';_T33=_get_fat_size(_T46,sizeof(char));if(_T33!=1U)goto _TLDF;if(_T47!=0)goto _TLDF;if(_T48==0)goto _TLDF;_throw_arraybounds();goto _TLE0;_TLDF: _TLE0: _T34=_T46.curr;_T35=(char*)_T34;*_T35=_T48;}}_T36=str;_T37=
# 385
_fat_ptr_plus(_T36,sizeof(char),1);_T38=size - 1;_T39=base;_T3A=x;_T3B=_T3A->ndigits;_T3C=q;Cyc_XP_tostr(_T37,_T38,_T39,_T3B,_T3C);goto _TLDE;
# 387
_TLDD: _T3D=str;_T3E=size;_T3F=base;_T40=x;_T41=_T40->ndigits;_T42=q;Cyc_XP_tostr(_T3D,_T3E,_T3F,_T41,_T42);_TLDE: _T43=str;_T44=_untag_fat_ptr_check_bound(_T43,sizeof(char),1U);_T45=(char*)_T44;
return _T45;}
# 390
struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_AP_T*_T6;unsigned _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_AP_T*_TC;int _TD;struct Cyc_AP_T*_TE;struct _fat_ptr _TF;char*_T10;unsigned char*_T11;unsigned char _T12;int _T13;struct Cyc_AP_T*_T14;int _T15;struct Cyc_AP_T*_T16;struct _fat_ptr _T17;struct Cyc_AP_T*_T18;struct _fat_ptr _T19;struct Cyc_AP_T*_T1A;int _T1B;unsigned _T1C;unsigned _T1D;unsigned _T1E;int _T1F;struct Cyc_AP_T*_T20;int _T21;unsigned _T22;unsigned _T23;unsigned _T24;unsigned _T25;unsigned _T26;unsigned _T27;struct Cyc_AP_T*_T28;struct Cyc_AP_T*_T29;struct Cyc_AP_T*_T2A;struct Cyc_AP_T*_T2B;struct Cyc_AP_T*_T2C;
struct Cyc_AP_T*z;struct Cyc_AP_T*tmp;_T0=x;_T1=(unsigned)_T0;
if(!_T1)goto _TLE1;goto _TLE2;_TLE1: _T3=Cyc___assert_fail;{int(*_T2D)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T2D;}_T4=_tag_fat("x",sizeof(char),2U);_T5=_tag_fat("ap.cyc",sizeof(char),7U);_T2(_T4,_T5,392U);_TLE2: _T6=y;_T7=(unsigned)_T6;
if(!_T7)goto _TLE3;goto _TLE4;_TLE3: _T9=Cyc___assert_fail;{int(*_T2D)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T9;_T8=_T2D;}_TA=_tag_fat("y",sizeof(char),2U);_TB=_tag_fat("ap.cyc",sizeof(char),7U);_T8(_TA,_TB,393U);_TLE4:
 _TLE5: _TC=_check_null(y);_TD=_TC->ndigits;if(_TD==1)goto _TLE8;else{goto _TLE6;}_TLE8: _TE=y;_TF=_TE->digits;_T10=_check_fat_subscript(_TF,sizeof(unsigned char),0);_T11=(unsigned char*)_T10;_T12=*_T11;_T13=(int)_T12;if(_T13==0)goto _TLE7;else{goto _TLE6;}
_TLE6: tmp=Cyc_AP_mod(x,y);
x=y;
y=tmp;goto _TLE5;_TLE7: _T14=x;_T15=_T14->ndigits;
# 399
z=Cyc_mk(_T15);_T16=z;_T17=_T16->digits;_T18=x;_T19=_T18->digits;_T1A=x;_T1B=_T1A->ndigits;_T1C=(unsigned)_T1B;_T1D=sizeof(*((unsigned char*)x->digits.curr));_T1E=_T1C / _T1D;_T20=x;_T21=_T20->ndigits;_T22=(unsigned)_T21;_T23=sizeof(*((unsigned char*)x->digits.curr));_T24=_T22 % _T23;
if(_T24!=0U)goto _TLE9;_T1F=0;goto _TLEA;_TLE9: _T1F=1;_TLEA: _T25=(unsigned)_T1F;_T26=_T1E + _T25;_T27=sizeof(*((unsigned char*)x->digits.curr));Cyc__memcpy(_T17,_T19,_T26,_T27);_T28=z;_T29=x;
_T28->ndigits=_T29->ndigits;_T2A=z;_T2B=x;
_T2A->sign=_T2B->sign;_T2C=z;
return _T2C;}
# 405
struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*x,struct Cyc_AP_T*y){struct Cyc_AP_T*_T0;struct Cyc_AP_T*_T1;struct Cyc_AP_T*_T2;struct Cyc_AP_T*_T3;struct Cyc_AP_T*_T4;_T0=x;_T1=y;_T2=
Cyc_AP_gcd(x,y);_T3=Cyc_AP_div(_T1,_T2);_T4=Cyc_AP_mul(_T0,_T3);return _T4;}

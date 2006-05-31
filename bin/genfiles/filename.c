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

# 95 "core.h"
 struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 62 "string.h"
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 110 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);
# 35 "filename.cyc"
struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr s1,struct _fat_ptr s2){struct _fat_ptr _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;_T0=s1;_T1=
_tag_fat("/",sizeof(char),2U);_T2=s2;_T3=Cyc_strconcat(_T1,_T2);_T4=Cyc_strconcat(_T0,_T3);return _T4;}
# 39
struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr filename){struct _fat_ptr _T0;unsigned _T1;unsigned _T2;struct _fat_ptr _T3;int _T4;char*_T5;const char*_T6;char _T7;int _T8;struct Cyc_Core_Invalid_argument_exn_struct*_T9;void*_TA;struct _fat_ptr _TB;int _TC;unsigned long _TD;struct _fat_ptr _TE;_T0=filename;_T1=
_get_fat_size(_T0,sizeof(char));_T2=_T1 - 1U;{int i=(int)_T2;
_TL0: if(i >= 0)goto _TL3;else{goto _TL2;}_TL3: _T3=filename;_T4=i;_T5=_check_fat_subscript(_T3,sizeof(char),_T4);_T6=(const char*)_T5;_T7=*_T6;_T8=(int)_T7;if(_T8!=46)goto _TL1;else{goto _TL2;}
_TL1: i=i + -1;goto _TL0;_TL2:
 if(i >= 0)goto _TL4;{struct Cyc_Core_Invalid_argument_exn_struct*_TF=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TF->tag=Cyc_Core_Invalid_argument;
_TF->f1=_tag_fat("chop_extension",sizeof(char),15U);_T9=(struct Cyc_Core_Invalid_argument_exn_struct*)_TF;}_TA=(void*)_T9;_throw(_TA);goto _TL5;_TL4: _TL5: _TB=filename;_TC=i;_TD=(unsigned long)_TC;_TE=
Cyc_substring(_TB,0,_TD);return _TE;}}
# 50
struct _fat_ptr Cyc_Filename_dirname(struct _fat_ptr filename){struct _fat_ptr _T0;unsigned _T1;unsigned _T2;struct _fat_ptr _T3;int _T4;char*_T5;const char*_T6;char _T7;int _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;int _TB;unsigned long _TC;struct _fat_ptr _TD;_T0=filename;_T1=
_get_fat_size(_T0,sizeof(char));_T2=_T1 - 1U;{int i=(int)_T2;
_TL6: if(i >= 0)goto _TL9;else{goto _TL8;}_TL9: _T3=filename;_T4=i;_T5=_check_fat_subscript(_T3,sizeof(char),_T4);_T6=(const char*)_T5;_T7=*_T6;_T8=(int)_T7;if(_T8!=47)goto _TL7;else{goto _TL8;}
_TL7: i=i + -1;goto _TL6;_TL8:
 if(i >= 0)goto _TLA;_T9=Cyc_Core_new_string(0U);return _T9;_TLA: _TA=filename;_TB=i;_TC=(unsigned long)_TB;_TD=
Cyc_substring(_TA,0,_TC);return _TD;}}
# 59
struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr filename){struct _fat_ptr _T0;unsigned _T1;unsigned _T2;struct _fat_ptr _T3;int _T4;char*_T5;const char*_T6;char _T7;int _T8;struct _fat_ptr _T9;int _TA;struct _fat_ptr _TB;unsigned _TC;int _TD;unsigned _TE;unsigned _TF;struct _fat_ptr _T10;_T0=filename;_T1=
_get_fat_size(_T0,sizeof(char));_T2=_T1 - 1U;{int i=(int)_T2;
_TLC: if(i >= 0)goto _TLF;else{goto _TLE;}_TLF: _T3=filename;_T4=i;_T5=_check_fat_subscript(_T3,sizeof(char),_T4);_T6=(const char*)_T5;_T7=*_T6;_T8=(int)_T7;if(_T8!=47)goto _TLD;else{goto _TLE;}
_TLD: i=i + -1;goto _TLC;_TLE: _T9=filename;_TA=i + 1;_TB=filename;_TC=
_get_fat_size(_TB,sizeof(char));_TD=i + 1;_TE=(unsigned)_TD;_TF=_TC - _TE;_T10=Cyc_substring(_T9,_TA,_TF);return _T10;}}
# 66
int Cyc_Filename_check_suffix(struct _fat_ptr filename,struct _fat_ptr suffix){struct _fat_ptr _T0;unsigned _T1;unsigned _T2;struct _fat_ptr _T3;unsigned _T4;unsigned _T5;struct _fat_ptr _T6;int _T7;int _T8;char*_T9;const char*_TA;char _TB;int _TC;struct _fat_ptr _TD;int _TE;int _TF;char*_T10;const char*_T11;char _T12;int _T13;_T0=filename;_T1=
_get_fat_size(_T0,sizeof(char));_T2=_T1 - 1U;{int i=(int)_T2;_T3=suffix;_T4=
_get_fat_size(_T3,sizeof(char));_T5=_T4 - 1U;{int j=(int)_T5;
_TL10: if(i >= 0)goto _TL13;else{goto _TL12;}_TL13: if(j >= 0)goto _TL11;else{goto _TL12;}
_TL11: _T6=filename;_T7=i;i=_T7 + -1;_T8=_T7;_T9=_check_fat_subscript(_T6,sizeof(char),_T8);_TA=(const char*)_T9;_TB=*_TA;_TC=(int)_TB;_TD=suffix;_TE=j;j=_TE + -1;_TF=_TE;_T10=_check_fat_subscript(_TD,sizeof(char),_TF);_T11=(const char*)_T10;_T12=*_T11;_T13=(int)_T12;if(_TC==_T13)goto _TL14;return 0;_TL14: goto _TL10;_TL12:
 if(j < 0)goto _TL16;return 0;_TL16:
 return 1;}}}
# 75
struct _fat_ptr Cyc_Filename_gnuify(struct _fat_ptr filename){int _T0;struct _fat_ptr _T1;unsigned _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;char _T6;int _T7;int _T8;struct _fat_ptr _T9;unsigned _TA;unsigned _TB;struct _fat_ptr _TC;int _TD;unsigned _TE;char*_TF;unsigned _T10;int _T11;char*_T12;unsigned _T13;char*_T14;struct _fat_ptr _T15;unsigned char*_T16;char*_T17;char _T18;struct _fat_ptr _T19;char*_T1A;char*_T1B;unsigned _T1C;unsigned char*_T1D;char*_T1E;unsigned _T1F;unsigned char*_T20;char*_T21;struct _fat_ptr _T22;char*_T23;char*_T24;struct _fat_ptr _T25;unsigned char*_T26;const char*_T27;unsigned _T28;unsigned char*_T29;char*_T2A;struct _fat_ptr _T2B;unsigned _T2C;struct _fat_ptr _T2D;int _T2E;unsigned _T2F;char*_T30;unsigned _T31;int _T32;char*_T33;unsigned _T34;char*_T35;struct _fat_ptr _T36;int _T37;int _T38;char*_T39;const char*_T3A;struct _fat_ptr _T3B;int _T3C;int _T3D;char*_T3E;char*_T3F;char _T40;char _T41;int _T42;unsigned _T43;unsigned char*_T44;char*_T45;struct _fat_ptr _T46;_T1=filename;_T2=
_get_fat_size(_T1,sizeof(char));if(_T2 <= 1U)goto _TL18;_T3=filename;_T4=_T3.curr;_T5=(const char*)_T4;_T6=_T5[1];_T7=(int)_T6;_T0=_T7==58;goto _TL19;_TL18: _T0=0;_TL19: {int has_drive_name=_T0;
int i;int j;
struct _fat_ptr ans;
int ans_sz;_T8=has_drive_name;
if(!_T8)goto _TL1A;_T9=filename;_TA=
_get_fat_size(_T9,sizeof(char));_TB=_TA + 1U;ans_sz=(int)_TB;_TD=ans_sz;_TE=(unsigned)_TD;{unsigned _T47=_TE + 1U;_T10=_check_times(_T47,sizeof(char));{char*_T48=_cycalloc_atomic(_T10);_T11=ans_sz;{unsigned _T49=(unsigned)_T11;unsigned k;k=0;_TL1F: if(k < _T49)goto _TL1D;else{goto _TL1E;}_TL1D: _T13=k;_T12=_T48 + _T13;
*_T12='\000';k=k + 1;goto _TL1F;_TL1E: _T14=_T48 + _T49;*_T14=0;}_TF=(char*)_T48;}_TC=_tag_fat(_TF,sizeof(char),_T47);}ans=_TC;_T15=ans;{struct _fat_ptr _T47=_fat_ptr_plus(_T15,sizeof(char),0);_T16=_T47.curr;_T17=(char*)_T16;{char _T48=*_T17;_T19=ans;{struct _fat_ptr _T49=_fat_ptr_plus(_T19,sizeof(char),1);_T1A=_check_fat_subscript(_T49,sizeof(char),0U);_T1B=(char*)_T1A;{char _T4A=*_T1B;char _T4B='/';_T1C=_get_fat_size(_T49,sizeof(char));if(_T1C!=1U)goto _TL20;if(_T4A!=0)goto _TL20;if(_T4B==0)goto _TL20;_throw_arraybounds();goto _TL21;_TL20: _TL21: _T1D=_T49.curr;_T1E=(char*)_T1D;*_T1E=_T4B;_T18=*_T1E;}}{char _T49=_T18;_T1F=_get_fat_size(_T47,sizeof(char));if(_T1F!=1U)goto _TL22;if(_T48!=0)goto _TL22;if(_T49==0)goto _TL22;_throw_arraybounds();goto _TL23;_TL22: _TL23: _T20=_T47.curr;_T21=(char*)_T20;*_T21=_T49;}}}_T22=ans;{struct _fat_ptr _T47=_fat_ptr_plus(_T22,sizeof(char),2);_T23=_check_fat_subscript(_T47,sizeof(char),0U);_T24=(char*)_T23;{char _T48=*_T24;_T25=filename;_T26=_T25.curr;_T27=(const char*)_T26;{char _T49=_T27[0];_T28=_get_fat_size(_T47,sizeof(char));if(_T28!=1U)goto _TL24;if(_T48!=0)goto _TL24;if(_T49==0)goto _TL24;_throw_arraybounds();goto _TL25;_TL24: _TL25: _T29=_T47.curr;_T2A=(char*)_T29;*_T2A=_T49;}}}
# 85
i=3;
j=2;goto _TL1B;
# 88
_TL1A: _T2B=filename;_T2C=_get_fat_size(_T2B,sizeof(char));ans_sz=(int)_T2C;_T2E=ans_sz;_T2F=(unsigned)_T2E;{unsigned _T47=_T2F + 1U;_T31=_check_times(_T47,sizeof(char));{char*_T48=_cycalloc_atomic(_T31);_T32=ans_sz;{unsigned _T49=(unsigned)_T32;unsigned k;k=0;_TL29: if(k < _T49)goto _TL27;else{goto _TL28;}_TL27: _T34=k;_T33=_T48 + _T34;
*_T33='\000';k=k + 1;goto _TL29;_TL28: _T35=_T48 + _T49;*_T35=0;}_T30=(char*)_T48;}_T2D=_tag_fat(_T30,sizeof(char),_T47);}ans=_T2D;
i=0;
j=0;_TL1B:
# 93
 _TL2A: if(i < ans_sz)goto _TL2B;else{goto _TL2C;}
_TL2B: _T36=filename;_T37=j;j=_T37 + 1;_T38=_T37;_T39=_check_fat_subscript(_T36,sizeof(char),_T38);_T3A=(const char*)_T39;{char c=*_T3A;_T3B=ans;_T3C=i;
i=_T3C + 1;_T3D=_T3C;{struct _fat_ptr _T47=_fat_ptr_plus(_T3B,sizeof(char),_T3D);_T3E=_check_fat_subscript(_T47,sizeof(char),0U);_T3F=(char*)_T3E;{char _T48=*_T3F;_T41=c;_T42=(int)_T41;if(_T42!=92)goto _TL2D;_T40='/';goto _TL2E;_TL2D: _T40=c;_TL2E: {char _T49=_T40;_T43=_get_fat_size(_T47,sizeof(char));if(_T43!=1U)goto _TL2F;if(_T48!=0)goto _TL2F;if(_T49==0)goto _TL2F;_throw_arraybounds();goto _TL30;_TL2F: _TL30: _T44=_T47.curr;_T45=(char*)_T44;*_T45=_T49;}}}}goto _TL2A;_TL2C: _T46=ans;
# 97
return _T46;}}

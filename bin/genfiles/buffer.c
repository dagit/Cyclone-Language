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
void Cyc_Core_ufree(void*);struct Cyc_Buffer_t;
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 72 "string.h"
extern struct _fat_ptr Cyc_strncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
extern struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 110 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Buffer_t{struct _fat_ptr buffer;unsigned position;unsigned length;struct _fat_ptr initial_buffer;};
# 48 "buffer.cyc"
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned n){struct _fat_ptr _T0;struct _RegionHandle*_T1;unsigned _T2;void*_T3;struct Cyc_Buffer_t*_T4;struct Cyc_Buffer_t*_T5;
if(n <= 0U)goto _TL0;{unsigned _T6=n + 1U;_T1=Cyc_Core_heap_region;_T2=Cyc_Core_unique_qual;_T3=_region_calloc(_T1,_T2,sizeof(char),_T6);_T0=_tag_fat(_T3,sizeof(char),_T6);}{
struct _fat_ptr s=_T0;{struct Cyc_Buffer_t*_T6=_cycalloc(sizeof(struct Cyc_Buffer_t));
_T6->buffer=s;_T6->position=0U;_T6->length=n;_T6->initial_buffer=_tag_fat(0,0,0);_T4=(struct Cyc_Buffer_t*)_T6;}return _T4;}
# 53
_TL0:{struct Cyc_Buffer_t*_T6=_cycalloc(sizeof(struct Cyc_Buffer_t));_T6->buffer=_tag_fat(0,0,0);_T6->position=0U;_T6->length=0U;_T6->initial_buffer=_tag_fat(0,0,0);_T5=(struct Cyc_Buffer_t*)_T6;}return _T5;}
# 57
struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*b){struct Cyc_Buffer_t*_T0;struct Cyc_Buffer_t*_T1;struct _fat_ptr _T2;unsigned char*_T3;char*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_Buffer_t*_T7;unsigned _T8;struct Cyc_Buffer_t*_T9;struct Cyc_Buffer_t*_TA;struct _fat_ptr _TB;
struct _fat_ptr buf=_tag_fat(0,0,0);{
struct _fat_ptr _TC=buf;_T0=b;{struct _fat_ptr _TD=_T0->buffer;buf=_TD;_T1=b;_T1->buffer=_TC;}}_T2=buf;_T3=_T2.curr;_T4=(char*)_T3;
if(_T4!=0)goto _TL2;_T5=
_tag_fat(0,0,0);return _T5;_TL2: _T6=buf;_T7=b;_T8=_T7->position;{
struct _fat_ptr res=Cyc_substring(_T6,0,_T8);{
struct _fat_ptr _TC=buf;_T9=b;{struct _fat_ptr _TD=_T9->buffer;buf=_TD;_TA=b;_TA->buffer=_TC;}}_TB=res;
return _TB;}}
# 67
struct _fat_ptr Cyc_Buffer_extract(struct Cyc_Buffer_t*b){struct Cyc_Buffer_t*_T0;struct Cyc_Buffer_t*_T1;struct _fat_ptr _T2;struct Cyc_Buffer_t*_T3;unsigned _T4;int _T5;unsigned char*_T6;char*_T7;unsigned _T8;unsigned char*_T9;char*_TA;struct Cyc_Buffer_t*_TB;struct Cyc_Buffer_t*_TC;struct _fat_ptr _TD;
struct _fat_ptr buf=_tag_fat(0,0,0);{
struct _fat_ptr _TE=buf;_T0=b;{struct _fat_ptr _TF=_T0->buffer;buf=_TF;_T1=b;_T1->buffer=_TE;}}_T2=buf;_T3=b;_T4=_T3->position;_T5=(int)_T4;{struct _fat_ptr _TE=_fat_ptr_plus(_T2,sizeof(char),_T5);_T6=_check_fat_subscript(_TE,sizeof(char),0U);_T7=(char*)_T6;{char _TF=*_T7;char _T10='\000';_T8=_get_fat_size(_TE,sizeof(char));if(_T8!=1U)goto _TL4;if(_TF!=0)goto _TL4;if(_T10==0)goto _TL4;_throw_arraybounds();goto _TL5;_TL4: _TL5: _T9=_TE.curr;_TA=(char*)_T9;*_TA=_T10;}}_TB=b;
# 71
_TB->position=0U;_TC=b;
_TC->length=0U;_TD=buf;
return _TD;}
# 76
int Cyc_Buffer_restore(struct Cyc_Buffer_t*b,struct _fat_ptr buf){unsigned long _T0;struct Cyc_Buffer_t*_T1;struct Cyc_Buffer_t*_T2;struct _fat_ptr _T3;unsigned char*_T4;char*_T5;struct Cyc_Buffer_t*_T6;struct Cyc_Buffer_t*_T7;void(*_T8)(char*);void(*_T9)(void*);struct _fat_ptr _TA;unsigned _TB;unsigned char*_TC;char*_TD;struct Cyc_Buffer_t*_TE;struct Cyc_Buffer_t*_TF;int _T10;_T0=
Cyc_strlen(buf);{int len=(int)_T0;{
struct _fat_ptr _T11=buf;_T1=b;{struct _fat_ptr _T12=_T1->buffer;buf=_T12;_T2=b;_T2->buffer=_T11;}}_T3=buf;_T4=_T3.curr;_T5=(char*)_T4;
if(_T5==0)goto _TL6;{
struct _fat_ptr _T11=buf;_T6=b;{struct _fat_ptr _T12=_T6->buffer;buf=_T12;_T7=b;_T7->buffer=_T11;}}_T9=Cyc_Core_ufree;{
void(*_T11)(char*)=(void(*)(char*))_T9;_T8=_T11;}_TA=buf;_TB=1U + 1U;_TC=_untag_fat_ptr_check_bound(_TA,sizeof(char),_TB);_TD=(char*)_TC;_T8(_TD);
return 0;_TL6: _TE=b;
# 84
_TE->position=0U;_TF=b;_T10=len;
_TF->length=(unsigned)_T10;
return 1;}}
# 89
unsigned long Cyc_Buffer_length(struct Cyc_Buffer_t*b){struct Cyc_Buffer_t*_T0;unsigned _T1;_T0=b;_T1=_T0->position;
return _T1;}
# 93
void Cyc_Buffer_clear(struct Cyc_Buffer_t*b){struct Cyc_Buffer_t*_T0;_T0=b;
_T0->position=0U;}
# 97
void Cyc_Buffer_reset(struct Cyc_Buffer_t*b){struct Cyc_Buffer_t*_T0;struct Cyc_Buffer_t*_T1;struct _fat_ptr _T2;unsigned char*_T3;char*_T4;struct Cyc_Buffer_t*_T5;struct Cyc_Buffer_t*_T6;struct Cyc_Buffer_t*_T7;struct _fat_ptr _T8;unsigned _T9;struct Cyc_Buffer_t*_TA;struct Cyc_Buffer_t*_TB;void(*_TC)(char*);void(*_TD)(void*);struct _fat_ptr _TE;unsigned _TF;unsigned char*_T10;char*_T11;_T0=b;
_T0->position=0U;_T1=b;_T2=_T1->initial_buffer;_T3=_T2.curr;_T4=(char*)_T3;
if(_T4==0)goto _TL8;{
struct _fat_ptr buf=_tag_fat(0,0,0);_T5=b;{
struct _fat_ptr _T12=_T5->initial_buffer;struct _fat_ptr _T13=buf;_T6=b;_T6->initial_buffer=_T13;buf=_T12;}_T7=b;_T8=buf;_T9=
_get_fat_size(_T8,sizeof(char));_T7->length=_T9 - 1U;_TA=b;{
struct _fat_ptr _T12=_TA->buffer;struct _fat_ptr _T13=buf;_TB=b;_TB->buffer=_T13;buf=_T12;}_TD=Cyc_Core_ufree;{
void(*_T12)(char*)=(void(*)(char*))_TD;_TC=_T12;}_TE=buf;_TF=1U + 1U;_T10=_untag_fat_ptr_check_bound(_TE,sizeof(char),_TF);_T11=(char*)_T10;_TC(_T11);}goto _TL9;_TL8: _TL9:
# 106
 return;}
# 109
static void Cyc_Buffer_resize(struct Cyc_Buffer_t*b,unsigned more){struct Cyc_Buffer_t*_T0;unsigned _T1;struct Cyc_Buffer_t*_T2;unsigned _T3;unsigned _T4;unsigned _T5;unsigned long _T6;struct _fat_ptr _T7;struct _RegionHandle*_T8;unsigned _T9;void*_TA;struct Cyc_Buffer_t*_TB;unsigned _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_Buffer_t*_TF;struct _fat_ptr _T10;struct Cyc_Buffer_t*_T11;unsigned _T12;struct Cyc_Buffer_t*_T13;struct _fat_ptr _T14;unsigned char*_T15;char*_T16;struct Cyc_Buffer_t*_T17;struct Cyc_Buffer_t*_T18;struct Cyc_Buffer_t*_T19;struct Cyc_Buffer_t*_T1A;struct Cyc_Buffer_t*_T1B;struct Cyc_Buffer_t*_T1C;void(*_T1D)(char*);void(*_T1E)(void*);struct _fat_ptr _T1F;unsigned _T20;unsigned char*_T21;char*_T22;struct Cyc_Buffer_t*_T23;_T0=b;{
unsigned long len=_T0->length;
if(len!=0U)goto _TLA;_T1=1U;goto _TLB;_TLA: _T1=len;_TLB: {unsigned long new_len=_T1;
struct _fat_ptr new_buffer;
_TLC: _T2=b;_T3=_T2->position;_T4=more;_T5=_T3 + _T4;_T6=new_len;if(_T5 > _T6)goto _TLD;else{goto _TLE;}
_TLD: new_len=2U * new_len;goto _TLC;_TLE:{unsigned _T24=new_len + 1U;_T8=Cyc_Core_heap_region;_T9=Cyc_Core_unique_qual;_TA=_region_calloc(_T8,_T9,sizeof(char),_T24);_T7=_tag_fat(_TA,sizeof(char),_T24);}
# 116
new_buffer=_T7;_TB=b;_TC=_TB->length;
if(_TC==0U)goto _TLF;{struct _fat_ptr _T24;_T24=new_buffer;{struct _fat_ptr x=_T24;_TD=x;_TE=
# 119
_fat_ptr_decrease_size(_TD,sizeof(char),1U);_TF=b;_T10=_TF->buffer;_T11=b;_T12=_T11->position;Cyc_strncpy(_TE,_T10,_T12);}}goto _TL10;_TLF: _TL10: _T13=b;_T14=_T13->initial_buffer;_T15=_T14.curr;_T16=(char*)_T15;
# 121
if(_T16!=0)goto _TL11;_T17=b;{
struct _fat_ptr _T24=_T17->initial_buffer;_T18=b;{struct _fat_ptr _T25=_T18->buffer;_T19=b;_T19->initial_buffer=_T25;_T1A=b;_T1A->buffer=_T24;}}goto _TL12;_TL11: _TL12: _T1B=b;{
struct _fat_ptr _T24=_T1B->buffer;struct _fat_ptr _T25=new_buffer;_T1C=b;_T1C->buffer=_T25;new_buffer=_T24;}_T1E=Cyc_Core_ufree;{
void(*_T24)(char*)=(void(*)(char*))_T1E;_T1D=_T24;}_T1F=new_buffer;_T20=1U + 1U;_T21=_untag_fat_ptr_check_bound(_T1F,sizeof(char),_T20);_T22=(char*)_T21;_T1D(_T22);_T23=b;
_T23->length=new_len;
return;}}}
# 129
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*b,char c){struct Cyc_Buffer_t*_T0;unsigned _T1;int _T2;unsigned _T3;struct Cyc_Buffer_t*_T4;unsigned _T5;struct Cyc_Buffer_t*_T6;struct _fat_ptr _T7;int _T8;unsigned char*_T9;char*_TA;unsigned _TB;unsigned char*_TC;char*_TD;struct Cyc_Buffer_t*_TE;int _TF;_T0=b;_T1=_T0->position;{
int pos=(int)_T1;_T2=pos;_T3=(unsigned)_T2;_T4=b;_T5=_T4->length;
if(_T3 < _T5)goto _TL13;Cyc_Buffer_resize(b,1U);goto _TL14;_TL13: _TL14: _T6=b;_T7=_T6->buffer;_T8=pos;{struct _fat_ptr _T10=_fat_ptr_plus(_T7,sizeof(char),_T8);_T9=_check_fat_subscript(_T10,sizeof(char),0U);_TA=(char*)_T9;{char _T11=*_TA;char _T12=c;_TB=_get_fat_size(_T10,sizeof(char));if(_TB!=1U)goto _TL15;if(_T11!=0)goto _TL15;if(_T12==0)goto _TL15;_throw_arraybounds();goto _TL16;_TL15: _TL16: _TC=_T10.curr;_TD=(char*)_TC;*_TD=_T12;}}_TE=b;_TF=pos + 1;
# 133
_TE->position=(unsigned)_TF;
return;}}
# 140
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*b,struct _fat_ptr s,int offset,int len){int _T0;unsigned _T1;struct _fat_ptr _T2;unsigned _T3;struct Cyc_Core_Invalid_argument_exn_struct*_T4;void*_T5;struct Cyc_Buffer_t*_T6;unsigned _T7;int _T8;unsigned _T9;unsigned _TA;int _TB;unsigned _TC;struct Cyc_Buffer_t*_TD;unsigned _TE;struct Cyc_Buffer_t*_TF;int _T10;unsigned _T11;struct Cyc_Buffer_t*_T12;struct Cyc_Buffer_t*_T13;struct _fat_ptr _T14;struct Cyc_Buffer_t*_T15;unsigned _T16;int _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;int _T1B;struct _fat_ptr _T1C;int _T1D;unsigned long _T1E;struct Cyc_Buffer_t*_T1F;struct Cyc_Buffer_t*_T20;struct Cyc_Buffer_t*_T21;int _T22;
if(offset < 0)goto _TL19;else{goto _TL1B;}_TL1B: if(len < 0)goto _TL19;else{goto _TL1A;}_TL1A: _T0=offset + len;_T1=(unsigned)_T0;_T2=s;_T3=_get_fat_size(_T2,sizeof(char));if(_T1 > _T3)goto _TL19;else{goto _TL17;}
_TL19:{struct Cyc_Core_Invalid_argument_exn_struct*_T23=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T23->tag=Cyc_Core_Invalid_argument;_T23->f1=_tag_fat("Buffer::add_substring",sizeof(char),22U);_T4=(struct Cyc_Core_Invalid_argument_exn_struct*)_T23;}_T5=(void*)_T4;_throw(_T5);goto _TL18;_TL17: _TL18: _T6=b;_T7=_T6->position;_T8=len;_T9=(unsigned)_T8;_TA=_T7 + _T9;{
int new_position=(int)_TA;_TB=new_position;_TC=(unsigned)_TB;_TD=b;_TE=_TD->length;
if(_TC <= _TE)goto _TL1C;_TF=b;_T10=len;_T11=(unsigned)_T10;Cyc_Buffer_resize(_TF,_T11);goto _TL1D;_TL1C: _TL1D: {
struct _fat_ptr buf=_tag_fat(0,0,0);{
struct _fat_ptr _T23=buf;_T12=b;{struct _fat_ptr _T24=_T12->buffer;buf=_T24;_T13=b;_T13->buffer=_T23;}}{struct _fat_ptr _T23;_T23=buf;{struct _fat_ptr x=_T23;_T14=x;_T15=b;_T16=_T15->position;_T17=(int)_T16;_T18=
# 148
_fat_ptr_plus(_T14,sizeof(char),_T17);_T19=_fat_ptr_decrease_size(_T18,sizeof(char),1U);_T1A=s;_T1B=offset;_T1C=_fat_ptr_plus(_T1A,sizeof(char),_T1B);_T1D=len;_T1E=(unsigned long)_T1D;Cyc_zstrncpy(_T19,_T1C,_T1E);}}{
# 150
struct _fat_ptr _T23=buf;_T1F=b;{struct _fat_ptr _T24=_T1F->buffer;buf=_T24;_T20=b;_T20->buffer=_T23;}}_T21=b;_T22=new_position;
_T21->position=(unsigned)_T22;
return;}}}
# 156
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*b,struct _fat_ptr s){unsigned long _T0;struct Cyc_Buffer_t*_T1;unsigned _T2;int _T3;unsigned _T4;unsigned _T5;int _T6;unsigned _T7;struct Cyc_Buffer_t*_T8;unsigned _T9;struct Cyc_Buffer_t*_TA;int _TB;unsigned _TC;struct Cyc_Buffer_t*_TD;struct Cyc_Buffer_t*_TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_Buffer_t*_T11;unsigned _T12;int _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;int _T16;unsigned long _T17;struct Cyc_Buffer_t*_T18;struct Cyc_Buffer_t*_T19;struct Cyc_Buffer_t*_T1A;int _T1B;_T0=
Cyc_strlen(s);{int len=(int)_T0;_T1=b;_T2=_T1->position;_T3=len;_T4=(unsigned)_T3;_T5=_T2 + _T4;{
int new_position=(int)_T5;_T6=new_position;_T7=(unsigned)_T6;_T8=b;_T9=_T8->length;
if(_T7 <= _T9)goto _TL1E;_TA=b;_TB=len;_TC=(unsigned)_TB;Cyc_Buffer_resize(_TA,_TC);goto _TL1F;_TL1E: _TL1F: {
struct _fat_ptr buf=_tag_fat(0,0,0);{
struct _fat_ptr _T1C=buf;_TD=b;{struct _fat_ptr _T1D=_TD->buffer;buf=_T1D;_TE=b;_TE->buffer=_T1C;}}{struct _fat_ptr _T1C;_T1C=buf;{struct _fat_ptr x=_T1C;_TF=x;_T10=
# 163
_fat_ptr_decrease_size(_TF,sizeof(char),1U);_T11=b;_T12=_T11->position;_T13=(int)_T12;_T14=_fat_ptr_plus(_T10,sizeof(char),_T13);_T15=s;_T16=len;_T17=(unsigned long)_T16;Cyc_strncpy(_T14,_T15,_T17);}}{
# 165
struct _fat_ptr _T1C=buf;_T18=b;{struct _fat_ptr _T1D=_T18->buffer;buf=_T1D;_T19=b;_T19->buffer=_T1C;}}_T1A=b;_T1B=new_position;
_T1A->position=(unsigned)_T1B;
return;}}}}
# 170
void Cyc_Buffer_add_buffer(struct Cyc_Buffer_t*b,struct Cyc_Buffer_t*bs){struct Cyc_Buffer_t*_T0;struct Cyc_Buffer_t*_T1;struct _fat_ptr _T2;struct Cyc_Buffer_t*_T3;unsigned _T4;int _T5;_T0=b;_T1=bs;_T2=_T1->buffer;_T3=bs;_T4=_T3->position;_T5=(int)_T4;
Cyc_Buffer_add_substring(_T0,_T2,0,_T5);
return;}

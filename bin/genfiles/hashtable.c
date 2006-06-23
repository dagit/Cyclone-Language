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
 struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 154 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Hashtable_Table;
# 72 "hashtable.h"
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);struct Cyc_Hashtable_Cell{void*key;void*value;struct Cyc_Hashtable_Cell*next;};struct Cyc_Hashtable_Bucket{unsigned length;struct Cyc_Hashtable_Cell*cells;};struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};
# 51 "hashtable.cyc"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*)){struct Cyc_Hashtable_Table*_T0;struct _RegionHandle*_T1;struct _fat_ptr _T2;struct _RegionHandle*_T3;void*_T4;_T1=r;{struct Cyc_Hashtable_Table*_T5=_region_malloc(_T1,0U,sizeof(struct Cyc_Hashtable_Table));
# 53
_T5->r=r;_T5->cmp=cmp;_T5->hash=hash;_T5->max_len=3;{unsigned _T6=sz;_T3=r;_T4=_region_calloc(_T3,0U,sizeof(struct Cyc_Hashtable_Bucket),_T6);_T2=_tag_fat(_T4,sizeof(struct Cyc_Hashtable_Bucket),_T6);}_T5->tab=_T2;_T0=(struct Cyc_Hashtable_Table*)_T5;}return _T0;}
# 56
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*)){struct _RegionHandle*_T0;int _T1;int(*_T2)(void*,void*);int(*_T3)(void*);struct Cyc_Hashtable_Table*_T4;_T0=Cyc_Core_heap_region;_T1=sz;_T2=cmp;_T3=hash;_T4=
Cyc_Hashtable_rcreate(_T0,_T1,_T2,_T3);return _T4;}
# 60
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val){struct Cyc_Hashtable_Table*_T0;struct Cyc_Hashtable_Table*_T1;int(*_T2)(void*);int _T3;unsigned _T4;struct _fat_ptr _T5;unsigned _T6;struct _fat_ptr _T7;unsigned _T8;int _T9;struct _fat_ptr _TA;unsigned char*_TB;unsigned char*_TC;struct Cyc_Hashtable_Bucket*_TD;struct Cyc_Hashtable_Cell*_TE;struct Cyc_Hashtable_Table*_TF;struct _RegionHandle*_T10;struct Cyc_Hashtable_Bucket*_T11;struct Cyc_Hashtable_Bucket*_T12;unsigned _T13;struct Cyc_Hashtable_Bucket*_T14;unsigned _T15;struct Cyc_Hashtable_Table*_T16;int _T17;unsigned _T18;_T0=t;{
struct _fat_ptr tab=_T0->tab;_T1=t;_T2=_T1->hash;_T3=
_T2(key);_T4=(unsigned)_T3;_T5=tab;_T6=_get_fat_size(_T5,sizeof(struct Cyc_Hashtable_Bucket));{unsigned bucket=_T4 % _T6;_T7=tab;_T8=bucket;_T9=(int)_T8;_TA=
_fat_ptr_plus(_T7,sizeof(struct Cyc_Hashtable_Bucket),_T9);_TB=_untag_fat_ptr_check_bound(_TA,sizeof(struct Cyc_Hashtable_Bucket),1U);_TC=_check_null(_TB);{struct Cyc_Hashtable_Bucket*b=(struct Cyc_Hashtable_Bucket*)_TC;_TD=b;_TF=t;_T10=_TF->r;{struct Cyc_Hashtable_Cell*_T19=_region_malloc(_T10,0U,sizeof(struct Cyc_Hashtable_Cell));
_T19->key=key;_T19->value=val;_T11=b;_T19->next=_T11->cells;_TE=(struct Cyc_Hashtable_Cell*)_T19;}_TD->cells=_TE;_T12=b;_T13=_T12->length;{
unsigned i=_T13 + 1U;_T14=b;
_T14->length=i;_T15=i;_T16=t;_T17=_T16->max_len;_T18=(unsigned)_T17;
if(_T15 <= _T18)goto _TL0;
# 69
Cyc_Hashtable_resize(t);goto _TL1;_TL0: _TL1:;}}}}}
# 73
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key){struct Cyc_Hashtable_Table*_T0;struct Cyc_Hashtable_Table*_T1;struct _fat_ptr _T2;unsigned char*_T3;struct Cyc_Hashtable_Bucket*_T4;struct Cyc_Hashtable_Table*_T5;int(*_T6)(void*);int _T7;unsigned _T8;struct _fat_ptr _T9;unsigned _TA;unsigned _TB;int _TC;struct Cyc_Hashtable_Bucket _TD;struct Cyc_Hashtable_Cell*_TE;void*_TF;void*_T10;int _T11;struct Cyc_Hashtable_Cell*_T12;void*_T13;struct Cyc_Hashtable_Cell*_T14;struct Cyc_Core_Not_found_exn_struct*_T15;struct Cyc_Core_Not_found_exn_struct*_T16;_T0=t;{
struct _fat_ptr tab=_T0->tab;_T1=t;{
int(*cmp)(void*,void*)=_T1->cmp;_T2=tab;_T3=_T2.curr;_T4=(struct Cyc_Hashtable_Bucket*)_T3;_T5=t;_T6=_T5->hash;_T7=
_T6(key);_T8=(unsigned)_T7;_T9=tab;_TA=_get_fat_size(_T9,sizeof(struct Cyc_Hashtable_Bucket));_TB=_T8 % _TA;_TC=(int)_TB;_TD=_T4[_TC];{struct Cyc_Hashtable_Cell*p=_TD.cells;
_TL5: if(p!=0)goto _TL3;else{goto _TL4;}
_TL3: _TE=p;_TF=_TE->key;_T10=key;_T11=cmp(_TF,_T10);if(_T11!=0)goto _TL6;_T12=p;_T13=_T12->value;return _T13;_TL6: _T14=p;
# 77
p=_T14->next;goto _TL5;_TL4: _T15=& Cyc_Core_Not_found_val;_T16=(struct Cyc_Core_Not_found_exn_struct*)_T15;_throw(_T16);}}}}
# 83
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key){struct Cyc_Hashtable_Table*_T0;struct Cyc_Hashtable_Table*_T1;struct _fat_ptr _T2;unsigned char*_T3;struct Cyc_Hashtable_Bucket*_T4;struct Cyc_Hashtable_Table*_T5;int(*_T6)(void*);int _T7;unsigned _T8;struct _fat_ptr _T9;unsigned _TA;unsigned _TB;int _TC;struct Cyc_Hashtable_Bucket _TD;struct Cyc_Hashtable_Cell*_TE;void*_TF;void*_T10;int _T11;struct Cyc_Hashtable_Cell*_T12;void**_T13;struct Cyc_Hashtable_Cell*_T14;_T0=t;{
struct _fat_ptr tab=_T0->tab;_T1=t;{
int(*cmp)(void*,void*)=_T1->cmp;_T2=tab;_T3=_T2.curr;_T4=(struct Cyc_Hashtable_Bucket*)_T3;_T5=t;_T6=_T5->hash;_T7=
_T6(key);_T8=(unsigned)_T7;_T9=tab;_TA=_get_fat_size(_T9,sizeof(struct Cyc_Hashtable_Bucket));_TB=_T8 % _TA;_TC=(int)_TB;_TD=_T4[_TC];{struct Cyc_Hashtable_Cell*p=_TD.cells;
_TLB: if(p!=0)goto _TL9;else{goto _TLA;}
_TL9: _TE=p;_TF=_TE->key;_T10=key;_T11=cmp(_TF,_T10);if(_T11!=0)goto _TLC;_T12=p;_T13=& _T12->value;return _T13;_TLC: _T14=p;
# 87
p=_T14->next;goto _TLB;_TLA:
# 90
 return 0;}}}}
# 94
void**Cyc_Hashtable_lookup_ptr_opt(struct Cyc_Hashtable_Table*t,void*key){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;unsigned char*_T2;struct Cyc_Hashtable_Bucket*_T3;struct Cyc_Hashtable_Table*_T4;int(*_T5)(void*);int _T6;unsigned _T7;struct _fat_ptr _T8;unsigned _T9;unsigned _TA;int _TB;struct Cyc_Hashtable_Bucket _TC;struct Cyc_Hashtable_Cell*_TD;void*_TE;int _TF;void*_T10;int _T11;struct Cyc_Hashtable_Cell*_T12;void**_T13;struct Cyc_Hashtable_Cell*_T14;_T0=t;{
struct _fat_ptr tab=_T0->tab;_T1=tab;_T2=_T1.curr;_T3=(struct Cyc_Hashtable_Bucket*)_T2;_T4=t;_T5=_T4->hash;_T6=
_T5(key);_T7=(unsigned)_T6;_T8=tab;_T9=_get_fat_size(_T8,sizeof(struct Cyc_Hashtable_Bucket));_TA=_T7 % _T9;_TB=(int)_TA;_TC=_T3[_TB];{struct Cyc_Hashtable_Cell*p=_TC.cells;
_TL11: if(p!=0)goto _TLF;else{goto _TL10;}
_TLF: _TD=p;_TE=_TD->key;_TF=(int)_TE;_T10=key;_T11=(int)_T10;if(_TF!=_T11)goto _TL12;_T12=p;_T13=& _T12->value;return _T13;_TL12: _T14=p;
# 97
p=_T14->next;goto _TL11;_TL10:
# 100
 return 0;}}}
# 107
void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*)){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;unsigned char*_T2;struct Cyc_Hashtable_Bucket*_T3;int _T4;unsigned _T5;struct _fat_ptr _T6;unsigned _T7;unsigned _T8;int _T9;struct Cyc_Hashtable_Bucket _TA;void*_TB;struct Cyc_Hashtable_Cell*_TC;void*_TD;int _TE;struct Cyc_Hashtable_Cell*_TF;void**_T10;struct Cyc_Hashtable_Cell*_T11;_T0=t;{
# 109
struct _fat_ptr tab=_T0->tab;_T1=tab;_T2=_T1.curr;_T3=(struct Cyc_Hashtable_Bucket*)_T2;_T4=
hash(key);_T5=(unsigned)_T4;_T6=tab;_T7=_get_fat_size(_T6,sizeof(struct Cyc_Hashtable_Bucket));_T8=_T5 % _T7;_T9=(int)_T8;_TA=_T3[_T9];{struct Cyc_Hashtable_Cell*p=_TA.cells;
_TL17: if(p!=0)goto _TL15;else{goto _TL16;}
_TL15: _TB=key;_TC=p;_TD=_TC->key;_TE=cmp(_TB,_TD);if(_TE!=0)goto _TL18;_TF=p;_T10=& _TF->value;
return _T10;_TL18: _T11=p;
# 111
p=_T11->next;goto _TL17;_TL16:
# 115
 return 0;}}}
# 118
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data){struct Cyc_Hashtable_Table*_T0;struct Cyc_Hashtable_Table*_T1;struct _fat_ptr _T2;unsigned char*_T3;struct Cyc_Hashtable_Bucket*_T4;struct Cyc_Hashtable_Table*_T5;int(*_T6)(void*);int _T7;unsigned _T8;struct _fat_ptr _T9;unsigned _TA;unsigned _TB;int _TC;struct Cyc_Hashtable_Bucket _TD;struct Cyc_Hashtable_Cell*_TE;void*_TF;void*_T10;int _T11;void**_T12;struct Cyc_Hashtable_Cell*_T13;struct Cyc_Hashtable_Cell*_T14;_T0=t;{
struct _fat_ptr tab=_T0->tab;_T1=t;{
int(*cmp)(void*,void*)=_T1->cmp;_T2=tab;_T3=_T2.curr;_T4=(struct Cyc_Hashtable_Bucket*)_T3;_T5=t;_T6=_T5->hash;_T7=
_T6(key);_T8=(unsigned)_T7;_T9=tab;_TA=_get_fat_size(_T9,sizeof(struct Cyc_Hashtable_Bucket));_TB=_T8 % _TA;_TC=(int)_TB;_TD=_T4[_TC];{struct Cyc_Hashtable_Cell*p=_TD.cells;
_TL1D: if(p!=0)goto _TL1B;else{goto _TL1C;}
_TL1B: _TE=p;_TF=_TE->key;_T10=key;_T11=cmp(_TF,_T10);if(_T11!=0)goto _TL1E;_T12=data;_T13=p;
*_T12=_T13->value;
return 1;_TL1E: _T14=p;
# 122
p=_T14->next;goto _TL1D;_TL1C:
# 128
 return 0;}}}}
# 131
void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key){struct Cyc_Hashtable_Table*_T0;struct Cyc_Hashtable_Table*_T1;struct Cyc_Hashtable_Table*_T2;int(*_T3)(void*);int _T4;unsigned _T5;struct _fat_ptr _T6;unsigned _T7;struct _fat_ptr _T8;unsigned _T9;int _TA;struct _fat_ptr _TB;unsigned char*_TC;unsigned char*_TD;struct Cyc_Hashtable_Bucket*_TE;void*_TF;struct Cyc_Hashtable_Cell*_T10;void*_T11;int _T12;struct Cyc_Hashtable_Bucket*_T13;struct Cyc_Hashtable_Cell*_T14;struct Cyc_Hashtable_Bucket*_T15;struct Cyc_Hashtable_Cell*_T16;struct Cyc_Hashtable_Cell*_T17;struct Cyc_Hashtable_Cell*_T18;void*_T19;struct Cyc_Hashtable_Cell*_T1A;void*_T1B;int _T1C;struct Cyc_Hashtable_Cell*_T1D;struct Cyc_Hashtable_Cell*_T1E;struct Cyc_Hashtable_Bucket*_T1F;struct Cyc_Hashtable_Cell*_T20;struct Cyc_Hashtable_Cell*_T21;_T0=t;{
# 134
struct _fat_ptr tab=_T0->tab;_T1=t;{
int(*cmp)(void*,void*)=_T1->cmp;_T2=t;_T3=_T2->hash;_T4=
_T3(key);_T5=(unsigned)_T4;_T6=tab;_T7=_get_fat_size(_T6,sizeof(struct Cyc_Hashtable_Bucket));{unsigned bucket=_T5 % _T7;_T8=tab;_T9=bucket;_TA=(int)_T9;_TB=
_fat_ptr_plus(_T8,sizeof(struct Cyc_Hashtable_Bucket),_TA);_TC=_untag_fat_ptr_check_bound(_TB,sizeof(struct Cyc_Hashtable_Bucket),1U);_TD=_check_null(_TC);{struct Cyc_Hashtable_Bucket*b=(struct Cyc_Hashtable_Bucket*)_TD;_TE=b;{
struct Cyc_Hashtable_Cell*l=_TE->cells;
if(l!=0)goto _TL20;return;_TL20: _TF=key;_T10=l;_T11=_T10->key;_T12=
cmp(_TF,_T11);if(_T12!=0)goto _TL22;_T13=b;_T14=l;
_T13->cells=_T14->next;_T15=b;
_T15->length=_T15->length + -1;
return;_TL22: _T16=l;{
# 145
struct Cyc_Hashtable_Cell*next=_T16->next;_TL27: _T17=_check_null(l);_T18=_T17->next;if(_T18!=0)goto _TL25;else{goto _TL26;}
# 147
_TL25: _T19=key;_T1A=_check_null(next);_T1B=_T1A->key;_T1C=cmp(_T19,_T1B);if(_T1C!=0)goto _TL28;_T1D=l;_T1E=next;
_T1D->next=_T1E->next;_T1F=b;
_T1F->length=_T1F->length + -1;
return;_TL28: _T20=l;
# 145
l=_T20->next;_T21=next;next=_T21->next;goto _TL27;_TL26:;}}}}}}}
# 154
int Cyc_Hashtable_hash_string(struct _fat_ptr s){struct _fat_ptr _T0;unsigned _T1;unsigned _T2;int _T3;unsigned _T4;int _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;unsigned _T9;int _TA;char _TB;int _TC;char _TD;int _TE;int _TF;
# 156
int ans=0;_T0=s;_T1=
_get_fat_size(_T0,sizeof(char));{int sz=(int)_T1;
int shift=0;{
unsigned i=0U;_TL2D: _T2=i;_T3=sz;_T4=(unsigned)_T3;if(_T2 < _T4)goto _TL2B;else{goto _TL2C;}
_TL2B: _T5=ans;_T6=s;_T7=_T6.curr;_T8=(const char*)_T7;_T9=i;_TA=(int)_T9;_TB=_T8[_TA];_TC=shift;_TD=_TB << _TC;_TE=(int)_TD;ans=_T5 ^ _TE;
shift=shift + 8;
if(shift!=32)goto _TL2E;
shift=0;goto _TL2F;_TL2E: _TL2F:
# 159
 i=i + 1;goto _TL2D;_TL2C:;}_TF=ans;
# 165
return _TF;}}
# 168
int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*s){struct _fat_ptr*_T0;struct _fat_ptr _T1;int _T2;_T0=s;_T1=*_T0;_T2=
Cyc_Hashtable_hash_string(_T1);return _T2;}
# 173
static struct Cyc_Hashtable_Cell*Cyc_Hashtable_rev_cells(struct Cyc_Hashtable_Cell*cells){struct Cyc_Hashtable_Cell*_T0;struct Cyc_Hashtable_Cell*_T1;struct Cyc_Hashtable_Cell*_T2;struct Cyc_Hashtable_Cell*_T3;struct Cyc_Hashtable_Cell*_T4;
if(cells!=0)goto _TL30;return 0;_TL30: {
struct Cyc_Hashtable_Cell*first=cells;_T0=cells;{
struct Cyc_Hashtable_Cell*second=_T0->next;_T1=cells;
_T1->next=0;
_TL32: if(second!=0)goto _TL33;else{goto _TL34;}
_TL33: _T2=second;{struct Cyc_Hashtable_Cell*temp=_T2->next;_T3=second;
_T3->next=first;
first=second;
second=temp;}goto _TL32;_TL34: _T4=first;
# 184
return _T4;}}}
# 189
static void Cyc_Hashtable_insert_bucket(struct _RegionHandle*r,struct _fat_ptr tab,int(*hash)(void*),struct Cyc_Hashtable_Cell*elems){struct Cyc_Hashtable_Cell*_T0;struct Cyc_Hashtable_Cell*_T1;int _T2;unsigned _T3;struct _fat_ptr _T4;unsigned _T5;struct _fat_ptr _T6;unsigned char*_T7;struct Cyc_Hashtable_Bucket*_T8;unsigned _T9;int _TA;struct Cyc_Hashtable_Cell*_TB;struct _RegionHandle*_TC;struct _fat_ptr _TD;unsigned char*_TE;struct Cyc_Hashtable_Bucket*_TF;unsigned _T10;int _T11;struct Cyc_Hashtable_Bucket _T12;struct _fat_ptr _T13;unsigned char*_T14;struct Cyc_Hashtable_Bucket*_T15;unsigned _T16;int _T17;struct Cyc_Hashtable_Cell*_T18;
# 192
_TL38: if(elems!=0)goto _TL36;else{goto _TL37;}
_TL36: _T0=elems;{void*key=_T0->key;_T1=elems;{
void*val=_T1->value;_T2=
hash(key);_T3=(unsigned)_T2;_T4=tab;_T5=_get_fat_size(_T4,sizeof(struct Cyc_Hashtable_Bucket));{unsigned nidx=_T3 % _T5;_T6=tab;_T7=_T6.curr;_T8=(struct Cyc_Hashtable_Bucket*)_T7;_T9=nidx;_TA=(int)_T9;_TC=r;{struct Cyc_Hashtable_Cell*_T19=_region_malloc(_TC,0U,sizeof(struct Cyc_Hashtable_Cell));
_T19->key=key;_T19->value=val;_TD=tab;_TE=_TD.curr;_TF=(struct Cyc_Hashtable_Bucket*)_TE;_T10=nidx;_T11=(int)_T10;_T12=_TF[_T11];_T19->next=_T12.cells;_TB=(struct Cyc_Hashtable_Cell*)_T19;}_T8[_TA].cells=_TB;_T13=tab;_T14=_T13.curr;_T15=(struct Cyc_Hashtable_Bucket*)_T14;_T16=nidx;_T17=(int)_T16;
_T15[_T17].length=_T15[_T17].length + 1;}}}_T18=elems;
# 192
elems=_T18->next;goto _TL38;_TL37:;}
# 201
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;unsigned _T2;struct _fat_ptr _T3;struct Cyc_Hashtable_Table*_T4;struct _RegionHandle*_T5;void*_T6;struct Cyc_Hashtable_Table*_T7;struct _RegionHandle*_T8;struct _fat_ptr _T9;struct Cyc_Hashtable_Table*_TA;int(*_TB)(void*);struct _fat_ptr _TC;unsigned char*_TD;struct Cyc_Hashtable_Bucket*_TE;unsigned _TF;int _T10;struct Cyc_Hashtable_Bucket _T11;struct Cyc_Hashtable_Cell*_T12;struct Cyc_Hashtable_Cell*_T13;struct Cyc_Hashtable_Table*_T14;struct _fat_ptr _T15;unsigned char*_T16;struct Cyc_Hashtable_Bucket*_T17;unsigned _T18;int _T19;struct Cyc_Hashtable_Bucket _T1A;struct Cyc_Hashtable_Table*_T1B;unsigned _T1C;unsigned _T1D;_T0=t;{
struct _fat_ptr odata=_T0->tab;_T1=odata;{
unsigned osize=_get_fat_size(_T1,sizeof(struct Cyc_Hashtable_Bucket));_T2=2U * osize;{
unsigned nsize=_T2 + 7U;{unsigned _T1E=nsize;_T4=t;_T5=_T4->r;_T6=_region_calloc(_T5,0U,sizeof(struct Cyc_Hashtable_Bucket),_T1E);_T3=_tag_fat(_T6,sizeof(struct Cyc_Hashtable_Bucket),_T1E);}{
struct _fat_ptr ndata=_T3;{
unsigned i=0U;_TL3C: if(i < osize)goto _TL3A;else{goto _TL3B;}
_TL3A: _T7=t;_T8=_T7->r;_T9=ndata;_TA=t;_TB=_TA->hash;_TC=odata;_TD=_TC.curr;_TE=(struct Cyc_Hashtable_Bucket*)_TD;_TF=i;_T10=(int)_TF;_T11=_TE[_T10];_T12=_T11.cells;_T13=Cyc_Hashtable_rev_cells(_T12);Cyc_Hashtable_insert_bucket(_T8,_T9,_TB,_T13);
# 206
i=i + 1;goto _TL3C;_TL3B:;}_T14=t;
# 208
_T14->tab=ndata;{
# 210
unsigned cur_max_len=0U;{
unsigned j=0U;_TL40: if(j < nsize)goto _TL3E;else{goto _TL3F;}
_TL3E: _T15=ndata;_T16=_T15.curr;_T17=(struct Cyc_Hashtable_Bucket*)_T16;_T18=j;_T19=(int)_T18;_T1A=_T17[_T19];{unsigned k=_T1A.length;
if(k <= cur_max_len)goto _TL41;cur_max_len=k;goto _TL42;_TL41: _TL42:;}
# 211
j=j + 1;goto _TL40;_TL3F:;}_T1B=t;_T1C=3U * cur_max_len;_T1D=_T1C / 2U;
# 215
_T1B->max_len=(int)_T1D;}}}}}}
# 219
void Cyc_Hashtable_iter(void(*f)(void*,void*),struct Cyc_Hashtable_Table*t){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;unsigned char*_T3;struct Cyc_Hashtable_Bucket*_T4;unsigned _T5;int _T6;struct Cyc_Hashtable_Bucket _T7;struct Cyc_Hashtable_Cell*_T8;void*_T9;struct Cyc_Hashtable_Cell*_TA;void*_TB;struct Cyc_Hashtable_Cell*_TC;_T0=t;{
struct _fat_ptr odata=_T0->tab;_T1=odata;{
unsigned osize=_get_fat_size(_T1,sizeof(struct Cyc_Hashtable_Bucket));
unsigned i=0U;_TL46: if(i < osize)goto _TL44;else{goto _TL45;}
_TL44: _T2=odata;_T3=_T2.curr;_T4=(struct Cyc_Hashtable_Bucket*)_T3;_T5=i;_T6=(int)_T5;_T7=_T4[_T6];{struct Cyc_Hashtable_Cell*iter=_T7.cells;_TL4A: if(iter!=0)goto _TL48;else{goto _TL49;}
_TL48: _T8=iter;_T9=_T8->key;_TA=iter;_TB=_TA->value;f(_T9,_TB);_TC=iter;
# 223
iter=_TC->next;goto _TL4A;_TL49:;}
# 222
i=i + 1;goto _TL46;_TL45:;}}}
# 227
void Cyc_Hashtable_iter_c(void(*f)(void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;unsigned char*_T3;struct Cyc_Hashtable_Bucket*_T4;unsigned _T5;int _T6;struct Cyc_Hashtable_Bucket _T7;struct Cyc_Hashtable_Cell*_T8;void*_T9;struct Cyc_Hashtable_Cell*_TA;void*_TB;void*_TC;struct Cyc_Hashtable_Cell*_TD;_T0=t;{
struct _fat_ptr odata=_T0->tab;_T1=odata;{
unsigned osize=_get_fat_size(_T1,sizeof(struct Cyc_Hashtable_Bucket));
unsigned i=0U;_TL4E: if(i < osize)goto _TL4C;else{goto _TL4D;}
_TL4C: _T2=odata;_T3=_T2.curr;_T4=(struct Cyc_Hashtable_Bucket*)_T3;_T5=i;_T6=(int)_T5;_T7=_T4[_T6];{struct Cyc_Hashtable_Cell*iter=_T7.cells;_TL52: if(iter!=0)goto _TL50;else{goto _TL51;}
_TL50: _T8=iter;_T9=_T8->key;_TA=iter;_TB=_TA->value;_TC=env;f(_T9,_TB,_TC);_TD=iter;
# 231
iter=_TD->next;goto _TL52;_TL51:;}
# 230
i=i + 1;goto _TL4E;_TL4D:;}}}
# 235
unsigned Cyc_Hashtable_num_buckets(struct Cyc_Hashtable_Table*t){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;unsigned _T2;_T0=t;_T1=_T0->tab;_T2=
_get_fat_size(_T1,sizeof(struct Cyc_Hashtable_Bucket));return _T2;}
# 240
void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*)){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;struct Cyc_Int_pa_PrintArg_struct _T2;unsigned _T3;int _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;unsigned char*_T8;struct Cyc_Hashtable_Bucket*_T9;unsigned _TA;int _TB;struct Cyc_Hashtable_Bucket _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_Hashtable_Cell*_TF;void*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct Cyc_Hashtable_Cell*_T13;void*_T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Hashtable_Cell*_T17;struct _fat_ptr _T18;struct _fat_ptr _T19;_T0=t;{
# 242
struct _fat_ptr odata=_T0->tab;_T1=odata;{
unsigned osize=_get_fat_size(_T1,sizeof(struct Cyc_Hashtable_Bucket));
unsigned i=0U;_TL56: if(i < osize)goto _TL54;else{goto _TL55;}
_TL54:{struct Cyc_Int_pa_PrintArg_struct _T1A;_T1A.tag=1;_T3=i;_T4=(int)_T3;_T1A.f1=(unsigned long)_T4;_T2=_T1A;}{struct Cyc_Int_pa_PrintArg_struct _T1A=_T2;void*_T1B[1];_T1B[0]=& _T1A;_T5=_tag_fat("%d: ",sizeof(char),5U);_T6=_tag_fat(_T1B,sizeof(void*),1);Cyc_printf(_T5,_T6);}_T7=odata;_T8=_T7.curr;_T9=(struct Cyc_Hashtable_Bucket*)_T8;_TA=i;_TB=(int)_TA;_TC=_T9[_TB];{
struct Cyc_Hashtable_Cell*iter=_TC.cells;_TL5A: if(iter!=0)goto _TL58;else{goto _TL59;}
_TL58: _TD=_tag_fat("(",sizeof(char),2U);_TE=_tag_fat(0U,sizeof(void*),0);Cyc_printf(_TD,_TE);_TF=iter;_T10=_TF->key;
prn_key(_T10);_T11=
_tag_fat(",",sizeof(char),2U);_T12=_tag_fat(0U,sizeof(void*),0);Cyc_printf(_T11,_T12);_T13=iter;_T14=_T13->value;
prn_val(_T14);_T15=
_tag_fat(") ",sizeof(char),3U);_T16=_tag_fat(0U,sizeof(void*),0);Cyc_printf(_T15,_T16);_T17=iter;
# 246
iter=_T17->next;goto _TL5A;_TL59:;}_T18=
# 253
_tag_fat("\n",sizeof(char),2U);_T19=_tag_fat(0U,sizeof(void*),0);Cyc_printf(_T18,_T19);
# 244
i=i + 1;goto _TL56;_TL55:;}}}
# 257
void Cyc_Hashtable_print_hist(struct Cyc_Hashtable_Table*t){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;unsigned*_T3;unsigned _T4;unsigned _T5;struct _fat_ptr _T6;unsigned char*_T7;struct Cyc_Hashtable_Bucket*_T8;unsigned _T9;int _TA;struct Cyc_Hashtable_Bucket _TB;struct _fat_ptr _TC;unsigned char*_TD;unsigned*_TE;unsigned _TF;int _T10;unsigned _T11;struct Cyc_Int_pa_PrintArg_struct _T12;unsigned _T13;int _T14;struct Cyc_Int_pa_PrintArg_struct _T15;struct _fat_ptr _T16;unsigned char*_T17;unsigned*_T18;unsigned _T19;int _T1A;unsigned _T1B;int _T1C;struct Cyc_Double_pa_PrintArg_struct _T1D;struct _fat_ptr _T1E;unsigned char*_T1F;unsigned*_T20;unsigned _T21;int _T22;unsigned _T23;unsigned _T24;unsigned _T25;unsigned _T26;struct Cyc___cycFILE*_T27;struct _fat_ptr _T28;struct _fat_ptr _T29;_T0=t;{
struct _fat_ptr odata=_T0->tab;_T1=odata;{
unsigned osize=_get_fat_size(_T1,sizeof(struct Cyc_Hashtable_Bucket));{unsigned _T2A=osize;_T4=_check_times(_T2A,sizeof(unsigned));{unsigned*_T2B=_cycalloc_atomic(_T4);{unsigned _T2C=_T2A;unsigned i;i=0;_TL5E: if(i < _T2C)goto _TL5C;else{goto _TL5D;}_TL5C: _T5=i;_T6=odata;_T7=_T6.curr;_T8=(struct Cyc_Hashtable_Bucket*)_T7;_T9=i;_TA=(int)_T9;_TB=_T8[_TA];
_T2B[_T5]=_TB.length;i=i + 1;goto _TL5E;_TL5D:;}_T3=(unsigned*)_T2B;}_T2=_tag_fat(_T3,sizeof(unsigned),_T2A);}{struct _fat_ptr counts=_T2;
unsigned sum=0U;{
unsigned i=0U;_TL62: if(i < osize)goto _TL60;else{goto _TL61;}
_TL60: _TC=counts;_TD=_TC.curr;_TE=(unsigned*)_TD;_TF=i;_T10=(int)_TF;_T11=_TE[_T10];sum=sum + _T11;
# 262
i=i + 1;goto _TL62;_TL61:;}
# 264
if(sum!=0U)goto _TL63;sum=1U;goto _TL64;_TL63: _TL64: {
unsigned i=0U;_TL68: if(i < osize)goto _TL66;else{goto _TL67;}
_TL66:{struct Cyc_Int_pa_PrintArg_struct _T2A;_T2A.tag=1;_T13=i;_T14=(int)_T13;_T2A.f1=(unsigned long)_T14;_T12=_T2A;}{struct Cyc_Int_pa_PrintArg_struct _T2A=_T12;{struct Cyc_Int_pa_PrintArg_struct _T2B;_T2B.tag=1;_T16=counts;_T17=_T16.curr;_T18=(unsigned*)_T17;_T19=i;_T1A=(int)_T19;_T1B=_T18[_T1A];_T1C=(int)_T1B;_T2B.f1=(unsigned long)_T1C;_T15=_T2B;}{struct Cyc_Int_pa_PrintArg_struct _T2B=_T15;{struct Cyc_Double_pa_PrintArg_struct _T2C;_T2C.tag=2;_T1E=counts;_T1F=_T1E.curr;_T20=(unsigned*)_T1F;_T21=i;_T22=(int)_T21;_T23=_T20[_T22];_T24=sum;_T25=_T23 / _T24;_T26=_T25 * 100U;_T2C.f1=(double)_T26;_T1D=_T2C;}{struct Cyc_Double_pa_PrintArg_struct _T2C=_T1D;void*_T2D[3];_T2D[0]=& _T2A;_T2D[1]=& _T2B;_T2D[2]=& _T2C;_T27=Cyc_stderr;_T28=_tag_fat("%d: %d (%g pct)\n",sizeof(char),17U);_T29=_tag_fat(_T2D,sizeof(void*),3);Cyc_fprintf(_T27,_T28,_T29);}}}
# 265
i=i + 1;goto _TL68;_TL67:;}}}}}

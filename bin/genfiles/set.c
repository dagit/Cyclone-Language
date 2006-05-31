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

# 173 "core.h"
 extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 117
extern void Cyc_List_app(void*(*)(void*),struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Set_Set{int(*cmp)(void*,void*);int cardinality;struct Cyc_List_List*nodes;};
# 37 "set.cyc"
struct Cyc_Set_Set*Cyc_Set_empty(int(*comp)(void*,void*)){struct Cyc_Set_Set*_T0;{struct Cyc_Set_Set*_T1=_cycalloc(sizeof(struct Cyc_Set_Set));
_T1->cmp=comp;_T1->cardinality=0;_T1->nodes=0;_T0=(struct Cyc_Set_Set*)_T1;}return _T0;}
# 40
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*rgn,int(*comp)(void*,void*)){struct Cyc_Set_Set*_T0;struct _RegionHandle*_T1;_T1=rgn;{struct Cyc_Set_Set*_T2=_region_malloc(_T1,0U,sizeof(struct Cyc_Set_Set));
_T2->cmp=comp;_T2->cardinality=0;_T2->nodes=0;_T0=(struct Cyc_Set_Set*)_T2;}return _T0;}
# 44
struct Cyc_Set_Set*Cyc_Set_singleton(int(*comp)(void*,void*),void*x){struct Cyc_Set_Set*_T0;struct Cyc_List_List*_T1;{struct Cyc_Set_Set*_T2=_cycalloc(sizeof(struct Cyc_Set_Set));
_T2->cmp=comp;_T2->cardinality=1;{struct Cyc_List_List*_T3=_cycalloc(sizeof(struct Cyc_List_List));_T3->hd=x;_T3->tl=0;_T1=(struct Cyc_List_List*)_T3;}_T2->nodes=_T1;_T0=(struct Cyc_Set_Set*)_T2;}return _T0;}
# 48
int Cyc_Set_cardinality(struct Cyc_Set_Set*s){struct Cyc_Set_Set*_T0;int _T1;_T0=s;_T1=_T0->cardinality;
return _T1;}
# 53
int Cyc_Set_is_empty(struct Cyc_Set_Set*s){struct Cyc_Set_Set*_T0;int _T1;int _T2;_T0=s;_T1=_T0->cardinality;_T2=_T1==0;
return _T2;}
# 58
static int Cyc_Set_member_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){void*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;
_TL3: if(n!=0)goto _TL1;else{goto _TL2;}
_TL1: _T0=elt;_T1=n;_T2=_T1->hd;{int i=cmp(_T0,_T2);
if(i!=0)goto _TL4;return 1;_TL4:
 if(i >= 0)goto _TL6;return 0;_TL6:;}_T3=n;
# 59
n=_T3->tl;goto _TL3;_TL2:
# 64
 return 0;}
# 67
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt){struct Cyc_Set_Set*_T0;int(*_T1)(void*,void*);struct Cyc_Set_Set*_T2;struct Cyc_List_List*_T3;void*_T4;int _T5;_T0=s;_T1=_T0->cmp;_T2=s;_T3=_T2->nodes;_T4=elt;_T5=
Cyc_Set_member_b(_T1,_T3,_T4);return _T5;}
# 74
static struct Cyc_List_List*Cyc_Set_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;struct _RegionHandle*_T6;struct _RegionHandle*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;void*_TD;void*_TE;int _TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct _RegionHandle*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct _RegionHandle*_T18;struct Cyc_List_List*_T19;
# 77
if(n!=0)goto _TL8;_T1=rgn;{struct Cyc_List_List*_T1A=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));
_T1A->hd=elt;_T1A->tl=0;_T0=(struct Cyc_List_List*)_T1A;}return _T0;_TL8: _T2=elt;_T3=n;_T4=_T3->hd;{
int i=cmp(_T2,_T4);
if(i >= 0)goto _TLA;_T6=rgn;{struct Cyc_List_List*_T1A=_region_malloc(_T6,0U,sizeof(struct Cyc_List_List));
_T1A->hd=elt;_T1A->tl=n;_T5=(struct Cyc_List_List*)_T1A;}return _T5;_TLA: {
struct Cyc_List_List*result;_T7=rgn;result=_region_malloc(_T7,0U,sizeof(struct Cyc_List_List));_T8=result;_T9=n;_T8->hd=_T9->hd;_TA=result;_TA->tl=0;{
struct Cyc_List_List*prev=result;_TB=n;
n=_TB->tl;
_TLC: if(n!=0)goto _TLF;else{goto _TLE;}_TLF: _TC=n;_TD=_TC->hd;_TE=elt;i=cmp(_TD,_TE);_TF=i;if(_TF < 0)goto _TLD;else{goto _TLE;}
_TLD: _T10=_check_null(prev);_T12=rgn;{struct Cyc_List_List*_T1A=_region_malloc(_T12,0U,sizeof(struct Cyc_List_List));_T13=n;_T1A->hd=_T13->hd;_T1A->tl=0;_T11=(struct Cyc_List_List*)_T1A;}_T10->tl=_T11;_T14=prev;
prev=_T14->tl;_T15=n;
n=_T15->tl;goto _TLC;_TLE: _T16=
# 90
_check_null(prev);_T18=rgn;{struct Cyc_List_List*_T1A=_region_malloc(_T18,0U,sizeof(struct Cyc_List_List));_T1A->hd=elt;_T1A->tl=n;_T17=(struct Cyc_List_List*)_T1A;}_T16->tl=_T17;_T19=result;
return _T19;}}}}
# 95
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt){int _T0;struct Cyc_Set_Set*_T1;struct Cyc_Set_Set*_T2;struct Cyc_Set_Set*_T3;struct Cyc_Set_Set*_T4;int _T5;struct _RegionHandle*_T6;struct Cyc_Set_Set*_T7;int(*_T8)(void*,void*);struct Cyc_Set_Set*_T9;struct Cyc_List_List*_TA;void*_TB;_T0=
Cyc_Set_member(s,elt);if(!_T0)goto _TL10;_T1=s;return _T1;_TL10:{struct Cyc_Set_Set*_TC=_cycalloc(sizeof(struct Cyc_Set_Set));_T3=s;
_TC->cmp=_T3->cmp;_T4=s;_T5=_T4->cardinality;_TC->cardinality=_T5 + 1;_T6=Cyc_Core_heap_region;_T7=s;_T8=_T7->cmp;_T9=s;_TA=_T9->nodes;_TB=elt;
_TC->nodes=Cyc_Set_insert_b(_T6,_T8,_TA,_TB);_T2=(struct Cyc_Set_Set*)_TC;}
# 97
return _T2;}
# 100
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){int _T0;struct Cyc_Set_Set*_T1;struct Cyc_Set_Set*_T2;struct _RegionHandle*_T3;struct Cyc_Set_Set*_T4;struct Cyc_Set_Set*_T5;int _T6;struct _RegionHandle*_T7;struct Cyc_Set_Set*_T8;int(*_T9)(void*,void*);struct Cyc_Set_Set*_TA;struct Cyc_List_List*_TB;void*_TC;_T0=
Cyc_Set_member(s,elt);if(!_T0)goto _TL12;_T1=s;return _T1;_TL12: _T3=rgn;{struct Cyc_Set_Set*_TD=_region_malloc(_T3,0U,sizeof(struct Cyc_Set_Set));_T4=s;
_TD->cmp=_T4->cmp;_T5=s;_T6=_T5->cardinality;_TD->cardinality=_T6 + 1;_T7=rgn;_T8=s;_T9=_T8->cmp;_TA=s;_TB=_TA->nodes;_TC=elt;
_TD->nodes=Cyc_Set_insert_b(_T7,_T9,_TB,_TC);_T2=(struct Cyc_Set_Set*)_TD;}
# 102
return _T2;}
# 108
static struct Cyc_List_List*Cyc_Set_imp_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;struct _RegionHandle*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;void*_T9;void*_TA;int _TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct _RegionHandle*_T10;struct Cyc_List_List*_T11;
# 111
if(n!=0)goto _TL14;_T1=rgn;{struct Cyc_List_List*_T12=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));
_T12->hd=elt;_T12->tl=0;_T0=(struct Cyc_List_List*)_T12;}return _T0;_TL14: _T2=elt;_T3=n;_T4=_T3->hd;{
int i=cmp(_T2,_T4);
if(i >= 0)goto _TL16;_T6=rgn;{struct Cyc_List_List*_T12=_region_malloc(_T6,0U,sizeof(struct Cyc_List_List));
_T12->hd=elt;_T12->tl=n;_T5=(struct Cyc_List_List*)_T12;}return _T5;_TL16: {
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;_T7=n;
n=_T7->tl;
_TL18: if(n!=0)goto _TL1B;else{goto _TL1A;}_TL1B: _T8=n;_T9=_T8->hd;_TA=elt;i=cmp(_T9,_TA);_TB=i;if(_TB < 0)goto _TL19;else{goto _TL1A;}
_TL19: _TC=_check_null(prev);prev=_TC->tl;_TD=n;
n=_TD->tl;goto _TL18;_TL1A: _TE=
# 122
_check_null(prev);_T10=rgn;{struct Cyc_List_List*_T12=_region_malloc(_T10,0U,sizeof(struct Cyc_List_List));_T12->hd=elt;_T12->tl=n;_TF=(struct Cyc_List_List*)_T12;}_TE->tl=_TF;_T11=res;
return _T11;}}}
# 127
void Cyc_Set_imp_insert(struct Cyc_Set_Set*s,void*elt){int _T0;struct Cyc_Set_Set*_T1;struct _RegionHandle*_T2;struct Cyc_Set_Set*_T3;int(*_T4)(void*,void*);struct Cyc_Set_Set*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Set_Set*_T8;_T0=
Cyc_Set_member(s,elt);if(_T0)goto _TL1C;else{goto _TL1E;}
_TL1E: _T1=s;_T2=Cyc_Core_heap_region;_T3=s;_T4=_T3->cmp;_T5=s;_T6=_T5->nodes;_T7=elt;_T1->nodes=Cyc_Set_imp_insert_b(_T2,_T4,_T6,_T7);_T8=s;
_T8->cardinality=_T8->cardinality + 1;goto _TL1D;_TL1C: _TL1D:;}
# 133
void Cyc_Set_imp_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){int _T0;struct Cyc_Set_Set*_T1;struct _RegionHandle*_T2;struct Cyc_Set_Set*_T3;int(*_T4)(void*,void*);struct Cyc_Set_Set*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Set_Set*_T8;_T0=
Cyc_Set_member(s,elt);if(_T0)goto _TL1F;else{goto _TL21;}
_TL21: _T1=s;_T2=rgn;_T3=s;_T4=_T3->cmp;_T5=s;_T6=_T5->nodes;_T7=elt;_T1->nodes=Cyc_Set_imp_insert_b(_T2,_T4,_T6,_T7);_T8=s;
_T8->cardinality=_T8->cardinality + 1;goto _TL20;_TL1F: _TL20:;}
# 141
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){struct Cyc_Set_Set*_T0;struct Cyc_Set_Set*_T1;int _T2;struct Cyc_Set_Set*_T3;struct Cyc_Set_Set*_T4;int _T5;struct Cyc_Set_Set*_T6;struct Cyc_Set_Set*_T7;struct Cyc_Set_Set*_T8;struct Cyc_Set_Set*_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_List_List*_TC;void*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;int _T1E;struct Cyc_List_List*_T1F;int _T20;struct Cyc_Set_Set*_T21;
if(s1!=s2)goto _TL22;_T0=s1;
return _T0;_TL22: _T1=s1;_T2=_T1->cardinality;
if(_T2!=0)goto _TL24;_T3=s2;
return _T3;_TL24: _T4=s2;_T5=_T4->cardinality;
if(_T5!=0)goto _TL26;_T6=s1;
return _T6;_TL26: {
# 149
struct Cyc_List_List*nodes=0;
int cardinality=0;_T7=s1;{
int(*comp)(void*,void*)=_T7->cmp;_T8=s1;{
# 153
struct Cyc_List_List*x1=_T8->nodes;_T9=s2;{
struct Cyc_List_List*x2=_T9->nodes;
struct Cyc_List_List*curr=0;
# 157
_TL28: if(x1!=0)goto _TL2B;else{goto _TL2A;}_TL2B: if(x2!=0)goto _TL29;else{goto _TL2A;}
_TL29: _TA=x1;_TB=_TA->hd;_TC=x2;_TD=_TC->hd;{int i=comp(_TB,_TD);
if(i!=0)goto _TL2C;_TE=x2;
# 161
x2=_TE->tl;goto _TL2D;
_TL2C: if(i >= 0)goto _TL2E;
# 164
if(curr!=0)goto _TL30;{struct Cyc_List_List*_T22=_cycalloc(sizeof(struct Cyc_List_List));_T10=x1;
_T22->hd=_T10->hd;_T22->tl=0;_TF=(struct Cyc_List_List*)_T22;}nodes=_TF;
curr=nodes;goto _TL31;
# 168
_TL30: _T11=curr;{struct Cyc_List_List*_T22=_cycalloc(sizeof(struct Cyc_List_List));_T13=x1;_T22->hd=_T13->hd;_T22->tl=0;_T12=(struct Cyc_List_List*)_T22;}_T11->tl=_T12;_T14=curr;
curr=_T14->tl;_TL31: _T15=x1;
# 171
x1=_T15->tl;
cardinality=cardinality + 1;goto _TL2F;
# 175
_TL2E: if(curr!=0)goto _TL32;{struct Cyc_List_List*_T22=_cycalloc(sizeof(struct Cyc_List_List));_T17=x2;
_T22->hd=_T17->hd;_T22->tl=0;_T16=(struct Cyc_List_List*)_T22;}nodes=_T16;
curr=nodes;goto _TL33;
# 179
_TL32: _T18=curr;{struct Cyc_List_List*_T22=_cycalloc(sizeof(struct Cyc_List_List));_T1A=x2;_T22->hd=_T1A->hd;_T22->tl=0;_T19=(struct Cyc_List_List*)_T22;}_T18->tl=_T19;_T1B=curr;
curr=_T1B->tl;_TL33: _T1C=x2;
# 182
x2=_T1C->tl;
cardinality=cardinality + 1;_TL2F: _TL2D:;}goto _TL28;_TL2A:
# 186
 if(x1==0)goto _TL34;
# 188
if(curr!=0)goto _TL36;
nodes=x1;goto _TL37;
# 191
_TL36: _T1D=curr;_T1D->tl=x1;_TL37: _T1E=
Cyc_List_length(x1);cardinality=cardinality + _T1E;goto _TL35;
_TL34: if(x2==0)goto _TL38;
# 195
if(curr!=0)goto _TL3A;
nodes=x2;goto _TL3B;
# 198
_TL3A: _T1F=curr;_T1F->tl=x2;_TL3B: _T20=
Cyc_List_length(x2);cardinality=cardinality + _T20;goto _TL39;_TL38: _TL39: _TL35:{struct Cyc_Set_Set*_T22=_cycalloc(sizeof(struct Cyc_Set_Set));
# 201
_T22->cmp=comp;_T22->cardinality=cardinality;_T22->nodes=nodes;_T21=(struct Cyc_Set_Set*)_T22;}return _T21;}}}}}
# 206
static struct Cyc_List_List*Cyc_Set_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){struct Cyc_List_List*_T0;void*_T1;void*_T2;int _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;void*_TB;void*_TC;int _TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;_T0=
_check_null(n);_T1=_T0->hd;_T2=elt;_T3=cmp(_T1,_T2);if(_T3!=0)goto _TL3C;_T4=n;_T5=_T4->tl;return _T5;_TL3C: {
# 209
struct Cyc_List_List*result;result=_cycalloc(sizeof(struct Cyc_List_List));_T6=result;_T7=n;_T6->hd=_T7->hd;_T8=result;_T8->tl=0;{
struct Cyc_List_List*prev=result;_T9=n;
n=_T9->tl;
_TL3E: if(n!=0)goto _TL41;else{goto _TL40;}_TL41: _TA=n;_TB=_TA->hd;_TC=elt;_TD=cmp(_TB,_TC);if(_TD!=0)goto _TL3F;else{goto _TL40;}
_TL3F: _TE=_check_null(prev);{struct Cyc_List_List*_T16=_cycalloc(sizeof(struct Cyc_List_List));_T10=n;_T16->hd=_T10->hd;_T16->tl=0;_TF=(struct Cyc_List_List*)_T16;}_TE->tl=_TF;_T11=prev;
prev=_T11->tl;_T12=n;
n=_T12->tl;goto _TL3E;_TL40: _T13=
# 217
_check_null(prev);_T14=_check_null(n);_T13->tl=_T14->tl;_T15=result;
return _T15;}}}
# 222
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt){int _T0;struct Cyc_Set_Set*_T1;struct Cyc_Set_Set*_T2;struct Cyc_Set_Set*_T3;int _T4;struct Cyc_Set_Set*_T5;int(*_T6)(void*,void*);struct Cyc_Set_Set*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_Set_Set*_TA;_T0=
Cyc_Set_member(s,elt);if(!_T0)goto _TL42;{struct Cyc_Set_Set*_TB=_cycalloc(sizeof(struct Cyc_Set_Set));_T2=s;
_TB->cmp=_T2->cmp;_T3=s;_T4=_T3->cardinality;_TB->cardinality=_T4 - 1;_T5=s;_T6=_T5->cmp;_T7=s;_T8=_T7->nodes;_T9=elt;
_TB->nodes=Cyc_Set_delete_b(_T6,_T8,_T9);_T1=(struct Cyc_Set_Set*)_TB;}
# 224
return _T1;_TL42: _TA=s;
# 226
return _TA;}
# 232
static struct Cyc_List_List*Cyc_Set_imp_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt,void**ret){struct Cyc_List_List*_T0;void*_T1;void*_T2;int _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;void*_T8;void*_T9;int _TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;void**_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;_T0=
_check_null(n);_T1=_T0->hd;_T2=elt;_T3=cmp(_T1,_T2);if(_T3!=0)goto _TL44;_T4=n;_T5=_T4->tl;return _T5;_TL44: {
# 235
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;_T6=n;
n=_T6->tl;
_TL46: if(n!=0)goto _TL49;else{goto _TL48;}_TL49: _T7=n;_T8=_T7->hd;_T9=elt;_TA=cmp(_T8,_T9);if(_TA!=0)goto _TL47;else{goto _TL48;}
_TL47: _TB=_check_null(prev);prev=_TB->tl;_TC=n;
n=_TC->tl;goto _TL46;_TL48: _TD=
# 241
_check_null(prev);_TE=_check_null(n);_TD->tl=_TE->tl;_TF=ret;_T10=n;
*_TF=_T10->hd;_T11=res;
return _T11;}}
# 247
void*Cyc_Set_imp_delete(struct Cyc_Set_Set*s,void*elt){int _T0;struct Cyc_Set_Set*_T1;struct Cyc_Set_Set*_T2;int(*_T3)(void*,void*);struct Cyc_Set_Set*_T4;struct Cyc_List_List*_T5;void*_T6;void**_T7;struct Cyc_Set_Set*_T8;void*_T9;
void*ret=elt;_T0=
Cyc_Set_member(s,elt);if(!_T0)goto _TL4A;_T1=s;_T2=s;_T3=_T2->cmp;_T4=s;_T5=_T4->nodes;_T6=elt;_T7=& ret;
_T1->nodes=Cyc_Set_imp_delete_b(_T3,_T5,_T6,_T7);_T8=s;
_T8->cardinality=_T8->cardinality + -1;goto _TL4B;_TL4A: _TL4B: _T9=ret;
# 253
return _T9;}
# 257
void*Cyc_Set_fold(void*(*f)(void*,void*),struct Cyc_Set_Set*s,void*accum){struct Cyc_Set_Set*_T0;struct Cyc_List_List*_T1;void*_T2;void*_T3;struct Cyc_List_List*_T4;void*_T5;_T0=s;{
struct Cyc_List_List*n=_T0->nodes;_TL4F: if(n!=0)goto _TL4D;else{goto _TL4E;}
_TL4D: _T1=n;_T2=_T1->hd;_T3=accum;accum=f(_T2,_T3);_T4=n;
# 258
n=_T4->tl;goto _TL4F;_TL4E:;}_T5=accum;
# 260
return _T5;}
# 262
void*Cyc_Set_fold_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Set_Set*s,void*accum){struct Cyc_Set_Set*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;void*_T4;struct Cyc_List_List*_T5;void*_T6;_T0=s;{
struct Cyc_List_List*n=_T0->nodes;_TL53: if(n!=0)goto _TL51;else{goto _TL52;}
_TL51: _T1=env;_T2=n;_T3=_T2->hd;_T4=accum;accum=f(_T1,_T3,_T4);_T5=n;
# 263
n=_T5->tl;goto _TL53;_TL52:;}_T6=accum;
# 265
return _T6;}
# 269
void Cyc_Set_app(void*(*f)(void*),struct Cyc_Set_Set*s){void*(*_T0)(void*);struct Cyc_Set_Set*_T1;struct Cyc_List_List*_T2;_T0=f;_T1=s;_T2=_T1->nodes;
Cyc_List_app(_T0,_T2);}
# 272
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s){void(*_T0)(void*);struct Cyc_Set_Set*_T1;struct Cyc_List_List*_T2;_T0=f;_T1=s;_T2=_T1->nodes;
Cyc_List_iter(_T0,_T2);}
# 275
void Cyc_Set_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Set_Set*s){void(*_T0)(void*,void*);void*_T1;struct Cyc_Set_Set*_T2;struct Cyc_List_List*_T3;_T0=f;_T1=env;_T2=s;_T3=_T2->nodes;
Cyc_List_iter_c(_T0,_T1,_T3);}
# 282
struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){struct Cyc_Set_Set*_T0;struct Cyc_Set_Set*_T1;struct Cyc_Set_Set*_T2;struct Cyc_Set_Set*_T3;struct Cyc_Set_Set*_T4;struct Cyc_Set_Set*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_Set_Set*_T14;
if(s1!=s2)goto _TL54;_T0=s1;
return _T0;_TL54: _T1=s1;{
int(*comp)(void*,void*)=_T1->cmp;_T2=s1;{
struct Cyc_List_List*x1=_T2->nodes;_T3=s2;{
struct Cyc_List_List*x2=_T3->nodes;
struct Cyc_List_List*result=0;struct Cyc_List_List*prev=0;
int card=0;
if(x1!=0)goto _TL56;_T4=s1;
return _T4;_TL56:
 if(x2!=0)goto _TL58;_T5=s2;
return _T5;_TL58:
# 295
 _TL5A: if(x1!=0)goto _TL5D;else{goto _TL5C;}_TL5D: if(x2!=0)goto _TL5B;else{goto _TL5C;}
_TL5B: _T6=x1;_T7=_T6->hd;_T8=x2;_T9=_T8->hd;{int i=comp(_T7,_T9);
if(i!=0)goto _TL5E;
if(result!=0)goto _TL60;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));_TB=x1;
_T15->hd=_TB->hd;_T15->tl=0;_TA=(struct Cyc_List_List*)_T15;}result=_TA;
prev=result;goto _TL61;
# 302
_TL60: _TC=_check_null(prev);{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));_TE=x1;_T15->hd=_TE->hd;_T15->tl=0;_TD=(struct Cyc_List_List*)_T15;}_TC->tl=_TD;_TF=prev;
prev=_TF->tl;_TL61:
# 305
 card=card + 1;_T10=x1;
x1=_T10->tl;_T11=x2;
x2=_T11->tl;goto _TL5F;
_TL5E: if(i >= 0)goto _TL62;_T12=x1;
x1=_T12->tl;goto _TL63;
# 311
_TL62: _T13=x2;x2=_T13->tl;_TL63: _TL5F:;}goto _TL5A;_TL5C:{struct Cyc_Set_Set*_T15=_cycalloc(sizeof(struct Cyc_Set_Set));
# 314
_T15->cmp=comp;_T15->cardinality=card;_T15->nodes=result;_T14=(struct Cyc_Set_Set*)_T15;}return _T14;}}}}
# 317
struct Cyc_Set_Set*Cyc_Set_from_list(int(*comp)(void*,void*),struct Cyc_List_List*x){int(*_T0)(void*,void*);struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;void*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_Set_Set*_TF;_T0=comp;_T1=x;{
struct Cyc_List_List*z=Cyc_List_merge_sort(_T0,_T1);{
# 320
struct Cyc_List_List*y=z;_TL67: if(y!=0)goto _TL65;else{goto _TL66;}
_TL65: _T2=y;_T3=_T2->tl;if(_T3==0)goto _TL68;_T4=y;_T5=_T4->hd;_T6=y;_T7=_T6->tl;_T8=_T7->hd;_T9=comp(_T5,_T8);if(_T9!=0)goto _TL68;_TA=y;_TB=y;_TC=_TB->tl;_TD=
_check_null(_TC);_TA->tl=_TD->tl;goto _TL69;_TL68: _TL69: _TE=y;
# 320
y=_TE->tl;goto _TL67;_TL66:;}{struct Cyc_Set_Set*_T10=_cycalloc(sizeof(struct Cyc_Set_Set));
# 323
_T10->cmp=comp;_T10->cardinality=Cyc_List_length(z);_T10->nodes=z;_TF=(struct Cyc_Set_Set*)_T10;}return _TF;}}
# 326
struct Cyc_List_List*Cyc_Set_to_list(struct Cyc_Set_Set*s){struct Cyc_Set_Set*_T0;struct Cyc_List_List*_T1;_T0=s;_T1=_T0->nodes;
return _T1;}
# 330
int Cyc_Set_subset(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){struct Cyc_Set_Set*_T0;struct Cyc_Set_Set*_T1;struct Cyc_Set_Set*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;void*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;_T0=s1;{
int(*comp)(void*,void*)=_T0->cmp;_T1=s1;{
struct Cyc_List_List*x1=_T1->nodes;_T2=s2;{
struct Cyc_List_List*x2=_T2->nodes;
# 335
_TL6A: if(1)goto _TL6B;else{goto _TL6C;}
_TL6B: if(x1!=0)goto _TL6D;return 1;_TL6D:
 if(x2!=0)goto _TL6F;return 0;_TL6F: _T3=x1;_T4=_T3->hd;_T5=x2;_T6=_T5->hd;{
int i=comp(_T4,_T6);
if(i >= 0)goto _TL71;
return 0;_TL71:
 if(i!=0)goto _TL73;_T7=x1;
x1=_T7->tl;goto _TL74;_TL73: _TL74: _T8=x2;
x2=_T8->tl;}goto _TL6A;_TL6C:
# 345
 return 1;}}}}
# 348
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){struct Cyc_Set_Set*_T0;struct Cyc_Set_Set*_T1;struct Cyc_Set_Set*_T2;struct Cyc_Set_Set*_T3;struct Cyc_Set_Set*_T4;struct Cyc_List_List*_T5;int(*_T6)(void*,void*);struct Cyc_List_List*_T7;void*_T8;int _T9;int(*_TA)(void*,void*);struct Cyc_List_List*_TB;void*_TC;struct Cyc_List_List*_TD;struct Cyc_Set_Set*_TE;_T0=s1;{
int(*comp)(void*,void*)=_T0->cmp;_T1=s1;{
struct Cyc_List_List*x1=_T1->nodes;_T2=s2;{
struct Cyc_List_List*x2=_T2->nodes;_T3=s1;{
int card=_T3->cardinality;
# 354
if(x2!=0)goto _TL75;_T4=s1;return _T4;_TL75:
# 356
 _TL7A: if(x2!=0)goto _TL78;else{goto _TL79;}
_TL78: _T5=x2;{void*elt=_T5->hd;_T6=comp;_T7=x1;_T8=elt;_T9=
Cyc_Set_member_b(_T6,_T7,_T8);if(!_T9)goto _TL7B;
card=card + -1;_TA=comp;_TB=x1;_TC=elt;
x1=Cyc_Set_delete_b(_TA,_TB,_TC);goto _TL7C;_TL7B: _TL7C:;}_TD=x2;
# 356
x2=_TD->tl;goto _TL7A;_TL79:{struct Cyc_Set_Set*_TF=_cycalloc(sizeof(struct Cyc_Set_Set));
# 363
_TF->cmp=comp;_TF->cardinality=card;_TF->nodes=x1;_TE=(struct Cyc_Set_Set*)_TF;}return _TE;}}}}}
# 366
int Cyc_Set_setcmp(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){struct Cyc_Set_Set*_T0;unsigned _T1;struct Cyc_Set_Set*_T2;unsigned _T3;struct Cyc_Set_Set*_T4;int _T5;struct Cyc_Set_Set*_T6;int _T7;struct Cyc_Set_Set*_T8;int _T9;struct Cyc_Set_Set*_TA;int _TB;int _TC;struct Cyc_Set_Set*_TD;struct Cyc_Set_Set*_TE;struct Cyc_Set_Set*_TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_List_List*_T12;void*_T13;int _T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;_T0=s1;_T1=(unsigned)_T0;_T2=s2;_T3=(unsigned)_T2;
if(_T1!=_T3)goto _TL7D;
return 0;_TL7D: _T4=s1;_T5=_T4->cardinality;_T6=s2;_T7=_T6->cardinality;
if(_T5==_T7)goto _TL7F;_T8=s1;_T9=_T8->cardinality;_TA=s2;_TB=_TA->cardinality;_TC=_T9 - _TB;
return _TC;_TL7F: _TD=s1;{
# 372
struct Cyc_List_List*x1=_TD->nodes;_TE=s2;{
struct Cyc_List_List*x2=_TE->nodes;_TF=s1;{
int(*cmp)(void*,void*)=_TF->cmp;
_TL81: if(x1!=0)goto _TL82;else{goto _TL83;}
_TL82: _T10=x1;_T11=_T10->hd;_T12=_check_null(x2);_T13=_T12->hd;{int diff=cmp(_T11,_T13);
if(diff==0)goto _TL84;_T14=diff;return _T14;_TL84: _T15=x1;
x1=_T15->tl;_T16=x2;
x2=_T16->tl;}goto _TL81;_TL83:
# 381
 return 0;}}}}
# 384
int Cyc_Set_equals(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){int _T0;int _T1;_T0=
Cyc_Set_setcmp(s1,s2);_T1=_T0==0;return _T1;}char Cyc_Set_Absent[7U]="Absent";
# 389
struct Cyc_Set_Absent_exn_struct Cyc_Set_Absent_val={Cyc_Set_Absent};
# 393
void*Cyc_Set_choose(struct Cyc_Set_Set*s){struct Cyc_Set_Set*_T0;struct Cyc_List_List*_T1;struct Cyc_Set_Absent_exn_struct*_T2;struct Cyc_Set_Absent_exn_struct*_T3;struct Cyc_Set_Set*_T4;struct Cyc_List_List*_T5;void*_T6;_T0=s;_T1=_T0->nodes;
if(_T1!=0)goto _TL86;_T2=& Cyc_Set_Absent_val;_T3=(struct Cyc_Set_Absent_exn_struct*)_T2;_throw(_T3);goto _TL87;_TL86: _TL87: _T4=s;_T5=_T4->nodes;_T6=_T5->hd;
return _T6;}
# 398
int Cyc_Set_iter_f(struct Cyc_List_List**elts_left,void**dest){struct Cyc_List_List**_T0;struct Cyc_List_List*_T1;unsigned _T2;void**_T3;struct Cyc_List_List**_T4;struct Cyc_List_List*_T5;struct Cyc_List_List**_T6;struct Cyc_List_List**_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;_T0=elts_left;_T1=*_T0;_T2=(unsigned)_T1;
if(_T2)goto _TL88;else{goto _TL8A;}
_TL8A: return 0;_TL88: _T3=dest;_T4=elts_left;_T5=*_T4;
*_T3=_T5->hd;_T6=elts_left;_T7=elts_left;_T8=*_T7;_T9=
_check_null(_T8);*_T6=_T9->tl;
return 1;}
# 405
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s){struct Cyc_Iter_Iter _T0;struct Cyc_List_List**_T1;struct _RegionHandle*_T2;struct Cyc_Set_Set*_T3;{struct Cyc_Iter_Iter _T4;_T2=rgn;{struct Cyc_List_List**_T5=_region_malloc(_T2,0U,sizeof(struct Cyc_List_List*));_T3=s;
*_T5=_T3->nodes;_T1=(struct Cyc_List_List**)_T5;}_T4.env=_T1;
_T4.next=Cyc_Set_iter_f;_T0=_T4;}
# 406
return _T0;}

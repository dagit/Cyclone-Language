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
 struct Cyc_Core_Opt{void*v;};struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 154 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 246 "list.h"
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*,int);struct _tuple0{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 374
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*,struct _fat_ptr);
# 30 "list.cyc"
int Cyc_List_length(struct Cyc_List_List*x){struct Cyc_List_List*_T0;int _T1;
int i=0;
_TL3: if(x!=0)goto _TL1;else{goto _TL2;}
_TL1: i=i + 1;_T0=x;
# 32
x=_T0->tl;goto _TL3;_TL2: _T1=i;
# 34
return _T1;}
# 38
void*Cyc_List_hd(struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;_T0=x;_T1=_T0->hd;
return _T1;}
# 43
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;_T0=x;_T1=_T0->tl;
return _T1;}
# 49
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*r,struct _fat_ptr argv){struct Cyc_List_List*_T0;_T0=
Cyc_List_rfrom_array(r,argv);return _T0;}
# 53
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr argv){struct Cyc_List_List*_T0;_T0=
Cyc_List_from_array(argv);return _T0;}
# 58
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*r,struct Cyc_List_List*x){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct _RegionHandle*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 61
if(x!=0)goto _TL4;return 0;_TL4: _T1=r;{struct Cyc_List_List*_TB=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));_T2=x;
_TB->hd=_T2->hd;_TB->tl=0;_T0=(struct Cyc_List_List*)_TB;}result=_T0;
prev=result;_T3=x;
x=_T3->tl;_TL9: if(x!=0)goto _TL7;else{goto _TL8;}
_TL7:{struct Cyc_List_List*temp;_T4=r;temp=_region_malloc(_T4,0U,sizeof(struct Cyc_List_List));_T5=temp;_T6=x;_T5->hd=_T6->hd;_T7=temp;_T7->tl=0;_T8=prev;
_T8->tl=temp;
prev=temp;}_T9=x;
# 64
x=_T9->tl;goto _TL9;_TL8: _TA=result;
# 69
return _TA;}
# 72
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x){struct Cyc_List_List*_T0;_T0=
Cyc_List_rcopy(Cyc_Core_heap_region,x);return _T0;}
# 77
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_List_List*x){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;struct _RegionHandle*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 80
if(x!=0)goto _TLA;return 0;_TLA: _T1=r;{struct Cyc_List_List*_TD=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));_T2=x;_T3=_T2->hd;
_TD->hd=f(_T3);_TD->tl=0;_T0=(struct Cyc_List_List*)_TD;}result=_T0;
prev=result;_T4=x;
x=_T4->tl;_TLF: if(x!=0)goto _TLD;else{goto _TLE;}
_TLD:{struct Cyc_List_List*temp;_T5=r;temp=_region_malloc(_T5,0U,sizeof(struct Cyc_List_List));_T6=temp;_T7=x;_T8=_T7->hd;_T6->hd=f(_T8);_T9=temp;_T9->tl=0;_TA=prev;
_TA->tl=temp;
prev=temp;}_TB=x;
# 83
x=_TB->tl;goto _TLF;_TLE: _TC=result;
# 88
return _TC;}
# 91
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x){struct _RegionHandle*_T0;void*(*_T1)(void*);struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;_T0=Cyc_Core_heap_region;_T1=f;_T2=x;_T3=
Cyc_List_rmap(_T0,_T1,_T2);return _T3;}
# 96
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;struct _RegionHandle*_T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 99
if(x!=0)goto _TL10;return 0;_TL10: _T1=r;{struct Cyc_List_List*_TF=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));_T2=env;_T3=x;_T4=_T3->hd;
_TF->hd=f(_T2,_T4);_TF->tl=0;_T0=(struct Cyc_List_List*)_TF;}result=_T0;
prev=result;_T5=x;
x=_T5->tl;_TL15: if(x!=0)goto _TL13;else{goto _TL14;}
_TL13:{struct Cyc_List_List*e;_T6=r;e=_region_malloc(_T6,0U,sizeof(struct Cyc_List_List));_T7=e;_T8=env;_T9=x;_TA=_T9->hd;_T7->hd=f(_T8,_TA);_TB=e;_TB->tl=0;_TC=prev;
_TC->tl=e;
prev=e;}_TD=x;
# 102
x=_TD->tl;goto _TL15;_TL14: _TE=result;
# 107
return _TE;}
# 110
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){struct _RegionHandle*_T0;void*(*_T1)(void*,void*);void*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;_T0=Cyc_Core_heap_region;_T1=f;_T2=env;_T3=x;_T4=
Cyc_List_rmap_c(_T0,_T1,_T2,_T3);return _T4;}char Cyc_List_List_mismatch[14U]="List_mismatch";
# 116
struct Cyc_List_List_mismatch_exn_struct Cyc_List_List_mismatch_val={Cyc_List_List_mismatch};
# 121
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*r,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List_mismatch_exn_struct*_T0;struct Cyc_List_List_mismatch_exn_struct*_T1;struct Cyc_List_List*_T2;struct _RegionHandle*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct _RegionHandle*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;void*_TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List_mismatch_exn_struct*_T14;struct Cyc_List_List_mismatch_exn_struct*_T15;struct Cyc_List_List*_T16;
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 124
if(x!=0)goto _TL16;if(y!=0)goto _TL16;return 0;_TL16:
 if(x==0)goto _TL1A;else{goto _TL1B;}_TL1B: if(y==0)goto _TL1A;else{goto _TL18;}_TL1A: _T0=& Cyc_List_List_mismatch_val;_T1=(struct Cyc_List_List_mismatch_exn_struct*)_T0;_throw(_T1);goto _TL19;_TL18: _TL19: _T3=r;{struct Cyc_List_List*_T17=_region_malloc(_T3,0U,sizeof(struct Cyc_List_List));_T4=x;_T5=_T4->hd;_T6=y;_T7=_T6->hd;
# 127
_T17->hd=f(_T5,_T7);_T17->tl=0;_T2=(struct Cyc_List_List*)_T17;}result=_T2;
prev=result;_T8=x;
x=_T8->tl;_T9=y;
y=_T9->tl;
# 132
_TL1C: if(x!=0)goto _TL1F;else{goto _TL1E;}_TL1F: if(y!=0)goto _TL1D;else{goto _TL1E;}
_TL1D:{struct Cyc_List_List*temp;_TA=r;temp=_region_malloc(_TA,0U,sizeof(struct Cyc_List_List));_TB=temp;_TC=x;_TD=_TC->hd;_TE=y;_TF=_TE->hd;_TB->hd=f(_TD,_TF);_T10=temp;_T10->tl=0;_T11=prev;
_T11->tl=temp;
prev=temp;_T12=x;
x=_T12->tl;_T13=y;
y=_T13->tl;}goto _TL1C;_TL1E:
# 139
 if(x!=0)goto _TL22;else{goto _TL23;}_TL23: if(y!=0)goto _TL22;else{goto _TL20;}_TL22: _T14=& Cyc_List_List_mismatch_val;_T15=(struct Cyc_List_List_mismatch_exn_struct*)_T14;_throw(_T15);goto _TL21;_TL20: _TL21: _T16=result;
return _T16;}
# 143
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){struct _RegionHandle*_T0;void*(*_T1)(void*,void*);struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;_T0=Cyc_Core_heap_region;_T1=f;_T2=x;_T3=y;_T4=
Cyc_List_rmap2(_T0,_T1,_T2,_T3);return _T4;}
# 148
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*r,void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){struct Cyc_List_List_mismatch_exn_struct*_T0;struct Cyc_List_List_mismatch_exn_struct*_T1;struct Cyc_List_List*_T2;struct _RegionHandle*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct _RegionHandle*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_List_List*_T11;void*_T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List_mismatch_exn_struct*_T1A;struct Cyc_List_List_mismatch_exn_struct*_T1B;struct Cyc_List_List*_T1C;
# 150
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 152
if(x!=0)goto _TL24;if(y!=0)goto _TL24;if(z!=0)goto _TL24;return 0;_TL24:
 if(x==0)goto _TL28;else{goto _TL2A;}_TL2A: if(y==0)goto _TL28;else{goto _TL29;}_TL29: if(z==0)goto _TL28;else{goto _TL26;}_TL28: _T0=& Cyc_List_List_mismatch_val;_T1=(struct Cyc_List_List_mismatch_exn_struct*)_T0;_throw(_T1);goto _TL27;_TL26: _TL27: _T3=r;{struct Cyc_List_List*_T1D=_region_malloc(_T3,0U,sizeof(struct Cyc_List_List));_T4=x;_T5=_T4->hd;_T6=y;_T7=_T6->hd;_T8=z;_T9=_T8->hd;
# 155
_T1D->hd=f(_T5,_T7,_T9);_T1D->tl=0;_T2=(struct Cyc_List_List*)_T1D;}result=_T2;
prev=result;_TA=x;
# 158
x=_TA->tl;_TB=y;
y=_TB->tl;_TC=z;
z=_TC->tl;
# 162
_TL2B: if(x!=0)goto _TL2F;else{goto _TL2D;}_TL2F: if(y!=0)goto _TL2E;else{goto _TL2D;}_TL2E: if(z!=0)goto _TL2C;else{goto _TL2D;}
_TL2C:{struct Cyc_List_List*temp;_TD=r;temp=_region_malloc(_TD,0U,sizeof(struct Cyc_List_List));_TE=temp;_TF=x;_T10=_TF->hd;_T11=y;_T12=_T11->hd;_T13=z;_T14=_T13->hd;_TE->hd=f(_T10,_T12,_T14);_T15=temp;_T15->tl=0;_T16=prev;
_T16->tl=temp;
prev=temp;_T17=x;
x=_T17->tl;_T18=y;
y=_T18->tl;_T19=z;
z=_T19->tl;}goto _TL2B;_TL2D:
# 170
 if(x!=0)goto _TL32;else{goto _TL34;}_TL34: if(y!=0)goto _TL32;else{goto _TL33;}_TL33: if(z!=0)goto _TL32;else{goto _TL30;}_TL32: _T1A=& Cyc_List_List_mismatch_val;_T1B=(struct Cyc_List_List_mismatch_exn_struct*)_T1A;_throw(_T1B);goto _TL31;_TL30: _TL31: _T1C=result;
return _T1C;}
# 174
struct Cyc_List_List*Cyc_List_map3(void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){struct _RegionHandle*_T0;void*(*_T1)(void*,void*,void*);struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;_T0=Cyc_Core_heap_region;_T1=f;_T2=x;_T3=y;_T4=z;_T5=
Cyc_List_rmap3(_T0,_T1,_T2,_T3,_T4);return _T5;}
# 181
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;
_TL38: if(x!=0)goto _TL36;else{goto _TL37;}
_TL36: _T0=x;_T1=_T0->hd;f(_T1);_T2=x;
# 182
x=_T2->tl;goto _TL38;_TL37:;}
# 186
void Cyc_List_app_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){void*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;
_TL3C: if(x!=0)goto _TL3A;else{goto _TL3B;}
_TL3A: _T0=env;_T1=x;_T2=_T1->hd;f(_T0,_T2);_T3=x;
# 187
x=_T3->tl;goto _TL3C;_TL3B:;}
# 194
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;
_TL40: if(x!=0)goto _TL3E;else{goto _TL3F;}
_TL3E: _T0=x;_T1=_T0->hd;f(_T1);_T2=x;
# 195
x=_T2->tl;goto _TL40;_TL3F:;}
# 199
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x){void*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;
_TL44: if(x!=0)goto _TL42;else{goto _TL43;}
_TL42: _T0=env;_T1=x;_T2=_T1->hd;f(_T0,_T2);_T3=x;
# 200
x=_T3->tl;goto _TL44;_TL43:;}
# 206
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List_mismatch_exn_struct*_T6;struct Cyc_List_List_mismatch_exn_struct*_T7;
_TL48: if(x!=0)goto _TL49;else{goto _TL47;}_TL49: if(y!=0)goto _TL46;else{goto _TL47;}
_TL46: _T0=x;_T1=_T0->hd;_T2=y;_T3=_T2->hd;f(_T1,_T3);_T4=x;
# 207
x=_T4->tl;_T5=y;y=_T5->tl;goto _TL48;_TL47:
# 209
 if(x!=0)goto _TL4C;else{goto _TL4D;}_TL4D: if(y!=0)goto _TL4C;else{goto _TL4A;}_TL4C: _T6=& Cyc_List_List_mismatch_val;_T7=(struct Cyc_List_List_mismatch_exn_struct*)_T6;_throw(_T7);goto _TL4B;_TL4A: _TL4B:;}
# 212
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){void*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List_mismatch_exn_struct*_T7;struct Cyc_List_List_mismatch_exn_struct*_T8;
_TL51: if(x!=0)goto _TL52;else{goto _TL50;}_TL52: if(y!=0)goto _TL4F;else{goto _TL50;}
_TL4F: _T0=env;_T1=x;_T2=_T1->hd;_T3=y;_T4=_T3->hd;f(_T0,_T2,_T4);_T5=x;
# 213
x=_T5->tl;_T6=y;y=_T6->tl;goto _TL51;_TL50:
# 215
 if(x!=0)goto _TL55;else{goto _TL56;}_TL56: if(y!=0)goto _TL55;else{goto _TL53;}_TL55: _T7=& Cyc_List_List_mismatch_val;_T8=(struct Cyc_List_List_mismatch_exn_struct*)_T7;_throw(_T8);goto _TL54;_TL53: _TL54:;}
# 220
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List_mismatch_exn_struct*_T6;struct Cyc_List_List_mismatch_exn_struct*_T7;
_TL5A: if(x!=0)goto _TL5B;else{goto _TL59;}_TL5B: if(y!=0)goto _TL58;else{goto _TL59;}
_TL58: _T0=x;_T1=_T0->hd;_T2=y;_T3=_T2->hd;f(_T1,_T3);_T4=x;
# 221
x=_T4->tl;_T5=y;y=_T5->tl;goto _TL5A;_TL59:
# 223
 if(x!=0)goto _TL5E;else{goto _TL5F;}_TL5F: if(y!=0)goto _TL5E;else{goto _TL5C;}_TL5E: _T6=& Cyc_List_List_mismatch_val;_T7=(struct Cyc_List_List_mismatch_exn_struct*)_T6;_throw(_T7);goto _TL5D;_TL5C: _TL5D:;}
# 225
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){void*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List_mismatch_exn_struct*_T7;struct Cyc_List_List_mismatch_exn_struct*_T8;
_TL63: if(x!=0)goto _TL64;else{goto _TL62;}_TL64: if(y!=0)goto _TL61;else{goto _TL62;}
_TL61: _T0=env;_T1=x;_T2=_T1->hd;_T3=y;_T4=_T3->hd;f(_T0,_T2,_T4);_T5=x;
# 226
x=_T5->tl;_T6=y;y=_T6->tl;goto _TL63;_TL62:
# 228
 if(x!=0)goto _TL67;else{goto _TL68;}_TL68: if(y!=0)goto _TL67;else{goto _TL65;}_TL67: _T7=& Cyc_List_List_mismatch_val;_T8=(struct Cyc_List_List_mismatch_exn_struct*)_T7;_throw(_T8);goto _TL66;_TL65: _TL66:;}
# 234
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x){void*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;
_TL6C: if(x!=0)goto _TL6A;else{goto _TL6B;}
_TL6A: _T0=accum;_T1=x;_T2=_T1->hd;accum=f(_T0,_T2);_T3=x;
# 235
x=_T3->tl;goto _TL6C;_TL6B: _T4=accum;
# 237
return _T4;}
# 240
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*accum,struct Cyc_List_List*x){void*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;void*_T5;
_TL70: if(x!=0)goto _TL6E;else{goto _TL6F;}
_TL6E: _T0=env;_T1=accum;_T2=x;_T3=_T2->hd;accum=f(_T0,_T1,_T3);_T4=x;
# 241
x=_T4->tl;goto _TL70;_TL6F: _T5=accum;
# 243
return _T5;}
# 249
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum){void*_T0;struct Cyc_List_List*_T1;void*_T2;void*(*_T3)(void*,void*);struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;void*_T6;void*_T7;void*_T8;
if(x!=0)goto _TL71;_T0=accum;return _T0;_TL71: _T1=x;_T2=_T1->hd;_T3=f;_T4=x;_T5=_T4->tl;_T6=accum;_T7=
Cyc_List_fold_right(_T3,_T5,_T6);_T8=f(_T2,_T7);return _T8;}
# 253
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,void*accum){void*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;void*(*_T4)(void*,void*,void*);void*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;void*_T8;void*_T9;void*_TA;
if(x!=0)goto _TL73;_T0=accum;return _T0;_TL73: _T1=env;_T2=x;_T3=_T2->hd;_T4=f;_T5=env;_T6=x;_T7=_T6->tl;_T8=accum;_T9=
Cyc_List_fold_right_c(_T4,_T5,_T7,_T8);_TA=f(_T1,_T3,_T9);return _TA;}
# 260
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;
# 262
_TL78: if(x!=0)goto _TL76;else{goto _TL77;}
_TL76: _T1=r2;{struct Cyc_List_List*_T5=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));_T2=x;_T5->hd=_T2->hd;_T5->tl=y;_T0=(struct Cyc_List_List*)_T5;}y=_T0;_T3=x;
# 262
x=_T3->tl;goto _TL78;_TL77: _T4=y;
# 264
return _T4;}
# 267
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;_T0=
Cyc_List_rrevappend(Cyc_Core_heap_region,x,y);return _T0;}
# 272
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*x){struct Cyc_List_List*_T0;
if(x!=0)goto _TL79;_T0=0;goto _TL7A;_TL79: _T0=Cyc_List_rrevappend(r2,x,0);_TL7A: return _T0;}
# 276
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x){struct Cyc_List_List*_T0;_T0=
Cyc_List_rrev(Cyc_Core_heap_region,x);return _T0;}
# 281
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;
if(x!=0)goto _TL7B;_T0=x;return _T0;_TL7B: {
struct Cyc_List_List*first=x;_T1=x;{
struct Cyc_List_List*second=_T1->tl;_T2=x;
_T2->tl=0;
_TL7D: if(second!=0)goto _TL7E;else{goto _TL7F;}
_TL7E: _T3=second;{struct Cyc_List_List*temp=_T3->tl;_T4=second;
_T4->tl=first;
first=second;
second=temp;}goto _TL7D;_TL7F: _T5=first;
# 292
return _T5;}}}
# 296
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct _RegionHandle*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct _RegionHandle*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 299
if(x!=0)goto _TL80;_T0=y;return _T0;_TL80:
 if(y!=0)goto _TL82;_T1=Cyc_List_rcopy(r2,x);return _T1;_TL82: _T3=r2;{struct Cyc_List_List*_TE=_region_malloc(_T3,0U,sizeof(struct Cyc_List_List));_T4=x;
_TE->hd=_T4->hd;_TE->tl=0;_T2=(struct Cyc_List_List*)_TE;}result=_T2;
prev=result;_T5=x;
x=_T5->tl;_TL87: if(x!=0)goto _TL85;else{goto _TL86;}
_TL85:{struct Cyc_List_List*temp;_T6=r2;temp=_region_malloc(_T6,0U,sizeof(struct Cyc_List_List));_T7=temp;_T8=x;_T7->hd=_T8->hd;_T9=temp;_T9->tl=0;_TA=prev;
_TA->tl=temp;
prev=temp;}_TB=x;
# 303
x=_TB->tl;goto _TL87;_TL86: _TC=prev;
# 308
_TC->tl=y;_TD=result;
return _TD;}
# 312
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;_T0=
Cyc_List_rappend(Cyc_Core_heap_region,x,y);return _T0;}
# 318
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;
struct Cyc_List_List*z;
# 321
if(x!=0)goto _TL88;_T0=y;return _T0;_TL88:
 if(y!=0)goto _TL8A;_T1=x;return _T1;_TL8A:
 z=x;_TL8F: if(1)goto _TL90;else{goto _TL8E;}_TL90: _T2=z;_T3=_T2->tl;if(_T3!=0)goto _TL8D;else{goto _TL8E;}
_TL8D: _T4=z;
# 323
z=_T4->tl;goto _TL8F;_TL8E: _T5=z;
# 325
_T5->tl=y;_T6=x;
return _T6;}
# 330
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*r,struct Cyc_List_List*x){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _RegionHandle*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;{
struct Cyc_List_List*(*_T4)(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_right_c;_T0=_T4;}_T1=r;_T2=x;_T3=_T0(Cyc_List_rappend,_T1,_T2,0);return _T3;}
# 334
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x){struct Cyc_List_List*_T0;_T0=
Cyc_List_rflatten(Cyc_Core_heap_region,x);return _T0;}
# 339
struct Cyc_List_List*Cyc_List_imp_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;void*_T5;int _T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_List_List*_TB;void*_TC;int _TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 342
if(a!=0)goto _TL91;_T0=b;return _T0;_TL91:
 if(b!=0)goto _TL93;_T1=a;return _T1;_TL93: _T2=a;_T3=_T2->hd;_T4=b;_T5=_T4->hd;_T6=
# 349
less_eq(_T3,_T5);if(_T6 > 0)goto _TL95;
c=a;_T7=a;
a=_T7->tl;goto _TL96;
# 353
_TL95: c=b;_T8=b;
b=_T8->tl;_TL96:
# 356
 d=c;
# 358
_TL97: if(a!=0)goto _TL9A;else{goto _TL99;}_TL9A: if(b!=0)goto _TL98;else{goto _TL99;}
# 360
_TL98: _T9=a;_TA=_T9->hd;_TB=b;_TC=_TB->hd;_TD=less_eq(_TA,_TC);if(_TD > 0)goto _TL9B;_TE=c;
_TE->tl=a;
c=a;_TF=a;
a=_TF->tl;goto _TL9C;
# 365
_TL9B: _T10=c;_T10->tl=b;
c=b;_T11=b;
b=_T11->tl;_TL9C: goto _TL97;_TL99: _T12=c;
# 371
if(a!=0)goto _TL9D;_T13=b;goto _TL9E;_TL9D: _T13=a;_TL9E: _T12->tl=_T13;_T14=d;
return _T14;}
# 376
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;int(*_TA)(void*,void*);int(*_TB)(void*,void*);struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;int(*_TE)(void*,void*);struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;
if(x!=0)goto _TL9F;_T0=x;return _T0;_TL9F: _T1=x;{
struct Cyc_List_List*temp=_T1->tl;
if(temp!=0)goto _TLA1;_T2=x;return _T2;_TLA1: {
# 382
struct Cyc_List_List*a=x;struct Cyc_List_List*aptr=a;
struct Cyc_List_List*b=temp;struct Cyc_List_List*bptr=b;_T3=b;
x=_T3->tl;
_TLA3: if(x!=0)goto _TLA4;else{goto _TLA5;}
_TLA4: _T4=aptr;_T4->tl=x;
aptr=x;_T5=x;
x=_T5->tl;
if(x==0)goto _TLA6;_T6=bptr;
_T6->tl=x;
bptr=x;_T7=x;
x=_T7->tl;goto _TLA7;_TLA6: _TLA7: goto _TLA3;_TLA5: _T8=aptr;
# 395
_T8->tl=0;_T9=bptr;
_T9->tl=0;_TA=less_eq;_TB=less_eq;_TC=a;_TD=
# 398
Cyc_List_rimp_merge_sort(_TB,_TC);_TE=less_eq;_TF=b;_T10=Cyc_List_rimp_merge_sort(_TE,_TF);_T11=
# 397
Cyc_List_imp_merge(_TA,_TD,_T10);return _T11;}}}
# 405
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r,int(*less_eq)(void*,void*),struct Cyc_List_List*x){int(*_T0)(void*,void*);struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;_T0=less_eq;_T1=
Cyc_List_rcopy(r,x);_T2=Cyc_List_rimp_merge_sort(_T0,_T1);return _T2;}
# 410
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*r,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;void*_T5;int _T6;struct Cyc_List_List*_T7;struct _RegionHandle*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct _RegionHandle*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_List_List*_T11;void*_T12;int _T13;struct Cyc_List_List*_T14;struct _RegionHandle*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;struct _RegionHandle*_T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;
# 412
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 415
if(a!=0)goto _TLA8;_T0=Cyc_List_rcopy(r,b);return _T0;_TLA8:
 if(b!=0)goto _TLAA;_T1=Cyc_List_rcopy(r,a);return _T1;_TLAA: _T2=a;_T3=_T2->hd;_T4=b;_T5=_T4->hd;_T6=
# 422
less_eq(_T3,_T5);if(_T6 > 0)goto _TLAC;_T8=r;{struct Cyc_List_List*_T21=_region_malloc(_T8,0U,sizeof(struct Cyc_List_List));_T9=a;
_T21->hd=_T9->hd;_T21->tl=0;_T7=(struct Cyc_List_List*)_T21;}c=_T7;_TA=a;
a=_TA->tl;goto _TLAD;
# 426
_TLAC: _TC=r;{struct Cyc_List_List*_T21=_region_malloc(_TC,0U,sizeof(struct Cyc_List_List));_TD=b;_T21->hd=_TD->hd;_T21->tl=0;_TB=(struct Cyc_List_List*)_T21;}c=_TB;_TE=b;
b=_TE->tl;_TLAD:
# 429
 d=c;
# 431
_TLAE: if(a!=0)goto _TLB1;else{goto _TLB0;}_TLB1: if(b!=0)goto _TLAF;else{goto _TLB0;}
# 433
_TLAF:{struct Cyc_List_List*temp;_TF=a;_T10=_TF->hd;_T11=b;_T12=_T11->hd;_T13=
less_eq(_T10,_T12);if(_T13 > 0)goto _TLB2;_T15=r;{struct Cyc_List_List*_T21=_region_malloc(_T15,0U,sizeof(struct Cyc_List_List));_T16=a;
_T21->hd=_T16->hd;_T21->tl=0;_T14=(struct Cyc_List_List*)_T21;}temp=_T14;_T17=c;
_T17->tl=temp;
c=temp;_T18=a;
a=_T18->tl;goto _TLB3;
# 440
_TLB2: _T1A=r;{struct Cyc_List_List*_T21=_region_malloc(_T1A,0U,sizeof(struct Cyc_List_List));_T1B=b;_T21->hd=_T1B->hd;_T21->tl=0;_T19=(struct Cyc_List_List*)_T21;}temp=_T19;_T1C=c;
_T1C->tl=temp;
c=temp;_T1D=b;
b=_T1D->tl;_TLB3:;}goto _TLAE;_TLB0: _T1E=c;
# 447
if(a!=0)goto _TLB4;_T1F=Cyc_List_rcopy(r,b);goto _TLB5;_TLB4: _T1F=Cyc_List_rcopy(r,a);_TLB5: _T1E->tl=_T1F;_T20=d;
return _T20;}
# 451
struct Cyc_List_List*Cyc_List_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){struct _RegionHandle*_T0;int(*_T1)(void*,void*);struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;_T0=Cyc_Core_heap_region;_T1=less_eq;_T2=x;_T3=
Cyc_List_rmerge_sort(_T0,_T1,_T2);return _T3;}
# 455
struct Cyc_List_List*Cyc_List_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){struct _RegionHandle*_T0;int(*_T1)(void*,void*);struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;_T0=Cyc_Core_heap_region;_T1=less_eq;_T2=a;_T3=b;_T4=
Cyc_List_rmerge(_T0,_T1,_T2,_T3);return _T4;}char Cyc_List_Nth[4U]="Nth";
# 461
struct Cyc_List_Nth_exn_struct Cyc_List_Nth_val={Cyc_List_Nth};
# 466
void*Cyc_List_nth(struct Cyc_List_List*x,int i){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;void*_T2;_T0=
Cyc_List_nth_tail(x,i);_T1=_check_null(_T0);_T2=_T1->hd;return _T2;}
# 472
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i){struct Cyc_List_Nth_exn_struct*_T0;struct Cyc_List_Nth_exn_struct*_T1;struct Cyc_List_Nth_exn_struct*_T2;struct Cyc_List_Nth_exn_struct*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;
if(i >= 0)goto _TLB6;_T0=& Cyc_List_Nth_val;_T1=(struct Cyc_List_Nth_exn_struct*)_T0;_throw(_T1);goto _TLB7;_TLB6: _TLB7:
 _TLBB: if(i > 0)goto _TLB9;else{goto _TLBA;}
_TLB9: if(x!=0)goto _TLBC;_T2=& Cyc_List_Nth_val;_T3=(struct Cyc_List_Nth_exn_struct*)_T2;_throw(_T3);goto _TLBD;_TLBC: _TLBD:
# 474
 i=i + -1;_T4=x;x=_T4->tl;goto _TLBB;_TLBA: _T5=x;
# 477
return _T5;}
# 482
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;int _T2;struct Cyc_List_List*_T3;
_TLC1: if(x!=0)goto _TLBF;else{goto _TLC0;}
_TLBF: _T0=x;_T1=_T0->hd;_T2=pred(_T1);if(_T2)goto _TLC2;else{goto _TLC4;}
_TLC4: return 0;_TLC2: _T3=x;
# 483
x=_T3->tl;goto _TLC1;_TLC0:
# 486
 return 1;}
# 488
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){void*_T0;struct Cyc_List_List*_T1;void*_T2;int _T3;struct Cyc_List_List*_T4;
_TLC8: if(x!=0)goto _TLC6;else{goto _TLC7;}
_TLC6: _T0=env;_T1=x;_T2=_T1->hd;_T3=pred(_T0,_T2);if(_T3)goto _TLC9;else{goto _TLCB;}
_TLCB: return 0;_TLC9: _T4=x;
# 489
x=_T4->tl;goto _TLC8;_TLC7:
# 492
 return 1;}
# 497
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;int _T2;struct Cyc_List_List*_T3;
_TLCF: if(x!=0)goto _TLCD;else{goto _TLCE;}
_TLCD: _T0=x;_T1=_T0->hd;_T2=pred(_T1);if(!_T2)goto _TLD0;
return 1;_TLD0: _T3=x;
# 498
x=_T3->tl;goto _TLCF;_TLCE:
# 501
 return 0;}
# 503
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){void*_T0;struct Cyc_List_List*_T1;void*_T2;int _T3;struct Cyc_List_List*_T4;
_TLD5: if(x!=0)goto _TLD3;else{goto _TLD4;}
_TLD3: _T0=env;_T1=x;_T2=_T1->hd;_T3=pred(_T0,_T2);if(!_T3)goto _TLD6;
return 1;_TLD6: _T4=x;
# 504
x=_T4->tl;goto _TLD5;_TLD4:
# 507
 return 0;}
# 510
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x){void*_T0;struct Cyc_List_List*_T1;void*_T2;void*_T3;unsigned _T4;void*_T5;struct Cyc_List_List*_T6;
_TLDB: if(x!=0)goto _TLD9;else{goto _TLDA;}
_TLD9: _T0=env;_T1=x;_T2=_T1->hd;{void*v=pred(_T0,_T2);_T3=v;_T4=(unsigned)_T3;
if(!_T4)goto _TLDC;_T5=v;return _T5;_TLDC:;}_T6=x;
# 511
x=_T6->tl;goto _TLDB;_TLDA:
# 515
 return 0;}struct _tuple2{void*f0;void*f1;};
# 520
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List_mismatch_exn_struct*_T0;struct Cyc_List_List_mismatch_exn_struct*_T1;struct Cyc_List_List*_T2;struct _RegionHandle*_T3;struct _tuple2*_T4;struct _RegionHandle*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct _RegionHandle*_TB;struct _tuple2*_TC;struct _RegionHandle*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List_mismatch_exn_struct*_T13;struct Cyc_List_List_mismatch_exn_struct*_T14;struct Cyc_List_List*_T15;
# 522
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 524
if(x!=0)goto _TLDE;if(y!=0)goto _TLDE;return 0;_TLDE:
 if(x==0)goto _TLE2;else{goto _TLE3;}_TLE3: if(y==0)goto _TLE2;else{goto _TLE0;}_TLE2: _T0=& Cyc_List_List_mismatch_val;_T1=(struct Cyc_List_List_mismatch_exn_struct*)_T0;_throw(_T1);goto _TLE1;_TLE0: _TLE1: _T3=r3;{struct Cyc_List_List*_T16=_region_malloc(_T3,0U,sizeof(struct Cyc_List_List));_T5=r4;{struct _tuple2*_T17=_region_malloc(_T5,0U,sizeof(struct _tuple2));_T6=x;
# 527
_T17->f0=_T6->hd;_T7=y;_T17->f1=_T7->hd;_T4=(struct _tuple2*)_T17;}_T16->hd=_T4;_T16->tl=0;_T2=(struct Cyc_List_List*)_T16;}result=_T2;
prev=result;_T8=x;
# 530
x=_T8->tl;_T9=y;
y=_T9->tl;
# 533
_TLE4: if(x!=0)goto _TLE7;else{goto _TLE6;}_TLE7: if(y!=0)goto _TLE5;else{goto _TLE6;}
_TLE5: _TB=r3;{struct Cyc_List_List*_T16=_region_malloc(_TB,0U,sizeof(struct Cyc_List_List));_TD=r4;{struct _tuple2*_T17=_region_malloc(_TD,0U,sizeof(struct _tuple2));_TE=x;_T17->f0=_TE->hd;_TF=y;_T17->f1=_TF->hd;_TC=(struct _tuple2*)_T17;}_T16->hd=_TC;_T16->tl=0;_TA=(struct Cyc_List_List*)_T16;}temp=_TA;_T10=prev;
_T10->tl=temp;
prev=temp;_T11=x;
x=_T11->tl;_T12=y;
y=_T12->tl;goto _TLE4;_TLE6:
# 540
 if(x!=0)goto _TLEA;else{goto _TLEB;}_TLEB: if(y!=0)goto _TLEA;else{goto _TLE8;}_TLEA: _T13=& Cyc_List_List_mismatch_val;_T14=(struct Cyc_List_List_mismatch_exn_struct*)_T13;_throw(_T14);goto _TLE9;_TLE8: _TLE9: _T15=result;
return _T15;}
# 544
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*_T0;_T0=
Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);return _T0;}struct _tuple3{void*f0;void*f1;void*f2;};
# 548
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){struct Cyc_List_List_mismatch_exn_struct*_T0;struct Cyc_List_List_mismatch_exn_struct*_T1;struct Cyc_List_List*_T2;struct _RegionHandle*_T3;struct _tuple3*_T4;struct _RegionHandle*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct _RegionHandle*_TD;struct _tuple3*_TE;struct _RegionHandle*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List_mismatch_exn_struct*_T17;struct Cyc_List_List_mismatch_exn_struct*_T18;struct Cyc_List_List*_T19;
# 550
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 552
if(x!=0)goto _TLEC;if(y!=0)goto _TLEC;if(z!=0)goto _TLEC;return 0;_TLEC:
 if(x==0)goto _TLF0;else{goto _TLF2;}_TLF2: if(y==0)goto _TLF0;else{goto _TLF1;}_TLF1: if(z==0)goto _TLF0;else{goto _TLEE;}_TLF0: _T0=& Cyc_List_List_mismatch_val;_T1=(struct Cyc_List_List_mismatch_exn_struct*)_T0;_throw(_T1);goto _TLEF;_TLEE: _TLEF: _T3=r3;{struct Cyc_List_List*_T1A=_region_malloc(_T3,0U,sizeof(struct Cyc_List_List));_T5=r4;{struct _tuple3*_T1B=_region_malloc(_T5,0U,sizeof(struct _tuple3));_T6=x;
# 555
_T1B->f0=_T6->hd;_T7=y;_T1B->f1=_T7->hd;_T8=z;_T1B->f2=_T8->hd;_T4=(struct _tuple3*)_T1B;}_T1A->hd=_T4;_T1A->tl=0;_T2=(struct Cyc_List_List*)_T1A;}result=_T2;
prev=result;_T9=x;
# 558
x=_T9->tl;_TA=y;
y=_TA->tl;_TB=z;
z=_TB->tl;
# 562
_TLF3: if(x!=0)goto _TLF7;else{goto _TLF5;}_TLF7: if(y!=0)goto _TLF6;else{goto _TLF5;}_TLF6: if(z!=0)goto _TLF4;else{goto _TLF5;}
_TLF4: _TD=r3;{struct Cyc_List_List*_T1A=_region_malloc(_TD,0U,sizeof(struct Cyc_List_List));_TF=r4;{struct _tuple3*_T1B=_region_malloc(_TF,0U,sizeof(struct _tuple3));_T10=x;_T1B->f0=_T10->hd;_T11=y;_T1B->f1=_T11->hd;_T12=z;_T1B->f2=_T12->hd;_TE=(struct _tuple3*)_T1B;}_T1A->hd=_TE;_T1A->tl=0;_TC=(struct Cyc_List_List*)_T1A;}temp=_TC;_T13=prev;
_T13->tl=temp;
prev=temp;_T14=x;
x=_T14->tl;_T15=y;
y=_T15->tl;_T16=z;
z=_T16->tl;goto _TLF3;_TLF5:
# 570
 if(x!=0)goto _TLFA;else{goto _TLFC;}_TLFC: if(y!=0)goto _TLFA;else{goto _TLFB;}_TLFB: if(z!=0)goto _TLFA;else{goto _TLF8;}_TLFA: _T17=& Cyc_List_List_mismatch_val;_T18=(struct Cyc_List_List_mismatch_exn_struct*)_T17;_throw(_T18);goto _TLF9;_TLF8: _TLF9: _T19=result;
return _T19;}
# 574
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){struct Cyc_List_List*_T0;_T0=
Cyc_List_rzip3(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y,z);return _T0;}
# 580
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x){struct _tuple0 _T0;struct Cyc_List_List*_T1;struct _RegionHandle*_T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple2*_T5;struct _tuple2 _T6;struct Cyc_List_List*_T7;struct _RegionHandle*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple2*_TB;struct _tuple2 _TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct _RegionHandle*_TF;struct Cyc_List_List*_T10;void*_T11;struct _tuple2*_T12;struct _tuple2 _T13;struct Cyc_List_List*_T14;struct _RegionHandle*_T15;struct Cyc_List_List*_T16;void*_T17;struct _tuple2*_T18;struct _tuple2 _T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct _tuple0 _T1D;
# 582
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
# 585
if(x!=0)goto _TLFD;{struct _tuple0 _T1E;_T1E.f0=0;_T1E.f1=0;_T0=_T1E;}return _T0;_TLFD: _T2=r3;{struct Cyc_List_List*_T1E=_region_malloc(_T2,0U,sizeof(struct Cyc_List_List));_T3=x;_T4=_T3->hd;_T5=(struct _tuple2*)_T4;_T6=_T5[0];
# 587
_T1E->hd=_T6.f0;_T1E->tl=0;_T1=(struct Cyc_List_List*)_T1E;}result1=_T1;prev1=result1;_T8=r4;{struct Cyc_List_List*_T1E=_region_malloc(_T8,0U,sizeof(struct Cyc_List_List));_T9=x;_TA=_T9->hd;_TB=(struct _tuple2*)_TA;_TC=_TB[0];
_T1E->hd=_TC.f1;_T1E->tl=0;_T7=(struct Cyc_List_List*)_T1E;}result2=_T7;prev2=result2;_TD=x;
# 590
x=_TD->tl;_TL102: if(x!=0)goto _TL100;else{goto _TL101;}
_TL100: _TF=r3;{struct Cyc_List_List*_T1E=_region_malloc(_TF,0U,sizeof(struct Cyc_List_List));_T10=x;_T11=_T10->hd;_T12=(struct _tuple2*)_T11;_T13=_T12[0];_T1E->hd=_T13.f0;_T1E->tl=0;_TE=(struct Cyc_List_List*)_T1E;}temp1=_TE;_T15=r4;{struct Cyc_List_List*_T1E=_region_malloc(_T15,0U,sizeof(struct Cyc_List_List));_T16=x;_T17=_T16->hd;_T18=(struct _tuple2*)_T17;_T19=_T18[0];
_T1E->hd=_T19.f1;_T1E->tl=0;_T14=(struct Cyc_List_List*)_T1E;}temp2=_T14;_T1A=prev1;
_T1A->tl=temp1;_T1B=prev2;
_T1B->tl=temp2;
prev1=temp1;
prev2=temp2;_T1C=x;
# 590
x=_T1C->tl;goto _TL102;_TL101:{struct _tuple0 _T1E;
# 598
_T1E.f0=result1;_T1E.f1=result2;_T1D=_T1E;}return _T1D;}
# 601
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x){struct _tuple0 _T0;_T0=
Cyc_List_rsplit(Cyc_Core_heap_region,Cyc_Core_heap_region,x);return _T0;}
# 607
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x){struct _tuple1 _T0;struct Cyc_List_List*_T1;struct _RegionHandle*_T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple3*_T5;struct _tuple3 _T6;struct Cyc_List_List*_T7;struct _RegionHandle*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple3*_TB;struct _tuple3 _TC;struct Cyc_List_List*_TD;struct _RegionHandle*_TE;struct Cyc_List_List*_TF;void*_T10;struct _tuple3*_T11;struct _tuple3 _T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct _RegionHandle*_T15;struct Cyc_List_List*_T16;void*_T17;struct _tuple3*_T18;struct _tuple3 _T19;struct Cyc_List_List*_T1A;struct _RegionHandle*_T1B;struct Cyc_List_List*_T1C;void*_T1D;struct _tuple3*_T1E;struct _tuple3 _T1F;struct Cyc_List_List*_T20;struct _RegionHandle*_T21;struct Cyc_List_List*_T22;void*_T23;struct _tuple3*_T24;struct _tuple3 _T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct _tuple1 _T2A;
# 610
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
struct Cyc_List_List*result3;struct Cyc_List_List*prev3;struct Cyc_List_List*temp3;
# 614
if(x!=0)goto _TL103;{struct _tuple1 _T2B;_T2B.f0=0;_T2B.f1=0;_T2B.f2=0;_T0=_T2B;}return _T0;_TL103: _T2=r3;{struct Cyc_List_List*_T2B=_region_malloc(_T2,0U,sizeof(struct Cyc_List_List));_T3=x;_T4=_T3->hd;_T5=(struct _tuple3*)_T4;_T6=_T5[0];
# 616
_T2B->hd=_T6.f0;_T2B->tl=0;_T1=(struct Cyc_List_List*)_T2B;}result1=_T1;prev1=result1;_T8=r4;{struct Cyc_List_List*_T2B=_region_malloc(_T8,0U,sizeof(struct Cyc_List_List));_T9=x;_TA=_T9->hd;_TB=(struct _tuple3*)_TA;_TC=_TB[0];
_T2B->hd=_TC.f1;_T2B->tl=0;_T7=(struct Cyc_List_List*)_T2B;}result2=_T7;prev2=result2;_TE=r5;{struct Cyc_List_List*_T2B=_region_malloc(_TE,0U,sizeof(struct Cyc_List_List));_TF=x;_T10=_TF->hd;_T11=(struct _tuple3*)_T10;_T12=_T11[0];
_T2B->hd=_T12.f2;_T2B->tl=0;_TD=(struct Cyc_List_List*)_T2B;}result3=_TD;prev3=result3;_T13=x;
# 620
x=_T13->tl;_TL108: if(x!=0)goto _TL106;else{goto _TL107;}
_TL106: _T15=r3;{struct Cyc_List_List*_T2B=_region_malloc(_T15,0U,sizeof(struct Cyc_List_List));_T16=x;_T17=_T16->hd;_T18=(struct _tuple3*)_T17;_T19=_T18[0];_T2B->hd=_T19.f0;_T2B->tl=0;_T14=(struct Cyc_List_List*)_T2B;}temp1=_T14;_T1B=r4;{struct Cyc_List_List*_T2B=_region_malloc(_T1B,0U,sizeof(struct Cyc_List_List));_T1C=x;_T1D=_T1C->hd;_T1E=(struct _tuple3*)_T1D;_T1F=_T1E[0];
_T2B->hd=_T1F.f1;_T2B->tl=0;_T1A=(struct Cyc_List_List*)_T2B;}temp2=_T1A;_T21=r5;{struct Cyc_List_List*_T2B=_region_malloc(_T21,0U,sizeof(struct Cyc_List_List));_T22=x;_T23=_T22->hd;_T24=(struct _tuple3*)_T23;_T25=_T24[0];
_T2B->hd=_T25.f2;_T2B->tl=0;_T20=(struct Cyc_List_List*)_T2B;}temp3=_T20;_T26=prev1;
_T26->tl=temp1;_T27=prev2;
_T27->tl=temp2;_T28=prev3;
_T28->tl=temp3;
prev1=temp1;
prev2=temp2;
prev3=temp3;_T29=x;
# 620
x=_T29->tl;goto _TL108;_TL107:{struct _tuple1 _T2B;
# 631
_T2B.f0=result1;_T2B.f1=result2;_T2B.f2=result3;_T2A=_T2B;}return _T2A;}
# 634
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){struct _tuple1 _T0;_T0=
Cyc_List_rsplit3(Cyc_Core_heap_region,Cyc_Core_heap_region,Cyc_Core_heap_region,x);return _T0;}
# 640
int Cyc_List_memq(struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;void*_T2;struct Cyc_List_List*_T3;
_TL10C: if(l!=0)goto _TL10A;else{goto _TL10B;}
_TL10A: _T0=l;_T1=_T0->hd;_T2=x;if(_T1!=_T2)goto _TL10D;
return 1;_TL10D: _T3=l;
# 641
l=_T3->tl;goto _TL10C;_TL10B:
# 644
 return 0;}
# 647
int Cyc_List_mem(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;void*_T2;int _T3;struct Cyc_List_List*_T4;
_TL112: if(l!=0)goto _TL110;else{goto _TL111;}
_TL110: _T0=l;_T1=_T0->hd;_T2=x;_T3=cmp(_T1,_T2);if(_T3!=0)goto _TL113;
return 1;_TL113: _T4=l;
# 648
l=_T4->tl;goto _TL112;_TL111:
# 651
 return 0;}
# 657
void*Cyc_List_assoc(struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple2*_T2;struct _tuple2 _T3;void*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;struct _tuple2*_T8;struct _tuple2 _T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_Core_Not_found_exn_struct*_TC;struct Cyc_Core_Not_found_exn_struct*_TD;
_TL118: if(l!=0)goto _TL116;else{goto _TL117;}
_TL116: _T0=l;_T1=_T0->hd;_T2=(struct _tuple2*)_T1;_T3=_T2[0];_T4=_T3.f0;_T5=x;if(_T4!=_T5)goto _TL119;_T6=l;_T7=_T6->hd;_T8=(struct _tuple2*)_T7;_T9=_T8[0];_TA=_T9.f1;
return _TA;_TL119: _TB=l;
# 658
l=_TB->tl;goto _TL118;_TL117: _TC=& Cyc_Core_Not_found_val;_TD=(struct Cyc_Core_Not_found_exn_struct*)_TC;_throw(_TD);}
# 664
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple2*_T2;struct _tuple2 _T3;void*_T4;void*_T5;int _T6;struct Cyc_List_List*_T7;void*_T8;struct _tuple2*_T9;struct _tuple2 _TA;void*_TB;struct Cyc_List_List*_TC;struct Cyc_Core_Not_found_exn_struct*_TD;struct Cyc_Core_Not_found_exn_struct*_TE;
_TL11E: if(l!=0)goto _TL11C;else{goto _TL11D;}
_TL11C: _T0=l;_T1=_T0->hd;_T2=(struct _tuple2*)_T1;_T3=*_T2;_T4=_T3.f0;_T5=x;_T6=cmp(_T4,_T5);if(_T6!=0)goto _TL11F;_T7=l;_T8=_T7->hd;_T9=(struct _tuple2*)_T8;_TA=*_T9;_TB=_TA.f1;
return _TB;_TL11F: _TC=l;
# 665
l=_TC->tl;goto _TL11E;_TL11D: _TD=& Cyc_Core_Not_found_val;_TE=(struct Cyc_Core_Not_found_exn_struct*)_TD;_throw(_TE);}
# 675
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;void*_T2;int _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_Core_Not_found_exn_struct*_TA;struct Cyc_Core_Not_found_exn_struct*_TB;
struct Cyc_List_List*prev=0;
struct Cyc_List_List*iter=l;
_TL121: if(iter!=0)goto _TL122;else{goto _TL123;}
_TL122: _T0=iter;_T1=_T0->hd;_T2=x;_T3=cmp(_T1,_T2);if(_T3!=0)goto _TL124;
if(prev!=0)goto _TL126;_T4=iter;_T5=_T4->tl;
return _T5;_TL126: _T6=prev;_T7=iter;
_T6->tl=_T7->tl;_T8=l;
return _T8;_TL124:
# 685
 prev=iter;_T9=iter;
iter=_T9->tl;goto _TL121;_TL123: _TA=& Cyc_Core_Not_found_val;_TB=(struct Cyc_Core_Not_found_exn_struct*)_TA;_throw(_TB);}
# 691
static int Cyc_List_ptrequal(void*x,void*y){int _T0;int _T1;_T0=x==y;_T1=!_T0;
return _T1;}
# 698
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;_T0=l;_T1=x;_T2=
Cyc_List_delete_cmp(Cyc_List_ptrequal,_T0,_T1);return _T2;}
# 704
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple2*_T2;struct _tuple2 _T3;void*_T4;void*_T5;struct Cyc_List_List*_T6;
_TL12B: if(l!=0)goto _TL129;else{goto _TL12A;}
_TL129: _T0=l;_T1=_T0->hd;_T2=(struct _tuple2*)_T1;_T3=_T2[0];_T4=_T3.f0;_T5=x;if(_T4!=_T5)goto _TL12C;
return 1;_TL12C: _T6=l;
# 705
l=_T6->tl;goto _TL12B;_TL12A:
# 708
 return 0;}
# 713
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple2*_T2;struct _tuple2 _T3;void*_T4;void*_T5;int _T6;struct Cyc_List_List*_T7;
_TL131: if(l!=0)goto _TL12F;else{goto _TL130;}
_TL12F: _T0=l;_T1=_T0->hd;_T2=(struct _tuple2*)_T1;_T3=_T2[0];_T4=_T3.f0;_T5=x;_T6=cmp(_T4,_T5);if(_T6!=0)goto _TL132;
return 1;_TL132: _T7=l;
# 714
l=_T7->tl;goto _TL131;_TL130:
# 717
 return 0;}
# 722
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;int _T5;struct Cyc_Core_Opt*_T6;
_TL134: if(x!=0)goto _TL135;else{goto _TL136;}
_TL135: _T0=x;{void*hd=_T0->hd;_T1=x;
x=_T1->tl;
if(x==0)goto _TL137;_T2=hd;_T3=x;_T4=_T3->hd;_T5=cmp(_T2,_T4);if(_T5!=0)goto _TL137;{struct Cyc_Core_Opt*_T7=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T7->v=hd;_T6=(struct Cyc_Core_Opt*)_T7;}return _T6;_TL137:;}goto _TL134;_TL136:
# 729
 return 0;}
# 733
struct _fat_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x){struct _fat_ptr _T0;int _T1;void**_T2;struct _RegionHandle*_T3;unsigned _T4;unsigned _T5;void*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;
int s=Cyc_List_length(x);_T1=s;{unsigned _T9=(unsigned)_T1;_T3=r2;_T4=_check_times(_T9,sizeof(void*));{void**_TA=_region_malloc(_T3,0U,_T4);{unsigned _TB=_T9;unsigned i;i=0;_TL13C: if(i < _TB)goto _TL13A;else{goto _TL13B;}_TL13A: _T5=i;_T7=
_check_null(x);{void*v=_T7->hd;_T8=x;x=_T8->tl;_T6=v;}_TA[_T5]=_T6;i=i + 1;goto _TL13C;_TL13B:;}_T2=(void**)_TA;}_T0=_tag_fat(_T2,sizeof(void*),_T9);}return _T0;}
# 738
struct _fat_ptr Cyc_List_to_array(struct Cyc_List_List*x){struct _fat_ptr _T0;_T0=
Cyc_List_rto_array(Cyc_Core_heap_region,x);return _T0;}
# 743
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _fat_ptr arr){struct _fat_ptr _T0;struct Cyc_List_List*_T1;struct _RegionHandle*_T2;struct _fat_ptr _T3;unsigned char*_T4;void**_T5;unsigned _T6;int _T7;struct Cyc_List_List*_T8;
struct Cyc_List_List*ans=0;_T0=arr;{
unsigned n=_get_fat_size(_T0,sizeof(void*));{
unsigned i=n - 1U;_TL140: if(i < n)goto _TL13E;else{goto _TL13F;}
_TL13E: _T2=r2;{struct Cyc_List_List*_T9=_region_malloc(_T2,0U,sizeof(struct Cyc_List_List));_T3=arr;_T4=_T3.curr;_T5=(void**)_T4;_T6=i;_T7=(int)_T6;_T9->hd=_T5[_T7];_T9->tl=ans;_T1=(struct Cyc_List_List*)_T9;}ans=_T1;
# 746
i=i + -1;goto _TL140;_TL13F:;}_T8=ans;
# 748
return _T8;}}
# 751
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr arr){struct Cyc_List_List*_T0;_T0=
Cyc_List_rfrom_array(Cyc_Core_heap_region,arr);return _T0;}
# 755
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int)){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;struct Cyc_List_List*_T2;
struct Cyc_List_List*res=0;{
int i=0;_TL144: if(i < n)goto _TL142;else{goto _TL143;}
_TL142: _T1=r;{struct Cyc_List_List*_T3=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));_T3->hd=f(i);_T3->tl=res;_T0=(struct Cyc_List_List*)_T3;}res=_T0;
# 757
i=i + 1;goto _TL144;_TL143:;}_T2=
# 759
Cyc_List_imp_rev(res);return _T2;}
# 762
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int)){struct _RegionHandle*_T0;int _T1;void*(*_T2)(int);struct Cyc_List_List*_T3;_T0=Cyc_Core_heap_region;_T1=n;_T2=f;_T3=
Cyc_List_rtabulate(_T0,_T1,_T2);return _T3;}
# 766
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env){struct Cyc_List_List*_T0;struct _RegionHandle*_T1;struct Cyc_List_List*_T2;
struct Cyc_List_List*res=0;{
int i=0;_TL148: if(i < n)goto _TL146;else{goto _TL147;}
_TL146: _T1=r;{struct Cyc_List_List*_T3=_region_malloc(_T1,0U,sizeof(struct Cyc_List_List));_T3->hd=f(env,i);_T3->tl=res;_T0=(struct Cyc_List_List*)_T3;}res=_T0;
# 768
i=i + 1;goto _TL148;_TL147:;}_T2=
# 770
Cyc_List_imp_rev(res);return _T2;}
# 773
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env){struct _RegionHandle*_T0;int _T1;void*(*_T2)(void*,int);void*_T3;struct Cyc_List_List*_T4;_T0=Cyc_Core_heap_region;_T1=n;_T2=f;_T3=env;_T4=
Cyc_List_rtabulate_c(_T0,_T1,_T2,_T3);return _T4;}
# 777
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;unsigned _T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;int _T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;int _TB;
_TL14C: if(l1!=0)goto _TL14D;else{goto _TL14B;}_TL14D: if(l2!=0)goto _TL14A;else{goto _TL14B;}
# 780
_TL14A: _T0=l1;_T1=(unsigned)_T0;_T2=l2;_T3=(unsigned)_T2;if(_T1!=_T3)goto _TL14E;
return 0;_TL14E: _T4=l1;_T5=_T4->hd;_T6=l2;_T7=_T6->hd;{
int i=cmp(_T5,_T7);
if(i==0)goto _TL150;_T8=i;
return _T8;_TL150:;}_T9=l1;
# 778
l1=_T9->tl;_TA=l2;l2=_TA->tl;goto _TL14C;_TL14B:
# 786
 if(l1==0)goto _TL152;
return 1;_TL152:
 if(l2==0)goto _TL154;_TB=- 1;
return _TB;_TL154:
 return 0;}
# 794
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;unsigned _T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;int _T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;int _TB;
_TL159: if(l1!=0)goto _TL15A;else{goto _TL158;}_TL15A: if(l2!=0)goto _TL157;else{goto _TL158;}
# 797
_TL157: _T0=l1;_T1=(unsigned)_T0;_T2=l2;_T3=(unsigned)_T2;if(_T1!=_T3)goto _TL15B;
return 1;_TL15B: _T4=l1;_T5=_T4->hd;_T6=l2;_T7=_T6->hd;_T8=
cmp(_T5,_T7);if(_T8==0)goto _TL15D;
return 0;_TL15D: _T9=l1;
# 795
l1=_T9->tl;_TA=l2;l2=_TA->tl;goto _TL159;_TL158: _TB=l1==0;
# 802
return _TB;}
# 805
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){struct _RegionHandle*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;void*_T6;int _T7;struct Cyc_List_List*_T8;struct _RegionHandle*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;
if(l!=0)goto _TL15F;
return 0;_TL15F: {
# 809
struct Cyc_List_List*result;_T0=r;result=_region_malloc(_T0,0U,sizeof(struct Cyc_List_List));_T1=result;_T2=l;_T1->hd=_T2->hd;_T3=result;_T3->tl=0;{
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
_TL164: if(l!=0)goto _TL162;else{goto _TL163;}
_TL162: _T4=env;_T5=l;_T6=_T5->hd;_T7=f(_T4,_T6);if(!_T7)goto _TL165;_T9=r;{struct Cyc_List_List*_TF=_region_malloc(_T9,0U,sizeof(struct Cyc_List_List));_TA=l;
_TF->hd=_TA->hd;_TF->tl=0;_T8=(struct Cyc_List_List*)_TF;}temp=_T8;_TB=end;
_TB->tl=temp;
end=temp;goto _TL166;_TL165: _TL166: _TC=l;
# 812
l=_TC->tl;goto _TL164;_TL163: _TD=result;_TE=_TD->tl;
# 818
return _TE;}}}
# 821
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*l){struct _RegionHandle*_T0;int(*_T1)(void*,void*);void*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;_T0=Cyc_Core_heap_region;_T1=f;_T2=env;_T3=l;_T4=
Cyc_List_rfilter_c(_T0,_T1,_T2,_T3);return _T4;}
# 825
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){struct _RegionHandle*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;void*_T5;int _T6;struct Cyc_List_List*_T7;struct _RegionHandle*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;
if(l!=0)goto _TL167;
return 0;_TL167: {
# 829
struct Cyc_List_List*result;_T0=r;result=_region_malloc(_T0,0U,sizeof(struct Cyc_List_List));_T1=result;_T2=l;_T1->hd=_T2->hd;_T3=result;_T3->tl=0;{
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
_TL16C: if(l!=0)goto _TL16A;else{goto _TL16B;}
_TL16A: _T4=l;_T5=_T4->hd;_T6=f(_T5);if(!_T6)goto _TL16D;_T8=r;{struct Cyc_List_List*_TE=_region_malloc(_T8,0U,sizeof(struct Cyc_List_List));_T9=l;
_TE->hd=_T9->hd;_TE->tl=0;_T7=(struct Cyc_List_List*)_TE;}temp=_T7;_TA=end;
_TA->tl=temp;
end=temp;goto _TL16E;_TL16D: _TL16E: _TB=l;
# 832
l=_TB->tl;goto _TL16C;_TL16B: _TC=result;_TD=_TC->tl;
# 838
return _TD;}}}
# 841
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*l){struct _RegionHandle*_T0;int(*_T1)(void*);struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;_T0=Cyc_Core_heap_region;_T1=f;_T2=l;_T3=
Cyc_List_rfilter(_T0,_T1,_T2);return _T3;}

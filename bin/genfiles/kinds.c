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
 struct Cyc_Core_Opt{void*v;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 933 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 962
extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 53 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_ako;
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
# 59
extern struct Cyc_Core_Opt Cyc_Kinds_ptrbko;
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 27 "kinds.cyc"
struct Cyc_Absyn_Kind Cyc_Kinds_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_NoHint};
struct Cyc_Absyn_Kind Cyc_Kinds_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_NoHint};
struct Cyc_Absyn_Kind Cyc_Kinds_mk={Cyc_Absyn_MemKind,Cyc_Absyn_NoHint};
struct Cyc_Absyn_Kind Cyc_Kinds_ik={Cyc_Absyn_IntKind,Cyc_Absyn_NoHint};
struct Cyc_Absyn_Kind Cyc_Kinds_ek={Cyc_Absyn_EffKind,Cyc_Absyn_NoHint};
struct Cyc_Absyn_Kind Cyc_Kinds_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_NoHint};
struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_NoHint};
struct Cyc_Absyn_Kind Cyc_Kinds_aqk={Cyc_Absyn_AqualKind,Cyc_Absyn_NoHint};
# 36
struct Cyc_Absyn_Kind Cyc_Kinds_bk_rhint={Cyc_Absyn_BoxKind,Cyc_Absyn_RestrictedHint};
struct Cyc_Absyn_Kind Cyc_Kinds_bk_uhint={Cyc_Absyn_BoxKind,Cyc_Absyn_UniqueHint};
struct Cyc_Absyn_Kind Cyc_Kinds_bk_rchint={Cyc_Absyn_BoxKind,Cyc_Absyn_RefcntHint};
# 40
struct Cyc_Absyn_Kind Cyc_Kinds_mk_rhint={Cyc_Absyn_MemKind,Cyc_Absyn_RestrictedHint};
struct Cyc_Absyn_Kind Cyc_Kinds_mk_uhint={Cyc_Absyn_MemKind,Cyc_Absyn_UniqueHint};
struct Cyc_Absyn_Kind Cyc_Kinds_mk_rchint={Cyc_Absyn_MemKind,Cyc_Absyn_RefcntHint};
# 44
struct Cyc_Absyn_Kind Cyc_Kinds_ak_rhint={Cyc_Absyn_AnyKind,Cyc_Absyn_RestrictedHint};
struct Cyc_Absyn_Kind Cyc_Kinds_ak_uhint={Cyc_Absyn_AnyKind,Cyc_Absyn_UniqueHint};
struct Cyc_Absyn_Kind Cyc_Kinds_ak_rchint={Cyc_Absyn_AnyKind,Cyc_Absyn_RefcntHint};
# 55 "kinds.cyc"
struct Cyc_Core_Opt Cyc_Kinds_ako={(void*)& Cyc_Kinds_ak};
struct Cyc_Core_Opt Cyc_Kinds_bko={(void*)& Cyc_Kinds_bk};
struct Cyc_Core_Opt Cyc_Kinds_mko={(void*)& Cyc_Kinds_mk};
struct Cyc_Core_Opt Cyc_Kinds_iko={(void*)& Cyc_Kinds_ik};
struct Cyc_Core_Opt Cyc_Kinds_eko={(void*)& Cyc_Kinds_ek};
struct Cyc_Core_Opt Cyc_Kinds_boolko={(void*)& Cyc_Kinds_boolk};
struct Cyc_Core_Opt Cyc_Kinds_ptrbko={(void*)& Cyc_Kinds_ptrbk};
struct Cyc_Core_Opt Cyc_Kinds_aqko={(void*)& Cyc_Kinds_aqk};
# 64
struct Cyc_Core_Opt Cyc_Kinds_bk_rhinto={(void*)& Cyc_Kinds_bk_rhint};
struct Cyc_Core_Opt Cyc_Kinds_bk_uhinto={(void*)& Cyc_Kinds_bk_uhint};
struct Cyc_Core_Opt Cyc_Kinds_bk_rchinto={(void*)& Cyc_Kinds_bk_rchint};
# 68
struct Cyc_Core_Opt Cyc_Kinds_mk_rhinto={(void*)& Cyc_Kinds_mk_rhint};
struct Cyc_Core_Opt Cyc_Kinds_mk_uhinto={(void*)& Cyc_Kinds_mk_uhint};
struct Cyc_Core_Opt Cyc_Kinds_mk_rchinto={(void*)& Cyc_Kinds_mk_rchint};
# 72
struct Cyc_Core_Opt Cyc_Kinds_ak_rhinto={(void*)& Cyc_Kinds_ak_rhint};
struct Cyc_Core_Opt Cyc_Kinds_ak_uhinto={(void*)& Cyc_Kinds_ak_uhint};
struct Cyc_Core_Opt Cyc_Kinds_ak_rchinto={(void*)& Cyc_Kinds_ak_rchint};
# 85 "kinds.cyc"
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*kh){
enum Cyc_Absyn_AliasHint _Tmp0;enum Cyc_Absyn_KindQual _Tmp1;_Tmp1=kh->kind;_Tmp0=kh->aliashint;{enum Cyc_Absyn_KindQual k=_Tmp1;enum Cyc_Absyn_AliasHint h=_Tmp0;
if((int)h==3){
switch((int)k){case Cyc_Absyn_AnyKind:
 return& Cyc_Kinds_ako;case Cyc_Absyn_MemKind:
 return& Cyc_Kinds_mko;case Cyc_Absyn_BoxKind:
 return& Cyc_Kinds_bko;case Cyc_Absyn_EffKind:
# 93
 return& Cyc_Kinds_eko;case Cyc_Absyn_IntKind:
 return& Cyc_Kinds_iko;case Cyc_Absyn_BoolKind:
 return& Cyc_Kinds_bko;case Cyc_Absyn_AqualKind:
 return& Cyc_Kinds_aqko;case Cyc_Absyn_PtrBndKind:
 return& Cyc_Kinds_ptrbko;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("kind_to_opt: bad kind ",sizeof(char),23U);_Tmp3;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp4;_Tmp4.tag=9,_Tmp4.f1=kh;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});};}else{
# 102
switch((int)k){case Cyc_Absyn_BoxKind:
# 104
 switch((int)h){case Cyc_Absyn_RestrictedHint:
# 106
 return& Cyc_Kinds_bk_rhinto;case Cyc_Absyn_UniqueHint:
# 108
 return& Cyc_Kinds_bk_uhinto;default:
# 110
 return& Cyc_Kinds_bk_rchinto;};case Cyc_Absyn_MemKind:
# 113
 switch((int)h){case Cyc_Absyn_RestrictedHint:
# 115
 return& Cyc_Kinds_mk_rhinto;case Cyc_Absyn_UniqueHint:
# 117
 return& Cyc_Kinds_mk_uhinto;default:
# 119
 return& Cyc_Kinds_mk_rchinto;};case Cyc_Absyn_AnyKind:
# 122
 switch((int)h){case Cyc_Absyn_RestrictedHint:
# 124
 return& Cyc_Kinds_ak_rhinto;case Cyc_Absyn_UniqueHint:
# 126
 return& Cyc_Kinds_ak_uhinto;default:
# 128
 return& Cyc_Kinds_ak_rchinto;};default:
# 131
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("kind_to_opt: bad kind ",sizeof(char),23U);_Tmp3;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp4;_Tmp4.tag=9,_Tmp4.f1=kh;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});};}}}
# 136
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp0->tag=0,_Tmp0->f1=k;_Tmp0;});}
# 139
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp1=Cyc_Kinds_kind_to_bound(k);_Tmp0->v=_Tmp1;});return _Tmp0;}
# 144
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr s,unsigned loc){
if(Cyc_strlen(s)==1U || Cyc_strlen(s)==2U){
char _Tmp0=((const char*)s.curr)[0];switch((int)_Tmp0){case 65:
 return& Cyc_Kinds_ak;case 77:
 return& Cyc_Kinds_mk;case 66:
 return& Cyc_Kinds_bk;case 82:
 Cyc_Warn_warn(loc,_tag_fat("Region kind ::R is deprecated; use effect kind ::E instead",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));return& Cyc_Kinds_ek;case 69:
 return& Cyc_Kinds_ek;case 73:
 return& Cyc_Kinds_ik;case 81:
 return& Cyc_Kinds_aqk;default:
# 155
 if(Cyc_strlen(s)==2U){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Usage of deprecated kind: ",sizeof(char),27U);_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=s;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});
if((int)((const char*)s.curr)[1]==66){
char _Tmp1=((const char*)s.curr)[0];switch((int)_Tmp1){case 85:
 return& Cyc_Kinds_bk_uhint;case 84:
 return& Cyc_Kinds_bk_rhint;case 67:
 return& Cyc_Kinds_bk_rchint;default:
 goto _LL11;}_LL11:;}
# 165
if((int)((const char*)s.curr)[1]==77){
char _Tmp1=((const char*)s.curr)[0];switch((int)_Tmp1){case 85:
 return& Cyc_Kinds_mk_uhint;case 84:
 return& Cyc_Kinds_mk_rhint;case 67:
 return& Cyc_Kinds_mk_rchint;default:
 goto _LL1A;}_LL1A:;}
# 173
if((int)((const char*)s.curr)[1]==65){
char _Tmp1=((const char*)s.curr)[0];switch((int)_Tmp1){case 85:
 return& Cyc_Kinds_ak_uhint;case 84:
 return& Cyc_Kinds_ak_rhint;case 67:
 return& Cyc_Kinds_ak_rchint;default:
 goto _LL23;}_LL23:;}
# 181
if((int)((const char*)s.curr)[1]==82){
Cyc_Warn_warn(loc,_tag_fat("Region kind ::R is deprecated; use effect kind ::E instead",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));
return& Cyc_Kinds_ek;}}
# 186
goto _LL0;}_LL0:;}
# 189
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad kind: ",sizeof(char),11U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=s;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" strlen=",sizeof(char),9U);_Tmp3;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp4;_Tmp4.tag=12,({int _Tmp5=(int)Cyc_strlen(s);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return& Cyc_Kinds_bk;}
# 193
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*kh){
enum Cyc_Absyn_AliasHint _Tmp0;enum Cyc_Absyn_KindQual _Tmp1;_Tmp1=kh->kind;_Tmp0=kh->aliashint;{enum Cyc_Absyn_KindQual k=_Tmp1;enum Cyc_Absyn_AliasHint h=_Tmp0;
if((int)h==3){
switch((int)k){case Cyc_Absyn_AnyKind:
 return _tag_fat("A",sizeof(char),2U);case Cyc_Absyn_MemKind:
 return _tag_fat("M",sizeof(char),2U);case Cyc_Absyn_BoxKind:
 return _tag_fat("B",sizeof(char),2U);case Cyc_Absyn_AqualKind:
# 201
 return _tag_fat("Q",sizeof(char),2U);case Cyc_Absyn_EffKind:
 return _tag_fat("E",sizeof(char),2U);case Cyc_Absyn_IntKind:
 return _tag_fat("I",sizeof(char),2U);case Cyc_Absyn_BoolKind:
 return _tag_fat("BOOL",sizeof(char),5U);case Cyc_Absyn_PtrBndKind:
 goto _LL15;default: _LL15: return _tag_fat("PTRBND",sizeof(char),7U);};}else{
# 209
switch((int)k){case Cyc_Absyn_BoxKind:
# 211
 switch((int)h){case Cyc_Absyn_RestrictedHint:
 return _tag_fat("TB",sizeof(char),3U);case Cyc_Absyn_UniqueHint:
 return _tag_fat("UB",sizeof(char),3U);default:
 return _tag_fat("CB",sizeof(char),3U);};case Cyc_Absyn_MemKind:
# 217
 switch((int)h){case Cyc_Absyn_RestrictedHint:
 return _tag_fat("TM",sizeof(char),3U);case Cyc_Absyn_UniqueHint:
 return _tag_fat("UM",sizeof(char),3U);default:
 return _tag_fat("CM",sizeof(char),3U);};case Cyc_Absyn_AnyKind:
# 223
 switch((int)h){case Cyc_Absyn_RestrictedHint:
 return _tag_fat("TA",sizeof(char),3U);case Cyc_Absyn_UniqueHint:
 return _tag_fat("UA",sizeof(char),3U);default:
 return _tag_fat("CA",sizeof(char),3U);};default:
# 229
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("Bad kind",sizeof(char),9U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});};}}}
# 234
void*Cyc_Kinds_compress_kb(void*k){
void*_Tmp0;switch(*((int*)k)){case 0:
 goto _LL4;case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)k)->f1==0){_LL4:
 goto _LL6;}else{_Tmp0=(void**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)k)->f1->v;{void**k2=_Tmp0;
# 239
_Tmp0=k2;goto _LLA;}}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)k)->f1==0){_LL6:
# 238
 return k;}else{_Tmp0=(void**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)k)->f1->v;_LLA: {void**k2=(void**)_Tmp0;
# 241
({void*_Tmp1=Cyc_Kinds_compress_kb(*k2);*k2=_Tmp1;});
return*k2;}}};}
# 245
struct Cyc_Absyn_Kind*Cyc_Kinds_force_kb(void*kb){
void*_Tmp0=Cyc_Kinds_compress_kb(kb);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp2;
return k;}case 1: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp2;
_Tmp2=f;_Tmp1=& Cyc_Kinds_bk;goto _LL6;}default: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;_LL6: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp2;struct Cyc_Absyn_Kind*k=_Tmp1;
# 250
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(k);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});
return k;}};}
# 258
void*Cyc_Kinds_copy_kindbound(void*kb){
void*_Tmp0=Cyc_Kinds_compress_kb(kb);void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp2->tag=1,_Tmp2->f1=0;_Tmp2;});case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp2->tag=2,_Tmp2->f1=0,_Tmp2->f2=k;_Tmp2;});}default:
 return kb;};}struct _tuple11{enum Cyc_Absyn_KindQual f0;enum Cyc_Absyn_KindQual f1;};
# 274 "kinds.cyc"
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _Tmp0;_Tmp0=ka1->kind;{enum Cyc_Absyn_KindQual k1=_Tmp0;
enum Cyc_Absyn_KindQual _Tmp1;_Tmp1=ka2->kind;{enum Cyc_Absyn_KindQual k2=_Tmp1;
# 278
if((int)k1!=(int)k2){
struct _tuple11 _Tmp2=({struct _tuple11 _Tmp3;_Tmp3.f0=k1,_Tmp3.f1=k2;_Tmp3;});switch((int)_Tmp2.f0){case Cyc_Absyn_BoxKind: switch((int)_Tmp2.f1){case Cyc_Absyn_MemKind:
 goto _LLA;case Cyc_Absyn_AnyKind: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_Tmp2.f1==Cyc_Absyn_AnyKind){_LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD:
 return 0;}_LL6:;}
# 292
return 1;}}}
# 295
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return k;}case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return k;}default:
({void*_Tmp2=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp3->tag=2,_Tmp3->f1=0,_Tmp3->f2=def;_Tmp3;});tv->kind=_Tmp2;});return def;};}
# 302
struct Cyc_Core_Opt Cyc_Kinds_tvar_kind_opt(struct Cyc_Absyn_Tvar*tv){
void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
struct Cyc_Core_Opt _Tmp2;_Tmp2.v=k;return _Tmp2;}case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
struct Cyc_Core_Opt _Tmp2;_Tmp2.v=k;return _Tmp2;}default:  {
struct Cyc_Core_Opt _Tmp2;_Tmp2.v=0;return _Tmp2;}};}struct _tuple12{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 310
struct _tuple12 Cyc_Kinds_swap_kind(void*t,void*kb){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 313
void*oldkb=tv->kind;
tv->kind=kb;{
struct _tuple12 _Tmp2;_Tmp2.f0=tv,_Tmp2.f1=oldkb;return _Tmp2;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("swap_kind: cannot update the kind of ",sizeof(char),38U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});};}
# 320
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1==k2 ||(int)k1->kind==(int)k2->kind &&(int)k1->aliashint==(int)k2->aliashint;}struct _tuple13{void*f0;void*f1;};
# 327
int Cyc_Kinds_constrain_kinds(void*c1,void*c2){
c1=Cyc_Kinds_compress_kb(c1);
c2=Cyc_Kinds_compress_kb(c2);
if(c1==c2)return 1;{
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=c1,_Tmp1.f1=c2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==0)switch(*((int*)_Tmp0.f1)){case 0: _Tmp4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Kind*k1=_Tmp4;struct Cyc_Absyn_Kind*k2=_Tmp3;
return k1==k2;}case 1: goto _LL3;default: _Tmp4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f1;_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Kind*k1=_Tmp4;struct Cyc_Core_Opt**f=_Tmp3;struct Cyc_Absyn_Kind*k2=_Tmp2;
# 341
if(!Cyc_Kinds_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c1;_Tmp6;});*f=_Tmp5;});
return 1;}}else{if(*((int*)_Tmp0.f1)==1){_LL3: _Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;{struct Cyc_Core_Opt**f=_Tmp4;
# 333
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c1;_Tmp6;});*f=_Tmp5;});return 1;}}else{if(*((int*)_Tmp0.f0)==1){_Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f1;{struct Cyc_Core_Opt**f=_Tmp4;
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c2;_Tmp6;});*f=_Tmp5;});return 1;}}else{if(*((int*)_Tmp0.f1)==0){_Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f2;_Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;{struct Cyc_Core_Opt**f=_Tmp4;struct Cyc_Absyn_Kind*k1=_Tmp3;struct Cyc_Absyn_Kind*k2=_Tmp2;
# 336
if(!Cyc_Kinds_kind_leq(k2,k1))
return 0;
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c2;_Tmp6;});*f=_Tmp5;});
return 1;}}else{_Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f2;_Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f2;{struct Cyc_Core_Opt**f1=(struct Cyc_Core_Opt**)_Tmp4;struct Cyc_Absyn_Kind*k1=_Tmp3;struct Cyc_Core_Opt**f2=(struct Cyc_Core_Opt**)_Tmp2;struct Cyc_Absyn_Kind*k2=_Tmp1;
# 346
if(Cyc_Kinds_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c1;_Tmp6;});*f2=_Tmp5;});
return 1;}
# 350
if(Cyc_Kinds_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c2;_Tmp6;});*f1=_Tmp5;});
return 1;}
# 354
return 0;}}}}};}}
# 358
void*Cyc_Kinds_consistent_aliashint(unsigned loc,void*kb,void*aq){
{enum Cyc_Absyn_AliasHint _Tmp0;if(*((int*)kb)==0){_Tmp0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)kb)->f1->aliashint;{enum Cyc_Absyn_AliasHint ah=_Tmp0;
# 361
switch((int)ah){case Cyc_Absyn_NoHint:
# 363
 return aq;case Cyc_Absyn_RestrictedHint:
# 365
 if(aq==0 || aq==Cyc_Absyn_rtd_qual_type)
return Cyc_Absyn_rtd_qual_type;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Inconsistent qualifier bounds",sizeof(char),30U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL5;case Cyc_Absyn_UniqueHint:
# 370
 if(aq==0 || aq==Cyc_Absyn_un_qual_type)
return Cyc_Absyn_un_qual_type;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Inconsistent qualifier bounds",sizeof(char),30U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL5;case Cyc_Absyn_RefcntHint:
# 375
 if(aq==0 || aq==Cyc_Absyn_rc_qual_type)
return Cyc_Absyn_rc_qual_type;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Inconsistent qualifier bounds",sizeof(char),30U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL5;default:
# 380
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Inconsistent qualifier bounds",sizeof(char),30U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});}_LL5:;
# 382
goto _LL0;}}else{
# 384
goto _LL0;}_LL0:;}
# 386
return aq;}

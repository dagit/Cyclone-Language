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
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 160 "absyn.h"
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 165
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 855 "absyn.h"
void*Cyc_Absyn_compress(void*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 42
extern struct Cyc_Absyn_Kind Cyc_Kinds_urk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_uak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ubk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_umk;
# 47
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
extern struct Cyc_Core_Opt Cyc_Kinds_ako;
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
# 54
extern struct Cyc_Core_Opt Cyc_Kinds_ptrbko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
extern struct Cyc_Core_Opt Cyc_Kinds_tako;
extern struct Cyc_Core_Opt Cyc_Kinds_tbko;
extern struct Cyc_Core_Opt Cyc_Kinds_tmko;
# 61
extern struct Cyc_Core_Opt Cyc_Kinds_urko;
extern struct Cyc_Core_Opt Cyc_Kinds_uako;
extern struct Cyc_Core_Opt Cyc_Kinds_ubko;
extern struct Cyc_Core_Opt Cyc_Kinds_umko;
# 26 "kinds.cyc"
struct Cyc_Absyn_Kind Cyc_Kinds_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Kinds_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Kinds_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Kinds_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Kinds_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Kinds_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Kinds_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 35
struct Cyc_Absyn_Kind Cyc_Kinds_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Kinds_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Kinds_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Kinds_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 40
struct Cyc_Absyn_Kind Cyc_Kinds_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Kinds_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Kinds_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Kinds_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 45
struct Cyc_Core_Opt Cyc_Kinds_rko={(void*)& Cyc_Kinds_rk};
struct Cyc_Core_Opt Cyc_Kinds_ako={(void*)& Cyc_Kinds_ak};
struct Cyc_Core_Opt Cyc_Kinds_bko={(void*)& Cyc_Kinds_bk};
struct Cyc_Core_Opt Cyc_Kinds_mko={(void*)& Cyc_Kinds_mk};
struct Cyc_Core_Opt Cyc_Kinds_iko={(void*)& Cyc_Kinds_ik};
struct Cyc_Core_Opt Cyc_Kinds_eko={(void*)& Cyc_Kinds_ek};
struct Cyc_Core_Opt Cyc_Kinds_boolko={(void*)& Cyc_Kinds_boolk};
struct Cyc_Core_Opt Cyc_Kinds_ptrbko={(void*)& Cyc_Kinds_ptrbk};
# 54
struct Cyc_Core_Opt Cyc_Kinds_trko={(void*)& Cyc_Kinds_trk};
struct Cyc_Core_Opt Cyc_Kinds_tako={(void*)& Cyc_Kinds_tak};
struct Cyc_Core_Opt Cyc_Kinds_tbko={(void*)& Cyc_Kinds_tbk};
struct Cyc_Core_Opt Cyc_Kinds_tmko={(void*)& Cyc_Kinds_tmk};
# 59
struct Cyc_Core_Opt Cyc_Kinds_urko={(void*)& Cyc_Kinds_urk};
struct Cyc_Core_Opt Cyc_Kinds_uako={(void*)& Cyc_Kinds_uak};
struct Cyc_Core_Opt Cyc_Kinds_ubko={(void*)& Cyc_Kinds_ubk};
struct Cyc_Core_Opt Cyc_Kinds_umko={(void*)& Cyc_Kinds_umk};
# 64
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_AliasQual _Tmp0;enum Cyc_Absyn_KindQual _Tmp1;_Tmp1=ka->kind;_Tmp0=ka->aliasqual;{enum Cyc_Absyn_KindQual k=_Tmp1;enum Cyc_Absyn_AliasQual a=_Tmp0;
switch((int)a){case Cyc_Absyn_Aliasable:
# 68
 switch((int)k){case Cyc_Absyn_AnyKind:
 return& Cyc_Kinds_ako;case Cyc_Absyn_MemKind:
 return& Cyc_Kinds_mko;case Cyc_Absyn_BoxKind:
 return& Cyc_Kinds_bko;case Cyc_Absyn_RgnKind:
 return& Cyc_Kinds_rko;case Cyc_Absyn_EffKind:
 return& Cyc_Kinds_eko;case Cyc_Absyn_IntKind:
 return& Cyc_Kinds_iko;case Cyc_Absyn_BoolKind:
 return& Cyc_Kinds_bko;case Cyc_Absyn_PtrBndKind:
 goto _LL1E;default: _LL1E: return& Cyc_Kinds_ptrbko;};case Cyc_Absyn_Unique:
# 79
 switch((int)k){case Cyc_Absyn_AnyKind:
 return& Cyc_Kinds_uako;case Cyc_Absyn_MemKind:
 return& Cyc_Kinds_umko;case Cyc_Absyn_BoxKind:
 return& Cyc_Kinds_ubko;case Cyc_Absyn_RgnKind:
 return& Cyc_Kinds_urko;default:
 goto _LL1F;}_LL1F:;
# 86
goto _LL3;case Cyc_Absyn_Top:
# 88
 switch((int)k){case Cyc_Absyn_AnyKind:
 return& Cyc_Kinds_tako;case Cyc_Absyn_MemKind:
 return& Cyc_Kinds_tmko;case Cyc_Absyn_BoxKind:
 return& Cyc_Kinds_tbko;case Cyc_Absyn_RgnKind:
 return& Cyc_Kinds_trko;default:
 goto _LL2A;}_LL2A:;
# 95
goto _LL3;default:
 goto _LL3;}_LL3:;
# 98
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("kind_to_opt: bad kind ",sizeof(char),23U);_Tmp3;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp4;_Tmp4.tag=9,_Tmp4.f1=ka;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});}}
# 101
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp0->tag=0,_Tmp0->f1=k;_Tmp0;});}
# 104
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp1=Cyc_Kinds_kind_to_bound(k);_Tmp0->v=_Tmp1;});_Tmp0;});}
# 109
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr s,unsigned loc){
if(Cyc_strlen(s)==1U || Cyc_strlen(s)==2U){
char _Tmp0=((const char*)s.curr)[0];switch((int)_Tmp0){case 65:
 return& Cyc_Kinds_ak;case 77:
 return& Cyc_Kinds_mk;case 66:
 return& Cyc_Kinds_bk;case 82:
 return& Cyc_Kinds_rk;case 69:
 return& Cyc_Kinds_ek;case 73:
 return& Cyc_Kinds_ik;case 85:
# 119
{char _Tmp1=((const char*)s.curr)[1];switch((int)_Tmp1){case 82:
 return& Cyc_Kinds_urk;case 65:
 return& Cyc_Kinds_uak;case 77:
 return& Cyc_Kinds_umk;case 66:
 return& Cyc_Kinds_ubk;default:
 goto _LL13;}_LL13:;}
# 126
goto _LL0;case 84:
# 128
{char _Tmp1=((const char*)s.curr)[1];switch((int)_Tmp1){case 82:
 return& Cyc_Kinds_trk;case 65:
 return& Cyc_Kinds_tak;case 77:
 return& Cyc_Kinds_tmk;case 66:
 return& Cyc_Kinds_tbk;default:
 goto _LL1E;}_LL1E:;}
# 135
goto _LL0;default:
 goto _LL0;}_LL0:;}
# 138
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad kind: ",sizeof(char),11U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=s;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" strlen=",sizeof(char),9U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp5;_Tmp5.tag=12,({int _Tmp6=(int)Cyc_strlen((struct _fat_ptr)s);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),5));});
return& Cyc_Kinds_bk;}
# 142
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_AliasQual _Tmp0;enum Cyc_Absyn_KindQual _Tmp1;_Tmp1=ka->kind;_Tmp0=ka->aliasqual;{enum Cyc_Absyn_KindQual k=_Tmp1;enum Cyc_Absyn_AliasQual a=_Tmp0;
switch((int)k){case Cyc_Absyn_AnyKind:
# 146
 switch((int)a){case Cyc_Absyn_Aliasable:
 return _tag_fat("A",sizeof(char),2U);case Cyc_Absyn_Unique:
 return _tag_fat("UA",sizeof(char),3U);case Cyc_Absyn_Top:
 goto _LL1E;default: _LL1E: return _tag_fat("TA",sizeof(char),3U);};case Cyc_Absyn_MemKind:
# 152
 switch((int)a){case Cyc_Absyn_Aliasable:
 return _tag_fat("M",sizeof(char),2U);case Cyc_Absyn_Unique:
 return _tag_fat("UM",sizeof(char),3U);case Cyc_Absyn_Top:
 goto _LL27;default: _LL27: return _tag_fat("TM",sizeof(char),3U);};case Cyc_Absyn_BoxKind:
# 158
 switch((int)a){case Cyc_Absyn_Aliasable:
 return _tag_fat("B",sizeof(char),2U);case Cyc_Absyn_Unique:
 return _tag_fat("UB",sizeof(char),3U);case Cyc_Absyn_Top:
 goto _LL30;default: _LL30: return _tag_fat("TB",sizeof(char),3U);};case Cyc_Absyn_RgnKind:
# 164
 switch((int)a){case Cyc_Absyn_Aliasable:
 return _tag_fat("R",sizeof(char),2U);case Cyc_Absyn_Unique:
 return _tag_fat("UR",sizeof(char),3U);case Cyc_Absyn_Top:
 goto _LL39;default: _LL39: return _tag_fat("TR",sizeof(char),3U);};case Cyc_Absyn_EffKind:
# 169
 return _tag_fat("E",sizeof(char),2U);case Cyc_Absyn_IntKind:
 return _tag_fat("I",sizeof(char),2U);case Cyc_Absyn_BoolKind:
 return _tag_fat("BOOL",sizeof(char),5U);case Cyc_Absyn_PtrBndKind:
 goto _LL15;default: _LL15: return _tag_fat("PTRBND",sizeof(char),7U);};}}
# 176
void*Cyc_Kinds_compress_kb(void*k){
void*_Tmp0;switch(*((int*)k)){case 0:
 goto _LL4;case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)k)->f1==0){_LL4:
 goto _LL6;}else{_Tmp0=(void**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)k)->f1->v;{void**k2=_Tmp0;
# 181
_Tmp0=k2;goto _LLA;}}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)k)->f1==0){_LL6:
# 180
 return k;}else{_Tmp0=(void**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)k)->f1->v;_LLA: {void**k2=(void**)_Tmp0;
# 183
({void*_Tmp1=Cyc_Kinds_compress_kb(*k2);*k2=_Tmp1;});
return*k2;}}};}
# 187
struct Cyc_Absyn_Kind*Cyc_Kinds_force_kb(void*kb){
void*_Tmp0=Cyc_Kinds_compress_kb(kb);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp2;
return k;}case 1: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp2;
_Tmp2=f;_Tmp1=& Cyc_Kinds_bk;goto _LL6;}default: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;_LL6: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp2;struct Cyc_Absyn_Kind*k=_Tmp1;
# 192
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(k);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});
return k;}};}
# 200
void*Cyc_Kinds_copy_kindbound(void*kb){
void*_Tmp0=Cyc_Kinds_compress_kb(kb);void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp2->tag=1,_Tmp2->f1=0;_Tmp2;});case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp2->tag=2,_Tmp2->f1=0,_Tmp2->f2=k;_Tmp2;});}default:
 return kb;};}struct _tuple11{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple12{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 216 "kinds.cyc"
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_AliasQual _Tmp0;enum Cyc_Absyn_KindQual _Tmp1;_Tmp1=ka1->kind;_Tmp0=ka1->aliasqual;{enum Cyc_Absyn_KindQual k1=_Tmp1;enum Cyc_Absyn_AliasQual a1=_Tmp0;
enum Cyc_Absyn_AliasQual _Tmp2;enum Cyc_Absyn_KindQual _Tmp3;_Tmp3=ka2->kind;_Tmp2=ka2->aliasqual;{enum Cyc_Absyn_KindQual k2=_Tmp3;enum Cyc_Absyn_AliasQual a2=_Tmp2;
# 220
if((int)k1!=(int)k2){
struct _tuple11 _Tmp4=({struct _tuple11 _Tmp5;_Tmp5.f1=k1,_Tmp5.f2=k2;_Tmp5;});switch((int)_Tmp4.f1){case Cyc_Absyn_BoxKind: switch((int)_Tmp4.f2){case Cyc_Absyn_MemKind:
 goto _LLA;case Cyc_Absyn_AnyKind: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_Tmp4.f2==Cyc_Absyn_AnyKind){_LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD:
 return 0;}_LL6:;}
# 228
if((int)a1!=(int)a2){
struct _tuple12 _Tmp4=({struct _tuple12 _Tmp5;_Tmp5.f1=a1,_Tmp5.f2=a2;_Tmp5;});switch((int)_Tmp4.f1){case Cyc_Absyn_Aliasable: if(_Tmp4.f2==Cyc_Absyn_Top)
goto _LL13;else{goto _LL14;}case Cyc_Absyn_Unique: if(_Tmp4.f2==Cyc_Absyn_Top){_LL13:
 return 1;}else{goto _LL14;}default: _LL14:
 return 0;};}
# 234
return 1;}}}
# 237
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return k;}case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return k;}default:
({void*_Tmp2=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp3->tag=2,_Tmp3->f1=0,_Tmp3->f2=def;_Tmp3;});tv->kind=_Tmp2;});return def;};}struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 245
struct _tuple13 Cyc_Kinds_swap_kind(void*t,void*kb){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 248
void*oldkb=tv->kind;
tv->kind=kb;
return({struct _tuple13 _Tmp2;_Tmp2.f1=tv,_Tmp2.f2=oldkb;_Tmp2;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("swap_kind: cannot update the kind of ",sizeof(char),38U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});};}
# 255
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1==k2 ||(int)k1->kind==(int)k2->kind &&(int)k1->aliasqual==(int)k2->aliasqual;}struct _tuple14{void*f1;void*f2;};
# 262
int Cyc_Kinds_constrain_kinds(void*c1,void*c2){
c1=Cyc_Kinds_compress_kb(c1);
c2=Cyc_Kinds_compress_kb(c2);
if(c1==c2)return 1;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f1=c1,_Tmp1.f2=c2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f1)==0)switch(*((int*)_Tmp0.f2)){case 0: _Tmp4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Kind*k1=_Tmp4;struct Cyc_Absyn_Kind*k2=_Tmp3;
return k1==k2;}case 1: goto _LL3;default: _Tmp4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Kind*k1=_Tmp4;struct Cyc_Core_Opt**f=_Tmp3;struct Cyc_Absyn_Kind*k2=_Tmp2;
# 276
if(!Cyc_Kinds_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c1;_Tmp6;});*f=_Tmp5;});
return 1;}}else{if(*((int*)_Tmp0.f2)==1){_LL3: _Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;{struct Cyc_Core_Opt**f=_Tmp4;
# 268
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c1;_Tmp6;});*f=_Tmp5;});return 1;}}else{if(*((int*)_Tmp0.f1)==1){_Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;{struct Cyc_Core_Opt**f=_Tmp4;
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c2;_Tmp6;});*f=_Tmp5;});return 1;}}else{if(*((int*)_Tmp0.f2)==0){_Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f2;_Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;{struct Cyc_Core_Opt**f=_Tmp4;struct Cyc_Absyn_Kind*k1=_Tmp3;struct Cyc_Absyn_Kind*k2=_Tmp2;
# 271
if(!Cyc_Kinds_kind_leq(k2,k1))
return 0;
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c2;_Tmp6;});*f=_Tmp5;});
return 1;}}else{_Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f2;_Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f2;{struct Cyc_Core_Opt**f1=(struct Cyc_Core_Opt**)_Tmp4;struct Cyc_Absyn_Kind*k1=_Tmp3;struct Cyc_Core_Opt**f2=(struct Cyc_Core_Opt**)_Tmp2;struct Cyc_Absyn_Kind*k2=_Tmp1;
# 281
if(Cyc_Kinds_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c1;_Tmp6;});*f2=_Tmp5;});
return 1;}
# 285
if(Cyc_Kinds_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=c2;_Tmp6;});*f1=_Tmp5;});
return 1;}
# 289
return 0;}}}}};}}

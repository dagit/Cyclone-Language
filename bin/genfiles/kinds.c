#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_*.c */
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
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
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

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons *);
void _push_region(struct _RegionHandle *);
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
#define _zero_arr_plus_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_char_fn _zero_arr_plus_fn
#define _zero_arr_plus_short_fn _zero_arr_plus_fn
#define _zero_arr_plus_int_fn _zero_arr_plus_fn
#define _zero_arr_plus_float_fn _zero_arr_plus_fn
#define _zero_arr_plus_double_fn _zero_arr_plus_fn
#define _zero_arr_plus_longdouble_fn _zero_arr_plus_fn
#define _zero_arr_plus_voidstar_fn _zero_arr_plus_fn
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
short* _zero_arr_plus_short_fn(short*,unsigned,int,const char*,unsigned);
int* _zero_arr_plus_int_fn(int*,unsigned,int,const char*,unsigned);
float* _zero_arr_plus_float_fn(float*,unsigned,int,const char*,unsigned);
double* _zero_arr_plus_double_fn(double*,unsigned,int,const char*,unsigned);
long double* _zero_arr_plus_longdouble_fn(long double*,unsigned,int,const char*, unsigned);
void** _zero_arr_plus_voidstar_fn(void**,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
int _get_zero_arr_size_char(const char*,unsigned);
int _get_zero_arr_size_short(const short*,unsigned);
int _get_zero_arr_size_int(const int*,unsigned);
int _get_zero_arr_size_float(const float*,unsigned);
int _get_zero_arr_size_double(const double*,unsigned);
int _get_zero_arr_size_longdouble(const long double*,unsigned);
int _get_zero_arr_size_voidstar(const void**,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_short_fn(short**,int,const char*,unsigned);
int* _zero_arr_inplace_plus_int(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_longdouble_fn(long double**,int,const char*,unsigned);
void** _zero_arr_inplace_plus_voidstar_fn(void***,int,const char*,unsigned);
/* like the previous functions, but does post-addition (as in e++) */
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_post_short_fn(short**x,int,const char*,unsigned);
int* _zero_arr_inplace_plus_post_int_fn(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_post_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_post_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_post_longdouble_fn(long double**,int,const char *,unsigned);
void** _zero_arr_inplace_plus_post_voidstar_fn(void***,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
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
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
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

/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 175 "absyn.h"
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 863 "absyn.h"
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
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
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
struct Cyc_Absyn_Kind*_tmp0=ka;enum Cyc_Absyn_AliasQual _tmp2;enum Cyc_Absyn_KindQual _tmp1;_tmp1=_tmp0->kind;_tmp2=_tmp0->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp1;enum Cyc_Absyn_AliasQual a=_tmp2;
{enum Cyc_Absyn_AliasQual _tmp3=a;switch((int)_tmp3){case Cyc_Absyn_Aliasable:  {
# 68
enum Cyc_Absyn_KindQual _tmp4=k;switch((int)_tmp4){case Cyc_Absyn_AnyKind:
 return& Cyc_Kinds_ako;case Cyc_Absyn_MemKind:
 return& Cyc_Kinds_mko;case Cyc_Absyn_BoxKind:
 return& Cyc_Kinds_bko;case Cyc_Absyn_RgnKind:
 return& Cyc_Kinds_rko;case Cyc_Absyn_EffKind:
 return& Cyc_Kinds_eko;case Cyc_Absyn_IntKind:
 return& Cyc_Kinds_iko;case Cyc_Absyn_BoolKind:
 return& Cyc_Kinds_bko;case Cyc_Absyn_PtrBndKind:
 goto _LL1E;default: _LL1E: return& Cyc_Kinds_ptrbko;};}case Cyc_Absyn_Unique:
# 79
{enum Cyc_Absyn_KindQual _tmp5=k;switch((int)_tmp5){case Cyc_Absyn_AnyKind:
 return& Cyc_Kinds_uako;case Cyc_Absyn_MemKind:
 return& Cyc_Kinds_umko;case Cyc_Absyn_BoxKind:
 return& Cyc_Kinds_ubko;case Cyc_Absyn_RgnKind:
 return& Cyc_Kinds_urko;default:
 goto _LL1F;}_LL1F:;}
# 86
goto _LL3;case Cyc_Absyn_Top:
# 88
{enum Cyc_Absyn_KindQual _tmp6=k;switch((int)_tmp6){case Cyc_Absyn_AnyKind:
 return& Cyc_Kinds_tako;case Cyc_Absyn_MemKind:
 return& Cyc_Kinds_tmko;case Cyc_Absyn_BoxKind:
 return& Cyc_Kinds_tbko;case Cyc_Absyn_RgnKind:
 return& Cyc_Kinds_trko;default:
 goto _LL2A;}_LL2A:;}
# 95
goto _LL3;default:
 goto _LL3;}_LL3:;}
# 98
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62;_tmp62.tag=0,({struct _fat_ptr _tmp6E=({const char*_tmpA="kind_to_opt: bad kind ";_tag_fat(_tmpA,sizeof(char),23U);});_tmp62.f1=_tmp6E;});_tmp62;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp9=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp61;_tmp61.tag=9,_tmp61.f1=ka;_tmp61;});void*_tmp7[2];_tmp7[0]=& _tmp8,_tmp7[1]=& _tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp7,sizeof(void*),2));});}}
# 101
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=0,_tmpB->f1=k;_tmpB;});}
# 104
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmpC=_cycalloc(sizeof(*_tmpC));({void*_tmp6F=Cyc_Kinds_kind_to_bound(k);_tmpC->v=_tmp6F;});_tmpC;});}
# 109
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr s,unsigned loc){
if(Cyc_strlen((struct _fat_ptr)s)== 1U || Cyc_strlen((struct _fat_ptr)s)== 2U){
char _tmpD=((const char*)s.curr)[0];char _stmttmp0=_tmpD;char _tmpE=_stmttmp0;switch((int)_tmpE){case 65:
 return& Cyc_Kinds_ak;case 77:
 return& Cyc_Kinds_mk;case 66:
 return& Cyc_Kinds_bk;case 82:
 return& Cyc_Kinds_rk;case 69:
 return& Cyc_Kinds_ek;case 73:
 return& Cyc_Kinds_ik;case 85:
# 119
{char _tmpF=((const char*)s.curr)[1];char _stmttmp1=_tmpF;char _tmp10=_stmttmp1;switch((int)_tmp10){case 82:
 return& Cyc_Kinds_urk;case 65:
 return& Cyc_Kinds_uak;case 77:
 return& Cyc_Kinds_umk;case 66:
 return& Cyc_Kinds_ubk;default:
 goto _LL13;}_LL13:;}
# 126
goto _LL0;case 84:
# 128
{char _tmp11=((const char*)s.curr)[1];char _stmttmp2=_tmp11;char _tmp12=_stmttmp2;switch((int)_tmp12){case 82:
 return& Cyc_Kinds_trk;case 65:
 return& Cyc_Kinds_tak;case 77:
 return& Cyc_Kinds_tmk;case 66:
 return& Cyc_Kinds_tbk;default:
 goto _LL1E;}_LL1E:;}
# 135
goto _LL0;default:
 goto _LL0;}_LL0:;}
# 138
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67;_tmp67.tag=0,({struct _fat_ptr _tmp70=({const char*_tmp1A="bad kind: ";_tag_fat(_tmp1A,sizeof(char),11U);});_tmp67.f1=_tmp70;});_tmp67;});struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66;_tmp66.tag=0,_tmp66.f1=s;_tmp66;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65;_tmp65.tag=0,({struct _fat_ptr _tmp71=({const char*_tmp19=" strlen=";_tag_fat(_tmp19,sizeof(char),9U);});_tmp65.f1=_tmp71;});_tmp65;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64;_tmp64.tag=0,_tmp64.f1=s;_tmp64;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp18=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp63;_tmp63.tag=12,({int _tmp72=(int)Cyc_strlen((struct _fat_ptr)s);_tmp63.f1=_tmp72;});_tmp63;});void*_tmp13[5];_tmp13[0]=& _tmp14,_tmp13[1]=& _tmp15,_tmp13[2]=& _tmp16,_tmp13[3]=& _tmp17,_tmp13[4]=& _tmp18;Cyc_Warn_err2(loc,_tag_fat(_tmp13,sizeof(void*),5));});
return& Cyc_Kinds_bk;}
# 142
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp1B=ka;enum Cyc_Absyn_AliasQual _tmp1D;enum Cyc_Absyn_KindQual _tmp1C;_tmp1C=_tmp1B->kind;_tmp1D=_tmp1B->aliasqual;{enum Cyc_Absyn_KindQual k=_tmp1C;enum Cyc_Absyn_AliasQual a=_tmp1D;
enum Cyc_Absyn_KindQual _tmp1E=k;switch((int)_tmp1E){case Cyc_Absyn_AnyKind:  {
# 146
enum Cyc_Absyn_AliasQual _tmp1F=a;switch((int)_tmp1F){case Cyc_Absyn_Aliasable:
 return({const char*_tmp20="A";_tag_fat(_tmp20,sizeof(char),2U);});case Cyc_Absyn_Unique:
 return({const char*_tmp21="UA";_tag_fat(_tmp21,sizeof(char),3U);});case Cyc_Absyn_Top:
 goto _LL1E;default: _LL1E: return({const char*_tmp22="TA";_tag_fat(_tmp22,sizeof(char),3U);});};}case Cyc_Absyn_MemKind:  {
# 152
enum Cyc_Absyn_AliasQual _tmp23=a;switch((int)_tmp23){case Cyc_Absyn_Aliasable:
 return({const char*_tmp24="M";_tag_fat(_tmp24,sizeof(char),2U);});case Cyc_Absyn_Unique:
 return({const char*_tmp25="UM";_tag_fat(_tmp25,sizeof(char),3U);});case Cyc_Absyn_Top:
 goto _LL27;default: _LL27: return({const char*_tmp26="TM";_tag_fat(_tmp26,sizeof(char),3U);});};}case Cyc_Absyn_BoxKind:  {
# 158
enum Cyc_Absyn_AliasQual _tmp27=a;switch((int)_tmp27){case Cyc_Absyn_Aliasable:
 return({const char*_tmp28="B";_tag_fat(_tmp28,sizeof(char),2U);});case Cyc_Absyn_Unique:
 return({const char*_tmp29="UB";_tag_fat(_tmp29,sizeof(char),3U);});case Cyc_Absyn_Top:
 goto _LL30;default: _LL30: return({const char*_tmp2A="TB";_tag_fat(_tmp2A,sizeof(char),3U);});};}case Cyc_Absyn_RgnKind:  {
# 164
enum Cyc_Absyn_AliasQual _tmp2B=a;switch((int)_tmp2B){case Cyc_Absyn_Aliasable:
 return({const char*_tmp2C="R";_tag_fat(_tmp2C,sizeof(char),2U);});case Cyc_Absyn_Unique:
 return({const char*_tmp2D="UR";_tag_fat(_tmp2D,sizeof(char),3U);});case Cyc_Absyn_Top:
 goto _LL39;default: _LL39: return({const char*_tmp2E="TR";_tag_fat(_tmp2E,sizeof(char),3U);});};}case Cyc_Absyn_EffKind:
# 169
 return({const char*_tmp2F="E";_tag_fat(_tmp2F,sizeof(char),2U);});case Cyc_Absyn_IntKind:
 return({const char*_tmp30="I";_tag_fat(_tmp30,sizeof(char),2U);});case Cyc_Absyn_BoolKind:
 return({const char*_tmp31="BOOL";_tag_fat(_tmp31,sizeof(char),5U);});case Cyc_Absyn_PtrBndKind:
 goto _LL15;default: _LL15: return({const char*_tmp32="PTRBND";_tag_fat(_tmp32,sizeof(char),7U);});};}}
# 176
void*Cyc_Kinds_compress_kb(void*k){
void*_tmp33=k;void*_tmp34;switch(*((int*)_tmp33)){case 0:
 goto _LL4;case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp33)->f1 == 0){_LL4:
 goto _LL6;}else{_tmp34=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp33)->f1)->v;{void**k2=_tmp34;
# 181
_tmp34=k2;goto _LLA;}}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp33)->f1 == 0){_LL6:
# 180
 return k;}else{_tmp34=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp33)->f1)->v;_LLA: {void**k2=(void**)_tmp34;
# 183
({void*_tmp73=Cyc_Kinds_compress_kb(*k2);*k2=_tmp73;});
return*k2;}}};}
# 187
struct Cyc_Absyn_Kind*Cyc_Kinds_force_kb(void*kb){
void*_tmp35=Cyc_Kinds_compress_kb(kb);void*_stmttmp3=_tmp35;void*_tmp36=_stmttmp3;void*_tmp38;void*_tmp37;switch(*((int*)_tmp36)){case 0: _tmp37=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp36)->f1;{struct Cyc_Absyn_Kind*k=_tmp37;
return k;}case 1: _tmp37=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp36)->f1;{struct Cyc_Core_Opt**f=_tmp37;
_tmp37=f;_tmp38=& Cyc_Kinds_bk;goto _LL6;}default: _tmp37=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp36)->f1;_tmp38=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp36)->f2;_LL6: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_tmp37;struct Cyc_Absyn_Kind*k=_tmp38;
# 192
({struct Cyc_Core_Opt*_tmp75=({struct Cyc_Core_Opt*_tmp39=_cycalloc(sizeof(*_tmp39));({void*_tmp74=Cyc_Kinds_kind_to_bound(k);_tmp39->v=_tmp74;});_tmp39;});*f=_tmp75;});
return k;}};}
# 200
void*Cyc_Kinds_copy_kindbound(void*kb){
void*_tmp3A=Cyc_Kinds_compress_kb(kb);void*_stmttmp4=_tmp3A;void*_tmp3B=_stmttmp4;void*_tmp3C;switch(*((int*)_tmp3B)){case 1:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->tag=1,_tmp3D->f1=0;_tmp3D;});case 2: _tmp3C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp3B)->f2;{struct Cyc_Absyn_Kind*k=_tmp3C;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->tag=2,_tmp3E->f1=0,_tmp3E->f2=k;_tmp3E;});}default:
 return kb;};}struct _tuple11{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple12{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 216 "kinds.cyc"
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp3F=ka1;enum Cyc_Absyn_AliasQual _tmp41;enum Cyc_Absyn_KindQual _tmp40;_tmp40=_tmp3F->kind;_tmp41=_tmp3F->aliasqual;{enum Cyc_Absyn_KindQual k1=_tmp40;enum Cyc_Absyn_AliasQual a1=_tmp41;
struct Cyc_Absyn_Kind*_tmp42=ka2;enum Cyc_Absyn_AliasQual _tmp44;enum Cyc_Absyn_KindQual _tmp43;_tmp43=_tmp42->kind;_tmp44=_tmp42->aliasqual;{enum Cyc_Absyn_KindQual k2=_tmp43;enum Cyc_Absyn_AliasQual a2=_tmp44;
# 220
if((int)k1 != (int)k2){
struct _tuple11 _tmp45=({struct _tuple11 _tmp68;_tmp68.f1=k1,_tmp68.f2=k2;_tmp68;});struct _tuple11 _stmttmp5=_tmp45;struct _tuple11 _tmp46=_stmttmp5;switch((int)_tmp46.f1){case Cyc_Absyn_BoxKind: switch((int)_tmp46.f2){case Cyc_Absyn_MemKind:
 goto _LLA;case Cyc_Absyn_AnyKind: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp46.f2 == Cyc_Absyn_AnyKind){_LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD:
 return 0;}_LL6:;}
# 228
if((int)a1 != (int)a2){
struct _tuple12 _tmp47=({struct _tuple12 _tmp69;_tmp69.f1=a1,_tmp69.f2=a2;_tmp69;});struct _tuple12 _stmttmp6=_tmp47;struct _tuple12 _tmp48=_stmttmp6;switch((int)_tmp48.f1){case Cyc_Absyn_Aliasable: if(_tmp48.f2 == Cyc_Absyn_Top)
goto _LL13;else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp48.f2 == Cyc_Absyn_Top){_LL13:
 return 1;}else{goto _LL14;}default: _LL14:
 return 0;};}
# 234
return 1;}}}
# 237
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp49=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp7=_tmp49;void*_tmp4A=_stmttmp7;void*_tmp4B;switch(*((int*)_tmp4A)){case 0: _tmp4B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4A)->f1;{struct Cyc_Absyn_Kind*k=_tmp4B;
return k;}case 2: _tmp4B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4A)->f2;{struct Cyc_Absyn_Kind*k=_tmp4B;
return k;}default:
({void*_tmp76=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=2,_tmp4C->f1=0,_tmp4C->f2=def;_tmp4C;});tv->kind=_tmp76;});return def;};}struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 245
struct _tuple13 Cyc_Kinds_swap_kind(void*t,void*kb){
void*_tmp4D=Cyc_Absyn_compress(t);void*_stmttmp8=_tmp4D;void*_tmp4E=_stmttmp8;void*_tmp4F;if(*((int*)_tmp4E)== 2){_tmp4F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp4F;
# 248
void*_tmp50=tv->kind;void*oldkb=_tmp50;
tv->kind=kb;
return({struct _tuple13 _tmp6A;_tmp6A.f1=tv,_tmp6A.f2=oldkb;_tmp6A;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C;_tmp6C.tag=0,({struct _fat_ptr _tmp77=({const char*_tmp54="swap_kind: cannot update the kind of ";_tag_fat(_tmp54,sizeof(char),38U);});_tmp6C.f1=_tmp77;});_tmp6C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp53=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6B;_tmp6B.tag=2,_tmp6B.f1=(void*)t;_tmp6B;});void*_tmp51[2];_tmp51[0]=& _tmp52,_tmp51[1]=& _tmp53;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_tmp51,sizeof(void*),2));});};}
# 255
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}struct _tuple14{void*f1;void*f2;};
# 262
int Cyc_Kinds_constrain_kinds(void*c1,void*c2){
c1=Cyc_Kinds_compress_kb(c1);
c2=Cyc_Kinds_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple14 _tmp55=({struct _tuple14 _tmp6D;_tmp6D.f1=c1,_tmp6D.f2=c2;_tmp6D;});struct _tuple14 _stmttmp9=_tmp55;struct _tuple14 _tmp56=_stmttmp9;void*_tmp5A;void*_tmp59;void*_tmp58;void*_tmp57;if(*((int*)_tmp56.f1)== 0)switch(*((int*)_tmp56.f2)){case 0: _tmp57=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp56.f1)->f1;_tmp58=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp56.f2)->f1;{struct Cyc_Absyn_Kind*k1=_tmp57;struct Cyc_Absyn_Kind*k2=_tmp58;
return k1 == k2;}case 1: goto _LL3;default: _tmp57=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp56.f1)->f1;_tmp58=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f2)->f1;_tmp59=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f2)->f2;{struct Cyc_Absyn_Kind*k1=_tmp57;struct Cyc_Core_Opt**f=_tmp58;struct Cyc_Absyn_Kind*k2=_tmp59;
# 276
if(!Cyc_Kinds_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_tmp78=({struct Cyc_Core_Opt*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->v=c1;_tmp5E;});*f=_tmp78;});
return 1;}}else{if(*((int*)_tmp56.f2)== 1){_LL3: _tmp57=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp56.f2)->f1;{struct Cyc_Core_Opt**f=_tmp57;
# 268
({struct Cyc_Core_Opt*_tmp79=({struct Cyc_Core_Opt*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->v=c1;_tmp5B;});*f=_tmp79;});return 1;}}else{if(*((int*)_tmp56.f1)== 1){_tmp57=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp56.f1)->f1;{struct Cyc_Core_Opt**f=_tmp57;
({struct Cyc_Core_Opt*_tmp7A=({struct Cyc_Core_Opt*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->v=c2;_tmp5C;});*f=_tmp7A;});return 1;}}else{if(*((int*)_tmp56.f2)== 0){_tmp57=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f1)->f1;_tmp58=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f1)->f2;_tmp59=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp56.f2)->f1;{struct Cyc_Core_Opt**f=_tmp57;struct Cyc_Absyn_Kind*k1=_tmp58;struct Cyc_Absyn_Kind*k2=_tmp59;
# 271
if(!Cyc_Kinds_kind_leq(k2,k1))
return 0;
({struct Cyc_Core_Opt*_tmp7B=({struct Cyc_Core_Opt*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->v=c2;_tmp5D;});*f=_tmp7B;});
return 1;}}else{_tmp57=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f1)->f1;_tmp58=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f1)->f2;_tmp59=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f2)->f1;_tmp5A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp56.f2)->f2;{struct Cyc_Core_Opt**f1=(struct Cyc_Core_Opt**)_tmp57;struct Cyc_Absyn_Kind*k1=_tmp58;struct Cyc_Core_Opt**f2=(struct Cyc_Core_Opt**)_tmp59;struct Cyc_Absyn_Kind*k2=_tmp5A;
# 281
if(Cyc_Kinds_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp7C=({struct Cyc_Core_Opt*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->v=c1;_tmp5F;});*f2=_tmp7C;});
return 1;}
# 285
if(Cyc_Kinds_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp7D=({struct Cyc_Core_Opt*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->v=c2;_tmp60;});*f1=_tmp7D;});
return 1;}
# 289
return 0;}}}}};}}

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

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Regions */
struct _RegionPage
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  /* MWH: wish we didn't have to include the stuff below ... */
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
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
void   _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void   _pop_dynregion();

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

struct _xtunion_struct* Cyc_Core_get_exn_thrown();
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
#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#else
#define _INLINE inline
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_cks_null = (void*)(ptr); \
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

static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
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

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 50 "string.h"
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 491
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 493
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 904 "absyn.h"
extern void*Cyc_Absyn_uint_type;
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
# 107
void*Cyc_Tcutil_compress(void*);
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 182
int Cyc_Tcutil_typecmp(void*,void*);
# 187
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple10{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{union Cyc_Absyn_Cnst f1;int f2;};
# 35 "evexp.cyc"
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 37
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;short _tmp2;enum Cyc_Absyn_Sign _tmp1;char _tmp4;enum Cyc_Absyn_Sign _tmp3;switch((_tmp0.Short_c).tag){case 2U: _LL1: _tmp3=((_tmp0.Char_c).val).f1;_tmp4=((_tmp0.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sn=_tmp3;char c=_tmp4;
return Cyc_Absyn_Int_c(sn,(int)c);}case 4U: _LL3: _tmp1=((_tmp0.Short_c).val).f1;_tmp2=((_tmp0.Short_c).val).f2;_LL4: {enum Cyc_Absyn_Sign sn=_tmp1;short s=_tmp2;
# 41
return Cyc_Absyn_Int_c(sn,(int)s);}default: _LL5: _LL6:
 return cn;}_LL0:;}
# 46
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp5=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _stmttmp0=_tmp5;struct _tuple11 _tmp6=_stmttmp0;int _tmp8;union Cyc_Absyn_Cnst _tmp7;_LL1: _tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmp7;int known=_tmp8;
if(!known)
return({struct _tuple10 _tmp114;_tmp114.f1=0U,_tmp114.f2=0;_tmp114;});{
union Cyc_Absyn_Cnst _tmp9=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp1=_tmp9;union Cyc_Absyn_Cnst _tmpA=_stmttmp1;long long _tmpB;int _tmpC;switch((_tmpA.Null_c).tag){case 5U: _LL4: _tmpC=((_tmpA.Int_c).val).f2;_LL5: {int i=_tmpC;
return({struct _tuple10 _tmp115;_tmp115.f1=(unsigned)i,_tmp115.f2=1;_tmp115;});}case 6U: _LL6: _tmpB=((_tmpA.LongLong_c).val).f2;_LL7: {long long x=_tmpB;
# 54
unsigned long long y=(unsigned long long)x >> (unsigned long long)32;
if(y != (unsigned long long)-1 && y != (unsigned long long)0)
return({struct _tuple10 _tmp116;_tmp116.f1=0U,_tmp116.f2=0;_tmp116;});else{
return({struct _tuple10 _tmp117;_tmp117.f1=(unsigned)x,_tmp117.f2=1;_tmp117;});}}case 7U: _LL8: _LL9:
 return({struct _tuple10 _tmp118;_tmp118.f1=0U,_tmp118.f2=0;_tmp118;});case 1U: _LLA: _LLB:
 return({struct _tuple10 _tmp119;_tmp119.f1=0U,_tmp119.f2=1;_tmp119;});default: _LLC: _LLD:
# 61
({struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp11A;_tmp11A.tag=0U,({struct _fat_ptr _tmp141=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp11A.f1=_tmp141;});_tmp11A;});void*_tmpD[1U];_tmpD[0]=& _tmpF;({unsigned _tmp143=e->loc;struct _fat_ptr _tmp142=({const char*_tmpE="expecting unsigned int found %s";_tag_fat(_tmpE,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp143,_tmp142,_tag_fat(_tmpD,sizeof(void*),1U));});});return({struct _tuple10 _tmp11B;_tmp11B.f1=0U,_tmp11B.f2=1;_tmp11B;});}_LL3:;}}}struct _tuple12{int f1;int f2;};
# 65
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp10=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _stmttmp2=_tmp10;struct _tuple11 _tmp11=_stmttmp2;int _tmp13;union Cyc_Absyn_Cnst _tmp12;_LL1: _tmp12=_tmp11.f1;_tmp13=_tmp11.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmp12;int known=_tmp13;
if(!known)
return({struct _tuple12 _tmp11C;_tmp11C.f1=0,_tmp11C.f2=0;_tmp11C;});{
union Cyc_Absyn_Cnst _tmp14=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp3=_tmp14;union Cyc_Absyn_Cnst _tmp15=_stmttmp3;long long _tmp16;int _tmp17;switch((_tmp15.Float_c).tag){case 5U: _LL4: _tmp17=((_tmp15.Int_c).val).f2;_LL5: {int b=_tmp17;
return({struct _tuple12 _tmp11D;_tmp11D.f1=b != 0,_tmp11D.f2=1;_tmp11D;});}case 6U: _LL6: _tmp16=((_tmp15.LongLong_c).val).f2;_LL7: {long long b=_tmp16;
return({struct _tuple12 _tmp11E;_tmp11E.f1=b != (long long)0,_tmp11E.f2=1;_tmp11E;});}case 1U: _LL8: _LL9:
 return({struct _tuple12 _tmp11F;_tmp11F.f1=0,_tmp11F.f2=0;_tmp11F;});case 7U: _LLA: _LLB:
 return({struct _tuple12 _tmp120;_tmp120.f1=0,_tmp120.f2=1;_tmp120;});default: _LLC: _LLD:
({void*_tmp18=0U;({unsigned _tmp145=e->loc;struct _fat_ptr _tmp144=({const char*_tmp19="expecting bool";_tag_fat(_tmp19,sizeof(char),15U);});Cyc_Tcutil_terr(_tmp145,_tmp144,_tag_fat(_tmp18,sizeof(void*),0U));});});return({struct _tuple12 _tmp121;_tmp121.f1=0,_tmp121.f2=0;_tmp121;});}_LL3:;}}}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 78
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp1A=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _stmttmp4=_tmp1A;struct _tuple11 _tmp1B=_stmttmp4;int _tmp1D;union Cyc_Absyn_Cnst _tmp1C;_LL1: _tmp1C=_tmp1B.f1;_tmp1D=_tmp1B.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmp1C;int known=_tmp1D;
if(!known)
return({struct _tuple11 _tmp122;_tmp122.f1=cn,_tmp122.f2=0;_tmp122;});
{struct _tuple13 _tmp1E=({struct _tuple13 _tmp124;_tmp124.f1=p,_tmp124.f2=cn;_tmp124;});struct _tuple13 _stmttmp5=_tmp1E;struct _tuple13 _tmp1F=_stmttmp5;long long _tmp20;char _tmp21;short _tmp22;int _tmp23;long long _tmp25;enum Cyc_Absyn_Sign _tmp24;char _tmp27;enum Cyc_Absyn_Sign _tmp26;short _tmp29;enum Cyc_Absyn_Sign _tmp28;int _tmp2B;enum Cyc_Absyn_Sign _tmp2A;long long _tmp2D;enum Cyc_Absyn_Sign _tmp2C;char _tmp2F;enum Cyc_Absyn_Sign _tmp2E;short _tmp31;enum Cyc_Absyn_Sign _tmp30;int _tmp33;enum Cyc_Absyn_Sign _tmp32;switch(_tmp1F.f1){case Cyc_Absyn_Plus: _LL4: _LL5:
 goto _LL3;case Cyc_Absyn_Minus: switch(((_tmp1F.f2).LongLong_c).tag){case 5U: _LL6: _tmp32=(((_tmp1F.f2).Int_c).val).f1;_tmp33=(((_tmp1F.f2).Int_c).val).f2;_LL7: {enum Cyc_Absyn_Sign s=_tmp32;int i=_tmp33;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 4U: _LL8: _tmp30=(((_tmp1F.f2).Short_c).val).f1;_tmp31=(((_tmp1F.f2).Short_c).val).f2;_LL9: {enum Cyc_Absyn_Sign s=_tmp30;short i=_tmp31;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 2U: _LLA: _tmp2E=(((_tmp1F.f2).Char_c).val).f1;_tmp2F=(((_tmp1F.f2).Char_c).val).f2;_LLB: {enum Cyc_Absyn_Sign s=_tmp2E;char i=_tmp2F;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 6U: _LLC: _tmp2C=(((_tmp1F.f2).LongLong_c).val).f1;_tmp2D=(((_tmp1F.f2).LongLong_c).val).f2;_LLD: {enum Cyc_Absyn_Sign s=_tmp2C;long long i=_tmp2D;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,- i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Bitnot: switch(((_tmp1F.f2).LongLong_c).tag){case 5U: _LLE: _tmp2A=(((_tmp1F.f2).Int_c).val).f1;_tmp2B=(((_tmp1F.f2).Int_c).val).f2;_LLF: {enum Cyc_Absyn_Sign s=_tmp2A;int i=_tmp2B;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 4U: _LL10: _tmp28=(((_tmp1F.f2).Short_c).val).f1;_tmp29=(((_tmp1F.f2).Short_c).val).f2;_LL11: {enum Cyc_Absyn_Sign s=_tmp28;short i=_tmp29;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 2U: _LL12: _tmp26=(((_tmp1F.f2).Char_c).val).f1;_tmp27=(((_tmp1F.f2).Char_c).val).f2;_LL13: {enum Cyc_Absyn_Sign s=_tmp26;char i=_tmp27;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 6U: _LL14: _tmp24=(((_tmp1F.f2).LongLong_c).val).f1;_tmp25=(((_tmp1F.f2).LongLong_c).val).f2;_LL15: {enum Cyc_Absyn_Sign s=_tmp24;long long i=_tmp25;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Not: switch(((_tmp1F.f2).Null_c).tag){case 5U: _LL16: _tmp23=(((_tmp1F.f2).Int_c).val).f2;_LL17: {int i=_tmp23;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i == 0?1: 0);goto _LL3;}case 4U: _LL18: _tmp22=(((_tmp1F.f2).Short_c).val).f2;_LL19: {short i=_tmp22;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,(int)i == 0?1: 0);goto _LL3;}case 2U: _LL1A: _tmp21=(((_tmp1F.f2).Char_c).val).f2;_LL1B: {char i=_tmp21;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,(int)i == 0?'\001':'\000');goto _LL3;}case 6U: _LL1C: _tmp20=(((_tmp1F.f2).LongLong_c).val).f2;_LL1D: {long long i=_tmp20;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,(long long)(i == (long long)0?1: 0));goto _LL3;}case 1U: _LL1E: _LL1F:
 cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL3;default: goto _LL20;}default: _LL20: _LL21:
 return({struct _tuple11 _tmp123;_tmp123.f1=cn,_tmp123.f2=0;_tmp123;});}_LL3:;}
# 99
return({struct _tuple11 _tmp125;_tmp125.f1=cn,_tmp125.f2=1;_tmp125;});}}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 103
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp34=Cyc_Evexp_eval_const_exp(e1);struct _tuple11 _stmttmp6=_tmp34;struct _tuple11 _tmp35=_stmttmp6;int _tmp37;union Cyc_Absyn_Cnst _tmp36;_LL1: _tmp36=_tmp35.f1;_tmp37=_tmp35.f2;_LL2: {union Cyc_Absyn_Cnst cn1=_tmp36;int known1=_tmp37;
struct _tuple11 _tmp38=Cyc_Evexp_eval_const_exp(e2);struct _tuple11 _stmttmp7=_tmp38;struct _tuple11 _tmp39=_stmttmp7;int _tmp3B;union Cyc_Absyn_Cnst _tmp3A;_LL4: _tmp3A=_tmp39.f1;_tmp3B=_tmp39.f2;_LL5: {union Cyc_Absyn_Cnst cn2=_tmp3A;int known2=_tmp3B;
if(!known1 || !known2)
return({struct _tuple11 _tmp126;_tmp126.f1=cn1,_tmp126.f2=0;_tmp126;});
cn1=Cyc_Evexp_promote_const(cn1);
cn2=Cyc_Evexp_promote_const(cn2);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp3C=cn1;int _tmp3E;enum Cyc_Absyn_Sign _tmp3D;if((_tmp3C.Int_c).tag == 5){_LL7: _tmp3D=((_tmp3C.Int_c).val).f1;_tmp3E=((_tmp3C.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign x=_tmp3D;int y=_tmp3E;
s1=x;i1=y;goto _LL6;}}else{_LL9: _LLA:
 return({struct _tuple11 _tmp127;_tmp127.f1=cn1,_tmp127.f2=0;_tmp127;});}_LL6:;}
# 116
{union Cyc_Absyn_Cnst _tmp3F=cn2;int _tmp41;enum Cyc_Absyn_Sign _tmp40;if((_tmp3F.Int_c).tag == 5){_LLC: _tmp40=((_tmp3F.Int_c).val).f1;_tmp41=((_tmp3F.Int_c).val).f2;_LLD: {enum Cyc_Absyn_Sign x=_tmp40;int y=_tmp41;
s2=x;i2=y;goto _LLB;}}else{_LLE: _LLF:
 return({struct _tuple11 _tmp128;_tmp128.f1=cn1,_tmp128.f2=0;_tmp128;});}_LLB:;}
# 120
{enum Cyc_Absyn_Primop _tmp42=p;switch(_tmp42){case Cyc_Absyn_Div: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Mod: _LL13: _LL14:
# 123
 if(i2 == 0){
({void*_tmp43=0U;({unsigned _tmp147=e2->loc;struct _fat_ptr _tmp146=({const char*_tmp44="division by zero in constant expression";_tag_fat(_tmp44,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp147,_tmp146,_tag_fat(_tmp43,sizeof(void*),0U));});});
return({struct _tuple11 _tmp129;_tmp129.f1=cn1,_tmp129.f2=1;_tmp129;});}
# 127
goto _LL10;default: _LL15: _LL16:
 goto _LL10;}_LL10:;}{
# 130
int has_u_arg=(int)s1 == (int)1U ||(int)s2 == (int)Cyc_Absyn_Unsigned;
unsigned u1=(unsigned)i1;
unsigned u2=(unsigned)i2;
int i3=0;
unsigned u3=0U;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple14 _tmp45=({struct _tuple14 _tmp12B;_tmp12B.f1=p,_tmp12B.f2=has_u_arg;_tmp12B;});struct _tuple14 _stmttmp8=_tmp45;struct _tuple14 _tmp46=_stmttmp8;switch(_tmp46.f1){case Cyc_Absyn_Plus: switch(_tmp46.f2){case 0U: _LL18: _LL19:
 i3=i1 + i2;use_i3=1;goto _LL17;case 1U: _LL22: _LL23:
# 145
 u3=u1 + u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Times: switch(_tmp46.f2){case 0U: _LL1A: _LL1B:
# 141
 i3=i1 * i2;use_i3=1;goto _LL17;case 1U: _LL24: _LL25:
# 146
 u3=u1 * u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Minus: switch(_tmp46.f2){case 0U: _LL1C: _LL1D:
# 142
 i3=i1 - i2;use_i3=1;goto _LL17;case 1U: _LL26: _LL27:
# 147
 u3=u1 - u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Div: switch(_tmp46.f2){case 0U: _LL1E: _LL1F:
# 143
 i3=i1 / i2;use_i3=1;goto _LL17;case 1U: _LL28: _LL29:
# 148
 u3=u1 / u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Mod: switch(_tmp46.f2){case 0U: _LL20: _LL21:
# 144
 i3=i1 % i2;use_i3=1;goto _LL17;case 1U: _LL2A: _LL2B:
# 149
 u3=u1 % u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Eq: _LL2C: _LL2D:
 b3=i1 == i2;use_b3=1;goto _LL17;case Cyc_Absyn_Neq: _LL2E: _LL2F:
 b3=i1 != i2;use_b3=1;goto _LL17;case Cyc_Absyn_Gt: switch(_tmp46.f2){case 0U: _LL30: _LL31:
 b3=i1 > i2;use_b3=1;goto _LL17;case 1U: _LL38: _LL39:
# 156
 b3=u1 > u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lt: switch(_tmp46.f2){case 0U: _LL32: _LL33:
# 153
 b3=i1 < i2;use_b3=1;goto _LL17;case 1U: _LL3A: _LL3B:
# 157
 b3=u1 < u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Gte: switch(_tmp46.f2){case 0U: _LL34: _LL35:
# 154
 b3=i1 >= i2;use_b3=1;goto _LL17;case 1U: _LL3C: _LL3D:
# 158
 b3=u1 >= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lte: switch(_tmp46.f2){case 0U: _LL36: _LL37:
# 155
 b3=i1 <= i2;use_b3=1;goto _LL17;case 1U: _LL3E: _LL3F:
# 159
 b3=u1 <= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Bitand: _LL40: _LL41:
 u3=u1 & u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitor: _LL42: _LL43:
 u3=u1 | u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitxor: _LL44: _LL45:
 u3=u1 ^ u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlshift: _LL46: _LL47:
 u3=u1 << u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlrshift: _LL48: _LL49:
 u3=u1 >> u2;use_u3=1;goto _LL17;default: _LL4A: _LL4B:
({void*_tmp47=0U;({unsigned _tmp149=e1->loc;struct _fat_ptr _tmp148=({const char*_tmp48="bad constant expression";_tag_fat(_tmp48,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp149,_tmp148,_tag_fat(_tmp47,sizeof(void*),0U));});});return({struct _tuple11 _tmp12A;_tmp12A.f1=cn1,_tmp12A.f2=1;_tmp12A;});}_LL17:;}
# 167
if(use_i3)return({struct _tuple11 _tmp12C;({union Cyc_Absyn_Cnst _tmp14A=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3);_tmp12C.f1=_tmp14A;}),_tmp12C.f2=1;_tmp12C;});
if(use_u3)return({struct _tuple11 _tmp12D;({union Cyc_Absyn_Cnst _tmp14B=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3);_tmp12D.f1=_tmp14B;}),_tmp12D.f2=1;_tmp12D;});
if(use_b3)return({struct _tuple11 _tmp12E;({union Cyc_Absyn_Cnst _tmp14C=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0);_tmp12E.f1=_tmp14C;}),_tmp12E.f2=1;_tmp12E;});
({void*_tmp49=0U;({struct _fat_ptr _tmp14D=({const char*_tmp4A="Evexp::eval_const_binop";_tag_fat(_tmp4A,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp14D,_tag_fat(_tmp49,sizeof(void*),0U));});});}}}}}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 176
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 ans;
{void*_tmp4B=e->r;void*_stmttmp9=_tmp4B;void*_tmp4C=_stmttmp9;struct Cyc_Absyn_Enumfield*_tmp4D;struct Cyc_Absyn_Enumfield*_tmp4E;struct Cyc_Absyn_Exp*_tmp50;void*_tmp4F;void*_tmp51;struct Cyc_List_List*_tmp53;enum Cyc_Absyn_Primop _tmp52;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Exp*_tmp59;struct Cyc_Absyn_Exp*_tmp58;union Cyc_Absyn_Cnst _tmp5B;switch(*((int*)_tmp4C)){case 1U: _LL1: _LL2:
# 180
 return({struct _tuple11 _tmp12F;({union Cyc_Absyn_Cnst _tmp14E=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp12F.f1=_tmp14E;}),_tmp12F.f2=0;_tmp12F;});case 0U: _LL3: _tmp5B=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_LL4: {union Cyc_Absyn_Cnst c=_tmp5B;
return({struct _tuple11 _tmp130;_tmp130.f1=c,_tmp130.f2=1;_tmp130;});}case 2U: _LL5: _LL6:
 return({struct _tuple11 _tmp131;({union Cyc_Absyn_Cnst _tmp14F=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp131.f1=_tmp14F;}),_tmp131.f2=1;_tmp131;});case 6U: _LL7: _tmp58=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_tmp59=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_tmp5A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp58;struct Cyc_Absyn_Exp*e2=_tmp59;struct Cyc_Absyn_Exp*e3=_tmp5A;
# 184
struct _tuple12 _tmp5C=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple12 _stmttmpA=_tmp5C;struct _tuple12 _tmp5D=_stmttmpA;int _tmp5F;int _tmp5E;_LL20: _tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;_LL21: {int bool1=_tmp5E;int known1=_tmp5F;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
Cyc_Evexp_eval_const_exp(e3);
return({struct _tuple11 _tmp132;({union Cyc_Absyn_Cnst _tmp150=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp132.f1=_tmp150;}),_tmp132.f2=0;_tmp132;});}
# 190
ans=bool1?Cyc_Evexp_eval_const_exp(e2): Cyc_Evexp_eval_const_exp(e3);
goto _LL0;}}case 7U: _LL9: _tmp56=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_tmp57=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp56;struct Cyc_Absyn_Exp*e2=_tmp57;
# 193
struct _tuple12 _tmp60=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple12 _stmttmpB=_tmp60;struct _tuple12 _tmp61=_stmttmpB;int _tmp63;int _tmp62;_LL23: _tmp62=_tmp61.f1;_tmp63=_tmp61.f2;_LL24: {int bool1=_tmp62;int known1=_tmp63;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple11 _tmp133;({union Cyc_Absyn_Cnst _tmp151=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp133.f1=_tmp151;}),_tmp133.f2=0;_tmp133;});}
# 198
if(bool1)ans=Cyc_Evexp_eval_const_exp(e2);else{
ans=({struct _tuple11 _tmp134;({union Cyc_Absyn_Cnst _tmp152=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp134.f1=_tmp152;}),_tmp134.f2=1;_tmp134;});}
goto _LL0;}}case 8U: _LLB: _tmp54=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_tmp55=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp54;struct Cyc_Absyn_Exp*e2=_tmp55;
# 202
struct _tuple12 _tmp64=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple12 _stmttmpC=_tmp64;struct _tuple12 _tmp65=_stmttmpC;int _tmp67;int _tmp66;_LL26: _tmp66=_tmp65.f1;_tmp67=_tmp65.f2;_LL27: {int bool1=_tmp66;int known1=_tmp67;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple11 _tmp135;({union Cyc_Absyn_Cnst _tmp153=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp135.f1=_tmp153;}),_tmp135.f2=0;_tmp135;});}
# 207
if(!bool1)ans=Cyc_Evexp_eval_const_exp(e2);else{
ans=({struct _tuple11 _tmp136;({union Cyc_Absyn_Cnst _tmp154=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);_tmp136.f1=_tmp154;}),_tmp136.f2=1;_tmp136;});}
goto _LL0;}}case 3U: _LLD: _tmp52=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_tmp53=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp52;struct Cyc_List_List*es=_tmp53;
# 211
if(es == 0){
({void*_tmp68=0U;({unsigned _tmp156=e->loc;struct _fat_ptr _tmp155=({const char*_tmp69="bad static expression (no args to primop)";_tag_fat(_tmp69,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp156,_tmp155,_tag_fat(_tmp68,sizeof(void*),0U));});});
return({struct _tuple11 _tmp137;({union Cyc_Absyn_Cnst _tmp157=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp137.f1=_tmp157;}),_tmp137.f2=1;_tmp137;});}
# 215
if(es->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(p,(struct Cyc_Absyn_Exp*)es->hd);
goto _LL0;}
# 219
if(((struct Cyc_List_List*)_check_null(es->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(p,(struct Cyc_Absyn_Exp*)es->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);
goto _LL0;}
# 223
({void*_tmp6A=0U;({unsigned _tmp159=e->loc;struct _fat_ptr _tmp158=({const char*_tmp6B="bad static expression (too many args to primop)";_tag_fat(_tmp6B,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp159,_tmp158,_tag_fat(_tmp6A,sizeof(void*),0U));});});
return({struct _tuple11 _tmp138;({union Cyc_Absyn_Cnst _tmp15A=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp138.f1=_tmp15A;}),_tmp138.f2=1;_tmp138;});}case 39U: _LLF: _tmp51=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_LL10: {void*t=_tmp51;
# 227
{void*_tmp6C=Cyc_Tcutil_compress(t);void*_stmttmpD=_tmp6C;void*_tmp6D=_stmttmpD;struct Cyc_Absyn_Exp*_tmp6E;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->tag == 9U){_LL29: _tmp6E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL2A: {struct Cyc_Absyn_Exp*e2=_tmp6E;
# 229
e->r=e2->r;
return Cyc_Evexp_eval_const_exp(e2);}}else{_LL2B: _LL2C:
 goto _LL28;}_LL28:;}
# 233
goto _LL12;}case 17U: _LL11: _LL12:
 goto _LL14;case 18U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 ans=({struct _tuple11 _tmp139;({union Cyc_Absyn_Cnst _tmp15B=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp139.f1=_tmp15B;}),_tmp139.f2=0;_tmp139;});goto _LL0;case 14U: _LL17: _tmp4F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C)->f1;_tmp50=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LL18: {void*t=_tmp4F;struct Cyc_Absyn_Exp*e2=_tmp50;
# 239
ans=Cyc_Evexp_eval_const_exp(e2);
if(ans.f2){
struct _tuple15 _tmp6F=({struct _tuple15 _tmp13B;({void*_tmp15C=Cyc_Tcutil_compress(t);_tmp13B.f1=_tmp15C;}),_tmp13B.f2=ans.f1;_tmp13B;});struct _tuple15 _stmttmpE=_tmp6F;struct _tuple15 _tmp70=_stmttmpE;int _tmp73;enum Cyc_Absyn_Sign _tmp72;void*_tmp71;short _tmp76;enum Cyc_Absyn_Sign _tmp75;void*_tmp74;char _tmp79;enum Cyc_Absyn_Sign _tmp78;void*_tmp77;int _tmp7D;enum Cyc_Absyn_Sign _tmp7C;enum Cyc_Absyn_Size_of _tmp7B;enum Cyc_Absyn_Sign _tmp7A;short _tmp81;enum Cyc_Absyn_Sign _tmp80;enum Cyc_Absyn_Size_of _tmp7F;enum Cyc_Absyn_Sign _tmp7E;char _tmp85;enum Cyc_Absyn_Sign _tmp84;enum Cyc_Absyn_Size_of _tmp83;enum Cyc_Absyn_Sign _tmp82;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)){case 1U: switch(((_tmp70.f2).Int_c).tag){case 2U: _LL2E: _tmp82=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f1;_tmp83=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f2;_tmp84=(((_tmp70.f2).Char_c).val).f1;_tmp85=(((_tmp70.f2).Char_c).val).f2;_LL2F: {enum Cyc_Absyn_Sign sn=_tmp82;enum Cyc_Absyn_Size_of sz=_tmp83;enum Cyc_Absyn_Sign sn2=_tmp84;char x=_tmp85;
# 244
_tmp7E=sn;_tmp7F=sz;_tmp80=sn2;_tmp81=(short)x;goto _LL31;}case 4U: _LL30: _tmp7E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f1;_tmp7F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f2;_tmp80=(((_tmp70.f2).Short_c).val).f1;_tmp81=(((_tmp70.f2).Short_c).val).f2;_LL31: {enum Cyc_Absyn_Sign sn=_tmp7E;enum Cyc_Absyn_Size_of sz=_tmp7F;enum Cyc_Absyn_Sign sn2=_tmp80;short x=_tmp81;
# 246
_tmp7A=sn;_tmp7B=sz;_tmp7C=sn2;_tmp7D=(int)x;goto _LL33;}case 5U: _LL32: _tmp7A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f1;_tmp7B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f1)->f2;_tmp7C=(((_tmp70.f2).Int_c).val).f1;_tmp7D=(((_tmp70.f2).Int_c).val).f2;_LL33: {enum Cyc_Absyn_Sign sn=_tmp7A;enum Cyc_Absyn_Size_of sz=_tmp7B;enum Cyc_Absyn_Sign sn2=_tmp7C;int x=_tmp7D;
# 248
if((int)sn != (int)sn2)
({union Cyc_Absyn_Cnst _tmp15D=Cyc_Absyn_Int_c(sn,x);ans.f1=_tmp15D;});
goto _LL2D;}default: goto _LL3A;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2 != 0)switch(((_tmp70.f2).Int_c).tag){case 2U: _LL34: _tmp77=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2)->hd;_tmp78=(((_tmp70.f2).Char_c).val).f1;_tmp79=(((_tmp70.f2).Char_c).val).f2;_LL35: {void*it=_tmp77;enum Cyc_Absyn_Sign sn2=_tmp78;char x=_tmp79;
# 253
_tmp74=it;_tmp75=sn2;_tmp76=(short)x;goto _LL37;}case 4U: _LL36: _tmp74=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2)->hd;_tmp75=(((_tmp70.f2).Short_c).val).f1;_tmp76=(((_tmp70.f2).Short_c).val).f2;_LL37: {void*it=_tmp74;enum Cyc_Absyn_Sign sn2=_tmp75;short x=_tmp76;
# 255
_tmp71=it;_tmp72=sn2;_tmp73=(int)x;goto _LL39;}case 5U: _LL38: _tmp71=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70.f1)->f2)->hd;_tmp72=(((_tmp70.f2).Int_c).val).f1;_tmp73=(((_tmp70.f2).Int_c).val).f2;_LL39: {void*it=_tmp71;enum Cyc_Absyn_Sign sn2=_tmp72;int x=_tmp73;
# 257
if(x < 0)
({void*_tmp86=0U;({unsigned _tmp15F=e->loc;struct _fat_ptr _tmp15E=({const char*_tmp87="cannot cast negative number to a tag type";_tag_fat(_tmp87,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp15F,_tmp15E,_tag_fat(_tmp86,sizeof(void*),0U));});});
({void*_tmp161=it;Cyc_Unify_unify(_tmp161,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=9U,({struct Cyc_Absyn_Exp*_tmp160=Cyc_Absyn_const_exp(ans.f1,0U);_tmp88->f1=_tmp160;});_tmp88;}));});
({union Cyc_Absyn_Cnst _tmp162=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,x);ans.f1=_tmp162;});
goto _LL2D;}default: goto _LL3A;}else{goto _LL3A;}default: goto _LL3A;}else{_LL3A: _LL3B:
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp13A;_tmp13A.tag=0U,({struct _fat_ptr _tmp163=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp13A.f1=_tmp163;});_tmp13A;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({unsigned _tmp165=e->loc;struct _fat_ptr _tmp164=({const char*_tmp8A="cannot cast to %s";_tag_fat(_tmp8A,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp165,_tmp164,_tag_fat(_tmp89,sizeof(void*),1U));});});goto _LL2D;}_LL2D:;}
# 264
goto _LL0;}case 33U: _LL19: _tmp4E=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LL1A: {struct Cyc_Absyn_Enumfield*ef=_tmp4E;
_tmp4D=ef;goto _LL1C;}case 32U: _LL1B: _tmp4D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_tmp4D;
ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));goto _LL0;}default: _LL1D: _LL1E:
# 269
 return({struct _tuple11 _tmp13C;({union Cyc_Absyn_Cnst _tmp166=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp13C.f1=_tmp166;}),_tmp13C.f2=0;_tmp13C;});}_LL0:;}
# 271
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmp8C=ans.f1;{union Cyc_Absyn_Cnst c=_tmp8C;
({void*_tmp167=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=0U,_tmp8D->f1=c;_tmp8D;});e->r=_tmp167;});}}
# 276
return ans;}
# 279
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmp8E=e->r;void*_stmttmpF=_tmp8E;void*_tmp8F=_stmttmpF;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_List_List*_tmp92;enum Cyc_Absyn_Primop _tmp91;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp97;switch(*((int*)_tmp8F)){case 33U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 0U: _LLB: _LLC:
 return 1;case 6U: _LLD: _tmp97=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F)->f1;_tmp98=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F)->f2;_tmp99=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp97;struct Cyc_Absyn_Exp*e2=_tmp98;struct Cyc_Absyn_Exp*e3=_tmp99;
# 288
return(Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2))&& Cyc_Evexp_c_can_eval(e3);}case 7U: _LLF: _tmp95=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8F)->f1;_tmp96=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8F)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp95;struct Cyc_Absyn_Exp*e2=_tmp96;
_tmp93=e1;_tmp94=e2;goto _LL12;}case 8U: _LL11: _tmp93=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8F)->f1;_tmp94=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8F)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp93;struct Cyc_Absyn_Exp*e2=_tmp94;
# 291
return Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2);}case 3U: _LL13: _tmp91=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8F)->f1;_tmp92=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8F)->f2;_LL14: {enum Cyc_Absyn_Primop p=_tmp91;struct Cyc_List_List*es=_tmp92;
# 293
for(0;es != 0;es=es->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)es->hd))return 0;}
return 1;}case 39U: _LL15: _LL16:
 return 0;case 14U: _LL17: _tmp90=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8F)->f2;_LL18: {struct Cyc_Absyn_Exp*e=_tmp90;
return Cyc_Evexp_c_can_eval(e);}default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 302
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmp9A=e->r;void*_stmttmp10=_tmp9A;void*_tmp9B=_stmttmp10;switch(*((int*)_tmp9B)){case 0U: _LL1: _LL2:
 return 1;case 6U: _LL3: _LL4:
 return 2;case 3U: _LL5: _LL6:
 return 3;case 17U: _LL7: _LL8:
 goto _LLA;case 18U: _LL9: _LLA:
 return 4;case 19U: _LLB: _LLC:
 return 5;case 14U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 39U: _LL13: _LL14:
 return 9;default: _LL15: _LL16:
# 315
({struct Cyc_String_pa_PrintArg_struct _tmp9E=({struct Cyc_String_pa_PrintArg_struct _tmp13D;_tmp13D.tag=0U,({struct _fat_ptr _tmp168=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp13D.f1=_tmp168;});_tmp13D;});void*_tmp9C[1U];_tmp9C[0]=& _tmp9E;({unsigned _tmp16A=e->loc;struct _fat_ptr _tmp169=({const char*_tmp9D="bad static expression %s";_tag_fat(_tmp9D,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp16A,_tmp169,_tag_fat(_tmp9C,sizeof(void*),1U));});});return 0;}_LL0:;}
# 320
static struct Cyc_Absyn_Exp*Cyc_Evexp_strip_cast(struct Cyc_Absyn_Exp*e){
{void*_tmp9F=e->r;void*_stmttmp11=_tmp9F;void*_tmpA0=_stmttmp11;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA1;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA0)->tag == 14U){_LL1: _tmpA1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA0)->f1;_tmpA2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA0)->f2;_LL2: {void*t=_tmpA1;struct Cyc_Absyn_Exp*e2=_tmpA2;
# 323
if(Cyc_Unify_unify(t,Cyc_Absyn_uint_type)){
void*_tmpA3=e2->r;void*_stmttmp12=_tmpA3;void*_tmpA4=_stmttmp12;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA4)->tag == 39U){_LL6: _LL7:
 return e2;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 329
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 332
return e;}struct _tuple16{void*f1;void*f2;};
# 335
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 337
e1=Cyc_Evexp_strip_cast(e1);
e2=Cyc_Evexp_strip_cast(e2);{
struct _tuple10 _tmpA5=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple10 _stmttmp13=_tmpA5;struct _tuple10 _tmpA6=_stmttmp13;int _tmpA8;unsigned _tmpA7;_LL1: _tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL2: {unsigned i1=_tmpA7;int known1=_tmpA8;
struct _tuple10 _tmpA9=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple10 _stmttmp14=_tmpA9;struct _tuple10 _tmpAA=_stmttmp14;int _tmpAC;unsigned _tmpAB;_LL4: _tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;_LL5: {unsigned i2=_tmpAB;int known2=_tmpAC;
if(known1 && known2)
return(int)(i1 - i2);{
int _tmpAD=Cyc_Evexp_const_exp_case_number(e1);int e1case=_tmpAD;
int _tmpAE=Cyc_Evexp_const_exp_case_number(e2);int e2case=_tmpAE;
if(e1case != e2case)
return e1case - e2case;{
struct _tuple16 _tmpAF=({struct _tuple16 _tmp13F;_tmp13F.f1=e1->r,_tmp13F.f2=e2->r;_tmp13F;});struct _tuple16 _stmttmp15=_tmpAF;struct _tuple16 _tmpB0=_stmttmp15;void*_tmpB2;void*_tmpB1;struct Cyc_Absyn_Exp*_tmpB6;void*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;void*_tmpB3;struct Cyc_List_List*_tmpBA;void*_tmpB9;struct Cyc_List_List*_tmpB8;void*_tmpB7;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBB;void*_tmpBE;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpC0;void*_tmpBF;void*_tmpC2;void*_tmpC1;struct Cyc_List_List*_tmpC6;enum Cyc_Absyn_Primop _tmpC5;struct Cyc_List_List*_tmpC4;enum Cyc_Absyn_Primop _tmpC3;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpCF;switch(*((int*)_tmpB0.f1)){case 6U: if(((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->tag == 6U){_LL7: _tmpCF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpD0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f2;_tmpD1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f3;_tmpD2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_tmpD3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f2;_tmpD4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f3;_LL8: {struct Cyc_Absyn_Exp*e11=_tmpCF;struct Cyc_Absyn_Exp*e12=_tmpD0;struct Cyc_Absyn_Exp*e13=_tmpD1;struct Cyc_Absyn_Exp*e21=_tmpD2;struct Cyc_Absyn_Exp*e22=_tmpD3;struct Cyc_Absyn_Exp*e23=_tmpD4;
# 352
int _tmpD5=Cyc_Evexp_const_exp_cmp(e13,e23);int c1=_tmpD5;
if(c1 != 0)return c1;
_tmpCB=e11;_tmpCC=e12;_tmpCD=e21;_tmpCE=e22;goto _LLA;}}else{goto _LL1D;}case 7U: if(((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->tag == 7U){_LL9: _tmpCB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpCC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f2;_tmpCD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_tmpCE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f2;_LLA: {struct Cyc_Absyn_Exp*e11=_tmpCB;struct Cyc_Absyn_Exp*e12=_tmpCC;struct Cyc_Absyn_Exp*e21=_tmpCD;struct Cyc_Absyn_Exp*e22=_tmpCE;
_tmpC7=e11;_tmpC8=e12;_tmpC9=e21;_tmpCA=e22;goto _LLC;}}else{goto _LL1D;}case 8U: if(((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->tag == 8U){_LLB: _tmpC7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpC8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f2;_tmpC9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_tmpCA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f2;_LLC: {struct Cyc_Absyn_Exp*e11=_tmpC7;struct Cyc_Absyn_Exp*e12=_tmpC8;struct Cyc_Absyn_Exp*e21=_tmpC9;struct Cyc_Absyn_Exp*e22=_tmpCA;
# 357
int _tmpD6=Cyc_Evexp_const_exp_cmp(e11,e21);int c1=_tmpD6;
if(c1 != 0)return c1;
return Cyc_Evexp_const_exp_cmp(e12,e22);}}else{goto _LL1D;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->tag == 3U){_LLD: _tmpC3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpC4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f2;_tmpC5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_tmpC6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f2;_LLE: {enum Cyc_Absyn_Primop p1=_tmpC3;struct Cyc_List_List*es1=_tmpC4;enum Cyc_Absyn_Primop p2=_tmpC5;struct Cyc_List_List*es2=_tmpC6;
# 361
int _tmpD7=(int)p1 - (int)p2;int c1=_tmpD7;
if(c1 != 0)return c1;
for(0;es1 != 0 && es2 != 0;(es1=es1->tl,es2=es2->tl)){
int _tmpD8=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)es1->hd,(struct Cyc_Absyn_Exp*)es2->hd);int c2=_tmpD8;
if(c2 != 0)
return c2;}
# 368
return 0;}}else{goto _LL1D;}case 17U: switch(*((int*)_tmpB0.f2)){case 17U: _LLF: _tmpC1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpC2=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_LL10: {void*t1=_tmpC1;void*t2=_tmpC2;
# 370
return Cyc_Tcutil_typecmp(t1,t2);}case 18U: _LL11: _tmpBF=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpC0=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_LL12: {void*t1=_tmpBF;struct Cyc_Absyn_Exp*e2a=_tmpC0;
# 372
void*_tmpD9=e2a->topt;void*e2atopt=_tmpD9;
if(e2atopt == 0)
({void*_tmpDA=0U;({unsigned _tmp16C=e2->loc;struct _fat_ptr _tmp16B=({const char*_tmpDB="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpDB,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp16C,_tmp16B,_tag_fat(_tmpDA,sizeof(void*),0U));});});
return Cyc_Tcutil_typecmp(t1,(void*)_check_null(e2atopt));}default: goto _LL1D;}case 18U: switch(*((int*)_tmpB0.f2)){case 17U: _LL13: _tmpBD=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpBE=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1a=_tmpBD;void*t2=_tmpBE;
# 377
void*_tmpDC=e1a->topt;void*e1atopt=_tmpDC;
if(e1atopt == 0)
({void*_tmpDD=0U;({unsigned _tmp16E=e1->loc;struct _fat_ptr _tmp16D=({const char*_tmpDE="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpDE,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp16E,_tmp16D,_tag_fat(_tmpDD,sizeof(void*),0U));});});
return Cyc_Tcutil_typecmp((void*)_check_null(e1atopt),t2);}case 18U: _LL15: _tmpBB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpBC=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_LL16: {struct Cyc_Absyn_Exp*e1a=_tmpBB;struct Cyc_Absyn_Exp*e2a=_tmpBC;
# 382
void*_tmpDF=e1a->topt;void*e1atopt=_tmpDF;
void*_tmpE0=e2a->topt;void*e2atopt=_tmpE0;
if(e1atopt == 0)
({void*_tmpE1=0U;({unsigned _tmp170=e1->loc;struct _fat_ptr _tmp16F=({const char*_tmpE2="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpE2,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp170,_tmp16F,_tag_fat(_tmpE1,sizeof(void*),0U));});});
if(e2atopt == 0)
({void*_tmpE3=0U;({unsigned _tmp172=e2->loc;struct _fat_ptr _tmp171=({const char*_tmpE4="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmpE4,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp172,_tmp171,_tag_fat(_tmpE3,sizeof(void*),0U));});});
return({void*_tmp173=(void*)_check_null(e1atopt);Cyc_Tcutil_typecmp(_tmp173,(void*)_check_null(e2atopt));});}default: goto _LL1D;}case 19U: if(((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->tag == 19U){_LL17: _tmpB7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpB8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f2;_tmpB9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_tmpBA=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f2;_LL18: {void*t1=_tmpB7;struct Cyc_List_List*f1=_tmpB8;void*t2=_tmpB9;struct Cyc_List_List*f2=_tmpBA;
# 390
int _tmpE5=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmpE5;
if(c1 != 0)return c1;{
int _tmpE6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(f1);int l1=_tmpE6;
int _tmpE7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(f2);int l2=_tmpE7;
if(l1 < l2)return - 1;
if(l2 < l1)return 1;
for(0;f1 != 0 && f2 != 0;(f1=f1->tl,f2=f2->tl)){
struct _tuple16 _tmpE8=({struct _tuple16 _tmp13E;_tmp13E.f1=(void*)f1->hd,_tmp13E.f2=(void*)f2->hd;_tmp13E;});struct _tuple16 _stmttmp16=_tmpE8;struct _tuple16 _tmpE9=_stmttmp16;unsigned _tmpEB;unsigned _tmpEA;struct _fat_ptr*_tmpED;struct _fat_ptr*_tmpEC;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpE9.f1)->tag == 0U){if(((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpE9.f2)->tag == 1U){_LL20: _LL21:
 return - 1;}else{_LL24: _tmpEC=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpE9.f1)->f1;_tmpED=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpE9.f2)->f1;_LL25: {struct _fat_ptr*fn1=_tmpEC;struct _fat_ptr*fn2=_tmpED;
# 401
int c=Cyc_strptrcmp(fn1,fn2);
if(c != 0)return c;
goto _LL1F;}}}else{if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpE9.f2)->tag == 0U){_LL22: _LL23:
# 399
 return 1;}else{_LL26: _tmpEA=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpE9.f1)->f1;_tmpEB=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpE9.f2)->f1;_LL27: {unsigned i1=_tmpEA;unsigned i2=_tmpEB;
# 405
int c=(int)(i1 - i2);
if(c != 0)return c;
goto _LL1F;}}}_LL1F:;}
# 409
return 0;}}}else{goto _LL1D;}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->tag == 14U){_LL19: _tmpB3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpB4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f2;_tmpB5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_tmpB6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f2;_LL1A: {void*t1=_tmpB3;struct Cyc_Absyn_Exp*e1a=_tmpB4;void*t2=_tmpB5;struct Cyc_Absyn_Exp*e2a=_tmpB6;
# 411
int _tmpEE=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmpEE;
if(c1 != 0)return c1;
return Cyc_Evexp_const_exp_cmp(e1a,e2a);}}else{goto _LL1D;}case 39U: if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->tag == 39U){_LL1B: _tmpB1=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB0.f1)->f1;_tmpB2=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB0.f2)->f1;_LL1C: {void*t1=_tmpB1;void*t2=_tmpB2;
# 415
if(Cyc_Unify_unify(t1,t2))return 0;
return Cyc_Tcutil_typecmp(t1,t2);}}else{goto _LL1D;}default: _LL1D: _LL1E:
({void*_tmpEF=0U;({struct _fat_ptr _tmp174=({const char*_tmpF0="Evexp::const_exp_cmp, unexpected case";_tag_fat(_tmpF0,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp174,_tag_fat(_tmpEF,sizeof(void*),0U));});});}_LL6:;}}}}}}
# 420
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 423
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmpF1=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple10 _stmttmp17=_tmpF1;struct _tuple10 _tmpF2=_stmttmp17;int _tmpF4;unsigned _tmpF3;_LL1: _tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;_LL2: {unsigned i1=_tmpF3;int known1=_tmpF4;
struct _tuple10 _tmpF5=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple10 _stmttmp18=_tmpF5;struct _tuple10 _tmpF6=_stmttmp18;int _tmpF8;unsigned _tmpF7;_LL4: _tmpF7=_tmpF6.f1;_tmpF8=_tmpF6.f2;_LL5: {unsigned i2=_tmpF7;int known2=_tmpF8;
if(known1 && known2)
return i1 <= i2;
return Cyc_Evexp_same_const_exp(e1,e2);}}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 433
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmpF9=Cyc_Tcutil_compress(t);void*_stmttmp19=_tmpF9;void*_tmpFA=_stmttmp19;struct Cyc_Absyn_Typedefdecl*_tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_Exp*_tmpFE;void*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Tvar*_tmp100;struct Cyc_List_List*_tmp102;void*_tmp101;switch(*((int*)_tmpFA)){case 0U: _LL1: _tmp101=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFA)->f1;_tmp102=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFA)->f2;_LL2: {void*c=_tmp101;struct Cyc_List_List*ts=_tmp102;
# 436
void*_tmp103=c;struct Cyc_Absyn_Aggrdecl*_tmp104;struct Cyc_Absyn_Datatypefield*_tmp105;switch(*((int*)_tmp103)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp103)->f1).KnownDatatypefield).tag == 2){_LL1C: _tmp105=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp103)->f1).KnownDatatypefield).val).f2;_LL1D: {struct Cyc_Absyn_Datatypefield*tuf=_tmp105;
# 438
{struct Cyc_List_List*_tmp106=tuf->typs;struct Cyc_List_List*tqs=_tmp106;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)tqs->hd)).f2))
return 0;}}
return 1;}}else{_LL1E: _LL1F:
 return 0;}case 4U: _LL20: _LL21:
 goto _LL23;case 1U: _LL22: _LL23:
 goto _LL25;case 2U: _LL24: _LL25:
 goto _LL27;case 17U: _LL26: _LL27:
 goto _LL29;case 15U: _LL28: _LL29:
 goto _LL2B;case 3U: _LL2A: _LL2B:
 goto _LL2D;case 18U: _LL2C: _LL2D:
 goto _LL2F;case 16U: _LL2E: _LL2F:
 return 1;case 0U: _LL30: _LL31:
# 452
 goto _LL33;case 5U: _LL32: _LL33:
 goto _LL35;case 7U: _LL34: _LL35:
 goto _LL37;case 6U: _LL36: _LL37:
 goto _LL39;case 9U: _LL38: _LL39:
 goto _LL3B;case 8U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 goto _LL3F;case 12U: _LL3E: _LL3F:
 goto _LL41;case 13U: _LL40: _LL41:
 goto _LL43;case 14U: _LL42: _LL43:
 goto _LL45;case 10U: _LL44: _LL45:
 return 0;default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp103)->f1).UnknownAggr).tag == 1){_LL46: _LL47:
# 464
({void*_tmp107=0U;({struct _fat_ptr _tmp175=({const char*_tmp108="szof on unchecked StructType";_tag_fat(_tmp108,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp175,_tag_fat(_tmp107,sizeof(void*),0U));});});}else{_LL48: _tmp104=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp103)->f1).KnownAggr).val;_LL49: {struct Cyc_Absyn_Aggrdecl*ad=_tmp104;
# 466
if(ad->impl == 0)return 0;else{
# 468
struct _RegionHandle _tmp109=_new_region("temp");struct _RegionHandle*temp=& _tmp109;_push_region(temp);
{struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,ts);struct Cyc_List_List*inst=_tmp10A;
{struct Cyc_List_List*_tmp10B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp10B;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp10C=0;_npop_handler(0U);return _tmp10C;}}}{
int _tmp10D=1;_npop_handler(0U);return _tmp10D;}}
# 469
;_pop_region(temp);}}}}_LL1B:;}case 2U: _LL3: _tmp100=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpFA)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp100;
# 476
enum Cyc_Absyn_KindQual _tmp10E=(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind;enum Cyc_Absyn_KindQual _stmttmp1A=_tmp10E;enum Cyc_Absyn_KindQual _tmp10F=_stmttmp1A;if(_tmp10F == Cyc_Absyn_BoxKind){_LL4B: _LL4C:
 return 1;}else{_LL4D: _LL4E:
 return 0;}_LL4A:;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFA)->f1 != 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFA)->f1)->v)->kind == Cyc_Absyn_BoxKind){_LL5: _LL6:
# 480
 return 1;}else{goto _LL7;}}else{_LL7: _LL8:
 return 0;}case 6U: _LL9: _tmpFF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFA)->f1;_LLA: {struct Cyc_List_List*tqs=_tmpFF;
# 483
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)tqs->hd)).f2))
return 0;}
return 1;}case 3U: _LLB: _LLC:
# 489
 return 1;case 4U: _LLD: _tmpFD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFA)->f1).elt_type;_tmpFE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFA)->f1).num_elts;_LLE: {void*t2=_tmpFD;struct Cyc_Absyn_Exp*e=_tmpFE;
# 492
return e != 0;}case 5U: _LLF: _LL10:
 return 0;case 7U: _LL11: _tmpFC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFA)->f2;_LL12: {struct Cyc_List_List*fs=_tmpFC;
# 495
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}case 10U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 return 0;default: _LL19: _tmpFB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpFA)->f3;_LL1A: {struct Cyc_Absyn_Typedefdecl*td=_tmpFB;
# 504
if(td == 0 || td->kind == 0)
({void*_tmp110=0U;({struct _fat_ptr _tmp178=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp140;_tmp140.tag=0U,({struct _fat_ptr _tmp176=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp140.f1=_tmp176;});_tmp140;});void*_tmp111[1U];_tmp111[0]=& _tmp113;({struct _fat_ptr _tmp177=({const char*_tmp112="szof typedeftype %s";_tag_fat(_tmp112,sizeof(char),20U);});Cyc_aprintf(_tmp177,_tag_fat(_tmp111,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp178,_tag_fat(_tmp110,sizeof(void*),0U));});});
return(int)((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind == (int)Cyc_Absyn_BoxKind;}}_LL0:;}

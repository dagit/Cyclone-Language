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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
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
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 876 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 915
extern void*Cyc_Absyn_empty_effect;
# 917
extern void*Cyc_Absyn_false_type;
# 919
void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 934
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 964
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned);
# 40
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
# 44
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*,void*rgn1,void*rgn2);
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 174
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 176
struct _tuple11 Cyc_Tcutil_swap_kind(void*,void*);
# 196
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 200
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 215
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 258
int Cyc_Tcutil_new_tvar_id();
# 260
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 66 "tcenv.h"
void*Cyc_Tcenv_env_err(struct _fat_ptr msg);
# 88
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 138
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);char Cyc_Tcenv_Env_error[10U]="Env_error";
# 51 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 53
void*Cyc_Tcenv_env_err(struct _fat_ptr msg){
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{enum Cyc_Tcenv_NewStatus in_new;int in_notreadctxt: 1;int in_lhs: 1;int abstract_ok: 1;int in_stmt_exp: 1;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct _tuple12*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;};
# 113
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(){
# 115
struct Cyc_Tcenv_Genv*_tmp0=({struct Cyc_Tcenv_Genv*_tmp3=_cycalloc(sizeof(*_tmp3));({struct Cyc_Dict_Dict _tmpE8=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->aggrdecls=_tmpE8;}),({
struct Cyc_Dict_Dict _tmpE7=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->datatypedecls=_tmpE7;}),({
struct Cyc_Dict_Dict _tmpE6=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->enumdecls=_tmpE6;}),({
struct Cyc_Dict_Dict _tmpE5=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->typedefs=_tmpE5;}),({
struct Cyc_Dict_Dict _tmpE4=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->ordinaries=_tmpE4;});_tmp3;});
({struct Cyc_Dict_Dict _tmpEC=({struct Cyc_Dict_Dict _tmpEB=_tmp0->datatypedecls;struct _tuple0*_tmpEA=Cyc_Absyn_exn_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpEB,_tmpEA,({struct Cyc_Absyn_Datatypedecl**_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Datatypedecl*_tmpE9=Cyc_Absyn_exn_tud();*_tmp1=_tmpE9;});_tmp1;}));});_tmp0->datatypedecls=_tmpEC;});
return({struct Cyc_Tcenv_Tenv*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->ns=0,_tmp2->ae=_tmp0,_tmp2->le=0,_tmp2->allow_valueof=0,_tmp2->in_extern_c_include=0,_tmp2->in_tempest=0,_tmp2->tempest_generalize=0;_tmp2;});}struct _tuple13{void*f1;int f2;};
# 123
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q,int is_use){
struct _tuple13*_tmp4=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
if(is_use)
(*_tmp4).f2=1;
return(*_tmp4).f1;}
# 129
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->aggrdecls,q);}
# 132
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);}
# 135
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmp5=_region_malloc(r,sizeof(*_tmp5));({struct Cyc_Absyn_Datatypedecl**_tmpED=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);*_tmp5=_tmpED;});_tmp5;});}
# 138
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);}
# 141
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->typedefs,q);}
# 147
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp6=te->le;
if(_tmp6 == 0)((int(*)(struct _fat_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp6;}
# 152
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _fat_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp7="put_fenv";_tag_fat(_tmp7,sizeof(char),9U);}));{
struct Cyc_Tcenv_Tenv*_tmp8=({struct Cyc_Tcenv_Tenv*_tmp9=_cycalloc(sizeof(*_tmp9));*_tmp9=*te;_tmp9;});
_tmp8->le=fe;
return _tmp8;};}
# 158
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmpA=({struct Cyc_Tcenv_Tenv*_tmpB=_cycalloc(sizeof(*_tmpB));*_tmpB=*te;_tmpB;});
_tmpA->le=0;
return _tmpA;}
# 164
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
return(({struct Cyc_Tcenv_Tenv*_tmpEE=te;Cyc_Tcenv_get_fenv(_tmpEE,({const char*_tmpC="return_typ";_tag_fat(_tmpC,sizeof(char),11U);}));})->shared)->return_typ;}
# 168
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD=te->le;
if(te->le == 0)return 0;
return((struct Cyc_Tcenv_Fenv*)_check_null(_tmpD))->type_vars;}
# 174
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpE=_cycalloc(sizeof(*_tmpE));({struct Cyc_List_List*_tmpEF=Cyc_Tcenv_lookup_type_vars(te);_tmpE->v=_tmpEF;});_tmpE;});}
# 178
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
struct Cyc_Tcenv_Fenv*_tmpF=({struct Cyc_Tcenv_Fenv*_tmp12=_cycalloc(sizeof(*_tmp12));({struct Cyc_Tcenv_Fenv _tmpF1=*({struct Cyc_Tcenv_Tenv*_tmpF0=te;Cyc_Tcenv_get_fenv(_tmpF0,({const char*_tmp11="add_type_vars";_tag_fat(_tmp11,sizeof(char),14U);}));});*_tmp12=_tmpF1;});_tmp12;});
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpF->type_vars);
Cyc_Tcutil_check_unique_tvars(loc,_tmp10);
_tmpF->type_vars=_tmp10;
return Cyc_Tcenv_put_fenv(te,_tmpF);};}
# 187
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp13=te->le;
if(_tmp13 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp14=({struct Cyc_Tcenv_Fenv*_tmp15=_cycalloc(sizeof(*_tmp15));*_tmp15=*_tmp13;_tmp15;});
(_tmp14->flags).in_new=status;
return Cyc_Tcenv_put_fenv(te,_tmp14);};}
# 194
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16=te->le;
if(_tmp16 == 0)return Cyc_Tcenv_NoneNew;
return(_tmp16->flags).in_new;}
# 199
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17=te->le;
if(_tmp17 == 0)return 0;
return(_tmp17->flags).abstract_ok;}
# 204
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18=te->le;
if(_tmp18 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp19=({struct Cyc_Tcenv_Fenv*_tmp1A=_cycalloc(sizeof(*_tmp1A));*_tmp1A=*_tmp18;_tmp1A;});
(_tmp19->flags).abstract_ok=1;
return Cyc_Tcenv_put_fenv(te,_tmp19);};}
# 211
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1B=te->le;
if(_tmp1B == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1C=({struct Cyc_Tcenv_Fenv*_tmp1D=_cycalloc(sizeof(*_tmp1D));*_tmp1D=*_tmp1B;_tmp1D;});
(_tmp1C->flags).abstract_ok=0;
return Cyc_Tcenv_put_fenv(te,_tmp1C);};}
# 218
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1E=te->le;
if(_tmp1E == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1F=({struct Cyc_Tcenv_Fenv*_tmp20=_cycalloc(sizeof(*_tmp20));*_tmp20=*_tmp1E;_tmp20;});
(_tmp1F->flags).in_notreadctxt=1;
return Cyc_Tcenv_put_fenv(te,_tmp1F);};}
# 225
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp21=te->le;
if(_tmp21 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp22=({struct Cyc_Tcenv_Fenv*_tmp23=_cycalloc(sizeof(*_tmp23));*_tmp23=*_tmp21;_tmp23;});
(_tmp22->flags).in_notreadctxt=0;
return Cyc_Tcenv_put_fenv(te,_tmp22);};}
# 232
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp24=te->le;
if(_tmp24 == 0)return 0;
return(_tmp24->flags).in_notreadctxt;}
# 237
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25=te->le;
if(_tmp25 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp26=({struct Cyc_Tcenv_Fenv*_tmp27=_cycalloc(sizeof(*_tmp27));*_tmp27=*_tmp25;_tmp27;});
(_tmp26->flags).in_lhs=1;
return Cyc_Tcenv_put_fenv(te,_tmp26);};}
# 244
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp28=te->le;
if(_tmp28 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp29=({struct Cyc_Tcenv_Fenv*_tmp2A=_cycalloc(sizeof(*_tmp2A));*_tmp2A=*_tmp28;_tmp2A;});
(_tmp29->flags).in_lhs=0;
return Cyc_Tcenv_put_fenv(te,_tmp29);};}
# 251
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B=te->le;
if(_tmp2B == 0)return 0;
return(_tmp2B->flags).in_lhs;}
# 256
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2C=te->le;
if(_tmp2C == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp2D=({struct Cyc_Tcenv_Fenv*_tmp2E=_cycalloc(sizeof(*_tmp2E));*_tmp2E=*_tmp2C;_tmp2E;});
(_tmp2D->flags).in_stmt_exp=1;
return Cyc_Tcenv_put_fenv(te,_tmp2D);};}
# 263
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2F=te->le;
if(_tmp2F == 0)return 0;
return(_tmp2F->flags).in_stmt_exp;}
# 270
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 272
const struct _tuple12*_tmp30=({struct Cyc_Tcenv_Tenv*_tmpF2=te;Cyc_Tcenv_get_fenv(_tmpF2,({const char*_tmp32="process_fallthru";_tag_fat(_tmp32,sizeof(char),17U);}));})->ctrl_env;
if(_tmp30 != (const struct _tuple12*)0)
({struct Cyc_Absyn_Switch_clause**_tmpF3=({struct Cyc_Absyn_Switch_clause**_tmp31=_cycalloc(sizeof(*_tmp31));*_tmp31=(*_tmp30).f1;_tmp31;});*clauseopt=_tmpF3;});
return _tmp30;}
# 278
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 280
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,_tmp33->tl=ft_typ;_tmp33;});}{
struct _tuple12*_tmp34=({struct _tuple12*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->f1=clause,_tmp38->f2=new_tvs,({struct Cyc_List_List*_tmpF4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);_tmp38->f3=_tmpF4;});_tmp38;});
struct Cyc_Tcenv_Fenv*_tmp35=({struct Cyc_Tcenv_Fenv*_tmp37=_cycalloc(sizeof(*_tmp37));({struct Cyc_Tcenv_Fenv _tmpF6=*({struct Cyc_Tcenv_Tenv*_tmpF5=te;Cyc_Tcenv_get_fenv(_tmpF5,({const char*_tmp36="set_fallthru";_tag_fat(_tmp36,sizeof(char),13U);}));});*_tmp37=_tmpF6;});_tmp37;});
_tmp35->ctrl_env=(const struct _tuple12*)_tmp34;
return Cyc_Tcenv_put_fenv(te,_tmp35);};}
# 289
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Fenv*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct Cyc_Tcenv_Fenv _tmpF8=*({struct Cyc_Tcenv_Tenv*_tmpF7=te;Cyc_Tcenv_get_fenv(_tmpF7,({const char*_tmp39="clear_fallthru";_tag_fat(_tmp39,sizeof(char),15U);}));});*_tmp3A=_tmpF8;});_tmp3A;});
fe->ctrl_env=0;
return Cyc_Tcenv_put_fenv(te,fe);}
# 295
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3B=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp3E=_cycalloc(sizeof(*_tmp3E));*_tmp3E=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp3E;});
struct Cyc_Tcenv_Tenv*_tmp3C=({struct Cyc_Tcenv_Tenv*_tmp3D=_cycalloc(sizeof(*_tmp3D));*_tmp3D=*te;_tmp3D;});
_tmp3C->allow_valueof=1;
return _tmp3C;}
# 301
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3F=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp42=_cycalloc(sizeof(*_tmp42));*_tmp42=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp42;});
struct Cyc_Tcenv_Tenv*_tmp40=({struct Cyc_Tcenv_Tenv*_tmp41=_cycalloc(sizeof(*_tmp41));*_tmp41=*te;_tmp41;});
_tmp40->in_extern_c_include=1;
return _tmp40;}
# 307
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp43=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp46;});
struct Cyc_Tcenv_Tenv*_tmp44=({struct Cyc_Tcenv_Tenv*_tmp45=_cycalloc(sizeof(*_tmp45));*_tmp45=*te;_tmp45;});
_tmp44->in_tempest=1;
return _tmp44;}
# 313
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp47=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp4A;});
struct Cyc_Tcenv_Tenv*_tmp48=({struct Cyc_Tcenv_Tenv*_tmp49=_cycalloc(sizeof(*_tmp49));*_tmp49=*te;_tmp49;});
_tmp48->in_tempest=0;
return _tmp48;}
# 319
int Cyc_Tcenv_in_tempest(struct Cyc_Tcenv_Tenv*te){
return te->in_tempest;}
# 322
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*rgn,int opened){
struct Cyc_Tcenv_Fenv*_tmp4B=({struct Cyc_Tcenv_Fenv*_tmp53=_cycalloc(sizeof(*_tmp53));({struct Cyc_Tcenv_Fenv _tmpFA=*({struct Cyc_Tcenv_Tenv*_tmpF9=te;Cyc_Tcenv_get_fenv(_tmpF9,({const char*_tmp52="add_region";_tag_fat(_tmp52,sizeof(char),11U);}));});*_tmp53=_tmpFA;});_tmp53;});
struct Cyc_RgnOrder_RgnPO*_tmp4C=_tmp4B->region_order;
{void*_tmp4D=Cyc_Tcutil_compress(rgn);void*_tmp4E=_tmp4D;struct Cyc_Absyn_Tvar*_tmp4F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->tag == 2U){_LL1: _tmp4F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->f1;_LL2:
# 327
 _tmp4C=Cyc_RgnOrder_add_youngest(_tmp4C,_tmp4F,opened);
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 331
_tmp4B->region_order=_tmp4C;
({void*_tmpFD=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({void*_tmpFC=Cyc_Absyn_access_eff(rgn);_tmp51->hd=_tmpFC;}),({
struct Cyc_List_List*_tmpFB=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=_tmp4B->capability,_tmp50->tl=0;_tmp50;});_tmp51->tl=_tmpFB;});_tmp51;}));
# 332
_tmp4B->capability=_tmpFD;});
# 334
return Cyc_Tcenv_put_fenv(te,_tmp4B);}
# 337
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
struct Cyc_Tcenv_Fenv*_tmp54=({struct Cyc_Tcenv_Fenv*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Tcenv_Fenv _tmpFF=*({struct Cyc_Tcenv_Tenv*_tmpFE=te;Cyc_Tcenv_get_fenv(_tmpFE,({const char*_tmp5A="new_named_block";_tag_fat(_tmp5A,sizeof(char),16U);}));});*_tmp5B=_tmpFF;});_tmp5B;});
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp55=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=2U,_tmp59->f1=block_rgn;_tmp59;});
({struct Cyc_List_List*_tmp100=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->hd=block_rgn,_tmp56->tl=_tmp54->type_vars;_tmp56;});_tmp54->type_vars=_tmp100;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp54->type_vars);
({struct Cyc_RgnOrder_RgnPO*_tmp101=Cyc_RgnOrder_add_youngest(_tmp54->region_order,block_rgn,0);_tmp54->region_order=_tmp101;});
({void*_tmp104=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({void*_tmp103=Cyc_Absyn_access_eff((void*)_tmp55);_tmp58->hd=_tmp103;}),({
struct Cyc_List_List*_tmp102=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=_tmp54->capability,_tmp57->tl=0;_tmp57;});_tmp58->tl=_tmp102;});_tmp58;}));
# 343
_tmp54->capability=_tmp104;});
# 345
_tmp54->curr_rgn=(void*)_tmp55;
return Cyc_Tcenv_put_fenv(te,_tmp54);}
# 349
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0U,& Cyc_Tcutil_rk};
# 351
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(loc,te,t);}struct _tuple14{void*f1;void*f2;};
# 357
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned loc){
# 359
struct Cyc_Tcenv_Fenv*_tmp5C=({struct Cyc_Tcenv_Fenv*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct Cyc_Tcenv_Fenv _tmp106=*({struct Cyc_Tcenv_Tenv*_tmp105=te;Cyc_Tcenv_get_fenv(_tmp105,({const char*_tmp5D="new_outlives_constraints";_tag_fat(_tmp5D,sizeof(char),25U);}));});*_tmp5E=_tmp106;});_tmp5E;});
for(0;cs != 0;cs=cs->tl){
({struct Cyc_RgnOrder_RgnPO*_tmp107=
Cyc_RgnOrder_add_outlives_constraint(_tmp5C->region_order,(*((struct _tuple14*)cs->hd)).f1,(*((struct _tuple14*)cs->hd)).f2,loc);
# 361
_tmp5C->region_order=_tmp107;});}
# 363
return Cyc_Tcenv_put_fenv(te,_tmp5C);}
# 366
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned loc){
# 370
void*_tmp5F=Cyc_Tcutil_compress(r1);
void*_tmp60=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp61=Cyc_Tcutil_type_kind(_tmp5F);
struct Cyc_Absyn_Kind*_tmp62=Cyc_Tcutil_type_kind(_tmp60);
# 376
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp61,_tmp62);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp62,_tmp61);
if(!r1_le_r2 && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmpDE;_tmpDE.tag=0U,({
struct _fat_ptr _tmp108=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp5F));_tmpDE.f1=_tmp108;});_tmpDE;});struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0U,({struct _fat_ptr _tmp109=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp60));_tmpDD.f1=_tmp109;});_tmpDD;});struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmpDC;_tmpDC.tag=0U,({struct _fat_ptr _tmp10A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(_tmp61));_tmpDC.f1=_tmp10A;});_tmpDC;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmpDB;_tmpDB.tag=0U,({struct _fat_ptr _tmp10B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(_tmp62));_tmpDB.f1=_tmp10B;});_tmpDB;});void*_tmp63[4U];_tmp63[0]=& _tmp65,_tmp63[1]=& _tmp66,_tmp63[2]=& _tmp67,_tmp63[3]=& _tmp68;({unsigned _tmp10D=loc;struct _fat_ptr _tmp10C=({const char*_tmp64="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_fat(_tmp64,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp10D,_tmp10C,_tag_fat(_tmp63,sizeof(void*),4U));});});
return Cyc_Tcenv_new_outlives_constraints(te,0,loc);}else{
# 383
if(r1_le_r2 && !r2_le_r1)
({struct _tuple11*_tmp110=({struct _tuple11*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _tuple11 _tmp10F=({void*_tmp10E=_tmp60;Cyc_Tcutil_swap_kind(_tmp10E,Cyc_Tcutil_kind_to_bound(_tmp61));});*_tmp69=_tmp10F;});_tmp69;});*oldtv=_tmp110;});else{
if(!r1_le_r2 && r2_le_r1)
({struct _tuple11*_tmp113=({struct _tuple11*_tmp6A=_cycalloc(sizeof(*_tmp6A));({struct _tuple11 _tmp112=({void*_tmp111=_tmp5F;Cyc_Tcutil_swap_kind(_tmp111,Cyc_Tcutil_kind_to_bound(_tmp62));});*_tmp6A=_tmp112;});_tmp6A;});*oldtv=_tmp113;});}}{
# 389
struct Cyc_List_List*_tmp6B=0;
if((_tmp5F != Cyc_Absyn_heap_rgn_type && _tmp5F != Cyc_Absyn_unique_rgn_type)&& _tmp5F != Cyc_Absyn_refcnt_rgn_type)
_tmp6B=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct _tuple14*_tmp115=({struct _tuple14*_tmp6C=_cycalloc(sizeof(*_tmp6C));({void*_tmp114=Cyc_Absyn_access_eff(_tmp5F);_tmp6C->f1=_tmp114;}),_tmp6C->f2=_tmp60;_tmp6C;});_tmp6D->hd=_tmp115;}),_tmp6D->tl=_tmp6B;_tmp6D;});
if((_tmp60 != Cyc_Absyn_heap_rgn_type && _tmp60 != Cyc_Absyn_unique_rgn_type)&& _tmp60 != Cyc_Absyn_refcnt_rgn_type)
_tmp6B=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct _tuple14*_tmp117=({struct _tuple14*_tmp6E=_cycalloc(sizeof(*_tmp6E));({void*_tmp116=Cyc_Absyn_access_eff(_tmp60);_tmp6E->f1=_tmp116;}),_tmp6E->f2=_tmp5F;_tmp6E;});_tmp6F->hd=_tmp117;}),_tmp6F->tl=_tmp6B;_tmp6F;});
return Cyc_Tcenv_new_outlives_constraints(te,_tmp6B,loc);};}
# 397
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp70=te->le;
if(_tmp70 == 0)return Cyc_Absyn_heap_rgn_type;
return _tmp70->curr_rgn;}
# 402
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp71=te->le;
if(_tmp71 == 0)return 0;
return _tmp71->region_order;}
# 411
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Tenv*_tmp118=te;Cyc_Tcenv_get_fenv(_tmp118,({const char*_tmp75="check_rgn_accessible";_tag_fat(_tmp75,sizeof(char),21U);}));});
if(Cyc_Tcutil_region_in_effect(0,rgn,fe->capability)||
 Cyc_Tcutil_region_in_effect(1,rgn,fe->capability))
return;
if(({struct Cyc_RgnOrder_RgnPO*_tmp11A=fe->region_order;void*_tmp119=Cyc_Absyn_access_eff(rgn);Cyc_RgnOrder_eff_outlives_eff(_tmp11A,_tmp119,fe->capability);}))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmpDF;_tmpDF.tag=0U,({struct _fat_ptr _tmp11B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(rgn));_tmpDF.f1=_tmp11B;});_tmpDF;});void*_tmp72[1U];_tmp72[0]=& _tmp74;({unsigned _tmp11D=loc;struct _fat_ptr _tmp11C=({const char*_tmp73="Expression accesses unavailable region %s";_tag_fat(_tmp73,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp11D,_tmp11C,_tag_fat(_tmp72,sizeof(void*),1U));});});}
# 424
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
return({struct Cyc_RgnOrder_RgnPO*_tmp11F=Cyc_Tcenv_curr_rgnpo(te);void*_tmp11E=rt_a;Cyc_RgnOrder_rgn_outlives_rgn(_tmp11F,_tmp11E,rt_b);});}struct _tuple15{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned f4;};
# 430
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp76=({
struct Cyc_Tcenv_Tenv*_tmp120=te;Cyc_Tcenv_get_fenv(_tmp120,({const char*_tmp7D="check_effect_accessible";_tag_fat(_tmp7D,sizeof(char),24U);}));});
# 431
struct Cyc_Tcenv_Fenv*_tmp77=_tmp76;void*_tmp7C;struct Cyc_RgnOrder_RgnPO*_tmp7B;struct Cyc_Tcenv_SharedFenv*_tmp7A;_LL1: _tmp7C=_tmp77->capability;_tmp7B=_tmp77->region_order;_tmp7A=_tmp77->shared;_LL2:;
# 433
if(Cyc_Tcutil_subset_effect(0,eff,_tmp7C))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp7B,eff,_tmp7C))
return;
({struct Cyc_List_List*_tmp122=({struct Cyc_List_List*_tmp79=_cycalloc(sizeof(*_tmp79));
({struct _tuple15*_tmp121=({struct _tuple15*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->f1=_tmp7C,_tmp78->f2=eff,_tmp78->f3=_tmp7B,_tmp78->f4=loc;_tmp78;});_tmp79->hd=_tmp121;}),_tmp79->tl=_tmp7A->delayed_effect_checks;_tmp79;});
# 437
_tmp7A->delayed_effect_checks=_tmp122;});}
# 440
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp7E=(({
struct Cyc_Tcenv_Tenv*_tmp123=te;Cyc_Tcenv_get_fenv(_tmp123,({const char*_tmp89="check_delayed_constraints";_tag_fat(_tmp89,sizeof(char),26U);}));})->shared)->delayed_effect_checks;
for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){
struct _tuple15*_tmp7F=(struct _tuple15*)_tmp7E->hd;struct _tuple15*_tmp80=_tmp7F;void*_tmp88;void*_tmp87;struct Cyc_RgnOrder_RgnPO*_tmp86;unsigned _tmp85;_LL1: _tmp88=_tmp80->f1;_tmp87=_tmp80->f2;_tmp86=_tmp80->f3;_tmp85=_tmp80->f4;_LL2:;
if(Cyc_Tcutil_subset_effect(1,_tmp87,_tmp88))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp86,_tmp87,_tmp88))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0U,({
struct _fat_ptr _tmp124=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp88));_tmpE1.f1=_tmp124;});_tmpE1;});struct Cyc_String_pa_PrintArg_struct _tmp84=({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0U,({struct _fat_ptr _tmp125=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp87));_tmpE0.f1=_tmp125;});_tmpE0;});void*_tmp81[2U];_tmp81[0]=& _tmp83,_tmp81[1]=& _tmp84;({unsigned _tmp127=_tmp85;struct _fat_ptr _tmp126=({const char*_tmp82="Capability \n%s\ndoes not cover function's effect\n%s";_tag_fat(_tmp82,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp127,_tmp126,_tag_fat(_tmp81,sizeof(void*),2U));});});}}struct _tuple16{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned f3;};
# 458
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*po){
# 460
struct Cyc_Tcenv_Fenv*_tmp8A=te->le;
if(_tmp8A == 0){
# 463
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple14*)po->hd)).f1,Cyc_Absyn_empty_effect)|| !({
void*_tmp128=Cyc_Absyn_access_eff((*((struct _tuple14*)po->hd)).f2);Cyc_Tcutil_subset_effect(1,_tmp128,Cyc_Absyn_empty_effect);}))
({void*_tmp8B=0U;({unsigned _tmp12A=loc;struct _fat_ptr _tmp129=({const char*_tmp8C="the required region ordering is not satisfied here";_tag_fat(_tmp8C,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp12A,_tmp129,_tag_fat(_tmp8B,sizeof(void*),0U));});});}
return;}{
# 469
struct Cyc_Tcenv_Fenv*_tmp8D=_tmp8A;struct Cyc_Tcenv_Fenv*_tmp8E=_tmp8D;struct Cyc_RgnOrder_RgnPO*_tmp92;struct Cyc_Tcenv_SharedFenv*_tmp91;_LL1: _tmp92=_tmp8E->region_order;_tmp91=_tmp8E->shared;_LL2:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp92,po,Cyc_Absyn_heap_rgn_type,0))
({struct Cyc_List_List*_tmp12C=({struct Cyc_List_List*_tmp90=_cycalloc(sizeof(*_tmp90));
({struct _tuple16*_tmp12B=({struct _tuple16*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->f1=_tmp92,_tmp8F->f2=po,_tmp8F->f3=loc;_tmp8F;});_tmp90->hd=_tmp12B;}),_tmp90->tl=_tmp91->delayed_constraint_checks;_tmp90;});
# 471
_tmp91->delayed_constraint_checks=_tmp12C;});};}
# 475
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp93=(({
struct Cyc_Tcenv_Tenv*_tmp12D=te;Cyc_Tcenv_get_fenv(_tmp12D,({const char*_tmp9B="check_delayed_constraints";_tag_fat(_tmp9B,sizeof(char),26U);}));})->shared)->delayed_constraint_checks;
for(0;_tmp93 != 0;_tmp93=_tmp93->tl){
struct _tuple16*_tmp94=(struct _tuple16*)_tmp93->hd;struct _tuple16*_tmp95=_tmp94;struct Cyc_RgnOrder_RgnPO*_tmp9A;struct Cyc_List_List*_tmp99;unsigned _tmp98;_LL1: _tmp9A=_tmp95->f1;_tmp99=_tmp95->f2;_tmp98=_tmp95->f3;_LL2:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp9A,_tmp99,Cyc_Absyn_heap_rgn_type,1))
({void*_tmp96=0U;({unsigned _tmp12F=_tmp98;struct _fat_ptr _tmp12E=({const char*_tmp97="the required region ordering is not satisfied here";_tag_fat(_tmp97,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp12F,_tmp12E,_tag_fat(_tmp96,sizeof(void*),0U));});});}}
# 485
static struct Cyc_Tcenv_SharedFenv*Cyc_Tcenv_new_shared_fenv(void*ret){
return({struct Cyc_Tcenv_SharedFenv*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->return_typ=ret,_tmp9C->delayed_effect_checks=0,_tmp9C->delayed_constraint_checks=0;_tmp9C;});}
# 491
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpB1=_cycalloc(sizeof(*_tmpB1));
({struct _fat_ptr*_tmp133=({struct _fat_ptr*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct _fat_ptr _tmp132=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmpE2;_tmpE2.tag=0U,_tmpE2.f1=(struct _fat_ptr)((struct _fat_ptr)*(*fd->name).f2);_tmpE2;});void*_tmpAD[1U];_tmpAD[0]=& _tmpAF;({struct _fat_ptr _tmp131=({const char*_tmpAE="`%s";_tag_fat(_tmpAE,sizeof(char),4U);});Cyc_aprintf(_tmp131,_tag_fat(_tmpAD,sizeof(void*),1U));});});*_tmpB0=_tmp132;});_tmpB0;});_tmpB1->name=_tmp133;}),({
int _tmp130=Cyc_Tcutil_new_tvar_id();_tmpB1->identity=_tmp130;}),_tmpB1->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpB1;});
struct Cyc_List_List*_tmp9D=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=rgn0,_tmpAC->tl=(fd->i).tvars;_tmpAC;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp9D);{
# 498
struct Cyc_RgnOrder_RgnPO*_tmp9E=Cyc_RgnOrder_initial_fn_po((fd->i).tvars,(fd->i).rgn_po,(void*)_check_null((fd->i).effect),rgn0,loc);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=2U,_tmpAB->f1=rgn0;_tmpAB;});
{struct Cyc_List_List*_tmp9F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
((struct Cyc_Absyn_Vardecl*)_tmp9F->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpA0=*((struct Cyc_Absyn_VarargInfo*)_check_null((fd->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmpA1=_tmpA0;struct _fat_ptr*_tmpA7;struct Cyc_Absyn_Tqual _tmpA6;void*_tmpA5;int _tmpA4;_LL1: _tmpA7=_tmpA1.name;_tmpA6=_tmpA1.tq;_tmpA5=_tmpA1.type;_tmpA4=_tmpA1.inject;_LL2:;{
# 505
struct Cyc_List_List*_tmpA2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
struct Cyc_Absyn_Vardecl*_tmpA3=(struct Cyc_Absyn_Vardecl*)_tmpA2->hd;
if(Cyc_strptrcmp((*_tmpA3->name).f2,(struct _fat_ptr*)_check_null(_tmpA7))== 0){
({void*_tmp134=Cyc_Absyn_fatptr_type(_tmpA5,param_rgn,_tmpA6,Cyc_Absyn_false_type);_tmpA3->type=_tmp134;});
break;}}};}
# 513
return({struct Cyc_Tcenv_Fenv*_tmpAA=_cycalloc(sizeof(*_tmpAA));
({struct Cyc_Tcenv_SharedFenv*_tmp138=Cyc_Tcenv_new_shared_fenv((fd->i).ret_type);_tmpAA->shared=_tmp138;}),_tmpAA->type_vars=_tmp9D,_tmpAA->region_order=_tmp9E,_tmpAA->ctrl_env=0,({
# 518
void*_tmp137=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpA9=_cycalloc(sizeof(*_tmpA9));({void*_tmp136=Cyc_Absyn_access_eff(param_rgn);_tmpA9->hd=_tmp136;}),({
struct Cyc_List_List*_tmp135=({struct Cyc_List_List*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->hd=(void*)_check_null((fd->i).effect),_tmpA8->tl=0;_tmpA8;});_tmpA9->tl=_tmp135;});_tmpA9;}));
# 518
_tmpAA->capability=_tmp137;}),_tmpAA->curr_rgn=param_rgn,
# 521
(_tmpAA->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpAA->flags).in_notreadctxt=0,(_tmpAA->flags).in_lhs=0,(_tmpAA->flags).abstract_ok=0,(_tmpAA->flags).in_stmt_exp=0;_tmpAA;});};}
# 525
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmpB2=old_fenv;struct Cyc_RgnOrder_RgnPO*_tmpD1;struct Cyc_List_List*_tmpD0;struct Cyc_Tcenv_SharedFenv*_tmpCF;_LL1: _tmpD1=_tmpB2->region_order;_tmpD0=_tmpB2->type_vars;_tmpCF=_tmpB2->shared;_LL2:;{
# 528
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpCE=_cycalloc(sizeof(*_tmpCE));
({struct _fat_ptr*_tmp13C=({struct _fat_ptr*_tmpCD=_cycalloc(sizeof(*_tmpCD));({struct _fat_ptr _tmp13B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpCC=({struct Cyc_String_pa_PrintArg_struct _tmpE3;_tmpE3.tag=0U,_tmpE3.f1=(struct _fat_ptr)((struct _fat_ptr)*(*fd->name).f2);_tmpE3;});void*_tmpCA[1U];_tmpCA[0]=& _tmpCC;({struct _fat_ptr _tmp13A=({const char*_tmpCB="`%s";_tag_fat(_tmpCB,sizeof(char),4U);});Cyc_aprintf(_tmp13A,_tag_fat(_tmpCA,sizeof(void*),1U));});});*_tmpCD=_tmp13B;});_tmpCD;});_tmpCE->name=_tmp13C;}),({
int _tmp139=Cyc_Tcutil_new_tvar_id();_tmpCE->identity=_tmp139;}),_tmpCE->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpCE;});
{struct Cyc_List_List*_tmpB3=(fd->i).tvars;for(0;_tmpB3 != 0;_tmpB3=_tmpB3->tl){
struct Cyc_Absyn_Kind*_tmpB4=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmpB3->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpB5=_tmpB4;enum Cyc_Absyn_KindQual _tmpB9;enum Cyc_Absyn_AliasQual _tmpB8;_LL4: _tmpB9=_tmpB5->kind;_tmpB8=_tmpB5->aliasqual;_LL5:;
if((int)_tmpB9 == (int)3U){
if((int)_tmpB8 == (int)0U)
_tmpD1=Cyc_RgnOrder_add_unordered(_tmpD1,(struct Cyc_Absyn_Tvar*)_tmpB3->hd);else{
# 537
({void*_tmpB6=0U;({struct _fat_ptr _tmp13D=({const char*_tmpB7="non-intuitionistic tvar in nested_fenv";_tag_fat(_tmpB7,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp13D,_tag_fat(_tmpB6,sizeof(void*),0U));});});}}}}
# 539
_tmpD1=Cyc_RgnOrder_add_youngest(_tmpD1,rgn0,0);
{struct Cyc_List_List*_tmpBA=(fd->i).rgn_po;for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
_tmpD1=Cyc_RgnOrder_add_outlives_constraint(_tmpD1,(*((struct _tuple14*)_tmpBA->hd)).f1,(*((struct _tuple14*)_tmpBA->hd)).f2,loc);}}{
struct Cyc_List_List*_tmpBB=({struct Cyc_List_List*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->hd=rgn0,({struct Cyc_List_List*_tmp13E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).tvars,_tmpD0);_tmpC9->tl=_tmp13E;});_tmpC9;});
Cyc_Tcutil_check_unique_tvars(loc,_tmpBB);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=2U,_tmpC8->f1=rgn0;_tmpC8;});
{struct Cyc_List_List*_tmpBC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){
((struct Cyc_Absyn_Vardecl*)_tmpBC->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpBD=*((struct Cyc_Absyn_VarargInfo*)_check_null((fd->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmpBE=_tmpBD;struct _fat_ptr*_tmpC4;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC2;int _tmpC1;_LL7: _tmpC4=_tmpBE.name;_tmpC3=_tmpBE.tq;_tmpC2=_tmpBE.type;_tmpC1=_tmpBE.inject;_LL8:;{
# 550
struct Cyc_List_List*_tmpBF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
struct Cyc_Absyn_Vardecl*_tmpC0=(struct Cyc_Absyn_Vardecl*)_tmpBF->hd;
if(Cyc_strptrcmp((*_tmpC0->name).f2,(struct _fat_ptr*)_check_null(_tmpC4))== 0){
({void*_tmp13F=Cyc_Absyn_fatptr_type(_tmpC2,param_rgn,_tmpC3,Cyc_Absyn_false_type);_tmpC0->type=_tmp13F;});
break;}}};}
# 558
return({struct Cyc_Tcenv_Fenv*_tmpC7=_cycalloc(sizeof(*_tmpC7));
({struct Cyc_Tcenv_SharedFenv*_tmp143=Cyc_Tcenv_new_shared_fenv((fd->i).ret_type);_tmpC7->shared=_tmp143;}),_tmpC7->type_vars=_tmpBB,_tmpC7->region_order=_tmpD1,_tmpC7->ctrl_env=0,({
# 563
void*_tmp142=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpC6=_cycalloc(sizeof(*_tmpC6));({void*_tmp141=Cyc_Absyn_access_eff(param_rgn);_tmpC6->hd=_tmp141;}),({
struct Cyc_List_List*_tmp140=({struct Cyc_List_List*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->hd=(void*)_check_null((fd->i).effect),_tmpC5->tl=0;_tmpC5;});_tmpC6->tl=_tmp140;});_tmpC6;}));
# 563
_tmpC7->capability=_tmp142;}),_tmpC7->curr_rgn=param_rgn,
# 566
(_tmpC7->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpC7->flags).in_notreadctxt=0,(_tmpC7->flags).in_lhs=0,(_tmpC7->flags).abstract_ok=0,(_tmpC7->flags).in_stmt_exp=0;_tmpC7;});};};};}
# 570
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args){
# 572
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct _fat_ptr*_tmp146=({struct _fat_ptr*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct _fat_ptr _tmp145=({const char*_tmpD8="bogus";_tag_fat(_tmpD8,sizeof(char),6U);});*_tmpD9=_tmp145;});_tmpD9;});_tmpDA->name=_tmp146;}),({int _tmp144=Cyc_Tcutil_new_tvar_id();_tmpDA->identity=_tmp144;}),_tmpDA->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpDA;});
struct Cyc_List_List*_tmpD2=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=rgn0,_tmpD7->tl=0;_tmpD7;});
struct Cyc_RgnOrder_RgnPO*_tmpD3=({void*_tmp147=Cyc_Absyn_join_eff(0);Cyc_RgnOrder_initial_fn_po(0,0,_tmp147,rgn0,0U);});
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=2U,_tmpD6->f1=rgn0;_tmpD6;});
return({struct Cyc_Tcenv_Fenv*_tmpD5=_cycalloc(sizeof(*_tmpD5));
({struct Cyc_Tcenv_SharedFenv*_tmp14A=Cyc_Tcenv_new_shared_fenv(ret_type);_tmpD5->shared=_tmp14A;}),_tmpD5->type_vars=_tmpD2,_tmpD5->region_order=_tmpD3,_tmpD5->ctrl_env=0,({
# 581
void*_tmp149=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));({void*_tmp148=Cyc_Absyn_access_eff(param_rgn);_tmpD4->hd=_tmp148;}),_tmpD4->tl=0;_tmpD4;}));_tmpD5->capability=_tmp149;}),_tmpD5->curr_rgn=param_rgn,
# 583
(_tmpD5->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpD5->flags).in_notreadctxt=0,(_tmpD5->flags).in_lhs=0,(_tmpD5->flags).abstract_ok=0,(_tmpD5->flags).in_stmt_exp=0;_tmpD5;});}

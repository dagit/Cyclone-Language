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
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 877 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 907
extern void*Cyc_Absyn_uint_type;
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1154
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 1160
unsigned Cyc_Absyn_exp_case_number(struct Cyc_Absyn_Exp*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
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
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 46
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 50
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 53
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple12{union Cyc_Absyn_Cnst f1;int f2;};
# 36 "evexp.cyc"
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 38
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;short _tmp2;enum Cyc_Absyn_Sign _tmp1;char _tmp4;enum Cyc_Absyn_Sign _tmp3;switch((_tmp0.Short_c).tag){case 2U: _LL1: _tmp3=((_tmp0.Char_c).val).f1;_tmp4=((_tmp0.Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sn=_tmp3;char c=_tmp4;
return Cyc_Absyn_Int_c(sn,(int)c);}case 4U: _LL3: _tmp1=((_tmp0.Short_c).val).f1;_tmp2=((_tmp0.Short_c).val).f2;_LL4: {enum Cyc_Absyn_Sign sn=_tmp1;short s=_tmp2;
# 42
return Cyc_Absyn_Int_c(sn,(int)s);}default: _LL5: _LL6:
 return cn;}_LL0:;}
# 47
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmp5=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp0=_tmp5;struct _tuple12 _tmp6=_stmttmp0;int _tmp8;union Cyc_Absyn_Cnst _tmp7;_LL1: _tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmp7;int known=_tmp8;
if(!known)
return({struct _tuple11 _tmp17C;_tmp17C.f1=0U,_tmp17C.f2=0;_tmp17C;});{
union Cyc_Absyn_Cnst _tmp9=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp1=_tmp9;union Cyc_Absyn_Cnst _tmpA=_stmttmp1;long long _tmpB;int _tmpC;switch((_tmpA.Null_c).tag){case 5U: _LL4: _tmpC=((_tmpA.Int_c).val).f2;_LL5: {int i=_tmpC;
return({struct _tuple11 _tmp17D;_tmp17D.f1=(unsigned)i,_tmp17D.f2=1;_tmp17D;});}case 6U: _LL6: _tmpB=((_tmpA.LongLong_c).val).f2;_LL7: {long long x=_tmpB;
# 55
unsigned long long y=(unsigned long long)x >> (unsigned long long)32;
if(y != (unsigned long long)-1 && y != (unsigned long long)0)
return({struct _tuple11 _tmp17E;_tmp17E.f1=0U,_tmp17E.f2=0;_tmp17E;});else{
return({struct _tuple11 _tmp17F;_tmp17F.f1=(unsigned)x,_tmp17F.f2=1;_tmp17F;});}}case 7U: _LL8: _LL9:
 return({struct _tuple11 _tmp180;_tmp180.f1=0U,_tmp180.f2=0;_tmp180;});case 1U: _LLA: _LLB:
 return({struct _tuple11 _tmp181;_tmp181.f1=0U,_tmp181.f2=1;_tmp181;});default: _LLC: _LLD:
 return({struct _tuple11 _tmp182;_tmp182.f1=0U,_tmp182.f2=0;_tmp182;});}_LL3:;}}}struct _tuple13{int f1;int f2;};
# 66
static struct _tuple13 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmpD=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp2=_tmpD;struct _tuple12 _tmpE=_stmttmp2;int _tmp10;union Cyc_Absyn_Cnst _tmpF;_LL1: _tmpF=_tmpE.f1;_tmp10=_tmpE.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmpF;int known=_tmp10;
if(!known)
return({struct _tuple13 _tmp183;_tmp183.f1=0,_tmp183.f2=0;_tmp183;});{
union Cyc_Absyn_Cnst _tmp11=Cyc_Evexp_promote_const(cn);union Cyc_Absyn_Cnst _stmttmp3=_tmp11;union Cyc_Absyn_Cnst _tmp12=_stmttmp3;long long _tmp13;int _tmp14;switch((_tmp12.Float_c).tag){case 5U: _LL4: _tmp14=((_tmp12.Int_c).val).f2;_LL5: {int b=_tmp14;
return({struct _tuple13 _tmp184;_tmp184.f1=b != 0,_tmp184.f2=1;_tmp184;});}case 6U: _LL6: _tmp13=((_tmp12.LongLong_c).val).f2;_LL7: {long long b=_tmp13;
return({struct _tuple13 _tmp185;_tmp185.f1=b != (long long)0,_tmp185.f2=1;_tmp185;});}case 1U: _LL8: _LL9:
 return({struct _tuple13 _tmp186;_tmp186.f1=0,_tmp186.f2=0;_tmp186;});case 7U: _LLA: _LLB:
 return({struct _tuple13 _tmp187;_tmp187.f1=0,_tmp187.f2=1;_tmp187;});default: _LLC: _LLD:
 return({struct _tuple13 _tmp188;_tmp188.f1=0,_tmp188.f2=0;_tmp188;});}_LL3:;}}}struct _tuple14{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 80
static struct _tuple12 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple12 _tmp15=Cyc_Evexp_eval_const_exp(e);struct _tuple12 _stmttmp4=_tmp15;struct _tuple12 _tmp16=_stmttmp4;int _tmp18;union Cyc_Absyn_Cnst _tmp17;_LL1: _tmp17=_tmp16.f1;_tmp18=_tmp16.f2;_LL2: {union Cyc_Absyn_Cnst cn=_tmp17;int known=_tmp18;
if(!known)
return({struct _tuple12 _tmp189;_tmp189.f1=cn,_tmp189.f2=0;_tmp189;});
{struct _tuple14 _tmp19=({struct _tuple14 _tmp18B;_tmp18B.f1=p,_tmp18B.f2=cn;_tmp18B;});struct _tuple14 _stmttmp5=_tmp19;struct _tuple14 _tmp1A=_stmttmp5;long long _tmp1B;char _tmp1C;short _tmp1D;int _tmp1E;long long _tmp20;enum Cyc_Absyn_Sign _tmp1F;char _tmp22;enum Cyc_Absyn_Sign _tmp21;short _tmp24;enum Cyc_Absyn_Sign _tmp23;int _tmp26;enum Cyc_Absyn_Sign _tmp25;long long _tmp28;enum Cyc_Absyn_Sign _tmp27;char _tmp2A;enum Cyc_Absyn_Sign _tmp29;short _tmp2C;enum Cyc_Absyn_Sign _tmp2B;int _tmp2E;enum Cyc_Absyn_Sign _tmp2D;switch(_tmp1A.f1){case Cyc_Absyn_Plus: _LL4: _LL5:
 goto _LL3;case Cyc_Absyn_Minus: switch(((_tmp1A.f2).LongLong_c).tag){case 5U: _LL6: _tmp2D=(((_tmp1A.f2).Int_c).val).f1;_tmp2E=(((_tmp1A.f2).Int_c).val).f2;_LL7: {enum Cyc_Absyn_Sign s=_tmp2D;int i=_tmp2E;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 4U: _LL8: _tmp2B=(((_tmp1A.f2).Short_c).val).f1;_tmp2C=(((_tmp1A.f2).Short_c).val).f2;_LL9: {enum Cyc_Absyn_Sign s=_tmp2B;short i=_tmp2C;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 2U: _LLA: _tmp29=(((_tmp1A.f2).Char_c).val).f1;_tmp2A=(((_tmp1A.f2).Char_c).val).f2;_LLB: {enum Cyc_Absyn_Sign s=_tmp29;char i=_tmp2A;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,- i);goto _LL3;}case 6U: _LLC: _tmp27=(((_tmp1A.f2).LongLong_c).val).f1;_tmp28=(((_tmp1A.f2).LongLong_c).val).f2;_LLD: {enum Cyc_Absyn_Sign s=_tmp27;long long i=_tmp28;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,- i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Bitnot: switch(((_tmp1A.f2).LongLong_c).tag){case 5U: _LLE: _tmp25=(((_tmp1A.f2).Int_c).val).f1;_tmp26=(((_tmp1A.f2).Int_c).val).f2;_LLF: {enum Cyc_Absyn_Sign s=_tmp25;int i=_tmp26;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 4U: _LL10: _tmp23=(((_tmp1A.f2).Short_c).val).f1;_tmp24=(((_tmp1A.f2).Short_c).val).f2;_LL11: {enum Cyc_Absyn_Sign s=_tmp23;short i=_tmp24;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 2U: _LL12: _tmp21=(((_tmp1A.f2).Char_c).val).f1;_tmp22=(((_tmp1A.f2).Char_c).val).f2;_LL13: {enum Cyc_Absyn_Sign s=_tmp21;char i=_tmp22;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}case 6U: _LL14: _tmp1F=(((_tmp1A.f2).LongLong_c).val).f1;_tmp20=(((_tmp1A.f2).LongLong_c).val).f2;_LL15: {enum Cyc_Absyn_Sign s=_tmp1F;long long i=_tmp20;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Unsigned,~ i);goto _LL3;}default: goto _LL20;}case Cyc_Absyn_Not: switch(((_tmp1A.f2).Null_c).tag){case 5U: _LL16: _tmp1E=(((_tmp1A.f2).Int_c).val).f2;_LL17: {int i=_tmp1E;
cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i == 0?1: 0);goto _LL3;}case 4U: _LL18: _tmp1D=(((_tmp1A.f2).Short_c).val).f2;_LL19: {short i=_tmp1D;
cn=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,(int)i == 0?1: 0);goto _LL3;}case 2U: _LL1A: _tmp1C=(((_tmp1A.f2).Char_c).val).f2;_LL1B: {char i=_tmp1C;
cn=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,(int)i == 0?'\001':'\000');goto _LL3;}case 6U: _LL1C: _tmp1B=(((_tmp1A.f2).LongLong_c).val).f2;_LL1D: {long long i=_tmp1B;
cn=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,(long long)(i == (long long)0?1: 0));goto _LL3;}case 1U: _LL1E: _LL1F:
 cn=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL3;default: goto _LL20;}default: _LL20: _LL21:
 return({struct _tuple12 _tmp18A;_tmp18A.f1=cn,_tmp18A.f2=0;_tmp18A;});}_LL3:;}
# 101
return({struct _tuple12 _tmp18C;_tmp18C.f1=cn,_tmp18C.f2=1;_tmp18C;});}}struct _tuple15{enum Cyc_Absyn_Primop f1;int f2;};
# 105
static struct _tuple12 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _tmp2F=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp6=_tmp2F;struct _tuple12 _tmp30=_stmttmp6;int _tmp32;union Cyc_Absyn_Cnst _tmp31;_LL1: _tmp31=_tmp30.f1;_tmp32=_tmp30.f2;_LL2: {union Cyc_Absyn_Cnst cn1=_tmp31;int known1=_tmp32;
struct _tuple12 _tmp33=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp7=_tmp33;struct _tuple12 _tmp34=_stmttmp7;int _tmp36;union Cyc_Absyn_Cnst _tmp35;_LL4: _tmp35=_tmp34.f1;_tmp36=_tmp34.f2;_LL5: {union Cyc_Absyn_Cnst cn2=_tmp35;int known2=_tmp36;
if(!known1 || !known2)
return({struct _tuple12 _tmp18D;_tmp18D.f1=cn1,_tmp18D.f2=0;_tmp18D;});
cn1=Cyc_Evexp_promote_const(cn1);
cn2=Cyc_Evexp_promote_const(cn2);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp37=cn1;int _tmp39;enum Cyc_Absyn_Sign _tmp38;if((_tmp37.Int_c).tag == 5){_LL7: _tmp38=((_tmp37.Int_c).val).f1;_tmp39=((_tmp37.Int_c).val).f2;_LL8: {enum Cyc_Absyn_Sign x=_tmp38;int y=_tmp39;
s1=x;i1=y;goto _LL6;}}else{_LL9: _LLA:
 return({struct _tuple12 _tmp18E;_tmp18E.f1=cn1,_tmp18E.f2=0;_tmp18E;});}_LL6:;}
# 118
{union Cyc_Absyn_Cnst _tmp3A=cn2;int _tmp3C;enum Cyc_Absyn_Sign _tmp3B;if((_tmp3A.Int_c).tag == 5){_LLC: _tmp3B=((_tmp3A.Int_c).val).f1;_tmp3C=((_tmp3A.Int_c).val).f2;_LLD: {enum Cyc_Absyn_Sign x=_tmp3B;int y=_tmp3C;
s2=x;i2=y;goto _LLB;}}else{_LLE: _LLF:
 return({struct _tuple12 _tmp18F;_tmp18F.f1=cn1,_tmp18F.f2=0;_tmp18F;});}_LLB:;}
# 122
{enum Cyc_Absyn_Primop _tmp3D=p;switch(_tmp3D){case Cyc_Absyn_Div: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Mod: _LL13: _LL14:
# 125
 if(i2 == 0){
({void*_tmp3E=0U;({unsigned _tmp1AC=e2->loc;struct _fat_ptr _tmp1AB=({const char*_tmp3F="division by zero in constant expression";_tag_fat(_tmp3F,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp1AC,_tmp1AB,_tag_fat(_tmp3E,sizeof(void*),0U));});});
return({struct _tuple12 _tmp190;_tmp190.f1=cn1,_tmp190.f2=1;_tmp190;});}
# 129
goto _LL10;default: _LL15: _LL16:
 goto _LL10;}_LL10:;}{
# 132
int has_u_arg=(int)s1 == (int)1U ||(int)s2 == (int)Cyc_Absyn_Unsigned;
unsigned u1=(unsigned)i1;
unsigned u2=(unsigned)i2;
int i3=0;
unsigned u3=0U;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple15 _tmp40=({struct _tuple15 _tmp192;_tmp192.f1=p,_tmp192.f2=has_u_arg;_tmp192;});struct _tuple15 _stmttmp8=_tmp40;struct _tuple15 _tmp41=_stmttmp8;switch(_tmp41.f1){case Cyc_Absyn_Plus: switch(_tmp41.f2){case 0U: _LL18: _LL19:
 i3=i1 + i2;use_i3=1;goto _LL17;case 1U: _LL22: _LL23:
# 147
 u3=u1 + u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Times: switch(_tmp41.f2){case 0U: _LL1A: _LL1B:
# 143
 i3=i1 * i2;use_i3=1;goto _LL17;case 1U: _LL24: _LL25:
# 148
 u3=u1 * u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Minus: switch(_tmp41.f2){case 0U: _LL1C: _LL1D:
# 144
 i3=i1 - i2;use_i3=1;goto _LL17;case 1U: _LL26: _LL27:
# 149
 u3=u1 - u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Div: switch(_tmp41.f2){case 0U: _LL1E: _LL1F:
# 145
 i3=i1 / i2;use_i3=1;goto _LL17;case 1U: _LL28: _LL29:
# 150
 u3=u1 / u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Mod: switch(_tmp41.f2){case 0U: _LL20: _LL21:
# 146
 i3=i1 % i2;use_i3=1;goto _LL17;case 1U: _LL2A: _LL2B:
# 151
 u3=u1 % u2;use_u3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Eq: _LL2C: _LL2D:
 b3=i1 == i2;use_b3=1;goto _LL17;case Cyc_Absyn_Neq: _LL2E: _LL2F:
 b3=i1 != i2;use_b3=1;goto _LL17;case Cyc_Absyn_Gt: switch(_tmp41.f2){case 0U: _LL30: _LL31:
 b3=i1 > i2;use_b3=1;goto _LL17;case 1U: _LL38: _LL39:
# 158
 b3=u1 > u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lt: switch(_tmp41.f2){case 0U: _LL32: _LL33:
# 155
 b3=i1 < i2;use_b3=1;goto _LL17;case 1U: _LL3A: _LL3B:
# 159
 b3=u1 < u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Gte: switch(_tmp41.f2){case 0U: _LL34: _LL35:
# 156
 b3=i1 >= i2;use_b3=1;goto _LL17;case 1U: _LL3C: _LL3D:
# 160
 b3=u1 >= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Lte: switch(_tmp41.f2){case 0U: _LL36: _LL37:
# 157
 b3=i1 <= i2;use_b3=1;goto _LL17;case 1U: _LL3E: _LL3F:
# 161
 b3=u1 <= u2;use_b3=1;goto _LL17;default: goto _LL4A;}case Cyc_Absyn_Bitand: _LL40: _LL41:
 u3=u1 & u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitor: _LL42: _LL43:
 u3=u1 | u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitxor: _LL44: _LL45:
 u3=u1 ^ u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlshift: _LL46: _LL47:
 u3=u1 << u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlrshift: _LL48: _LL49:
 u3=u1 >> u2;use_u3=1;goto _LL17;default: _LL4A: _LL4B:
({void*_tmp42=0U;({unsigned _tmp1AE=e1->loc;struct _fat_ptr _tmp1AD=({const char*_tmp43="bad constant expression";_tag_fat(_tmp43,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp1AE,_tmp1AD,_tag_fat(_tmp42,sizeof(void*),0U));});});return({struct _tuple12 _tmp191;_tmp191.f1=cn1,_tmp191.f2=1;_tmp191;});}_LL17:;}
# 169
if(use_i3)return({struct _tuple12 _tmp193;({union Cyc_Absyn_Cnst _tmp1AF=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3);_tmp193.f1=_tmp1AF;}),_tmp193.f2=1;_tmp193;});
if(use_u3)return({struct _tuple12 _tmp194;({union Cyc_Absyn_Cnst _tmp1B0=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3);_tmp194.f1=_tmp1B0;}),_tmp194.f2=1;_tmp194;});
if(use_b3)return({struct _tuple12 _tmp195;({union Cyc_Absyn_Cnst _tmp1B1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0);_tmp195.f1=_tmp1B1;}),_tmp195.f2=1;_tmp195;});
({void*_tmp44=0U;({struct _fat_ptr _tmp1B2=({const char*_tmp45="Evexp::eval_const_binop";_tag_fat(_tmp45,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp1B2,_tag_fat(_tmp44,sizeof(void*),0U));});});}}}}}struct _tuple16{void*f1;union Cyc_Absyn_Cnst f2;};
# 178
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple12 ans;
{void*_tmp46=e->r;void*_stmttmp9=_tmp46;void*_tmp47=_stmttmp9;struct Cyc_Absyn_Enumfield*_tmp48;struct Cyc_Absyn_Enumfield*_tmp49;struct Cyc_Absyn_Exp*_tmp4B;void*_tmp4A;void*_tmp4C;struct Cyc_List_List*_tmp4E;enum Cyc_Absyn_Primop _tmp4D;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_Absyn_Exp*_tmp4F;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Exp*_tmp51;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp53;union Cyc_Absyn_Cnst _tmp56;switch(*((int*)_tmp47)){case 1U: _LL1: _LL2:
# 182
 return({struct _tuple12 _tmp196;({union Cyc_Absyn_Cnst _tmp1B3=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp196.f1=_tmp1B3;}),_tmp196.f2=0;_tmp196;});case 0U: _LL3: _tmp56=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_LL4: {union Cyc_Absyn_Cnst c=_tmp56;
return({struct _tuple12 _tmp197;_tmp197.f1=c,_tmp197.f2=1;_tmp197;});}case 2U: _LL5: _LL6:
 return({struct _tuple12 _tmp198;({union Cyc_Absyn_Cnst _tmp1B4=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp198.f1=_tmp1B4;}),_tmp198.f2=1;_tmp198;});case 6U: _LL7: _tmp53=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_tmp54=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_tmp55=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp53;struct Cyc_Absyn_Exp*e2=_tmp54;struct Cyc_Absyn_Exp*e3=_tmp55;
# 186
struct _tuple13 _tmp57=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpA=_tmp57;struct _tuple13 _tmp58=_stmttmpA;int _tmp5A;int _tmp59;_LL20: _tmp59=_tmp58.f1;_tmp5A=_tmp58.f2;_LL21: {int bool1=_tmp59;int known1=_tmp5A;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
Cyc_Evexp_eval_const_exp(e3);
return({struct _tuple12 _tmp199;({union Cyc_Absyn_Cnst _tmp1B5=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp199.f1=_tmp1B5;}),_tmp199.f2=0;_tmp199;});}
# 192
ans=bool1?Cyc_Evexp_eval_const_exp(e2): Cyc_Evexp_eval_const_exp(e3);
goto _LL0;}}case 7U: _LL9: _tmp51=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_tmp52=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
# 195
struct _tuple13 _tmp5B=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpB=_tmp5B;struct _tuple13 _tmp5C=_stmttmpB;int _tmp5E;int _tmp5D;_LL23: _tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;_LL24: {int bool1=_tmp5D;int known1=_tmp5E;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _tmp19A;({union Cyc_Absyn_Cnst _tmp1B6=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp19A.f1=_tmp1B6;}),_tmp19A.f2=0;_tmp19A;});}
# 200
if(bool1)ans=Cyc_Evexp_eval_const_exp(e2);else{
ans=({struct _tuple12 _tmp19B;({union Cyc_Absyn_Cnst _tmp1B7=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp19B.f1=_tmp1B7;}),_tmp19B.f2=1;_tmp19B;});}
goto _LL0;}}case 8U: _LLB: _tmp4F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_tmp50=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp4F;struct Cyc_Absyn_Exp*e2=_tmp50;
# 204
struct _tuple13 _tmp5F=Cyc_Evexp_eval_const_bool_exp(e1);struct _tuple13 _stmttmpC=_tmp5F;struct _tuple13 _tmp60=_stmttmpC;int _tmp62;int _tmp61;_LL26: _tmp61=_tmp60.f1;_tmp62=_tmp60.f2;_LL27: {int bool1=_tmp61;int known1=_tmp62;
if(!known1){
Cyc_Evexp_eval_const_exp(e2);
return({struct _tuple12 _tmp19C;({union Cyc_Absyn_Cnst _tmp1B8=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp19C.f1=_tmp1B8;}),_tmp19C.f2=0;_tmp19C;});}
# 209
if(!bool1)ans=Cyc_Evexp_eval_const_exp(e2);else{
ans=({struct _tuple12 _tmp19D;({union Cyc_Absyn_Cnst _tmp1B9=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);_tmp19D.f1=_tmp1B9;}),_tmp19D.f2=1;_tmp19D;});}
goto _LL0;}}case 3U: _LLD: _tmp4D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_tmp4E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp4D;struct Cyc_List_List*es=_tmp4E;
# 213
if(es == 0){
({void*_tmp63=0U;({unsigned _tmp1BB=e->loc;struct _fat_ptr _tmp1BA=({const char*_tmp64="bad static expression (no args to primop)";_tag_fat(_tmp64,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp1BB,_tmp1BA,_tag_fat(_tmp63,sizeof(void*),0U));});});
return({struct _tuple12 _tmp19E;({union Cyc_Absyn_Cnst _tmp1BC=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp19E.f1=_tmp1BC;}),_tmp19E.f2=1;_tmp19E;});}
# 217
if(es->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(p,(struct Cyc_Absyn_Exp*)es->hd);
goto _LL0;}
# 221
if(((struct Cyc_List_List*)_check_null(es->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(p,(struct Cyc_Absyn_Exp*)es->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);
goto _LL0;}
# 225
({void*_tmp65=0U;({unsigned _tmp1BE=e->loc;struct _fat_ptr _tmp1BD=({const char*_tmp66="bad static expression (too many args to primop)";_tag_fat(_tmp66,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp1BE,_tmp1BD,_tag_fat(_tmp65,sizeof(void*),0U));});});
return({struct _tuple12 _tmp19F;({union Cyc_Absyn_Cnst _tmp1BF=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp19F.f1=_tmp1BF;}),_tmp19F.f2=1;_tmp19F;});}case 39U: _LLF: _tmp4C=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_LL10: {void*t=_tmp4C;
# 229
{void*_tmp67=Cyc_Tcutil_compress(t);void*_stmttmpD=_tmp67;void*_tmp68=_stmttmpD;struct Cyc_Absyn_Exp*_tmp69;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68)->tag == 9U){_LL29: _tmp69=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68)->f1;_LL2A: {struct Cyc_Absyn_Exp*e2=_tmp69;
# 231
e->r=e2->r;
return Cyc_Evexp_eval_const_exp(e2);}}else{_LL2B: _LL2C:
 goto _LL28;}_LL28:;}
# 235
goto _LL12;}case 17U: _LL11: _LL12:
 goto _LL14;case 18U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 ans=({struct _tuple12 _tmp1A0;({union Cyc_Absyn_Cnst _tmp1C0=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A0.f1=_tmp1C0;}),_tmp1A0.f2=0;_tmp1A0;});goto _LL0;case 14U: _LL17: _tmp4A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_tmp4B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_LL18: {void*t=_tmp4A;struct Cyc_Absyn_Exp*e2=_tmp4B;
# 241
ans=Cyc_Evexp_eval_const_exp(e2);
if(ans.f2){
struct _tuple16 _tmp6A=({struct _tuple16 _tmp1A3;({void*_tmp1C1=Cyc_Tcutil_compress(t);_tmp1A3.f1=_tmp1C1;}),_tmp1A3.f2=ans.f1;_tmp1A3;});struct _tuple16 _stmttmpE=_tmp6A;struct _tuple16 _tmp6B=_stmttmpE;int _tmp6E;enum Cyc_Absyn_Sign _tmp6D;void*_tmp6C;short _tmp71;enum Cyc_Absyn_Sign _tmp70;void*_tmp6F;char _tmp74;enum Cyc_Absyn_Sign _tmp73;void*_tmp72;int _tmp78;enum Cyc_Absyn_Sign _tmp77;enum Cyc_Absyn_Size_of _tmp76;enum Cyc_Absyn_Sign _tmp75;short _tmp7C;enum Cyc_Absyn_Sign _tmp7B;enum Cyc_Absyn_Size_of _tmp7A;enum Cyc_Absyn_Sign _tmp79;char _tmp80;enum Cyc_Absyn_Sign _tmp7F;enum Cyc_Absyn_Size_of _tmp7E;enum Cyc_Absyn_Sign _tmp7D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)){case 1U: switch(((_tmp6B.f2).Int_c).tag){case 2U: _LL2E: _tmp7D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)->f1;_tmp7E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)->f2;_tmp7F=(((_tmp6B.f2).Char_c).val).f1;_tmp80=(((_tmp6B.f2).Char_c).val).f2;_LL2F: {enum Cyc_Absyn_Sign sn=_tmp7D;enum Cyc_Absyn_Size_of sz=_tmp7E;enum Cyc_Absyn_Sign sn2=_tmp7F;char x=_tmp80;
# 246
_tmp79=sn;_tmp7A=sz;_tmp7B=sn2;_tmp7C=(short)x;goto _LL31;}case 4U: _LL30: _tmp79=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)->f1;_tmp7A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)->f2;_tmp7B=(((_tmp6B.f2).Short_c).val).f1;_tmp7C=(((_tmp6B.f2).Short_c).val).f2;_LL31: {enum Cyc_Absyn_Sign sn=_tmp79;enum Cyc_Absyn_Size_of sz=_tmp7A;enum Cyc_Absyn_Sign sn2=_tmp7B;short x=_tmp7C;
# 248
_tmp75=sn;_tmp76=sz;_tmp77=sn2;_tmp78=(int)x;goto _LL33;}case 5U: _LL32: _tmp75=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)->f1;_tmp76=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f1)->f2;_tmp77=(((_tmp6B.f2).Int_c).val).f1;_tmp78=(((_tmp6B.f2).Int_c).val).f2;_LL33: {enum Cyc_Absyn_Sign sn=_tmp75;enum Cyc_Absyn_Size_of sz=_tmp76;enum Cyc_Absyn_Sign sn2=_tmp77;int x=_tmp78;
# 250
if((int)sn != (int)sn2)
({union Cyc_Absyn_Cnst _tmp1C2=Cyc_Absyn_Int_c(sn,x);ans.f1=_tmp1C2;});
goto _LL2D;}default: goto _LL3A;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f2 != 0)switch(((_tmp6B.f2).Int_c).tag){case 2U: _LL34: _tmp72=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f2)->hd;_tmp73=(((_tmp6B.f2).Char_c).val).f1;_tmp74=(((_tmp6B.f2).Char_c).val).f2;_LL35: {void*it=_tmp72;enum Cyc_Absyn_Sign sn2=_tmp73;char x=_tmp74;
# 255
_tmp6F=it;_tmp70=sn2;_tmp71=(short)x;goto _LL37;}case 4U: _LL36: _tmp6F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f2)->hd;_tmp70=(((_tmp6B.f2).Short_c).val).f1;_tmp71=(((_tmp6B.f2).Short_c).val).f2;_LL37: {void*it=_tmp6F;enum Cyc_Absyn_Sign sn2=_tmp70;short x=_tmp71;
# 257
_tmp6C=it;_tmp6D=sn2;_tmp6E=(int)x;goto _LL39;}case 5U: _LL38: _tmp6C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B.f1)->f2)->hd;_tmp6D=(((_tmp6B.f2).Int_c).val).f1;_tmp6E=(((_tmp6B.f2).Int_c).val).f2;_LL39: {void*it=_tmp6C;enum Cyc_Absyn_Sign sn2=_tmp6D;int x=_tmp6E;
# 259
if(x < 0)
# 261
return({struct _tuple12 _tmp1A1;({union Cyc_Absyn_Cnst _tmp1C3=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A1.f1=_tmp1C3;}),_tmp1A1.f2=0;_tmp1A1;});
({void*_tmp1C5=it;Cyc_Unify_unify(_tmp1C5,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=9U,({struct Cyc_Absyn_Exp*_tmp1C4=Cyc_Absyn_const_exp(ans.f1,0U);_tmp81->f1=_tmp1C4;});_tmp81;}));});
({union Cyc_Absyn_Cnst _tmp1C6=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,x);ans.f1=_tmp1C6;});
goto _LL2D;}default: goto _LL3A;}else{goto _LL3A;}default: goto _LL3A;}else{_LL3A: _LL3B:
 return({struct _tuple12 _tmp1A2;({union Cyc_Absyn_Cnst _tmp1C7=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A2.f1=_tmp1C7;}),_tmp1A2.f2=0;_tmp1A2;});}_LL2D:;}
# 268
goto _LL0;}case 33U: _LL19: _tmp49=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_LL1A: {struct Cyc_Absyn_Enumfield*ef=_tmp49;
_tmp48=ef;goto _LL1C;}case 32U: _LL1B: _tmp48=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_tmp48;
ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));goto _LL0;}default: _LL1D: _LL1E:
# 273
 return({struct _tuple12 _tmp1A4;({union Cyc_Absyn_Cnst _tmp1C8=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp1A4.f1=_tmp1C8;}),_tmp1A4.f2=0;_tmp1A4;});}_LL0:;}
# 275
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmp82=ans.f1;{union Cyc_Absyn_Cnst c=_tmp82;
({void*_tmp1C9=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=0U,_tmp83->f1=c;_tmp83;});e->r=_tmp1C9;});}}
# 280
return ans;}
# 283
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmp84=e->r;void*_stmttmpF=_tmp84;void*_tmp85=_stmttmpF;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_List_List*_tmp88;enum Cyc_Absyn_Primop _tmp87;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;switch(*((int*)_tmp85)){case 33U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 0U: _LLB: _LLC:
 return 1;case 6U: _LLD: _tmp8D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp85)->f1;_tmp8E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp85)->f2;_tmp8F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp85)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp8D;struct Cyc_Absyn_Exp*e2=_tmp8E;struct Cyc_Absyn_Exp*e3=_tmp8F;
# 292
return(Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2))&& Cyc_Evexp_c_can_eval(e3);}case 7U: _LLF: _tmp8B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp85)->f1;_tmp8C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp85)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp8B;struct Cyc_Absyn_Exp*e2=_tmp8C;
_tmp89=e1;_tmp8A=e2;goto _LL12;}case 8U: _LL11: _tmp89=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp85)->f1;_tmp8A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp85)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp89;struct Cyc_Absyn_Exp*e2=_tmp8A;
# 295
return Cyc_Evexp_c_can_eval(e1)&& Cyc_Evexp_c_can_eval(e2);}case 3U: _LL13: _tmp87=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp85)->f1;_tmp88=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp85)->f2;_LL14: {enum Cyc_Absyn_Primop p=_tmp87;struct Cyc_List_List*es=_tmp88;
# 297
for(0;es != 0;es=es->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)es->hd))return 0;}
return 1;}case 39U: _LL15: _LL16:
 return 0;case 14U: _LL17: _tmp86=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp85)->f2;_LL18: {struct Cyc_Absyn_Exp*e=_tmp86;
return Cyc_Evexp_c_can_eval(e);}default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 307
static struct Cyc_Absyn_Exp*Cyc_Evexp_strip_cast(struct Cyc_Absyn_Exp*e){
{void*_tmp90=e->r;void*_stmttmp10=_tmp90;void*_tmp91=_stmttmp10;struct Cyc_Absyn_Exp*_tmp93;void*_tmp92;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp91)->tag == 14U){_LL1: _tmp92=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp91)->f1;_tmp93=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp91)->f2;_LL2: {void*t=_tmp92;struct Cyc_Absyn_Exp*e2=_tmp93;
# 310
if(Cyc_Unify_unify(t,Cyc_Absyn_uint_type)){
void*_tmp94=e2->r;void*_stmttmp11=_tmp94;void*_tmp95=_stmttmp11;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp95)->tag == 39U){_LL6: _LL7:
 return e2;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 316
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 319
return e;}
# 322
static unsigned long long Cyc_Evexp_integral_const_to_long_long(union Cyc_Absyn_Cnst c){
unsigned long long res;
{union Cyc_Absyn_Cnst _tmp96=c;long long _tmp97;int _tmp98;short _tmp99;char _tmp9A;switch((_tmp96.LongLong_c).tag){case 2U: _LL1: _tmp9A=((_tmp96.Char_c).val).f2;_LL2: {char c=_tmp9A;
res=(unsigned long long)c;goto _LL0;}case 4U: _LL3: _tmp99=((_tmp96.Short_c).val).f2;_LL4: {short s=_tmp99;
res=(unsigned long long)s;goto _LL0;}case 5U: _LL5: _tmp98=((_tmp96.Int_c).val).f2;_LL6: {int i=_tmp98;
res=(unsigned long long)i;goto _LL0;}case 6U: _LL7: _tmp97=((_tmp96.LongLong_c).val).f2;_LL8: {long long x=_tmp97;
res=(unsigned long long)x;goto _LL0;}default: _LL9: _LLA:
({void*_tmp9B=0U;({struct _fat_ptr _tmp1CA=({const char*_tmp9C="Evexp::integral_const_to_long_long";_tag_fat(_tmp9C,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp1CA,_tag_fat(_tmp9B,sizeof(void*),0U));});});}_LL0:;}
# 331
return res;}struct _tuple17{union Cyc_Absyn_Cnst f1;union Cyc_Absyn_Cnst f2;};
# 334
static int Cyc_Evexp_cmp_cnst(union Cyc_Absyn_Cnst c1,union Cyc_Absyn_Cnst c2){
struct _tuple17 _tmp9D=({struct _tuple17 _tmp1A5;_tmp1A5.f1=c1,_tmp1A5.f2=c2;_tmp1A5;});struct _tuple17 _stmttmp12=_tmp9D;struct _tuple17 _tmp9E=_stmttmp12;int _tmpA2;struct _fat_ptr _tmpA1;int _tmpA0;struct _fat_ptr _tmp9F;struct _fat_ptr _tmpA3;struct _fat_ptr _tmpA4;struct _fat_ptr _tmpA6;struct _fat_ptr _tmpA5;struct _fat_ptr _tmpA8;struct _fat_ptr _tmpA7;struct _fat_ptr _tmpAA;struct _fat_ptr _tmpA9;if(((_tmp9E.f1).Null_c).tag == 1){if(((_tmp9E.f2).Null_c).tag == 1){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((_tmp9E.f2).Null_c).tag == 1){_LL5: _LL6:
 return 1;}else{if(((_tmp9E.f1).Wchar_c).tag == 3){if(((_tmp9E.f2).Wchar_c).tag == 3){_LL7: _tmpA9=((_tmp9E.f1).Wchar_c).val;_tmpAA=((_tmp9E.f2).Wchar_c).val;_LL8: {struct _fat_ptr s1=_tmpA9;struct _fat_ptr s2=_tmpAA;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LL9: _LLA:
 return - 1;}}else{if(((_tmp9E.f2).Wchar_c).tag == 3){_LLB: _LLC:
 return 1;}else{if(((_tmp9E.f1).String_c).tag == 8){if(((_tmp9E.f2).String_c).tag == 8){_LLD: _tmpA7=((_tmp9E.f1).String_c).val;_tmpA8=((_tmp9E.f2).String_c).val;_LLE: {struct _fat_ptr s1=_tmpA7;struct _fat_ptr s2=_tmpA8;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LLF: _LL10:
 return - 1;}}else{if(((_tmp9E.f2).String_c).tag == 8){_LL11: _LL12:
 return 1;}else{if(((_tmp9E.f1).Wstring_c).tag == 9){if(((_tmp9E.f2).Wstring_c).tag == 9){_LL13: _tmpA5=((_tmp9E.f1).Wstring_c).val;_tmpA6=((_tmp9E.f2).Wstring_c).val;_LL14: {struct _fat_ptr s1=_tmpA5;struct _fat_ptr s2=_tmpA6;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LL15: _tmpA4=((_tmp9E.f1).Wstring_c).val;_LL16: {struct _fat_ptr s1=_tmpA4;
return - 1;}}}else{if(((_tmp9E.f2).Wstring_c).tag == 9){_LL17: _tmpA3=((_tmp9E.f2).Wstring_c).val;_LL18: {struct _fat_ptr s1=_tmpA3;
return 1;}}else{if(((_tmp9E.f1).Float_c).tag == 7){if(((_tmp9E.f2).Float_c).tag == 7){_LL19: _tmp9F=(((_tmp9E.f1).Float_c).val).f1;_tmpA0=(((_tmp9E.f1).Float_c).val).f2;_tmpA1=(((_tmp9E.f2).Float_c).val).f1;_tmpA2=(((_tmp9E.f2).Float_c).val).f2;_LL1A: {struct _fat_ptr s1=_tmp9F;int i1=_tmpA0;struct _fat_ptr s2=_tmpA1;int i2=_tmpA2;
# 349
if(i1 != i2)return i1 - i2;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{_LL1B: _LL1C:
 return - 1;}}else{if(((_tmp9E.f2).Float_c).tag == 7){_LL1D: _LL1E:
 return 1;}else{_LL1F: _LL20: {
# 354
unsigned long long i1=Cyc_Evexp_integral_const_to_long_long(c1);
unsigned long long i2=Cyc_Evexp_integral_const_to_long_long(c2);
long long diff=(long long)(i1 - i2);
return diff == (long long)0?0:(diff < (long long)0?- 1: 1);}}}}}}}}}}}_LL0:;}
# 366
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _tuple18{void*f1;void*f2;};
# 368
int Cyc_Evexp_designator_cmp(void*d1,void*d2){
struct _tuple18 _tmpAB=({struct _tuple18 _tmp1A6;_tmp1A6.f1=d1,_tmp1A6.f2=d2;_tmp1A6;});struct _tuple18 _stmttmp13=_tmpAB;struct _tuple18 _tmpAC=_stmttmp13;struct _fat_ptr*_tmpAE;struct _fat_ptr*_tmpAD;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB1;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC.f1)->tag == 0U){if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC.f2)->tag == 0U){_LL1: _tmpB1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC.f1)->f1;_tmpB2=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC.f2)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmpB1;struct Cyc_Absyn_Exp*e2=_tmpB2;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{_LL3: _tmpB0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC.f1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmpB0;
return - 1;}}}else{if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC.f2)->tag == 0U){_LL5: _tmpAF=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC.f2)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmpAF;
return 1;}}else{_LL7: _tmpAD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpAC.f1)->f1;_tmpAE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpAC.f2)->f1;_LL8: {struct _fat_ptr*v1=_tmpAD;struct _fat_ptr*v2=_tmpAE;
return Cyc_strcmp((struct _fat_ptr)*v1,(struct _fat_ptr)*v2);}}}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 377
int Cyc_Evexp_designator_exp_cmp(struct _tuple19*d1,struct _tuple19*d2){
# 379
struct _tuple19 _tmpB3=*d1;struct _tuple19 _stmttmp14=_tmpB3;struct _tuple19 _tmpB4=_stmttmp14;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_List_List*_tmpB5;_LL1: _tmpB5=_tmpB4.f1;_tmpB6=_tmpB4.f2;_LL2: {struct Cyc_List_List*des1=_tmpB5;struct Cyc_Absyn_Exp*e1=_tmpB6;
struct _tuple19 _tmpB7=*d2;struct _tuple19 _stmttmp15=_tmpB7;struct _tuple19 _tmpB8=_stmttmp15;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_List_List*_tmpB9;_LL4: _tmpB9=_tmpB8.f1;_tmpBA=_tmpB8.f2;_LL5: {struct Cyc_List_List*des2=_tmpB9;struct Cyc_Absyn_Exp*e2=_tmpBA;
int _tmpBB=Cyc_Evexp_exp_cmp(e1,e2);int c1=_tmpBB;
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_designator_cmp,des1,des2);}}}
# 386
struct Cyc_Absyn_Vardecl*Cyc_Evexp_binding2vardecl(void*b){
void*_tmpBC=b;struct Cyc_Absyn_Vardecl*_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;struct Cyc_Absyn_Vardecl*_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC0;struct Cyc_Absyn_Vardecl*_tmpC1;switch(*((int*)_tmpBC)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _tmpC1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpBC)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmpC1;
return vd;}case 2U: _LL5: _tmpC0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpBC)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmpC0;
return fd->fn_vardecl;}case 3U: _LL7: _tmpBF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpBC)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpBF;
return vd;}case 4U: _LL9: _tmpBE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpBC)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmpBE;
return vd;}default: _LLB: _tmpBD=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpBC)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmpBD;
return vd;}}_LL0:;}struct _tuple20{struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Vardecl*f2;};
# 397
int Cyc_Evexp_cmp_binding(void*b1,void*b2){
struct Cyc_Absyn_Vardecl*_tmpC2=Cyc_Evexp_binding2vardecl(b1);struct Cyc_Absyn_Vardecl*vdopt1=_tmpC2;
struct Cyc_Absyn_Vardecl*_tmpC3=Cyc_Evexp_binding2vardecl(b2);struct Cyc_Absyn_Vardecl*vdopt2=_tmpC3;
struct _tuple20 _tmpC4=({struct _tuple20 _tmp1A7;_tmp1A7.f1=vdopt1,_tmp1A7.f2=vdopt2;_tmp1A7;});struct _tuple20 _stmttmp16=_tmpC4;struct _tuple20 _tmpC5=_stmttmp16;if(_tmpC5.f1 == 0){if(_tmpC5.f2 == 0){_LL1: _LL2:
 return({struct _tuple0*_tmp1CB=Cyc_Absyn_binding2qvar(b1);Cyc_Absyn_qvar_cmp(_tmp1CB,Cyc_Absyn_binding2qvar(b2));});}else{_LL3: _LL4:
 return - 1;}}else{if(_tmpC5.f2 == 0){_LL5: _LL6:
 return 1;}else{_LL7: _LL8:
 return(int)vdopt1 - (int)vdopt2;}}_LL0:;}
# 411
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple12 _tmpC6=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp17=_tmpC6;struct _tuple12 _tmpC7=_stmttmp17;int _tmpC9;union Cyc_Absyn_Cnst _tmpC8;_LL1: _tmpC8=_tmpC7.f1;_tmpC9=_tmpC7.f2;_LL2: {union Cyc_Absyn_Cnst c1=_tmpC8;int known1=_tmpC9;
struct _tuple12 _tmpCA=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp18=_tmpCA;struct _tuple12 _tmpCB=_stmttmp18;int _tmpCD;union Cyc_Absyn_Cnst _tmpCC;_LL4: _tmpCC=_tmpCB.f1;_tmpCD=_tmpCB.f2;_LL5: {union Cyc_Absyn_Cnst c2=_tmpCC;int known2=_tmpCD;
if(known1 && known2)return Cyc_Evexp_cmp_cnst(c1,c2);{
unsigned _tmpCE=Cyc_Absyn_exp_case_number(e1);unsigned e1case=_tmpCE;
unsigned _tmpCF=Cyc_Absyn_exp_case_number(e2);unsigned e2case=_tmpCF;
if(e1case != e2case)
return(int)(e1case - e2case);{
struct _tuple18 _tmpD0=({struct _tuple18 _tmp1A9;_tmp1A9.f1=e1->r,_tmp1A9.f2=e2->r;_tmp1A9;});struct _tuple18 _stmttmp19=_tmpD0;struct _tuple18 _tmpD1=_stmttmp19;struct Cyc_Absyn_Datatypefield*_tmpD7;struct Cyc_Absyn_Datatypedecl*_tmpD6;struct Cyc_List_List*_tmpD5;struct Cyc_Absyn_Datatypefield*_tmpD4;struct Cyc_Absyn_Datatypedecl*_tmpD3;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*_tmpD8;struct Cyc_Absyn_Aggrdecl*_tmpDF;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Aggrdecl*_tmpDC;struct Cyc_List_List*_tmpDB;struct Cyc_List_List*_tmpDA;struct Cyc_List_List*_tmpE1;struct Cyc_List_List*_tmpE0;struct Cyc_List_List*_tmpE5;void*_tmpE4;struct Cyc_List_List*_tmpE3;void*_tmpE2;int _tmpEB;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpE9;int _tmpE8;void*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF4;void*_tmpF7;void*_tmpF6;struct Cyc_Absyn_Exp*_tmpFB;void*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpF8;struct Cyc_List_List*_tmpFF;void*_tmpFE;struct Cyc_List_List*_tmpFD;void*_tmpFC;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;void*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp105;void*_tmp104;void*_tmp107;void*_tmp106;struct Cyc_List_List*_tmp10B;enum Cyc_Absyn_Primop _tmp10A;struct Cyc_List_List*_tmp109;enum Cyc_Absyn_Primop _tmp108;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Vardecl*_tmp118;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct _fat_ptr _tmp125;struct _fat_ptr _tmp124;void*_tmp127;void*_tmp126;union Cyc_Absyn_Cnst _tmp129;union Cyc_Absyn_Cnst _tmp128;switch(*((int*)_tmpD1.f1)){case 0U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 0U){_LL7: _tmp128=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp129=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL8: {union Cyc_Absyn_Cnst c1=_tmp128;union Cyc_Absyn_Cnst c2=_tmp129;
# 421
return Cyc_Evexp_cmp_cnst(c1,c2);}}else{goto _LL3F;}case 1U: if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 1U){_LL9: _tmp126=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp127=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LLA: {void*b1=_tmp126;void*b2=_tmp127;
return Cyc_Evexp_cmp_binding(b1,b2);}}else{goto _LL3F;}case 2U: if(((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 2U){_LLB: _tmp124=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp125=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LLC: {struct _fat_ptr s1=_tmp124;struct _fat_ptr s2=_tmp125;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LL3F;}case 6U: if(((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 6U){_LLD: _tmp11E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp11F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmp120=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f3;_tmp121=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmp122=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_tmp123=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f3;_LLE: {struct Cyc_Absyn_Exp*e11=_tmp11E;struct Cyc_Absyn_Exp*e12=_tmp11F;struct Cyc_Absyn_Exp*e13=_tmp120;struct Cyc_Absyn_Exp*e21=_tmp121;struct Cyc_Absyn_Exp*e22=_tmp122;struct Cyc_Absyn_Exp*e23=_tmp123;
# 427
int _tmp12A=Cyc_Evexp_exp_cmp(e11,e21);int c1=_tmp12A;
if(c1 != 0)return c1;
c1=Cyc_Evexp_exp_cmp(e12,e22);
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e13,e23);}}else{goto _LL3F;}case 27U: if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 27U){_LLF: _tmp118=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp119=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmp11A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f3;_tmp11B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmp11C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_tmp11D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f3;_LL10: {struct Cyc_Absyn_Vardecl*vd1=_tmp118;struct Cyc_Absyn_Exp*e11=_tmp119;struct Cyc_Absyn_Exp*e12=_tmp11A;struct Cyc_Absyn_Vardecl*vd2=_tmp11B;struct Cyc_Absyn_Exp*e21=_tmp11C;struct Cyc_Absyn_Exp*e22=_tmp11D;
# 433
if(vd1 != vd2)return(int)vd1 - (int)vd2;
_tmp114=e11;_tmp115=e12;_tmp116=e21;_tmp117=e22;goto _LL12;}}else{goto _LL3F;}case 7U: if(((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 7U){_LL11: _tmp114=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp115=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmp116=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmp117=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL12: {struct Cyc_Absyn_Exp*e11=_tmp114;struct Cyc_Absyn_Exp*e12=_tmp115;struct Cyc_Absyn_Exp*e21=_tmp116;struct Cyc_Absyn_Exp*e22=_tmp117;
_tmp110=e11;_tmp111=e12;_tmp112=e21;_tmp113=e22;goto _LL14;}}else{goto _LL3F;}case 8U: if(((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 8U){_LL13: _tmp110=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp111=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmp112=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmp113=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL14: {struct Cyc_Absyn_Exp*e11=_tmp110;struct Cyc_Absyn_Exp*e12=_tmp111;struct Cyc_Absyn_Exp*e21=_tmp112;struct Cyc_Absyn_Exp*e22=_tmp113;
_tmp10C=e11;_tmp10D=e12;_tmp10E=e21;_tmp10F=e22;goto _LL16;}}else{goto _LL3F;}case 9U: if(((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 9U){_LL15: _tmp10C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp10D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmp10E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmp10F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL16: {struct Cyc_Absyn_Exp*e11=_tmp10C;struct Cyc_Absyn_Exp*e12=_tmp10D;struct Cyc_Absyn_Exp*e21=_tmp10E;struct Cyc_Absyn_Exp*e22=_tmp10F;
# 438
int _tmp12B=Cyc_Evexp_exp_cmp(e11,e21);int c1=_tmp12B;
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e12,e22);}}else{goto _LL3F;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 3U){_LL17: _tmp108=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp109=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmp10A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmp10B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL18: {enum Cyc_Absyn_Primop p1=_tmp108;struct Cyc_List_List*es1=_tmp109;enum Cyc_Absyn_Primop p2=_tmp10A;struct Cyc_List_List*es2=_tmp10B;
# 442
int _tmp12C=(int)p1 - (int)p2;int c1=_tmp12C;
if(c1 != 0)return c1;
for(0;es1 != 0 && es2 != 0;(es1=es1->tl,es2=es2->tl)){
int _tmp12D=Cyc_Evexp_exp_cmp((struct Cyc_Absyn_Exp*)es1->hd,(struct Cyc_Absyn_Exp*)es2->hd);int c2=_tmp12D;
if(c2 != 0)
return c2;}
# 449
return 0;}}else{goto _LL3F;}case 17U: switch(*((int*)_tmpD1.f2)){case 17U: _LL19: _tmp106=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp107=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL1A: {void*t1=_tmp106;void*t2=_tmp107;
# 451
return Cyc_Tcutil_typecmp(t1,t2);}case 18U: _LL1B: _tmp104=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp105=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL1C: {void*t1=_tmp104;struct Cyc_Absyn_Exp*e2a=_tmp105;
# 453
void*_tmp12E=e2a->topt;void*e2atopt=_tmp12E;
if(e2atopt == 0)
({void*_tmp12F=0U;({unsigned _tmp1CD=e2->loc;struct _fat_ptr _tmp1CC=({const char*_tmp130="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp130,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp1CD,_tmp1CC,_tag_fat(_tmp12F,sizeof(void*),0U));});});
return Cyc_Tcutil_typecmp(t1,(void*)_check_null(e2atopt));}default: goto _LL3F;}case 18U: switch(*((int*)_tmpD1.f2)){case 17U: _LL1D: _tmp102=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp103=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1a=_tmp102;void*t2=_tmp103;
# 458
void*_tmp131=e1a->topt;void*e1atopt=_tmp131;
if(e1atopt == 0)
({void*_tmp132=0U;({unsigned _tmp1CF=e1->loc;struct _fat_ptr _tmp1CE=({const char*_tmp133="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp133,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp1CF,_tmp1CE,_tag_fat(_tmp132,sizeof(void*),0U));});});
return Cyc_Tcutil_typecmp((void*)_check_null(e1atopt),t2);}case 18U: _LL1F: _tmp100=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmp101=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL20: {struct Cyc_Absyn_Exp*e1a=_tmp100;struct Cyc_Absyn_Exp*e2a=_tmp101;
# 463
void*_tmp134=e1a->topt;void*e1atopt=_tmp134;
void*_tmp135=e2a->topt;void*e2atopt=_tmp135;
if(e1atopt == 0)
({void*_tmp136=0U;({unsigned _tmp1D1=e1->loc;struct _fat_ptr _tmp1D0=({const char*_tmp137="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp137,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp1D1,_tmp1D0,_tag_fat(_tmp136,sizeof(void*),0U));});});
if(e2atopt == 0)
({void*_tmp138=0U;({unsigned _tmp1D3=e2->loc;struct _fat_ptr _tmp1D2=({const char*_tmp139="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_fat(_tmp139,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp1D3,_tmp1D2,_tag_fat(_tmp138,sizeof(void*),0U));});});
return({void*_tmp1D4=(void*)_check_null(e1atopt);Cyc_Tcutil_typecmp(_tmp1D4,(void*)_check_null(e2atopt));});}default: goto _LL3F;}case 19U: if(((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 19U){_LL21: _tmpFC=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpFD=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmpFE=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmpFF=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL22: {void*t1=_tmpFC;struct Cyc_List_List*f1=_tmpFD;void*t2=_tmpFE;struct Cyc_List_List*f2=_tmpFF;
# 471
int _tmp13A=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmp13A;
if(c1 != 0)return c1;{
int _tmp13B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(f1);int l1=_tmp13B;
int _tmp13C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(f2);int l2=_tmp13C;
if(l1 < l2)return - 1;
if(l2 < l1)return 1;
for(0;f1 != 0 && f2 != 0;(f1=f1->tl,f2=f2->tl)){
struct _tuple18 _tmp13D=({struct _tuple18 _tmp1A8;_tmp1A8.f1=(void*)f1->hd,_tmp1A8.f2=(void*)f2->hd;_tmp1A8;});struct _tuple18 _stmttmp1A=_tmp13D;struct _tuple18 _tmp13E=_stmttmp1A;unsigned _tmp140;unsigned _tmp13F;struct _fat_ptr*_tmp142;struct _fat_ptr*_tmp141;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp13E.f1)->tag == 0U){if(((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp13E.f2)->tag == 1U){_LL42: _LL43:
 return - 1;}else{_LL46: _tmp141=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp13E.f1)->f1;_tmp142=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp13E.f2)->f1;_LL47: {struct _fat_ptr*fn1=_tmp141;struct _fat_ptr*fn2=_tmp142;
# 482
int c=Cyc_strptrcmp(fn1,fn2);
if(c != 0)return c;
goto _LL41;}}}else{if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp13E.f2)->tag == 0U){_LL44: _LL45:
# 480
 return 1;}else{_LL48: _tmp13F=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp13E.f1)->f1;_tmp140=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp13E.f2)->f1;_LL49: {unsigned i1=_tmp13F;unsigned i2=_tmp140;
# 486
int c=(int)(i1 - i2);
if(c != 0)return c;
goto _LL41;}}}_LL41:;}
# 490
return 0;}}}else{goto _LL3F;}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 14U){_LL23: _tmpF8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpF9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmpFA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmpFB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL24: {void*t1=_tmpF8;struct Cyc_Absyn_Exp*e1a=_tmpF9;void*t2=_tmpFA;struct Cyc_Absyn_Exp*e2a=_tmpFB;
# 492
int _tmp143=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmp143;
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e1a,e2a);}}else{goto _LL3F;}case 39U: if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 39U){_LL25: _tmpF6=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpF7=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL26: {void*t1=_tmpF6;void*t2=_tmpF7;
# 496
if(Cyc_Unify_unify(t1,t2))return 0;
return Cyc_Tcutil_typecmp(t1,t2);}}else{goto _LL3F;}case 41U: if(((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 41U){_LL27: _tmpF4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpF5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF4;struct Cyc_Absyn_Exp*e2=_tmpF5;
_tmpF2=e1;_tmpF3=e2;goto _LL2A;}}else{goto _LL3F;}case 12U: if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 12U){_LL29: _tmpF2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpF3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
# 500
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 13U: if(((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 13U){_LL2B: _tmpEE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpEF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmpF0=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmpF1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpEE;struct Cyc_List_List*t1=_tmpEF;struct Cyc_Absyn_Exp*e2=_tmpF0;struct Cyc_List_List*t2=_tmpF1;
# 502
int _tmp144=Cyc_Evexp_exp_cmp(e1,e2);int c1=_tmp144;
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,t1,t2);}}else{goto _LL3F;}case 15U: if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 15U){_LL2D: _tmpEC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpED=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpEC;struct Cyc_Absyn_Exp*e2=_tmpED;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 28U: if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 28U){_LL2F: _tmpE6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpE7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmpE8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f3;_tmpE9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmpEA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_tmpEB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f3;_LL30: {struct Cyc_Absyn_Exp*e1=_tmpE6;void*t1=_tmpE7;int b1=_tmpE8;struct Cyc_Absyn_Exp*e2=_tmpE9;void*t2=_tmpEA;int b2=_tmpEB;
# 507
int _tmp145=b1 - b2;int c1=_tmp145;
if(c1 != 0)return c1;
c1=Cyc_Tcutil_typecmp(t1,t2);
if(c1 != 0)return c1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL3F;}case 30U: if(((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 30U){_LL31: _tmpE2=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpE3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmpE4=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmpE5=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_LL32: {void*t1=_tmpE2;struct Cyc_List_List*des1=_tmpE3;void*t2=_tmpE4;struct Cyc_List_List*des2=_tmpE5;
# 513
int _tmp146=Cyc_Tcutil_typecmp(t1,t2);int c1=_tmp146;
if(c1 != 0)return c1;
_tmpE0=des1;_tmpE1=des2;goto _LL34;}}else{goto _LL3F;}case 26U: if(((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 26U){_LL33: _tmpE0=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpE1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL34: {struct Cyc_List_List*des1=_tmpE0;struct Cyc_List_List*des2=_tmpE1;
# 517
return((int(*)(int(*cmp)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_designator_exp_cmp,des1,des2);}}else{goto _LL3F;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 29U){_LL35: _tmpDA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmpDB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f3;_tmpDC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f4;_tmpDD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_tmpDE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f3;_tmpDF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f4;_LL36: {struct Cyc_List_List*ts1=_tmpDA;struct Cyc_List_List*des1=_tmpDB;struct Cyc_Absyn_Aggrdecl*ad1=_tmpDC;struct Cyc_List_List*ts2=_tmpDD;struct Cyc_List_List*des2=_tmpDE;struct Cyc_Absyn_Aggrdecl*ad2=_tmpDF;
# 519
int _tmp147=(int)ad1 - (int)ad2;int c1=_tmp147;
if(c1 != 0)return c1;
c1=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_designator_exp_cmp,des1,des1);}}else{goto _LL3F;}case 24U: if(((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 24U){_LL37: _tmpD8=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpD9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_LL38: {struct Cyc_List_List*es1=_tmpD8;struct Cyc_List_List*es2=_tmpD9;
# 525
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 31U){_LL39: _tmpD2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f1;_tmpD3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f2;_tmpD4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD1.f1)->f3;_tmpD5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f1;_tmpD6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f2;_tmpD7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->f3;_LL3A: {struct Cyc_List_List*es1=_tmpD2;struct Cyc_Absyn_Datatypedecl*dd1=_tmpD3;struct Cyc_Absyn_Datatypefield*df1=_tmpD4;struct Cyc_List_List*es2=_tmpD5;struct Cyc_Absyn_Datatypedecl*dd2=_tmpD6;struct Cyc_Absyn_Datatypefield*df2=_tmpD7;
# 527
int _tmp148=(int)dd1 - (int)dd2;int c1=_tmp148;
if(c1 != 0)return c1;
c1=(int)df1 - (int)df2;
if(c1 != 0)return c1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Evexp_exp_cmp,es1,es2);}}else{goto _LL3F;}case 32U: if(((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 32U){_LL3B: _LL3C:
# 533
 goto _LL3E;}else{goto _LL3F;}case 33U: if(((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD1.f2)->tag == 33U){_LL3D: _LL3E:
 goto _LL40;}else{goto _LL3F;}default: _LL3F: _LL40:
({void*_tmp149=0U;({struct _fat_ptr _tmp1D5=({const char*_tmp14A="Evexp::exp_cmp, unexpected case";_tag_fat(_tmp14A,sizeof(char),32U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp1D5,_tag_fat(_tmp149,sizeof(void*),0U));});});}_LL6:;}}}}}
# 539
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 541
e1=Cyc_Evexp_strip_cast(e1);
e2=Cyc_Evexp_strip_cast(e2);{
struct _tuple12 _tmp14B=Cyc_Evexp_eval_const_exp(e1);struct _tuple12 _stmttmp1B=_tmp14B;struct _tuple12 _tmp14C=_stmttmp1B;int _tmp14E;union Cyc_Absyn_Cnst _tmp14D;_LL1: _tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;_LL2: {union Cyc_Absyn_Cnst c1=_tmp14D;int known1=_tmp14E;
struct _tuple12 _tmp14F=Cyc_Evexp_eval_const_exp(e2);struct _tuple12 _stmttmp1C=_tmp14F;struct _tuple12 _tmp150=_stmttmp1C;int _tmp152;union Cyc_Absyn_Cnst _tmp151;_LL4: _tmp151=_tmp150.f1;_tmp152=_tmp150.f2;_LL5: {union Cyc_Absyn_Cnst c2=_tmp151;int known2=_tmp152;
if(known1 && known2){
struct _tuple11 _tmp153=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple11 _stmttmp1D=_tmp153;struct _tuple11 _tmp154=_stmttmp1D;unsigned _tmp155;_LL7: _tmp155=_tmp154.f1;_LL8: {unsigned i1=_tmp155;
struct _tuple11 _tmp156=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple11 _stmttmp1E=_tmp156;struct _tuple11 _tmp157=_stmttmp1E;unsigned _tmp158;_LLA: _tmp158=_tmp157.f1;_LLB: {unsigned i2=_tmp158;
return(int)(i1 - i2);}}}
# 550
return Cyc_Evexp_exp_cmp(e1,e2);}}}}
# 553
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_uint_exp_cmp(e1,e2)== 0;}
# 556
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp159=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple11 _stmttmp1F=_tmp159;struct _tuple11 _tmp15A=_stmttmp1F;int _tmp15C;unsigned _tmp15B;_LL1: _tmp15B=_tmp15A.f1;_tmp15C=_tmp15A.f2;_LL2: {unsigned i1=_tmp15B;int known1=_tmp15C;
struct _tuple11 _tmp15D=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple11 _stmttmp20=_tmp15D;struct _tuple11 _tmp15E=_stmttmp20;int _tmp160;unsigned _tmp15F;_LL4: _tmp15F=_tmp15E.f1;_tmp160=_tmp15E.f2;_LL5: {unsigned i2=_tmp15F;int known2=_tmp160;
if(known1 && known2)
return i1 <= i2;
return Cyc_Evexp_same_uint_const_exp(e1,e2);}}}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*f2;};
# 566
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp161=Cyc_Tcutil_compress(t);void*_stmttmp21=_tmp161;void*_tmp162=_stmttmp21;struct Cyc_Absyn_Typedefdecl*_tmp163;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Exp*_tmp166;void*_tmp165;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_Tvar*_tmp168;struct Cyc_List_List*_tmp16A;void*_tmp169;switch(*((int*)_tmp162)){case 0U: _LL1: _tmp169=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp162)->f1;_tmp16A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp162)->f2;_LL2: {void*c=_tmp169;struct Cyc_List_List*ts=_tmp16A;
# 569
void*_tmp16B=c;struct Cyc_Absyn_Aggrdecl*_tmp16C;struct Cyc_Absyn_Datatypefield*_tmp16D;switch(*((int*)_tmp16B)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp16B)->f1).KnownDatatypefield).tag == 2){_LL1C: _tmp16D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp16B)->f1).KnownDatatypefield).val).f2;_LL1D: {struct Cyc_Absyn_Datatypefield*tuf=_tmp16D;
# 571
{struct Cyc_List_List*_tmp16E=tuf->typs;struct Cyc_List_List*tqs=_tmp16E;for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple21*)tqs->hd)).f2))
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
# 585
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
 return 0;default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp16B)->f1).UnknownAggr).tag == 1){_LL46: _LL47:
# 597
({void*_tmp16F=0U;({struct _fat_ptr _tmp1D6=({const char*_tmp170="szof on unchecked StructType";_tag_fat(_tmp170,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp1D6,_tag_fat(_tmp16F,sizeof(void*),0U));});});}else{_LL48: _tmp16C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp16B)->f1).KnownAggr).val;_LL49: {struct Cyc_Absyn_Aggrdecl*ad=_tmp16C;
# 599
if(ad->impl == 0)return 0;else{
# 601
struct _RegionHandle _tmp171=_new_region("temp");struct _RegionHandle*temp=& _tmp171;_push_region(temp);
{struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,ts);struct Cyc_List_List*inst=_tmp172;
{struct Cyc_List_List*_tmp173=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fs=_tmp173;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp174=0;_npop_handler(0U);return _tmp174;}}}{
int _tmp175=1;_npop_handler(0U);return _tmp175;}}
# 602
;_pop_region();}}}}_LL1B:;}case 2U: _LL3: _tmp168=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp162)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp168;
# 609
enum Cyc_Absyn_KindQual _tmp176=(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind;enum Cyc_Absyn_KindQual _stmttmp22=_tmp176;enum Cyc_Absyn_KindQual _tmp177=_stmttmp22;if(_tmp177 == Cyc_Absyn_BoxKind){_LL4B: _LL4C:
 return 1;}else{_LL4D: _LL4E:
 return 0;}_LL4A:;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp162)->f1 != 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp162)->f1)->v)->kind == Cyc_Absyn_BoxKind){_LL5: _LL6:
# 613
 return 1;}else{goto _LL7;}}else{_LL7: _LL8:
 return 0;}case 6U: _LL9: _tmp167=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp162)->f1;_LLA: {struct Cyc_List_List*tqs=_tmp167;
# 616
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple21*)tqs->hd)).f2))
return 0;}
return 1;}case 3U: _LLB: _LLC:
# 622
 return 1;case 4U: _LLD: _tmp165=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp162)->f1).elt_type;_tmp166=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp162)->f1).num_elts;_LLE: {void*t2=_tmp165;struct Cyc_Absyn_Exp*e=_tmp166;
# 625
return e != 0;}case 5U: _LLF: _LL10:
 return 0;case 7U: _LL11: _tmp164=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp162)->f2;_LL12: {struct Cyc_List_List*fs=_tmp164;
# 628
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}case 10U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 return 0;default: _LL19: _tmp163=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp162)->f3;_LL1A: {struct Cyc_Absyn_Typedefdecl*td=_tmp163;
# 637
if(td == 0 || td->kind == 0)
({void*_tmp178=0U;({struct _fat_ptr _tmp1D9=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp1AA;_tmp1AA.tag=0U,({struct _fat_ptr _tmp1D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp1AA.f1=_tmp1D7;});_tmp1AA;});void*_tmp179[1U];_tmp179[0]=& _tmp17B;({struct _fat_ptr _tmp1D8=({const char*_tmp17A="szof typedeftype %s";_tag_fat(_tmp17A,sizeof(char),20U);});Cyc_aprintf(_tmp1D8,_tag_fat(_tmp179,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp1D9,_tag_fat(_tmp178,sizeof(void*),0U));});});
return(int)((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v)->kind == (int)Cyc_Absyn_BoxKind;}}_LL0:;}

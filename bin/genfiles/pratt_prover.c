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
 extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 863 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 879
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 971
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};
# 62 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 64
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
void*Cyc_AssnDef_zero (void);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
# 75
int Cyc_AssnDef_cmp_term(void*,void*);
void*Cyc_AssnDef_get_term_type(void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 117 "assndef.h"
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
# 121
void*Cyc_AssnDef_prim(void*,enum Cyc_AssnDef_Primreln,void*);
# 124
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
# 132
void*Cyc_AssnDef_not(void*);
# 28 "pratt_prover.cyc"
enum Cyc_PrattProver_DistanceInfo{Cyc_PrattProver_Unsigned_valid =1,Cyc_PrattProver_Signed_valid =2,Cyc_PrattProver_Both_valid =3};struct Cyc_PrattProver_Dist{int length;void*prim;};struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;void*target;enum Cyc_PrattProver_DistanceInfo dinfo;struct Cyc_PrattProver_Dist unsigned_dist;struct Cyc_PrattProver_Dist signed_dist;};struct Cyc_PrattProver_Row{struct Cyc_PrattProver_Row*next;void*source;struct Cyc_PrattProver_Distance*distance;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Row*rows;};
# 66 "pratt_prover.cyc"
static void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*gs){
({void*_tmp0=0U;({struct Cyc___cycFILE*_tmp112=Cyc_stderr;struct _fat_ptr _tmp111=({const char*_tmp1="Graphs:\n";_tag_fat(_tmp1,sizeof(char),9U);});Cyc_fprintf(_tmp112,_tmp111,_tag_fat(_tmp0,sizeof(void*),0));});});
for(1;gs != 0;gs=gs->next){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp114=Cyc_stderr;struct _fat_ptr _tmp113=({const char*_tmp3="\t{";_tag_fat(_tmp3,sizeof(char),3U);});Cyc_fprintf(_tmp114,_tmp113,_tag_fat(_tmp2,sizeof(void*),0));});});{
struct Cyc_PrattProver_Row*_tmp4=gs->rows;struct Cyc_PrattProver_Row*ns=_tmp4;for(0;ns != 0;ns=ns->next){
void*_tmp5=ns->source;void*t1=_tmp5;
{struct Cyc_PrattProver_Distance*_tmp6=ns->distance;struct Cyc_PrattProver_Distance*ds=_tmp6;for(0;ds != 0;ds=ds->next){
void*_tmp7=ds->target;void*t2=_tmp7;
if((int)ds->dinfo == 3)
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmpED;_tmpED.tag=0,({struct _fat_ptr _tmp115=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpED.f1=_tmp115;});_tmpED;});struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmpEC;_tmpEC.tag=0,({
struct _fat_ptr _tmp116=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpEC.f1=_tmp116;});_tmpEC;});struct Cyc_Int_pa_PrintArg_struct _tmpC=({struct Cyc_Int_pa_PrintArg_struct _tmpEB;_tmpEB.tag=1,_tmpEB.f1=(unsigned long)(ds->unsigned_dist).length;_tmpEB;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmpEA;_tmpEA.tag=0,({struct _fat_ptr _tmp117=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpEA.f1=_tmp117;});_tmpEA;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmpE9;_tmpE9.tag=0,({
struct _fat_ptr _tmp118=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpE9.f1=_tmp118;});_tmpE9;});struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmpE8;_tmpE8.tag=1,_tmpE8.f1=(unsigned long)(ds->signed_dist).length;_tmpE8;});void*_tmp8[6];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB,_tmp8[2]=& _tmpC,_tmp8[3]=& _tmpD,_tmp8[4]=& _tmpE,_tmp8[5]=& _tmpF;({struct Cyc___cycFILE*_tmp11A=Cyc_stderr;struct _fat_ptr _tmp119=({const char*_tmp9="%s - %s U<= %d, %s - %s S<= %d";_tag_fat(_tmp9,sizeof(char),31U);});Cyc_fprintf(_tmp11A,_tmp119,_tag_fat(_tmp8,sizeof(void*),6));});});else{
if((int)ds->dinfo == 2)
({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmpF0;_tmpF0.tag=0,({struct _fat_ptr _tmp11B=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpF0.f1=_tmp11B;});_tmpF0;});struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0,({
struct _fat_ptr _tmp11C=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpEF.f1=_tmp11C;});_tmpEF;});struct Cyc_Int_pa_PrintArg_struct _tmp14=({struct Cyc_Int_pa_PrintArg_struct _tmpEE;_tmpEE.tag=1,_tmpEE.f1=(unsigned long)(ds->signed_dist).length;_tmpEE;});void*_tmp10[3];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13,_tmp10[2]=& _tmp14;({struct Cyc___cycFILE*_tmp11E=Cyc_stderr;struct _fat_ptr _tmp11D=({const char*_tmp11="%s - %s S<= %d";_tag_fat(_tmp11,sizeof(char),15U);});Cyc_fprintf(_tmp11E,_tmp11D,_tag_fat(_tmp10,sizeof(void*),3));});});else{
# 82
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmpF3;_tmpF3.tag=0,({struct _fat_ptr _tmp11F=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmpF3.f1=_tmp11F;});_tmpF3;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmpF2;_tmpF2.tag=0,({
struct _fat_ptr _tmp120=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmpF2.f1=_tmp120;});_tmpF2;});struct Cyc_Int_pa_PrintArg_struct _tmp19=({struct Cyc_Int_pa_PrintArg_struct _tmpF1;_tmpF1.tag=1,_tmpF1.f1=(unsigned long)(ds->unsigned_dist).length;_tmpF1;});void*_tmp15[3];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18,_tmp15[2]=& _tmp19;({struct Cyc___cycFILE*_tmp122=Cyc_stderr;struct _fat_ptr _tmp121=({const char*_tmp16="%s - %s U<= %d";_tag_fat(_tmp16,sizeof(char),15U);});Cyc_fprintf(_tmp122,_tmp121,_tag_fat(_tmp15,sizeof(void*),3));});});}}
# 85
if(ds->next != 0)({void*_tmp1A=0U;({struct Cyc___cycFILE*_tmp124=Cyc_stderr;struct _fat_ptr _tmp123=({const char*_tmp1B=",";_tag_fat(_tmp1B,sizeof(char),2U);});Cyc_fprintf(_tmp124,_tmp123,_tag_fat(_tmp1A,sizeof(void*),0));});});}}
# 87
({void*_tmp1C=0U;({struct Cyc___cycFILE*_tmp126=Cyc_stderr;struct _fat_ptr _tmp125=({const char*_tmp1D="}\n";_tag_fat(_tmp1D,sizeof(char),3U);});Cyc_fprintf(_tmp126,_tmp125,_tag_fat(_tmp1C,sizeof(void*),0));});});}}}}
# 93
static unsigned Cyc_PrattProver_num_graphs(struct Cyc_PrattProver_Graph*gs){
unsigned n=0U;
for(1;gs != 0;gs=gs->next){
++ n;}
return n;}
# 102
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_true_graph (void){return({struct Cyc_PrattProver_Graph*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->next=0,_tmp1E->rows=0;_tmp1E;});}
# 105
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_copy_distance(struct Cyc_PrattProver_Distance*ds){
struct Cyc_PrattProver_Distance*res=0;
for(1;ds != 0;ds=ds->next){
struct Cyc_PrattProver_Distance*_tmp1F=({struct Cyc_PrattProver_Distance*_tmp20=_cycalloc(sizeof(*_tmp20));*_tmp20=*ds;_tmp20;});struct Cyc_PrattProver_Distance*newds=_tmp1F;
newds->next=res;
res=newds;}
# 112
return res;}
# 115
static struct Cyc_PrattProver_Row*Cyc_PrattProver_copy_rows(struct Cyc_PrattProver_Row*ns){
struct Cyc_PrattProver_Row*res=0;
for(1;ns != 0;ns=ns->next){
res=({struct Cyc_PrattProver_Row*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->next=res,_tmp21->source=ns->source,({
# 120
struct Cyc_PrattProver_Distance*_tmp127=Cyc_PrattProver_copy_distance(ns->distance);_tmp21->distance=_tmp127;});_tmp21;});}
# 122
return res;}
# 126
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graph(struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Graph*res=0;
for(1;g != 0;g=g->next){
res=({struct Cyc_PrattProver_Graph*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->next=res,({struct Cyc_PrattProver_Row*_tmp128=Cyc_PrattProver_copy_rows(g->rows);_tmp22->rows=_tmp128;});_tmp22;});}
# 131
return res;}
# 136
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_graph_append(struct Cyc_PrattProver_Graph*g1,struct Cyc_PrattProver_Graph*g2){
if(g1 == 0)return g2;
if(g2 == 0)return g1;{
struct Cyc_PrattProver_Graph*p=g1;
{struct Cyc_PrattProver_Graph*_tmp23=p->next;struct Cyc_PrattProver_Graph*x=_tmp23;for(0;x != 0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}
# 145
static int Cyc_PrattProver_add_edge(struct Cyc_PrattProver_Graph*,int,void*,void*,int,void*);
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*,void*,enum Cyc_AssnDef_Primreln,void*);
# 148
static void Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*,void*);
# 150
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
Cyc_PrattProver_add_node(g,t2);
# 153
Cyc_PrattProver_add_constraint(g,t1,5U,t2);
Cyc_PrattProver_add_constraint(g,t1,3U,t2);
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);}
# 162
static void Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*n){
void*_tmp24=Cyc_AssnDef_get_term_type(n);void*topt=_tmp24;
if(topt != 0){
void*t=topt;
struct Cyc_Absyn_Exp*_tmp25=Cyc_Tcutil_get_type_bound(t);struct Cyc_Absyn_Exp*eopt=_tmp25;
if(eopt != 0){
void*t1=Cyc_AssnDef_primop(18U,({void*_tmp2D[1];_tmp2D[0]=n;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp2D,sizeof(void*),1));}));
# 170
struct Cyc_Absyn_Exp*e=eopt;
loop: {
void*_tmp26=e->r;void*_stmttmp0=_tmp26;void*_tmp27=_stmttmp0;void*_tmp29;void*_tmp28;if(*((int*)_tmp27)== 14){_tmp28=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27)->f1;_tmp29=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27)->f2;{void*tp2=_tmp28;struct Cyc_Absyn_Exp*e2=_tmp29;
# 174
{void*_tmp2A=Cyc_Absyn_compress(tp2);void*_stmttmp1=_tmp2A;void*_tmp2B=_stmttmp1;enum Cyc_Absyn_Size_of _tmp2C;if(*((int*)_tmp2B)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)== 1){_tmp2C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f2;{enum Cyc_Absyn_Size_of s=_tmp2C;
# 176
if((int)s == 2 ||(int)s == 3){
e=e2;goto loop;}
# 179
goto _LL5;}}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}
# 182
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 185
void*t2=Cyc_AssnDef_cnst(e);
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);}}{
# 189
void*_tmp2E=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp2E;void*_tmp2F=_stmttmp2;void*_tmp30;if(*((int*)_tmp2F)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f2 != 0){_tmp30=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f2)->hd;{void*v=_tmp30;
# 191
{void*_tmp31=Cyc_Absyn_compress(v);void*_stmttmp3=_tmp31;void*_tmp32=_stmttmp3;void*_tmp33;if(*((int*)_tmp32)== 9){_tmp33=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp32)->f1;{struct Cyc_Absyn_Exp*e=_tmp33;
({struct Cyc_PrattProver_Graph*_tmp12A=g;void*_tmp129=n;Cyc_PrattProver_add_eq(_tmp12A,_tmp129,Cyc_AssnDef_cnst(e));});goto _LLF;}}else{_tmp33=_tmp32;{void*v2=_tmp33;
({struct Cyc_PrattProver_Graph*_tmp12C=g;void*_tmp12B=n;Cyc_PrattProver_add_eq(_tmp12C,_tmp12B,Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U)));});goto _LLF;}}_LLF:;}
# 195
goto _LLA;}}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}}}
# 202
static void Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*g,void*n){
{struct Cyc_PrattProver_Row*_tmp34=g->rows;struct Cyc_PrattProver_Row*ns=_tmp34;for(0;ns != 0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(ns->source,n)== 0)return;}}
# 206
({struct Cyc_PrattProver_Row*_tmp12D=({struct Cyc_PrattProver_Row*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->next=g->rows,_tmp35->source=n,_tmp35->distance=0;_tmp35;});g->rows=_tmp12D;});{
# 208
void*_tmp36=n;void*_tmp37;if(*((int*)_tmp36)== 4){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp36)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp36)->f2 != 0){_tmp37=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp36)->f2)->hd;{void*x=_tmp37;
# 210
({struct Cyc_PrattProver_Graph*_tmp12F=g;void*_tmp12E=n;Cyc_PrattProver_add_edge(_tmp12F,0,_tmp12E,Cyc_AssnDef_zero(),2147483646,0);});
({struct Cyc_PrattProver_Graph*_tmp131=g;void*_tmp130=n;Cyc_PrattProver_add_edge(_tmp131,1,_tmp130,Cyc_AssnDef_zero(),2147483646,0);});
# 213
Cyc_PrattProver_add_type_info(g,x);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 216
 Cyc_PrattProver_add_type_info(g,n);
goto _LL0;}_LL0:;}}
# 221
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row(struct Cyc_PrattProver_Graph*g,void*i){
{struct Cyc_PrattProver_Row*_tmp38=g->rows;struct Cyc_PrattProver_Row*ns=_tmp38;for(0;ns != 0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(i,ns->source)== 0)return ns;}}
return 0;}
# 227
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_lookup_col(struct Cyc_PrattProver_Row*n,void*j){
{struct Cyc_PrattProver_Distance*_tmp39=((struct Cyc_PrattProver_Row*)_check_null(n))->distance;struct Cyc_PrattProver_Distance*ds=_tmp39;for(0;ds != 0;ds=ds->next){
if(Cyc_AssnDef_cmp_term(j,ds->target)== 0)return ds;}}
return 0;}
# 235
static struct Cyc_PrattProver_Dist*Cyc_PrattProver_lookup_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j){
static struct Cyc_PrattProver_Dist zero={0,0};
if(i == 0)return 0;
if(Cyc_AssnDef_cmp_term(i->source,j)== 0)
return& zero;{
struct Cyc_PrattProver_Distance*_tmp3A=Cyc_PrattProver_lookup_col(i,j);struct Cyc_PrattProver_Distance*ds=_tmp3A;
if(ds != 0){
if(is_signed &&(int)ds->dinfo & 2)
return& ds->signed_dist;else{
if(!is_signed &&(int)ds->dinfo & 1)
return& ds->unsigned_dist;else{
# 247
return 0;}}}else{
# 249
return 0;}}}
# 257
static void Cyc_PrattProver_insert_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j,int dist,void*origin){
if(Cyc_AssnDef_cmp_term(((struct Cyc_PrattProver_Row*)_check_null(i))->source,j)== 0)return;{
struct Cyc_PrattProver_Distance*_tmp3B=Cyc_PrattProver_lookup_col(i,j);struct Cyc_PrattProver_Distance*ds=_tmp3B;
if(ds != 0){
if(is_signed){
ds->dinfo |=2;
(ds->signed_dist).length=dist;
(ds->signed_dist).prim=origin;}else{
# 266
ds->dinfo |=1;
(ds->unsigned_dist).length=dist;
(ds->unsigned_dist).prim=origin;}
# 270
return;}{
# 272
struct Cyc_PrattProver_Dist d=({struct Cyc_PrattProver_Dist _tmpF4;_tmpF4.length=dist,_tmpF4.prim=origin;_tmpF4;});
({struct Cyc_PrattProver_Distance*_tmp132=({struct Cyc_PrattProver_Distance*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->next=i->distance,_tmp3C->target=j,is_signed?_tmp3C->dinfo=2:(_tmp3C->dinfo=1),_tmp3C->unsigned_dist=d,_tmp3C->signed_dist=d;_tmp3C;});i->distance=_tmp132;});}}}struct _tuple12{int f1;int f2;};
# 280
enum Cyc_PrattProver_Operator{Cyc_PrattProver_PLUS =0U,Cyc_PrattProver_MINUS =1U};
# 290
static struct _tuple12 Cyc_PrattProver_is_signed_overflow(int c1,enum Cyc_PrattProver_Operator op,int c2){
enum Cyc_PrattProver_Operator _tmp3D=op;switch((int)_tmp3D){case Cyc_PrattProver_PLUS:  {
# 293
int _tmp3E=c1 + c2;int sum=_tmp3E;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 296
if(sum > 0)return({struct _tuple12 _tmpF5;_tmpF5.f1=1,_tmpF5.f2=-2147483648;_tmpF5;});else{
return({struct _tuple12 _tmpF6;_tmpF6.f1=1,_tmpF6.f2=2147483647;_tmpF6;});}}else{
return({struct _tuple12 _tmpF7;_tmpF7.f1=0,_tmpF7.f2=sum;_tmpF7;});}}case Cyc_PrattProver_MINUS:  {
# 300
int _tmp3F=c1 - c2;int dif=_tmp3F;
if(((c1 ^ c2)& (c1 ^ dif))>> 31){
# 303
if(dif > 0)return({struct _tuple12 _tmpF8;_tmpF8.f1=1,_tmpF8.f2=-2147483648;_tmpF8;});else{
return({struct _tuple12 _tmpF9;_tmpF9.f1=1,_tmpF9.f2=2147483647;_tmpF9;});}}else{
return({struct _tuple12 _tmpFA;_tmpFA.f1=0,_tmpFA.f2=dif;_tmpFA;});}}default:
# 307
({void*_tmp40=0U;({int(*_tmp134)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp133=({const char*_tmp41="Pratt_Prover: bad operator in overflow check";_tag_fat(_tmp41,sizeof(char),45U);});_tmp134(_tmp133,_tag_fat(_tmp40,sizeof(void*),0));});});};}
# 315
static struct _tuple12 Cyc_PrattProver_is_unsigned_overflow(unsigned c1,enum Cyc_PrattProver_Operator op,unsigned c2){
# 317
enum Cyc_PrattProver_Operator _tmp42=op;if(_tmp42 == Cyc_PrattProver_MINUS){
# 319
int dif=(int)(c1 - c2);
if((int)(((c1 ^ c2)& ~(c1 ^ (unsigned)dif))>> 31U)){
# 322
if(dif > 0)return({struct _tuple12 _tmpFB;_tmpFB.f1=1,_tmpFB.f2=-2147483648;_tmpFB;});else{
return({struct _tuple12 _tmpFC;_tmpFC.f1=1,_tmpFC.f2=2147483647;_tmpFC;});}}else{
return({struct _tuple12 _tmpFD;_tmpFD.f1=0,_tmpFD.f2=dif;_tmpFD;});}}else{
# 326
({void*_tmp43=0U;({int(*_tmp136)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp135=({const char*_tmp44="Pratt_Prover: bad operator in overflow check";_tag_fat(_tmp44,sizeof(char),45U);});_tmp136(_tmp135,_tag_fat(_tmp43,sizeof(void*),0));});});};}
# 338 "pratt_prover.cyc"
static int Cyc_PrattProver_add_edge(struct Cyc_PrattProver_Graph*g,int is_signed,void*i,void*j,int dist,void*origin){
Cyc_PrattProver_add_node(g,i);
Cyc_PrattProver_add_node(g,j);{
struct Cyc_PrattProver_Dist*ij_dist=({struct Cyc_PrattProver_Row*_tmp138=Cyc_PrattProver_lookup_row(g,i);int _tmp137=is_signed;Cyc_PrattProver_lookup_distance(_tmp138,_tmp137,j);});
# 343
if(ij_dist != 0 && ij_dist->length < dist)
return 1;{
struct Cyc_PrattProver_Dist*ji_dist=({struct Cyc_PrattProver_Row*_tmp13A=Cyc_PrattProver_lookup_row(g,j);int _tmp139=is_signed;Cyc_PrattProver_lookup_distance(_tmp13A,_tmp139,i);});
# 347
if(ji_dist != 0){
struct _tuple12 _tmp45=Cyc_PrattProver_is_signed_overflow(ji_dist->length,0U,dist);struct _tuple12 _stmttmp4=_tmp45;struct _tuple12 _tmp46=_stmttmp4;int _tmp48;int _tmp47;_tmp47=_tmp46.f1;_tmp48=_tmp46.f2;{int overflow=_tmp47;int sum=_tmp48;
if(sum < 0)
# 351
return 0;else{
if(overflow && sum > 0)
# 356
return 1;}}}{
# 362
struct Cyc_PrattProver_Row*_tmp49=Cyc_PrattProver_lookup_row(g,j);struct Cyc_PrattProver_Row*jrow=_tmp49;
{struct Cyc_PrattProver_Row*_tmp4A=g->rows;struct Cyc_PrattProver_Row*ks=_tmp4A;for(0;ks != 0;ks=ks->next){
void*_tmp4B=ks->source;void*k=_tmp4B;
struct Cyc_PrattProver_Dist*_tmp4C=Cyc_PrattProver_lookup_distance(ks,is_signed,i);struct Cyc_PrattProver_Dist*ki_dist=_tmp4C;
if(ki_dist == 0)continue;{
struct Cyc_PrattProver_Row*_tmp4D=g->rows;struct Cyc_PrattProver_Row*ls=_tmp4D;for(0;ls != 0;ls=ls->next){
void*_tmp4E=ls->source;void*l=_tmp4E;
struct Cyc_PrattProver_Dist*_tmp4F=Cyc_PrattProver_lookup_distance(ks,is_signed,l);struct Cyc_PrattProver_Dist*kl_dist=_tmp4F;
# 371
if(kl_dist != 0 && kl_dist->length == -2147483648)continue;{
# 373
struct Cyc_PrattProver_Dist*_tmp50=Cyc_PrattProver_lookup_distance(jrow,is_signed,l);struct Cyc_PrattProver_Dist*jl_dist=_tmp50;
if(jl_dist == 0)continue;{
# 377
int final;
int sum;
struct _tuple12 _tmp51=Cyc_PrattProver_is_signed_overflow(ki_dist->length,0U,dist);struct _tuple12 _stmttmp5=_tmp51;struct _tuple12 _tmp52=_stmttmp5;int _tmp54;int _tmp53;_tmp53=_tmp52.f1;_tmp54=_tmp52.f2;{int of1=_tmp53;int sum1=_tmp54;
if(!of1){
# 382
struct _tuple12 _tmp55=Cyc_PrattProver_is_signed_overflow(sum1,0U,jl_dist->length);struct _tuple12 _stmttmp6=_tmp55;struct _tuple12 _tmp56=_stmttmp6;int _tmp58;int _tmp57;_tmp57=_tmp56.f1;_tmp58=_tmp56.f2;{int of2=_tmp57;int sum2=_tmp58;
final=of2;
sum=sum2;}}else{
# 388
struct _tuple12 _tmp59=Cyc_PrattProver_is_signed_overflow(ki_dist->length,0U,jl_dist->length);struct _tuple12 _stmttmp7=_tmp59;struct _tuple12 _tmp5A=_stmttmp7;int _tmp5C;int _tmp5B;_tmp5B=_tmp5A.f1;_tmp5C=_tmp5A.f2;{int of1=_tmp5B;int sum1=_tmp5C;
if(!of1){
struct _tuple12 _tmp5D=Cyc_PrattProver_is_signed_overflow(sum1,0U,dist);struct _tuple12 _stmttmp8=_tmp5D;struct _tuple12 _tmp5E=_stmttmp8;int _tmp60;int _tmp5F;_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{int of2=_tmp5F;int sum2=_tmp60;
final=of2;
sum=sum;}}else{
# 396
final=1;
if(dist < 0)
# 399
sum=-2147483648;else{
# 402
sum=2147483647;}}}}
# 406
if(!final || sum < 0){
# 409
if(kl_dist == 0 || kl_dist->length > sum)
Cyc_PrattProver_insert_distance(ks,is_signed,l,sum,0);}}}}}}}}
# 414
return 1;}}}}
# 417
static struct _tuple11 Cyc_PrattProver_eval_term(void*t){
void*_tmp61=t;void*_tmp62;unsigned _tmp63;switch(*((int*)_tmp61)){case 0: _tmp63=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp61)->f1;{unsigned i=_tmp63;
return({struct _tuple11 _tmpFE;_tmpFE.f1=i,_tmpFE.f2=1;_tmpFE;});}case 1: _tmp62=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp61)->f1;{struct Cyc_Absyn_Exp*e=_tmp62;
return Cyc_Evexp_eval_const_uint_exp(e);}default:
 return({struct _tuple11 _tmpFF;_tmpFF.f1=0U,_tmpFF.f2=0;_tmpFF;});};}struct _tuple13{void*f1;int f2;};
# 425
static struct _tuple13 Cyc_PrattProver_break_term(struct Cyc_PrattProver_Graph*g,int is_signed,void*t){
int c=0;
unsigned cu=0U;
void*res=t;
enum Cyc_Absyn_Primop p;
{void*_tmp64=t;void*_tmp65;if(*((int*)_tmp64)== 4)switch((int)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp64)->f1){case Cyc_Absyn_Plus: _tmp65=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp64)->f2;{struct Cyc_List_List*ts=_tmp65;
# 432
p=0U;
if(ts == 0)
({void*_tmp66=0U;({int(*_tmp13C)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp13B=({const char*_tmp67="no argument for primop";_tag_fat(_tmp67,sizeof(char),23U);});_tmp13C(_tmp13B,_tag_fat(_tmp66,sizeof(void*),0));});});else{
if(ts->tl == 0)
# 437
return({struct _tuple13 _tmp100;_tmp100.f1=(void*)ts->hd,_tmp100.f2=0;_tmp100;});else{
if(((struct Cyc_List_List*)_check_null(ts->tl))->tl == 0){
struct _tuple11 _tmp68=Cyc_PrattProver_eval_term((void*)ts->hd);struct _tuple11 _stmttmp9=_tmp68;struct _tuple11 _tmp69=_stmttmp9;int _tmp6B;unsigned _tmp6A;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{unsigned c1=_tmp6A;int okay1=_tmp6B;
struct _tuple11 _tmp6C=Cyc_PrattProver_eval_term((void*)((struct Cyc_List_List*)_check_null(ts->tl))->hd);struct _tuple11 _stmttmpA=_tmp6C;struct _tuple11 _tmp6D=_stmttmpA;int _tmp6F;unsigned _tmp6E;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;{unsigned c2=_tmp6E;int okay2=_tmp6F;
if(okay1){
res=(void*)((struct Cyc_List_List*)_check_null(ts->tl))->hd;
c=(int)c1;
cu=c1;}else{
if(okay2){
res=(void*)ts->hd;
c=(int)c2;
cu=c2;}else{
return({struct _tuple13 _tmp101;_tmp101.f1=t,_tmp101.f2=0;_tmp101;});}}}}}else{
# 452
return({struct _tuple13 _tmp102;_tmp102.f1=t,_tmp102.f2=0;_tmp102;});}}}
goto _LL0;}case Cyc_Absyn_Minus: _tmp65=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp64)->f2;{struct Cyc_List_List*ts=_tmp65;
# 455
p=2U;
if(ts == 0)
({void*_tmp70=0U;({int(*_tmp13E)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp13D=({const char*_tmp71="no argument for primop";_tag_fat(_tmp71,sizeof(char),23U);});_tmp13E(_tmp13D,_tag_fat(_tmp70,sizeof(void*),0));});});else{
if(ts->tl == 0)
# 460
return({struct _tuple13 _tmp103;_tmp103.f1=t,_tmp103.f2=0;_tmp103;});else{
if(((struct Cyc_List_List*)_check_null(ts->tl))->tl == 0){
struct _tuple11 _tmp72=Cyc_PrattProver_eval_term((void*)((struct Cyc_List_List*)_check_null(ts->tl))->hd);struct _tuple11 _stmttmpB=_tmp72;struct _tuple11 _tmp73=_stmttmpB;int _tmp75;unsigned _tmp74;_tmp74=_tmp73.f1;_tmp75=_tmp73.f2;{unsigned c2=_tmp74;int okay2=_tmp75;
if(okay2){
res=(void*)ts->hd;
c=-(int)c2;
cu=c2;}else{
# 468
return({struct _tuple13 _tmp104;_tmp104.f1=t,_tmp104.f2=0;_tmp104;});}}}else{
return({struct _tuple13 _tmp105;_tmp105.f1=t,_tmp105.f2=0;_tmp105;});}}}
goto _LL0;}default: goto _LL5;}else{_LL5:
# 472
 return({struct _tuple13 _tmp106;_tmp106.f1=t,_tmp106.f2=0;_tmp106;});}_LL0:;}
# 476
if(is_signed){
if(c == 0)return({struct _tuple13 _tmp107;_tmp107.f1=res,_tmp107.f2=0;_tmp107;});else{
if(c > 0){
# 480
struct Cyc_PrattProver_Dist*_tmp76=({struct Cyc_PrattProver_Row*_tmp13F=Cyc_PrattProver_lookup_row(g,res);Cyc_PrattProver_lookup_distance(_tmp13F,1,Cyc_AssnDef_zero());});struct Cyc_PrattProver_Dist*dist=_tmp76;
if(dist == 0 || dist->length > 2147483647 - c)
return({struct _tuple13 _tmp108;_tmp108.f1=t,_tmp108.f2=0;_tmp108;});else{
# 484
return({struct _tuple13 _tmp109;_tmp109.f1=res,_tmp109.f2=c;_tmp109;});}}else{
# 487
struct Cyc_PrattProver_Dist*_tmp77=({struct Cyc_PrattProver_Row*_tmp141=({struct Cyc_PrattProver_Graph*_tmp140=g;Cyc_PrattProver_lookup_row(_tmp140,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_distance(_tmp141,1,res);});struct Cyc_PrattProver_Dist*dist=_tmp77;
if(dist == 0 || dist->length > c - -2147483648)
return({struct _tuple13 _tmp10A;_tmp10A.f1=t,_tmp10A.f2=0;_tmp10A;});else{
# 494
return({struct _tuple13 _tmp10B;_tmp10B.f1=res,_tmp10B.f2=c;_tmp10B;});}}}}else{
# 498
if((int)p == 0){
struct Cyc_PrattProver_Dist*_tmp78=({struct Cyc_PrattProver_Row*_tmp142=Cyc_PrattProver_lookup_row(g,res);Cyc_PrattProver_lookup_distance(_tmp142,0,Cyc_AssnDef_zero());});struct Cyc_PrattProver_Dist*dist=_tmp78;
if(dist == 0)return({struct _tuple13 _tmp10C;_tmp10C.f1=t,_tmp10C.f2=0;_tmp10C;});
if(dist->length <= 0)return({struct _tuple13 _tmp10D;_tmp10D.f1=res,_tmp10D.f2=(int)cu;_tmp10D;});
if(dist->length > 0 &&(unsigned)dist->length <= 4294967295U - cu)
return({struct _tuple13 _tmp10E;_tmp10E.f1=res,_tmp10E.f2=(int)cu;_tmp10E;});}else{
# 506
return({struct _tuple13 _tmp10F;_tmp10F.f1=t,_tmp10F.f2=0;_tmp10F;});}}
# 509
return({struct _tuple13 _tmp110;_tmp110.f1=t,_tmp110.f2=0;_tmp110;});}
# 514
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
# 517
struct _tuple11 _tmp79=Cyc_PrattProver_eval_term(t1);struct _tuple11 _stmttmpC=_tmp79;struct _tuple11 _tmp7A=_stmttmpC;int _tmp7C;unsigned _tmp7B;_tmp7B=_tmp7A.f1;_tmp7C=_tmp7A.f2;{unsigned c1=_tmp7B;int okay1=_tmp7C;
struct _tuple11 _tmp7D=Cyc_PrattProver_eval_term(t2);struct _tuple11 _stmttmpD=_tmp7D;struct _tuple11 _tmp7E=_stmttmpD;int _tmp80;unsigned _tmp7F;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;{unsigned c2=_tmp7F;int okay2=_tmp80;
void*_tmp81=Cyc_AssnDef_prim(t1,p,t2);void*origin=_tmp81;
# 522
if(okay1 && okay2){
# 524
enum Cyc_AssnDef_Primreln _tmp82=p;switch((int)_tmp82){case Cyc_AssnDef_ULt:
 return c1 < c2;case Cyc_AssnDef_SLt:
 return(int)c1 < (int)c2;case Cyc_AssnDef_ULte:
 return c1 <= c2;case Cyc_AssnDef_SLte:
 return(int)c1 <= (int)c2;default:
({void*_tmp83=0U;({int(*_tmp144)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp143=({const char*_tmp84="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp84,sizeof(char),42U);});_tmp144(_tmp143,_tag_fat(_tmp83,sizeof(void*),0));});});};}else{
# 531
if(okay2){
# 533
enum Cyc_AssnDef_Primreln _tmp85=p;switch((int)_tmp85){case Cyc_AssnDef_ULt:
# 536
 if(c2 == 0U)return 0;
c2=c2 - 1U;
goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 541
 if(c2 <= 2147483647U){
if(!({struct Cyc_PrattProver_Graph*_tmp148=g;void*_tmp147=t1;void*_tmp146=Cyc_AssnDef_zero();int _tmp145=(int)c2;Cyc_PrattProver_add_edge(_tmp148,0,_tmp147,_tmp146,_tmp145,origin);}))return 0;
# 544
return({struct Cyc_PrattProver_Graph*_tmp14B=g;void*_tmp14A=t1;void*_tmp149=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmp14B,1,_tmp14A,_tmp149,(int)c2,0);});}else{
# 546
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_tmp14C=Cyc_PrattProver_lookup_row(g,t1);Cyc_PrattProver_lookup_distance(_tmp14C,1,Cyc_AssnDef_zero());});
if(c1 != 0 && c1->length <= -1)
return({struct Cyc_PrattProver_Graph*_tmp14F=g;void*_tmp14E=t1;void*_tmp14D=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmp14F,1,_tmp14E,_tmp14D,(int)c2,0);});}
# 551
return 1;case Cyc_AssnDef_SLt:
# 554
 if(c2 == 2147483648U)return 0;
c2=c2 - 1U;
goto _LL19;case Cyc_AssnDef_SLte: _LL19:
# 559
 if(!({struct Cyc_PrattProver_Graph*_tmp153=g;void*_tmp152=t1;void*_tmp151=Cyc_AssnDef_zero();int _tmp150=(int)c2;Cyc_PrattProver_add_edge(_tmp153,1,_tmp152,_tmp151,_tmp150,origin);}))return 0;
if((int)c2 > 0){
# 562
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_tmp155=({struct Cyc_PrattProver_Graph*_tmp154=g;Cyc_PrattProver_lookup_row(_tmp154,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_distance(_tmp155,1,t1);});
if(c1 != 0 && c1->length <= 0)
# 565
return({struct Cyc_PrattProver_Graph*_tmp158=g;void*_tmp157=t1;void*_tmp156=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_tmp158,0,_tmp157,_tmp156,(int)c2,0);});}
# 567
return 1;default:
({void*_tmp86=0U;({int(*_tmp15A)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp159=({const char*_tmp87="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp87,sizeof(char),42U);});_tmp15A(_tmp159,_tag_fat(_tmp86,sizeof(void*),0));});});};}else{
# 570
if(okay1){
# 572
enum Cyc_AssnDef_Primreln _tmp88=p;switch((int)_tmp88){case Cyc_AssnDef_ULt:
# 575
 if(c1 == 4294967295U)return 0;
c1=c1 + 1U;
goto _LL20;case Cyc_AssnDef_ULte: _LL20:
# 580
 if(c1 > 2147483647U){
# 583
if(!({struct Cyc_PrattProver_Graph*_tmp15C=g;void*_tmp15B=t2;Cyc_PrattProver_add_edge(_tmp15C,1,_tmp15B,Cyc_AssnDef_zero(),-1,0);}))return 0;{
int k=-(int)c1;
if(k > 0){
if(!({struct Cyc_PrattProver_Graph*_tmp15F=g;void*_tmp15E=Cyc_AssnDef_zero();void*_tmp15D=t2;Cyc_PrattProver_add_edge(_tmp15F,1,_tmp15E,_tmp15D,k,0);}))return 0;}}}
# 589
if(c1 <= 2147483648U){
# 591
int k=-(int)c1;
return({struct Cyc_PrattProver_Graph*_tmp163=g;void*_tmp162=Cyc_AssnDef_zero();void*_tmp161=t2;int _tmp160=k;Cyc_PrattProver_add_edge(_tmp163,0,_tmp162,_tmp161,_tmp160,origin);});}
# 594
return 1;case Cyc_AssnDef_SLt:
# 597
 if(c1 == 2147483647U)return 0;
c1=c1 + 1U;
goto _LL24;case Cyc_AssnDef_SLte: _LL24:
# 602
 if(c1 == 2147483648U)return 1;else{
# 604
int k=-(int)c1;
if((int)c1 >= 0){
# 607
if(!({struct Cyc_PrattProver_Graph*_tmp166=g;void*_tmp165=Cyc_AssnDef_zero();void*_tmp164=t2;Cyc_PrattProver_add_edge(_tmp166,0,_tmp165,_tmp164,k,0);}))return 0;}
# 609
return({struct Cyc_PrattProver_Graph*_tmp16A=g;void*_tmp169=Cyc_AssnDef_zero();void*_tmp168=t2;int _tmp167=k;Cyc_PrattProver_add_edge(_tmp16A,1,_tmp169,_tmp168,_tmp167,origin);});}default:
# 611
({void*_tmp89=0U;({int(*_tmp16C)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp16B=({const char*_tmp8A="Vcgen: found bad primop in add_constraint";_tag_fat(_tmp8A,sizeof(char),42U);});_tmp16C(_tmp16B,_tag_fat(_tmp89,sizeof(void*),0));});});};}}}{
# 618
enum Cyc_AssnDef_Primreln _tmp8B=p;switch((int)_tmp8B){case Cyc_AssnDef_ULt:  {
# 621
struct _tuple13 _tmp8C=Cyc_PrattProver_break_term(g,0,t1);struct _tuple13 _stmttmpE=_tmp8C;struct _tuple13 _tmp8D=_stmttmpE;int _tmp8F;void*_tmp8E;_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;{void*tt1=_tmp8E;int c1=_tmp8F;
struct _tuple13 _tmp90=Cyc_PrattProver_break_term(g,0,t2);struct _tuple13 _stmttmpF=_tmp90;struct _tuple13 _tmp91=_stmttmpF;int _tmp93;void*_tmp92;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{void*tt2=_tmp92;int c2=_tmp93;
struct _tuple12 _tmp94=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);struct _tuple12 _stmttmp10=_tmp94;struct _tuple12 _tmp95=_stmttmp10;int _tmp97;int _tmp96;_tmp96=_tmp95.f1;_tmp97=_tmp95.f2;{int of=_tmp96;int dist=_tmp97;
if(!of){
struct _tuple12 _tmp98=Cyc_PrattProver_is_unsigned_overflow((unsigned)dist,1U,1U);struct _tuple12 _stmttmp11=_tmp98;struct _tuple12 _tmp99=_stmttmp11;int _tmp9B;int _tmp9A;_tmp9A=_tmp99.f1;_tmp9B=_tmp99.f2;{int of=_tmp9A;int dist=_tmp9B;
if(!of){
if(!Cyc_PrattProver_add_edge(g,0,tt1,tt2,dist,origin))
return 0;}}}
# 631
return Cyc_PrattProver_add_edge(g,0,t1,t2,-1,origin);}}}}case Cyc_AssnDef_ULte:  {
# 634
struct _tuple13 _tmp9C=Cyc_PrattProver_break_term(g,0,t1);struct _tuple13 _stmttmp12=_tmp9C;struct _tuple13 _tmp9D=_stmttmp12;int _tmp9F;void*_tmp9E;_tmp9E=_tmp9D.f1;_tmp9F=_tmp9D.f2;{void*tt1=_tmp9E;int c1=_tmp9F;
struct _tuple13 _tmpA0=Cyc_PrattProver_break_term(g,0,t2);struct _tuple13 _stmttmp13=_tmpA0;struct _tuple13 _tmpA1=_stmttmp13;int _tmpA3;void*_tmpA2;_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;{void*tt2=_tmpA2;int c2=_tmpA3;
struct _tuple12 _tmpA4=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);struct _tuple12 _stmttmp14=_tmpA4;struct _tuple12 _tmpA5=_stmttmp14;int _tmpA7;int _tmpA6;_tmpA6=_tmpA5.f1;_tmpA7=_tmpA5.f2;{int of=_tmpA6;int dist=_tmpA7;
if(!of){
# 640
if(!Cyc_PrattProver_add_edge(g,0,tt1,tt2,dist,origin))
return 0;}
# 643
return Cyc_PrattProver_add_edge(g,0,t1,t2,0,origin);}}}}case Cyc_AssnDef_SLt:  {
# 645
struct _tuple13 _tmpA8=Cyc_PrattProver_break_term(g,1,t1);struct _tuple13 _stmttmp15=_tmpA8;struct _tuple13 _tmpA9=_stmttmp15;int _tmpAB;void*_tmpAA;_tmpAA=_tmpA9.f1;_tmpAB=_tmpA9.f2;{void*tt1=_tmpAA;int c1=_tmpAB;
struct _tuple13 _tmpAC=Cyc_PrattProver_break_term(g,1,t2);struct _tuple13 _stmttmp16=_tmpAC;struct _tuple13 _tmpAD=_stmttmp16;int _tmpAF;void*_tmpAE;_tmpAE=_tmpAD.f1;_tmpAF=_tmpAD.f2;{void*tt2=_tmpAE;int c2=_tmpAF;
struct _tuple12 _tmpB0=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);struct _tuple12 _stmttmp17=_tmpB0;struct _tuple12 _tmpB1=_stmttmp17;int _tmpB3;int _tmpB2;_tmpB2=_tmpB1.f1;_tmpB3=_tmpB1.f2;{int of=_tmpB2;int dist=_tmpB3;
if(!of){
struct _tuple12 _tmpB4=Cyc_PrattProver_is_signed_overflow(dist,1U,1);struct _tuple12 _stmttmp18=_tmpB4;struct _tuple12 _tmpB5=_stmttmp18;int _tmpB7;int _tmpB6;_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;{int of=_tmpB6;int dist=_tmpB7;
if(!of){
if(!Cyc_PrattProver_add_edge(g,1,tt1,tt2,dist,origin))
return 0;}}}
# 655
return Cyc_PrattProver_add_edge(g,1,t1,t2,-1,origin);}}}}case Cyc_AssnDef_SLte:  {
# 658
struct _tuple13 _tmpB8=Cyc_PrattProver_break_term(g,1,t1);struct _tuple13 _stmttmp19=_tmpB8;struct _tuple13 _tmpB9=_stmttmp19;int _tmpBB;void*_tmpBA;_tmpBA=_tmpB9.f1;_tmpBB=_tmpB9.f2;{void*tt1=_tmpBA;int c1=_tmpBB;
struct _tuple13 _tmpBC=Cyc_PrattProver_break_term(g,1,t2);struct _tuple13 _stmttmp1A=_tmpBC;struct _tuple13 _tmpBD=_stmttmp1A;int _tmpBF;void*_tmpBE;_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;{void*tt2=_tmpBE;int c2=_tmpBF;
struct _tuple12 _tmpC0=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);struct _tuple12 _stmttmp1B=_tmpC0;struct _tuple12 _tmpC1=_stmttmp1B;int _tmpC3;int _tmpC2;_tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;{int of=_tmpC2;int dist=_tmpC3;
if(!of){
if(!Cyc_PrattProver_add_edge(g,1,tt1,tt2,dist,origin))
return 0;}
# 665
return Cyc_PrattProver_add_edge(g,1,t1,t2,0,origin);}}}}default:
({void*_tmpC4=0U;({int(*_tmp16E)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp16D=({const char*_tmpC5="Vcgen:found bad primop in add_constraint";_tag_fat(_tmpC5,sizeof(char),41U);});_tmp16E(_tmp16D,_tag_fat(_tmpC4,sizeof(void*),0));});});};}}}}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 671
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 674
unsigned Cyc_PrattProver_max_paths=33U;
unsigned Cyc_PrattProver_max_paths_seen=0U;
# 683
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_cgraph(struct Cyc_PrattProver_Graph*gs,void*a){
LOOP:
# 687
 if(gs == 0)return gs;
{void*_tmpC6=a;enum Cyc_AssnDef_Primreln _tmpC8;void*_tmpC9;void*_tmpC7;switch(*((int*)_tmpC6)){case 0:
 goto _LL0;case 1:
 gs=0;goto _LL0;case 3: _tmpC7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpC6)->f1;_tmpC9=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpC6)->f2;{void*a1=_tmpC7;void*a2=_tmpC9;
# 692
gs=Cyc_PrattProver_cgraph(gs,a1);
a=a2;
goto LOOP;}case 4: _tmpC7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpC6)->f1;_tmpC9=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpC6)->f2;{void*a1=_tmpC7;void*a2=_tmpC9;
# 696
unsigned n=Cyc_PrattProver_num_graphs(gs);
if(Cyc_PrattProver_max_paths_seen < n)Cyc_PrattProver_max_paths_seen=n;
# 699
if(n >= Cyc_PrattProver_max_paths)(int)_throw((void*)& Cyc_PrattProver_too_large);{
# 701
struct Cyc_PrattProver_Graph*gs1=gs;
struct Cyc_PrattProver_Graph*gs2=Cyc_PrattProver_copy_graph(gs);
# 704
gs1=Cyc_PrattProver_cgraph(gs1,a1);
# 706
gs2=Cyc_PrattProver_cgraph(gs2,a2);
# 708
gs=Cyc_PrattProver_graph_append(gs1,gs2);
goto _LL0;}}case 2: switch((int)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f2){case Cyc_AssnDef_Neq: _tmpC7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f1;_tmpC9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f3;{void*t1=_tmpC7;void*t2=_tmpC9;
# 713
a=({void*_tmp171=({void*_tmp16F=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_tmp16F,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_tmp171,({void*_tmp170=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_tmp170,Cyc_AssnDef_ult(t2,t1));}));});
goto LOOP;}case Cyc_AssnDef_Eq: _tmpC7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f1;_tmpC9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f3;{void*t1=_tmpC7;void*t2=_tmpC9;
# 717
a=({void*_tmp174=({void*_tmp173=({void*_tmp172=Cyc_AssnDef_slte(t1,t2);Cyc_AssnDef_and(_tmp172,Cyc_AssnDef_slte(t2,t1));});Cyc_AssnDef_and(_tmp173,Cyc_AssnDef_ulte(t1,t2));});Cyc_AssnDef_and(_tmp174,Cyc_AssnDef_ulte(t2,t1));});
goto LOOP;}default: _tmpC7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f2;_tmpC9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpC6)->f3;{void*t1=_tmpC7;enum Cyc_AssnDef_Primreln p=_tmpC8;void*t2=_tmpC9;
# 722
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*_tmpCA=gs;struct Cyc_PrattProver_Graph*g=_tmpCA;for(0;g != 0;g=g->next){
if(!Cyc_PrattProver_add_constraint(g,t1,p,t2)){
if(prev == 0)
gs=g->next;else{
# 728
prev->next=g->next;}}else{
# 731
prev=g;}}}
# 734
goto _LL0;}}default:
# 737
 a=Cyc_AssnDef_reduce(a);
goto LOOP;}_LL0:;}
# 741
return gs;}
# 747
static int Cyc_PrattProver_consistent(void*a){
struct _handler_cons _tmpCB;_push_handler(& _tmpCB);{int _tmpCD=0;if(setjmp(_tmpCB.handler))_tmpCD=1;if(!_tmpCD){
# 750
{struct _fat_ptr gs=_tag_fat(({struct Cyc_PrattProver_Graph*_tmp175=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_tmp175,a);}),sizeof(struct Cyc_PrattProver_Graph),1U);
# 753
int _tmpCE=(struct Cyc_PrattProver_Graph*)gs.curr != (struct Cyc_PrattProver_Graph*)(_tag_fat(0,0,0)).curr;_npop_handler(0);return _tmpCE;}
# 750
;_pop_handler();}else{void*_tmpCC=(void*)Cyc_Core_get_exn_thrown();void*_tmpCF=_tmpCC;void*_tmpD0;if(((struct Cyc_PrattProver_TooLarge_exn_struct*)_tmpCF)->tag == Cyc_PrattProver_TooLarge)
# 757
return 1;else{_tmpD0=_tmpCF;{void*exn=_tmpD0;(int)_rethrow(exn);}};}}}
# 762
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
void*b=({void*_tmp176=ctxt;Cyc_AssnDef_and(_tmp176,Cyc_AssnDef_not(a));});
# 767
return !Cyc_PrattProver_consistent(b);}
# 770
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row_in_rows(struct Cyc_PrattProver_Row*r,void*i){
for(1;r != 0;r=r->next){
if(Cyc_AssnDef_cmp_term(i,r->source)== 0)return r;}
# 774
return 0;}
# 777
static struct Cyc_PrattProver_Row*Cyc_PrattProver_or2rows(struct Cyc_PrattProver_Row*g1,struct Cyc_PrattProver_Row*g2){
struct Cyc_PrattProver_Row*res=0;
for(1;g1 != 0;g1=g1->next){
struct Cyc_PrattProver_Row*_tmpD1=Cyc_PrattProver_lookup_row_in_rows(g2,g1->source);struct Cyc_PrattProver_Row*r=_tmpD1;
if(r != 0){
# 783
struct Cyc_PrattProver_Row*_tmpD2=({struct Cyc_PrattProver_Row*_tmpDE=_cycalloc(sizeof(*_tmpDE));*_tmpDE=*g1;_tmpDE;});struct Cyc_PrattProver_Row*newrow=_tmpD2;
newrow->next=res;
res=newrow;
res->distance=0;{
# 788
struct Cyc_PrattProver_Distance*_tmpD3=g1->distance;struct Cyc_PrattProver_Distance*d=_tmpD3;for(0;d != 0;d=d->next){
struct Cyc_PrattProver_Distance*dist_res=0;
if((int)d->dinfo & 2){
int _tmpD4=(d->signed_dist).length;int d1=_tmpD4;
struct Cyc_PrattProver_Dist*_tmpD5=Cyc_PrattProver_lookup_distance(r,1,d->target);struct Cyc_PrattProver_Dist*d2_ptr=_tmpD5;
if(d2_ptr != 0){
int _tmpD6=d2_ptr->length;int d2=_tmpD6;
struct Cyc_PrattProver_Dist _tmpD7=d1 > d2?d->signed_dist:*d2_ptr;struct Cyc_PrattProver_Dist newd=_tmpD7;
dist_res=({struct Cyc_PrattProver_Distance*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->target=d->target,_tmpD8->next=res->distance,_tmpD8->dinfo=2,
# 799
(_tmpD8->unsigned_dist).length=0,(_tmpD8->unsigned_dist).prim=0,_tmpD8->signed_dist=newd;_tmpD8;});}}
# 803
if((int)d->dinfo & 1){
int _tmpD9=(d->unsigned_dist).length;int d1=_tmpD9;
struct Cyc_PrattProver_Dist*_tmpDA=Cyc_PrattProver_lookup_distance(r,0,d->target);struct Cyc_PrattProver_Dist*d2_ptr=_tmpDA;
if(d2_ptr != 0){
int _tmpDB=d2_ptr->length;int d2=_tmpDB;
struct Cyc_PrattProver_Dist _tmpDC=d1 > d2?d->unsigned_dist:*d2_ptr;struct Cyc_PrattProver_Dist newd=_tmpDC;
if(dist_res != 0){
dist_res->unsigned_dist=newd;
dist_res->dinfo=3;}else{
# 813
dist_res=({struct Cyc_PrattProver_Distance*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->target=d->target,_tmpDD->next=res->distance,_tmpDD->dinfo=1,_tmpDD->unsigned_dist=newd,
# 817
(_tmpDD->signed_dist).length=0,(_tmpDD->signed_dist).prim=0;_tmpDD;});}}}
# 821
if(dist_res != 0)
res->distance=dist_res;}}}}
# 827
return res;}
# 830
void*Cyc_PrattProver_int2term(int is_signed,int c){
enum Cyc_Absyn_Sign _tmpDF=is_signed?0U: 1U;enum Cyc_Absyn_Sign s=_tmpDF;
struct Cyc_Absyn_Exp*_tmpE0=Cyc_Absyn_int_exp(s,c,0U);struct Cyc_Absyn_Exp*e=_tmpE0;
({void*_tmp177=Cyc_Absyn_int_type(s,2U);e->topt=_tmp177;});
return Cyc_AssnDef_cnst(e);}
# 837
void*Cyc_PrattProver_edge2assn(void*source,void*target,int weight,int is_signed){
enum Cyc_AssnDef_Primreln _tmpE1=is_signed?3U: 5U;enum Cyc_AssnDef_Primreln s=_tmpE1;
enum Cyc_AssnDef_Primreln _tmpE2=is_signed?2U: 4U;enum Cyc_AssnDef_Primreln s1=_tmpE2;
void*result=(void*)& Cyc_AssnDef_true_assn;
if(({void*_tmp178=source;_tmp178 == Cyc_AssnDef_zero();}))
result=({void*_tmp17A=Cyc_PrattProver_int2term(is_signed,- weight);enum Cyc_AssnDef_Primreln _tmp179=s;Cyc_AssnDef_prim(_tmp17A,_tmp179,target);});else{
if(({void*_tmp17B=target;_tmp17B == Cyc_AssnDef_zero();}))
result=({void*_tmp17D=source;enum Cyc_AssnDef_Primreln _tmp17C=s;Cyc_AssnDef_prim(_tmp17D,_tmp17C,Cyc_PrattProver_int2term(is_signed,weight));});else{
# 846
int _tmpE3=weight;switch((int)_tmpE3){case 0:
 result=Cyc_AssnDef_prim(source,s,target);goto _LL0;case -1:
 result=Cyc_AssnDef_prim(source,s1,target);goto _LL0;default:
# 854
 goto _LL0;}_LL0:;}}
# 857
return result;}
# 861
void*Cyc_PrattProver_row2assn(struct Cyc_PrattProver_Row*r){
void*a=(void*)& Cyc_AssnDef_true_assn;
for(1;r != 0;r=r->next){
struct Cyc_PrattProver_Distance*_tmpE4=r->distance;struct Cyc_PrattProver_Distance*d=_tmpE4;for(0;d != 0;d=d->next){
if((int)d->dinfo & 2){
if((d->signed_dist).prim != 0)
a=Cyc_AssnDef_and(a,(void*)_check_null((d->signed_dist).prim));else{
# 869
a=({void*_tmp17E=a;Cyc_AssnDef_and(_tmp17E,Cyc_PrattProver_edge2assn(r->source,d->target,(d->signed_dist).length,1));});}}
# 872
if((int)d->dinfo & 1){
if((d->unsigned_dist).prim != 0)
a=Cyc_AssnDef_and(a,(void*)_check_null((d->unsigned_dist).prim));else{
# 876
a=({void*_tmp17F=a;Cyc_AssnDef_and(_tmp17F,Cyc_PrattProver_edge2assn(r->source,d->target,(d->unsigned_dist).length,0));});}}}}
# 881
return a;}
# 884
void*Cyc_PrattProver_merge_assn(void*a1,void*a2){
# 886
struct Cyc_PrattProver_Graph*_tmpE5=({struct Cyc_PrattProver_Graph*_tmp180=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_tmp180,Cyc_AssnDef_or(a1,a2));});struct Cyc_PrattProver_Graph*g=_tmpE5;
if(g == 0)return(void*)& Cyc_AssnDef_false_assn;{
struct Cyc_PrattProver_Row*_tmpE6=g->rows;struct Cyc_PrattProver_Row*row=_tmpE6;
{struct Cyc_PrattProver_Graph*_tmpE7=g->next;struct Cyc_PrattProver_Graph*row1=_tmpE7;for(0;row1 != 0;row1=row1->next){
row=Cyc_PrattProver_or2rows(row,row1->rows);}}
# 892
return Cyc_PrattProver_row2assn(row);}}

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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);extern char Cyc_Set_Absent[7U];extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];extern char Cyc_CfFlowInfo_NotZero[8U];extern char Cyc_CfFlowInfo_UnknownZ[9U];
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
# 13
extern char*Cyc_AP_tostr(struct Cyc_AP_T*,int);
# 16
extern struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 32
extern int Cyc_AP_cmp(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned val;};struct _union_Node_NParamNumelts{int tag;unsigned val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 61 "pratt-ap.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;struct Cyc_Pratt_Graph;
# 85 "pratt-ap.cyc"
union Cyc_Pratt_Node Cyc_Pratt_zero_node={.NZero={1,0}};
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*vd){return({union Cyc_Pratt_Node _tmp40;(_tmp40.NVar).tag=2U,(_tmp40.NVar).val=vd;_tmp40;});}
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*vd){return({union Cyc_Pratt_Node _tmp41;(_tmp41.NNumelts).tag=3U,(_tmp41.NNumelts).val=vd;_tmp41;});}
union Cyc_Pratt_Node Cyc_Pratt_NType(void*t){return({union Cyc_Pratt_Node _tmp42;(_tmp42.NType).tag=4U,(_tmp42.NType).val=t;_tmp42;});}
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned i){return({union Cyc_Pratt_Node _tmp43;(_tmp43.NParam).tag=5U,(_tmp43.NParam).val=i;_tmp43;});}
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned i){return({union Cyc_Pratt_Node _tmp44;(_tmp44.NParamNumelts).tag=6U,(_tmp44.NParamNumelts).val=i;_tmp44;});}
union Cyc_Pratt_Node Cyc_Pratt_NReturn (void){return({union Cyc_Pratt_Node _tmp45;(_tmp45.NReturn).tag=7U,(_tmp45.NReturn).val=0;_tmp45;});}struct _tuple12{union Cyc_Pratt_Node f1;union Cyc_Pratt_Node f2;};
# 93
static int Cyc_Pratt_cmp_node(union Cyc_Pratt_Node n1,union Cyc_Pratt_Node n2){
struct _tuple12 _tmp0=({struct _tuple12 _tmp46;_tmp46.f1=n1,_tmp46.f2=n2;_tmp46;});struct _tuple12 _stmttmp0=_tmp0;struct _tuple12 _tmp1=_stmttmp0;unsigned _tmp3;unsigned _tmp2;void*_tmp5;void*_tmp4;if(((_tmp1.f1).NZero).tag == 1){if(((_tmp1.f2).NZero).tag == 1)
return 0;else{
return -1;}}else{if(((_tmp1.f2).NZero).tag == 1)
return 1;else{if(((_tmp1.f1).NReturn).tag == 7){if(((_tmp1.f2).NReturn).tag == 7)
return 0;else{
return -1;}}else{if(((_tmp1.f2).NReturn).tag == 7)
return 1;else{if(((_tmp1.f1).NVar).tag == 2){if(((_tmp1.f2).NVar).tag == 2){_tmp4=((_tmp1.f1).NVar).val;_tmp5=((_tmp1.f2).NVar).val;{struct Cyc_Absyn_Vardecl*x=_tmp4;struct Cyc_Absyn_Vardecl*y=_tmp5;
return(int)x - (int)y;}}else{
return -1;}}else{if(((_tmp1.f2).NVar).tag == 2)
return 1;else{if(((_tmp1.f1).NNumelts).tag == 3){if(((_tmp1.f2).NNumelts).tag == 3){_tmp4=((_tmp1.f1).NNumelts).val;_tmp5=((_tmp1.f2).NNumelts).val;{struct Cyc_Absyn_Vardecl*x=_tmp4;struct Cyc_Absyn_Vardecl*y=_tmp5;
return(int)x - (int)y;}}else{_tmp4=((_tmp1.f1).NNumelts).val;{struct Cyc_Absyn_Vardecl*x=_tmp4;
return -1;}}}else{if(((_tmp1.f2).NNumelts).tag == 3){_tmp4=((_tmp1.f2).NNumelts).val;{struct Cyc_Absyn_Vardecl*y=_tmp4;
return 1;}}else{if(((_tmp1.f1).NType).tag == 4){if(((_tmp1.f2).NType).tag == 4){_tmp4=((_tmp1.f1).NType).val;_tmp5=((_tmp1.f2).NType).val;{void*x=_tmp4;void*y=_tmp5;
# 108
if(Cyc_Unify_unify(x,y))return 0;
return Cyc_Tcutil_typecmp(x,y);}}else{
return -1;}}else{if(((_tmp1.f2).NType).tag == 4)
return 1;else{if(((_tmp1.f1).NParam).tag == 5){if(((_tmp1.f2).NParam).tag == 5){_tmp2=((_tmp1.f1).NParam).val;_tmp3=((_tmp1.f2).NParam).val;{unsigned i=_tmp2;unsigned j=_tmp3;
return(int)i - (int)j;}}else{_tmp2=((_tmp1.f1).NParam).val;{unsigned i=_tmp2;
return -1;}}}else{if(((_tmp1.f2).NParam).tag == 5){_tmp2=((_tmp1.f2).NParam).val;{unsigned j=_tmp2;
return 1;}}else{_tmp2=((_tmp1.f1).NParamNumelts).val;_tmp3=((_tmp1.f2).NParamNumelts).val;{unsigned i=_tmp2;unsigned j=_tmp3;
return(int)i - (int)j;}}}}}}}}}}}}};}
# 119
struct _fat_ptr Cyc_Pratt_node2string(union Cyc_Pratt_Node n){
union Cyc_Pratt_Node _tmp6=n;unsigned _tmp7;void*_tmp8;switch((_tmp6.NParamNumelts).tag){case 1:
 return({const char*_tmp9="0";_tag_fat(_tmp9,sizeof(char),2U);});case 2: _tmp8=(_tmp6.NVar).val;{struct Cyc_Absyn_Vardecl*x=_tmp8;
return Cyc_Absynpp_qvar2string(x->name);}case 4: _tmp8=(_tmp6.NType).val;{void*x=_tmp8;
return Cyc_Absynpp_typ2string(x);}case 3: _tmp8=(_tmp6.NNumelts).val;{struct Cyc_Absyn_Vardecl*x=_tmp8;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp47;_tmp47.tag=0,({struct _fat_ptr _tmp4D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(x->name));_tmp47.f1=_tmp4D;});_tmp47;});void*_tmpA[1];_tmpA[0]=& _tmpC;({struct _fat_ptr _tmp4E=({const char*_tmpB="numelts(%s)";_tag_fat(_tmpB,sizeof(char),12U);});Cyc_aprintf(_tmp4E,_tag_fat(_tmpA,sizeof(void*),1));});});}case 5: _tmp7=(_tmp6.NParam).val;{unsigned i=_tmp7;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmp48;_tmp48.tag=1,_tmp48.f1=(unsigned long)((int)i);_tmp48;});void*_tmpD[1];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp4F=({const char*_tmpE="param(%d)";_tag_fat(_tmpE,sizeof(char),10U);});Cyc_aprintf(_tmp4F,_tag_fat(_tmpD,sizeof(void*),1));});});}case 6: _tmp7=(_tmp6.NParamNumelts).val;{unsigned i=_tmp7;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp49;_tmp49.tag=1,_tmp49.f1=(unsigned long)((int)i);_tmp49;});void*_tmp10[1];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp50=({const char*_tmp11="numelts(param(%d))";_tag_fat(_tmp11,sizeof(char),19U);});Cyc_aprintf(_tmp50,_tag_fat(_tmp10,sizeof(void*),1));});});}default:
 return(struct _fat_ptr)({void*_tmp13=0U;({struct _fat_ptr _tmp51=({const char*_tmp14="return_value";_tag_fat(_tmp14,sizeof(char),13U);});Cyc_aprintf(_tmp51,_tag_fat(_tmp13,sizeof(void*),0));});});};}struct Cyc_Pratt_Edge{union Cyc_Pratt_Node x;union Cyc_Pratt_Node y;struct Cyc_AP_T*value;};struct Cyc_Pratt_Graph{struct Cyc_List_List*nodes;struct Cyc_List_List*edges;};
# 149
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph (void){
struct Cyc_List_List*_tmp15=({struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=& Cyc_Pratt_zero_node,_tmp17->tl=0;_tmp17;});struct Cyc_List_List*nodes=_tmp15;
return({struct Cyc_Pratt_Graph*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->nodes=nodes,_tmp16->edges=0;_tmp16;});}
# 155
static struct Cyc_List_List*Cyc_Pratt_copy_edges(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp18=0;struct Cyc_List_List*res=_tmp18;
for(1;ds != 0;ds=ds->tl){
struct Cyc_Pratt_Edge*_tmp19=(struct Cyc_Pratt_Edge*)ds->hd;struct Cyc_Pratt_Edge*e=_tmp19;
res=({struct Cyc_List_List*_tmp1B=_cycalloc(sizeof(*_tmp1B));({struct Cyc_Pratt_Edge*_tmp52=({struct Cyc_Pratt_Edge*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->x=e->x,_tmp1A->y=e->y,_tmp1A->value=e->value;_tmp1A;});_tmp1B->hd=_tmp52;}),_tmp1B->tl=res;_tmp1B;});}
# 161
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res);}
# 164
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*G){
struct Cyc_Pratt_Graph*_tmp1C=G;void*_tmp1E;void*_tmp1D;_tmp1D=_tmp1C->nodes;_tmp1E=_tmp1C->edges;{struct Cyc_List_List*nodes=_tmp1D;struct Cyc_List_List*edges=_tmp1E;
return({struct Cyc_Pratt_Graph*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->nodes=nodes,({struct Cyc_List_List*_tmp53=Cyc_Pratt_copy_edges(edges);_tmp1F->edges=_tmp53;});_tmp1F;});}}
# 171
static struct Cyc_AP_T**Cyc_Pratt_lookup_distance(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node x,union Cyc_Pratt_Node y){
# 173
if(Cyc_Pratt_cmp_node(x,y)== 0)return& Cyc_AP_zero;
{struct Cyc_List_List*_tmp20=G->edges;struct Cyc_List_List*ds=_tmp20;for(0;ds != 0;ds=ds->tl){
struct Cyc_Pratt_Edge*_tmp21=(struct Cyc_Pratt_Edge*)ds->hd;struct Cyc_Pratt_Edge*e=_tmp21;
if(Cyc_Pratt_cmp_node(e->x,x)== 0 && Cyc_Pratt_cmp_node(e->y,y)== 0)
return& e->value;}}
# 179
return 0;}
# 185
static void Cyc_Pratt_set_distance(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node x,union Cyc_Pratt_Node y,struct Cyc_AP_T*d){
struct Cyc_AP_T**r=Cyc_Pratt_lookup_distance(G,x,y);
if(r == 0)
({struct Cyc_List_List*_tmp55=({struct Cyc_List_List*_tmp23=_cycalloc(sizeof(*_tmp23));({struct Cyc_Pratt_Edge*_tmp54=({struct Cyc_Pratt_Edge*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->x=x,_tmp22->y=y,_tmp22->value=d;_tmp22;});_tmp23->hd=_tmp54;}),_tmp23->tl=G->edges;_tmp23;});G->edges=_tmp55;});else{
# 190
*r=d;}}
# 194
void Cyc_Pratt_print_graph(struct Cyc_Pratt_Graph*G){
if(G == 0){
({void*_tmp24=0U;({struct Cyc___cycFILE*_tmp57=Cyc_stderr;struct _fat_ptr _tmp56=({const char*_tmp25="<inconsistent>";_tag_fat(_tmp25,sizeof(char),15U);});Cyc_fprintf(_tmp57,_tmp56,_tag_fat(_tmp24,sizeof(void*),0));});});
return;}{
# 199
struct Cyc_Pratt_Graph _tmp26=*G;struct Cyc_Pratt_Graph _stmttmp1=_tmp26;struct Cyc_Pratt_Graph _tmp27=_stmttmp1;void*_tmp29;void*_tmp28;_tmp28=_tmp27.nodes;_tmp29=_tmp27.edges;{struct Cyc_List_List*nodes=_tmp28;struct Cyc_List_List*ds=_tmp29;
for(1;ds != 0;ds=ds->tl){
struct Cyc_Pratt_Edge*_tmp2A=(struct Cyc_Pratt_Edge*)ds->hd;struct Cyc_Pratt_Edge*e=_tmp2A;
({struct Cyc_String_pa_PrintArg_struct _tmp2D=({struct Cyc_String_pa_PrintArg_struct _tmp4C;_tmp4C.tag=0,({
struct _fat_ptr _tmp58=(struct _fat_ptr)((struct _fat_ptr)Cyc_Pratt_node2string(e->x));_tmp4C.f1=_tmp58;});_tmp4C;});struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp4B;_tmp4B.tag=0,({struct _fat_ptr _tmp59=(struct _fat_ptr)((struct _fat_ptr)Cyc_Pratt_node2string(e->y));_tmp4B.f1=_tmp59;});_tmp4B;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp4A;_tmp4A.tag=0,({struct _fat_ptr _tmp5A=(struct _fat_ptr)({char*_tmp30=Cyc_AP_tostr(e->value,10);_tag_fat(_tmp30,sizeof(char),_get_zero_arr_size_char((void*)_tmp30,1U));});_tmp4A.f1=_tmp5A;});_tmp4A;});void*_tmp2B[3];_tmp2B[0]=& _tmp2D,_tmp2B[1]=& _tmp2E,_tmp2B[2]=& _tmp2F;({struct Cyc___cycFILE*_tmp5C=Cyc_stderr;struct _fat_ptr _tmp5B=({const char*_tmp2C="%s - %s <= %s";_tag_fat(_tmp2C,sizeof(char),14U);});Cyc_fprintf(_tmp5C,_tmp5B,_tag_fat(_tmp2B,sizeof(void*),3));});});
if(ds->tl != 0)({void*_tmp31=0U;({struct Cyc___cycFILE*_tmp5E=Cyc_stderr;struct _fat_ptr _tmp5D=({const char*_tmp32=", ";_tag_fat(_tmp32,sizeof(char),3U);});Cyc_fprintf(_tmp5E,_tmp5D,_tag_fat(_tmp31,sizeof(void*),0));});});}}}}
# 209
static void Cyc_Pratt_add_node(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node x){
{struct Cyc_List_List*_tmp33=G->nodes;struct Cyc_List_List*vs=_tmp33;for(0;vs != 0;vs=vs->tl){
union Cyc_Pratt_Node y=*((union Cyc_Pratt_Node*)vs->hd);
if(Cyc_Pratt_cmp_node(x,y)== 0)return;}}
# 215
({struct Cyc_List_List*_tmp60=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));({union Cyc_Pratt_Node*_tmp5F=({union Cyc_Pratt_Node*_tmp34=_cycalloc(sizeof(*_tmp34));*_tmp34=x;_tmp34;});_tmp35->hd=_tmp5F;}),_tmp35->tl=G->nodes;_tmp35;});G->nodes=_tmp60;});}
# 221
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,struct Cyc_AP_T*a){
# 228
Cyc_Pratt_add_node(G,i);
Cyc_Pratt_add_node(G,j);{
# 231
struct Cyc_AP_T**dist=Cyc_Pratt_lookup_distance(G,i,j);
# 233
if(dist != 0 && Cyc_AP_cmp(*dist,a)< 1)
# 237
return G;
# 240
dist=Cyc_Pratt_lookup_distance(G,j,i);
if(dist != 0 &&({struct Cyc_AP_T*_tmp61=Cyc_AP_add(*dist,a);Cyc_AP_cmp(_tmp61,Cyc_AP_zero);})< 0)
# 245
return 0;
# 248
{struct Cyc_List_List*_tmp36=G->nodes;struct Cyc_List_List*ks=_tmp36;for(0;ks != 0;ks=ks->tl){
union Cyc_Pratt_Node _tmp37=*((union Cyc_Pratt_Node*)ks->hd);union Cyc_Pratt_Node k=_tmp37;
struct Cyc_AP_T**_tmp38=Cyc_Pratt_lookup_distance(G,k,i);struct Cyc_AP_T**dist_ki=_tmp38;
if(dist_ki == 0)continue;{
struct Cyc_List_List*_tmp39=G->nodes;struct Cyc_List_List*ls=_tmp39;for(0;ls != 0;ls=ls->tl){
union Cyc_Pratt_Node _tmp3A=*((union Cyc_Pratt_Node*)ls->hd);union Cyc_Pratt_Node l=_tmp3A;
struct Cyc_AP_T**_tmp3B=Cyc_Pratt_lookup_distance(G,j,l);struct Cyc_AP_T**dist_jl=_tmp3B;
if(dist_jl != 0){
struct Cyc_AP_T**_tmp3C=Cyc_Pratt_lookup_distance(G,k,l);struct Cyc_AP_T**dist_kl=_tmp3C;
# 258
struct Cyc_AP_T*_tmp3D=({struct Cyc_AP_T*_tmp62=*dist_ki;Cyc_AP_add(_tmp62,Cyc_AP_add(a,*dist_jl));});struct Cyc_AP_T*d=_tmp3D;
# 262
if(dist_kl == 0)
# 266
({struct Cyc_List_List*_tmp64=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));({struct Cyc_Pratt_Edge*_tmp63=({struct Cyc_Pratt_Edge*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->x=k,_tmp3E->y=l,_tmp3E->value=d;_tmp3E;});_tmp3F->hd=_tmp63;}),_tmp3F->tl=G->edges;_tmp3F;});G->edges=_tmp64;});else{
if(Cyc_AP_cmp(*dist_kl,d)> 0)
# 271
*dist_kl=d;}}}}}}
# 276
return G;}}

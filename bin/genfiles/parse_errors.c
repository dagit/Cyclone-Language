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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 319 "cycboot.h"
extern void exit(int);extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};extern char Cyc_Absyn_EmptyAnnot[11U];
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 78 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 126
extern int Cyc_Flags_print_parser_state_and_token;extern char Cyc_Parse_Exit[5U];
# 27 "parse.h"
struct _fat_ptr Cyc_token2string(int);
# 54
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 92 "parse_tab.h"
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_ParseErrors_ParseState{int state;int token;struct _fat_ptr msg;};static char _tmp0[34U]="undeclared type or missing comma ";static char _tmp1[28U]="type has not been declared ";static char _tmp2[13U]="missing ';' ";static char _tmp3[31U]="missing ')' on parameter list ";static char _tmp4[29U]="expecting IDENTIFIER or '{' ";static char _tmp5[29U]="expecting IDENTIFIER or '{' ";static char _tmp6[19U]="expecting PATTERN ";static char _tmp7[22U]="expecting IDENTIFIER ";static char _tmp8[22U]="expecting IDENTIFIER ";static char _tmp9[30U]="expecting '(' EXPRESSION ')' ";static char _tmpA[19U]="expecting 'union' ";static char _tmpB[22U]="expecting 'datatype' ";static char _tmpC[36U]="expecting '((' ATTRIBUTE_LIST '))' ";static char _tmpD[34U]="expecting '(' PARAMETER_LIST ')' ";static char _tmpE[16U]="expecting KIND ";static char _tmpF[16U]="expecting KIND ";static char _tmp10[22U]="expecting ';' or '{' ";static char _tmp11[22U]="expecting ';' or '{' ";static char _tmp12[29U]="expecting '<' or IDENTIFIER ";static char _tmp13[14U]="missing '};' ";static char _tmp14[22U]="expecting IDENTIFIER ";static char _tmp15[15U]="expecting '(' ";static char _tmp16[22U]="expecting IDENTIFIER ";static char _tmp17[19U]="expecting PATTERN ";static char _tmp18[35U]="expecting IDENTIFIER or extra ',' ";static char _tmp19[22U]="expecting EXPRESSION ";static char _tmp1A[13U]="missing '}' ";static char _tmp1B[13U]="missing '}' ";static char _tmp1C[13U]="missing ';' ";static char _tmp1D[22U]="expecting EXPRESSION ";static char _tmp1E[23U]="expecting DECLARATION ";static char _tmp1F[13U]="missing '}' ";static char _tmp20[29U]="expecting ']' or EXPRESSION ";static char _tmp21[22U]="expecting EXPRESSION ";static char _tmp22[16U]="expecting TYPE ";static char _tmp23[13U]="missing ')' ";static char _tmp24[22U]="expecting EXPRESSION ";static char _tmp25[13U]="missing ')' ";static char _tmp26[16U]="expecting TYPE ";static char _tmp27[22U]="expecting EXPRESSION ";static char _tmp28[13U]="missing '}' ";static char _tmp29[32U]="expecting TYPE_VARIABLE or '_' ";static char _tmp2A[45U]="expecting '(' EXPRESSION ')' or '( TYPE ')' ";static char _tmp2B[13U]="missing ')' ";static char _tmp2C[22U]="expecting EXPRESSION ";static char _tmp2D[37U]="expecting INITIALIZER or EXPRESSION ";static char _tmp2E[29U]="expecting '(' EXPRESSION ') ";static char _tmp2F[25U]="expecting EXPRESSION ') ";static char _tmp30[12U]="missing ') ";static char _tmp31[44U]="expecting '(' EXPRESSION ',' EXPRESSION ') ";static char _tmp32[40U]="expecting EXPRESSION ',' EXPRESSION ') ";static char _tmp33[29U]="expecting ',' EXPRESSION ') ";static char _tmp34[13U]="missing ')' ";static char _tmp35[15U]="expecting '(' ";static char _tmp36[22U]="expecting EXPRESSION ";static char _tmp37[15U]="expecting ',' ";static char _tmp38[30U]="expecting 'sizeof(' TYPE ')' ";static char _tmp39[16U]="expecting TYPE ";static char _tmp3A[13U]="missing ')' ";static char _tmp3B[15U]="expecting '(' ";static char _tmp3C[22U]="expecting EXPRESSION ";static char _tmp3D[15U]="expecting ',' ";static char _tmp3E[22U]="expecting EXPRESSION ";static char _tmp3F[15U]="expecting ',' ";static char _tmp40[20U]="expecting 'sizeof' ";static char _tmp41[15U]="expecting '(' ";static char _tmp42[16U]="expecting TYPE ";static char _tmp43[13U]="missing ')' ";static char _tmp44[37U]="expecting '(' REGION_EXPRESSION ')' ";static char _tmp45[13U]="missing ')' ";static char _tmp46[37U]="expecting INITIALIZER or EXPRESSION ";static char _tmp47[30U]="expecting '(' EXPRESSION ')' ";static char _tmp48[26U]="expecting EXPRESSION ')' ";static char _tmp49[13U]="missing ')' ";static char _tmp4A[24U]="expecting '(' TYPE ')' ";static char _tmp4B[20U]="expecting TYPE ')' ";static char _tmp4C[13U]="missing ')' ";static char _tmp4D[15U]="expecting '(' ";static char _tmp4E[22U]="expecting EXPRESSION ";static char _tmp4F[41U]="expecting '.' IDENTIFIER or missing ')' ";static char _tmp50[22U]="expecting IDENTIFIER ";static char _tmp51[46U]="expecting TYPE_LIST, EXPRESSION_LIST, or ')' ";static char _tmp52[15U]="expecting ')' ";static char _tmp53[24U]="missing ':' EXPRESSION ";static char _tmp54[33U]="missing EXPRESSION or extra '?' ";static char _tmp55[33U]="missing EXPRESSION or extra '+' ";static char _tmp56[33U]="missing EXPRESSION or extra '-' ";static char _tmp57[33U]="missing EXPRESSION or extra '*' ";static char _tmp58[33U]="missing EXPRESSION or extra '/' ";static char _tmp59[33U]="missing EXPRESSION or extra '|' ";static char _tmp5A[33U]="missing EXPRESSION or extra '&' ";static char _tmp5B[33U]="missing EXPRESSION or extra '^' ";static char _tmp5C[33U]="missing EXPRESSION or extra '&' ";static char _tmp5D[34U]="missing EXPRESSION or extra '==' ";static char _tmp5E[34U]="missing EXPRESSION or extra '!=' ";static char _tmp5F[33U]="missing EXPRESSION or extra '<' ";static char _tmp60[34U]="missing EXPRESSION or extra '<=' ";static char _tmp61[33U]="missing EXPRESSION or extra '>' ";static char _tmp62[34U]="missing EXPRESSION or extra '>=' ";static char _tmp63[34U]="missing EXPRESSION or extra '>>' ";static char _tmp64[34U]="missing EXPRESSION or extra '<<' ";static char _tmp65[33U]="missing EXPRESSION or extra '%' ";static char _tmp66[22U]="expecting EXPRESSION ";static char _tmp67[22U]="expecting EXPRESSION ";static char _tmp68[22U]="expecting EXPRESSION ";static char _tmp69[31U]="extra ',' or missing argument ";static char _tmp6A[13U]="missing ')' ";static char _tmp6B[22U]="expecting IDENTIFIER ";static char _tmp6C[22U]="expecting IDENTIFIER ";static char _tmp6D[48U]="extra space not allowed in empty instantiation ";static char _tmp6E[37U]="expecting '<>' or '<' TYPE_LIST '>' ";
# 11 "parse_errors.cyc"
static struct Cyc_ParseErrors_ParseState Cyc_ParseErrors_msg_table[111U]={{130,363,{_tmp0,_tmp0,_tmp0 + 34U}},{69,363,{_tmp1,_tmp1,_tmp1 + 28U}},{489,- 1,{_tmp2,_tmp2,_tmp2 + 13U}},{317,- 1,{_tmp3,_tmp3,_tmp3 + 31U}},{66,- 1,{_tmp4,_tmp4,_tmp4 + 29U}},{22,- 1,{_tmp5,_tmp5,_tmp5 + 29U}},{26,- 1,{_tmp6,_tmp6,_tmp6 + 19U}},{28,- 1,{_tmp7,_tmp7,_tmp7 + 22U}},{29,- 1,{_tmp8,_tmp8,_tmp8 + 22U}},{36,- 1,{_tmp9,_tmp9,_tmp9 + 30U}},{37,- 1,{_tmpA,_tmpA,_tmpA + 19U}},{38,- 1,{_tmpB,_tmpB,_tmpB + 22U}},{44,- 1,{_tmpC,_tmpC,_tmpC + 36U}},{47,- 1,{_tmpD,_tmpD,_tmpD + 34U}},{114,- 1,{_tmpE,_tmpE,_tmpE + 16U}},{118,- 1,{_tmpF,_tmpF,_tmpF + 16U}},{55,- 1,{_tmp10,_tmp10,_tmp10 + 22U}},{56,- 1,{_tmp11,_tmp11,_tmp11 + 22U}},{33,- 1,{_tmp12,_tmp12,_tmp12 + 29U}},{859,- 1,{_tmp13,_tmp13,_tmp13 + 14U}},{68,- 1,{_tmp14,_tmp14,_tmp14 + 22U}},{95,- 1,{_tmp15,_tmp15,_tmp15 + 15U}},{96,- 1,{_tmp16,_tmp16,_tmp16 + 22U}},{97,- 1,{_tmp17,_tmp17,_tmp17 + 19U}},{230,- 1,{_tmp18,_tmp18,_tmp18 + 35U}},{120,- 1,{_tmp19,_tmp19,_tmp19 + 22U}},{126,- 1,{_tmp1A,_tmp1A,_tmp1A + 13U}},{266,- 1,{_tmp1B,_tmp1B,_tmp1B + 13U}},{130,- 1,{_tmp1C,_tmp1C,_tmp1C + 13U}},{269,- 1,{_tmp1D,_tmp1D,_tmp1D + 22U}},{268,- 1,{_tmp1E,_tmp1E,_tmp1E + 23U}},{143,0,{_tmp1F,_tmp1F,_tmp1F + 13U}},{150,- 1,{_tmp20,_tmp20,_tmp20 + 29U}},{445,- 1,{_tmp21,_tmp21,_tmp21 + 22U}},{324,- 1,{_tmp22,_tmp22,_tmp22 + 16U}},{542,- 1,{_tmp23,_tmp23,_tmp23 + 13U}},{323,- 1,{_tmp24,_tmp24,_tmp24 + 22U}},{541,- 1,{_tmp25,_tmp25,_tmp25 + 13U}},{164,- 1,{_tmp26,_tmp26,_tmp26 + 16U}},{332,- 1,{_tmp27,_tmp27,_tmp27 + 22U}},{170,- 1,{_tmp28,_tmp28,_tmp28 + 13U}},{222,- 1,{_tmp29,_tmp29,_tmp29 + 32U}},{171,- 1,{_tmp2A,_tmp2A,_tmp2A + 45U}},{358,- 1,{_tmp2B,_tmp2B,_tmp2B + 13U}},{174,- 1,{_tmp2C,_tmp2C,_tmp2C + 22U}},{175,- 1,{_tmp2D,_tmp2D,_tmp2D + 37U}},{176,- 1,{_tmp2E,_tmp2E,_tmp2E + 29U}},{344,- 1,{_tmp2F,_tmp2F,_tmp2F + 25U}},{559,- 1,{_tmp30,_tmp30,_tmp30 + 12U}},{177,- 1,{_tmp31,_tmp31,_tmp31 + 44U}},{345,- 1,{_tmp32,_tmp32,_tmp32 + 40U}},{560,- 1,{_tmp33,_tmp33,_tmp33 + 29U}},{717,- 1,{_tmp34,_tmp34,_tmp34 + 13U}},{179,- 1,{_tmp35,_tmp35,_tmp35 + 15U}},{347,- 1,{_tmp36,_tmp36,_tmp36 + 22U}},{562,- 1,{_tmp37,_tmp37,_tmp37 + 15U}},{719,- 1,{_tmp38,_tmp38,_tmp38 + 30U}},{915,- 1,{_tmp39,_tmp39,_tmp39 + 16U}},{1045,- 1,{_tmp3A,_tmp3A,_tmp3A + 13U}},{180,- 1,{_tmp3B,_tmp3B,_tmp3B + 15U}},{348,- 1,{_tmp3C,_tmp3C,_tmp3C + 22U}},{563,- 1,{_tmp3D,_tmp3D,_tmp3D + 15U}},{720,- 1,{_tmp3E,_tmp3E,_tmp3E + 22U}},{821,- 1,{_tmp3F,_tmp3F,_tmp3F + 15U}},{916,- 1,{_tmp40,_tmp40,_tmp40 + 20U}},{989,- 1,{_tmp41,_tmp41,_tmp41 + 15U}},{1046,- 1,{_tmp42,_tmp42,_tmp42 + 16U}},{1065,- 1,{_tmp43,_tmp43,_tmp43 + 13U}},{181,- 1,{_tmp44,_tmp44,_tmp44 + 37U}},{564,- 1,{_tmp45,_tmp45,_tmp45 + 13U}},{721,- 1,{_tmp46,_tmp46,_tmp46 + 37U}},{183,- 1,{_tmp47,_tmp47,_tmp47 + 30U}},{351,- 1,{_tmp48,_tmp48,_tmp48 + 26U}},{566,- 1,{_tmp49,_tmp49,_tmp49 + 13U}},{184,- 1,{_tmp4A,_tmp4A,_tmp4A + 24U}},{352,- 1,{_tmp4B,_tmp4B,_tmp4B + 20U}},{567,- 1,{_tmp4C,_tmp4C,_tmp4C + 13U}},{185,- 1,{_tmp4D,_tmp4D,_tmp4D + 15U}},{353,- 1,{_tmp4E,_tmp4E,_tmp4E + 22U}},{568,- 1,{_tmp4F,_tmp4F,_tmp4F + 41U}},{726,- 1,{_tmp50,_tmp50,_tmp50 + 22U}},{514,- 1,{_tmp51,_tmp51,_tmp51 + 46U}},{573,- 1,{_tmp52,_tmp52,_tmp52 + 15U}},{577,- 1,{_tmp53,_tmp53,_tmp53 + 24U}},{366,- 1,{_tmp54,_tmp54,_tmp54 + 33U}},{379,- 1,{_tmp55,_tmp55,_tmp55 + 33U}},{380,- 1,{_tmp56,_tmp56,_tmp56 + 33U}},{381,- 1,{_tmp57,_tmp57,_tmp57 + 33U}},{382,- 1,{_tmp58,_tmp58,_tmp58 + 33U}},{368,- 1,{_tmp59,_tmp59,_tmp59 + 33U}},{370,- 1,{_tmp5A,_tmp5A,_tmp5A + 33U}},{369,- 1,{_tmp5B,_tmp5B,_tmp5B + 33U}},{195,- 1,{_tmp5C,_tmp5C,_tmp5C + 33U}},{371,- 1,{_tmp5D,_tmp5D,_tmp5D + 34U}},{372,- 1,{_tmp5E,_tmp5E,_tmp5E + 34U}},{375,- 1,{_tmp5F,_tmp5F,_tmp5F + 33U}},{373,- 1,{_tmp60,_tmp60,_tmp60 + 34U}},{376,- 1,{_tmp61,_tmp61,_tmp61 + 33U}},{374,- 1,{_tmp62,_tmp62,_tmp62 + 34U}},{378,- 1,{_tmp63,_tmp63,_tmp63 + 34U}},{377,- 1,{_tmp64,_tmp64,_tmp64 + 34U}},{383,- 1,{_tmp65,_tmp65,_tmp65 + 33U}},{396,- 1,{_tmp66,_tmp66,_tmp66 + 22U}},{186,- 1,{_tmp67,_tmp67,_tmp67 + 22U}},{403,- 1,{_tmp68,_tmp68,_tmp68 + 22U}},{731,41,{_tmp69,_tmp69,_tmp69 + 31U}},{599,59,{_tmp6A,_tmp6A,_tmp6A + 13U}},{402,- 1,{_tmp6B,_tmp6B,_tmp6B + 22U}},{398,- 1,{_tmp6C,_tmp6C,_tmp6C + 22U}},{602,62,{_tmp6D,_tmp6D,_tmp6D + 48U}},{405,- 1,{_tmp6E,_tmp6E,_tmp6E + 37U}}};
# 19 "parse_errors.cyc"
void Cyc_yyerror(struct _fat_ptr s,int state,int token){
if(Cyc_Flags_print_parser_state_and_token){
({struct Cyc_Int_pa_PrintArg_struct _tmp71=({struct Cyc_Int_pa_PrintArg_struct _tmp7C;_tmp7C.tag=1,_tmp7C.f1=(unsigned long)state;_tmp7C;});struct Cyc_Int_pa_PrintArg_struct _tmp72=({struct Cyc_Int_pa_PrintArg_struct _tmp7B;_tmp7B.tag=1,_tmp7B.f1=(unsigned long)token;_tmp7B;});void*_tmp6F[2];_tmp6F[0]=& _tmp71,_tmp6F[1]=& _tmp72;({struct _fat_ptr _tmp80=({const char*_tmp70="parse error: state [%d], token [%d]\n";_tag_fat(_tmp70,sizeof(char),37U);});Cyc_printf(_tmp80,_tag_fat(_tmp6F,sizeof(void*),2));});});
exit(1);}
# 27
{unsigned i=0U;for(0;i < 111U;++ i){
if((Cyc_ParseErrors_msg_table[(int)i]).state == state &&(Cyc_ParseErrors_msg_table[(int)i]).token == token){
# 30
s=(Cyc_ParseErrors_msg_table[(int)i]).msg;
break;}else{
if((Cyc_ParseErrors_msg_table[(int)i]).state == state)
s=(Cyc_ParseErrors_msg_table[(int)i]).msg;}}}{
# 35
struct _fat_ptr _tmp73=Cyc_token2string(token);struct _fat_ptr ts=_tmp73;
if((char*)ts.curr != (char*)(_tag_fat(0,0,0)).curr)
({struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp7E;_tmp7E.tag=0,_tmp7E.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp7E;});struct Cyc_String_pa_PrintArg_struct _tmp77=({struct Cyc_String_pa_PrintArg_struct _tmp7D;_tmp7D.tag=0,({struct _fat_ptr _tmp81=(struct _fat_ptr)((struct _fat_ptr)Cyc_token2string(token));_tmp7D.f1=_tmp81;});_tmp7D;});void*_tmp74[2];_tmp74[0]=& _tmp76,_tmp74[1]=& _tmp77;({unsigned _tmp83=(unsigned)Cyc_yylloc.first_line;struct _fat_ptr _tmp82=({const char*_tmp75="%s (found %s instead)";_tag_fat(_tmp75,sizeof(char),22U);});Cyc_Warn_err(_tmp83,_tmp82,_tag_fat(_tmp74,sizeof(void*),2));});});else{
# 39
({struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp7F;_tmp7F.tag=0,_tmp7F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp7F;});void*_tmp78[1];_tmp78[0]=& _tmp7A;({unsigned _tmp85=(unsigned)Cyc_yylloc.first_line;struct _fat_ptr _tmp84=({const char*_tmp79="%s ";_tag_fat(_tmp79,sizeof(char),4U);});Cyc_Warn_err(_tmp85,_tmp84,_tag_fat(_tmp78,sizeof(void*),1));});});}}}

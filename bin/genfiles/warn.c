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
 extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);extern char Cyc_List_List_mismatch[14U];
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 37 "position.h"
extern struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 42
extern struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned,struct _fat_ptr);
# 48
extern void Cyc_Position_post_error(struct Cyc_Position_Error*);
extern int Cyc_Position_error_p (void);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 232 "cycboot.h"
extern struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 62 "string.h"
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 27 "warn.h"
void Cyc_Warn_vwarn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 38
void*Cyc_Warn_vimpos(struct _fat_ptr,struct _fat_ptr);
# 43
void*Cyc_Warn_vimpos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
# 68
void Cyc_Warn_vwarn2(unsigned,struct _fat_ptr);
# 70
void*Cyc_Warn_vimpos2(struct _fat_ptr);
# 72
void*Cyc_Warn_vimpos_loc2(unsigned,struct _fat_ptr);
# 30 "warn.cyc"
static struct Cyc_List_List*Cyc_Warn_warning_segs=0;
static struct Cyc_List_List*Cyc_Warn_warning_msgs=0;
# 51
void Cyc_Warn_vwarn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
struct _fat_ptr msg=(struct _fat_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
Cyc_Warn_warning_segs=({struct Cyc_List_List*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->hd=(void*)loc,_tmp0->tl=Cyc_Warn_warning_segs;_tmp0;});
Cyc_Warn_warning_msgs=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({struct _fat_ptr*_tmp40=({struct _fat_ptr*_tmp1=_cycalloc(sizeof(*_tmp1));*_tmp1=msg;_tmp1;});_tmp2->hd=_tmp40;}),_tmp2->tl=Cyc_Warn_warning_msgs;_tmp2;});}
# 57
void Cyc_Warn_warn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 59
Cyc_Warn_vwarn(loc,fmt,ap);}
# 61
void Cyc_Warn_flush_warnings (void){
if(Cyc_Warn_warning_segs == 0)
return;
({void*_tmp3=0U;({struct Cyc___cycFILE*_tmp42=Cyc_stderr;struct _fat_ptr _tmp41=({const char*_tmp4="***Warnings***\n";_tag_fat(_tmp4,sizeof(char),16U);});Cyc_fprintf(_tmp42,_tmp41,_tag_fat(_tmp3,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp5=Cyc_Position_strings_of_segments(Cyc_Warn_warning_segs);struct Cyc_List_List*seg_strs=_tmp5;
Cyc_Warn_warning_segs=0;
Cyc_Warn_warning_msgs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(Cyc_Warn_warning_msgs);
while(Cyc_Warn_warning_msgs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0,_tmp3B.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(seg_strs))->hd));_tmp3B;});struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp3A;_tmp3A.tag=0,_tmp3A.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Warn_warning_msgs))->hd));_tmp3A;});void*_tmp6[2];_tmp6[0]=& _tmp8,_tmp6[1]=& _tmp9;({struct Cyc___cycFILE*_tmp44=Cyc_stderr;struct _fat_ptr _tmp43=({const char*_tmp7="%s: %s\n";_tag_fat(_tmp7,sizeof(char),8U);});Cyc_fprintf(_tmp44,_tmp43,_tag_fat(_tmp6,sizeof(void*),2));});});
seg_strs=seg_strs->tl;
Cyc_Warn_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Warn_warning_msgs))->tl;}
# 73
({void*_tmpA=0U;({struct Cyc___cycFILE*_tmp46=Cyc_stderr;struct _fat_ptr _tmp45=({const char*_tmpB="**************\n";_tag_fat(_tmpB,sizeof(char),16U);});Cyc_fprintf(_tmp46,_tmp45,_tag_fat(_tmpA,sizeof(void*),0));});});
Cyc_fflush(Cyc_stderr);}}
# 77
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
Cyc_Position_post_error(({unsigned _tmp47=loc;Cyc_Position_mk_err(_tmp47,(struct _fat_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 81
void Cyc_Warn_err(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 83
Cyc_Warn_verr(loc,fmt,ap);}
# 86
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap){
struct _fat_ptr msg=(struct _fat_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
if(!Cyc_Position_error_p()){
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp3C;_tmp3C.tag=0,_tmp3C.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp3C;});void*_tmpC[1];_tmpC[0]=& _tmpE;({struct Cyc___cycFILE*_tmp49=Cyc_stderr;struct _fat_ptr _tmp48=({const char*_tmpD="Compiler Error: %s\n";_tag_fat(_tmpD,sizeof(char),20U);});Cyc_fprintf(_tmp49,_tmp48,_tag_fat(_tmpC,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);}
# 92
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->tag=Cyc_Core_Impossible,_tmpF->f1=msg;_tmpF;}));}
# 95
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 97
Cyc_Warn_vimpos(fmt,ap);}
# 100
void*Cyc_Warn_vimpos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
if(!Cyc_Position_error_p()){
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmp4B=Cyc_stderr;struct _fat_ptr _tmp4A=({const char*_tmp11="Compiler Error: \n";_tag_fat(_tmp11,sizeof(char),18U);});Cyc_fprintf(_tmp4B,_tmp4A,_tag_fat(_tmp10,sizeof(void*),0));});});
Cyc_Warn_verr(loc,fmt,ap);}
# 105
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp4C=({const char*_tmp12="Compiler Error";_tag_fat(_tmp12,sizeof(char),15U);});_tmp13->f1=_tmp4C;});_tmp13;}));}
# 108
void*Cyc_Warn_impos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 110
Cyc_Warn_vimpos_loc(loc,fmt,ap);}
# 113
static struct _fat_ptr Cyc_Warn_args2string(struct _fat_ptr args){
struct Cyc_List_List*lst=0;
int curr_line_len=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(args,sizeof(void*));++ i){
struct _fat_ptr s;
{void*_tmp14=((void**)args.curr)[i];void*_stmttmp0=_tmp14;void*_tmp15=_stmttmp0;int _tmp16;void*_tmp17;struct _fat_ptr _tmp18;switch(*((int*)_tmp15)){case 0: _tmp18=((struct Cyc_Warn_String_Warn_Warg_struct*)_tmp15)->f1;{struct _fat_ptr s2=_tmp18;
s=s2;goto _LL0;}case 4: _tmp17=((struct Cyc_Warn_Exp_Warn_Warg_struct*)_tmp15)->f1;{struct Cyc_Absyn_Exp*e=_tmp17;
s=Cyc_Absynpp_exp2string(e);goto _LL0;}case 3: _tmp17=(void*)((struct Cyc_Warn_TypOpt_Warn_Warg_struct*)_tmp15)->f1;{void*t=_tmp17;
if((unsigned)t){_tmp17=t;goto _LL8;}else{s=({const char*_tmp19="<?>";_tag_fat(_tmp19,sizeof(char),4U);});}goto _LL0;}case 2: _tmp17=(void*)((struct Cyc_Warn_Typ_Warn_Warg_struct*)_tmp15)->f1;_LL8: {void*t=_tmp17;
s=Cyc_Absynpp_typ2string(t);goto _LL0;}case 1: _tmp17=((struct Cyc_Warn_Qvar_Warn_Warg_struct*)_tmp15)->f1;{struct _tuple0*qv=_tmp17;
s=Cyc_Absynpp_qvar2string(qv);goto _LL0;}case 5: _tmp17=((struct Cyc_Warn_Stmt_Warn_Warg_struct*)_tmp15)->f1;{struct Cyc_Absyn_Stmt*s2=_tmp17;
s=Cyc_Absynpp_stmt2string(s2);goto _LL0;}case 6: _tmp17=((struct Cyc_Warn_Aggrdecl_Warn_Warg_struct*)_tmp15)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp17;
s=(struct _fat_ptr)({struct _fat_ptr _tmp4D=(int)ad->kind == 1?({const char*_tmp1A="union ";_tag_fat(_tmp1A,sizeof(char),7U);}):({const char*_tmp1B="struct ";_tag_fat(_tmp1B,sizeof(char),8U);});Cyc_strconcat(_tmp4D,(struct _fat_ptr)
Cyc_Absynpp_qvar2string(ad->name));});
goto _LL0;}case 7: _tmp17=((struct Cyc_Warn_Tvar_Warn_Warg_struct*)_tmp15)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp17;
s=*tv->name;goto _LL0;}case 9: _tmp17=((struct Cyc_Warn_Kind_Warn_Warg_struct*)_tmp15)->f1;{struct Cyc_Absyn_Kind*k=_tmp17;
s=Cyc_Absynpp_kind2string(k);goto _LL0;}case 8: _tmp17=(void*)((struct Cyc_Warn_KindBound_Warn_Warg_struct*)_tmp15)->f1;{void*kb=_tmp17;
s=Cyc_Absynpp_kindbound2string(kb);goto _LL0;}case 10: _tmp17=(void*)((struct Cyc_Warn_Attribute_Warn_Warg_struct*)_tmp15)->f1;{void*a=_tmp17;
s=Cyc_Absynpp_attribute2string(a);goto _LL0;}case 11: _tmp17=((struct Cyc_Warn_Vardecl_Warn_Warg_struct*)_tmp15)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp17;
s=Cyc_Absynpp_qvar2string(vd->name);goto _LL0;}default: _tmp16=((struct Cyc_Warn_Int_Warn_Warg_struct*)_tmp15)->f1;{int i=_tmp16;
s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1E=({struct Cyc_Int_pa_PrintArg_struct _tmp3D;_tmp3D.tag=1,_tmp3D.f1=(unsigned long)i;_tmp3D;});void*_tmp1C[1];_tmp1C[0]=& _tmp1E;({struct _fat_ptr _tmp4E=({const char*_tmp1D="%d";_tag_fat(_tmp1D,sizeof(char),3U);});Cyc_aprintf(_tmp4E,_tag_fat(_tmp1C,sizeof(void*),1));});});goto _LL0;}}_LL0:;}{
# 135
int s_len=(int)Cyc_strlen((struct _fat_ptr)s);
if(s_len >= 80){
lst=({struct Cyc_List_List*(*_tmp55)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp54=({struct _fat_ptr*_tmp1F[3];({struct _fat_ptr*_tmp53=({struct _fat_ptr*_tmp21=_cycalloc(sizeof(*_tmp21));({struct _fat_ptr _tmp52=({const char*_tmp20="\n";_tag_fat(_tmp20,sizeof(char),2U);});*_tmp21=_tmp52;});_tmp21;});_tmp1F[0]=_tmp53;}),({struct _fat_ptr*_tmp51=({struct _fat_ptr*_tmp22=_cycalloc(sizeof(*_tmp22));*_tmp22=s;_tmp22;});_tmp1F[1]=_tmp51;}),({struct _fat_ptr*_tmp50=({struct _fat_ptr*_tmp24=_cycalloc(sizeof(*_tmp24));({struct _fat_ptr _tmp4F=({const char*_tmp23="\n";_tag_fat(_tmp23,sizeof(char),2U);});*_tmp24=_tmp4F;});_tmp24;});_tmp1F[2]=_tmp50;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp1F,sizeof(struct _fat_ptr*),3));});_tmp55(_tmp54,lst);});
curr_line_len=0;}else{
if(curr_line_len + s_len >= 80){
lst=({struct Cyc_List_List*(*_tmp5A)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp59=({struct _fat_ptr*_tmp25[2];({struct _fat_ptr*_tmp58=({struct _fat_ptr*_tmp27=_cycalloc(sizeof(*_tmp27));({struct _fat_ptr _tmp57=({const char*_tmp26="\n";_tag_fat(_tmp26,sizeof(char),2U);});*_tmp27=_tmp57;});_tmp27;});_tmp25[0]=_tmp58;}),({struct _fat_ptr*_tmp56=({struct _fat_ptr*_tmp28=_cycalloc(sizeof(*_tmp28));*_tmp28=s;_tmp28;});_tmp25[1]=_tmp56;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp25,sizeof(struct _fat_ptr*),2));});_tmp5A(_tmp59,lst);});
curr_line_len=s_len;}else{
# 143
lst=({struct Cyc_List_List*_tmp2A=_cycalloc(sizeof(*_tmp2A));({struct _fat_ptr*_tmp5B=({struct _fat_ptr*_tmp29=_cycalloc(sizeof(*_tmp29));*_tmp29=s;_tmp29;});_tmp2A->hd=_tmp5B;}),_tmp2A->tl=lst;_tmp2A;});
curr_line_len +=s_len;}}}}}
# 147
return(struct _fat_ptr)Cyc_strconcat_l(({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(lst));}
# 149
void Cyc_Warn_verr2(unsigned loc,struct _fat_ptr args){
Cyc_Position_post_error(({unsigned _tmp5C=loc;Cyc_Position_mk_err(_tmp5C,Cyc_Warn_args2string(args));}));}
# 152
void Cyc_Warn_err2(unsigned loc,struct _fat_ptr args){
Cyc_Warn_verr2(loc,args);}
# 155
void Cyc_Warn_vwarn2(unsigned loc,struct _fat_ptr args){
struct _fat_ptr msg=Cyc_Warn_args2string(args);
Cyc_Warn_warning_segs=({struct Cyc_List_List*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->hd=(void*)loc,_tmp2B->tl=Cyc_Warn_warning_segs;_tmp2B;});
Cyc_Warn_warning_msgs=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));({struct _fat_ptr*_tmp5D=({struct _fat_ptr*_tmp2C=_cycalloc(sizeof(*_tmp2C));*_tmp2C=msg;_tmp2C;});_tmp2D->hd=_tmp5D;}),_tmp2D->tl=Cyc_Warn_warning_msgs;_tmp2D;});}
# 160
void Cyc_Warn_warn2(unsigned loc,struct _fat_ptr args){
Cyc_Warn_vwarn2(loc,args);}
# 163
void*Cyc_Warn_vimpos2(struct _fat_ptr args){
struct _fat_ptr _tmp2E=Cyc_Warn_args2string(args);struct _fat_ptr msg=_tmp2E;
if(!Cyc_Position_error_p()){
({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp3E;_tmp3E.tag=0,_tmp3E.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp3E;});void*_tmp2F[1];_tmp2F[0]=& _tmp31;({struct Cyc___cycFILE*_tmp5F=Cyc_stderr;struct _fat_ptr _tmp5E=({const char*_tmp30="Compiler Error: %s\n";_tag_fat(_tmp30,sizeof(char),20U);});Cyc_fprintf(_tmp5F,_tmp5E,_tag_fat(_tmp2F,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);}
# 169
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=Cyc_Core_Impossible,_tmp32->f1=msg;_tmp32;}));}
# 171
void*Cyc_Warn_impos2(struct _fat_ptr args){
Cyc_Warn_vimpos2(args);}
# 174
void*Cyc_Warn_vimpos_loc2(unsigned loc,struct _fat_ptr args){
struct _fat_ptr _tmp33=Cyc_Warn_args2string(args);struct _fat_ptr msg=_tmp33;
if(!Cyc_Position_error_p()){
({void*_tmp34=0U;({struct Cyc___cycFILE*_tmp61=Cyc_stderr;struct _fat_ptr _tmp60=({const char*_tmp35="Compiler Error: \n";_tag_fat(_tmp35,sizeof(char),18U);});Cyc_fprintf(_tmp61,_tmp60,_tag_fat(_tmp34,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp3F;_tmp3F.tag=0,_tmp3F.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp3F;});void*_tmp36[1];_tmp36[0]=& _tmp38;({struct Cyc___cycFILE*_tmp63=Cyc_stderr;struct _fat_ptr _tmp62=({const char*_tmp37="%s";_tag_fat(_tmp37,sizeof(char),3U);});Cyc_fprintf(_tmp63,_tmp62,_tag_fat(_tmp36,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);}
# 181
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=Cyc_Core_Impossible,_tmp39->f1=msg;_tmp39;}));}
# 183
void*Cyc_Warn_impos_loc2(unsigned loc,struct _fat_ptr args){
# 185
Cyc_Warn_vimpos_loc2(loc,args);}

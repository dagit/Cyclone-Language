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
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190 "list.h"
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;
# 47 "position.h"
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 888 "absyn.h"
void*Cyc_Absyn_compress_kb(void*);
# 917
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 946
void*Cyc_Absyn_bounds_one();struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 74
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 131
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2);struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 187
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 196
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 253
void*Cyc_Tcutil_normalize_effect(void*e);
# 266
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 313
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 317
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_tycon_cmp(void*,void*);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);char Cyc_Unify_Unify[6U]="Unify";struct Cyc_Unify_Unify_exn_struct{char*tag;};
# 34 "unify.cyc"
struct Cyc_Unify_Unify_exn_struct Cyc_Unify_Unify_val={Cyc_Unify_Unify};struct _tuple10{void*f1;void*f2;};
# 37
static struct _tuple10 Cyc_Unify_ts_failure={0,0};struct _tuple11{int f1;int f2;};
static struct _tuple11 Cyc_Unify_tqs_const={0,0};
static struct _fat_ptr Cyc_Unify_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 41
static void Cyc_Unify_fail_because(struct _fat_ptr reason){
Cyc_Unify_failure_reason=reason;
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 49
void Cyc_Unify_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;
Cyc_fflush(Cyc_stderr);
# 55
if(({struct _fat_ptr _tmp10F=({const char*_tmp0="(qualifiers don't match)";_tag_fat(_tmp0,sizeof(char),25U);});Cyc_strcmp(_tmp10F,(struct _fat_ptr)Cyc_Unify_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0U,_tmpFB.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Unify_failure_reason);_tmpFB;});void*_tmp1[1U];_tmp1[0]=& _tmp3;({struct Cyc___cycFILE*_tmp111=Cyc_stderr;struct _fat_ptr _tmp110=({const char*_tmp2="  %s\n";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_fprintf(_tmp111,_tmp110,_tag_fat(_tmp1,sizeof(void*),1U));});});
return;}
# 60
if(({struct _fat_ptr _tmp112=({const char*_tmp4="(function effects do not match)";_tag_fat(_tmp4,sizeof(char),32U);});Cyc_strcmp(_tmp112,(struct _fat_ptr)Cyc_Unify_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp5=Cyc_Absynpp_tc_params_r;struct Cyc_Absynpp_Params p=_tmp5;
p.print_all_effects=1;
Cyc_Absynpp_set_params(& p);}{
# 65
struct _tuple10 _tmp6=Cyc_Unify_ts_failure;void*_tmp8;void*_tmp7;_LL1: _tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_LL2: {void*t1f=_tmp7;void*t2f=_tmp8;
struct _fat_ptr s1=(unsigned)t1f?Cyc_Absynpp_typ2string(t1f):({const char*_tmp23="<?>";_tag_fat(_tmp23,sizeof(char),4U);});
struct _fat_ptr s2=(unsigned)t2f?Cyc_Absynpp_typ2string(t2f):({const char*_tmp22="<?>";_tag_fat(_tmp22,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmpFC;_tmpFC.tag=0U,_tmpFC.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmpFC;});void*_tmp9[1U];_tmp9[0]=& _tmpB;({struct Cyc___cycFILE*_tmp114=Cyc_stderr;struct _fat_ptr _tmp113=({const char*_tmpA="  %s";_tag_fat(_tmpA,sizeof(char),5U);});Cyc_fprintf(_tmp114,_tmp113,_tag_fat(_tmp9,sizeof(void*),1U));});});
pos +=_get_fat_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpC=0U;({struct Cyc___cycFILE*_tmp116=Cyc_stderr;struct _fat_ptr _tmp115=({const char*_tmpD="\n\t";_tag_fat(_tmpD,sizeof(char),3U);});Cyc_fprintf(_tmp116,_tmp115,_tag_fat(_tmpC,sizeof(void*),0U));});});
pos=8;}else{
# 75
({void*_tmpE=0U;({struct Cyc___cycFILE*_tmp118=Cyc_stderr;struct _fat_ptr _tmp117=({const char*_tmpF=" ";_tag_fat(_tmpF,sizeof(char),2U);});Cyc_fprintf(_tmp118,_tmp117,_tag_fat(_tmpE,sizeof(void*),0U));});});
++ pos;}
# 78
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmp11A=Cyc_stderr;struct _fat_ptr _tmp119=({const char*_tmp11="and ";_tag_fat(_tmp11,sizeof(char),5U);});Cyc_fprintf(_tmp11A,_tmp119,_tag_fat(_tmp10,sizeof(void*),0U));});});
pos +=4;
if((unsigned)pos + _get_fat_size(s2,sizeof(char))>= (unsigned)80){
({void*_tmp12=0U;({struct Cyc___cycFILE*_tmp11C=Cyc_stderr;struct _fat_ptr _tmp11B=({const char*_tmp13="\n\t";_tag_fat(_tmp13,sizeof(char),3U);});Cyc_fprintf(_tmp11C,_tmp11B,_tag_fat(_tmp12,sizeof(void*),0U));});});
pos=8;}
# 84
({struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0U,_tmpFD.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmpFD;});void*_tmp14[1U];_tmp14[0]=& _tmp16;({struct Cyc___cycFILE*_tmp11E=Cyc_stderr;struct _fat_ptr _tmp11D=({const char*_tmp15="%s ";_tag_fat(_tmp15,sizeof(char),4U);});Cyc_fprintf(_tmp11E,_tmp11D,_tag_fat(_tmp14,sizeof(void*),1U));});});
pos +=_get_fat_size(s2,sizeof(char))+ (unsigned)1;
if(pos + 17 >= 80){
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmp120=Cyc_stderr;struct _fat_ptr _tmp11F=({const char*_tmp18="\n\t";_tag_fat(_tmp18,sizeof(char),3U);});Cyc_fprintf(_tmp120,_tmp11F,_tag_fat(_tmp17,sizeof(void*),0U));});});
pos=8;}
# 90
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmp122=Cyc_stderr;struct _fat_ptr _tmp121=({const char*_tmp1A="are not compatible. ";_tag_fat(_tmp1A,sizeof(char),21U);});Cyc_fprintf(_tmp122,_tmp121,_tag_fat(_tmp19,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmp123=(char*)Cyc_Unify_failure_reason.curr;_tmp123 != (char*)(_tag_fat(0,0,0)).curr;})){
if(({unsigned long _tmp124=(unsigned long)pos;_tmp124 + Cyc_strlen((struct _fat_ptr)Cyc_Unify_failure_reason);})>= (unsigned long)80)
({void*_tmp1B=0U;({struct Cyc___cycFILE*_tmp126=Cyc_stderr;struct _fat_ptr _tmp125=({const char*_tmp1C="\n\t";_tag_fat(_tmp1C,sizeof(char),3U);});Cyc_fprintf(_tmp126,_tmp125,_tag_fat(_tmp1B,sizeof(void*),0U));});});
# 96
({struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0U,_tmpFE.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Unify_failure_reason);_tmpFE;});void*_tmp1D[1U];_tmp1D[0]=& _tmp1F;({struct Cyc___cycFILE*_tmp128=Cyc_stderr;struct _fat_ptr _tmp127=({const char*_tmp1E="%s";_tag_fat(_tmp1E,sizeof(char),3U);});Cyc_fprintf(_tmp128,_tmp127,_tag_fat(_tmp1D,sizeof(void*),1U));});});}
# 98
({void*_tmp20=0U;({struct Cyc___cycFILE*_tmp12A=Cyc_stderr;struct _fat_ptr _tmp129=({const char*_tmp21="\n";_tag_fat(_tmp21,sizeof(char),2U);});Cyc_fprintf(_tmp12A,_tmp129,_tag_fat(_tmp20,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);}}}
# 104
static int Cyc_Unify_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Relations_check_logical_implication(r1,r2)&&
 Cyc_Relations_check_logical_implication(r2,r1);}
# 110
int Cyc_Unify_unify_kindbound(void*kb1,void*kb2){
struct _tuple10 _tmp24=({struct _tuple10 _tmpFF;({void*_tmp12C=Cyc_Absyn_compress_kb(kb1);_tmpFF.f1=_tmp12C;}),({void*_tmp12B=Cyc_Absyn_compress_kb(kb2);_tmpFF.f2=_tmp12B;});_tmpFF;});struct _tuple10 _stmttmp0=_tmp24;struct _tuple10 _tmp25=_stmttmp0;void*_tmp27;struct Cyc_Core_Opt**_tmp26;struct Cyc_Core_Opt**_tmp29;void*_tmp28;struct Cyc_Absyn_Kind*_tmp2D;struct Cyc_Core_Opt**_tmp2C;struct Cyc_Absyn_Kind*_tmp2B;struct Cyc_Core_Opt**_tmp2A;struct Cyc_Absyn_Kind*_tmp30;struct Cyc_Absyn_Kind*_tmp2F;struct Cyc_Core_Opt**_tmp2E;struct Cyc_Absyn_Kind*_tmp33;struct Cyc_Core_Opt**_tmp32;struct Cyc_Absyn_Kind*_tmp31;struct Cyc_Absyn_Kind*_tmp35;struct Cyc_Absyn_Kind*_tmp34;switch(*((int*)_tmp25.f1)){case 0U: switch(*((int*)_tmp25.f2)){case 0U: _LL1: _tmp34=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp35=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_LL2: {struct Cyc_Absyn_Kind*k1=_tmp34;struct Cyc_Absyn_Kind*k2=_tmp35;
return k1 == k2;}case 2U: _LL5: _tmp31=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp32=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_tmp33=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f2;_LL6: {struct Cyc_Absyn_Kind*k1=_tmp31;struct Cyc_Core_Opt**x=_tmp32;struct Cyc_Absyn_Kind*k2=_tmp33;
# 119
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp12D=({struct Cyc_Core_Opt*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->v=kb1;_tmp37;});*x=_tmp12D;});
return 1;}else{
return 0;}}default: goto _LLB;}case 2U: switch(*((int*)_tmp25.f2)){case 0U: _LL3: _tmp2E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp2F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f2;_tmp30=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_LL4: {struct Cyc_Core_Opt**x=_tmp2E;struct Cyc_Absyn_Kind*k2=_tmp2F;struct Cyc_Absyn_Kind*k1=_tmp30;
# 114
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp12E=({struct Cyc_Core_Opt*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->v=kb2;_tmp36;});*x=_tmp12E;});
return 1;}else{
return 0;}}case 2U: _LL7: _tmp2A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp2B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f2;_tmp2C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_tmp2D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f2;_LL8: {struct Cyc_Core_Opt**x=_tmp2A;struct Cyc_Absyn_Kind*k1=_tmp2B;struct Cyc_Core_Opt**y=_tmp2C;struct Cyc_Absyn_Kind*k2=_tmp2D;
# 124
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp12F=({struct Cyc_Core_Opt*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->v=kb1;_tmp38;});*y=_tmp12F;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp130=({struct Cyc_Core_Opt*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->v=kb2;_tmp39;});*x=_tmp130;});
return 1;}else{
return 0;}}}default: _LLB: _tmp28=_tmp25.f1;_tmp29=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_LLC: {void*y=_tmp28;struct Cyc_Core_Opt**x=_tmp29;
# 133
({struct Cyc_Core_Opt*_tmp131=({struct Cyc_Core_Opt*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->v=y;_tmp3A;});*x=_tmp131;});
return 1;}}default: _LL9: _tmp26=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp27=_tmp25.f2;_LLA: {struct Cyc_Core_Opt**x=_tmp26;void*y=_tmp27;
# 131
_tmp28=y;_tmp29=x;goto _LLC;}}_LL0:;}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 140
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp3B=t;struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3E;struct Cyc_List_List*_tmp3F;struct Cyc_List_List*_tmp46;struct Cyc_Absyn_VarargInfo*_tmp45;struct Cyc_List_List*_tmp44;void*_tmp43;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;struct Cyc_List_List*_tmp40;void*_tmp48;void*_tmp47;struct Cyc_Absyn_PtrInfo _tmp49;struct Cyc_Core_Opt**_tmp4B;void*_tmp4A;struct Cyc_Absyn_Tvar*_tmp4C;switch(*((int*)_tmp3B)){case 2U: _LL1: _tmp4C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp4C;
# 144
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,tv))
Cyc_Unify_fail_because(({const char*_tmp4D="(type variable would escape scope)";_tag_fat(_tmp4D,sizeof(char),35U);}));
goto _LL0;}case 1U: _LL3: _tmp4A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B)->f2;_tmp4B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B)->f4;_LL4: {void*rg=_tmp4A;struct Cyc_Core_Opt**sopt=_tmp4B;
# 148
if(t == evar)
Cyc_Unify_fail_because(({const char*_tmp4E="(occurs check)";_tag_fat(_tmp4E,sizeof(char),15U);}));else{
if(rg != 0)Cyc_Unify_occurs(evar,r,env,rg);else{
# 153
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*sopt))->v;for(0;s != 0 && !problem;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
problem=1;}}
# 158
if(problem){
struct Cyc_List_List*_tmp4F=0;struct Cyc_List_List*result=_tmp4F;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*sopt))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
result=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp50->tl=result;_tmp50;});}}
({struct Cyc_Core_Opt*_tmp132=({struct Cyc_Core_Opt*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->v=result;_tmp51;});*sopt=_tmp132;});}}}
# 166
goto _LL0;}case 3U: _LL5: _tmp49=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->f1;_LL6: {struct Cyc_Absyn_PtrInfo pinfo=_tmp49;
# 168
Cyc_Unify_occurs(evar,r,env,pinfo.elt_type);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).rgn);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).nullable);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).bounds);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).zero_term);
goto _LL0;}case 4U: _LL7: _tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).elt_type;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).zero_term;_LL8: {void*t2=_tmp47;void*zt=_tmp48;
# 176
Cyc_Unify_occurs(evar,r,env,t2);
Cyc_Unify_occurs(evar,r,env,zt);
goto _LL0;}case 5U: _LL9: _tmp40=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).tvars;_tmp41=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).effect;_tmp42=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).ret_tqual;_tmp43=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).ret_type;_tmp44=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).args;_tmp45=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).cyc_varargs;_tmp46=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).rgn_po;_LLA: {struct Cyc_List_List*tvs=_tmp40;void*eff=_tmp41;struct Cyc_Absyn_Tqual rt_tq=_tmp42;void*rt=_tmp43;struct Cyc_List_List*args=_tmp44;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp45;struct Cyc_List_List*rgn_po=_tmp46;
# 181
env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,tvs,env);
if(eff != 0)Cyc_Unify_occurs(evar,r,env,eff);
Cyc_Unify_occurs(evar,r,env,rt);
for(0;args != 0;args=args->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple8*)args->hd)).f3);}
if(cyc_varargs != 0)
Cyc_Unify_occurs(evar,r,env,cyc_varargs->type);
for(0;rgn_po != 0;rgn_po=rgn_po->tl){
struct _tuple10*_tmp52=(struct _tuple10*)rgn_po->hd;struct _tuple10*_stmttmp1=_tmp52;struct _tuple10*_tmp53=_stmttmp1;void*_tmp55;void*_tmp54;_LL16: _tmp54=_tmp53->f1;_tmp55=_tmp53->f2;_LL17: {void*r1=_tmp54;void*r2=_tmp55;
Cyc_Unify_occurs(evar,r,env,r1);
Cyc_Unify_occurs(evar,r,env,r2);}}
# 193
goto _LL0;}case 6U: _LLB: _tmp3F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B)->f1;_LLC: {struct Cyc_List_List*args=_tmp3F;
# 195
for(0;args != 0;args=args->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple12*)args->hd)).f2);}
goto _LL0;}case 7U: _LLD: _tmp3E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B)->f2;_LLE: {struct Cyc_List_List*fs=_tmp3E;
# 200
for(0;fs != 0;fs=fs->tl){
Cyc_Unify_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);}
goto _LL0;}case 8U: _LLF: _tmp3D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3B)->f2;_LL10: {struct Cyc_List_List*ts=_tmp3D;
_tmp3C=ts;goto _LL12;}case 0U: _LL11: _tmp3C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2;_LL12: {struct Cyc_List_List*ts=_tmp3C;
# 205
for(0;ts != 0;ts=ts->tl){
Cyc_Unify_occurs(evar,r,env,(void*)ts->hd);}
goto _LL0;}default: _LL13: _LL14:
# 210
 goto _LL0;}_LL0:;}}
# 214
static void Cyc_Unify_unify_it(void*t1,void*t2);
# 217
int Cyc_Unify_unify(void*t1,void*t2){
struct _handler_cons _tmp56;_push_handler(& _tmp56);{int _tmp58=0;if(setjmp(_tmp56.handler))_tmp58=1;if(!_tmp58){
Cyc_Unify_unify_it(t1,t2);{
int _tmp59=1;_npop_handler(0U);return _tmp59;}
# 219
;_pop_handler();}else{void*_tmp57=(void*)Cyc_Core_get_exn_thrown();void*_tmp5A=_tmp57;void*_tmp5B;if(((struct Cyc_Unify_Unify_exn_struct*)_tmp5A)->tag == Cyc_Unify_Unify){_LL1: _LL2:
# 221
 return 0;}else{_LL3: _tmp5B=_tmp5A;_LL4: {void*exn=_tmp5B;(int)_rethrow(exn);}}_LL0:;}}}
# 225
static void Cyc_Unify_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0 && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Unify_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0 || t2 != 0)
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 233
static void Cyc_Unify_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const && !tq1.real_const)
({void*_tmp5C=0U;({struct _fat_ptr _tmp133=({const char*_tmp5D="tq1 real_const not set.";_tag_fat(_tmp5D,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp133,_tag_fat(_tmp5C,sizeof(void*),0U));});});
if(tq2.print_const && !tq2.real_const)
({void*_tmp5E=0U;({struct _fat_ptr _tmp134=({const char*_tmp5F="tq2 real_const not set.";_tag_fat(_tmp5F,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp134,_tag_fat(_tmp5E,sizeof(void*),0U));});});
# 239
if((tq1.real_const != tq2.real_const || tq1.q_volatile != tq2.q_volatile)|| tq1.q_restrict != tq2.q_restrict){
# 242
Cyc_Unify_ts_failure=({struct _tuple10 _tmp100;_tmp100.f1=t1,_tmp100.f2=t2;_tmp100;});
Cyc_Unify_tqs_const=({struct _tuple11 _tmp101;_tmp101.f1=tq1.real_const,_tmp101.f2=tq2.real_const;_tmp101;});
Cyc_Unify_failure_reason=({const char*_tmp60="(qualifiers don't match)";_tag_fat(_tmp60,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 248
Cyc_Unify_tqs_const=({struct _tuple11 _tmp102;_tmp102.f1=0,_tmp102.f2=0;_tmp102;});}
# 262 "unify.cyc"
static int Cyc_Unify_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2)&& Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2)&& Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 272
static int Cyc_Unify_unify_const_exp_opt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
if(e1 == 0 && e2 == 0)
return 1;
if(e1 == 0 || e2 == 0)
return 0;
return Cyc_Evexp_same_const_exp(e1,e2);}struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple14{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 281
static void Cyc_Unify_unify_it(void*t1,void*t2){
Cyc_Unify_ts_failure=({struct _tuple10 _tmp103;_tmp103.f1=t1,_tmp103.f2=t2;_tmp103;});
Cyc_Unify_failure_reason=_tag_fat(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp61=t1;struct Cyc_Core_Opt*_tmp64;void**_tmp63;struct Cyc_Core_Opt*_tmp62;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->tag == 1U){_LL1: _tmp62=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f1;_tmp63=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f2;_tmp64=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f4;_LL2: {struct Cyc_Core_Opt*kind1=_tmp62;void**ref1_ref=_tmp63;struct Cyc_Core_Opt*s1opt=_tmp64;
# 291
Cyc_Unify_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s1opt))->v,t2);{
struct Cyc_Absyn_Kind*_tmp65=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*kind2=_tmp65;
# 296
if(Cyc_Tcutil_kind_leq(kind2,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(kind1))->v)){
*ref1_ref=t2;
return;}{
# 300
void*_tmp66=t2;struct Cyc_Absyn_PtrInfo _tmp67;struct Cyc_Core_Opt*_tmp69;void**_tmp68;switch(*((int*)_tmp66)){case 1U: _LL6: _tmp68=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp66)->f2;_tmp69=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp66)->f4;_LL7: {void**ref2_ref=_tmp68;struct Cyc_Core_Opt*s2opt=_tmp69;
# 303
struct Cyc_List_List*_tmp6A=(struct Cyc_List_List*)s1opt->v;struct Cyc_List_List*s1=_tmp6A;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s2opt))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,s1,(struct Cyc_Absyn_Tvar*)s2->hd))
Cyc_Unify_fail_because(({const char*_tmp6B="(type variable would escape scope)";_tag_fat(_tmp6B,sizeof(char),35U);}));}}
if(!Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)kind1->v,kind2))
Cyc_Unify_fail_because(({const char*_tmp6C="(kinds are incompatible)";_tag_fat(_tmp6C,sizeof(char),25U);}));
*ref2_ref=t1;
return;}case 3U: _LL8: _tmp67=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66)->f1;if((int)((struct Cyc_Absyn_Kind*)kind1->v)->kind == (int)Cyc_Absyn_BoxKind){_LL9: {struct Cyc_Absyn_PtrInfo pinfo=_tmp67;
# 315
void*_tmp6D=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*c=_tmp6D;
void*_tmp6E=c;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6E)->tag == 1U){_LLD: _LLE:
# 318
({void*_tmp135=c;Cyc_Unify_unify(_tmp135,Cyc_Absyn_bounds_one());});
*ref1_ref=t2;
return;}else{_LLF: _LL10:
 Cyc_Unify_fail_because(({const char*_tmp6F="(kinds are incompatible)";_tag_fat(_tmp6F,sizeof(char),25U);}));}_LLC:;}}else{goto _LLA;}default: _LLA: _LLB:
# 323
 Cyc_Unify_fail_because(({const char*_tmp70="(kinds are incompatible)";_tag_fat(_tmp70,sizeof(char),25U);}));}_LL5:;}}}}else{_LL3: _LL4:
# 325
 goto _LL0;}_LL0:;}{
# 329
struct _tuple10 _tmp71=({struct _tuple10 _tmp10E;_tmp10E.f1=t2,_tmp10E.f2=t1;_tmp10E;});struct _tuple10 _stmttmp2=_tmp71;struct _tuple10 _tmp72=_stmttmp2;struct Cyc_Absyn_Typedefdecl*_tmp76;struct Cyc_List_List*_tmp75;struct Cyc_Absyn_Typedefdecl*_tmp74;struct Cyc_List_List*_tmp73;struct Cyc_List_List*_tmp7A;enum Cyc_Absyn_AggrKind _tmp79;struct Cyc_List_List*_tmp78;enum Cyc_Absyn_AggrKind _tmp77;struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp91;struct Cyc_Absyn_VarargInfo*_tmp90;int _tmp8F;struct Cyc_List_List*_tmp8E;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8C;void*_tmp8B;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_List_List*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp84;struct Cyc_Absyn_VarargInfo*_tmp83;int _tmp82;struct Cyc_List_List*_tmp81;void*_tmp80;struct Cyc_Absyn_Tqual _tmp7F;void*_tmp7E;struct Cyc_List_List*_tmp7D;void*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Tqual _tmp9C;void*_tmp9B;void*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Tqual _tmp98;void*_tmp97;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;void*_tmpAC;void*_tmpAB;void*_tmpAA;void*_tmpA9;struct Cyc_Absyn_Tqual _tmpA8;void*_tmpA7;void*_tmpA6;void*_tmpA5;void*_tmpA4;void*_tmpA3;struct Cyc_Absyn_Tqual _tmpA2;void*_tmpA1;struct Cyc_Absyn_Tvar*_tmpAE;struct Cyc_Absyn_Tvar*_tmpAD;struct Cyc_List_List*_tmpB2;void*_tmpB1;struct Cyc_List_List*_tmpB0;void*_tmpAF;switch(*((int*)_tmp72.f1)){case 1U: _LL12: _LL13:
# 332
 Cyc_Unify_unify_it(t2,t1);return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)->tag == 9U){_LL14: _LL15:
# 334
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmpAF=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpB0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f2;_tmpB1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1;_tmpB2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f2;_LL21: {void*c1=_tmpAF;struct Cyc_List_List*ts1=_tmpB0;void*c2=_tmpB1;struct Cyc_List_List*ts2=_tmpB2;
# 345
if(Cyc_Tcutil_tycon_cmp(c1,c2)!= 0)
Cyc_Unify_fail_because(({const char*_tmpB4="(different type constructors)";_tag_fat(_tmpB4,sizeof(char),30U);}));
Cyc_Unify_unify_list(ts1,ts2);
return;}}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)){case 8U: _LL18: _LL19:
# 336
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 338
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1)){case 9U: _LL16: _LL17:
# 335
 goto _LL19;case 8U: _LL1A: _LL1B:
# 337
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 340
 if(!Cyc_Unify_unify_effect(t1,t2))
Cyc_Unify_fail_because(({const char*_tmpB3="(effects don't unify)";_tag_fat(_tmpB3,sizeof(char),22U);}));
return;default: switch(*((int*)_tmp72.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp72.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72.f2)->tag == 2U){_LL22: _tmpAD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpAE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72.f2)->f1;_LL23: {struct Cyc_Absyn_Tvar*tv2=_tmpAD;struct Cyc_Absyn_Tvar*tv1=_tmpAE;
# 352
if(tv2->identity != tv1->identity)
Cyc_Unify_fail_because(({const char*_tmpB5="(variable types are not the same)";_tag_fat(_tmpB5,sizeof(char),34U);}));
return;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->tag == 3U){_LL24: _tmpA1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).elt_type;_tmpA2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).elt_tq;_tmpA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).rgn;_tmpA4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).nullable;_tmpA5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).bounds;_tmpA6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).zero_term;_tmpA7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).elt_type;_tmpA8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).elt_tq;_tmpA9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).rgn;_tmpAA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).nullable;_tmpAB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).bounds;_tmpAC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).zero_term;_LL25: {void*t2a=_tmpA1;struct Cyc_Absyn_Tqual tqual2a=_tmpA2;void*rgn2=_tmpA3;void*null2a=_tmpA4;void*b2=_tmpA5;void*zt2=_tmpA6;void*t1a=_tmpA7;struct Cyc_Absyn_Tqual tqual1a=_tmpA8;void*rgn1=_tmpA9;void*null1a=_tmpAA;void*b1=_tmpAB;void*zt1=_tmpAC;
# 358
Cyc_Unify_unify_it(t1a,t2a);
Cyc_Unify_unify_it(rgn2,rgn1);{
struct _fat_ptr _tmpB6=Cyc_Unify_failure_reason;struct _fat_ptr orig_failure=_tmpB6;
if(!Cyc_Unify_unify(zt1,zt2)){
Cyc_Unify_ts_failure=({struct _tuple10 _tmp104;_tmp104.f1=t1,_tmp104.f2=t2;_tmp104;});
Cyc_Unify_fail_because(({const char*_tmpB7="(not both zero terminated)";_tag_fat(_tmpB7,sizeof(char),27U);}));}
# 365
Cyc_Unify_unify_tqual(tqual1a,t1a,tqual2a,t2a);
if(!Cyc_Unify_unify(b1,b2)){
Cyc_Unify_ts_failure=({struct _tuple10 _tmp105;_tmp105.f1=t1,_tmp105.f2=t2;_tmp105;});
Cyc_Unify_fail_because(({const char*_tmpB8="(different pointer bounds)";_tag_fat(_tmpB8,sizeof(char),27U);}));}{
# 371
void*_tmpB9=Cyc_Tcutil_compress(b1);void*_stmttmp3=_tmpB9;void*_tmpBA=_stmttmp3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBA)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBA)->f1)->tag == 14U){_LL35: _LL36:
# 373
 Cyc_Unify_failure_reason=orig_failure;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 376
 Cyc_Unify_failure_reason=({const char*_tmpBB="(incompatible pointer types)";_tag_fat(_tmpBB,sizeof(char),29U);});
Cyc_Unify_unify_it(null1a,null2a);
return;}_LL34:;}}}}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp72.f2)->tag == 9U){_LL26: _tmp9F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpA0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp72.f2)->f1;_LL27: {struct Cyc_Absyn_Exp*e1=_tmp9F;struct Cyc_Absyn_Exp*e2=_tmpA0;
# 382
if(!Cyc_Evexp_same_const_exp(e1,e2))
Cyc_Unify_fail_because(({const char*_tmpBC="(cannot prove expressions are the same)";_tag_fat(_tmpBC,sizeof(char),40U);}));
return;}}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->tag == 4U){_LL28: _tmp97=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).elt_type;_tmp98=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).tq;_tmp99=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).num_elts;_tmp9A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).zero_term;_tmp9B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).elt_type;_tmp9C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).tq;_tmp9D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).num_elts;_tmp9E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).zero_term;_LL29: {void*t2a=_tmp97;struct Cyc_Absyn_Tqual tq2a=_tmp98;struct Cyc_Absyn_Exp*e1=_tmp99;void*zt1=_tmp9A;void*t1a=_tmp9B;struct Cyc_Absyn_Tqual tq1a=_tmp9C;struct Cyc_Absyn_Exp*e2=_tmp9D;void*zt2=_tmp9E;
# 388
Cyc_Unify_unify_it(t1a,t2a);
Cyc_Unify_unify_tqual(tq1a,t1a,tq2a,t2a);
Cyc_Unify_failure_reason=({const char*_tmpBD="(not both zero terminated)";_tag_fat(_tmpBD,sizeof(char),27U);});
Cyc_Unify_unify_it(zt1,zt2);
if(!Cyc_Unify_unify_const_exp_opt(e1,e2))
Cyc_Unify_fail_because(({const char*_tmpBE="(different array sizes)";_tag_fat(_tmpBE,sizeof(char),24U);}));
return;}}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->tag == 5U){_LL2A: _tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).tvars;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).effect;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ret_tqual;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ret_type;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).args;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).c_varargs;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).cyc_varargs;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).rgn_po;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).attributes;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).requires_clause;_tmp87=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).requires_relns;_tmp88=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ensures_clause;_tmp89=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ensures_relns;_tmp8A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).tvars;_tmp8B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).effect;_tmp8C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ret_tqual;_tmp8D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ret_type;_tmp8E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).args;_tmp8F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).c_varargs;_tmp90=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).cyc_varargs;_tmp91=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).rgn_po;_tmp92=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).attributes;_tmp93=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).requires_clause;_tmp94=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).requires_relns;_tmp95=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ensures_clause;_tmp96=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ensures_relns;_LL2B: {struct Cyc_List_List*tvs2=_tmp7D;void*eff2=_tmp7E;struct Cyc_Absyn_Tqual rt_tq2=_tmp7F;void*rt2=_tmp80;struct Cyc_List_List*args2=_tmp81;int c_vararg2=_tmp82;struct Cyc_Absyn_VarargInfo*cyc_vararg2=_tmp83;struct Cyc_List_List*rpo2=_tmp84;struct Cyc_List_List*atts2=_tmp85;struct Cyc_Absyn_Exp*req1=_tmp86;struct Cyc_List_List*req_relns1=_tmp87;struct Cyc_Absyn_Exp*ens1=_tmp88;struct Cyc_List_List*ens_relns1=_tmp89;struct Cyc_List_List*tvs1=_tmp8A;void*eff1=_tmp8B;struct Cyc_Absyn_Tqual rt_tq1=_tmp8C;void*rt1=_tmp8D;struct Cyc_List_List*args1=_tmp8E;int c_vararg1=_tmp8F;struct Cyc_Absyn_VarargInfo*cyc_vararg1=_tmp90;struct Cyc_List_List*rpo1=_tmp91;struct Cyc_List_List*atts1=_tmp92;struct Cyc_Absyn_Exp*req2=_tmp93;struct Cyc_List_List*req_relns2=_tmp94;struct Cyc_Absyn_Exp*ens2=_tmp95;struct Cyc_List_List*ens_relns2=_tmp96;
# 398
{struct _RegionHandle _tmpBF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpBF;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(tvs2 == 0)
Cyc_Unify_fail_because(({const char*_tmpC0="(second function type has too few type variables)";_tag_fat(_tmpC0,sizeof(char),50U);}));{
void*_tmpC1=((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind;void*kb1=_tmpC1;
void*_tmpC2=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*kb2=_tmpC2;
if(!Cyc_Unify_unify_kindbound(kb1,kb2))
Cyc_Unify_fail_because((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0U,({
struct _fat_ptr _tmp136=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)tvs1->hd));_tmp108.f1=_tmp136;});_tmp108;});struct Cyc_String_pa_PrintArg_struct _tmpC6=({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0U,({
struct _fat_ptr _tmp137=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs1->hd,& Cyc_Tcutil_bk)));_tmp107.f1=_tmp137;});_tmp107;});struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp106;_tmp106.tag=0U,({
struct _fat_ptr _tmp138=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs2->hd,& Cyc_Tcutil_bk)));_tmp106.f1=_tmp138;});_tmp106;});void*_tmpC3[3U];_tmpC3[0]=& _tmpC5,_tmpC3[1]=& _tmpC6,_tmpC3[2]=& _tmpC7;({struct _fat_ptr _tmp139=({const char*_tmpC4="(type var %s has different kinds %s and %s)";_tag_fat(_tmpC4,sizeof(char),44U);});Cyc_aprintf(_tmp139,_tag_fat(_tmpC3,sizeof(void*),3U));});}));
inst=({struct Cyc_List_List*_tmpC9=_region_malloc(rgn,sizeof(*_tmpC9));({struct _tuple13*_tmp13B=({struct _tuple13*_tmpC8=_region_malloc(rgn,sizeof(*_tmpC8));_tmpC8->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp13A=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmpC8->f2=_tmp13A;});_tmpC8;});_tmpC9->hd=_tmp13B;}),_tmpC9->tl=inst;_tmpC9;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}}
# 414
if(tvs2 != 0)
Cyc_Unify_fail_because(({const char*_tmpCA="(second function type has too many type variables)";_tag_fat(_tmpCA,sizeof(char),51U);}));
if(inst != 0){
({void*_tmp13E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=5U,(_tmpCB->f1).tvars=0,(_tmpCB->f1).effect=eff1,(_tmpCB->f1).ret_tqual=rt_tq1,(_tmpCB->f1).ret_type=rt1,(_tmpCB->f1).args=args1,(_tmpCB->f1).c_varargs=c_vararg1,(_tmpCB->f1).cyc_varargs=cyc_vararg1,(_tmpCB->f1).rgn_po=rpo1,(_tmpCB->f1).attributes=atts1,(_tmpCB->f1).requires_clause=req1,(_tmpCB->f1).requires_relns=req_relns1,(_tmpCB->f1).ensures_clause=ens1,(_tmpCB->f1).ensures_relns=ens_relns1;_tmpCB;});Cyc_Unify_unify_it(_tmp13E,({
# 420
struct _RegionHandle*_tmp13D=rgn;struct Cyc_List_List*_tmp13C=inst;Cyc_Tcutil_rsubstitute(_tmp13D,_tmp13C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));
_tmpCC->tag=5U,(_tmpCC->f1).tvars=0,(_tmpCC->f1).effect=eff2,(_tmpCC->f1).ret_tqual=rt_tq2,(_tmpCC->f1).ret_type=rt2,(_tmpCC->f1).args=args2,(_tmpCC->f1).c_varargs=c_vararg2,(_tmpCC->f1).cyc_varargs=cyc_vararg2,(_tmpCC->f1).rgn_po=rpo2,(_tmpCC->f1).attributes=atts2,(_tmpCC->f1).requires_clause=req2,(_tmpCC->f1).requires_relns=req_relns2,(_tmpCC->f1).ensures_clause=ens2,(_tmpCC->f1).ensures_relns=ens_relns2;_tmpCC;}));}));});
# 425
_npop_handler(0U);return;}}
# 399
;_pop_region(rgn);}
# 428
Cyc_Unify_unify_it(rt1,rt2);
Cyc_Unify_unify_tqual(rt_tq1,rt1,rt_tq2,rt2);
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple8 _tmpCD=*((struct _tuple8*)args1->hd);struct _tuple8 _stmttmp4=_tmpCD;struct _tuple8 _tmpCE=_stmttmp4;void*_tmpD0;struct Cyc_Absyn_Tqual _tmpCF;_LL3A: _tmpCF=_tmpCE.f2;_tmpD0=_tmpCE.f3;_LL3B: {struct Cyc_Absyn_Tqual tqa=_tmpCF;void*ta=_tmpD0;
struct _tuple8 _tmpD1=*((struct _tuple8*)args2->hd);struct _tuple8 _stmttmp5=_tmpD1;struct _tuple8 _tmpD2=_stmttmp5;void*_tmpD4;struct Cyc_Absyn_Tqual _tmpD3;_LL3D: _tmpD3=_tmpD2.f2;_tmpD4=_tmpD2.f3;_LL3E: {struct Cyc_Absyn_Tqual tqb=_tmpD3;void*tb=_tmpD4;
Cyc_Unify_unify_it(ta,tb);
Cyc_Unify_unify_tqual(tqa,ta,tqb,tb);}}}
# 436
Cyc_Unify_ts_failure=({struct _tuple10 _tmp109;_tmp109.f1=t1,_tmp109.f2=t2;_tmp109;});
if(args1 != 0 || args2 != 0)
Cyc_Unify_fail_because(({const char*_tmpD5="(function types have different number of arguments)";_tag_fat(_tmpD5,sizeof(char),52U);}));
if(c_vararg1 != c_vararg2)
Cyc_Unify_fail_because(({const char*_tmpD6="(only one function type takes C varargs)";_tag_fat(_tmpD6,sizeof(char),41U);}));
# 442
{struct _tuple14 _tmpD7=({struct _tuple14 _tmp10A;_tmp10A.f1=cyc_vararg1,_tmp10A.f2=cyc_vararg2;_tmp10A;});struct _tuple14 _stmttmp6=_tmpD7;struct _tuple14 _tmpD8=_stmttmp6;int _tmpE0;void*_tmpDF;struct Cyc_Absyn_Tqual _tmpDE;struct _fat_ptr*_tmpDD;int _tmpDC;void*_tmpDB;struct Cyc_Absyn_Tqual _tmpDA;struct _fat_ptr*_tmpD9;if(_tmpD8.f1 == 0){if(_tmpD8.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmpD8.f2 == 0){_LL44: _LL45:
 Cyc_Unify_fail_because(({const char*_tmpE1="(only one function type takes varargs)";_tag_fat(_tmpE1,sizeof(char),39U);}));}else{_LL46: _tmpD9=(_tmpD8.f1)->name;_tmpDA=(_tmpD8.f1)->tq;_tmpDB=(_tmpD8.f1)->type;_tmpDC=(_tmpD8.f1)->inject;_tmpDD=(_tmpD8.f2)->name;_tmpDE=(_tmpD8.f2)->tq;_tmpDF=(_tmpD8.f2)->type;_tmpE0=(_tmpD8.f2)->inject;_LL47: {struct _fat_ptr*n1=_tmpD9;struct Cyc_Absyn_Tqual tq1=_tmpDA;void*tp1=_tmpDB;int i1=_tmpDC;struct _fat_ptr*n2=_tmpDD;struct Cyc_Absyn_Tqual tq2=_tmpDE;void*tp2=_tmpDF;int i2=_tmpE0;
# 447
Cyc_Unify_unify_it(tp1,tp2);
Cyc_Unify_unify_tqual(tq1,tp1,tq2,tp2);
if(i1 != i2)
Cyc_Unify_fail_because(({const char*_tmpE2="(only one function type injects varargs)";_tag_fat(_tmpE2,sizeof(char),41U);}));}}}_LL3F:;}{
# 454
int bad_effect;
if(eff1 == 0 && eff2 == 0)
bad_effect=0;else{
if(eff1 == 0 || eff2 == 0)
bad_effect=1;else{
# 460
bad_effect=!Cyc_Unify_unify_effect(eff1,eff2);}}
Cyc_Unify_ts_failure=({struct _tuple10 _tmp10B;_tmp10B.f1=t1,_tmp10B.f2=t2;_tmp10B;});
if(bad_effect)
Cyc_Unify_fail_because(({const char*_tmpE3="(function effects do not match)";_tag_fat(_tmpE3,sizeof(char),32U);}));
if(!Cyc_Tcutil_same_atts(atts2,atts1))
Cyc_Unify_fail_because(({const char*_tmpE4="(function types have different attributes)";_tag_fat(_tmpE4,sizeof(char),43U);}));
if(!Cyc_Tcutil_same_rgn_po(rpo2,rpo1))
Cyc_Unify_fail_because(({const char*_tmpE5="(function types have different region lifetime orderings)";_tag_fat(_tmpE5,sizeof(char),58U);}));
if(!Cyc_Unify_check_logical_equivalence(req_relns1,req_relns2))
Cyc_Unify_fail_because(({const char*_tmpE6="(@requires clauses not equivalent)";_tag_fat(_tmpE6,sizeof(char),35U);}));
if(!Cyc_Unify_check_logical_equivalence(ens_relns1,ens_relns2))
Cyc_Unify_fail_because(({const char*_tmpE7="(@ensures clauses not equivalent)";_tag_fat(_tmpE7,sizeof(char),34U);}));
return;}}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp72.f2)->tag == 6U){_LL2C: _tmp7B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmp7C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp72.f2)->f1;_LL2D: {struct Cyc_List_List*ts2=_tmp7B;struct Cyc_List_List*ts1=_tmp7C;
# 475
for(0;ts1 != 0 && ts2 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
struct _tuple12 _tmpE8=*((struct _tuple12*)ts1->hd);struct _tuple12 _stmttmp7=_tmpE8;struct _tuple12 _tmpE9=_stmttmp7;void*_tmpEB;struct Cyc_Absyn_Tqual _tmpEA;_LL49: _tmpEA=_tmpE9.f1;_tmpEB=_tmpE9.f2;_LL4A: {struct Cyc_Absyn_Tqual tqa=_tmpEA;void*ta=_tmpEB;
struct _tuple12 _tmpEC=*((struct _tuple12*)ts2->hd);struct _tuple12 _stmttmp8=_tmpEC;struct _tuple12 _tmpED=_stmttmp8;void*_tmpEF;struct Cyc_Absyn_Tqual _tmpEE;_LL4C: _tmpEE=_tmpED.f1;_tmpEF=_tmpED.f2;_LL4D: {struct Cyc_Absyn_Tqual tqb=_tmpEE;void*tb=_tmpEF;
Cyc_Unify_unify_it(ta,tb);
Cyc_Unify_unify_tqual(tqa,ta,tqb,tb);}}}
# 481
if(ts1 == 0 && ts2 == 0)
return;
Cyc_Unify_ts_failure=({struct _tuple10 _tmp10C;_tmp10C.f1=t1,_tmp10C.f2=t2;_tmp10C;});
Cyc_Unify_fail_because(({const char*_tmpF0="(tuple types have different numbers of components)";_tag_fat(_tmpF0,sizeof(char),51U);}));}}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f2)->tag == 7U){_LL2E: _tmp77=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmp78=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f1)->f2;_tmp79=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f2)->f1;_tmp7A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f2)->f2;_LL2F: {enum Cyc_Absyn_AggrKind k2=_tmp77;struct Cyc_List_List*fs2=_tmp78;enum Cyc_Absyn_AggrKind k1=_tmp79;struct Cyc_List_List*fs1=_tmp7A;
# 487
if((int)k1 != (int)k2)
Cyc_Unify_fail_because(({const char*_tmpF1="(struct and union type)";_tag_fat(_tmpF1,sizeof(char),24U);}));
for(0;fs1 != 0 && fs2 != 0;(fs1=fs1->tl,fs2=fs2->tl)){
struct Cyc_Absyn_Aggrfield*_tmpF2=(struct Cyc_Absyn_Aggrfield*)fs1->hd;struct Cyc_Absyn_Aggrfield*f1=_tmpF2;
struct Cyc_Absyn_Aggrfield*_tmpF3=(struct Cyc_Absyn_Aggrfield*)fs2->hd;struct Cyc_Absyn_Aggrfield*f2=_tmpF3;
if(Cyc_strptrcmp(f1->name,f2->name)!= 0)
Cyc_Unify_fail_because(({const char*_tmpF4="(different member names)";_tag_fat(_tmpF4,sizeof(char),25U);}));
Cyc_Unify_unify_it(f1->type,f2->type);
Cyc_Unify_unify_tqual(f1->tq,f1->type,f2->tq,f2->type);
Cyc_Unify_ts_failure=({struct _tuple10 _tmp10D;_tmp10D.f1=t1,_tmp10D.f2=t2;_tmp10D;});
if(!Cyc_Tcutil_same_atts(f1->attributes,f2->attributes))
Cyc_Unify_fail_because(({const char*_tmpF5="(different attributes on member)";_tag_fat(_tmpF5,sizeof(char),33U);}));
if(!Cyc_Unify_unify_const_exp_opt(f1->width,f2->width))
Cyc_Unify_fail_because(({const char*_tmpF6="(different bitfield widths on member)";_tag_fat(_tmpF6,sizeof(char),38U);}));
if(!Cyc_Unify_unify_const_exp_opt(f1->requires_clause,f2->requires_clause))
Cyc_Unify_fail_because(({const char*_tmpF7="(different @requires clauses on member)";_tag_fat(_tmpF7,sizeof(char),40U);}));}
# 504
if(fs1 != 0 || fs2 != 0)
Cyc_Unify_fail_because(({const char*_tmpF8="(different number of members)";_tag_fat(_tmpF8,sizeof(char),30U);}));
return;}}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f2)->tag == 8U){_LL30: _tmp73=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f1)->f2;_tmp74=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f1)->f3;_tmp75=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f2)->f2;_tmp76=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f2)->f3;_LL31: {struct Cyc_List_List*ts1=_tmp73;struct Cyc_Absyn_Typedefdecl*td1=_tmp74;struct Cyc_List_List*ts2=_tmp75;struct Cyc_Absyn_Typedefdecl*td2=_tmp76;
# 509
if(td1 != td2)
Cyc_Unify_fail_because(({const char*_tmpF9="(different abstract typedefs)";_tag_fat(_tmpF9,sizeof(char),30U);}));
Cyc_Unify_failure_reason=({const char*_tmpFA="(type parameters to typedef differ)";_tag_fat(_tmpFA,sizeof(char),36U);});
Cyc_Unify_unify_list(ts1,ts2);
return;}}else{goto _LL32;}default: _LL32: _LL33:
(int)_throw((void*)& Cyc_Unify_Unify_val);}}}_LL11:;}}

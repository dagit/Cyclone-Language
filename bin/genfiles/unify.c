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
struct Cyc_Absynpp_Params _tmp5=Cyc_Absynpp_tc_params_r;
_tmp5.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp5);}{
# 65
struct _tuple10 _tmp6=Cyc_Unify_ts_failure;void*_tmp23;void*_tmp22;_LL1: _tmp23=_tmp6.f1;_tmp22=_tmp6.f2;_LL2:;{
struct _fat_ptr s1=(unsigned)_tmp23?Cyc_Absynpp_typ2string(_tmp23):({const char*_tmp21="<?>";_tag_fat(_tmp21,sizeof(char),4U);});
struct _fat_ptr s2=(unsigned)_tmp22?Cyc_Absynpp_typ2string(_tmp22):({const char*_tmp20="<?>";_tag_fat(_tmp20,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmpFC;_tmpFC.tag=0U,_tmpFC.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmpFC;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({struct Cyc___cycFILE*_tmp114=Cyc_stderr;struct _fat_ptr _tmp113=({const char*_tmp8="  %s";_tag_fat(_tmp8,sizeof(char),5U);});Cyc_fprintf(_tmp114,_tmp113,_tag_fat(_tmp7,sizeof(void*),1U));});});
pos +=_get_fat_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpA=0U;({struct Cyc___cycFILE*_tmp116=Cyc_stderr;struct _fat_ptr _tmp115=({const char*_tmpB="\n\t";_tag_fat(_tmpB,sizeof(char),3U);});Cyc_fprintf(_tmp116,_tmp115,_tag_fat(_tmpA,sizeof(void*),0U));});});
pos=8;}else{
# 75
({void*_tmpC=0U;({struct Cyc___cycFILE*_tmp118=Cyc_stderr;struct _fat_ptr _tmp117=({const char*_tmpD=" ";_tag_fat(_tmpD,sizeof(char),2U);});Cyc_fprintf(_tmp118,_tmp117,_tag_fat(_tmpC,sizeof(void*),0U));});});
++ pos;}
# 78
({void*_tmpE=0U;({struct Cyc___cycFILE*_tmp11A=Cyc_stderr;struct _fat_ptr _tmp119=({const char*_tmpF="and ";_tag_fat(_tmpF,sizeof(char),5U);});Cyc_fprintf(_tmp11A,_tmp119,_tag_fat(_tmpE,sizeof(void*),0U));});});
pos +=4;
if((unsigned)pos + _get_fat_size(s2,sizeof(char))>= (unsigned)80){
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmp11C=Cyc_stderr;struct _fat_ptr _tmp11B=({const char*_tmp11="\n\t";_tag_fat(_tmp11,sizeof(char),3U);});Cyc_fprintf(_tmp11C,_tmp11B,_tag_fat(_tmp10,sizeof(void*),0U));});});
pos=8;}
# 84
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0U,_tmpFD.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmpFD;});void*_tmp12[1U];_tmp12[0]=& _tmp14;({struct Cyc___cycFILE*_tmp11E=Cyc_stderr;struct _fat_ptr _tmp11D=({const char*_tmp13="%s ";_tag_fat(_tmp13,sizeof(char),4U);});Cyc_fprintf(_tmp11E,_tmp11D,_tag_fat(_tmp12,sizeof(void*),1U));});});
pos +=_get_fat_size(s2,sizeof(char))+ (unsigned)1;
if(pos + 17 >= 80){
({void*_tmp15=0U;({struct Cyc___cycFILE*_tmp120=Cyc_stderr;struct _fat_ptr _tmp11F=({const char*_tmp16="\n\t";_tag_fat(_tmp16,sizeof(char),3U);});Cyc_fprintf(_tmp120,_tmp11F,_tag_fat(_tmp15,sizeof(void*),0U));});});
pos=8;}
# 90
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmp122=Cyc_stderr;struct _fat_ptr _tmp121=({const char*_tmp18="are not compatible. ";_tag_fat(_tmp18,sizeof(char),21U);});Cyc_fprintf(_tmp122,_tmp121,_tag_fat(_tmp17,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmp123=(char*)Cyc_Unify_failure_reason.curr;_tmp123 != (char*)(_tag_fat(0,0,0)).curr;})){
if(({unsigned long _tmp124=(unsigned long)pos;_tmp124 + Cyc_strlen((struct _fat_ptr)Cyc_Unify_failure_reason);})>= (unsigned long)80)
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmp126=Cyc_stderr;struct _fat_ptr _tmp125=({const char*_tmp1A="\n\t";_tag_fat(_tmp1A,sizeof(char),3U);});Cyc_fprintf(_tmp126,_tmp125,_tag_fat(_tmp19,sizeof(void*),0U));});});
# 96
({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0U,_tmpFE.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Unify_failure_reason);_tmpFE;});void*_tmp1B[1U];_tmp1B[0]=& _tmp1D;({struct Cyc___cycFILE*_tmp128=Cyc_stderr;struct _fat_ptr _tmp127=({const char*_tmp1C="%s";_tag_fat(_tmp1C,sizeof(char),3U);});Cyc_fprintf(_tmp128,_tmp127,_tag_fat(_tmp1B,sizeof(void*),1U));});});}
# 98
({void*_tmp1E=0U;({struct Cyc___cycFILE*_tmp12A=Cyc_stderr;struct _fat_ptr _tmp129=({const char*_tmp1F="\n";_tag_fat(_tmp1F,sizeof(char),2U);});Cyc_fprintf(_tmp12A,_tmp129,_tag_fat(_tmp1E,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};};}
# 104
static int Cyc_Unify_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Relations_check_logical_implication(r1,r2)&&
 Cyc_Relations_check_logical_implication(r2,r1);}
# 110
int Cyc_Unify_unify_kindbound(void*kb1,void*kb2){
struct _tuple10 _tmp24=({struct _tuple10 _tmpFF;({void*_tmp12C=Cyc_Absyn_compress_kb(kb1);_tmpFF.f1=_tmp12C;}),({void*_tmp12B=Cyc_Absyn_compress_kb(kb2);_tmpFF.f2=_tmp12B;});_tmpFF;});struct _tuple10 _tmp25=_tmp24;struct Cyc_Core_Opt**_tmp3A;void*_tmp39;void*_tmp38;struct Cyc_Core_Opt**_tmp37;struct Cyc_Core_Opt**_tmp36;struct Cyc_Absyn_Kind*_tmp35;struct Cyc_Core_Opt**_tmp34;struct Cyc_Absyn_Kind*_tmp33;struct Cyc_Core_Opt**_tmp32;struct Cyc_Absyn_Kind*_tmp31;struct Cyc_Absyn_Kind*_tmp30;struct Cyc_Absyn_Kind*_tmp2F;struct Cyc_Core_Opt**_tmp2E;struct Cyc_Absyn_Kind*_tmp2D;struct Cyc_Absyn_Kind*_tmp2C;struct Cyc_Absyn_Kind*_tmp2B;switch(*((int*)_tmp25.f1)){case 0U: switch(*((int*)_tmp25.f2)){case 0U: _LL1: _tmp2C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp2B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_LL2:
 return _tmp2C == _tmp2B;case 2U: _LL5: _tmp2F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp2E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_tmp2D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f2;_LL6:
# 119
 if(Cyc_Tcutil_kind_leq(_tmp2F,_tmp2D)){
({struct Cyc_Core_Opt*_tmp12D=({struct Cyc_Core_Opt*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->v=kb1;_tmp27;});*_tmp2E=_tmp12D;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp25.f2)){case 0U: _LL3: _tmp32=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp31=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f2;_tmp30=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_LL4:
# 114
 if(Cyc_Tcutil_kind_leq(_tmp30,_tmp31)){
({struct Cyc_Core_Opt*_tmp12E=({struct Cyc_Core_Opt*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->v=kb2;_tmp26;});*_tmp32=_tmp12E;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp36=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp35=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f1)->f2;_tmp34=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_tmp33=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25.f2)->f2;_LL8:
# 124
 if(Cyc_Tcutil_kind_leq(_tmp35,_tmp33)){
({struct Cyc_Core_Opt*_tmp12F=({struct Cyc_Core_Opt*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->v=kb1;_tmp28;});*_tmp34=_tmp12F;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp33,_tmp35)){
({struct Cyc_Core_Opt*_tmp130=({struct Cyc_Core_Opt*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->v=kb2;_tmp29;});*_tmp36=_tmp130;});
return 1;}else{
return 0;}}default: _LLB: _tmp38=_tmp25.f1;_tmp37=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25.f2)->f1;_LLC:
# 133
({struct Cyc_Core_Opt*_tmp131=({struct Cyc_Core_Opt*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->v=_tmp38;_tmp2A;});*_tmp37=_tmp131;});
return 1;}default: _LL9: _tmp3A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25.f1)->f1;_tmp39=_tmp25.f2;_LLA:
# 131
 _tmp38=_tmp39;_tmp37=_tmp3A;goto _LLC;}_LL0:;}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 140
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp3B=t;struct Cyc_List_List*_tmp55;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp53;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp51;void*_tmp50;struct Cyc_Absyn_Tqual _tmp4F;void*_tmp4E;struct Cyc_List_List*_tmp4D;struct Cyc_Absyn_VarargInfo*_tmp4C;struct Cyc_List_List*_tmp4B;void*_tmp4A;void*_tmp49;struct Cyc_Absyn_PtrInfo _tmp48;void*_tmp47;struct Cyc_Core_Opt**_tmp46;struct Cyc_Absyn_Tvar*_tmp45;switch(*((int*)_tmp3B)){case 2U: _LL1: _tmp45=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B)->f1;_LL2:
# 144
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp45))
Cyc_Unify_fail_because(({const char*_tmp3C="(type variable would escape scope)";_tag_fat(_tmp3C,sizeof(char),35U);}));
goto _LL0;case 1U: _LL3: _tmp47=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B)->f2;_tmp46=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B)->f4;_LL4:
# 148
 if(t == evar)
Cyc_Unify_fail_because(({const char*_tmp3D="(occurs check)";_tag_fat(_tmp3D,sizeof(char),15U);}));else{
if(_tmp47 != 0)Cyc_Unify_occurs(evar,r,env,_tmp47);else{
# 153
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp46))->v;for(0;s != 0 && !problem;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
problem=1;}}
# 158
if(problem){
struct Cyc_List_List*_tmp3E=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp46))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp3E=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp3F->tl=_tmp3E;_tmp3F;});}}
({struct Cyc_Core_Opt*_tmp132=({struct Cyc_Core_Opt*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->v=_tmp3E;_tmp40;});*_tmp46=_tmp132;});}}}
# 166
goto _LL0;case 3U: _LL5: _tmp48=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->f1;_LL6:
# 168
 Cyc_Unify_occurs(evar,r,env,_tmp48.elt_type);
Cyc_Unify_occurs(evar,r,env,(_tmp48.ptr_atts).rgn);
Cyc_Unify_occurs(evar,r,env,(_tmp48.ptr_atts).nullable);
Cyc_Unify_occurs(evar,r,env,(_tmp48.ptr_atts).bounds);
Cyc_Unify_occurs(evar,r,env,(_tmp48.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).elt_type;_tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).zero_term;_LL8:
# 176
 Cyc_Unify_occurs(evar,r,env,_tmp4A);
Cyc_Unify_occurs(evar,r,env,_tmp49);
goto _LL0;case 5U: _LL9: _tmp51=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).tvars;_tmp50=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).effect;_tmp4F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).ret_tqual;_tmp4E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).ret_type;_tmp4D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).args;_tmp4C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).cyc_varargs;_tmp4B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3B)->f1).rgn_po;_LLA:
# 181
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp51,env);
if(_tmp50 != 0)Cyc_Unify_occurs(evar,r,env,_tmp50);
Cyc_Unify_occurs(evar,r,env,_tmp4E);
for(0;_tmp4D != 0;_tmp4D=_tmp4D->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple8*)_tmp4D->hd)).f3);}
if(_tmp4C != 0)
Cyc_Unify_occurs(evar,r,env,_tmp4C->type);
for(0;_tmp4B != 0;_tmp4B=_tmp4B->tl){
struct _tuple10*_tmp41=(struct _tuple10*)_tmp4B->hd;struct _tuple10*_tmp42=_tmp41;void*_tmp44;void*_tmp43;_LL16: _tmp44=_tmp42->f1;_tmp43=_tmp42->f2;_LL17:;
Cyc_Unify_occurs(evar,r,env,_tmp44);
Cyc_Unify_occurs(evar,r,env,_tmp43);}
# 193
goto _LL0;case 6U: _LLB: _tmp52=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B)->f1;_LLC:
# 195
 for(0;_tmp52 != 0;_tmp52=_tmp52->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple12*)_tmp52->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp53=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B)->f2;_LLE:
# 200
 for(0;_tmp53 != 0;_tmp53=_tmp53->tl){
Cyc_Unify_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp53->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp54=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3B)->f2;_LL10:
 _tmp55=_tmp54;goto _LL12;case 0U: _LL11: _tmp55=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2;_LL12:
# 205
 for(0;_tmp55 != 0;_tmp55=_tmp55->tl){
Cyc_Unify_occurs(evar,r,env,(void*)_tmp55->hd);}
goto _LL0;default: _LL13: _LL14:
# 210
 goto _LL0;}_LL0:;};}
# 214
static void Cyc_Unify_unify_it(void*t1,void*t2);
# 217
int Cyc_Unify_unify(void*t1,void*t2){
struct _handler_cons _tmp56;_push_handler(& _tmp56);{int _tmp58=0;if(setjmp(_tmp56.handler))_tmp58=1;if(!_tmp58){
Cyc_Unify_unify_it(t1,t2);{
int _tmp59=1;_npop_handler(0U);return _tmp59;};
# 219
;_pop_handler();}else{void*_tmp57=(void*)Cyc_Core_get_exn_thrown();void*_tmp5A=_tmp57;void*_tmp5B;if(((struct Cyc_Unify_Unify_exn_struct*)_tmp5A)->tag == Cyc_Unify_Unify){_LL1: _LL2:
# 221
 return 0;}else{_LL3: _tmp5B=_tmp5A;_LL4:(int)_rethrow(_tmp5B);}_LL0:;}};}
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
{void*_tmp61=t1;struct Cyc_Core_Opt*_tmp70;void**_tmp6F;struct Cyc_Core_Opt*_tmp6E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->tag == 1U){_LL1: _tmp70=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f1;_tmp6F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f2;_tmp6E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61)->f4;_LL2:
# 291
 Cyc_Unify_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6E))->v,t2);{
struct Cyc_Absyn_Kind*_tmp62=Cyc_Tcutil_type_kind(t2);
# 296
if(Cyc_Tcutil_kind_leq(_tmp62,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp70))->v)){
*_tmp6F=t2;
return;}{
# 300
void*_tmp63=t2;struct Cyc_Absyn_PtrInfo _tmp6D;void**_tmp6C;struct Cyc_Core_Opt*_tmp6B;switch(*((int*)_tmp63)){case 1U: _LL6: _tmp6C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp63)->f2;_tmp6B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp63)->f4;_LL7: {
# 303
struct Cyc_List_List*_tmp64=(struct Cyc_List_List*)_tmp6E->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6B))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp64,(struct Cyc_Absyn_Tvar*)s2->hd))
Cyc_Unify_fail_because(({const char*_tmp65="(type variable would escape scope)";_tag_fat(_tmp65,sizeof(char),35U);}));}}
if(!Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp70->v,_tmp62))
Cyc_Unify_fail_because(({const char*_tmp66="(kinds are incompatible)";_tag_fat(_tmp66,sizeof(char),25U);}));
*_tmp6C=t1;
return;}case 3U: _LL8: _tmp6D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63)->f1;if((int)((struct Cyc_Absyn_Kind*)_tmp70->v)->kind == (int)Cyc_Absyn_BoxKind){_LL9: {
# 315
void*_tmp67=Cyc_Tcutil_compress((_tmp6D.ptr_atts).bounds);
void*_tmp68=_tmp67;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68)->tag == 1U){_LLD: _LLE:
# 318
({void*_tmp135=_tmp67;Cyc_Unify_unify(_tmp135,Cyc_Absyn_bounds_one());});
*_tmp6F=t2;
return;}else{_LLF: _LL10:
 Cyc_Unify_fail_because(({const char*_tmp69="(kinds are incompatible)";_tag_fat(_tmp69,sizeof(char),25U);}));}_LLC:;}}else{goto _LLA;}default: _LLA: _LLB:
# 323
 Cyc_Unify_fail_because(({const char*_tmp6A="(kinds are incompatible)";_tag_fat(_tmp6A,sizeof(char),25U);}));}_LL5:;};};}else{_LL3: _LL4:
# 325
 goto _LL0;}_LL0:;}{
# 329
struct _tuple10 _tmp71=({struct _tuple10 _tmp10E;_tmp10E.f1=t2,_tmp10E.f2=t1;_tmp10E;});struct _tuple10 _tmp72=_tmp71;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Typedefdecl*_tmpF9;struct Cyc_List_List*_tmpF8;struct Cyc_Absyn_Typedefdecl*_tmpF7;enum Cyc_Absyn_AggrKind _tmpF6;struct Cyc_List_List*_tmpF5;enum Cyc_Absyn_AggrKind _tmpF4;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF1;struct Cyc_List_List*_tmpF0;void*_tmpEF;struct Cyc_Absyn_Tqual _tmpEE;void*_tmpED;struct Cyc_List_List*_tmpEC;int _tmpEB;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_List_List*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE3;void*_tmpE2;struct Cyc_Absyn_Tqual _tmpE1;void*_tmpE0;struct Cyc_List_List*_tmpDF;int _tmpDE;struct Cyc_Absyn_VarargInfo*_tmpDD;struct Cyc_List_List*_tmpDC;struct Cyc_List_List*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_List_List*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_List_List*_tmpD7;void*_tmpD6;struct Cyc_Absyn_Tqual _tmpD5;struct Cyc_Absyn_Exp*_tmpD4;void*_tmpD3;void*_tmpD2;struct Cyc_Absyn_Tqual _tmpD1;struct Cyc_Absyn_Exp*_tmpD0;void*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCD;void*_tmpCC;struct Cyc_Absyn_Tqual _tmpCB;void*_tmpCA;void*_tmpC9;void*_tmpC8;void*_tmpC7;void*_tmpC6;struct Cyc_Absyn_Tqual _tmpC5;void*_tmpC4;void*_tmpC3;void*_tmpC2;void*_tmpC1;struct Cyc_Absyn_Tvar*_tmpC0;struct Cyc_Absyn_Tvar*_tmpBF;void*_tmpBE;struct Cyc_List_List*_tmpBD;void*_tmpBC;struct Cyc_List_List*_tmpBB;switch(*((int*)_tmp72.f1)){case 1U: _LL12: _LL13:
# 332
 Cyc_Unify_unify_it(t2,t1);return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)->tag == 9U){_LL14: _LL15:
# 334
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmpBE=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpBD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f2;_tmpBC=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f1;_tmpBB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f2)->f2;_LL21:
# 345
 if(Cyc_Tcutil_tycon_cmp(_tmpBE,_tmpBC)!= 0)
Cyc_Unify_fail_because(({const char*_tmp74="(different type constructors)";_tag_fat(_tmp74,sizeof(char),30U);}));
Cyc_Unify_unify_list(_tmpBD,_tmpBB);
return;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72.f1)->f1)){case 8U: _LL18: _LL19:
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
Cyc_Unify_fail_because(({const char*_tmp73="(effects don't unify)";_tag_fat(_tmp73,sizeof(char),22U);}));
return;default: switch(*((int*)_tmp72.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp72.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72.f2)->tag == 2U){_LL22: _tmpC0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpBF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72.f2)->f1;_LL23:
# 352
 if(_tmpC0->identity != _tmpBF->identity)
Cyc_Unify_fail_because(({const char*_tmp75="(variable types are not the same)";_tag_fat(_tmp75,sizeof(char),34U);}));
return;}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->tag == 3U){_LL24: _tmpCC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).elt_type;_tmpCB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).elt_tq;_tmpCA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).rgn;_tmpC9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).nullable;_tmpC8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).bounds;_tmpC7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f1)->f1).ptr_atts).zero_term;_tmpC6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).elt_type;_tmpC5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).elt_tq;_tmpC4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).rgn;_tmpC3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).nullable;_tmpC2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).bounds;_tmpC1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72.f2)->f1).ptr_atts).zero_term;_LL25:
# 358
 Cyc_Unify_unify_it(_tmpC6,_tmpCC);
Cyc_Unify_unify_it(_tmpCA,_tmpC4);{
struct _fat_ptr _tmp76=Cyc_Unify_failure_reason;
if(!Cyc_Unify_unify(_tmpC1,_tmpC7)){
Cyc_Unify_ts_failure=({struct _tuple10 _tmp104;_tmp104.f1=t1,_tmp104.f2=t2;_tmp104;});
Cyc_Unify_fail_because(({const char*_tmp77="(not both zero terminated)";_tag_fat(_tmp77,sizeof(char),27U);}));}
# 365
Cyc_Unify_unify_tqual(_tmpC5,_tmpC6,_tmpCB,_tmpCC);
if(!Cyc_Unify_unify(_tmpC2,_tmpC8)){
Cyc_Unify_ts_failure=({struct _tuple10 _tmp105;_tmp105.f1=t1,_tmp105.f2=t2;_tmp105;});
Cyc_Unify_fail_because(({const char*_tmp78="(different pointer bounds)";_tag_fat(_tmp78,sizeof(char),27U);}));}{
# 371
void*_tmp79=Cyc_Tcutil_compress(_tmpC2);void*_tmp7A=_tmp79;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7A)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7A)->f1)->tag == 14U){_LL35: _LL36:
# 373
 Cyc_Unify_failure_reason=_tmp76;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 376
 Cyc_Unify_failure_reason=({const char*_tmp7B="(incompatible pointer types)";_tag_fat(_tmp7B,sizeof(char),29U);});
Cyc_Unify_unify_it(_tmpC3,_tmpC9);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp72.f2)->tag == 9U){_LL26: _tmpCE=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpCD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp72.f2)->f1;_LL27:
# 382
 if(!Cyc_Evexp_same_const_exp(_tmpCE,_tmpCD))
Cyc_Unify_fail_because(({const char*_tmp7C="(cannot prove expressions are the same)";_tag_fat(_tmp7C,sizeof(char),40U);}));
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->tag == 4U){_LL28: _tmpD6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).elt_type;_tmpD5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).tq;_tmpD4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).num_elts;_tmpD3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f1)->f1).zero_term;_tmpD2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).elt_type;_tmpD1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).tq;_tmpD0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).num_elts;_tmpCF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp72.f2)->f1).zero_term;_LL29:
# 388
 Cyc_Unify_unify_it(_tmpD2,_tmpD6);
Cyc_Unify_unify_tqual(_tmpD1,_tmpD2,_tmpD5,_tmpD6);
Cyc_Unify_failure_reason=({const char*_tmp7D="(not both zero terminated)";_tag_fat(_tmp7D,sizeof(char),27U);});
Cyc_Unify_unify_it(_tmpD3,_tmpCF);
if(!Cyc_Unify_unify_const_exp_opt(_tmpD4,_tmpD0))
Cyc_Unify_fail_because(({const char*_tmp7E="(different array sizes)";_tag_fat(_tmp7E,sizeof(char),24U);}));
return;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->tag == 5U){_LL2A: _tmpF0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).tvars;_tmpEF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).effect;_tmpEE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ret_tqual;_tmpED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ret_type;_tmpEC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).args;_tmpEB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).c_varargs;_tmpEA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).cyc_varargs;_tmpE9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).rgn_po;_tmpE8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).attributes;_tmpE7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).requires_clause;_tmpE6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).requires_relns;_tmpE5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ensures_clause;_tmpE4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f1)->f1).ensures_relns;_tmpE3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).tvars;_tmpE2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).effect;_tmpE1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ret_tqual;_tmpE0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ret_type;_tmpDF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).args;_tmpDE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).c_varargs;_tmpDD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).cyc_varargs;_tmpDC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).rgn_po;_tmpDB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).attributes;_tmpDA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).requires_clause;_tmpD9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).requires_relns;_tmpD8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ensures_clause;_tmpD7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72.f2)->f1).ensures_relns;_LL2B:
# 398
{struct _RegionHandle _tmp7F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7F;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmpE3 != 0){
if(_tmpF0 == 0)
Cyc_Unify_fail_because(({const char*_tmp80="(second function type has too few type variables)";_tag_fat(_tmp80,sizeof(char),50U);}));{
void*_tmp81=((struct Cyc_Absyn_Tvar*)_tmpE3->hd)->kind;
void*_tmp82=((struct Cyc_Absyn_Tvar*)_tmpF0->hd)->kind;
if(!Cyc_Unify_unify_kindbound(_tmp81,_tmp82))
Cyc_Unify_fail_because((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp85=({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0U,({
struct _fat_ptr _tmp136=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmpE3->hd));_tmp108.f1=_tmp136;});_tmp108;});struct Cyc_String_pa_PrintArg_struct _tmp86=({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0U,({
struct _fat_ptr _tmp137=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmpE3->hd,& Cyc_Tcutil_bk)));_tmp107.f1=_tmp137;});_tmp107;});struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp106;_tmp106.tag=0U,({
struct _fat_ptr _tmp138=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmpF0->hd,& Cyc_Tcutil_bk)));_tmp106.f1=_tmp138;});_tmp106;});void*_tmp83[3U];_tmp83[0]=& _tmp85,_tmp83[1]=& _tmp86,_tmp83[2]=& _tmp87;({struct _fat_ptr _tmp139=({const char*_tmp84="(type var %s has different kinds %s and %s)";_tag_fat(_tmp84,sizeof(char),44U);});Cyc_aprintf(_tmp139,_tag_fat(_tmp83,sizeof(void*),3U));});}));
inst=({struct Cyc_List_List*_tmp89=_region_malloc(rgn,sizeof(*_tmp89));({struct _tuple13*_tmp13B=({struct _tuple13*_tmp88=_region_malloc(rgn,sizeof(*_tmp88));_tmp88->f1=(struct Cyc_Absyn_Tvar*)_tmpF0->hd,({void*_tmp13A=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmpE3->hd);_tmp88->f2=_tmp13A;});_tmp88;});_tmp89->hd=_tmp13B;}),_tmp89->tl=inst;_tmp89;});
_tmpE3=_tmpE3->tl;
_tmpF0=_tmpF0->tl;};}
# 414
if(_tmpF0 != 0)
Cyc_Unify_fail_because(({const char*_tmp8A="(second function type has too many type variables)";_tag_fat(_tmp8A,sizeof(char),51U);}));
if(inst != 0){
({void*_tmp13E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=5U,(_tmp8B->f1).tvars=0,(_tmp8B->f1).effect=_tmpE2,(_tmp8B->f1).ret_tqual=_tmpE1,(_tmp8B->f1).ret_type=_tmpE0,(_tmp8B->f1).args=_tmpDF,(_tmp8B->f1).c_varargs=_tmpDE,(_tmp8B->f1).cyc_varargs=_tmpDD,(_tmp8B->f1).rgn_po=_tmpDC,(_tmp8B->f1).attributes=_tmpDB,(_tmp8B->f1).requires_clause=_tmpE7,(_tmp8B->f1).requires_relns=_tmpE6,(_tmp8B->f1).ensures_clause=_tmpE5,(_tmp8B->f1).ensures_relns=_tmpE4;_tmp8B;});Cyc_Unify_unify_it(_tmp13E,({
# 420
struct _RegionHandle*_tmp13D=rgn;struct Cyc_List_List*_tmp13C=inst;Cyc_Tcutil_rsubstitute(_tmp13D,_tmp13C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));
_tmp8C->tag=5U,(_tmp8C->f1).tvars=0,(_tmp8C->f1).effect=_tmpEF,(_tmp8C->f1).ret_tqual=_tmpEE,(_tmp8C->f1).ret_type=_tmpED,(_tmp8C->f1).args=_tmpEC,(_tmp8C->f1).c_varargs=_tmpEB,(_tmp8C->f1).cyc_varargs=_tmpEA,(_tmp8C->f1).rgn_po=_tmpE9,(_tmp8C->f1).attributes=_tmpE8,(_tmp8C->f1).requires_clause=_tmpDA,(_tmp8C->f1).requires_relns=_tmpD9,(_tmp8C->f1).ensures_clause=_tmpD8,(_tmp8C->f1).ensures_relns=_tmpD7;_tmp8C;}));}));});
# 425
_npop_handler(0U);return;}}
# 399
;_pop_region(rgn);}
# 428
Cyc_Unify_unify_it(_tmpE0,_tmpED);
Cyc_Unify_unify_tqual(_tmpE1,_tmpE0,_tmpEE,_tmpED);
for(0;_tmpDF != 0 && _tmpEC != 0;(_tmpDF=_tmpDF->tl,_tmpEC=_tmpEC->tl)){
struct _tuple8 _tmp8D=*((struct _tuple8*)_tmpDF->hd);struct _tuple8 _tmp8E=_tmp8D;struct Cyc_Absyn_Tqual _tmp94;void*_tmp93;_LL3A: _tmp94=_tmp8E.f2;_tmp93=_tmp8E.f3;_LL3B:;{
struct _tuple8 _tmp8F=*((struct _tuple8*)_tmpEC->hd);struct _tuple8 _tmp90=_tmp8F;struct Cyc_Absyn_Tqual _tmp92;void*_tmp91;_LL3D: _tmp92=_tmp90.f2;_tmp91=_tmp90.f3;_LL3E:;
Cyc_Unify_unify_it(_tmp93,_tmp91);
Cyc_Unify_unify_tqual(_tmp94,_tmp93,_tmp92,_tmp91);};}
# 436
Cyc_Unify_ts_failure=({struct _tuple10 _tmp109;_tmp109.f1=t1,_tmp109.f2=t2;_tmp109;});
if(_tmpDF != 0 || _tmpEC != 0)
Cyc_Unify_fail_because(({const char*_tmp95="(function types have different number of arguments)";_tag_fat(_tmp95,sizeof(char),52U);}));
if(_tmpDE != _tmpEB)
Cyc_Unify_fail_because(({const char*_tmp96="(only one function type takes C varargs)";_tag_fat(_tmp96,sizeof(char),41U);}));
# 442
{struct _tuple14 _tmp97=({struct _tuple14 _tmp10A;_tmp10A.f1=_tmpDD,_tmp10A.f2=_tmpEA;_tmp10A;});struct _tuple14 _tmp98=_tmp97;struct _fat_ptr*_tmpA2;struct Cyc_Absyn_Tqual _tmpA1;void*_tmpA0;int _tmp9F;struct _fat_ptr*_tmp9E;struct Cyc_Absyn_Tqual _tmp9D;void*_tmp9C;int _tmp9B;if(_tmp98.f1 == 0){if(_tmp98.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmp98.f2 == 0){_LL44: _LL45:
 Cyc_Unify_fail_because(({const char*_tmp99="(only one function type takes varargs)";_tag_fat(_tmp99,sizeof(char),39U);}));}else{_LL46: _tmpA2=(_tmp98.f1)->name;_tmpA1=(_tmp98.f1)->tq;_tmpA0=(_tmp98.f1)->type;_tmp9F=(_tmp98.f1)->inject;_tmp9E=(_tmp98.f2)->name;_tmp9D=(_tmp98.f2)->tq;_tmp9C=(_tmp98.f2)->type;_tmp9B=(_tmp98.f2)->inject;_LL47:
# 447
 Cyc_Unify_unify_it(_tmpA0,_tmp9C);
Cyc_Unify_unify_tqual(_tmpA1,_tmpA0,_tmp9D,_tmp9C);
if(_tmp9F != _tmp9B)
Cyc_Unify_fail_because(({const char*_tmp9A="(only one function type injects varargs)";_tag_fat(_tmp9A,sizeof(char),41U);}));}}_LL3F:;}{
# 454
int bad_effect;
if(_tmpE2 == 0 && _tmpEF == 0)
bad_effect=0;else{
if(_tmpE2 == 0 || _tmpEF == 0)
bad_effect=1;else{
# 460
bad_effect=!Cyc_Unify_unify_effect(_tmpE2,_tmpEF);}}
Cyc_Unify_ts_failure=({struct _tuple10 _tmp10B;_tmp10B.f1=t1,_tmp10B.f2=t2;_tmp10B;});
if(bad_effect)
Cyc_Unify_fail_because(({const char*_tmpA3="(function effects do not match)";_tag_fat(_tmpA3,sizeof(char),32U);}));
if(!Cyc_Tcutil_same_atts(_tmpE8,_tmpDB))
Cyc_Unify_fail_because(({const char*_tmpA4="(function types have different attributes)";_tag_fat(_tmpA4,sizeof(char),43U);}));
if(!Cyc_Tcutil_same_rgn_po(_tmpE9,_tmpDC))
Cyc_Unify_fail_because(({const char*_tmpA5="(function types have different region lifetime orderings)";_tag_fat(_tmpA5,sizeof(char),58U);}));
if(!Cyc_Unify_check_logical_equivalence(_tmpE6,_tmpD9))
Cyc_Unify_fail_because(({const char*_tmpA6="(@requires clauses not equivalent)";_tag_fat(_tmpA6,sizeof(char),35U);}));
if(!Cyc_Unify_check_logical_equivalence(_tmpE4,_tmpD7))
Cyc_Unify_fail_because(({const char*_tmpA7="(@ensures clauses not equivalent)";_tag_fat(_tmpA7,sizeof(char),34U);}));
return;};}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp72.f2)->tag == 6U){_LL2C: _tmpF2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpF1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp72.f2)->f1;_LL2D:
# 475
 for(0;_tmpF1 != 0 && _tmpF2 != 0;(_tmpF1=_tmpF1->tl,_tmpF2=_tmpF2->tl)){
struct _tuple12 _tmpA8=*((struct _tuple12*)_tmpF1->hd);struct _tuple12 _tmpA9=_tmpA8;struct Cyc_Absyn_Tqual _tmpAF;void*_tmpAE;_LL49: _tmpAF=_tmpA9.f1;_tmpAE=_tmpA9.f2;_LL4A:;{
struct _tuple12 _tmpAA=*((struct _tuple12*)_tmpF2->hd);struct _tuple12 _tmpAB=_tmpAA;struct Cyc_Absyn_Tqual _tmpAD;void*_tmpAC;_LL4C: _tmpAD=_tmpAB.f1;_tmpAC=_tmpAB.f2;_LL4D:;
Cyc_Unify_unify_it(_tmpAE,_tmpAC);
Cyc_Unify_unify_tqual(_tmpAF,_tmpAE,_tmpAD,_tmpAC);};}
# 481
if(_tmpF1 == 0 && _tmpF2 == 0)
return;
Cyc_Unify_ts_failure=({struct _tuple10 _tmp10C;_tmp10C.f1=t1,_tmp10C.f2=t2;_tmp10C;});
Cyc_Unify_fail_because(({const char*_tmpB0="(tuple types have different numbers of components)";_tag_fat(_tmpB0,sizeof(char),51U);}));}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f2)->tag == 7U){_LL2E: _tmpF6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f1)->f1;_tmpF5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f1)->f2;_tmpF4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f2)->f1;_tmpF3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp72.f2)->f2;_LL2F:
# 487
 if((int)_tmpF4 != (int)_tmpF6)
Cyc_Unify_fail_because(({const char*_tmpB1="(struct and union type)";_tag_fat(_tmpB1,sizeof(char),24U);}));
for(0;_tmpF3 != 0 && _tmpF5 != 0;(_tmpF3=_tmpF3->tl,_tmpF5=_tmpF5->tl)){
struct Cyc_Absyn_Aggrfield*_tmpB2=(struct Cyc_Absyn_Aggrfield*)_tmpF3->hd;
struct Cyc_Absyn_Aggrfield*_tmpB3=(struct Cyc_Absyn_Aggrfield*)_tmpF5->hd;
if(Cyc_strptrcmp(_tmpB2->name,_tmpB3->name)!= 0)
Cyc_Unify_fail_because(({const char*_tmpB4="(different member names)";_tag_fat(_tmpB4,sizeof(char),25U);}));
Cyc_Unify_unify_it(_tmpB2->type,_tmpB3->type);
Cyc_Unify_unify_tqual(_tmpB2->tq,_tmpB2->type,_tmpB3->tq,_tmpB3->type);
Cyc_Unify_ts_failure=({struct _tuple10 _tmp10D;_tmp10D.f1=t1,_tmp10D.f2=t2;_tmp10D;});
if(!Cyc_Tcutil_same_atts(_tmpB2->attributes,_tmpB3->attributes))
Cyc_Unify_fail_because(({const char*_tmpB5="(different attributes on member)";_tag_fat(_tmpB5,sizeof(char),33U);}));
if(!Cyc_Unify_unify_const_exp_opt(_tmpB2->width,_tmpB3->width))
Cyc_Unify_fail_because(({const char*_tmpB6="(different bitfield widths on member)";_tag_fat(_tmpB6,sizeof(char),38U);}));
if(!Cyc_Unify_unify_const_exp_opt(_tmpB2->requires_clause,_tmpB3->requires_clause))
Cyc_Unify_fail_because(({const char*_tmpB7="(different @requires clauses on member)";_tag_fat(_tmpB7,sizeof(char),40U);}));}
# 504
if(_tmpF3 != 0 || _tmpF5 != 0)
Cyc_Unify_fail_because(({const char*_tmpB8="(different number of members)";_tag_fat(_tmpB8,sizeof(char),30U);}));
return;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f2)->tag == 8U){_LL30: _tmpFA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f1)->f2;_tmpF9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f1)->f3;_tmpF8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f2)->f2;_tmpF7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp72.f2)->f3;_LL31:
# 509
 if(_tmpF9 != _tmpF7)
Cyc_Unify_fail_because(({const char*_tmpB9="(different abstract typedefs)";_tag_fat(_tmpB9,sizeof(char),30U);}));
Cyc_Unify_failure_reason=({const char*_tmpBA="(type parameters to typedef differ)";_tag_fat(_tmpBA,sizeof(char),36U);});
Cyc_Unify_unify_list(_tmpFA,_tmpF8);
return;}else{goto _LL32;}default: _LL32: _LL33:
(int)_throw((void*)& Cyc_Unify_Unify_val);}}}_LL11:;};}

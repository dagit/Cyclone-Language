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
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _fat_ptr src,int src_offset,int max_count);
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 448
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 456
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 691 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 883
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 888
void*Cyc_Absyn_compress_kb(void*);
# 894
int Cyc_Absyn_type2bool(int def,void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1127
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1137
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1145
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 61
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 78
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{int f1;struct Cyc_List_List*f2;};
# 93 "absynpp.h"
struct _tuple12 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 107 "tcutil.h"
void*Cyc_Tcutil_compress(void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple14{struct _fat_ptr f1;unsigned f2;};
# 27 "absyndump.cyc"
struct _tuple14 Cyc_Lex_xlate_pos(unsigned);
# 37
static int Cyc_Absyndump_expand_typedefs;
# 41
static int Cyc_Absyndump_qvar_to_Cids;
# 44
static int Cyc_Absyndump_add_cyc_prefix;
# 47
static int Cyc_Absyndump_generate_line_directives;
# 51
static int Cyc_Absyndump_to_VC;
# 53
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 68 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 71
static void Cyc_Absyndump_dumptyp(void*);
static void Cyc_Absyndump_dumpntyp(void*t);
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt,struct Cyc_List_List*);
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned,int do_semi);
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*,void(*f)(void*),void*a);
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 90
static void Cyc_Absyndump_dumploc(unsigned);
# 93
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
static char Cyc_Absyndump_prev_char='x';
# 96
static int Cyc_Absyndump_need_space_before(){
char _tmp0=Cyc_Absyndump_prev_char;switch(_tmp0){case 123U: _LL1: _LL2:
 goto _LL4;case 125U: _LL3: _LL4:
 goto _LL6;case 40U: _LL5: _LL6:
 goto _LL8;case 41U: _LL7: _LL8:
 goto _LLA;case 91U: _LL9: _LLA:
 goto _LLC;case 93U: _LLB: _LLC:
 goto _LLE;case 59U: _LLD: _LLE:
 goto _LL10;case 44U: _LLF: _LL10:
 goto _LL12;case 61U: _LL11: _LL12:
 goto _LL14;case 63U: _LL13: _LL14:
 goto _LL16;case 33U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 10U: _LL19: _LL1A:
 goto _LL1C;case 42U: _LL1B: _LL1C:
 return 0;default: _LL1D: _LL1E:
 return 1;}_LL0:;}
# 116
static void Cyc_Absyndump_dump(struct _fat_ptr s){
unsigned sz=Cyc_strlen((struct _fat_ptr)s);
# 123
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= (unsigned)1){
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),(int)(sz - (unsigned)1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 131
static void Cyc_Absyndump_dump_nospace(struct _fat_ptr s){
int sz=(int)Cyc_strlen((struct _fat_ptr)s);
# 134
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),sz - 1));}}
# 139
static void Cyc_Absyndump_dump_char(int c){
# 141
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpE[1U]="";
# 145
static void Cyc_Absyndump_dumploc(unsigned loc){
static struct _fat_ptr last_file={_tmpE,_tmpE,_tmpE + 1U};
static unsigned last_line=0U;
if(loc == (unsigned)0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 151
struct _tuple14 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple14 _stmttmp0=_tmp1;struct _tuple14 _tmp2=_stmttmp0;unsigned _tmp4;struct _fat_ptr _tmp3;_LL1: _tmp3=_tmp2.f1;_tmp4=_tmp2.f2;_LL2: {struct _fat_ptr f=_tmp3;unsigned d=_tmp4;
if(({char*_tmp2E7=(char*)f.curr;_tmp2E7 == (char*)(_tag_fat(0,0,0)).curr;})||(char*)f.curr == (char*)last_file.curr && d == last_line)return;
if((char*)f.curr == (char*)last_file.curr && d == last_line + (unsigned)1)
({void*_tmp5=0U;({struct Cyc___cycFILE*_tmp2E9=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp2E8=({const char*_tmp6="\n";_tag_fat(_tmp6,sizeof(char),2U);});Cyc_fprintf(_tmp2E9,_tmp2E8,_tag_fat(_tmp5,sizeof(void*),0U));});});else{
if((char*)f.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp9=({struct Cyc_Int_pa_PrintArg_struct _tmp2DA;_tmp2DA.tag=1U,_tmp2DA.f1=(unsigned long)((int)d);_tmp2DA;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({struct Cyc___cycFILE*_tmp2EB=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp2EA=({const char*_tmp8="\n# %d\n";_tag_fat(_tmp8,sizeof(char),7U);});Cyc_fprintf(_tmp2EB,_tmp2EA,_tag_fat(_tmp7,sizeof(void*),1U));});});else{
# 158
({struct Cyc_Int_pa_PrintArg_struct _tmpC=({struct Cyc_Int_pa_PrintArg_struct _tmp2DC;_tmp2DC.tag=1U,_tmp2DC.f1=(unsigned long)((int)d);_tmp2DC;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp2DB;_tmp2DB.tag=0U,_tmp2DB.f1=(struct _fat_ptr)((struct _fat_ptr)f);_tmp2DB;});void*_tmpA[2U];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD;({struct Cyc___cycFILE*_tmp2ED=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp2EC=({const char*_tmpB="\n# %d %s\n";_tag_fat(_tmpB,sizeof(char),10U);});Cyc_fprintf(_tmp2ED,_tmp2EC,_tag_fat(_tmpA,sizeof(void*),2U));});});}}
# 160
last_file=f;
last_line=d;}}}
# 166
static void Cyc_Absyndump_dump_str(struct _fat_ptr*s){
Cyc_Absyndump_dump(*s);}
# 169
static void Cyc_Absyndump_dump_str_nospace(struct _fat_ptr*s){
Cyc_Absyndump_dump_nospace(*s);}
# 173
static void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 177
static void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 184
f(l->hd);}
# 186
static void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 193
f(env,l->hd);}
# 195
static void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 197
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 201
static void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 203
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 207
static void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 209
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 213
static void Cyc_Absyndump_ignore(void*x){;}
# 215
static void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmpF=0;struct Cyc_List_List*nsl=_tmpF;
struct _fat_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp10=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmp10;union Cyc_Absyn_Nmspace _tmp11=_stmttmp1;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp14;switch((_tmp11.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp14=(_tmp11.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp14;
nsl=x;goto _LL0;}case 3U: _LL5: _tmp13=(_tmp11.C_n).val;_LL6: {struct Cyc_List_List*x=_tmp13;
goto _LL0;}default: _LL7: _tmp12=(_tmp11.Abs_n).val;_LL8: {struct Cyc_List_List*x=_tmp12;
# 223
if(Cyc_Absyndump_qvar_to_Cids || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
nsl=x;
goto _LL0;}}_LL0:;}
# 228
if(Cyc_Absyndump_need_space_before())
Cyc_Absyndump_dump_char((int)' ');{
struct _fat_ptr sep=Cyc_Absyndump_qvar_to_Cids?({const char*_tmp15="_";_tag_fat(_tmp15,sizeof(char),2U);}):({const char*_tmp16="::";_tag_fat(_tmp16,sizeof(char),3U);});
if(prefix != 0){
Cyc_Absyndump_dump_nospace(*(*prefix));Cyc_Absyndump_dump_nospace(sep);}
# 234
if(nsl != 0){
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_str_nospace,nsl,sep);Cyc_Absyndump_dump_nospace(sep);}
# 237
Cyc_Absyndump_dump_nospace(*(*v).f2);}}
# 240
static void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp17="restrict";_tag_fat(_tmp17,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp18="volatile";_tag_fat(_tmp18,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp19="const";_tag_fat(_tmp19,sizeof(char),6U);}));}
# 245
static void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1A=sc;switch(_tmp1A){case Cyc_Absyn_Static: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp1B="static";_tag_fat(_tmp1B,sizeof(char),7U);}));return;case Cyc_Absyn_Public: _LL3: _LL4:
 return;case Cyc_Absyn_Extern: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp1C="extern";_tag_fat(_tmp1C,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1D="extern \"C\"";_tag_fat(_tmp1D,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract: _LL9: _LLA:
 Cyc_Absyndump_dump(({const char*_tmp1E="abstract";_tag_fat(_tmp1E,sizeof(char),9U);}));return;case Cyc_Absyn_Register: _LLB: _LLC:
 Cyc_Absyndump_dump(({const char*_tmp1F="register";_tag_fat(_tmp1F,sizeof(char),9U);}));return;default: _LLD: _LLE:
 return;}_LL0:;}
# 256
static void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp20=k;if(_tmp20 == Cyc_Absyn_StructA){_LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp21="struct";_tag_fat(_tmp21,sizeof(char),7U);}));return;}else{_LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp22="union";_tag_fat(_tmp22,sizeof(char),6U);}));return;}_LL0:;}
# 263
static void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
Cyc_Absyndump_dump(Cyc_Absynpp_kind2string(ka));}
# 267
static void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp2F0=ts;struct _fat_ptr _tmp2EF=({const char*_tmp23="<";_tag_fat(_tmp23,sizeof(char),2U);});struct _fat_ptr _tmp2EE=({const char*_tmp24=">";_tag_fat(_tmp24,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp2F0,_tmp2EF,_tmp2EE,({const char*_tmp25=",";_tag_fat(_tmp25,sizeof(char),2U);}));});}
# 270
static void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
if((int)*((const char*)_check_fat_subscript(n,sizeof(char),0))== (int)'#'){
Cyc_Absyndump_dump(({const char*_tmp26="`G";_tag_fat(_tmp26,sizeof(char),3U);}));
{void*_tmp27=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp2=_tmp27;void*_tmp28=_stmttmp2;struct Cyc_Absyn_Kind*_tmp29;struct Cyc_Absyn_Kind*_tmp2A;switch(*((int*)_tmp28)){case 0U: _LL1: _tmp2A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp28)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp2A;
_tmp29=k;goto _LL4;}case 2U: _LL3: _tmp29=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp28)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp29;
Cyc_Absyndump_dumpkind(k);goto _LL0;}default: _LL5: _LL6:
 Cyc_Absyndump_dump_nospace(({const char*_tmp2B="K";_tag_fat(_tmp2B,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 279
Cyc_Absyndump_dump_nospace(_fat_ptr_plus(n,sizeof(char),1));}else{
# 281
Cyc_Absyndump_dump(n);}}
# 283
static void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp2C=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp3=_tmp2C;void*_tmp2D=_stmttmp3;struct Cyc_Absyn_Kind*_tmp2E;switch(*((int*)_tmp2D)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2D)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2D)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL5: _LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _tmp2E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2D)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmp2E;
Cyc_Absyndump_dump(({const char*_tmp2F="::";_tag_fat(_tmp2F,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(k);goto _LL0;}}}_LL0:;}}
# 292
static void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp2F3=tvs;struct _fat_ptr _tmp2F2=({const char*_tmp30="<";_tag_fat(_tmp30,sizeof(char),2U);});struct _fat_ptr _tmp2F1=({const char*_tmp31=">";_tag_fat(_tmp31,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp2F3,_tmp2F2,_tmp2F1,({const char*_tmp32=",";_tag_fat(_tmp32,sizeof(char),2U);}));});}
# 295
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp2F6=tvs;struct _fat_ptr _tmp2F5=({const char*_tmp33="<";_tag_fat(_tmp33,sizeof(char),2U);});struct _fat_ptr _tmp2F4=({const char*_tmp34=">";_tag_fat(_tmp34,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp2F6,_tmp2F5,_tmp2F4,({const char*_tmp35=",";_tag_fat(_tmp35,sizeof(char),2U);}));});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 298
static void Cyc_Absyndump_dumparg(struct _tuple15*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 301
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp2F9=ts;struct _fat_ptr _tmp2F8=({const char*_tmp36="(";_tag_fat(_tmp36,sizeof(char),2U);});struct _fat_ptr _tmp2F7=({const char*_tmp37=")";_tag_fat(_tmp37,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple15*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp2F9,_tmp2F8,_tmp2F7,({const char*_tmp38=",";_tag_fat(_tmp38,sizeof(char),2U);}));});}
# 305
static void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp39=(void*)atts->hd;void*_stmttmp4=_tmp39;void*_tmp3A=_stmttmp4;switch(*((int*)_tmp3A)){case 1U: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp3B="_stdcall";_tag_fat(_tmp3B,sizeof(char),9U);}));return;case 2U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp3C="_cdecl";_tag_fat(_tmp3C,sizeof(char),7U);}));return;case 3U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp3D="_fastcall";_tag_fat(_tmp3D,sizeof(char),10U);}));return;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 315
static void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 317
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp3E=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_stmttmp5=_tmp3E;void*_tmp3F=_stmttmp5;switch(*((int*)_tmp3F)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 325
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp40="__declspec(";_tag_fat(_tmp40,sizeof(char),12U);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp41=(void*)atts->hd;void*_stmttmp6=_tmp41;void*_tmp42=_stmttmp6;switch(*((int*)_tmp42)){case 1U: _LLA: _LLB:
 goto _LLD;case 2U: _LLC: _LLD:
 goto _LLF;case 3U: _LLE: _LLF:
 goto _LL9;default: _LL10: _LL11:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 335
Cyc_Absyndump_dump_char((int)')');}
# 338
static void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp43=Cyc_Cyclone_c_compiler;if(_tmp43 == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 342
 Cyc_Absyndump_dump(({const char*_tmp44=" __attribute__((";_tag_fat(_tmp44,sizeof(char),17U);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp45=",";_tag_fat(_tmp45,sizeof(char),2U);}));}
# 347
Cyc_Absyndump_dump(({const char*_tmp46=")) ";_tag_fat(_tmp46,sizeof(char),4U);}));
return;}else{_LL3: _LL4:
# 350
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL0:;}}
# 355
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 359
static struct _tuple16 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
{void*_tmp47=Cyc_Tcutil_compress(t);void*_stmttmp7=_tmp47;void*_tmp48=_stmttmp7;struct Cyc_List_List*_tmp49;void*_tmp4A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2 != 0){_LL1: _tmp4A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2)->hd;_LL2: {void*r=_tmp4A;
rgions=({struct Cyc_List_List*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->hd=r,_tmp4B->tl=rgions;_tmp4B;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp49=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2;_LL4: {struct Cyc_List_List*ts=_tmp49;
# 365
for(0;ts != 0;ts=ts->tl){
struct _tuple16 _tmp4C=Cyc_Absyndump_effects_split((void*)ts->hd);struct _tuple16 _stmttmp8=_tmp4C;struct _tuple16 _tmp4D=_stmttmp8;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp4E;_LL8: _tmp4E=_tmp4D.f1;_tmp4F=_tmp4D.f2;_LL9: {struct Cyc_List_List*rs=_tmp4E;struct Cyc_List_List*es=_tmp4F;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(rs,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,effects);}}
# 370
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 effects=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=t,_tmp50->tl=effects;_tmp50;});goto _LL0;}_LL0:;}
# 373
return({struct _tuple16 _tmp2DD;_tmp2DD.f1=rgions,_tmp2DD.f2=effects;_tmp2DD;});}
# 376
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple16 _tmp51=Cyc_Absyndump_effects_split(t);struct _tuple16 _stmttmp9=_tmp51;struct _tuple16 _tmp52=_stmttmp9;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp53;_LL1: _tmp53=_tmp52.f1;_tmp54=_tmp52.f2;_LL2: {struct Cyc_List_List*rgions=_tmp53;struct Cyc_List_List*effects=_tmp54;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(effects != 0){
({struct Cyc_List_List*_tmp2FC=effects;struct _fat_ptr _tmp2FB=({const char*_tmp55="";_tag_fat(_tmp55,sizeof(char),1U);});struct _fat_ptr _tmp2FA=({const char*_tmp56="";_tag_fat(_tmp56,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpntyp,_tmp2FC,_tmp2FB,_tmp2FA,({const char*_tmp57="+";_tag_fat(_tmp57,sizeof(char),2U);}));});
if(rgions != 0)Cyc_Absyndump_dump_char((int)'+');}
# 384
if(rgions != 0 || effects == 0)
({struct Cyc_List_List*_tmp2FF=rgions;struct _fat_ptr _tmp2FE=({const char*_tmp58="{";_tag_fat(_tmp58,sizeof(char),2U);});struct _fat_ptr _tmp2FD=({const char*_tmp59="}";_tag_fat(_tmp59,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumprgn,_tmp2FF,_tmp2FE,_tmp2FD,({const char*_tmp5A=",";_tag_fat(_tmp5A,sizeof(char),2U);}));});}}
# 389
static void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp5B=t;struct Cyc_Absyn_Exp*_tmp5C;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_List_List*_tmp5F;struct _tuple0*_tmp5E;struct Cyc_Absyn_Datatypedecl*_tmp60;struct Cyc_Absyn_Enumdecl*_tmp61;struct Cyc_Absyn_Aggrdecl*_tmp62;struct Cyc_List_List*_tmp64;enum Cyc_Absyn_AggrKind _tmp63;struct Cyc_List_List*_tmp65;int _tmp68;void*_tmp67;struct Cyc_Core_Opt*_tmp66;int _tmp6A;struct Cyc_Core_Opt*_tmp69;struct Cyc_Absyn_Tvar*_tmp6B;struct Cyc_List_List*_tmp6D;void*_tmp6C;switch(*((int*)_tmp5B)){case 4U: _LL1: _LL2:
# 392
 goto _LL4;case 5U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return;case 0U: if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B)->f1)->tag == 0U){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp6E="void";_tag_fat(_tmp6E,sizeof(char),5U);}));return;}else{_LL1F: _tmp6C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B)->f1;_tmp6D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B)->f2;_LL20: {void*c=_tmp6C;struct Cyc_List_List*ts=_tmp6D;
# 412
void*_tmp78=c;struct _fat_ptr _tmp79;struct Cyc_List_List*_tmp7A;struct _tuple0*_tmp7B;union Cyc_Absyn_AggrInfo _tmp7C;int _tmp7D;union Cyc_Absyn_DatatypeFieldInfo _tmp7E;union Cyc_Absyn_DatatypeInfo _tmp7F;switch(*((int*)_tmp78)){case 18U: _LL22: _tmp7F=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp78)->f1;_LL23: {union Cyc_Absyn_DatatypeInfo tu_info=_tmp7F;
# 414
{union Cyc_Absyn_DatatypeInfo _tmp80=tu_info;int _tmp82;struct _tuple0*_tmp81;int _tmp84;struct _tuple0*_tmp83;if((_tmp80.UnknownDatatype).tag == 1){_LL69: _tmp83=((_tmp80.UnknownDatatype).val).name;_tmp84=((_tmp80.UnknownDatatype).val).is_extensible;_LL6A: {struct _tuple0*n=_tmp83;int is_x=_tmp84;
_tmp81=n;_tmp82=is_x;goto _LL6C;}}else{_LL6B: _tmp81=(*(_tmp80.KnownDatatype).val)->name;_tmp82=(*(_tmp80.KnownDatatype).val)->is_extensible;_LL6C: {struct _tuple0*n=_tmp81;int is_x=_tmp82;
# 417
if(is_x)Cyc_Absyndump_dump(({const char*_tmp85="@extensible";_tag_fat(_tmp85,sizeof(char),12U);}));
Cyc_Absyndump_dump(({const char*_tmp86="datatype";_tag_fat(_tmp86,sizeof(char),9U);}));Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
goto _LL68;}}_LL68:;}
# 421
return;}case 19U: _LL24: _tmp7E=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp78)->f1;_LL25: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp7E;
# 423
{union Cyc_Absyn_DatatypeFieldInfo _tmp87=tuf_info;struct _tuple0*_tmp8A;int _tmp89;struct _tuple0*_tmp88;int _tmp8D;struct _tuple0*_tmp8C;struct _tuple0*_tmp8B;if((_tmp87.UnknownDatatypefield).tag == 1){_LL6E: _tmp8B=((_tmp87.UnknownDatatypefield).val).datatype_name;_tmp8C=((_tmp87.UnknownDatatypefield).val).field_name;_tmp8D=((_tmp87.UnknownDatatypefield).val).is_extensible;_LL6F: {struct _tuple0*tname=_tmp8B;struct _tuple0*fname=_tmp8C;int is_x=_tmp8D;
# 425
_tmp88=tname;_tmp89=is_x;_tmp8A=fname;goto _LL71;}}else{_LL70: _tmp88=(((_tmp87.KnownDatatypefield).val).f1)->name;_tmp89=(((_tmp87.KnownDatatypefield).val).f1)->is_extensible;_tmp8A=(((_tmp87.KnownDatatypefield).val).f2)->name;_LL71: {struct _tuple0*tname=_tmp88;int is_x=_tmp89;struct _tuple0*fname=_tmp8A;
# 428
if(is_x)Cyc_Absyndump_dump(({const char*_tmp8E="@extensible ";_tag_fat(_tmp8E,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp8F="datatype ";_tag_fat(_tmp8F,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(tname);Cyc_Absyndump_dump(({const char*_tmp90=".";_tag_fat(_tmp90,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(fname);
Cyc_Absyndump_dumptps(ts);
goto _LL6D;}}_LL6D:;}
# 433
return;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp78)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp78)->f2){case Cyc_Absyn_Int_sz: _LL26: _LL27:
 goto _LL29;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
# 436
 goto _LL2D;case Cyc_Absyn_Char_sz: _LL2E: _LL2F:
# 438
 Cyc_Absyndump_dump(({const char*_tmp93="char";_tag_fat(_tmp93,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz: _LL34: _LL35:
# 441
 goto _LL37;case Cyc_Absyn_LongLong_sz: _LL40: _LL41:
# 447
 goto _LL43;default: goto _LL66;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp78)->f2){case Cyc_Absyn_Int_sz: _LL28: _LL29:
# 435
 Cyc_Absyndump_dump(({const char*_tmp91="int";_tag_fat(_tmp91,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL2C: _LL2D:
# 437
 Cyc_Absyndump_dump(({const char*_tmp92="long";_tag_fat(_tmp92,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz: _LL30: _LL31:
# 439
 Cyc_Absyndump_dump(({const char*_tmp94="signed char";_tag_fat(_tmp94,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL36: _LL37:
# 442
 Cyc_Absyndump_dump(({const char*_tmp96="short";_tag_fat(_tmp96,sizeof(char),6U);}));return;case Cyc_Absyn_LongLong_sz: _LL42: _LL43: {
# 449
enum Cyc_Cyclone_C_Compilers _tmp9B=Cyc_Cyclone_c_compiler;if(_tmp9B == Cyc_Cyclone_Gcc_c){_LL73: _LL74:
 Cyc_Absyndump_dump(({const char*_tmp9C="long long";_tag_fat(_tmp9C,sizeof(char),10U);}));return;}else{_LL75: _LL76:
 Cyc_Absyndump_dump(({const char*_tmp9D="__int64";_tag_fat(_tmp9D,sizeof(char),8U);}));return;}_LL72:;}default: goto _LL66;}case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp78)->f2){case Cyc_Absyn_Char_sz: _LL32: _LL33:
# 440
 Cyc_Absyndump_dump(({const char*_tmp95="unsigned char";_tag_fat(_tmp95,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz: _LL38: _LL39:
# 443
 Cyc_Absyndump_dump(({const char*_tmp97="unsigned short";_tag_fat(_tmp97,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz: _LL3A: _LL3B:
 Cyc_Absyndump_dump(({const char*_tmp98="unsigned";_tag_fat(_tmp98,sizeof(char),9U);}));return;case Cyc_Absyn_Long_sz: _LL3C: _LL3D:
 Cyc_Absyndump_dump(({const char*_tmp99="unsigned long";_tag_fat(_tmp99,sizeof(char),14U);}));return;case Cyc_Absyn_LongLong_sz: _LL3E: _LL3F:
 Cyc_Absyndump_dump(({const char*_tmp9A="unsigned";_tag_fat(_tmp9A,sizeof(char),9U);}));goto _LL41;default: goto _LL66;}default: goto _LL66;}case 2U: _LL44: _tmp7D=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp78)->f1;_LL45: {int i=_tmp7D;
# 454
int _tmp9E=i;switch(_tmp9E){case 0U: _LL78: _LL79:
 Cyc_Absyndump_dump(({const char*_tmp9F="float";_tag_fat(_tmp9F,sizeof(char),6U);}));return;case 1U: _LL7A: _LL7B:
 Cyc_Absyndump_dump(({const char*_tmpA0="double";_tag_fat(_tmpA0,sizeof(char),7U);}));return;default: _LL7C: _LL7D:
 Cyc_Absyndump_dump(({const char*_tmpA1="long double";_tag_fat(_tmpA1,sizeof(char),12U);}));return;}_LL77:;}case 20U: _LL46: _tmp7C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp78)->f1;_LL47: {union Cyc_Absyn_AggrInfo info=_tmp7C;
# 460
struct _tuple10 _tmpA2=Cyc_Absyn_aggr_kinded_name(info);struct _tuple10 _stmttmpA=_tmpA2;struct _tuple10 _tmpA3=_stmttmpA;struct _tuple0*_tmpA5;enum Cyc_Absyn_AggrKind _tmpA4;_LL7F: _tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;_LL80: {enum Cyc_Absyn_AggrKind k=_tmpA4;struct _tuple0*n=_tmpA5;
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
return;}}case 15U: _LL48: _tmp7B=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp78)->f1;_LL49: {struct _tuple0*n=_tmp7B;
Cyc_Absyndump_dump(({const char*_tmpA6="enum";_tag_fat(_tmpA6,sizeof(char),5U);}));Cyc_Absyndump_dumpqvar(n);return;}case 16U: _LL4A: _tmp7A=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp78)->f1;_LL4B: {struct Cyc_List_List*fs=_tmp7A;
Cyc_Absyndump_dump(({const char*_tmpA7="enum{";_tag_fat(_tmpA7,sizeof(char),6U);}));Cyc_Absyndump_dumpenumfields(fs);Cyc_Absyndump_dump(({const char*_tmpA8="}";_tag_fat(_tmpA8,sizeof(char),2U);}));return;}case 17U: _LL4C: _tmp79=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp78)->f1;_LL4D: {struct _fat_ptr t=_tmp79;
Cyc_Absyndump_dump(t);return;}case 3U: _LL4E: _LL4F:
 Cyc_Absyndump_dump(({const char*_tmpA9="region_t<";_tag_fat(_tmpA9,sizeof(char),10U);}));Cyc_Absyndump_dumprgn((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpAA=">";_tag_fat(_tmpAA,sizeof(char),2U);}));return;case 4U: _LL50: _LL51:
 Cyc_Absyndump_dump(({const char*_tmpAB="tag_t<";_tag_fat(_tmpAB,sizeof(char),7U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpAC=">";_tag_fat(_tmpAC,sizeof(char),2U);}));return;case 6U: _LL52: _LL53:
 Cyc_Absyndump_dump(({const char*_tmpAD="`U";_tag_fat(_tmpAD,sizeof(char),3U);}));return;case 7U: _LL54: _LL55:
 Cyc_Absyndump_dump(({const char*_tmpAE="`RC";_tag_fat(_tmpAE,sizeof(char),4U);}));return;case 5U: _LL56: _LL57:
 Cyc_Absyndump_dump(({const char*_tmpAF="`H";_tag_fat(_tmpAF,sizeof(char),3U);}));return;case 8U: _LL58: _LL59:
 Cyc_Absyndump_dump(({const char*_tmpB0="{";_tag_fat(_tmpB0,sizeof(char),2U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpB1="}";_tag_fat(_tmpB1,sizeof(char),2U);}));return;case 10U: _LL5A: _LL5B:
 Cyc_Absyndump_dump(({const char*_tmpB2="regions(";_tag_fat(_tmpB2,sizeof(char),9U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpB3=")";_tag_fat(_tmpB3,sizeof(char),2U);}));return;case 9U: _LL5C: _LL5D:
({struct Cyc_List_List*_tmp302=ts;struct _fat_ptr _tmp301=({const char*_tmpB4="";_tag_fat(_tmpB4,sizeof(char),1U);});struct _fat_ptr _tmp300=({const char*_tmpB5="";_tag_fat(_tmpB5,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumptyp,_tmp302,_tmp301,_tmp300,({const char*_tmpB6="+";_tag_fat(_tmpB6,sizeof(char),2U);}));});return;case 11U: _LL5E: _LL5F:
 Cyc_Absyndump_dump(({const char*_tmpB7="@true";_tag_fat(_tmpB7,sizeof(char),6U);}));return;case 12U: _LL60: _LL61:
 Cyc_Absyndump_dump(({const char*_tmpB8="@false";_tag_fat(_tmpB8,sizeof(char),7U);}));return;case 13U: _LL62: _LL63:
 Cyc_Absyndump_dump(({const char*_tmpB9="@thin";_tag_fat(_tmpB9,sizeof(char),6U);}));return;case 14U: _LL64: _LL65:
 Cyc_Absyndump_dump(({const char*_tmpBA="@fat";_tag_fat(_tmpBA,sizeof(char),5U);}));return;default: _LL66: _LL67:
({void*_tmpBB=0U;({struct _fat_ptr _tmp303=({const char*_tmpBC="bad type constructor";_tag_fat(_tmpBC,sizeof(char),21U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp303,_tag_fat(_tmpBB,sizeof(void*),0U));});});}_LL21:;}}case 2U: _LL9: _tmp6B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5B)->f1;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp6B;
# 396
Cyc_Absyndump_dumptvar(tv);return;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f2 == 0){_LLB: _tmp69=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f1;_tmp6A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f3;_LLC: {struct Cyc_Core_Opt*k=_tmp69;int i=_tmp6A;
# 398
Cyc_Absyndump_dump(({const char*_tmp6F="`E";_tag_fat(_tmp6F,sizeof(char),3U);}));
if(k == 0)Cyc_Absyndump_dump(({const char*_tmp70="K";_tag_fat(_tmp70,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)k->v);}
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp73=({struct Cyc_Int_pa_PrintArg_struct _tmp2DE;_tmp2DE.tag=1U,_tmp2DE.f1=(unsigned long)i;_tmp2DE;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({struct _fat_ptr _tmp304=({const char*_tmp72="%d";_tag_fat(_tmp72,sizeof(char),3U);});Cyc_aprintf(_tmp304,_tag_fat(_tmp71,sizeof(void*),1U));});}));return;}}else{_LLD: _tmp66=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f1;_tmp67=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f2;_tmp68=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f3;_LLE: {struct Cyc_Core_Opt*k=_tmp66;void*t=_tmp67;int i=_tmp68;
Cyc_Absyndump_dumpntyp((void*)_check_null(t));return;}}case 6U: _LLF: _tmp65=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B)->f1;_LL10: {struct Cyc_List_List*ts=_tmp65;
Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(ts);return;}case 7U: _LL11: _tmp63=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B)->f1;_tmp64=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp63;struct Cyc_List_List*fs=_tmp64;
# 404
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(fs);Cyc_Absyndump_dump_char((int)'}');return;}case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)){case 0U: _LL13: _tmp62=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)->f1;_LL14: {struct Cyc_Absyn_Aggrdecl*d=_tmp62;
Cyc_Absyndump_dump_aggrdecl(d);return;}case 1U: _LL15: _tmp61=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Enumdecl*d=_tmp61;
Cyc_Absyndump_dump_enumdecl(d);return;}default: _LL17: _tmp60=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Datatypedecl*d=_tmp60;
Cyc_Absyndump_dump_datatypedecl(d);return;}}case 8U: _LL19: _tmp5E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5B)->f1;_tmp5F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5B)->f2;_LL1A: {struct _tuple0*n=_tmp5E;struct Cyc_List_List*ts=_tmp5F;
Cyc_Absyndump_dumpqvar(n),Cyc_Absyndump_dumptps(ts);return;}case 9U: _LL1B: _tmp5D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5B)->f1;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5D;
Cyc_Absyndump_dump(({const char*_tmp74="valueof_t(";_tag_fat(_tmp74,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp75=")";_tag_fat(_tmp75,sizeof(char),2U);}));return;}default: _LL1D: _tmp5C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5B)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp5C;
Cyc_Absyndump_dump(({const char*_tmp76="typeof(";_tag_fat(_tmp76,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp77=")";_tag_fat(_tmp77,sizeof(char),2U);}));return;}}_LL0:;}
# 483
static void Cyc_Absyndump_dumpvaropt(struct _fat_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 486
static void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple17{void*f1;void*f2;};
# 489
static void Cyc_Absyndump_dump_rgncmp(struct _tuple17*cmp){
Cyc_Absyndump_dumpeff((*cmp).f1);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn((*cmp).f2);}
# 492
static void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp305=rgn_po;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp305,({const char*_tmpBD=",";_tag_fat(_tmpBD,sizeof(char),2U);}));});}
# 495
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 499
({struct Cyc_List_List*_tmp308=args;struct _fat_ptr _tmp307=({const char*_tmpBE="(";_tag_fat(_tmpBE,sizeof(char),2U);});struct _fat_ptr _tmp306=({const char*_tmpBF="";_tag_fat(_tmpBF,sizeof(char),1U);});((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpfunarg,_tmp308,_tmp307,_tmp306,({const char*_tmpC0=",";_tag_fat(_tmpC0,sizeof(char),2U);}));});
if(c_varargs || cyc_varargs != 0)
Cyc_Absyndump_dump(({const char*_tmpC1=",...";_tag_fat(_tmpC1,sizeof(char),5U);}));
if(cyc_varargs != 0){
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpC2=" inject ";_tag_fat(_tmpC2,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(({struct _tuple8*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=cyc_varargs->name,_tmpC3->f2=cyc_varargs->tq,_tmpC3->f3=cyc_varargs->type;_tmpC3;}));}
# 506
if(effopt != 0){
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff(effopt);}
# 509
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 512
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpC4=" @requires(";_tag_fat(_tmpC4,sizeof(char),12U);}));Cyc_Absyndump_dumpexp(req);Cyc_Absyndump_dump_char((int)')');}
# 516
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpC5=" @ensures(";_tag_fat(_tmpC5,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(ens);Cyc_Absyndump_dump_char((int)')');}}
# 521
static void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _tmp309=Cyc_Absyn_empty_tqual(0U);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp309,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 525
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpC6=d;struct _fat_ptr*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC6)->tag == 0U){_LL1: _tmpC8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC6)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpC8;
Cyc_Absyndump_dump(({const char*_tmpC9=".[";_tag_fat(_tmpC9,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)']');goto _LL0;}}else{_LL3: _tmpC7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpC6)->f1;_LL4: {struct _fat_ptr*v=_tmpC7;
Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*v);goto _LL0;}}_LL0:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 531
static void Cyc_Absyndump_dumpde(struct _tuple18*de){
({struct Cyc_List_List*_tmp30C=(*de).f1;struct _fat_ptr _tmp30B=({const char*_tmpCA="";_tag_fat(_tmpCA,sizeof(char),1U);});struct _fat_ptr _tmp30A=({const char*_tmpCB="=";_tag_fat(_tmpCB,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp30C,_tmp30B,_tmp30A,({const char*_tmpCC="=";_tag_fat(_tmpCC,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 536
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_tmpCD=f;unsigned _tmpCE;struct _fat_ptr*_tmpCF;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpCD)->tag == 0U){_LL1: _tmpCF=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpCD)->f1;_LL2: {struct _fat_ptr*n=_tmpCF;
Cyc_Absyndump_dump_nospace(*n);return;}}else{_LL3: _tmpCE=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpCD)->f1;_LL4: {unsigned n=_tmpCE;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD2=({struct Cyc_Int_pa_PrintArg_struct _tmp2DF;_tmp2DF.tag=1U,_tmp2DF.f1=(unsigned long)((int)n);_tmp2DF;});void*_tmpD0[1U];_tmpD0[0]=& _tmpD2;({struct _fat_ptr _tmp30D=({const char*_tmpD1="%d";_tag_fat(_tmpD1,sizeof(char),3U);});Cyc_aprintf(_tmp30D,_tag_fat(_tmpD0,sizeof(void*),1U));});}));return;}}_LL0:;}
# 543
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp311=inprec;struct Cyc_List_List*_tmp310=es;struct _fat_ptr _tmp30F=({const char*_tmpD3="";_tag_fat(_tmpD3,sizeof(char),1U);});struct _fat_ptr _tmp30E=({const char*_tmpD4="";_tag_fat(_tmpD4,sizeof(char),1U);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp311,_tmp310,_tmp30F,_tmp30E,({const char*_tmpD5=",";_tag_fat(_tmpD5,sizeof(char),2U);}));});}
# 547
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpD6="(";_tag_fat(_tmpD6,sizeof(char),2U);}));
{void*_tmpD7=e->r;void*_stmttmpB=_tmpD7;void*_tmpD8=_stmttmpB;struct Cyc_Absyn_Stmt*_tmpD9;struct Cyc_List_List*_tmpDB;struct Cyc_Core_Opt*_tmpDA;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;int _tmpE2;struct Cyc_Absyn_Exp*_tmpE1;void**_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;int _tmpDE;struct Cyc_Absyn_Datatypefield*_tmpE4;struct Cyc_List_List*_tmpE3;struct Cyc_Absyn_Enumfield*_tmpE5;struct Cyc_Absyn_Enumfield*_tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_List_List*_tmpEA;struct Cyc_List_List*_tmpE9;struct _tuple0*_tmpE8;void*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Vardecl*_tmpED;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpF2;struct _tuple8*_tmpF1;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF4;struct _fat_ptr*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct _fat_ptr*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_List_List*_tmpFB;void*_tmpFA;struct _fat_ptr*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct _fat_ptr _tmpFF;int _tmpFE;struct Cyc_Absyn_Exp*_tmp100;void*_tmp101;struct Cyc_Absyn_Exp*_tmp102;void*_tmp103;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp109;void*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Core_Opt*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_List_List*_tmp120;enum Cyc_Absyn_Primop _tmp11F;struct _fat_ptr _tmp121;void*_tmp122;struct _fat_ptr _tmp123;struct _fat_ptr _tmp124;struct _fat_ptr _tmp125;long long _tmp127;enum Cyc_Absyn_Sign _tmp126;int _tmp128;int _tmp129;int _tmp12A;short _tmp12C;enum Cyc_Absyn_Sign _tmp12B;struct _fat_ptr _tmp12D;char _tmp12F;enum Cyc_Absyn_Sign _tmp12E;switch(*((int*)_tmpD8)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).String_c).tag){case 2U: _LL1: _tmp12E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Char_c).val).f1;_tmp12F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp12E;char ch=_tmp12F;
# 553
Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_char((int)'\'');
goto _LL0;}case 3U: _LL3: _tmp12D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp12D;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp132=({struct Cyc_String_pa_PrintArg_struct _tmp2E0;_tmp2E0.tag=0U,_tmp2E0.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2E0;});void*_tmp130[1U];_tmp130[0]=& _tmp132;({struct _fat_ptr _tmp312=({const char*_tmp131="L'%s'";_tag_fat(_tmp131,sizeof(char),6U);});Cyc_aprintf(_tmp312,_tag_fat(_tmp130,sizeof(void*),1U));});}));goto _LL0;}case 4U: _LL5: _tmp12B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Short_c).val).f1;_tmp12C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp12B;short s=_tmp12C;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp135=({struct Cyc_Int_pa_PrintArg_struct _tmp2E1;_tmp2E1.tag=1U,_tmp2E1.f1=(unsigned long)((int)s);_tmp2E1;});void*_tmp133[1U];_tmp133[0]=& _tmp135;({struct _fat_ptr _tmp313=({const char*_tmp134="%d";_tag_fat(_tmp134,sizeof(char),3U);});Cyc_aprintf(_tmp313,_tag_fat(_tmp133,sizeof(void*),1U));});}));goto _LL0;}case 5U: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL7: _tmp12A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f2;_LL8: {int i=_tmp12A;
_tmp129=i;goto _LLA;}case Cyc_Absyn_Signed: _LL9: _tmp129=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f2;_LLA: {int i=_tmp129;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp138=({struct Cyc_Int_pa_PrintArg_struct _tmp2E2;_tmp2E2.tag=1U,_tmp2E2.f1=(unsigned long)i;_tmp2E2;});void*_tmp136[1U];_tmp136[0]=& _tmp138;({struct _fat_ptr _tmp314=({const char*_tmp137="%d";_tag_fat(_tmp137,sizeof(char),3U);});Cyc_aprintf(_tmp314,_tag_fat(_tmp136,sizeof(void*),1U));});}));goto _LL0;}default: _LLB: _tmp128=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f2;_LLC: {int i=_tmp128;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp13B=({struct Cyc_Int_pa_PrintArg_struct _tmp2E3;_tmp2E3.tag=1U,_tmp2E3.f1=(unsigned)i;_tmp2E3;});void*_tmp139[1U];_tmp139[0]=& _tmp13B;({struct _fat_ptr _tmp315=({const char*_tmp13A="%uU";_tag_fat(_tmp13A,sizeof(char),4U);});Cyc_aprintf(_tmp315,_tag_fat(_tmp139,sizeof(void*),1U));});}));goto _LL0;}}case 6U: _LLD: _tmp126=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).LongLong_c).val).f1;_tmp127=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).LongLong_c).val).f2;_LLE: {enum Cyc_Absyn_Sign sg=_tmp126;long long i=_tmp127;
# 562
Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)i));goto _LL0;}case 7U: _LLF: _tmp125=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Float_c).val).f1;_LL10: {struct _fat_ptr x=_tmp125;
Cyc_Absyndump_dump(x);goto _LL0;}case 1U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp13C="NULL";_tag_fat(_tmp13C,sizeof(char),5U);}));goto _LL0;case 8U: _LL13: _tmp124=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).String_c).val;_LL14: {struct _fat_ptr s=_tmp124;
# 566
Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(s));Cyc_Absyndump_dump_char((int)'"');
goto _LL0;}default: _LL15: _tmp123=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Wstring_c).val;_LL16: {struct _fat_ptr s=_tmp123;
# 569
Cyc_Absyndump_dump(({const char*_tmp13D="L\"";_tag_fat(_tmp13D,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(s);Cyc_Absyndump_dump_char((int)'"');goto _LL0;}}case 1U: _LL17: _tmp122=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL18: {void*b=_tmp122;
# 571
Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL19: _tmp121=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL1A: {struct _fat_ptr p=_tmp121;
# 574
Cyc_Absyndump_dump(({const char*_tmp13E="__cyclone_pragma__(";_tag_fat(_tmp13E,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(p);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 3U: _LL1B: _tmp11F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp120=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL1C: {enum Cyc_Absyn_Primop p=_tmp11F;struct Cyc_List_List*es=_tmp120;
# 577
struct _fat_ptr _tmp13F=Cyc_Absynpp_prim2str(p);struct _fat_ptr pstr=_tmp13F;
{int _tmp140=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmp140;int _tmp141=_stmttmpC;switch(_tmp141){case 1U: _LL70: _LL71:
# 580
 if((int)p == (int)18U){
Cyc_Absyndump_dump(({const char*_tmp142="numelts(";_tag_fat(_tmp142,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(({const char*_tmp143=")";_tag_fat(_tmp143,sizeof(char),2U);}));}else{
# 585
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);}
# 588
goto _LL6F;case 2U: _LL72: _LL73:
# 590
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);
goto _LL6F;default: _LL74: _LL75:
({void*_tmp144=0U;({struct _fat_ptr _tmp316=({const char*_tmp145="Absyndump -- Bad number of arguments to primop";_tag_fat(_tmp145,sizeof(char),47U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp316,_tag_fat(_tmp144,sizeof(void*),0U));});});}_LL6F:;}
# 597
goto _LL0;}case 4U: _LL1D: _tmp11C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp11D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmp11E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp11C;struct Cyc_Core_Opt*popt=_tmp11D;struct Cyc_Absyn_Exp*e2=_tmp11E;
# 600
Cyc_Absyndump_dumpexp_prec(myprec,e1);
if(popt != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)popt->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp146="=";_tag_fat(_tmp146,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,e2);
goto _LL0;}case 5U: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f2){case Cyc_Absyn_PreInc: _LL1F: _tmp11B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp11B;
# 607
Cyc_Absyndump_dump(({const char*_tmp147="++";_tag_fat(_tmp147,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PreDec: _LL21: _tmp11A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL22: {struct Cyc_Absyn_Exp*e2=_tmp11A;
Cyc_Absyndump_dump(({const char*_tmp148="--";_tag_fat(_tmp148,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PostInc: _LL23: _tmp119=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL24: {struct Cyc_Absyn_Exp*e2=_tmp119;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmp149="++";_tag_fat(_tmp149,sizeof(char),3U);}));goto _LL0;}default: _LL25: _tmp118=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL26: {struct Cyc_Absyn_Exp*e2=_tmp118;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmp14A="--";_tag_fat(_tmp14A,sizeof(char),3U);}));goto _LL0;}}case 6U: _LL27: _tmp115=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp116=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp115;struct Cyc_Absyn_Exp*e2=_tmp116;struct Cyc_Absyn_Exp*e3=_tmp117;
# 613
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,e2);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,e3);
goto _LL0;}case 7U: _LL29: _tmp113=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp114=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp113;struct Cyc_Absyn_Exp*e2=_tmp114;
# 620
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmp14B="&&";_tag_fat(_tmp14B,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 8U: _LL2B: _tmp111=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp112=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp112;
# 624
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmp14C="||";_tag_fat(_tmp14C,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 9U: _LL2D: _tmp10F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp110=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp10F;struct Cyc_Absyn_Exp*e2=_tmp110;
# 630
Cyc_Absyndump_dumploc(e->loc);
# 633
Cyc_Absyndump_dumpexp_prec(myprec - 1,e1);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec - 1,e2);
goto _LL0;}case 10U: _LL2F: _tmp10D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp10E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp10D;struct Cyc_List_List*es=_tmp10E;
# 637
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_nospace(({const char*_tmp14D="(";_tag_fat(_tmp14D,sizeof(char),2U);}));{
# 642
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives && !(e->loc == (unsigned)0))&& !(e1->loc == (unsigned)0);
Cyc_Absyndump_dumpexps_prec(20,es);
Cyc_Absyndump_dump_nospace(({const char*_tmp14E=")";_tag_fat(_tmp14E,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;}}case 11U: _LL31: _tmp10C=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp10C;
# 651
Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(({const char*_tmp14F="throw";_tag_fat(_tmp14F,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 12U: _LL33: _tmp10B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp10B;
# 653
_tmp10A=e1;goto _LL36;}case 13U: _LL35: _tmp10A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp10A;
Cyc_Absyndump_dumpexp_prec(inprec,e1);goto _LL0;}case 14U: _LL37: _tmp108=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp109=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL38: {void*t=_tmp108;struct Cyc_Absyn_Exp*e1=_tmp109;
# 657
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,e1);
goto _LL0;}case 15U: _LL39: _tmp107=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp107;
# 660
Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 20U: _LL3B: _tmp106=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmp106;
Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 16U: _LL3D: _tmp104=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp105=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL3E: {struct Cyc_Absyn_Exp*ropt=_tmp104;struct Cyc_Absyn_Exp*e1=_tmp105;
Cyc_Absyndump_dump(({const char*_tmp150="new";_tag_fat(_tmp150,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 17U: _LL3F: _tmp103=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL40: {void*t=_tmp103;
Cyc_Absyndump_dump(({const char*_tmp151="sizeof(";_tag_fat(_tmp151,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 18U: _LL41: _tmp102=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL42: {struct Cyc_Absyn_Exp*e1=_tmp102;
Cyc_Absyndump_dump(({const char*_tmp152="sizeof(";_tag_fat(_tmp152,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 39U: _LL43: _tmp101=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL44: {void*t=_tmp101;
Cyc_Absyndump_dump(({const char*_tmp153="valueof(";_tag_fat(_tmp153,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 41U: _LL45: _tmp100=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL46: {struct Cyc_Absyn_Exp*e1=_tmp100;
# 668
Cyc_Absyndump_dump(({const char*_tmp154="__extension__(";_tag_fat(_tmp154,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 40U: _LL47: _tmpFE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpFF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL48: {int vol=_tmpFE;struct _fat_ptr s=_tmpFF;
# 671
Cyc_Absyndump_dump(({const char*_tmp155="__asm__";_tag_fat(_tmp155,sizeof(char),8U);}));
if(vol)Cyc_Absyndump_dump(({const char*_tmp156="volatile";_tag_fat(_tmp156,sizeof(char),9U);}));
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dump_nospace(s);Cyc_Absyndump_dump_char((int)')');
goto _LL0;}case 38U: _LL49: _tmpFC=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpFD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL4A: {struct Cyc_Absyn_Exp*e=_tmpFC;struct _fat_ptr*f=_tmpFD;
# 677
Cyc_Absyndump_dump(({const char*_tmp157="tagcheck(";_tag_fat(_tmp157,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*f);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;}case 19U: _LL4B: _tmpFA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpFB=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL4C: {void*t=_tmpFA;struct Cyc_List_List*l=_tmpFB;
# 682
Cyc_Absyndump_dump(({const char*_tmp158="offsetof(";_tag_fat(_tmp158,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(t);({struct Cyc_List_List*_tmp319=l;struct _fat_ptr _tmp318=({const char*_tmp159=",";_tag_fat(_tmp159,sizeof(char),2U);});struct _fat_ptr _tmp317=({const char*_tmp15A=")";_tag_fat(_tmp15A,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpoffset_field,_tmp319,_tmp318,_tmp317,({const char*_tmp15B=".";_tag_fat(_tmp15B,sizeof(char),2U);}));});goto _LL0;}case 21U: _LL4D: _tmpF8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpF9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmpF8;struct _fat_ptr*n=_tmpF9;
# 685
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 22U: _LL4F: _tmpF6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpF7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL50: {struct Cyc_Absyn_Exp*e1=_tmpF6;struct _fat_ptr*n=_tmpF7;
# 687
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp15C="->";_tag_fat(_tmp15C,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 23U: _LL51: _tmpF4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpF5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpF4;struct Cyc_Absyn_Exp*e2=_tmpF5;
# 690
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char((int)']');goto _LL0;}case 24U: _LL53: _tmpF3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL54: {struct Cyc_List_List*es=_tmpF3;
# 692
Cyc_Absyndump_dump(({const char*_tmp15D="$(";_tag_fat(_tmp15D,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 25U: _LL55: _tmpF1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpF2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL56: {struct _tuple8*vat=_tmpF1;struct Cyc_List_List*des=_tmpF2;
# 695
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*vat).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp31C=des;struct _fat_ptr _tmp31B=({const char*_tmp15E="{";_tag_fat(_tmp15E,sizeof(char),2U);});struct _fat_ptr _tmp31A=({const char*_tmp15F="}";_tag_fat(_tmp15F,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp31C,_tmp31B,_tmp31A,({const char*_tmp160=",";_tag_fat(_tmp160,sizeof(char),2U);}));});
goto _LL0;}case 26U: _LL57: _tmpF0=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL58: {struct Cyc_List_List*des=_tmpF0;
# 701
({struct Cyc_List_List*_tmp31F=des;struct _fat_ptr _tmp31E=({const char*_tmp161="{";_tag_fat(_tmp161,sizeof(char),2U);});struct _fat_ptr _tmp31D=({const char*_tmp162="}";_tag_fat(_tmp162,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp31F,_tmp31E,_tmp31D,({const char*_tmp163=",";_tag_fat(_tmp163,sizeof(char),2U);}));});goto _LL0;}case 27U: _LL59: _tmpED=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpEE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmpEF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL5A: {struct Cyc_Absyn_Vardecl*vd=_tmpED;struct Cyc_Absyn_Exp*e1=_tmpEE;struct Cyc_Absyn_Exp*e2=_tmpEF;
# 704
Cyc_Absyndump_dump(({const char*_tmp164="{for";_tag_fat(_tmp164,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*vd->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(e1);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 28U: _LL5B: _tmpEB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpEC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL5C: {struct Cyc_Absyn_Exp*e=_tmpEB;void*t=_tmpEC;
# 709
Cyc_Absyndump_dump(({const char*_tmp165="{for x ";_tag_fat(_tmp165,sizeof(char),8U);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(e);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 29U: _LL5D: _tmpE8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpE9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmpEA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL5E: {struct _tuple0*n=_tmpE8;struct Cyc_List_List*ts=_tmpE9;struct Cyc_List_List*des=_tmpEA;
# 714
Cyc_Absyndump_dumpqvar(n);
Cyc_Absyndump_dump_char((int)'{');
if(ts != 0)
Cyc_Absyndump_dumptps(ts);
({struct Cyc_List_List*_tmp322=des;struct _fat_ptr _tmp321=({const char*_tmp166="";_tag_fat(_tmp166,sizeof(char),1U);});struct _fat_ptr _tmp320=({const char*_tmp167="}";_tag_fat(_tmp167,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp322,_tmp321,_tmp320,({const char*_tmp168=",";_tag_fat(_tmp168,sizeof(char),2U);}));});
goto _LL0;}case 30U: _LL5F: _tmpE7=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL60: {struct Cyc_List_List*des=_tmpE7;
# 721
({struct Cyc_List_List*_tmp325=des;struct _fat_ptr _tmp324=({const char*_tmp169="{";_tag_fat(_tmp169,sizeof(char),2U);});struct _fat_ptr _tmp323=({const char*_tmp16A="}";_tag_fat(_tmp16A,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp325,_tmp324,_tmp323,({const char*_tmp16B=",";_tag_fat(_tmp16B,sizeof(char),2U);}));});goto _LL0;}case 32U: _LL61: _tmpE6=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL62: {struct Cyc_Absyn_Enumfield*ef=_tmpE6;
_tmpE5=ef;goto _LL64;}case 33U: _LL63: _tmpE5=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL64: {struct Cyc_Absyn_Enumfield*ef=_tmpE5;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 31U: _LL65: _tmpE3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpE4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL66: {struct Cyc_List_List*es=_tmpE3;struct Cyc_Absyn_Datatypefield*tuf=_tmpE4;
# 726
Cyc_Absyndump_dumpqvar(tuf->name);
if(es != 0)({struct Cyc_List_List*_tmp328=es;struct _fat_ptr _tmp327=({const char*_tmp16C="(";_tag_fat(_tmp16C,sizeof(char),2U);});struct _fat_ptr _tmp326=({const char*_tmp16D=")";_tag_fat(_tmp16D,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp328,_tmp327,_tmp326,({const char*_tmp16E=",";_tag_fat(_tmp16E,sizeof(char),2U);}));});
goto _LL0;}case 34U: _LL67: _tmpDE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).is_calloc;_tmpDF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).rgn;_tmpE0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).elt_type;_tmpE1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).num_elts;_tmpE2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).inline_call;_LL68: {int is_calloc=_tmpDE;struct Cyc_Absyn_Exp*ropt=_tmpDF;void**topt=_tmpE0;struct Cyc_Absyn_Exp*e=_tmpE1;int inline_call=_tmpE2;
# 731
Cyc_Absyndump_dumploc(e->loc);
if(is_calloc){
if(ropt != 0){
Cyc_Absyndump_dump(({const char*_tmp16F="rcalloc(";_tag_fat(_tmp16F,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp170=",";_tag_fat(_tmp170,sizeof(char),2U);}));}else{
# 736
Cyc_Absyndump_dump(({const char*_tmp171="calloc(";_tag_fat(_tmp171,sizeof(char),8U);}));}
# 738
Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp172=",";_tag_fat(_tmp172,sizeof(char),2U);}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U));Cyc_Absyndump_dump(({const char*_tmp173=")";_tag_fat(_tmp173,sizeof(char),2U);}));}else{
# 740
if(ropt != 0){
Cyc_Absyndump_dump(inline_call?({const char*_tmp174="rmalloc_inline(";_tag_fat(_tmp174,sizeof(char),16U);}):({const char*_tmp175="rmalloc(";_tag_fat(_tmp175,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp176=",";_tag_fat(_tmp176,sizeof(char),2U);}));}else{
# 744
Cyc_Absyndump_dump(({const char*_tmp177="malloc(";_tag_fat(_tmp177,sizeof(char),8U);}));}
# 747
if(topt != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp329=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp329,e,0U);}));else{
# 750
Cyc_Absyndump_dumpexp(e);}
Cyc_Absyndump_dump(({const char*_tmp178=")";_tag_fat(_tmp178,sizeof(char),2U);}));}
# 753
goto _LL0;}case 35U: _LL69: _tmpDC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL6A: {struct Cyc_Absyn_Exp*e1=_tmpDC;struct Cyc_Absyn_Exp*e2=_tmpDD;
# 756
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp179=":=:";_tag_fat(_tmp179,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 36U: _LL6B: _tmpDA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL6C: {struct Cyc_Core_Opt*n=_tmpDA;struct Cyc_List_List*des=_tmpDB;
# 759
({struct Cyc_List_List*_tmp32C=des;struct _fat_ptr _tmp32B=({const char*_tmp17A="{";_tag_fat(_tmp17A,sizeof(char),2U);});struct _fat_ptr _tmp32A=({const char*_tmp17B="}";_tag_fat(_tmp17B,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp32C,_tmp32B,_tmp32A,({const char*_tmp17C=",";_tag_fat(_tmp17C,sizeof(char),2U);}));});goto _LL0;}default: _LL6D: _tmpD9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL6E: {struct Cyc_Absyn_Stmt*s=_tmpD9;
# 762
Cyc_Absyndump_dump_nospace(({const char*_tmp17D="({";_tag_fat(_tmp17D,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s,1,0);Cyc_Absyndump_dump_nospace(({const char*_tmp17E="})";_tag_fat(_tmp17E,sizeof(char),3U);}));goto _LL0;}}_LL0:;}
# 764
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 768
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 772
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs,struct Cyc_List_List*varsinblock){
# 774
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp17F=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*c=_tmp17F;
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp180="default:";_tag_fat(_tmp180,sizeof(char),9U);}));else{
# 779
Cyc_Absyndump_dump(({const char*_tmp181="case";_tag_fat(_tmp181,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(c->pattern);
if(c->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp182="&&";_tag_fat(_tmp182,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));}
# 785
Cyc_Absyndump_dump_nospace(({const char*_tmp183=":";_tag_fat(_tmp183,sizeof(char),2U);}));}
# 787
if(Cyc_Absynpp_is_declaration(c->body)){
Cyc_Absyndump_dump(({const char*_tmp184=" {";_tag_fat(_tmp184,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(c->body,0,0),Cyc_Absyndump_dump_char((int)'}');}else{
# 790
Cyc_Absyndump_dumpstmt(c->body,0,varsinblock);}}}
# 794
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt,struct Cyc_List_List*varsinblock){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp185=s->r;void*_stmttmpD=_tmp185;void*_tmp186=_stmttmpD;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Stmt*_tmp189;struct _fat_ptr*_tmp188;struct Cyc_Absyn_Stmt*_tmp18B;struct Cyc_Absyn_Decl*_tmp18A;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_Stmt*_tmp18C;struct Cyc_List_List*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;struct _fat_ptr*_tmp190;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Stmt*_tmp191;struct Cyc_Absyn_Stmt*_tmp196;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Stmt*_tmp198;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Stmt*_tmp19B;struct Cyc_Absyn_Stmt*_tmp19A;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Stmt*_tmp19E;struct Cyc_Absyn_Stmt*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;switch(*((int*)_tmp186)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1U: _LL3: _tmp19F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp19F;
Cyc_Absyndump_dumpexp_prec(- 100,e);Cyc_Absyndump_dump_semi();goto _LL0;}case 2U: _LL5: _tmp19D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp19E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp186)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp19D;struct Cyc_Absyn_Stmt*s2=_tmp19E;
# 803
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 806
Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}
if(Cyc_Absynpp_is_declaration(s2)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(s2,expstmt,0);Cyc_Absyndump_dump_char((int)'}');
if(expstmt){Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}}else{
# 812
Cyc_Absyndump_dumpstmt(s2,expstmt,varsinblock);}
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp186)->f1 == 0){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1A0="return;";_tag_fat(_tmp1A0,sizeof(char),8U);}));goto _LL0;}else{_LL9: _tmp19C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp19C;
Cyc_Absyndump_dump(({const char*_tmp1A1="return";_tag_fat(_tmp1A1,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(e));Cyc_Absyndump_dump_semi();goto _LL0;}}case 4U: _LLB: _tmp199=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp19A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp186)->f2;_tmp19B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp186)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp199;struct Cyc_Absyn_Stmt*s1=_tmp19A;struct Cyc_Absyn_Stmt*s2=_tmp19B;
# 817
Cyc_Absyndump_dump(({const char*_tmp1A2="if(";_tag_fat(_tmp1A2,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(e);
{void*_tmp1A3=s1->r;void*_stmttmpE=_tmp1A3;void*_tmp1A4=_stmttmpE;switch(*((int*)_tmp1A4)){case 2U: _LL26: _LL27:
 goto _LL29;case 12U: _LL28: _LL29:
 goto _LL2B;case 4U: _LL2A: _LL2B:
 goto _LL2D;case 13U: _LL2C: _LL2D:
# 823
 Cyc_Absyndump_dump_nospace(({const char*_tmp1A5="){";_tag_fat(_tmp1A5,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');goto _LL25;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}_LL25:;}
# 826
{void*_tmp1A6=s2->r;void*_stmttmpF=_tmp1A6;void*_tmp1A7=_stmttmpF;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1A7)->tag == 0U){_LL31: _LL32:
 goto _LL30;}else{_LL33: _LL34:
 Cyc_Absyndump_dump(({const char*_tmp1A8="else{";_tag_fat(_tmp1A8,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(s2,0,0);Cyc_Absyndump_dump_char((int)'}');goto _LL30;}_LL30:;}
# 830
goto _LL0;}case 5U: _LLD: _tmp197=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp186)->f1).f1;_tmp198=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp186)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp197;struct Cyc_Absyn_Stmt*s1=_tmp198;
# 833
Cyc_Absyndump_dump(({const char*_tmp1A9="while(";_tag_fat(_tmp1A9,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1AA="){";_tag_fat(_tmp1AA,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 9U: _LLF: _tmp193=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp194=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp186)->f2).f1;_tmp195=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp186)->f3).f1;_tmp196=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp186)->f4;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp193;struct Cyc_Absyn_Exp*e2=_tmp194;struct Cyc_Absyn_Exp*e3=_tmp195;struct Cyc_Absyn_Stmt*s1=_tmp196;
# 837
Cyc_Absyndump_dump(({const char*_tmp1AB="for(";_tag_fat(_tmp1AB,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e3);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AC="){";_tag_fat(_tmp1AC,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 14U: _LL11: _tmp191=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp192=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp186)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s1=_tmp191;struct Cyc_Absyn_Exp*e=_tmp192;
# 841
Cyc_Absyndump_dump(({const char*_tmp1AD="do{";_tag_fat(_tmp1AD,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AE="}while(";_tag_fat(_tmp1AE,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1AF=");";_tag_fat(_tmp1AF,sizeof(char),3U);}));
goto _LL0;}case 6U: _LL13: _LL14:
# 845
 Cyc_Absyndump_dump(({const char*_tmp1B0="break;";_tag_fat(_tmp1B0,sizeof(char),7U);}));goto _LL0;case 7U: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp1B1="continue;";_tag_fat(_tmp1B1,sizeof(char),10U);}));goto _LL0;case 8U: _LL17: _tmp190=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_LL18: {struct _fat_ptr*x=_tmp190;
Cyc_Absyndump_dump(({const char*_tmp1B2="goto";_tag_fat(_tmp1B2,sizeof(char),5U);}));Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_semi();goto _LL0;}case 10U: _LL19: _tmp18E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp18F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp186)->f2;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp18E;struct Cyc_List_List*ss=_tmp18F;
# 850
Cyc_Absyndump_dump(({const char*_tmp1B3="switch(";_tag_fat(_tmp1B3,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1B4="){";_tag_fat(_tmp1B4,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 15U: _LL1B: _tmp18C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp18D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp186)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp18C;struct Cyc_List_List*ss=_tmp18D;
# 855
Cyc_Absyndump_dump(({const char*_tmp1B5="try";_tag_fat(_tmp1B5,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,varsinblock);
Cyc_Absyndump_dump(({const char*_tmp1B6="catch{";_tag_fat(_tmp1B6,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 12U: _LL1D: _tmp18A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp18B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp186)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp18A;struct Cyc_Absyn_Stmt*s1=_tmp18B;
# 861
struct _tuple12 _tmp1B7=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple12 _stmttmp10=_tmp1B7;struct _tuple12 _tmp1B8=_stmttmp10;struct Cyc_List_List*_tmp1BA;int _tmp1B9;_LL36: _tmp1B9=_tmp1B8.f1;_tmp1BA=_tmp1B8.f2;_LL37: {int newblock=_tmp1B9;struct Cyc_List_List*newvarsinblock=_tmp1BA;
if(newblock){
if(expstmt)Cyc_Absyndump_dump(({const char*_tmp1BB="({";_tag_fat(_tmp1BB,sizeof(char),3U);}));else{Cyc_Absyndump_dump(({const char*_tmp1BC="{";_tag_fat(_tmp1BC,sizeof(char),2U);}));}
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1BD="});";_tag_fat(_tmp1BD,sizeof(char),4U);}));else{Cyc_Absyndump_dump(({const char*_tmp1BE="}";_tag_fat(_tmp1BE,sizeof(char),2U);}));}}else{
# 868
Cyc_Absyndump_dumpdecl(d);Cyc_Absyndump_dumpstmt(s1,expstmt,newvarsinblock);}
# 870
goto _LL0;}}case 13U: _LL1F: _tmp188=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_tmp189=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp186)->f2;_LL20: {struct _fat_ptr*x=_tmp188;struct Cyc_Absyn_Stmt*s1=_tmp189;
# 876
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_str(x);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1BF=": ({";_tag_fat(_tmp1BF,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1C0=": {";_tag_fat(_tmp1C0,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1C1="});";_tag_fat(_tmp1C1,sizeof(char),4U);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 882
Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(s1,expstmt,varsinblock);}
# 884
goto _LL0;}default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp186)->f1 == 0){_LL21: _LL22:
# 886
 Cyc_Absyndump_dump(({const char*_tmp1C2="fallthru;";_tag_fat(_tmp1C2,sizeof(char),10U);}));goto _LL0;}else{_LL23: _tmp187=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp186)->f1;_LL24: {struct Cyc_List_List*es=_tmp187;
# 888
Cyc_Absyndump_dump(({const char*_tmp1C3="fallthru(";_tag_fat(_tmp1C3,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_nospace(({const char*_tmp1C4=");";_tag_fat(_tmp1C4,sizeof(char),3U);}));goto _LL0;}}}_LL0:;}}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 892
static void Cyc_Absyndump_dumpdp(struct _tuple19*dp){
({struct Cyc_List_List*_tmp32F=(*dp).f1;struct _fat_ptr _tmp32E=({const char*_tmp1C5="";_tag_fat(_tmp1C5,sizeof(char),1U);});struct _fat_ptr _tmp32D=({const char*_tmp1C6="=";_tag_fat(_tmp1C6,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp32F,_tmp32E,_tmp32D,({const char*_tmp1C7="=";_tag_fat(_tmp1C7,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 896
static struct _fat_ptr Cyc_Absyndump_pat_term(int dots){return dots?({const char*_tmp1C8="...)";_tag_fat(_tmp1C8,sizeof(char),5U);}):({const char*_tmp1C9=")";_tag_fat(_tmp1C9,sizeof(char),2U);});}
# 898
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1CA=p->r;void*_stmttmp11=_tmp1CA;void*_tmp1CB=_stmttmp11;struct Cyc_Absyn_Exp*_tmp1CC;int _tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Datatypefield*_tmp1CD;int _tmp1D2;struct Cyc_List_List*_tmp1D1;struct Cyc_List_List*_tmp1D0;int _tmp1D6;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D4;union Cyc_Absyn_AggrInfo _tmp1D3;int _tmp1D9;struct Cyc_List_List*_tmp1D8;struct _tuple0*_tmp1D7;struct _tuple0*_tmp1DA;struct Cyc_Absyn_Vardecl*_tmp1DC;struct Cyc_Absyn_Tvar*_tmp1DB;struct Cyc_Absyn_Pat*_tmp1DD;int _tmp1DF;struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_Vardecl*_tmp1E1;struct Cyc_Absyn_Tvar*_tmp1E0;struct Cyc_Absyn_Pat*_tmp1E3;struct Cyc_Absyn_Vardecl*_tmp1E2;struct Cyc_Absyn_Vardecl*_tmp1E4;struct Cyc_Absyn_Pat*_tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E5;struct Cyc_Absyn_Vardecl*_tmp1E7;char _tmp1E8;struct Cyc_Absyn_Enumfield*_tmp1E9;struct Cyc_Absyn_Enumfield*_tmp1EA;struct _fat_ptr _tmp1EB;int _tmp1EC;int _tmp1ED;int _tmp1EE;switch(*((int*)_tmp1CB)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_char((int)'_');goto _LL0;case 9U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp1EF="NULL";_tag_fat(_tmp1EF,sizeof(char),5U);}));goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1){case Cyc_Absyn_None: _LL5: _tmp1EE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL6: {int i=_tmp1EE;
_tmp1ED=i;goto _LL8;}case Cyc_Absyn_Signed: _LL7: _tmp1ED=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL8: {int i=_tmp1ED;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1F2=({struct Cyc_Int_pa_PrintArg_struct _tmp2E4;_tmp2E4.tag=1U,_tmp2E4.f1=(unsigned long)i;_tmp2E4;});void*_tmp1F0[1U];_tmp1F0[0]=& _tmp1F2;({struct _fat_ptr _tmp330=({const char*_tmp1F1="%d";_tag_fat(_tmp1F1,sizeof(char),3U);});Cyc_aprintf(_tmp330,_tag_fat(_tmp1F0,sizeof(void*),1U));});}));goto _LL0;}default: _LL9: _tmp1EC=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LLA: {int i=_tmp1EC;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1F5=({struct Cyc_Int_pa_PrintArg_struct _tmp2E5;_tmp2E5.tag=1U,_tmp2E5.f1=(unsigned)i;_tmp2E5;});void*_tmp1F3[1U];_tmp1F3[0]=& _tmp1F5;({struct _fat_ptr _tmp331=({const char*_tmp1F4="%u";_tag_fat(_tmp1F4,sizeof(char),3U);});Cyc_aprintf(_tmp331,_tag_fat(_tmp1F3,sizeof(void*),1U));});}));goto _LL0;}}case 12U: _LLB: _tmp1EB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_LLC: {struct _fat_ptr x=_tmp1EB;
Cyc_Absyndump_dump(x);goto _LL0;}case 13U: _LLD: _tmp1EA=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LLE: {struct Cyc_Absyn_Enumfield*ef=_tmp1EA;
_tmp1E9=ef;goto _LL10;}case 14U: _LLF: _tmp1E9=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL10: {struct Cyc_Absyn_Enumfield*ef=_tmp1E9;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 11U: _LL11: _tmp1E8=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_LL12: {char ch=_tmp1E8;
# 909
Cyc_Absyndump_dump(({const char*_tmp1F6="'";_tag_fat(_tmp1F6,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_nospace(({const char*_tmp1F7="'";_tag_fat(_tmp1F7,sizeof(char),2U);}));goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2)->r)->tag == 0U){_LL13: _tmp1E7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_LL14: {struct Cyc_Absyn_Vardecl*vd=_tmp1E7;
Cyc_Absyndump_dump(({const char*_tmp1F8="*";_tag_fat(_tmp1F8,sizeof(char),2U);}));_tmp1E4=vd;goto _LL16;}}else{_LL17: _tmp1E5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_tmp1E6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp1E5;struct Cyc_Absyn_Pat*p2=_tmp1E6;
# 912
Cyc_Absyndump_dump(({const char*_tmp1F9="*";_tag_fat(_tmp1F9,sizeof(char),2U);}));_tmp1E2=vd;_tmp1E3=p2;goto _LL1A;}}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2)->r)->tag == 0U){_LL15: _tmp1E4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_LL16: {struct Cyc_Absyn_Vardecl*vd=_tmp1E4;
# 911
Cyc_Absyndump_dumpqvar(vd->name);goto _LL0;}}else{_LL19: _tmp1E2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_tmp1E3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp1E2;struct Cyc_Absyn_Pat*p2=_tmp1E3;
# 913
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump(({const char*_tmp1FA=" as ";_tag_fat(_tmp1FA,sizeof(char),5U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}}case 2U: _LL1B: _tmp1E0=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_tmp1E1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL1C: {struct Cyc_Absyn_Tvar*tv=_tmp1E0;struct Cyc_Absyn_Vardecl*vd=_tmp1E1;
# 915
Cyc_Absyndump_dump(({const char*_tmp1FB="alias";_tag_fat(_tmp1FB,sizeof(char),6U);}));Cyc_Absyndump_dump(({const char*_tmp1FC="<";_tag_fat(_tmp1FC,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp1FD=">";_tag_fat(_tmp1FD,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(vd,p->loc,0);
goto _LL0;}case 5U: _LL1D: _tmp1DE=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_tmp1DF=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL1E: {struct Cyc_List_List*ts=_tmp1DE;int dots=_tmp1DF;
({struct Cyc_List_List*_tmp334=ts;struct _fat_ptr _tmp333=({const char*_tmp1FE="$(";_tag_fat(_tmp1FE,sizeof(char),3U);});struct _fat_ptr _tmp332=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp334,_tmp333,_tmp332,({const char*_tmp1FF=",";_tag_fat(_tmp1FF,sizeof(char),2U);}));});goto _LL0;}case 6U: _LL1F: _tmp1DD=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_LL20: {struct Cyc_Absyn_Pat*p2=_tmp1DD;
Cyc_Absyndump_dump(({const char*_tmp200="&";_tag_fat(_tmp200,sizeof(char),2U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}case 4U: _LL21: _tmp1DB=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_tmp1DC=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_LL22: {struct Cyc_Absyn_Tvar*tv=_tmp1DB;struct Cyc_Absyn_Vardecl*vd=_tmp1DC;
# 921
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump_char((int)'>');goto _LL0;}case 15U: _LL23: _tmp1DA=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_LL24: {struct _tuple0*q=_tmp1DA;
Cyc_Absyndump_dumpqvar(q);goto _LL0;}case 16U: _LL25: _tmp1D7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_tmp1D8=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_tmp1D9=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1CB)->f3;_LL26: {struct _tuple0*q=_tmp1D7;struct Cyc_List_List*ps=_tmp1D8;int dots=_tmp1D9;
# 924
Cyc_Absyndump_dumpqvar(q);({struct Cyc_List_List*_tmp337=ps;struct _fat_ptr _tmp336=({const char*_tmp201="(";_tag_fat(_tmp201,sizeof(char),2U);});struct _fat_ptr _tmp335=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp337,_tmp336,_tmp335,({const char*_tmp202=",";_tag_fat(_tmp202,sizeof(char),2U);}));});goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1 != 0){_LL27: _tmp1D3=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_tmp1D4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_tmp1D5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f3;_tmp1D6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f4;_LL28: {union Cyc_Absyn_AggrInfo info=_tmp1D3;struct Cyc_List_List*exists=_tmp1D4;struct Cyc_List_List*dps=_tmp1D5;int dots=_tmp1D6;
# 926
Cyc_Absyndump_dumpqvar((Cyc_Absyn_aggr_kinded_name(info)).f2);
_tmp1D0=exists;_tmp1D1=dps;_tmp1D2=dots;goto _LL2A;}}else{_LL29: _tmp1D0=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_tmp1D1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f3;_tmp1D2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CB)->f4;_LL2A: {struct Cyc_List_List*exists=_tmp1D0;struct Cyc_List_List*dps=_tmp1D1;int dots=_tmp1D2;
# 929
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp33A=exists;struct _fat_ptr _tmp339=({const char*_tmp203="<";_tag_fat(_tmp203,sizeof(char),2U);});struct _fat_ptr _tmp338=({const char*_tmp204=">";_tag_fat(_tmp204,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp33A,_tmp339,_tmp338,({const char*_tmp205=",";_tag_fat(_tmp205,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp33D=dps;struct _fat_ptr _tmp33C=({const char*_tmp206="";_tag_fat(_tmp206,sizeof(char),1U);});struct _fat_ptr _tmp33B=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp33D,_tmp33C,_tmp33B,({const char*_tmp207=",";_tag_fat(_tmp207,sizeof(char),2U);}));});
goto _LL0;}}case 8U: _LL2B: _tmp1CD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1CB)->f2;_tmp1CE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1CB)->f3;_tmp1CF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1CB)->f4;_LL2C: {struct Cyc_Absyn_Datatypefield*tuf=_tmp1CD;struct Cyc_List_List*ps=_tmp1CE;int dots=_tmp1CF;
# 934
Cyc_Absyndump_dumpqvar(tuf->name);
if(ps != 0)({struct Cyc_List_List*_tmp340=ps;struct _fat_ptr _tmp33F=({const char*_tmp208="(";_tag_fat(_tmp208,sizeof(char),2U);});struct _fat_ptr _tmp33E=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp340,_tmp33F,_tmp33E,({const char*_tmp209=",";_tag_fat(_tmp209,sizeof(char),2U);}));});
goto _LL0;}default: _LL2D: _tmp1CC=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1CB)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp1CC;
Cyc_Absyndump_dumpexp(e);goto _LL0;}}_LL0:;}
# 941
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 946
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp341=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp341,({const char*_tmp20A=",";_tag_fat(_tmp20A,sizeof(char),2U);}));});}
# 949
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp20B="=";_tag_fat(_tmp20B,sizeof(char),2U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 955
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp342=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp342,({const char*_tmp20C=",";_tag_fat(_tmp20C,sizeof(char),2U);}));});}
# 959
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp20D=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_stmttmp12=_tmp20D;struct Cyc_Absyn_Aggrfield*_tmp20E=_stmttmp12;struct Cyc_Absyn_Exp*_tmp214;struct Cyc_List_List*_tmp213;struct Cyc_Absyn_Exp*_tmp212;void*_tmp211;struct Cyc_Absyn_Tqual _tmp210;struct _fat_ptr*_tmp20F;_LL1: _tmp20F=_tmp20E->name;_tmp210=_tmp20E->tq;_tmp211=_tmp20E->type;_tmp212=_tmp20E->width;_tmp213=_tmp20E->attributes;_tmp214=_tmp20E->requires_clause;_LL2: {struct _fat_ptr*name=_tmp20F;struct Cyc_Absyn_Tqual tq=_tmp210;void*type=_tmp211;struct Cyc_Absyn_Exp*width=_tmp212;struct Cyc_List_List*atts=_tmp213;struct Cyc_Absyn_Exp*req=_tmp214;
{enum Cyc_Cyclone_C_Compilers _tmp215=Cyc_Cyclone_c_compiler;if(_tmp215 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dump_str,name);Cyc_Absyndump_dumpatts(atts);goto _LL3;}else{_LL6: _LL7:
 Cyc_Absyndump_dumpatts(atts);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dump_str,name);goto _LL3;}_LL3:;}
# 966
if((unsigned)req){
Cyc_Absyndump_dump(({const char*_tmp216="@requires ";_tag_fat(_tmp216,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(req);}
# 969
if(width != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(width);}
# 972
Cyc_Absyndump_dump_semi();}}}
# 976
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 981
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple20{void*f1;struct _tuple0*f2;};
# 985
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple20*pr){
{void*_tmp217=(*pr).f1;void*_stmttmp13=_tmp217;void*_tmp218=_stmttmp13;switch(*((int*)_tmp218)){case 11U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp219="_stdcall";_tag_fat(_tmp219,sizeof(char),9U);}));goto _LL0;case 2U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp21A="_cdecl";_tag_fat(_tmp21A,sizeof(char),7U);}));goto _LL0;case 3U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp21B="_fastcall";_tag_fat(_tmp21B,sizeof(char),10U);}));goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 993
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 995
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 999
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 1003
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1005
void*_tmp21C=t;void*_tmp21D;void*_tmp21E;switch(*((int*)_tmp21C)){case 1U: _LL1: _tmp21E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21C)->f2;if(_tmp21E != 0){_LL2: {void*def=_tmp21E;
return Cyc_Absyndump_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).elt_type;_LL4: {void*elt_typ=_tmp21D;
# 1008
while(1){
void*_tmp21F=elt_typ;void*_tmp220;void*_tmp221;switch(*((int*)_tmp21F)){case 1U: _LL8: _tmp221=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21F)->f2;if(_tmp221 != 0){_LL9: {void*t=_tmp221;
elt_typ=t;goto _LL7;}}else{goto _LLE;}case 8U: _LLA: _tmp220=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp21F)->f4;if(_tmp220 != 0){_LLB: {void*t=_tmp220;
elt_typ=t;goto _LL7;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 1015
 return 0;}_LL0:;}
# 1019
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp222=vd;struct Cyc_List_List*_tmp229;struct Cyc_Absyn_Exp*_tmp228;void*_tmp227;struct Cyc_Absyn_Tqual _tmp226;unsigned _tmp225;struct _tuple0*_tmp224;enum Cyc_Absyn_Scope _tmp223;_LL1: _tmp223=_tmp222->sc;_tmp224=_tmp222->name;_tmp225=_tmp222->varloc;_tmp226=_tmp222->tq;_tmp227=_tmp222->type;_tmp228=_tmp222->initializer;_tmp229=_tmp222->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp223;struct _tuple0*name=_tmp224;unsigned varloc=_tmp225;struct Cyc_Absyn_Tqual tq=_tmp226;void*type=_tmp227;struct Cyc_Absyn_Exp*initializer=_tmp228;struct Cyc_List_List*atts=_tmp229;
{enum Cyc_Cyclone_C_Compilers _tmp22A=Cyc_Cyclone_c_compiler;if(_tmp22A == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1024
 if((int)sc == (int)3U && Cyc_Absyndump_qvar_to_Cids){
void*_tmp22B=Cyc_Tcutil_compress(type);void*_stmttmp14=_tmp22B;void*_tmp22C=_stmttmp14;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp22C)->tag == 5U){_LL9: _LLA:
 goto _LL8;}else{_LLB: _LLC:
 Cyc_Absyndump_dumpscope(sc);}_LL8:;}else{
# 1030
Cyc_Absyndump_dumpscope(sc);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dumpqvar,name);
Cyc_Absyndump_dumpatts(atts);
goto _LL3;}else{_LL6: _LL7:
# 1035
 Cyc_Absyndump_dumpatts(atts);
Cyc_Absyndump_dumpscope(sc);{
struct _RegionHandle _tmp22D=_new_region("temp");struct _RegionHandle*temp=& _tmp22D;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(type);
struct _tuple11 _tmp22E=Cyc_Absynpp_to_tms(temp,tq,type);struct _tuple11 _stmttmp15=_tmp22E;struct _tuple11 _tmp22F=_stmttmp15;struct Cyc_List_List*_tmp232;void*_tmp231;struct Cyc_Absyn_Tqual _tmp230;_LLE: _tmp230=_tmp22F.f1;_tmp231=_tmp22F.f2;_tmp232=_tmp22F.f3;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp230;void*t=_tmp231;struct Cyc_List_List*tms=_tmp232;
# 1041
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=tms;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp233=(void*)tms2->hd;void*_stmttmp16=_tmp233;void*_tmp234=_stmttmp16;struct Cyc_List_List*_tmp235;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp234)->tag == 5U){_LL11: _tmp235=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp234)->f2;_LL12: {struct Cyc_List_List*atts=_tmp235;
# 1045
for(0;atts != 0;atts=atts->tl){
void*_tmp236=(void*)atts->hd;void*_stmttmp17=_tmp236;void*_tmp237=_stmttmp17;switch(*((int*)_tmp237)){case 1U: _LL16: _LL17:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL15;case 2U: _LL18: _LL19:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL15;case 3U: _LL1A: _LL1B:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL15;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 1052
goto _LL10;}}else{_LL13: _LL14:
 goto _LL10;}_LL10:;}}
# 1055
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);{
struct _tuple20 _tmp238=({struct _tuple20 _tmp2E6;_tmp2E6.f1=call_conv,_tmp2E6.f2=name;_tmp2E6;});struct _tuple20 pr=_tmp238;
({int _tmp343=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*,void(*f)(struct _tuple20*),struct _tuple20*a))Cyc_Absyndump_dumptms)(_tmp343,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms),Cyc_Absyndump_dump_callconv_qvar,& pr);});
_npop_handler(0U);goto _LL3;}}}
# 1038
;_pop_region(temp);}}_LL3:;}
# 1062
if(initializer != 0){
Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(initializer);}
# 1065
if(do_semi)Cyc_Absyndump_dump_semi();}}
# 1068
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp239="@tagged ";_tag_fat(_tmp239,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl != 0){
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp346=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _fat_ptr _tmp345=({const char*_tmp23A="<";_tag_fat(_tmp23A,sizeof(char),2U);});struct _fat_ptr _tmp344=({const char*_tmp23B=">";_tag_fat(_tmp23B,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp346,_tmp345,_tmp344,({const char*_tmp23C=",";_tag_fat(_tmp23C,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1079
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump_char((int)'}');
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1084
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp23D=ed;struct Cyc_Core_Opt*_tmp240;struct _tuple0*_tmp23F;enum Cyc_Absyn_Scope _tmp23E;_LL1: _tmp23E=_tmp23D->sc;_tmp23F=_tmp23D->name;_tmp240=_tmp23D->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp23E;struct _tuple0*nm=_tmp23F;struct Cyc_Core_Opt*fields=_tmp240;
Cyc_Absyndump_dumpscope(sc);Cyc_Absyndump_dump(({const char*_tmp241="enum ";_tag_fat(_tmp241,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(nm);
if(fields != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char((int)'}');}}}
# 1091
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp242=dd;int _tmp247;struct Cyc_Core_Opt*_tmp246;struct Cyc_List_List*_tmp245;struct _tuple0*_tmp244;enum Cyc_Absyn_Scope _tmp243;_LL1: _tmp243=_tmp242->sc;_tmp244=_tmp242->name;_tmp245=_tmp242->tvs;_tmp246=_tmp242->fields;_tmp247=_tmp242->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp243;struct _tuple0*name=_tmp244;struct Cyc_List_List*tvs=_tmp245;struct Cyc_Core_Opt*fields=_tmp246;int is_x=_tmp247;
Cyc_Absyndump_dumpscope(sc);
if(is_x)
Cyc_Absyndump_dump(({const char*_tmp248="@extensible ";_tag_fat(_tmp248,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp249="datatype ";_tag_fat(_tmp249,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(name);Cyc_Absyndump_dumptvars(tvs);
if(fields != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char((int)'}');}}}struct _tuple21{unsigned f1;struct _tuple0*f2;int f3;};
# 1102
static void Cyc_Absyndump_dumpexport(struct _tuple21*tup){
Cyc_Absyndump_dumpqvar((*tup).f2);}
# 1105
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char((int)'{');
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char((int)'}');}
# 1112
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp24A=d->r;void*_stmttmp18=_tmp24A;void*_tmp24B=_stmttmp18;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24D;struct Cyc_List_List*_tmp24C;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp251;struct _tuple0*_tmp250;struct Cyc_List_List*_tmp253;struct _fat_ptr*_tmp252;struct Cyc_Absyn_Typedefdecl*_tmp254;struct Cyc_Absyn_Exp*_tmp257;struct Cyc_Absyn_Vardecl*_tmp256;struct Cyc_Absyn_Tvar*_tmp255;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Pat*_tmp258;struct Cyc_List_List*_tmp25A;struct Cyc_Absyn_Enumdecl*_tmp25B;struct Cyc_Absyn_Datatypedecl*_tmp25C;struct Cyc_Absyn_Aggrdecl*_tmp25D;struct Cyc_Absyn_Vardecl*_tmp25E;struct Cyc_Absyn_Fndecl*_tmp25F;switch(*((int*)_tmp24B)){case 1U: _LL1: _tmp25F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp25F;
# 1116
{enum Cyc_Cyclone_C_Compilers _tmp260=Cyc_Cyclone_c_compiler;if(_tmp260 == Cyc_Cyclone_Vc_c){_LL24: _LL25:
 Cyc_Absyndump_dumpatts((fd->i).attributes);goto _LL23;}else{_LL26: _LL27:
 goto _LL23;}_LL23:;}
# 1120
if(fd->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp261=Cyc_Cyclone_c_compiler;if(_tmp261 == Cyc_Cyclone_Vc_c){_LL29: _LL2A:
 Cyc_Absyndump_dump(({const char*_tmp262="__inline";_tag_fat(_tmp262,sizeof(char),9U);}));goto _LL28;}else{_LL2B: _LL2C:
 Cyc_Absyndump_dump(({const char*_tmp263="inline";_tag_fat(_tmp263,sizeof(char),7U);}));goto _LL28;}_LL28:;}
# 1125
Cyc_Absyndump_dumpscope(fd->sc);{
# 1127
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->tag=5U,_tmp26A->f1=type_info;_tmp26A;});
if(fd->cached_type != 0){
void*_tmp264=Cyc_Tcutil_compress((void*)_check_null(fd->cached_type));void*_stmttmp19=_tmp264;void*_tmp265=_stmttmp19;struct Cyc_Absyn_FnInfo _tmp266;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp265)->tag == 5U){_LL2E: _tmp266=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp265)->f1;_LL2F: {struct Cyc_Absyn_FnInfo i=_tmp266;
# 1133
({struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp347;});goto _LL2D;}}else{_LL30: _LL31:
({void*_tmp267=0U;({struct _fat_ptr _tmp348=({const char*_tmp268="function has non-function type";_tag_fat(_tmp268,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp348,_tag_fat(_tmp267,sizeof(void*),0U));});});}_LL2D:;}
# 1137
{enum Cyc_Cyclone_C_Compilers _tmp269=Cyc_Cyclone_c_compiler;if(_tmp269 == Cyc_Cyclone_Gcc_c){_LL33: _LL34:
({struct Cyc_Absyn_Tqual _tmp34A=Cyc_Absyn_empty_tqual(0U);void*_tmp349=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp34A,_tmp349,Cyc_Absyndump_dump_atts_qvar,fd);});goto _LL32;}else{_LL35: _LL36:
({struct Cyc_Absyn_Tqual _tmp34C=Cyc_Absyn_empty_tqual(0U);void*_tmp34B=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp34C,_tmp34B,Cyc_Absyndump_dump_callconv_fdqvar,fd);});goto _LL32;}_LL32:;}
# 1141
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(fd->body,0,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}}}case 0U: _LL3: _tmp25E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp25E;
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);goto _LL0;}case 5U: _LL5: _tmp25D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp25D;
Cyc_Absyndump_dump_aggrdecl(ad);Cyc_Absyndump_dump_semi();goto _LL0;}case 6U: _LL7: _tmp25C=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*dd=_tmp25C;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump_semi();goto _LL0;}case 7U: _LL9: _tmp25B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp25B;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump_semi();goto _LL0;}case 3U: _LLB: _tmp25A=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LLC: {struct Cyc_List_List*vds=_tmp25A;
({struct Cyc_List_List*_tmp34F=vds;struct _fat_ptr _tmp34E=({const char*_tmp26B=" let ";_tag_fat(_tmp26B,sizeof(char),6U);});struct _fat_ptr _tmp34D=({const char*_tmp26C=";";_tag_fat(_tmp26C,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpid,_tmp34F,_tmp34E,_tmp34D,({const char*_tmp26D=",";_tag_fat(_tmp26D,sizeof(char),2U);}));});goto _LL0;}case 2U: _LLD: _tmp258=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_tmp259=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp24B)->f3;_LLE: {struct Cyc_Absyn_Pat*p=_tmp258;struct Cyc_Absyn_Exp*e=_tmp259;
# 1151
Cyc_Absyndump_dump(({const char*_tmp26E="let";_tag_fat(_tmp26E,sizeof(char),4U);}));Cyc_Absyndump_dumppat(p);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_semi();goto _LL0;}case 4U: _LLF: _tmp255=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_tmp256=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp24B)->f2;_tmp257=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp24B)->f3;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp255;struct Cyc_Absyn_Vardecl*vd=_tmp256;struct Cyc_Absyn_Exp*open_exp_opt=_tmp257;
# 1153
Cyc_Absyndump_dump(({const char*_tmp26F="region";_tag_fat(_tmp26F,sizeof(char),7U);}));Cyc_Absyndump_dump(({const char*_tmp270="<";_tag_fat(_tmp270,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp271="> ";_tag_fat(_tmp271,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(vd->name);
if((unsigned)open_exp_opt){
Cyc_Absyndump_dump(({const char*_tmp272=" = open(";_tag_fat(_tmp272,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(open_exp_opt);Cyc_Absyndump_dump(({const char*_tmp273=")";_tag_fat(_tmp273,sizeof(char),2U);}));}
# 1157
Cyc_Absyndump_dump_semi();
goto _LL0;}case 8U: _LL11: _tmp254=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp254;
# 1164
Cyc_Absyndump_dump(({const char*_tmp274="typedef";_tag_fat(_tmp274,sizeof(char),8U);}));{
void*t=td->defn == 0?Cyc_Absyn_new_evar(td->kind,0):(void*)_check_null(td->defn);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(td->tq,t,Cyc_Absyndump_dumptypedefname,td);
Cyc_Absyndump_dumpatts(td->atts);
Cyc_Absyndump_dump_semi();
# 1170
goto _LL0;}}case 9U: _LL13: _tmp252=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_tmp253=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp24B)->f2;_LL14: {struct _fat_ptr*v=_tmp252;struct Cyc_List_List*tdl=_tmp253;
# 1172
Cyc_Absyndump_dump(({const char*_tmp275="namespace";_tag_fat(_tmp275,sizeof(char),10U);}));Cyc_Absyndump_dump_str(v);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 10U: _LL15: _tmp250=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_tmp251=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp24B)->f2;_LL16: {struct _tuple0*q=_tmp250;struct Cyc_List_List*tdl=_tmp251;
# 1174
Cyc_Absyndump_dump(({const char*_tmp276="using";_tag_fat(_tmp276,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(q);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 11U: _LL17: _tmp24F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp24F;
# 1176
Cyc_Absyndump_dump(({const char*_tmp277="extern \"C\" ";_tag_fat(_tmp277,sizeof(char),12U);}));Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 12U: _LL19: _tmp24C=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp24B)->f1;_tmp24D=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp24B)->f2;_tmp24E=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp24B)->f3;_LL1A: {struct Cyc_List_List*tdl=_tmp24C;struct Cyc_List_List*ovrs=_tmp24D;struct Cyc_List_List*exs=_tmp24E;
# 1178
Cyc_Absyndump_dump(({const char*_tmp278="extern \"C include\" ";_tag_fat(_tmp278,sizeof(char),20U);}));Cyc_Absyndump_dump_braced_decls(tdl);
if(ovrs != 0){
Cyc_Absyndump_dump(({const char*_tmp279=" cyclone_override ";_tag_fat(_tmp279,sizeof(char),19U);}));
Cyc_Absyndump_dump_braced_decls(ovrs);}
# 1183
({struct Cyc_List_List*_tmp352=exs;struct _fat_ptr _tmp351=({const char*_tmp27A=" export {";_tag_fat(_tmp27A,sizeof(char),10U);});struct _fat_ptr _tmp350=({const char*_tmp27B="}";_tag_fat(_tmp27B,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple21*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpexport,_tmp352,_tmp351,_tmp350,({const char*_tmp27C=",";_tag_fat(_tmp27C,sizeof(char),2U);}));});
goto _LL0;}case 13U: _LL1B: _LL1C:
 Cyc_Absyndump_dump(({const char*_tmp27D=" __cyclone_port_on__; ";_tag_fat(_tmp27D,sizeof(char),23U);}));goto _LL0;case 14U: _LL1D: _LL1E:
 Cyc_Absyndump_dump(({const char*_tmp27E=" __cyclone_port_off__; ";_tag_fat(_tmp27E,sizeof(char),24U);}));goto _LL0;case 15U: _LL1F: _LL20:
 Cyc_Absyndump_dump(({const char*_tmp27F=" __tempest_on__; ";_tag_fat(_tmp27F,sizeof(char),18U);}));goto _LL0;default: _LL21: _LL22:
 Cyc_Absyndump_dump(({const char*_tmp280=" __tempest_off__; ";_tag_fat(_tmp280,sizeof(char),19U);}));goto _LL0;}_LL0:;}}
# 1192
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple13 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != (unsigned)1 || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1199
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1201
if(tms == 0){
f(a);
return;}{
# 1205
void*_tmp281=(void*)tms->hd;void*_stmttmp1A=_tmp281;void*_tmp282=_stmttmp1A;struct Cyc_Absyn_Tqual _tmp287;void*_tmp286;void*_tmp285;void*_tmp284;void*_tmp283;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp282)->tag == 2U){_LL1: _tmp283=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp282)->f1).rgn;_tmp284=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp282)->f1).nullable;_tmp285=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp282)->f1).bounds;_tmp286=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp282)->f1).zero_term;_tmp287=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp282)->f2;_LL2: {void*rgn=_tmp283;void*nullable=_tmp284;void*bd=_tmp285;void*zt=_tmp286;struct Cyc_Absyn_Tqual tq2=_tmp287;
# 1209
{void*_tmp288=Cyc_Tcutil_compress(bd);void*_stmttmp1B=_tmp288;void*_tmp289=_stmttmp1B;struct Cyc_Absyn_Exp*_tmp28A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp289)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp289)->f1)){case 14U: _LL6: _LL7:
 Cyc_Absyndump_dump_char((int)'?');goto _LL5;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp289)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp289)->f2)->hd)->tag == 9U){_LL8: _tmp28A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp289)->f2)->hd)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp28A;
# 1212
Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));Cyc_Absyndump_dump_upperbound(e);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA: _LLB:
# 1214
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));}_LL5:;}
# 1216
if((!Cyc_Absyndump_qvar_to_Cids && !is_char_ptr)&& Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp28B="@zeroterm";_tag_fat(_tmp28B,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids && is_char_ptr)&& !Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp28C="@nozeroterm";_tag_fat(_tmp28C,sizeof(char),12U);}));
{void*_tmp28D=Cyc_Tcutil_compress(rgn);void*_stmttmp1C=_tmp28D;void*_tmp28E=_stmttmp1C;struct Cyc_Absyn_Tvar*_tmp28F;switch(*((int*)_tmp28E)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28E)->f1)){case 5U: _LLD: _LLE:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp290="`H";_tag_fat(_tmp290,sizeof(char),3U);}));goto _LLC;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp291="`U";_tag_fat(_tmp291,sizeof(char),3U);}));goto _LLC;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp292="`RC";_tag_fat(_tmp292,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2U: _LL13: _tmp28F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp28E)->f1;_LL14: {struct Cyc_Absyn_Tvar*tv=_tmp28F;
Cyc_Absyndump_dumptvar(tv);goto _LLC;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28E)->f2 == 0){_LL15: _LL16:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(rgn));goto _LLC;}else{goto _LL19;}case 8U: _LL17: _LL18:
 Cyc_Absyndump_dump(({const char*_tmp293="@region(";_tag_fat(_tmp293,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(rgn);Cyc_Absyndump_dump(({const char*_tmp294=")";_tag_fat(_tmp294,sizeof(char),2U);}));goto _LLC;default: _LL19: _LL1A:
({void*_tmp295=0U;({struct _fat_ptr _tmp353=({const char*_tmp296="dumptms: bad rgn type in Pointer_mod";_tag_fat(_tmp296,sizeof(char),37U);});Cyc_Warn_impos(_tmp353,_tag_fat(_tmp295,sizeof(void*),0U));});});}_LLC:;}
# 1227
Cyc_Absyndump_dumptq(tq2);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}}else{_LL3: _LL4: {
# 1232
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp297=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmp1D=_tmp297;void*_tmp298=_stmttmp1D;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->tag == 2U){_LL1C: _LL1D:
 next_is_pointer=1;goto _LL1B;}else{_LL1E: _LL1F:
 goto _LL1B;}_LL1B:;}
# 1238
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp299=(void*)tms->hd;void*_stmttmp1E=_tmp299;void*_tmp29A=_stmttmp1E;struct Cyc_List_List*_tmp29B;int _tmp29E;unsigned _tmp29D;struct Cyc_List_List*_tmp29C;unsigned _tmp2A0;struct Cyc_List_List*_tmp29F;struct Cyc_Absyn_Exp*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_List_List*_tmp2A5;void*_tmp2A4;struct Cyc_Absyn_VarargInfo*_tmp2A3;int _tmp2A2;struct Cyc_List_List*_tmp2A1;void*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;void*_tmp2AA;switch(*((int*)_tmp29A)){case 0U: _LL21: _tmp2AA=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1;_LL22: {void*zeroterm=_tmp2AA;
# 1245
Cyc_Absyndump_dump(({const char*_tmp2AB="[]";_tag_fat(_tmp2AB,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp2AC="@zeroterm";_tag_fat(_tmp2AC,sizeof(char),10U);}));
goto _LL20;}case 1U: _LL23: _tmp2A8=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1;_tmp2A9=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp29A)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp2A8;void*zeroterm=_tmp2A9;
# 1249
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)']');
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp2AD="@zeroterm";_tag_fat(_tmp2AD,sizeof(char),10U);}));
goto _LL20;}case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->tag == 1U){_LL25: _tmp2A1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f1;_tmp2A2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f2;_tmp2A3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f3;_tmp2A4=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f4;_tmp2A5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f5;_tmp2A6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f6;_tmp2A7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f7;_LL26: {struct Cyc_List_List*args=_tmp2A1;int c_varargs=_tmp2A2;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp2A3;void*effopt=_tmp2A4;struct Cyc_List_List*rgn_po=_tmp2A5;struct Cyc_Absyn_Exp*req=_tmp2A6;struct Cyc_Absyn_Exp*ens=_tmp2A7;
# 1253
Cyc_Absyndump_dumpfunargs(args,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);goto _LL20;}}else{_LL27: _tmp29F=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f1;_tmp2A0=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1)->f2;_LL28: {struct Cyc_List_List*sl=_tmp29F;unsigned loc=_tmp2A0;
# 1255
({struct Cyc_List_List*_tmp356=sl;struct _fat_ptr _tmp355=({const char*_tmp2AE="(";_tag_fat(_tmp2AE,sizeof(char),2U);});struct _fat_ptr _tmp354=({const char*_tmp2AF=")";_tag_fat(_tmp2AF,sizeof(char),2U);});((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp356,_tmp355,_tmp354,({const char*_tmp2B0=",";_tag_fat(_tmp2B0,sizeof(char),2U);}));});goto _LL20;}}case 4U: _LL29: _tmp29C=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp29A)->f1;_tmp29D=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp29A)->f2;_tmp29E=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp29A)->f3;_LL2A: {struct Cyc_List_List*ts=_tmp29C;unsigned loc=_tmp29D;int print_kinds=_tmp29E;
# 1257
if(print_kinds)Cyc_Absyndump_dumpkindedtvars(ts);else{Cyc_Absyndump_dumptvars(ts);}goto _LL20;}case 5U: _LL2B: _tmp29B=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp29A)->f2;_LL2C: {struct Cyc_List_List*atts=_tmp29B;
Cyc_Absyndump_dumpatts(atts);goto _LL20;}default: _LL2D: _LL2E:
({void*_tmp2B1=0U;({struct _fat_ptr _tmp357=({const char*_tmp2B2="dumptms";_tag_fat(_tmp2B2,sizeof(char),8U);});Cyc_Warn_impos(_tmp357,_tag_fat(_tmp2B1,sizeof(void*),0U));});});}_LL20:;}
# 1261
return;}}_LL0:;}}
# 1265
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2B3=Cyc_Absyndump_is_char_ptr(t);int cp=_tmp2B3;
struct _RegionHandle _tmp2B4=_new_region("temp");struct _RegionHandle*temp=& _tmp2B4;_push_region(temp);
{struct _tuple11 _tmp2B5=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _stmttmp1F=_tmp2B5;struct _tuple11 _tmp2B6=_stmttmp1F;struct Cyc_List_List*_tmp2B9;void*_tmp2B8;struct Cyc_Absyn_Tqual _tmp2B7;_LL1: _tmp2B7=_tmp2B6.f1;_tmp2B8=_tmp2B6.f2;_tmp2B9=_tmp2B6.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp2B7;void*t=_tmp2B8;struct Cyc_List_List*tms=_tmp2B9;
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);
({int _tmp35A=cp;struct Cyc_List_List*_tmp359=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);void(*_tmp358)(void*)=f;Cyc_Absyndump_dumptms(_tmp35A,_tmp359,_tmp358,a);});}}
# 1268
;_pop_region(temp);}
# 1274
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1276
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2BA=0U;({struct Cyc___cycFILE*_tmp35C=f;struct _fat_ptr _tmp35B=({const char*_tmp2BB="\n";_tag_fat(_tmp2BB,sizeof(char),2U);});Cyc_fprintf(_tmp35C,_tmp35B,_tag_fat(_tmp2BA,sizeof(void*),0U));});});}
# 1282
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2BC=d->r;void*_stmttmp20=_tmp2BC;void*_tmp2BD=_stmttmp20;struct Cyc_List_List*_tmp2C0;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2BE;struct Cyc_List_List*_tmp2C1;struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*_tmp2C4;struct _fat_ptr*_tmp2C3;struct Cyc_Absyn_Typedefdecl*_tmp2C5;struct Cyc_Absyn_Enumdecl*_tmp2C6;struct Cyc_Absyn_Datatypedecl*_tmp2C7;struct Cyc_Absyn_Aggrdecl*_tmp2C8;struct Cyc_Absyn_Fndecl*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CA;switch(*((int*)_tmp2BD)){case 0U: _LL1: _tmp2CA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp2CA;
# 1285
if((int)vd->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=vd->initializer;
vd->initializer=0;
if((int)vd->sc == (int)Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2CB="extern ";_tag_fat(_tmp2CB,sizeof(char),8U);}));
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2CC="\n";_tag_fat(_tmp2CC,sizeof(char),2U);}));
vd->initializer=init;
goto _LL0;}}case 1U: _LL3: _tmp2C9=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp2C9;
# 1295
if((int)fd->sc == (int)Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(fd->sc);{
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->tag=5U,_tmp2CE->f1=type_info;_tmp2CE;});
({struct Cyc_Absyn_Tqual _tmp35E=Cyc_Absyn_empty_tqual(0U);void*_tmp35D=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp35E,_tmp35D,Cyc_Absyndump_dump_atts_qvar,fd);});
Cyc_Absyndump_dump(({const char*_tmp2CD=";\n";_tag_fat(_tmp2CD,sizeof(char),3U);}));
goto _LL0;}}}case 5U: _LL5: _tmp2C8=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2C8;
# 1304
if((int)ad->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if((int)ad->sc == (int)Cyc_Absyn_Abstract)ad->impl=0;
Cyc_Absyndump_dump_aggrdecl(ad);
ad->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2CF=";\n";_tag_fat(_tmp2CF,sizeof(char),3U);}));
goto _LL0;}}case 6U: _LL7: _tmp2C7=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*dd=_tmp2C7;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump(({const char*_tmp2D0=";\n";_tag_fat(_tmp2D0,sizeof(char),3U);}));goto _LL0;}case 7U: _LL9: _tmp2C6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp2C6;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump(({const char*_tmp2D1=";\n";_tag_fat(_tmp2D1,sizeof(char),3U);}));goto _LL0;}case 8U: _LLB: _tmp2C5=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp2C5;
# 1314
if(td->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2D2="\n";_tag_fat(_tmp2D2,sizeof(char),2U);}));}
# 1318
goto _LL0;}case 9U: _LLD: _tmp2C3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_tmp2C4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2BD)->f2;_LLE: {struct _fat_ptr*v=_tmp2C3;struct Cyc_List_List*ds=_tmp2C4;
# 1320
Cyc_Absyndump_dump(({const char*_tmp2D3="namespace ";_tag_fat(_tmp2D3,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(v);
Cyc_Absyndump_dump(({const char*_tmp2D4="{\n";_tag_fat(_tmp2D4,sizeof(char),3U);}));
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp2D5="}\n";_tag_fat(_tmp2D5,sizeof(char),3U);}));
goto _LL0;}case 10U: _LLF: _tmp2C2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2BD)->f2;_LL10: {struct Cyc_List_List*ds=_tmp2C2;
# 1328
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
goto _LL0;}case 11U: _LL11: _tmp2C1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_LL12: {struct Cyc_List_List*ds=_tmp2C1;
# 1332
Cyc_Absyndump_dump(({const char*_tmp2D6="extern \"C\" {";_tag_fat(_tmp2D6,sizeof(char),13U);}));
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp2D7="}\n";_tag_fat(_tmp2D7,sizeof(char),3U);}));
goto _LL0;}case 12U: _LL13: _tmp2BE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2BD)->f1;_tmp2BF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2BD)->f2;_tmp2C0=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2BD)->f3;_LL14: {struct Cyc_List_List*ds=_tmp2BE;struct Cyc_List_List*ovrs=_tmp2BF;struct Cyc_List_List*exs=_tmp2C0;
# 1353 "absyndump.cyc"
goto _LL0;}default: _LL15: _LL16:
({void*_tmp2D8=0U;({struct _fat_ptr _tmp35F=({const char*_tmp2D9="bad top-level declaration";_tag_fat(_tmp2D9,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp35F,_tag_fat(_tmp2D8,sizeof(void*),0U));});});}_LL0:;}
# 1358
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}

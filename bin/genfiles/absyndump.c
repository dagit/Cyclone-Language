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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 693 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 885
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 890
void*Cyc_Absyn_compress_kb(void*);
# 896
int Cyc_Absyn_type2bool(int def,void*);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1132
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1142
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1150
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 62
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 79
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple13{int f1;struct Cyc_List_List*f2;};
# 94 "absynpp.h"
struct _tuple13 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 107 "tcutil.h"
void*Cyc_Tcutil_compress(void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple15{struct _fat_ptr f1;unsigned f2;};
# 27 "absyndump.cyc"
struct _tuple15 Cyc_Lex_xlate_pos(unsigned);
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
struct _tuple15 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple15 _stmttmp0=_tmp1;struct _tuple15 _tmp2=_stmttmp0;unsigned _tmp4;struct _fat_ptr _tmp3;_LL1: _tmp3=_tmp2.f1;_tmp4=_tmp2.f2;_LL2: {struct _fat_ptr f=_tmp3;unsigned d=_tmp4;
if(({char*_tmp2F8=(char*)f.curr;_tmp2F8 == (char*)(_tag_fat(0,0,0)).curr;})||(char*)f.curr == (char*)last_file.curr && d == last_line)return;
if((char*)f.curr == (char*)last_file.curr && d == last_line + (unsigned)1)
({void*_tmp5=0U;({struct Cyc___cycFILE*_tmp2FA=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp2F9=({const char*_tmp6="\n";_tag_fat(_tmp6,sizeof(char),2U);});Cyc_fprintf(_tmp2FA,_tmp2F9,_tag_fat(_tmp5,sizeof(void*),0U));});});else{
if((char*)f.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp9=({struct Cyc_Int_pa_PrintArg_struct _tmp2EB;_tmp2EB.tag=1U,_tmp2EB.f1=(unsigned long)((int)d);_tmp2EB;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({struct Cyc___cycFILE*_tmp2FC=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp2FB=({const char*_tmp8="\n# %d\n";_tag_fat(_tmp8,sizeof(char),7U);});Cyc_fprintf(_tmp2FC,_tmp2FB,_tag_fat(_tmp7,sizeof(void*),1U));});});else{
# 158
({struct Cyc_Int_pa_PrintArg_struct _tmpC=({struct Cyc_Int_pa_PrintArg_struct _tmp2ED;_tmp2ED.tag=1U,_tmp2ED.f1=(unsigned long)((int)d);_tmp2ED;});struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp2EC;_tmp2EC.tag=0U,_tmp2EC.f1=(struct _fat_ptr)((struct _fat_ptr)f);_tmp2EC;});void*_tmpA[2U];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD;({struct Cyc___cycFILE*_tmp2FE=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp2FD=({const char*_tmpB="\n# %d %s\n";_tag_fat(_tmpB,sizeof(char),10U);});Cyc_fprintf(_tmp2FE,_tmp2FD,_tag_fat(_tmpA,sizeof(void*),2U));});});}}
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
({struct Cyc_List_List*_tmp301=ts;struct _fat_ptr _tmp300=({const char*_tmp23="<";_tag_fat(_tmp23,sizeof(char),2U);});struct _fat_ptr _tmp2FF=({const char*_tmp24=">";_tag_fat(_tmp24,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp301,_tmp300,_tmp2FF,({const char*_tmp25=",";_tag_fat(_tmp25,sizeof(char),2U);}));});}
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
({struct Cyc_List_List*_tmp304=tvs;struct _fat_ptr _tmp303=({const char*_tmp30="<";_tag_fat(_tmp30,sizeof(char),2U);});struct _fat_ptr _tmp302=({const char*_tmp31=">";_tag_fat(_tmp31,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp304,_tmp303,_tmp302,({const char*_tmp32=",";_tag_fat(_tmp32,sizeof(char),2U);}));});}
# 295
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp307=tvs;struct _fat_ptr _tmp306=({const char*_tmp33="<";_tag_fat(_tmp33,sizeof(char),2U);});struct _fat_ptr _tmp305=({const char*_tmp34=">";_tag_fat(_tmp34,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp307,_tmp306,_tmp305,({const char*_tmp35=",";_tag_fat(_tmp35,sizeof(char),2U);}));});}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 298
static void Cyc_Absyndump_dumparg(struct _tuple16*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 301
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp30A=ts;struct _fat_ptr _tmp309=({const char*_tmp36="(";_tag_fat(_tmp36,sizeof(char),2U);});struct _fat_ptr _tmp308=({const char*_tmp37=")";_tag_fat(_tmp37,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp30A,_tmp309,_tmp308,({const char*_tmp38=",";_tag_fat(_tmp38,sizeof(char),2U);}));});}
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
Cyc_Absyndump_dumpntyp(t);}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 359
static struct _tuple17 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
{void*_tmp47=Cyc_Tcutil_compress(t);void*_stmttmp7=_tmp47;void*_tmp48=_stmttmp7;struct Cyc_List_List*_tmp49;void*_tmp4A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2 != 0){_LL1: _tmp4A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2)->hd;_LL2: {void*r=_tmp4A;
rgions=({struct Cyc_List_List*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->hd=r,_tmp4B->tl=rgions;_tmp4B;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp49=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2;_LL4: {struct Cyc_List_List*ts=_tmp49;
# 365
for(0;ts != 0;ts=ts->tl){
struct _tuple17 _tmp4C=Cyc_Absyndump_effects_split((void*)ts->hd);struct _tuple17 _stmttmp8=_tmp4C;struct _tuple17 _tmp4D=_stmttmp8;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp4E;_LL8: _tmp4E=_tmp4D.f1;_tmp4F=_tmp4D.f2;_LL9: {struct Cyc_List_List*rs=_tmp4E;struct Cyc_List_List*es=_tmp4F;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(rs,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,effects);}}
# 370
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 effects=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=t,_tmp50->tl=effects;_tmp50;});goto _LL0;}_LL0:;}
# 373
return({struct _tuple17 _tmp2EE;_tmp2EE.f1=rgions,_tmp2EE.f2=effects;_tmp2EE;});}
# 376
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple17 _tmp51=Cyc_Absyndump_effects_split(t);struct _tuple17 _stmttmp9=_tmp51;struct _tuple17 _tmp52=_stmttmp9;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp53;_LL1: _tmp53=_tmp52.f1;_tmp54=_tmp52.f2;_LL2: {struct Cyc_List_List*rgions=_tmp53;struct Cyc_List_List*effects=_tmp54;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(effects != 0){
({struct Cyc_List_List*_tmp30D=effects;struct _fat_ptr _tmp30C=({const char*_tmp55="";_tag_fat(_tmp55,sizeof(char),1U);});struct _fat_ptr _tmp30B=({const char*_tmp56="";_tag_fat(_tmp56,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpntyp,_tmp30D,_tmp30C,_tmp30B,({const char*_tmp57="+";_tag_fat(_tmp57,sizeof(char),2U);}));});
if(rgions != 0)Cyc_Absyndump_dump_char((int)'+');}
# 384
if(rgions != 0 || effects == 0)
({struct Cyc_List_List*_tmp310=rgions;struct _fat_ptr _tmp30F=({const char*_tmp58="{";_tag_fat(_tmp58,sizeof(char),2U);});struct _fat_ptr _tmp30E=({const char*_tmp59="}";_tag_fat(_tmp59,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumprgn,_tmp310,_tmp30F,_tmp30E,({const char*_tmp5A=",";_tag_fat(_tmp5A,sizeof(char),2U);}));});}}
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
struct _tuple11 _tmpA2=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmpA=_tmpA2;struct _tuple11 _tmpA3=_stmttmpA;struct _tuple0*_tmpA5;enum Cyc_Absyn_AggrKind _tmpA4;_LL7F: _tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;_LL80: {enum Cyc_Absyn_AggrKind k=_tmpA4;struct _tuple0*n=_tmpA5;
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
({struct Cyc_List_List*_tmp313=ts;struct _fat_ptr _tmp312=({const char*_tmpB4="";_tag_fat(_tmpB4,sizeof(char),1U);});struct _fat_ptr _tmp311=({const char*_tmpB5="";_tag_fat(_tmpB5,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumptyp,_tmp313,_tmp312,_tmp311,({const char*_tmpB6="+";_tag_fat(_tmpB6,sizeof(char),2U);}));});return;case 11U: _LL5E: _LL5F:
 Cyc_Absyndump_dump(({const char*_tmpB7="@true";_tag_fat(_tmpB7,sizeof(char),6U);}));return;case 12U: _LL60: _LL61:
 Cyc_Absyndump_dump(({const char*_tmpB8="@false";_tag_fat(_tmpB8,sizeof(char),7U);}));return;case 13U: _LL62: _LL63:
 Cyc_Absyndump_dump(({const char*_tmpB9="@thin";_tag_fat(_tmpB9,sizeof(char),6U);}));return;case 14U: _LL64: _LL65:
 Cyc_Absyndump_dump(({const char*_tmpBA="@fat";_tag_fat(_tmpBA,sizeof(char),5U);}));return;default: _LL66: _LL67:
({void*_tmpBB=0U;({struct _fat_ptr _tmp314=({const char*_tmpBC="bad type constructor";_tag_fat(_tmpBC,sizeof(char),21U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp314,_tag_fat(_tmpBB,sizeof(void*),0U));});});}_LL21:;}}case 2U: _LL9: _tmp6B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5B)->f1;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp6B;
# 396
Cyc_Absyndump_dumptvar(tv);return;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f2 == 0){_LLB: _tmp69=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f1;_tmp6A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f3;_LLC: {struct Cyc_Core_Opt*k=_tmp69;int i=_tmp6A;
# 398
Cyc_Absyndump_dump(({const char*_tmp6F="`E";_tag_fat(_tmp6F,sizeof(char),3U);}));
if(k == 0)Cyc_Absyndump_dump(({const char*_tmp70="K";_tag_fat(_tmp70,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)k->v);}
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp73=({struct Cyc_Int_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=1U,_tmp2EF.f1=(unsigned long)i;_tmp2EF;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({struct _fat_ptr _tmp315=({const char*_tmp72="%d";_tag_fat(_tmp72,sizeof(char),3U);});Cyc_aprintf(_tmp315,_tag_fat(_tmp71,sizeof(void*),1U));});}));return;}}else{_LLD: _tmp66=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f1;_tmp67=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f2;_tmp68=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f3;_LLE: {struct Cyc_Core_Opt*k=_tmp66;void*t=_tmp67;int i=_tmp68;
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
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple18{void*f1;void*f2;};
# 489
static void Cyc_Absyndump_dump_rgncmp(struct _tuple18*cmp){
Cyc_Absyndump_dumpeff((*cmp).f1);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn((*cmp).f2);}
# 492
static void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp316=rgn_po;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp316,({const char*_tmpBD=",";_tag_fat(_tmpBD,sizeof(char),2U);}));});}
# 495
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 499
({struct Cyc_List_List*_tmp319=args;struct _fat_ptr _tmp318=({const char*_tmpBE="(";_tag_fat(_tmpBE,sizeof(char),2U);});struct _fat_ptr _tmp317=({const char*_tmpBF="";_tag_fat(_tmpBF,sizeof(char),1U);});((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpfunarg,_tmp319,_tmp318,_tmp317,({const char*_tmpC0=",";_tag_fat(_tmpC0,sizeof(char),2U);}));});
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
({struct Cyc_Absyn_Tqual _tmp31A=Cyc_Absyn_empty_tqual(0U);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp31A,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 525
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpC6=d;struct _fat_ptr*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC6)->tag == 0U){_LL1: _tmpC8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC6)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpC8;
Cyc_Absyndump_dump(({const char*_tmpC9=".[";_tag_fat(_tmpC9,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)']');goto _LL0;}}else{_LL3: _tmpC7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpC6)->f1;_LL4: {struct _fat_ptr*v=_tmpC7;
Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*v);goto _LL0;}}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 531
static void Cyc_Absyndump_dumpde(struct _tuple19*de){
({struct Cyc_List_List*_tmp31D=(*de).f1;struct _fat_ptr _tmp31C=({const char*_tmpCA="";_tag_fat(_tmpCA,sizeof(char),1U);});struct _fat_ptr _tmp31B=({const char*_tmpCB="=";_tag_fat(_tmpCB,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp31D,_tmp31C,_tmp31B,({const char*_tmpCC="=";_tag_fat(_tmpCC,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 536
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_tmpCD=f;unsigned _tmpCE;struct _fat_ptr*_tmpCF;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpCD)->tag == 0U){_LL1: _tmpCF=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpCD)->f1;_LL2: {struct _fat_ptr*n=_tmpCF;
Cyc_Absyndump_dump_nospace(*n);return;}}else{_LL3: _tmpCE=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpCD)->f1;_LL4: {unsigned n=_tmpCE;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD2=({struct Cyc_Int_pa_PrintArg_struct _tmp2F0;_tmp2F0.tag=1U,_tmp2F0.f1=(unsigned long)((int)n);_tmp2F0;});void*_tmpD0[1U];_tmpD0[0]=& _tmpD2;({struct _fat_ptr _tmp31E=({const char*_tmpD1="%d";_tag_fat(_tmpD1,sizeof(char),3U);});Cyc_aprintf(_tmp31E,_tag_fat(_tmpD0,sizeof(void*),1U));});}));return;}}_LL0:;}struct _tuple20{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 543
static void Cyc_Absyndump_dump_asm_iolist(struct Cyc_List_List*iolist){
while((unsigned)iolist){
struct _tuple20*_tmpD3=(struct _tuple20*)iolist->hd;struct _tuple20*_stmttmpB=_tmpD3;struct _tuple20*_tmpD4=_stmttmpB;struct Cyc_Absyn_Exp*_tmpD6;struct _fat_ptr _tmpD5;_LL1: _tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;_LL2: {struct _fat_ptr constr=_tmpD5;struct Cyc_Absyn_Exp*e1=_tmpD6;
Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(constr);Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');
iolist=iolist->tl;
if((unsigned)iolist)
Cyc_Absyndump_dump_char((int)',');}}}
# 554
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp322=inprec;struct Cyc_List_List*_tmp321=es;struct _fat_ptr _tmp320=({const char*_tmpD7="";_tag_fat(_tmpD7,sizeof(char),1U);});struct _fat_ptr _tmp31F=({const char*_tmpD8="";_tag_fat(_tmpD8,sizeof(char),1U);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp322,_tmp321,_tmp320,_tmp31F,({const char*_tmpD9=",";_tag_fat(_tmpD9,sizeof(char),2U);}));});}
# 558
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpDA="(";_tag_fat(_tmpDA,sizeof(char),2U);}));
{void*_tmpDB=e->r;void*_stmttmpC=_tmpDB;void*_tmpDC=_stmttmpC;struct Cyc_Absyn_Stmt*_tmpDD;struct Cyc_List_List*_tmpDF;struct Cyc_Core_Opt*_tmpDE;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;int _tmpE6;struct Cyc_Absyn_Exp*_tmpE5;void**_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;int _tmpE2;struct Cyc_Absyn_Datatypefield*_tmpE8;struct Cyc_List_List*_tmpE7;struct Cyc_Absyn_Enumfield*_tmpE9;struct Cyc_Absyn_Enumfield*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpED;struct _tuple0*_tmpEC;void*_tmpF0;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Vardecl*_tmpF1;struct Cyc_List_List*_tmpF4;struct Cyc_List_List*_tmpF6;struct _tuple8*_tmpF5;struct Cyc_List_List*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;struct _fat_ptr*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;struct _fat_ptr*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_List_List*_tmpFF;void*_tmpFE;struct _fat_ptr*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp105;struct Cyc_List_List*_tmp104;struct _fat_ptr _tmp103;int _tmp102;struct Cyc_Absyn_Exp*_tmp107;void*_tmp108;struct Cyc_Absyn_Exp*_tmp109;void*_tmp10A;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp110;void*_tmp10F;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Core_Opt*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_List_List*_tmp127;enum Cyc_Absyn_Primop _tmp126;struct _fat_ptr _tmp128;void*_tmp129;struct _fat_ptr _tmp12A;struct _fat_ptr _tmp12B;struct _fat_ptr _tmp12C;long long _tmp12E;enum Cyc_Absyn_Sign _tmp12D;int _tmp12F;int _tmp130;int _tmp131;short _tmp133;enum Cyc_Absyn_Sign _tmp132;struct _fat_ptr _tmp134;char _tmp136;enum Cyc_Absyn_Sign _tmp135;switch(*((int*)_tmpDC)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).String_c).tag){case 2U: _LL1: _tmp135=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Char_c).val).f1;_tmp136=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp135;char ch=_tmp136;
# 564
Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_char((int)'\'');
goto _LL0;}case 3U: _LL3: _tmp134=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp134;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp139=({struct Cyc_String_pa_PrintArg_struct _tmp2F1;_tmp2F1.tag=0U,_tmp2F1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2F1;});void*_tmp137[1U];_tmp137[0]=& _tmp139;({struct _fat_ptr _tmp323=({const char*_tmp138="L'%s'";_tag_fat(_tmp138,sizeof(char),6U);});Cyc_aprintf(_tmp323,_tag_fat(_tmp137,sizeof(void*),1U));});}));goto _LL0;}case 4U: _LL5: _tmp132=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Short_c).val).f1;_tmp133=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp132;short s=_tmp133;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp13C=({struct Cyc_Int_pa_PrintArg_struct _tmp2F2;_tmp2F2.tag=1U,_tmp2F2.f1=(unsigned long)((int)s);_tmp2F2;});void*_tmp13A[1U];_tmp13A[0]=& _tmp13C;({struct _fat_ptr _tmp324=({const char*_tmp13B="%d";_tag_fat(_tmp13B,sizeof(char),3U);});Cyc_aprintf(_tmp324,_tag_fat(_tmp13A,sizeof(void*),1U));});}));goto _LL0;}case 5U: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL7: _tmp131=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Int_c).val).f2;_LL8: {int i=_tmp131;
_tmp130=i;goto _LLA;}case Cyc_Absyn_Signed: _LL9: _tmp130=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Int_c).val).f2;_LLA: {int i=_tmp130;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp13F=({struct Cyc_Int_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=1U,_tmp2F3.f1=(unsigned long)i;_tmp2F3;});void*_tmp13D[1U];_tmp13D[0]=& _tmp13F;({struct _fat_ptr _tmp325=({const char*_tmp13E="%d";_tag_fat(_tmp13E,sizeof(char),3U);});Cyc_aprintf(_tmp325,_tag_fat(_tmp13D,sizeof(void*),1U));});}));goto _LL0;}default: _LLB: _tmp12F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Int_c).val).f2;_LLC: {int i=_tmp12F;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp142=({struct Cyc_Int_pa_PrintArg_struct _tmp2F4;_tmp2F4.tag=1U,_tmp2F4.f1=(unsigned)i;_tmp2F4;});void*_tmp140[1U];_tmp140[0]=& _tmp142;({struct _fat_ptr _tmp326=({const char*_tmp141="%uU";_tag_fat(_tmp141,sizeof(char),4U);});Cyc_aprintf(_tmp326,_tag_fat(_tmp140,sizeof(void*),1U));});}));goto _LL0;}}case 6U: _LLD: _tmp12D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).LongLong_c).val).f1;_tmp12E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).LongLong_c).val).f2;_LLE: {enum Cyc_Absyn_Sign sg=_tmp12D;long long i=_tmp12E;
# 573
Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)i));goto _LL0;}case 7U: _LLF: _tmp12C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Float_c).val).f1;_LL10: {struct _fat_ptr x=_tmp12C;
Cyc_Absyndump_dump(x);goto _LL0;}case 1U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp143="NULL";_tag_fat(_tmp143,sizeof(char),5U);}));goto _LL0;case 8U: _LL13: _tmp12B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).String_c).val;_LL14: {struct _fat_ptr s=_tmp12B;
# 577
Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(s));Cyc_Absyndump_dump_char((int)'"');
goto _LL0;}default: _LL15: _tmp12A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).Wstring_c).val;_LL16: {struct _fat_ptr s=_tmp12A;
# 580
Cyc_Absyndump_dump(({const char*_tmp144="L\"";_tag_fat(_tmp144,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(s);Cyc_Absyndump_dump_char((int)'"');goto _LL0;}}case 1U: _LL17: _tmp129=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL18: {void*b=_tmp129;
# 582
Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL19: _tmp128=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL1A: {struct _fat_ptr p=_tmp128;
# 585
Cyc_Absyndump_dump(({const char*_tmp145="__cyclone_pragma__(";_tag_fat(_tmp145,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(p);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 3U: _LL1B: _tmp126=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp127=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL1C: {enum Cyc_Absyn_Primop p=_tmp126;struct Cyc_List_List*es=_tmp127;
# 588
struct _fat_ptr _tmp146=Cyc_Absynpp_prim2str(p);struct _fat_ptr pstr=_tmp146;
{int _tmp147=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpD=_tmp147;int _tmp148=_stmttmpD;switch(_tmp148){case 1U: _LL70: _LL71:
# 591
 if((int)p == (int)18U){
Cyc_Absyndump_dump(({const char*_tmp149="numelts(";_tag_fat(_tmp149,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(({const char*_tmp14A=")";_tag_fat(_tmp14A,sizeof(char),2U);}));}else{
# 596
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);}
# 599
goto _LL6F;case 2U: _LL72: _LL73:
# 601
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);
goto _LL6F;default: _LL74: _LL75:
({void*_tmp14B=0U;({struct _fat_ptr _tmp327=({const char*_tmp14C="Absyndump -- Bad number of arguments to primop";_tag_fat(_tmp14C,sizeof(char),47U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp327,_tag_fat(_tmp14B,sizeof(void*),0U));});});}_LL6F:;}
# 608
goto _LL0;}case 4U: _LL1D: _tmp123=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp124=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp125=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp123;struct Cyc_Core_Opt*popt=_tmp124;struct Cyc_Absyn_Exp*e2=_tmp125;
# 611
Cyc_Absyndump_dumpexp_prec(myprec,e1);
if(popt != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)popt->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp14D="=";_tag_fat(_tmp14D,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,e2);
goto _LL0;}case 5U: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDC)->f2){case Cyc_Absyn_PreInc: _LL1F: _tmp122=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp122;
# 618
Cyc_Absyndump_dump(({const char*_tmp14E="++";_tag_fat(_tmp14E,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PreDec: _LL21: _tmp121=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL22: {struct Cyc_Absyn_Exp*e2=_tmp121;
Cyc_Absyndump_dump(({const char*_tmp14F="--";_tag_fat(_tmp14F,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PostInc: _LL23: _tmp120=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL24: {struct Cyc_Absyn_Exp*e2=_tmp120;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmp150="++";_tag_fat(_tmp150,sizeof(char),3U);}));goto _LL0;}default: _LL25: _tmp11F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL26: {struct Cyc_Absyn_Exp*e2=_tmp11F;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmp151="--";_tag_fat(_tmp151,sizeof(char),3U);}));goto _LL0;}}case 6U: _LL27: _tmp11C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp11D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp11E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp11C;struct Cyc_Absyn_Exp*e2=_tmp11D;struct Cyc_Absyn_Exp*e3=_tmp11E;
# 624
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,e2);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,e3);
goto _LL0;}case 7U: _LL29: _tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 631
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmp152="&&";_tag_fat(_tmp152,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 8U: _LL2B: _tmp118=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp119=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp118;struct Cyc_Absyn_Exp*e2=_tmp119;
# 635
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmp153="||";_tag_fat(_tmp153,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 9U: _LL2D: _tmp116=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp117=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp116;struct Cyc_Absyn_Exp*e2=_tmp117;
# 641
Cyc_Absyndump_dumploc(e->loc);
# 644
Cyc_Absyndump_dumpexp_prec(myprec - 1,e1);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec - 1,e2);
goto _LL0;}case 10U: _LL2F: _tmp114=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp115=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_List_List*es=_tmp115;
# 648
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_nospace(({const char*_tmp154="(";_tag_fat(_tmp154,sizeof(char),2U);}));{
# 653
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives && !(e->loc == (unsigned)0))&& !(e1->loc == (unsigned)0);
Cyc_Absyndump_dumpexps_prec(20,es);
Cyc_Absyndump_dump_nospace(({const char*_tmp155=")";_tag_fat(_tmp155,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;}}case 11U: _LL31: _tmp113=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp113;
# 662
Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(({const char*_tmp156="throw";_tag_fat(_tmp156,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 12U: _LL33: _tmp112=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp112;
# 664
_tmp111=e1;goto _LL36;}case 13U: _LL35: _tmp111=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp111;
Cyc_Absyndump_dumpexp_prec(inprec,e1);goto _LL0;}case 14U: _LL37: _tmp10F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp110=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL38: {void*t=_tmp10F;struct Cyc_Absyn_Exp*e1=_tmp110;
# 668
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,e1);
goto _LL0;}case 15U: _LL39: _tmp10E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp10E;
# 671
Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 20U: _LL3B: _tmp10D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmp10D;
Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 16U: _LL3D: _tmp10B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp10C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL3E: {struct Cyc_Absyn_Exp*ropt=_tmp10B;struct Cyc_Absyn_Exp*e1=_tmp10C;
Cyc_Absyndump_dump(({const char*_tmp157="new";_tag_fat(_tmp157,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 17U: _LL3F: _tmp10A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL40: {void*t=_tmp10A;
Cyc_Absyndump_dump(({const char*_tmp158="sizeof(";_tag_fat(_tmp158,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 18U: _LL41: _tmp109=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL42: {struct Cyc_Absyn_Exp*e1=_tmp109;
Cyc_Absyndump_dump(({const char*_tmp159="sizeof(";_tag_fat(_tmp159,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 39U: _LL43: _tmp108=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL44: {void*t=_tmp108;
Cyc_Absyndump_dump(({const char*_tmp15A="valueof(";_tag_fat(_tmp15A,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 41U: _LL45: _tmp107=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL46: {struct Cyc_Absyn_Exp*e1=_tmp107;
# 679
Cyc_Absyndump_dump(({const char*_tmp15B="__extension__(";_tag_fat(_tmp15B,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 40U: _LL47: _tmp102=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp103=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp104=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_tmp105=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_tmp106=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDC)->f5;_LL48: {int vol=_tmp102;struct _fat_ptr t=_tmp103;struct Cyc_List_List*o=_tmp104;struct Cyc_List_List*i=_tmp105;struct Cyc_List_List*cl=_tmp106;
# 682
Cyc_Absyndump_dump(({const char*_tmp15C="__asm__";_tag_fat(_tmp15C,sizeof(char),8U);}));
if(vol)Cyc_Absyndump_dump(({const char*_tmp15D=" volatile ";_tag_fat(_tmp15D,sizeof(char),11U);}));
Cyc_Absyndump_dump_nospace(({const char*_tmp15E="(\"";_tag_fat(_tmp15E,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(t);Cyc_Absyndump_dump_char((int)'"');
if((unsigned)o){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_asm_iolist(o);}
# 689
if((unsigned)i){
if(!((unsigned)o))
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_asm_iolist(i);}
# 695
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
{int cols=0;for(0;cols < 3 - ncol;++ cols){
Cyc_Absyndump_dump_nospace(({const char*_tmp15F=" :";_tag_fat(_tmp15F,sizeof(char),3U);}));}}
while((unsigned)cl){
Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(*((struct _fat_ptr*)cl->hd));
Cyc_Absyndump_dump_char((int)'"');
cl=cl->tl;
if((unsigned)cl)
Cyc_Absyndump_dump_char((int)',');}}
# 708
Cyc_Absyndump_dump_char((int)')');
goto _LL0;}case 38U: _LL49: _tmp100=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp101=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL4A: {struct Cyc_Absyn_Exp*e=_tmp100;struct _fat_ptr*f=_tmp101;
# 712
Cyc_Absyndump_dump(({const char*_tmp160="tagcheck(";_tag_fat(_tmp160,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*f);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;}case 19U: _LL4B: _tmpFE=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpFF=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL4C: {void*t=_tmpFE;struct Cyc_List_List*l=_tmpFF;
# 717
Cyc_Absyndump_dump(({const char*_tmp161="offsetof(";_tag_fat(_tmp161,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(t);({struct Cyc_List_List*_tmp32A=l;struct _fat_ptr _tmp329=({const char*_tmp162=",";_tag_fat(_tmp162,sizeof(char),2U);});struct _fat_ptr _tmp328=({const char*_tmp163=")";_tag_fat(_tmp163,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpoffset_field,_tmp32A,_tmp329,_tmp328,({const char*_tmp164=".";_tag_fat(_tmp164,sizeof(char),2U);}));});goto _LL0;}case 21U: _LL4D: _tmpFC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpFD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmpFC;struct _fat_ptr*n=_tmpFD;
# 720
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 22U: _LL4F: _tmpFA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpFB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL50: {struct Cyc_Absyn_Exp*e1=_tmpFA;struct _fat_ptr*n=_tmpFB;
# 722
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp165="->";_tag_fat(_tmp165,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 23U: _LL51: _tmpF8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpF9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpF8;struct Cyc_Absyn_Exp*e2=_tmpF9;
# 725
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char((int)']');goto _LL0;}case 24U: _LL53: _tmpF7=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL54: {struct Cyc_List_List*es=_tmpF7;
# 727
Cyc_Absyndump_dump(({const char*_tmp166="$(";_tag_fat(_tmp166,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 25U: _LL55: _tmpF5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpF6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL56: {struct _tuple8*vat=_tmpF5;struct Cyc_List_List*des=_tmpF6;
# 730
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*vat).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp32D=des;struct _fat_ptr _tmp32C=({const char*_tmp167="{";_tag_fat(_tmp167,sizeof(char),2U);});struct _fat_ptr _tmp32B=({const char*_tmp168="}";_tag_fat(_tmp168,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp32D,_tmp32C,_tmp32B,({const char*_tmp169=",";_tag_fat(_tmp169,sizeof(char),2U);}));});
goto _LL0;}case 26U: _LL57: _tmpF4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL58: {struct Cyc_List_List*des=_tmpF4;
# 736
({struct Cyc_List_List*_tmp330=des;struct _fat_ptr _tmp32F=({const char*_tmp16A="{";_tag_fat(_tmp16A,sizeof(char),2U);});struct _fat_ptr _tmp32E=({const char*_tmp16B="}";_tag_fat(_tmp16B,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp330,_tmp32F,_tmp32E,({const char*_tmp16C=",";_tag_fat(_tmp16C,sizeof(char),2U);}));});goto _LL0;}case 27U: _LL59: _tmpF1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpF2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmpF3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL5A: {struct Cyc_Absyn_Vardecl*vd=_tmpF1;struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_Absyn_Exp*e2=_tmpF3;
# 739
Cyc_Absyndump_dump(({const char*_tmp16D="{for";_tag_fat(_tmp16D,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*vd->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(e1);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 28U: _LL5B: _tmpEF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpF0=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL5C: {struct Cyc_Absyn_Exp*e=_tmpEF;void*t=_tmpF0;
# 744
Cyc_Absyndump_dump(({const char*_tmp16E="{for x ";_tag_fat(_tmp16E,sizeof(char),8U);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(e);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 29U: _LL5D: _tmpEC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpED=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmpEE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL5E: {struct _tuple0*n=_tmpEC;struct Cyc_List_List*ts=_tmpED;struct Cyc_List_List*des=_tmpEE;
# 749
Cyc_Absyndump_dumpqvar(n);
Cyc_Absyndump_dump_char((int)'{');
if(ts != 0)
Cyc_Absyndump_dumptps(ts);
({struct Cyc_List_List*_tmp333=des;struct _fat_ptr _tmp332=({const char*_tmp16F="";_tag_fat(_tmp16F,sizeof(char),1U);});struct _fat_ptr _tmp331=({const char*_tmp170="}";_tag_fat(_tmp170,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp333,_tmp332,_tmp331,({const char*_tmp171=",";_tag_fat(_tmp171,sizeof(char),2U);}));});
goto _LL0;}case 30U: _LL5F: _tmpEB=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL60: {struct Cyc_List_List*des=_tmpEB;
# 756
({struct Cyc_List_List*_tmp336=des;struct _fat_ptr _tmp335=({const char*_tmp172="{";_tag_fat(_tmp172,sizeof(char),2U);});struct _fat_ptr _tmp334=({const char*_tmp173="}";_tag_fat(_tmp173,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp336,_tmp335,_tmp334,({const char*_tmp174=",";_tag_fat(_tmp174,sizeof(char),2U);}));});goto _LL0;}case 32U: _LL61: _tmpEA=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL62: {struct Cyc_Absyn_Enumfield*ef=_tmpEA;
_tmpE9=ef;goto _LL64;}case 33U: _LL63: _tmpE9=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL64: {struct Cyc_Absyn_Enumfield*ef=_tmpE9;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 31U: _LL65: _tmpE7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpE8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL66: {struct Cyc_List_List*es=_tmpE7;struct Cyc_Absyn_Datatypefield*tuf=_tmpE8;
# 761
Cyc_Absyndump_dumpqvar(tuf->name);
if(es != 0)({struct Cyc_List_List*_tmp339=es;struct _fat_ptr _tmp338=({const char*_tmp175="(";_tag_fat(_tmp175,sizeof(char),2U);});struct _fat_ptr _tmp337=({const char*_tmp176=")";_tag_fat(_tmp176,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp339,_tmp338,_tmp337,({const char*_tmp177=",";_tag_fat(_tmp177,sizeof(char),2U);}));});
goto _LL0;}case 34U: _LL67: _tmpE2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).is_calloc;_tmpE3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).rgn;_tmpE4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).elt_type;_tmpE5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).num_elts;_tmpE6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).inline_call;_LL68: {int is_calloc=_tmpE2;struct Cyc_Absyn_Exp*ropt=_tmpE3;void**topt=_tmpE4;struct Cyc_Absyn_Exp*e=_tmpE5;int inline_call=_tmpE6;
# 766
Cyc_Absyndump_dumploc(e->loc);
if(is_calloc){
if(ropt != 0){
Cyc_Absyndump_dump(({const char*_tmp178="rcalloc(";_tag_fat(_tmp178,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp179=",";_tag_fat(_tmp179,sizeof(char),2U);}));}else{
# 771
Cyc_Absyndump_dump(({const char*_tmp17A="calloc(";_tag_fat(_tmp17A,sizeof(char),8U);}));}
# 773
Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp17B=",";_tag_fat(_tmp17B,sizeof(char),2U);}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U));Cyc_Absyndump_dump(({const char*_tmp17C=")";_tag_fat(_tmp17C,sizeof(char),2U);}));}else{
# 775
if(ropt != 0){
Cyc_Absyndump_dump(inline_call?({const char*_tmp17D="rmalloc_inline(";_tag_fat(_tmp17D,sizeof(char),16U);}):({const char*_tmp17E="rmalloc(";_tag_fat(_tmp17E,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp17F=",";_tag_fat(_tmp17F,sizeof(char),2U);}));}else{
# 779
Cyc_Absyndump_dump(({const char*_tmp180="malloc(";_tag_fat(_tmp180,sizeof(char),8U);}));}
# 782
if(topt != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp33A=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp33A,e,0U);}));else{
# 785
Cyc_Absyndump_dumpexp(e);}
Cyc_Absyndump_dump(({const char*_tmp181=")";_tag_fat(_tmp181,sizeof(char),2U);}));}
# 788
goto _LL0;}case 35U: _LL69: _tmpE0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpE1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL6A: {struct Cyc_Absyn_Exp*e1=_tmpE0;struct Cyc_Absyn_Exp*e2=_tmpE1;
# 791
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp182=":=:";_tag_fat(_tmp182,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 36U: _LL6B: _tmpDE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpDF=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL6C: {struct Cyc_Core_Opt*n=_tmpDE;struct Cyc_List_List*des=_tmpDF;
# 794
({struct Cyc_List_List*_tmp33D=des;struct _fat_ptr _tmp33C=({const char*_tmp183="{";_tag_fat(_tmp183,sizeof(char),2U);});struct _fat_ptr _tmp33B=({const char*_tmp184="}";_tag_fat(_tmp184,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp33D,_tmp33C,_tmp33B,({const char*_tmp185=",";_tag_fat(_tmp185,sizeof(char),2U);}));});goto _LL0;}default: _LL6D: _tmpDD=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL6E: {struct Cyc_Absyn_Stmt*s=_tmpDD;
# 797
Cyc_Absyndump_dump_nospace(({const char*_tmp186="({";_tag_fat(_tmp186,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s,1,0);Cyc_Absyndump_dump_nospace(({const char*_tmp187="})";_tag_fat(_tmp187,sizeof(char),3U);}));goto _LL0;}}_LL0:;}
# 799
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 803
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 807
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs,struct Cyc_List_List*varsinblock){
# 809
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp188=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*c=_tmp188;
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp189="default:";_tag_fat(_tmp189,sizeof(char),9U);}));else{
# 814
Cyc_Absyndump_dump(({const char*_tmp18A="case";_tag_fat(_tmp18A,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(c->pattern);
if(c->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp18B="&&";_tag_fat(_tmp18B,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));}
# 820
Cyc_Absyndump_dump_nospace(({const char*_tmp18C=":";_tag_fat(_tmp18C,sizeof(char),2U);}));}
# 822
if(Cyc_Absynpp_is_declaration(c->body)){
Cyc_Absyndump_dump(({const char*_tmp18D=" {";_tag_fat(_tmp18D,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(c->body,0,0),Cyc_Absyndump_dump_char((int)'}');}else{
# 825
Cyc_Absyndump_dumpstmt(c->body,0,varsinblock);}}}
# 829
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt,struct Cyc_List_List*varsinblock){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp18E=s->r;void*_stmttmpE=_tmp18E;void*_tmp18F=_stmttmpE;struct Cyc_List_List*_tmp190;struct Cyc_Absyn_Stmt*_tmp192;struct _fat_ptr*_tmp191;struct Cyc_Absyn_Stmt*_tmp194;struct Cyc_Absyn_Decl*_tmp193;struct Cyc_List_List*_tmp196;struct Cyc_Absyn_Stmt*_tmp195;struct Cyc_List_List*_tmp198;struct Cyc_Absyn_Exp*_tmp197;struct _fat_ptr*_tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Stmt*_tmp19A;struct Cyc_Absyn_Stmt*_tmp19F;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Stmt*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Stmt*_tmp1A4;struct Cyc_Absyn_Stmt*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Stmt*_tmp1A7;struct Cyc_Absyn_Stmt*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;switch(*((int*)_tmp18F)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1U: _LL3: _tmp1A8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp1A8;
Cyc_Absyndump_dumpexp_prec(- 100,e);Cyc_Absyndump_dump_semi();goto _LL0;}case 2U: _LL5: _tmp1A6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp1A7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp1A6;struct Cyc_Absyn_Stmt*s2=_tmp1A7;
# 838
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 841
Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}
if(Cyc_Absynpp_is_declaration(s2)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(s2,expstmt,0);Cyc_Absyndump_dump_char((int)'}');
if(expstmt){Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}}else{
# 847
Cyc_Absyndump_dumpstmt(s2,expstmt,varsinblock);}
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1 == 0){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1A9="return;";_tag_fat(_tmp1A9,sizeof(char),8U);}));goto _LL0;}else{_LL9: _tmp1A5=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp1A5;
Cyc_Absyndump_dump(({const char*_tmp1AA="return";_tag_fat(_tmp1AA,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(e));Cyc_Absyndump_dump_semi();goto _LL0;}}case 4U: _LLB: _tmp1A2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp1A3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2;_tmp1A4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp18F)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp1A2;struct Cyc_Absyn_Stmt*s1=_tmp1A3;struct Cyc_Absyn_Stmt*s2=_tmp1A4;
# 852
Cyc_Absyndump_dump(({const char*_tmp1AB="if(";_tag_fat(_tmp1AB,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(e);
{void*_tmp1AC=s1->r;void*_stmttmpF=_tmp1AC;void*_tmp1AD=_stmttmpF;switch(*((int*)_tmp1AD)){case 2U: _LL26: _LL27:
 goto _LL29;case 12U: _LL28: _LL29:
 goto _LL2B;case 4U: _LL2A: _LL2B:
 goto _LL2D;case 13U: _LL2C: _LL2D:
# 858
 Cyc_Absyndump_dump_nospace(({const char*_tmp1AE="){";_tag_fat(_tmp1AE,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');goto _LL25;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}_LL25:;}
# 861
{void*_tmp1AF=s2->r;void*_stmttmp10=_tmp1AF;void*_tmp1B0=_stmttmp10;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1B0)->tag == 0U){_LL31: _LL32:
 goto _LL30;}else{_LL33: _LL34:
 Cyc_Absyndump_dump(({const char*_tmp1B1="else{";_tag_fat(_tmp1B1,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(s2,0,0);Cyc_Absyndump_dump_char((int)'}');goto _LL30;}_LL30:;}
# 865
goto _LL0;}case 5U: _LLD: _tmp1A0=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1).f1;_tmp1A1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp1A0;struct Cyc_Absyn_Stmt*s1=_tmp1A1;
# 868
Cyc_Absyndump_dump(({const char*_tmp1B2="while(";_tag_fat(_tmp1B2,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1B3="){";_tag_fat(_tmp1B3,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 9U: _LLF: _tmp19C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp19D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2).f1;_tmp19E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp18F)->f3).f1;_tmp19F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp18F)->f4;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp19C;struct Cyc_Absyn_Exp*e2=_tmp19D;struct Cyc_Absyn_Exp*e3=_tmp19E;struct Cyc_Absyn_Stmt*s1=_tmp19F;
# 872
Cyc_Absyndump_dump(({const char*_tmp1B4="for(";_tag_fat(_tmp1B4,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e3);
Cyc_Absyndump_dump_nospace(({const char*_tmp1B5="){";_tag_fat(_tmp1B5,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 14U: _LL11: _tmp19A=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp19B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s1=_tmp19A;struct Cyc_Absyn_Exp*e=_tmp19B;
# 876
Cyc_Absyndump_dump(({const char*_tmp1B6="do{";_tag_fat(_tmp1B6,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1B7="}while(";_tag_fat(_tmp1B7,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1B8=");";_tag_fat(_tmp1B8,sizeof(char),3U);}));
goto _LL0;}case 6U: _LL13: _LL14:
# 880
 Cyc_Absyndump_dump(({const char*_tmp1B9="break;";_tag_fat(_tmp1B9,sizeof(char),7U);}));goto _LL0;case 7U: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp1BA="continue;";_tag_fat(_tmp1BA,sizeof(char),10U);}));goto _LL0;case 8U: _LL17: _tmp199=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_LL18: {struct _fat_ptr*x=_tmp199;
Cyc_Absyndump_dump(({const char*_tmp1BB="goto";_tag_fat(_tmp1BB,sizeof(char),5U);}));Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_semi();goto _LL0;}case 10U: _LL19: _tmp197=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp198=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp197;struct Cyc_List_List*ss=_tmp198;
# 885
Cyc_Absyndump_dump(({const char*_tmp1BC="switch(";_tag_fat(_tmp1BC,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1BD="){";_tag_fat(_tmp1BD,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 15U: _LL1B: _tmp195=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp196=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp195;struct Cyc_List_List*ss=_tmp196;
# 890
Cyc_Absyndump_dump(({const char*_tmp1BE="try";_tag_fat(_tmp1BE,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,varsinblock);
Cyc_Absyndump_dump(({const char*_tmp1BF="catch{";_tag_fat(_tmp1BF,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 12U: _LL1D: _tmp193=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp194=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp193;struct Cyc_Absyn_Stmt*s1=_tmp194;
# 896
struct _tuple13 _tmp1C0=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple13 _stmttmp11=_tmp1C0;struct _tuple13 _tmp1C1=_stmttmp11;struct Cyc_List_List*_tmp1C3;int _tmp1C2;_LL36: _tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;_LL37: {int newblock=_tmp1C2;struct Cyc_List_List*newvarsinblock=_tmp1C3;
if(newblock){
if(expstmt)Cyc_Absyndump_dump(({const char*_tmp1C4="({";_tag_fat(_tmp1C4,sizeof(char),3U);}));else{Cyc_Absyndump_dump(({const char*_tmp1C5="{";_tag_fat(_tmp1C5,sizeof(char),2U);}));}
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1C6="});";_tag_fat(_tmp1C6,sizeof(char),4U);}));else{Cyc_Absyndump_dump(({const char*_tmp1C7="}";_tag_fat(_tmp1C7,sizeof(char),2U);}));}}else{
# 903
Cyc_Absyndump_dumpdecl(d);Cyc_Absyndump_dumpstmt(s1,expstmt,newvarsinblock);}
# 905
goto _LL0;}}case 13U: _LL1F: _tmp191=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_tmp192=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp18F)->f2;_LL20: {struct _fat_ptr*x=_tmp191;struct Cyc_Absyn_Stmt*s1=_tmp192;
# 911
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_str(x);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1C8=": ({";_tag_fat(_tmp1C8,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1C9=": {";_tag_fat(_tmp1C9,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1CA="});";_tag_fat(_tmp1CA,sizeof(char),4U);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 917
Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(s1,expstmt,varsinblock);}
# 919
goto _LL0;}default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1 == 0){_LL21: _LL22:
# 921
 Cyc_Absyndump_dump(({const char*_tmp1CB="fallthru;";_tag_fat(_tmp1CB,sizeof(char),10U);}));goto _LL0;}else{_LL23: _tmp190=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp18F)->f1;_LL24: {struct Cyc_List_List*es=_tmp190;
# 923
Cyc_Absyndump_dump(({const char*_tmp1CC="fallthru(";_tag_fat(_tmp1CC,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_nospace(({const char*_tmp1CD=");";_tag_fat(_tmp1CD,sizeof(char),3U);}));goto _LL0;}}}_LL0:;}}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 927
static void Cyc_Absyndump_dumpdp(struct _tuple21*dp){
({struct Cyc_List_List*_tmp340=(*dp).f1;struct _fat_ptr _tmp33F=({const char*_tmp1CE="";_tag_fat(_tmp1CE,sizeof(char),1U);});struct _fat_ptr _tmp33E=({const char*_tmp1CF="=";_tag_fat(_tmp1CF,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp340,_tmp33F,_tmp33E,({const char*_tmp1D0="=";_tag_fat(_tmp1D0,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 931
static struct _fat_ptr Cyc_Absyndump_pat_term(int dots){return dots?({const char*_tmp1D1="...)";_tag_fat(_tmp1D1,sizeof(char),5U);}):({const char*_tmp1D2=")";_tag_fat(_tmp1D2,sizeof(char),2U);});}
# 933
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1D3=p->r;void*_stmttmp12=_tmp1D3;void*_tmp1D4=_stmttmp12;struct Cyc_Absyn_Exp*_tmp1D5;int _tmp1D8;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_Datatypefield*_tmp1D6;int _tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_List_List*_tmp1D9;int _tmp1DF;struct Cyc_List_List*_tmp1DE;struct Cyc_List_List*_tmp1DD;union Cyc_Absyn_AggrInfo _tmp1DC;int _tmp1E2;struct Cyc_List_List*_tmp1E1;struct _tuple0*_tmp1E0;struct _tuple0*_tmp1E3;struct Cyc_Absyn_Vardecl*_tmp1E5;struct Cyc_Absyn_Tvar*_tmp1E4;struct Cyc_Absyn_Pat*_tmp1E6;int _tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1EA;struct Cyc_Absyn_Tvar*_tmp1E9;struct Cyc_Absyn_Pat*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1EB;struct Cyc_Absyn_Vardecl*_tmp1ED;struct Cyc_Absyn_Pat*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1EE;struct Cyc_Absyn_Vardecl*_tmp1F0;char _tmp1F1;struct Cyc_Absyn_Enumfield*_tmp1F2;struct Cyc_Absyn_Enumfield*_tmp1F3;struct _fat_ptr _tmp1F4;int _tmp1F5;int _tmp1F6;int _tmp1F7;switch(*((int*)_tmp1D4)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_char((int)'_');goto _LL0;case 9U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp1F8="NULL";_tag_fat(_tmp1F8,sizeof(char),5U);}));goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1){case Cyc_Absyn_None: _LL5: _tmp1F7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL6: {int i=_tmp1F7;
_tmp1F6=i;goto _LL8;}case Cyc_Absyn_Signed: _LL7: _tmp1F6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL8: {int i=_tmp1F6;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1FB=({struct Cyc_Int_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=1U,_tmp2F5.f1=(unsigned long)i;_tmp2F5;});void*_tmp1F9[1U];_tmp1F9[0]=& _tmp1FB;({struct _fat_ptr _tmp341=({const char*_tmp1FA="%d";_tag_fat(_tmp1FA,sizeof(char),3U);});Cyc_aprintf(_tmp341,_tag_fat(_tmp1F9,sizeof(void*),1U));});}));goto _LL0;}default: _LL9: _tmp1F5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LLA: {int i=_tmp1F5;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1FE=({struct Cyc_Int_pa_PrintArg_struct _tmp2F6;_tmp2F6.tag=1U,_tmp2F6.f1=(unsigned)i;_tmp2F6;});void*_tmp1FC[1U];_tmp1FC[0]=& _tmp1FE;({struct _fat_ptr _tmp342=({const char*_tmp1FD="%u";_tag_fat(_tmp1FD,sizeof(char),3U);});Cyc_aprintf(_tmp342,_tag_fat(_tmp1FC,sizeof(void*),1U));});}));goto _LL0;}}case 12U: _LLB: _tmp1F4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LLC: {struct _fat_ptr x=_tmp1F4;
Cyc_Absyndump_dump(x);goto _LL0;}case 13U: _LLD: _tmp1F3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LLE: {struct Cyc_Absyn_Enumfield*ef=_tmp1F3;
_tmp1F2=ef;goto _LL10;}case 14U: _LLF: _tmp1F2=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL10: {struct Cyc_Absyn_Enumfield*ef=_tmp1F2;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 11U: _LL11: _tmp1F1=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL12: {char ch=_tmp1F1;
# 944
Cyc_Absyndump_dump(({const char*_tmp1FF="'";_tag_fat(_tmp1FF,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_nospace(({const char*_tmp200="'";_tag_fat(_tmp200,sizeof(char),2U);}));goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2)->r)->tag == 0U){_LL13: _tmp1F0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL14: {struct Cyc_Absyn_Vardecl*vd=_tmp1F0;
Cyc_Absyndump_dump(({const char*_tmp201="*";_tag_fat(_tmp201,sizeof(char),2U);}));_tmp1ED=vd;goto _LL16;}}else{_LL17: _tmp1EE=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp1EF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp1EE;struct Cyc_Absyn_Pat*p2=_tmp1EF;
# 947
Cyc_Absyndump_dump(({const char*_tmp202="*";_tag_fat(_tmp202,sizeof(char),2U);}));_tmp1EB=vd;_tmp1EC=p2;goto _LL1A;}}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2)->r)->tag == 0U){_LL15: _tmp1ED=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL16: {struct Cyc_Absyn_Vardecl*vd=_tmp1ED;
# 946
Cyc_Absyndump_dumpqvar(vd->name);goto _LL0;}}else{_LL19: _tmp1EB=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp1EC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp1EB;struct Cyc_Absyn_Pat*p2=_tmp1EC;
# 948
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump(({const char*_tmp203=" as ";_tag_fat(_tmp203,sizeof(char),5U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}}case 2U: _LL1B: _tmp1E9=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp1EA=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL1C: {struct Cyc_Absyn_Tvar*tv=_tmp1E9;struct Cyc_Absyn_Vardecl*vd=_tmp1EA;
# 950
Cyc_Absyndump_dump(({const char*_tmp204="alias";_tag_fat(_tmp204,sizeof(char),6U);}));Cyc_Absyndump_dump(({const char*_tmp205="<";_tag_fat(_tmp205,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp206=">";_tag_fat(_tmp206,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(vd,p->loc,0);
goto _LL0;}case 5U: _LL1D: _tmp1E7=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp1E8=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL1E: {struct Cyc_List_List*ts=_tmp1E7;int dots=_tmp1E8;
({struct Cyc_List_List*_tmp345=ts;struct _fat_ptr _tmp344=({const char*_tmp207="$(";_tag_fat(_tmp207,sizeof(char),3U);});struct _fat_ptr _tmp343=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp345,_tmp344,_tmp343,({const char*_tmp208=",";_tag_fat(_tmp208,sizeof(char),2U);}));});goto _LL0;}case 6U: _LL1F: _tmp1E6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL20: {struct Cyc_Absyn_Pat*p2=_tmp1E6;
Cyc_Absyndump_dump(({const char*_tmp209="&";_tag_fat(_tmp209,sizeof(char),2U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}case 4U: _LL21: _tmp1E4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp1E5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_LL22: {struct Cyc_Absyn_Tvar*tv=_tmp1E4;struct Cyc_Absyn_Vardecl*vd=_tmp1E5;
# 956
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump_char((int)'>');goto _LL0;}case 15U: _LL23: _tmp1E3=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL24: {struct _tuple0*q=_tmp1E3;
Cyc_Absyndump_dumpqvar(q);goto _LL0;}case 16U: _LL25: _tmp1E0=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp1E1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_tmp1E2=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_LL26: {struct _tuple0*q=_tmp1E0;struct Cyc_List_List*ps=_tmp1E1;int dots=_tmp1E2;
# 959
Cyc_Absyndump_dumpqvar(q);({struct Cyc_List_List*_tmp348=ps;struct _fat_ptr _tmp347=({const char*_tmp20A="(";_tag_fat(_tmp20A,sizeof(char),2U);});struct _fat_ptr _tmp346=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp348,_tmp347,_tmp346,({const char*_tmp20B=",";_tag_fat(_tmp20B,sizeof(char),2U);}));});goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1 != 0){_LL27: _tmp1DC=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_tmp1DD=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_tmp1DE=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_tmp1DF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f4;_LL28: {union Cyc_Absyn_AggrInfo info=_tmp1DC;struct Cyc_List_List*exists=_tmp1DD;struct Cyc_List_List*dps=_tmp1DE;int dots=_tmp1DF;
# 961
Cyc_Absyndump_dumpqvar((Cyc_Absyn_aggr_kinded_name(info)).f2);
_tmp1D9=exists;_tmp1DA=dps;_tmp1DB=dots;goto _LL2A;}}else{_LL29: _tmp1D9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_tmp1DA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_tmp1DB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4)->f4;_LL2A: {struct Cyc_List_List*exists=_tmp1D9;struct Cyc_List_List*dps=_tmp1DA;int dots=_tmp1DB;
# 964
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp34B=exists;struct _fat_ptr _tmp34A=({const char*_tmp20C="<";_tag_fat(_tmp20C,sizeof(char),2U);});struct _fat_ptr _tmp349=({const char*_tmp20D=">";_tag_fat(_tmp20D,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp34B,_tmp34A,_tmp349,({const char*_tmp20E=",";_tag_fat(_tmp20E,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp34E=dps;struct _fat_ptr _tmp34D=({const char*_tmp20F="";_tag_fat(_tmp20F,sizeof(char),1U);});struct _fat_ptr _tmp34C=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct _tuple21*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp34E,_tmp34D,_tmp34C,({const char*_tmp210=",";_tag_fat(_tmp210,sizeof(char),2U);}));});
goto _LL0;}}case 8U: _LL2B: _tmp1D6=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D4)->f2;_tmp1D7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D4)->f3;_tmp1D8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D4)->f4;_LL2C: {struct Cyc_Absyn_Datatypefield*tuf=_tmp1D6;struct Cyc_List_List*ps=_tmp1D7;int dots=_tmp1D8;
# 969
Cyc_Absyndump_dumpqvar(tuf->name);
if(ps != 0)({struct Cyc_List_List*_tmp351=ps;struct _fat_ptr _tmp350=({const char*_tmp211="(";_tag_fat(_tmp211,sizeof(char),2U);});struct _fat_ptr _tmp34F=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp351,_tmp350,_tmp34F,({const char*_tmp212=",";_tag_fat(_tmp212,sizeof(char),2U);}));});
goto _LL0;}default: _LL2D: _tmp1D5=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D4)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp1D5;
Cyc_Absyndump_dumpexp(e);goto _LL0;}}_LL0:;}
# 976
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 981
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp352=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp352,({const char*_tmp213=",";_tag_fat(_tmp213,sizeof(char),2U);}));});}
# 984
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp214="=";_tag_fat(_tmp214,sizeof(char),2U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 990
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp353=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp353,({const char*_tmp215=",";_tag_fat(_tmp215,sizeof(char),2U);}));});}
# 994
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp216=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_stmttmp13=_tmp216;struct Cyc_Absyn_Aggrfield*_tmp217=_stmttmp13;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_List_List*_tmp21C;struct Cyc_Absyn_Exp*_tmp21B;void*_tmp21A;struct Cyc_Absyn_Tqual _tmp219;struct _fat_ptr*_tmp218;_LL1: _tmp218=_tmp217->name;_tmp219=_tmp217->tq;_tmp21A=_tmp217->type;_tmp21B=_tmp217->width;_tmp21C=_tmp217->attributes;_tmp21D=_tmp217->requires_clause;_LL2: {struct _fat_ptr*name=_tmp218;struct Cyc_Absyn_Tqual tq=_tmp219;void*type=_tmp21A;struct Cyc_Absyn_Exp*width=_tmp21B;struct Cyc_List_List*atts=_tmp21C;struct Cyc_Absyn_Exp*req=_tmp21D;
{enum Cyc_Cyclone_C_Compilers _tmp21E=Cyc_Cyclone_c_compiler;if(_tmp21E == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dump_str,name);Cyc_Absyndump_dumpatts(atts);goto _LL3;}else{_LL6: _LL7:
 Cyc_Absyndump_dumpatts(atts);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dump_str,name);goto _LL3;}_LL3:;}
# 1001
if((unsigned)req){
Cyc_Absyndump_dump(({const char*_tmp21F="@requires ";_tag_fat(_tmp21F,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(req);}
# 1004
if(width != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(width);}
# 1007
Cyc_Absyndump_dump_semi();}}}
# 1011
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1016
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple22{void*f1;struct _tuple0*f2;};
# 1020
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple22*pr){
{void*_tmp220=(*pr).f1;void*_stmttmp14=_tmp220;void*_tmp221=_stmttmp14;switch(*((int*)_tmp221)){case 11U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp222="_stdcall";_tag_fat(_tmp222,sizeof(char),9U);}));goto _LL0;case 2U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp223="_cdecl";_tag_fat(_tmp223,sizeof(char),7U);}));goto _LL0;case 3U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp224="_fastcall";_tag_fat(_tmp224,sizeof(char),10U);}));goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 1028
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1030
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1034
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 1038
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1040
void*_tmp225=t;void*_tmp226;void*_tmp227;switch(*((int*)_tmp225)){case 1U: _LL1: _tmp227=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp225)->f2;if(_tmp227 != 0){_LL2: {void*def=_tmp227;
return Cyc_Absyndump_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp226=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp225)->f1).elt_type;_LL4: {void*elt_typ=_tmp226;
# 1043
while(1){
void*_tmp228=elt_typ;void*_tmp229;void*_tmp22A;switch(*((int*)_tmp228)){case 1U: _LL8: _tmp22A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp228)->f2;if(_tmp22A != 0){_LL9: {void*t=_tmp22A;
elt_typ=t;goto _LL7;}}else{goto _LLE;}case 8U: _LLA: _tmp229=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp228)->f4;if(_tmp229 != 0){_LLB: {void*t=_tmp229;
elt_typ=t;goto _LL7;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp228)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp228)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 1050
 return 0;}_LL0:;}
# 1054
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp22B=vd;struct Cyc_List_List*_tmp232;struct Cyc_Absyn_Exp*_tmp231;void*_tmp230;struct Cyc_Absyn_Tqual _tmp22F;unsigned _tmp22E;struct _tuple0*_tmp22D;enum Cyc_Absyn_Scope _tmp22C;_LL1: _tmp22C=_tmp22B->sc;_tmp22D=_tmp22B->name;_tmp22E=_tmp22B->varloc;_tmp22F=_tmp22B->tq;_tmp230=_tmp22B->type;_tmp231=_tmp22B->initializer;_tmp232=_tmp22B->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp22C;struct _tuple0*name=_tmp22D;unsigned varloc=_tmp22E;struct Cyc_Absyn_Tqual tq=_tmp22F;void*type=_tmp230;struct Cyc_Absyn_Exp*initializer=_tmp231;struct Cyc_List_List*atts=_tmp232;
{enum Cyc_Cyclone_C_Compilers _tmp233=Cyc_Cyclone_c_compiler;if(_tmp233 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1059
 if((int)sc == (int)3U && Cyc_Absyndump_qvar_to_Cids){
void*_tmp234=Cyc_Tcutil_compress(type);void*_stmttmp15=_tmp234;void*_tmp235=_stmttmp15;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp235)->tag == 5U){_LL9: _LLA:
 goto _LL8;}else{_LLB: _LLC:
 Cyc_Absyndump_dumpscope(sc);}_LL8:;}else{
# 1065
Cyc_Absyndump_dumpscope(sc);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dumpqvar,name);
Cyc_Absyndump_dumpatts(atts);
goto _LL3;}else{_LL6: _LL7:
# 1070
 Cyc_Absyndump_dumpatts(atts);
Cyc_Absyndump_dumpscope(sc);{
struct _RegionHandle _tmp236=_new_region("temp");struct _RegionHandle*temp=& _tmp236;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(type);
struct _tuple12 _tmp237=Cyc_Absynpp_to_tms(temp,tq,type);struct _tuple12 _stmttmp16=_tmp237;struct _tuple12 _tmp238=_stmttmp16;struct Cyc_List_List*_tmp23B;void*_tmp23A;struct Cyc_Absyn_Tqual _tmp239;_LLE: _tmp239=_tmp238.f1;_tmp23A=_tmp238.f2;_tmp23B=_tmp238.f3;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp239;void*t=_tmp23A;struct Cyc_List_List*tms=_tmp23B;
# 1076
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=tms;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp23C=(void*)tms2->hd;void*_stmttmp17=_tmp23C;void*_tmp23D=_stmttmp17;struct Cyc_List_List*_tmp23E;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp23D)->tag == 5U){_LL11: _tmp23E=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp23D)->f2;_LL12: {struct Cyc_List_List*atts=_tmp23E;
# 1080
for(0;atts != 0;atts=atts->tl){
void*_tmp23F=(void*)atts->hd;void*_stmttmp18=_tmp23F;void*_tmp240=_stmttmp18;switch(*((int*)_tmp240)){case 1U: _LL16: _LL17:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL15;case 2U: _LL18: _LL19:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL15;case 3U: _LL1A: _LL1B:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL15;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 1087
goto _LL10;}}else{_LL13: _LL14:
 goto _LL10;}_LL10:;}}
# 1090
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);{
struct _tuple22 _tmp241=({struct _tuple22 _tmp2F7;_tmp2F7.f1=call_conv,_tmp2F7.f2=name;_tmp2F7;});struct _tuple22 pr=_tmp241;
({int _tmp354=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*,void(*f)(struct _tuple22*),struct _tuple22*a))Cyc_Absyndump_dumptms)(_tmp354,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms),Cyc_Absyndump_dump_callconv_qvar,& pr);});
_npop_handler(0U);goto _LL3;}}}
# 1073
;_pop_region(temp);}}_LL3:;}
# 1097
if(initializer != 0){
Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(initializer);}
# 1100
if(do_semi)Cyc_Absyndump_dump_semi();}}
# 1103
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp242="@tagged ";_tag_fat(_tmp242,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl != 0){
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp357=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _fat_ptr _tmp356=({const char*_tmp243="<";_tag_fat(_tmp243,sizeof(char),2U);});struct _fat_ptr _tmp355=({const char*_tmp244=">";_tag_fat(_tmp244,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp357,_tmp356,_tmp355,({const char*_tmp245=",";_tag_fat(_tmp245,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1114
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump_char((int)'}');
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1119
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp246=ed;struct Cyc_Core_Opt*_tmp249;struct _tuple0*_tmp248;enum Cyc_Absyn_Scope _tmp247;_LL1: _tmp247=_tmp246->sc;_tmp248=_tmp246->name;_tmp249=_tmp246->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp247;struct _tuple0*nm=_tmp248;struct Cyc_Core_Opt*fields=_tmp249;
Cyc_Absyndump_dumpscope(sc);Cyc_Absyndump_dump(({const char*_tmp24A="enum ";_tag_fat(_tmp24A,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(nm);
if(fields != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char((int)'}');}}}
# 1126
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp24B=dd;int _tmp250;struct Cyc_Core_Opt*_tmp24F;struct Cyc_List_List*_tmp24E;struct _tuple0*_tmp24D;enum Cyc_Absyn_Scope _tmp24C;_LL1: _tmp24C=_tmp24B->sc;_tmp24D=_tmp24B->name;_tmp24E=_tmp24B->tvs;_tmp24F=_tmp24B->fields;_tmp250=_tmp24B->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp24C;struct _tuple0*name=_tmp24D;struct Cyc_List_List*tvs=_tmp24E;struct Cyc_Core_Opt*fields=_tmp24F;int is_x=_tmp250;
Cyc_Absyndump_dumpscope(sc);
if(is_x)
Cyc_Absyndump_dump(({const char*_tmp251="@extensible ";_tag_fat(_tmp251,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp252="datatype ";_tag_fat(_tmp252,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(name);Cyc_Absyndump_dumptvars(tvs);
if(fields != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char((int)'}');}}}struct _tuple23{unsigned f1;struct _tuple0*f2;int f3;};
# 1137
static void Cyc_Absyndump_dumpexport(struct _tuple23*tup){
Cyc_Absyndump_dumpqvar((*tup).f2);}
# 1140
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char((int)'{');
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char((int)'}');}
# 1147
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp253=d->r;void*_stmttmp19=_tmp253;void*_tmp254=_stmttmp19;struct _tuple10*_tmp258;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp256;struct Cyc_List_List*_tmp255;struct Cyc_List_List*_tmp259;struct Cyc_List_List*_tmp25B;struct _tuple0*_tmp25A;struct Cyc_List_List*_tmp25D;struct _fat_ptr*_tmp25C;struct Cyc_Absyn_Typedefdecl*_tmp25E;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Vardecl*_tmp260;struct Cyc_Absyn_Tvar*_tmp25F;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Pat*_tmp262;struct Cyc_List_List*_tmp264;struct Cyc_Absyn_Enumdecl*_tmp265;struct Cyc_Absyn_Datatypedecl*_tmp266;struct Cyc_Absyn_Aggrdecl*_tmp267;struct Cyc_Absyn_Vardecl*_tmp268;struct Cyc_Absyn_Fndecl*_tmp269;switch(*((int*)_tmp254)){case 1U: _LL1: _tmp269=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp269;
# 1151
{enum Cyc_Cyclone_C_Compilers _tmp26A=Cyc_Cyclone_c_compiler;if(_tmp26A == Cyc_Cyclone_Vc_c){_LL24: _LL25:
 Cyc_Absyndump_dumpatts((fd->i).attributes);goto _LL23;}else{_LL26: _LL27:
 goto _LL23;}_LL23:;}
# 1155
if(fd->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp26B=Cyc_Cyclone_c_compiler;if(_tmp26B == Cyc_Cyclone_Vc_c){_LL29: _LL2A:
 Cyc_Absyndump_dump(({const char*_tmp26C="__inline";_tag_fat(_tmp26C,sizeof(char),9U);}));goto _LL28;}else{_LL2B: _LL2C:
 Cyc_Absyndump_dump(({const char*_tmp26D="inline";_tag_fat(_tmp26D,sizeof(char),7U);}));goto _LL28;}_LL28:;}
# 1160
Cyc_Absyndump_dumpscope(fd->sc);{
# 1162
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->tag=5U,_tmp274->f1=type_info;_tmp274;});
if(fd->cached_type != 0){
void*_tmp26E=Cyc_Tcutil_compress((void*)_check_null(fd->cached_type));void*_stmttmp1A=_tmp26E;void*_tmp26F=_stmttmp1A;struct Cyc_Absyn_FnInfo _tmp270;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26F)->tag == 5U){_LL2E: _tmp270=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26F)->f1;_LL2F: {struct Cyc_Absyn_FnInfo i=_tmp270;
# 1168
({struct Cyc_List_List*_tmp358=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp358;});goto _LL2D;}}else{_LL30: _LL31:
({void*_tmp271=0U;({struct _fat_ptr _tmp359=({const char*_tmp272="function has non-function type";_tag_fat(_tmp272,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp359,_tag_fat(_tmp271,sizeof(void*),0U));});});}_LL2D:;}
# 1172
{enum Cyc_Cyclone_C_Compilers _tmp273=Cyc_Cyclone_c_compiler;if(_tmp273 == Cyc_Cyclone_Gcc_c){_LL33: _LL34:
({struct Cyc_Absyn_Tqual _tmp35B=Cyc_Absyn_empty_tqual(0U);void*_tmp35A=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp35B,_tmp35A,Cyc_Absyndump_dump_atts_qvar,fd);});goto _LL32;}else{_LL35: _LL36:
({struct Cyc_Absyn_Tqual _tmp35D=Cyc_Absyn_empty_tqual(0U);void*_tmp35C=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp35D,_tmp35C,Cyc_Absyndump_dump_callconv_fdqvar,fd);});goto _LL32;}_LL32:;}
# 1176
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(fd->body,0,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}}}case 0U: _LL3: _tmp268=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp268;
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);goto _LL0;}case 5U: _LL5: _tmp267=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp267;
Cyc_Absyndump_dump_aggrdecl(ad);Cyc_Absyndump_dump_semi();goto _LL0;}case 6U: _LL7: _tmp266=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*dd=_tmp266;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump_semi();goto _LL0;}case 7U: _LL9: _tmp265=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp265;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump_semi();goto _LL0;}case 3U: _LLB: _tmp264=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LLC: {struct Cyc_List_List*vds=_tmp264;
({struct Cyc_List_List*_tmp360=vds;struct _fat_ptr _tmp35F=({const char*_tmp275=" let ";_tag_fat(_tmp275,sizeof(char),6U);});struct _fat_ptr _tmp35E=({const char*_tmp276=";";_tag_fat(_tmp276,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpid,_tmp360,_tmp35F,_tmp35E,({const char*_tmp277=",";_tag_fat(_tmp277,sizeof(char),2U);}));});goto _LL0;}case 2U: _LLD: _tmp262=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_tmp263=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp254)->f3;_LLE: {struct Cyc_Absyn_Pat*p=_tmp262;struct Cyc_Absyn_Exp*e=_tmp263;
# 1186
Cyc_Absyndump_dump(({const char*_tmp278="let";_tag_fat(_tmp278,sizeof(char),4U);}));Cyc_Absyndump_dumppat(p);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_semi();goto _LL0;}case 4U: _LLF: _tmp25F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_tmp260=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp254)->f2;_tmp261=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp254)->f3;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp25F;struct Cyc_Absyn_Vardecl*vd=_tmp260;struct Cyc_Absyn_Exp*open_exp_opt=_tmp261;
# 1188
Cyc_Absyndump_dump(({const char*_tmp279="region";_tag_fat(_tmp279,sizeof(char),7U);}));Cyc_Absyndump_dump(({const char*_tmp27A="<";_tag_fat(_tmp27A,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp27B="> ";_tag_fat(_tmp27B,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(vd->name);
if((unsigned)open_exp_opt){
Cyc_Absyndump_dump(({const char*_tmp27C=" = open(";_tag_fat(_tmp27C,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(open_exp_opt);Cyc_Absyndump_dump(({const char*_tmp27D=")";_tag_fat(_tmp27D,sizeof(char),2U);}));}
# 1192
Cyc_Absyndump_dump_semi();
goto _LL0;}case 8U: _LL11: _tmp25E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp25E;
# 1199
Cyc_Absyndump_dump(({const char*_tmp27E="typedef";_tag_fat(_tmp27E,sizeof(char),8U);}));{
void*t=td->defn == 0?Cyc_Absyn_new_evar(td->kind,0):(void*)_check_null(td->defn);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(td->tq,t,Cyc_Absyndump_dumptypedefname,td);
Cyc_Absyndump_dumpatts(td->atts);
Cyc_Absyndump_dump_semi();
# 1205
goto _LL0;}}case 9U: _LL13: _tmp25C=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_tmp25D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp254)->f2;_LL14: {struct _fat_ptr*v=_tmp25C;struct Cyc_List_List*tdl=_tmp25D;
# 1207
Cyc_Absyndump_dump(({const char*_tmp27F="namespace";_tag_fat(_tmp27F,sizeof(char),10U);}));Cyc_Absyndump_dump_str(v);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 10U: _LL15: _tmp25A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_tmp25B=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp254)->f2;_LL16: {struct _tuple0*q=_tmp25A;struct Cyc_List_List*tdl=_tmp25B;
# 1209
Cyc_Absyndump_dump(({const char*_tmp280="using";_tag_fat(_tmp280,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(q);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 11U: _LL17: _tmp259=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp259;
# 1211
Cyc_Absyndump_dump(({const char*_tmp281="extern \"C\" ";_tag_fat(_tmp281,sizeof(char),12U);}));Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 12U: _LL19: _tmp255=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_tmp256=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp254)->f2;_tmp257=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp254)->f3;_tmp258=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp254)->f4;_LL1A: {struct Cyc_List_List*tdl=_tmp255;struct Cyc_List_List*ovrs=_tmp256;struct Cyc_List_List*exs=_tmp257;struct _tuple10*wc=_tmp258;
# 1213
Cyc_Absyndump_dump(({const char*_tmp282="extern \"C include\" ";_tag_fat(_tmp282,sizeof(char),20U);}));Cyc_Absyndump_dump_braced_decls(tdl);
if(ovrs != 0){
Cyc_Absyndump_dump(({const char*_tmp283=" cyclone_override ";_tag_fat(_tmp283,sizeof(char),19U);}));
Cyc_Absyndump_dump_braced_decls(ovrs);}
# 1218
if((unsigned)wc){
Cyc_Absyndump_dump(({const char*_tmp284=" export { * }\n";_tag_fat(_tmp284,sizeof(char),15U);}));{
struct _tuple10*_tmp285=wc;struct Cyc_List_List*_tmp286;_LL38: _tmp286=_tmp285->f2;_LL39: {struct Cyc_List_List*hides=_tmp286;
if((unsigned)hides)
({struct Cyc_List_List*_tmp363=hides;struct _fat_ptr _tmp362=({const char*_tmp287=" hide {";_tag_fat(_tmp287,sizeof(char),8U);});struct _fat_ptr _tmp361=({const char*_tmp288="}";_tag_fat(_tmp288,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple0*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpqvar,_tmp363,_tmp362,_tmp361,({const char*_tmp289=",";_tag_fat(_tmp289,sizeof(char),2U);}));});}}}else{
# 1226
({struct Cyc_List_List*_tmp366=exs;struct _fat_ptr _tmp365=({const char*_tmp28A=" export {";_tag_fat(_tmp28A,sizeof(char),10U);});struct _fat_ptr _tmp364=({const char*_tmp28B="}";_tag_fat(_tmp28B,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple23*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpexport,_tmp366,_tmp365,_tmp364,({const char*_tmp28C=",";_tag_fat(_tmp28C,sizeof(char),2U);}));});}
# 1228
goto _LL0;}case 13U: _LL1B: _LL1C:
 Cyc_Absyndump_dump(({const char*_tmp28D=" __cyclone_port_on__; ";_tag_fat(_tmp28D,sizeof(char),23U);}));goto _LL0;case 14U: _LL1D: _LL1E:
 Cyc_Absyndump_dump(({const char*_tmp28E=" __cyclone_port_off__; ";_tag_fat(_tmp28E,sizeof(char),24U);}));goto _LL0;case 15U: _LL1F: _LL20:
 Cyc_Absyndump_dump(({const char*_tmp28F=" __tempest_on__; ";_tag_fat(_tmp28F,sizeof(char),18U);}));goto _LL0;default: _LL21: _LL22:
 Cyc_Absyndump_dump(({const char*_tmp290=" __tempest_off__; ";_tag_fat(_tmp290,sizeof(char),19U);}));goto _LL0;}_LL0:;}}
# 1236
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple14 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != (unsigned)1 || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1243
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1245
if(tms == 0){
f(a);
return;}{
# 1249
void*_tmp291=(void*)tms->hd;void*_stmttmp1B=_tmp291;void*_tmp292=_stmttmp1B;struct Cyc_Absyn_Tqual _tmp297;void*_tmp296;void*_tmp295;void*_tmp294;void*_tmp293;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp292)->tag == 2U){_LL1: _tmp293=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp292)->f1).rgn;_tmp294=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp292)->f1).nullable;_tmp295=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp292)->f1).bounds;_tmp296=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp292)->f1).zero_term;_tmp297=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp292)->f2;_LL2: {void*rgn=_tmp293;void*nullable=_tmp294;void*bd=_tmp295;void*zt=_tmp296;struct Cyc_Absyn_Tqual tq2=_tmp297;
# 1253
{void*_tmp298=Cyc_Tcutil_compress(bd);void*_stmttmp1C=_tmp298;void*_tmp299=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp29A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f1)){case 14U: _LL6: _LL7:
 Cyc_Absyndump_dump_char((int)'?');goto _LL5;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f2)->hd)->tag == 9U){_LL8: _tmp29A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp299)->f2)->hd)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp29A;
# 1256
Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));Cyc_Absyndump_dump_upperbound(e);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA: _LLB:
# 1258
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));}_LL5:;}
# 1260
if((!Cyc_Absyndump_qvar_to_Cids && !is_char_ptr)&& Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp29B="@zeroterm";_tag_fat(_tmp29B,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids && is_char_ptr)&& !Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp29C="@nozeroterm";_tag_fat(_tmp29C,sizeof(char),12U);}));
{void*_tmp29D=Cyc_Tcutil_compress(rgn);void*_stmttmp1D=_tmp29D;void*_tmp29E=_stmttmp1D;struct Cyc_Absyn_Tvar*_tmp29F;switch(*((int*)_tmp29E)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E)->f1)){case 5U: _LLD: _LLE:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp2A0="`H";_tag_fat(_tmp2A0,sizeof(char),3U);}));goto _LLC;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp2A1="`U";_tag_fat(_tmp2A1,sizeof(char),3U);}));goto _LLC;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp2A2="`RC";_tag_fat(_tmp2A2,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2U: _LL13: _tmp29F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29E)->f1;_LL14: {struct Cyc_Absyn_Tvar*tv=_tmp29F;
Cyc_Absyndump_dumptvar(tv);goto _LLC;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29E)->f2 == 0){_LL15: _LL16:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(rgn));goto _LLC;}else{goto _LL19;}case 8U: _LL17: _LL18:
 Cyc_Absyndump_dump(({const char*_tmp2A3="@region(";_tag_fat(_tmp2A3,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(rgn);Cyc_Absyndump_dump(({const char*_tmp2A4=")";_tag_fat(_tmp2A4,sizeof(char),2U);}));goto _LLC;default: _LL19: _LL1A:
({void*_tmp2A5=0U;({struct _fat_ptr _tmp367=({const char*_tmp2A6="dumptms: bad rgn type in Pointer_mod";_tag_fat(_tmp2A6,sizeof(char),37U);});Cyc_Warn_impos(_tmp367,_tag_fat(_tmp2A5,sizeof(void*),0U));});});}_LLC:;}
# 1271
Cyc_Absyndump_dumptq(tq2);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}}else{_LL3: _LL4: {
# 1276
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2A7=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmp1E=_tmp2A7;void*_tmp2A8=_stmttmp1E;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A8)->tag == 2U){_LL1C: _LL1D:
 next_is_pointer=1;goto _LL1B;}else{_LL1E: _LL1F:
 goto _LL1B;}_LL1B:;}
# 1282
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2A9=(void*)tms->hd;void*_stmttmp1F=_tmp2A9;void*_tmp2AA=_stmttmp1F;struct Cyc_List_List*_tmp2AB;int _tmp2AE;unsigned _tmp2AD;struct Cyc_List_List*_tmp2AC;unsigned _tmp2B0;struct Cyc_List_List*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_List_List*_tmp2B5;void*_tmp2B4;struct Cyc_Absyn_VarargInfo*_tmp2B3;int _tmp2B2;struct Cyc_List_List*_tmp2B1;void*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2B8;void*_tmp2BA;switch(*((int*)_tmp2AA)){case 0U: _LL21: _tmp2BA=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1;_LL22: {void*zeroterm=_tmp2BA;
# 1289
Cyc_Absyndump_dump(({const char*_tmp2BB="[]";_tag_fat(_tmp2BB,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp2BC="@zeroterm";_tag_fat(_tmp2BC,sizeof(char),10U);}));
goto _LL20;}case 1U: _LL23: _tmp2B8=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1;_tmp2B9=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp2B8;void*zeroterm=_tmp2B9;
# 1293
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)']');
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp2BD="@zeroterm";_tag_fat(_tmp2BD,sizeof(char),10U);}));
goto _LL20;}case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->tag == 1U){_LL25: _tmp2B1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f1;_tmp2B2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f2;_tmp2B3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f3;_tmp2B4=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f4;_tmp2B5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f5;_tmp2B6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f6;_tmp2B7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f7;_LL26: {struct Cyc_List_List*args=_tmp2B1;int c_varargs=_tmp2B2;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp2B3;void*effopt=_tmp2B4;struct Cyc_List_List*rgn_po=_tmp2B5;struct Cyc_Absyn_Exp*req=_tmp2B6;struct Cyc_Absyn_Exp*ens=_tmp2B7;
# 1297
Cyc_Absyndump_dumpfunargs(args,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);goto _LL20;}}else{_LL27: _tmp2AF=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f1;_tmp2B0=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1)->f2;_LL28: {struct Cyc_List_List*sl=_tmp2AF;unsigned loc=_tmp2B0;
# 1299
({struct Cyc_List_List*_tmp36A=sl;struct _fat_ptr _tmp369=({const char*_tmp2BE="(";_tag_fat(_tmp2BE,sizeof(char),2U);});struct _fat_ptr _tmp368=({const char*_tmp2BF=")";_tag_fat(_tmp2BF,sizeof(char),2U);});((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp36A,_tmp369,_tmp368,({const char*_tmp2C0=",";_tag_fat(_tmp2C0,sizeof(char),2U);}));});goto _LL20;}}case 4U: _LL29: _tmp2AC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f1;_tmp2AD=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f2;_tmp2AE=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f3;_LL2A: {struct Cyc_List_List*ts=_tmp2AC;unsigned loc=_tmp2AD;int print_kinds=_tmp2AE;
# 1301
if(print_kinds)Cyc_Absyndump_dumpkindedtvars(ts);else{Cyc_Absyndump_dumptvars(ts);}goto _LL20;}case 5U: _LL2B: _tmp2AB=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2AA)->f2;_LL2C: {struct Cyc_List_List*atts=_tmp2AB;
Cyc_Absyndump_dumpatts(atts);goto _LL20;}default: _LL2D: _LL2E:
({void*_tmp2C1=0U;({struct _fat_ptr _tmp36B=({const char*_tmp2C2="dumptms";_tag_fat(_tmp2C2,sizeof(char),8U);});Cyc_Warn_impos(_tmp36B,_tag_fat(_tmp2C1,sizeof(void*),0U));});});}_LL20:;}
# 1305
return;}}_LL0:;}}
# 1309
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2C3=Cyc_Absyndump_is_char_ptr(t);int cp=_tmp2C3;
struct _RegionHandle _tmp2C4=_new_region("temp");struct _RegionHandle*temp=& _tmp2C4;_push_region(temp);
{struct _tuple12 _tmp2C5=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple12 _stmttmp20=_tmp2C5;struct _tuple12 _tmp2C6=_stmttmp20;struct Cyc_List_List*_tmp2C9;void*_tmp2C8;struct Cyc_Absyn_Tqual _tmp2C7;_LL1: _tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;_tmp2C9=_tmp2C6.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp2C7;void*t=_tmp2C8;struct Cyc_List_List*tms=_tmp2C9;
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);
({int _tmp36E=cp;struct Cyc_List_List*_tmp36D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);void(*_tmp36C)(void*)=f;Cyc_Absyndump_dumptms(_tmp36E,_tmp36D,_tmp36C,a);});}}
# 1312
;_pop_region(temp);}
# 1318
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1320
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2CA=0U;({struct Cyc___cycFILE*_tmp370=f;struct _fat_ptr _tmp36F=({const char*_tmp2CB="\n";_tag_fat(_tmp2CB,sizeof(char),2U);});Cyc_fprintf(_tmp370,_tmp36F,_tag_fat(_tmp2CA,sizeof(void*),0U));});});}
# 1326
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2CC=d->r;void*_stmttmp21=_tmp2CC;void*_tmp2CD=_stmttmp21;struct _tuple10*_tmp2D1;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*_tmp2CF;struct Cyc_List_List*_tmp2CE;struct Cyc_List_List*_tmp2D2;struct Cyc_List_List*_tmp2D3;struct Cyc_List_List*_tmp2D5;struct _fat_ptr*_tmp2D4;struct Cyc_Absyn_Typedefdecl*_tmp2D6;struct Cyc_Absyn_Enumdecl*_tmp2D7;struct Cyc_Absyn_Datatypedecl*_tmp2D8;struct Cyc_Absyn_Aggrdecl*_tmp2D9;struct Cyc_Absyn_Fndecl*_tmp2DA;struct Cyc_Absyn_Vardecl*_tmp2DB;switch(*((int*)_tmp2CD)){case 0U: _LL1: _tmp2DB=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp2DB;
# 1329
if((int)vd->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=vd->initializer;
vd->initializer=0;
if((int)vd->sc == (int)Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2DC="extern ";_tag_fat(_tmp2DC,sizeof(char),8U);}));
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2DD="\n";_tag_fat(_tmp2DD,sizeof(char),2U);}));
vd->initializer=init;
goto _LL0;}}case 1U: _LL3: _tmp2DA=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp2DA;
# 1339
if((int)fd->sc == (int)Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(fd->sc);{
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));_tmp2DF->tag=5U,_tmp2DF->f1=type_info;_tmp2DF;});
({struct Cyc_Absyn_Tqual _tmp372=Cyc_Absyn_empty_tqual(0U);void*_tmp371=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp372,_tmp371,Cyc_Absyndump_dump_atts_qvar,fd);});
Cyc_Absyndump_dump(({const char*_tmp2DE=";\n";_tag_fat(_tmp2DE,sizeof(char),3U);}));
goto _LL0;}}}case 5U: _LL5: _tmp2D9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2D9;
# 1348
if((int)ad->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if((int)ad->sc == (int)Cyc_Absyn_Abstract)ad->impl=0;
Cyc_Absyndump_dump_aggrdecl(ad);
ad->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2E0=";\n";_tag_fat(_tmp2E0,sizeof(char),3U);}));
goto _LL0;}}case 6U: _LL7: _tmp2D8=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*dd=_tmp2D8;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump(({const char*_tmp2E1=";\n";_tag_fat(_tmp2E1,sizeof(char),3U);}));goto _LL0;}case 7U: _LL9: _tmp2D7=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp2D7;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump(({const char*_tmp2E2=";\n";_tag_fat(_tmp2E2,sizeof(char),3U);}));goto _LL0;}case 8U: _LLB: _tmp2D6=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp2D6;
# 1358
if(td->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2E3="\n";_tag_fat(_tmp2E3,sizeof(char),2U);}));}
# 1362
goto _LL0;}case 9U: _LLD: _tmp2D4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_tmp2D5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2CD)->f2;_LLE: {struct _fat_ptr*v=_tmp2D4;struct Cyc_List_List*ds=_tmp2D5;
# 1364
Cyc_Absyndump_dump(({const char*_tmp2E4="namespace ";_tag_fat(_tmp2E4,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(v);
Cyc_Absyndump_dump(({const char*_tmp2E5="{\n";_tag_fat(_tmp2E5,sizeof(char),3U);}));
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E6="}\n";_tag_fat(_tmp2E6,sizeof(char),3U);}));
goto _LL0;}case 10U: _LLF: _tmp2D3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2CD)->f2;_LL10: {struct Cyc_List_List*ds=_tmp2D3;
# 1372
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
goto _LL0;}case 11U: _LL11: _tmp2D2=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LL12: {struct Cyc_List_List*ds=_tmp2D2;
# 1376
Cyc_Absyndump_dump(({const char*_tmp2E7="extern \"C\" {";_tag_fat(_tmp2E7,sizeof(char),13U);}));
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E8="}\n";_tag_fat(_tmp2E8,sizeof(char),3U);}));
goto _LL0;}case 12U: _LL13: _tmp2CE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_tmp2CF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CD)->f2;_tmp2D0=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CD)->f3;_tmp2D1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CD)->f4;_LL14: {struct Cyc_List_List*ds=_tmp2CE;struct Cyc_List_List*ovrs=_tmp2CF;struct Cyc_List_List*exs=_tmp2D0;struct _tuple10*wc=_tmp2D1;
# 1397 "absyndump.cyc"
goto _LL0;}default: _LL15: _LL16:
({void*_tmp2E9=0U;({struct _fat_ptr _tmp373=({const char*_tmp2EA="bad top-level declaration";_tag_fat(_tmp2EA,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp373,_tag_fat(_tmp2E9,sizeof(void*),0U));});});}_LL0:;}
# 1402
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}

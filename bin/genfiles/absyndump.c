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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 104
extern int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 278 "cycboot.h"
extern int Cyc_file_string_write(struct Cyc___cycFILE*,struct _fat_ptr src,int src_offset,int max_count);struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 415 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};
# 453
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 461
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 514
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 532
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 700 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 896
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress(void*);
# 907
int Cyc_Absyn_type2bool(int def,void*);
# 916
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1151
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1159
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 62
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 80
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual,void*);struct _tuple13{int f1;struct Cyc_List_List*f2;};
# 95 "absynpp.h"
struct _tuple13 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock);struct _tuple14{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 78 "kinds.h"
void*Cyc_Kinds_compress_kb(void*);
# 73 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 77
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct Cyc_Hashtable_Table;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};struct _tuple15{struct _fat_ptr f1;unsigned f2;};
# 28 "absyndump.cyc"
extern struct _tuple15 Cyc_Lex_xlate_pos(unsigned);
# 37
static int Cyc_Absyndump_expand_typedefs;
# 41
static int Cyc_Absyndump_qvar_to_Cids;
# 44
static int Cyc_Absyndump_add_cyc_prefix;
# 47
static int Cyc_Absyndump_generate_line_directives;
# 51
static int Cyc_Absyndump_to_VC;static char _tmp0[14U]="__NoCycPrefix";
# 53
static struct _fat_ptr Cyc_Absyndump_nocyc_str={_tmp0,_tmp0,_tmp0 + 14U};
static struct _fat_ptr*Cyc_Absyndump_nocyc_strptr=& Cyc_Absyndump_nocyc_str;
# 56
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 71 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 74
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
# 93
static void Cyc_Absyndump_dumploc(unsigned);
# 96
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
static char Cyc_Absyndump_prev_char='x';
# 99
static int Cyc_Absyndump_need_space_before (void){
char _tmp1=Cyc_Absyndump_prev_char;switch(_tmp1){case 123U: _LL1: _LL2:
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
# 119
static void Cyc_Absyndump_dump(struct _fat_ptr s){
unsigned sz=Cyc_strlen((struct _fat_ptr)s);
# 126
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= (unsigned)1){
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),(int)(sz - (unsigned)1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 134
static void Cyc_Absyndump_dump_nospace(struct _fat_ptr s){
int sz=(int)Cyc_strlen((struct _fat_ptr)s);
# 137
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_fat_subscript(s,sizeof(char),sz - 1));}}
# 142
static void Cyc_Absyndump_dump_char(int c){
# 144
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpF[1U]="";
# 148
static void Cyc_Absyndump_dumploc(unsigned loc){
static struct _fat_ptr last_file={_tmpF,_tmpF,_tmpF + 1U};
static unsigned last_line=0U;
if(loc == (unsigned)0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 154
struct _tuple15 _tmp2=Cyc_Lex_xlate_pos(loc);struct _tuple15 _stmttmp0=_tmp2;struct _tuple15 _tmp3=_stmttmp0;unsigned _tmp5;struct _fat_ptr _tmp4;_LL1: _tmp4=_tmp3.f1;_tmp5=_tmp3.f2;_LL2: {struct _fat_ptr f=_tmp4;unsigned d=_tmp5;
if(({char*_tmp304=(char*)f.curr;_tmp304 == (char*)(_tag_fat(0,0,0)).curr;})||(char*)f.curr == (char*)last_file.curr && d == last_line)return;
if((char*)f.curr == (char*)last_file.curr && d == last_line + (unsigned)1)
({void*_tmp6=0U;({struct Cyc___cycFILE*_tmp306=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp305=({const char*_tmp7="\n";_tag_fat(_tmp7,sizeof(char),2U);});Cyc_fprintf(_tmp306,_tmp305,_tag_fat(_tmp6,sizeof(void*),0U));});});else{
if((char*)f.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp2F6;_tmp2F6.tag=1U,_tmp2F6.f1=(unsigned long)((int)d);_tmp2F6;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp308=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp307=({const char*_tmp9="\n# %d\n";_tag_fat(_tmp9,sizeof(char),7U);});Cyc_fprintf(_tmp308,_tmp307,_tag_fat(_tmp8,sizeof(void*),1U));});});else{
# 161
({struct Cyc_Int_pa_PrintArg_struct _tmpD=({struct Cyc_Int_pa_PrintArg_struct _tmp2F8;_tmp2F8.tag=1U,_tmp2F8.f1=(unsigned long)((int)d);_tmp2F8;});struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=0U,_tmp2F7.f1=(struct _fat_ptr)((struct _fat_ptr)f);_tmp2F7;});void*_tmpB[2U];_tmpB[0]=& _tmpD,_tmpB[1]=& _tmpE;({struct Cyc___cycFILE*_tmp30A=*Cyc_Absyndump_dump_file;struct _fat_ptr _tmp309=({const char*_tmpC="\n# %d %s\n";_tag_fat(_tmpC,sizeof(char),10U);});Cyc_fprintf(_tmp30A,_tmp309,_tag_fat(_tmpB,sizeof(void*),2U));});});}}
# 163
last_file=f;
last_line=d;}}}
# 169
static void Cyc_Absyndump_dump_str(struct _fat_ptr*s){
Cyc_Absyndump_dump(*s);}
# 172
static void Cyc_Absyndump_dump_str_nospace(struct _fat_ptr*s){
Cyc_Absyndump_dump_nospace(*s);}
# 176
static void Cyc_Absyndump_dump_semi (void){
Cyc_Absyndump_dump_char((int)';');}
# 180
static void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
static void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 196
f(env,l->hd);}
# 198
static void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 200
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 204
static void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 206
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 210
static void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 212
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 216
static void Cyc_Absyndump_ignore(void*x){;}
# 218
static void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmp10=0;struct Cyc_List_List*nsl=_tmp10;
struct _fat_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp11=(*v).f1;union Cyc_Absyn_Nmspace _stmttmp1=_tmp11;union Cyc_Absyn_Nmspace _tmp12=_stmttmp1;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;switch((_tmp12.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp15=(_tmp12.Rel_n).val;_LL4: {struct Cyc_List_List*x=_tmp15;
nsl=x;goto _LL0;}case 3U: _LL5: _tmp14=(_tmp12.C_n).val;_LL6: {struct Cyc_List_List*x=_tmp14;
goto _LL0;}default: _LL7: _tmp13=(_tmp12.Abs_n).val;_LL8: {struct Cyc_List_List*x=_tmp13;
# 226
if(Cyc_Absyndump_qvar_to_Cids || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
nsl=x;
goto _LL0;}}_LL0:;}
# 231
if(Cyc_Absyndump_need_space_before())
Cyc_Absyndump_dump_char((int)' ');{
struct _fat_ptr sep=Cyc_Absyndump_qvar_to_Cids?({const char*_tmp16="_";_tag_fat(_tmp16,sizeof(char),2U);}):({const char*_tmp17="::";_tag_fat(_tmp17,sizeof(char),3U);});
if((unsigned)nsl && !Cyc_strptrcmp((struct _fat_ptr*)nsl->hd,Cyc_Absyndump_nocyc_strptr)){
nsl=nsl->tl;
prefix=0;}
# 238
if(prefix != 0){
Cyc_Absyndump_dump_nospace(*(*prefix));Cyc_Absyndump_dump_nospace(sep);}
# 241
if(nsl != 0){
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_str_nospace,nsl,sep);Cyc_Absyndump_dump_nospace(sep);}
# 244
Cyc_Absyndump_dump_nospace(*(*v).f2);}}
# 247
static void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp18="restrict";_tag_fat(_tmp18,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp19="volatile";_tag_fat(_tmp19,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp1A="const";_tag_fat(_tmp1A,sizeof(char),6U);}));}
# 252
static void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1B=sc;switch(_tmp1B){case Cyc_Absyn_Static: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp1C="static";_tag_fat(_tmp1C,sizeof(char),7U);}));return;case Cyc_Absyn_Public: _LL3: _LL4:
 return;case Cyc_Absyn_Extern: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp1D="extern";_tag_fat(_tmp1D,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1E="extern \"C\"";_tag_fat(_tmp1E,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract: _LL9: _LLA:
 Cyc_Absyndump_dump(({const char*_tmp1F="abstract";_tag_fat(_tmp1F,sizeof(char),9U);}));return;case Cyc_Absyn_Register: _LLB: _LLC:
 Cyc_Absyndump_dump(({const char*_tmp20="register";_tag_fat(_tmp20,sizeof(char),9U);}));return;default: _LLD: _LLE:
 return;}_LL0:;}
# 263
static void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp21=k;if(_tmp21 == Cyc_Absyn_StructA){_LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp22="struct";_tag_fat(_tmp22,sizeof(char),7U);}));return;}else{_LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp23="union";_tag_fat(_tmp23,sizeof(char),6U);}));return;}_LL0:;}
# 270
static void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
Cyc_Absyndump_dump(Cyc_Absynpp_kind2string(ka));}
# 274
static void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp30D=ts;struct _fat_ptr _tmp30C=({const char*_tmp24="<";_tag_fat(_tmp24,sizeof(char),2U);});struct _fat_ptr _tmp30B=({const char*_tmp25=">";_tag_fat(_tmp25,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp30D,_tmp30C,_tmp30B,({const char*_tmp26=",";_tag_fat(_tmp26,sizeof(char),2U);}));});}
# 277
static void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
if((int)*((const char*)_check_fat_subscript(n,sizeof(char),0))== (int)'#'){
Cyc_Absyndump_dump(({const char*_tmp27="`G";_tag_fat(_tmp27,sizeof(char),3U);}));
{void*_tmp28=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp2=_tmp28;void*_tmp29=_stmttmp2;struct Cyc_Absyn_Kind*_tmp2A;struct Cyc_Absyn_Kind*_tmp2B;switch(*((int*)_tmp29)){case 0U: _LL1: _tmp2B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp29)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp2B;
_tmp2A=k;goto _LL4;}case 2U: _LL3: _tmp2A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp29)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp2A;
Cyc_Absyndump_dumpkind(k);goto _LL0;}default: _LL5: _LL6:
 Cyc_Absyndump_dump_nospace(({const char*_tmp2C="K";_tag_fat(_tmp2C,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 286
Cyc_Absyndump_dump_nospace(_fat_ptr_plus(n,sizeof(char),1));}else{
# 288
Cyc_Absyndump_dump(n);}}
# 290
static void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp2D=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp3=_tmp2D;void*_tmp2E=_stmttmp3;struct Cyc_Absyn_Kind*_tmp2F;switch(*((int*)_tmp2E)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL5: _LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _tmp2F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_tmp2F;
Cyc_Absyndump_dump(({const char*_tmp30="::";_tag_fat(_tmp30,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(k);goto _LL0;}}}_LL0:;}}
# 299
static void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp310=tvs;struct _fat_ptr _tmp30F=({const char*_tmp31="<";_tag_fat(_tmp31,sizeof(char),2U);});struct _fat_ptr _tmp30E=({const char*_tmp32=">";_tag_fat(_tmp32,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp310,_tmp30F,_tmp30E,({const char*_tmp33=",";_tag_fat(_tmp33,sizeof(char),2U);}));});}
# 302
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp313=tvs;struct _fat_ptr _tmp312=({const char*_tmp34="<";_tag_fat(_tmp34,sizeof(char),2U);});struct _fat_ptr _tmp311=({const char*_tmp35=">";_tag_fat(_tmp35,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp313,_tmp312,_tmp311,({const char*_tmp36=",";_tag_fat(_tmp36,sizeof(char),2U);}));});}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 305
static void Cyc_Absyndump_dumparg(struct _tuple16*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 308
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp316=ts;struct _fat_ptr _tmp315=({const char*_tmp37="(";_tag_fat(_tmp37,sizeof(char),2U);});struct _fat_ptr _tmp314=({const char*_tmp38=")";_tag_fat(_tmp38,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp316,_tmp315,_tmp314,({const char*_tmp39=",";_tag_fat(_tmp39,sizeof(char),2U);}));});}
# 312
static void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp3A=(void*)atts->hd;void*_stmttmp4=_tmp3A;void*_tmp3B=_stmttmp4;switch(*((int*)_tmp3B)){case 1U: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp3C="_stdcall";_tag_fat(_tmp3C,sizeof(char),9U);}));return;case 2U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp3D="_cdecl";_tag_fat(_tmp3D,sizeof(char),7U);}));return;case 3U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp3E="_fastcall";_tag_fat(_tmp3E,sizeof(char),10U);}));return;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 322
static void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 324
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp3F=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_stmttmp5=_tmp3F;void*_tmp40=_stmttmp5;switch(*((int*)_tmp40)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 332
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp41="__declspec(";_tag_fat(_tmp41,sizeof(char),12U);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp42=(void*)atts->hd;void*_stmttmp6=_tmp42;void*_tmp43=_stmttmp6;switch(*((int*)_tmp43)){case 1U: _LLA: _LLB:
 goto _LLD;case 2U: _LLC: _LLD:
 goto _LLF;case 3U: _LLE: _LLF:
 goto _LL9;default: _LL10: _LL11:
 Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 342
Cyc_Absyndump_dump_char((int)')');}
# 345
static void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Flags_C_Compilers _tmp44=Cyc_Flags_c_compiler;if(_tmp44 == Cyc_Flags_Gcc_c){_LL1: _LL2:
# 349
 Cyc_Absyndump_dump(({const char*_tmp45=" __attribute__((";_tag_fat(_tmp45,sizeof(char),17U);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp46=",";_tag_fat(_tmp46,sizeof(char),2U);}));}
# 354
Cyc_Absyndump_dump(({const char*_tmp47=")) ";_tag_fat(_tmp47,sizeof(char),4U);}));
return;}else{_LL3: _LL4:
# 357
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL0:;}}
# 362
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 366
static struct _tuple17 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
{void*_tmp48=Cyc_Absyn_compress(t);void*_stmttmp7=_tmp48;void*_tmp49=_stmttmp7;struct Cyc_List_List*_tmp4A;void*_tmp4B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f2 != 0){_LL1: _tmp4B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f2)->hd;_LL2: {void*r=_tmp4B;
rgions=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=r,_tmp4C->tl=rgions;_tmp4C;});goto _LL0;}}else{goto _LL5;}case 9U: _LL3: _tmp4A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp49)->f2;_LL4: {struct Cyc_List_List*ts=_tmp4A;
# 372
for(0;ts != 0;ts=ts->tl){
struct _tuple17 _tmp4D=Cyc_Absyndump_effects_split((void*)ts->hd);struct _tuple17 _stmttmp8=_tmp4D;struct _tuple17 _tmp4E=_stmttmp8;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp4F;_LL8: _tmp4F=_tmp4E.f1;_tmp50=_tmp4E.f2;_LL9: {struct Cyc_List_List*rs=_tmp4F;struct Cyc_List_List*es=_tmp50;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(rs,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,effects);}}
# 377
goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 effects=({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->hd=t,_tmp51->tl=effects;_tmp51;});goto _LL0;}_LL0:;}
# 380
return({struct _tuple17 _tmp2F9;_tmp2F9.f1=rgions,_tmp2F9.f2=effects;_tmp2F9;});}
# 383
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple17 _tmp52=Cyc_Absyndump_effects_split(t);struct _tuple17 _stmttmp9=_tmp52;struct _tuple17 _tmp53=_stmttmp9;struct Cyc_List_List*_tmp55;struct Cyc_List_List*_tmp54;_LL1: _tmp54=_tmp53.f1;_tmp55=_tmp53.f2;_LL2: {struct Cyc_List_List*rgions=_tmp54;struct Cyc_List_List*effects=_tmp55;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(effects != 0){
({struct Cyc_List_List*_tmp319=effects;struct _fat_ptr _tmp318=({const char*_tmp56="";_tag_fat(_tmp56,sizeof(char),1U);});struct _fat_ptr _tmp317=({const char*_tmp57="";_tag_fat(_tmp57,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpntyp,_tmp319,_tmp318,_tmp317,({const char*_tmp58="+";_tag_fat(_tmp58,sizeof(char),2U);}));});
if(rgions != 0)Cyc_Absyndump_dump_char((int)'+');}
# 391
if(rgions != 0 || effects == 0)
({struct Cyc_List_List*_tmp31C=rgions;struct _fat_ptr _tmp31B=({const char*_tmp59="{";_tag_fat(_tmp59,sizeof(char),2U);});struct _fat_ptr _tmp31A=({const char*_tmp5A="}";_tag_fat(_tmp5A,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumprgn,_tmp31C,_tmp31B,_tmp31A,({const char*_tmp5B=",";_tag_fat(_tmp5B,sizeof(char),2U);}));});}}
# 396
static void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp5C=t;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_List_List*_tmp60;struct _tuple0*_tmp5F;struct Cyc_Absyn_Datatypedecl*_tmp61;struct Cyc_Absyn_Enumdecl*_tmp62;struct Cyc_Absyn_Aggrdecl*_tmp63;struct Cyc_List_List*_tmp65;enum Cyc_Absyn_AggrKind _tmp64;struct Cyc_List_List*_tmp66;int _tmp69;void*_tmp68;struct Cyc_Core_Opt*_tmp67;int _tmp6B;struct Cyc_Core_Opt*_tmp6A;struct Cyc_Absyn_Tvar*_tmp6C;struct Cyc_List_List*_tmp6E;void*_tmp6D;switch(*((int*)_tmp5C)){case 4U: _LL1: _LL2:
# 399
 goto _LL4;case 5U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return;case 0U: if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C)->f1)->tag == 0U){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp6F="void";_tag_fat(_tmp6F,sizeof(char),5U);}));return;}else{_LL1F: _tmp6D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C)->f1;_tmp6E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C)->f2;_LL20: {void*c=_tmp6D;struct Cyc_List_List*ts=_tmp6E;
# 419
void*_tmp79=c;struct _fat_ptr _tmp7A;struct Cyc_List_List*_tmp7B;struct _tuple0*_tmp7C;union Cyc_Absyn_AggrInfo _tmp7D;int _tmp7E;union Cyc_Absyn_DatatypeFieldInfo _tmp7F;union Cyc_Absyn_DatatypeInfo _tmp80;switch(*((int*)_tmp79)){case 18U: _LL22: _tmp80=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp79)->f1;_LL23: {union Cyc_Absyn_DatatypeInfo tu_info=_tmp80;
# 421
{union Cyc_Absyn_DatatypeInfo _tmp81=tu_info;int _tmp83;struct _tuple0*_tmp82;int _tmp85;struct _tuple0*_tmp84;if((_tmp81.UnknownDatatype).tag == 1){_LL69: _tmp84=((_tmp81.UnknownDatatype).val).name;_tmp85=((_tmp81.UnknownDatatype).val).is_extensible;_LL6A: {struct _tuple0*n=_tmp84;int is_x=_tmp85;
_tmp82=n;_tmp83=is_x;goto _LL6C;}}else{_LL6B: _tmp82=(*(_tmp81.KnownDatatype).val)->name;_tmp83=(*(_tmp81.KnownDatatype).val)->is_extensible;_LL6C: {struct _tuple0*n=_tmp82;int is_x=_tmp83;
# 424
if(is_x)Cyc_Absyndump_dump(({const char*_tmp86="@extensible";_tag_fat(_tmp86,sizeof(char),12U);}));
Cyc_Absyndump_dump(({const char*_tmp87="datatype";_tag_fat(_tmp87,sizeof(char),9U);}));Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
goto _LL68;}}_LL68:;}
# 428
return;}case 19U: _LL24: _tmp7F=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp79)->f1;_LL25: {union Cyc_Absyn_DatatypeFieldInfo tuf_info=_tmp7F;
# 430
{union Cyc_Absyn_DatatypeFieldInfo _tmp88=tuf_info;struct _tuple0*_tmp8B;int _tmp8A;struct _tuple0*_tmp89;int _tmp8E;struct _tuple0*_tmp8D;struct _tuple0*_tmp8C;if((_tmp88.UnknownDatatypefield).tag == 1){_LL6E: _tmp8C=((_tmp88.UnknownDatatypefield).val).datatype_name;_tmp8D=((_tmp88.UnknownDatatypefield).val).field_name;_tmp8E=((_tmp88.UnknownDatatypefield).val).is_extensible;_LL6F: {struct _tuple0*tname=_tmp8C;struct _tuple0*fname=_tmp8D;int is_x=_tmp8E;
# 432
_tmp89=tname;_tmp8A=is_x;_tmp8B=fname;goto _LL71;}}else{_LL70: _tmp89=(((_tmp88.KnownDatatypefield).val).f1)->name;_tmp8A=(((_tmp88.KnownDatatypefield).val).f1)->is_extensible;_tmp8B=(((_tmp88.KnownDatatypefield).val).f2)->name;_LL71: {struct _tuple0*tname=_tmp89;int is_x=_tmp8A;struct _tuple0*fname=_tmp8B;
# 435
if(is_x)Cyc_Absyndump_dump(({const char*_tmp8F="@extensible ";_tag_fat(_tmp8F,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp90="datatype ";_tag_fat(_tmp90,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(tname);Cyc_Absyndump_dump(({const char*_tmp91=".";_tag_fat(_tmp91,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(fname);
Cyc_Absyndump_dumptps(ts);
goto _LL6D;}}_LL6D:;}
# 440
return;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp79)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp79)->f2){case Cyc_Absyn_Int_sz: _LL26: _LL27:
 goto _LL29;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
# 443
 goto _LL2D;case Cyc_Absyn_Char_sz: _LL2E: _LL2F:
# 445
 Cyc_Absyndump_dump(({const char*_tmp94="char";_tag_fat(_tmp94,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz: _LL34: _LL35:
# 448
 goto _LL37;case Cyc_Absyn_LongLong_sz: _LL40: _LL41:
# 454
 goto _LL43;default: goto _LL66;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp79)->f2){case Cyc_Absyn_Int_sz: _LL28: _LL29:
# 442
 Cyc_Absyndump_dump(({const char*_tmp92="int";_tag_fat(_tmp92,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL2C: _LL2D:
# 444
 Cyc_Absyndump_dump(({const char*_tmp93="long";_tag_fat(_tmp93,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz: _LL30: _LL31:
# 446
 Cyc_Absyndump_dump(({const char*_tmp95="signed char";_tag_fat(_tmp95,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL36: _LL37:
# 449
 Cyc_Absyndump_dump(({const char*_tmp97="short";_tag_fat(_tmp97,sizeof(char),6U);}));return;case Cyc_Absyn_LongLong_sz: _LL42: _LL43: {
# 456
enum Cyc_Flags_C_Compilers _tmp9C=Cyc_Flags_c_compiler;if(_tmp9C == Cyc_Flags_Gcc_c){_LL73: _LL74:
 Cyc_Absyndump_dump(({const char*_tmp9D="long long";_tag_fat(_tmp9D,sizeof(char),10U);}));return;}else{_LL75: _LL76:
 Cyc_Absyndump_dump(({const char*_tmp9E="__int64";_tag_fat(_tmp9E,sizeof(char),8U);}));return;}_LL72:;}default: goto _LL66;}case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp79)->f2){case Cyc_Absyn_Char_sz: _LL32: _LL33:
# 447
 Cyc_Absyndump_dump(({const char*_tmp96="unsigned char";_tag_fat(_tmp96,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz: _LL38: _LL39:
# 450
 Cyc_Absyndump_dump(({const char*_tmp98="unsigned short";_tag_fat(_tmp98,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz: _LL3A: _LL3B:
 Cyc_Absyndump_dump(({const char*_tmp99="unsigned";_tag_fat(_tmp99,sizeof(char),9U);}));return;case Cyc_Absyn_Long_sz: _LL3C: _LL3D:
 Cyc_Absyndump_dump(({const char*_tmp9A="unsigned long";_tag_fat(_tmp9A,sizeof(char),14U);}));return;case Cyc_Absyn_LongLong_sz: _LL3E: _LL3F:
 Cyc_Absyndump_dump(({const char*_tmp9B="unsigned";_tag_fat(_tmp9B,sizeof(char),9U);}));goto _LL41;default: goto _LL66;}default: goto _LL66;}case 2U: _LL44: _tmp7E=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp79)->f1;_LL45: {int i=_tmp7E;
# 461
int _tmp9F=i;switch(_tmp9F){case 0U: _LL78: _LL79:
 Cyc_Absyndump_dump(({const char*_tmpA0="float";_tag_fat(_tmpA0,sizeof(char),6U);}));return;case 1U: _LL7A: _LL7B:
 Cyc_Absyndump_dump(({const char*_tmpA1="double";_tag_fat(_tmpA1,sizeof(char),7U);}));return;default: _LL7C: _LL7D:
 Cyc_Absyndump_dump(({const char*_tmpA2="long double";_tag_fat(_tmpA2,sizeof(char),12U);}));return;}_LL77:;}case 20U: _LL46: _tmp7D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp79)->f1;_LL47: {union Cyc_Absyn_AggrInfo info=_tmp7D;
# 467
struct _tuple11 _tmpA3=Cyc_Absyn_aggr_kinded_name(info);struct _tuple11 _stmttmpA=_tmpA3;struct _tuple11 _tmpA4=_stmttmpA;struct _tuple0*_tmpA6;enum Cyc_Absyn_AggrKind _tmpA5;_LL7F: _tmpA5=_tmpA4.f1;_tmpA6=_tmpA4.f2;_LL80: {enum Cyc_Absyn_AggrKind k=_tmpA5;struct _tuple0*n=_tmpA6;
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
return;}}case 15U: _LL48: _tmp7C=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp79)->f1;_LL49: {struct _tuple0*n=_tmp7C;
Cyc_Absyndump_dump(({const char*_tmpA7="enum";_tag_fat(_tmpA7,sizeof(char),5U);}));Cyc_Absyndump_dumpqvar(n);return;}case 16U: _LL4A: _tmp7B=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp79)->f1;_LL4B: {struct Cyc_List_List*fs=_tmp7B;
Cyc_Absyndump_dump(({const char*_tmpA8="enum{";_tag_fat(_tmpA8,sizeof(char),6U);}));Cyc_Absyndump_dumpenumfields(fs);Cyc_Absyndump_dump(({const char*_tmpA9="}";_tag_fat(_tmpA9,sizeof(char),2U);}));return;}case 17U: _LL4C: _tmp7A=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp79)->f1;_LL4D: {struct _fat_ptr t=_tmp7A;
Cyc_Absyndump_dump(t);return;}case 3U: _LL4E: _LL4F:
 Cyc_Absyndump_dump(({const char*_tmpAA="region_t<";_tag_fat(_tmpAA,sizeof(char),10U);}));Cyc_Absyndump_dumprgn((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpAB=">";_tag_fat(_tmpAB,sizeof(char),2U);}));return;case 4U: _LL50: _LL51:
 Cyc_Absyndump_dump(({const char*_tmpAC="tag_t<";_tag_fat(_tmpAC,sizeof(char),7U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpAD=">";_tag_fat(_tmpAD,sizeof(char),2U);}));return;case 6U: _LL52: _LL53:
 Cyc_Absyndump_dump(({const char*_tmpAE="`U";_tag_fat(_tmpAE,sizeof(char),3U);}));return;case 7U: _LL54: _LL55:
 Cyc_Absyndump_dump(({const char*_tmpAF="`RC";_tag_fat(_tmpAF,sizeof(char),4U);}));return;case 5U: _LL56: _LL57:
 Cyc_Absyndump_dump(({const char*_tmpB0="`H";_tag_fat(_tmpB0,sizeof(char),3U);}));return;case 8U: _LL58: _LL59:
 Cyc_Absyndump_dump(({const char*_tmpB1="{";_tag_fat(_tmpB1,sizeof(char),2U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpB2="}";_tag_fat(_tmpB2,sizeof(char),2U);}));return;case 10U: _LL5A: _LL5B:
 Cyc_Absyndump_dump(({const char*_tmpB3="regions(";_tag_fat(_tmpB3,sizeof(char),9U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(ts))->hd);Cyc_Absyndump_dump(({const char*_tmpB4=")";_tag_fat(_tmpB4,sizeof(char),2U);}));return;case 9U: _LL5C: _LL5D:
({struct Cyc_List_List*_tmp31F=ts;struct _fat_ptr _tmp31E=({const char*_tmpB5="";_tag_fat(_tmpB5,sizeof(char),1U);});struct _fat_ptr _tmp31D=({const char*_tmpB6="";_tag_fat(_tmpB6,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumptyp,_tmp31F,_tmp31E,_tmp31D,({const char*_tmpB7="+";_tag_fat(_tmpB7,sizeof(char),2U);}));});return;case 11U: _LL5E: _LL5F:
 Cyc_Absyndump_dump(({const char*_tmpB8="@true";_tag_fat(_tmpB8,sizeof(char),6U);}));return;case 12U: _LL60: _LL61:
 Cyc_Absyndump_dump(({const char*_tmpB9="@false";_tag_fat(_tmpB9,sizeof(char),7U);}));return;case 13U: _LL62: _LL63:
 Cyc_Absyndump_dump(({const char*_tmpBA="@thin";_tag_fat(_tmpBA,sizeof(char),6U);}));return;case 14U: _LL64: _LL65:
 Cyc_Absyndump_dump(({const char*_tmpBB="@fat";_tag_fat(_tmpBB,sizeof(char),5U);}));return;default: _LL66: _LL67:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FA;_tmp2FA.tag=0U,({struct _fat_ptr _tmp320=({const char*_tmpBE="bad type constructor";_tag_fat(_tmpBE,sizeof(char),21U);});_tmp2FA.f1=_tmp320;});_tmp2FA;});void*_tmpBC[1U];_tmpBC[0]=& _tmpBD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpBC,sizeof(void*),1U));});}_LL21:;}}case 2U: _LL9: _tmp6C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5C)->f1;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp6C;
# 403
Cyc_Absyndump_dumptvar(tv);return;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5C)->f2 == 0){_LLB: _tmp6A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5C)->f1;_tmp6B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5C)->f3;_LLC: {struct Cyc_Core_Opt*k=_tmp6A;int i=_tmp6B;
# 405
Cyc_Absyndump_dump(({const char*_tmp70="`E";_tag_fat(_tmp70,sizeof(char),3U);}));
if(k == 0)Cyc_Absyndump_dump(({const char*_tmp71="K";_tag_fat(_tmp71,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)k->v);}
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp74=({struct Cyc_Int_pa_PrintArg_struct _tmp2FB;_tmp2FB.tag=1U,_tmp2FB.f1=(unsigned long)i;_tmp2FB;});void*_tmp72[1U];_tmp72[0]=& _tmp74;({struct _fat_ptr _tmp321=({const char*_tmp73="%d";_tag_fat(_tmp73,sizeof(char),3U);});Cyc_aprintf(_tmp321,_tag_fat(_tmp72,sizeof(void*),1U));});}));return;}}else{_LLD: _tmp67=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5C)->f1;_tmp68=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5C)->f2;_tmp69=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5C)->f3;_LLE: {struct Cyc_Core_Opt*k=_tmp67;void*t=_tmp68;int i=_tmp69;
Cyc_Absyndump_dumpntyp((void*)_check_null(t));return;}}case 6U: _LLF: _tmp66=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C)->f1;_LL10: {struct Cyc_List_List*ts=_tmp66;
Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(ts);return;}case 7U: _LL11: _tmp64=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C)->f1;_tmp65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp64;struct Cyc_List_List*fs=_tmp65;
# 411
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(fs);Cyc_Absyndump_dump_char((int)'}');return;}case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5C)->f1)->r)){case 0U: _LL13: _tmp63=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5C)->f1)->r)->f1;_LL14: {struct Cyc_Absyn_Aggrdecl*d=_tmp63;
Cyc_Absyndump_dump_aggrdecl(d);return;}case 1U: _LL15: _tmp62=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5C)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Enumdecl*d=_tmp62;
Cyc_Absyndump_dump_enumdecl(d);return;}default: _LL17: _tmp61=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5C)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Datatypedecl*d=_tmp61;
Cyc_Absyndump_dump_datatypedecl(d);return;}}case 8U: _LL19: _tmp5F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5C)->f1;_tmp60=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5C)->f2;_LL1A: {struct _tuple0*n=_tmp5F;struct Cyc_List_List*ts=_tmp60;
Cyc_Absyndump_dumpqvar(n),Cyc_Absyndump_dumptps(ts);return;}case 9U: _LL1B: _tmp5E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5C)->f1;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp5E;
Cyc_Absyndump_dump(({const char*_tmp75="valueof_t(";_tag_fat(_tmp75,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp76=")";_tag_fat(_tmp76,sizeof(char),2U);}));return;}default: _LL1D: _tmp5D=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5C)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp5D;
Cyc_Absyndump_dump(({const char*_tmp77="typeof(";_tag_fat(_tmp77,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp78=")";_tag_fat(_tmp78,sizeof(char),2U);}));return;}}_LL0:;}
# 490
static void Cyc_Absyndump_dumpvaropt(struct _fat_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 493
static void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple18{void*f1;void*f2;};
# 496
static void Cyc_Absyndump_dump_rgncmp(struct _tuple18*cmp){
Cyc_Absyndump_dumpeff((*cmp).f1);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn((*cmp).f2);}
# 499
static void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp322=rgn_po;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp322,({const char*_tmpBF=",";_tag_fat(_tmpBF,sizeof(char),2U);}));});}
# 502
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 506
if((((args == 0 && !c_varargs)&& cyc_varargs == 0)&& effopt == 0)&& rgn_po == 0)
# 508
Cyc_Absyndump_dump(({const char*_tmpC0="(void)";_tag_fat(_tmpC0,sizeof(char),7U);}));else{
# 511
({struct Cyc_List_List*_tmp325=args;struct _fat_ptr _tmp324=({const char*_tmpC1="(";_tag_fat(_tmpC1,sizeof(char),2U);});struct _fat_ptr _tmp323=({const char*_tmpC2="";_tag_fat(_tmpC2,sizeof(char),1U);});((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpfunarg,_tmp325,_tmp324,_tmp323,({const char*_tmpC3=",";_tag_fat(_tmpC3,sizeof(char),2U);}));});
if(c_varargs || cyc_varargs != 0)
Cyc_Absyndump_dump(({const char*_tmpC4=",...";_tag_fat(_tmpC4,sizeof(char),5U);}));
if(cyc_varargs != 0){
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpC5=" inject ";_tag_fat(_tmpC5,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(({struct _tuple8*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->f1=cyc_varargs->name,_tmpC6->f2=cyc_varargs->tq,_tmpC6->f3=cyc_varargs->type;_tmpC6;}));}
# 518
if(effopt != 0){
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff(effopt);}
# 521
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 524
Cyc_Absyndump_dump_char((int)')');}
# 526
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpC7=" @requires(";_tag_fat(_tmpC7,sizeof(char),12U);}));Cyc_Absyndump_dumpexp(req);Cyc_Absyndump_dump_char((int)')');}
# 529
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpC8=" @ensures(";_tag_fat(_tmpC8,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(ens);Cyc_Absyndump_dump_char((int)')');}}
# 534
static void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _tmp326=Cyc_Absyn_empty_tqual(0U);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp326,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 538
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpC9=d;struct _fat_ptr*_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC9)->tag == 0U){_LL1: _tmpCB=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC9)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpCB;
Cyc_Absyndump_dump(({const char*_tmpCC=".[";_tag_fat(_tmpCC,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)']');goto _LL0;}}else{_LL3: _tmpCA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpC9)->f1;_LL4: {struct _fat_ptr*v=_tmpCA;
Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*v);goto _LL0;}}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 544
static void Cyc_Absyndump_dumpde(struct _tuple19*de){
({struct Cyc_List_List*_tmp329=(*de).f1;struct _fat_ptr _tmp328=({const char*_tmpCD="";_tag_fat(_tmpCD,sizeof(char),1U);});struct _fat_ptr _tmp327=({const char*_tmpCE="=";_tag_fat(_tmpCE,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp329,_tmp328,_tmp327,({const char*_tmpCF="=";_tag_fat(_tmpCF,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 549
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_tmpD0=f;unsigned _tmpD1;struct _fat_ptr*_tmpD2;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpD0)->tag == 0U){_LL1: _tmpD2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpD0)->f1;_LL2: {struct _fat_ptr*n=_tmpD2;
Cyc_Absyndump_dump_nospace(*n);return;}}else{_LL3: _tmpD1=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpD0)->f1;_LL4: {unsigned n=_tmpD1;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD5=({struct Cyc_Int_pa_PrintArg_struct _tmp2FC;_tmp2FC.tag=1U,_tmp2FC.f1=(unsigned long)((int)n);_tmp2FC;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({struct _fat_ptr _tmp32A=({const char*_tmpD4="%d";_tag_fat(_tmpD4,sizeof(char),3U);});Cyc_aprintf(_tmp32A,_tag_fat(_tmpD3,sizeof(void*),1U));});}));return;}}_LL0:;}struct _tuple20{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 556
static void Cyc_Absyndump_dump_asm_iolist(struct Cyc_List_List*iolist){
while((unsigned)iolist){
struct _tuple20*_tmpD6=(struct _tuple20*)iolist->hd;struct _tuple20*_stmttmpB=_tmpD6;struct _tuple20*_tmpD7=_stmttmpB;struct Cyc_Absyn_Exp*_tmpD9;struct _fat_ptr _tmpD8;_LL1: _tmpD8=_tmpD7->f1;_tmpD9=_tmpD7->f2;_LL2: {struct _fat_ptr constr=_tmpD8;struct Cyc_Absyn_Exp*e1=_tmpD9;
Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(constr));Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');
iolist=iolist->tl;
if((unsigned)iolist)
Cyc_Absyndump_dump_char((int)',');}}}
# 577 "absyndump.cyc"
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp32E=inprec;struct Cyc_List_List*_tmp32D=es;struct _fat_ptr _tmp32C=({const char*_tmpDA="";_tag_fat(_tmpDA,sizeof(char),1U);});struct _fat_ptr _tmp32B=({const char*_tmpDB="";_tag_fat(_tmpDB,sizeof(char),1U);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp32E,_tmp32D,_tmp32C,_tmp32B,({const char*_tmpDC=",";_tag_fat(_tmpDC,sizeof(char),2U);}));});}
# 581
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpDD="(";_tag_fat(_tmpDD,sizeof(char),2U);}));
{void*_tmpDE=e->r;void*_stmttmpC=_tmpDE;void*_tmpDF=_stmttmpC;struct Cyc_Absyn_Stmt*_tmpE0;struct Cyc_List_List*_tmpE2;struct Cyc_Core_Opt*_tmpE1;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;int _tmpE9;struct Cyc_Absyn_Exp*_tmpE8;void**_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;int _tmpE5;struct Cyc_Absyn_Datatypefield*_tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_Enumfield*_tmpEC;struct Cyc_Absyn_Enumfield*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpF1;struct Cyc_List_List*_tmpF0;struct _tuple0*_tmpEF;void*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Vardecl*_tmpF4;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF9;struct _tuple8*_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct _fat_ptr*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct _fat_ptr*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_List_List*_tmp102;void*_tmp101;struct _fat_ptr*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_List_List*_tmp107;struct _fat_ptr _tmp106;int _tmp105;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;void*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;void*_tmp10E;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp114;void*_tmp113;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_List_List*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Core_Opt*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_List_List*_tmp12B;enum Cyc_Absyn_Primop _tmp12A;struct _fat_ptr _tmp12C;void*_tmp12D;struct _fat_ptr _tmp12E;struct _fat_ptr _tmp12F;struct _fat_ptr _tmp130;long long _tmp132;enum Cyc_Absyn_Sign _tmp131;int _tmp133;int _tmp134;int _tmp135;short _tmp137;enum Cyc_Absyn_Sign _tmp136;struct _fat_ptr _tmp138;char _tmp13A;enum Cyc_Absyn_Sign _tmp139;switch(*((int*)_tmpDF)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).String_c).tag){case 2U: _LL1: _tmp139=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Char_c).val).f1;_tmp13A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Char_c).val).f2;_LL2: {enum Cyc_Absyn_Sign sg=_tmp139;char ch=_tmp13A;
# 587
Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_char((int)'\'');
goto _LL0;}case 3U: _LL3: _tmp138=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Wchar_c).val;_LL4: {struct _fat_ptr s=_tmp138;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp13D=({struct Cyc_String_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=0U,_tmp2FD.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2FD;});void*_tmp13B[1U];_tmp13B[0]=& _tmp13D;({struct _fat_ptr _tmp32F=({const char*_tmp13C="L'%s'";_tag_fat(_tmp13C,sizeof(char),6U);});Cyc_aprintf(_tmp32F,_tag_fat(_tmp13B,sizeof(void*),1U));});}));goto _LL0;}case 4U: _LL5: _tmp136=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Short_c).val).f1;_tmp137=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Short_c).val).f2;_LL6: {enum Cyc_Absyn_Sign sg=_tmp136;short s=_tmp137;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp140=({struct Cyc_Int_pa_PrintArg_struct _tmp2FE;_tmp2FE.tag=1U,_tmp2FE.f1=(unsigned long)((int)s);_tmp2FE;});void*_tmp13E[1U];_tmp13E[0]=& _tmp140;({struct _fat_ptr _tmp330=({const char*_tmp13F="%d";_tag_fat(_tmp13F,sizeof(char),3U);});Cyc_aprintf(_tmp330,_tag_fat(_tmp13E,sizeof(void*),1U));});}));goto _LL0;}case 5U: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL7: _tmp135=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Int_c).val).f2;_LL8: {int i=_tmp135;
_tmp134=i;goto _LLA;}case Cyc_Absyn_Signed: _LL9: _tmp134=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Int_c).val).f2;_LLA: {int i=_tmp134;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp143=({struct Cyc_Int_pa_PrintArg_struct _tmp2FF;_tmp2FF.tag=1U,_tmp2FF.f1=(unsigned long)i;_tmp2FF;});void*_tmp141[1U];_tmp141[0]=& _tmp143;({struct _fat_ptr _tmp331=({const char*_tmp142="%d";_tag_fat(_tmp142,sizeof(char),3U);});Cyc_aprintf(_tmp331,_tag_fat(_tmp141,sizeof(void*),1U));});}));goto _LL0;}default: _LLB: _tmp133=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Int_c).val).f2;_LLC: {int i=_tmp133;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp146=({struct Cyc_Int_pa_PrintArg_struct _tmp300;_tmp300.tag=1U,_tmp300.f1=(unsigned)i;_tmp300;});void*_tmp144[1U];_tmp144[0]=& _tmp146;({struct _fat_ptr _tmp332=({const char*_tmp145="%uU";_tag_fat(_tmp145,sizeof(char),4U);});Cyc_aprintf(_tmp332,_tag_fat(_tmp144,sizeof(void*),1U));});}));goto _LL0;}}case 6U: _LLD: _tmp131=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).LongLong_c).val).f1;_tmp132=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).LongLong_c).val).f2;_LLE: {enum Cyc_Absyn_Sign sg=_tmp131;long long i=_tmp132;
# 596
Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)i));goto _LL0;}case 7U: _LLF: _tmp130=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Float_c).val).f1;_LL10: {struct _fat_ptr x=_tmp130;
Cyc_Absyndump_dump(x);goto _LL0;}case 1U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp147="NULL";_tag_fat(_tmp147,sizeof(char),5U);}));goto _LL0;case 8U: _LL13: _tmp12F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).String_c).val;_LL14: {struct _fat_ptr s=_tmp12F;
# 600
Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(s));Cyc_Absyndump_dump_char((int)'"');
goto _LL0;}default: _LL15: _tmp12E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).Wstring_c).val;_LL16: {struct _fat_ptr s=_tmp12E;
# 603
Cyc_Absyndump_dump(({const char*_tmp148="L\"";_tag_fat(_tmp148,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(s);Cyc_Absyndump_dump_char((int)'"');goto _LL0;}}case 1U: _LL17: _tmp12D=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL18: {void*b=_tmp12D;
# 605
Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2U: _LL19: _tmp12C=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL1A: {struct _fat_ptr p=_tmp12C;
# 608
Cyc_Absyndump_dump(({const char*_tmp149="__cyclone_pragma__(";_tag_fat(_tmp149,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(p);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 3U: _LL1B: _tmp12A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp12B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL1C: {enum Cyc_Absyn_Primop p=_tmp12A;struct Cyc_List_List*es=_tmp12B;
# 611
struct _fat_ptr _tmp14A=Cyc_Absynpp_prim2str(p);struct _fat_ptr pstr=_tmp14A;
{int _tmp14B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpD=_tmp14B;int _tmp14C=_stmttmpD;switch(_tmp14C){case 1U: _LL72: _LL73:
# 614
 if((int)p == (int)18U){
Cyc_Absyndump_dump(({const char*_tmp14D="numelts(";_tag_fat(_tmp14D,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(({const char*_tmp14E=")";_tag_fat(_tmp14E,sizeof(char),2U);}));}else{
# 619
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);}
# 622
goto _LL71;case 2U: _LL74: _LL75:
# 624
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);
goto _LL71;default: _LL76: _LL77:
({void*_tmp14F=0U;({struct _fat_ptr _tmp333=({const char*_tmp150="Absyndump -- Bad number of arguments to primop";_tag_fat(_tmp150,sizeof(char),47U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp333,_tag_fat(_tmp14F,sizeof(void*),0U));});});}_LL71:;}
# 631
goto _LL0;}case 4U: _LL1D: _tmp127=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp128=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_tmp129=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDF)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp127;struct Cyc_Core_Opt*popt=_tmp128;struct Cyc_Absyn_Exp*e2=_tmp129;
# 634
Cyc_Absyndump_dumpexp_prec(myprec,e1);
if(popt != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)popt->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp151="=";_tag_fat(_tmp151,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,e2);
goto _LL0;}case 5U: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDF)->f2){case Cyc_Absyn_PreInc: _LL1F: _tmp126=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp126;
# 641
Cyc_Absyndump_dump(({const char*_tmp152="++";_tag_fat(_tmp152,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PreDec: _LL21: _tmp125=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL22: {struct Cyc_Absyn_Exp*e2=_tmp125;
Cyc_Absyndump_dump(({const char*_tmp153="--";_tag_fat(_tmp153,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PostInc: _LL23: _tmp124=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL24: {struct Cyc_Absyn_Exp*e2=_tmp124;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmp154="++";_tag_fat(_tmp154,sizeof(char),3U);}));goto _LL0;}default: _LL25: _tmp123=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL26: {struct Cyc_Absyn_Exp*e2=_tmp123;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(({const char*_tmp155="--";_tag_fat(_tmp155,sizeof(char),3U);}));goto _LL0;}}case 6U: _LL27: _tmp120=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp121=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_tmp122=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDF)->f3;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp120;struct Cyc_Absyn_Exp*e2=_tmp121;struct Cyc_Absyn_Exp*e3=_tmp122;
# 647
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,e2);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,e3);
goto _LL0;}case 7U: _LL29: _tmp11E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp11F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp11E;struct Cyc_Absyn_Exp*e2=_tmp11F;
# 654
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmp156="&&";_tag_fat(_tmp156,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 8U: _LL2B: _tmp11C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp11C;struct Cyc_Absyn_Exp*e2=_tmp11D;
# 658
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(({const char*_tmp157="||";_tag_fat(_tmp157,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 9U: _LL2D: _tmp11A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp11B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
# 664
Cyc_Absyndump_dumploc(e->loc);
# 667
Cyc_Absyndump_dumpexp_prec(myprec - 1,e1);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec - 1,e2);
goto _LL0;}case 10U: _LL2F: _tmp118=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp119=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp118;struct Cyc_List_List*es=_tmp119;
# 671
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_nospace(({const char*_tmp158="(";_tag_fat(_tmp158,sizeof(char),2U);}));{
# 676
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives && !(e->loc == (unsigned)0))&& !(e1->loc == (unsigned)0);
Cyc_Absyndump_dumpexps_prec(20,es);
Cyc_Absyndump_dump_nospace(({const char*_tmp159=")";_tag_fat(_tmp159,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;}}case 11U: _LL31: _tmp117=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp117;
# 685
Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(({const char*_tmp15A="throw";_tag_fat(_tmp15A,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 12U: _LL33: _tmp116=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp116;
# 687
_tmp115=e1;goto _LL36;}case 13U: _LL35: _tmp115=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp115;
Cyc_Absyndump_dumpexp_prec(inprec,e1);goto _LL0;}case 14U: _LL37: _tmp113=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp114=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL38: {void*t=_tmp113;struct Cyc_Absyn_Exp*e1=_tmp114;
# 691
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,e1);
goto _LL0;}case 15U: _LL39: _tmp112=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp112;
# 694
Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 20U: _LL3B: _tmp111=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmp111;
Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 16U: _LL3D: _tmp10F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp110=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL3E: {struct Cyc_Absyn_Exp*ropt=_tmp10F;struct Cyc_Absyn_Exp*e1=_tmp110;
Cyc_Absyndump_dump(({const char*_tmp15B="new";_tag_fat(_tmp15B,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 17U: _LL3F: _tmp10E=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL40: {void*t=_tmp10E;
Cyc_Absyndump_dump(({const char*_tmp15C="sizeof(";_tag_fat(_tmp15C,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 18U: _LL41: _tmp10D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL42: {struct Cyc_Absyn_Exp*e1=_tmp10D;
Cyc_Absyndump_dump(({const char*_tmp15D="sizeof(";_tag_fat(_tmp15D,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 39U: _LL43: _tmp10C=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL44: {void*t=_tmp10C;
Cyc_Absyndump_dump(({const char*_tmp15E="valueof(";_tag_fat(_tmp15E,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 41U: _LL45: _tmp10B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL46: {struct Cyc_Absyn_Exp*e1=_tmp10B;
# 702
Cyc_Absyndump_dump(({const char*_tmp15F="__extension__(";_tag_fat(_tmp15F,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 42U: _LL47: _tmp10A=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL48: {struct Cyc_Absyn_Exp*e1=_tmp10A;
# 704
Cyc_Absyndump_dump(({const char*_tmp160="@assert(";_tag_fat(_tmp160,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 40U: _LL49: _tmp105=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp106=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_tmp107=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDF)->f3;_tmp108=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDF)->f4;_tmp109=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDF)->f5;_LL4A: {int vol=_tmp105;struct _fat_ptr t=_tmp106;struct Cyc_List_List*o=_tmp107;struct Cyc_List_List*i=_tmp108;struct Cyc_List_List*cl=_tmp109;
# 707
Cyc_Absyndump_dump(({const char*_tmp161="__asm__";_tag_fat(_tmp161,sizeof(char),8U);}));
if(vol)Cyc_Absyndump_dump(({const char*_tmp162=" volatile ";_tag_fat(_tmp162,sizeof(char),11U);}));
Cyc_Absyndump_dump_nospace(({const char*_tmp163="(\"";_tag_fat(_tmp163,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(t));Cyc_Absyndump_dump_char((int)'"');
if((unsigned)o){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_asm_iolist(o);}
# 714
if((unsigned)i){
if(!((unsigned)o))
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_asm_iolist(i);}
# 720
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
{int cols=0;for(0;cols < 3 - ncol;++ cols){
Cyc_Absyndump_dump_nospace(({const char*_tmp164=" :";_tag_fat(_tmp164,sizeof(char),3U);}));}}
while((unsigned)cl){
Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(*((struct _fat_ptr*)cl->hd)));
Cyc_Absyndump_dump_char((int)'"');
cl=cl->tl;
if((unsigned)cl)
Cyc_Absyndump_dump_char((int)',');}}
# 733
Cyc_Absyndump_dump_char((int)')');
goto _LL0;}case 38U: _LL4B: _tmp103=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp104=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL4C: {struct Cyc_Absyn_Exp*e=_tmp103;struct _fat_ptr*f=_tmp104;
# 737
Cyc_Absyndump_dump(({const char*_tmp165="tagcheck(";_tag_fat(_tmp165,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*f);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;}case 19U: _LL4D: _tmp101=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp102=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL4E: {void*t=_tmp101;struct Cyc_List_List*l=_tmp102;
# 742
Cyc_Absyndump_dump(({const char*_tmp166="offsetof(";_tag_fat(_tmp166,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(t);({struct Cyc_List_List*_tmp336=l;struct _fat_ptr _tmp335=({const char*_tmp167=",";_tag_fat(_tmp167,sizeof(char),2U);});struct _fat_ptr _tmp334=({const char*_tmp168=")";_tag_fat(_tmp168,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpoffset_field,_tmp336,_tmp335,_tmp334,({const char*_tmp169=".";_tag_fat(_tmp169,sizeof(char),2U);}));});goto _LL0;}case 21U: _LL4F: _tmpFF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmp100=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL50: {struct Cyc_Absyn_Exp*e1=_tmpFF;struct _fat_ptr*n=_tmp100;
# 745
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 22U: _LL51: _tmpFD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpFE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpFD;struct _fat_ptr*n=_tmpFE;
# 747
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp16A="->";_tag_fat(_tmp16A,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 23U: _LL53: _tmpFB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpFC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL54: {struct Cyc_Absyn_Exp*e1=_tmpFB;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 750
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char((int)']');goto _LL0;}case 24U: _LL55: _tmpFA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL56: {struct Cyc_List_List*es=_tmpFA;
# 752
Cyc_Absyndump_dump(({const char*_tmp16B="$(";_tag_fat(_tmp16B,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_char((int)')');goto _LL0;}case 25U: _LL57: _tmpF8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpF9=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL58: {struct _tuple8*vat=_tmpF8;struct Cyc_List_List*des=_tmpF9;
# 755
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*vat).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp339=des;struct _fat_ptr _tmp338=({const char*_tmp16C="{";_tag_fat(_tmp16C,sizeof(char),2U);});struct _fat_ptr _tmp337=({const char*_tmp16D="}";_tag_fat(_tmp16D,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp339,_tmp338,_tmp337,({const char*_tmp16E=",";_tag_fat(_tmp16E,sizeof(char),2U);}));});
goto _LL0;}case 26U: _LL59: _tmpF7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL5A: {struct Cyc_List_List*des=_tmpF7;
# 761
({struct Cyc_List_List*_tmp33C=des;struct _fat_ptr _tmp33B=({const char*_tmp16F="{";_tag_fat(_tmp16F,sizeof(char),2U);});struct _fat_ptr _tmp33A=({const char*_tmp170="}";_tag_fat(_tmp170,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp33C,_tmp33B,_tmp33A,({const char*_tmp171=",";_tag_fat(_tmp171,sizeof(char),2U);}));});goto _LL0;}case 27U: _LL5B: _tmpF4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpF5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_tmpF6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDF)->f3;_LL5C: {struct Cyc_Absyn_Vardecl*vd=_tmpF4;struct Cyc_Absyn_Exp*e1=_tmpF5;struct Cyc_Absyn_Exp*e2=_tmpF6;
# 764
Cyc_Absyndump_dump(({const char*_tmp172="{for";_tag_fat(_tmp172,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*vd->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(e1);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 28U: _LL5D: _tmpF2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpF3=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL5E: {struct Cyc_Absyn_Exp*e=_tmpF2;void*t=_tmpF3;
# 769
Cyc_Absyndump_dump(({const char*_tmp173="{for x ";_tag_fat(_tmp173,sizeof(char),8U);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(e);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 29U: _LL5F: _tmpEF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpF0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_tmpF1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDF)->f3;_LL60: {struct _tuple0*n=_tmpEF;struct Cyc_List_List*ts=_tmpF0;struct Cyc_List_List*des=_tmpF1;
# 774
Cyc_Absyndump_dumpqvar(n);
Cyc_Absyndump_dump_char((int)'{');
if(ts != 0)
Cyc_Absyndump_dumptps(ts);
({struct Cyc_List_List*_tmp33F=des;struct _fat_ptr _tmp33E=({const char*_tmp174="";_tag_fat(_tmp174,sizeof(char),1U);});struct _fat_ptr _tmp33D=({const char*_tmp175="}";_tag_fat(_tmp175,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp33F,_tmp33E,_tmp33D,({const char*_tmp176=",";_tag_fat(_tmp176,sizeof(char),2U);}));});
goto _LL0;}case 30U: _LL61: _tmpEE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL62: {struct Cyc_List_List*des=_tmpEE;
# 781
({struct Cyc_List_List*_tmp342=des;struct _fat_ptr _tmp341=({const char*_tmp177="{";_tag_fat(_tmp177,sizeof(char),2U);});struct _fat_ptr _tmp340=({const char*_tmp178="}";_tag_fat(_tmp178,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp342,_tmp341,_tmp340,({const char*_tmp179=",";_tag_fat(_tmp179,sizeof(char),2U);}));});goto _LL0;}case 32U: _LL63: _tmpED=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL64: {struct Cyc_Absyn_Enumfield*ef=_tmpED;
_tmpEC=ef;goto _LL66;}case 33U: _LL65: _tmpEC=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL66: {struct Cyc_Absyn_Enumfield*ef=_tmpEC;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 31U: _LL67: _tmpEA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpEB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDF)->f3;_LL68: {struct Cyc_List_List*es=_tmpEA;struct Cyc_Absyn_Datatypefield*tuf=_tmpEB;
# 786
Cyc_Absyndump_dumpqvar(tuf->name);
if(es != 0)({struct Cyc_List_List*_tmp345=es;struct _fat_ptr _tmp344=({const char*_tmp17A="(";_tag_fat(_tmp17A,sizeof(char),2U);});struct _fat_ptr _tmp343=({const char*_tmp17B=")";_tag_fat(_tmp17B,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp345,_tmp344,_tmp343,({const char*_tmp17C=",";_tag_fat(_tmp17C,sizeof(char),2U);}));});
goto _LL0;}case 34U: _LL69: _tmpE5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).is_calloc;_tmpE6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).rgn;_tmpE7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).elt_type;_tmpE8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).num_elts;_tmpE9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDF)->f1).inline_call;_LL6A: {int is_calloc=_tmpE5;struct Cyc_Absyn_Exp*ropt=_tmpE6;void**topt=_tmpE7;struct Cyc_Absyn_Exp*e=_tmpE8;int inline_call=_tmpE9;
# 791
Cyc_Absyndump_dumploc(e->loc);
if(is_calloc){
if(ropt != 0){
Cyc_Absyndump_dump(({const char*_tmp17D="rcalloc(";_tag_fat(_tmp17D,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp17E=",";_tag_fat(_tmp17E,sizeof(char),2U);}));}else{
# 796
Cyc_Absyndump_dump(({const char*_tmp17F="calloc(";_tag_fat(_tmp17F,sizeof(char),8U);}));}
# 798
Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(({const char*_tmp180=",";_tag_fat(_tmp180,sizeof(char),2U);}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(topt)),0U));Cyc_Absyndump_dump(({const char*_tmp181=")";_tag_fat(_tmp181,sizeof(char),2U);}));}else{
# 800
if(ropt != 0){
Cyc_Absyndump_dump(inline_call?({const char*_tmp182="rmalloc_inline(";_tag_fat(_tmp182,sizeof(char),16U);}):({const char*_tmp183="rmalloc(";_tag_fat(_tmp183,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(({const char*_tmp184=",";_tag_fat(_tmp184,sizeof(char),2U);}));}else{
# 804
Cyc_Absyndump_dump(({const char*_tmp185="malloc(";_tag_fat(_tmp185,sizeof(char),8U);}));}
# 807
if(topt != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp346=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_tmp346,e,0U);}));else{
# 810
Cyc_Absyndump_dumpexp(e);}
Cyc_Absyndump_dump(({const char*_tmp186=")";_tag_fat(_tmp186,sizeof(char),2U);}));}
# 813
goto _LL0;}case 35U: _LL6B: _tmpE3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpE4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL6C: {struct Cyc_Absyn_Exp*e1=_tmpE3;struct Cyc_Absyn_Exp*e2=_tmpE4;
# 816
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(({const char*_tmp187=":=:";_tag_fat(_tmp187,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 36U: _LL6D: _tmpE1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_tmpE2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDF)->f2;_LL6E: {struct Cyc_Core_Opt*n=_tmpE1;struct Cyc_List_List*des=_tmpE2;
# 819
({struct Cyc_List_List*_tmp349=des;struct _fat_ptr _tmp348=({const char*_tmp188="{";_tag_fat(_tmp188,sizeof(char),2U);});struct _fat_ptr _tmp347=({const char*_tmp189="}";_tag_fat(_tmp189,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple19*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp349,_tmp348,_tmp347,({const char*_tmp18A=",";_tag_fat(_tmp18A,sizeof(char),2U);}));});goto _LL0;}default: _LL6F: _tmpE0=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL70: {struct Cyc_Absyn_Stmt*s=_tmpE0;
# 822
Cyc_Absyndump_dump_nospace(({const char*_tmp18B="({";_tag_fat(_tmp18B,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s,1,0);Cyc_Absyndump_dump_nospace(({const char*_tmp18C="})";_tag_fat(_tmp18C,sizeof(char),3U);}));goto _LL0;}}_LL0:;}
# 824
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 828
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 832
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs,struct Cyc_List_List*varsinblock){
# 834
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp18D=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*c=_tmp18D;
if(c->where_clause == 0 &&(c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp18E="default:";_tag_fat(_tmp18E,sizeof(char),9U);}));else{
# 839
Cyc_Absyndump_dump(({const char*_tmp18F="case";_tag_fat(_tmp18F,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(c->pattern);
if(c->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp190="&&";_tag_fat(_tmp190,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));}
# 845
Cyc_Absyndump_dump_nospace(({const char*_tmp191=":";_tag_fat(_tmp191,sizeof(char),2U);}));}
# 847
if(Cyc_Absynpp_is_declaration(c->body)){
Cyc_Absyndump_dump(({const char*_tmp192=" {";_tag_fat(_tmp192,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(c->body,0,0),Cyc_Absyndump_dump_char((int)'}');}else{
# 850
Cyc_Absyndump_dumpstmt(c->body,0,varsinblock);}}}
# 854
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt,struct Cyc_List_List*varsinblock){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp193=s->r;void*_stmttmpE=_tmp193;void*_tmp194=_stmttmpE;struct Cyc_List_List*_tmp195;struct Cyc_Absyn_Stmt*_tmp197;struct _fat_ptr*_tmp196;struct Cyc_Absyn_Stmt*_tmp199;struct Cyc_Absyn_Decl*_tmp198;struct Cyc_List_List*_tmp19B;struct Cyc_Absyn_Stmt*_tmp19A;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Exp*_tmp19C;struct _fat_ptr*_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Stmt*_tmp19F;struct Cyc_Absyn_Stmt*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Stmt*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Stmt*_tmp1A9;struct Cyc_Absyn_Stmt*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Stmt*_tmp1AC;struct Cyc_Absyn_Stmt*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;switch(*((int*)_tmp194)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1U: _LL3: _tmp1AD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp1AD;
Cyc_Absyndump_dumpexp_prec(- 100,e);Cyc_Absyndump_dump_semi();goto _LL0;}case 2U: _LL5: _tmp1AB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp1AC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp194)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp1AB;struct Cyc_Absyn_Stmt*s2=_tmp1AC;
# 863
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 866
Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}
if(Cyc_Absynpp_is_declaration(s2)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(s2,expstmt,0);Cyc_Absyndump_dump_char((int)'}');
if(expstmt){Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}}else{
# 872
Cyc_Absyndump_dumpstmt(s2,expstmt,varsinblock);}
goto _LL0;}case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp194)->f1 == 0){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1AE="return;";_tag_fat(_tmp1AE,sizeof(char),8U);}));goto _LL0;}else{_LL9: _tmp1AA=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp1AA;
Cyc_Absyndump_dump(({const char*_tmp1AF="return";_tag_fat(_tmp1AF,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(e));Cyc_Absyndump_dump_semi();goto _LL0;}}case 4U: _LLB: _tmp1A7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp1A8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp194)->f2;_tmp1A9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp194)->f3;_LLC: {struct Cyc_Absyn_Exp*e=_tmp1A7;struct Cyc_Absyn_Stmt*s1=_tmp1A8;struct Cyc_Absyn_Stmt*s2=_tmp1A9;
# 877
Cyc_Absyndump_dump(({const char*_tmp1B0="if(";_tag_fat(_tmp1B0,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(e);
{void*_tmp1B1=s1->r;void*_stmttmpF=_tmp1B1;void*_tmp1B2=_stmttmpF;switch(*((int*)_tmp1B2)){case 2U: _LL26: _LL27:
 goto _LL29;case 12U: _LL28: _LL29:
 goto _LL2B;case 4U: _LL2A: _LL2B:
 goto _LL2D;case 13U: _LL2C: _LL2D:
# 883
 Cyc_Absyndump_dump_nospace(({const char*_tmp1B3="){";_tag_fat(_tmp1B3,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');goto _LL25;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}_LL25:;}
# 886
{void*_tmp1B4=s2->r;void*_stmttmp10=_tmp1B4;void*_tmp1B5=_stmttmp10;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1B5)->tag == 0U){_LL31: _LL32:
 goto _LL30;}else{_LL33: _LL34:
 Cyc_Absyndump_dump(({const char*_tmp1B6="else{";_tag_fat(_tmp1B6,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(s2,0,0);Cyc_Absyndump_dump_char((int)'}');goto _LL30;}_LL30:;}
# 890
goto _LL0;}case 5U: _LLD: _tmp1A5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp194)->f1).f1;_tmp1A6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp194)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp1A5;struct Cyc_Absyn_Stmt*s1=_tmp1A6;
# 893
Cyc_Absyndump_dump(({const char*_tmp1B7="while(";_tag_fat(_tmp1B7,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1B8="){";_tag_fat(_tmp1B8,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 9U: _LLF: _tmp1A1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp1A2=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp194)->f2).f1;_tmp1A3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp194)->f3).f1;_tmp1A4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp194)->f4;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp1A1;struct Cyc_Absyn_Exp*e2=_tmp1A2;struct Cyc_Absyn_Exp*e3=_tmp1A3;struct Cyc_Absyn_Stmt*s1=_tmp1A4;
# 897
Cyc_Absyndump_dump(({const char*_tmp1B9="for(";_tag_fat(_tmp1B9,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e3);
Cyc_Absyndump_dump_nospace(({const char*_tmp1BA="){";_tag_fat(_tmp1BA,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 14U: _LL11: _tmp19F=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp1A0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp194)->f2).f1;_LL12: {struct Cyc_Absyn_Stmt*s1=_tmp19F;struct Cyc_Absyn_Exp*e=_tmp1A0;
# 901
Cyc_Absyndump_dump(({const char*_tmp1BB="do{";_tag_fat(_tmp1BB,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1BC="}while(";_tag_fat(_tmp1BC,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1BD=");";_tag_fat(_tmp1BD,sizeof(char),3U);}));
goto _LL0;}case 6U: _LL13: _LL14:
# 905
 Cyc_Absyndump_dump(({const char*_tmp1BE="break;";_tag_fat(_tmp1BE,sizeof(char),7U);}));goto _LL0;case 7U: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp1BF="continue;";_tag_fat(_tmp1BF,sizeof(char),10U);}));goto _LL0;case 8U: _LL17: _tmp19E=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_LL18: {struct _fat_ptr*x=_tmp19E;
Cyc_Absyndump_dump(({const char*_tmp1C0="goto";_tag_fat(_tmp1C0,sizeof(char),5U);}));Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_semi();goto _LL0;}case 10U: _LL19: _tmp19C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp19D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp194)->f2;_LL1A: {struct Cyc_Absyn_Exp*e=_tmp19C;struct Cyc_List_List*ss=_tmp19D;
# 910
Cyc_Absyndump_dump(({const char*_tmp1C1="switch(";_tag_fat(_tmp1C1,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(({const char*_tmp1C2="){";_tag_fat(_tmp1C2,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 15U: _LL1B: _tmp19A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp19B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp194)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp19A;struct Cyc_List_List*ss=_tmp19B;
# 915
Cyc_Absyndump_dump(({const char*_tmp1C3="try";_tag_fat(_tmp1C3,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(s1,0,varsinblock);
Cyc_Absyndump_dump(({const char*_tmp1C4="catch{";_tag_fat(_tmp1C4,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}case 12U: _LL1D: _tmp198=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp199=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp194)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp198;struct Cyc_Absyn_Stmt*s1=_tmp199;
# 921
struct _tuple13 _tmp1C5=Cyc_Absynpp_shadows(d,varsinblock);struct _tuple13 _stmttmp11=_tmp1C5;struct _tuple13 _tmp1C6=_stmttmp11;struct Cyc_List_List*_tmp1C8;int _tmp1C7;_LL36: _tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;_LL37: {int newblock=_tmp1C7;struct Cyc_List_List*newvarsinblock=_tmp1C8;
if(newblock){
if(expstmt)Cyc_Absyndump_dump(({const char*_tmp1C9="({";_tag_fat(_tmp1C9,sizeof(char),3U);}));else{Cyc_Absyndump_dump(({const char*_tmp1CA="{";_tag_fat(_tmp1CA,sizeof(char),2U);}));}
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1CB="});";_tag_fat(_tmp1CB,sizeof(char),4U);}));else{Cyc_Absyndump_dump(({const char*_tmp1CC="}";_tag_fat(_tmp1CC,sizeof(char),2U);}));}}else{
# 928
Cyc_Absyndump_dumpdecl(d);Cyc_Absyndump_dumpstmt(s1,expstmt,newvarsinblock);}
# 930
goto _LL0;}}case 13U: _LL1F: _tmp196=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_tmp197=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp194)->f2;_LL20: {struct _fat_ptr*x=_tmp196;struct Cyc_Absyn_Stmt*s1=_tmp197;
# 936
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_str(x);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1CD=": ({";_tag_fat(_tmp1CD,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1CE=": {";_tag_fat(_tmp1CE,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1CF="});";_tag_fat(_tmp1CF,sizeof(char),4U);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 942
Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(s1,expstmt,varsinblock);}
# 944
goto _LL0;}default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp194)->f1 == 0){_LL21: _LL22:
# 946
 Cyc_Absyndump_dump(({const char*_tmp1D0="fallthru;";_tag_fat(_tmp1D0,sizeof(char),10U);}));goto _LL0;}else{_LL23: _tmp195=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp194)->f1;_LL24: {struct Cyc_List_List*es=_tmp195;
# 948
Cyc_Absyndump_dump(({const char*_tmp1D1="fallthru(";_tag_fat(_tmp1D1,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_nospace(({const char*_tmp1D2=");";_tag_fat(_tmp1D2,sizeof(char),3U);}));goto _LL0;}}}_LL0:;}}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 952
static void Cyc_Absyndump_dumpdp(struct _tuple21*dp){
({struct Cyc_List_List*_tmp34C=(*dp).f1;struct _fat_ptr _tmp34B=({const char*_tmp1D3="";_tag_fat(_tmp1D3,sizeof(char),1U);});struct _fat_ptr _tmp34A=({const char*_tmp1D4="=";_tag_fat(_tmp1D4,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp34C,_tmp34B,_tmp34A,({const char*_tmp1D5="=";_tag_fat(_tmp1D5,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 956
static struct _fat_ptr Cyc_Absyndump_pat_term(int dots){return dots?({const char*_tmp1D6="...)";_tag_fat(_tmp1D6,sizeof(char),5U);}):({const char*_tmp1D7=")";_tag_fat(_tmp1D7,sizeof(char),2U);});}
# 958
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1D8=p->r;void*_stmttmp12=_tmp1D8;void*_tmp1D9=_stmttmp12;struct Cyc_Absyn_Exp*_tmp1DA;int _tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Datatypefield*_tmp1DB;int _tmp1E0;struct Cyc_List_List*_tmp1DF;struct Cyc_List_List*_tmp1DE;int _tmp1E4;struct Cyc_List_List*_tmp1E3;struct Cyc_List_List*_tmp1E2;union Cyc_Absyn_AggrInfo _tmp1E1;int _tmp1E7;struct Cyc_List_List*_tmp1E6;struct _tuple0*_tmp1E5;struct _tuple0*_tmp1E8;struct Cyc_Absyn_Vardecl*_tmp1EA;struct Cyc_Absyn_Tvar*_tmp1E9;struct Cyc_Absyn_Pat*_tmp1EB;int _tmp1ED;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1EF;struct Cyc_Absyn_Tvar*_tmp1EE;struct Cyc_Absyn_Pat*_tmp1F1;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_Absyn_Vardecl*_tmp1F2;struct Cyc_Absyn_Pat*_tmp1F4;struct Cyc_Absyn_Vardecl*_tmp1F3;struct Cyc_Absyn_Vardecl*_tmp1F5;char _tmp1F6;struct Cyc_Absyn_Enumfield*_tmp1F7;struct Cyc_Absyn_Enumfield*_tmp1F8;struct _fat_ptr _tmp1F9;int _tmp1FA;int _tmp1FB;int _tmp1FC;switch(*((int*)_tmp1D9)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_char((int)'_');goto _LL0;case 9U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp1FD="NULL";_tag_fat(_tmp1FD,sizeof(char),5U);}));goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1){case Cyc_Absyn_None: _LL5: _tmp1FC=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL6: {int i=_tmp1FC;
_tmp1FB=i;goto _LL8;}case Cyc_Absyn_Signed: _LL7: _tmp1FB=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL8: {int i=_tmp1FB;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp200=({struct Cyc_Int_pa_PrintArg_struct _tmp301;_tmp301.tag=1U,_tmp301.f1=(unsigned long)i;_tmp301;});void*_tmp1FE[1U];_tmp1FE[0]=& _tmp200;({struct _fat_ptr _tmp34D=({const char*_tmp1FF="%d";_tag_fat(_tmp1FF,sizeof(char),3U);});Cyc_aprintf(_tmp34D,_tag_fat(_tmp1FE,sizeof(void*),1U));});}));goto _LL0;}default: _LL9: _tmp1FA=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LLA: {int i=_tmp1FA;
Cyc_Absyndump_dump((struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp203=({struct Cyc_Int_pa_PrintArg_struct _tmp302;_tmp302.tag=1U,_tmp302.f1=(unsigned)i;_tmp302;});void*_tmp201[1U];_tmp201[0]=& _tmp203;({struct _fat_ptr _tmp34E=({const char*_tmp202="%u";_tag_fat(_tmp202,sizeof(char),3U);});Cyc_aprintf(_tmp34E,_tag_fat(_tmp201,sizeof(void*),1U));});}));goto _LL0;}}case 12U: _LLB: _tmp1F9=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_LLC: {struct _fat_ptr x=_tmp1F9;
Cyc_Absyndump_dump(x);goto _LL0;}case 13U: _LLD: _tmp1F8=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LLE: {struct Cyc_Absyn_Enumfield*ef=_tmp1F8;
_tmp1F7=ef;goto _LL10;}case 14U: _LLF: _tmp1F7=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL10: {struct Cyc_Absyn_Enumfield*ef=_tmp1F7;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 11U: _LL11: _tmp1F6=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_LL12: {char ch=_tmp1F6;
# 969
Cyc_Absyndump_dump(({const char*_tmp204="'";_tag_fat(_tmp204,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_nospace(({const char*_tmp205="'";_tag_fat(_tmp205,sizeof(char),2U);}));goto _LL0;}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2)->r)->tag == 0U){_LL13: _tmp1F5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_LL14: {struct Cyc_Absyn_Vardecl*vd=_tmp1F5;
Cyc_Absyndump_dump(({const char*_tmp206="*";_tag_fat(_tmp206,sizeof(char),2U);}));_tmp1F2=vd;goto _LL16;}}else{_LL17: _tmp1F3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_tmp1F4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL18: {struct Cyc_Absyn_Vardecl*vd=_tmp1F3;struct Cyc_Absyn_Pat*p2=_tmp1F4;
# 972
Cyc_Absyndump_dump(({const char*_tmp207="*";_tag_fat(_tmp207,sizeof(char),2U);}));_tmp1F0=vd;_tmp1F1=p2;goto _LL1A;}}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2)->r)->tag == 0U){_LL15: _tmp1F2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_LL16: {struct Cyc_Absyn_Vardecl*vd=_tmp1F2;
# 971
Cyc_Absyndump_dumpqvar(vd->name);goto _LL0;}}else{_LL19: _tmp1F0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_tmp1F1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_tmp1F0;struct Cyc_Absyn_Pat*p2=_tmp1F1;
# 973
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump(({const char*_tmp208=" as ";_tag_fat(_tmp208,sizeof(char),5U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}}case 2U: _LL1B: _tmp1EE=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_tmp1EF=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL1C: {struct Cyc_Absyn_Tvar*tv=_tmp1EE;struct Cyc_Absyn_Vardecl*vd=_tmp1EF;
# 975
Cyc_Absyndump_dump(({const char*_tmp209="alias";_tag_fat(_tmp209,sizeof(char),6U);}));Cyc_Absyndump_dump(({const char*_tmp20A="<";_tag_fat(_tmp20A,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp20B=">";_tag_fat(_tmp20B,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(vd,p->loc,0);
goto _LL0;}case 5U: _LL1D: _tmp1EC=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_tmp1ED=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL1E: {struct Cyc_List_List*ts=_tmp1EC;int dots=_tmp1ED;
({struct Cyc_List_List*_tmp351=ts;struct _fat_ptr _tmp350=({const char*_tmp20C="$(";_tag_fat(_tmp20C,sizeof(char),3U);});struct _fat_ptr _tmp34F=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp351,_tmp350,_tmp34F,({const char*_tmp20D=",";_tag_fat(_tmp20D,sizeof(char),2U);}));});goto _LL0;}case 6U: _LL1F: _tmp1EB=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_LL20: {struct Cyc_Absyn_Pat*p2=_tmp1EB;
Cyc_Absyndump_dump(({const char*_tmp20E="&";_tag_fat(_tmp20E,sizeof(char),2U);}));Cyc_Absyndump_dumppat(p2);goto _LL0;}case 4U: _LL21: _tmp1E9=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_tmp1EA=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_LL22: {struct Cyc_Absyn_Tvar*tv=_tmp1E9;struct Cyc_Absyn_Vardecl*vd=_tmp1EA;
# 981
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump_char((int)'>');goto _LL0;}case 15U: _LL23: _tmp1E8=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_LL24: {struct _tuple0*q=_tmp1E8;
Cyc_Absyndump_dumpqvar(q);goto _LL0;}case 16U: _LL25: _tmp1E5=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_tmp1E6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_tmp1E7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D9)->f3;_LL26: {struct _tuple0*q=_tmp1E5;struct Cyc_List_List*ps=_tmp1E6;int dots=_tmp1E7;
# 984
Cyc_Absyndump_dumpqvar(q);({struct Cyc_List_List*_tmp354=ps;struct _fat_ptr _tmp353=({const char*_tmp20F="(";_tag_fat(_tmp20F,sizeof(char),2U);});struct _fat_ptr _tmp352=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp354,_tmp353,_tmp352,({const char*_tmp210=",";_tag_fat(_tmp210,sizeof(char),2U);}));});goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1 != 0){_LL27: _tmp1E1=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_tmp1E2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_tmp1E3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f3;_tmp1E4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f4;_LL28: {union Cyc_Absyn_AggrInfo info=_tmp1E1;struct Cyc_List_List*exists=_tmp1E2;struct Cyc_List_List*dps=_tmp1E3;int dots=_tmp1E4;
# 986
Cyc_Absyndump_dumpqvar((Cyc_Absyn_aggr_kinded_name(info)).f2);
_tmp1DE=exists;_tmp1DF=dps;_tmp1E0=dots;goto _LL2A;}}else{_LL29: _tmp1DE=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_tmp1DF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f3;_tmp1E0=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D9)->f4;_LL2A: {struct Cyc_List_List*exists=_tmp1DE;struct Cyc_List_List*dps=_tmp1DF;int dots=_tmp1E0;
# 989
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp357=exists;struct _fat_ptr _tmp356=({const char*_tmp211="<";_tag_fat(_tmp211,sizeof(char),2U);});struct _fat_ptr _tmp355=({const char*_tmp212=">";_tag_fat(_tmp212,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp357,_tmp356,_tmp355,({const char*_tmp213=",";_tag_fat(_tmp213,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp35A=dps;struct _fat_ptr _tmp359=({const char*_tmp214="";_tag_fat(_tmp214,sizeof(char),1U);});struct _fat_ptr _tmp358=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct _tuple21*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp35A,_tmp359,_tmp358,({const char*_tmp215=",";_tag_fat(_tmp215,sizeof(char),2U);}));});
goto _LL0;}}case 8U: _LL2B: _tmp1DB=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D9)->f2;_tmp1DC=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D9)->f3;_tmp1DD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D9)->f4;_LL2C: {struct Cyc_Absyn_Datatypefield*tuf=_tmp1DB;struct Cyc_List_List*ps=_tmp1DC;int dots=_tmp1DD;
# 994
Cyc_Absyndump_dumpqvar(tuf->name);
if(ps != 0)({struct Cyc_List_List*_tmp35D=ps;struct _fat_ptr _tmp35C=({const char*_tmp216="(";_tag_fat(_tmp216,sizeof(char),2U);});struct _fat_ptr _tmp35B=Cyc_Absyndump_pat_term(dots);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp35D,_tmp35C,_tmp35B,({const char*_tmp217=",";_tag_fat(_tmp217,sizeof(char),2U);}));});
goto _LL0;}default: _LL2D: _tmp1DA=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D9)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp1DA;
Cyc_Absyndump_dumpexp(e);goto _LL0;}}_LL0:;}
# 1001
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1006
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp35E=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp35E,({const char*_tmp218=",";_tag_fat(_tmp218,sizeof(char),2U);}));});}
# 1009
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp219="=";_tag_fat(_tmp219,sizeof(char),2U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1015
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp35F=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _fat_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp35F,({const char*_tmp21A=",";_tag_fat(_tmp21A,sizeof(char),2U);}));});}
# 1019
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp21B=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_stmttmp13=_tmp21B;struct Cyc_Absyn_Aggrfield*_tmp21C=_stmttmp13;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_List_List*_tmp221;struct Cyc_Absyn_Exp*_tmp220;void*_tmp21F;struct Cyc_Absyn_Tqual _tmp21E;struct _fat_ptr*_tmp21D;_LL1: _tmp21D=_tmp21C->name;_tmp21E=_tmp21C->tq;_tmp21F=_tmp21C->type;_tmp220=_tmp21C->width;_tmp221=_tmp21C->attributes;_tmp222=_tmp21C->requires_clause;_LL2: {struct _fat_ptr*name=_tmp21D;struct Cyc_Absyn_Tqual tq=_tmp21E;void*type=_tmp21F;struct Cyc_Absyn_Exp*width=_tmp220;struct Cyc_List_List*atts=_tmp221;struct Cyc_Absyn_Exp*req=_tmp222;
{enum Cyc_Flags_C_Compilers _tmp223=Cyc_Flags_c_compiler;if(_tmp223 == Cyc_Flags_Gcc_c){_LL4: _LL5:
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dump_str,name);Cyc_Absyndump_dumpatts(atts);goto _LL3;}else{_LL6: _LL7:
 Cyc_Absyndump_dumpatts(atts);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dump_str,name);goto _LL3;}_LL3:;}
# 1026
if((unsigned)req){
Cyc_Absyndump_dump(({const char*_tmp224="@requires ";_tag_fat(_tmp224,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(req);}
# 1029
if(width != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(width);}
# 1032
Cyc_Absyndump_dump_semi();}}}
# 1036
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1041
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple22{void*f1;struct _tuple0*f2;};
# 1045
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple22*pr){
{void*_tmp225=(*pr).f1;void*_stmttmp14=_tmp225;void*_tmp226=_stmttmp14;switch(*((int*)_tmp226)){case 11U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp227="_stdcall";_tag_fat(_tmp227,sizeof(char),9U);}));goto _LL0;case 2U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp228="_cdecl";_tag_fat(_tmp228,sizeof(char),7U);}));goto _LL0;case 3U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp229="_fastcall";_tag_fat(_tmp229,sizeof(char),10U);}));goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 1053
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1055
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1059
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 1063
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1065
void*_tmp22A=t;void*_tmp22B;void*_tmp22C;switch(*((int*)_tmp22A)){case 1U: _LL1: _tmp22C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22A)->f2;if(_tmp22C != 0){_LL2: {void*def=_tmp22C;
return Cyc_Absyndump_is_char_ptr(def);}}else{goto _LL5;}case 3U: _LL3: _tmp22B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22A)->f1).elt_type;_LL4: {void*elt_typ=_tmp22B;
# 1068
while(1){
void*_tmp22D=elt_typ;void*_tmp22E;void*_tmp22F;switch(*((int*)_tmp22D)){case 1U: _LL8: _tmp22F=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22D)->f2;if(_tmp22F != 0){_LL9: {void*t=_tmp22F;
elt_typ=t;goto _LL7;}}else{goto _LLE;}case 8U: _LLA: _tmp22E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp22D)->f4;if(_tmp22E != 0){_LLB: {void*t=_tmp22E;
elt_typ=t;goto _LL7;}}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}}default: _LL5: _LL6:
# 1075
 return 0;}_LL0:;}
# 1079
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp230=vd;int _tmp238;struct Cyc_List_List*_tmp237;struct Cyc_Absyn_Exp*_tmp236;void*_tmp235;struct Cyc_Absyn_Tqual _tmp234;unsigned _tmp233;struct _tuple0*_tmp232;enum Cyc_Absyn_Scope _tmp231;_LL1: _tmp231=_tmp230->sc;_tmp232=_tmp230->name;_tmp233=_tmp230->varloc;_tmp234=_tmp230->tq;_tmp235=_tmp230->type;_tmp236=_tmp230->initializer;_tmp237=_tmp230->attributes;_tmp238=_tmp230->is_proto;_LL2: {enum Cyc_Absyn_Scope sc=_tmp231;struct _tuple0*name=_tmp232;unsigned varloc=_tmp233;struct Cyc_Absyn_Tqual tq=_tmp234;void*type=_tmp235;struct Cyc_Absyn_Exp*initializer=_tmp236;struct Cyc_List_List*atts=_tmp237;int is_proto=_tmp238;
{enum Cyc_Flags_C_Compilers _tmp239=Cyc_Flags_c_compiler;if(_tmp239 == Cyc_Flags_Gcc_c){_LL4: _LL5:
# 1084
 if((int)sc == (int)3U && Cyc_Absyndump_qvar_to_Cids){
void*_tmp23A=Cyc_Absyn_compress(type);void*_stmttmp15=_tmp23A;void*_tmp23B=_stmttmp15;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23B)->tag == 5U){_LL9: _LLA:
# 1087
 if(!is_proto)
Cyc_Absyndump_dumpscope(sc);
goto _LL8;}else{_LLB: _LLC:
 Cyc_Absyndump_dumpscope(sc);}_LL8:;}else{
# 1093
Cyc_Absyndump_dumpscope(sc);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(tq,type,Cyc_Absyndump_dumpqvar,name);
Cyc_Absyndump_dumpatts(atts);
goto _LL3;}else{_LL6: _LL7:
# 1098
 Cyc_Absyndump_dumpatts(atts);
Cyc_Absyndump_dumpscope(sc);{
struct _RegionHandle _tmp23C=_new_region("temp");struct _RegionHandle*temp=& _tmp23C;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(type);
struct _tuple12 _tmp23D=Cyc_Absynpp_to_tms(temp,tq,type);struct _tuple12 _stmttmp16=_tmp23D;struct _tuple12 _tmp23E=_stmttmp16;struct Cyc_List_List*_tmp241;void*_tmp240;struct Cyc_Absyn_Tqual _tmp23F;_LLE: _tmp23F=_tmp23E.f1;_tmp240=_tmp23E.f2;_tmp241=_tmp23E.f3;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp23F;void*t=_tmp240;struct Cyc_List_List*tms=_tmp241;
# 1104
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=tms;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp242=(void*)tms2->hd;void*_stmttmp17=_tmp242;void*_tmp243=_stmttmp17;struct Cyc_List_List*_tmp244;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp243)->tag == 5U){_LL11: _tmp244=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp243)->f2;_LL12: {struct Cyc_List_List*atts=_tmp244;
# 1108
for(0;atts != 0;atts=atts->tl){
void*_tmp245=(void*)atts->hd;void*_stmttmp18=_tmp245;void*_tmp246=_stmttmp18;switch(*((int*)_tmp246)){case 1U: _LL16: _LL17:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL15;case 2U: _LL18: _LL19:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL15;case 3U: _LL1A: _LL1B:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL15;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 1115
goto _LL10;}}else{_LL13: _LL14:
 goto _LL10;}_LL10:;}}
# 1118
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);{
struct _tuple22 _tmp247=({struct _tuple22 _tmp303;_tmp303.f1=call_conv,_tmp303.f2=name;_tmp303;});struct _tuple22 pr=_tmp247;
({int _tmp360=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*,void(*f)(struct _tuple22*),struct _tuple22*a))Cyc_Absyndump_dumptms)(_tmp360,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms),Cyc_Absyndump_dump_callconv_qvar,& pr);});
_npop_handler(0U);goto _LL3;}}}
# 1101
;_pop_region();}}_LL3:;}
# 1125
if(initializer != 0){
Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(initializer);}
# 1128
if(do_semi)Cyc_Absyndump_dump_semi();}}
# 1131
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp248="@tagged ";_tag_fat(_tmp248,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl != 0){
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp363=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _fat_ptr _tmp362=({const char*_tmp249="<";_tag_fat(_tmp249,sizeof(char),2U);});struct _fat_ptr _tmp361=({const char*_tmp24A=">";_tag_fat(_tmp24A,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp363,_tmp362,_tmp361,({const char*_tmp24B=",";_tag_fat(_tmp24B,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1142
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump_char((int)'}');
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1147
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp24C=ed;struct Cyc_Core_Opt*_tmp24F;struct _tuple0*_tmp24E;enum Cyc_Absyn_Scope _tmp24D;_LL1: _tmp24D=_tmp24C->sc;_tmp24E=_tmp24C->name;_tmp24F=_tmp24C->fields;_LL2: {enum Cyc_Absyn_Scope sc=_tmp24D;struct _tuple0*nm=_tmp24E;struct Cyc_Core_Opt*fields=_tmp24F;
Cyc_Absyndump_dumpscope(sc);Cyc_Absyndump_dump(({const char*_tmp250="enum ";_tag_fat(_tmp250,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(nm);
if(fields != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char((int)'}');}}}
# 1154
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp251=dd;int _tmp256;struct Cyc_Core_Opt*_tmp255;struct Cyc_List_List*_tmp254;struct _tuple0*_tmp253;enum Cyc_Absyn_Scope _tmp252;_LL1: _tmp252=_tmp251->sc;_tmp253=_tmp251->name;_tmp254=_tmp251->tvs;_tmp255=_tmp251->fields;_tmp256=_tmp251->is_extensible;_LL2: {enum Cyc_Absyn_Scope sc=_tmp252;struct _tuple0*name=_tmp253;struct Cyc_List_List*tvs=_tmp254;struct Cyc_Core_Opt*fields=_tmp255;int is_x=_tmp256;
Cyc_Absyndump_dumpscope(sc);
if(is_x)
Cyc_Absyndump_dump(({const char*_tmp257="@extensible ";_tag_fat(_tmp257,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp258="datatype ";_tag_fat(_tmp258,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(name);Cyc_Absyndump_dumptvars(tvs);
if(fields != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char((int)'}');}}}struct _tuple23{unsigned f1;struct _tuple0*f2;int f3;};
# 1165
static void Cyc_Absyndump_dumpexport(struct _tuple23*tup){
Cyc_Absyndump_dumpqvar((*tup).f2);}
# 1168
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char((int)'{');
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char((int)'}');}
# 1175
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp259=d->r;void*_stmttmp19=_tmp259;void*_tmp25A=_stmttmp19;struct _tuple10*_tmp25E;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25F;struct Cyc_List_List*_tmp261;struct _tuple0*_tmp260;struct Cyc_List_List*_tmp263;struct _fat_ptr*_tmp262;struct Cyc_Absyn_Typedefdecl*_tmp264;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Vardecl*_tmp266;struct Cyc_Absyn_Tvar*_tmp265;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Pat*_tmp268;struct Cyc_List_List*_tmp26A;struct Cyc_Absyn_Enumdecl*_tmp26B;struct Cyc_Absyn_Datatypedecl*_tmp26C;struct Cyc_Absyn_Aggrdecl*_tmp26D;struct Cyc_Absyn_Vardecl*_tmp26E;struct Cyc_Absyn_Fndecl*_tmp26F;switch(*((int*)_tmp25A)){case 1U: _LL1: _tmp26F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LL2: {struct Cyc_Absyn_Fndecl*fd=_tmp26F;
# 1179
{enum Cyc_Flags_C_Compilers _tmp270=Cyc_Flags_c_compiler;if(_tmp270 == Cyc_Flags_Vc_c){_LL24: _LL25:
 Cyc_Absyndump_dumpatts((fd->i).attributes);goto _LL23;}else{_LL26: _LL27:
 goto _LL23;}_LL23:;}
# 1183
if(fd->is_inline){
enum Cyc_Flags_C_Compilers _tmp271=Cyc_Flags_c_compiler;if(_tmp271 == Cyc_Flags_Vc_c){_LL29: _LL2A:
 Cyc_Absyndump_dump(({const char*_tmp272="__inline";_tag_fat(_tmp272,sizeof(char),9U);}));goto _LL28;}else{_LL2B: _LL2C:
 Cyc_Absyndump_dump(({const char*_tmp273="inline";_tag_fat(_tmp273,sizeof(char),7U);}));goto _LL28;}_LL28:;}
# 1188
Cyc_Absyndump_dumpscope(fd->sc);{
# 1190
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A->tag=5U,_tmp27A->f1=type_info;_tmp27A;});
if(fd->cached_type != 0){
void*_tmp274=Cyc_Absyn_compress((void*)_check_null(fd->cached_type));void*_stmttmp1A=_tmp274;void*_tmp275=_stmttmp1A;struct Cyc_Absyn_FnInfo _tmp276;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp275)->tag == 5U){_LL2E: _tmp276=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp275)->f1;_LL2F: {struct Cyc_Absyn_FnInfo i=_tmp276;
# 1196
({struct Cyc_List_List*_tmp364=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).attributes,i.attributes);(fd->i).attributes=_tmp364;});goto _LL2D;}}else{_LL30: _LL31:
({void*_tmp277=0U;({struct _fat_ptr _tmp365=({const char*_tmp278="function has non-function type";_tag_fat(_tmp278,sizeof(char),31U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp365,_tag_fat(_tmp277,sizeof(void*),0U));});});}_LL2D:;}
# 1200
{enum Cyc_Flags_C_Compilers _tmp279=Cyc_Flags_c_compiler;if(_tmp279 == Cyc_Flags_Gcc_c){_LL33: _LL34:
({struct Cyc_Absyn_Tqual _tmp367=Cyc_Absyn_empty_tqual(0U);void*_tmp366=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp367,_tmp366,Cyc_Absyndump_dump_atts_qvar,fd);});goto _LL32;}else{_LL35: _LL36:
({struct Cyc_Absyn_Tqual _tmp369=Cyc_Absyn_empty_tqual(0U);void*_tmp368=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp369,_tmp368,Cyc_Absyndump_dump_callconv_fdqvar,fd);});goto _LL32;}_LL32:;}
# 1204
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(fd->body,0,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}}}case 0U: _LL3: _tmp26E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp26E;
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);goto _LL0;}case 5U: _LL5: _tmp26D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp26D;
Cyc_Absyndump_dump_aggrdecl(ad);Cyc_Absyndump_dump_semi();goto _LL0;}case 6U: _LL7: _tmp26C=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*dd=_tmp26C;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump_semi();goto _LL0;}case 7U: _LL9: _tmp26B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp26B;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump_semi();goto _LL0;}case 3U: _LLB: _tmp26A=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LLC: {struct Cyc_List_List*vds=_tmp26A;
({struct Cyc_List_List*_tmp36C=vds;struct _fat_ptr _tmp36B=({const char*_tmp27B=" let ";_tag_fat(_tmp27B,sizeof(char),6U);});struct _fat_ptr _tmp36A=({const char*_tmp27C=";";_tag_fat(_tmp27C,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpid,_tmp36C,_tmp36B,_tmp36A,({const char*_tmp27D=",";_tag_fat(_tmp27D,sizeof(char),2U);}));});goto _LL0;}case 2U: _LLD: _tmp268=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_tmp269=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp25A)->f3;_LLE: {struct Cyc_Absyn_Pat*p=_tmp268;struct Cyc_Absyn_Exp*e=_tmp269;
# 1214
Cyc_Absyndump_dump(({const char*_tmp27E="let";_tag_fat(_tmp27E,sizeof(char),4U);}));Cyc_Absyndump_dumppat(p);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_semi();goto _LL0;}case 4U: _LLF: _tmp265=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_tmp266=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp25A)->f2;_tmp267=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp25A)->f3;_LL10: {struct Cyc_Absyn_Tvar*tv=_tmp265;struct Cyc_Absyn_Vardecl*vd=_tmp266;struct Cyc_Absyn_Exp*open_exp_opt=_tmp267;
# 1216
Cyc_Absyndump_dump(({const char*_tmp27F="region";_tag_fat(_tmp27F,sizeof(char),7U);}));Cyc_Absyndump_dump(({const char*_tmp280="<";_tag_fat(_tmp280,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(({const char*_tmp281="> ";_tag_fat(_tmp281,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(vd->name);
if((unsigned)open_exp_opt){
Cyc_Absyndump_dump(({const char*_tmp282=" = open(";_tag_fat(_tmp282,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(open_exp_opt);Cyc_Absyndump_dump(({const char*_tmp283=")";_tag_fat(_tmp283,sizeof(char),2U);}));}
# 1220
Cyc_Absyndump_dump_semi();
goto _LL0;}case 8U: _LL11: _tmp264=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LL12: {struct Cyc_Absyn_Typedefdecl*td=_tmp264;
# 1227
Cyc_Absyndump_dump(({const char*_tmp284="typedef";_tag_fat(_tmp284,sizeof(char),8U);}));{
void*t=td->defn == 0?Cyc_Absyn_new_evar(td->kind,0):(void*)_check_null(td->defn);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(td->tq,t,Cyc_Absyndump_dumptypedefname,td);
Cyc_Absyndump_dumpatts(td->atts);
Cyc_Absyndump_dump_semi();
# 1233
goto _LL0;}}case 9U: _LL13: _tmp262=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_tmp263=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp25A)->f2;_LL14: {struct _fat_ptr*v=_tmp262;struct Cyc_List_List*tdl=_tmp263;
# 1235
Cyc_Absyndump_dump(({const char*_tmp285="namespace";_tag_fat(_tmp285,sizeof(char),10U);}));Cyc_Absyndump_dump_str(v);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 10U: _LL15: _tmp260=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_tmp261=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp25A)->f2;_LL16: {struct _tuple0*q=_tmp260;struct Cyc_List_List*tdl=_tmp261;
# 1237
Cyc_Absyndump_dump(({const char*_tmp286="using";_tag_fat(_tmp286,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(q);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 11U: _LL17: _tmp25F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_LL18: {struct Cyc_List_List*tdl=_tmp25F;
# 1239
Cyc_Absyndump_dump(({const char*_tmp287="extern \"C\" ";_tag_fat(_tmp287,sizeof(char),12U);}));Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 12U: _LL19: _tmp25B=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp25A)->f1;_tmp25C=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp25A)->f2;_tmp25D=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp25A)->f3;_tmp25E=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp25A)->f4;_LL1A: {struct Cyc_List_List*tdl=_tmp25B;struct Cyc_List_List*ovrs=_tmp25C;struct Cyc_List_List*exs=_tmp25D;struct _tuple10*wc=_tmp25E;
# 1241
Cyc_Absyndump_dump(({const char*_tmp288="extern \"C include\" ";_tag_fat(_tmp288,sizeof(char),20U);}));Cyc_Absyndump_dump_braced_decls(tdl);
if(ovrs != 0){
Cyc_Absyndump_dump(({const char*_tmp289=" cyclone_override ";_tag_fat(_tmp289,sizeof(char),19U);}));
Cyc_Absyndump_dump_braced_decls(ovrs);}
# 1246
if((unsigned)wc){
Cyc_Absyndump_dump(({const char*_tmp28A=" export { * }\n";_tag_fat(_tmp28A,sizeof(char),15U);}));{
struct _tuple10*_tmp28B=wc;struct Cyc_List_List*_tmp28C;_LL38: _tmp28C=_tmp28B->f2;_LL39: {struct Cyc_List_List*hides=_tmp28C;
if((unsigned)hides)
({struct Cyc_List_List*_tmp36F=hides;struct _fat_ptr _tmp36E=({const char*_tmp28D=" cyclone_hide {";_tag_fat(_tmp28D,sizeof(char),16U);});struct _fat_ptr _tmp36D=({const char*_tmp28E="}";_tag_fat(_tmp28E,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple0*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpqvar,_tmp36F,_tmp36E,_tmp36D,({const char*_tmp28F=",";_tag_fat(_tmp28F,sizeof(char),2U);}));});}}}else{
# 1254
({struct Cyc_List_List*_tmp372=exs;struct _fat_ptr _tmp371=({const char*_tmp290=" export {";_tag_fat(_tmp290,sizeof(char),10U);});struct _fat_ptr _tmp370=({const char*_tmp291="}";_tag_fat(_tmp291,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple23*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpexport,_tmp372,_tmp371,_tmp370,({const char*_tmp292=",";_tag_fat(_tmp292,sizeof(char),2U);}));});}
# 1256
goto _LL0;}case 13U: _LL1B: _LL1C:
 Cyc_Absyndump_dump(({const char*_tmp293=" __cyclone_port_on__; ";_tag_fat(_tmp293,sizeof(char),23U);}));goto _LL0;case 14U: _LL1D: _LL1E:
 Cyc_Absyndump_dump(({const char*_tmp294=" __cyclone_port_off__; ";_tag_fat(_tmp294,sizeof(char),24U);}));goto _LL0;case 15U: _LL1F: _LL20:
 Cyc_Absyndump_dump(({const char*_tmp295=" __tempest_on__; ";_tag_fat(_tmp295,sizeof(char),18U);}));goto _LL0;default: _LL21: _LL22:
 Cyc_Absyndump_dump(({const char*_tmp296=" __tempest_off__; ";_tag_fat(_tmp296,sizeof(char),19U);}));goto _LL0;}_LL0:;}}
# 1264
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple14 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != (unsigned)1 || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1271
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1273
if(tms == 0){
f(a);
return;}{
# 1277
void*_tmp297=(void*)tms->hd;void*_stmttmp1B=_tmp297;void*_tmp298=_stmttmp1B;struct Cyc_Absyn_Tqual _tmp29E;void*_tmp29D;void*_tmp29C;void*_tmp29B;void*_tmp29A;void*_tmp299;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->tag == 2U){_LL1: _tmp299=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->f1).rgn;_tmp29A=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->f1).nullable;_tmp29B=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->f1).bounds;_tmp29C=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->f1).zero_term;_tmp29D=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->f1).released;_tmp29E=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp298)->f2;_LL2: {void*rgn=_tmp299;void*nullable=_tmp29A;void*bd=_tmp29B;void*zt=_tmp29C;void*rel=_tmp29D;struct Cyc_Absyn_Tqual tq2=_tmp29E;
# 1281
{void*_tmp29F=Cyc_Absyn_compress(bd);void*_stmttmp1C=_tmp29F;void*_tmp2A0=_stmttmp1C;struct Cyc_Absyn_Exp*_tmp2A1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0)->f1)){case 14U: _LL6: _LL7:
 Cyc_Absyndump_dump_char((int)'?');goto _LL5;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0)->f2)->hd)->tag == 9U){_LL8: _tmp2A1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0)->f2)->hd)->f1;_LL9: {struct Cyc_Absyn_Exp*e=_tmp2A1;
# 1284
Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));Cyc_Absyndump_dump_upperbound(e);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA: _LLB:
# 1286
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));}_LL5:;}
# 1288
if((!Cyc_Absyndump_qvar_to_Cids && !is_char_ptr)&& Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp2A2="@zeroterm";_tag_fat(_tmp2A2,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids && is_char_ptr)&& !Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(({const char*_tmp2A3="@nozeroterm";_tag_fat(_tmp2A3,sizeof(char),12U);}));
if(!Cyc_Absyndump_qvar_to_Cids && Cyc_Absyn_type2bool(0,rel))Cyc_Absyndump_dump(({const char*_tmp2A4="@released";_tag_fat(_tmp2A4,sizeof(char),10U);}));
{void*_tmp2A5=Cyc_Absyn_compress(rgn);void*_stmttmp1D=_tmp2A5;void*_tmp2A6=_stmttmp1D;struct Cyc_Absyn_Tvar*_tmp2A7;switch(*((int*)_tmp2A6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A6)->f1)){case 5U: _LLD: _LLE:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp2A8="`H";_tag_fat(_tmp2A8,sizeof(char),3U);}));goto _LLC;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp2A9="`U";_tag_fat(_tmp2A9,sizeof(char),3U);}));goto _LLC;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp2AA="`RC";_tag_fat(_tmp2AA,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2U: _LL13: _tmp2A7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2A6)->f1;_LL14: {struct Cyc_Absyn_Tvar*tv=_tmp2A7;
Cyc_Absyndump_dumptvar(tv);goto _LLC;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f2 == 0){_LL15: _LL16:
 Cyc_Absyndump_dumpntyp(Cyc_Absyn_compress(rgn));goto _LLC;}else{goto _LL19;}case 8U: _LL17: _LL18:
 Cyc_Absyndump_dump(({const char*_tmp2AB="@region(";_tag_fat(_tmp2AB,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(rgn);Cyc_Absyndump_dump(({const char*_tmp2AC=")";_tag_fat(_tmp2AC,sizeof(char),2U);}));goto _LLC;default: _LL19: _LL1A:
({void*_tmp2AD=0U;({struct _fat_ptr _tmp373=({const char*_tmp2AE="dumptms: bad rgn type in Pointer_mod";_tag_fat(_tmp2AE,sizeof(char),37U);});Cyc_Warn_impos(_tmp373,_tag_fat(_tmp2AD,sizeof(void*),0U));});});}_LLC:;}
# 1300
Cyc_Absyndump_dumptq(tq2);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}}else{_LL3: _LL4: {
# 1305
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2AF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmp1E=_tmp2AF;void*_tmp2B0=_stmttmp1E;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2B0)->tag == 2U){_LL1C: _LL1D:
 next_is_pointer=1;goto _LL1B;}else{_LL1E: _LL1F:
 goto _LL1B;}_LL1B:;}
# 1311
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2B1=(void*)tms->hd;void*_stmttmp1F=_tmp2B1;void*_tmp2B2=_stmttmp1F;struct Cyc_List_List*_tmp2B3;int _tmp2B6;unsigned _tmp2B5;struct Cyc_List_List*_tmp2B4;unsigned _tmp2B8;struct Cyc_List_List*_tmp2B7;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_List_List*_tmp2BD;void*_tmp2BC;struct Cyc_Absyn_VarargInfo*_tmp2BB;int _tmp2BA;struct Cyc_List_List*_tmp2B9;void*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;void*_tmp2C2;switch(*((int*)_tmp2B2)){case 0U: _LL21: _tmp2C2=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1;_LL22: {void*zeroterm=_tmp2C2;
# 1318
Cyc_Absyndump_dump(({const char*_tmp2C3="[]";_tag_fat(_tmp2C3,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp2C4="@zeroterm";_tag_fat(_tmp2C4,sizeof(char),10U);}));
goto _LL20;}case 1U: _LL23: _tmp2C0=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1;_tmp2C1=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f2;_LL24: {struct Cyc_Absyn_Exp*e=_tmp2C0;void*zeroterm=_tmp2C1;
# 1322
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)']');
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(({const char*_tmp2C5="@zeroterm";_tag_fat(_tmp2C5,sizeof(char),10U);}));
goto _LL20;}case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->tag == 1U){_LL25: _tmp2B9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f1;_tmp2BA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f2;_tmp2BB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f3;_tmp2BC=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f4;_tmp2BD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f5;_tmp2BE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f6;_tmp2BF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f7;_LL26: {struct Cyc_List_List*args=_tmp2B9;int c_varargs=_tmp2BA;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp2BB;void*effopt=_tmp2BC;struct Cyc_List_List*rgn_po=_tmp2BD;struct Cyc_Absyn_Exp*req=_tmp2BE;struct Cyc_Absyn_Exp*ens=_tmp2BF;
# 1326
Cyc_Absyndump_dumpfunargs(args,c_varargs,cyc_varargs,effopt,rgn_po,req,ens);goto _LL20;}}else{_LL27: _tmp2B7=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f1;_tmp2B8=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f2;_LL28: {struct Cyc_List_List*sl=_tmp2B7;unsigned loc=_tmp2B8;
# 1328
({struct Cyc_List_List*_tmp376=sl;struct _fat_ptr _tmp375=({const char*_tmp2C6="(";_tag_fat(_tmp2C6,sizeof(char),2U);});struct _fat_ptr _tmp374=({const char*_tmp2C7=")";_tag_fat(_tmp2C7,sizeof(char),2U);});((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp376,_tmp375,_tmp374,({const char*_tmp2C8=",";_tag_fat(_tmp2C8,sizeof(char),2U);}));});goto _LL20;}}case 4U: _LL29: _tmp2B4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1;_tmp2B5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f2;_tmp2B6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f3;_LL2A: {struct Cyc_List_List*ts=_tmp2B4;unsigned loc=_tmp2B5;int print_kinds=_tmp2B6;
# 1330
if(print_kinds)Cyc_Absyndump_dumpkindedtvars(ts);else{Cyc_Absyndump_dumptvars(ts);}goto _LL20;}case 5U: _LL2B: _tmp2B3=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f2;_LL2C: {struct Cyc_List_List*atts=_tmp2B3;
Cyc_Absyndump_dumpatts(atts);goto _LL20;}default: _LL2D: _LL2E:
({void*_tmp2C9=0U;({struct _fat_ptr _tmp377=({const char*_tmp2CA="dumptms";_tag_fat(_tmp2CA,sizeof(char),8U);});Cyc_Warn_impos(_tmp377,_tag_fat(_tmp2C9,sizeof(void*),0U));});});}_LL20:;}
# 1334
return;}}_LL0:;}}
# 1338
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2CB=Cyc_Absyndump_is_char_ptr(t);int cp=_tmp2CB;
struct _RegionHandle _tmp2CC=_new_region("temp");struct _RegionHandle*temp=& _tmp2CC;_push_region(temp);
{struct _tuple12 _tmp2CD=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple12 _stmttmp20=_tmp2CD;struct _tuple12 _tmp2CE=_stmttmp20;struct Cyc_List_List*_tmp2D1;void*_tmp2D0;struct Cyc_Absyn_Tqual _tmp2CF;_LL1: _tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;_tmp2D1=_tmp2CE.f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp2CF;void*t=_tmp2D0;struct Cyc_List_List*tms=_tmp2D1;
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);
({int _tmp37A=cp;struct Cyc_List_List*_tmp379=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(tms);void(*_tmp378)(void*)=f;Cyc_Absyndump_dumptms(_tmp37A,_tmp379,_tmp378,a);});}}
# 1341
;_pop_region();}
# 1347
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1349
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2D2=0U;({struct Cyc___cycFILE*_tmp37C=f;struct _fat_ptr _tmp37B=({const char*_tmp2D3="\n";_tag_fat(_tmp2D3,sizeof(char),2U);});Cyc_fprintf(_tmp37C,_tmp37B,_tag_fat(_tmp2D2,sizeof(void*),0U));});});}
# 1355
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2D4=d->r;void*_stmttmp21=_tmp2D4;void*_tmp2D5=_stmttmp21;struct _tuple10*_tmp2D9;struct Cyc_List_List*_tmp2D8;struct Cyc_List_List*_tmp2D7;struct Cyc_List_List*_tmp2D6;struct Cyc_List_List*_tmp2DA;struct Cyc_List_List*_tmp2DB;struct Cyc_List_List*_tmp2DD;struct _fat_ptr*_tmp2DC;struct Cyc_Absyn_Typedefdecl*_tmp2DE;struct Cyc_Absyn_Enumdecl*_tmp2DF;struct Cyc_Absyn_Datatypedecl*_tmp2E0;struct Cyc_Absyn_Aggrdecl*_tmp2E1;struct Cyc_Absyn_Fndecl*_tmp2E2;struct Cyc_Absyn_Vardecl*_tmp2E3;switch(*((int*)_tmp2D5)){case 0U: _LL1: _tmp2E3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp2E3;
# 1358
if((int)vd->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=vd->initializer;
vd->initializer=0;
if((int)vd->sc == (int)Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2E4="extern ";_tag_fat(_tmp2E4,sizeof(char),8U);}));else{
if(!vd->is_proto){
void*_tmp2E5=Cyc_Absyn_compress(vd->type);void*_stmttmp22=_tmp2E5;void*_tmp2E6=_stmttmp22;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2E6)->tag == 5U){_LL18: _LL19:
 Cyc_Absyndump_dump(({const char*_tmp2E7="extern ";_tag_fat(_tmp2E7,sizeof(char),8U);}));goto _LL17;}else{_LL1A: _LL1B:
 goto _LL17;}_LL17:;}}
# 1368
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2E8="\n";_tag_fat(_tmp2E8,sizeof(char),2U);}));
vd->initializer=init;
goto _LL0;}}case 1U: _LL3: _tmp2E2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp2E2;
# 1373
if((int)fd->sc == (int)Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(fd->sc);{
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->tag=5U,_tmp2EA->f1=type_info;_tmp2EA;});
({struct Cyc_Absyn_Tqual _tmp37E=Cyc_Absyn_empty_tqual(0U);void*_tmp37D=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp37E,_tmp37D,Cyc_Absyndump_dump_atts_qvar,fd);});
Cyc_Absyndump_dump(({const char*_tmp2E9=";\n";_tag_fat(_tmp2E9,sizeof(char),3U);}));
goto _LL0;}}}case 5U: _LL5: _tmp2E1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2E1;
# 1382
if((int)ad->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if((int)ad->sc == (int)Cyc_Absyn_Abstract)ad->impl=0;
Cyc_Absyndump_dump_aggrdecl(ad);
ad->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2EB=";\n";_tag_fat(_tmp2EB,sizeof(char),3U);}));
goto _LL0;}}case 6U: _LL7: _tmp2E0=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*dd=_tmp2E0;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump(({const char*_tmp2EC=";\n";_tag_fat(_tmp2EC,sizeof(char),3U);}));goto _LL0;}case 7U: _LL9: _tmp2DF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp2DF;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump(({const char*_tmp2ED=";\n";_tag_fat(_tmp2ED,sizeof(char),3U);}));goto _LL0;}case 8U: _LLB: _tmp2DE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp2DE;
# 1392
if(td->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2EE="\n";_tag_fat(_tmp2EE,sizeof(char),2U);}));}
# 1396
goto _LL0;}case 9U: _LLD: _tmp2DC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_tmp2DD=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2D5)->f2;_LLE: {struct _fat_ptr*v=_tmp2DC;struct Cyc_List_List*ds=_tmp2DD;
# 1398
Cyc_Absyndump_dump(({const char*_tmp2EF="namespace ";_tag_fat(_tmp2EF,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(v);
Cyc_Absyndump_dump(({const char*_tmp2F0="{\n";_tag_fat(_tmp2F0,sizeof(char),3U);}));
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp2F1="}\n";_tag_fat(_tmp2F1,sizeof(char),3U);}));
goto _LL0;}case 10U: _LLF: _tmp2DB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2D5)->f2;_LL10: {struct Cyc_List_List*ds=_tmp2DB;
# 1406
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
goto _LL0;}case 11U: _LL11: _tmp2DA=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_LL12: {struct Cyc_List_List*ds=_tmp2DA;
# 1410
Cyc_Absyndump_dump(({const char*_tmp2F2="extern \"C\" {";_tag_fat(_tmp2F2,sizeof(char),13U);}));
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(({const char*_tmp2F3="}\n";_tag_fat(_tmp2F3,sizeof(char),3U);}));
goto _LL0;}case 12U: _LL13: _tmp2D6=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D5)->f1;_tmp2D7=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D5)->f2;_tmp2D8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D5)->f3;_tmp2D9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D5)->f4;_LL14: {struct Cyc_List_List*ds=_tmp2D6;struct Cyc_List_List*ovrs=_tmp2D7;struct Cyc_List_List*exs=_tmp2D8;struct _tuple10*wc=_tmp2D9;
# 1431 "absyndump.cyc"
goto _LL0;}default: _LL15: _LL16:
({void*_tmp2F4=0U;({struct _fat_ptr _tmp37F=({const char*_tmp2F5="bad top-level declaration";_tag_fat(_tmp2F5,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp37F,_tag_fat(_tmp2F4,sizeof(void*),0U));});});}_LL0:;}
# 1436
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}

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
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 64
struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 38 "position.h"
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 46
extern int Cyc_Position_use_gcc_style_location;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
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
# 874 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 883
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 901
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 906
extern void*Cyc_Absyn_sint_type;
# 917
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);
# 939
void*Cyc_Absyn_string_type(void*rgn);
# 942
extern void*Cyc_Absyn_fat_bound_type;
# 962
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
# 1145
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 107
void*Cyc_Tcutil_compress(void*);
# 303
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 306
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 88 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 59
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data);struct Cyc_Port_Edit{unsigned loc;struct _fat_ptr old_string;struct _fat_ptr new_string;};
# 88 "port.cyc"
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){
return(int)e1 - (int)e2;}
# 91
static unsigned Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
return e->loc;}
# 94
int Cyc_Port_cmp_seg_t(unsigned loc1,unsigned loc2){
return(int)(loc1 - loc2);}
# 97
int Cyc_Port_hash_seg_t(unsigned loc){
return(int)loc;}struct Cyc_Port_VarUsage{int address_taken;struct Cyc_Absyn_Exp*init;struct Cyc_List_List*usage_locs;};struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};struct Cyc_Port_Cfield{void*qual;struct _fat_ptr*f;void*type;};struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple10{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple10*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
# 153
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0U};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1U};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2U};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3U};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4U};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5U};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6U};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7U};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8U};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9U};
# 166
static struct _fat_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
static struct _fat_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);
static struct _fat_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_tmp0=t;struct Cyc_Port_Cvar*_tmp44;void*_tmp43;struct Cyc_List_List*_tmp42;struct Cyc_List_List*_tmp41;struct Cyc_List_List*_tmp40;void*_tmp3F;struct Cyc_Absyn_Aggrdecl*_tmp3E;struct Cyc_List_List*_tmp3D;void*_tmp3C;void*_tmp3B;void*_tmp3A;void*_tmp39;void*_tmp38;void*_tmp37;void*_tmp36;void*_tmp35;struct _fat_ptr*_tmp34;switch(*((int*)_tmp0)){case 0U: _LL1: _LL2:
 return({const char*_tmp1="const";_tag_fat(_tmp1,sizeof(char),6U);});case 1U: _LL3: _LL4:
 return({const char*_tmp2="notconst";_tag_fat(_tmp2,sizeof(char),9U);});case 2U: _LL5: _LL6:
 return({const char*_tmp3="thin";_tag_fat(_tmp3,sizeof(char),5U);});case 3U: _LL7: _LL8:
 return({const char*_tmp4="fat";_tag_fat(_tmp4,sizeof(char),4U);});case 4U: _LL9: _LLA:
 return({const char*_tmp5="void";_tag_fat(_tmp5,sizeof(char),5U);});case 5U: _LLB: _LLC:
 return({const char*_tmp6="zero";_tag_fat(_tmp6,sizeof(char),5U);});case 6U: _LLD: _LLE:
 return({const char*_tmp7="arith";_tag_fat(_tmp7,sizeof(char),6U);});case 7U: _LLF: _LL10:
 return({const char*_tmp8="heap";_tag_fat(_tmp8,sizeof(char),5U);});case 8U: _LL11: _LL12:
 return({const char*_tmp9="ZT";_tag_fat(_tmp9,sizeof(char),3U);});case 9U: _LL13: _LL14:
 return({const char*_tmpA="NZT";_tag_fat(_tmpA,sizeof(char),4U);});case 10U: _LL15: _tmp34=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp0)->f1;_LL16:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp383;_tmp383.tag=0U,_tmp383.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp34);_tmp383;});void*_tmpB[1U];_tmpB[0]=& _tmpD;({struct _fat_ptr _tmp3B3=({const char*_tmpC="%s";_tag_fat(_tmpC,sizeof(char),3U);});Cyc_aprintf(_tmp3B3,_tag_fat(_tmpB,sizeof(void*),1U));});});case 11U: _LL17: _tmp39=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp38=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f2;_tmp37=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f3;_tmp36=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f4;_tmp35=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f5;_LL18:
# 182
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0U,({struct _fat_ptr _tmp3B4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp39));_tmp388.f1=_tmp3B4;});_tmp388;});struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmp387;_tmp387.tag=0U,({
struct _fat_ptr _tmp3B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp38));_tmp387.f1=_tmp3B5;});_tmp387;});struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0U,({struct _fat_ptr _tmp3B6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp37));_tmp386.f1=_tmp3B6;});_tmp386;});struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0U,({
struct _fat_ptr _tmp3B7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp36));_tmp385.f1=_tmp3B7;});_tmp385;});struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp384;_tmp384.tag=0U,({struct _fat_ptr _tmp3B8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp35));_tmp384.f1=_tmp3B8;});_tmp384;});void*_tmpE[5U];_tmpE[0]=& _tmp10,_tmpE[1]=& _tmp11,_tmpE[2]=& _tmp12,_tmpE[3]=& _tmp13,_tmpE[4]=& _tmp14;({struct _fat_ptr _tmp3B9=({const char*_tmpF="ptr(%s,%s,%s,%s,%s)";_tag_fat(_tmpF,sizeof(char),20U);});Cyc_aprintf(_tmp3B9,_tag_fat(_tmpE,sizeof(void*),5U));});});case 12U: _LL19: _tmp3C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp3B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f2;_tmp3A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f3;_LL1A:
# 186
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U,({struct _fat_ptr _tmp3BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp3C));_tmp38B.f1=_tmp3BA;});_tmp38B;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp38A;_tmp38A.tag=0U,({
struct _fat_ptr _tmp3BB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp3B));_tmp38A.f1=_tmp3BB;});_tmp38A;});struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0U,({struct _fat_ptr _tmp3BC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp3A));_tmp389.f1=_tmp3BC;});_tmp389;});void*_tmp15[3U];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18,_tmp15[2]=& _tmp19;({struct _fat_ptr _tmp3BD=({const char*_tmp16="array(%s,%s,%s)";_tag_fat(_tmp16,sizeof(char),16U);});Cyc_aprintf(_tmp3BD,_tag_fat(_tmp15,sizeof(void*),3U));});});case 13U: _LL1B: _tmp3E=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f1;_tmp3D=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f2;_LL1C: {
# 189
struct _fat_ptr s=(int)_tmp3E->kind == (int)Cyc_Absyn_StructA?({const char*_tmp23="struct";_tag_fat(_tmp23,sizeof(char),7U);}):({const char*_tmp24="union";_tag_fat(_tmp24,sizeof(char),6U);});
if(!deep)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0U,_tmp38D.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp38D;});struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp38C;_tmp38C.tag=0U,({struct _fat_ptr _tmp3BE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp3E->name));_tmp38C.f1=_tmp3BE;});_tmp38C;});void*_tmp1A[2U];_tmp1A[0]=& _tmp1C,_tmp1A[1]=& _tmp1D;({struct _fat_ptr _tmp3BF=({const char*_tmp1B="%s %s";_tag_fat(_tmp1B,sizeof(char),6U);});Cyc_aprintf(_tmp3BF,_tag_fat(_tmp1A,sizeof(void*),2U));});});else{
# 193
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp390;_tmp390.tag=0U,_tmp390.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp390;});struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,({struct _fat_ptr _tmp3C0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp3E->name));_tmp38F.f1=_tmp3C0;});_tmp38F;});struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,({
struct _fat_ptr _tmp3C1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_cfields2string(0,_tmp3D));_tmp38E.f1=_tmp3C1;});_tmp38E;});void*_tmp1E[3U];_tmp1E[0]=& _tmp20,_tmp1E[1]=& _tmp21,_tmp1E[2]=& _tmp22;({struct _fat_ptr _tmp3C2=({const char*_tmp1F="%s %s {%s}";_tag_fat(_tmp1F,sizeof(char),11U);});Cyc_aprintf(_tmp3C2,_tag_fat(_tmp1E,sizeof(void*),3U));});});}}case 14U: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2 != 0){_LL1D: _tmp40=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp3F=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2;_LL1E:
 return Cyc_Port_ctype2string(deep,_tmp3F);}else{_LL1F: _tmp41=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;_LL20:
# 197
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp391;_tmp391.tag=0U,({struct _fat_ptr _tmp3C3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_cfields2string(deep,_tmp41));_tmp391.f1=_tmp3C3;});_tmp391;});void*_tmp25[1U];_tmp25[0]=& _tmp27;({struct _fat_ptr _tmp3C4=({const char*_tmp26="aggr {%s}";_tag_fat(_tmp26,sizeof(char),10U);});Cyc_aprintf(_tmp3C4,_tag_fat(_tmp25,sizeof(void*),1U));});});}case 15U: _LL21: _tmp43=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp42=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f2;_LL22:
# 199
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp393;_tmp393.tag=0U,({struct _fat_ptr _tmp3C5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(deep,_tmp42));_tmp393.f1=_tmp3C5;});_tmp393;});struct Cyc_String_pa_PrintArg_struct _tmp2B=({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0U,({struct _fat_ptr _tmp3C6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,_tmp43));_tmp392.f1=_tmp3C6;});_tmp392;});void*_tmp28[2U];_tmp28[0]=& _tmp2A,_tmp28[1]=& _tmp2B;({struct _fat_ptr _tmp3C7=({const char*_tmp29="fn(%s)->%s";_tag_fat(_tmp29,sizeof(char),11U);});Cyc_aprintf(_tmp3C7,_tag_fat(_tmp28,sizeof(void*),2U));});});default: _LL23: _tmp44=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp0)->f1;_LL24:
# 201
 if((unsigned)_tmp44->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp44->eq)));else{
if(!deep || _tmp44->uppers == 0 && _tmp44->lowers == 0)
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2E=({struct Cyc_Int_pa_PrintArg_struct _tmp394;_tmp394.tag=1U,_tmp394.f1=(unsigned long)_tmp44->id;_tmp394;});void*_tmp2C[1U];_tmp2C[0]=& _tmp2E;({struct _fat_ptr _tmp3C8=({const char*_tmp2D="var(%d)";_tag_fat(_tmp2D,sizeof(char),8U);});Cyc_aprintf(_tmp3C8,_tag_fat(_tmp2C,sizeof(void*),1U));});});else{
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp397;_tmp397.tag=0U,({
struct _fat_ptr _tmp3C9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(0,_tmp44->lowers));_tmp397.f1=_tmp3C9;});_tmp397;});struct Cyc_Int_pa_PrintArg_struct _tmp32=({struct Cyc_Int_pa_PrintArg_struct _tmp396;_tmp396.tag=1U,_tmp396.f1=(unsigned long)_tmp44->id;_tmp396;});struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp395;_tmp395.tag=0U,({
# 208
struct _fat_ptr _tmp3CA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(0,_tmp44->uppers));_tmp395.f1=_tmp3CA;});_tmp395;});void*_tmp2F[3U];_tmp2F[0]=& _tmp31,_tmp2F[1]=& _tmp32,_tmp2F[2]=& _tmp33;({struct _fat_ptr _tmp3CB=({const char*_tmp30="var([%s]<=%d<=[%s])";_tag_fat(_tmp30,sizeof(char),20U);});Cyc_aprintf(_tmp3CB,_tag_fat(_tmp2F,sizeof(void*),3U));});});}}}_LL0:;}
# 211
static struct _fat_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){return({struct _fat_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));({struct _fat_ptr _tmp3CC=Cyc_Port_ctype2string(deep,t);*_tmp45=_tmp3CC;});_tmp45;});}
struct Cyc_List_List*Cyc_Port_sep(struct _fat_ptr s,struct Cyc_List_List*xs){
struct _fat_ptr*_tmp46=({struct _fat_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=s;_tmp4A;});
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp47=xs;
struct Cyc_List_List*_tmp48=xs->tl;
for(0;_tmp48 != 0;(_tmp47=_tmp48,_tmp48=_tmp48->tl)){
({struct Cyc_List_List*_tmp3CD=({struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->hd=_tmp46,_tmp49->tl=_tmp48;_tmp49;});_tmp47->tl=_tmp3CD;});}
# 220
return xs;};}
# 222
static struct _fat_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0U,({
struct _fat_ptr _tmp3CE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,f->qual));_tmp39A.f1=_tmp3CE;});_tmp39A;});struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0U,_tmp399.f1=(struct _fat_ptr)((struct _fat_ptr)*f->f);_tmp399;});struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0U,({struct _fat_ptr _tmp3CF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,f->type));_tmp398.f1=_tmp3CF;});_tmp398;});void*_tmp4C[3U];_tmp4C[0]=& _tmp4E,_tmp4C[1]=& _tmp4F,_tmp4C[2]=& _tmp50;({struct _fat_ptr _tmp3D0=({const char*_tmp4D="%s %s: %s";_tag_fat(_tmp4D,sizeof(char),10U);});Cyc_aprintf(_tmp3D0,_tag_fat(_tmp4C,sizeof(void*),3U));});});
return({struct _fat_ptr*_tmp4B=_cycalloc(sizeof(*_tmp4B));*_tmp4B=s;_tmp4B;});}
# 227
static struct _fat_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr _tmp3D1=({const char*_tmp51=",";_tag_fat(_tmp51,sizeof(char),2U);});Cyc_Port_sep(_tmp3D1,((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts));}));}
# 230
static struct _fat_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr _tmp3D2=({const char*_tmp52=";";_tag_fat(_tmp52,sizeof(char),2U);});Cyc_Port_sep(_tmp3D2,((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs));}));}
# 236
static void*Cyc_Port_notconst_ct(){return(void*)& Cyc_Port_Notconst_ct_val;}
static void*Cyc_Port_const_ct(){return(void*)& Cyc_Port_Const_ct_val;}
static void*Cyc_Port_thin_ct(){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct(){return(void*)& Cyc_Port_Fat_ct_val;}
static void*Cyc_Port_void_ct(){return(void*)& Cyc_Port_Void_ct_val;}
static void*Cyc_Port_zero_ct(){return(void*)& Cyc_Port_Zero_ct_val;}
static void*Cyc_Port_arith_ct(){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct(){return(void*)& Cyc_Port_Heap_ct_val;}
static void*Cyc_Port_zterm_ct(){return(void*)& Cyc_Port_Zterm_ct_val;}
static void*Cyc_Port_nozterm_ct(){return(void*)& Cyc_Port_Nozterm_ct_val;}
static void*Cyc_Port_rgnvar_ct(struct _fat_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->tag=10U,_tmp53->f1=n;_tmp53;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=14U,_tmp54->f1=fs,_tmp54->f2=0;_tmp54;});}
# 250
static void*Cyc_Port_known_aggr_ct(struct _tuple10*p){
return(void*)({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=13U,_tmp55->f1=p;_tmp55;});}
# 253
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 255
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=11U,_tmp56->f1=elt,_tmp56->f2=qual,_tmp56->f3=ptr_kind,_tmp56->f4=r,_tmp56->f5=zt;_tmp56;});}
# 257
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=12U,_tmp57->f1=elt,_tmp57->f2=qual,_tmp57->f3=zt;_tmp57;});}
# 260
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=15U,_tmp58->f1=return_type,_tmp58->f2=args;_tmp58;});}
# 263
static void*Cyc_Port_var(){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=16U,({struct Cyc_Port_Cvar*_tmp3D3=({struct Cyc_Port_Cvar*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->id=counter ++,_tmp59->eq=0,_tmp59->uppers=0,_tmp59->lowers=0;_tmp59;});_tmp5A->f1=_tmp3D3;});_tmp5A;});}
# 267
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 270
static struct _fat_ptr*Cyc_Port_new_region_var(){
static int counter=0;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5E=({struct Cyc_Int_pa_PrintArg_struct _tmp39B;_tmp39B.tag=1U,_tmp39B.f1=(unsigned long)counter ++;_tmp39B;});void*_tmp5C[1U];_tmp5C[0]=& _tmp5E;({struct _fat_ptr _tmp3D4=({const char*_tmp5D="`R%d";_tag_fat(_tmp5D,sizeof(char),5U);});Cyc_aprintf(_tmp3D4,_tag_fat(_tmp5C,sizeof(void*),1U));});});
return({struct _fat_ptr*_tmp5B=_cycalloc(sizeof(*_tmp5B));*_tmp5B=s;_tmp5B;});}
# 278
static int Cyc_Port_unifies(void*t1,void*t2);
# 280
static void*Cyc_Port_compress_ct(void*t){
void*_tmp5F=t;void**_tmp6B;void***_tmp6A;struct Cyc_List_List*_tmp69;struct Cyc_List_List*_tmp68;switch(*((int*)_tmp5F)){case 16U: _LL1: _tmp6A=(void***)&(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->eq;_tmp69=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->uppers;_tmp68=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->lowers;_LL2: {
# 283
void**_tmp60=*_tmp6A;
if((unsigned)_tmp60){
# 287
void*r=Cyc_Port_compress_ct(*_tmp60);
if(*_tmp60 != r)({void**_tmp3D5=({void**_tmp61=_cycalloc(sizeof(*_tmp61));*_tmp61=r;_tmp61;});*_tmp6A=_tmp3D5;});
return r;}
# 294
for(0;_tmp68 != 0;_tmp68=_tmp68->tl){
void*_tmp62=(void*)_tmp68->hd;void*_tmp63=_tmp62;switch(*((int*)_tmp63)){case 0U: _LL8: _LL9:
 goto _LLB;case 9U: _LLA: _LLB:
 goto _LLD;case 4U: _LLC: _LLD:
 goto _LLF;case 13U: _LLE: _LLF:
 goto _LL11;case 15U: _LL10: _LL11:
# 301
({void**_tmp3D6=({void**_tmp64=_cycalloc(sizeof(*_tmp64));*_tmp64=(void*)_tmp68->hd;_tmp64;});*_tmp6A=_tmp3D6;});
return(void*)_tmp68->hd;default: _LL12: _LL13:
# 304
 goto _LL7;}_LL7:;}
# 307
for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
void*_tmp65=(void*)_tmp69->hd;void*_tmp66=_tmp65;switch(*((int*)_tmp66)){case 1U: _LL15: _LL16:
 goto _LL18;case 8U: _LL17: _LL18:
 goto _LL1A;case 5U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 15U: _LL1D: _LL1E:
# 314
({void**_tmp3D7=({void**_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=(void*)_tmp69->hd;_tmp67;});*_tmp6A=_tmp3D7;});
return(void*)_tmp69->hd;default: _LL1F: _LL20:
# 317
 goto _LL14;}_LL14:;}
# 320
return t;}case 14U: _LL3: _tmp6B=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp5F)->f2;_LL4:
# 323
 if((unsigned)_tmp6B)return Cyc_Port_compress_ct(*_tmp6B);else{
return t;}default: _LL5: _LL6:
# 327
 return t;}_LL0:;}struct _tuple11{void*f1;void*f2;};
# 333
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp6C=Cyc_Port_compress_ct(t);void*_tmp6D=_tmp6C;void*_tmp80;struct Cyc_List_List*_tmp7F;void*_tmp7E;void*_tmp7D;void*_tmp7C;void*_tmp7B;void*_tmp7A;void*_tmp79;void*_tmp78;void*_tmp77;struct _fat_ptr*_tmp76;switch(*((int*)_tmp6D)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 8U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 13U: _LL15: _LL16:
 goto _LL18;case 16U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 return t;case 10U: _LL1B: _tmp76=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp6D)->f1;_LL1C:
# 349
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*instenv,_tmp76))
({struct Cyc_Dict_Dict _tmp3DA=({struct Cyc_Dict_Dict _tmp3D9=*instenv;struct _fat_ptr*_tmp3D8=_tmp76;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3D9,_tmp3D8,Cyc_Port_var());});*instenv=_tmp3DA;});
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*instenv,_tmp76);case 11U: _LL1D: _tmp7B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp7A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f2;_tmp79=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f3;_tmp78=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f4;_tmp77=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f5;_LL1E: {
# 353
struct _tuple11 _tmp6E=({struct _tuple11 _tmp39C;({void*_tmp3DC=Cyc_Port_inst(instenv,_tmp7B);_tmp39C.f1=_tmp3DC;}),({void*_tmp3DB=Cyc_Port_inst(instenv,_tmp78);_tmp39C.f2=_tmp3DB;});_tmp39C;});struct _tuple11 _tmp6F=_tmp6E;void*_tmp72;void*_tmp71;_LL24: _tmp72=_tmp6F.f1;_tmp71=_tmp6F.f2;_LL25:;
if(_tmp72 == _tmp7B && _tmp71 == _tmp78)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=11U,_tmp70->f1=_tmp72,_tmp70->f2=_tmp7A,_tmp70->f3=_tmp79,_tmp70->f4=_tmp71,_tmp70->f5=_tmp77;_tmp70;});}case 12U: _LL1F: _tmp7E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp7D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f2;_tmp7C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f3;_LL20: {
# 357
void*_tmp73=Cyc_Port_inst(instenv,_tmp7E);
if(_tmp73 == _tmp7E)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=12U,_tmp74->f1=_tmp73,_tmp74->f2=_tmp7D,_tmp74->f3=_tmp7C;_tmp74;});}default: _LL21: _tmp80=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp7F=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp6D)->f2;_LL22:
# 361
 return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=15U,({void*_tmp3DE=Cyc_Port_inst(instenv,_tmp80);_tmp75->f1=_tmp3DE;}),({struct Cyc_List_List*_tmp3DD=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmp7F);_tmp75->f2=_tmp3DD;});_tmp75;});}_LL0:;}
# 365
void*Cyc_Port_instantiate(void*t){
return({struct Cyc_Dict_Dict*_tmp3E0=({struct Cyc_Dict_Dict*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Dict_Dict _tmp3DF=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp81=_tmp3DF;});_tmp81;});Cyc_Port_inst(_tmp3E0,t);});}
# 372
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*_tmp82=ts;for(0;(unsigned)_tmp82;_tmp82=_tmp82->tl){
void*_tmp83=Cyc_Port_compress_ct((void*)_tmp82->hd);
if(t == _tmp83)found=1;}}
# 378
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(0;ts != 0;ts=ts->tl){
if(({void*_tmp3E1=t;_tmp3E1 == Cyc_Port_compress_ct((void*)ts->hd);}))continue;
res=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->hd=(void*)ts->hd,_tmp84->tl=res;_tmp84;});}
# 384
return res;};}
# 389
void Cyc_Port_generalize(int is_rgn,void*t){
t=Cyc_Port_compress_ct(t);{
void*_tmp85=t;void*_tmp90;struct Cyc_List_List*_tmp8F;void*_tmp8E;void*_tmp8D;void*_tmp8C;void*_tmp8B;void*_tmp8A;void*_tmp89;void*_tmp88;void*_tmp87;struct Cyc_Port_Cvar*_tmp86;switch(*((int*)_tmp85)){case 16U: _LL1: _tmp86=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp85)->f1;_LL2:
# 394
({struct Cyc_List_List*_tmp3E2=Cyc_Port_filter_self(t,_tmp86->uppers);_tmp86->uppers=_tmp3E2;});
({struct Cyc_List_List*_tmp3E3=Cyc_Port_filter_self(t,_tmp86->lowers);_tmp86->lowers=_tmp3E3;});
if(is_rgn){
# 399
if(_tmp86->uppers == 0 && _tmp86->lowers == 0){
({void*_tmp3E4=t;Cyc_Port_unifies(_tmp3E4,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));});
return;}
# 404
if((unsigned)_tmp86->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp86->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 408
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp86->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 412
return;case 0U: _LL3: _LL4:
 goto _LL6;case 1U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 3U: _LL9: _LLA:
 goto _LLC;case 4U: _LLB: _LLC:
 goto _LLE;case 5U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 14U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
 goto _LL18;case 9U: _LL17: _LL18:
 goto _LL1A;case 8U: _LL19: _LL1A:
 goto _LL1C;case 7U: _LL1B: _LL1C:
 return;case 11U: _LL1D: _tmp8B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp8A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f2;_tmp89=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f3;_tmp88=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f4;_tmp87=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f5;_LL1E:
# 429
 Cyc_Port_generalize(0,_tmp8B);Cyc_Port_generalize(1,_tmp88);goto _LL0;case 12U: _LL1F: _tmp8E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp8D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f2;_tmp8C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f3;_LL20:
# 431
 Cyc_Port_generalize(0,_tmp8E);Cyc_Port_generalize(0,_tmp8D);goto _LL0;default: _LL21: _tmp90=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp8F=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp85)->f2;_LL22:
# 433
 Cyc_Port_generalize(0,_tmp90);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmp8F);goto _LL0;}_LL0:;};}
# 439
static int Cyc_Port_occurs(void*v,void*t){
t=Cyc_Port_compress_ct(t);
if(t == v)return 1;{
void*_tmp91=t;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;void*_tmp9B;struct Cyc_List_List*_tmp9A;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;void*_tmp95;void*_tmp94;void*_tmp93;void*_tmp92;switch(*((int*)_tmp91)){case 11U: _LL1: _tmp96=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp95=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f2;_tmp94=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f3;_tmp93=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f4;_tmp92=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f5;_LL2:
# 444
 return(((Cyc_Port_occurs(v,_tmp96)|| Cyc_Port_occurs(v,_tmp95))|| Cyc_Port_occurs(v,_tmp94))|| Cyc_Port_occurs(v,_tmp93))||
 Cyc_Port_occurs(v,_tmp92);case 12U: _LL3: _tmp99=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp98=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f2;_tmp97=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f3;_LL4:
# 447
 return(Cyc_Port_occurs(v,_tmp99)|| Cyc_Port_occurs(v,_tmp98))|| Cyc_Port_occurs(v,_tmp97);case 15U: _LL5: _tmp9B=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp9A=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp91)->f2;_LL6:
# 449
 if(Cyc_Port_occurs(v,_tmp9B))return 1;
for(0;(unsigned)_tmp9A;_tmp9A=_tmp9A->tl){
if(Cyc_Port_occurs(v,(void*)_tmp9A->hd))return 1;}
return 0;case 13U: _LL7: _tmp9C=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp91)->f1)->f2;_LL8:
 return 0;case 14U: _LL9: _tmp9D=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp91)->f1;_LLA:
# 455
 for(0;(unsigned)_tmp9D;_tmp9D=_tmp9D->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmp9D->hd)->qual)|| Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmp9D->hd)->type))return 1;}
return 0;default: _LLB: _LLC:
 return 0;}_LL0:;};}char Cyc_Port_Unify_ct[9U]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 467
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 469
static int Cyc_Port_leq(void*t1,void*t2);
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);
# 473
static void Cyc_Port_unify_ct(void*t1,void*t2){
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);
if(t1 == t2)return;{
struct _tuple11 _tmp9E=({struct _tuple11 _tmp39D;_tmp39D.f1=t1,_tmp39D.f2=t2;_tmp39D;});struct _tuple11 _tmp9F=_tmp9E;struct Cyc_List_List*_tmpCC;void***_tmpCB;struct Cyc_Absyn_Aggrdecl*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpC8;void***_tmpC7;struct Cyc_List_List*_tmpC6;void***_tmpC5;struct Cyc_Absyn_Aggrdecl*_tmpC4;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC2;void***_tmpC1;struct _tuple10*_tmpC0;struct _tuple10*_tmpBF;void*_tmpBE;struct Cyc_List_List*_tmpBD;void*_tmpBC;struct Cyc_List_List*_tmpBB;void*_tmpBA;void*_tmpB9;void*_tmpB8;void*_tmpB7;void*_tmpB6;void*_tmpB5;struct _fat_ptr _tmpB4;struct _fat_ptr _tmpB3;void*_tmpB2;void*_tmpB1;void*_tmpB0;void*_tmpAF;void*_tmpAE;void*_tmpAD;void*_tmpAC;void*_tmpAB;void*_tmpAA;void*_tmpA9;struct Cyc_Port_Cvar*_tmpA8;struct Cyc_Port_Cvar*_tmpA7;if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f1)->tag == 16U){_LL1: _tmpA7=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_LL2:
# 479
 if(!Cyc_Port_occurs(t1,t2)){
# 482
{struct Cyc_List_List*_tmpA0=Cyc_Port_filter_self(t1,_tmpA7->uppers);for(0;(unsigned)_tmpA0;_tmpA0=_tmpA0->tl){
if(!Cyc_Port_leq(t2,(void*)_tmpA0->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmpA1=Cyc_Port_filter_self(t1,_tmpA7->lowers);for(0;(unsigned)_tmpA1;_tmpA1=_tmpA1->tl){
if(!Cyc_Port_leq((void*)_tmpA1->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
({void**_tmp3E5=({void**_tmpA2=_cycalloc(sizeof(*_tmpA2));*_tmpA2=t2;_tmpA2;});_tmpA7->eq=_tmp3E5;});
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}else{if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 16U){_LL3: _tmpA8=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LL4:
 Cyc_Port_unify_ct(t2,t1);return;}else{switch(*((int*)_tmp9F.f1)){case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 11U){_LL5: _tmpB2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpB0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f3;_tmpAF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f4;_tmpAE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f5;_tmpAD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpAC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_tmpAB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f3;_tmpAA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f4;_tmpA9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f5;_LL6:
# 491
 Cyc_Port_unify_ct(_tmpB2,_tmpAD);Cyc_Port_unify_ct(_tmpB1,_tmpAC);Cyc_Port_unify_ct(_tmpB0,_tmpAB);Cyc_Port_unify_ct(_tmpAF,_tmpAA);
Cyc_Port_unify_ct(_tmpAE,_tmpA9);
return;}else{goto _LL15;}case 10U: if(((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 10U){_LL7: _tmpB4=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB3=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LL8:
# 495
 if(Cyc_strcmp((struct _fat_ptr)_tmpB4,(struct _fat_ptr)_tmpB3)!= 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;}else{goto _LL15;}case 12U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 12U){_LL9: _tmpBA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpB8=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f3;_tmpB7=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpB6=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_tmpB5=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f3;_LLA:
# 498
 Cyc_Port_unify_ct(_tmpBA,_tmpB7);Cyc_Port_unify_ct(_tmpB9,_tmpB6);Cyc_Port_unify_ct(_tmpB8,_tmpB5);return;}else{goto _LL15;}case 15U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 15U){_LLB: _tmpBE=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpBD=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpBC=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpBB=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LLC:
# 500
 Cyc_Port_unify_ct(_tmpBE,_tmpBC);Cyc_Port_unify_cts(_tmpBD,_tmpBB);return;}else{goto _LL15;}case 13U: switch(*((int*)_tmp9F.f2)){case 13U: _LLD: _tmpC0=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpBF=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LLE:
# 502
 if(_tmpC0 == _tmpBF)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}case 14U: _LL13: _tmpC4=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1)->f1;_tmpC3=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1)->f2;_tmpC2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpC1=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LL14:
# 512
 Cyc_Port_merge_fields(_tmpC3,_tmpC2,0);
({void**_tmp3E6=({void**_tmpA6=_cycalloc(sizeof(*_tmpA6));*_tmpA6=t1;_tmpA6;});*_tmpC1=_tmp3E6;});
return;default: goto _LL15;}case 14U: switch(*((int*)_tmp9F.f2)){case 14U: _LLF: _tmpC8=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpC7=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpC6=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpC5=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LL10: {
# 504
void*_tmpA3=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmpC8,_tmpC6,1));
({void**_tmp3E8=({void**_tmp3E7=({void**_tmpA4=_cycalloc(sizeof(*_tmpA4));*_tmpA4=_tmpA3;_tmpA4;});*_tmpC5=_tmp3E7;});*_tmpC7=_tmp3E8;});
return;}case 13U: _LL11: _tmpCC=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpCB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpCA=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1)->f1;_tmpC9=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1)->f2;_LL12:
# 508
 Cyc_Port_merge_fields(_tmpC9,_tmpCC,0);
({void**_tmp3E9=({void**_tmpA5=_cycalloc(sizeof(*_tmpA5));*_tmpA5=t2;_tmpA5;});*_tmpCB=_tmp3E9;});
return;default: goto _LL15;}default: _LL15: _LL16:
# 515
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}_LL0:;};}
# 519
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0 && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 523
if(t1 != 0 || t2 != 0)
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}
# 529
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){
# 531
struct Cyc_List_List*common=0;
{struct Cyc_List_List*_tmpCD=fs2;for(0;(unsigned)_tmpCD;_tmpCD=_tmpCD->tl){
struct Cyc_Port_Cfield*_tmpCE=(struct Cyc_Port_Cfield*)_tmpCD->hd;
int found=0;
{struct Cyc_List_List*_tmpCF=fs1;for(0;(unsigned)_tmpCF;_tmpCF=_tmpCF->tl){
struct Cyc_Port_Cfield*_tmpD0=(struct Cyc_Port_Cfield*)_tmpCF->hd;
if(Cyc_strptrcmp(_tmpD0->f,_tmpCE->f)== 0){
common=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=_tmpD0,_tmpD1->tl=common;_tmpD1;});
Cyc_Port_unify_ct(_tmpD0->qual,_tmpCE->qual);
Cyc_Port_unify_ct(_tmpD0->type,_tmpCE->type);
found=1;
break;}}}
# 545
if(!found){
if(allow_f1_subset_f2)
common=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=_tmpCE,_tmpD2->tl=common;_tmpD2;});else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 551
{struct Cyc_List_List*_tmpD3=fs1;for(0;(unsigned)_tmpD3;_tmpD3=_tmpD3->tl){
struct Cyc_Port_Cfield*_tmpD4=(struct Cyc_Port_Cfield*)_tmpD3->hd;
int found=0;
{struct Cyc_List_List*_tmpD5=fs2;for(0;(unsigned)_tmpD5;_tmpD5=_tmpD5->tl){
struct Cyc_Port_Cfield*_tmpD6=(struct Cyc_Port_Cfield*)_tmpD5->hd;
if(Cyc_strptrcmp(_tmpD4->f,_tmpD6->f))found=1;}}
# 558
if(!found)
common=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=_tmpD4,_tmpD7->tl=common;_tmpD7;});}}
# 561
return common;}
# 564
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int _tmpDA=0;if(setjmp(_tmpD8.handler))_tmpDA=1;if(!_tmpDA){
# 571
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmpD9=(void*)Cyc_Core_get_exn_thrown();void*_tmpDB=_tmpD9;void*_tmpDC;if(((struct Cyc_Port_Unify_ct_exn_struct*)_tmpDB)->tag == Cyc_Port_Unify_ct){_LL1: _LL2:
# 578
 return 0;}else{_LL3: _tmpDC=_tmpDB;_LL4:(int)_rethrow(_tmpDC);}_LL0:;}};}
# 580
return 1;}struct _tuple12{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 586
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){
# 588
t=Cyc_Port_compress_ct(t);
{void*_tmpDD=t;switch(*((int*)_tmpDD)){case 1U: _LL1: _LL2:
# 592
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 3U: _LL9: _LLA:
 goto _LLC;case 12U: _LLB: _LLC:
 goto _LLE;case 13U: _LLD: _LLE:
 goto _LL10;case 15U: _LLF: _LL10:
 goto _LL12;case 7U: _LL11: _LL12:
# 603
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;case 4U: _LL13: _LL14:
# 608
 goto _LL16;case 0U: _LL15: _LL16:
 goto _LL18;case 9U: _LL17: _LL18:
# 611
 return*uppers;default: _LL19: _LL1A:
 goto _LL0;}_LL0:;}
# 615
{struct Cyc_List_List*_tmpDE=*uppers;for(0;(unsigned)_tmpDE;_tmpDE=_tmpDE->tl){
void*_tmpDF=Cyc_Port_compress_ct((void*)_tmpDE->hd);
# 618
if(t == _tmpDF)return*uppers;{
struct _tuple11 _tmpE0=({struct _tuple11 _tmp39F;_tmp39F.f1=t,_tmp39F.f2=_tmpDF;_tmp39F;});struct _tuple11 _tmpE1=_tmpE0;void*_tmpF4;void*_tmpF3;void*_tmpF2;void*_tmpF1;void*_tmpF0;void*_tmpEF;void*_tmpEE;void*_tmpED;void*_tmpEC;void*_tmpEB;switch(*((int*)_tmpE1.f1)){case 6U: switch(*((int*)_tmpE1.f2)){case 11U: _LL1C: _LL1D:
# 623
 goto _LL1F;case 5U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
# 626
 return*uppers;default: goto _LL24;}case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->tag == 11U){_LL22: _tmpF4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f1;_tmpF3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f2;_tmpF2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f3;_tmpF1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f4;_tmpF0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f5;_tmpEF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f1;_tmpEE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f2;_tmpED=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f3;_tmpEC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f4;_tmpEB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f5;_LL23: {
# 631
struct _tuple12 _tmpE2=({struct _tuple12 _tmp39E;({void*_tmp3EE=Cyc_Port_var();_tmp39E.f1=_tmp3EE;}),({void*_tmp3ED=Cyc_Port_var();_tmp39E.f2=_tmp3ED;}),({void*_tmp3EC=Cyc_Port_var();_tmp39E.f3=_tmp3EC;}),({void*_tmp3EB=Cyc_Port_var();_tmp39E.f4=_tmp3EB;}),({void*_tmp3EA=Cyc_Port_var();_tmp39E.f5=_tmp3EA;});_tmp39E;});struct _tuple12 _tmpE3=_tmpE2;void*_tmpEA;void*_tmpE9;void*_tmpE8;void*_tmpE7;void*_tmpE6;_LL27: _tmpEA=_tmpE3.f1;_tmpE9=_tmpE3.f2;_tmpE8=_tmpE3.f3;_tmpE7=_tmpE3.f4;_tmpE6=_tmpE3.f5;_LL28:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpE4=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->tag=11U,_tmpE5->f1=_tmpEA,_tmpE5->f2=_tmpE9,_tmpE5->f3=_tmpE8,_tmpE5->f4=_tmpE7,_tmpE5->f5=_tmpE6;_tmpE5;});
Cyc_Port_leq(_tmpEA,_tmpF4);Cyc_Port_leq(_tmpEA,_tmpEF);
Cyc_Port_leq(_tmpE9,_tmpF3);Cyc_Port_leq(_tmpE9,_tmpEE);
Cyc_Port_leq(_tmpE8,_tmpF2);Cyc_Port_leq(_tmpE8,_tmpEE);
Cyc_Port_leq(_tmpE7,_tmpF1);Cyc_Port_leq(_tmpE7,_tmpEC);
Cyc_Port_leq(_tmpE6,_tmpF0);Cyc_Port_leq(_tmpE6,_tmpEB);
_tmpDE->hd=(void*)((void*)_tmpE4);
return*uppers;};}}else{goto _LL24;}default: _LL24: _LL25:
 goto _LL1B;}_LL1B:;};}}
# 643
return({struct Cyc_List_List*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=t,_tmpF5->tl=*uppers;_tmpF5;});}
# 648
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){
# 650
t=Cyc_Port_compress_ct(t);
{void*_tmpF6=t;switch(*((int*)_tmpF6)){case 0U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 13U: _LLB: _LLC:
 goto _LLE;case 15U: _LLD: _LLE:
 goto _LL10;case 10U: _LLF: _LL10:
# 660
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;case 7U: _LL11: _LL12:
 goto _LL14;case 1U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
# 666
 return*lowers;default: _LL17: _LL18:
# 668
 goto _LL0;}_LL0:;}
# 670
{struct Cyc_List_List*_tmpF7=*lowers;for(0;(unsigned)_tmpF7;_tmpF7=_tmpF7->tl){
void*_tmpF8=Cyc_Port_compress_ct((void*)_tmpF7->hd);
if(t == _tmpF8)return*lowers;{
struct _tuple11 _tmpF9=({struct _tuple11 _tmp3A1;_tmp3A1.f1=t,_tmp3A1.f2=_tmpF8;_tmp3A1;});struct _tuple11 _tmpFA=_tmpF9;void*_tmp10D;void*_tmp10C;void*_tmp10B;void*_tmp10A;void*_tmp109;void*_tmp108;void*_tmp107;void*_tmp106;void*_tmp105;void*_tmp104;if(((struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmpFA.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{switch(*((int*)_tmpFA.f1)){case 5U: switch(*((int*)_tmpFA.f2)){case 6U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;default: goto _LL26;}case 11U: switch(*((int*)_tmpFA.f2)){case 6U: _LL20: _LL21:
 goto _LL23;case 11U: _LL24: _tmp10D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f1;_tmp10C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f2;_tmp10B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f3;_tmp10A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f4;_tmp109=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f5;_tmp108=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f1;_tmp107=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f2;_tmp106=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f3;_tmp105=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f4;_tmp104=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f5;_LL25: {
# 684
struct _tuple12 _tmpFB=({struct _tuple12 _tmp3A0;({void*_tmp3F3=Cyc_Port_var();_tmp3A0.f1=_tmp3F3;}),({void*_tmp3F2=Cyc_Port_var();_tmp3A0.f2=_tmp3F2;}),({void*_tmp3F1=Cyc_Port_var();_tmp3A0.f3=_tmp3F1;}),({void*_tmp3F0=Cyc_Port_var();_tmp3A0.f4=_tmp3F0;}),({void*_tmp3EF=Cyc_Port_var();_tmp3A0.f5=_tmp3EF;});_tmp3A0;});struct _tuple12 _tmpFC=_tmpFB;void*_tmp103;void*_tmp102;void*_tmp101;void*_tmp100;void*_tmpFF;_LL29: _tmp103=_tmpFC.f1;_tmp102=_tmpFC.f2;_tmp101=_tmpFC.f3;_tmp100=_tmpFC.f4;_tmpFF=_tmpFC.f5;_LL2A:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFD=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=11U,_tmpFE->f1=_tmp103,_tmpFE->f2=_tmp102,_tmpFE->f3=_tmp101,_tmpFE->f4=_tmp100,_tmpFE->f5=_tmpFF;_tmpFE;});
Cyc_Port_leq(_tmp10D,_tmp103);Cyc_Port_leq(_tmp108,_tmp103);
Cyc_Port_leq(_tmp10C,_tmp102);Cyc_Port_leq(_tmp107,_tmp102);
Cyc_Port_leq(_tmp10B,_tmp101);Cyc_Port_leq(_tmp107,_tmp101);
Cyc_Port_leq(_tmp10A,_tmp100);Cyc_Port_leq(_tmp105,_tmp100);
Cyc_Port_leq(_tmp109,_tmpFF);Cyc_Port_leq(_tmp104,_tmpFF);
_tmpF7->hd=(void*)((void*)_tmpFD);
return*lowers;};}default: goto _LL26;}case 12U: if(((struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmpFA.f2)->tag == 6U){_LL22: _LL23:
# 679
 return*lowers;}else{goto _LL26;}default: _LL26: _LL27:
# 693
 goto _LL19;}}_LL19:;};}}
# 696
return({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=t,_tmp10E->tl=*lowers;_tmp10E;});}
# 703
static int Cyc_Port_leq(void*t1,void*t2){
# 709
if(t1 == t2)return 1;
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{
struct _tuple11 _tmp10F=({struct _tuple11 _tmp3A2;_tmp3A2.f1=t1,_tmp3A2.f2=t2;_tmp3A2;});struct _tuple11 _tmp110=_tmp10F;struct Cyc_Port_Cvar*_tmp12E;void*_tmp12D;void*_tmp12C;void*_tmp12B;void*_tmp12A;void*_tmp129;void*_tmp128;void*_tmp127;void*_tmp126;void*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;void*_tmp11F;void*_tmp11E;void*_tmp11D;void*_tmp11C;void*_tmp11B;void*_tmp11A;void*_tmp119;void*_tmp118;void*_tmp117;struct Cyc_Port_Cvar*_tmp116;struct Cyc_Port_Cvar*_tmp115;struct Cyc_Port_Cvar*_tmp114;struct _fat_ptr _tmp113;struct _fat_ptr _tmp112;struct _fat_ptr _tmp111;switch(*((int*)_tmp110.f1)){case 7U: _LL1: _LL2:
 return 1;case 10U: switch(*((int*)_tmp110.f2)){case 10U: _LL3: _tmp112=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp111=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL4:
 return Cyc_strcmp((struct _fat_ptr)_tmp112,(struct _fat_ptr)_tmp111)== 0;case 7U: _LL5: _tmp113=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_LL6:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 1U: switch(*((int*)_tmp110.f2)){case 0U: _LL7: _LL8:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 0U: switch(*((int*)_tmp110.f2)){case 1U: _LL9: _LLA:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 9U: switch(*((int*)_tmp110.f2)){case 8U: _LLB: _LLC:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 8U: switch(*((int*)_tmp110.f2)){case 9U: _LLD: _LLE:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 16U: switch(*((int*)_tmp110.f2)){case 0U: _LLF: _LL10:
 return 1;case 4U: _LL11: _LL12:
 return 1;case 16U: _LL29: _tmp115=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp114=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL2A:
# 739
({struct Cyc_List_List*_tmp3F4=Cyc_Port_filter_self(t1,_tmp115->uppers);_tmp115->uppers=_tmp3F4;});
({struct Cyc_List_List*_tmp3F5=Cyc_Port_filter_self(t2,_tmp114->lowers);_tmp114->lowers=_tmp3F5;});
({struct Cyc_List_List*_tmp3F6=Cyc_Port_insert_upper(t1,t2,& _tmp115->uppers);_tmp115->uppers=_tmp3F6;});
({struct Cyc_List_List*_tmp3F7=Cyc_Port_insert_lower(t2,t1,& _tmp114->lowers);_tmp114->lowers=_tmp3F7;});
return 1;default: _LL2B: _tmp116=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_LL2C:
# 745
({struct Cyc_List_List*_tmp3F8=Cyc_Port_filter_self(t1,_tmp116->uppers);_tmp116->uppers=_tmp3F8;});
({struct Cyc_List_List*_tmp3F9=Cyc_Port_insert_upper(t1,t2,& _tmp116->uppers);_tmp116->uppers=_tmp3F9;});
return 1;}case 4U: _LL13: _LL14:
# 722
 return 0;case 5U: switch(*((int*)_tmp110.f2)){case 6U: _LL15: _LL16:
 return 1;case 11U: _LL17: _LL18:
 return 1;case 4U: _LL19: _LL1A:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 11U: switch(*((int*)_tmp110.f2)){case 6U: _LL1B: _LL1C:
 return 1;case 4U: _LL1D: _LL1E:
 return 1;case 11U: _LL23: _tmp120=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp11F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp11E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp11D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f4;_tmp11C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f5;_tmp11B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp11A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp119=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_tmp118=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f4;_tmp117=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f5;_LL24:
# 731
 return(((Cyc_Port_leq(_tmp120,_tmp11B)&& Cyc_Port_leq(_tmp11F,_tmp11A))&& Cyc_Port_unifies(_tmp11E,_tmp119))&& Cyc_Port_leq(_tmp11D,_tmp118))&&
 Cyc_Port_leq(_tmp11C,_tmp117);case 16U: goto _LL2D;default: goto _LL2F;}case 12U: switch(*((int*)_tmp110.f2)){case 6U: _LL1F: _LL20:
# 728
 return 1;case 4U: _LL21: _LL22:
 return 1;case 12U: _LL25: _tmp126=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp125=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp124=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp123=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp122=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp121=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_LL26:
# 734
 return(Cyc_Port_leq(_tmp126,_tmp123)&& Cyc_Port_leq(_tmp125,_tmp122))&& Cyc_Port_leq(_tmp124,_tmp121);case 11U: _LL27: _tmp12D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp12C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp12B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp12A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp129=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp128=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_tmp127=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f5;_LL28:
# 736
 return((Cyc_Port_leq(_tmp12D,_tmp12A)&& Cyc_Port_leq(_tmp12C,_tmp129))&& Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,_tmp128))&&
 Cyc_Port_leq(_tmp12B,_tmp127);case 16U: goto _LL2D;default: goto _LL2F;}default: if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->tag == 16U){_LL2D: _tmp12E=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL2E:
# 749
({struct Cyc_List_List*_tmp3FA=Cyc_Port_filter_self(t2,_tmp12E->lowers);_tmp12E->lowers=_tmp3FA;});
({struct Cyc_List_List*_tmp3FB=Cyc_Port_insert_lower(t2,t1,& _tmp12E->lowers);_tmp12E->lowers=_tmp3FB;});
return 1;}else{_LL2F: _LL30:
 return Cyc_Port_unifies(t1,t2);}}_LL0:;};}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 783
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos =0U,Cyc_Port_FnArg_pos =1U,Cyc_Port_FnBody_pos =2U,Cyc_Port_Toplevel_pos =3U};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 796
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmp130=_cycalloc(sizeof(*_tmp130));({struct Cyc_Dict_Dict _tmp400=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->typedef_dict=_tmp400;}),({
struct Cyc_Dict_Dict _tmp3FF=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->struct_dict=_tmp3FF;}),({
struct Cyc_Dict_Dict _tmp3FE=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->union_dict=_tmp3FE;}),_tmp130->qualifier_edits=0,_tmp130->pointer_edits=0,_tmp130->zeroterm_edits=0,_tmp130->vardecl_locs=0,({
# 804
struct Cyc_Hashtable_Table*_tmp3FD=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(unsigned,unsigned),int(*hash)(unsigned)))Cyc_Hashtable_create)(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);_tmp130->varusage_tab=_tmp3FD;}),_tmp130->edits=0,_tmp130->porting=0,({
# 807
void*_tmp3FC=Cyc_Port_void_ct();_tmp130->return_type=_tmp3FC;});_tmp130;});
return({struct Cyc_Port_Cenv*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->gcenv=g,_tmp12F->cpos=Cyc_Port_Toplevel_pos,({
# 810
struct Cyc_Dict_Dict _tmp401=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp12F->var_dict=_tmp401;});_tmp12F;});}
# 816
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
return(int)env->cpos == (int)Cyc_Port_FnArg_pos;}
# 819
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return(int)env->cpos == (int)Cyc_Port_FnRes_pos;}
# 822
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
return(int)env->cpos == (int)Cyc_Port_Toplevel_pos;}
# 825
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
return(env->gcenv)->return_type;}
# 828
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp131;_push_handler(& _tmp131);{int _tmp133=0;if(setjmp(_tmp131.handler))_tmp133=1;if(!_tmp133){
{struct _tuple11 _tmp134=*((struct _tuple11*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple11 _tmp135=_tmp134;void*_tmp137;_LL1: _tmp137=_tmp135.f1;_LL2:;{
void*_tmp136=_tmp137;_npop_handler(0U);return _tmp136;};}
# 830
;_pop_handler();}else{void*_tmp132=(void*)Cyc_Core_get_exn_thrown();void*_tmp138=_tmp132;void*_tmp139;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp138)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 837
 return Cyc_Absyn_sint_type;}else{_LL6: _tmp139=_tmp138;_LL7:(int)_rethrow(_tmp139);}_LL3:;}};}
# 842
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp13A;_push_handler(& _tmp13A);{int _tmp13C=0;if(setjmp(_tmp13A.handler))_tmp13C=1;if(!_tmp13C){
{struct _tuple11 _tmp13D=*((struct _tuple11*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple11 _tmp13E=_tmp13D;void*_tmp140;_LL1: _tmp140=_tmp13E.f2;_LL2:;{
void*_tmp13F=_tmp140;_npop_handler(0U);return _tmp13F;};}
# 844
;_pop_handler();}else{void*_tmp13B=(void*)Cyc_Core_get_exn_thrown();void*_tmp141=_tmp13B;void*_tmp142;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp141)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 851
 return Cyc_Port_var();}else{_LL6: _tmp142=_tmp141;_LL7:(int)_rethrow(_tmp142);}_LL3:;}};}
# 857
static struct _tuple10*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 859
struct _tuple10**_tmp143=((struct _tuple10**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);
if((unsigned)_tmp143)
return*_tmp143;else{
# 863
struct Cyc_Absyn_Aggrdecl*_tmp144=({struct Cyc_Absyn_Aggrdecl*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->kind=Cyc_Absyn_StructA,_tmp146->sc=Cyc_Absyn_Public,_tmp146->name=n,_tmp146->tvs=0,_tmp146->impl=0,_tmp146->attributes=0,_tmp146->expected_mem_kind=0;_tmp146;});
# 866
struct _tuple10*p=({struct _tuple10*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=_tmp144,_tmp145->f2=0;_tmp145;});
({struct Cyc_Dict_Dict _tmp402=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);(env->gcenv)->struct_dict=_tmp402;});
return p;}}
# 874
static struct _tuple10*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 876
struct _tuple10**_tmp147=((struct _tuple10**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);
if((unsigned)_tmp147)
return*_tmp147;else{
# 880
struct Cyc_Absyn_Aggrdecl*_tmp148=({struct Cyc_Absyn_Aggrdecl*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->kind=Cyc_Absyn_UnionA,_tmp14A->sc=Cyc_Absyn_Public,_tmp14A->name=n,_tmp14A->tvs=0,_tmp14A->impl=0,_tmp14A->attributes=0,_tmp14A->expected_mem_kind=0;_tmp14A;});
# 883
struct _tuple10*p=({struct _tuple10*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->f1=_tmp148,_tmp149->f2=0;_tmp149;});
({struct Cyc_Dict_Dict _tmp403=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);(env->gcenv)->union_dict=_tmp403;});
return p;}}struct _tuple13{struct _tuple11 f1;unsigned f2;};struct _tuple14{void*f1;struct _tuple11*f2;unsigned f3;};
# 890
static struct _tuple13 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp14B;_push_handler(& _tmp14B);{int _tmp14D=0;if(setjmp(_tmp14B.handler))_tmp14D=1;if(!_tmp14D){
{struct _tuple14 _tmp14E=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp14F=_tmp14E;struct _tuple11*_tmp152;unsigned _tmp151;_LL1: _tmp152=_tmp14F.f2;_tmp151=_tmp14F.f3;_LL2:;{
struct _tuple13 _tmp150=({struct _tuple13 _tmp3A3;_tmp3A3.f1=*_tmp152,_tmp3A3.f2=_tmp151;_tmp3A3;});_npop_handler(0U);return _tmp150;};}
# 892
;_pop_handler();}else{void*_tmp14C=(void*)Cyc_Core_get_exn_thrown();void*_tmp153=_tmp14C;void*_tmp154;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp153)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 899
 return({struct _tuple13 _tmp3A4;({void*_tmp405=Cyc_Port_var();(_tmp3A4.f1).f1=_tmp405;}),({void*_tmp404=Cyc_Port_var();(_tmp3A4.f1).f2=_tmp404;}),_tmp3A4.f2=0U;_tmp3A4;});}else{_LL6: _tmp154=_tmp153;_LL7:(int)_rethrow(_tmp154);}_LL3:;}};}
# 902
static struct _tuple14*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}
# 906
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(env->var_dict,x);}
# 910
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=1;if(!_tmp157){
{struct _tuple14 _tmp158=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp159=_tmp158;void*_tmp15E;_LL1: _tmp15E=_tmp159.f1;_LL2:;
LOOP: {void*_tmp15A=_tmp15E;struct _tuple0*_tmp15D;switch(*((int*)_tmp15A)){case 8U: _LL4: _tmp15D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15A)->f1;_LL5:
 _tmp15E=Cyc_Port_lookup_typedef(env,_tmp15D);goto LOOP;case 5U: _LL6: _LL7: {
int _tmp15B=1;_npop_handler(0U);return _tmp15B;}default: _LL8: _LL9: {
int _tmp15C=0;_npop_handler(0U);return _tmp15C;}}_LL3:;}}
# 912
;_pop_handler();}else{void*_tmp156=(void*)Cyc_Core_get_exn_thrown();void*_tmp15F=_tmp156;void*_tmp160;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15F)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 923
 return 0;}else{_LLD: _tmp160=_tmp15F;_LLE:(int)_rethrow(_tmp160);}_LLA:;}};}
# 927
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp161;_push_handler(& _tmp161);{int _tmp163=0;if(setjmp(_tmp161.handler))_tmp163=1;if(!_tmp163){
{struct _tuple14 _tmp164=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp165=_tmp164;void*_tmp16A;_LL1: _tmp16A=_tmp165.f1;_LL2:;
LOOP: {void*_tmp166=_tmp16A;struct _tuple0*_tmp169;switch(*((int*)_tmp166)){case 8U: _LL4: _tmp169=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp166)->f1;_LL5:
 _tmp16A=Cyc_Port_lookup_typedef(env,_tmp169);goto LOOP;case 4U: _LL6: _LL7: {
int _tmp167=1;_npop_handler(0U);return _tmp167;}default: _LL8: _LL9: {
int _tmp168=0;_npop_handler(0U);return _tmp168;}}_LL3:;}}
# 929
;_pop_handler();}else{void*_tmp162=(void*)Cyc_Core_get_exn_thrown();void*_tmp16B=_tmp162;void*_tmp16C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16B)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 940
 return 0;}else{_LLD: _tmp16C=_tmp16B;_LLE:(int)_rethrow(_tmp16C);}_LLA:;}};}
# 946
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){
return({struct Cyc_Port_Cenv*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->gcenv=env->gcenv,_tmp16D->var_dict=env->var_dict,_tmp16D->cpos=cpos;_tmp16D;});}
# 951
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
(env->gcenv)->return_type=t;}
# 956
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype,unsigned loc){
# 958
return({struct Cyc_Port_Cenv*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->gcenv=env->gcenv,({
struct Cyc_Dict_Dict _tmp409=({struct Cyc_Dict_Dict _tmp408=env->var_dict;struct _tuple0*_tmp407=x;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp408,_tmp407,({struct _tuple14*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->f1=t,({struct _tuple11*_tmp406=({struct _tuple11*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->f1=qual,_tmp16E->f2=ctype;_tmp16E;});_tmp16F->f2=_tmp406;}),_tmp16F->f3=loc;_tmp16F;}));});_tmp170->var_dict=_tmp409;}),_tmp170->cpos=env->cpos;_tmp170;});}
# 964
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
({struct Cyc_Dict_Dict _tmp40C=({struct Cyc_Dict_Dict _tmp40B=(env->gcenv)->typedef_dict;struct _tuple0*_tmp40A=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)(_tmp40B,_tmp40A,({struct _tuple11*_tmp171=_cycalloc(sizeof(*_tmp171));
_tmp171->f1=t,_tmp171->f2=ct;_tmp171;}));});
# 965
(env->gcenv)->typedef_dict=_tmp40C;});}struct _tuple15{struct _tuple0*f1;unsigned f2;void*f3;};
# 968
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned loc,void*type,struct Cyc_Absyn_Exp*init){
({struct Cyc_List_List*_tmp40E=({struct Cyc_List_List*_tmp173=_cycalloc(sizeof(*_tmp173));({struct _tuple15*_tmp40D=({struct _tuple15*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->f1=x,_tmp172->f2=loc,_tmp172->f3=type;_tmp172;});_tmp173->hd=_tmp40D;}),_tmp173->tl=(env->gcenv)->vardecl_locs;_tmp173;});(env->gcenv)->vardecl_locs=_tmp40E;});
({struct Cyc_Hashtable_Table*_tmp410=(env->gcenv)->varusage_tab;unsigned _tmp40F=loc;((void(*)(struct Cyc_Hashtable_Table*t,unsigned key,struct Cyc_Port_VarUsage*val))Cyc_Hashtable_insert)(_tmp410,_tmp40F,({struct Cyc_Port_VarUsage*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->address_taken=0,_tmp174->init=init,_tmp174->usage_locs=0;_tmp174;}));});}
# 972
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned declloc,unsigned useloc,int takeaddress){
struct Cyc_Port_VarUsage*varusage=0;
if(((int(*)(struct Cyc_Hashtable_Table*t,unsigned key,struct Cyc_Port_VarUsage**data))Cyc_Hashtable_try_lookup)((env->gcenv)->varusage_tab,declloc,& varusage)){
if(takeaddress)((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken=1;{
struct Cyc_List_List*_tmp175=((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs;
({struct Cyc_List_List*_tmp411=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=(void*)useloc,_tmp176->tl=_tmp175;_tmp176;});((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs=_tmp411;});};}}struct _tuple16{void*f1;void*f2;unsigned f3;};
# 983
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned loc){
# 985
({struct Cyc_List_List*_tmp413=({struct Cyc_List_List*_tmp178=_cycalloc(sizeof(*_tmp178));({struct _tuple16*_tmp412=({struct _tuple16*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=new_qual,_tmp177->f2=orig_qual,_tmp177->f3=loc;_tmp177;});_tmp178->hd=_tmp412;}),_tmp178->tl=(env->gcenv)->qualifier_edits;_tmp178;});(env->gcenv)->qualifier_edits=_tmp413;});}
# 991
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned loc){
# 993
({struct Cyc_List_List*_tmp415=({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));({struct _tuple16*_tmp414=({struct _tuple16*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->f1=new_ptr,_tmp179->f2=orig_ptr,_tmp179->f3=loc;_tmp179;});_tmp17A->hd=_tmp414;}),_tmp17A->tl=(env->gcenv)->pointer_edits;_tmp17A;});(env->gcenv)->pointer_edits=_tmp415;});}
# 997
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned loc){
# 999
({struct Cyc_List_List*_tmp417=({struct Cyc_List_List*_tmp17C=_cycalloc(sizeof(*_tmp17C));({struct _tuple16*_tmp416=({struct _tuple16*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=new_zt,_tmp17B->f2=orig_zt,_tmp17B->f3=loc;_tmp17B;});_tmp17C->hd=_tmp416;}),_tmp17C->tl=(env->gcenv)->zeroterm_edits;_tmp17C;});(env->gcenv)->zeroterm_edits=_tmp417;});}
# 1005
static void*Cyc_Port_main_type(){
struct _tuple8*arg1=({struct _tuple8*_tmp180=_cycalloc(sizeof(*_tmp180));
_tmp180->f1=0,({struct Cyc_Absyn_Tqual _tmp418=Cyc_Absyn_empty_tqual(0U);_tmp180->f2=_tmp418;}),_tmp180->f3=Cyc_Absyn_sint_type;_tmp180;});
struct _tuple8*arg2=({struct _tuple8*_tmp17F=_cycalloc(sizeof(*_tmp17F));
_tmp17F->f1=0,({struct Cyc_Absyn_Tqual _tmp41D=Cyc_Absyn_empty_tqual(0U);_tmp17F->f2=_tmp41D;}),({
void*_tmp41C=({void*_tmp41B=Cyc_Absyn_string_type(Cyc_Absyn_wildtyp(0));void*_tmp41A=Cyc_Absyn_wildtyp(0);struct Cyc_Absyn_Tqual _tmp419=
Cyc_Absyn_empty_tqual(0U);
# 1010
Cyc_Absyn_fatptr_type(_tmp41B,_tmp41A,_tmp419,
Cyc_Tcutil_any_bool(0));});
# 1010
_tmp17F->f3=_tmp41C;});_tmp17F;});
# 1012
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->tag=5U,(_tmp17E->f1).tvars=0,(_tmp17E->f1).effect=0,({
struct Cyc_Absyn_Tqual _tmp41F=Cyc_Absyn_empty_tqual(0U);(_tmp17E->f1).ret_tqual=_tmp41F;}),(_tmp17E->f1).ret_type=Cyc_Absyn_sint_type,({
# 1015
struct Cyc_List_List*_tmp41E=({struct _tuple8*_tmp17D[2U];_tmp17D[0]=arg1,_tmp17D[1]=arg2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17D,sizeof(struct _tuple8*),2U));});(_tmp17E->f1).args=_tmp41E;}),(_tmp17E->f1).c_varargs=0,(_tmp17E->f1).cyc_varargs=0,(_tmp17E->f1).rgn_po=0,(_tmp17E->f1).attributes=0,(_tmp17E->f1).requires_clause=0,(_tmp17E->f1).requires_relns=0,(_tmp17E->f1).ensures_clause=0,(_tmp17E->f1).ensures_relns=0;_tmp17E;});}
# 1024
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);
# 1027
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_Cenv*_tmp181=Cyc_Port_empty_cenv();
# 1030
_tmp181=({struct Cyc_Port_Cenv*_tmp426=_tmp181;struct _tuple0*_tmp425=({struct _tuple0*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp421=({struct _fat_ptr*_tmp183=_cycalloc(sizeof(*_tmp183));({struct _fat_ptr _tmp420=({const char*_tmp182="main";_tag_fat(_tmp182,sizeof(char),5U);});*_tmp183=_tmp420;});_tmp183;});_tmp184->f2=_tmp421;});_tmp184;});void*_tmp424=Cyc_Port_main_type();void*_tmp423=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp426,_tmp425,_tmp424,_tmp423,({
struct Cyc_Port_Cenv*_tmp422=_tmp181;Cyc_Port_type_to_ctype(_tmp422,Cyc_Port_main_type());}),0U);});
return _tmp181;}
# 1038
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp185=Cyc_Port_compress_ct(t);void*_tmp186=_tmp185;void*_tmp193;struct Cyc_List_List*_tmp192;void*_tmp191;void*_tmp190;void*_tmp18F;void*_tmp18E;void*_tmp18D;void*_tmp18C;void*_tmp18B;void*_tmp18A;struct _fat_ptr*_tmp189;switch(*((int*)_tmp186)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 14U: _LLF: _LL10:
 goto _LL12;case 13U: _LL11: _LL12:
 goto _LL14;case 16U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 goto _LL18;case 9U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 return;case 10U: _LL1B: _tmp189=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp186)->f1;_LL1C:
# 1054
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*cts,_tmp189))
({struct Cyc_Dict_Dict _tmp429=({struct Cyc_Dict_Dict _tmp428=*cts;struct _fat_ptr*_tmp427=_tmp189;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,int*v))Cyc_Dict_insert)(_tmp428,_tmp427,({int*_tmp187=_cycalloc_atomic(sizeof(*_tmp187));*_tmp187=0;_tmp187;}));});*cts=_tmp429;});{
int*_tmp188=((int*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*cts,_tmp189);
*_tmp188=*_tmp188 + 1;
return;};case 11U: _LL1D: _tmp18E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp18D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f2;_tmp18C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f3;_tmp18B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f4;_tmp18A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f5;_LL1E:
# 1060
 Cyc_Port_region_counts(cts,_tmp18E);
Cyc_Port_region_counts(cts,_tmp18B);
return;case 12U: _LL1F: _tmp191=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp190=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f2;_tmp18F=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f3;_LL20:
# 1064
 Cyc_Port_region_counts(cts,_tmp191);
return;default: _LL21: _tmp193=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp192=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp186)->f2;_LL22:
# 1067
 Cyc_Port_region_counts(cts,_tmp193);
for(0;(unsigned)_tmp192;_tmp192=_tmp192->tl){Cyc_Port_region_counts(cts,(void*)_tmp192->hd);}
return;}_LL0:;}
# 1078
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1080
c=Cyc_Port_compress_ct(c);{
struct _tuple11 _tmp194=({struct _tuple11 _tmp3A5;_tmp3A5.f1=t,_tmp3A5.f2=c;_tmp3A5;});struct _tuple11 _tmp195=_tmp194;void*_tmp1A9;struct Cyc_List_List*_tmp1A8;void*_tmp1A7;struct Cyc_List_List*_tmp1A6;void*_tmp1A5;void*_tmp1A4;void*_tmp1A3;void*_tmp1A2;struct Cyc_Absyn_PtrLoc*_tmp1A1;void*_tmp1A0;void*_tmp19F;switch(*((int*)_tmp195.f1)){case 3U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 11U){_LL1: _tmp1A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).elt_type;_tmp1A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).ptr_atts).rgn;_tmp1A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).ptr_atts).ptrloc;_tmp1A0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_tmp19F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->f4;_LL2:
# 1083
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1A3,_tmp1A0);{
unsigned _tmp196=(unsigned)_tmp1A1?_tmp1A1->rgn_loc:(unsigned)0;
_tmp19F=Cyc_Port_compress_ct(_tmp19F);
# 1100 "port.cyc"
({struct Cyc_List_List*_tmp42D=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));
({struct Cyc_Port_Edit*_tmp42C=({struct Cyc_Port_Edit*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->loc=_tmp196,({struct _fat_ptr _tmp42B=({const char*_tmp197="`H ";_tag_fat(_tmp197,sizeof(char),4U);});_tmp199->old_string=_tmp42B;}),({struct _fat_ptr _tmp42A=({const char*_tmp198="";_tag_fat(_tmp198,sizeof(char),1U);});_tmp199->new_string=_tmp42A;});_tmp199;});_tmp19A->hd=_tmp42C;}),_tmp19A->tl=(env->gcenv)->edits;_tmp19A;});
# 1100
(env->gcenv)->edits=_tmp42D;});
# 1103
goto _LL0;};}else{goto _LL7;}case 4U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 12U){_LL3: _tmp1A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195.f1)->f1).elt_type;_tmp1A4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_LL4:
# 1105
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1A5,_tmp1A4);
goto _LL0;}else{goto _LL7;}case 5U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 15U){_LL5: _tmp1A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp195.f1)->f1).ret_type;_tmp1A8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp195.f1)->f1).args;_tmp1A7=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_tmp1A6=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->f2;_LL6:
# 1108
 Cyc_Port_register_rgns(env,counts,1,_tmp1A9,_tmp1A7);
for(0;_tmp1A8 != 0 && _tmp1A6 != 0;(_tmp1A8=_tmp1A8->tl,_tmp1A6=_tmp1A6->tl)){
struct _tuple8 _tmp19B=*((struct _tuple8*)_tmp1A8->hd);struct _tuple8 _tmp19C=_tmp19B;void*_tmp19E;_LLA: _tmp19E=_tmp19C.f3;_LLB:;{
void*_tmp19D=(void*)_tmp1A6->hd;
Cyc_Port_register_rgns(env,counts,0,_tmp19E,_tmp19D);};}
# 1114
goto _LL0;}else{goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;};}
# 1121
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int takeaddress);
# 1126
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1AA=t;struct _tuple0*_tmp1AB;switch(*((int*)_tmp1AA)){case 8U: _LL1: _tmp1AB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1AA)->f1;_LL2:
# 1129
(*_tmp1AB).f1=Cyc_Absyn_Loc_n;
return({struct Cyc_Port_Cenv*_tmp42E=env;Cyc_Port_is_char(_tmp42E,Cyc_Port_lookup_typedef(env,_tmp1AB));});case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->f2 == Cyc_Absyn_Char_sz){_LL3: _LL4:
 return 1;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 1137
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1AC=t;void*_tmp1E3;struct Cyc_List_List*_tmp1E2;void*_tmp1E1;struct Cyc_Absyn_Tqual _tmp1E0;void*_tmp1DF;unsigned _tmp1DE;void*_tmp1DD;struct Cyc_Absyn_Tqual _tmp1DC;void*_tmp1DB;void*_tmp1DA;void*_tmp1D9;void*_tmp1D8;struct Cyc_Absyn_PtrLoc*_tmp1D7;struct Cyc_List_List*_tmp1D6;union Cyc_Absyn_AggrInfo _tmp1D5;struct _tuple0*_tmp1D4;switch(*((int*)_tmp1AC)){case 8U: _LL1: _tmp1D4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1AC)->f1;_LL2:
# 1140
(*_tmp1D4).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,_tmp1D4);case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)){case 0U: _LL3: _LL4:
 return Cyc_Port_void_ct();case 1U: _LL7: _LL8:
# 1219
 goto _LLA;case 2U: _LL9: _LLA:
 return Cyc_Port_arith_ct();case 20U: _LLF: _tmp1D5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL10: {
# 1262
union Cyc_Absyn_AggrInfo _tmp1C9=_tmp1D5;struct Cyc_Absyn_Aggrdecl**_tmp1D3;struct _tuple0*_tmp1D2;struct _tuple0*_tmp1D1;if((_tmp1C9.UnknownAggr).tag == 1){if(((_tmp1C9.UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL38: _tmp1D1=((_tmp1C9.UnknownAggr).val).f2;_LL39:
# 1264
(*_tmp1D1).f1=Cyc_Absyn_Loc_n;{
struct _tuple10*_tmp1CA=Cyc_Port_lookup_struct_decl(env,_tmp1D1);
return Cyc_Port_known_aggr_ct(_tmp1CA);};}else{_LL3A: _tmp1D2=((_tmp1C9.UnknownAggr).val).f2;_LL3B:
# 1268
(*_tmp1D2).f1=Cyc_Absyn_Loc_n;{
struct _tuple10*_tmp1CB=Cyc_Port_lookup_union_decl(env,_tmp1D2);
return Cyc_Port_known_aggr_ct(_tmp1CB);};}}else{_LL3C: _tmp1D3=(_tmp1C9.KnownAggr).val;_LL3D: {
# 1272
struct Cyc_Absyn_Aggrdecl*_tmp1CC=*_tmp1D3;
enum Cyc_Absyn_AggrKind _tmp1CD=_tmp1CC->kind;enum Cyc_Absyn_AggrKind _tmp1CE=_tmp1CD;if(_tmp1CE == Cyc_Absyn_StructA){_LL3F: _LL40: {
# 1275
struct _tuple10*_tmp1CF=Cyc_Port_lookup_struct_decl(env,_tmp1CC->name);
return Cyc_Port_known_aggr_ct(_tmp1CF);}}else{_LL41: _LL42: {
# 1278
struct _tuple10*_tmp1D0=Cyc_Port_lookup_union_decl(env,_tmp1CC->name);
return Cyc_Port_known_aggr_ct(_tmp1D0);}}_LL3E:;}}_LL37:;}case 15U: _LL11: _LL12:
# 1282
 return Cyc_Port_arith_ct();case 16U: _LL13: _tmp1D6=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL14:
# 1285
 for(0;(unsigned)_tmp1D6;_tmp1D6=_tmp1D6->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp1D6->hd)->name).f1=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_tmp432=env;struct _tuple0*_tmp431=((struct Cyc_Absyn_Enumfield*)_tmp1D6->hd)->name;void*_tmp430=Cyc_Absyn_sint_type;void*_tmp42F=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp432,_tmp431,_tmp430,_tmp42F,Cyc_Port_arith_ct(),0U);});}
# 1289
return Cyc_Port_arith_ct();default: goto _LL15;}case 3U: _LL5: _tmp1DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).elt_type;_tmp1DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).elt_tq;_tmp1DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).rgn;_tmp1DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).nullable;_tmp1D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).bounds;_tmp1D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).zero_term;_tmp1D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).ptrloc;_LL6: {
# 1144
unsigned _tmp1AD=(unsigned)_tmp1D7?_tmp1D7->ptr_loc:(unsigned)0;
unsigned _tmp1AE=(unsigned)_tmp1D7?_tmp1D7->rgn_loc:(unsigned)0;
unsigned _tmp1AF=(unsigned)_tmp1D7?_tmp1D7->zt_loc:(unsigned)0;
# 1150
void*_tmp1B0=Cyc_Port_type_to_ctype(env,_tmp1DD);
void*new_rgn;
# 1153
{void*_tmp1B1=_tmp1DB;struct _fat_ptr*_tmp1B2;switch(*((int*)_tmp1B1)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f1)->tag == 5U){_LL18: _LL19:
# 1155
 new_rgn=Cyc_Port_heap_ct();goto _LL17;}else{goto _LL1E;}case 2U: _LL1A: _tmp1B2=(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1)->f1)->name;_LL1B:
# 1157
 new_rgn=Cyc_Port_rgnvar_ct(_tmp1B2);goto _LL17;case 1U: _LL1C: _LL1D:
# 1161
 if(Cyc_Port_in_fn_arg(env))
new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());else{
# 1164
if(Cyc_Port_in_fn_res(env)|| Cyc_Port_in_toplevel(env))
new_rgn=Cyc_Port_heap_ct();else{
new_rgn=Cyc_Port_var();}}
goto _LL17;default: _LL1E: _LL1F:
# 1169
 new_rgn=Cyc_Port_heap_ct();goto _LL17;}_LL17:;}{
# 1171
int _tmp1B3=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp1D9)== 0;
int orig_zt;
{void*_tmp1B4=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1B5=_tmp1B4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B5)->tag == 1U){_LL21: _LL22:
 orig_zt=Cyc_Port_is_char(env,_tmp1DD);goto _LL20;}else{_LL23: _LL24:
 orig_zt=Cyc_Tcutil_force_type2bool(0,_tmp1D8);goto _LL20;}_LL20:;}
# 1177
if((env->gcenv)->porting){
void*_tmp1B6=Cyc_Port_var();
# 1182
({struct Cyc_Port_Cenv*_tmp435=env;void*_tmp434=_tmp1B6;void*_tmp433=_tmp1DC.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp435,_tmp434,_tmp433,_tmp1DC.loc);});
# 1185
if(_tmp1DC.print_const)({void*_tmp436=_tmp1B6;Cyc_Port_unify_ct(_tmp436,Cyc_Port_const_ct());});{
# 1191
void*_tmp1B7=Cyc_Port_var();
# 1194
({struct Cyc_Port_Cenv*_tmp439=env;void*_tmp438=_tmp1B7;void*_tmp437=_tmp1B3?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();Cyc_Port_register_ptr_cvars(_tmp439,_tmp438,_tmp437,_tmp1AD);});
# 1196
if(_tmp1B3)({void*_tmp43A=_tmp1B7;Cyc_Port_unify_ct(_tmp43A,Cyc_Port_fat_ct());});{
void*_tmp1B8=Cyc_Port_var();
# 1200
({struct Cyc_Port_Cenv*_tmp43D=env;void*_tmp43C=_tmp1B8;void*_tmp43B=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp43D,_tmp43C,_tmp43B,_tmp1AF);});
# 1202
{void*_tmp1B9=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1BA=_tmp1B9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BA)->tag == 1U){_LL26: _LL27:
 goto _LL25;}else{_LL28: _LL29:
# 1205
({void*_tmp43E=_tmp1B8;Cyc_Port_unify_ct(_tmp43E,Cyc_Tcutil_force_type2bool(0,_tmp1D8)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL25;}_LL25:;}
# 1211
return Cyc_Port_ptr_ct(_tmp1B0,_tmp1B6,_tmp1B7,new_rgn,_tmp1B8);};};}else{
# 1215
return({void*_tmp442=_tmp1B0;void*_tmp441=_tmp1DC.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();void*_tmp440=
_tmp1B3?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();
# 1215
void*_tmp43F=new_rgn;Cyc_Port_ptr_ct(_tmp442,_tmp441,_tmp440,_tmp43F,
# 1217
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}};}case 4U: _LLB: _tmp1E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).elt_type;_tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).tq;_tmp1DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).zero_term;_tmp1DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).zt_loc;_LLC: {
# 1223
void*_tmp1BB=Cyc_Port_type_to_ctype(env,_tmp1E1);
int orig_zt;
{void*_tmp1BC=Cyc_Tcutil_compress(_tmp1DF);void*_tmp1BD=_tmp1BC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->tag == 1U){_LL2B: _LL2C:
 orig_zt=0;goto _LL2A;}else{_LL2D: _LL2E:
 orig_zt=Cyc_Tcutil_force_type2bool(0,_tmp1DF);goto _LL2A;}_LL2A:;}
# 1229
if((env->gcenv)->porting){
void*_tmp1BE=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp445=env;void*_tmp444=_tmp1BE;void*_tmp443=_tmp1E0.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp445,_tmp444,_tmp443,_tmp1E0.loc);});{
# 1238
void*_tmp1BF=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp448=env;void*_tmp447=_tmp1BF;void*_tmp446=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp448,_tmp447,_tmp446,_tmp1DE);});
# 1241
{void*_tmp1C0=Cyc_Tcutil_compress(_tmp1DF);void*_tmp1C1=_tmp1C0;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C1)->tag == 1U){_LL30: _LL31:
 goto _LL2F;}else{_LL32: _LL33:
# 1244
({void*_tmp449=_tmp1BF;Cyc_Port_unify_ct(_tmp449,Cyc_Tcutil_force_type2bool(0,_tmp1DF)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL2F;}_LL2F:;}
# 1247
return Cyc_Port_array_ct(_tmp1BB,_tmp1BE,_tmp1BF);};}else{
# 1249
return({void*_tmp44B=_tmp1BB;void*_tmp44A=_tmp1E0.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_array_ct(_tmp44B,_tmp44A,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}case 5U: _LLD: _tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ret_type;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).args;_LLE: {
# 1253
void*_tmp1C2=({struct Cyc_Port_Cenv*_tmp44C=Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos);Cyc_Port_type_to_ctype(_tmp44C,_tmp1E3);});
struct Cyc_Port_Cenv*_tmp1C3=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;
for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
struct _tuple8 _tmp1C4=*((struct _tuple8*)_tmp1E2->hd);struct _tuple8 _tmp1C5=_tmp1C4;struct Cyc_Absyn_Tqual _tmp1C8;void*_tmp1C7;_LL35: _tmp1C8=_tmp1C5.f2;_tmp1C7=_tmp1C5.f3;_LL36:;
cargs=({struct Cyc_List_List*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));({void*_tmp44D=Cyc_Port_type_to_ctype(_tmp1C3,_tmp1C7);_tmp1C6->hd=_tmp44D;}),_tmp1C6->tl=cargs;_tmp1C6;});}
# 1260
return({void*_tmp44E=_tmp1C2;Cyc_Port_fn_ct(_tmp44E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));});}default: _LL15: _LL16:
# 1290
 return Cyc_Port_arith_ct();}_LL0:;}
# 1295
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*_tmp1E4=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);
struct Cyc_List_List*_tmp1E5=args->tl;
enum Cyc_Absyn_Primop _tmp1E6=p;switch(_tmp1E6){case Cyc_Absyn_Plus: _LL1: _LL2: {
# 1303
void*_tmp1E7=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp1E5))->hd);
if(({void*_tmp453=_tmp1E4;Cyc_Port_leq(_tmp453,({void*_tmp452=Cyc_Port_var();void*_tmp451=Cyc_Port_var();void*_tmp450=Cyc_Port_fat_ct();void*_tmp44F=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp452,_tmp451,_tmp450,_tmp44F,Cyc_Port_var());}));})){
({void*_tmp454=_tmp1E7;Cyc_Port_leq(_tmp454,Cyc_Port_arith_ct());});
return _tmp1E4;}else{
if(({void*_tmp459=_tmp1E7;Cyc_Port_leq(_tmp459,({void*_tmp458=Cyc_Port_var();void*_tmp457=Cyc_Port_var();void*_tmp456=Cyc_Port_fat_ct();void*_tmp455=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp458,_tmp457,_tmp456,_tmp455,Cyc_Port_var());}));})){
({void*_tmp45A=_tmp1E4;Cyc_Port_leq(_tmp45A,Cyc_Port_arith_ct());});
return _tmp1E7;}else{
# 1311
({void*_tmp45B=_tmp1E4;Cyc_Port_leq(_tmp45B,Cyc_Port_arith_ct());});
({void*_tmp45C=_tmp1E7;Cyc_Port_leq(_tmp45C,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus: _LL3: _LL4:
# 1320
 if(_tmp1E5 == 0){
# 1322
({void*_tmp45D=_tmp1E4;Cyc_Port_leq(_tmp45D,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}else{
# 1325
void*_tmp1E8=Cyc_Port_compress_ct((void*)_tmp1E5->hd);
if(({void*_tmp462=_tmp1E4;Cyc_Port_leq(_tmp462,({void*_tmp461=Cyc_Port_var();void*_tmp460=Cyc_Port_var();void*_tmp45F=Cyc_Port_fat_ct();void*_tmp45E=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp461,_tmp460,_tmp45F,_tmp45E,Cyc_Port_var());}));})){
if(({void*_tmp467=_tmp1E8;Cyc_Port_leq(_tmp467,({void*_tmp466=Cyc_Port_var();void*_tmp465=Cyc_Port_var();void*_tmp464=Cyc_Port_fat_ct();void*_tmp463=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp466,_tmp465,_tmp464,_tmp463,Cyc_Port_var());}));}))
return Cyc_Port_arith_ct();else{
# 1330
({void*_tmp468=_tmp1E8;Cyc_Port_leq(_tmp468,Cyc_Port_arith_ct());});
return _tmp1E4;}}else{
# 1334
({void*_tmp469=_tmp1E4;Cyc_Port_leq(_tmp469,Cyc_Port_arith_ct());});
({void*_tmp46A=_tmp1E4;Cyc_Port_leq(_tmp46A,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times: _LL5: _LL6:
# 1339
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8: goto _LLA;case Cyc_Absyn_Mod: _LL9: _LLA: goto _LLC;case Cyc_Absyn_Eq: _LLB: _LLC: goto _LLE;case Cyc_Absyn_Neq: _LLD: _LLE: goto _LL10;case Cyc_Absyn_Lt: _LLF: _LL10: goto _LL12;case Cyc_Absyn_Gt: _LL11: _LL12: goto _LL14;case Cyc_Absyn_Lte: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Gte: _LL15: _LL16: goto _LL18;case Cyc_Absyn_Bitand: _LL17: _LL18: goto _LL1A;case Cyc_Absyn_Bitor: _LL19: _LL1A: goto _LL1C;case Cyc_Absyn_Bitxor: _LL1B: _LL1C: goto _LL1E;case Cyc_Absyn_Bitlshift: _LL1D: _LL1E: goto _LL20;case Cyc_Absyn_Bitlrshift: _LL1F: _LL20:
# 1342
({void*_tmp46B=_tmp1E4;Cyc_Port_leq(_tmp46B,Cyc_Port_arith_ct());});
({void*_tmp46C=(void*)((struct Cyc_List_List*)_check_null(_tmp1E5))->hd;Cyc_Port_leq(_tmp46C,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case Cyc_Absyn_Not: _LL21: _LL22:
 goto _LL24;case Cyc_Absyn_Bitnot: _LL23: _LL24:
# 1347
({void*_tmp46D=_tmp1E4;Cyc_Port_leq(_tmp46D,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();default: _LL25: _LL26:
({void*_tmp1E9=0U;({struct _fat_ptr _tmp46E=({const char*_tmp1EA="Port: unknown primop";_tag_fat(_tmp1EA,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp46E,_tag_fat(_tmp1E9,sizeof(void*),0U));});});}_LL0:;}
# 1354
static struct _tuple11 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){
void*_tmp1EB=e->r;void*_tmp1EC=_tmp1EB;struct Cyc_Absyn_Exp*_tmp20E;struct _fat_ptr*_tmp20D;struct Cyc_Absyn_Exp*_tmp20C;struct _fat_ptr*_tmp20B;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*_tmp208;void*_tmp207;switch(*((int*)_tmp1EC)){case 1U: _LL1: _tmp207=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_LL2: {
# 1357
struct _tuple0*_tmp1ED=Cyc_Absyn_binding2qvar(_tmp207);
(*_tmp1ED).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp1EE=Cyc_Port_lookup_var(env,_tmp1ED);struct _tuple13 _tmp1EF=_tmp1EE;struct _tuple11 _tmp1F1;unsigned _tmp1F0;_LLE: _tmp1F1=_tmp1EF.f1;_tmp1F0=_tmp1EF.f2;_LLF:;
Cyc_Port_register_localvar_usage(env,_tmp1F0,e->loc,takeaddress);
return _tmp1F1;};}case 23U: _LL3: _tmp209=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_tmp208=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1EC)->f2;_LL4: {
# 1363
void*_tmp1F2=Cyc_Port_var();
void*_tmp1F3=Cyc_Port_var();
void*_tmp1F4=Cyc_Port_gen_exp(0,env,_tmp209);
({void*_tmp46F=Cyc_Port_gen_exp(0,env,_tmp208);Cyc_Port_leq(_tmp46F,Cyc_Port_arith_ct());});{
void*_tmp1F5=({void*_tmp473=_tmp1F2;void*_tmp472=_tmp1F3;void*_tmp471=Cyc_Port_fat_ct();void*_tmp470=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp473,_tmp472,_tmp471,_tmp470,Cyc_Port_var());});
Cyc_Port_leq(_tmp1F4,_tmp1F5);
return({struct _tuple11 _tmp3A6;_tmp3A6.f1=_tmp1F3,_tmp3A6.f2=_tmp1F2;_tmp3A6;});};}case 20U: _LL5: _tmp20A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_LL6: {
# 1371
void*_tmp1F6=Cyc_Port_var();
void*_tmp1F7=Cyc_Port_var();
void*_tmp1F8=({void*_tmp477=_tmp1F6;void*_tmp476=_tmp1F7;void*_tmp475=Cyc_Port_var();void*_tmp474=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp477,_tmp476,_tmp475,_tmp474,Cyc_Port_var());});
({void*_tmp478=Cyc_Port_gen_exp(0,env,_tmp20A);Cyc_Port_leq(_tmp478,_tmp1F8);});
return({struct _tuple11 _tmp3A7;_tmp3A7.f1=_tmp1F7,_tmp3A7.f2=_tmp1F6;_tmp3A7;});}case 21U: _LL7: _tmp20C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_tmp20B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1EC)->f2;_LL8: {
# 1377
void*_tmp1F9=Cyc_Port_var();
void*_tmp1FA=Cyc_Port_var();
void*_tmp1FB=Cyc_Port_gen_exp(takeaddress,env,_tmp20C);
({void*_tmp47A=_tmp1FB;Cyc_Port_leq(_tmp47A,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp1FC[1U];({struct Cyc_Port_Cfield*_tmp479=({struct Cyc_Port_Cfield*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->qual=_tmp1FA,_tmp1FD->f=_tmp20B,_tmp1FD->type=_tmp1F9;_tmp1FD;});_tmp1FC[0]=_tmp479;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1FC,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return({struct _tuple11 _tmp3A8;_tmp3A8.f1=_tmp1FA,_tmp3A8.f2=_tmp1F9;_tmp3A8;});}case 22U: _LL9: _tmp20E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_tmp20D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1EC)->f2;_LLA: {
# 1383
void*_tmp1FE=Cyc_Port_var();
void*_tmp1FF=Cyc_Port_var();
void*_tmp200=Cyc_Port_gen_exp(0,env,_tmp20E);
({void*_tmp480=_tmp200;Cyc_Port_leq(_tmp480,({void*_tmp47F=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp201[1U];({struct Cyc_Port_Cfield*_tmp47B=({struct Cyc_Port_Cfield*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->qual=_tmp1FF,_tmp202->f=_tmp20D,_tmp202->type=_tmp1FE;_tmp202;});_tmp201[0]=_tmp47B;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp201,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp47E=
Cyc_Port_var();
# 1386
void*_tmp47D=
Cyc_Port_var();
# 1386
void*_tmp47C=
Cyc_Port_var();
# 1386
Cyc_Port_ptr_ct(_tmp47F,_tmp47E,_tmp47D,_tmp47C,
Cyc_Port_var());}));});
return({struct _tuple11 _tmp3A9;_tmp3A9.f1=_tmp1FF,_tmp3A9.f2=_tmp1FE;_tmp3A9;});}default: _LLB: _LLC:
({void*_tmp203=0U;({struct _fat_ptr _tmp483=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp206=({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0U,({struct _fat_ptr _tmp481=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3AA.f1=_tmp481;});_tmp3AA;});void*_tmp204[1U];_tmp204[0]=& _tmp206;({struct _fat_ptr _tmp482=({const char*_tmp205="gen_lhs: %s";_tag_fat(_tmp205,sizeof(char),12U);});Cyc_aprintf(_tmp482,_tag_fat(_tmp204,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp483,_tag_fat(_tmp203,sizeof(void*),0U));});});}_LL0:;}
# 1394
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
return Cyc_Port_gen_exp(0,env,e);}
# 1397
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp20F=e->r;void*_tmp210=_tmp20F;struct Cyc_Absyn_Stmt*_tmp285;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp283;void**_tmp282;struct Cyc_Absyn_Exp*_tmp281;struct Cyc_Absyn_Exp*_tmp280;struct _fat_ptr*_tmp27F;struct Cyc_Absyn_Exp*_tmp27E;struct _fat_ptr*_tmp27D;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Exp*_tmp279;void*_tmp278;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_List_List*_tmp273;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*_tmp269;enum Cyc_Absyn_Incrementor _tmp268;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Core_Opt*_tmp266;struct Cyc_Absyn_Exp*_tmp265;enum Cyc_Absyn_Primop _tmp264;struct Cyc_List_List*_tmp263;void*_tmp262;switch(*((int*)_tmp210)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp210)->f1).Wstring_c).tag){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 goto _LL8;case 6U: _LL7: _LL8:
 goto _LLA;case 7U: _LL13: _LL14:
# 1408
 return Cyc_Port_arith_ct();case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp210)->f1).Int_c).val).f2 == 0){_LL15: _LL16:
 return Cyc_Port_zero_ct();}else{_LL17: _LL18:
 return Cyc_Port_arith_ct();}case 8U: _LL19: _LL1A:
# 1412
 return({void*_tmp487=Cyc_Port_arith_ct();void*_tmp486=Cyc_Port_const_ct();void*_tmp485=Cyc_Port_fat_ct();void*_tmp484=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp487,_tmp486,_tmp485,_tmp484,Cyc_Port_zterm_ct());});case 9U: _LL1B: _LL1C:
# 1414
 return({void*_tmp48B=Cyc_Port_arith_ct();void*_tmp48A=Cyc_Port_const_ct();void*_tmp489=Cyc_Port_fat_ct();void*_tmp488=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp48B,_tmp48A,_tmp489,_tmp488,Cyc_Port_zterm_ct());});default: _LL1D: _LL1E:
 return Cyc_Port_zero_ct();}case 17U: _LL9: _LLA:
# 1403
 goto _LLC;case 18U: _LLB: _LLC:
 goto _LLE;case 19U: _LLD: _LLE:
 goto _LL10;case 32U: _LLF: _LL10:
 goto _LL12;case 33U: _LL11: _LL12:
 goto _LL14;case 1U: _LL1F: _tmp262=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL20: {
# 1417
struct _tuple0*_tmp211=Cyc_Absyn_binding2qvar(_tmp262);
(*_tmp211).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp212=Cyc_Port_lookup_var(env,_tmp211);struct _tuple13 _tmp213=_tmp212;struct _tuple11 _tmp21C;unsigned _tmp21B;_LL68: _tmp21C=_tmp213.f1;_tmp21B=_tmp213.f2;_LL69:;{
struct _tuple11 _tmp214=_tmp21C;void*_tmp21A;_LL6B: _tmp21A=_tmp214.f2;_LL6C:;
if(Cyc_Port_isfn(env,_tmp211)){
_tmp21A=Cyc_Port_instantiate(_tmp21A);
return({void*_tmp48F=_tmp21A;void*_tmp48E=Cyc_Port_var();void*_tmp48D=Cyc_Port_var();void*_tmp48C=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp48F,_tmp48E,_tmp48D,_tmp48C,Cyc_Port_nozterm_ct());});}else{
# 1425
if(Cyc_Port_isarray(env,_tmp211)){
void*_tmp215=Cyc_Port_var();
void*_tmp216=Cyc_Port_var();
void*_tmp217=Cyc_Port_var();
void*_tmp218=Cyc_Port_array_ct(_tmp215,_tmp216,_tmp217);
Cyc_Port_unifies(_tmp21A,_tmp218);{
void*_tmp219=({void*_tmp493=_tmp215;void*_tmp492=_tmp216;void*_tmp491=Cyc_Port_fat_ct();void*_tmp490=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp493,_tmp492,_tmp491,_tmp490,_tmp217);});
return _tmp219;};}else{
# 1434
Cyc_Port_register_localvar_usage(env,_tmp21B,e->loc,takeaddress);
return _tmp21A;}}};};}case 3U: _LL21: _tmp264=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp263=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL22:
# 1437
 return({struct Cyc_Port_Cenv*_tmp495=env;enum Cyc_Absyn_Primop _tmp494=_tmp264;Cyc_Port_gen_primop(_tmp495,_tmp494,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp263));});case 4U: _LL23: _tmp267=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp266=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_tmp265=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp210)->f3;_LL24: {
# 1439
struct _tuple11 _tmp21D=Cyc_Port_gen_lhs(env,_tmp267,0);struct _tuple11 _tmp21E=_tmp21D;void*_tmp222;void*_tmp221;_LL6E: _tmp222=_tmp21E.f1;_tmp221=_tmp21E.f2;_LL6F:;
({void*_tmp496=_tmp222;Cyc_Port_unifies(_tmp496,Cyc_Port_notconst_ct());});{
void*_tmp21F=Cyc_Port_gen_exp(0,env,_tmp265);
if((unsigned)_tmp266){
struct Cyc_List_List x2=({struct Cyc_List_List _tmp3AC;_tmp3AC.hd=_tmp21F,_tmp3AC.tl=0;_tmp3AC;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp3AB;_tmp3AB.hd=_tmp221,_tmp3AB.tl=& x2;_tmp3AB;});
void*_tmp220=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)_tmp266->v,& x1);
Cyc_Port_leq(_tmp220,_tmp221);
return _tmp221;}else{
# 1449
Cyc_Port_leq(_tmp21F,_tmp221);
return _tmp221;}};}case 5U: _LL25: _tmp269=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp268=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL26: {
# 1453
struct _tuple11 _tmp223=Cyc_Port_gen_lhs(env,_tmp269,0);struct _tuple11 _tmp224=_tmp223;void*_tmp226;void*_tmp225;_LL71: _tmp226=_tmp224.f1;_tmp225=_tmp224.f2;_LL72:;
({void*_tmp497=_tmp226;Cyc_Port_unifies(_tmp497,Cyc_Port_notconst_ct());});
# 1456
({void*_tmp49C=_tmp225;Cyc_Port_leq(_tmp49C,({void*_tmp49B=Cyc_Port_var();void*_tmp49A=Cyc_Port_var();void*_tmp499=Cyc_Port_fat_ct();void*_tmp498=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp49B,_tmp49A,_tmp499,_tmp498,Cyc_Port_var());}));});
({void*_tmp49D=_tmp225;Cyc_Port_leq(_tmp49D,Cyc_Port_arith_ct());});
return _tmp225;}case 6U: _LL27: _tmp26C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp26B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_tmp26A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp210)->f3;_LL28: {
# 1460
void*_tmp227=Cyc_Port_var();
({void*_tmp49E=Cyc_Port_gen_exp(0,env,_tmp26C);Cyc_Port_leq(_tmp49E,Cyc_Port_arith_ct());});
({void*_tmp49F=Cyc_Port_gen_exp(0,env,_tmp26B);Cyc_Port_leq(_tmp49F,_tmp227);});
({void*_tmp4A0=Cyc_Port_gen_exp(0,env,_tmp26A);Cyc_Port_leq(_tmp4A0,_tmp227);});
return _tmp227;}case 7U: _LL29: _tmp26E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp26D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL2A:
 _tmp270=_tmp26E;_tmp26F=_tmp26D;goto _LL2C;case 8U: _LL2B: _tmp270=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp26F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL2C:
# 1467
({void*_tmp4A1=Cyc_Port_gen_exp(0,env,_tmp270);Cyc_Port_leq(_tmp4A1,Cyc_Port_arith_ct());});
({void*_tmp4A2=Cyc_Port_gen_exp(0,env,_tmp26F);Cyc_Port_leq(_tmp4A2,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case 9U: _LL2D: _tmp272=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp271=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL2E:
# 1471
 Cyc_Port_gen_exp(0,env,_tmp272);
return Cyc_Port_gen_exp(0,env,_tmp271);case 10U: _LL2F: _tmp274=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp273=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL30: {
# 1474
void*_tmp228=Cyc_Port_var();
void*_tmp229=Cyc_Port_gen_exp(0,env,_tmp274);
struct Cyc_List_List*_tmp22A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp273);
struct Cyc_List_List*_tmp22B=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp22A);
({void*_tmp4A7=_tmp229;Cyc_Port_unifies(_tmp4A7,({void*_tmp4A6=Cyc_Port_fn_ct(_tmp228,_tmp22B);void*_tmp4A5=Cyc_Port_var();void*_tmp4A4=Cyc_Port_var();void*_tmp4A3=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4A6,_tmp4A5,_tmp4A4,_tmp4A3,Cyc_Port_nozterm_ct());}));});
for(0;_tmp22A != 0;(_tmp22A=_tmp22A->tl,_tmp22B=_tmp22B->tl)){
Cyc_Port_leq((void*)_tmp22A->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp22B))->hd);}
# 1482
return _tmp228;}case 11U: _LL31: _LL32:
({void*_tmp22C=0U;({struct _fat_ptr _tmp4A8=({const char*_tmp22D="throw";_tag_fat(_tmp22D,sizeof(char),6U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4A8,_tag_fat(_tmp22C,sizeof(void*),0U));});});case 41U: _LL33: _tmp275=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL34:
 return Cyc_Port_gen_exp(0,env,_tmp275);case 12U: _LL35: _tmp276=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL36:
 return Cyc_Port_gen_exp(0,env,_tmp276);case 13U: _LL37: _LL38:
({void*_tmp22E=0U;({struct _fat_ptr _tmp4A9=({const char*_tmp22F="instantiate";_tag_fat(_tmp22F,sizeof(char),12U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4A9,_tag_fat(_tmp22E,sizeof(void*),0U));});});case 14U: _LL39: _tmp278=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp277=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL3A: {
# 1488
void*_tmp230=Cyc_Port_type_to_ctype(env,_tmp278);
void*_tmp231=Cyc_Port_gen_exp(0,env,_tmp277);
Cyc_Port_leq(_tmp230,_tmp231);
return _tmp231;}case 15U: _LL3B: _tmp279=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL3C: {
# 1493
struct _tuple11 _tmp232=Cyc_Port_gen_lhs(env,_tmp279,1);struct _tuple11 _tmp233=_tmp232;void*_tmp235;void*_tmp234;_LL74: _tmp235=_tmp233.f1;_tmp234=_tmp233.f2;_LL75:;
return({void*_tmp4AD=_tmp234;void*_tmp4AC=_tmp235;void*_tmp4AB=Cyc_Port_var();void*_tmp4AA=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4AD,_tmp4AC,_tmp4AB,_tmp4AA,Cyc_Port_var());});}case 23U: _LL3D: _tmp27B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp27A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL3E: {
# 1496
void*_tmp236=Cyc_Port_var();
({void*_tmp4AE=Cyc_Port_gen_exp(0,env,_tmp27A);Cyc_Port_leq(_tmp4AE,Cyc_Port_arith_ct());});{
void*_tmp237=Cyc_Port_gen_exp(0,env,_tmp27B);
({void*_tmp4B3=_tmp237;Cyc_Port_leq(_tmp4B3,({void*_tmp4B2=_tmp236;void*_tmp4B1=Cyc_Port_var();void*_tmp4B0=Cyc_Port_fat_ct();void*_tmp4AF=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4B2,_tmp4B1,_tmp4B0,_tmp4AF,Cyc_Port_var());}));});
return _tmp236;};}case 20U: _LL3F: _tmp27C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL40: {
# 1502
void*_tmp238=Cyc_Port_var();
({void*_tmp4B8=Cyc_Port_gen_exp(0,env,_tmp27C);Cyc_Port_leq(_tmp4B8,({void*_tmp4B7=_tmp238;void*_tmp4B6=Cyc_Port_var();void*_tmp4B5=Cyc_Port_var();void*_tmp4B4=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4B7,_tmp4B6,_tmp4B5,_tmp4B4,Cyc_Port_var());}));});
return _tmp238;}case 21U: _LL41: _tmp27E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp27D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL42: {
# 1506
void*_tmp239=Cyc_Port_var();
void*_tmp23A=Cyc_Port_gen_exp(takeaddress,env,_tmp27E);
({void*_tmp4BB=_tmp23A;Cyc_Port_leq(_tmp4BB,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp23B[1U];({struct Cyc_Port_Cfield*_tmp4BA=({struct Cyc_Port_Cfield*_tmp23C=_cycalloc(sizeof(*_tmp23C));({void*_tmp4B9=Cyc_Port_var();_tmp23C->qual=_tmp4B9;}),_tmp23C->f=_tmp27D,_tmp23C->type=_tmp239;_tmp23C;});_tmp23B[0]=_tmp4BA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp23B,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return _tmp239;}case 22U: _LL43: _tmp280=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp27F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL44: {
# 1511
void*_tmp23D=Cyc_Port_var();
void*_tmp23E=Cyc_Port_gen_exp(0,env,_tmp280);
({void*_tmp4C2=Cyc_Port_gen_exp(0,env,_tmp280);Cyc_Port_leq(_tmp4C2,({void*_tmp4C1=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp23F[1U];({struct Cyc_Port_Cfield*_tmp4BD=({struct Cyc_Port_Cfield*_tmp240=_cycalloc(sizeof(*_tmp240));({void*_tmp4BC=Cyc_Port_var();_tmp240->qual=_tmp4BC;}),_tmp240->f=_tmp27F,_tmp240->type=_tmp23D;_tmp240;});_tmp23F[0]=_tmp4BD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp23F,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp4C0=
Cyc_Port_var();
# 1513
void*_tmp4BF=
Cyc_Port_var();
# 1513
void*_tmp4BE=
Cyc_Port_var();
# 1513
Cyc_Port_ptr_ct(_tmp4C1,_tmp4C0,_tmp4BF,_tmp4BE,
Cyc_Port_var());}));});
return _tmp23D;}case 34U: _LL45: _tmp282=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp210)->f1).elt_type;_tmp281=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp210)->f1).num_elts;_LL46:
# 1517
({void*_tmp4C3=Cyc_Port_gen_exp(0,env,_tmp281);Cyc_Port_leq(_tmp4C3,Cyc_Port_arith_ct());});{
void*_tmp241=(unsigned)_tmp282?Cyc_Port_type_to_ctype(env,*_tmp282): Cyc_Port_var();
return({void*_tmp4C7=_tmp241;void*_tmp4C6=Cyc_Port_var();void*_tmp4C5=Cyc_Port_fat_ct();void*_tmp4C4=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4C7,_tmp4C6,_tmp4C5,_tmp4C4,Cyc_Port_var());});};case 2U: _LL47: _LL48:
({void*_tmp242=0U;({struct _fat_ptr _tmp4C8=({const char*_tmp243="Pragma_e";_tag_fat(_tmp243,sizeof(char),9U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4C8,_tag_fat(_tmp242,sizeof(void*),0U));});});case 35U: _LL49: _tmp284=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp283=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL4A:
({void*_tmp244=0U;({struct _fat_ptr _tmp4C9=({const char*_tmp245="Swap_e";_tag_fat(_tmp245,sizeof(char),7U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4C9,_tag_fat(_tmp244,sizeof(void*),0U));});});case 16U: _LL4B: _LL4C:
({void*_tmp246=0U;({struct _fat_ptr _tmp4CA=({const char*_tmp247="New_e";_tag_fat(_tmp247,sizeof(char),6U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CA,_tag_fat(_tmp246,sizeof(void*),0U));});});case 24U: _LL4D: _LL4E:
({void*_tmp248=0U;({struct _fat_ptr _tmp4CB=({const char*_tmp249="Tuple_e";_tag_fat(_tmp249,sizeof(char),8U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CB,_tag_fat(_tmp248,sizeof(void*),0U));});});case 25U: _LL4F: _LL50:
({void*_tmp24A=0U;({struct _fat_ptr _tmp4CC=({const char*_tmp24B="CompoundLit_e";_tag_fat(_tmp24B,sizeof(char),14U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CC,_tag_fat(_tmp24A,sizeof(void*),0U));});});case 26U: _LL51: _LL52:
({void*_tmp24C=0U;({struct _fat_ptr _tmp4CD=({const char*_tmp24D="Array_e";_tag_fat(_tmp24D,sizeof(char),8U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CD,_tag_fat(_tmp24C,sizeof(void*),0U));});});case 27U: _LL53: _LL54:
({void*_tmp24E=0U;({struct _fat_ptr _tmp4CE=({const char*_tmp24F="Comprehension_e";_tag_fat(_tmp24F,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CE,_tag_fat(_tmp24E,sizeof(void*),0U));});});case 28U: _LL55: _LL56:
({void*_tmp250=0U;({struct _fat_ptr _tmp4CF=({const char*_tmp251="ComprehensionNoinit_e";_tag_fat(_tmp251,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CF,_tag_fat(_tmp250,sizeof(void*),0U));});});case 29U: _LL57: _LL58:
({void*_tmp252=0U;({struct _fat_ptr _tmp4D0=({const char*_tmp253="Aggregate_e";_tag_fat(_tmp253,sizeof(char),12U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D0,_tag_fat(_tmp252,sizeof(void*),0U));});});case 30U: _LL59: _LL5A:
({void*_tmp254=0U;({struct _fat_ptr _tmp4D1=({const char*_tmp255="AnonStruct_e";_tag_fat(_tmp255,sizeof(char),13U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D1,_tag_fat(_tmp254,sizeof(void*),0U));});});case 31U: _LL5B: _LL5C:
({void*_tmp256=0U;({struct _fat_ptr _tmp4D2=({const char*_tmp257="Datatype_e";_tag_fat(_tmp257,sizeof(char),11U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D2,_tag_fat(_tmp256,sizeof(void*),0U));});});case 36U: _LL5D: _LL5E:
({void*_tmp258=0U;({struct _fat_ptr _tmp4D3=({const char*_tmp259="UnresolvedMem_e";_tag_fat(_tmp259,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D3,_tag_fat(_tmp258,sizeof(void*),0U));});});case 37U: _LL5F: _tmp285=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL60:
({void*_tmp25A=0U;({struct _fat_ptr _tmp4D4=({const char*_tmp25B="StmtExp_e";_tag_fat(_tmp25B,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D4,_tag_fat(_tmp25A,sizeof(void*),0U));});});case 39U: _LL61: _LL62:
({void*_tmp25C=0U;({struct _fat_ptr _tmp4D5=({const char*_tmp25D="Valueof_e";_tag_fat(_tmp25D,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D5,_tag_fat(_tmp25C,sizeof(void*),0U));});});case 40U: _LL63: _LL64:
({void*_tmp25E=0U;({struct _fat_ptr _tmp4D6=({const char*_tmp25F="Asm_e";_tag_fat(_tmp25F,sizeof(char),6U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D6,_tag_fat(_tmp25E,sizeof(void*),0U));});});default: _LL65: _LL66:
({void*_tmp260=0U;({struct _fat_ptr _tmp4D7=({const char*_tmp261="Tagcheck_e";_tag_fat(_tmp261,sizeof(char),11U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D7,_tag_fat(_tmp260,sizeof(void*),0U));});});}_LL0:;}
# 1540
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp286=s->r;void*_tmp287=_tmp286;struct Cyc_Absyn_Stmt*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Stmt*_tmp29F;struct Cyc_Absyn_Decl*_tmp29E;struct Cyc_Absyn_Stmt*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_List_List*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Stmt*_tmp297;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_Absyn_Stmt*_tmp295;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Stmt*_tmp293;struct Cyc_Absyn_Stmt*_tmp292;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Stmt*_tmp290;struct Cyc_Absyn_Stmt*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;switch(*((int*)_tmp287)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp28E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_LL4:
 Cyc_Port_gen_exp(0,env,_tmp28E);return;case 2U: _LL5: _tmp290=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_tmp28F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp287)->f2;_LL6:
 Cyc_Port_gen_stmt(env,_tmp290);Cyc_Port_gen_stmt(env,_tmp28F);return;case 3U: _LL7: _tmp291=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_LL8: {
# 1546
void*_tmp288=Cyc_Port_lookup_return_type(env);
void*_tmp289=(unsigned)_tmp291?Cyc_Port_gen_exp(0,env,_tmp291): Cyc_Port_void_ct();
Cyc_Port_leq(_tmp289,_tmp288);
return;}case 4U: _LL9: _tmp294=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_tmp293=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp287)->f2;_tmp292=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp287)->f3;_LLA:
# 1551
({void*_tmp4D8=Cyc_Port_gen_exp(0,env,_tmp294);Cyc_Port_leq(_tmp4D8,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp293);
Cyc_Port_gen_stmt(env,_tmp292);
return;case 5U: _LLB: _tmp296=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp287)->f1).f1;_tmp295=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp287)->f2;_LLC:
# 1556
({void*_tmp4D9=Cyc_Port_gen_exp(0,env,_tmp296);Cyc_Port_leq(_tmp4D9,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp295);
return;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 8U: _LL11: _LL12:
 return;case 9U: _LL13: _tmp29A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_tmp299=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp287)->f2).f1;_tmp298=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp287)->f3).f1;_tmp297=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp287)->f4;_LL14:
# 1563
 Cyc_Port_gen_exp(0,env,_tmp29A);
({void*_tmp4DA=Cyc_Port_gen_exp(0,env,_tmp299);Cyc_Port_leq(_tmp4DA,Cyc_Port_arith_ct());});
Cyc_Port_gen_exp(0,env,_tmp298);
Cyc_Port_gen_stmt(env,_tmp297);
return;case 10U: _LL15: _tmp29C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_tmp29B=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp287)->f2;_LL16:
# 1569
({void*_tmp4DB=Cyc_Port_gen_exp(0,env,_tmp29C);Cyc_Port_leq(_tmp4DB,Cyc_Port_arith_ct());});
for(0;(unsigned)_tmp29B;_tmp29B=_tmp29B->tl){
# 1572
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp29B->hd)->body);}
# 1574
return;case 11U: _LL17: _LL18:
({void*_tmp28A=0U;({struct _fat_ptr _tmp4DC=({const char*_tmp28B="fallthru";_tag_fat(_tmp28B,sizeof(char),9U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4DC,_tag_fat(_tmp28A,sizeof(void*),0U));});});case 12U: _LL19: _tmp29E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_tmp29D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp287)->f2;_LL1A:
# 1577
 env=Cyc_Port_gen_localdecl(env,_tmp29E,0);Cyc_Port_gen_stmt(env,_tmp29D);return;case 13U: _LL1B: _tmp29F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp287)->f2;_LL1C:
 Cyc_Port_gen_stmt(env,_tmp29F);return;case 14U: _LL1D: _tmp2A1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp287)->f1;_tmp2A0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp287)->f2).f1;_LL1E:
# 1580
 Cyc_Port_gen_stmt(env,_tmp2A1);
({void*_tmp4DD=Cyc_Port_gen_exp(0,env,_tmp2A0);Cyc_Port_leq(_tmp4DD,Cyc_Port_arith_ct());});
return;default: _LL1F: _LL20:
({void*_tmp28C=0U;({struct _fat_ptr _tmp4DE=({const char*_tmp28D="try/catch";_tag_fat(_tmp28D,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4DE,_tag_fat(_tmp28C,sizeof(void*),0U));});});}_LL0:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1588
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp2A2=e->r;void*_tmp2A3=_tmp2A2;struct Cyc_List_List*_tmp2CA;struct Cyc_List_List*_tmp2C9;struct Cyc_List_List*_tmp2C8;switch(*((int*)_tmp2A3)){case 36U: _LL1: _tmp2C8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2A3)->f2;_LL2:
 _tmp2C9=_tmp2C8;goto _LL4;case 26U: _LL3: _tmp2C9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2A3)->f1;_LL4:
 _tmp2CA=_tmp2C9;goto _LL6;case 25U: _LL5: _tmp2CA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2A3)->f2;_LL6:
# 1593
 LOOP: {
void*_tmp2A4=t;struct Cyc_Absyn_Aggrdecl*_tmp2C5;struct _tuple0*_tmp2C4;void*_tmp2C3;void*_tmp2C2;unsigned _tmp2C1;struct _tuple0*_tmp2C0;switch(*((int*)_tmp2A4)){case 8U: _LLE: _tmp2C0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A4)->f1;_LLF:
# 1596
(*_tmp2C0).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,_tmp2C0);goto LOOP;case 4U: _LL10: _tmp2C3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A4)->f1).elt_type;_tmp2C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A4)->f1).zero_term;_tmp2C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A4)->f1).zt_loc;_LL11: {
# 1599
void*_tmp2A5=Cyc_Port_var();
void*_tmp2A6=Cyc_Port_var();
void*_tmp2A7=Cyc_Port_var();
void*_tmp2A8=Cyc_Port_type_to_ctype(env,_tmp2C3);
for(0;_tmp2CA != 0;_tmp2CA=_tmp2CA->tl){
struct _tuple17 _tmp2A9=*((struct _tuple17*)_tmp2CA->hd);struct _tuple17 _tmp2AA=_tmp2A9;struct Cyc_List_List*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;_LL19: _tmp2AF=_tmp2AA.f1;_tmp2AE=_tmp2AA.f2;_LL1A:;
if((unsigned)_tmp2AF)({void*_tmp2AB=0U;({struct _fat_ptr _tmp4DF=({const char*_tmp2AC="designators in initializers";_tag_fat(_tmp2AC,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4DF,_tag_fat(_tmp2AB,sizeof(void*),0U));});});{
void*_tmp2AD=Cyc_Port_gen_initializer(env,_tmp2C3,_tmp2AE);
Cyc_Port_leq(_tmp2AD,_tmp2A5);};}
# 1609
return Cyc_Port_array_ct(_tmp2A5,_tmp2A6,_tmp2A7);}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->f1)->f1).UnknownAggr).tag == 1){if((((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->f1)->f1).UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL12: _tmp2C4=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->f1)->f1).UnknownAggr).val).f2;_LL13:
# 1611
(*_tmp2C4).f1=Cyc_Absyn_Loc_n;{
struct _tuple10 _tmp2B0=*Cyc_Port_lookup_struct_decl(env,_tmp2C4);struct _tuple10 _tmp2B1=_tmp2B0;struct Cyc_Absyn_Aggrdecl*_tmp2B4;_LL1C: _tmp2B4=_tmp2B1.f1;_LL1D:;
if(_tmp2B4 == 0)({void*_tmp2B2=0U;({struct _fat_ptr _tmp4E0=({const char*_tmp2B3="struct is not yet defined";_tag_fat(_tmp2B3,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E0,_tag_fat(_tmp2B2,sizeof(void*),0U));});});
_tmp2C5=_tmp2B4;goto _LL15;};}else{goto _LL16;}}else{_LL14: _tmp2C5=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4)->f1)->f1).KnownAggr).val;_LL15: {
# 1616
struct Cyc_List_List*_tmp2B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C5->impl))->fields;
for(0;_tmp2CA != 0;_tmp2CA=_tmp2CA->tl){
struct _tuple17 _tmp2B6=*((struct _tuple17*)_tmp2CA->hd);struct _tuple17 _tmp2B7=_tmp2B6;struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;_LL1F: _tmp2BD=_tmp2B7.f1;_tmp2BC=_tmp2B7.f2;_LL20:;
if((unsigned)_tmp2BD)({void*_tmp2B8=0U;({struct _fat_ptr _tmp4E1=({const char*_tmp2B9="designators in initializers";_tag_fat(_tmp2B9,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E1,_tag_fat(_tmp2B8,sizeof(void*),0U));});});{
struct Cyc_Absyn_Aggrfield*_tmp2BA=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2B5))->hd;
_tmp2B5=_tmp2B5->tl;{
void*_tmp2BB=Cyc_Port_gen_initializer(env,_tmp2BA->type,_tmp2BC);;};};}
# 1624
return Cyc_Port_type_to_ctype(env,t);}}}else{goto _LL16;}default: _LL16: _LL17:
({void*_tmp2BE=0U;({struct _fat_ptr _tmp4E2=({const char*_tmp2BF="bad type for aggregate initializer";_tag_fat(_tmp2BF,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E2,_tag_fat(_tmp2BE,sizeof(void*),0U));});});}_LLD:;}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2A3)->f1).Wstring_c).tag){case 8U: _LL7: _LL8:
# 1627
 goto _LLA;case 9U: _LL9: _LLA:
# 1629
 LOOP2: {
void*_tmp2C6=t;struct _tuple0*_tmp2C7;switch(*((int*)_tmp2C6)){case 8U: _LL22: _tmp2C7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C6)->f1;_LL23:
# 1632
(*_tmp2C7).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,_tmp2C7);goto LOOP2;case 4U: _LL24: _LL25:
 return({void*_tmp4E4=Cyc_Port_arith_ct();void*_tmp4E3=Cyc_Port_const_ct();Cyc_Port_array_ct(_tmp4E4,_tmp4E3,Cyc_Port_zterm_ct());});default: _LL26: _LL27:
 return Cyc_Port_gen_exp(0,env,e);}_LL21:;}default: goto _LLB;}default: _LLB: _LLC:
# 1637
 return Cyc_Port_gen_exp(0,env,e);}_LL0:;}
# 1642
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){
void*_tmp2CB=d->r;void*_tmp2CC=_tmp2CB;struct Cyc_Absyn_Vardecl*_tmp2D0;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CC)->tag == 0U){_LL1: _tmp2D0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CC)->f1;_LL2:
# 1645
 if(!fromglobal)Cyc_Port_register_localvar_decl(env,_tmp2D0->name,_tmp2D0->varloc,_tmp2D0->type,_tmp2D0->initializer);{
void*_tmp2CD=Cyc_Port_var();
void*q;
if((env->gcenv)->porting){
q=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp4E7=env;void*_tmp4E6=q;void*_tmp4E5=
(_tmp2D0->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1650
Cyc_Port_register_const_cvar(_tmp4E7,_tmp4E6,_tmp4E5,(_tmp2D0->tq).loc);});}else{
# 1659
q=(_tmp2D0->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1661
(*_tmp2D0->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,_tmp2D0->name,_tmp2D0->type,q,_tmp2CD,_tmp2D0->varloc);
({void*_tmp4E8=_tmp2CD;Cyc_Port_unifies(_tmp4E8,Cyc_Port_type_to_ctype(env,_tmp2D0->type));});
if((unsigned)_tmp2D0->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp2D0->initializer);
void*t2=Cyc_Port_gen_initializer(env,_tmp2D0->type,e);
Cyc_Port_leq(t2,_tmp2CD);}
# 1669
return env;};}else{_LL3: _LL4:
({void*_tmp2CE=0U;({struct _fat_ptr _tmp4E9=({const char*_tmp2CF="non-local decl that isn't a variable";_tag_fat(_tmp2CF,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E9,_tag_fat(_tmp2CE,sizeof(void*),0U));});});}_LL0:;}
# 1674
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp2D1=*arg;struct _tuple8 _tmp2D2=_tmp2D1;struct _fat_ptr*_tmp2D6;struct Cyc_Absyn_Tqual _tmp2D5;void*_tmp2D4;_LL1: _tmp2D6=_tmp2D2.f1;_tmp2D5=_tmp2D2.f2;_tmp2D4=_tmp2D2.f3;_LL2:;
return({struct _tuple8*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->f1=0,_tmp2D3->f2=_tmp2D5,_tmp2D3->f3=_tmp2D4;_tmp2D3;});}
# 1679
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct _tuple18{struct _fat_ptr*f1;void*f2;void*f3;void*f4;};
# 1681
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp2D7=d->r;void*_tmp2D8=_tmp2D7;struct Cyc_Absyn_Enumdecl*_tmp314;struct Cyc_Absyn_Aggrdecl*_tmp313;struct Cyc_Absyn_Typedefdecl*_tmp312;struct Cyc_Absyn_Fndecl*_tmp311;struct Cyc_Absyn_Vardecl*_tmp310;switch(*((int*)_tmp2D8)){case 13U: _LL1: _LL2:
# 1684
(env->gcenv)->porting=1;
return env;case 14U: _LL3: _LL4:
# 1687
(env->gcenv)->porting=0;
return env;case 0U: _LL5: _tmp310=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL6:
# 1690
(*_tmp310->name).f1=Cyc_Absyn_Loc_n;
# 1694
if(Cyc_Port_declared_var(env,_tmp310->name)){
struct _tuple13 _tmp2D9=Cyc_Port_lookup_var(env,_tmp310->name);struct _tuple13 _tmp2DA=_tmp2D9;struct _tuple11 _tmp2DF;unsigned _tmp2DE;_LL12: _tmp2DF=_tmp2DA.f1;_tmp2DE=_tmp2DA.f2;_LL13:;{
struct _tuple11 _tmp2DB=_tmp2DF;void*_tmp2DD;void*_tmp2DC;_LL15: _tmp2DD=_tmp2DB.f1;_tmp2DC=_tmp2DB.f2;_LL16:;{
void*q2;
if((env->gcenv)->porting && !Cyc_Port_isfn(env,_tmp310->name)){
q2=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp4EC=env;void*_tmp4EB=q2;void*_tmp4EA=
(_tmp310->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1700
Cyc_Port_register_const_cvar(_tmp4EC,_tmp4EB,_tmp4EA,(_tmp310->tq).loc);});}else{
# 1709
q2=(_tmp310->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1711
Cyc_Port_unifies(_tmp2DD,q2);
({void*_tmp4ED=_tmp2DC;Cyc_Port_unifies(_tmp4ED,Cyc_Port_type_to_ctype(env,_tmp310->type));});
if((unsigned)_tmp310->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp310->initializer);
({void*_tmp4EE=Cyc_Port_gen_initializer(env,_tmp310->type,e);Cyc_Port_leq(_tmp4EE,_tmp2DC);});}
# 1717
return env;};};}else{
# 1719
return Cyc_Port_gen_localdecl(env,d,1);}case 1U: _LL7: _tmp311=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LL8:
# 1725
(*_tmp311->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple14*predeclared=0;
# 1728
if(Cyc_Port_declared_var(env,_tmp311->name))
predeclared=Cyc_Port_lookup_full_var(env,_tmp311->name);{
# 1731
void*_tmp2E0=(_tmp311->i).ret_type;
struct Cyc_List_List*_tmp2E1=(_tmp311->i).args;
struct Cyc_List_List*_tmp2E2=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp2E1);
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E3=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));
_tmp2FE->tag=5U,(_tmp2FE->f1).tvars=0,(_tmp2FE->f1).effect=0,({struct Cyc_Absyn_Tqual _tmp4EF=Cyc_Absyn_empty_tqual(0U);(_tmp2FE->f1).ret_tqual=_tmp4EF;}),(_tmp2FE->f1).ret_type=_tmp2E0,(_tmp2FE->f1).args=_tmp2E2,(_tmp2FE->f1).c_varargs=0,(_tmp2FE->f1).cyc_varargs=0,(_tmp2FE->f1).rgn_po=0,(_tmp2FE->f1).attributes=0,(_tmp2FE->f1).requires_clause=0,(_tmp2FE->f1).requires_relns=0,(_tmp2FE->f1).ensures_clause=0,(_tmp2FE->f1).ensures_relns=0;_tmp2FE;});
# 1738
struct Cyc_Port_Cenv*_tmp2E4=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp2E5=Cyc_Port_type_to_ctype(_tmp2E4,_tmp2E0);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp2E6=_tmp2E1;for(0;(unsigned)_tmp2E6;_tmp2E6=_tmp2E6->tl){
struct _tuple8 _tmp2E7=*((struct _tuple8*)_tmp2E6->hd);struct _tuple8 _tmp2E8=_tmp2E7;struct _fat_ptr*_tmp2EF;struct Cyc_Absyn_Tqual _tmp2EE;void*_tmp2ED;_LL18: _tmp2EF=_tmp2E8.f1;_tmp2EE=_tmp2E8.f2;_tmp2ED=_tmp2E8.f3;_LL19:;{
# 1746
void*_tmp2E9=Cyc_Port_type_to_ctype(_tmp2E4,_tmp2ED);
void*tqv;
if((env->gcenv)->porting){
tqv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp4F2=env;void*_tmp4F1=tqv;void*_tmp4F0=_tmp2EE.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp4F2,_tmp4F1,_tmp4F0,_tmp2EE.loc);});}else{
# 1758
tqv=_tmp2EE.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1760
c_args=({struct Cyc_List_List*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));({struct _tuple18*_tmp4F3=({struct _tuple18*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->f1=_tmp2EF,_tmp2EA->f2=_tmp2ED,_tmp2EA->f3=tqv,_tmp2EA->f4=_tmp2E9;_tmp2EA;});_tmp2EB->hd=_tmp4F3;}),_tmp2EB->tl=c_args;_tmp2EB;});
c_arg_types=({struct Cyc_List_List*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC->hd=_tmp2E9,_tmp2EC->tl=c_arg_types;_tmp2EC;});};}}
# 1763
c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);
c_arg_types=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_arg_types);{
void*_tmp2F0=Cyc_Port_fn_ct(_tmp2E5,c_arg_types);
# 1769
(*_tmp311->name).f1=Cyc_Absyn_Loc_n;
_tmp2E4=({struct Cyc_Port_Cenv*_tmp4F7=_tmp2E4;struct _tuple0*_tmp4F6=_tmp311->name;void*_tmp4F5=(void*)_tmp2E3;void*_tmp4F4=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp4F7,_tmp4F6,_tmp4F5,_tmp4F4,_tmp2F0,0U);});
Cyc_Port_add_return_type(_tmp2E4,_tmp2E5);
{struct Cyc_List_List*_tmp2F1=c_args;for(0;(unsigned)_tmp2F1;_tmp2F1=_tmp2F1->tl){
struct _tuple18 _tmp2F2=*((struct _tuple18*)_tmp2F1->hd);struct _tuple18 _tmp2F3=_tmp2F2;struct _fat_ptr*_tmp2F8;void*_tmp2F7;void*_tmp2F6;void*_tmp2F5;_LL1B: _tmp2F8=_tmp2F3.f1;_tmp2F7=_tmp2F3.f2;_tmp2F6=_tmp2F3.f3;_tmp2F5=_tmp2F3.f4;_LL1C:;
_tmp2E4=({struct Cyc_Port_Cenv*_tmp4FB=_tmp2E4;struct _tuple0*_tmp4FA=({struct _tuple0*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->f1=Cyc_Absyn_Loc_n,_tmp2F4->f2=(struct _fat_ptr*)_check_null(_tmp2F8);_tmp2F4;});void*_tmp4F9=_tmp2F7;void*_tmp4F8=_tmp2F6;Cyc_Port_add_var(_tmp4FB,_tmp4FA,_tmp4F9,_tmp4F8,_tmp2F5,0U);});}}
# 1776
Cyc_Port_gen_stmt(_tmp2E4,_tmp311->body);
# 1781
Cyc_Port_generalize(0,_tmp2F0);{
# 1788
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,_tmp2F0);
# 1792
Cyc_Port_register_rgns(env,counts,1,(void*)_tmp2E3,_tmp2F0);
env=({struct Cyc_Port_Cenv*_tmp4FF=_tmp2E4;struct _tuple0*_tmp4FE=_tmp311->name;void*_tmp4FD=(void*)_tmp2E3;void*_tmp4FC=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp4FF,_tmp4FE,_tmp4FD,_tmp4FC,_tmp2F0,0U);});
if((unsigned)predeclared){
# 1801
struct _tuple14 _tmp2F9=*predeclared;struct _tuple14 _tmp2FA=_tmp2F9;void*_tmp2FD;void*_tmp2FC;void*_tmp2FB;_LL1E: _tmp2FD=_tmp2FA.f1;_tmp2FC=(_tmp2FA.f2)->f1;_tmp2FB=(_tmp2FA.f2)->f2;_LL1F:;
({void*_tmp500=_tmp2FC;Cyc_Port_unifies(_tmp500,Cyc_Port_const_ct());});
({void*_tmp501=_tmp2FB;Cyc_Port_unifies(_tmp501,Cyc_Port_instantiate(_tmp2F0));});
# 1805
Cyc_Port_register_rgns(env,counts,1,_tmp2FD,_tmp2F0);}
# 1807
return env;};};};};case 8U: _LL9: _tmp312=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LLA: {
# 1813
void*_tmp2FF=(void*)_check_null(_tmp312->defn);
void*_tmp300=Cyc_Port_type_to_ctype(env,_tmp2FF);
(*_tmp312->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,_tmp312->name,_tmp2FF,_tmp300);
return env;}case 5U: _LLB: _tmp313=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LLC: {
# 1823
struct _tuple0*_tmp301=_tmp313->name;
(*_tmp313->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple10*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp302=_tmp313->kind;enum Cyc_Absyn_AggrKind _tmp303=_tmp302;if(_tmp303 == Cyc_Absyn_StructA){_LL21: _LL22:
# 1829
 previous=Cyc_Port_lookup_struct_decl(env,_tmp301);
goto _LL20;}else{_LL23: _LL24:
# 1832
 previous=Cyc_Port_lookup_union_decl(env,_tmp301);
goto _LL20;}_LL20:;}
# 1835
if((unsigned)_tmp313->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp304=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp313->impl))->fields;for(0;(unsigned)_tmp304;_tmp304=_tmp304->tl){
struct Cyc_Absyn_Aggrfield*_tmp305=(struct Cyc_Absyn_Aggrfield*)_tmp304->hd;
void*qv;
if((env->gcenv)->porting){
qv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp504=env;void*_tmp503=qv;void*_tmp502=
(_tmp305->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1842
Cyc_Port_register_const_cvar(_tmp504,_tmp503,_tmp502,(_tmp305->tq).loc);});}else{
# 1851
qv=(_tmp305->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
# 1853
void*_tmp306=Cyc_Port_type_to_ctype(env,_tmp305->type);
if(cf != 0){
struct Cyc_Port_Cfield _tmp307=*((struct Cyc_Port_Cfield*)cf->hd);struct Cyc_Port_Cfield _tmp308=_tmp307;void*_tmp30A;void*_tmp309;_LL26: _tmp30A=_tmp308.qual;_tmp309=_tmp308.type;_LL27:;
cf=cf->tl;
Cyc_Port_unifies(qv,_tmp30A);
Cyc_Port_unifies(_tmp306,_tmp309);}
# 1860
curr=({struct Cyc_List_List*_tmp30C=_cycalloc(sizeof(*_tmp30C));({struct Cyc_Port_Cfield*_tmp505=({struct Cyc_Port_Cfield*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->qual=qv,_tmp30B->f=_tmp305->name,_tmp30B->type=_tmp306;_tmp30B;});_tmp30C->hd=_tmp505;}),_tmp30C->tl=curr;_tmp30C;});};}}
# 1862
curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1867
return env;};}case 7U: _LLD: _tmp314=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2D8)->f1;_LLE:
# 1872
(*_tmp314->name).f1=Cyc_Absyn_Loc_n;
# 1874
if((unsigned)_tmp314->fields){
struct Cyc_List_List*_tmp30D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp314->fields))->v;for(0;(unsigned)_tmp30D;_tmp30D=_tmp30D->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp30D->hd)->name).f1=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_tmp509=env;struct _tuple0*_tmp508=((struct Cyc_Absyn_Enumfield*)_tmp30D->hd)->name;void*_tmp507=Cyc_Absyn_enum_type(_tmp314->name,_tmp314);void*_tmp506=
Cyc_Port_const_ct();
# 1877
Cyc_Port_add_var(_tmp509,_tmp508,_tmp507,_tmp506,
Cyc_Port_arith_ct(),0U);});}}
# 1880
return env;default: _LLF: _LL10:
# 1882
 if((env->gcenv)->porting)
({void*_tmp30E=0U;({struct Cyc___cycFILE*_tmp50B=Cyc_stderr;struct _fat_ptr _tmp50A=({const char*_tmp30F="Warning: Cyclone declaration found in code to be ported -- skipping.";_tag_fat(_tmp30F,sizeof(char),69U);});Cyc_fprintf(_tmp50B,_tmp50A,_tag_fat(_tmp30E,sizeof(void*),0U));});});
return env;}_LL0:;}
# 1889
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(0;(unsigned)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}
return env;}
# 1896
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1898
struct Cyc_Port_Cenv*env=({struct Cyc_Port_Cenv*_tmp50C=Cyc_Port_initial_cenv();Cyc_Port_gen_topdecls(_tmp50C,ds);});
# 1900
struct Cyc_List_List*_tmp315=(env->gcenv)->pointer_edits;
struct Cyc_List_List*_tmp316=(env->gcenv)->qualifier_edits;
struct Cyc_List_List*_tmp317=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp318=(env->gcenv)->edits;
struct Cyc_List_List*_tmp319=(env->gcenv)->vardecl_locs;
# 1906
for(0;(unsigned)_tmp319;_tmp319=_tmp319->tl){
struct _tuple15 _tmp31A=*((struct _tuple15*)_tmp319->hd);struct _tuple15 _tmp31B=_tmp31A;struct _tuple0*_tmp336;unsigned _tmp335;void*_tmp334;_LL1: _tmp336=_tmp31B.f1;_tmp335=_tmp31B.f2;_tmp334=_tmp31B.f3;_LL2:;{
struct _tuple0 _tmp31C=*_tmp336;struct _tuple0 _tmp31D=_tmp31C;struct _fat_ptr*_tmp333;_LL4: _tmp333=_tmp31D.f2;_LL5:;{
struct Cyc_Port_VarUsage*_tmp31E=((struct Cyc_Port_VarUsage*(*)(struct Cyc_Hashtable_Table*t,unsigned key))Cyc_Hashtable_lookup)((env->gcenv)->varusage_tab,_tmp335);
if(((struct Cyc_Port_VarUsage*)_check_null(_tmp31E))->address_taken){
if((unsigned)_tmp31E->init){
# 1913
_tmp318=({struct Cyc_List_List*_tmp322=_cycalloc(sizeof(*_tmp322));({struct Cyc_Port_Edit*_tmp50F=({struct Cyc_Port_Edit*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->loc=_tmp335,({struct _fat_ptr _tmp50E=({const char*_tmp31F="@";_tag_fat(_tmp31F,sizeof(char),2U);});_tmp321->old_string=_tmp50E;}),({struct _fat_ptr _tmp50D=({const char*_tmp320="";_tag_fat(_tmp320,sizeof(char),1U);});_tmp321->new_string=_tmp50D;});_tmp321;});_tmp322->hd=_tmp50F;}),_tmp322->tl=_tmp318;_tmp322;});
_tmp318=({struct Cyc_List_List*_tmp326=_cycalloc(sizeof(*_tmp326));({struct Cyc_Port_Edit*_tmp512=({struct Cyc_Port_Edit*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325->loc=((struct Cyc_Absyn_Exp*)_check_null(_tmp31E->init))->loc,({struct _fat_ptr _tmp511=({const char*_tmp323="new ";_tag_fat(_tmp323,sizeof(char),5U);});_tmp325->old_string=_tmp511;}),({struct _fat_ptr _tmp510=({const char*_tmp324="";_tag_fat(_tmp324,sizeof(char),1U);});_tmp325->new_string=_tmp510;});_tmp325;});_tmp326->hd=_tmp512;}),_tmp326->tl=_tmp318;_tmp326;});}else{
# 1918
_tmp318=({struct Cyc_List_List*_tmp32C=_cycalloc(sizeof(*_tmp32C));({struct Cyc_Port_Edit*_tmp516=({struct Cyc_Port_Edit*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->loc=_tmp335,({struct _fat_ptr _tmp515=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp329=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0U,_tmp3AE.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp333);_tmp3AE;});struct Cyc_String_pa_PrintArg_struct _tmp32A=({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0U,({struct _fat_ptr _tmp513=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp334));_tmp3AD.f1=_tmp513;});_tmp3AD;});void*_tmp327[2U];_tmp327[0]=& _tmp329,_tmp327[1]=& _tmp32A;({struct _fat_ptr _tmp514=({const char*_tmp328="?%s = malloc(sizeof(%s))";_tag_fat(_tmp328,sizeof(char),25U);});Cyc_aprintf(_tmp514,_tag_fat(_tmp327,sizeof(void*),2U));});});_tmp32B->old_string=_tmp515;}),_tmp32B->new_string=*_tmp333;_tmp32B;});_tmp32C->hd=_tmp516;}),_tmp32C->tl=_tmp318;_tmp32C;});}{
# 1920
struct Cyc_List_List*_tmp32D=_tmp31E->usage_locs;
for(0;(unsigned)_tmp32D;_tmp32D=_tmp32D->tl){
_tmp318=({struct Cyc_List_List*_tmp332=_cycalloc(sizeof(*_tmp332));({struct Cyc_Port_Edit*_tmp519=({struct Cyc_Port_Edit*_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331->loc=(unsigned)_tmp32D->hd,({struct _fat_ptr _tmp518=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0U,_tmp3AF.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp333);_tmp3AF;});void*_tmp32E[1U];_tmp32E[0]=& _tmp330;({struct _fat_ptr _tmp517=({const char*_tmp32F="(*%s)";_tag_fat(_tmp32F,sizeof(char),6U);});Cyc_aprintf(_tmp517,_tag_fat(_tmp32E,sizeof(void*),1U));});});_tmp331->old_string=_tmp518;}),_tmp331->new_string=*_tmp333;_tmp331;});_tmp332->hd=_tmp519;}),_tmp332->tl=_tmp318;_tmp332;});}};}};};}
# 1928
{struct Cyc_List_List*_tmp337=_tmp315;for(0;(unsigned)_tmp337;_tmp337=_tmp337->tl){
struct _tuple16 _tmp338=*((struct _tuple16*)_tmp337->hd);struct _tuple16 _tmp339=_tmp338;void*_tmp33C;void*_tmp33B;unsigned _tmp33A;_LL7: _tmp33C=_tmp339.f1;_tmp33B=_tmp339.f2;_tmp33A=_tmp339.f3;_LL8:;
Cyc_Port_unifies(_tmp33C,_tmp33B);}}
# 1932
{struct Cyc_List_List*_tmp33D=_tmp316;for(0;(unsigned)_tmp33D;_tmp33D=_tmp33D->tl){
struct _tuple16 _tmp33E=*((struct _tuple16*)_tmp33D->hd);struct _tuple16 _tmp33F=_tmp33E;void*_tmp342;void*_tmp341;unsigned _tmp340;_LLA: _tmp342=_tmp33F.f1;_tmp341=_tmp33F.f2;_tmp340=_tmp33F.f3;_LLB:;
Cyc_Port_unifies(_tmp342,_tmp341);}}
# 1936
{struct Cyc_List_List*_tmp343=_tmp317;for(0;(unsigned)_tmp343;_tmp343=_tmp343->tl){
struct _tuple16 _tmp344=*((struct _tuple16*)_tmp343->hd);struct _tuple16 _tmp345=_tmp344;void*_tmp348;void*_tmp347;unsigned _tmp346;_LLD: _tmp348=_tmp345.f1;_tmp347=_tmp345.f2;_tmp346=_tmp345.f3;_LLE:;
Cyc_Port_unifies(_tmp348,_tmp347);}}
# 1942
for(0;(unsigned)_tmp315;_tmp315=_tmp315->tl){
struct _tuple16 _tmp349=*((struct _tuple16*)_tmp315->hd);struct _tuple16 _tmp34A=_tmp349;void*_tmp357;void*_tmp356;unsigned _tmp355;_LL10: _tmp357=_tmp34A.f1;_tmp356=_tmp34A.f2;_tmp355=_tmp34A.f3;_LL11:;
if(!Cyc_Port_unifies(_tmp357,_tmp356)&&(int)_tmp355){
void*_tmp34B=Cyc_Port_compress_ct(_tmp356);void*_tmp34C=_tmp34B;switch(*((int*)_tmp34C)){case 2U: _LL13: _LL14:
# 1947
 _tmp318=({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*_tmp350));({struct Cyc_Port_Edit*_tmp51C=({struct Cyc_Port_Edit*_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F->loc=_tmp355,({struct _fat_ptr _tmp51B=({const char*_tmp34D="?";_tag_fat(_tmp34D,sizeof(char),2U);});_tmp34F->old_string=_tmp51B;}),({struct _fat_ptr _tmp51A=({const char*_tmp34E="*";_tag_fat(_tmp34E,sizeof(char),2U);});_tmp34F->new_string=_tmp51A;});_tmp34F;});_tmp350->hd=_tmp51C;}),_tmp350->tl=_tmp318;_tmp350;});
goto _LL12;case 3U: _LL15: _LL16:
# 1950
 _tmp318=({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));({struct Cyc_Port_Edit*_tmp51F=({struct Cyc_Port_Edit*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->loc=_tmp355,({struct _fat_ptr _tmp51E=({const char*_tmp351="*";_tag_fat(_tmp351,sizeof(char),2U);});_tmp353->old_string=_tmp51E;}),({struct _fat_ptr _tmp51D=({const char*_tmp352="?";_tag_fat(_tmp352,sizeof(char),2U);});_tmp353->new_string=_tmp51D;});_tmp353;});_tmp354->hd=_tmp51F;}),_tmp354->tl=_tmp318;_tmp354;});
goto _LL12;default: _LL17: _LL18:
 goto _LL12;}_LL12:;}}
# 1957
for(0;(unsigned)_tmp316;_tmp316=_tmp316->tl){
struct _tuple16 _tmp358=*((struct _tuple16*)_tmp316->hd);struct _tuple16 _tmp359=_tmp358;void*_tmp366;void*_tmp365;unsigned _tmp364;_LL1A: _tmp366=_tmp359.f1;_tmp365=_tmp359.f2;_tmp364=_tmp359.f3;_LL1B:;
if(!Cyc_Port_unifies(_tmp366,_tmp365)&&(int)_tmp364){
void*_tmp35A=Cyc_Port_compress_ct(_tmp365);void*_tmp35B=_tmp35A;switch(*((int*)_tmp35B)){case 1U: _LL1D: _LL1E:
# 1962
 _tmp318=({struct Cyc_List_List*_tmp35F=_cycalloc(sizeof(*_tmp35F));({struct Cyc_Port_Edit*_tmp522=({struct Cyc_Port_Edit*_tmp35E=_cycalloc(sizeof(*_tmp35E));_tmp35E->loc=_tmp364,({struct _fat_ptr _tmp521=({const char*_tmp35C="const ";_tag_fat(_tmp35C,sizeof(char),7U);});_tmp35E->old_string=_tmp521;}),({struct _fat_ptr _tmp520=({const char*_tmp35D="";_tag_fat(_tmp35D,sizeof(char),1U);});_tmp35E->new_string=_tmp520;});_tmp35E;});_tmp35F->hd=_tmp522;}),_tmp35F->tl=_tmp318;_tmp35F;});goto _LL1C;case 0U: _LL1F: _LL20:
# 1964
 _tmp318=({struct Cyc_List_List*_tmp363=_cycalloc(sizeof(*_tmp363));({struct Cyc_Port_Edit*_tmp525=({struct Cyc_Port_Edit*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362->loc=_tmp364,({struct _fat_ptr _tmp524=({const char*_tmp360="";_tag_fat(_tmp360,sizeof(char),1U);});_tmp362->old_string=_tmp524;}),({struct _fat_ptr _tmp523=({const char*_tmp361="const ";_tag_fat(_tmp361,sizeof(char),7U);});_tmp362->new_string=_tmp523;});_tmp362;});_tmp363->hd=_tmp525;}),_tmp363->tl=_tmp318;_tmp363;});goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}}
# 1970
for(0;(unsigned)_tmp317;_tmp317=_tmp317->tl){
struct _tuple16 _tmp367=*((struct _tuple16*)_tmp317->hd);struct _tuple16 _tmp368=_tmp367;void*_tmp375;void*_tmp374;unsigned _tmp373;_LL24: _tmp375=_tmp368.f1;_tmp374=_tmp368.f2;_tmp373=_tmp368.f3;_LL25:;
if(!Cyc_Port_unifies(_tmp375,_tmp374)&&(int)_tmp373){
void*_tmp369=Cyc_Port_compress_ct(_tmp374);void*_tmp36A=_tmp369;switch(*((int*)_tmp36A)){case 8U: _LL27: _LL28:
# 1975
 _tmp318=({struct Cyc_List_List*_tmp36E=_cycalloc(sizeof(*_tmp36E));({struct Cyc_Port_Edit*_tmp528=({struct Cyc_Port_Edit*_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D->loc=_tmp373,({struct _fat_ptr _tmp527=({const char*_tmp36B="@nozeroterm ";_tag_fat(_tmp36B,sizeof(char),13U);});_tmp36D->old_string=_tmp527;}),({struct _fat_ptr _tmp526=({const char*_tmp36C="";_tag_fat(_tmp36C,sizeof(char),1U);});_tmp36D->new_string=_tmp526;});_tmp36D;});_tmp36E->hd=_tmp528;}),_tmp36E->tl=_tmp318;_tmp36E;});goto _LL26;case 9U: _LL29: _LL2A:
# 1977
 _tmp318=({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));({struct Cyc_Port_Edit*_tmp52B=({struct Cyc_Port_Edit*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->loc=_tmp373,({struct _fat_ptr _tmp52A=({const char*_tmp36F="@zeroterm ";_tag_fat(_tmp36F,sizeof(char),11U);});_tmp371->old_string=_tmp52A;}),({struct _fat_ptr _tmp529=({const char*_tmp370="";_tag_fat(_tmp370,sizeof(char),1U);});_tmp371->new_string=_tmp529;});_tmp371;});_tmp372->hd=_tmp52B;}),_tmp372->tl=_tmp318;_tmp372;});goto _LL26;default: _LL2B: _LL2C:
 goto _LL26;}_LL26:;}}
# 1984
_tmp318=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp318);
# 1986
while((unsigned)_tmp318 &&((struct Cyc_Port_Edit*)_tmp318->hd)->loc == (unsigned)0){
# 1990
_tmp318=_tmp318->tl;}
# 1992
return _tmp318;}
# 1997
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp376=Cyc_Port_gen_edits(ds);
struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(unsigned(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp376);
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp377));
for(0;(unsigned)_tmp376;(_tmp376=_tmp376->tl,_tmp378=_tmp378->tl)){
struct Cyc_Port_Edit _tmp379=*((struct Cyc_Port_Edit*)_tmp376->hd);struct Cyc_Port_Edit _tmp37A=_tmp379;unsigned _tmp382;struct _fat_ptr _tmp381;struct _fat_ptr _tmp380;_LL1: _tmp382=_tmp37A.loc;_tmp381=_tmp37A.old_string;_tmp380=_tmp37A.new_string;_LL2:;{
struct _fat_ptr sloc=(struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(_tmp378))->hd);
({struct Cyc_String_pa_PrintArg_struct _tmp37D=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0U,_tmp3B2.f1=(struct _fat_ptr)((struct _fat_ptr)sloc);_tmp3B2;});struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0U,_tmp3B1.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp381);_tmp3B1;});struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0U,_tmp3B0.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp380);_tmp3B0;});void*_tmp37B[3U];_tmp37B[0]=& _tmp37D,_tmp37B[1]=& _tmp37E,_tmp37B[2]=& _tmp37F;({struct _fat_ptr _tmp52C=({const char*_tmp37C="%s: insert `%s' for `%s'\n";_tag_fat(_tmp37C,sizeof(char),26U);});Cyc_printf(_tmp52C,_tag_fat(_tmp37B,sizeof(void*),3U));});});};}};}

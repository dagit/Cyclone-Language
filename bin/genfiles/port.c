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
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 38 "position.h"
extern struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 891
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 913
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 918
extern void*Cyc_Absyn_sint_type;
# 929
extern void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);
# 951
void*Cyc_Absyn_string_type(void*rgn);
# 954
extern void*Cyc_Absyn_fat_bound_type;
# 972
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 1154
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 106
void*Cyc_Tcutil_compress(void*);
# 291
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 294
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 67
extern int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data);struct Cyc_Port_Edit{unsigned loc;struct _fat_ptr old_string;struct _fat_ptr new_string;};
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
return(int)loc;}struct Cyc_Port_VarUsage{int address_taken;struct Cyc_Absyn_Exp*init;struct Cyc_List_List*usage_locs;};struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};struct Cyc_Port_Cfield{void*qual;struct _fat_ptr*f;void*type;};struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
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
void*_tmp0=t;struct Cyc_Port_Cvar*_tmp1;struct Cyc_List_List*_tmp3;void*_tmp2;struct Cyc_List_List*_tmp4;void*_tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp8;struct Cyc_Absyn_Aggrdecl*_tmp7;void*_tmpB;void*_tmpA;void*_tmp9;void*_tmp10;void*_tmpF;void*_tmpE;void*_tmpD;void*_tmpC;struct _fat_ptr*_tmp11;switch(*((int*)_tmp0)){case 0U: _LL1: _LL2:
 return({const char*_tmp12="const";_tag_fat(_tmp12,sizeof(char),6U);});case 1U: _LL3: _LL4:
 return({const char*_tmp13="notconst";_tag_fat(_tmp13,sizeof(char),9U);});case 2U: _LL5: _LL6:
 return({const char*_tmp14="thin";_tag_fat(_tmp14,sizeof(char),5U);});case 3U: _LL7: _LL8:
 return({const char*_tmp15="fat";_tag_fat(_tmp15,sizeof(char),4U);});case 4U: _LL9: _LLA:
 return({const char*_tmp16="void";_tag_fat(_tmp16,sizeof(char),5U);});case 5U: _LLB: _LLC:
 return({const char*_tmp17="zero";_tag_fat(_tmp17,sizeof(char),5U);});case 6U: _LLD: _LLE:
 return({const char*_tmp18="arith";_tag_fat(_tmp18,sizeof(char),6U);});case 7U: _LLF: _LL10:
 return({const char*_tmp19="heap";_tag_fat(_tmp19,sizeof(char),5U);});case 8U: _LL11: _LL12:
 return({const char*_tmp1A="ZT";_tag_fat(_tmp1A,sizeof(char),3U);});case 9U: _LL13: _LL14:
 return({const char*_tmp1B="NZT";_tag_fat(_tmp1B,sizeof(char),4U);});case 10U: _LL15: _tmp11=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp0)->f1;_LL16: {struct _fat_ptr*n=_tmp11;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0U,_tmp385.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp385;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1E;({struct _fat_ptr _tmp3B5=({const char*_tmp1D="%s";_tag_fat(_tmp1D,sizeof(char),3U);});Cyc_aprintf(_tmp3B5,_tag_fat(_tmp1C,sizeof(void*),1U));});});}case 11U: _LL17: _tmpC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmpD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f2;_tmpE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f3;_tmpF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f4;_tmp10=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f5;_LL18: {void*elt=_tmpC;void*qual=_tmpD;void*k=_tmpE;void*rgn=_tmpF;void*zt=_tmp10;
# 182
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp38A;_tmp38A.tag=0U,({struct _fat_ptr _tmp3B6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,elt));_tmp38A.f1=_tmp3B6;});_tmp38A;});struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0U,({
struct _fat_ptr _tmp3B7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,qual));_tmp389.f1=_tmp3B7;});_tmp389;});struct Cyc_String_pa_PrintArg_struct _tmp23=({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0U,({struct _fat_ptr _tmp3B8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,k));_tmp388.f1=_tmp3B8;});_tmp388;});struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp387;_tmp387.tag=0U,({
struct _fat_ptr _tmp3B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,rgn));_tmp387.f1=_tmp3B9;});_tmp387;});struct Cyc_String_pa_PrintArg_struct _tmp25=({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0U,({struct _fat_ptr _tmp3BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,zt));_tmp386.f1=_tmp3BA;});_tmp386;});void*_tmp1F[5U];_tmp1F[0]=& _tmp21,_tmp1F[1]=& _tmp22,_tmp1F[2]=& _tmp23,_tmp1F[3]=& _tmp24,_tmp1F[4]=& _tmp25;({struct _fat_ptr _tmp3BB=({const char*_tmp20="ptr(%s,%s,%s,%s,%s)";_tag_fat(_tmp20,sizeof(char),20U);});Cyc_aprintf(_tmp3BB,_tag_fat(_tmp1F,sizeof(void*),5U));});});}case 12U: _LL19: _tmp9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f1;_tmpA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f2;_tmpB=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f3;_LL1A: {void*elt=_tmp9;void*qual=_tmpA;void*zt=_tmpB;
# 186
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0U,({struct _fat_ptr _tmp3BC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,elt));_tmp38D.f1=_tmp3BC;});_tmp38D;});struct Cyc_String_pa_PrintArg_struct _tmp29=({struct Cyc_String_pa_PrintArg_struct _tmp38C;_tmp38C.tag=0U,({
struct _fat_ptr _tmp3BD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,qual));_tmp38C.f1=_tmp3BD;});_tmp38C;});struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U,({struct _fat_ptr _tmp3BE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,zt));_tmp38B.f1=_tmp3BE;});_tmp38B;});void*_tmp26[3U];_tmp26[0]=& _tmp28,_tmp26[1]=& _tmp29,_tmp26[2]=& _tmp2A;({struct _fat_ptr _tmp3BF=({const char*_tmp27="array(%s,%s,%s)";_tag_fat(_tmp27,sizeof(char),16U);});Cyc_aprintf(_tmp3BF,_tag_fat(_tmp26,sizeof(void*),3U));});});}case 13U: _LL1B: _tmp7=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f1;_tmp8=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f2;_LL1C: {struct Cyc_Absyn_Aggrdecl*ad=_tmp7;struct Cyc_List_List*cfs=_tmp8;
# 189
struct _fat_ptr s=(int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp34="struct";_tag_fat(_tmp34,sizeof(char),7U);}):({const char*_tmp35="union";_tag_fat(_tmp35,sizeof(char),6U);});
if(!deep)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,_tmp38F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp38F;});struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,({struct _fat_ptr _tmp3C0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp38E.f1=_tmp3C0;});_tmp38E;});void*_tmp2B[2U];_tmp2B[0]=& _tmp2D,_tmp2B[1]=& _tmp2E;({struct _fat_ptr _tmp3C1=({const char*_tmp2C="%s %s";_tag_fat(_tmp2C,sizeof(char),6U);});Cyc_aprintf(_tmp3C1,_tag_fat(_tmp2B,sizeof(void*),2U));});});else{
# 193
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0U,_tmp392.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp392;});struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp391;_tmp391.tag=0U,({struct _fat_ptr _tmp3C2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp391.f1=_tmp3C2;});_tmp391;});struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp390;_tmp390.tag=0U,({
struct _fat_ptr _tmp3C3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_cfields2string(0,cfs));_tmp390.f1=_tmp3C3;});_tmp390;});void*_tmp2F[3U];_tmp2F[0]=& _tmp31,_tmp2F[1]=& _tmp32,_tmp2F[2]=& _tmp33;({struct _fat_ptr _tmp3C4=({const char*_tmp30="%s %s {%s}";_tag_fat(_tmp30,sizeof(char),11U);});Cyc_aprintf(_tmp3C4,_tag_fat(_tmp2F,sizeof(void*),3U));});});}}case 14U: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2 != 0){_LL1D: _tmp5=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp6=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2;_LL1E: {struct Cyc_List_List*cfs=_tmp5;void*eq=_tmp6;
return Cyc_Port_ctype2string(deep,eq);}}else{_LL1F: _tmp4=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;_LL20: {struct Cyc_List_List*cfs=_tmp4;
# 197
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp393;_tmp393.tag=0U,({struct _fat_ptr _tmp3C5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_cfields2string(deep,cfs));_tmp393.f1=_tmp3C5;});_tmp393;});void*_tmp36[1U];_tmp36[0]=& _tmp38;({struct _fat_ptr _tmp3C6=({const char*_tmp37="aggr {%s}";_tag_fat(_tmp37,sizeof(char),10U);});Cyc_aprintf(_tmp3C6,_tag_fat(_tmp36,sizeof(void*),1U));});});}}case 15U: _LL21: _tmp2=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp3=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f2;_LL22: {void*t=_tmp2;struct Cyc_List_List*ts=_tmp3;
# 199
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B=({struct Cyc_String_pa_PrintArg_struct _tmp395;_tmp395.tag=0U,({struct _fat_ptr _tmp3C7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(deep,ts));_tmp395.f1=_tmp3C7;});_tmp395;});struct Cyc_String_pa_PrintArg_struct _tmp3C=({struct Cyc_String_pa_PrintArg_struct _tmp394;_tmp394.tag=0U,({struct _fat_ptr _tmp3C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,t));_tmp394.f1=_tmp3C8;});_tmp394;});void*_tmp39[2U];_tmp39[0]=& _tmp3B,_tmp39[1]=& _tmp3C;({struct _fat_ptr _tmp3C9=({const char*_tmp3A="fn(%s)->%s";_tag_fat(_tmp3A,sizeof(char),11U);});Cyc_aprintf(_tmp3C9,_tag_fat(_tmp39,sizeof(void*),2U));});});}default: _LL23: _tmp1=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp0)->f1;_LL24: {struct Cyc_Port_Cvar*cv=_tmp1;
# 201
if((unsigned)cv->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(cv->eq)));else{
if(!deep || cv->uppers == 0 && cv->lowers == 0)
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3F=({struct Cyc_Int_pa_PrintArg_struct _tmp396;_tmp396.tag=1U,_tmp396.f1=(unsigned long)cv->id;_tmp396;});void*_tmp3D[1U];_tmp3D[0]=& _tmp3F;({struct _fat_ptr _tmp3CA=({const char*_tmp3E="var(%d)";_tag_fat(_tmp3E,sizeof(char),8U);});Cyc_aprintf(_tmp3CA,_tag_fat(_tmp3D,sizeof(void*),1U));});});else{
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0U,({
struct _fat_ptr _tmp3CB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(0,cv->lowers));_tmp399.f1=_tmp3CB;});_tmp399;});struct Cyc_Int_pa_PrintArg_struct _tmp43=({struct Cyc_Int_pa_PrintArg_struct _tmp398;_tmp398.tag=1U,_tmp398.f1=(unsigned long)cv->id;_tmp398;});struct Cyc_String_pa_PrintArg_struct _tmp44=({struct Cyc_String_pa_PrintArg_struct _tmp397;_tmp397.tag=0U,({
# 208
struct _fat_ptr _tmp3CC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(0,cv->uppers));_tmp397.f1=_tmp3CC;});_tmp397;});void*_tmp40[3U];_tmp40[0]=& _tmp42,_tmp40[1]=& _tmp43,_tmp40[2]=& _tmp44;({struct _fat_ptr _tmp3CD=({const char*_tmp41="var([%s]<=%d<=[%s])";_tag_fat(_tmp41,sizeof(char),20U);});Cyc_aprintf(_tmp3CD,_tag_fat(_tmp40,sizeof(void*),3U));});});}}}}_LL0:;}
# 211
static struct _fat_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){return({struct _fat_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));({struct _fat_ptr _tmp3CE=Cyc_Port_ctype2string(deep,t);*_tmp45=_tmp3CE;});_tmp45;});}
struct Cyc_List_List*Cyc_Port_sep(struct _fat_ptr s,struct Cyc_List_List*xs){
struct _fat_ptr*_tmp46=({struct _fat_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=s;_tmp4A;});struct _fat_ptr*sptr=_tmp46;
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp47=xs;struct Cyc_List_List*prev=_tmp47;
struct Cyc_List_List*_tmp48=xs->tl;struct Cyc_List_List*curr=_tmp48;
for(0;curr != 0;(prev=curr,curr=curr->tl)){
({struct Cyc_List_List*_tmp3CF=({struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->hd=sptr,_tmp49->tl=curr;_tmp49;});prev->tl=_tmp3CF;});}
# 220
return xs;}}
# 222
static struct _fat_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0U,({
struct _fat_ptr _tmp3D0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,f->qual));_tmp39C.f1=_tmp3D0;});_tmp39C;});struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp39B;_tmp39B.tag=0U,_tmp39B.f1=(struct _fat_ptr)((struct _fat_ptr)*f->f);_tmp39B;});struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0U,({struct _fat_ptr _tmp3D1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,f->type));_tmp39A.f1=_tmp3D1;});_tmp39A;});void*_tmp4C[3U];_tmp4C[0]=& _tmp4E,_tmp4C[1]=& _tmp4F,_tmp4C[2]=& _tmp50;({struct _fat_ptr _tmp3D2=({const char*_tmp4D="%s %s: %s";_tag_fat(_tmp4D,sizeof(char),10U);});Cyc_aprintf(_tmp3D2,_tag_fat(_tmp4C,sizeof(void*),3U));});});
return({struct _fat_ptr*_tmp4B=_cycalloc(sizeof(*_tmp4B));*_tmp4B=s;_tmp4B;});}
# 227
static struct _fat_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr _tmp3D3=({const char*_tmp51=",";_tag_fat(_tmp51,sizeof(char),2U);});Cyc_Port_sep(_tmp3D3,((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts));}));}
# 230
static struct _fat_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr _tmp3D4=({const char*_tmp52=";";_tag_fat(_tmp52,sizeof(char),2U);});Cyc_Port_sep(_tmp3D4,((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs));}));}
# 236
static void*Cyc_Port_notconst_ct (void){return(void*)& Cyc_Port_Notconst_ct_val;}
static void*Cyc_Port_const_ct (void){return(void*)& Cyc_Port_Const_ct_val;}
static void*Cyc_Port_thin_ct (void){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct (void){return(void*)& Cyc_Port_Fat_ct_val;}
static void*Cyc_Port_void_ct (void){return(void*)& Cyc_Port_Void_ct_val;}
static void*Cyc_Port_zero_ct (void){return(void*)& Cyc_Port_Zero_ct_val;}
static void*Cyc_Port_arith_ct (void){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct (void){return(void*)& Cyc_Port_Heap_ct_val;}
static void*Cyc_Port_zterm_ct (void){return(void*)& Cyc_Port_Zterm_ct_val;}
static void*Cyc_Port_nozterm_ct (void){return(void*)& Cyc_Port_Nozterm_ct_val;}
static void*Cyc_Port_rgnvar_ct(struct _fat_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->tag=10U,_tmp53->f1=n;_tmp53;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=14U,_tmp54->f1=fs,_tmp54->f2=0;_tmp54;});}
# 250
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
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
static void*Cyc_Port_var (void){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=16U,({struct Cyc_Port_Cvar*_tmp3D5=({struct Cyc_Port_Cvar*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->id=counter ++,_tmp59->eq=0,_tmp59->uppers=0,_tmp59->lowers=0;_tmp59;});_tmp5A->f1=_tmp3D5;});_tmp5A;});}
# 267
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 270
static struct _fat_ptr*Cyc_Port_new_region_var (void){
static int counter=0;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5E=({struct Cyc_Int_pa_PrintArg_struct _tmp39D;_tmp39D.tag=1U,_tmp39D.f1=(unsigned long)counter ++;_tmp39D;});void*_tmp5C[1U];_tmp5C[0]=& _tmp5E;({struct _fat_ptr _tmp3D6=({const char*_tmp5D="`R%d";_tag_fat(_tmp5D,sizeof(char),5U);});Cyc_aprintf(_tmp3D6,_tag_fat(_tmp5C,sizeof(void*),1U));});});
return({struct _fat_ptr*_tmp5B=_cycalloc(sizeof(*_tmp5B));*_tmp5B=s;_tmp5B;});}
# 278
static int Cyc_Port_unifies(void*t1,void*t2);
# 280
static void*Cyc_Port_compress_ct(void*t){
void*_tmp5F=t;void**_tmp60;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp62;void***_tmp61;switch(*((int*)_tmp5F)){case 16U: _LL1: _tmp61=(void***)&(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->eq;_tmp62=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->uppers;_tmp63=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->lowers;_LL2: {void***eqp=_tmp61;struct Cyc_List_List*us=_tmp62;struct Cyc_List_List*ls=_tmp63;
# 283
void**_tmp64=*eqp;void**eq=_tmp64;
if((unsigned)eq){
# 287
void*r=Cyc_Port_compress_ct(*eq);
if(*eq != r)({void**_tmp3D7=({void**_tmp65=_cycalloc(sizeof(*_tmp65));*_tmp65=r;_tmp65;});*eqp=_tmp3D7;});
return r;}
# 294
for(0;ls != 0;ls=ls->tl){
void*_tmp66=(void*)ls->hd;void*_stmttmp0=_tmp66;void*_tmp67=_stmttmp0;switch(*((int*)_tmp67)){case 0U: _LL8: _LL9:
 goto _LLB;case 9U: _LLA: _LLB:
 goto _LLD;case 4U: _LLC: _LLD:
 goto _LLF;case 13U: _LLE: _LLF:
 goto _LL11;case 15U: _LL10: _LL11:
# 301
({void**_tmp3D8=({void**_tmp68=_cycalloc(sizeof(*_tmp68));*_tmp68=(void*)ls->hd;_tmp68;});*eqp=_tmp3D8;});
return(void*)ls->hd;default: _LL12: _LL13:
# 304
 goto _LL7;}_LL7:;}
# 307
for(0;us != 0;us=us->tl){
void*_tmp69=(void*)us->hd;void*_stmttmp1=_tmp69;void*_tmp6A=_stmttmp1;switch(*((int*)_tmp6A)){case 1U: _LL15: _LL16:
 goto _LL18;case 8U: _LL17: _LL18:
 goto _LL1A;case 5U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 15U: _LL1D: _LL1E:
# 314
({void**_tmp3D9=({void**_tmp6B=_cycalloc(sizeof(*_tmp6B));*_tmp6B=(void*)us->hd;_tmp6B;});*eqp=_tmp3D9;});
return(void*)us->hd;default: _LL1F: _LL20:
# 317
 goto _LL14;}_LL14:;}
# 320
return t;}case 14U: _LL3: _tmp60=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp5F)->f2;_LL4: {void**eq=_tmp60;
# 323
if((unsigned)eq)return Cyc_Port_compress_ct(*eq);else{
return t;}}default: _LL5: _LL6:
# 327
 return t;}_LL0:;}struct _tuple12{void*f1;void*f2;};
# 333
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp6C=Cyc_Port_compress_ct(t);void*_stmttmp2=_tmp6C;void*_tmp6D=_stmttmp2;struct Cyc_List_List*_tmp6F;void*_tmp6E;void*_tmp72;void*_tmp71;void*_tmp70;void*_tmp77;void*_tmp76;void*_tmp75;void*_tmp74;void*_tmp73;struct _fat_ptr*_tmp78;switch(*((int*)_tmp6D)){case 0U: _LL1: _LL2:
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
 return t;case 10U: _LL1B: _tmp78=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp6D)->f1;_LL1C: {struct _fat_ptr*n=_tmp78;
# 349
if(!((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*instenv,n))
({struct Cyc_Dict_Dict _tmp3DC=({struct Cyc_Dict_Dict _tmp3DB=*instenv;struct _fat_ptr*_tmp3DA=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3DB,_tmp3DA,Cyc_Port_var());});*instenv=_tmp3DC;});
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*instenv,n);}case 11U: _LL1D: _tmp73=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp74=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f2;_tmp75=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f3;_tmp76=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f4;_tmp77=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f5;_LL1E: {void*t1=_tmp73;void*t2=_tmp74;void*t3=_tmp75;void*t4=_tmp76;void*zt=_tmp77;
# 353
struct _tuple12 _tmp79=({struct _tuple12 _tmp39E;({void*_tmp3DE=Cyc_Port_inst(instenv,t1);_tmp39E.f1=_tmp3DE;}),({void*_tmp3DD=Cyc_Port_inst(instenv,t4);_tmp39E.f2=_tmp3DD;});_tmp39E;});struct _tuple12 _stmttmp3=_tmp79;struct _tuple12 _tmp7A=_stmttmp3;void*_tmp7C;void*_tmp7B;_LL24: _tmp7B=_tmp7A.f1;_tmp7C=_tmp7A.f2;_LL25: {void*nt1=_tmp7B;void*nt4=_tmp7C;
if(nt1 == t1 && nt4 == t4)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=11U,_tmp7D->f1=nt1,_tmp7D->f2=t2,_tmp7D->f3=t3,_tmp7D->f4=nt4,_tmp7D->f5=zt;_tmp7D;});}}case 12U: _LL1F: _tmp70=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp71=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f2;_tmp72=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f3;_LL20: {void*t1=_tmp70;void*t2=_tmp71;void*zt=_tmp72;
# 357
void*_tmp7E=Cyc_Port_inst(instenv,t1);void*nt1=_tmp7E;
if(nt1 == t1)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=12U,_tmp7F->f1=nt1,_tmp7F->f2=t2,_tmp7F->f3=zt;_tmp7F;});}default: _LL21: _tmp6E=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp6F=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp6D)->f2;_LL22: {void*t1=_tmp6E;struct Cyc_List_List*ts=_tmp6F;
# 361
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=15U,({void*_tmp3E0=Cyc_Port_inst(instenv,t1);_tmp80->f1=_tmp3E0;}),({struct Cyc_List_List*_tmp3DF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,ts);_tmp80->f2=_tmp3DF;});_tmp80;});}}_LL0:;}
# 365
void*Cyc_Port_instantiate(void*t){
return({struct Cyc_Dict_Dict*_tmp3E2=({struct Cyc_Dict_Dict*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Dict_Dict _tmp3E1=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp81=_tmp3E1;});_tmp81;});Cyc_Port_inst(_tmp3E2,t);});}
# 372
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*_tmp82=ts;struct Cyc_List_List*xs=_tmp82;for(0;(unsigned)xs;xs=xs->tl){
void*_tmp83=Cyc_Port_compress_ct((void*)xs->hd);void*t2=_tmp83;
if(t == t2)found=1;}}
# 378
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(0;ts != 0;ts=ts->tl){
if(({void*_tmp3E3=t;_tmp3E3 == Cyc_Port_compress_ct((void*)ts->hd);}))continue;
res=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->hd=(void*)ts->hd,_tmp84->tl=res;_tmp84;});}
# 384
return res;}}
# 389
void Cyc_Port_generalize(int is_rgn,void*t){
t=Cyc_Port_compress_ct(t);{
void*_tmp85=t;struct Cyc_List_List*_tmp87;void*_tmp86;void*_tmp8A;void*_tmp89;void*_tmp88;void*_tmp8F;void*_tmp8E;void*_tmp8D;void*_tmp8C;void*_tmp8B;struct Cyc_Port_Cvar*_tmp90;switch(*((int*)_tmp85)){case 16U: _LL1: _tmp90=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp85)->f1;_LL2: {struct Cyc_Port_Cvar*cv=_tmp90;
# 394
({struct Cyc_List_List*_tmp3E4=Cyc_Port_filter_self(t,cv->uppers);cv->uppers=_tmp3E4;});
({struct Cyc_List_List*_tmp3E5=Cyc_Port_filter_self(t,cv->lowers);cv->lowers=_tmp3E5;});
if(is_rgn){
# 399
if(cv->uppers == 0 && cv->lowers == 0){
({void*_tmp3E6=t;Cyc_Port_unifies(_tmp3E6,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));});
return;}
# 404
if((unsigned)cv->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(cv->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 408
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(cv->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 412
return;}case 0U: _LL3: _LL4:
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
 return;case 11U: _LL1D: _tmp8B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp8C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f2;_tmp8D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f3;_tmp8E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f4;_tmp8F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f5;_LL1E: {void*t1=_tmp8B;void*t2=_tmp8C;void*t3=_tmp8D;void*t4=_tmp8E;void*t5=_tmp8F;
# 429
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(1,t4);goto _LL0;}case 12U: _LL1F: _tmp88=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp89=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f2;_tmp8A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f3;_LL20: {void*t1=_tmp88;void*t2=_tmp89;void*t3=_tmp8A;
# 431
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(0,t2);goto _LL0;}default: _LL21: _tmp86=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp87=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp85)->f2;_LL22: {void*t1=_tmp86;struct Cyc_List_List*ts=_tmp87;
# 433
Cyc_Port_generalize(0,t1);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,ts);goto _LL0;}}_LL0:;}}
# 439
static int Cyc_Port_occurs(void*v,void*t){
t=Cyc_Port_compress_ct(t);
if(t == v)return 1;{
void*_tmp91=t;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;struct Cyc_List_List*_tmp95;void*_tmp94;void*_tmp98;void*_tmp97;void*_tmp96;void*_tmp9D;void*_tmp9C;void*_tmp9B;void*_tmp9A;void*_tmp99;switch(*((int*)_tmp91)){case 11U: _LL1: _tmp99=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp9A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f2;_tmp9B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f3;_tmp9C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f4;_tmp9D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f5;_LL2: {void*t1=_tmp99;void*t2=_tmp9A;void*t3=_tmp9B;void*t4=_tmp9C;void*t5=_tmp9D;
# 444
return(((Cyc_Port_occurs(v,t1)|| Cyc_Port_occurs(v,t2))|| Cyc_Port_occurs(v,t3))|| Cyc_Port_occurs(v,t4))||
 Cyc_Port_occurs(v,t5);}case 12U: _LL3: _tmp96=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp97=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f2;_tmp98=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f3;_LL4: {void*t1=_tmp96;void*t2=_tmp97;void*t3=_tmp98;
# 447
return(Cyc_Port_occurs(v,t1)|| Cyc_Port_occurs(v,t2))|| Cyc_Port_occurs(v,t3);}case 15U: _LL5: _tmp94=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp95=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp91)->f2;_LL6: {void*t=_tmp94;struct Cyc_List_List*ts=_tmp95;
# 449
if(Cyc_Port_occurs(v,t))return 1;
for(0;(unsigned)ts;ts=ts->tl){
if(Cyc_Port_occurs(v,(void*)ts->hd))return 1;}
return 0;}case 13U: _LL7: _tmp93=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp91)->f1)->f2;_LL8: {struct Cyc_List_List*fs=_tmp93;
return 0;}case 14U: _LL9: _tmp92=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp91)->f1;_LLA: {struct Cyc_List_List*fs=_tmp92;
# 455
for(0;(unsigned)fs;fs=fs->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)fs->hd)->qual)|| Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)fs->hd)->type))return 1;}
return 0;}default: _LLB: _LLC:
 return 0;}_LL0:;}}char Cyc_Port_Unify_ct[9U]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
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
struct _tuple12 _tmp9E=({struct _tuple12 _tmp39F;_tmp39F.f1=t1,_tmp39F.f2=t2;_tmp39F;});struct _tuple12 _stmttmp4=_tmp9E;struct _tuple12 _tmp9F=_stmttmp4;struct Cyc_List_List*_tmpA3;struct Cyc_Absyn_Aggrdecl*_tmpA2;void***_tmpA1;struct Cyc_List_List*_tmpA0;void***_tmpA7;struct Cyc_List_List*_tmpA6;void***_tmpA5;struct Cyc_List_List*_tmpA4;void***_tmpAB;struct Cyc_List_List*_tmpAA;struct Cyc_List_List*_tmpA9;struct Cyc_Absyn_Aggrdecl*_tmpA8;struct _tuple11*_tmpAD;struct _tuple11*_tmpAC;struct Cyc_List_List*_tmpB1;void*_tmpB0;struct Cyc_List_List*_tmpAF;void*_tmpAE;void*_tmpB7;void*_tmpB6;void*_tmpB5;void*_tmpB4;void*_tmpB3;void*_tmpB2;struct _fat_ptr _tmpB9;struct _fat_ptr _tmpB8;void*_tmpC3;void*_tmpC2;void*_tmpC1;void*_tmpC0;void*_tmpBF;void*_tmpBE;void*_tmpBD;void*_tmpBC;void*_tmpBB;void*_tmpBA;struct Cyc_Port_Cvar*_tmpC4;struct Cyc_Port_Cvar*_tmpC5;if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f1)->tag == 16U){_LL1: _tmpC5=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_LL2: {struct Cyc_Port_Cvar*v1=_tmpC5;
# 479
if(!Cyc_Port_occurs(t1,t2)){
# 482
{struct Cyc_List_List*_tmpC6=Cyc_Port_filter_self(t1,v1->uppers);struct Cyc_List_List*us=_tmpC6;for(0;(unsigned)us;us=us->tl){
if(!Cyc_Port_leq(t2,(void*)us->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmpC7=Cyc_Port_filter_self(t1,v1->lowers);struct Cyc_List_List*ls=_tmpC7;for(0;(unsigned)ls;ls=ls->tl){
if(!Cyc_Port_leq((void*)ls->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
({void**_tmp3E7=({void**_tmpC8=_cycalloc(sizeof(*_tmpC8));*_tmpC8=t2;_tmpC8;});v1->eq=_tmp3E7;});
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}else{if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 16U){_LL3: _tmpC4=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LL4: {struct Cyc_Port_Cvar*v1=_tmpC4;
Cyc_Port_unify_ct(t2,t1);return;}}else{switch(*((int*)_tmp9F.f1)){case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 11U){_LL5: _tmpBA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpBB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpBC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f3;_tmpBD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f4;_tmpBE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f5;_tmpBF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpC0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_tmpC1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f3;_tmpC2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f4;_tmpC3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f5;_LL6: {void*e1=_tmpBA;void*q1=_tmpBB;void*k1=_tmpBC;void*r1=_tmpBD;void*zt1=_tmpBE;void*e2=_tmpBF;void*q2=_tmpC0;void*k2=_tmpC1;void*r2=_tmpC2;void*zt2=_tmpC3;
# 491
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(k1,k2);Cyc_Port_unify_ct(r1,r2);
Cyc_Port_unify_ct(zt1,zt2);
return;}}else{goto _LL15;}case 10U: if(((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 10U){_LL7: _tmpB8=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB9=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LL8: {struct _fat_ptr n1=_tmpB8;struct _fat_ptr n2=_tmpB9;
# 495
if(Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2)!= 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;}}else{goto _LL15;}case 12U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 12U){_LL9: _tmpB2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpB4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f3;_tmpB5=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpB6=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_tmpB7=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f3;_LLA: {void*e1=_tmpB2;void*q1=_tmpB3;void*zt1=_tmpB4;void*e2=_tmpB5;void*q2=_tmpB6;void*zt2=_tmpB7;
# 498
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(zt1,zt2);return;}}else{goto _LL15;}case 15U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 15U){_LLB: _tmpAE=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpAF=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpB0=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpB1=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LLC: {void*t1=_tmpAE;struct Cyc_List_List*ts1=_tmpAF;void*t2=_tmpB0;struct Cyc_List_List*ts2=_tmpB1;
# 500
Cyc_Port_unify_ct(t1,t2);Cyc_Port_unify_cts(ts1,ts2);return;}}else{goto _LL15;}case 13U: switch(*((int*)_tmp9F.f2)){case 13U: _LLD: _tmpAC=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpAD=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LLE: {struct _tuple11*p1=_tmpAC;struct _tuple11*p2=_tmpAD;
# 502
if(p1 == p2)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}case 14U: _LL13: _tmpA8=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1)->f1;_tmpA9=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1)->f2;_tmpAA=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpAB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LL14: {struct Cyc_Absyn_Aggrdecl*ad=_tmpA8;struct Cyc_List_List*fs2=_tmpA9;struct Cyc_List_List*fs1=_tmpAA;void***eq=_tmpAB;
# 512
Cyc_Port_merge_fields(fs2,fs1,0);
({void**_tmp3E8=({void**_tmpCC=_cycalloc(sizeof(*_tmpCC));*_tmpCC=t1;_tmpCC;});*eq=_tmp3E8;});
return;}default: goto _LL15;}case 14U: switch(*((int*)_tmp9F.f2)){case 14U: _LLF: _tmpA4=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpA5=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpA6=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpA7=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LL10: {struct Cyc_List_List*fs1=_tmpA4;void***eq1=_tmpA5;struct Cyc_List_List*fs2=_tmpA6;void***eq2=_tmpA7;
# 504
void*_tmpC9=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(fs1,fs2,1));void*t=_tmpC9;
({void**_tmp3EA=({void**_tmp3E9=({void**_tmpCA=_cycalloc(sizeof(*_tmpCA));*_tmpCA=t;_tmpCA;});*eq2=_tmp3E9;});*eq1=_tmp3EA;});
return;}case 13U: _LL11: _tmpA0=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpA1=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpA2=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1)->f1;_tmpA3=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1)->f2;_LL12: {struct Cyc_List_List*fs1=_tmpA0;void***eq=_tmpA1;struct Cyc_Absyn_Aggrdecl*ad=_tmpA2;struct Cyc_List_List*fs2=_tmpA3;
# 508
Cyc_Port_merge_fields(fs2,fs1,0);
({void**_tmp3EB=({void**_tmpCB=_cycalloc(sizeof(*_tmpCB));*_tmpCB=t2;_tmpCB;});*eq=_tmp3EB;});
return;}default: goto _LL15;}default: _LL15: _LL16:
# 515
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}_LL0:;}}
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
{struct Cyc_List_List*_tmpCD=fs2;struct Cyc_List_List*xs2=_tmpCD;for(0;(unsigned)xs2;xs2=xs2->tl){
struct Cyc_Port_Cfield*_tmpCE=(struct Cyc_Port_Cfield*)xs2->hd;struct Cyc_Port_Cfield*f2=_tmpCE;
int found=0;
{struct Cyc_List_List*_tmpCF=fs1;struct Cyc_List_List*xs1=_tmpCF;for(0;(unsigned)xs1;xs1=xs1->tl){
struct Cyc_Port_Cfield*_tmpD0=(struct Cyc_Port_Cfield*)xs1->hd;struct Cyc_Port_Cfield*f1=_tmpD0;
if(Cyc_strptrcmp(f1->f,f2->f)== 0){
common=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=f1,_tmpD1->tl=common;_tmpD1;});
Cyc_Port_unify_ct(f1->qual,f2->qual);
Cyc_Port_unify_ct(f1->type,f2->type);
found=1;
break;}}}
# 545
if(!found){
if(allow_f1_subset_f2)
common=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=f2,_tmpD2->tl=common;_tmpD2;});else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 551
{struct Cyc_List_List*_tmpD3=fs1;struct Cyc_List_List*xs1=_tmpD3;for(0;(unsigned)xs1;xs1=xs1->tl){
struct Cyc_Port_Cfield*_tmpD4=(struct Cyc_Port_Cfield*)xs1->hd;struct Cyc_Port_Cfield*f1=_tmpD4;
int found=0;
{struct Cyc_List_List*_tmpD5=fs2;struct Cyc_List_List*xs2=_tmpD5;for(0;(unsigned)xs2;xs2=xs2->tl){
struct Cyc_Port_Cfield*_tmpD6=(struct Cyc_Port_Cfield*)xs2->hd;struct Cyc_Port_Cfield*f2=_tmpD6;
if(Cyc_strptrcmp(f1->f,f2->f))found=1;}}
# 558
if(!found)
common=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=f1,_tmpD7->tl=common;_tmpD7;});}}
# 561
return common;}
# 564
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int _tmpDA=0;if(setjmp(_tmpD8.handler))_tmpDA=1;if(!_tmpDA){
# 571
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmpD9=(void*)Cyc_Core_get_exn_thrown();void*_tmpDB=_tmpD9;void*_tmpDC;if(((struct Cyc_Port_Unify_ct_exn_struct*)_tmpDB)->tag == Cyc_Port_Unify_ct){_LL1: _LL2:
# 578
 return 0;}else{_LL3: _tmpDC=_tmpDB;_LL4: {void*exn=_tmpDC;(int)_rethrow(exn);}}_LL0:;}}}
# 580
return 1;}struct _tuple13{void*f1;void*f2;void*f3;void*f4;void*f5;};
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
{struct Cyc_List_List*_tmpDE=*uppers;struct Cyc_List_List*us=_tmpDE;for(0;(unsigned)us;us=us->tl){
void*_tmpDF=Cyc_Port_compress_ct((void*)us->hd);void*t2=_tmpDF;
# 618
if(t == t2)return*uppers;{
struct _tuple12 _tmpE0=({struct _tuple12 _tmp3A1;_tmp3A1.f1=t,_tmp3A1.f2=t2;_tmp3A1;});struct _tuple12 _stmttmp5=_tmpE0;struct _tuple12 _tmpE1=_stmttmp5;void*_tmpEB;void*_tmpEA;void*_tmpE9;void*_tmpE8;void*_tmpE7;void*_tmpE6;void*_tmpE5;void*_tmpE4;void*_tmpE3;void*_tmpE2;switch(*((int*)_tmpE1.f1)){case 6U: switch(*((int*)_tmpE1.f2)){case 11U: _LL1C: _LL1D:
# 623
 goto _LL1F;case 5U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
# 626
 return*uppers;default: goto _LL24;}case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->tag == 11U){_LL22: _tmpE2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f1;_tmpE3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f2;_tmpE4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f3;_tmpE5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f4;_tmpE6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f5;_tmpE7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f1;_tmpE8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f2;_tmpE9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f3;_tmpEA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f4;_tmpEB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f5;_LL23: {void*ta=_tmpE2;void*qa=_tmpE3;void*ka=_tmpE4;void*ra=_tmpE5;void*za=_tmpE6;void*tb=_tmpE7;void*qb=_tmpE8;void*kb=_tmpE9;void*rb=_tmpEA;void*zb=_tmpEB;
# 631
struct _tuple13 _tmpEC=({struct _tuple13 _tmp3A0;({void*_tmp3F0=Cyc_Port_var();_tmp3A0.f1=_tmp3F0;}),({void*_tmp3EF=Cyc_Port_var();_tmp3A0.f2=_tmp3EF;}),({void*_tmp3EE=Cyc_Port_var();_tmp3A0.f3=_tmp3EE;}),({void*_tmp3ED=Cyc_Port_var();_tmp3A0.f4=_tmp3ED;}),({void*_tmp3EC=Cyc_Port_var();_tmp3A0.f5=_tmp3EC;});_tmp3A0;});struct _tuple13 _stmttmp6=_tmpEC;struct _tuple13 _tmpED=_stmttmp6;void*_tmpF2;void*_tmpF1;void*_tmpF0;void*_tmpEF;void*_tmpEE;_LL27: _tmpEE=_tmpED.f1;_tmpEF=_tmpED.f2;_tmpF0=_tmpED.f3;_tmpF1=_tmpED.f4;_tmpF2=_tmpED.f5;_LL28: {void*tc=_tmpEE;void*qc=_tmpEF;void*kc=_tmpF0;void*rc=_tmpF1;void*zc=_tmpF2;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpF3=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=11U,_tmpF4->f1=tc,_tmpF4->f2=qc,_tmpF4->f3=kc,_tmpF4->f4=rc,_tmpF4->f5=zc;_tmpF4;});struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p=_tmpF3;
Cyc_Port_leq(tc,ta);Cyc_Port_leq(tc,tb);
Cyc_Port_leq(qc,qa);Cyc_Port_leq(qc,qb);
Cyc_Port_leq(kc,ka);Cyc_Port_leq(kc,qb);
Cyc_Port_leq(rc,ra);Cyc_Port_leq(rc,rb);
Cyc_Port_leq(zc,za);Cyc_Port_leq(zc,zb);
us->hd=(void*)((void*)p);
return*uppers;}}}else{goto _LL24;}default: _LL24: _LL25:
 goto _LL1B;}_LL1B:;}}}
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
{struct Cyc_List_List*_tmpF7=*lowers;struct Cyc_List_List*ls=_tmpF7;for(0;(unsigned)ls;ls=ls->tl){
void*_tmpF8=Cyc_Port_compress_ct((void*)ls->hd);void*t2=_tmpF8;
if(t == t2)return*lowers;{
struct _tuple12 _tmpF9=({struct _tuple12 _tmp3A3;_tmp3A3.f1=t,_tmp3A3.f2=t2;_tmp3A3;});struct _tuple12 _stmttmp7=_tmpF9;struct _tuple12 _tmpFA=_stmttmp7;void*_tmp104;void*_tmp103;void*_tmp102;void*_tmp101;void*_tmp100;void*_tmpFF;void*_tmpFE;void*_tmpFD;void*_tmpFC;void*_tmpFB;if(((struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmpFA.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{switch(*((int*)_tmpFA.f1)){case 5U: switch(*((int*)_tmpFA.f2)){case 6U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;default: goto _LL26;}case 11U: switch(*((int*)_tmpFA.f2)){case 6U: _LL20: _LL21:
 goto _LL23;case 11U: _LL24: _tmpFB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f1;_tmpFC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f2;_tmpFD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f3;_tmpFE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f4;_tmpFF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f5;_tmp100=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f1;_tmp101=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f2;_tmp102=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f3;_tmp103=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f4;_tmp104=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f5;_LL25: {void*ta=_tmpFB;void*qa=_tmpFC;void*ka=_tmpFD;void*ra=_tmpFE;void*za=_tmpFF;void*tb=_tmp100;void*qb=_tmp101;void*kb=_tmp102;void*rb=_tmp103;void*zb=_tmp104;
# 684
struct _tuple13 _tmp105=({struct _tuple13 _tmp3A2;({void*_tmp3F5=Cyc_Port_var();_tmp3A2.f1=_tmp3F5;}),({void*_tmp3F4=Cyc_Port_var();_tmp3A2.f2=_tmp3F4;}),({void*_tmp3F3=Cyc_Port_var();_tmp3A2.f3=_tmp3F3;}),({void*_tmp3F2=Cyc_Port_var();_tmp3A2.f4=_tmp3F2;}),({void*_tmp3F1=Cyc_Port_var();_tmp3A2.f5=_tmp3F1;});_tmp3A2;});struct _tuple13 _stmttmp8=_tmp105;struct _tuple13 _tmp106=_stmttmp8;void*_tmp10B;void*_tmp10A;void*_tmp109;void*_tmp108;void*_tmp107;_LL29: _tmp107=_tmp106.f1;_tmp108=_tmp106.f2;_tmp109=_tmp106.f3;_tmp10A=_tmp106.f4;_tmp10B=_tmp106.f5;_LL2A: {void*tc=_tmp107;void*qc=_tmp108;void*kc=_tmp109;void*rc=_tmp10A;void*zc=_tmp10B;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp10C=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->tag=11U,_tmp10D->f1=tc,_tmp10D->f2=qc,_tmp10D->f3=kc,_tmp10D->f4=rc,_tmp10D->f5=zc;_tmp10D;});struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p=_tmp10C;
Cyc_Port_leq(ta,tc);Cyc_Port_leq(tb,tc);
Cyc_Port_leq(qa,qc);Cyc_Port_leq(qb,qc);
Cyc_Port_leq(ka,kc);Cyc_Port_leq(qb,kc);
Cyc_Port_leq(ra,rc);Cyc_Port_leq(rb,rc);
Cyc_Port_leq(za,zc);Cyc_Port_leq(zb,zc);
ls->hd=(void*)((void*)p);
return*lowers;}}default: goto _LL26;}case 12U: if(((struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmpFA.f2)->tag == 6U){_LL22: _LL23:
# 679
 return*lowers;}else{goto _LL26;}default: _LL26: _LL27:
# 693
 goto _LL19;}}_LL19:;}}}
# 696
return({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=t,_tmp10E->tl=*lowers;_tmp10E;});}
# 703
static int Cyc_Port_leq(void*t1,void*t2){
# 709
if(t1 == t2)return 1;
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{
struct _tuple12 _tmp10F=({struct _tuple12 _tmp3A4;_tmp3A4.f1=t1,_tmp3A4.f2=t2;_tmp3A4;});struct _tuple12 _stmttmp9=_tmp10F;struct _tuple12 _tmp110=_stmttmp9;struct Cyc_Port_Cvar*_tmp111;void*_tmp118;void*_tmp117;void*_tmp116;void*_tmp115;void*_tmp114;void*_tmp113;void*_tmp112;void*_tmp11E;void*_tmp11D;void*_tmp11C;void*_tmp11B;void*_tmp11A;void*_tmp119;void*_tmp128;void*_tmp127;void*_tmp126;void*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;void*_tmp11F;struct Cyc_Port_Cvar*_tmp129;struct Cyc_Port_Cvar*_tmp12B;struct Cyc_Port_Cvar*_tmp12A;struct _fat_ptr _tmp12C;struct _fat_ptr _tmp12E;struct _fat_ptr _tmp12D;switch(*((int*)_tmp110.f1)){case 7U: _LL1: _LL2:
 return 1;case 10U: switch(*((int*)_tmp110.f2)){case 10U: _LL3: _tmp12D=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp12E=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL4: {struct _fat_ptr n1=_tmp12D;struct _fat_ptr n2=_tmp12E;
return Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2)== 0;}case 7U: _LL5: _tmp12C=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_LL6: {struct _fat_ptr n1=_tmp12C;
return 0;}case 16U: goto _LL2D;default: goto _LL2F;}case 1U: switch(*((int*)_tmp110.f2)){case 0U: _LL7: _LL8:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 0U: switch(*((int*)_tmp110.f2)){case 1U: _LL9: _LLA:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 9U: switch(*((int*)_tmp110.f2)){case 8U: _LLB: _LLC:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 8U: switch(*((int*)_tmp110.f2)){case 9U: _LLD: _LLE:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 16U: switch(*((int*)_tmp110.f2)){case 0U: _LLF: _LL10:
 return 1;case 4U: _LL11: _LL12:
 return 1;case 16U: _LL29: _tmp12A=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp12B=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL2A: {struct Cyc_Port_Cvar*v1=_tmp12A;struct Cyc_Port_Cvar*v2=_tmp12B;
# 739
({struct Cyc_List_List*_tmp3F6=Cyc_Port_filter_self(t1,v1->uppers);v1->uppers=_tmp3F6;});
({struct Cyc_List_List*_tmp3F7=Cyc_Port_filter_self(t2,v2->lowers);v2->lowers=_tmp3F7;});
({struct Cyc_List_List*_tmp3F8=Cyc_Port_insert_upper(t1,t2,& v1->uppers);v1->uppers=_tmp3F8;});
({struct Cyc_List_List*_tmp3F9=Cyc_Port_insert_lower(t2,t1,& v2->lowers);v2->lowers=_tmp3F9;});
return 1;}default: _LL2B: _tmp129=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_LL2C: {struct Cyc_Port_Cvar*v1=_tmp129;
# 745
({struct Cyc_List_List*_tmp3FA=Cyc_Port_filter_self(t1,v1->uppers);v1->uppers=_tmp3FA;});
({struct Cyc_List_List*_tmp3FB=Cyc_Port_insert_upper(t1,t2,& v1->uppers);v1->uppers=_tmp3FB;});
return 1;}}case 4U: _LL13: _LL14:
# 722
 return 0;case 5U: switch(*((int*)_tmp110.f2)){case 6U: _LL15: _LL16:
 return 1;case 11U: _LL17: _LL18:
 return 1;case 4U: _LL19: _LL1A:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 11U: switch(*((int*)_tmp110.f2)){case 6U: _LL1B: _LL1C:
 return 1;case 4U: _LL1D: _LL1E:
 return 1;case 11U: _LL23: _tmp11F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp120=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp121=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp122=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f4;_tmp123=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f5;_tmp124=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp125=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp126=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_tmp127=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f4;_tmp128=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f5;_LL24: {void*t1=_tmp11F;void*q1=_tmp120;void*k1=_tmp121;void*r1=_tmp122;void*z1=_tmp123;void*t2=_tmp124;void*q2=_tmp125;void*k2=_tmp126;void*r2=_tmp127;void*z2=_tmp128;
# 731
return(((Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_unifies(k1,k2))&& Cyc_Port_leq(r1,r2))&&
 Cyc_Port_leq(z1,z2);}case 16U: goto _LL2D;default: goto _LL2F;}case 12U: switch(*((int*)_tmp110.f2)){case 6U: _LL1F: _LL20:
# 728
 return 1;case 4U: _LL21: _LL22:
 return 1;case 12U: _LL25: _tmp119=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp11A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp11B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp11C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp11D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp11E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_LL26: {void*t1=_tmp119;void*q1=_tmp11A;void*z1=_tmp11B;void*t2=_tmp11C;void*q2=_tmp11D;void*z2=_tmp11E;
# 734
return(Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_leq(z1,z2);}case 11U: _LL27: _tmp112=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp113=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp114=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp115=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp116=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp117=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_tmp118=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f5;_LL28: {void*t1=_tmp112;void*q1=_tmp113;void*z1=_tmp114;void*t2=_tmp115;void*q2=_tmp116;void*k2=_tmp117;void*z2=_tmp118;
# 736
return((Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,k2))&&
 Cyc_Port_leq(z1,z2);}case 16U: goto _LL2D;default: goto _LL2F;}default: if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->tag == 16U){_LL2D: _tmp111=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL2E: {struct Cyc_Port_Cvar*v2=_tmp111;
# 749
({struct Cyc_List_List*_tmp3FC=Cyc_Port_filter_self(t2,v2->lowers);v2->lowers=_tmp3FC;});
({struct Cyc_List_List*_tmp3FD=Cyc_Port_insert_lower(t2,t1,& v2->lowers);v2->lowers=_tmp3FD;});
return 1;}}else{_LL2F: _LL30:
 return Cyc_Port_unifies(t1,t2);}}_LL0:;}}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 783
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos =0U,Cyc_Port_FnArg_pos =1U,Cyc_Port_FnBody_pos =2U,Cyc_Port_Toplevel_pos =3U};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 796
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv (void){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmp130=_cycalloc(sizeof(*_tmp130));({struct Cyc_Dict_Dict _tmp402=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->typedef_dict=_tmp402;}),({
struct Cyc_Dict_Dict _tmp401=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->struct_dict=_tmp401;}),({
struct Cyc_Dict_Dict _tmp400=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->union_dict=_tmp400;}),_tmp130->qualifier_edits=0,_tmp130->pointer_edits=0,_tmp130->zeroterm_edits=0,_tmp130->vardecl_locs=0,({
# 804
struct Cyc_Hashtable_Table*_tmp3FF=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(unsigned,unsigned),int(*hash)(unsigned)))Cyc_Hashtable_create)(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);_tmp130->varusage_tab=_tmp3FF;}),_tmp130->edits=0,_tmp130->porting=0,({
# 807
void*_tmp3FE=Cyc_Port_void_ct();_tmp130->return_type=_tmp3FE;});_tmp130;});
return({struct Cyc_Port_Cenv*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->gcenv=g,_tmp12F->cpos=Cyc_Port_Toplevel_pos,({
# 810
struct Cyc_Dict_Dict _tmp403=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp12F->var_dict=_tmp403;});_tmp12F;});}
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
{struct _tuple12 _tmp134=*((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple12 _stmttmpA=_tmp134;struct _tuple12 _tmp135=_stmttmpA;void*_tmp136;_LL1: _tmp136=_tmp135.f1;_LL2: {void*t=_tmp136;
void*_tmp137=t;_npop_handler(0U);return _tmp137;}}
# 830
;_pop_handler();}else{void*_tmp132=(void*)Cyc_Core_get_exn_thrown();void*_tmp138=_tmp132;void*_tmp139;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp138)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 837
 return Cyc_Absyn_sint_type;}else{_LL6: _tmp139=_tmp138;_LL7: {void*exn=_tmp139;(int)_rethrow(exn);}}_LL3:;}}}
# 842
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp13A;_push_handler(& _tmp13A);{int _tmp13C=0;if(setjmp(_tmp13A.handler))_tmp13C=1;if(!_tmp13C){
{struct _tuple12 _tmp13D=*((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple12 _stmttmpB=_tmp13D;struct _tuple12 _tmp13E=_stmttmpB;void*_tmp13F;_LL1: _tmp13F=_tmp13E.f2;_LL2: {void*ct=_tmp13F;
void*_tmp140=ct;_npop_handler(0U);return _tmp140;}}
# 844
;_pop_handler();}else{void*_tmp13B=(void*)Cyc_Core_get_exn_thrown();void*_tmp141=_tmp13B;void*_tmp142;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp141)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 851
 return Cyc_Port_var();}else{_LL6: _tmp142=_tmp141;_LL7: {void*exn=_tmp142;(int)_rethrow(exn);}}_LL3:;}}}
# 857
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 859
struct _tuple11**_tmp143=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);struct _tuple11**popt=_tmp143;
if((unsigned)popt)
return*popt;else{
# 863
struct Cyc_Absyn_Aggrdecl*_tmp144=({struct Cyc_Absyn_Aggrdecl*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->kind=Cyc_Absyn_StructA,_tmp146->sc=Cyc_Absyn_Public,_tmp146->name=n,_tmp146->tvs=0,_tmp146->impl=0,_tmp146->attributes=0,_tmp146->expected_mem_kind=0;_tmp146;});struct Cyc_Absyn_Aggrdecl*ad=_tmp144;
# 866
struct _tuple11*p=({struct _tuple11*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=ad,_tmp145->f2=0;_tmp145;});
({struct Cyc_Dict_Dict _tmp404=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);(env->gcenv)->struct_dict=_tmp404;});
return p;}}
# 874
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 876
struct _tuple11**_tmp147=((struct _tuple11**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);struct _tuple11**popt=_tmp147;
if((unsigned)popt)
return*popt;else{
# 880
struct Cyc_Absyn_Aggrdecl*_tmp148=({struct Cyc_Absyn_Aggrdecl*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->kind=Cyc_Absyn_UnionA,_tmp14A->sc=Cyc_Absyn_Public,_tmp14A->name=n,_tmp14A->tvs=0,_tmp14A->impl=0,_tmp14A->attributes=0,_tmp14A->expected_mem_kind=0;_tmp14A;});struct Cyc_Absyn_Aggrdecl*ad=_tmp148;
# 883
struct _tuple11*p=({struct _tuple11*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->f1=ad,_tmp149->f2=0;_tmp149;});
({struct Cyc_Dict_Dict _tmp405=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);(env->gcenv)->union_dict=_tmp405;});
return p;}}struct _tuple14{struct _tuple12 f1;unsigned f2;};struct _tuple15{void*f1;struct _tuple12*f2;unsigned f3;};
# 890
static struct _tuple14 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp14B;_push_handler(& _tmp14B);{int _tmp14D=0;if(setjmp(_tmp14B.handler))_tmp14D=1;if(!_tmp14D){
{struct _tuple15 _tmp14E=*((struct _tuple15*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple15 _stmttmpC=_tmp14E;struct _tuple15 _tmp14F=_stmttmpC;unsigned _tmp151;struct _tuple12*_tmp150;_LL1: _tmp150=_tmp14F.f2;_tmp151=_tmp14F.f3;_LL2: {struct _tuple12*p=_tmp150;unsigned loc=_tmp151;
struct _tuple14 _tmp152=({struct _tuple14 _tmp3A5;_tmp3A5.f1=*p,_tmp3A5.f2=loc;_tmp3A5;});_npop_handler(0U);return _tmp152;}}
# 892
;_pop_handler();}else{void*_tmp14C=(void*)Cyc_Core_get_exn_thrown();void*_tmp153=_tmp14C;void*_tmp154;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp153)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 899
 return({struct _tuple14 _tmp3A6;({void*_tmp407=Cyc_Port_var();(_tmp3A6.f1).f1=_tmp407;}),({void*_tmp406=Cyc_Port_var();(_tmp3A6.f1).f2=_tmp406;}),_tmp3A6.f2=0U;_tmp3A6;});}else{_LL6: _tmp154=_tmp153;_LL7: {void*exn=_tmp154;(int)_rethrow(exn);}}_LL3:;}}}
# 902
static struct _tuple15*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((struct _tuple15*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}
# 906
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(env->var_dict,x);}
# 910
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=1;if(!_tmp157){
{struct _tuple15 _tmp158=*((struct _tuple15*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple15 _stmttmpD=_tmp158;struct _tuple15 _tmp159=_stmttmpD;void*_tmp15A;_LL1: _tmp15A=_tmp159.f1;_LL2: {void*t=_tmp15A;
LOOP: {void*_tmp15B=t;struct _tuple0*_tmp15C;switch(*((int*)_tmp15B)){case 8U: _LL4: _tmp15C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f1;_LL5: {struct _tuple0*n=_tmp15C;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 5U: _LL6: _LL7: {
int _tmp15D=1;_npop_handler(0U);return _tmp15D;}default: _LL8: _LL9: {
int _tmp15E=0;_npop_handler(0U);return _tmp15E;}}_LL3:;}}}
# 912
;_pop_handler();}else{void*_tmp156=(void*)Cyc_Core_get_exn_thrown();void*_tmp15F=_tmp156;void*_tmp160;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15F)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 923
 return 0;}else{_LLD: _tmp160=_tmp15F;_LLE: {void*exn=_tmp160;(int)_rethrow(exn);}}_LLA:;}}}
# 927
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp161;_push_handler(& _tmp161);{int _tmp163=0;if(setjmp(_tmp161.handler))_tmp163=1;if(!_tmp163){
{struct _tuple15 _tmp164=*((struct _tuple15*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple15 _stmttmpE=_tmp164;struct _tuple15 _tmp165=_stmttmpE;void*_tmp166;_LL1: _tmp166=_tmp165.f1;_LL2: {void*t=_tmp166;
LOOP: {void*_tmp167=t;struct _tuple0*_tmp168;switch(*((int*)_tmp167)){case 8U: _LL4: _tmp168=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp167)->f1;_LL5: {struct _tuple0*n=_tmp168;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 4U: _LL6: _LL7: {
int _tmp169=1;_npop_handler(0U);return _tmp169;}default: _LL8: _LL9: {
int _tmp16A=0;_npop_handler(0U);return _tmp16A;}}_LL3:;}}}
# 929
;_pop_handler();}else{void*_tmp162=(void*)Cyc_Core_get_exn_thrown();void*_tmp16B=_tmp162;void*_tmp16C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16B)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 940
 return 0;}else{_LLD: _tmp16C=_tmp16B;_LLE: {void*exn=_tmp16C;(int)_rethrow(exn);}}_LLA:;}}}
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
struct Cyc_Dict_Dict _tmp40B=({struct Cyc_Dict_Dict _tmp40A=env->var_dict;struct _tuple0*_tmp409=x;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(_tmp40A,_tmp409,({struct _tuple15*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->f1=t,({struct _tuple12*_tmp408=({struct _tuple12*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->f1=qual,_tmp16E->f2=ctype;_tmp16E;});_tmp16F->f2=_tmp408;}),_tmp16F->f3=loc;_tmp16F;}));});_tmp170->var_dict=_tmp40B;}),_tmp170->cpos=env->cpos;_tmp170;});}
# 964
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
({struct Cyc_Dict_Dict _tmp40E=({struct Cyc_Dict_Dict _tmp40D=(env->gcenv)->typedef_dict;struct _tuple0*_tmp40C=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp40D,_tmp40C,({struct _tuple12*_tmp171=_cycalloc(sizeof(*_tmp171));
_tmp171->f1=t,_tmp171->f2=ct;_tmp171;}));});
# 965
(env->gcenv)->typedef_dict=_tmp40E;});}struct _tuple16{struct _tuple0*f1;unsigned f2;void*f3;};
# 968
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned loc,void*type,struct Cyc_Absyn_Exp*init){
({struct Cyc_List_List*_tmp410=({struct Cyc_List_List*_tmp173=_cycalloc(sizeof(*_tmp173));({struct _tuple16*_tmp40F=({struct _tuple16*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->f1=x,_tmp172->f2=loc,_tmp172->f3=type;_tmp172;});_tmp173->hd=_tmp40F;}),_tmp173->tl=(env->gcenv)->vardecl_locs;_tmp173;});(env->gcenv)->vardecl_locs=_tmp410;});
({struct Cyc_Hashtable_Table*_tmp412=(env->gcenv)->varusage_tab;unsigned _tmp411=loc;((void(*)(struct Cyc_Hashtable_Table*t,unsigned key,struct Cyc_Port_VarUsage*val))Cyc_Hashtable_insert)(_tmp412,_tmp411,({struct Cyc_Port_VarUsage*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->address_taken=0,_tmp174->init=init,_tmp174->usage_locs=0;_tmp174;}));});}
# 972
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned declloc,unsigned useloc,int takeaddress){
struct Cyc_Port_VarUsage*varusage=0;
if(((int(*)(struct Cyc_Hashtable_Table*t,unsigned key,struct Cyc_Port_VarUsage**data))Cyc_Hashtable_try_lookup)((env->gcenv)->varusage_tab,declloc,& varusage)){
if(takeaddress)((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken=1;{
struct Cyc_List_List*_tmp175=((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs;struct Cyc_List_List*l=_tmp175;
({struct Cyc_List_List*_tmp413=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=(void*)useloc,_tmp176->tl=l;_tmp176;});((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs=_tmp413;});}}}struct _tuple17{void*f1;void*f2;unsigned f3;};
# 983
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned loc){
# 985
({struct Cyc_List_List*_tmp415=({struct Cyc_List_List*_tmp178=_cycalloc(sizeof(*_tmp178));({struct _tuple17*_tmp414=({struct _tuple17*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=new_qual,_tmp177->f2=orig_qual,_tmp177->f3=loc;_tmp177;});_tmp178->hd=_tmp414;}),_tmp178->tl=(env->gcenv)->qualifier_edits;_tmp178;});(env->gcenv)->qualifier_edits=_tmp415;});}
# 991
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned loc){
# 993
({struct Cyc_List_List*_tmp417=({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));({struct _tuple17*_tmp416=({struct _tuple17*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->f1=new_ptr,_tmp179->f2=orig_ptr,_tmp179->f3=loc;_tmp179;});_tmp17A->hd=_tmp416;}),_tmp17A->tl=(env->gcenv)->pointer_edits;_tmp17A;});(env->gcenv)->pointer_edits=_tmp417;});}
# 997
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned loc){
# 999
({struct Cyc_List_List*_tmp419=({struct Cyc_List_List*_tmp17C=_cycalloc(sizeof(*_tmp17C));({struct _tuple17*_tmp418=({struct _tuple17*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=new_zt,_tmp17B->f2=orig_zt,_tmp17B->f3=loc;_tmp17B;});_tmp17C->hd=_tmp418;}),_tmp17C->tl=(env->gcenv)->zeroterm_edits;_tmp17C;});(env->gcenv)->zeroterm_edits=_tmp419;});}
# 1005
static void*Cyc_Port_main_type (void){
struct _tuple8*arg1=({struct _tuple8*_tmp180=_cycalloc(sizeof(*_tmp180));
_tmp180->f1=0,({struct Cyc_Absyn_Tqual _tmp41A=Cyc_Absyn_empty_tqual(0U);_tmp180->f2=_tmp41A;}),_tmp180->f3=Cyc_Absyn_sint_type;_tmp180;});
struct _tuple8*arg2=({struct _tuple8*_tmp17F=_cycalloc(sizeof(*_tmp17F));
_tmp17F->f1=0,({struct Cyc_Absyn_Tqual _tmp41F=Cyc_Absyn_empty_tqual(0U);_tmp17F->f2=_tmp41F;}),({
void*_tmp41E=({void*_tmp41D=Cyc_Absyn_string_type(Cyc_Absyn_wildtyp(0));void*_tmp41C=Cyc_Absyn_wildtyp(0);struct Cyc_Absyn_Tqual _tmp41B=
Cyc_Absyn_empty_tqual(0U);
# 1010
Cyc_Absyn_fatptr_type(_tmp41D,_tmp41C,_tmp41B,
Cyc_Tcutil_any_bool(0));});
# 1010
_tmp17F->f3=_tmp41E;});_tmp17F;});
# 1012
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->tag=5U,(_tmp17E->f1).tvars=0,(_tmp17E->f1).effect=0,({
struct Cyc_Absyn_Tqual _tmp421=Cyc_Absyn_empty_tqual(0U);(_tmp17E->f1).ret_tqual=_tmp421;}),(_tmp17E->f1).ret_type=Cyc_Absyn_sint_type,({
# 1015
struct Cyc_List_List*_tmp420=({struct _tuple8*_tmp17D[2U];_tmp17D[0]=arg1,_tmp17D[1]=arg2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17D,sizeof(struct _tuple8*),2U));});(_tmp17E->f1).args=_tmp420;}),(_tmp17E->f1).c_varargs=0,(_tmp17E->f1).cyc_varargs=0,(_tmp17E->f1).rgn_po=0,(_tmp17E->f1).attributes=0,(_tmp17E->f1).requires_clause=0,(_tmp17E->f1).requires_relns=0,(_tmp17E->f1).ensures_clause=0,(_tmp17E->f1).ensures_relns=0,(_tmp17E->f1).return_value=0;_tmp17E;});}
# 1024
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);
# 1027
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv (void){
struct Cyc_Port_Cenv*_tmp181=Cyc_Port_empty_cenv();struct Cyc_Port_Cenv*e=_tmp181;
# 1030
e=({struct Cyc_Port_Cenv*_tmp428=e;struct _tuple0*_tmp427=({struct _tuple0*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp423=({struct _fat_ptr*_tmp183=_cycalloc(sizeof(*_tmp183));({struct _fat_ptr _tmp422=({const char*_tmp182="main";_tag_fat(_tmp182,sizeof(char),5U);});*_tmp183=_tmp422;});_tmp183;});_tmp184->f2=_tmp423;});_tmp184;});void*_tmp426=Cyc_Port_main_type();void*_tmp425=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp428,_tmp427,_tmp426,_tmp425,({
struct Cyc_Port_Cenv*_tmp424=e;Cyc_Port_type_to_ctype(_tmp424,Cyc_Port_main_type());}),0U);});
return e;}
# 1038
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp185=Cyc_Port_compress_ct(t);void*_stmttmpF=_tmp185;void*_tmp186=_stmttmpF;struct Cyc_List_List*_tmp188;void*_tmp187;void*_tmp18B;void*_tmp18A;void*_tmp189;void*_tmp190;void*_tmp18F;void*_tmp18E;void*_tmp18D;void*_tmp18C;struct _fat_ptr*_tmp191;switch(*((int*)_tmp186)){case 0U: _LL1: _LL2:
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
 return;case 10U: _LL1B: _tmp191=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp186)->f1;_LL1C: {struct _fat_ptr*n=_tmp191;
# 1054
if(!((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*cts,n))
({struct Cyc_Dict_Dict _tmp42B=({struct Cyc_Dict_Dict _tmp42A=*cts;struct _fat_ptr*_tmp429=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,int*v))Cyc_Dict_insert)(_tmp42A,_tmp429,({int*_tmp192=_cycalloc_atomic(sizeof(*_tmp192));*_tmp192=0;_tmp192;}));});*cts=_tmp42B;});{
int*_tmp193=((int*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*cts,n);int*p=_tmp193;
*p=*p + 1;
return;}}case 11U: _LL1D: _tmp18C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp18D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f2;_tmp18E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f3;_tmp18F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f4;_tmp190=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f5;_LL1E: {void*t1=_tmp18C;void*t2=_tmp18D;void*t3=_tmp18E;void*t4=_tmp18F;void*zt=_tmp190;
# 1060
Cyc_Port_region_counts(cts,t1);
Cyc_Port_region_counts(cts,t4);
return;}case 12U: _LL1F: _tmp189=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp18A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f2;_tmp18B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f3;_LL20: {void*t1=_tmp189;void*t2=_tmp18A;void*zt=_tmp18B;
# 1064
Cyc_Port_region_counts(cts,t1);
return;}default: _LL21: _tmp187=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp188=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp186)->f2;_LL22: {void*t1=_tmp187;struct Cyc_List_List*ts=_tmp188;
# 1067
Cyc_Port_region_counts(cts,t1);
for(0;(unsigned)ts;ts=ts->tl){Cyc_Port_region_counts(cts,(void*)ts->hd);}
return;}}_LL0:;}
# 1078
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1080
c=Cyc_Port_compress_ct(c);{
struct _tuple12 _tmp194=({struct _tuple12 _tmp3A7;_tmp3A7.f1=t,_tmp3A7.f2=c;_tmp3A7;});struct _tuple12 _stmttmp10=_tmp194;struct _tuple12 _tmp195=_stmttmp10;struct Cyc_List_List*_tmp199;void*_tmp198;struct Cyc_List_List*_tmp197;void*_tmp196;void*_tmp19B;void*_tmp19A;void*_tmp1A0;void*_tmp19F;struct Cyc_Absyn_PtrLoc*_tmp19E;void*_tmp19D;void*_tmp19C;switch(*((int*)_tmp195.f1)){case 3U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 11U){_LL1: _tmp19C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).elt_type;_tmp19D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).ptr_atts).rgn;_tmp19E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).ptr_atts).ptrloc;_tmp19F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_tmp1A0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->f4;_LL2: {void*et=_tmp19C;void*rt=_tmp19D;struct Cyc_Absyn_PtrLoc*ploc=_tmp19E;void*ec=_tmp19F;void*rc=_tmp1A0;
# 1083
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);{
unsigned _tmp1A1=(unsigned)ploc?ploc->rgn_loc:(unsigned)0;unsigned loc=_tmp1A1;
rc=Cyc_Port_compress_ct(rc);
# 1100 "port.cyc"
({struct Cyc_List_List*_tmp42F=({struct Cyc_List_List*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));
({struct Cyc_Port_Edit*_tmp42E=({struct Cyc_Port_Edit*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->loc=loc,({struct _fat_ptr _tmp42D=({const char*_tmp1A2="`H ";_tag_fat(_tmp1A2,sizeof(char),4U);});_tmp1A4->old_string=_tmp42D;}),({struct _fat_ptr _tmp42C=({const char*_tmp1A3="";_tag_fat(_tmp1A3,sizeof(char),1U);});_tmp1A4->new_string=_tmp42C;});_tmp1A4;});_tmp1A5->hd=_tmp42E;}),_tmp1A5->tl=(env->gcenv)->edits;_tmp1A5;});
# 1100
(env->gcenv)->edits=_tmp42F;});
# 1103
goto _LL0;}}}else{goto _LL7;}case 4U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 12U){_LL3: _tmp19A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195.f1)->f1).elt_type;_tmp19B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_LL4: {void*et=_tmp19A;void*ec=_tmp19B;
# 1105
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);
goto _LL0;}}else{goto _LL7;}case 5U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 15U){_LL5: _tmp196=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp195.f1)->f1).ret_type;_tmp197=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp195.f1)->f1).args;_tmp198=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_tmp199=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->f2;_LL6: {void*rt=_tmp196;struct Cyc_List_List*argst=_tmp197;void*rc=_tmp198;struct Cyc_List_List*argsc=_tmp199;
# 1108
Cyc_Port_register_rgns(env,counts,1,rt,rc);
for(0;argst != 0 && argsc != 0;(argst=argst->tl,argsc=argsc->tl)){
struct _tuple8 _tmp1A6=*((struct _tuple8*)argst->hd);struct _tuple8 _stmttmp11=_tmp1A6;struct _tuple8 _tmp1A7=_stmttmp11;void*_tmp1A8;_LLA: _tmp1A8=_tmp1A7.f3;_LLB: {void*at=_tmp1A8;
void*_tmp1A9=(void*)argsc->hd;void*ac=_tmp1A9;
Cyc_Port_register_rgns(env,counts,0,at,ac);}}
# 1114
goto _LL0;}}else{goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 1121
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int takeaddress);
# 1126
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1AA=t;struct _tuple0*_tmp1AB;switch(*((int*)_tmp1AA)){case 8U: _LL1: _tmp1AB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1AA)->f1;_LL2: {struct _tuple0*n=_tmp1AB;
# 1129
(*n).f1=Cyc_Absyn_Loc_n;
return({struct Cyc_Port_Cenv*_tmp430=env;Cyc_Port_is_char(_tmp430,Cyc_Port_lookup_typedef(env,n));});}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->f2 == Cyc_Absyn_Char_sz){_LL3: _LL4:
 return 1;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 1137
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1AC=t;struct Cyc_List_List*_tmp1AE;void*_tmp1AD;unsigned _tmp1B2;void*_tmp1B1;struct Cyc_Absyn_Tqual _tmp1B0;void*_tmp1AF;struct Cyc_Absyn_PtrLoc*_tmp1B9;void*_tmp1B8;void*_tmp1B7;void*_tmp1B6;void*_tmp1B5;struct Cyc_Absyn_Tqual _tmp1B4;void*_tmp1B3;struct Cyc_List_List*_tmp1BA;union Cyc_Absyn_AggrInfo _tmp1BB;struct _tuple0*_tmp1BC;switch(*((int*)_tmp1AC)){case 8U: _LL1: _tmp1BC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1AC)->f1;_LL2: {struct _tuple0*n=_tmp1BC;
# 1140
(*n).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,n);}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)){case 0U: _LL3: _LL4:
 return Cyc_Port_void_ct();case 1U: _LL7: _LL8:
# 1219
 goto _LLA;case 2U: _LL9: _LLA:
 return Cyc_Port_arith_ct();case 20U: _LLF: _tmp1BB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL10: {union Cyc_Absyn_AggrInfo ai=_tmp1BB;
# 1262
union Cyc_Absyn_AggrInfo _tmp1D9=ai;struct Cyc_Absyn_Aggrdecl**_tmp1DA;struct _tuple0*_tmp1DB;struct _tuple0*_tmp1DC;if((_tmp1D9.UnknownAggr).tag == 1){if(((_tmp1D9.UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL38: _tmp1DC=((_tmp1D9.UnknownAggr).val).f2;_LL39: {struct _tuple0*n=_tmp1DC;
# 1264
(*n).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp1DD=Cyc_Port_lookup_struct_decl(env,n);struct _tuple11*p=_tmp1DD;
return Cyc_Port_known_aggr_ct(p);}}}else{_LL3A: _tmp1DB=((_tmp1D9.UnknownAggr).val).f2;_LL3B: {struct _tuple0*n=_tmp1DB;
# 1268
(*n).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp1DE=Cyc_Port_lookup_union_decl(env,n);struct _tuple11*p=_tmp1DE;
return Cyc_Port_known_aggr_ct(p);}}}}else{_LL3C: _tmp1DA=(_tmp1D9.KnownAggr).val;_LL3D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp1DA;
# 1272
struct Cyc_Absyn_Aggrdecl*_tmp1DF=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp1DF;
enum Cyc_Absyn_AggrKind _tmp1E0=ad->kind;enum Cyc_Absyn_AggrKind _stmttmp17=_tmp1E0;enum Cyc_Absyn_AggrKind _tmp1E1=_stmttmp17;if(_tmp1E1 == Cyc_Absyn_StructA){_LL3F: _LL40: {
# 1275
struct _tuple11*_tmp1E2=Cyc_Port_lookup_struct_decl(env,ad->name);struct _tuple11*p=_tmp1E2;
return Cyc_Port_known_aggr_ct(p);}}else{_LL41: _LL42: {
# 1278
struct _tuple11*_tmp1E3=Cyc_Port_lookup_union_decl(env,ad->name);struct _tuple11*p=_tmp1E3;
return Cyc_Port_known_aggr_ct(p);}}_LL3E:;}}_LL37:;}case 15U: _LL11: _LL12:
# 1282
 return Cyc_Port_arith_ct();case 16U: _LL13: _tmp1BA=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL14: {struct Cyc_List_List*fs=_tmp1BA;
# 1285
for(0;(unsigned)fs;fs=fs->tl){
(*((struct Cyc_Absyn_Enumfield*)fs->hd)->name).f1=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_tmp434=env;struct _tuple0*_tmp433=((struct Cyc_Absyn_Enumfield*)fs->hd)->name;void*_tmp432=Cyc_Absyn_sint_type;void*_tmp431=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp434,_tmp433,_tmp432,_tmp431,Cyc_Port_arith_ct(),0U);});}
# 1289
return Cyc_Port_arith_ct();}default: goto _LL15;}case 3U: _LL5: _tmp1B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).elt_type;_tmp1B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).elt_tq;_tmp1B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).rgn;_tmp1B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).nullable;_tmp1B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).bounds;_tmp1B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).zero_term;_tmp1B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).ptrloc;_LL6: {void*et=_tmp1B3;struct Cyc_Absyn_Tqual tq=_tmp1B4;void*r=_tmp1B5;void*n=_tmp1B6;void*b=_tmp1B7;void*zt=_tmp1B8;struct Cyc_Absyn_PtrLoc*loc=_tmp1B9;
# 1144
unsigned _tmp1BD=(unsigned)loc?loc->ptr_loc:(unsigned)0;unsigned ptr_loc=_tmp1BD;
unsigned _tmp1BE=(unsigned)loc?loc->rgn_loc:(unsigned)0;unsigned rgn_loc=_tmp1BE;
unsigned _tmp1BF=(unsigned)loc?loc->zt_loc:(unsigned)0;unsigned zt_loc=_tmp1BF;
# 1150
void*_tmp1C0=Cyc_Port_type_to_ctype(env,et);void*cet=_tmp1C0;
void*new_rgn;
# 1153
{void*_tmp1C1=r;struct _fat_ptr*_tmp1C2;switch(*((int*)_tmp1C1)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C1)->f1)->tag == 5U){_LL18: _LL19:
# 1155
 new_rgn=Cyc_Port_heap_ct();goto _LL17;}else{goto _LL1E;}case 2U: _LL1A: _tmp1C2=(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C1)->f1)->name;_LL1B: {struct _fat_ptr*name=_tmp1C2;
# 1157
new_rgn=Cyc_Port_rgnvar_ct(name);goto _LL17;}case 1U: _LL1C: _LL1D:
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
int _tmp1C3=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b)== 0;int orig_fat=_tmp1C3;
int orig_zt;
{void*_tmp1C4=Cyc_Tcutil_compress(zt);void*_stmttmp12=_tmp1C4;void*_tmp1C5=_stmttmp12;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C5)->tag == 1U){_LL21: _LL22:
 orig_zt=Cyc_Port_is_char(env,et);goto _LL20;}else{_LL23: _LL24:
 orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL20;}_LL20:;}
# 1177
if((env->gcenv)->porting){
void*_tmp1C6=Cyc_Port_var();void*cqv=_tmp1C6;
# 1182
({struct Cyc_Port_Cenv*_tmp437=env;void*_tmp436=cqv;void*_tmp435=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp437,_tmp436,_tmp435,tq.loc);});
# 1185
if(tq.print_const)({void*_tmp438=cqv;Cyc_Port_unify_ct(_tmp438,Cyc_Port_const_ct());});{
# 1191
void*_tmp1C7=Cyc_Port_var();void*cbv=_tmp1C7;
# 1194
({struct Cyc_Port_Cenv*_tmp43B=env;void*_tmp43A=cbv;void*_tmp439=orig_fat?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();Cyc_Port_register_ptr_cvars(_tmp43B,_tmp43A,_tmp439,ptr_loc);});
# 1196
if(orig_fat)({void*_tmp43C=cbv;Cyc_Port_unify_ct(_tmp43C,Cyc_Port_fat_ct());});{
void*_tmp1C8=Cyc_Port_var();void*czv=_tmp1C8;
# 1200
({struct Cyc_Port_Cenv*_tmp43F=env;void*_tmp43E=czv;void*_tmp43D=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp43F,_tmp43E,_tmp43D,zt_loc);});
# 1202
{void*_tmp1C9=Cyc_Tcutil_compress(zt);void*_stmttmp13=_tmp1C9;void*_tmp1CA=_stmttmp13;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CA)->tag == 1U){_LL26: _LL27:
 goto _LL25;}else{_LL28: _LL29:
# 1205
({void*_tmp440=czv;Cyc_Port_unify_ct(_tmp440,Cyc_Tcutil_force_type2bool(0,zt)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL25;}_LL25:;}
# 1211
return Cyc_Port_ptr_ct(cet,cqv,cbv,new_rgn,czv);}}}else{
# 1215
return({void*_tmp444=cet;void*_tmp443=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();void*_tmp442=
orig_fat?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();
# 1215
void*_tmp441=new_rgn;Cyc_Port_ptr_ct(_tmp444,_tmp443,_tmp442,_tmp441,
# 1217
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}}case 4U: _LLB: _tmp1AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).elt_type;_tmp1B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).tq;_tmp1B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).zero_term;_tmp1B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).zt_loc;_LLC: {void*et=_tmp1AF;struct Cyc_Absyn_Tqual tq=_tmp1B0;void*zt=_tmp1B1;unsigned ztloc=_tmp1B2;
# 1223
void*_tmp1CB=Cyc_Port_type_to_ctype(env,et);void*cet=_tmp1CB;
int orig_zt;
{void*_tmp1CC=Cyc_Tcutil_compress(zt);void*_stmttmp14=_tmp1CC;void*_tmp1CD=_stmttmp14;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CD)->tag == 1U){_LL2B: _LL2C:
 orig_zt=0;goto _LL2A;}else{_LL2D: _LL2E:
 orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL2A;}_LL2A:;}
# 1229
if((env->gcenv)->porting){
void*_tmp1CE=Cyc_Port_var();void*cqv=_tmp1CE;
({struct Cyc_Port_Cenv*_tmp447=env;void*_tmp446=cqv;void*_tmp445=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp447,_tmp446,_tmp445,tq.loc);});{
# 1238
void*_tmp1CF=Cyc_Port_var();void*czv=_tmp1CF;
({struct Cyc_Port_Cenv*_tmp44A=env;void*_tmp449=czv;void*_tmp448=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp44A,_tmp449,_tmp448,ztloc);});
# 1241
{void*_tmp1D0=Cyc_Tcutil_compress(zt);void*_stmttmp15=_tmp1D0;void*_tmp1D1=_stmttmp15;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D1)->tag == 1U){_LL30: _LL31:
 goto _LL2F;}else{_LL32: _LL33:
# 1244
({void*_tmp44B=czv;Cyc_Port_unify_ct(_tmp44B,Cyc_Tcutil_force_type2bool(0,zt)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL2F;}_LL2F:;}
# 1247
return Cyc_Port_array_ct(cet,cqv,czv);}}else{
# 1249
return({void*_tmp44D=cet;void*_tmp44C=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_array_ct(_tmp44D,_tmp44C,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}case 5U: _LLD: _tmp1AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ret_type;_tmp1AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).args;_LLE: {void*rt=_tmp1AD;struct Cyc_List_List*args=_tmp1AE;
# 1253
void*_tmp1D2=({struct Cyc_Port_Cenv*_tmp44E=Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos);Cyc_Port_type_to_ctype(_tmp44E,rt);});void*crt=_tmp1D2;
struct Cyc_Port_Cenv*_tmp1D3=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);struct Cyc_Port_Cenv*env_arg=_tmp1D3;
struct Cyc_List_List*cargs=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp1D4=*((struct _tuple8*)args->hd);struct _tuple8 _stmttmp16=_tmp1D4;struct _tuple8 _tmp1D5=_stmttmp16;void*_tmp1D7;struct Cyc_Absyn_Tqual _tmp1D6;_LL35: _tmp1D6=_tmp1D5.f2;_tmp1D7=_tmp1D5.f3;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp1D6;void*t=_tmp1D7;
cargs=({struct Cyc_List_List*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));({void*_tmp44F=Cyc_Port_type_to_ctype(env_arg,t);_tmp1D8->hd=_tmp44F;}),_tmp1D8->tl=cargs;_tmp1D8;});}}
# 1260
return({void*_tmp450=crt;Cyc_Port_fn_ct(_tmp450,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));});}default: _LL15: _LL16:
# 1290
 return Cyc_Port_arith_ct();}_LL0:;}
# 1295
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*_tmp1E4=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);void*arg1=_tmp1E4;
struct Cyc_List_List*_tmp1E5=args->tl;struct Cyc_List_List*arg2s=_tmp1E5;
enum Cyc_Absyn_Primop _tmp1E6=p;switch(_tmp1E6){case Cyc_Absyn_Plus: _LL1: _LL2: {
# 1303
void*_tmp1E7=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(arg2s))->hd);void*arg2=_tmp1E7;
if(({void*_tmp455=arg1;Cyc_Port_leq(_tmp455,({void*_tmp454=Cyc_Port_var();void*_tmp453=Cyc_Port_var();void*_tmp452=Cyc_Port_fat_ct();void*_tmp451=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp454,_tmp453,_tmp452,_tmp451,Cyc_Port_var());}));})){
({void*_tmp456=arg2;Cyc_Port_leq(_tmp456,Cyc_Port_arith_ct());});
return arg1;}else{
if(({void*_tmp45B=arg2;Cyc_Port_leq(_tmp45B,({void*_tmp45A=Cyc_Port_var();void*_tmp459=Cyc_Port_var();void*_tmp458=Cyc_Port_fat_ct();void*_tmp457=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp45A,_tmp459,_tmp458,_tmp457,Cyc_Port_var());}));})){
({void*_tmp45C=arg1;Cyc_Port_leq(_tmp45C,Cyc_Port_arith_ct());});
return arg2;}else{
# 1311
({void*_tmp45D=arg1;Cyc_Port_leq(_tmp45D,Cyc_Port_arith_ct());});
({void*_tmp45E=arg2;Cyc_Port_leq(_tmp45E,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus: _LL3: _LL4:
# 1320
 if(arg2s == 0){
# 1322
({void*_tmp45F=arg1;Cyc_Port_leq(_tmp45F,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}else{
# 1325
void*_tmp1E8=Cyc_Port_compress_ct((void*)arg2s->hd);void*arg2=_tmp1E8;
if(({void*_tmp464=arg1;Cyc_Port_leq(_tmp464,({void*_tmp463=Cyc_Port_var();void*_tmp462=Cyc_Port_var();void*_tmp461=Cyc_Port_fat_ct();void*_tmp460=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp463,_tmp462,_tmp461,_tmp460,Cyc_Port_var());}));})){
if(({void*_tmp469=arg2;Cyc_Port_leq(_tmp469,({void*_tmp468=Cyc_Port_var();void*_tmp467=Cyc_Port_var();void*_tmp466=Cyc_Port_fat_ct();void*_tmp465=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp468,_tmp467,_tmp466,_tmp465,Cyc_Port_var());}));}))
return Cyc_Port_arith_ct();else{
# 1330
({void*_tmp46A=arg2;Cyc_Port_leq(_tmp46A,Cyc_Port_arith_ct());});
return arg1;}}else{
# 1334
({void*_tmp46B=arg1;Cyc_Port_leq(_tmp46B,Cyc_Port_arith_ct());});
({void*_tmp46C=arg1;Cyc_Port_leq(_tmp46C,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times: _LL5: _LL6:
# 1339
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8: goto _LLA;case Cyc_Absyn_Mod: _LL9: _LLA: goto _LLC;case Cyc_Absyn_Eq: _LLB: _LLC: goto _LLE;case Cyc_Absyn_Neq: _LLD: _LLE: goto _LL10;case Cyc_Absyn_Lt: _LLF: _LL10: goto _LL12;case Cyc_Absyn_Gt: _LL11: _LL12: goto _LL14;case Cyc_Absyn_Lte: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Gte: _LL15: _LL16: goto _LL18;case Cyc_Absyn_Bitand: _LL17: _LL18: goto _LL1A;case Cyc_Absyn_Bitor: _LL19: _LL1A: goto _LL1C;case Cyc_Absyn_Bitxor: _LL1B: _LL1C: goto _LL1E;case Cyc_Absyn_Bitlshift: _LL1D: _LL1E: goto _LL20;case Cyc_Absyn_Bitlrshift: _LL1F: _LL20:
# 1342
({void*_tmp46D=arg1;Cyc_Port_leq(_tmp46D,Cyc_Port_arith_ct());});
({void*_tmp46E=(void*)((struct Cyc_List_List*)_check_null(arg2s))->hd;Cyc_Port_leq(_tmp46E,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case Cyc_Absyn_Not: _LL21: _LL22:
 goto _LL24;case Cyc_Absyn_Bitnot: _LL23: _LL24:
# 1347
({void*_tmp46F=arg1;Cyc_Port_leq(_tmp46F,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();default: _LL25: _LL26:
({void*_tmp1E9=0U;({struct _fat_ptr _tmp470=({const char*_tmp1EA="Port: unknown primop";_tag_fat(_tmp1EA,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp470,_tag_fat(_tmp1E9,sizeof(void*),0U));});});}_LL0:;}
# 1354
static struct _tuple12 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){
void*_tmp1EB=e->r;void*_stmttmp18=_tmp1EB;void*_tmp1EC=_stmttmp18;struct _fat_ptr*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1ED;struct _fat_ptr*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F2;void*_tmp1F4;switch(*((int*)_tmp1EC)){case 1U: _LL1: _tmp1F4=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_LL2: {void*b=_tmp1F4;
# 1357
struct _tuple0*_tmp1F5=Cyc_Absyn_binding2qvar(b);struct _tuple0*x=_tmp1F5;
(*x).f1=Cyc_Absyn_Loc_n;{
struct _tuple14 _tmp1F6=Cyc_Port_lookup_var(env,x);struct _tuple14 _stmttmp19=_tmp1F6;struct _tuple14 _tmp1F7=_stmttmp19;unsigned _tmp1F9;struct _tuple12 _tmp1F8;_LLE: _tmp1F8=_tmp1F7.f1;_tmp1F9=_tmp1F7.f2;_LLF: {struct _tuple12 p=_tmp1F8;unsigned loc=_tmp1F9;
Cyc_Port_register_localvar_usage(env,loc,e->loc,takeaddress);
return p;}}}case 23U: _LL3: _tmp1F2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_tmp1F3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1EC)->f2;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp1F2;struct Cyc_Absyn_Exp*e2=_tmp1F3;
# 1363
void*_tmp1FA=Cyc_Port_var();void*v=_tmp1FA;
void*_tmp1FB=Cyc_Port_var();void*q=_tmp1FB;
void*_tmp1FC=Cyc_Port_gen_exp(0,env,e1);void*t1=_tmp1FC;
({void*_tmp471=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp471,Cyc_Port_arith_ct());});{
void*_tmp1FD=({void*_tmp475=v;void*_tmp474=q;void*_tmp473=Cyc_Port_fat_ct();void*_tmp472=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp475,_tmp474,_tmp473,_tmp472,Cyc_Port_var());});void*p=_tmp1FD;
Cyc_Port_leq(t1,p);
return({struct _tuple12 _tmp3A8;_tmp3A8.f1=q,_tmp3A8.f2=v;_tmp3A8;});}}case 20U: _LL5: _tmp1F1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp1F1;
# 1371
void*_tmp1FE=Cyc_Port_var();void*v=_tmp1FE;
void*_tmp1FF=Cyc_Port_var();void*q=_tmp1FF;
void*_tmp200=({void*_tmp479=v;void*_tmp478=q;void*_tmp477=Cyc_Port_var();void*_tmp476=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp479,_tmp478,_tmp477,_tmp476,Cyc_Port_var());});void*p=_tmp200;
({void*_tmp47A=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp47A,p);});
return({struct _tuple12 _tmp3A9;_tmp3A9.f1=q,_tmp3A9.f2=v;_tmp3A9;});}case 21U: _LL7: _tmp1EF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_tmp1F0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1EC)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp1EF;struct _fat_ptr*f=_tmp1F0;
# 1377
void*_tmp201=Cyc_Port_var();void*v=_tmp201;
void*_tmp202=Cyc_Port_var();void*q=_tmp202;
void*_tmp203=Cyc_Port_gen_exp(takeaddress,env,e1);void*t1=_tmp203;
({void*_tmp47C=t1;Cyc_Port_leq(_tmp47C,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp204[1U];({struct Cyc_Port_Cfield*_tmp47B=({struct Cyc_Port_Cfield*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->qual=q,_tmp205->f=f,_tmp205->type=v;_tmp205;});_tmp204[0]=_tmp47B;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp204,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return({struct _tuple12 _tmp3AA;_tmp3AA.f1=q,_tmp3AA.f2=v;_tmp3AA;});}case 22U: _LL9: _tmp1ED=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1EC)->f1;_tmp1EE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1EC)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp1ED;struct _fat_ptr*f=_tmp1EE;
# 1383
void*_tmp206=Cyc_Port_var();void*v=_tmp206;
void*_tmp207=Cyc_Port_var();void*q=_tmp207;
void*_tmp208=Cyc_Port_gen_exp(0,env,e1);void*t1=_tmp208;
({void*_tmp482=t1;Cyc_Port_leq(_tmp482,({void*_tmp481=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp209[1U];({struct Cyc_Port_Cfield*_tmp47D=({struct Cyc_Port_Cfield*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->qual=q,_tmp20A->f=f,_tmp20A->type=v;_tmp20A;});_tmp209[0]=_tmp47D;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp209,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp480=
Cyc_Port_var();
# 1386
void*_tmp47F=
Cyc_Port_var();
# 1386
void*_tmp47E=
Cyc_Port_var();
# 1386
Cyc_Port_ptr_ct(_tmp481,_tmp480,_tmp47F,_tmp47E,
Cyc_Port_var());}));});
return({struct _tuple12 _tmp3AB;_tmp3AB.f1=q,_tmp3AB.f2=v;_tmp3AB;});}default: _LLB: _LLC:
({void*_tmp20B=0U;({struct _fat_ptr _tmp485=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp20E=({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0U,({struct _fat_ptr _tmp483=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp3AC.f1=_tmp483;});_tmp3AC;});void*_tmp20C[1U];_tmp20C[0]=& _tmp20E;({struct _fat_ptr _tmp484=({const char*_tmp20D="gen_lhs: %s";_tag_fat(_tmp20D,sizeof(char),12U);});Cyc_aprintf(_tmp484,_tag_fat(_tmp20C,sizeof(void*),1U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp485,_tag_fat(_tmp20B,sizeof(void*),0U));});});}_LL0:;}
# 1394
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
return Cyc_Port_gen_exp(0,env,e);}
# 1397
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp20F=e->r;void*_stmttmp1A=_tmp20F;void*_tmp210=_stmttmp1A;struct Cyc_Absyn_Stmt*_tmp211;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*_tmp212;struct Cyc_Absyn_Exp*_tmp215;void**_tmp214;struct _fat_ptr*_tmp217;struct Cyc_Absyn_Exp*_tmp216;struct _fat_ptr*_tmp219;struct Cyc_Absyn_Exp*_tmp218;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Absyn_Exp*_tmp21F;void*_tmp21E;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_List_List*_tmp223;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Exp*_tmp224;struct Cyc_Absyn_Exp*_tmp227;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Exp*_tmp22A;enum Cyc_Absyn_Incrementor _tmp22E;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp231;struct Cyc_Core_Opt*_tmp230;struct Cyc_Absyn_Exp*_tmp22F;struct Cyc_List_List*_tmp233;enum Cyc_Absyn_Primop _tmp232;void*_tmp234;switch(*((int*)_tmp210)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp210)->f1).Wstring_c).tag){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 goto _LL8;case 6U: _LL7: _LL8:
 goto _LLA;case 7U: _LL13: _LL14:
# 1408
 return Cyc_Port_arith_ct();case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp210)->f1).Int_c).val).f2 == 0){_LL15: _LL16:
 return Cyc_Port_zero_ct();}else{_LL17: _LL18:
 return Cyc_Port_arith_ct();}case 8U: _LL19: _LL1A:
# 1412
 return({void*_tmp489=Cyc_Port_arith_ct();void*_tmp488=Cyc_Port_const_ct();void*_tmp487=Cyc_Port_fat_ct();void*_tmp486=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp489,_tmp488,_tmp487,_tmp486,Cyc_Port_zterm_ct());});case 9U: _LL1B: _LL1C:
# 1414
 return({void*_tmp48D=Cyc_Port_arith_ct();void*_tmp48C=Cyc_Port_const_ct();void*_tmp48B=Cyc_Port_fat_ct();void*_tmp48A=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp48D,_tmp48C,_tmp48B,_tmp48A,Cyc_Port_zterm_ct());});default: _LL1D: _LL1E:
 return Cyc_Port_zero_ct();}case 17U: _LL9: _LLA:
# 1403
 goto _LLC;case 18U: _LLB: _LLC:
 goto _LLE;case 19U: _LLD: _LLE:
 goto _LL10;case 32U: _LLF: _LL10:
 goto _LL12;case 33U: _LL11: _LL12:
 goto _LL14;case 1U: _LL1F: _tmp234=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL20: {void*b=_tmp234;
# 1417
struct _tuple0*_tmp235=Cyc_Absyn_binding2qvar(b);struct _tuple0*x=_tmp235;
(*x).f1=Cyc_Absyn_Loc_n;{
struct _tuple14 _tmp236=Cyc_Port_lookup_var(env,x);struct _tuple14 _stmttmp1B=_tmp236;struct _tuple14 _tmp237=_stmttmp1B;unsigned _tmp239;struct _tuple12 _tmp238;_LL6A: _tmp238=_tmp237.f1;_tmp239=_tmp237.f2;_LL6B: {struct _tuple12 p=_tmp238;unsigned loc=_tmp239;
struct _tuple12 _tmp23A=p;void*_tmp23B;_LL6D: _tmp23B=_tmp23A.f2;_LL6E: {void*t=_tmp23B;
if(Cyc_Port_isfn(env,x)){
t=Cyc_Port_instantiate(t);
return({void*_tmp491=t;void*_tmp490=Cyc_Port_var();void*_tmp48F=Cyc_Port_var();void*_tmp48E=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp491,_tmp490,_tmp48F,_tmp48E,Cyc_Port_nozterm_ct());});}else{
# 1425
if(Cyc_Port_isarray(env,x)){
void*_tmp23C=Cyc_Port_var();void*elt_type=_tmp23C;
void*_tmp23D=Cyc_Port_var();void*qual=_tmp23D;
void*_tmp23E=Cyc_Port_var();void*zt=_tmp23E;
void*_tmp23F=Cyc_Port_array_ct(elt_type,qual,zt);void*array_type=_tmp23F;
Cyc_Port_unifies(t,array_type);{
void*_tmp240=({void*_tmp495=elt_type;void*_tmp494=qual;void*_tmp493=Cyc_Port_fat_ct();void*_tmp492=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp495,_tmp494,_tmp493,_tmp492,zt);});void*ptr_type=_tmp240;
return ptr_type;}}else{
# 1434
Cyc_Port_register_localvar_usage(env,loc,e->loc,takeaddress);
return t;}}}}}}case 3U: _LL21: _tmp232=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp233=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL22: {enum Cyc_Absyn_Primop p=_tmp232;struct Cyc_List_List*es=_tmp233;
# 1437
return({struct Cyc_Port_Cenv*_tmp497=env;enum Cyc_Absyn_Primop _tmp496=p;Cyc_Port_gen_primop(_tmp497,_tmp496,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,es));});}case 4U: _LL23: _tmp22F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp230=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_tmp231=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp210)->f3;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp22F;struct Cyc_Core_Opt*popt=_tmp230;struct Cyc_Absyn_Exp*e2=_tmp231;
# 1439
struct _tuple12 _tmp241=Cyc_Port_gen_lhs(env,e1,0);struct _tuple12 _stmttmp1C=_tmp241;struct _tuple12 _tmp242=_stmttmp1C;void*_tmp244;void*_tmp243;_LL70: _tmp243=_tmp242.f1;_tmp244=_tmp242.f2;_LL71: {void*q=_tmp243;void*t1=_tmp244;
({void*_tmp498=q;Cyc_Port_unifies(_tmp498,Cyc_Port_notconst_ct());});{
void*_tmp245=Cyc_Port_gen_exp(0,env,e2);void*t2=_tmp245;
if((unsigned)popt){
struct Cyc_List_List x2=({struct Cyc_List_List _tmp3AE;_tmp3AE.hd=t2,_tmp3AE.tl=0;_tmp3AE;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp3AD;_tmp3AD.hd=t1,_tmp3AD.tl=& x2;_tmp3AD;});
void*_tmp246=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)popt->v,& x1);void*t3=_tmp246;
Cyc_Port_leq(t3,t1);
return t1;}else{
# 1449
Cyc_Port_leq(t2,t1);
return t1;}}}}case 5U: _LL25: _tmp22D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp22E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp22D;enum Cyc_Absyn_Incrementor inc=_tmp22E;
# 1453
struct _tuple12 _tmp247=Cyc_Port_gen_lhs(env,e1,0);struct _tuple12 _stmttmp1D=_tmp247;struct _tuple12 _tmp248=_stmttmp1D;void*_tmp24A;void*_tmp249;_LL73: _tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;_LL74: {void*q=_tmp249;void*t=_tmp24A;
({void*_tmp499=q;Cyc_Port_unifies(_tmp499,Cyc_Port_notconst_ct());});
# 1456
({void*_tmp49E=t;Cyc_Port_leq(_tmp49E,({void*_tmp49D=Cyc_Port_var();void*_tmp49C=Cyc_Port_var();void*_tmp49B=Cyc_Port_fat_ct();void*_tmp49A=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp49D,_tmp49C,_tmp49B,_tmp49A,Cyc_Port_var());}));});
({void*_tmp49F=t;Cyc_Port_leq(_tmp49F,Cyc_Port_arith_ct());});
return t;}}case 6U: _LL27: _tmp22A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp22B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_tmp22C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp210)->f3;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp22A;struct Cyc_Absyn_Exp*e2=_tmp22B;struct Cyc_Absyn_Exp*e3=_tmp22C;
# 1460
void*_tmp24B=Cyc_Port_var();void*v=_tmp24B;
({void*_tmp4A0=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp4A0,Cyc_Port_arith_ct());});
({void*_tmp4A1=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp4A1,v);});
({void*_tmp4A2=Cyc_Port_gen_exp(0,env,e3);Cyc_Port_leq(_tmp4A2,v);});
return v;}case 7U: _LL29: _tmp228=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp229=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp228;struct Cyc_Absyn_Exp*e2=_tmp229;
_tmp226=e1;_tmp227=e2;goto _LL2C;}case 8U: _LL2B: _tmp226=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp227=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp226;struct Cyc_Absyn_Exp*e2=_tmp227;
# 1467
({void*_tmp4A3=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp4A3,Cyc_Port_arith_ct());});
({void*_tmp4A4=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp4A4,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}case 9U: _LL2D: _tmp224=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp225=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp224;struct Cyc_Absyn_Exp*e2=_tmp225;
# 1471
Cyc_Port_gen_exp(0,env,e1);
return Cyc_Port_gen_exp(0,env,e2);}case 10U: _LL2F: _tmp222=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp223=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp222;struct Cyc_List_List*es=_tmp223;
# 1474
void*_tmp24C=Cyc_Port_var();void*v=_tmp24C;
void*_tmp24D=Cyc_Port_gen_exp(0,env,e1);void*t1=_tmp24D;
struct Cyc_List_List*_tmp24E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,es);struct Cyc_List_List*ts=_tmp24E;
struct Cyc_List_List*_tmp24F=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,ts);struct Cyc_List_List*vs=_tmp24F;
({void*_tmp4A9=t1;Cyc_Port_unifies(_tmp4A9,({void*_tmp4A8=Cyc_Port_fn_ct(v,vs);void*_tmp4A7=Cyc_Port_var();void*_tmp4A6=Cyc_Port_var();void*_tmp4A5=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4A8,_tmp4A7,_tmp4A6,_tmp4A5,Cyc_Port_nozterm_ct());}));});
for(0;ts != 0;(ts=ts->tl,vs=vs->tl)){
Cyc_Port_leq((void*)ts->hd,(void*)((struct Cyc_List_List*)_check_null(vs))->hd);}
# 1482
return v;}case 42U: _LL31: _LL32:
({void*_tmp250=0U;({struct _fat_ptr _tmp4AA=({const char*_tmp251="@assert";_tag_fat(_tmp251,sizeof(char),8U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4AA,_tag_fat(_tmp250,sizeof(void*),0U));});});case 11U: _LL33: _LL34:
({void*_tmp252=0U;({struct _fat_ptr _tmp4AB=({const char*_tmp253="throw";_tag_fat(_tmp253,sizeof(char),6U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4AB,_tag_fat(_tmp252,sizeof(void*),0U));});});case 41U: _LL35: _tmp221=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp221;
return Cyc_Port_gen_exp(0,env,e1);}case 12U: _LL37: _tmp220=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp220;
return Cyc_Port_gen_exp(0,env,e1);}case 13U: _LL39: _LL3A:
({void*_tmp254=0U;({struct _fat_ptr _tmp4AC=({const char*_tmp255="instantiate";_tag_fat(_tmp255,sizeof(char),12U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4AC,_tag_fat(_tmp254,sizeof(void*),0U));});});case 14U: _LL3B: _tmp21E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp21F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL3C: {void*t=_tmp21E;struct Cyc_Absyn_Exp*e1=_tmp21F;
# 1489
void*_tmp256=Cyc_Port_type_to_ctype(env,t);void*t1=_tmp256;
void*_tmp257=Cyc_Port_gen_exp(0,env,e1);void*t2=_tmp257;
Cyc_Port_leq(t1,t2);
return t2;}case 15U: _LL3D: _tmp21D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp21D;
# 1494
struct _tuple12 _tmp258=Cyc_Port_gen_lhs(env,e1,1);struct _tuple12 _stmttmp1E=_tmp258;struct _tuple12 _tmp259=_stmttmp1E;void*_tmp25B;void*_tmp25A;_LL76: _tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;_LL77: {void*q=_tmp25A;void*t=_tmp25B;
return({void*_tmp4B0=t;void*_tmp4AF=q;void*_tmp4AE=Cyc_Port_var();void*_tmp4AD=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4B0,_tmp4AF,_tmp4AE,_tmp4AD,Cyc_Port_var());});}}case 23U: _LL3F: _tmp21B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp21C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp21B;struct Cyc_Absyn_Exp*e2=_tmp21C;
# 1497
void*_tmp25C=Cyc_Port_var();void*v=_tmp25C;
({void*_tmp4B1=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp4B1,Cyc_Port_arith_ct());});{
void*_tmp25D=Cyc_Port_gen_exp(0,env,e1);void*t=_tmp25D;
({void*_tmp4B6=t;Cyc_Port_leq(_tmp4B6,({void*_tmp4B5=v;void*_tmp4B4=Cyc_Port_var();void*_tmp4B3=Cyc_Port_fat_ct();void*_tmp4B2=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4B5,_tmp4B4,_tmp4B3,_tmp4B2,Cyc_Port_var());}));});
return v;}}case 20U: _LL41: _tmp21A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL42: {struct Cyc_Absyn_Exp*e1=_tmp21A;
# 1503
void*_tmp25E=Cyc_Port_var();void*v=_tmp25E;
({void*_tmp4BB=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp4BB,({void*_tmp4BA=v;void*_tmp4B9=Cyc_Port_var();void*_tmp4B8=Cyc_Port_var();void*_tmp4B7=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4BA,_tmp4B9,_tmp4B8,_tmp4B7,Cyc_Port_var());}));});
return v;}case 21U: _LL43: _tmp218=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp219=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL44: {struct Cyc_Absyn_Exp*e1=_tmp218;struct _fat_ptr*f=_tmp219;
# 1507
void*_tmp25F=Cyc_Port_var();void*v=_tmp25F;
void*_tmp260=Cyc_Port_gen_exp(takeaddress,env,e1);void*t=_tmp260;
({void*_tmp4BE=t;Cyc_Port_leq(_tmp4BE,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp261[1U];({struct Cyc_Port_Cfield*_tmp4BD=({struct Cyc_Port_Cfield*_tmp262=_cycalloc(sizeof(*_tmp262));({void*_tmp4BC=Cyc_Port_var();_tmp262->qual=_tmp4BC;}),_tmp262->f=f,_tmp262->type=v;_tmp262;});_tmp261[0]=_tmp4BD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp261,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return v;}case 22U: _LL45: _tmp216=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp217=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL46: {struct Cyc_Absyn_Exp*e1=_tmp216;struct _fat_ptr*f=_tmp217;
# 1512
void*_tmp263=Cyc_Port_var();void*v=_tmp263;
void*_tmp264=Cyc_Port_gen_exp(0,env,e1);void*t=_tmp264;
({void*_tmp4C5=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp4C5,({void*_tmp4C4=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp265[1U];({struct Cyc_Port_Cfield*_tmp4C0=({struct Cyc_Port_Cfield*_tmp266=_cycalloc(sizeof(*_tmp266));({void*_tmp4BF=Cyc_Port_var();_tmp266->qual=_tmp4BF;}),_tmp266->f=f,_tmp266->type=v;_tmp266;});_tmp265[0]=_tmp4C0;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp265,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp4C3=
Cyc_Port_var();
# 1514
void*_tmp4C2=
Cyc_Port_var();
# 1514
void*_tmp4C1=
Cyc_Port_var();
# 1514
Cyc_Port_ptr_ct(_tmp4C4,_tmp4C3,_tmp4C2,_tmp4C1,
Cyc_Port_var());}));});
return v;}case 34U: _LL47: _tmp214=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp210)->f1).elt_type;_tmp215=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp210)->f1).num_elts;_LL48: {void**topt=_tmp214;struct Cyc_Absyn_Exp*e1=_tmp215;
# 1518
({void*_tmp4C6=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp4C6,Cyc_Port_arith_ct());});{
void*_tmp267=(unsigned)topt?Cyc_Port_type_to_ctype(env,*topt): Cyc_Port_var();void*t=_tmp267;
return({void*_tmp4CA=t;void*_tmp4C9=Cyc_Port_var();void*_tmp4C8=Cyc_Port_fat_ct();void*_tmp4C7=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4CA,_tmp4C9,_tmp4C8,_tmp4C7,Cyc_Port_var());});}}case 2U: _LL49: _LL4A:
({void*_tmp268=0U;({struct _fat_ptr _tmp4CB=({const char*_tmp269="Pragma_e";_tag_fat(_tmp269,sizeof(char),9U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CB,_tag_fat(_tmp268,sizeof(void*),0U));});});case 35U: _LL4B: _tmp212=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_tmp213=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp210)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmp212;struct Cyc_Absyn_Exp*e2=_tmp213;
({void*_tmp26A=0U;({struct _fat_ptr _tmp4CC=({const char*_tmp26B="Swap_e";_tag_fat(_tmp26B,sizeof(char),7U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CC,_tag_fat(_tmp26A,sizeof(void*),0U));});});}case 16U: _LL4D: _LL4E:
({void*_tmp26C=0U;({struct _fat_ptr _tmp4CD=({const char*_tmp26D="New_e";_tag_fat(_tmp26D,sizeof(char),6U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CD,_tag_fat(_tmp26C,sizeof(void*),0U));});});case 24U: _LL4F: _LL50:
({void*_tmp26E=0U;({struct _fat_ptr _tmp4CE=({const char*_tmp26F="Tuple_e";_tag_fat(_tmp26F,sizeof(char),8U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CE,_tag_fat(_tmp26E,sizeof(void*),0U));});});case 25U: _LL51: _LL52:
({void*_tmp270=0U;({struct _fat_ptr _tmp4CF=({const char*_tmp271="CompoundLit_e";_tag_fat(_tmp271,sizeof(char),14U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4CF,_tag_fat(_tmp270,sizeof(void*),0U));});});case 26U: _LL53: _LL54:
({void*_tmp272=0U;({struct _fat_ptr _tmp4D0=({const char*_tmp273="Array_e";_tag_fat(_tmp273,sizeof(char),8U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D0,_tag_fat(_tmp272,sizeof(void*),0U));});});case 27U: _LL55: _LL56:
({void*_tmp274=0U;({struct _fat_ptr _tmp4D1=({const char*_tmp275="Comprehension_e";_tag_fat(_tmp275,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D1,_tag_fat(_tmp274,sizeof(void*),0U));});});case 28U: _LL57: _LL58:
({void*_tmp276=0U;({struct _fat_ptr _tmp4D2=({const char*_tmp277="ComprehensionNoinit_e";_tag_fat(_tmp277,sizeof(char),22U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D2,_tag_fat(_tmp276,sizeof(void*),0U));});});case 29U: _LL59: _LL5A:
({void*_tmp278=0U;({struct _fat_ptr _tmp4D3=({const char*_tmp279="Aggregate_e";_tag_fat(_tmp279,sizeof(char),12U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D3,_tag_fat(_tmp278,sizeof(void*),0U));});});case 30U: _LL5B: _LL5C:
({void*_tmp27A=0U;({struct _fat_ptr _tmp4D4=({const char*_tmp27B="AnonStruct_e";_tag_fat(_tmp27B,sizeof(char),13U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D4,_tag_fat(_tmp27A,sizeof(void*),0U));});});case 31U: _LL5D: _LL5E:
({void*_tmp27C=0U;({struct _fat_ptr _tmp4D5=({const char*_tmp27D="Datatype_e";_tag_fat(_tmp27D,sizeof(char),11U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D5,_tag_fat(_tmp27C,sizeof(void*),0U));});});case 36U: _LL5F: _LL60:
({void*_tmp27E=0U;({struct _fat_ptr _tmp4D6=({const char*_tmp27F="UnresolvedMem_e";_tag_fat(_tmp27F,sizeof(char),16U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D6,_tag_fat(_tmp27E,sizeof(void*),0U));});});case 37U: _LL61: _tmp211=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp210)->f1;_LL62: {struct Cyc_Absyn_Stmt*s=_tmp211;
({void*_tmp280=0U;({struct _fat_ptr _tmp4D7=({const char*_tmp281="StmtExp_e";_tag_fat(_tmp281,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D7,_tag_fat(_tmp280,sizeof(void*),0U));});});}case 39U: _LL63: _LL64:
({void*_tmp282=0U;({struct _fat_ptr _tmp4D8=({const char*_tmp283="Valueof_e";_tag_fat(_tmp283,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D8,_tag_fat(_tmp282,sizeof(void*),0U));});});case 40U: _LL65: _LL66:
({void*_tmp284=0U;({struct _fat_ptr _tmp4D9=({const char*_tmp285="Asm_e";_tag_fat(_tmp285,sizeof(char),6U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4D9,_tag_fat(_tmp284,sizeof(void*),0U));});});default: _LL67: _LL68:
({void*_tmp286=0U;({struct _fat_ptr _tmp4DA=({const char*_tmp287="Tagcheck_e";_tag_fat(_tmp287,sizeof(char),11U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4DA,_tag_fat(_tmp286,sizeof(void*),0U));});});}_LL0:;}
# 1541
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp288=s->r;void*_stmttmp1F=_tmp288;void*_tmp289=_stmttmp1F;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Stmt*_tmp28A;struct Cyc_Absyn_Stmt*_tmp28C;struct Cyc_Absyn_Stmt*_tmp28E;struct Cyc_Absyn_Decl*_tmp28D;struct Cyc_List_List*_tmp290;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Stmt*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Stmt*_tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Stmt*_tmp299;struct Cyc_Absyn_Stmt*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Stmt*_tmp29C;struct Cyc_Absyn_Stmt*_tmp29B;struct Cyc_Absyn_Exp*_tmp29D;switch(*((int*)_tmp289)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp29D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp29D;
Cyc_Port_gen_exp(0,env,e);return;}case 2U: _LL5: _tmp29B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_tmp29C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp29B;struct Cyc_Absyn_Stmt*s2=_tmp29C;
Cyc_Port_gen_stmt(env,s1);Cyc_Port_gen_stmt(env,s2);return;}case 3U: _LL7: _tmp29A=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp29A;
# 1547
void*_tmp29E=Cyc_Port_lookup_return_type(env);void*v=_tmp29E;
void*_tmp29F=(unsigned)eopt?Cyc_Port_gen_exp(0,env,eopt): Cyc_Port_void_ct();void*t=_tmp29F;
Cyc_Port_leq(t,v);
return;}case 4U: _LL9: _tmp297=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_tmp298=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_tmp299=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp289)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp297;struct Cyc_Absyn_Stmt*s1=_tmp298;struct Cyc_Absyn_Stmt*s2=_tmp299;
# 1552
({void*_tmp4DB=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp4DB,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,s1);
Cyc_Port_gen_stmt(env,s2);
return;}case 5U: _LLB: _tmp295=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp289)->f1).f1;_tmp296=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp295;struct Cyc_Absyn_Stmt*s=_tmp296;
# 1557
({void*_tmp4DC=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp4DC,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,s);
return;}case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 8U: _LL11: _LL12:
 return;case 9U: _LL13: _tmp291=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_tmp292=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp289)->f2).f1;_tmp293=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp289)->f3).f1;_tmp294=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp289)->f4;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp291;struct Cyc_Absyn_Exp*e2=_tmp292;struct Cyc_Absyn_Exp*e3=_tmp293;struct Cyc_Absyn_Stmt*s=_tmp294;
# 1564
Cyc_Port_gen_exp(0,env,e1);
({void*_tmp4DD=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp4DD,Cyc_Port_arith_ct());});
Cyc_Port_gen_exp(0,env,e3);
Cyc_Port_gen_stmt(env,s);
return;}case 10U: _LL15: _tmp28F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_tmp290=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp28F;struct Cyc_List_List*scs=_tmp290;
# 1570
({void*_tmp4DE=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp4DE,Cyc_Port_arith_ct());});
for(0;(unsigned)scs;scs=scs->tl){
# 1573
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
# 1575
return;}case 11U: _LL17: _LL18:
({void*_tmp2A0=0U;({struct _fat_ptr _tmp4DF=({const char*_tmp2A1="fallthru";_tag_fat(_tmp2A1,sizeof(char),9U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4DF,_tag_fat(_tmp2A0,sizeof(void*),0U));});});case 12U: _LL19: _tmp28D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_tmp28E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_LL1A: {struct Cyc_Absyn_Decl*d=_tmp28D;struct Cyc_Absyn_Stmt*s=_tmp28E;
# 1578
env=Cyc_Port_gen_localdecl(env,d,0);Cyc_Port_gen_stmt(env,s);return;}case 13U: _LL1B: _tmp28C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s=_tmp28C;
Cyc_Port_gen_stmt(env,s);return;}case 14U: _LL1D: _tmp28A=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_tmp28B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp289)->f2).f1;_LL1E: {struct Cyc_Absyn_Stmt*s=_tmp28A;struct Cyc_Absyn_Exp*e=_tmp28B;
# 1581
Cyc_Port_gen_stmt(env,s);
({void*_tmp4E0=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp4E0,Cyc_Port_arith_ct());});
return;}default: _LL1F: _LL20:
({void*_tmp2A2=0U;({struct _fat_ptr _tmp4E1=({const char*_tmp2A3="try/catch";_tag_fat(_tmp2A3,sizeof(char),10U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E1,_tag_fat(_tmp2A2,sizeof(void*),0U));});});}_LL0:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1589
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp2A4=e->r;void*_stmttmp20=_tmp2A4;void*_tmp2A5=_stmttmp20;struct Cyc_List_List*_tmp2A6;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A8;switch(*((int*)_tmp2A5)){case 36U: _LL1: _tmp2A8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2A5)->f2;_LL2: {struct Cyc_List_List*dles=_tmp2A8;
_tmp2A7=dles;goto _LL4;}case 26U: _LL3: _tmp2A7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2A5)->f1;_LL4: {struct Cyc_List_List*dles=_tmp2A7;
_tmp2A6=dles;goto _LL6;}case 25U: _LL5: _tmp2A6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2A5)->f2;_LL6: {struct Cyc_List_List*dles=_tmp2A6;
# 1594
LOOP: {
void*_tmp2A9=t;struct Cyc_Absyn_Aggrdecl*_tmp2AA;struct _tuple0*_tmp2AB;unsigned _tmp2AE;void*_tmp2AD;void*_tmp2AC;struct _tuple0*_tmp2AF;switch(*((int*)_tmp2A9)){case 8U: _LLE: _tmp2AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A9)->f1;_LLF: {struct _tuple0*n=_tmp2AF;
# 1597
(*n).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 4U: _LL10: _tmp2AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A9)->f1).elt_type;_tmp2AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A9)->f1).zero_term;_tmp2AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A9)->f1).zt_loc;_LL11: {void*et=_tmp2AC;void*zt=_tmp2AD;unsigned ztl=_tmp2AE;
# 1600
void*_tmp2B0=Cyc_Port_var();void*v=_tmp2B0;
void*_tmp2B1=Cyc_Port_var();void*q=_tmp2B1;
void*_tmp2B2=Cyc_Port_var();void*z=_tmp2B2;
void*_tmp2B3=Cyc_Port_type_to_ctype(env,et);void*t=_tmp2B3;
for(0;dles != 0;dles=dles->tl){
struct _tuple18 _tmp2B4=*((struct _tuple18*)dles->hd);struct _tuple18 _stmttmp21=_tmp2B4;struct _tuple18 _tmp2B5=_stmttmp21;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_List_List*_tmp2B6;_LL19: _tmp2B6=_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;_LL1A: {struct Cyc_List_List*d=_tmp2B6;struct Cyc_Absyn_Exp*e=_tmp2B7;
if((unsigned)d)({void*_tmp2B8=0U;({struct _fat_ptr _tmp4E2=({const char*_tmp2B9="designators in initializers";_tag_fat(_tmp2B9,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E2,_tag_fat(_tmp2B8,sizeof(void*),0U));});});{
void*_tmp2BA=Cyc_Port_gen_initializer(env,et,e);void*te=_tmp2BA;
Cyc_Port_leq(te,v);}}}
# 1610
return Cyc_Port_array_ct(v,q,z);}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9)->f1)->f1).UnknownAggr).tag == 1){if((((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9)->f1)->f1).UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL12: _tmp2AB=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9)->f1)->f1).UnknownAggr).val).f2;_LL13: {struct _tuple0*n=_tmp2AB;
# 1612
(*n).f1=Cyc_Absyn_Loc_n;{
struct _tuple11 _tmp2BB=*Cyc_Port_lookup_struct_decl(env,n);struct _tuple11 _stmttmp22=_tmp2BB;struct _tuple11 _tmp2BC=_stmttmp22;struct Cyc_Absyn_Aggrdecl*_tmp2BD;_LL1C: _tmp2BD=_tmp2BC.f1;_LL1D: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2BD;
if(ad == 0)({void*_tmp2BE=0U;({struct _fat_ptr _tmp4E3=({const char*_tmp2BF="struct is not yet defined";_tag_fat(_tmp2BF,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E3,_tag_fat(_tmp2BE,sizeof(void*),0U));});});
_tmp2AA=ad;goto _LL15;}}}}else{goto _LL16;}}else{_LL14: _tmp2AA=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9)->f1)->f1).KnownAggr).val;_LL15: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2AA;
# 1617
struct Cyc_List_List*_tmp2C0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp2C0;
for(0;dles != 0;dles=dles->tl){
struct _tuple18 _tmp2C1=*((struct _tuple18*)dles->hd);struct _tuple18 _stmttmp23=_tmp2C1;struct _tuple18 _tmp2C2=_stmttmp23;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_List_List*_tmp2C3;_LL1F: _tmp2C3=_tmp2C2.f1;_tmp2C4=_tmp2C2.f2;_LL20: {struct Cyc_List_List*d=_tmp2C3;struct Cyc_Absyn_Exp*e=_tmp2C4;
if((unsigned)d)({void*_tmp2C5=0U;({struct _fat_ptr _tmp4E4=({const char*_tmp2C6="designators in initializers";_tag_fat(_tmp2C6,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E4,_tag_fat(_tmp2C5,sizeof(void*),0U));});});{
struct Cyc_Absyn_Aggrfield*_tmp2C7=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fields))->hd;struct Cyc_Absyn_Aggrfield*f=_tmp2C7;
fields=fields->tl;{
void*_tmp2C8=Cyc_Port_gen_initializer(env,f->type,e);void*te=_tmp2C8;;}}}}
# 1625
return Cyc_Port_type_to_ctype(env,t);}}}else{goto _LL16;}default: _LL16: _LL17:
({void*_tmp2C9=0U;({struct _fat_ptr _tmp4E5=({const char*_tmp2CA="bad type for aggregate initializer";_tag_fat(_tmp2CA,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4E5,_tag_fat(_tmp2C9,sizeof(void*),0U));});});}_LLD:;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2A5)->f1).Wstring_c).tag){case 8U: _LL7: _LL8:
# 1628
 goto _LLA;case 9U: _LL9: _LLA:
# 1630
 LOOP2: {
void*_tmp2CB=t;struct _tuple0*_tmp2CC;switch(*((int*)_tmp2CB)){case 8U: _LL22: _tmp2CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2CB)->f1;_LL23: {struct _tuple0*n=_tmp2CC;
# 1633
(*n).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP2;}case 4U: _LL24: _LL25:
 return({void*_tmp4E7=Cyc_Port_arith_ct();void*_tmp4E6=Cyc_Port_const_ct();Cyc_Port_array_ct(_tmp4E7,_tmp4E6,Cyc_Port_zterm_ct());});default: _LL26: _LL27:
 return Cyc_Port_gen_exp(0,env,e);}_LL21:;}default: goto _LLB;}default: _LLB: _LLC:
# 1638
 return Cyc_Port_gen_exp(0,env,e);}_LL0:;}
# 1643
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){
void*_tmp2CD=d->r;void*_stmttmp24=_tmp2CD;void*_tmp2CE=_stmttmp24;struct Cyc_Absyn_Vardecl*_tmp2CF;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CE)->tag == 0U){_LL1: _tmp2CF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CE)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp2CF;
# 1646
if(!fromglobal)Cyc_Port_register_localvar_decl(env,vd->name,vd->varloc,vd->type,vd->initializer);{
void*_tmp2D0=Cyc_Port_var();void*t=_tmp2D0;
void*q;
if((env->gcenv)->porting){
q=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp4EA=env;void*_tmp4E9=q;void*_tmp4E8=
(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1651
Cyc_Port_register_const_cvar(_tmp4EA,_tmp4E9,_tmp4E8,(vd->tq).loc);});}else{
# 1660
q=(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1662
(*vd->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,vd->name,vd->type,q,t,vd->varloc);
({void*_tmp4EB=t;Cyc_Port_unifies(_tmp4EB,Cyc_Port_type_to_ctype(env,vd->type));});
if((unsigned)vd->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*t2=Cyc_Port_gen_initializer(env,vd->type,e);
Cyc_Port_leq(t2,t);}
# 1670
return env;}}}else{_LL3: _LL4:
({void*_tmp2D1=0U;({struct _fat_ptr _tmp4EC=({const char*_tmp2D2="non-local decl that isn't a variable";_tag_fat(_tmp2D2,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp4EC,_tag_fat(_tmp2D1,sizeof(void*),0U));});});}_LL0:;}
# 1675
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp2D3=*arg;struct _tuple8 _stmttmp25=_tmp2D3;struct _tuple8 _tmp2D4=_stmttmp25;void*_tmp2D7;struct Cyc_Absyn_Tqual _tmp2D6;struct _fat_ptr*_tmp2D5;_LL1: _tmp2D5=_tmp2D4.f1;_tmp2D6=_tmp2D4.f2;_tmp2D7=_tmp2D4.f3;_LL2: {struct _fat_ptr*x=_tmp2D5;struct Cyc_Absyn_Tqual q=_tmp2D6;void*t=_tmp2D7;
return({struct _tuple8*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->f1=0,_tmp2D8->f2=q,_tmp2D8->f3=t;_tmp2D8;});}}
# 1680
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct _tuple19{struct _fat_ptr*f1;void*f2;void*f3;void*f4;};
# 1682
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp2D9=d->r;void*_stmttmp26=_tmp2D9;void*_tmp2DA=_stmttmp26;struct Cyc_Absyn_Enumdecl*_tmp2DB;struct Cyc_Absyn_Aggrdecl*_tmp2DC;struct Cyc_Absyn_Typedefdecl*_tmp2DD;struct Cyc_Absyn_Fndecl*_tmp2DE;struct Cyc_Absyn_Vardecl*_tmp2DF;switch(*((int*)_tmp2DA)){case 13U: _LL1: _LL2:
# 1685
(env->gcenv)->porting=1;
return env;case 14U: _LL3: _LL4:
# 1688
(env->gcenv)->porting=0;
return env;case 0U: _LL5: _tmp2DF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp2DF;
# 1691
(*vd->name).f1=Cyc_Absyn_Loc_n;
# 1695
if(Cyc_Port_declared_var(env,vd->name)){
struct _tuple14 _tmp2E0=Cyc_Port_lookup_var(env,vd->name);struct _tuple14 _stmttmp27=_tmp2E0;struct _tuple14 _tmp2E1=_stmttmp27;unsigned _tmp2E3;struct _tuple12 _tmp2E2;_LL12: _tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;_LL13: {struct _tuple12 p=_tmp2E2;unsigned loc=_tmp2E3;
struct _tuple12 _tmp2E4=p;void*_tmp2E6;void*_tmp2E5;_LL15: _tmp2E5=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;_LL16: {void*q=_tmp2E5;void*t=_tmp2E6;
void*q2;
if((env->gcenv)->porting && !Cyc_Port_isfn(env,vd->name)){
q2=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp4EF=env;void*_tmp4EE=q2;void*_tmp4ED=
(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1701
Cyc_Port_register_const_cvar(_tmp4EF,_tmp4EE,_tmp4ED,(vd->tq).loc);});}else{
# 1710
q2=(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1712
Cyc_Port_unifies(q,q2);
({void*_tmp4F0=t;Cyc_Port_unifies(_tmp4F0,Cyc_Port_type_to_ctype(env,vd->type));});
if((unsigned)vd->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
({void*_tmp4F1=Cyc_Port_gen_initializer(env,vd->type,e);Cyc_Port_leq(_tmp4F1,t);});}
# 1718
return env;}}}else{
# 1720
return Cyc_Port_gen_localdecl(env,d,1);}}case 1U: _LL7: _tmp2DE=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LL8: {struct Cyc_Absyn_Fndecl*fd=_tmp2DE;
# 1726
(*fd->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple15*predeclared=0;
# 1729
if(Cyc_Port_declared_var(env,fd->name))
predeclared=Cyc_Port_lookup_full_var(env,fd->name);{
# 1732
void*_tmp2E7=(fd->i).ret_type;void*rettype=_tmp2E7;
struct Cyc_List_List*_tmp2E8=(fd->i).args;struct Cyc_List_List*args=_tmp2E8;
struct Cyc_List_List*_tmp2E9=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,args);struct Cyc_List_List*targs=_tmp2E9;
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2EA=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp305=_cycalloc(sizeof(*_tmp305));
_tmp305->tag=5U,(_tmp305->f1).tvars=0,(_tmp305->f1).effect=0,({struct Cyc_Absyn_Tqual _tmp4F2=Cyc_Absyn_empty_tqual(0U);(_tmp305->f1).ret_tqual=_tmp4F2;}),(_tmp305->f1).ret_type=rettype,(_tmp305->f1).args=targs,(_tmp305->f1).c_varargs=0,(_tmp305->f1).cyc_varargs=0,(_tmp305->f1).rgn_po=0,(_tmp305->f1).attributes=0,(_tmp305->f1).requires_clause=0,(_tmp305->f1).requires_relns=0,(_tmp305->f1).ensures_clause=0,(_tmp305->f1).ensures_relns=0,(_tmp305->f1).return_value=0;_tmp305;});
# 1735
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp2EA;
# 1739
struct Cyc_Port_Cenv*_tmp2EB=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);struct Cyc_Port_Cenv*fn_env=_tmp2EB;
void*_tmp2EC=Cyc_Port_type_to_ctype(fn_env,rettype);void*c_rettype=_tmp2EC;
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp2ED=args;struct Cyc_List_List*xs=_tmp2ED;for(0;(unsigned)xs;xs=xs->tl){
struct _tuple8 _tmp2EE=*((struct _tuple8*)xs->hd);struct _tuple8 _stmttmp28=_tmp2EE;struct _tuple8 _tmp2EF=_stmttmp28;void*_tmp2F2;struct Cyc_Absyn_Tqual _tmp2F1;struct _fat_ptr*_tmp2F0;_LL18: _tmp2F0=_tmp2EF.f1;_tmp2F1=_tmp2EF.f2;_tmp2F2=_tmp2EF.f3;_LL19: {struct _fat_ptr*x=_tmp2F0;struct Cyc_Absyn_Tqual tq=_tmp2F1;void*t=_tmp2F2;
# 1747
void*_tmp2F3=Cyc_Port_type_to_ctype(fn_env,t);void*ct=_tmp2F3;
void*tqv;
if((env->gcenv)->porting){
tqv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp4F5=env;void*_tmp4F4=tqv;void*_tmp4F3=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp4F5,_tmp4F4,_tmp4F3,tq.loc);});}else{
# 1759
tqv=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1761
c_args=({struct Cyc_List_List*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));({struct _tuple19*_tmp4F6=({struct _tuple19*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->f1=x,_tmp2F4->f2=t,_tmp2F4->f3=tqv,_tmp2F4->f4=ct;_tmp2F4;});_tmp2F5->hd=_tmp4F6;}),_tmp2F5->tl=c_args;_tmp2F5;});
c_arg_types=({struct Cyc_List_List*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6->hd=ct,_tmp2F6->tl=c_arg_types;_tmp2F6;});}}}
# 1764
c_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);
c_arg_types=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_arg_types);{
void*_tmp2F7=Cyc_Port_fn_ct(c_rettype,c_arg_types);void*ctype=_tmp2F7;
# 1770
(*fd->name).f1=Cyc_Absyn_Loc_n;
fn_env=({struct Cyc_Port_Cenv*_tmp4FA=fn_env;struct _tuple0*_tmp4F9=fd->name;void*_tmp4F8=(void*)fntype;void*_tmp4F7=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp4FA,_tmp4F9,_tmp4F8,_tmp4F7,ctype,0U);});
Cyc_Port_add_return_type(fn_env,c_rettype);
{struct Cyc_List_List*_tmp2F8=c_args;struct Cyc_List_List*xs=_tmp2F8;for(0;(unsigned)xs;xs=xs->tl){
struct _tuple19 _tmp2F9=*((struct _tuple19*)xs->hd);struct _tuple19 _stmttmp29=_tmp2F9;struct _tuple19 _tmp2FA=_stmttmp29;void*_tmp2FE;void*_tmp2FD;void*_tmp2FC;struct _fat_ptr*_tmp2FB;_LL1B: _tmp2FB=_tmp2FA.f1;_tmp2FC=_tmp2FA.f2;_tmp2FD=_tmp2FA.f3;_tmp2FE=_tmp2FA.f4;_LL1C: {struct _fat_ptr*x=_tmp2FB;void*t=_tmp2FC;void*q=_tmp2FD;void*ct=_tmp2FE;
fn_env=({struct Cyc_Port_Cenv*_tmp4FE=fn_env;struct _tuple0*_tmp4FD=({struct _tuple0*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->f1=Cyc_Absyn_Loc_n,_tmp2FF->f2=(struct _fat_ptr*)_check_null(x);_tmp2FF;});void*_tmp4FC=t;void*_tmp4FB=q;Cyc_Port_add_var(_tmp4FE,_tmp4FD,_tmp4FC,_tmp4FB,ct,0U);});}}}
# 1777
Cyc_Port_gen_stmt(fn_env,fd->body);
# 1782
Cyc_Port_generalize(0,ctype);{
# 1789
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,ctype);
# 1793
Cyc_Port_register_rgns(env,counts,1,(void*)fntype,ctype);
env=({struct Cyc_Port_Cenv*_tmp502=fn_env;struct _tuple0*_tmp501=fd->name;void*_tmp500=(void*)fntype;void*_tmp4FF=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp502,_tmp501,_tmp500,_tmp4FF,ctype,0U);});
if((unsigned)predeclared){
# 1802
struct _tuple15 _tmp300=*predeclared;struct _tuple15 _stmttmp2A=_tmp300;struct _tuple15 _tmp301=_stmttmp2A;void*_tmp304;void*_tmp303;void*_tmp302;_LL1E: _tmp302=_tmp301.f1;_tmp303=(_tmp301.f2)->f1;_tmp304=(_tmp301.f2)->f2;_LL1F: {void*orig_type=_tmp302;void*q2=_tmp303;void*t2=_tmp304;
({void*_tmp503=q2;Cyc_Port_unifies(_tmp503,Cyc_Port_const_ct());});
({void*_tmp504=t2;Cyc_Port_unifies(_tmp504,Cyc_Port_instantiate(ctype));});
# 1806
Cyc_Port_register_rgns(env,counts,1,orig_type,ctype);}}
# 1808
return env;}}}}}case 8U: _LL9: _tmp2DD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*td=_tmp2DD;
# 1814
void*_tmp306=(void*)_check_null(td->defn);void*t=_tmp306;
void*_tmp307=Cyc_Port_type_to_ctype(env,t);void*ct=_tmp307;
(*td->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,td->name,t,ct);
return env;}case 5U: _LLB: _tmp2DC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2DC;
# 1824
struct _tuple0*_tmp308=ad->name;struct _tuple0*name=_tmp308;
(*ad->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp309=ad->kind;enum Cyc_Absyn_AggrKind _stmttmp2B=_tmp309;enum Cyc_Absyn_AggrKind _tmp30A=_stmttmp2B;if(_tmp30A == Cyc_Absyn_StructA){_LL21: _LL22:
# 1830
 previous=Cyc_Port_lookup_struct_decl(env,name);
goto _LL20;}else{_LL23: _LL24:
# 1833
 previous=Cyc_Port_lookup_union_decl(env,name);
goto _LL20;}_LL20:;}
# 1836
if((unsigned)ad->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp30B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp30B;for(0;(unsigned)fields;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp30C=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp30C;
void*qv;
if((env->gcenv)->porting){
qv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp507=env;void*_tmp506=qv;void*_tmp505=
(f->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1843
Cyc_Port_register_const_cvar(_tmp507,_tmp506,_tmp505,(f->tq).loc);});}else{
# 1852
qv=(f->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
# 1854
void*_tmp30D=Cyc_Port_type_to_ctype(env,f->type);void*ct=_tmp30D;
if(cf != 0){
struct Cyc_Port_Cfield _tmp30E=*((struct Cyc_Port_Cfield*)cf->hd);struct Cyc_Port_Cfield _stmttmp2C=_tmp30E;struct Cyc_Port_Cfield _tmp30F=_stmttmp2C;void*_tmp311;void*_tmp310;_LL26: _tmp310=_tmp30F.qual;_tmp311=_tmp30F.type;_LL27: {void*qv2=_tmp310;void*ct2=_tmp311;
cf=cf->tl;
Cyc_Port_unifies(qv,qv2);
Cyc_Port_unifies(ct,ct2);}}
# 1861
curr=({struct Cyc_List_List*_tmp313=_cycalloc(sizeof(*_tmp313));({struct Cyc_Port_Cfield*_tmp508=({struct Cyc_Port_Cfield*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->qual=qv,_tmp312->f=f->name,_tmp312->type=ct;_tmp312;});_tmp313->hd=_tmp508;}),_tmp313->tl=curr;_tmp313;});}}}
# 1863
curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1868
return env;}}case 7U: _LLD: _tmp2DB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LLE: {struct Cyc_Absyn_Enumdecl*ed=_tmp2DB;
# 1873
(*ed->name).f1=Cyc_Absyn_Loc_n;
# 1875
if((unsigned)ed->fields){
struct Cyc_List_List*_tmp314=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp314;for(0;(unsigned)fs;fs=fs->tl){
(*((struct Cyc_Absyn_Enumfield*)fs->hd)->name).f1=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_tmp50C=env;struct _tuple0*_tmp50B=((struct Cyc_Absyn_Enumfield*)fs->hd)->name;void*_tmp50A=Cyc_Absyn_enum_type(ed->name,ed);void*_tmp509=
Cyc_Port_const_ct();
# 1878
Cyc_Port_add_var(_tmp50C,_tmp50B,_tmp50A,_tmp509,
Cyc_Port_arith_ct(),0U);});}}
# 1881
return env;}default: _LLF: _LL10:
# 1883
 if((env->gcenv)->porting)
({void*_tmp315=0U;({struct Cyc___cycFILE*_tmp50E=Cyc_stderr;struct _fat_ptr _tmp50D=({const char*_tmp316="Warning: Cyclone declaration found in code to be ported -- skipping.";_tag_fat(_tmp316,sizeof(char),69U);});Cyc_fprintf(_tmp50E,_tmp50D,_tag_fat(_tmp315,sizeof(void*),0U));});});
return env;}_LL0:;}
# 1890
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(0;(unsigned)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}
return env;}
# 1897
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1899
struct Cyc_Port_Cenv*env=({struct Cyc_Port_Cenv*_tmp50F=Cyc_Port_initial_cenv();Cyc_Port_gen_topdecls(_tmp50F,ds);});
# 1901
struct Cyc_List_List*_tmp317=(env->gcenv)->pointer_edits;struct Cyc_List_List*ptrs=_tmp317;
struct Cyc_List_List*_tmp318=(env->gcenv)->qualifier_edits;struct Cyc_List_List*consts=_tmp318;
struct Cyc_List_List*_tmp319=(env->gcenv)->zeroterm_edits;struct Cyc_List_List*zts=_tmp319;
struct Cyc_List_List*_tmp31A=(env->gcenv)->edits;struct Cyc_List_List*edits=_tmp31A;
struct Cyc_List_List*_tmp31B=(env->gcenv)->vardecl_locs;struct Cyc_List_List*localvars=_tmp31B;
# 1907
for(0;(unsigned)localvars;localvars=localvars->tl){
struct _tuple16 _tmp31C=*((struct _tuple16*)localvars->hd);struct _tuple16 _stmttmp2D=_tmp31C;struct _tuple16 _tmp31D=_stmttmp2D;void*_tmp320;unsigned _tmp31F;struct _tuple0*_tmp31E;_LL1: _tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;_tmp320=_tmp31D.f3;_LL2: {struct _tuple0*var=_tmp31E;unsigned loc=_tmp31F;void*vartype=_tmp320;
struct _tuple0 _tmp321=*var;struct _tuple0 _stmttmp2E=_tmp321;struct _tuple0 _tmp322=_stmttmp2E;struct _fat_ptr*_tmp323;_LL4: _tmp323=_tmp322.f2;_LL5: {struct _fat_ptr*x=_tmp323;
struct Cyc_Port_VarUsage*_tmp324=((struct Cyc_Port_VarUsage*(*)(struct Cyc_Hashtable_Table*t,unsigned key))Cyc_Hashtable_lookup)((env->gcenv)->varusage_tab,loc);struct Cyc_Port_VarUsage*varusage=_tmp324;
if(((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken){
if((unsigned)varusage->init){
# 1914
edits=({struct Cyc_List_List*_tmp328=_cycalloc(sizeof(*_tmp328));({struct Cyc_Port_Edit*_tmp512=({struct Cyc_Port_Edit*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->loc=loc,({struct _fat_ptr _tmp511=({const char*_tmp325="@";_tag_fat(_tmp325,sizeof(char),2U);});_tmp327->old_string=_tmp511;}),({struct _fat_ptr _tmp510=({const char*_tmp326="";_tag_fat(_tmp326,sizeof(char),1U);});_tmp327->new_string=_tmp510;});_tmp327;});_tmp328->hd=_tmp512;}),_tmp328->tl=edits;_tmp328;});
edits=({struct Cyc_List_List*_tmp32C=_cycalloc(sizeof(*_tmp32C));({struct Cyc_Port_Edit*_tmp515=({struct Cyc_Port_Edit*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->loc=((struct Cyc_Absyn_Exp*)_check_null(varusage->init))->loc,({struct _fat_ptr _tmp514=({const char*_tmp329="new ";_tag_fat(_tmp329,sizeof(char),5U);});_tmp32B->old_string=_tmp514;}),({struct _fat_ptr _tmp513=({const char*_tmp32A="";_tag_fat(_tmp32A,sizeof(char),1U);});_tmp32B->new_string=_tmp513;});_tmp32B;});_tmp32C->hd=_tmp515;}),_tmp32C->tl=edits;_tmp32C;});}else{
# 1919
edits=({struct Cyc_List_List*_tmp332=_cycalloc(sizeof(*_tmp332));({struct Cyc_Port_Edit*_tmp519=({struct Cyc_Port_Edit*_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331->loc=loc,({struct _fat_ptr _tmp518=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32F=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0U,_tmp3B0.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp3B0;});struct Cyc_String_pa_PrintArg_struct _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0U,({struct _fat_ptr _tmp516=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(vartype));_tmp3AF.f1=_tmp516;});_tmp3AF;});void*_tmp32D[2U];_tmp32D[0]=& _tmp32F,_tmp32D[1]=& _tmp330;({struct _fat_ptr _tmp517=({const char*_tmp32E="?%s = malloc(sizeof(%s))";_tag_fat(_tmp32E,sizeof(char),25U);});Cyc_aprintf(_tmp517,_tag_fat(_tmp32D,sizeof(void*),2U));});});_tmp331->old_string=_tmp518;}),_tmp331->new_string=*x;_tmp331;});_tmp332->hd=_tmp519;}),_tmp332->tl=edits;_tmp332;});}{
# 1921
struct Cyc_List_List*_tmp333=varusage->usage_locs;struct Cyc_List_List*loclist=_tmp333;
for(0;(unsigned)loclist;loclist=loclist->tl){
edits=({struct Cyc_List_List*_tmp338=_cycalloc(sizeof(*_tmp338));({struct Cyc_Port_Edit*_tmp51C=({struct Cyc_Port_Edit*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->loc=(unsigned)loclist->hd,({struct _fat_ptr _tmp51B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp336=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0U,_tmp3B1.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp3B1;});void*_tmp334[1U];_tmp334[0]=& _tmp336;({struct _fat_ptr _tmp51A=({const char*_tmp335="(*%s)";_tag_fat(_tmp335,sizeof(char),6U);});Cyc_aprintf(_tmp51A,_tag_fat(_tmp334,sizeof(void*),1U));});});_tmp337->old_string=_tmp51B;}),_tmp337->new_string=*x;_tmp337;});_tmp338->hd=_tmp51C;}),_tmp338->tl=edits;_tmp338;});}}}}}}
# 1929
{struct Cyc_List_List*_tmp339=ptrs;struct Cyc_List_List*ps=_tmp339;for(0;(unsigned)ps;ps=ps->tl){
struct _tuple17 _tmp33A=*((struct _tuple17*)ps->hd);struct _tuple17 _stmttmp2F=_tmp33A;struct _tuple17 _tmp33B=_stmttmp2F;unsigned _tmp33E;void*_tmp33D;void*_tmp33C;_LL7: _tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;_tmp33E=_tmp33B.f3;_LL8: {void*new_ctype=_tmp33C;void*orig_ctype=_tmp33D;unsigned loc=_tmp33E;
Cyc_Port_unifies(new_ctype,orig_ctype);}}}
# 1933
{struct Cyc_List_List*_tmp33F=consts;struct Cyc_List_List*cs=_tmp33F;for(0;(unsigned)cs;cs=cs->tl){
struct _tuple17 _tmp340=*((struct _tuple17*)cs->hd);struct _tuple17 _stmttmp30=_tmp340;struct _tuple17 _tmp341=_stmttmp30;unsigned _tmp344;void*_tmp343;void*_tmp342;_LLA: _tmp342=_tmp341.f1;_tmp343=_tmp341.f2;_tmp344=_tmp341.f3;_LLB: {void*new_qual=_tmp342;void*old_qual=_tmp343;unsigned loc=_tmp344;
Cyc_Port_unifies(new_qual,old_qual);}}}
# 1937
{struct Cyc_List_List*_tmp345=zts;struct Cyc_List_List*zs=_tmp345;for(0;(unsigned)zs;zs=zs->tl){
struct _tuple17 _tmp346=*((struct _tuple17*)zs->hd);struct _tuple17 _stmttmp31=_tmp346;struct _tuple17 _tmp347=_stmttmp31;unsigned _tmp34A;void*_tmp349;void*_tmp348;_LLD: _tmp348=_tmp347.f1;_tmp349=_tmp347.f2;_tmp34A=_tmp347.f3;_LLE: {void*new_zt=_tmp348;void*old_zt=_tmp349;unsigned loc=_tmp34A;
Cyc_Port_unifies(new_zt,old_zt);}}}
# 1943
for(0;(unsigned)ptrs;ptrs=ptrs->tl){
struct _tuple17 _tmp34B=*((struct _tuple17*)ptrs->hd);struct _tuple17 _stmttmp32=_tmp34B;struct _tuple17 _tmp34C=_stmttmp32;unsigned _tmp34F;void*_tmp34E;void*_tmp34D;_LL10: _tmp34D=_tmp34C.f1;_tmp34E=_tmp34C.f2;_tmp34F=_tmp34C.f3;_LL11: {void*new_ctype=_tmp34D;void*orig_ctype=_tmp34E;unsigned loc=_tmp34F;
if(!Cyc_Port_unifies(new_ctype,orig_ctype)&&(int)loc){
void*_tmp350=Cyc_Port_compress_ct(orig_ctype);void*_stmttmp33=_tmp350;void*_tmp351=_stmttmp33;switch(*((int*)_tmp351)){case 2U: _LL13: _LL14:
# 1948
 edits=({struct Cyc_List_List*_tmp355=_cycalloc(sizeof(*_tmp355));({struct Cyc_Port_Edit*_tmp51F=({struct Cyc_Port_Edit*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->loc=loc,({struct _fat_ptr _tmp51E=({const char*_tmp352="?";_tag_fat(_tmp352,sizeof(char),2U);});_tmp354->old_string=_tmp51E;}),({struct _fat_ptr _tmp51D=({const char*_tmp353="*";_tag_fat(_tmp353,sizeof(char),2U);});_tmp354->new_string=_tmp51D;});_tmp354;});_tmp355->hd=_tmp51F;}),_tmp355->tl=edits;_tmp355;});
goto _LL12;case 3U: _LL15: _LL16:
# 1951
 edits=({struct Cyc_List_List*_tmp359=_cycalloc(sizeof(*_tmp359));({struct Cyc_Port_Edit*_tmp522=({struct Cyc_Port_Edit*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->loc=loc,({struct _fat_ptr _tmp521=({const char*_tmp356="*";_tag_fat(_tmp356,sizeof(char),2U);});_tmp358->old_string=_tmp521;}),({struct _fat_ptr _tmp520=({const char*_tmp357="?";_tag_fat(_tmp357,sizeof(char),2U);});_tmp358->new_string=_tmp520;});_tmp358;});_tmp359->hd=_tmp522;}),_tmp359->tl=edits;_tmp359;});
goto _LL12;default: _LL17: _LL18:
 goto _LL12;}_LL12:;}}}
# 1958
for(0;(unsigned)consts;consts=consts->tl){
struct _tuple17 _tmp35A=*((struct _tuple17*)consts->hd);struct _tuple17 _stmttmp34=_tmp35A;struct _tuple17 _tmp35B=_stmttmp34;unsigned _tmp35E;void*_tmp35D;void*_tmp35C;_LL1A: _tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;_tmp35E=_tmp35B.f3;_LL1B: {void*new_qual=_tmp35C;void*old_qual=_tmp35D;unsigned loc=_tmp35E;
if(!Cyc_Port_unifies(new_qual,old_qual)&&(int)loc){
void*_tmp35F=Cyc_Port_compress_ct(old_qual);void*_stmttmp35=_tmp35F;void*_tmp360=_stmttmp35;switch(*((int*)_tmp360)){case 1U: _LL1D: _LL1E:
# 1963
 edits=({struct Cyc_List_List*_tmp364=_cycalloc(sizeof(*_tmp364));({struct Cyc_Port_Edit*_tmp525=({struct Cyc_Port_Edit*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363->loc=loc,({struct _fat_ptr _tmp524=({const char*_tmp361="const ";_tag_fat(_tmp361,sizeof(char),7U);});_tmp363->old_string=_tmp524;}),({struct _fat_ptr _tmp523=({const char*_tmp362="";_tag_fat(_tmp362,sizeof(char),1U);});_tmp363->new_string=_tmp523;});_tmp363;});_tmp364->hd=_tmp525;}),_tmp364->tl=edits;_tmp364;});goto _LL1C;case 0U: _LL1F: _LL20:
# 1965
 edits=({struct Cyc_List_List*_tmp368=_cycalloc(sizeof(*_tmp368));({struct Cyc_Port_Edit*_tmp528=({struct Cyc_Port_Edit*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->loc=loc,({struct _fat_ptr _tmp527=({const char*_tmp365="";_tag_fat(_tmp365,sizeof(char),1U);});_tmp367->old_string=_tmp527;}),({struct _fat_ptr _tmp526=({const char*_tmp366="const ";_tag_fat(_tmp366,sizeof(char),7U);});_tmp367->new_string=_tmp526;});_tmp367;});_tmp368->hd=_tmp528;}),_tmp368->tl=edits;_tmp368;});goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}}}
# 1971
for(0;(unsigned)zts;zts=zts->tl){
struct _tuple17 _tmp369=*((struct _tuple17*)zts->hd);struct _tuple17 _stmttmp36=_tmp369;struct _tuple17 _tmp36A=_stmttmp36;unsigned _tmp36D;void*_tmp36C;void*_tmp36B;_LL24: _tmp36B=_tmp36A.f1;_tmp36C=_tmp36A.f2;_tmp36D=_tmp36A.f3;_LL25: {void*new_zt=_tmp36B;void*old_zt=_tmp36C;unsigned loc=_tmp36D;
if(!Cyc_Port_unifies(new_zt,old_zt)&&(int)loc){
void*_tmp36E=Cyc_Port_compress_ct(old_zt);void*_stmttmp37=_tmp36E;void*_tmp36F=_stmttmp37;switch(*((int*)_tmp36F)){case 8U: _LL27: _LL28:
# 1976
 edits=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*_tmp373));({struct Cyc_Port_Edit*_tmp52B=({struct Cyc_Port_Edit*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->loc=loc,({struct _fat_ptr _tmp52A=({const char*_tmp370="@nozeroterm ";_tag_fat(_tmp370,sizeof(char),13U);});_tmp372->old_string=_tmp52A;}),({struct _fat_ptr _tmp529=({const char*_tmp371="";_tag_fat(_tmp371,sizeof(char),1U);});_tmp372->new_string=_tmp529;});_tmp372;});_tmp373->hd=_tmp52B;}),_tmp373->tl=edits;_tmp373;});goto _LL26;case 9U: _LL29: _LL2A:
# 1978
 edits=({struct Cyc_List_List*_tmp377=_cycalloc(sizeof(*_tmp377));({struct Cyc_Port_Edit*_tmp52E=({struct Cyc_Port_Edit*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->loc=loc,({struct _fat_ptr _tmp52D=({const char*_tmp374="@zeroterm ";_tag_fat(_tmp374,sizeof(char),11U);});_tmp376->old_string=_tmp52D;}),({struct _fat_ptr _tmp52C=({const char*_tmp375="";_tag_fat(_tmp375,sizeof(char),1U);});_tmp376->new_string=_tmp52C;});_tmp376;});_tmp377->hd=_tmp52E;}),_tmp377->tl=edits;_tmp377;});goto _LL26;default: _LL2B: _LL2C:
 goto _LL26;}_LL26:;}}}
# 1985
edits=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,edits);
# 1987
while((unsigned)edits &&((struct Cyc_Port_Edit*)edits->hd)->loc == (unsigned)0){
# 1991
edits=edits->tl;}
# 1993
return edits;}
# 1998
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp378=Cyc_Port_gen_edits(ds);struct Cyc_List_List*edits=_tmp378;
struct Cyc_List_List*_tmp379=((struct Cyc_List_List*(*)(unsigned(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,edits);struct Cyc_List_List*locs=_tmp379;
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp37A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(locs));struct Cyc_List_List*slocs=_tmp37A;
for(0;(unsigned)edits;(edits=edits->tl,slocs=slocs->tl)){
struct Cyc_Port_Edit _tmp37B=*((struct Cyc_Port_Edit*)edits->hd);struct Cyc_Port_Edit _stmttmp38=_tmp37B;struct Cyc_Port_Edit _tmp37C=_stmttmp38;struct _fat_ptr _tmp37F;struct _fat_ptr _tmp37E;unsigned _tmp37D;_LL1: _tmp37D=_tmp37C.loc;_tmp37E=_tmp37C.old_string;_tmp37F=_tmp37C.new_string;_LL2: {unsigned loc=_tmp37D;struct _fat_ptr s1=_tmp37E;struct _fat_ptr s2=_tmp37F;
struct _fat_ptr sloc=(struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(slocs))->hd);
({struct Cyc_String_pa_PrintArg_struct _tmp382=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0U,_tmp3B4.f1=(struct _fat_ptr)((struct _fat_ptr)sloc);_tmp3B4;});struct Cyc_String_pa_PrintArg_struct _tmp383=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0U,_tmp3B3.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp3B3;});struct Cyc_String_pa_PrintArg_struct _tmp384=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0U,_tmp3B2.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp3B2;});void*_tmp380[3U];_tmp380[0]=& _tmp382,_tmp380[1]=& _tmp383,_tmp380[2]=& _tmp384;({struct _fat_ptr _tmp52F=({const char*_tmp381="%s: insert `%s' for `%s'\n";_tag_fat(_tmp381,sizeof(char),26U);});Cyc_printf(_tmp52F,_tag_fat(_tmp380,sizeof(void*),3U));});});}}}}

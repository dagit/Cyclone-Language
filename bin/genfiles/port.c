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
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 38 "position.h"
extern struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 46
extern int Cyc_Position_use_gcc_style_location;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 844 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 851
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 858
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 864
void*Cyc_Absyn_compress(void*);
# 879
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 884
extern void*Cyc_Absyn_sint_type;
# 893
extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);
# 917
void*Cyc_Absyn_string_type(void*);
# 920
extern void*Cyc_Absyn_fat_bound_type;
# 938
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1127
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
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 67
extern int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*,void*,void**);struct Cyc_Port_Edit{unsigned loc;struct _fat_ptr old_string;struct _fat_ptr new_string;};
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
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9};
# 166
static struct _fat_ptr Cyc_Port_ctypes2string(int,struct Cyc_List_List*);
static struct _fat_ptr Cyc_Port_cfields2string(int,struct Cyc_List_List*);
static struct _fat_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_tmp0=t;void*_tmp5;void*_tmp4;void*_tmp3;void*_tmp2;void*_tmp1;switch(*((int*)_tmp0)){case 0:
 return({const char*_tmp6="const";_tag_fat(_tmp6,sizeof(char),6U);});case 1:
 return({const char*_tmp7="notconst";_tag_fat(_tmp7,sizeof(char),9U);});case 2:
 return({const char*_tmp8="thin";_tag_fat(_tmp8,sizeof(char),5U);});case 3:
 return({const char*_tmp9="fat";_tag_fat(_tmp9,sizeof(char),4U);});case 4:
 return({const char*_tmpA="void";_tag_fat(_tmpA,sizeof(char),5U);});case 5:
 return({const char*_tmpB="zero";_tag_fat(_tmpB,sizeof(char),5U);});case 6:
 return({const char*_tmpC="arith";_tag_fat(_tmpC,sizeof(char),6U);});case 7:
 return({const char*_tmpD="heap";_tag_fat(_tmpD,sizeof(char),5U);});case 8:
 return({const char*_tmpE="ZT";_tag_fat(_tmpE,sizeof(char),3U);});case 9:
 return({const char*_tmpF="NZT";_tag_fat(_tmpF,sizeof(char),4U);});case 10: _tmp1=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp0)->f1;{struct _fat_ptr*n=_tmp1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp2E6;_tmp2E6.tag=0,_tmp2E6.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp2E6;});void*_tmp10[1];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp316=({const char*_tmp11="%s";_tag_fat(_tmp11,sizeof(char),3U);});Cyc_aprintf(_tmp316,_tag_fat(_tmp10,sizeof(void*),1));});});}case 11: _tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f2;_tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f3;_tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f4;_tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f5;{void*elt=_tmp1;void*qual=_tmp2;void*k=_tmp3;void*rgn=_tmp4;void*zt=_tmp5;
# 182
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp2EB;_tmp2EB.tag=0,({struct _fat_ptr _tmp317=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,elt));_tmp2EB.f1=_tmp317;});_tmp2EB;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp2EA;_tmp2EA.tag=0,({
struct _fat_ptr _tmp318=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,qual));_tmp2EA.f1=_tmp318;});_tmp2EA;});struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp2E9;_tmp2E9.tag=0,({struct _fat_ptr _tmp319=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,k));_tmp2E9.f1=_tmp319;});_tmp2E9;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp2E8;_tmp2E8.tag=0,({
struct _fat_ptr _tmp31A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,rgn));_tmp2E8.f1=_tmp31A;});_tmp2E8;});struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp2E7;_tmp2E7.tag=0,({struct _fat_ptr _tmp31B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,zt));_tmp2E7.f1=_tmp31B;});_tmp2E7;});void*_tmp13[5];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16,_tmp13[2]=& _tmp17,_tmp13[3]=& _tmp18,_tmp13[4]=& _tmp19;({struct _fat_ptr _tmp31C=({const char*_tmp14="ptr(%s,%s,%s,%s,%s)";_tag_fat(_tmp14,sizeof(char),20U);});Cyc_aprintf(_tmp31C,_tag_fat(_tmp13,sizeof(void*),5));});});}case 12: _tmp1=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f2;_tmp3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f3;{void*elt=_tmp1;void*qual=_tmp2;void*zt=_tmp3;
# 186
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp2EE;_tmp2EE.tag=0,({struct _fat_ptr _tmp31D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,elt));_tmp2EE.f1=_tmp31D;});_tmp2EE;});struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp2ED;_tmp2ED.tag=0,({
struct _fat_ptr _tmp31E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,qual));_tmp2ED.f1=_tmp31E;});_tmp2ED;});struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp2EC;_tmp2EC.tag=0,({struct _fat_ptr _tmp31F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,zt));_tmp2EC.f1=_tmp31F;});_tmp2EC;});void*_tmp1A[3];_tmp1A[0]=& _tmp1C,_tmp1A[1]=& _tmp1D,_tmp1A[2]=& _tmp1E;({struct _fat_ptr _tmp320=({const char*_tmp1B="array(%s,%s,%s)";_tag_fat(_tmp1B,sizeof(char),16U);});Cyc_aprintf(_tmp320,_tag_fat(_tmp1A,sizeof(void*),3));});});}case 13: _tmp1=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f1;_tmp2=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_tmp1;struct Cyc_List_List*cfs=_tmp2;
# 189
struct _fat_ptr s=(int)ad->kind == 0?({const char*_tmp28="struct";_tag_fat(_tmp28,sizeof(char),7U);}):({const char*_tmp29="union";_tag_fat(_tmp29,sizeof(char),6U);});
if(!deep)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp2F0;_tmp2F0.tag=0,_tmp2F0.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2F0;});struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=0,({struct _fat_ptr _tmp321=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp2EF.f1=_tmp321;});_tmp2EF;});void*_tmp1F[2];_tmp1F[0]=& _tmp21,_tmp1F[1]=& _tmp22;({struct _fat_ptr _tmp322=({const char*_tmp20="%s %s";_tag_fat(_tmp20,sizeof(char),6U);});Cyc_aprintf(_tmp322,_tag_fat(_tmp1F,sizeof(void*),2));});});else{
# 193
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp25=({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0,_tmp2F3.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2F3;});struct Cyc_String_pa_PrintArg_struct _tmp26=({struct Cyc_String_pa_PrintArg_struct _tmp2F2;_tmp2F2.tag=0,({struct _fat_ptr _tmp323=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp2F2.f1=_tmp323;});_tmp2F2;});struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp2F1;_tmp2F1.tag=0,({
struct _fat_ptr _tmp324=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_cfields2string(0,cfs));_tmp2F1.f1=_tmp324;});_tmp2F1;});void*_tmp23[3];_tmp23[0]=& _tmp25,_tmp23[1]=& _tmp26,_tmp23[2]=& _tmp27;({struct _fat_ptr _tmp325=({const char*_tmp24="%s %s {%s}";_tag_fat(_tmp24,sizeof(char),11U);});Cyc_aprintf(_tmp325,_tag_fat(_tmp23,sizeof(void*),3));});});}}case 14: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2 != 0){_tmp1=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp2=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2;{struct Cyc_List_List*cfs=_tmp1;void*eq=_tmp2;
return Cyc_Port_ctype2string(deep,eq);}}else{_tmp1=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;{struct Cyc_List_List*cfs=_tmp1;
# 197
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C=({struct Cyc_String_pa_PrintArg_struct _tmp2F4;_tmp2F4.tag=0,({struct _fat_ptr _tmp326=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_cfields2string(deep,cfs));_tmp2F4.f1=_tmp326;});_tmp2F4;});void*_tmp2A[1];_tmp2A[0]=& _tmp2C;({struct _fat_ptr _tmp327=({const char*_tmp2B="aggr {%s}";_tag_fat(_tmp2B,sizeof(char),10U);});Cyc_aprintf(_tmp327,_tag_fat(_tmp2A,sizeof(void*),1));});});}}case 15: _tmp1=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp2=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f2;{void*t=_tmp1;struct Cyc_List_List*ts=_tmp2;
# 199
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp2F6;_tmp2F6.tag=0,({struct _fat_ptr _tmp328=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(deep,ts));_tmp2F6.f1=_tmp328;});_tmp2F6;});struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=0,({struct _fat_ptr _tmp329=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,t));_tmp2F5.f1=_tmp329;});_tmp2F5;});void*_tmp2D[2];_tmp2D[0]=& _tmp2F,_tmp2D[1]=& _tmp30;({struct _fat_ptr _tmp32A=({const char*_tmp2E="fn(%s)->%s";_tag_fat(_tmp2E,sizeof(char),11U);});Cyc_aprintf(_tmp32A,_tag_fat(_tmp2D,sizeof(void*),2));});});}default: _tmp1=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp0)->f1;{struct Cyc_Port_Cvar*cv=_tmp1;
# 201
if((unsigned)cv->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(cv->eq)));else{
if(!deep || cv->uppers == 0 && cv->lowers == 0)
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp33=({struct Cyc_Int_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=1,_tmp2F7.f1=(unsigned long)cv->id;_tmp2F7;});void*_tmp31[1];_tmp31[0]=& _tmp33;({struct _fat_ptr _tmp32B=({const char*_tmp32="var(%d)";_tag_fat(_tmp32,sizeof(char),8U);});Cyc_aprintf(_tmp32B,_tag_fat(_tmp31,sizeof(void*),1));});});else{
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp36=({struct Cyc_String_pa_PrintArg_struct _tmp2FA;_tmp2FA.tag=0,({
struct _fat_ptr _tmp32C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(0,cv->lowers));_tmp2FA.f1=_tmp32C;});_tmp2FA;});struct Cyc_Int_pa_PrintArg_struct _tmp37=({struct Cyc_Int_pa_PrintArg_struct _tmp2F9;_tmp2F9.tag=1,_tmp2F9.f1=(unsigned long)cv->id;_tmp2F9;});struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp2F8;_tmp2F8.tag=0,({
# 208
struct _fat_ptr _tmp32D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctypes2string(0,cv->uppers));_tmp2F8.f1=_tmp32D;});_tmp2F8;});void*_tmp34[3];_tmp34[0]=& _tmp36,_tmp34[1]=& _tmp37,_tmp34[2]=& _tmp38;({struct _fat_ptr _tmp32E=({const char*_tmp35="var([%s]<=%d<=[%s])";_tag_fat(_tmp35,sizeof(char),20U);});Cyc_aprintf(_tmp32E,_tag_fat(_tmp34,sizeof(void*),3));});});}}}};}
# 211
static struct _fat_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){return({struct _fat_ptr*_tmp39=_cycalloc(sizeof(*_tmp39));({struct _fat_ptr _tmp32F=Cyc_Port_ctype2string(deep,t);*_tmp39=_tmp32F;});_tmp39;});}
struct Cyc_List_List*Cyc_Port_sep(struct _fat_ptr s,struct Cyc_List_List*xs){
struct _fat_ptr*_tmp3A=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));*_tmp3E=s;_tmp3E;});struct _fat_ptr*sptr=_tmp3A;
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp3B=xs;struct Cyc_List_List*prev=_tmp3B;
struct Cyc_List_List*_tmp3C=xs->tl;struct Cyc_List_List*curr=_tmp3C;
for(1;curr != 0;(prev=curr,curr=curr->tl)){
({struct Cyc_List_List*_tmp330=({struct Cyc_List_List*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->hd=sptr,_tmp3D->tl=curr;_tmp3D;});prev->tl=_tmp330;});}
# 220
return xs;}}
# 222
static struct _fat_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=0,({
struct _fat_ptr _tmp331=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,f->qual));_tmp2FD.f1=_tmp331;});_tmp2FD;});struct Cyc_String_pa_PrintArg_struct _tmp43=({struct Cyc_String_pa_PrintArg_struct _tmp2FC;_tmp2FC.tag=0,_tmp2FC.f1=(struct _fat_ptr)((struct _fat_ptr)*f->f);_tmp2FC;});struct Cyc_String_pa_PrintArg_struct _tmp44=({struct Cyc_String_pa_PrintArg_struct _tmp2FB;_tmp2FB.tag=0,({struct _fat_ptr _tmp332=(struct _fat_ptr)((struct _fat_ptr)Cyc_Port_ctype2string(deep,f->type));_tmp2FB.f1=_tmp332;});_tmp2FB;});void*_tmp40[3];_tmp40[0]=& _tmp42,_tmp40[1]=& _tmp43,_tmp40[2]=& _tmp44;({struct _fat_ptr _tmp333=({const char*_tmp41="%s %s: %s";_tag_fat(_tmp41,sizeof(char),10U);});Cyc_aprintf(_tmp333,_tag_fat(_tmp40,sizeof(void*),3));});});
return({struct _fat_ptr*_tmp3F=_cycalloc(sizeof(*_tmp3F));*_tmp3F=s;_tmp3F;});}
# 227
static struct _fat_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr _tmp334=({const char*_tmp45=",";_tag_fat(_tmp45,sizeof(char),2U);});Cyc_Port_sep(_tmp334,({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(int,void*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Port_ctype2stringptr,deep,ts));}));}
# 230
static struct _fat_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr _tmp335=({const char*_tmp46=";";_tag_fat(_tmp46,sizeof(char),2U);});Cyc_Port_sep(_tmp335,({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(int,struct Cyc_Port_Cfield*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Port_cfield2stringptr,deep,fs));}));}
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
static void*Cyc_Port_rgnvar_ct(struct _fat_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->tag=10,_tmp47->f1=n;_tmp47;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=14,_tmp48->f1=fs,_tmp48->f2=0;_tmp48;});}
# 250
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
return(void*)({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=13,_tmp49->f1=p;_tmp49;});}
# 253
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 255
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=11,_tmp4A->f1=elt,_tmp4A->f2=qual,_tmp4A->f3=ptr_kind,_tmp4A->f4=r,_tmp4A->f5=zt;_tmp4A;});}
# 257
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=12,_tmp4B->f1=elt,_tmp4B->f2=qual,_tmp4B->f3=zt;_tmp4B;});}
# 260
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=15,_tmp4C->f1=return_type,_tmp4C->f2=args;_tmp4C;});}
# 263
static void*Cyc_Port_var (void){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=16,({struct Cyc_Port_Cvar*_tmp336=({struct Cyc_Port_Cvar*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->id=counter ++,_tmp4D->eq=0,_tmp4D->uppers=0,_tmp4D->lowers=0;_tmp4D;});_tmp4E->f1=_tmp336;});_tmp4E;});}
# 267
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 270
static struct _fat_ptr*Cyc_Port_new_region_var (void){
static int counter=0;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp52=({struct Cyc_Int_pa_PrintArg_struct _tmp2FE;_tmp2FE.tag=1,_tmp2FE.f1=(unsigned long)counter ++;_tmp2FE;});void*_tmp50[1];_tmp50[0]=& _tmp52;({struct _fat_ptr _tmp337=({const char*_tmp51="`R%d";_tag_fat(_tmp51,sizeof(char),5U);});Cyc_aprintf(_tmp337,_tag_fat(_tmp50,sizeof(void*),1));});});
return({struct _fat_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));*_tmp4F=s;_tmp4F;});}
# 278
static int Cyc_Port_unifies(void*,void*);
# 280
static void*Cyc_Port_compress_ct(void*t){
void*_tmp53=t;void*_tmp56;void*_tmp55;void*_tmp54;switch(*((int*)_tmp53)){case 16: _tmp54=(void***)&(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp53)->f1)->eq;_tmp55=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp53)->f1)->uppers;_tmp56=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp53)->f1)->lowers;{void***eqp=_tmp54;struct Cyc_List_List*us=_tmp55;struct Cyc_List_List*ls=_tmp56;
# 283
void**_tmp57=*eqp;void**eq=_tmp57;
if((unsigned)eq){
# 287
void*r=Cyc_Port_compress_ct(*eq);
if(*eq != r)({void**_tmp338=({void**_tmp58=_cycalloc(sizeof(*_tmp58));*_tmp58=r;_tmp58;});*eqp=_tmp338;});
return r;}
# 294
for(1;ls != 0;ls=ls->tl){
void*_tmp59=(void*)ls->hd;void*_stmttmp0=_tmp59;void*_tmp5A=_stmttmp0;switch(*((int*)_tmp5A)){case 0:
 goto _LLB;case 9: _LLB:
 goto _LLD;case 4: _LLD:
 goto _LLF;case 13: _LLF:
 goto _LL11;case 15: _LL11:
# 301
({void**_tmp339=({void**_tmp5B=_cycalloc(sizeof(*_tmp5B));*_tmp5B=(void*)ls->hd;_tmp5B;});*eqp=_tmp339;});
return(void*)ls->hd;default:
# 304
 goto _LL7;}_LL7:;}
# 307
for(1;us != 0;us=us->tl){
void*_tmp5C=(void*)us->hd;void*_stmttmp1=_tmp5C;void*_tmp5D=_stmttmp1;switch(*((int*)_tmp5D)){case 1:
 goto _LL18;case 8: _LL18:
 goto _LL1A;case 5: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 15: _LL1E:
# 314
({void**_tmp33A=({void**_tmp5E=_cycalloc(sizeof(*_tmp5E));*_tmp5E=(void*)us->hd;_tmp5E;});*eqp=_tmp33A;});
return(void*)us->hd;default:
# 317
 goto _LL14;}_LL14:;}
# 320
return t;}case 14: _tmp54=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp53)->f2;{void**eq=_tmp54;
# 323
if((unsigned)eq)return Cyc_Port_compress_ct(*eq);else{
return t;}}default:
# 327
 return t;};}struct _tuple12{void*f1;void*f2;};
# 333
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp5F=Cyc_Port_compress_ct(t);void*_stmttmp2=_tmp5F;void*_tmp60=_stmttmp2;void*_tmp65;void*_tmp64;void*_tmp63;void*_tmp62;void*_tmp61;switch(*((int*)_tmp60)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 5: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 8: _LL10:
 goto _LL12;case 9: _LL12:
 goto _LL14;case 14: _LL14:
 goto _LL16;case 13: _LL16:
 goto _LL18;case 16: _LL18:
 goto _LL1A;case 7: _LL1A:
 return t;case 10: _tmp61=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp60)->f1;{struct _fat_ptr*n=_tmp61;
# 349
if(!({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(*instenv,n))
({struct Cyc_Dict_Dict _tmp33E=({struct Cyc_Dict_Dict(*_tmp33D)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmp33C=*instenv;struct _fat_ptr*_tmp33B=n;_tmp33D(_tmp33C,_tmp33B,Cyc_Port_var());});*instenv=_tmp33E;});
return({(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(*instenv,n);}case 11: _tmp61=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp60)->f1;_tmp62=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp60)->f2;_tmp63=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp60)->f3;_tmp64=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp60)->f4;_tmp65=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp60)->f5;{void*t1=_tmp61;void*t2=_tmp62;void*t3=_tmp63;void*t4=_tmp64;void*zt=_tmp65;
# 353
struct _tuple12 _tmp66=({struct _tuple12 _tmp2FF;({void*_tmp340=Cyc_Port_inst(instenv,t1);_tmp2FF.f1=_tmp340;}),({void*_tmp33F=Cyc_Port_inst(instenv,t4);_tmp2FF.f2=_tmp33F;});_tmp2FF;});struct _tuple12 _stmttmp3=_tmp66;struct _tuple12 _tmp67=_stmttmp3;void*_tmp69;void*_tmp68;_tmp68=_tmp67.f1;_tmp69=_tmp67.f2;{void*nt1=_tmp68;void*nt4=_tmp69;
if(nt1 == t1 && nt4 == t4)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=11,_tmp6A->f1=nt1,_tmp6A->f2=t2,_tmp6A->f3=t3,_tmp6A->f4=nt4,_tmp6A->f5=zt;_tmp6A;});}}case 12: _tmp61=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp60)->f1;_tmp62=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp60)->f2;_tmp63=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp60)->f3;{void*t1=_tmp61;void*t2=_tmp62;void*zt=_tmp63;
# 357
void*_tmp6B=Cyc_Port_inst(instenv,t1);void*nt1=_tmp6B;
if(nt1 == t1)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=12,_tmp6C->f1=nt1,_tmp6C->f2=t2,_tmp6C->f3=zt;_tmp6C;});}default: _tmp61=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp60)->f1;_tmp62=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp60)->f2;{void*t1=_tmp61;struct Cyc_List_List*ts=_tmp62;
# 361
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=15,({void*_tmp342=Cyc_Port_inst(instenv,t1);_tmp6D->f1=_tmp342;}),({struct Cyc_List_List*_tmp341=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Port_inst,instenv,ts);_tmp6D->f2=_tmp341;});_tmp6D;});}};}
# 365
void*Cyc_Port_instantiate(void*t){
return({struct Cyc_Dict_Dict*_tmp344=({struct Cyc_Dict_Dict*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct Cyc_Dict_Dict _tmp343=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);*_tmp6E=_tmp343;});_tmp6E;});Cyc_Port_inst(_tmp344,t);});}
# 372
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*_tmp6F=ts;struct Cyc_List_List*xs=_tmp6F;for(0;(unsigned)xs;xs=xs->tl){
void*_tmp70=Cyc_Port_compress_ct((void*)xs->hd);void*t2=_tmp70;
if(t == t2)found=1;}}
# 378
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(1;ts != 0;ts=ts->tl){
if(({void*_tmp345=t;_tmp345 == Cyc_Port_compress_ct((void*)ts->hd);}))continue;
res=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=(void*)ts->hd,_tmp71->tl=res;_tmp71;});}
# 384
return res;}}
# 389
void Cyc_Port_generalize(int is_rgn,void*t){
t=Cyc_Port_compress_ct(t);{
void*_tmp72=t;void*_tmp77;void*_tmp76;void*_tmp75;void*_tmp74;void*_tmp73;switch(*((int*)_tmp72)){case 16: _tmp73=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp72)->f1;{struct Cyc_Port_Cvar*cv=_tmp73;
# 394
({struct Cyc_List_List*_tmp346=Cyc_Port_filter_self(t,cv->uppers);cv->uppers=_tmp346;});
({struct Cyc_List_List*_tmp347=Cyc_Port_filter_self(t,cv->lowers);cv->lowers=_tmp347;});
if(is_rgn){
# 399
if(cv->uppers == 0 && cv->lowers == 0){
({void*_tmp348=t;Cyc_Port_unifies(_tmp348,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));});
return;}
# 404
if((unsigned)cv->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(cv->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 408
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(cv->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 412
return;}case 0:
 goto _LL6;case 1: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 3: _LLA:
 goto _LLC;case 4: _LLC:
 goto _LLE;case 5: _LLE:
 goto _LL10;case 6: _LL10:
 goto _LL12;case 14: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 9: _LL18:
 goto _LL1A;case 8: _LL1A:
 goto _LL1C;case 7: _LL1C:
 return;case 11: _tmp73=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp72)->f1;_tmp74=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp72)->f2;_tmp75=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp72)->f3;_tmp76=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp72)->f4;_tmp77=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp72)->f5;{void*t1=_tmp73;void*t2=_tmp74;void*t3=_tmp75;void*t4=_tmp76;void*t5=_tmp77;
# 429
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(1,t4);goto _LL0;}case 12: _tmp73=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp72)->f1;_tmp74=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp72)->f2;_tmp75=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp72)->f3;{void*t1=_tmp73;void*t2=_tmp74;void*t3=_tmp75;
# 431
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(0,t2);goto _LL0;}default: _tmp73=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp72)->f1;_tmp74=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp72)->f2;{void*t1=_tmp73;struct Cyc_List_List*ts=_tmp74;
# 433
Cyc_Port_generalize(0,t1);({(void(*)(void(*)(int,void*),int,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Port_generalize,0,ts);goto _LL0;}}_LL0:;}}
# 439
static int Cyc_Port_occurs(void*v,void*t){
t=Cyc_Port_compress_ct(t);
if(t == v)return 1;{
void*_tmp78=t;void*_tmp7D;void*_tmp7C;void*_tmp7B;void*_tmp7A;void*_tmp79;switch(*((int*)_tmp78)){case 11: _tmp79=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp78)->f1;_tmp7A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp78)->f2;_tmp7B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp78)->f3;_tmp7C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp78)->f4;_tmp7D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp78)->f5;{void*t1=_tmp79;void*t2=_tmp7A;void*t3=_tmp7B;void*t4=_tmp7C;void*t5=_tmp7D;
# 444
return(((Cyc_Port_occurs(v,t1)|| Cyc_Port_occurs(v,t2))|| Cyc_Port_occurs(v,t3))|| Cyc_Port_occurs(v,t4))||
 Cyc_Port_occurs(v,t5);}case 12: _tmp79=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp78)->f1;_tmp7A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp78)->f2;_tmp7B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp78)->f3;{void*t1=_tmp79;void*t2=_tmp7A;void*t3=_tmp7B;
# 447
return(Cyc_Port_occurs(v,t1)|| Cyc_Port_occurs(v,t2))|| Cyc_Port_occurs(v,t3);}case 15: _tmp79=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp78)->f1;_tmp7A=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp78)->f2;{void*t=_tmp79;struct Cyc_List_List*ts=_tmp7A;
# 449
if(Cyc_Port_occurs(v,t))return 1;
for(1;(unsigned)ts;ts=ts->tl){
if(Cyc_Port_occurs(v,(void*)ts->hd))return 1;}
return 0;}case 13: _tmp79=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp78)->f1)->f2;{struct Cyc_List_List*fs=_tmp79;
return 0;}case 14: _tmp79=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp78)->f1;{struct Cyc_List_List*fs=_tmp79;
# 455
for(1;(unsigned)fs;fs=fs->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)fs->hd)->qual)|| Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)fs->hd)->type))return 1;}
return 0;}default:
 return 0;};}}char Cyc_Port_Unify_ct[9U]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 467
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 469
static int Cyc_Port_leq(void*,void*);
static void Cyc_Port_unify_cts(struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*,struct Cyc_List_List*,int);
# 473
static void Cyc_Port_unify_ct(void*t1,void*t2){
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);
if(t1 == t2)return;{
struct _tuple12 _tmp7E=({struct _tuple12 _tmp300;_tmp300.f1=t1,_tmp300.f2=t2;_tmp300;});struct _tuple12 _stmttmp4=_tmp7E;struct _tuple12 _tmp7F=_stmttmp4;struct _fat_ptr _tmp87;struct _fat_ptr _tmp86;void*_tmp8B;void*_tmp8A;void*_tmp89;void*_tmp88;void*_tmp85;void*_tmp84;void*_tmp83;void*_tmp82;void*_tmp81;void*_tmp80;if(*((int*)_tmp7F.f1)== 16){_tmp80=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;{struct Cyc_Port_Cvar*v1=_tmp80;
# 479
if(!Cyc_Port_occurs(t1,t2)){
# 482
{struct Cyc_List_List*_tmp8C=Cyc_Port_filter_self(t1,v1->uppers);struct Cyc_List_List*us=_tmp8C;for(0;(unsigned)us;us=us->tl){
if(!Cyc_Port_leq(t2,(void*)us->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmp8D=Cyc_Port_filter_self(t1,v1->lowers);struct Cyc_List_List*ls=_tmp8D;for(0;(unsigned)ls;ls=ls->tl){
if(!Cyc_Port_leq((void*)ls->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
({void**_tmp349=({void**_tmp8E=_cycalloc(sizeof(*_tmp8E));*_tmp8E=t2;_tmp8E;});v1->eq=_tmp349;});
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}else{if(*((int*)_tmp7F.f2)== 16){_tmp80=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;{struct Cyc_Port_Cvar*v1=_tmp80;
Cyc_Port_unify_ct(t2,t1);return;}}else{switch(*((int*)_tmp7F.f1)){case 11: if(*((int*)_tmp7F.f2)== 11){_tmp80=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;_tmp81=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f1)->f2;_tmp82=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f1)->f3;_tmp83=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f1)->f4;_tmp84=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f1)->f5;_tmp85=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;_tmp88=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f2)->f2;_tmp89=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f2)->f3;_tmp8A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f2)->f4;_tmp8B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp7F.f2)->f5;{void*e1=_tmp80;void*q1=_tmp81;void*k1=_tmp82;void*r1=_tmp83;void*zt1=_tmp84;void*e2=_tmp85;void*q2=_tmp88;void*k2=_tmp89;void*r2=_tmp8A;void*zt2=_tmp8B;
# 491
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(k1,k2);Cyc_Port_unify_ct(r1,r2);
Cyc_Port_unify_ct(zt1,zt2);
return;}}else{goto _LL15;}case 10: if(*((int*)_tmp7F.f2)== 10){_tmp86=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;_tmp87=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;{struct _fat_ptr n1=_tmp86;struct _fat_ptr n2=_tmp87;
# 495
if(Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2)!= 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;}}else{goto _LL15;}case 12: if(*((int*)_tmp7F.f2)== 12){_tmp80=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;_tmp81=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7F.f1)->f2;_tmp82=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7F.f1)->f3;_tmp83=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;_tmp84=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7F.f2)->f2;_tmp85=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp7F.f2)->f3;{void*e1=_tmp80;void*q1=_tmp81;void*zt1=_tmp82;void*e2=_tmp83;void*q2=_tmp84;void*zt2=_tmp85;
# 498
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(zt1,zt2);return;}}else{goto _LL15;}case 15: if(*((int*)_tmp7F.f2)== 15){_tmp80=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;_tmp81=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7F.f1)->f2;_tmp82=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;_tmp83=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp7F.f2)->f2;{void*t1=_tmp80;struct Cyc_List_List*ts1=_tmp81;void*t2=_tmp82;struct Cyc_List_List*ts2=_tmp83;
# 500
Cyc_Port_unify_ct(t1,t2);Cyc_Port_unify_cts(ts1,ts2);return;}}else{goto _LL15;}case 13: switch(*((int*)_tmp7F.f2)){case 13: _tmp80=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;_tmp81=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;{struct _tuple11*p1=_tmp80;struct _tuple11*p2=_tmp81;
# 502
if(p1 == p2)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}case 14: _tmp80=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp7F.f1)->f1)->f1;_tmp81=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp7F.f1)->f1)->f2;_tmp82=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;_tmp83=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f2)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_tmp80;struct Cyc_List_List*fs2=_tmp81;struct Cyc_List_List*fs1=_tmp82;void***eq=_tmp83;
# 512
Cyc_Port_merge_fields(fs2,fs1,0);
({void**_tmp34A=({void**_tmp92=_cycalloc(sizeof(*_tmp92));*_tmp92=t1;_tmp92;});*eq=_tmp34A;});
return;}default: goto _LL15;}case 14: switch(*((int*)_tmp7F.f2)){case 14: _tmp80=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;_tmp81=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f1)->f2;_tmp82=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f2)->f1;_tmp83=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f2)->f2;{struct Cyc_List_List*fs1=_tmp80;void***eq1=_tmp81;struct Cyc_List_List*fs2=_tmp82;void***eq2=_tmp83;
# 504
void*_tmp8F=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(fs1,fs2,1));void*t=_tmp8F;
({void**_tmp34C=({void**_tmp34B=({void**_tmp90=_cycalloc(sizeof(*_tmp90));*_tmp90=t;_tmp90;});*eq2=_tmp34B;});*eq1=_tmp34C;});
return;}case 13: _tmp80=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f1)->f1;_tmp81=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp7F.f1)->f2;_tmp82=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp7F.f2)->f1)->f1;_tmp83=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp7F.f2)->f1)->f2;{struct Cyc_List_List*fs1=_tmp80;void***eq=(void***)_tmp81;struct Cyc_Absyn_Aggrdecl*ad=_tmp82;struct Cyc_List_List*fs2=_tmp83;
# 508
Cyc_Port_merge_fields(fs2,fs1,0);
({void**_tmp34D=({void**_tmp91=_cycalloc(sizeof(*_tmp91));*_tmp91=t2;_tmp91;});*eq=_tmp34D;});
return;}default: goto _LL15;}default: _LL15:
# 515
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}};}}
# 519
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(1;t1 != 0 && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 523
if(t1 != 0 || t2 != 0)
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}
# 529
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){
# 531
struct Cyc_List_List*common=0;
{struct Cyc_List_List*_tmp93=fs2;struct Cyc_List_List*xs2=_tmp93;for(0;(unsigned)xs2;xs2=xs2->tl){
struct Cyc_Port_Cfield*_tmp94=(struct Cyc_Port_Cfield*)xs2->hd;struct Cyc_Port_Cfield*f2=_tmp94;
int found=0;
{struct Cyc_List_List*_tmp95=fs1;struct Cyc_List_List*xs1=_tmp95;for(0;(unsigned)xs1;xs1=xs1->tl){
struct Cyc_Port_Cfield*_tmp96=(struct Cyc_Port_Cfield*)xs1->hd;struct Cyc_Port_Cfield*f1=_tmp96;
if(Cyc_strptrcmp(f1->f,f2->f)== 0){
common=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=f1,_tmp97->tl=common;_tmp97;});
Cyc_Port_unify_ct(f1->qual,f2->qual);
Cyc_Port_unify_ct(f1->type,f2->type);
found=1;
break;}}}
# 545
if(!found){
if(allow_f1_subset_f2)
common=({struct Cyc_List_List*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->hd=f2,_tmp98->tl=common;_tmp98;});else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 551
{struct Cyc_List_List*_tmp99=fs1;struct Cyc_List_List*xs1=_tmp99;for(0;(unsigned)xs1;xs1=xs1->tl){
struct Cyc_Port_Cfield*_tmp9A=(struct Cyc_Port_Cfield*)xs1->hd;struct Cyc_Port_Cfield*f1=_tmp9A;
int found=0;
{struct Cyc_List_List*_tmp9B=fs2;struct Cyc_List_List*xs2=_tmp9B;for(0;(unsigned)xs2;xs2=xs2->tl){
struct Cyc_Port_Cfield*_tmp9C=(struct Cyc_Port_Cfield*)xs2->hd;struct Cyc_Port_Cfield*f2=_tmp9C;
if(Cyc_strptrcmp(f1->f,f2->f))found=1;}}
# 558
if(!found)
common=({struct Cyc_List_List*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->hd=f1,_tmp9D->tl=common;_tmp9D;});}}
# 561
return common;}
# 564
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _tmp9E;_push_handler(& _tmp9E);{int _tmpA0=0;if(setjmp(_tmp9E.handler))_tmpA0=1;if(!_tmpA0){
# 571
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmp9F=(void*)Cyc_Core_get_exn_thrown();void*_tmpA1=_tmp9F;void*_tmpA2;if(((struct Cyc_Port_Unify_ct_exn_struct*)_tmpA1)->tag == Cyc_Port_Unify_ct)
# 578
return 0;else{_tmpA2=_tmpA1;{void*exn=_tmpA2;(int)_rethrow(exn);}};}}}
# 580
return 1;}struct _tuple13{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 586
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){
# 588
t=Cyc_Port_compress_ct(t);
{void*_tmpA3=t;switch(*((int*)_tmpA3)){case 1:
# 592
 goto _LL4;case 8: _LL4:
 goto _LL6;case 5: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 3: _LLA:
 goto _LLC;case 12: _LLC:
 goto _LLE;case 13: _LLE:
 goto _LL10;case 15: _LL10:
 goto _LL12;case 7: _LL12:
# 603
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;case 4:
# 608
 goto _LL16;case 0: _LL16:
 goto _LL18;case 9: _LL18:
# 611
 return*uppers;default:
 goto _LL0;}_LL0:;}
# 615
{struct Cyc_List_List*_tmpA4=*uppers;struct Cyc_List_List*us=_tmpA4;for(0;(unsigned)us;us=us->tl){
void*_tmpA5=Cyc_Port_compress_ct((void*)us->hd);void*t2=_tmpA5;
# 618
if(t == t2)return*uppers;{
struct _tuple12 _tmpA6=({struct _tuple12 _tmp302;_tmp302.f1=t,_tmp302.f2=t2;_tmp302;});struct _tuple12 _stmttmp5=_tmpA6;struct _tuple12 _tmpA7=_stmttmp5;void*_tmpB1;void*_tmpB0;void*_tmpAF;void*_tmpAE;void*_tmpAD;void*_tmpAC;void*_tmpAB;void*_tmpAA;void*_tmpA9;void*_tmpA8;switch(*((int*)_tmpA7.f1)){case 6: switch(*((int*)_tmpA7.f2)){case 11:
# 623
 goto _LL1F;case 5: _LL1F:
 goto _LL21;case 12: _LL21:
# 626
 return*uppers;default: goto _LL24;}case 11: if(*((int*)_tmpA7.f2)== 11){_tmpA8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f1)->f1;_tmpA9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f1)->f2;_tmpAA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f1)->f3;_tmpAB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f1)->f4;_tmpAC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f1)->f5;_tmpAD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f2)->f1;_tmpAE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f2)->f2;_tmpAF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f2)->f3;_tmpB0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f2)->f4;_tmpB1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpA7.f2)->f5;{void*ta=_tmpA8;void*qa=_tmpA9;void*ka=_tmpAA;void*ra=_tmpAB;void*za=_tmpAC;void*tb=_tmpAD;void*qb=_tmpAE;void*kb=_tmpAF;void*rb=_tmpB0;void*zb=_tmpB1;
# 631
struct _tuple13 _tmpB2=({struct _tuple13 _tmp301;({void*_tmp352=Cyc_Port_var();_tmp301.f1=_tmp352;}),({void*_tmp351=Cyc_Port_var();_tmp301.f2=_tmp351;}),({void*_tmp350=Cyc_Port_var();_tmp301.f3=_tmp350;}),({void*_tmp34F=Cyc_Port_var();_tmp301.f4=_tmp34F;}),({void*_tmp34E=Cyc_Port_var();_tmp301.f5=_tmp34E;});_tmp301;});struct _tuple13 _stmttmp6=_tmpB2;struct _tuple13 _tmpB3=_stmttmp6;void*_tmpB8;void*_tmpB7;void*_tmpB6;void*_tmpB5;void*_tmpB4;_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;_tmpB6=_tmpB3.f3;_tmpB7=_tmpB3.f4;_tmpB8=_tmpB3.f5;{void*tc=_tmpB4;void*qc=_tmpB5;void*kc=_tmpB6;void*rc=_tmpB7;void*zc=_tmpB8;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpB9=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->tag=11,_tmpBA->f1=tc,_tmpBA->f2=qc,_tmpBA->f3=kc,_tmpBA->f4=rc,_tmpBA->f5=zc;_tmpBA;});struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p=_tmpB9;
Cyc_Port_leq(tc,ta);Cyc_Port_leq(tc,tb);
Cyc_Port_leq(qc,qa);Cyc_Port_leq(qc,qb);
Cyc_Port_leq(kc,ka);Cyc_Port_leq(kc,qb);
Cyc_Port_leq(rc,ra);Cyc_Port_leq(rc,rb);
Cyc_Port_leq(zc,za);Cyc_Port_leq(zc,zb);
us->hd=(void*)((void*)p);
return*uppers;}}}else{goto _LL24;}default: _LL24:
 goto _LL1B;}_LL1B:;}}}
# 643
return({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->hd=t,_tmpBB->tl=*uppers;_tmpBB;});}
# 648
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){
# 650
t=Cyc_Port_compress_ct(t);
{void*_tmpBC=t;switch(*((int*)_tmpBC)){case 0:
 goto _LL4;case 8: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 13: _LLC:
 goto _LLE;case 15: _LLE:
 goto _LL10;case 10: _LL10:
# 660
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;case 7:
 goto _LL14;case 1: _LL14:
 goto _LL16;case 9: _LL16:
# 666
 return*lowers;default:
# 668
 goto _LL0;}_LL0:;}
# 670
{struct Cyc_List_List*_tmpBD=*lowers;struct Cyc_List_List*ls=_tmpBD;for(0;(unsigned)ls;ls=ls->tl){
void*_tmpBE=Cyc_Port_compress_ct((void*)ls->hd);void*t2=_tmpBE;
if(t == t2)return*lowers;{
struct _tuple12 _tmpBF=({struct _tuple12 _tmp304;_tmp304.f1=t,_tmp304.f2=t2;_tmp304;});struct _tuple12 _stmttmp7=_tmpBF;struct _tuple12 _tmpC0=_stmttmp7;void*_tmpCA;void*_tmpC9;void*_tmpC8;void*_tmpC7;void*_tmpC6;void*_tmpC5;void*_tmpC4;void*_tmpC3;void*_tmpC2;void*_tmpC1;if(*((int*)_tmpC0.f2)== 4)
goto _LL1D;else{switch(*((int*)_tmpC0.f1)){case 5: switch(*((int*)_tmpC0.f2)){case 6: _LL1D:
 goto _LL1F;case 11: _LL1F:
 goto _LL21;default: goto _LL26;}case 11: switch(*((int*)_tmpC0.f2)){case 6: _LL21:
 goto _LL23;case 11: _tmpC1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f1)->f1;_tmpC2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f1)->f2;_tmpC3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f1)->f3;_tmpC4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f1)->f4;_tmpC5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f1)->f5;_tmpC6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f2)->f1;_tmpC7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f2)->f2;_tmpC8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f2)->f3;_tmpC9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f2)->f4;_tmpCA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpC0.f2)->f5;{void*ta=_tmpC1;void*qa=_tmpC2;void*ka=_tmpC3;void*ra=_tmpC4;void*za=_tmpC5;void*tb=_tmpC6;void*qb=_tmpC7;void*kb=_tmpC8;void*rb=_tmpC9;void*zb=_tmpCA;
# 684
struct _tuple13 _tmpCB=({struct _tuple13 _tmp303;({void*_tmp357=Cyc_Port_var();_tmp303.f1=_tmp357;}),({void*_tmp356=Cyc_Port_var();_tmp303.f2=_tmp356;}),({void*_tmp355=Cyc_Port_var();_tmp303.f3=_tmp355;}),({void*_tmp354=Cyc_Port_var();_tmp303.f4=_tmp354;}),({void*_tmp353=Cyc_Port_var();_tmp303.f5=_tmp353;});_tmp303;});struct _tuple13 _stmttmp8=_tmpCB;struct _tuple13 _tmpCC=_stmttmp8;void*_tmpD1;void*_tmpD0;void*_tmpCF;void*_tmpCE;void*_tmpCD;_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;_tmpCF=_tmpCC.f3;_tmpD0=_tmpCC.f4;_tmpD1=_tmpCC.f5;{void*tc=_tmpCD;void*qc=_tmpCE;void*kc=_tmpCF;void*rc=_tmpD0;void*zc=_tmpD1;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpD2=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->tag=11,_tmpD3->f1=tc,_tmpD3->f2=qc,_tmpD3->f3=kc,_tmpD3->f4=rc,_tmpD3->f5=zc;_tmpD3;});struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p=_tmpD2;
Cyc_Port_leq(ta,tc);Cyc_Port_leq(tb,tc);
Cyc_Port_leq(qa,qc);Cyc_Port_leq(qb,qc);
Cyc_Port_leq(ka,kc);Cyc_Port_leq(qb,kc);
Cyc_Port_leq(ra,rc);Cyc_Port_leq(rb,rc);
Cyc_Port_leq(za,zc);Cyc_Port_leq(zb,zc);
ls->hd=(void*)((void*)p);
return*lowers;}}default: goto _LL26;}case 12: if(*((int*)_tmpC0.f2)== 6){_LL23:
# 679
 return*lowers;}else{goto _LL26;}default: _LL26:
# 693
 goto _LL19;}}_LL19:;}}}
# 696
return({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=t,_tmpD4->tl=*lowers;_tmpD4;});}
# 703
static int Cyc_Port_leq(void*t1,void*t2){
# 709
if(t1 == t2)return 1;
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{
struct _tuple12 _tmpD5=({struct _tuple12 _tmp305;_tmp305.f1=t1,_tmp305.f2=t2;_tmp305;});struct _tuple12 _stmttmp9=_tmpD5;struct _tuple12 _tmpD6=_stmttmp9;void*_tmpE0;void*_tmpDF;void*_tmpDE;void*_tmpDD;void*_tmpDC;void*_tmpDB;void*_tmpDA;void*_tmpD9;void*_tmpD8;void*_tmpD7;struct _fat_ptr _tmpE2;struct _fat_ptr _tmpE1;switch(*((int*)_tmpD6.f1)){case 7:
 return 1;case 10: switch(*((int*)_tmpD6.f2)){case 10: _tmpE1=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpD6.f1)->f1;_tmpE2=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpD6.f2)->f1;{struct _fat_ptr n1=_tmpE1;struct _fat_ptr n2=_tmpE2;
return Cyc_strcmp((struct _fat_ptr)n1,(struct _fat_ptr)n2)== 0;}case 7: _tmpE1=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmpD6.f1)->f1;{struct _fat_ptr n1=_tmpE1;
return 0;}case 16: goto _LL2D;default: goto _LL2F;}case 1: switch(*((int*)_tmpD6.f2)){case 0:
 return 1;case 16: goto _LL2D;default: goto _LL2F;}case 0: switch(*((int*)_tmpD6.f2)){case 1:
 return 0;case 16: goto _LL2D;default: goto _LL2F;}case 9: switch(*((int*)_tmpD6.f2)){case 8:
 return 0;case 16: goto _LL2D;default: goto _LL2F;}case 8: switch(*((int*)_tmpD6.f2)){case 9:
 return 1;case 16: goto _LL2D;default: goto _LL2F;}case 16: switch(*((int*)_tmpD6.f2)){case 0:
 return 1;case 4:
 return 1;case 16: _tmpD7=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpD6.f1)->f1;_tmpD8=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpD6.f2)->f1;{struct Cyc_Port_Cvar*v1=_tmpD7;struct Cyc_Port_Cvar*v2=_tmpD8;
# 739
({struct Cyc_List_List*_tmp358=Cyc_Port_filter_self(t1,v1->uppers);v1->uppers=_tmp358;});
({struct Cyc_List_List*_tmp359=Cyc_Port_filter_self(t2,v2->lowers);v2->lowers=_tmp359;});
({struct Cyc_List_List*_tmp35A=Cyc_Port_insert_upper(t1,t2,& v1->uppers);v1->uppers=_tmp35A;});
({struct Cyc_List_List*_tmp35B=Cyc_Port_insert_lower(t2,t1,& v2->lowers);v2->lowers=_tmp35B;});
return 1;}default: _tmpD7=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpD6.f1)->f1;{struct Cyc_Port_Cvar*v1=_tmpD7;
# 745
({struct Cyc_List_List*_tmp35C=Cyc_Port_filter_self(t1,v1->uppers);v1->uppers=_tmp35C;});
({struct Cyc_List_List*_tmp35D=Cyc_Port_insert_upper(t1,t2,& v1->uppers);v1->uppers=_tmp35D;});
return 1;}}case 4:
# 722
 return 0;case 5: switch(*((int*)_tmpD6.f2)){case 6:
 return 1;case 11:
 return 1;case 4:
 return 1;case 16: goto _LL2D;default: goto _LL2F;}case 11: switch(*((int*)_tmpD6.f2)){case 6:
 return 1;case 4:
 return 1;case 11: _tmpD7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f1)->f1;_tmpD8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f1)->f2;_tmpD9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f1)->f3;_tmpDA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f1)->f4;_tmpDB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f1)->f5;_tmpDC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f1;_tmpDD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f2;_tmpDE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f3;_tmpDF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f4;_tmpE0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f5;{void*t1=_tmpD7;void*q1=_tmpD8;void*k1=_tmpD9;void*r1=_tmpDA;void*z1=_tmpDB;void*t2=_tmpDC;void*q2=_tmpDD;void*k2=_tmpDE;void*r2=_tmpDF;void*z2=_tmpE0;
# 731
return(((Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_unifies(k1,k2))&& Cyc_Port_leq(r1,r2))&&
 Cyc_Port_leq(z1,z2);}case 16: goto _LL2D;default: goto _LL2F;}case 12: switch(*((int*)_tmpD6.f2)){case 6:
# 728
 return 1;case 4:
 return 1;case 12: _tmpD7=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f1)->f1;_tmpD8=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f1)->f2;_tmpD9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f1)->f3;_tmpDA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f2)->f1;_tmpDB=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f2)->f2;_tmpDC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f2)->f3;{void*t1=_tmpD7;void*q1=_tmpD8;void*z1=_tmpD9;void*t2=_tmpDA;void*q2=_tmpDB;void*z2=_tmpDC;
# 734
return(Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_leq(z1,z2);}case 11: _tmpD7=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f1)->f1;_tmpD8=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f1)->f2;_tmpD9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmpD6.f1)->f3;_tmpDA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f1;_tmpDB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f2;_tmpDC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f3;_tmpDD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpD6.f2)->f5;{void*t1=_tmpD7;void*q1=_tmpD8;void*z1=_tmpD9;void*t2=_tmpDA;void*q2=_tmpDB;void*k2=_tmpDC;void*z2=_tmpDD;
# 736
return((Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,k2))&&
 Cyc_Port_leq(z1,z2);}case 16: goto _LL2D;default: goto _LL2F;}default: if(*((int*)_tmpD6.f2)== 16){_LL2D: _tmpD7=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmpD6.f2)->f1;{struct Cyc_Port_Cvar*v2=_tmpD7;
# 749
({struct Cyc_List_List*_tmp35E=Cyc_Port_filter_self(t2,v2->lowers);v2->lowers=_tmp35E;});
({struct Cyc_List_List*_tmp35F=Cyc_Port_insert_lower(t2,t1,& v2->lowers);v2->lowers=_tmp35F;});
return 1;}}else{_LL2F:
 return Cyc_Port_unifies(t1,t2);}};}}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 783
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos =0U,Cyc_Port_FnArg_pos =1U,Cyc_Port_FnBody_pos =2U,Cyc_Port_Toplevel_pos =3U};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 796
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv (void){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmpE4=_cycalloc(sizeof(*_tmpE4));({struct Cyc_Dict_Dict _tmp364=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmpE4->typedef_dict=_tmp364;}),({
struct Cyc_Dict_Dict _tmp363=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmpE4->struct_dict=_tmp363;}),({
struct Cyc_Dict_Dict _tmp362=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmpE4->union_dict=_tmp362;}),_tmpE4->qualifier_edits=0,_tmpE4->pointer_edits=0,_tmpE4->zeroterm_edits=0,_tmpE4->vardecl_locs=0,({
# 804
struct Cyc_Hashtable_Table*_tmp361=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(unsigned,unsigned),int(*)(unsigned)))Cyc_Hashtable_create;})(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);_tmpE4->varusage_tab=_tmp361;}),_tmpE4->edits=0,_tmpE4->porting=0,({
# 807
void*_tmp360=Cyc_Port_void_ct();_tmpE4->return_type=_tmp360;});_tmpE4;});
return({struct Cyc_Port_Cenv*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->gcenv=g,_tmpE3->cpos=3U,({
# 810
struct Cyc_Dict_Dict _tmp365=({(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;})(Cyc_Absyn_qvar_cmp);_tmpE3->var_dict=_tmp365;});_tmpE3;});}
# 816
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
return(int)env->cpos == 1;}
# 819
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return(int)env->cpos == 0;}
# 822
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
return(int)env->cpos == 3;}
# 825
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
return(env->gcenv)->return_type;}
# 828
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmpE5;_push_handler(& _tmpE5);{int _tmpE7=0;if(setjmp(_tmpE5.handler))_tmpE7=1;if(!_tmpE7){
{struct _tuple12 _tmpE8=*({(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((env->gcenv)->typedef_dict,n);struct _tuple12 _stmttmpA=_tmpE8;struct _tuple12 _tmpE9=_stmttmpA;void*_tmpEA;_tmpEA=_tmpE9.f1;{void*t=_tmpEA;
void*_tmpEB=t;_npop_handler(0);return _tmpEB;}}
# 830
;_pop_handler();}else{void*_tmpE6=(void*)Cyc_Core_get_exn_thrown();void*_tmpEC=_tmpE6;void*_tmpED;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpEC)->tag == Cyc_Dict_Absent)
# 837
return Cyc_Absyn_sint_type;else{_tmpED=_tmpEC;{void*exn=_tmpED;(int)_rethrow(exn);}};}}}
# 842
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmpEE;_push_handler(& _tmpEE);{int _tmpF0=0;if(setjmp(_tmpEE.handler))_tmpF0=1;if(!_tmpF0){
{struct _tuple12 _tmpF1=*({(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})((env->gcenv)->typedef_dict,n);struct _tuple12 _stmttmpB=_tmpF1;struct _tuple12 _tmpF2=_stmttmpB;void*_tmpF3;_tmpF3=_tmpF2.f2;{void*ct=_tmpF3;
void*_tmpF4=ct;_npop_handler(0);return _tmpF4;}}
# 844
;_pop_handler();}else{void*_tmpEF=(void*)Cyc_Core_get_exn_thrown();void*_tmpF5=_tmpEF;void*_tmpF6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpF5)->tag == Cyc_Dict_Absent)
# 851
return Cyc_Port_var();else{_tmpF6=_tmpF5;{void*exn=_tmpF6;(int)_rethrow(exn);}};}}}
# 857
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 859
struct _tuple11**_tmpF7=({(struct _tuple11**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt;})((env->gcenv)->struct_dict,n);struct _tuple11**popt=_tmpF7;
if((unsigned)popt)
return*popt;else{
# 863
struct Cyc_Absyn_Aggrdecl*_tmpF8=({struct Cyc_Absyn_Aggrdecl*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->kind=0U,_tmpFA->sc=2U,_tmpFA->name=n,_tmpFA->tvs=0,_tmpFA->impl=0,_tmpFA->attributes=0,_tmpFA->expected_mem_kind=0;_tmpFA;});struct Cyc_Absyn_Aggrdecl*ad=_tmpF8;
# 866
struct _tuple11*p=({struct _tuple11*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->f1=ad,_tmpF9->f2=0;_tmpF9;});
({struct Cyc_Dict_Dict _tmp366=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*))Cyc_Dict_insert;})((env->gcenv)->struct_dict,n,p);(env->gcenv)->struct_dict=_tmp366;});
return p;}}
# 874
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 876
struct _tuple11**_tmpFB=({(struct _tuple11**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt;})((env->gcenv)->union_dict,n);struct _tuple11**popt=_tmpFB;
if((unsigned)popt)
return*popt;else{
# 880
struct Cyc_Absyn_Aggrdecl*_tmpFC=({struct Cyc_Absyn_Aggrdecl*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->kind=1U,_tmpFE->sc=2U,_tmpFE->name=n,_tmpFE->tvs=0,_tmpFE->impl=0,_tmpFE->attributes=0,_tmpFE->expected_mem_kind=0;_tmpFE;});struct Cyc_Absyn_Aggrdecl*ad=_tmpFC;
# 883
struct _tuple11*p=({struct _tuple11*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->f1=ad,_tmpFD->f2=0;_tmpFD;});
({struct Cyc_Dict_Dict _tmp367=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*))Cyc_Dict_insert;})((env->gcenv)->union_dict,n,p);(env->gcenv)->union_dict=_tmp367;});
return p;}}struct _tuple14{struct _tuple12 f1;unsigned f2;};struct _tuple15{void*f1;struct _tuple12*f2;unsigned f3;};
# 890
static struct _tuple14 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmpFF;_push_handler(& _tmpFF);{int _tmp101=0;if(setjmp(_tmpFF.handler))_tmp101=1;if(!_tmp101){
{struct _tuple15 _tmp102=*({(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(env->var_dict,x);struct _tuple15 _stmttmpC=_tmp102;struct _tuple15 _tmp103=_stmttmpC;unsigned _tmp105;void*_tmp104;_tmp104=_tmp103.f2;_tmp105=_tmp103.f3;{struct _tuple12*p=_tmp104;unsigned loc=_tmp105;
struct _tuple14 _tmp106=({struct _tuple14 _tmp306;_tmp306.f1=*p,_tmp306.f2=loc;_tmp306;});_npop_handler(0);return _tmp106;}}
# 892
;_pop_handler();}else{void*_tmp100=(void*)Cyc_Core_get_exn_thrown();void*_tmp107=_tmp100;void*_tmp108;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp107)->tag == Cyc_Dict_Absent)
# 899
return({struct _tuple14 _tmp307;({void*_tmp369=Cyc_Port_var();(_tmp307.f1).f1=_tmp369;}),({void*_tmp368=Cyc_Port_var();(_tmp307.f1).f2=_tmp368;}),_tmp307.f2=0U;_tmp307;});else{_tmp108=_tmp107;{void*exn=_tmp108;(int)_rethrow(exn);}};}}}
# 902
static struct _tuple15*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return({(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(env->var_dict,x);}
# 906
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return({(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_member;})(env->var_dict,x);}
# 910
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp109;_push_handler(& _tmp109);{int _tmp10B=0;if(setjmp(_tmp109.handler))_tmp10B=1;if(!_tmp10B){
{struct _tuple15 _tmp10C=*({(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(env->var_dict,x);struct _tuple15 _stmttmpD=_tmp10C;struct _tuple15 _tmp10D=_stmttmpD;void*_tmp10E;_tmp10E=_tmp10D.f1;{void*t=_tmp10E;
LOOP: {void*_tmp10F=t;void*_tmp110;switch(*((int*)_tmp10F)){case 8: _tmp110=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp10F)->f1;{struct _tuple0*n=_tmp110;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 5:  {
int _tmp111=1;_npop_handler(0);return _tmp111;}default:  {
int _tmp112=0;_npop_handler(0);return _tmp112;}};}}}
# 912
;_pop_handler();}else{void*_tmp10A=(void*)Cyc_Core_get_exn_thrown();void*_tmp113=_tmp10A;void*_tmp114;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp113)->tag == Cyc_Dict_Absent)
# 923
return 0;else{_tmp114=_tmp113;{void*exn=_tmp114;(int)_rethrow(exn);}};}}}
# 927
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp115;_push_handler(& _tmp115);{int _tmp117=0;if(setjmp(_tmp115.handler))_tmp117=1;if(!_tmp117){
{struct _tuple15 _tmp118=*({(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;})(env->var_dict,x);struct _tuple15 _stmttmpE=_tmp118;struct _tuple15 _tmp119=_stmttmpE;void*_tmp11A;_tmp11A=_tmp119.f1;{void*t=_tmp11A;
LOOP: {void*_tmp11B=t;void*_tmp11C;switch(*((int*)_tmp11B)){case 8: _tmp11C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11B)->f1;{struct _tuple0*n=_tmp11C;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 4:  {
int _tmp11D=1;_npop_handler(0);return _tmp11D;}default:  {
int _tmp11E=0;_npop_handler(0);return _tmp11E;}};}}}
# 929
;_pop_handler();}else{void*_tmp116=(void*)Cyc_Core_get_exn_thrown();void*_tmp11F=_tmp116;void*_tmp120;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp11F)->tag == Cyc_Dict_Absent)
# 940
return 0;else{_tmp120=_tmp11F;{void*exn=_tmp120;(int)_rethrow(exn);}};}}}
# 946
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){
return({struct Cyc_Port_Cenv*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->gcenv=env->gcenv,_tmp121->var_dict=env->var_dict,_tmp121->cpos=cpos;_tmp121;});}
# 951
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
(env->gcenv)->return_type=t;}
# 956
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype,unsigned loc){
# 958
return({struct Cyc_Port_Cenv*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->gcenv=env->gcenv,({
struct Cyc_Dict_Dict _tmp36E=({struct Cyc_Dict_Dict(*_tmp36D)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmp36C=env->var_dict;struct _tuple0*_tmp36B=x;_tmp36D(_tmp36C,_tmp36B,({struct _tuple15*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->f1=t,({struct _tuple12*_tmp36A=({struct _tuple12*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=qual,_tmp122->f2=ctype;_tmp122;});_tmp123->f2=_tmp36A;}),_tmp123->f3=loc;_tmp123;}));});_tmp124->var_dict=_tmp36E;}),_tmp124->cpos=env->cpos;_tmp124;});}
# 964
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
({struct Cyc_Dict_Dict _tmp372=({struct Cyc_Dict_Dict(*_tmp371)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmp370=(env->gcenv)->typedef_dict;struct _tuple0*_tmp36F=n;_tmp371(_tmp370,_tmp36F,({struct _tuple12*_tmp125=_cycalloc(sizeof(*_tmp125));
_tmp125->f1=t,_tmp125->f2=ct;_tmp125;}));});
# 965
(env->gcenv)->typedef_dict=_tmp372;});}struct _tuple16{struct _tuple0*f1;unsigned f2;void*f3;};
# 968
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned loc,void*type,struct Cyc_Absyn_Exp*init){
({struct Cyc_List_List*_tmp374=({struct Cyc_List_List*_tmp127=_cycalloc(sizeof(*_tmp127));({struct _tuple16*_tmp373=({struct _tuple16*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->f1=x,_tmp126->f2=loc,_tmp126->f3=type;_tmp126;});_tmp127->hd=_tmp373;}),_tmp127->tl=(env->gcenv)->vardecl_locs;_tmp127;});(env->gcenv)->vardecl_locs=_tmp374;});
({void(*_tmp377)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*)=({(void(*)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp376=(env->gcenv)->varusage_tab;unsigned _tmp375=loc;_tmp377(_tmp376,_tmp375,({struct Cyc_Port_VarUsage*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->address_taken=0,_tmp128->init=init,_tmp128->usage_locs=0;_tmp128;}));});}
# 972
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned declloc,unsigned useloc,int takeaddress){
struct Cyc_Port_VarUsage*varusage=0;
if(({(int(*)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage**))Cyc_Hashtable_try_lookup;})((env->gcenv)->varusage_tab,declloc,& varusage)){
if(takeaddress)((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken=1;{
struct Cyc_List_List*_tmp129=((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs;struct Cyc_List_List*l=_tmp129;
({struct Cyc_List_List*_tmp378=({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->hd=(void*)useloc,_tmp12A->tl=l;_tmp12A;});((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs=_tmp378;});}}}struct _tuple17{void*f1;void*f2;unsigned f3;};
# 983
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned loc){
# 985
({struct Cyc_List_List*_tmp37A=({struct Cyc_List_List*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct _tuple17*_tmp379=({struct _tuple17*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->f1=new_qual,_tmp12B->f2=orig_qual,_tmp12B->f3=loc;_tmp12B;});_tmp12C->hd=_tmp379;}),_tmp12C->tl=(env->gcenv)->qualifier_edits;_tmp12C;});(env->gcenv)->qualifier_edits=_tmp37A;});}
# 991
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned loc){
# 993
({struct Cyc_List_List*_tmp37C=({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct _tuple17*_tmp37B=({struct _tuple17*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->f1=new_ptr,_tmp12D->f2=orig_ptr,_tmp12D->f3=loc;_tmp12D;});_tmp12E->hd=_tmp37B;}),_tmp12E->tl=(env->gcenv)->pointer_edits;_tmp12E;});(env->gcenv)->pointer_edits=_tmp37C;});}
# 997
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned loc){
# 999
({struct Cyc_List_List*_tmp37E=({struct Cyc_List_List*_tmp130=_cycalloc(sizeof(*_tmp130));({struct _tuple17*_tmp37D=({struct _tuple17*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->f1=new_zt,_tmp12F->f2=orig_zt,_tmp12F->f3=loc;_tmp12F;});_tmp130->hd=_tmp37D;}),_tmp130->tl=(env->gcenv)->zeroterm_edits;_tmp130;});(env->gcenv)->zeroterm_edits=_tmp37E;});}
# 1005
static void*Cyc_Port_main_type (void){
struct _tuple8*arg1=({struct _tuple8*_tmp134=_cycalloc(sizeof(*_tmp134));
_tmp134->f1=0,({struct Cyc_Absyn_Tqual _tmp37F=Cyc_Absyn_empty_tqual(0U);_tmp134->f2=_tmp37F;}),_tmp134->f3=Cyc_Absyn_sint_type;_tmp134;});
struct _tuple8*arg2=({struct _tuple8*_tmp133=_cycalloc(sizeof(*_tmp133));
_tmp133->f1=0,({struct Cyc_Absyn_Tqual _tmp385=Cyc_Absyn_empty_tqual(0U);_tmp133->f2=_tmp385;}),({
void*_tmp384=({void*_tmp383=Cyc_Absyn_string_type(Cyc_Absyn_wildtyp(0));void*_tmp382=Cyc_Absyn_wildtyp(0);struct Cyc_Absyn_Tqual _tmp381=
Cyc_Absyn_empty_tqual(0U);
# 1010
void*_tmp380=
Cyc_Tcutil_any_bool(0);
# 1010
Cyc_Absyn_fatptr_type(_tmp383,_tmp382,_tmp381,_tmp380,Cyc_Absyn_false_type);});_tmp133->f3=_tmp384;});_tmp133;});
# 1012
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=5,(_tmp132->f1).tvars=0,(_tmp132->f1).effect=0,({
struct Cyc_Absyn_Tqual _tmp388=Cyc_Absyn_empty_tqual(0U);(_tmp132->f1).ret_tqual=_tmp388;}),(_tmp132->f1).ret_type=Cyc_Absyn_sint_type,({
# 1015
struct Cyc_List_List*_tmp387=({struct _tuple8*_tmp131[2];_tmp131[0]=arg1,_tmp131[1]=arg2;({struct Cyc_List_List*(*_tmp386)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp386(_tag_fat(_tmp131,sizeof(struct _tuple8*),2));});});(_tmp132->f1).args=_tmp387;}),(_tmp132->f1).c_varargs=0,(_tmp132->f1).cyc_varargs=0,(_tmp132->f1).rgn_po=0,(_tmp132->f1).attributes=0,(_tmp132->f1).requires_clause=0,(_tmp132->f1).requires_relns=0,(_tmp132->f1).ensures_clause=0,(_tmp132->f1).ensures_relns=0,(_tmp132->f1).return_value=0;_tmp132;});}
# 1024
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*,void*);
# 1027
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv (void){
struct Cyc_Port_Cenv*_tmp135=Cyc_Port_empty_cenv();struct Cyc_Port_Cenv*e=_tmp135;
# 1030
e=({struct Cyc_Port_Cenv*_tmp38F=e;struct _tuple0*_tmp38E=({struct _tuple0*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp38A=({struct _fat_ptr*_tmp137=_cycalloc(sizeof(*_tmp137));({struct _fat_ptr _tmp389=({const char*_tmp136="main";_tag_fat(_tmp136,sizeof(char),5U);});*_tmp137=_tmp389;});_tmp137;});_tmp138->f2=_tmp38A;});_tmp138;});void*_tmp38D=Cyc_Port_main_type();void*_tmp38C=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp38F,_tmp38E,_tmp38D,_tmp38C,({
struct Cyc_Port_Cenv*_tmp38B=e;Cyc_Port_type_to_ctype(_tmp38B,Cyc_Port_main_type());}),0U);});
return e;}
# 1038
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp139=Cyc_Port_compress_ct(t);void*_stmttmpF=_tmp139;void*_tmp13A=_stmttmpF;void*_tmp13F;void*_tmp13E;void*_tmp13D;void*_tmp13C;void*_tmp13B;switch(*((int*)_tmp13A)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 5: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 14: _LL10:
 goto _LL12;case 13: _LL12:
 goto _LL14;case 16: _LL14:
 goto _LL16;case 8: _LL16:
 goto _LL18;case 9: _LL18:
 goto _LL1A;case 7: _LL1A:
 return;case 10: _tmp13B=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp13A)->f1;{struct _fat_ptr*n=_tmp13B;
# 1054
if(!({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(*cts,n))
({struct Cyc_Dict_Dict _tmp393=({struct Cyc_Dict_Dict(*_tmp392)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _tmp391=*cts;struct _fat_ptr*_tmp390=n;_tmp392(_tmp391,_tmp390,({int*_tmp140=_cycalloc_atomic(sizeof(*_tmp140));*_tmp140=0;_tmp140;}));});*cts=_tmp393;});{
int*_tmp141=({(int*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(*cts,n);int*p=_tmp141;
*p=*p + 1;
return;}}case 11: _tmp13B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp13A)->f1;_tmp13C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp13A)->f2;_tmp13D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp13A)->f3;_tmp13E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp13A)->f4;_tmp13F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp13A)->f5;{void*t1=_tmp13B;void*t2=_tmp13C;void*t3=_tmp13D;void*t4=_tmp13E;void*zt=_tmp13F;
# 1060
Cyc_Port_region_counts(cts,t1);
Cyc_Port_region_counts(cts,t4);
return;}case 12: _tmp13B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp13A)->f1;_tmp13C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp13A)->f2;_tmp13D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp13A)->f3;{void*t1=_tmp13B;void*t2=_tmp13C;void*zt=_tmp13D;
# 1064
Cyc_Port_region_counts(cts,t1);
return;}default: _tmp13B=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp13A)->f1;_tmp13C=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp13A)->f2;{void*t1=_tmp13B;struct Cyc_List_List*ts=_tmp13C;
# 1067
Cyc_Port_region_counts(cts,t1);
for(1;(unsigned)ts;ts=ts->tl){Cyc_Port_region_counts(cts,(void*)ts->hd);}
return;}};}
# 1078
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1080
c=Cyc_Port_compress_ct(c);{
struct _tuple12 _tmp142=({struct _tuple12 _tmp308;_tmp308.f1=t,_tmp308.f2=c;_tmp308;});struct _tuple12 _stmttmp10=_tmp142;struct _tuple12 _tmp143=_stmttmp10;void*_tmp148;void*_tmp147;void*_tmp146;void*_tmp145;void*_tmp144;switch(*((int*)_tmp143.f1)){case 3: if(*((int*)_tmp143.f2)== 11){_tmp144=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143.f1)->f1).elt_type;_tmp145=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143.f1)->f1).ptr_atts).rgn;_tmp146=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp143.f1)->f1).ptr_atts).ptrloc;_tmp147=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp143.f2)->f1;_tmp148=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp143.f2)->f4;{void*et=_tmp144;void*rt=_tmp145;struct Cyc_Absyn_PtrLoc*ploc=_tmp146;void*ec=_tmp147;void*rc=_tmp148;
# 1083
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);{
unsigned _tmp149=(unsigned)ploc?ploc->rgn_loc: 0U;unsigned loc=_tmp149;
rc=Cyc_Port_compress_ct(rc);
# 1100 "port.cyc"
({struct Cyc_List_List*_tmp397=({struct Cyc_List_List*_tmp14D=_cycalloc(sizeof(*_tmp14D));
({struct Cyc_Port_Edit*_tmp396=({struct Cyc_Port_Edit*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->loc=loc,({struct _fat_ptr _tmp395=({const char*_tmp14A="`H ";_tag_fat(_tmp14A,sizeof(char),4U);});_tmp14C->old_string=_tmp395;}),({struct _fat_ptr _tmp394=({const char*_tmp14B="";_tag_fat(_tmp14B,sizeof(char),1U);});_tmp14C->new_string=_tmp394;});_tmp14C;});_tmp14D->hd=_tmp396;}),_tmp14D->tl=(env->gcenv)->edits;_tmp14D;});
# 1100
(env->gcenv)->edits=_tmp397;});
# 1103
goto _LL0;}}}else{goto _LL7;}case 4: if(*((int*)_tmp143.f2)== 12){_tmp144=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp143.f1)->f1).elt_type;_tmp145=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp143.f2)->f1;{void*et=_tmp144;void*ec=_tmp145;
# 1105
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);
goto _LL0;}}else{goto _LL7;}case 5: if(*((int*)_tmp143.f2)== 15){_tmp144=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143.f1)->f1).ret_type;_tmp145=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp143.f1)->f1).args;_tmp146=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp143.f2)->f1;_tmp147=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp143.f2)->f2;{void*rt=_tmp144;struct Cyc_List_List*argst=_tmp145;void*rc=_tmp146;struct Cyc_List_List*argsc=_tmp147;
# 1108
Cyc_Port_register_rgns(env,counts,1,rt,rc);
for(1;argst != 0 && argsc != 0;(argst=argst->tl,argsc=argsc->tl)){
struct _tuple8 _tmp14E=*((struct _tuple8*)argst->hd);struct _tuple8 _stmttmp11=_tmp14E;struct _tuple8 _tmp14F=_stmttmp11;void*_tmp150;_tmp150=_tmp14F.f3;{void*at=_tmp150;
void*_tmp151=(void*)argsc->hd;void*ac=_tmp151;
Cyc_Port_register_rgns(env,counts,0,at,ac);}}
# 1114
goto _LL0;}}else{goto _LL7;}default: _LL7:
 goto _LL0;}_LL0:;}}
# 1121
static void*Cyc_Port_gen_exp(int,struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*);
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Stmt*);
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Decl*,int);
# 1126
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp152=t;void*_tmp153;switch(*((int*)_tmp152)){case 8: _tmp153=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp152)->f1;{struct _tuple0*n=_tmp153;
# 1129
(*n).f1=Cyc_Absyn_Loc_n;
return({struct Cyc_Port_Cenv*_tmp398=env;Cyc_Port_is_char(_tmp398,Cyc_Port_lookup_typedef(env,n));});}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp152)->f1)== 1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp152)->f1)->f2 == Cyc_Absyn_Char_sz)
return 1;else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 return 0;};}
# 1137
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp154=t;unsigned _tmp158;void*_tmp15C;void*_tmp15B;void*_tmp15A;void*_tmp159;void*_tmp156;struct Cyc_Absyn_Tqual _tmp157;union Cyc_Absyn_AggrInfo _tmp15D;void*_tmp155;switch(*((int*)_tmp154)){case 8: _tmp155=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp154)->f1;{struct _tuple0*n=_tmp155;
# 1140
(*n).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,n);}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp154)->f1)){case 0:
 return Cyc_Port_void_ct();case 1:
# 1219
 goto _LLA;case 2: _LLA:
 return Cyc_Port_arith_ct();case 20: _tmp15D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp154)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_tmp15D;
# 1262
union Cyc_Absyn_AggrInfo _tmp17A=ai;void*_tmp17B;if((_tmp17A.UnknownAggr).tag == 1){if(((_tmp17A.UnknownAggr).val).f1 == Cyc_Absyn_StructA){_tmp17B=((_tmp17A.UnknownAggr).val).f2;{struct _tuple0*n=_tmp17B;
# 1264
(*n).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp17C=Cyc_Port_lookup_struct_decl(env,n);struct _tuple11*p=_tmp17C;
return Cyc_Port_known_aggr_ct(p);}}}else{_tmp17B=((_tmp17A.UnknownAggr).val).f2;{struct _tuple0*n=_tmp17B;
# 1268
(*n).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*_tmp17D=Cyc_Port_lookup_union_decl(env,n);struct _tuple11*p=_tmp17D;
return Cyc_Port_known_aggr_ct(p);}}}}else{_tmp17B=(_tmp17A.KnownAggr).val;{struct Cyc_Absyn_Aggrdecl**adp=_tmp17B;
# 1272
struct Cyc_Absyn_Aggrdecl*_tmp17E=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp17E;
enum Cyc_Absyn_AggrKind _tmp17F=ad->kind;enum Cyc_Absyn_AggrKind _stmttmp17=_tmp17F;enum Cyc_Absyn_AggrKind _tmp180=_stmttmp17;if(_tmp180 == Cyc_Absyn_StructA){
# 1275
struct _tuple11*_tmp181=Cyc_Port_lookup_struct_decl(env,ad->name);struct _tuple11*p=_tmp181;
return Cyc_Port_known_aggr_ct(p);}else{
# 1278
struct _tuple11*_tmp182=Cyc_Port_lookup_union_decl(env,ad->name);struct _tuple11*p=_tmp182;
return Cyc_Port_known_aggr_ct(p);};}};}case 15:
# 1282
 return Cyc_Port_arith_ct();case 16: _tmp155=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp154)->f1)->f1;{struct Cyc_List_List*fs=_tmp155;
# 1285
for(1;(unsigned)fs;fs=fs->tl){
(*((struct Cyc_Absyn_Enumfield*)fs->hd)->name).f1=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_tmp39C=env;struct _tuple0*_tmp39B=((struct Cyc_Absyn_Enumfield*)fs->hd)->name;void*_tmp39A=Cyc_Absyn_sint_type;void*_tmp399=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp39C,_tmp39B,_tmp39A,_tmp399,Cyc_Port_arith_ct(),0U);});}
# 1289
return Cyc_Port_arith_ct();}default: goto _LL15;}case 3: _tmp155=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154)->f1).elt_type;_tmp157=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154)->f1).elt_tq;_tmp156=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154)->f1).ptr_atts).rgn;_tmp159=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154)->f1).ptr_atts).nullable;_tmp15A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154)->f1).ptr_atts).bounds;_tmp15B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154)->f1).ptr_atts).zero_term;_tmp15C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154)->f1).ptr_atts).ptrloc;{void*et=_tmp155;struct Cyc_Absyn_Tqual tq=_tmp157;void*r=_tmp156;void*n=_tmp159;void*b=_tmp15A;void*zt=_tmp15B;struct Cyc_Absyn_PtrLoc*loc=_tmp15C;
# 1144
unsigned _tmp15E=(unsigned)loc?loc->ptr_loc: 0U;unsigned ptr_loc=_tmp15E;
unsigned _tmp15F=(unsigned)loc?loc->rgn_loc: 0U;unsigned rgn_loc=_tmp15F;
unsigned _tmp160=(unsigned)loc?loc->zt_loc: 0U;unsigned zt_loc=_tmp160;
# 1150
void*_tmp161=Cyc_Port_type_to_ctype(env,et);void*cet=_tmp161;
void*new_rgn;
# 1153
{void*_tmp162=r;void*_tmp163;switch(*((int*)_tmp162)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp162)->f1)== 5){
# 1155
new_rgn=Cyc_Port_heap_ct();goto _LL17;}else{goto _LL1E;}case 2: _tmp163=(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp162)->f1)->name;{struct _fat_ptr*name=_tmp163;
# 1157
new_rgn=Cyc_Port_rgnvar_ct(name);goto _LL17;}case 1:
# 1161
 if(Cyc_Port_in_fn_arg(env))
new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());else{
# 1164
if(Cyc_Port_in_fn_res(env)|| Cyc_Port_in_toplevel(env))
new_rgn=Cyc_Port_heap_ct();else{
new_rgn=Cyc_Port_var();}}
goto _LL17;default: _LL1E:
# 1169
 new_rgn=Cyc_Port_heap_ct();goto _LL17;}_LL17:;}{
# 1171
int _tmp164=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b)== 0;int orig_fat=_tmp164;
int orig_zt;
{void*_tmp165=Cyc_Absyn_compress(zt);void*_stmttmp12=_tmp165;void*_tmp166=_stmttmp12;if(*((int*)_tmp166)== 1){
orig_zt=Cyc_Port_is_char(env,et);goto _LL20;}else{
orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL20;}_LL20:;}
# 1177
if((env->gcenv)->porting){
void*_tmp167=Cyc_Port_var();void*cqv=_tmp167;
# 1182
({struct Cyc_Port_Cenv*_tmp39F=env;void*_tmp39E=cqv;void*_tmp39D=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp39F,_tmp39E,_tmp39D,tq.loc);});
# 1185
if(tq.print_const)({void*_tmp3A0=cqv;Cyc_Port_unify_ct(_tmp3A0,Cyc_Port_const_ct());});{
# 1191
void*_tmp168=Cyc_Port_var();void*cbv=_tmp168;
# 1194
({struct Cyc_Port_Cenv*_tmp3A3=env;void*_tmp3A2=cbv;void*_tmp3A1=orig_fat?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();Cyc_Port_register_ptr_cvars(_tmp3A3,_tmp3A2,_tmp3A1,ptr_loc);});
# 1196
if(orig_fat)({void*_tmp3A4=cbv;Cyc_Port_unify_ct(_tmp3A4,Cyc_Port_fat_ct());});{
void*_tmp169=Cyc_Port_var();void*czv=_tmp169;
# 1200
({struct Cyc_Port_Cenv*_tmp3A7=env;void*_tmp3A6=czv;void*_tmp3A5=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp3A7,_tmp3A6,_tmp3A5,zt_loc);});
# 1202
{void*_tmp16A=Cyc_Absyn_compress(zt);void*_stmttmp13=_tmp16A;void*_tmp16B=_stmttmp13;if(*((int*)_tmp16B)== 1)
goto _LL25;else{
# 1205
({void*_tmp3A8=czv;Cyc_Port_unify_ct(_tmp3A8,Cyc_Tcutil_force_type2bool(0,zt)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL25;}_LL25:;}
# 1211
return Cyc_Port_ptr_ct(cet,cqv,cbv,new_rgn,czv);}}}else{
# 1215
return({void*_tmp3AC=cet;void*_tmp3AB=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();void*_tmp3AA=
orig_fat?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();
# 1215
void*_tmp3A9=new_rgn;Cyc_Port_ptr_ct(_tmp3AC,_tmp3AB,_tmp3AA,_tmp3A9,
# 1217
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}}case 4: _tmp155=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp154)->f1).elt_type;_tmp157=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp154)->f1).tq;_tmp156=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp154)->f1).zero_term;_tmp158=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp154)->f1).zt_loc;{void*et=_tmp155;struct Cyc_Absyn_Tqual tq=_tmp157;void*zt=_tmp156;unsigned ztloc=_tmp158;
# 1223
void*_tmp16C=Cyc_Port_type_to_ctype(env,et);void*cet=_tmp16C;
int orig_zt;
{void*_tmp16D=Cyc_Absyn_compress(zt);void*_stmttmp14=_tmp16D;void*_tmp16E=_stmttmp14;if(*((int*)_tmp16E)== 1){
orig_zt=0;goto _LL2A;}else{
orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL2A;}_LL2A:;}
# 1229
if((env->gcenv)->porting){
void*_tmp16F=Cyc_Port_var();void*cqv=_tmp16F;
({struct Cyc_Port_Cenv*_tmp3AF=env;void*_tmp3AE=cqv;void*_tmp3AD=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp3AF,_tmp3AE,_tmp3AD,tq.loc);});{
# 1238
void*_tmp170=Cyc_Port_var();void*czv=_tmp170;
({struct Cyc_Port_Cenv*_tmp3B2=env;void*_tmp3B1=czv;void*_tmp3B0=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp3B2,_tmp3B1,_tmp3B0,ztloc);});
# 1241
{void*_tmp171=Cyc_Absyn_compress(zt);void*_stmttmp15=_tmp171;void*_tmp172=_stmttmp15;if(*((int*)_tmp172)== 1)
goto _LL2F;else{
# 1244
({void*_tmp3B3=czv;Cyc_Port_unify_ct(_tmp3B3,Cyc_Tcutil_force_type2bool(0,zt)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL2F;}_LL2F:;}
# 1247
return Cyc_Port_array_ct(cet,cqv,czv);}}else{
# 1249
return({void*_tmp3B5=cet;void*_tmp3B4=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_array_ct(_tmp3B5,_tmp3B4,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}case 5: _tmp155=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp154)->f1).ret_type;_tmp156=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp154)->f1).args;{void*rt=_tmp155;struct Cyc_List_List*args=_tmp156;
# 1253
void*_tmp173=({struct Cyc_Port_Cenv*_tmp3B6=Cyc_Port_set_cpos(env,0U);Cyc_Port_type_to_ctype(_tmp3B6,rt);});void*crt=_tmp173;
struct Cyc_Port_Cenv*_tmp174=Cyc_Port_set_cpos(env,1U);struct Cyc_Port_Cenv*env_arg=_tmp174;
struct Cyc_List_List*cargs=0;
for(1;args != 0;args=args->tl){
struct _tuple8 _tmp175=*((struct _tuple8*)args->hd);struct _tuple8 _stmttmp16=_tmp175;struct _tuple8 _tmp176=_stmttmp16;void*_tmp178;struct Cyc_Absyn_Tqual _tmp177;_tmp177=_tmp176.f2;_tmp178=_tmp176.f3;{struct Cyc_Absyn_Tqual tq=_tmp177;void*t=_tmp178;
cargs=({struct Cyc_List_List*_tmp179=_cycalloc(sizeof(*_tmp179));({void*_tmp3B7=Cyc_Port_type_to_ctype(env_arg,t);_tmp179->hd=_tmp3B7;}),_tmp179->tl=cargs;_tmp179;});}}
# 1260
return({void*_tmp3B8=crt;Cyc_Port_fn_ct(_tmp3B8,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(cargs));});}default: _LL15:
# 1290
 return Cyc_Port_arith_ct();};}
# 1295
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*_tmp183=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);void*arg1=_tmp183;
struct Cyc_List_List*_tmp184=args->tl;struct Cyc_List_List*arg2s=_tmp184;
enum Cyc_Absyn_Primop _tmp185=p;switch((int)_tmp185){case Cyc_Absyn_Plus:  {
# 1303
void*_tmp186=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(arg2s))->hd);void*arg2=_tmp186;
if(({void*_tmp3BD=arg1;Cyc_Port_leq(_tmp3BD,({void*_tmp3BC=Cyc_Port_var();void*_tmp3BB=Cyc_Port_var();void*_tmp3BA=Cyc_Port_fat_ct();void*_tmp3B9=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp3BC,_tmp3BB,_tmp3BA,_tmp3B9,Cyc_Port_var());}));})){
({void*_tmp3BE=arg2;Cyc_Port_leq(_tmp3BE,Cyc_Port_arith_ct());});
return arg1;}else{
if(({void*_tmp3C3=arg2;Cyc_Port_leq(_tmp3C3,({void*_tmp3C2=Cyc_Port_var();void*_tmp3C1=Cyc_Port_var();void*_tmp3C0=Cyc_Port_fat_ct();void*_tmp3BF=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp3C2,_tmp3C1,_tmp3C0,_tmp3BF,Cyc_Port_var());}));})){
({void*_tmp3C4=arg1;Cyc_Port_leq(_tmp3C4,Cyc_Port_arith_ct());});
return arg2;}else{
# 1311
({void*_tmp3C5=arg1;Cyc_Port_leq(_tmp3C5,Cyc_Port_arith_ct());});
({void*_tmp3C6=arg2;Cyc_Port_leq(_tmp3C6,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus:
# 1320
 if(arg2s == 0){
# 1322
({void*_tmp3C7=arg1;Cyc_Port_leq(_tmp3C7,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}else{
# 1325
void*_tmp187=Cyc_Port_compress_ct((void*)arg2s->hd);void*arg2=_tmp187;
if(({void*_tmp3CC=arg1;Cyc_Port_leq(_tmp3CC,({void*_tmp3CB=Cyc_Port_var();void*_tmp3CA=Cyc_Port_var();void*_tmp3C9=Cyc_Port_fat_ct();void*_tmp3C8=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp3CB,_tmp3CA,_tmp3C9,_tmp3C8,Cyc_Port_var());}));})){
if(({void*_tmp3D1=arg2;Cyc_Port_leq(_tmp3D1,({void*_tmp3D0=Cyc_Port_var();void*_tmp3CF=Cyc_Port_var();void*_tmp3CE=Cyc_Port_fat_ct();void*_tmp3CD=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp3D0,_tmp3CF,_tmp3CE,_tmp3CD,Cyc_Port_var());}));}))
return Cyc_Port_arith_ct();else{
# 1330
({void*_tmp3D2=arg2;Cyc_Port_leq(_tmp3D2,Cyc_Port_arith_ct());});
return arg1;}}else{
# 1334
({void*_tmp3D3=arg1;Cyc_Port_leq(_tmp3D3,Cyc_Port_arith_ct());});
({void*_tmp3D4=arg1;Cyc_Port_leq(_tmp3D4,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times:
# 1339
 goto _LL8;case Cyc_Absyn_Div: _LL8: goto _LLA;case Cyc_Absyn_Mod: _LLA: goto _LLC;case Cyc_Absyn_Eq: _LLC: goto _LLE;case Cyc_Absyn_Neq: _LLE: goto _LL10;case Cyc_Absyn_Lt: _LL10: goto _LL12;case Cyc_Absyn_Gt: _LL12: goto _LL14;case Cyc_Absyn_Lte: _LL14:
 goto _LL16;case Cyc_Absyn_Gte: _LL16: goto _LL18;case Cyc_Absyn_Bitand: _LL18: goto _LL1A;case Cyc_Absyn_Bitor: _LL1A: goto _LL1C;case Cyc_Absyn_Bitxor: _LL1C: goto _LL1E;case Cyc_Absyn_Bitlshift: _LL1E: goto _LL20;case Cyc_Absyn_Bitlrshift: _LL20:
# 1342
({void*_tmp3D5=arg1;Cyc_Port_leq(_tmp3D5,Cyc_Port_arith_ct());});
({void*_tmp3D6=(void*)((struct Cyc_List_List*)_check_null(arg2s))->hd;Cyc_Port_leq(_tmp3D6,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case Cyc_Absyn_Not:
 goto _LL24;case Cyc_Absyn_Bitnot: _LL24:
# 1347
({void*_tmp3D7=arg1;Cyc_Port_leq(_tmp3D7,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();default:
({void*_tmp188=0U;({int(*_tmp3D9)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp3D8=({const char*_tmp189="Port: unknown primop";_tag_fat(_tmp189,sizeof(char),21U);});_tmp3D9(_tmp3D8,_tag_fat(_tmp188,sizeof(void*),0));});});};}
# 1354
static struct _tuple12 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){
void*_tmp18A=e->r;void*_stmttmp18=_tmp18A;void*_tmp18B=_stmttmp18;void*_tmp18D;void*_tmp18C;switch(*((int*)_tmp18B)){case 1: _tmp18C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp18B)->f1;{void*b=_tmp18C;
# 1357
struct _tuple0*_tmp18E=Cyc_Absyn_binding2qvar(b);struct _tuple0*x=_tmp18E;
(*x).f1=Cyc_Absyn_Loc_n;{
struct _tuple14 _tmp18F=Cyc_Port_lookup_var(env,x);struct _tuple14 _stmttmp19=_tmp18F;struct _tuple14 _tmp190=_stmttmp19;unsigned _tmp192;struct _tuple12 _tmp191;_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;{struct _tuple12 p=_tmp191;unsigned loc=_tmp192;
Cyc_Port_register_localvar_usage(env,loc,e->loc,takeaddress);
return p;}}}case 23: _tmp18C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp18B)->f1;_tmp18D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp18B)->f2;{struct Cyc_Absyn_Exp*e1=_tmp18C;struct Cyc_Absyn_Exp*e2=_tmp18D;
# 1363
void*_tmp193=Cyc_Port_var();void*v=_tmp193;
void*_tmp194=Cyc_Port_var();void*q=_tmp194;
void*_tmp195=Cyc_Port_gen_exp(0,env,e1);void*t1=_tmp195;
({void*_tmp3DA=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp3DA,Cyc_Port_arith_ct());});{
void*_tmp196=({void*_tmp3DE=v;void*_tmp3DD=q;void*_tmp3DC=Cyc_Port_fat_ct();void*_tmp3DB=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp3DE,_tmp3DD,_tmp3DC,_tmp3DB,Cyc_Port_var());});void*p=_tmp196;
Cyc_Port_leq(t1,p);
return({struct _tuple12 _tmp309;_tmp309.f1=q,_tmp309.f2=v;_tmp309;});}}case 20: _tmp18C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp18B)->f1;{struct Cyc_Absyn_Exp*e1=_tmp18C;
# 1371
void*_tmp197=Cyc_Port_var();void*v=_tmp197;
void*_tmp198=Cyc_Port_var();void*q=_tmp198;
void*_tmp199=({void*_tmp3E2=v;void*_tmp3E1=q;void*_tmp3E0=Cyc_Port_var();void*_tmp3DF=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp3E2,_tmp3E1,_tmp3E0,_tmp3DF,Cyc_Port_var());});void*p=_tmp199;
({void*_tmp3E3=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp3E3,p);});
return({struct _tuple12 _tmp30A;_tmp30A.f1=q,_tmp30A.f2=v;_tmp30A;});}case 21: _tmp18C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp18B)->f1;_tmp18D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp18B)->f2;{struct Cyc_Absyn_Exp*e1=_tmp18C;struct _fat_ptr*f=_tmp18D;
# 1377
void*_tmp19A=Cyc_Port_var();void*v=_tmp19A;
void*_tmp19B=Cyc_Port_var();void*q=_tmp19B;
void*_tmp19C=Cyc_Port_gen_exp(takeaddress,env,e1);void*t1=_tmp19C;
({void*_tmp3E6=t1;Cyc_Port_leq(_tmp3E6,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp19D[1];({struct Cyc_Port_Cfield*_tmp3E4=({struct Cyc_Port_Cfield*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->qual=q,_tmp19E->f=f,_tmp19E->type=v;_tmp19E;});_tmp19D[0]=_tmp3E4;});({struct Cyc_List_List*(*_tmp3E5)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp3E5(_tag_fat(_tmp19D,sizeof(struct Cyc_Port_Cfield*),1));});})));});
return({struct _tuple12 _tmp30B;_tmp30B.f1=q,_tmp30B.f2=v;_tmp30B;});}case 22: _tmp18C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp18B)->f1;_tmp18D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp18B)->f2;{struct Cyc_Absyn_Exp*e1=_tmp18C;struct _fat_ptr*f=_tmp18D;
# 1383
void*_tmp19F=Cyc_Port_var();void*v=_tmp19F;
void*_tmp1A0=Cyc_Port_var();void*q=_tmp1A0;
void*_tmp1A1=Cyc_Port_gen_exp(0,env,e1);void*t1=_tmp1A1;
({void*_tmp3ED=t1;Cyc_Port_leq(_tmp3ED,({void*_tmp3EC=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp1A2[1];({struct Cyc_Port_Cfield*_tmp3E7=({struct Cyc_Port_Cfield*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->qual=q,_tmp1A3->f=f,_tmp1A3->type=v;_tmp1A3;});_tmp1A2[0]=_tmp3E7;});({struct Cyc_List_List*(*_tmp3E8)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp3E8(_tag_fat(_tmp1A2,sizeof(struct Cyc_Port_Cfield*),1));});}));void*_tmp3EB=
Cyc_Port_var();
# 1386
void*_tmp3EA=
Cyc_Port_var();
# 1386
void*_tmp3E9=
Cyc_Port_var();
# 1386
Cyc_Port_ptr_ct(_tmp3EC,_tmp3EB,_tmp3EA,_tmp3E9,
Cyc_Port_var());}));});
return({struct _tuple12 _tmp30C;_tmp30C.f1=q,_tmp30C.f2=v;_tmp30C;});}default:
({void*_tmp1A4=0U;({int(*_tmp3F1)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp3F0=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A7=({struct Cyc_String_pa_PrintArg_struct _tmp30D;_tmp30D.tag=0,({struct _fat_ptr _tmp3EE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp30D.f1=_tmp3EE;});_tmp30D;});void*_tmp1A5[1];_tmp1A5[0]=& _tmp1A7;({struct _fat_ptr _tmp3EF=({const char*_tmp1A6="gen_lhs: %s";_tag_fat(_tmp1A6,sizeof(char),12U);});Cyc_aprintf(_tmp3EF,_tag_fat(_tmp1A5,sizeof(void*),1));});});_tmp3F1(_tmp3F0,_tag_fat(_tmp1A4,sizeof(void*),0));});});};}
# 1394
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
return Cyc_Port_gen_exp(0,env,e);}
# 1397
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp1A8=e->r;void*_stmttmp1A=_tmp1A8;void*_tmp1A9=_stmttmp1A;enum Cyc_Absyn_Incrementor _tmp1AD;void*_tmp1AC;void*_tmp1AB;enum Cyc_Absyn_Primop _tmp1AE;void*_tmp1AA;switch(*((int*)_tmp1A9)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).Wstring_c).tag){case 2:
 goto _LL4;case 3: _LL4:
 goto _LL6;case 4: _LL6:
 goto _LL8;case 6: _LL8:
 goto _LLA;case 7: _LL14:
# 1408
 return Cyc_Port_arith_ct();case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).Int_c).val).f2 == 0)
return Cyc_Port_zero_ct();else{
return Cyc_Port_arith_ct();}case 8:
# 1412
 return({void*_tmp3F5=Cyc_Port_arith_ct();void*_tmp3F4=Cyc_Port_const_ct();void*_tmp3F3=Cyc_Port_fat_ct();void*_tmp3F2=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp3F5,_tmp3F4,_tmp3F3,_tmp3F2,Cyc_Port_zterm_ct());});case 9:
# 1414
 return({void*_tmp3F9=Cyc_Port_arith_ct();void*_tmp3F8=Cyc_Port_const_ct();void*_tmp3F7=Cyc_Port_fat_ct();void*_tmp3F6=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp3F9,_tmp3F8,_tmp3F7,_tmp3F6,Cyc_Port_zterm_ct());});default:
 return Cyc_Port_zero_ct();}case 17: _LLA:
# 1403
 goto _LLC;case 18: _LLC:
 goto _LLE;case 19: _LLE:
 goto _LL10;case 32: _LL10:
 goto _LL12;case 33: _LL12:
 goto _LL14;case 1: _tmp1AA=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;{void*b=_tmp1AA;
# 1417
struct _tuple0*_tmp1AF=Cyc_Absyn_binding2qvar(b);struct _tuple0*x=_tmp1AF;
(*x).f1=Cyc_Absyn_Loc_n;{
struct _tuple14 _tmp1B0=Cyc_Port_lookup_var(env,x);struct _tuple14 _stmttmp1B=_tmp1B0;struct _tuple14 _tmp1B1=_stmttmp1B;unsigned _tmp1B3;struct _tuple12 _tmp1B2;_tmp1B2=_tmp1B1.f1;_tmp1B3=_tmp1B1.f2;{struct _tuple12 p=_tmp1B2;unsigned loc=_tmp1B3;
struct _tuple12 _tmp1B4=p;void*_tmp1B5;_tmp1B5=_tmp1B4.f2;{void*t=_tmp1B5;
if(Cyc_Port_isfn(env,x)){
t=Cyc_Port_instantiate(t);
return({void*_tmp3FD=t;void*_tmp3FC=Cyc_Port_var();void*_tmp3FB=Cyc_Port_var();void*_tmp3FA=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp3FD,_tmp3FC,_tmp3FB,_tmp3FA,Cyc_Port_nozterm_ct());});}else{
# 1425
if(Cyc_Port_isarray(env,x)){
void*_tmp1B6=Cyc_Port_var();void*elt_type=_tmp1B6;
void*_tmp1B7=Cyc_Port_var();void*qual=_tmp1B7;
void*_tmp1B8=Cyc_Port_var();void*zt=_tmp1B8;
void*_tmp1B9=Cyc_Port_array_ct(elt_type,qual,zt);void*array_type=_tmp1B9;
Cyc_Port_unifies(t,array_type);{
void*_tmp1BA=({void*_tmp401=elt_type;void*_tmp400=qual;void*_tmp3FF=Cyc_Port_fat_ct();void*_tmp3FE=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp401,_tmp400,_tmp3FF,_tmp3FE,zt);});void*ptr_type=_tmp1BA;
return ptr_type;}}else{
# 1434
Cyc_Port_register_localvar_usage(env,loc,e->loc,takeaddress);
return t;}}}}}}case 3: _tmp1AE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{enum Cyc_Absyn_Primop p=_tmp1AE;struct Cyc_List_List*es=_tmp1AA;
# 1437
return({struct Cyc_Port_Cenv*_tmp403=env;enum Cyc_Absyn_Primop _tmp402=p;Cyc_Port_gen_primop(_tmp403,_tmp402,({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Port_gen_exp_false,env,es));});}case 4: _tmp1AA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;_tmp1AC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1A9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_Core_Opt*popt=_tmp1AB;struct Cyc_Absyn_Exp*e2=_tmp1AC;
# 1439
struct _tuple12 _tmp1BB=Cyc_Port_gen_lhs(env,e1,0);struct _tuple12 _stmttmp1C=_tmp1BB;struct _tuple12 _tmp1BC=_stmttmp1C;void*_tmp1BE;void*_tmp1BD;_tmp1BD=_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;{void*q=_tmp1BD;void*t1=_tmp1BE;
({void*_tmp404=q;Cyc_Port_unifies(_tmp404,Cyc_Port_notconst_ct());});{
void*_tmp1BF=Cyc_Port_gen_exp(0,env,e2);void*t2=_tmp1BF;
if((unsigned)popt){
struct Cyc_List_List x2=({struct Cyc_List_List _tmp30F;_tmp30F.hd=t2,_tmp30F.tl=0;_tmp30F;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp30E;_tmp30E.hd=t1,_tmp30E.tl=& x2;_tmp30E;});
void*_tmp1C0=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)popt->v,& x1);void*t3=_tmp1C0;
Cyc_Port_leq(t3,t1);
return t1;}else{
# 1449
Cyc_Port_leq(t2,t1);
return t1;}}}}case 5: _tmp1AA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;enum Cyc_Absyn_Incrementor inc=_tmp1AD;
# 1453
struct _tuple12 _tmp1C1=Cyc_Port_gen_lhs(env,e1,0);struct _tuple12 _stmttmp1D=_tmp1C1;struct _tuple12 _tmp1C2=_stmttmp1D;void*_tmp1C4;void*_tmp1C3;_tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;{void*q=_tmp1C3;void*t=_tmp1C4;
({void*_tmp405=q;Cyc_Port_unifies(_tmp405,Cyc_Port_notconst_ct());});
# 1456
({void*_tmp40A=t;Cyc_Port_leq(_tmp40A,({void*_tmp409=Cyc_Port_var();void*_tmp408=Cyc_Port_var();void*_tmp407=Cyc_Port_fat_ct();void*_tmp406=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp409,_tmp408,_tmp407,_tmp406,Cyc_Port_var());}));});
({void*_tmp40B=t;Cyc_Port_leq(_tmp40B,Cyc_Port_arith_ct());});
return t;}}case 6: _tmp1AA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;_tmp1AC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1A9)->f3;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_Absyn_Exp*e2=_tmp1AB;struct Cyc_Absyn_Exp*e3=_tmp1AC;
# 1460
void*_tmp1C5=Cyc_Port_var();void*v=_tmp1C5;
({void*_tmp40C=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp40C,Cyc_Port_arith_ct());});
({void*_tmp40D=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp40D,v);});
({void*_tmp40E=Cyc_Port_gen_exp(0,env,e3);Cyc_Port_leq(_tmp40E,v);});
return v;}case 7: _tmp1AA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_Absyn_Exp*e2=_tmp1AB;
_tmp1AA=e1;_tmp1AB=e2;goto _LL2C;}case 8: _tmp1AA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_Absyn_Exp*e2=_tmp1AB;
# 1467
({void*_tmp40F=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp40F,Cyc_Port_arith_ct());});
({void*_tmp410=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp410,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}case 9: _tmp1AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_Absyn_Exp*e2=_tmp1AB;
# 1471
Cyc_Port_gen_exp(0,env,e1);
return Cyc_Port_gen_exp(0,env,e2);}case 10: _tmp1AA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_List_List*es=_tmp1AB;
# 1474
void*_tmp1C6=Cyc_Port_var();void*v=_tmp1C6;
void*_tmp1C7=Cyc_Port_gen_exp(0,env,e1);void*t1=_tmp1C7;
struct Cyc_List_List*_tmp1C8=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Port_gen_exp_false,env,es);struct Cyc_List_List*ts=_tmp1C8;
struct Cyc_List_List*_tmp1C9=({(struct Cyc_List_List*(*)(void*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(void*))Cyc_Port_new_var;}),ts);struct Cyc_List_List*vs=_tmp1C9;
({void*_tmp415=t1;Cyc_Port_unifies(_tmp415,({void*_tmp414=Cyc_Port_fn_ct(v,vs);void*_tmp413=Cyc_Port_var();void*_tmp412=Cyc_Port_var();void*_tmp411=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp414,_tmp413,_tmp412,_tmp411,Cyc_Port_nozterm_ct());}));});
for(1;ts != 0;(ts=ts->tl,vs=vs->tl)){
Cyc_Port_leq((void*)ts->hd,(void*)((struct Cyc_List_List*)_check_null(vs))->hd);}
# 1482
return v;}case 42:
({void*_tmp1CA=0U;({int(*_tmp417)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp416=({const char*_tmp1CB="@assert";_tag_fat(_tmp1CB,sizeof(char),8U);});_tmp417(_tmp416,_tag_fat(_tmp1CA,sizeof(void*),0));});});case 11:
({void*_tmp1CC=0U;({int(*_tmp419)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp418=({const char*_tmp1CD="throw";_tag_fat(_tmp1CD,sizeof(char),6U);});_tmp419(_tmp418,_tag_fat(_tmp1CC,sizeof(void*),0));});});case 41: _tmp1AA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp1AA;
return Cyc_Port_gen_exp(0,env,e1);}case 12: _tmp1AA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp1AA;
return Cyc_Port_gen_exp(0,env,e1);}case 13:
({void*_tmp1CE=0U;({int(*_tmp41B)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp41A=({const char*_tmp1CF="instantiate";_tag_fat(_tmp1CF,sizeof(char),12U);});_tmp41B(_tmp41A,_tag_fat(_tmp1CE,sizeof(void*),0));});});case 14: _tmp1AA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{void*t=_tmp1AA;struct Cyc_Absyn_Exp*e1=_tmp1AB;
# 1489
void*_tmp1D0=Cyc_Port_type_to_ctype(env,t);void*t1=_tmp1D0;
void*_tmp1D1=Cyc_Port_gen_exp(0,env,e1);void*t2=_tmp1D1;
Cyc_Port_leq(t1,t2);
return t2;}case 15: _tmp1AA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp1AA;
# 1494
struct _tuple12 _tmp1D2=Cyc_Port_gen_lhs(env,e1,1);struct _tuple12 _stmttmp1E=_tmp1D2;struct _tuple12 _tmp1D3=_stmttmp1E;void*_tmp1D5;void*_tmp1D4;_tmp1D4=_tmp1D3.f1;_tmp1D5=_tmp1D3.f2;{void*q=_tmp1D4;void*t=_tmp1D5;
return({void*_tmp41F=t;void*_tmp41E=q;void*_tmp41D=Cyc_Port_var();void*_tmp41C=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp41F,_tmp41E,_tmp41D,_tmp41C,Cyc_Port_var());});}}case 23: _tmp1AA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_Absyn_Exp*e2=_tmp1AB;
# 1497
void*_tmp1D6=Cyc_Port_var();void*v=_tmp1D6;
({void*_tmp420=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp420,Cyc_Port_arith_ct());});{
void*_tmp1D7=Cyc_Port_gen_exp(0,env,e1);void*t=_tmp1D7;
({void*_tmp425=t;Cyc_Port_leq(_tmp425,({void*_tmp424=v;void*_tmp423=Cyc_Port_var();void*_tmp422=Cyc_Port_fat_ct();void*_tmp421=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp424,_tmp423,_tmp422,_tmp421,Cyc_Port_var());}));});
return v;}}case 20: _tmp1AA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;{struct Cyc_Absyn_Exp*e1=_tmp1AA;
# 1503
void*_tmp1D8=Cyc_Port_var();void*v=_tmp1D8;
({void*_tmp42A=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp42A,({void*_tmp429=v;void*_tmp428=Cyc_Port_var();void*_tmp427=Cyc_Port_var();void*_tmp426=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp429,_tmp428,_tmp427,_tmp426,Cyc_Port_var());}));});
return v;}case 21: _tmp1AA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct _fat_ptr*f=_tmp1AB;
# 1507
void*_tmp1D9=Cyc_Port_var();void*v=_tmp1D9;
void*_tmp1DA=Cyc_Port_gen_exp(takeaddress,env,e1);void*t=_tmp1DA;
({void*_tmp42E=t;Cyc_Port_leq(_tmp42E,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp1DB[1];({struct Cyc_Port_Cfield*_tmp42C=({struct Cyc_Port_Cfield*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));({void*_tmp42B=Cyc_Port_var();_tmp1DC->qual=_tmp42B;}),_tmp1DC->f=f,_tmp1DC->type=v;_tmp1DC;});_tmp1DB[0]=_tmp42C;});({struct Cyc_List_List*(*_tmp42D)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp42D(_tag_fat(_tmp1DB,sizeof(struct Cyc_Port_Cfield*),1));});})));});
return v;}case 22: _tmp1AA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct _fat_ptr*f=_tmp1AB;
# 1512
void*_tmp1DD=Cyc_Port_var();void*v=_tmp1DD;
void*_tmp1DE=Cyc_Port_gen_exp(0,env,e1);void*t=_tmp1DE;
({void*_tmp436=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp436,({void*_tmp435=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp1DF[1];({struct Cyc_Port_Cfield*_tmp430=({struct Cyc_Port_Cfield*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));({void*_tmp42F=Cyc_Port_var();_tmp1E0->qual=_tmp42F;}),_tmp1E0->f=f,_tmp1E0->type=v;_tmp1E0;});_tmp1DF[0]=_tmp430;});({struct Cyc_List_List*(*_tmp431)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp431(_tag_fat(_tmp1DF,sizeof(struct Cyc_Port_Cfield*),1));});}));void*_tmp434=
Cyc_Port_var();
# 1514
void*_tmp433=
Cyc_Port_var();
# 1514
void*_tmp432=
Cyc_Port_var();
# 1514
Cyc_Port_ptr_ct(_tmp435,_tmp434,_tmp433,_tmp432,
Cyc_Port_var());}));});
return v;}case 34: _tmp1AA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).elt_type;_tmp1AB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1).num_elts;{void**topt=_tmp1AA;struct Cyc_Absyn_Exp*e1=_tmp1AB;
# 1518
({void*_tmp437=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_tmp437,Cyc_Port_arith_ct());});{
void*_tmp1E1=(unsigned)topt?Cyc_Port_type_to_ctype(env,*topt): Cyc_Port_var();void*t=_tmp1E1;
return({void*_tmp43B=t;void*_tmp43A=Cyc_Port_var();void*_tmp439=Cyc_Port_fat_ct();void*_tmp438=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp43B,_tmp43A,_tmp439,_tmp438,Cyc_Port_var());});}}case 2:
({void*_tmp1E2=0U;({int(*_tmp43D)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp43C=({const char*_tmp1E3="Pragma_e";_tag_fat(_tmp1E3,sizeof(char),9U);});_tmp43D(_tmp43C,_tag_fat(_tmp1E2,sizeof(void*),0));});});case 35: _tmp1AA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;_tmp1AB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp1A9)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1AA;struct Cyc_Absyn_Exp*e2=_tmp1AB;
({void*_tmp1E4=0U;({int(*_tmp43F)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp43E=({const char*_tmp1E5="Swap_e";_tag_fat(_tmp1E5,sizeof(char),7U);});_tmp43F(_tmp43E,_tag_fat(_tmp1E4,sizeof(void*),0));});});}case 16:
({void*_tmp1E6=0U;({int(*_tmp441)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp440=({const char*_tmp1E7="New_e";_tag_fat(_tmp1E7,sizeof(char),6U);});_tmp441(_tmp440,_tag_fat(_tmp1E6,sizeof(void*),0));});});case 24:
({void*_tmp1E8=0U;({int(*_tmp443)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp442=({const char*_tmp1E9="Tuple_e";_tag_fat(_tmp1E9,sizeof(char),8U);});_tmp443(_tmp442,_tag_fat(_tmp1E8,sizeof(void*),0));});});case 25:
({void*_tmp1EA=0U;({int(*_tmp445)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp444=({const char*_tmp1EB="CompoundLit_e";_tag_fat(_tmp1EB,sizeof(char),14U);});_tmp445(_tmp444,_tag_fat(_tmp1EA,sizeof(void*),0));});});case 26:
({void*_tmp1EC=0U;({int(*_tmp447)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp446=({const char*_tmp1ED="Array_e";_tag_fat(_tmp1ED,sizeof(char),8U);});_tmp447(_tmp446,_tag_fat(_tmp1EC,sizeof(void*),0));});});case 27:
({void*_tmp1EE=0U;({int(*_tmp449)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp448=({const char*_tmp1EF="Comprehension_e";_tag_fat(_tmp1EF,sizeof(char),16U);});_tmp449(_tmp448,_tag_fat(_tmp1EE,sizeof(void*),0));});});case 28:
({void*_tmp1F0=0U;({int(*_tmp44B)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp44A=({const char*_tmp1F1="ComprehensionNoinit_e";_tag_fat(_tmp1F1,sizeof(char),22U);});_tmp44B(_tmp44A,_tag_fat(_tmp1F0,sizeof(void*),0));});});case 29:
({void*_tmp1F2=0U;({int(*_tmp44D)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp44C=({const char*_tmp1F3="Aggregate_e";_tag_fat(_tmp1F3,sizeof(char),12U);});_tmp44D(_tmp44C,_tag_fat(_tmp1F2,sizeof(void*),0));});});case 30:
({void*_tmp1F4=0U;({int(*_tmp44F)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp44E=({const char*_tmp1F5="AnonStruct_e";_tag_fat(_tmp1F5,sizeof(char),13U);});_tmp44F(_tmp44E,_tag_fat(_tmp1F4,sizeof(void*),0));});});case 31:
({void*_tmp1F6=0U;({int(*_tmp451)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp450=({const char*_tmp1F7="Datatype_e";_tag_fat(_tmp1F7,sizeof(char),11U);});_tmp451(_tmp450,_tag_fat(_tmp1F6,sizeof(void*),0));});});case 36:
({void*_tmp1F8=0U;({int(*_tmp453)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp452=({const char*_tmp1F9="UnresolvedMem_e";_tag_fat(_tmp1F9,sizeof(char),16U);});_tmp453(_tmp452,_tag_fat(_tmp1F8,sizeof(void*),0));});});case 37: _tmp1AA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp1A9)->f1;{struct Cyc_Absyn_Stmt*s=_tmp1AA;
({void*_tmp1FA=0U;({int(*_tmp455)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp454=({const char*_tmp1FB="StmtExp_e";_tag_fat(_tmp1FB,sizeof(char),10U);});_tmp455(_tmp454,_tag_fat(_tmp1FA,sizeof(void*),0));});});}case 39:
({void*_tmp1FC=0U;({int(*_tmp457)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp456=({const char*_tmp1FD="Valueof_e";_tag_fat(_tmp1FD,sizeof(char),10U);});_tmp457(_tmp456,_tag_fat(_tmp1FC,sizeof(void*),0));});});case 40:
({void*_tmp1FE=0U;({int(*_tmp459)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp458=({const char*_tmp1FF="Asm_e";_tag_fat(_tmp1FF,sizeof(char),6U);});_tmp459(_tmp458,_tag_fat(_tmp1FE,sizeof(void*),0));});});default:
({void*_tmp200=0U;({int(*_tmp45B)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp45A=({const char*_tmp201="Tagcheck_e";_tag_fat(_tmp201,sizeof(char),11U);});_tmp45B(_tmp45A,_tag_fat(_tmp200,sizeof(void*),0));});});};}
# 1541
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp202=s->r;void*_stmttmp1F=_tmp202;void*_tmp203=_stmttmp1F;void*_tmp207;void*_tmp206;void*_tmp205;void*_tmp204;switch(*((int*)_tmp203)){case 0:
 return;case 1: _tmp204=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;{struct Cyc_Absyn_Exp*e=_tmp204;
Cyc_Port_gen_exp(0,env,e);return;}case 2: _tmp204=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;_tmp205=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp203)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp204;struct Cyc_Absyn_Stmt*s2=_tmp205;
Cyc_Port_gen_stmt(env,s1);Cyc_Port_gen_stmt(env,s2);return;}case 3: _tmp204=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp204;
# 1547
void*_tmp208=Cyc_Port_lookup_return_type(env);void*v=_tmp208;
void*_tmp209=(unsigned)eopt?Cyc_Port_gen_exp(0,env,eopt): Cyc_Port_void_ct();void*t=_tmp209;
Cyc_Port_leq(t,v);
return;}case 4: _tmp204=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;_tmp205=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp203)->f2;_tmp206=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp203)->f3;{struct Cyc_Absyn_Exp*e=_tmp204;struct Cyc_Absyn_Stmt*s1=_tmp205;struct Cyc_Absyn_Stmt*s2=_tmp206;
# 1552
({void*_tmp45C=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp45C,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,s1);
Cyc_Port_gen_stmt(env,s2);
return;}case 5: _tmp204=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp203)->f1).f1;_tmp205=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp203)->f2;{struct Cyc_Absyn_Exp*e=_tmp204;struct Cyc_Absyn_Stmt*s=_tmp205;
# 1557
({void*_tmp45D=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp45D,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,s);
return;}case 6:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 return;case 9: _tmp204=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;_tmp205=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp203)->f2).f1;_tmp206=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp203)->f3).f1;_tmp207=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp203)->f4;{struct Cyc_Absyn_Exp*e1=_tmp204;struct Cyc_Absyn_Exp*e2=_tmp205;struct Cyc_Absyn_Exp*e3=_tmp206;struct Cyc_Absyn_Stmt*s=_tmp207;
# 1564
Cyc_Port_gen_exp(0,env,e1);
({void*_tmp45E=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_tmp45E,Cyc_Port_arith_ct());});
Cyc_Port_gen_exp(0,env,e3);
Cyc_Port_gen_stmt(env,s);
return;}case 10: _tmp204=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;_tmp205=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp203)->f2;{struct Cyc_Absyn_Exp*e=_tmp204;struct Cyc_List_List*scs=_tmp205;
# 1570
({void*_tmp45F=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp45F,Cyc_Port_arith_ct());});
for(1;(unsigned)scs;scs=scs->tl){
# 1573
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
# 1575
return;}case 11:
({void*_tmp20A=0U;({int(*_tmp461)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp460=({const char*_tmp20B="fallthru";_tag_fat(_tmp20B,sizeof(char),9U);});_tmp461(_tmp460,_tag_fat(_tmp20A,sizeof(void*),0));});});case 12: _tmp204=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;_tmp205=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp203)->f2;{struct Cyc_Absyn_Decl*d=_tmp204;struct Cyc_Absyn_Stmt*s=_tmp205;
# 1578
env=Cyc_Port_gen_localdecl(env,d,0);Cyc_Port_gen_stmt(env,s);return;}case 13: _tmp204=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp203)->f2;{struct Cyc_Absyn_Stmt*s=_tmp204;
Cyc_Port_gen_stmt(env,s);return;}case 14: _tmp204=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp203)->f1;_tmp205=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp203)->f2).f1;{struct Cyc_Absyn_Stmt*s=_tmp204;struct Cyc_Absyn_Exp*e=_tmp205;
# 1581
Cyc_Port_gen_stmt(env,s);
({void*_tmp462=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_tmp462,Cyc_Port_arith_ct());});
return;}default:
({void*_tmp20C=0U;({int(*_tmp464)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp463=({const char*_tmp20D="try/catch";_tag_fat(_tmp20D,sizeof(char),10U);});_tmp464(_tmp463,_tag_fat(_tmp20C,sizeof(void*),0));});});};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1589
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp20E=e->r;void*_stmttmp20=_tmp20E;void*_tmp20F=_stmttmp20;void*_tmp210;switch(*((int*)_tmp20F)){case 36: _tmp210=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;{struct Cyc_List_List*dles=_tmp210;
_tmp210=dles;goto _LL4;}case 26: _tmp210=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp20F)->f1;_LL4: {struct Cyc_List_List*dles=_tmp210;
_tmp210=dles;goto _LL6;}case 25: _tmp210=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp20F)->f2;_LL6: {struct Cyc_List_List*dles=_tmp210;
# 1594
LOOP: {
void*_tmp211=t;unsigned _tmp214;void*_tmp213;void*_tmp212;switch(*((int*)_tmp211)){case 8: _tmp212=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp211)->f1;{struct _tuple0*n=_tmp212;
# 1597
(*n).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 4: _tmp212=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp211)->f1).elt_type;_tmp213=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp211)->f1).zero_term;_tmp214=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp211)->f1).zt_loc;{void*et=_tmp212;void*zt=_tmp213;unsigned ztl=_tmp214;
# 1600
void*_tmp215=Cyc_Port_var();void*v=_tmp215;
void*_tmp216=Cyc_Port_var();void*q=_tmp216;
void*_tmp217=Cyc_Port_var();void*z=_tmp217;
void*_tmp218=Cyc_Port_type_to_ctype(env,et);void*t=_tmp218;
for(1;dles != 0;dles=dles->tl){
struct _tuple18 _tmp219=*((struct _tuple18*)dles->hd);struct _tuple18 _stmttmp21=_tmp219;struct _tuple18 _tmp21A=_stmttmp21;void*_tmp21C;void*_tmp21B;_tmp21B=_tmp21A.f1;_tmp21C=_tmp21A.f2;{struct Cyc_List_List*d=_tmp21B;struct Cyc_Absyn_Exp*e=_tmp21C;
if((unsigned)d)({void*_tmp21D=0U;({int(*_tmp466)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp465=({const char*_tmp21E="designators in initializers";_tag_fat(_tmp21E,sizeof(char),28U);});_tmp466(_tmp465,_tag_fat(_tmp21D,sizeof(void*),0));});});{
void*_tmp21F=Cyc_Port_gen_initializer(env,et,e);void*te=_tmp21F;
Cyc_Port_leq(te,v);}}}
# 1610
return Cyc_Port_array_ct(v,q,z);}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp211)->f1)== 20){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp211)->f1)->f1).UnknownAggr).tag == 1){if((((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp211)->f1)->f1).UnknownAggr).val).f1 == Cyc_Absyn_StructA){_tmp212=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp211)->f1)->f1).UnknownAggr).val).f2;{struct _tuple0*n=_tmp212;
# 1612
(*n).f1=Cyc_Absyn_Loc_n;{
struct _tuple11 _tmp220=*Cyc_Port_lookup_struct_decl(env,n);struct _tuple11 _stmttmp22=_tmp220;struct _tuple11 _tmp221=_stmttmp22;void*_tmp222;_tmp222=_tmp221.f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp222;
if(ad == 0)({void*_tmp223=0U;({int(*_tmp468)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp467=({const char*_tmp224="struct is not yet defined";_tag_fat(_tmp224,sizeof(char),26U);});_tmp468(_tmp467,_tag_fat(_tmp223,sizeof(void*),0));});});
_tmp212=ad;goto _LL15;}}}}else{goto _LL16;}}else{_tmp212=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp211)->f1)->f1).KnownAggr).val;_LL15: {struct Cyc_Absyn_Aggrdecl*ad=_tmp212;
# 1617
struct Cyc_List_List*_tmp225=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp225;
for(1;dles != 0;dles=dles->tl){
struct _tuple18 _tmp226=*((struct _tuple18*)dles->hd);struct _tuple18 _stmttmp23=_tmp226;struct _tuple18 _tmp227=_stmttmp23;void*_tmp229;void*_tmp228;_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;{struct Cyc_List_List*d=_tmp228;struct Cyc_Absyn_Exp*e=_tmp229;
if((unsigned)d)({void*_tmp22A=0U;({int(*_tmp46A)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp469=({const char*_tmp22B="designators in initializers";_tag_fat(_tmp22B,sizeof(char),28U);});_tmp46A(_tmp469,_tag_fat(_tmp22A,sizeof(void*),0));});});{
struct Cyc_Absyn_Aggrfield*_tmp22C=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fields))->hd;struct Cyc_Absyn_Aggrfield*f=_tmp22C;
fields=fields->tl;{
void*_tmp22D=Cyc_Port_gen_initializer(env,f->type,e);void*te=_tmp22D;;}}}}
# 1625
return Cyc_Port_type_to_ctype(env,t);}}}else{goto _LL16;}default: _LL16:
({void*_tmp22E=0U;({int(*_tmp46C)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp46B=({const char*_tmp22F="bad type for aggregate initializer";_tag_fat(_tmp22F,sizeof(char),35U);});_tmp46C(_tmp46B,_tag_fat(_tmp22E,sizeof(void*),0));});});};}}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp20F)->f1).Wstring_c).tag){case 8:
# 1628
 goto _LLA;case 9: _LLA:
# 1630
 LOOP2: {
void*_tmp230=t;void*_tmp231;switch(*((int*)_tmp230)){case 8: _tmp231=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp230)->f1;{struct _tuple0*n=_tmp231;
# 1633
(*n).f1=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP2;}case 4:
 return({void*_tmp46E=Cyc_Port_arith_ct();void*_tmp46D=Cyc_Port_const_ct();Cyc_Port_array_ct(_tmp46E,_tmp46D,Cyc_Port_zterm_ct());});default:
 return Cyc_Port_gen_exp(0,env,e);};}default: goto _LLB;}default: _LLB:
# 1638
 return Cyc_Port_gen_exp(0,env,e);};}
# 1643
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){
void*_tmp232=d->r;void*_stmttmp24=_tmp232;void*_tmp233=_stmttmp24;void*_tmp234;if(*((int*)_tmp233)== 0){_tmp234=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp233)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp234;
# 1646
if(!fromglobal)Cyc_Port_register_localvar_decl(env,vd->name,vd->varloc,vd->type,vd->initializer);{
void*_tmp235=Cyc_Port_var();void*t=_tmp235;
void*q;
if((env->gcenv)->porting){
q=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp471=env;void*_tmp470=q;void*_tmp46F=
(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1651
Cyc_Port_register_const_cvar(_tmp471,_tmp470,_tmp46F,(vd->tq).loc);});}else{
# 1660
q=(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1662
(*vd->name).f1=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,vd->name,vd->type,q,t,vd->varloc);
({void*_tmp472=t;Cyc_Port_unifies(_tmp472,Cyc_Port_type_to_ctype(env,vd->type));});
if((unsigned)vd->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*t2=Cyc_Port_gen_initializer(env,vd->type,e);
Cyc_Port_leq(t2,t);}
# 1670
return env;}}}else{
({void*_tmp236=0U;({int(*_tmp474)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;});struct _fat_ptr _tmp473=({const char*_tmp237="non-local decl that isn't a variable";_tag_fat(_tmp237,sizeof(char),37U);});_tmp474(_tmp473,_tag_fat(_tmp236,sizeof(void*),0));});});};}
# 1675
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp238=*arg;struct _tuple8 _stmttmp25=_tmp238;struct _tuple8 _tmp239=_stmttmp25;void*_tmp23C;struct Cyc_Absyn_Tqual _tmp23B;void*_tmp23A;_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;_tmp23C=_tmp239.f3;{struct _fat_ptr*x=_tmp23A;struct Cyc_Absyn_Tqual q=_tmp23B;void*t=_tmp23C;
return({struct _tuple8*_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D->f1=0,_tmp23D->f2=q,_tmp23D->f3=t;_tmp23D;});}}
# 1680
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*,struct Cyc_List_List*);struct _tuple19{struct _fat_ptr*f1;void*f2;void*f3;void*f4;};
# 1682
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp23E=d->r;void*_stmttmp26=_tmp23E;void*_tmp23F=_stmttmp26;void*_tmp240;switch(*((int*)_tmp23F)){case 13:
# 1685
(env->gcenv)->porting=1;
return env;case 14:
# 1688
(env->gcenv)->porting=0;
return env;case 0: _tmp240=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp23F)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp240;
# 1691
(*vd->name).f1=Cyc_Absyn_Loc_n;
# 1695
if(Cyc_Port_declared_var(env,vd->name)){
struct _tuple14 _tmp241=Cyc_Port_lookup_var(env,vd->name);struct _tuple14 _stmttmp27=_tmp241;struct _tuple14 _tmp242=_stmttmp27;unsigned _tmp244;struct _tuple12 _tmp243;_tmp243=_tmp242.f1;_tmp244=_tmp242.f2;{struct _tuple12 p=_tmp243;unsigned loc=_tmp244;
struct _tuple12 _tmp245=p;void*_tmp247;void*_tmp246;_tmp246=_tmp245.f1;_tmp247=_tmp245.f2;{void*q=_tmp246;void*t=_tmp247;
void*q2;
if((env->gcenv)->porting && !Cyc_Port_isfn(env,vd->name)){
q2=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp477=env;void*_tmp476=q2;void*_tmp475=
(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1701
Cyc_Port_register_const_cvar(_tmp477,_tmp476,_tmp475,(vd->tq).loc);});}else{
# 1710
q2=(vd->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1712
Cyc_Port_unifies(q,q2);
({void*_tmp478=t;Cyc_Port_unifies(_tmp478,Cyc_Port_type_to_ctype(env,vd->type));});
if((unsigned)vd->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
({void*_tmp479=Cyc_Port_gen_initializer(env,vd->type,e);Cyc_Port_leq(_tmp479,t);});}
# 1718
return env;}}}else{
# 1720
return Cyc_Port_gen_localdecl(env,d,1);}}case 1: _tmp240=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp23F)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp240;
# 1726
(*fd->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple15*predeclared=0;
# 1729
if(Cyc_Port_declared_var(env,fd->name))
predeclared=Cyc_Port_lookup_full_var(env,fd->name);{
# 1732
void*_tmp248=(fd->i).ret_type;void*rettype=_tmp248;
struct Cyc_List_List*_tmp249=(fd->i).args;struct Cyc_List_List*args=_tmp249;
struct Cyc_List_List*_tmp24A=({(struct Cyc_List_List*(*)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Port_make_targ,args);struct Cyc_List_List*targs=_tmp24A;
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24B=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp266=_cycalloc(sizeof(*_tmp266));
_tmp266->tag=5,(_tmp266->f1).tvars=0,(_tmp266->f1).effect=0,({struct Cyc_Absyn_Tqual _tmp47A=Cyc_Absyn_empty_tqual(0U);(_tmp266->f1).ret_tqual=_tmp47A;}),(_tmp266->f1).ret_type=rettype,(_tmp266->f1).args=targs,(_tmp266->f1).c_varargs=0,(_tmp266->f1).cyc_varargs=0,(_tmp266->f1).rgn_po=0,(_tmp266->f1).attributes=0,(_tmp266->f1).requires_clause=0,(_tmp266->f1).requires_relns=0,(_tmp266->f1).ensures_clause=0,(_tmp266->f1).ensures_relns=0,(_tmp266->f1).return_value=0;_tmp266;});
# 1735
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp24B;
# 1739
struct Cyc_Port_Cenv*_tmp24C=Cyc_Port_set_cpos(env,2U);struct Cyc_Port_Cenv*fn_env=_tmp24C;
void*_tmp24D=Cyc_Port_type_to_ctype(fn_env,rettype);void*c_rettype=_tmp24D;
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp24E=args;struct Cyc_List_List*xs=_tmp24E;for(0;(unsigned)xs;xs=xs->tl){
struct _tuple8 _tmp24F=*((struct _tuple8*)xs->hd);struct _tuple8 _stmttmp28=_tmp24F;struct _tuple8 _tmp250=_stmttmp28;void*_tmp253;struct Cyc_Absyn_Tqual _tmp252;void*_tmp251;_tmp251=_tmp250.f1;_tmp252=_tmp250.f2;_tmp253=_tmp250.f3;{struct _fat_ptr*x=_tmp251;struct Cyc_Absyn_Tqual tq=_tmp252;void*t=_tmp253;
# 1747
void*_tmp254=Cyc_Port_type_to_ctype(fn_env,t);void*ct=_tmp254;
void*tqv;
if((env->gcenv)->porting){
tqv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp47D=env;void*_tmp47C=tqv;void*_tmp47B=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp47D,_tmp47C,_tmp47B,tq.loc);});}else{
# 1759
tqv=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1761
c_args=({struct Cyc_List_List*_tmp256=_cycalloc(sizeof(*_tmp256));({struct _tuple19*_tmp47E=({struct _tuple19*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255->f1=x,_tmp255->f2=t,_tmp255->f3=tqv,_tmp255->f4=ct;_tmp255;});_tmp256->hd=_tmp47E;}),_tmp256->tl=c_args;_tmp256;});
c_arg_types=({struct Cyc_List_List*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257->hd=ct,_tmp257->tl=c_arg_types;_tmp257;});}}}
# 1764
c_args=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(c_args);
c_arg_types=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(c_arg_types);{
void*_tmp258=Cyc_Port_fn_ct(c_rettype,c_arg_types);void*ctype=_tmp258;
# 1770
(*fd->name).f1=Cyc_Absyn_Loc_n;
fn_env=({struct Cyc_Port_Cenv*_tmp482=fn_env;struct _tuple0*_tmp481=fd->name;void*_tmp480=(void*)fntype;void*_tmp47F=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp482,_tmp481,_tmp480,_tmp47F,ctype,0U);});
Cyc_Port_add_return_type(fn_env,c_rettype);
{struct Cyc_List_List*_tmp259=c_args;struct Cyc_List_List*xs=_tmp259;for(0;(unsigned)xs;xs=xs->tl){
struct _tuple19 _tmp25A=*((struct _tuple19*)xs->hd);struct _tuple19 _stmttmp29=_tmp25A;struct _tuple19 _tmp25B=_stmttmp29;void*_tmp25F;void*_tmp25E;void*_tmp25D;void*_tmp25C;_tmp25C=_tmp25B.f1;_tmp25D=_tmp25B.f2;_tmp25E=_tmp25B.f3;_tmp25F=_tmp25B.f4;{struct _fat_ptr*x=_tmp25C;void*t=_tmp25D;void*q=_tmp25E;void*ct=_tmp25F;
fn_env=({struct Cyc_Port_Cenv*_tmp486=fn_env;struct _tuple0*_tmp485=({struct _tuple0*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->f1=Cyc_Absyn_Loc_n,_tmp260->f2=(struct _fat_ptr*)_check_null(x);_tmp260;});void*_tmp484=t;void*_tmp483=q;Cyc_Port_add_var(_tmp486,_tmp485,_tmp484,_tmp483,ct,0U);});}}}
# 1777
Cyc_Port_gen_stmt(fn_env,fd->body);
# 1782
Cyc_Port_generalize(0,ctype);{
# 1789
struct Cyc_Dict_Dict counts=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,ctype);
# 1793
Cyc_Port_register_rgns(env,counts,1,(void*)fntype,ctype);
env=({struct Cyc_Port_Cenv*_tmp48A=fn_env;struct _tuple0*_tmp489=fd->name;void*_tmp488=(void*)fntype;void*_tmp487=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp48A,_tmp489,_tmp488,_tmp487,ctype,0U);});
if((unsigned)predeclared){
# 1802
struct _tuple15 _tmp261=*predeclared;struct _tuple15 _stmttmp2A=_tmp261;struct _tuple15 _tmp262=_stmttmp2A;void*_tmp265;void*_tmp264;void*_tmp263;_tmp263=_tmp262.f1;_tmp264=(_tmp262.f2)->f1;_tmp265=(_tmp262.f2)->f2;{void*orig_type=_tmp263;void*q2=_tmp264;void*t2=_tmp265;
({void*_tmp48B=q2;Cyc_Port_unifies(_tmp48B,Cyc_Port_const_ct());});
({void*_tmp48C=t2;Cyc_Port_unifies(_tmp48C,Cyc_Port_instantiate(ctype));});
# 1806
Cyc_Port_register_rgns(env,counts,1,orig_type,ctype);}}
# 1808
return env;}}}}}case 8: _tmp240=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp23F)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmp240;
# 1814
void*_tmp267=(void*)_check_null(td->defn);void*t=_tmp267;
void*_tmp268=Cyc_Port_type_to_ctype(env,t);void*ct=_tmp268;
(*td->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,td->name,t,ct);
return env;}case 5: _tmp240=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp23F)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp240;
# 1824
struct _tuple0*_tmp269=ad->name;struct _tuple0*name=_tmp269;
(*ad->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp26A=ad->kind;enum Cyc_Absyn_AggrKind _stmttmp2B=_tmp26A;enum Cyc_Absyn_AggrKind _tmp26B=_stmttmp2B;if(_tmp26B == Cyc_Absyn_StructA){
# 1830
previous=Cyc_Port_lookup_struct_decl(env,name);
goto _LL20;}else{
# 1833
previous=Cyc_Port_lookup_union_decl(env,name);
goto _LL20;}_LL20:;}
# 1836
if((unsigned)ad->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp26C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*fields=_tmp26C;for(0;(unsigned)fields;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp26D=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*f=_tmp26D;
void*qv;
if((env->gcenv)->porting){
qv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp48F=env;void*_tmp48E=qv;void*_tmp48D=
(f->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1843
Cyc_Port_register_const_cvar(_tmp48F,_tmp48E,_tmp48D,(f->tq).loc);});}else{
# 1852
qv=(f->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
# 1854
void*_tmp26E=Cyc_Port_type_to_ctype(env,f->type);void*ct=_tmp26E;
if(cf != 0){
struct Cyc_Port_Cfield _tmp26F=*((struct Cyc_Port_Cfield*)cf->hd);struct Cyc_Port_Cfield _stmttmp2C=_tmp26F;struct Cyc_Port_Cfield _tmp270=_stmttmp2C;void*_tmp272;void*_tmp271;_tmp271=_tmp270.qual;_tmp272=_tmp270.type;{void*qv2=_tmp271;void*ct2=_tmp272;
cf=cf->tl;
Cyc_Port_unifies(qv,qv2);
Cyc_Port_unifies(ct,ct2);}}
# 1861
curr=({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));({struct Cyc_Port_Cfield*_tmp490=({struct Cyc_Port_Cfield*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->qual=qv,_tmp273->f=f->name,_tmp273->type=ct;_tmp273;});_tmp274->hd=_tmp490;}),_tmp274->tl=curr;_tmp274;});}}}
# 1863
curr=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(curr);
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1868
return env;}}case 7: _tmp240=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp23F)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp240;
# 1873
(*ed->name).f1=Cyc_Absyn_Loc_n;
# 1875
if((unsigned)ed->fields){
struct Cyc_List_List*_tmp275=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;struct Cyc_List_List*fs=_tmp275;for(0;(unsigned)fs;fs=fs->tl){
(*((struct Cyc_Absyn_Enumfield*)fs->hd)->name).f1=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_tmp494=env;struct _tuple0*_tmp493=((struct Cyc_Absyn_Enumfield*)fs->hd)->name;void*_tmp492=Cyc_Absyn_enum_type(ed->name,ed);void*_tmp491=
Cyc_Port_const_ct();
# 1878
Cyc_Port_add_var(_tmp494,_tmp493,_tmp492,_tmp491,
Cyc_Port_arith_ct(),0U);});}}
# 1881
return env;}default:
# 1883
 if((env->gcenv)->porting)
({void*_tmp276=0U;({struct Cyc___cycFILE*_tmp496=Cyc_stderr;struct _fat_ptr _tmp495=({const char*_tmp277="Warning: Cyclone declaration found in code to be ported -- skipping.";_tag_fat(_tmp277,sizeof(char),69U);});Cyc_fprintf(_tmp496,_tmp495,_tag_fat(_tmp276,sizeof(void*),0));});});
return env;};}
# 1890
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(1;(unsigned)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}
return env;}
# 1897
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1899
struct Cyc_Port_Cenv*env=({struct Cyc_Port_Cenv*_tmp497=Cyc_Port_initial_cenv();Cyc_Port_gen_topdecls(_tmp497,ds);});
# 1901
struct Cyc_List_List*_tmp278=(env->gcenv)->pointer_edits;struct Cyc_List_List*ptrs=_tmp278;
struct Cyc_List_List*_tmp279=(env->gcenv)->qualifier_edits;struct Cyc_List_List*consts=_tmp279;
struct Cyc_List_List*_tmp27A=(env->gcenv)->zeroterm_edits;struct Cyc_List_List*zts=_tmp27A;
struct Cyc_List_List*_tmp27B=(env->gcenv)->edits;struct Cyc_List_List*edits=_tmp27B;
struct Cyc_List_List*_tmp27C=(env->gcenv)->vardecl_locs;struct Cyc_List_List*localvars=_tmp27C;
# 1907
for(1;(unsigned)localvars;localvars=localvars->tl){
struct _tuple16 _tmp27D=*((struct _tuple16*)localvars->hd);struct _tuple16 _stmttmp2D=_tmp27D;struct _tuple16 _tmp27E=_stmttmp2D;void*_tmp281;unsigned _tmp280;void*_tmp27F;_tmp27F=_tmp27E.f1;_tmp280=_tmp27E.f2;_tmp281=_tmp27E.f3;{struct _tuple0*var=_tmp27F;unsigned loc=_tmp280;void*vartype=_tmp281;
struct _tuple0 _tmp282=*var;struct _tuple0 _stmttmp2E=_tmp282;struct _tuple0 _tmp283=_stmttmp2E;void*_tmp284;_tmp284=_tmp283.f2;{struct _fat_ptr*x=_tmp284;
struct Cyc_Port_VarUsage*_tmp285=({(struct Cyc_Port_VarUsage*(*)(struct Cyc_Hashtable_Table*,unsigned))Cyc_Hashtable_lookup;})((env->gcenv)->varusage_tab,loc);struct Cyc_Port_VarUsage*varusage=_tmp285;
if(((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken){
if((unsigned)varusage->init){
# 1914
edits=({struct Cyc_List_List*_tmp289=_cycalloc(sizeof(*_tmp289));({struct Cyc_Port_Edit*_tmp49A=({struct Cyc_Port_Edit*_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->loc=loc,({struct _fat_ptr _tmp499=({const char*_tmp286="@";_tag_fat(_tmp286,sizeof(char),2U);});_tmp288->old_string=_tmp499;}),({struct _fat_ptr _tmp498=({const char*_tmp287="";_tag_fat(_tmp287,sizeof(char),1U);});_tmp288->new_string=_tmp498;});_tmp288;});_tmp289->hd=_tmp49A;}),_tmp289->tl=edits;_tmp289;});
edits=({struct Cyc_List_List*_tmp28D=_cycalloc(sizeof(*_tmp28D));({struct Cyc_Port_Edit*_tmp49D=({struct Cyc_Port_Edit*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->loc=((struct Cyc_Absyn_Exp*)_check_null(varusage->init))->loc,({struct _fat_ptr _tmp49C=({const char*_tmp28A="new ";_tag_fat(_tmp28A,sizeof(char),5U);});_tmp28C->old_string=_tmp49C;}),({struct _fat_ptr _tmp49B=({const char*_tmp28B="";_tag_fat(_tmp28B,sizeof(char),1U);});_tmp28C->new_string=_tmp49B;});_tmp28C;});_tmp28D->hd=_tmp49D;}),_tmp28D->tl=edits;_tmp28D;});}else{
# 1919
edits=({struct Cyc_List_List*_tmp293=_cycalloc(sizeof(*_tmp293));({struct Cyc_Port_Edit*_tmp4A1=({struct Cyc_Port_Edit*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->loc=loc,({struct _fat_ptr _tmp4A0=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp311;_tmp311.tag=0,_tmp311.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp311;});struct Cyc_String_pa_PrintArg_struct _tmp291=({struct Cyc_String_pa_PrintArg_struct _tmp310;_tmp310.tag=0,({struct _fat_ptr _tmp49E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(vartype));_tmp310.f1=_tmp49E;});_tmp310;});void*_tmp28E[2];_tmp28E[0]=& _tmp290,_tmp28E[1]=& _tmp291;({struct _fat_ptr _tmp49F=({const char*_tmp28F="?%s = malloc(sizeof(%s))";_tag_fat(_tmp28F,sizeof(char),25U);});Cyc_aprintf(_tmp49F,_tag_fat(_tmp28E,sizeof(void*),2));});});_tmp292->old_string=_tmp4A0;}),_tmp292->new_string=*x;_tmp292;});_tmp293->hd=_tmp4A1;}),_tmp293->tl=edits;_tmp293;});}{
# 1921
struct Cyc_List_List*_tmp294=varusage->usage_locs;struct Cyc_List_List*loclist=_tmp294;
for(1;(unsigned)loclist;loclist=loclist->tl){
edits=({struct Cyc_List_List*_tmp299=_cycalloc(sizeof(*_tmp299));({struct Cyc_Port_Edit*_tmp4A4=({struct Cyc_Port_Edit*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->loc=(unsigned)loclist->hd,({struct _fat_ptr _tmp4A3=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp297=({struct Cyc_String_pa_PrintArg_struct _tmp312;_tmp312.tag=0,_tmp312.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_tmp312;});void*_tmp295[1];_tmp295[0]=& _tmp297;({struct _fat_ptr _tmp4A2=({const char*_tmp296="(*%s)";_tag_fat(_tmp296,sizeof(char),6U);});Cyc_aprintf(_tmp4A2,_tag_fat(_tmp295,sizeof(void*),1));});});_tmp298->old_string=_tmp4A3;}),_tmp298->new_string=*x;_tmp298;});_tmp299->hd=_tmp4A4;}),_tmp299->tl=edits;_tmp299;});}}}}}}
# 1929
{struct Cyc_List_List*_tmp29A=ptrs;struct Cyc_List_List*ps=_tmp29A;for(0;(unsigned)ps;ps=ps->tl){
struct _tuple17 _tmp29B=*((struct _tuple17*)ps->hd);struct _tuple17 _stmttmp2F=_tmp29B;struct _tuple17 _tmp29C=_stmttmp2F;unsigned _tmp29F;void*_tmp29E;void*_tmp29D;_tmp29D=_tmp29C.f1;_tmp29E=_tmp29C.f2;_tmp29F=_tmp29C.f3;{void*new_ctype=_tmp29D;void*orig_ctype=_tmp29E;unsigned loc=_tmp29F;
Cyc_Port_unifies(new_ctype,orig_ctype);}}}
# 1933
{struct Cyc_List_List*_tmp2A0=consts;struct Cyc_List_List*cs=_tmp2A0;for(0;(unsigned)cs;cs=cs->tl){
struct _tuple17 _tmp2A1=*((struct _tuple17*)cs->hd);struct _tuple17 _stmttmp30=_tmp2A1;struct _tuple17 _tmp2A2=_stmttmp30;unsigned _tmp2A5;void*_tmp2A4;void*_tmp2A3;_tmp2A3=_tmp2A2.f1;_tmp2A4=_tmp2A2.f2;_tmp2A5=_tmp2A2.f3;{void*new_qual=_tmp2A3;void*old_qual=_tmp2A4;unsigned loc=_tmp2A5;
Cyc_Port_unifies(new_qual,old_qual);}}}
# 1937
{struct Cyc_List_List*_tmp2A6=zts;struct Cyc_List_List*zs=_tmp2A6;for(0;(unsigned)zs;zs=zs->tl){
struct _tuple17 _tmp2A7=*((struct _tuple17*)zs->hd);struct _tuple17 _stmttmp31=_tmp2A7;struct _tuple17 _tmp2A8=_stmttmp31;unsigned _tmp2AB;void*_tmp2AA;void*_tmp2A9;_tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;_tmp2AB=_tmp2A8.f3;{void*new_zt=_tmp2A9;void*old_zt=_tmp2AA;unsigned loc=_tmp2AB;
Cyc_Port_unifies(new_zt,old_zt);}}}
# 1943
for(1;(unsigned)ptrs;ptrs=ptrs->tl){
struct _tuple17 _tmp2AC=*((struct _tuple17*)ptrs->hd);struct _tuple17 _stmttmp32=_tmp2AC;struct _tuple17 _tmp2AD=_stmttmp32;unsigned _tmp2B0;void*_tmp2AF;void*_tmp2AE;_tmp2AE=_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;_tmp2B0=_tmp2AD.f3;{void*new_ctype=_tmp2AE;void*orig_ctype=_tmp2AF;unsigned loc=_tmp2B0;
if(!Cyc_Port_unifies(new_ctype,orig_ctype)&&(int)loc){
void*_tmp2B1=Cyc_Port_compress_ct(orig_ctype);void*_stmttmp33=_tmp2B1;void*_tmp2B2=_stmttmp33;switch(*((int*)_tmp2B2)){case 2:
# 1948
 edits=({struct Cyc_List_List*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));({struct Cyc_Port_Edit*_tmp4A7=({struct Cyc_Port_Edit*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->loc=loc,({struct _fat_ptr _tmp4A6=({const char*_tmp2B3="?";_tag_fat(_tmp2B3,sizeof(char),2U);});_tmp2B5->old_string=_tmp4A6;}),({struct _fat_ptr _tmp4A5=({const char*_tmp2B4="*";_tag_fat(_tmp2B4,sizeof(char),2U);});_tmp2B5->new_string=_tmp4A5;});_tmp2B5;});_tmp2B6->hd=_tmp4A7;}),_tmp2B6->tl=edits;_tmp2B6;});
goto _LL12;case 3:
# 1951
 edits=({struct Cyc_List_List*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));({struct Cyc_Port_Edit*_tmp4AA=({struct Cyc_Port_Edit*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->loc=loc,({struct _fat_ptr _tmp4A9=({const char*_tmp2B7="*";_tag_fat(_tmp2B7,sizeof(char),2U);});_tmp2B9->old_string=_tmp4A9;}),({struct _fat_ptr _tmp4A8=({const char*_tmp2B8="?";_tag_fat(_tmp2B8,sizeof(char),2U);});_tmp2B9->new_string=_tmp4A8;});_tmp2B9;});_tmp2BA->hd=_tmp4AA;}),_tmp2BA->tl=edits;_tmp2BA;});
goto _LL12;default:
 goto _LL12;}_LL12:;}}}
# 1958
for(1;(unsigned)consts;consts=consts->tl){
struct _tuple17 _tmp2BB=*((struct _tuple17*)consts->hd);struct _tuple17 _stmttmp34=_tmp2BB;struct _tuple17 _tmp2BC=_stmttmp34;unsigned _tmp2BF;void*_tmp2BE;void*_tmp2BD;_tmp2BD=_tmp2BC.f1;_tmp2BE=_tmp2BC.f2;_tmp2BF=_tmp2BC.f3;{void*new_qual=_tmp2BD;void*old_qual=_tmp2BE;unsigned loc=_tmp2BF;
if(!Cyc_Port_unifies(new_qual,old_qual)&&(int)loc){
void*_tmp2C0=Cyc_Port_compress_ct(old_qual);void*_stmttmp35=_tmp2C0;void*_tmp2C1=_stmttmp35;switch(*((int*)_tmp2C1)){case 1:
# 1963
 edits=({struct Cyc_List_List*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));({struct Cyc_Port_Edit*_tmp4AD=({struct Cyc_Port_Edit*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4->loc=loc,({struct _fat_ptr _tmp4AC=({const char*_tmp2C2="const ";_tag_fat(_tmp2C2,sizeof(char),7U);});_tmp2C4->old_string=_tmp4AC;}),({struct _fat_ptr _tmp4AB=({const char*_tmp2C3="";_tag_fat(_tmp2C3,sizeof(char),1U);});_tmp2C4->new_string=_tmp4AB;});_tmp2C4;});_tmp2C5->hd=_tmp4AD;}),_tmp2C5->tl=edits;_tmp2C5;});goto _LL1C;case 0:
# 1965
 edits=({struct Cyc_List_List*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));({struct Cyc_Port_Edit*_tmp4B0=({struct Cyc_Port_Edit*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->loc=loc,({struct _fat_ptr _tmp4AF=({const char*_tmp2C6="";_tag_fat(_tmp2C6,sizeof(char),1U);});_tmp2C8->old_string=_tmp4AF;}),({struct _fat_ptr _tmp4AE=({const char*_tmp2C7="const ";_tag_fat(_tmp2C7,sizeof(char),7U);});_tmp2C8->new_string=_tmp4AE;});_tmp2C8;});_tmp2C9->hd=_tmp4B0;}),_tmp2C9->tl=edits;_tmp2C9;});goto _LL1C;default:
 goto _LL1C;}_LL1C:;}}}
# 1971
for(1;(unsigned)zts;zts=zts->tl){
struct _tuple17 _tmp2CA=*((struct _tuple17*)zts->hd);struct _tuple17 _stmttmp36=_tmp2CA;struct _tuple17 _tmp2CB=_stmttmp36;unsigned _tmp2CE;void*_tmp2CD;void*_tmp2CC;_tmp2CC=_tmp2CB.f1;_tmp2CD=_tmp2CB.f2;_tmp2CE=_tmp2CB.f3;{void*new_zt=_tmp2CC;void*old_zt=_tmp2CD;unsigned loc=_tmp2CE;
if(!Cyc_Port_unifies(new_zt,old_zt)&&(int)loc){
void*_tmp2CF=Cyc_Port_compress_ct(old_zt);void*_stmttmp37=_tmp2CF;void*_tmp2D0=_stmttmp37;switch(*((int*)_tmp2D0)){case 8:
# 1976
 edits=({struct Cyc_List_List*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));({struct Cyc_Port_Edit*_tmp4B3=({struct Cyc_Port_Edit*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->loc=loc,({struct _fat_ptr _tmp4B2=({const char*_tmp2D1="@nozeroterm ";_tag_fat(_tmp2D1,sizeof(char),13U);});_tmp2D3->old_string=_tmp4B2;}),({struct _fat_ptr _tmp4B1=({const char*_tmp2D2="";_tag_fat(_tmp2D2,sizeof(char),1U);});_tmp2D3->new_string=_tmp4B1;});_tmp2D3;});_tmp2D4->hd=_tmp4B3;}),_tmp2D4->tl=edits;_tmp2D4;});goto _LL26;case 9:
# 1978
 edits=({struct Cyc_List_List*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));({struct Cyc_Port_Edit*_tmp4B6=({struct Cyc_Port_Edit*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->loc=loc,({struct _fat_ptr _tmp4B5=({const char*_tmp2D5="@zeroterm ";_tag_fat(_tmp2D5,sizeof(char),11U);});_tmp2D7->old_string=_tmp4B5;}),({struct _fat_ptr _tmp4B4=({const char*_tmp2D6="";_tag_fat(_tmp2D6,sizeof(char),1U);});_tmp2D7->new_string=_tmp4B4;});_tmp2D7;});_tmp2D8->hd=_tmp4B6;}),_tmp2D8->tl=edits;_tmp2D8;});goto _LL26;default:
 goto _LL26;}_LL26:;}}}
# 1985
edits=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*))Cyc_List_merge_sort;})(Cyc_Port_cmp_edit,edits);
# 1987
while((unsigned)edits &&((struct Cyc_Port_Edit*)edits->hd)->loc == 0U){
# 1991
edits=edits->tl;}
# 1993
return edits;}
# 1998
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp2D9=Cyc_Port_gen_edits(ds);struct Cyc_List_List*edits=_tmp2D9;
struct Cyc_List_List*_tmp2DA=({(struct Cyc_List_List*(*)(unsigned(*)(struct Cyc_Port_Edit*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Port_get_seg,edits);struct Cyc_List_List*locs=_tmp2DA;
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp2DB=({struct Cyc_List_List*(*_tmp4B7)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp4B7(Cyc_Position_strings_of_segments(locs));});struct Cyc_List_List*slocs=_tmp2DB;
for(1;(unsigned)edits;(edits=edits->tl,slocs=slocs->tl)){
struct Cyc_Port_Edit _tmp2DC=*((struct Cyc_Port_Edit*)edits->hd);struct Cyc_Port_Edit _stmttmp38=_tmp2DC;struct Cyc_Port_Edit _tmp2DD=_stmttmp38;struct _fat_ptr _tmp2E0;struct _fat_ptr _tmp2DF;unsigned _tmp2DE;_tmp2DE=_tmp2DD.loc;_tmp2DF=_tmp2DD.old_string;_tmp2E0=_tmp2DD.new_string;{unsigned loc=_tmp2DE;struct _fat_ptr s1=_tmp2DF;struct _fat_ptr s2=_tmp2E0;
struct _fat_ptr sloc=(struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(slocs))->hd);
({struct Cyc_String_pa_PrintArg_struct _tmp2E3=({struct Cyc_String_pa_PrintArg_struct _tmp315;_tmp315.tag=0,_tmp315.f1=(struct _fat_ptr)((struct _fat_ptr)sloc);_tmp315;});struct Cyc_String_pa_PrintArg_struct _tmp2E4=({struct Cyc_String_pa_PrintArg_struct _tmp314;_tmp314.tag=0,_tmp314.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp314;});struct Cyc_String_pa_PrintArg_struct _tmp2E5=({struct Cyc_String_pa_PrintArg_struct _tmp313;_tmp313.tag=0,_tmp313.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp313;});void*_tmp2E1[3];_tmp2E1[0]=& _tmp2E3,_tmp2E1[1]=& _tmp2E4,_tmp2E1[2]=& _tmp2E5;({struct _fat_ptr _tmp4B8=({const char*_tmp2E2="%s: insert `%s' for `%s'\n";_tag_fat(_tmp2E2,sizeof(char),26U);});Cyc_printf(_tmp4B8,_tag_fat(_tmp2E1,sizeof(void*),3));});});}}}}

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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 111
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 691 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 911
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 915
extern void*Cyc_Absyn_false_type;
# 917
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_rgn_handle_type(void*);
# 934
void*Cyc_Absyn_exn_type();
# 958
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 970
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 975
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 978
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1046
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1059
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1068
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1073
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1077
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple2*,void*,struct Cyc_Absyn_Exp*init);
# 1127
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "unify.h"
void Cyc_Unify_explain_failure();
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 36
int Cyc_Tcutil_is_void_type(void*);
# 38
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 107
void*Cyc_Tcutil_compress(void*);
# 113
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 147
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 174
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 202
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 206
struct _tuple12*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 241
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 246
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 258
int Cyc_Tcutil_new_tvar_id();
# 269
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 276
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 280
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 290
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 88
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 96
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 100
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 102
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 106
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 124
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple13{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 127
const struct _tuple13*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 137
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 140
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 151
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
# 35 "tctyp.h"
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 107
void Cyc_Tcpat_check_switch_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 109
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 111
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 51 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp3;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3;
({void*_tmp164=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=5U,_tmp4->f1=e1,_tmp4->f2=Cyc_Absyn_PreInc;_tmp4;});e->r=_tmp164;});goto _LL0;}case Cyc_Absyn_PostDec: _LL3: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2;
({void*_tmp165=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=5U,_tmp5->f1=e1,_tmp5->f2=Cyc_Absyn_PreDec;_tmp5;});e->r=_tmp165;});goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 60
static int Cyc_Tcstmt_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp6=e->r;void*_stmttmp1=_tmp6;void*_tmp7=_stmttmp1;struct Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmp9;switch(*((int*)_tmp7)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL4: {struct Cyc_Absyn_Exp*e2=_tmp9;
_tmp8=e2;goto _LL6;}case 13U: _LL5: _tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL6: {struct Cyc_Absyn_Exp*e2=_tmp8;
e=e2;continue;}default: _LL7: _LL8:
# 67
 return 0;}_LL0:;}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 77
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmpA=e->r;void*_stmttmp2=_tmpA;void*_tmpB=_stmttmp2;struct Cyc_List_List*_tmpC;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_Absyn_Exp*_tmp2E;switch(*((int*)_tmpB)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 86
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp2E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp30=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp2E;struct Cyc_Absyn_Exp*e2=_tmp2F;struct Cyc_Absyn_Exp*e3=_tmp30;
# 96
if(!Cyc_Tcstmt_noassign_exp(e1))return 0;
_tmp2C=e2;_tmp2D=e3;goto _LL20;}case 27U: _LL1F: _tmp2C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp2D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp2C;struct Cyc_Absyn_Exp*e2=_tmp2D;
_tmp2A=e1;_tmp2B=e2;goto _LL22;}case 7U: _LL21: _tmp2A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp2A;struct Cyc_Absyn_Exp*e2=_tmp2B;
_tmp28=e1;_tmp29=e2;goto _LL24;}case 8U: _LL23: _tmp28=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp29=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp28;struct Cyc_Absyn_Exp*e2=_tmp29;
_tmp26=e1;_tmp27=e2;goto _LL26;}case 23U: _LL25: _tmp26=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp27=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp26;struct Cyc_Absyn_Exp*e2=_tmp27;
_tmp24=e1;_tmp25=e2;goto _LL28;}case 9U: _LL27: _tmp24=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp25=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;
return Cyc_Tcstmt_noassign_exp(e1)&& Cyc_Tcstmt_noassign_exp(e2);}case 41U: _LL29: _tmp23=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp23;
# 104
_tmp22=e;goto _LL2C;}case 38U: _LL2B: _tmp22=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp22;
_tmp21=e;goto _LL2E;}case 11U: _LL2D: _tmp21=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp21;
_tmp20=e;goto _LL30;}case 12U: _LL2F: _tmp20=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp20;
_tmp1F=e;goto _LL32;}case 13U: _LL31: _tmp1F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp1F;
_tmp1E=e;goto _LL34;}case 14U: _LL33: _tmp1E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmp1E;
_tmp1D=e;goto _LL36;}case 18U: _LL35: _tmp1D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp1D;
_tmp1C=e;goto _LL38;}case 20U: _LL37: _tmp1C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp1C;
_tmp1B=e;goto _LL3A;}case 21U: _LL39: _tmp1B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmp1B;
_tmp1A=e;goto _LL3C;}case 22U: _LL3B: _tmp1A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp1A;
_tmp19=e;goto _LL3E;}case 28U: _LL3D: _tmp19=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmp19;
_tmp18=e;goto _LL40;}case 15U: _LL3F: _tmp18=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmp18;
return Cyc_Tcstmt_noassign_exp(e);}case 34U: _LL41: _tmp16=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB)->f1).rgn;_tmp17=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB)->f1).num_elts;_LL42: {struct Cyc_Absyn_Exp*eopt=_tmp16;struct Cyc_Absyn_Exp*e=_tmp17;
# 118
_tmp14=eopt;_tmp15=e;goto _LL44;}case 16U: _LL43: _tmp14=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp15=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp14;struct Cyc_Absyn_Exp*e=_tmp15;
# 120
if(!Cyc_Tcstmt_noassign_exp(e))return 0;
if(eopt != 0)return Cyc_Tcstmt_noassign_exp(eopt);else{
return 1;}}case 3U: _LL45: _tmp13=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL46: {struct Cyc_List_List*es=_tmp13;
# 124
_tmp12=es;goto _LL48;}case 31U: _LL47: _tmp12=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL48: {struct Cyc_List_List*es=_tmp12;
_tmp11=es;goto _LL4A;}case 24U: _LL49: _tmp11=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL4A: {struct Cyc_List_List*es=_tmp11;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,es);}case 36U: _LL4B: _tmp10=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL4C: {struct Cyc_List_List*dles=_tmp10;
# 128
_tmpF=dles;goto _LL4E;}case 29U: _LL4D: _tmpF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL4E: {struct Cyc_List_List*dles=_tmpF;
_tmpE=dles;goto _LL50;}case 30U: _LL4F: _tmpE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL50: {struct Cyc_List_List*dles=_tmpE;
_tmpD=dles;goto _LL52;}case 26U: _LL51: _tmpD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL52: {struct Cyc_List_List*dles=_tmpD;
_tmpC=dles;goto _LL54;}default: _LL53: _tmpC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL54: {struct Cyc_List_List*dles=_tmpC;
# 133
for(0;dles != 0;dles=dles->tl){
struct _tuple15*_tmp31=(struct _tuple15*)dles->hd;struct _tuple15*_stmttmp3=_tmp31;struct _tuple15*_tmp32=_stmttmp3;struct Cyc_Absyn_Exp*_tmp33;_LL56: _tmp33=_tmp32->f2;_LL57: {struct Cyc_Absyn_Exp*e=_tmp33;
if(!Cyc_Tcstmt_noassign_exp(e))return 0;}}
# 137
return 1;}}_LL0:;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 142
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 144
struct Cyc_Tcpat_TcPatResult _tmp34=pat_res;struct Cyc_List_List*_tmp36;struct _tuple1*_tmp35;_LL1: _tmp35=_tmp34.tvars_and_bounds_opt;_tmp36=_tmp34.patvars;_LL2: {struct _tuple1*p=_tmp35;struct Cyc_List_List*vse=_tmp36;
struct Cyc_List_List*_tmp37=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple17*))Cyc_Core_fst,vse);struct Cyc_List_List*vs=_tmp37;
struct Cyc_List_List*_tmp38=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*p).f1);struct Cyc_List_List*tvs=_tmp38;
struct Cyc_List_List*_tmp39=p == 0?0:(*p).f2;struct Cyc_List_List*bds=_tmp39;
struct Cyc_List_List*_tmp3A=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple16*))Cyc_Core_snd,(*p).f1));struct Cyc_List_List*rgns=_tmp3A;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(0;rgns != 0;rgns=rgns->tl){
te2=({struct Cyc_Tcenv_Tenv*_tmp166=te2;Cyc_Tcenv_add_region(_tmp166,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->tag=2U,_tmp3B->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_tmp3B;}),0);});}
te2=Cyc_Tcenv_new_outlives_constraints(te2,bds,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(loc,te2);{
void*_tmp3C=Cyc_Tcenv_curr_rgn(te2);void*ropt=_tmp3C;
{struct Cyc_List_List*_tmp3D=vs;struct Cyc_List_List*vs2=_tmp3D;for(0;vs2 != 0;vs2=vs2->tl){
if((struct Cyc_Absyn_Vardecl**)vs2->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)vs2->hd)))->rgn=ropt;}}
# 160
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp168=te2;struct Cyc_Absyn_Exp*_tmp167=where_opt;Cyc_Tcexp_tcTest(_tmp168,_tmp167,({const char*_tmp3E="switch clause guard";_tag_fat(_tmp3E,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp3F=0U;({unsigned _tmp16A=where_opt->loc;struct _fat_ptr _tmp169=({const char*_tmp40="cannot show &&-clause in pattern has no effects";_tag_fat(_tmp40,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp16A,_tmp169,_tag_fat(_tmp3F,sizeof(void*),0U));});});}
# 165
Cyc_Tcstmt_tcStmt(te2,s,0);}}}
# 168
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp41=Cyc_Tcstmt_stmt_temp_var_counter ++;int i=_tmp41;
struct _tuple2*res=({struct _tuple2*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp16D=({struct _fat_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));({struct _fat_ptr _tmp16C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp44=({struct Cyc_Int_pa_PrintArg_struct _tmp149;_tmp149.tag=1U,_tmp149.f1=(unsigned)i;_tmp149;});void*_tmp42[1U];_tmp42[0]=& _tmp44;({struct _fat_ptr _tmp16B=({const char*_tmp43="_stmttmp%X";_tag_fat(_tmp43,sizeof(char),11U);});Cyc_aprintf(_tmp16B,_tag_fat(_tmp42,sizeof(void*),1U));});});*_tmp45=_tmp16C;});_tmp45;});_tmp46->f2=_tmp16D;});_tmp46;});
return res;}
# 182 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 184
void*_tmp47=s0->r;void*_stmttmp4=_tmp47;void*_tmp48=_stmttmp4;struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Decl*_tmp49;void**_tmp4D;struct Cyc_List_List**_tmp4C;struct Cyc_Absyn_Stmt*_tmp4B;void**_tmp50;struct Cyc_List_List*_tmp4F;struct Cyc_Absyn_Exp**_tmp4E;struct Cyc_Absyn_Stmt*_tmp52;struct _fat_ptr*_tmp51;struct Cyc_Absyn_Switch_clause***_tmp54;struct Cyc_List_List*_tmp53;struct Cyc_Absyn_Stmt*_tmp57;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Stmt*_tmp55;struct Cyc_Absyn_Stmt*_tmp5D;struct Cyc_Absyn_Stmt*_tmp5C;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Stmt*_tmp5A;struct Cyc_Absyn_Exp*_tmp59;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Stmt*_tmp60;struct Cyc_Absyn_Stmt*_tmp5F;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_Absyn_Stmt*_tmp63;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Stmt*_tmp66;struct Cyc_Absyn_Stmt*_tmp65;struct Cyc_Absyn_Exp*_tmp67;switch(*((int*)_tmp48)){case 0U: _LL1: _LL2:
# 186
 return;case 1U: _LL3: _tmp67=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp67;
# 189
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2U: _LL5: _tmp65=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp66=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp65;struct Cyc_Absyn_Stmt*s2=_tmp66;
# 195
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3U: _LL7: _tmp64=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp64;
# 200
void*t=Cyc_Tcenv_return_typ(te);
if(eopt == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_String_pa_PrintArg_struct _tmp6A=({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U,({struct _fat_ptr _tmp16E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp14A.f1=_tmp16E;});_tmp14A;});void*_tmp68[1U];_tmp68[0]=& _tmp6A;({unsigned _tmp170=s0->loc;struct _fat_ptr _tmp16F=({const char*_tmp69="should return a value of type %s";_tag_fat(_tmp69,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp170,_tmp16F,_tag_fat(_tmp68,sizeof(void*),1U));});});else{
# 206
({struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0U,({struct _fat_ptr _tmp171=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp14B.f1=_tmp171;});_tmp14B;});void*_tmp6B[1U];_tmp6B[0]=& _tmp6D;({unsigned _tmp173=s0->loc;struct _fat_ptr _tmp172=({const char*_tmp6C="must return a value of type %s";_tag_fat(_tmp6C,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp173,_tmp172,_tag_fat(_tmp6B,sizeof(void*),1U));});});}}}else{
# 210
int bogus=0;
struct Cyc_Absyn_Exp*e=eopt;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp175=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp174=e;Cyc_Tcutil_coerce_arg(_tmp175,_tmp174,t,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0U,({
struct _fat_ptr _tmp176=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp14D.f1=_tmp176;});_tmp14D;});struct Cyc_String_pa_PrintArg_struct _tmp71=({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0U,({struct _fat_ptr _tmp177=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp14C.f1=_tmp177;});_tmp14C;});void*_tmp6E[2U];_tmp6E[0]=& _tmp70,_tmp6E[1]=& _tmp71;({unsigned _tmp179=s0->loc;struct _fat_ptr _tmp178=({const char*_tmp6F="returns value of type %s but requires %s";_tag_fat(_tmp6F,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp179,_tmp178,_tag_fat(_tmp6E,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 218
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp72=0U;({unsigned _tmp17B=e->loc;struct _fat_ptr _tmp17A=({const char*_tmp73="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp73,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp17B,_tmp17A,_tag_fat(_tmp72,sizeof(void*),0U));});});}
# 221
return;}case 4U: _LL9: _tmp61=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp62=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_tmp63=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp48)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp61;struct Cyc_Absyn_Stmt*s1=_tmp62;struct Cyc_Absyn_Stmt*s2=_tmp63;
# 224
({struct Cyc_Tcenv_Tenv*_tmp17D=te;struct Cyc_Absyn_Exp*_tmp17C=e;Cyc_Tcexp_tcTest(_tmp17D,_tmp17C,({const char*_tmp74="if statement";_tag_fat(_tmp74,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5U: _LLB: _tmp5E=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp48)->f1).f1;_tmp5F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp48)->f1).f2;_tmp60=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5E;struct Cyc_Absyn_Stmt*cont_s=_tmp5F;struct Cyc_Absyn_Stmt*s=_tmp60;
# 230
({struct Cyc_Tcenv_Tenv*_tmp17F=te;struct Cyc_Absyn_Exp*_tmp17E=e;Cyc_Tcexp_tcTest(_tmp17F,_tmp17E,({const char*_tmp75="while loop";_tag_fat(_tmp75,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9U: _LLD: _tmp58=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp59=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f1;_tmp5A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f2;_tmp5B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f3).f1;_tmp5C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f3).f2;_tmp5D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp58;struct Cyc_Absyn_Exp*e2=_tmp59;struct Cyc_Absyn_Stmt*guard_s=_tmp5A;struct Cyc_Absyn_Exp*e3=_tmp5B;struct Cyc_Absyn_Stmt*cont_s=_tmp5C;struct Cyc_Absyn_Stmt*s=_tmp5D;
# 235
Cyc_Tcexp_tcExp(te,0,e1);
({struct Cyc_Tcenv_Tenv*_tmp181=te;struct Cyc_Absyn_Exp*_tmp180=e2;Cyc_Tcexp_tcTest(_tmp181,_tmp180,({const char*_tmp76="for loop";_tag_fat(_tmp76,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14U: _LLF: _tmp55=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp56=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f1;_tmp57=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f2;_LL10: {struct Cyc_Absyn_Stmt*s=_tmp55;struct Cyc_Absyn_Exp*e=_tmp56;struct Cyc_Absyn_Stmt*cont_s=_tmp57;
# 243
Cyc_Tcstmt_tcStmt(te,s,1);
({struct Cyc_Tcenv_Tenv*_tmp183=te;struct Cyc_Absyn_Exp*_tmp182=e;Cyc_Tcexp_tcTest(_tmp183,_tmp182,({const char*_tmp77="do loop";_tag_fat(_tmp77,sizeof(char),8U);}));});
return;}case 6U: _LL11: _LL12:
# 247
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp53=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp54=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL18: {struct Cyc_List_List*es=_tmp53;struct Cyc_Absyn_Switch_clause***clauseopt=_tmp54;
# 252
const struct _tuple13*_tmp78=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);const struct _tuple13*trip_opt=_tmp78;
if(trip_opt == (const struct _tuple13*)0){
({void*_tmp79=0U;({unsigned _tmp185=s0->loc;struct _fat_ptr _tmp184=({const char*_tmp7A="fallthru not in a non-last case";_tag_fat(_tmp7A,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp185,_tmp184,_tag_fat(_tmp79,sizeof(void*),0U));});});
return;}{
# 257
struct Cyc_List_List*_tmp7B=(*trip_opt).f2;struct Cyc_List_List*tvs=_tmp7B;
struct Cyc_List_List*_tmp7C=(*trip_opt).f3;struct Cyc_List_List*ts_orig=_tmp7C;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp186=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp186,tvs);});
struct Cyc_List_List*_tmp7D=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,ts_orig);struct Cyc_List_List*ts=_tmp7D;
for(0;ts != 0 && es != 0;(ts=ts->tl,es=es->tl)){
# 264
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp188=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp187=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp188,_tmp187,(void*)ts->hd,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0U,({
# 269
struct _fat_ptr _tmp189=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt)));_tmp14F.f1=_tmp189;});_tmp14F;});struct Cyc_String_pa_PrintArg_struct _tmp81=({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0U,({struct _fat_ptr _tmp18A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)ts->hd));_tmp14E.f1=_tmp18A;});_tmp14E;});void*_tmp7E[2U];_tmp7E[0]=& _tmp80,_tmp7E[1]=& _tmp81;({unsigned _tmp18C=s0->loc;struct _fat_ptr _tmp18B=({const char*_tmp7F="fallthru argument has type %s but pattern variable has type %s";_tag_fat(_tmp7F,sizeof(char),63U);});Cyc_Tcutil_terr(_tmp18C,_tmp18B,_tag_fat(_tmp7E,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 273
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)ts->hd)&& !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)es->hd))
({void*_tmp82=0U;({unsigned _tmp18E=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _fat_ptr _tmp18D=({const char*_tmp83="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp83,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp18E,_tmp18D,_tag_fat(_tmp82,sizeof(void*),0U));});});}
# 276
if(es != 0)
({void*_tmp84=0U;({unsigned _tmp190=s0->loc;struct _fat_ptr _tmp18F=({const char*_tmp85="too many arguments in explicit fallthru";_tag_fat(_tmp85,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp190,_tmp18F,_tag_fat(_tmp84,sizeof(void*),0U));});});
if(ts != 0)
({void*_tmp86=0U;({unsigned _tmp192=s0->loc;struct _fat_ptr _tmp191=({const char*_tmp87="too few arguments in explicit fallthru";_tag_fat(_tmp87,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp192,_tmp191,_tag_fat(_tmp86,sizeof(void*),0U));});});
return;}}case 13U: _LL19: _tmp51=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp52=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL1A: {struct _fat_ptr*l=_tmp51;struct Cyc_Absyn_Stmt*s=_tmp52;
# 286
({struct Cyc_Tcenv_Tenv*_tmp19A=({unsigned _tmp199=s0->loc;struct Cyc_Tcenv_Tenv*_tmp198=te;Cyc_Tcenv_new_named_block(_tmp199,_tmp198,({struct Cyc_Absyn_Tvar*_tmp8C=_cycalloc(sizeof(*_tmp8C));
({struct _fat_ptr*_tmp197=({struct _fat_ptr*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct _fat_ptr _tmp196=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0U,_tmp150.f1=(struct _fat_ptr)((struct _fat_ptr)*l);_tmp150;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({struct _fat_ptr _tmp195=({const char*_tmp89="`%s";_tag_fat(_tmp89,sizeof(char),4U);});Cyc_aprintf(_tmp195,_tag_fat(_tmp88,sizeof(void*),1U));});});*_tmp8B=_tmp196;});_tmp8B;});_tmp8C->name=_tmp197;}),({
int _tmp194=Cyc_Tcutil_new_tvar_id();_tmp8C->identity=_tmp194;}),({
void*_tmp193=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp8C->kind=_tmp193;});_tmp8C;}));});
# 286
Cyc_Tcstmt_tcStmt(_tmp19A,s,0);});
# 290
return;}case 10U: _LL1B: _tmp4E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp4F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_tmp50=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp48)->f3;_LL1C: {struct Cyc_Absyn_Exp**exp=_tmp4E;struct Cyc_List_List*scs0=_tmp4F;void**dtp=_tmp50;
# 295
struct Cyc_Absyn_Exp*_tmp8D=*exp;struct Cyc_Absyn_Exp*e=_tmp8D;
if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmp8E=({struct _tuple2*_tmp19B=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp19B,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp8E;
struct Cyc_Absyn_Stmt*_tmp8F=Cyc_Absyn_new_stmt(s0->r,s0->loc);struct Cyc_Absyn_Stmt*s1=_tmp8F;
struct Cyc_Absyn_Decl*_tmp90=({struct Cyc_Absyn_Pat*_tmp19F=({void*_tmp19D=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=1U,_tmp93->f1=vd,({struct Cyc_Absyn_Pat*_tmp19C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp93->f2=_tmp19C;});_tmp93;});Cyc_Absyn_new_pat(_tmp19D,e->loc);});struct Cyc_Absyn_Exp*_tmp19E=e;Cyc_Absyn_let_decl(_tmp19F,_tmp19E,s0->loc);});struct Cyc_Absyn_Decl*d=_tmp90;
# 302
struct Cyc_Absyn_Stmt*_tmp91=Cyc_Absyn_decl_stmt(d,s1,s0->loc);struct Cyc_Absyn_Stmt*s2=_tmp91;
s0->r=s2->r;
({struct Cyc_Absyn_Exp*_tmp1A1=({void*_tmp1A0=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=4U,_tmp92->f1=vd;_tmp92;});Cyc_Absyn_varb_exp(_tmp1A0,e->loc);});*exp=_tmp1A1;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 309
void*_tmp94=Cyc_Tcexp_tcExp(te,0,e);void*t=_tmp94;
# 311
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp95=0U;({unsigned _tmp1A3=e->loc;struct _fat_ptr _tmp1A2=({const char*_tmp96="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp96,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp1A3,_tmp1A2,_tag_fat(_tmp95,sizeof(void*),0U));});});{
# 316
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp97=scs0;struct Cyc_List_List*scs=_tmp97;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 324
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 326
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp98=_cycalloc(sizeof(*_tmp98));({struct Cyc_Tcpat_TcPatResult _tmp1A4=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& t,e);*_tmp98=_tmp1A4;});_tmp98;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 330
te=Cyc_Tcenv_clear_fallthru(te);else{
# 332
struct Cyc_List_List*_tmp99=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp99;
struct Cyc_List_List*_tmp9A=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 333
struct Cyc_List_List*new_tvs=_tmp9A;
# 335
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 338
te=Cyc_Tcenv_clear_fallthru(te);}{
# 340
struct Cyc_Absyn_Pat*_tmp9B=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp9B;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),t)){
({struct Cyc_String_pa_PrintArg_struct _tmp9E=({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0U,({
struct _fat_ptr _tmp1A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp152.f1=_tmp1A5;});_tmp152;});struct Cyc_String_pa_PrintArg_struct _tmp9F=({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0U,({struct _fat_ptr _tmp1A6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(p->topt)));_tmp151.f1=_tmp1A6;});_tmp151;});void*_tmp9C[2U];_tmp9C[0]=& _tmp9E,_tmp9C[1]=& _tmp9F;({unsigned _tmp1A8=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;struct _fat_ptr _tmp1A7=({const char*_tmp9D="switch on type %s, but case expects type %s";_tag_fat(_tmp9D,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1A8,_tmp1A7,_tag_fat(_tmp9C,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}else{
# 347
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 349
({struct Cyc_Core_Opt*_tmp1A9=({struct Cyc_Core_Opt*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->v=this_pattern.patvars;_tmpA0;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1A9;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 353
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15U: _LL1D: _tmp4B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp4C=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_tmp4D=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp48)->f3;_LL1E: {struct Cyc_Absyn_Stmt*s=_tmp4B;struct Cyc_List_List**scs0=_tmp4C;void**dtp=_tmp4D;
# 361
({void*_tmp1AD=({void*_tmp1AC=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1AB=Cyc_Absyn_new_stmt(s->r,s->loc);_tmpA1->f1=_tmp1AB;}),({struct Cyc_Absyn_Stmt*_tmp1AA=Cyc_Absyn_skip_stmt(s->loc);_tmpA1->f2=_tmp1AA;});_tmpA1;});Cyc_Absyn_new_stmt(_tmp1AC,s->loc);})->r;s->r=_tmp1AD;});
# 363
Cyc_Tcstmt_tcStmt(te,s,1);{
# 366
struct _tuple2*def_v=({struct _tuple2*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp1AF=({struct _fat_ptr*_tmpB7=_cycalloc(sizeof(*_tmpB7));({struct _fat_ptr _tmp1AE=({const char*_tmpB6="exn";_tag_fat(_tmpB6,sizeof(char),4U);});*_tmpB7=_tmp1AE;});_tmpB7;});_tmpB8->f2=_tmp1AF;});_tmpB8;});
struct Cyc_Absyn_Vardecl*_tmpA2=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmpA2;
struct Cyc_Absyn_Pat*_tmpA3=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=1U,_tmpB5->f1=vd,({struct Cyc_Absyn_Pat*_tmp1B0=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmpB5->f2=_tmp1B0;});_tmpB5;}),0U);struct Cyc_Absyn_Pat*def_pat=_tmpA3;
struct Cyc_Absyn_Stmt*_tmpA4=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->tag=11U,({struct Cyc_Absyn_Exp*_tmp1B1=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=4U,_tmpB3->f1=vd;_tmpB3;}),0U);_tmpB4->f1=_tmp1B1;}),_tmpB4->f2=1;_tmpB4;}),0U),0U);
# 369
struct Cyc_Absyn_Stmt*def_stmt=_tmpA4;
# 371
struct Cyc_Absyn_Switch_clause*_tmpA5=({struct Cyc_Absyn_Switch_clause*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->pattern=def_pat,_tmpB2->pat_vars=0,_tmpB2->where_clause=0,_tmpB2->body=def_stmt,_tmpB2->loc=0U;_tmpB2;});struct Cyc_Absyn_Switch_clause*def_clause=_tmpA5;
({struct Cyc_List_List*_tmp1B3=({struct Cyc_List_List*_tmp1B2=*scs0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1B2,({struct Cyc_Absyn_Switch_clause*_tmpA6[1U];_tmpA6[0]=def_clause;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpA6,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*scs0=_tmp1B3;});{
# 378
void*_tmpA7=Cyc_Absyn_exn_type();void*exception_type=_tmpA7;
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmpA8=*scs0;struct Cyc_List_List*scs=_tmpA8;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 387
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 389
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_Tcpat_TcPatResult _tmp1B4=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& exception_type,0);*_tmpA9=_tmp1B4;});_tmpA9;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 393
te=Cyc_Tcenv_clear_fallthru(te);else{
# 395
struct Cyc_List_List*_tmpAA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmpAA;
struct Cyc_List_List*_tmpAB=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 396
struct Cyc_List_List*new_tvs=_tmpAB;
# 398
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 401
te=Cyc_Tcenv_clear_fallthru(te);}{
# 403
struct Cyc_Absyn_Pat*_tmpAC=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmpAC;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),exception_type)){
({struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,({
struct _fat_ptr _tmp1B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(exception_type));_tmp154.f1=_tmp1B5;});_tmp154;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,({struct _fat_ptr _tmp1B6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(p->topt)));_tmp153.f1=_tmp1B6;});_tmp153;});void*_tmpAD[2U];_tmpAD[0]=& _tmpAF,_tmpAD[1]=& _tmpB0;({unsigned _tmp1B8=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;struct _fat_ptr _tmp1B7=({const char*_tmpAE="switch on type %s, but case expects type %s";_tag_fat(_tmpAE,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1B8,_tmp1B7,_tag_fat(_tmpAD,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}else{
# 410
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 412
({struct Cyc_Core_Opt*_tmp1B9=({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->v=this_pattern.patvars;_tmpB1;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1B9;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 416
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}}default: _LL1F: _tmp49=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp4A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp49;struct Cyc_Absyn_Stmt*s=_tmp4A;
# 420
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmpB9=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_tmpB9;
{void*_tmpBA=d->r;void*_stmttmp5=_tmpBA;void*_tmpBB=_stmttmp5;struct Cyc_List_List*_tmpBD;struct _tuple2*_tmpBC;struct Cyc_List_List*_tmpBF;struct _fat_ptr*_tmpBE;struct Cyc_Absyn_Fndecl*_tmpC0;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC2;struct Cyc_Absyn_Tvar*_tmpC1;struct Cyc_List_List*_tmpC4;void**_tmpC8;struct Cyc_Absyn_Exp**_tmpC7;struct Cyc_Core_Opt**_tmpC6;struct Cyc_Absyn_Pat*_tmpC5;struct Cyc_Absyn_Vardecl*_tmpC9;switch(*((int*)_tmpBB)){case 0U: _LL22: _tmpC9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBB)->f1;_LL23: {struct Cyc_Absyn_Vardecl*vd=_tmpC9;
# 424
struct Cyc_Absyn_Vardecl*_tmpCA=vd;struct Cyc_List_List*_tmpD2;void**_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;void*_tmpCF;struct Cyc_Absyn_Tqual _tmpCE;struct _fat_ptr*_tmpCD;union Cyc_Absyn_Nmspace _tmpCC;enum Cyc_Absyn_Scope _tmpCB;_LL45: _tmpCB=_tmpCA->sc;_tmpCC=(_tmpCA->name)->f1;_tmpCD=(_tmpCA->name)->f2;_tmpCE=_tmpCA->tq;_tmpCF=_tmpCA->type;_tmpD0=_tmpCA->initializer;_tmpD1=(void**)& _tmpCA->rgn;_tmpD2=_tmpCA->attributes;_LL46: {enum Cyc_Absyn_Scope sc=_tmpCB;union Cyc_Absyn_Nmspace nsl=_tmpCC;struct _fat_ptr*x=_tmpCD;struct Cyc_Absyn_Tqual tq=_tmpCE;void*t=_tmpCF;struct Cyc_Absyn_Exp*initializer=_tmpD0;void**rgn_ptr=_tmpD1;struct Cyc_List_List*atts=_tmpD2;
void*_tmpD3=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpD3;
int is_local;
{enum Cyc_Absyn_Scope _tmpD4=sc;switch(_tmpD4){case Cyc_Absyn_Static: _LL48: _LL49:
 goto _LL4B;case Cyc_Absyn_Extern: _LL4A: _LL4B:
 goto _LL4D;case Cyc_Absyn_ExternC: _LL4C: _LL4D:
# 431
 vd->escapes=1;
is_local=0;goto _LL47;case Cyc_Absyn_Abstract: _LL4E: _LL4F:
# 434
({void*_tmpD5=0U;({unsigned _tmp1BB=d->loc;struct _fat_ptr _tmp1BA=({const char*_tmpD6="bad abstract scope for local variable";_tag_fat(_tmpD6,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp1BB,_tmp1BA,_tag_fat(_tmpD5,sizeof(void*),0U));});});
goto _LL51;case Cyc_Absyn_Register: _LL50: _LL51:
 goto _LL53;case Cyc_Absyn_Public: _LL52: _LL53:
 goto _LL55;default: _LL54: _LL55:
 is_local=1;goto _LL47;}_LL47:;}
# 441
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
# 443
{void*_tmpD7=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmpD7;void*_tmpD8=_stmttmp6;unsigned _tmpDC;void*_tmpDB;struct Cyc_Absyn_Tqual _tmpDA;void*_tmpD9;switch(*((int*)_tmpD8)){case 5U: _LL57: if(is_local){_LL58:
# 446
 vd->escapes=1;
sc=3U;
is_local=0;
goto _LL56;}else{goto _LL5B;}case 4U: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).num_elts == 0){_LL59: _tmpD9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).elt_type;_tmpDA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).tq;_tmpDB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).zero_term;_tmpDC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).zt_loc;if(vd->initializer != 0){_LL5A: {void*telt=_tmpD9;struct Cyc_Absyn_Tqual tq=_tmpDA;void*zt=_tmpDB;unsigned ztl=_tmpDC;
# 451
{void*_tmpDD=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmp7=_tmpDD;void*_tmpDE=_stmttmp7;struct Cyc_List_List*_tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct _fat_ptr _tmpE3;struct _fat_ptr _tmpE4;switch(*((int*)_tmpDE)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE)->f1).Wstring_c).tag){case 8U: _LL5E: _tmpE4=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE)->f1).String_c).val;_LL5F: {struct _fat_ptr s=_tmpE4;
# 453
t=({void*_tmp1C0=({void*_tmp1BF=telt;struct Cyc_Absyn_Tqual _tmp1BE=tq;struct Cyc_Absyn_Exp*_tmp1BD=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_tmp1BC=zt;Cyc_Absyn_array_type(_tmp1BF,_tmp1BE,_tmp1BD,_tmp1BC,ztl);});vd->type=_tmp1C0;});
goto _LL5D;}case 9U: _LL60: _tmpE3=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE)->f1).Wstring_c).val;_LL61: {struct _fat_ptr s=_tmpE3;
# 456
t=({void*_tmp1C5=({void*_tmp1C4=telt;struct Cyc_Absyn_Tqual _tmp1C3=tq;struct Cyc_Absyn_Exp*_tmp1C2=Cyc_Absyn_uint_exp(1U,0U);void*_tmp1C1=zt;Cyc_Absyn_array_type(_tmp1C4,_tmp1C3,_tmp1C2,_tmp1C1,ztl);});vd->type=_tmp1C5;});
goto _LL5D;}default: goto _LL6A;}case 27U: _LL62: _tmpE2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDE)->f2;_LL63: {struct Cyc_Absyn_Exp*e=_tmpE2;
_tmpE1=e;goto _LL65;}case 28U: _LL64: _tmpE1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDE)->f1;_LL65: {struct Cyc_Absyn_Exp*e=_tmpE1;
# 461
t=({void*_tmp1C6=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_tmp1C6;});
goto _LL5D;}case 36U: _LL66: _tmpE0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDE)->f2;_LL67: {struct Cyc_List_List*es=_tmpE0;
_tmpDF=es;goto _LL69;}case 26U: _LL68: _tmpDF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDE)->f1;_LL69: {struct Cyc_List_List*es=_tmpDF;
# 465
t=({void*_tmp1CB=({void*_tmp1CA=telt;struct Cyc_Absyn_Tqual _tmp1C9=tq;struct Cyc_Absyn_Exp*_tmp1C8=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);void*_tmp1C7=zt;Cyc_Absyn_array_type(_tmp1CA,_tmp1C9,_tmp1C8,_tmp1C7,ztl);});vd->type=_tmp1CB;});
goto _LL5D;}default: _LL6A: _LL6B:
 goto _LL5D;}_LL5D:;}
# 469
goto _LL56;}}else{goto _LL5B;}}else{goto _LL5B;}default: _LL5B: _LL5C:
 goto _LL56;}_LL56:;}{
# 476
struct Cyc_List_List*_tmpE5=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*bound_vars=_tmpE5;
int _tmpE6=!is_local?0: 1;int allow_evars=_tmpE6;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Tcutil_tmk,allow_evars,1,t);
({int _tmp1CC=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp1CC;});{
# 481
struct Cyc_Tcenv_Tenv*_tmpE7=te;struct Cyc_Tcenv_Tenv*te2=_tmpE7;
if((int)sc == (int)3U ||(int)sc == (int)4U)
({void*_tmpE8=0U;({unsigned _tmp1CE=d->loc;struct _fat_ptr _tmp1CD=({const char*_tmpE9="extern declarations are not yet supported within functions";_tag_fat(_tmpE9,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1CE,_tmp1CD,_tag_fat(_tmpE8,sizeof(void*),0U));});});
if(initializer != 0){
Cyc_Tcexp_tcExpInitializer(te2,& t,initializer);
# 487
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({void*_tmpEA=0U;({unsigned _tmp1D0=d->loc;struct _fat_ptr _tmp1CF=({const char*_tmpEB="initializer for static variable needs to be a constant expression";_tag_fat(_tmpEB,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1D0,_tmp1CF,_tag_fat(_tmpEA,sizeof(void*),0U));});});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp1D2=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp1D1=initializer;Cyc_Tcutil_coerce_assign(_tmp1D2,_tmp1D1,t);})){
struct _fat_ptr _tmpEC=*x;struct _fat_ptr varstr=_tmpEC;
struct _fat_ptr _tmpED=Cyc_Absynpp_typ2string(t);struct _fat_ptr tstr=_tmpED;
struct _fat_ptr _tmpEE=Cyc_Absynpp_typ2string((void*)_check_null(initializer->topt));struct _fat_ptr estr=_tmpEE;
if(((_get_fat_size(varstr,sizeof(char))+ _get_fat_size(tstr,sizeof(char)))+ _get_fat_size(estr,sizeof(char)))+ (unsigned)50 < (unsigned)80)
({struct Cyc_String_pa_PrintArg_struct _tmpF1=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U,_tmp157.f1=(struct _fat_ptr)((struct _fat_ptr)varstr);_tmp157;});struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U,_tmp156.f1=(struct _fat_ptr)((struct _fat_ptr)tstr);_tmp156;});struct Cyc_String_pa_PrintArg_struct _tmpF3=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U,_tmp155.f1=(struct _fat_ptr)((struct _fat_ptr)estr);_tmp155;});void*_tmpEF[3U];_tmpEF[0]=& _tmpF1,_tmpEF[1]=& _tmpF2,_tmpEF[2]=& _tmpF3;({unsigned _tmp1D4=d->loc;struct _fat_ptr _tmp1D3=({const char*_tmpF0="%s was declared with type %s but initialized with type %s.";_tag_fat(_tmpF0,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1D4,_tmp1D3,_tag_fat(_tmpEF,sizeof(void*),3U));});});else{
# 496
if((_get_fat_size(varstr,sizeof(char))+ _get_fat_size(tstr,sizeof(char)))+ (unsigned)25 < (unsigned)80 &&
 _get_fat_size(estr,sizeof(char))+ (unsigned)25 < (unsigned)80)
({struct Cyc_String_pa_PrintArg_struct _tmpF6=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0U,_tmp15A.f1=(struct _fat_ptr)((struct _fat_ptr)varstr);_tmp15A;});struct Cyc_String_pa_PrintArg_struct _tmpF7=({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0U,_tmp159.f1=(struct _fat_ptr)((struct _fat_ptr)tstr);_tmp159;});struct Cyc_String_pa_PrintArg_struct _tmpF8=({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0U,_tmp158.f1=(struct _fat_ptr)((struct _fat_ptr)estr);_tmp158;});void*_tmpF4[3U];_tmpF4[0]=& _tmpF6,_tmpF4[1]=& _tmpF7,_tmpF4[2]=& _tmpF8;({unsigned _tmp1D6=d->loc;struct _fat_ptr _tmp1D5=({const char*_tmpF5="%s was declared with type %s\n but initialized with type %s.";_tag_fat(_tmpF5,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp1D6,_tmp1D5,_tag_fat(_tmpF4,sizeof(void*),3U));});});else{
# 501
({struct Cyc_String_pa_PrintArg_struct _tmpFB=({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0U,_tmp15D.f1=(struct _fat_ptr)((struct _fat_ptr)varstr);_tmp15D;});struct Cyc_String_pa_PrintArg_struct _tmpFC=({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0U,_tmp15C.f1=(struct _fat_ptr)((struct _fat_ptr)tstr);_tmp15C;});struct Cyc_String_pa_PrintArg_struct _tmpFD=({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0U,_tmp15B.f1=(struct _fat_ptr)((struct _fat_ptr)estr);_tmp15B;});void*_tmpF9[3U];_tmpF9[0]=& _tmpFB,_tmpF9[1]=& _tmpFC,_tmpF9[2]=& _tmpFD;({unsigned _tmp1D8=d->loc;struct _fat_ptr _tmp1D7=({const char*_tmpFA="%s declared with type \n%s\n but initialized with type \n%s.";_tag_fat(_tmpFA,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp1D8,_tmp1D7,_tag_fat(_tmpF9,sizeof(void*),3U));});});}}
# 503
Cyc_Unify_unify(t,(void*)_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 507
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 2U: _LL24: _tmpC5=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBB)->f1;_tmpC6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBB)->f2;_tmpC7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBB)->f3;_tmpC8=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBB)->f4;_LL25: {struct Cyc_Absyn_Pat*p=_tmpC5;struct Cyc_Core_Opt**vds=_tmpC6;struct Cyc_Absyn_Exp**exp=_tmpC7;void**dtp=_tmpC8;
# 515
struct Cyc_Absyn_Exp*_tmpFE=*exp;struct Cyc_Absyn_Exp*e=_tmpFE;
{void*_tmpFF=p->r;void*_stmttmp8=_tmpFF;void*_tmp100=_stmttmp8;switch(*((int*)_tmp100)){case 1U: _LL6D: _LL6E:
 goto _LL70;case 2U: _LL6F: _LL70:
 goto _LL72;case 15U: _LL71: _LL72:
 goto _LL6C;default: _LL73: _LL74:
# 521
 if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmp101=({struct _tuple2*_tmp1D9=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1D9,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp101;
struct Cyc_Absyn_Stmt*_tmp102=Cyc_Absyn_new_stmt(s0->r,s0->loc);struct Cyc_Absyn_Stmt*s1=_tmp102;
struct Cyc_Absyn_Decl*_tmp103=({struct Cyc_Absyn_Pat*_tmp1DD=({void*_tmp1DB=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=1U,_tmp106->f1=vd,({struct Cyc_Absyn_Pat*_tmp1DA=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp106->f2=_tmp1DA;});_tmp106;});Cyc_Absyn_new_pat(_tmp1DB,e->loc);});struct Cyc_Absyn_Exp*_tmp1DC=e;Cyc_Absyn_let_decl(_tmp1DD,_tmp1DC,s0->loc);});struct Cyc_Absyn_Decl*d=_tmp103;
# 527
struct Cyc_Absyn_Stmt*_tmp104=Cyc_Absyn_decl_stmt(d,s1,s0->loc);struct Cyc_Absyn_Stmt*s2=_tmp104;
# 529
s0->r=s2->r;
({struct Cyc_Absyn_Exp*_tmp1DF=({void*_tmp1DE=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=4U,_tmp105->f1=vd;_tmp105;});Cyc_Absyn_varb_exp(_tmp1DE,e->loc);});*exp=_tmp1DF;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL6C:;}
# 535
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 537
void*pat_type=(void*)_check_null(e->topt);
# 539
struct Cyc_Tcpat_TcPatResult _tmp107=Cyc_Tcpat_tcPat(te,p,& pat_type,e);struct Cyc_Tcpat_TcPatResult pat_res=_tmp107;
({struct Cyc_Core_Opt*_tmp1E0=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->v=pat_res.patvars;_tmp108;});*vds=_tmp1E0;});
if(!({void*_tmp1E3=(void*)_check_null(p->topt);Cyc_Unify_unify(_tmp1E3,(void*)_check_null(e->topt));})&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1E2=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1E1=e;Cyc_Tcutil_coerce_assign(_tmp1E2,_tmp1E1,(void*)_check_null(p->topt));})){
({struct Cyc_String_pa_PrintArg_struct _tmp10B=({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0U,({
struct _fat_ptr _tmp1E4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(p->topt)));_tmp15F.f1=_tmp1E4;});_tmp15F;});struct Cyc_String_pa_PrintArg_struct _tmp10C=({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0U,({struct _fat_ptr _tmp1E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp15E.f1=_tmp1E5;});_tmp15E;});void*_tmp109[2U];_tmp109[0]=& _tmp10B,_tmp109[1]=& _tmp10C;({unsigned _tmp1E7=d->loc;struct _fat_ptr _tmp1E6=({const char*_tmp10A="pattern type %s does not match definition type %s";_tag_fat(_tmp10A,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp1E7,_tmp1E6,_tag_fat(_tmp109,sizeof(void*),2U));});});
({void*_tmp1E8=(void*)_check_null(p->topt);Cyc_Unify_unify(_tmp1E8,(void*)_check_null(e->topt));});
Cyc_Unify_explain_failure();}else{
# 549
Cyc_Tcpat_check_pat_regions(te,p,pat_res.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}case 3U: _LL26: _tmpC4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpBB)->f1;_LL27: {struct Cyc_List_List*vds=_tmpC4;
# 555
void*_tmp10D=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp10D;
struct Cyc_Tcenv_Tenv*_tmp10E=te;struct Cyc_Tcenv_Tenv*te2=_tmp10E;
for(0;vds != 0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*_tmp10F=(struct Cyc_Absyn_Vardecl*)vds->hd;struct Cyc_Absyn_Vardecl*vd=_tmp10F;
struct Cyc_Absyn_Vardecl*_tmp110=vd;void**_tmp113;void*_tmp112;union Cyc_Absyn_Nmspace _tmp111;_LL76: _tmp111=(_tmp110->name)->f1;_tmp112=_tmp110->type;_tmp113=(void**)& _tmp110->rgn;_LL77: {union Cyc_Absyn_Nmspace nsl=_tmp111;void*t=_tmp112;void**rgn_ptr=_tmp113;
*rgn_ptr=curr_bl;
({unsigned _tmp1EB=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1EA=te2;struct Cyc_List_List*_tmp1E9=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp1EB,_tmp1EA,_tmp1E9,& Cyc_Tcutil_tmk,1,1,t);});}}
# 563
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}case 4U: _LL28: _tmpC1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpBB)->f1;_tmpC2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpBB)->f2;_tmpC3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpBB)->f3;_LL29: {struct Cyc_Absyn_Tvar*tv=_tmpC1;struct Cyc_Absyn_Vardecl*vd=_tmpC2;struct Cyc_Absyn_Exp*open_exp_opt=_tmpC3;
# 569
({void*_tmp1EF=({void*_tmp1EE=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1ED=Cyc_Absyn_new_stmt(s->r,s->loc);_tmp114->f1=_tmp1ED;}),({struct Cyc_Absyn_Stmt*_tmp1EC=Cyc_Absyn_skip_stmt(s->loc);_tmp114->f2=_tmp1EC;});_tmp114;});Cyc_Absyn_new_stmt(_tmp1EE,s->loc);})->r;s->r=_tmp1EF;});{
# 571
struct Cyc_Tcenv_Tenv*_tmp115=te;struct Cyc_Tcenv_Tenv*te2=_tmp115;
struct Cyc_Absyn_Vardecl*_tmp116=vd;void**_tmp118;void**_tmp117;_LL79: _tmp117=(void**)& _tmp116->type;_tmp118=(void**)& _tmp116->rgn;_LL7A: {void**t=_tmp117;void**rgn_ptr=_tmp118;
void*_tmp119=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp119;
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 579
struct _tuple2*drname=({struct _tuple2*_tmp12C=_cycalloc(sizeof(*_tmp12C));((_tmp12C->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp1F4=({struct _fat_ptr*_tmp127[1U];({struct _fat_ptr*_tmp1F3=({struct _fat_ptr*_tmp129=_cycalloc(sizeof(*_tmp129));({struct _fat_ptr _tmp1F2=({const char*_tmp128="Core";_tag_fat(_tmp128,sizeof(char),5U);});*_tmp129=_tmp1F2;});_tmp129;});_tmp127[0]=_tmp1F3;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp127,sizeof(struct _fat_ptr*),1U));});((_tmp12C->f1).Abs_n).val=_tmp1F4;}),({
struct _fat_ptr*_tmp1F1=({struct _fat_ptr*_tmp12B=_cycalloc(sizeof(*_tmp12B));({struct _fat_ptr _tmp1F0=({const char*_tmp12A="DynamicRegion";_tag_fat(_tmp12A,sizeof(char),14U);});*_tmp12B=_tmp1F0;});_tmp12B;});_tmp12C->f2=_tmp1F1;});_tmp12C;});
void*_tmp11A=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->v=& Cyc_Tcutil_trk;_tmp126;}),0);void*outer_rgn=_tmp11A;
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->v=& Cyc_Tcutil_rk;_tmp11B;}),0);{
void*_tmp11C=({
union Cyc_Absyn_AggrInfo _tmp1F5=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);Cyc_Absyn_aggr_type(_tmp1F5,({void*_tmp125[1U];_tmp125[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp125,sizeof(void*),1U));}));});
# 583
void*dr_type=_tmp11C;
# 585
void*_tmp11D=({void*_tmp1F8=dr_type;void*_tmp1F7=outer_rgn;struct Cyc_Absyn_Tqual _tmp1F6=
Cyc_Absyn_empty_tqual(0U);
# 585
Cyc_Absyn_at_type(_tmp1F8,_tmp1F7,_tmp1F6,Cyc_Absyn_false_type);});void*exp_type=_tmp11D;
# 587
({unsigned _tmp1FB=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FA=te;struct Cyc_List_List*_tmp1F9=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1FB,_tmp1FA,_tmp1F9,& Cyc_Tcutil_tmk,1,0,exp_type);});{
void*_tmp11E=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);void*key_typ=_tmp11E;
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1FD=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1FC=open_exp_opt;Cyc_Tcutil_coerce_assign(_tmp1FD,_tmp1FC,exp_type);}))
({struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0U,({
struct _fat_ptr _tmp1FE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(exp_type));_tmp161.f1=_tmp1FE;});_tmp161;});struct Cyc_String_pa_PrintArg_struct _tmp122=({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0U,({struct _fat_ptr _tmp1FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(key_typ));_tmp160.f1=_tmp1FF;});_tmp160;});void*_tmp11F[2U];_tmp11F[0]=& _tmp121,_tmp11F[1]=& _tmp122;({unsigned _tmp201=s0->loc;struct _fat_ptr _tmp200=({const char*_tmp120="expected %s but found %s";_tag_fat(_tmp120,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp201,_tmp200,_tag_fat(_tmp11F,sizeof(void*),2U));});});
if(!Cyc_Unify_unify(outer_rgn,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Unify_unify(outer_rgn,Cyc_Absyn_refcnt_rgn_type))
({void*_tmp123=0U;({unsigned _tmp203=s0->loc;struct _fat_ptr _tmp202=({const char*_tmp124="open is only allowed on unique or reference-counted keys";_tag_fat(_tmp124,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp203,_tmp202,_tag_fat(_tmp123,sizeof(void*),0U));});});
({void*_tmp204=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_tmp204;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1);}}}else{
# 599
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=2U,_tmp12D->f1=tv;_tmp12D;});
te2=({unsigned _tmp206=s0->loc;struct Cyc_Tcenv_Tenv*_tmp205=te2;Cyc_Tcenv_add_type_vars(_tmp206,_tmp205,({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->hd=tv,_tmp12E->tl=0;_tmp12E;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0);}
# 603
({unsigned _tmp209=s0->loc;struct Cyc_Tcenv_Tenv*_tmp208=te;struct Cyc_List_List*_tmp207=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp209,_tmp208,_tmp207,& Cyc_Tcutil_bk,1,0,*t);});
if(!({void*_tmp20A=*t;Cyc_Unify_unify(_tmp20A,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmp12F=0U;({struct _fat_ptr _tmp20B=({const char*_tmp130="region stmt: type of region handle is wrong!";_tag_fat(_tmp130,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp20B,_tag_fat(_tmp12F,sizeof(void*),0U));});});
# 607
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1U: _LL2A: _tmpC0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpBB)->f1;_LL2B: {struct Cyc_Absyn_Fndecl*fd=_tmpC0;
# 612
void*_tmp131=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp131;
if((int)fd->sc != (int)Cyc_Absyn_Public)
({void*_tmp132=0U;({unsigned _tmp20D=d->loc;struct _fat_ptr _tmp20C=({const char*_tmp133="bad storage class for inner function";_tag_fat(_tmp133,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp20D,_tmp20C,_tag_fat(_tmp132,sizeof(void*),0U));});});
# 616
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;(unsigned)atts;atts=atts->tl){
void*_tmp134=(void*)atts->hd;void*_stmttmp9=_tmp134;void*_tmp135=_stmttmp9;switch(*((int*)_tmp135)){case 7U: _LL7C: _LL7D:
 goto _LL7F;case 6U: _LL7E: _LL7F:
({struct Cyc_String_pa_PrintArg_struct _tmp138=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0U,({
struct _fat_ptr _tmp20E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp162.f1=_tmp20E;});_tmp162;});void*_tmp136[1U];_tmp136[0]=& _tmp138;({unsigned _tmp210=d->loc;struct _fat_ptr _tmp20F=({const char*_tmp137="bad attribute %s for function";_tag_fat(_tmp137,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp210,_tmp20F,_tag_fat(_tmp136,sizeof(void*),1U));});});
goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}}
# 624
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_tmp211=Cyc_Tcutil_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp211;});{
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->sc=fd->sc,_tmp139->name=fd->name,_tmp139->varloc=0U,({struct Cyc_Absyn_Tqual _tmp216=Cyc_Absyn_const_tqual(0U);_tmp139->tq=_tmp216;}),({
void*_tmp215=({void*_tmp214=t;void*_tmp213=curr_bl;struct Cyc_Absyn_Tqual _tmp212=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp214,_tmp213,_tmp212,Cyc_Absyn_false_type);});_tmp139->type=_tmp215;}),_tmp139->initializer=0,_tmp139->rgn=curr_bl,_tmp139->attributes=0,_tmp139->escapes=0;_tmp139;});
# 631
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_tmp217=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_tmp217;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9U: _LL2C: _tmpBE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpBB)->f1;_tmpBF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpBB)->f2;_LL2D: {struct _fat_ptr*n=_tmpBE;struct Cyc_List_List*tds=_tmpBF;
unimp_msg_part=({const char*_tmp13A="namespace";_tag_fat(_tmp13A,sizeof(char),10U);});goto _LL21;}case 10U: _LL2E: _tmpBC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpBB)->f1;_tmpBD=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpBB)->f2;_LL2F: {struct _tuple2*q=_tmpBC;struct Cyc_List_List*tds=_tmpBD;
unimp_msg_part=({const char*_tmp13B="using";_tag_fat(_tmp13B,sizeof(char),6U);});goto _LL21;}case 5U: _LL30: _LL31:
 unimp_msg_part=({const char*_tmp13C="type";_tag_fat(_tmp13C,sizeof(char),5U);});goto _LL21;case 6U: _LL32: _LL33:
 unimp_msg_part=({const char*_tmp13D="datatype";_tag_fat(_tmp13D,sizeof(char),9U);});goto _LL21;case 7U: _LL34: _LL35:
 unimp_msg_part=({const char*_tmp13E="enum";_tag_fat(_tmp13E,sizeof(char),5U);});goto _LL21;case 8U: _LL36: _LL37:
 unimp_msg_part=({const char*_tmp13F="typedef";_tag_fat(_tmp13F,sizeof(char),8U);});goto _LL21;case 11U: _LL38: _LL39:
 unimp_msg_part=({const char*_tmp140="extern \"C\"";_tag_fat(_tmp140,sizeof(char),11U);});goto _LL21;case 12U: _LL3A: _LL3B:
# 648
 unimp_msg_part=({const char*_tmp141="extern \"C include\"";_tag_fat(_tmp141,sizeof(char),19U);});goto _LL21;case 13U: _LL3C: _LL3D:
 unimp_msg_part=({const char*_tmp142="__cyclone_port_on__";_tag_fat(_tmp142,sizeof(char),20U);});goto _LL21;case 14U: _LL3E: _LL3F:
 unimp_msg_part=({const char*_tmp143="__cyclone_port_off__";_tag_fat(_tmp143,sizeof(char),21U);});goto _LL21;case 15U: _LL40: _LL41:
 unimp_msg_part=({const char*_tmp144="__tempest_on__";_tag_fat(_tmp144,sizeof(char),15U);});goto _LL21;default: _LL42: _LL43:
 unimp_msg_part=({const char*_tmp145="__tempest_off__";_tag_fat(_tmp145,sizeof(char),16U);});goto _LL21;}_LL21:;}
# 654
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp148=({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0U,_tmp163.f1=(struct _fat_ptr)((struct _fat_ptr)unimp_msg_part);_tmp163;});void*_tmp146[1U];_tmp146[0]=& _tmp148;({struct _fat_ptr _tmp218=({const char*_tmp147="tcStmt: nested %s declarations unimplemented";_tag_fat(_tmp147,sizeof(char),45U);});((void*(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp218,_tag_fat(_tmp146,sizeof(void*),1U));});}));}}_LL0:;}

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
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 321
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 424 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 513
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 520
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 538
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 703 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 894
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 923
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 927
extern void*Cyc_Absyn_false_type;
# 929
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_rgn_handle_type(void*);
# 946
void*Cyc_Absyn_exn_type();
# 970
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 982
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 987
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1058
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1080
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1085
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1089
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple2*,void*,struct Cyc_Absyn_Exp*init);
# 1139
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 97
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 101
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 103
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 107
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 125
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 128
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 140
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 152
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
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
# 109
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 134
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 146
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 173
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 187
void Cyc_Tcutil_explain_failure();
# 189
int Cyc_Tcutil_unify(void*,void*);
# 194
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 211
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 215
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 254
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 259
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 271
int Cyc_Tcutil_new_tvar_id();
# 284
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 291
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 295
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 305
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 54 "tctyp.h"
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 62
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
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 50 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp5;struct Cyc_Absyn_Exp*_tmp4;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
({void*_tmp164=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=5U,_tmp2->f1=_tmp4,_tmp2->f2=Cyc_Absyn_PreInc;_tmp2;});e->r=_tmp164;});goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
({void*_tmp165=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=5U,_tmp3->f1=_tmp5,_tmp3->f2=Cyc_Absyn_PreDec;_tmp3;});e->r=_tmp165;});goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 59
static int Cyc_Tcstmt_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp6=e->r;void*_tmp7=_tmp6;struct Cyc_Absyn_Exp*_tmp9;struct Cyc_Absyn_Exp*_tmp8;switch(*((int*)_tmp7)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL4:
 _tmp9=_tmp8;goto _LL6;case 13U: _LL5: _tmp9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL6:
 e=_tmp9;continue;default: _LL7: _LL8:
# 66
 return 0;}_LL0:;}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 76
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmpA=e->r;void*_tmpB=_tmpA;struct Cyc_List_List*_tmp33;struct Cyc_List_List*_tmp32;struct Cyc_List_List*_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_List_List*_tmp2F;struct Cyc_List_List*_tmp2E;struct Cyc_List_List*_tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmpF;switch(*((int*)_tmpB)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 85
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp11=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp10=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmpF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL1E:
# 95
 if(!Cyc_Tcstmt_noassign_exp(_tmp11))return 0;
_tmp13=_tmp10;_tmp12=_tmpF;goto _LL20;case 27U: _LL1F: _tmp13=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp12=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL20:
 _tmp15=_tmp13;_tmp14=_tmp12;goto _LL22;case 7U: _LL21: _tmp15=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp14=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL22:
 _tmp17=_tmp15;_tmp16=_tmp14;goto _LL24;case 8U: _LL23: _tmp17=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp16=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL24:
 _tmp19=_tmp17;_tmp18=_tmp16;goto _LL26;case 23U: _LL25: _tmp19=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp18=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL26:
 _tmp1B=_tmp19;_tmp1A=_tmp18;goto _LL28;case 9U: _LL27: _tmp1B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp1A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL28:
 return Cyc_Tcstmt_noassign_exp(_tmp1B)&& Cyc_Tcstmt_noassign_exp(_tmp1A);case 41U: _LL29: _tmp1C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2A:
# 103
 _tmp1D=_tmp1C;goto _LL2C;case 38U: _LL2B: _tmp1D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2C:
 _tmp1E=_tmp1D;goto _LL2E;case 11U: _LL2D: _tmp1E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2E:
 _tmp1F=_tmp1E;goto _LL30;case 12U: _LL2F: _tmp1F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL30:
 _tmp20=_tmp1F;goto _LL32;case 13U: _LL31: _tmp20=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL32:
 _tmp21=_tmp20;goto _LL34;case 14U: _LL33: _tmp21=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL34:
 _tmp22=_tmp21;goto _LL36;case 18U: _LL35: _tmp22=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL36:
 _tmp23=_tmp22;goto _LL38;case 20U: _LL37: _tmp23=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL38:
 _tmp24=_tmp23;goto _LL3A;case 21U: _LL39: _tmp24=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3A:
 _tmp25=_tmp24;goto _LL3C;case 22U: _LL3B: _tmp25=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3C:
 _tmp26=_tmp25;goto _LL3E;case 28U: _LL3D: _tmp26=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3E:
 _tmp27=_tmp26;goto _LL40;case 15U: _LL3F: _tmp27=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL40:
 return Cyc_Tcstmt_noassign_exp(_tmp27);case 34U: _LL41: _tmp29=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB)->f1).rgn;_tmp28=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB)->f1).num_elts;_LL42:
# 117
 _tmp2B=_tmp29;_tmp2A=_tmp28;goto _LL44;case 16U: _LL43: _tmp2B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL44:
# 119
 if(!Cyc_Tcstmt_noassign_exp(_tmp2A))return 0;
if(_tmp2B != 0)return Cyc_Tcstmt_noassign_exp(_tmp2B);else{
return 1;}case 3U: _LL45: _tmp2C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL46:
# 123
 _tmp2D=_tmp2C;goto _LL48;case 31U: _LL47: _tmp2D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL48:
 _tmp2E=_tmp2D;goto _LL4A;case 24U: _LL49: _tmp2E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL4A:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,_tmp2E);case 36U: _LL4B: _tmp2F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL4C:
# 127
 _tmp30=_tmp2F;goto _LL4E;case 29U: _LL4D: _tmp30=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL4E:
 _tmp31=_tmp30;goto _LL50;case 30U: _LL4F: _tmp31=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL50:
 _tmp32=_tmp31;goto _LL52;case 26U: _LL51: _tmp32=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL52:
 _tmp33=_tmp32;goto _LL54;default: _LL53: _tmp33=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL54:
# 132
 for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
struct _tuple15*_tmpC=(struct _tuple15*)_tmp33->hd;struct _tuple15*_tmpD=_tmpC;struct Cyc_Absyn_Exp*_tmpE;_LL56: _tmpE=_tmpD->f2;_LL57:;
if(!Cyc_Tcstmt_noassign_exp(_tmpE))return 0;}
# 136
return 1;}_LL0:;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 141
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 143
struct Cyc_Tcpat_TcPatResult _tmp34=pat_res;struct _tuple1*_tmp40;struct Cyc_List_List*_tmp3F;_LL1: _tmp40=_tmp34.tvars_and_bounds_opt;_tmp3F=_tmp34.patvars;_LL2:;{
struct Cyc_List_List*_tmp35=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple17*))Cyc_Core_fst,_tmp3F);
struct Cyc_List_List*_tmp36=_tmp40 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*_tmp40).f1);
struct Cyc_List_List*_tmp37=_tmp40 == 0?0:(*_tmp40).f2;
struct Cyc_List_List*_tmp38=_tmp40 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple16*))Cyc_Core_snd,(*_tmp40).f1));
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp36);
for(0;_tmp38 != 0;_tmp38=_tmp38->tl){
te2=({struct Cyc_Tcenv_Tenv*_tmp166=te2;Cyc_Tcenv_add_region(_tmp166,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=2U,_tmp39->f1=(struct Cyc_Absyn_Tvar*)_tmp38->hd;_tmp39;}),0);});}
te2=Cyc_Tcenv_new_outlives_constraints(te2,_tmp37,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(loc,te2);{
void*_tmp3A=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp3B=_tmp35;for(0;_tmp3B != 0;_tmp3B=_tmp3B->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp3B->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp3B->hd)))->rgn=_tmp3A;}}
# 159
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp168=te2;struct Cyc_Absyn_Exp*_tmp167=where_opt;Cyc_Tcexp_tcTest(_tmp168,_tmp167,({const char*_tmp3C="switch clause guard";_tag_fat(_tmp3C,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp3D=0U;({unsigned _tmp16A=where_opt->loc;struct _fat_ptr _tmp169=({const char*_tmp3E="cannot show &&-clause in pattern has no effects";_tag_fat(_tmp3E,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp16A,_tmp169,_tag_fat(_tmp3D,sizeof(void*),0U));});});}
# 164
Cyc_Tcstmt_tcStmt(te2,s,0);};};}
# 167
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp41=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _tuple2*res=({struct _tuple2*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp16D=({struct _fat_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));({struct _fat_ptr _tmp16C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp44=({struct Cyc_Int_pa_PrintArg_struct _tmp149;_tmp149.tag=1U,_tmp149.f1=(unsigned)_tmp41;_tmp149;});void*_tmp42[1U];_tmp42[0]=& _tmp44;({struct _fat_ptr _tmp16B=({const char*_tmp43="_stmttmp%X";_tag_fat(_tmp43,sizeof(char),11U);});Cyc_aprintf(_tmp16B,_tag_fat(_tmp42,sizeof(void*),1U));});});*_tmp45=_tmp16C;});_tmp45;});_tmp46->f2=_tmp16D;});_tmp46;});
return res;}
# 181 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 183
void*_tmp47=s0->r;void*_tmp48=_tmp47;struct Cyc_Absyn_Decl*_tmp148;struct Cyc_Absyn_Stmt*_tmp147;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_List_List**_tmp145;void**_tmp144;struct Cyc_Absyn_Exp**_tmp143;struct Cyc_List_List*_tmp142;void**_tmp141;struct _fat_ptr*_tmp140;struct Cyc_Absyn_Stmt*_tmp13F;struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_Switch_clause***_tmp13D;struct Cyc_Absyn_Stmt*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Stmt*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Stmt*_tmp137;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Stmt*_tmp135;struct Cyc_Absyn_Stmt*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Stmt*_tmp132;struct Cyc_Absyn_Stmt*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Stmt*_tmp12F;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12C;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;switch(*((int*)_tmp48)){case 0U: _LL1: _LL2:
# 185
 return;case 1U: _LL3: _tmp12A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_LL4:
# 188
 Cyc_Tcexp_tcExp(te,0,_tmp12A);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(_tmp12A);
return;case 2U: _LL5: _tmp12C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp12B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL6:
# 194
 Cyc_Tcstmt_tcStmt(te,_tmp12C,1);
Cyc_Tcstmt_tcStmt(te,_tmp12B,1);
return;case 3U: _LL7: _tmp12D=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_LL8: {
# 199
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp12D == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_String_pa_PrintArg_struct _tmp4B=({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U,({struct _fat_ptr _tmp16E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp14A.f1=_tmp16E;});_tmp14A;});void*_tmp49[1U];_tmp49[0]=& _tmp4B;({unsigned _tmp170=s0->loc;struct _fat_ptr _tmp16F=({const char*_tmp4A="should return a value of type %s";_tag_fat(_tmp4A,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp170,_tmp16F,_tag_fat(_tmp49,sizeof(void*),1U));});});else{
# 205
({struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0U,({struct _fat_ptr _tmp171=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp14B.f1=_tmp171;});_tmp14B;});void*_tmp4C[1U];_tmp4C[0]=& _tmp4E;({unsigned _tmp173=s0->loc;struct _fat_ptr _tmp172=({const char*_tmp4D="must return a value of type %s";_tag_fat(_tmp4D,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp173,_tmp172,_tag_fat(_tmp4C,sizeof(void*),1U));});});}}}else{
# 209
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp12D;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0U,({
struct _fat_ptr _tmp174=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp14D.f1=_tmp174;});_tmp14D;});struct Cyc_String_pa_PrintArg_struct _tmp52=({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0U,({struct _fat_ptr _tmp175=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp14C.f1=_tmp175;});_tmp14C;});void*_tmp4F[2U];_tmp4F[0]=& _tmp51,_tmp4F[1]=& _tmp52;({unsigned _tmp177=s0->loc;struct _fat_ptr _tmp176=({const char*_tmp50="returns value of type %s but requires %s";_tag_fat(_tmp50,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp177,_tmp176,_tag_fat(_tmp4F,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 217
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp53=0U;({unsigned _tmp179=e->loc;struct _fat_ptr _tmp178=({const char*_tmp54="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp54,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp179,_tmp178,_tag_fat(_tmp53,sizeof(void*),0U));});});}
# 220
return;}case 4U: _LL9: _tmp130=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp12F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_tmp12E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp48)->f3;_LLA:
# 223
({struct Cyc_Tcenv_Tenv*_tmp17B=te;struct Cyc_Absyn_Exp*_tmp17A=_tmp130;Cyc_Tcexp_tcTest(_tmp17B,_tmp17A,({const char*_tmp55="if statement";_tag_fat(_tmp55,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp12F,1);
Cyc_Tcstmt_tcStmt(te,_tmp12E,1);
return;case 5U: _LLB: _tmp133=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp48)->f1).f1;_tmp132=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp48)->f1).f2;_tmp131=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LLC:
# 229
({struct Cyc_Tcenv_Tenv*_tmp17D=te;struct Cyc_Absyn_Exp*_tmp17C=_tmp133;Cyc_Tcexp_tcTest(_tmp17D,_tmp17C,({const char*_tmp56="while loop";_tag_fat(_tmp56,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp131,1);
return;case 9U: _LLD: _tmp139=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp138=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f1;_tmp137=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f2;_tmp136=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f3).f1;_tmp135=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f3).f2;_tmp134=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp48)->f4;_LLE:
# 234
 Cyc_Tcexp_tcExp(te,0,_tmp139);
({struct Cyc_Tcenv_Tenv*_tmp17F=te;struct Cyc_Absyn_Exp*_tmp17E=_tmp138;Cyc_Tcexp_tcTest(_tmp17F,_tmp17E,({const char*_tmp57="for loop";_tag_fat(_tmp57,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp134,1);
Cyc_Tcexp_tcExp(te,0,_tmp136);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp136);
return;case 14U: _LLF: _tmp13C=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp13B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f1;_tmp13A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp48)->f2).f2;_LL10:
# 242
 Cyc_Tcstmt_tcStmt(te,_tmp13C,1);
({struct Cyc_Tcenv_Tenv*_tmp181=te;struct Cyc_Absyn_Exp*_tmp180=_tmp13B;Cyc_Tcexp_tcTest(_tmp181,_tmp180,({const char*_tmp58="do loop";_tag_fat(_tmp58,sizeof(char),8U);}));});
return;case 6U: _LL11: _LL12:
# 246
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp13E=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp13D=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL18: {
# 251
const struct _tuple12*_tmp59=Cyc_Tcenv_process_fallthru(te,s0,_tmp13D);
if(_tmp59 == (const struct _tuple12*)0){
({void*_tmp5A=0U;({unsigned _tmp183=s0->loc;struct _fat_ptr _tmp182=({const char*_tmp5B="fallthru not in a non-last case";_tag_fat(_tmp5B,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp183,_tmp182,_tag_fat(_tmp5A,sizeof(void*),0U));});});
return;}{
# 256
struct Cyc_List_List*_tmp5C=(*_tmp59).f2;
struct Cyc_List_List*_tmp5D=(*_tmp59).f3;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp184=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp184,_tmp5C);});
struct Cyc_List_List*_tmp5E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,_tmp5D);
for(0;_tmp5E != 0 && _tmp13E != 0;(_tmp5E=_tmp5E->tl,_tmp13E=_tmp13E->tl)){
# 263
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp13E->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp13E->hd,(void*)_tmp5E->hd,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp61=({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0U,({
# 268
struct _fat_ptr _tmp185=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp13E->hd)->topt)));_tmp14F.f1=_tmp185;});_tmp14F;});struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0U,({struct _fat_ptr _tmp186=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_tmp5E->hd));_tmp14E.f1=_tmp186;});_tmp14E;});void*_tmp5F[2U];_tmp5F[0]=& _tmp61,_tmp5F[1]=& _tmp62;({unsigned _tmp188=s0->loc;struct _fat_ptr _tmp187=({const char*_tmp60="fallthru argument has type %s but pattern variable has type %s";_tag_fat(_tmp60,sizeof(char),63U);});Cyc_Tcutil_terr(_tmp188,_tmp187,_tag_fat(_tmp5F,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 272
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp5E->hd)&& !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp13E->hd))
({void*_tmp63=0U;({unsigned _tmp18A=((struct Cyc_Absyn_Exp*)_tmp13E->hd)->loc;struct _fat_ptr _tmp189=({const char*_tmp64="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp64,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp18A,_tmp189,_tag_fat(_tmp63,sizeof(void*),0U));});});}
# 275
if(_tmp13E != 0)
({void*_tmp65=0U;({unsigned _tmp18C=s0->loc;struct _fat_ptr _tmp18B=({const char*_tmp66="too many arguments in explicit fallthru";_tag_fat(_tmp66,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp18C,_tmp18B,_tag_fat(_tmp65,sizeof(void*),0U));});});
if(_tmp5E != 0)
({void*_tmp67=0U;({unsigned _tmp18E=s0->loc;struct _fat_ptr _tmp18D=({const char*_tmp68="too few arguments in explicit fallthru";_tag_fat(_tmp68,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp18E,_tmp18D,_tag_fat(_tmp67,sizeof(void*),0U));});});
return;};}case 13U: _LL19: _tmp140=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp13F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL1A:
# 285
({struct Cyc_Tcenv_Tenv*_tmp196=({unsigned _tmp195=s0->loc;struct Cyc_Tcenv_Tenv*_tmp194=te;Cyc_Tcenv_new_named_block(_tmp195,_tmp194,({struct Cyc_Absyn_Tvar*_tmp6D=_cycalloc(sizeof(*_tmp6D));
({struct _fat_ptr*_tmp193=({struct _fat_ptr*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct _fat_ptr _tmp192=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0U,_tmp150.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp140);_tmp150;});void*_tmp69[1U];_tmp69[0]=& _tmp6B;({struct _fat_ptr _tmp191=({const char*_tmp6A="`%s";_tag_fat(_tmp6A,sizeof(char),4U);});Cyc_aprintf(_tmp191,_tag_fat(_tmp69,sizeof(void*),1U));});});*_tmp6C=_tmp192;});_tmp6C;});_tmp6D->name=_tmp193;}),({
int _tmp190=Cyc_Tcutil_new_tvar_id();_tmp6D->identity=_tmp190;}),({
void*_tmp18F=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp6D->kind=_tmp18F;});_tmp6D;}));});
# 285
Cyc_Tcstmt_tcStmt(_tmp196,_tmp13F,0);});
# 289
return;case 10U: _LL1B: _tmp143=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp142=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_tmp141=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp48)->f3;_LL1C: {
# 294
struct Cyc_Absyn_Exp*_tmp6E=*_tmp143;
if(!Cyc_Tcstmt_is_var_exp(_tmp6E)){
struct Cyc_Absyn_Vardecl*_tmp6F=({struct _tuple2*_tmp197=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp197,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmp70=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmp71=({struct Cyc_Absyn_Pat*_tmp19B=({void*_tmp199=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=1U,_tmp74->f1=_tmp6F,({struct Cyc_Absyn_Pat*_tmp198=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp6E->loc);_tmp74->f2=_tmp198;});_tmp74;});Cyc_Absyn_new_pat(_tmp199,_tmp6E->loc);});struct Cyc_Absyn_Exp*_tmp19A=_tmp6E;Cyc_Absyn_let_decl(_tmp19B,_tmp19A,s0->loc);});
# 301
struct Cyc_Absyn_Stmt*_tmp72=Cyc_Absyn_decl_stmt(_tmp71,_tmp70,s0->loc);
s0->r=_tmp72->r;
({struct Cyc_Absyn_Exp*_tmp19D=({void*_tmp19C=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=4U,_tmp73->f1=_tmp6F;_tmp73;});Cyc_Absyn_varb_exp(_tmp19C,_tmp6E->loc);});*_tmp143=_tmp19D;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 308
void*_tmp75=Cyc_Tcexp_tcExp(te,0,_tmp6E);
# 310
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp75)&& !Cyc_Tcutil_is_noalias_path(_tmp6E))
({void*_tmp76=0U;({unsigned _tmp19F=_tmp6E->loc;struct _fat_ptr _tmp19E=({const char*_tmp77="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp77,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp19F,_tmp19E,_tag_fat(_tmp76,sizeof(void*),0U));});});{
# 315
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp78=_tmp142;for(0;_tmp78 != 0;_tmp78=_tmp78->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp78->hd)->pattern,& _tmp75,_tmp6E);}else{
# 323
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 325
if(_tmp78->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp79=_cycalloc(sizeof(*_tmp79));({struct Cyc_Tcpat_TcPatResult _tmp1A0=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp78->tl))->hd)->pattern,& _tmp75,_tmp6E);*_tmp79=_tmp1A0;});_tmp79;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 329
te=Cyc_Tcenv_clear_fallthru(te);else{
# 331
struct Cyc_List_List*_tmp7A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp7B=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
te=Cyc_Tcenv_set_fallthru(te,_tmp7B,_tmp7A,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp78->tl))->hd);}}else{
# 337
te=Cyc_Tcenv_clear_fallthru(te);}{
# 339
struct Cyc_Absyn_Pat*_tmp7C=((struct Cyc_Absyn_Switch_clause*)_tmp78->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp7C->topt),_tmp75)){
({struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0U,({
struct _fat_ptr _tmp1A1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp75));_tmp152.f1=_tmp1A1;});_tmp152;});struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0U,({struct _fat_ptr _tmp1A2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7C->topt)));_tmp151.f1=_tmp1A2;});_tmp151;});void*_tmp7D[2U];_tmp7D[0]=& _tmp7F,_tmp7D[1]=& _tmp80;({unsigned _tmp1A4=((struct Cyc_Absyn_Switch_clause*)_tmp78->hd)->loc;struct _fat_ptr _tmp1A3=({const char*_tmp7E="switch on type %s, but case expects type %s";_tag_fat(_tmp7E,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1A4,_tmp1A3,_tag_fat(_tmp7D,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 346
Cyc_Tcpat_check_pat_regions(te,_tmp7C,this_pattern.patvars);}
# 348
({struct Cyc_Core_Opt*_tmp1A5=({struct Cyc_Core_Opt*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->v=this_pattern.patvars;_tmp81;});((struct Cyc_Absyn_Switch_clause*)_tmp78->hd)->pat_vars=_tmp1A5;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp78->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp78->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp78->hd)->where_clause,1);};}}
# 352
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp142,_tmp141);
return;};};}case 15U: _LL1D: _tmp146=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp145=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_tmp144=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp48)->f3;_LL1E:
# 360
({void*_tmp1A9=({void*_tmp1A8=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1A7=Cyc_Absyn_new_stmt(_tmp146->r,_tmp146->loc);_tmp82->f1=_tmp1A7;}),({struct Cyc_Absyn_Stmt*_tmp1A6=Cyc_Absyn_skip_stmt(_tmp146->loc);_tmp82->f2=_tmp1A6;});_tmp82;});Cyc_Absyn_new_stmt(_tmp1A8,_tmp146->loc);})->r;_tmp146->r=_tmp1A9;});
# 362
Cyc_Tcstmt_tcStmt(te,_tmp146,1);{
# 365
struct _tuple2*def_v=({struct _tuple2*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp1AB=({struct _fat_ptr*_tmp98=_cycalloc(sizeof(*_tmp98));({struct _fat_ptr _tmp1AA=({const char*_tmp97="exn";_tag_fat(_tmp97,sizeof(char),4U);});*_tmp98=_tmp1AA;});_tmp98;});_tmp99->f2=_tmp1AB;});_tmp99;});
struct Cyc_Absyn_Vardecl*_tmp83=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);
struct Cyc_Absyn_Pat*_tmp84=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=1U,_tmp96->f1=_tmp83,({struct Cyc_Absyn_Pat*_tmp1AC=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp96->f2=_tmp1AC;});_tmp96;}),0U);
struct Cyc_Absyn_Stmt*_tmp85=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=11U,({struct Cyc_Absyn_Exp*_tmp1AD=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=4U,_tmp94->f1=_tmp83;_tmp94;}),0U);_tmp95->f1=_tmp1AD;}),_tmp95->f2=1;_tmp95;}),0U),0U);
struct Cyc_Absyn_Switch_clause*_tmp86=({struct Cyc_Absyn_Switch_clause*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->pattern=_tmp84,_tmp93->pat_vars=0,_tmp93->where_clause=0,_tmp93->body=_tmp85,_tmp93->loc=0U;_tmp93;});
({struct Cyc_List_List*_tmp1AF=({struct Cyc_List_List*_tmp1AE=*_tmp145;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1AE,({struct Cyc_Absyn_Switch_clause*_tmp87[1U];_tmp87[0]=_tmp86;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp87,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*_tmp145=_tmp1AF;});{
# 377
void*_tmp88=Cyc_Absyn_exn_type();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp89=*_tmp145;for(0;_tmp89 != 0;_tmp89=_tmp89->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp89->hd)->pattern,& _tmp88,0);}else{
# 386
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 388
if(_tmp89->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp8A=_cycalloc(sizeof(*_tmp8A));({struct Cyc_Tcpat_TcPatResult _tmp1B0=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp89->tl))->hd)->pattern,& _tmp88,0);*_tmp8A=_tmp1B0;});_tmp8A;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 392
te=Cyc_Tcenv_clear_fallthru(te);else{
# 394
struct Cyc_List_List*_tmp8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp8C=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
te=Cyc_Tcenv_set_fallthru(te,_tmp8C,_tmp8B,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp89->tl))->hd);}}else{
# 400
te=Cyc_Tcenv_clear_fallthru(te);}{
# 402
struct Cyc_Absyn_Pat*_tmp8D=((struct Cyc_Absyn_Switch_clause*)_tmp89->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp8D->topt),_tmp88)){
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,({
struct _fat_ptr _tmp1B1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp88));_tmp154.f1=_tmp1B1;});_tmp154;});struct Cyc_String_pa_PrintArg_struct _tmp91=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,({struct _fat_ptr _tmp1B2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp8D->topt)));_tmp153.f1=_tmp1B2;});_tmp153;});void*_tmp8E[2U];_tmp8E[0]=& _tmp90,_tmp8E[1]=& _tmp91;({unsigned _tmp1B4=((struct Cyc_Absyn_Switch_clause*)_tmp89->hd)->loc;struct _fat_ptr _tmp1B3=({const char*_tmp8F="switch on type %s, but case expects type %s";_tag_fat(_tmp8F,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1B4,_tmp1B3,_tag_fat(_tmp8E,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 409
Cyc_Tcpat_check_pat_regions(te,_tmp8D,this_pattern.patvars);}
# 411
({struct Cyc_Core_Opt*_tmp1B5=({struct Cyc_Core_Opt*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->v=this_pattern.patvars;_tmp92;});((struct Cyc_Absyn_Switch_clause*)_tmp89->hd)->pat_vars=_tmp1B5;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp89->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp89->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp89->hd)->where_clause,1);};}}
# 415
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*_tmp145,_tmp144);
return;};};default: _LL1F: _tmp148=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp48)->f1;_tmp147=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp48)->f2;_LL20: {
# 419
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmp9A=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;
{void*_tmp9B=_tmp148->r;void*_tmp9C=_tmp9B;struct _tuple2*_tmp126;struct Cyc_List_List*_tmp125;struct _fat_ptr*_tmp124;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_Fndecl*_tmp122;struct Cyc_Absyn_Tvar*_tmp121;struct Cyc_Absyn_Vardecl*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_List_List*_tmp11E;struct Cyc_Absyn_Pat*_tmp11D;struct Cyc_Core_Opt**_tmp11C;struct Cyc_Absyn_Exp**_tmp11B;void**_tmp11A;struct Cyc_Absyn_Vardecl*_tmp119;switch(*((int*)_tmp9C)){case 0U: _LL22: _tmp119=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9C)->f1;_LL23: {
# 423
struct Cyc_Absyn_Vardecl*_tmp9D=_tmp119;enum Cyc_Absyn_Scope _tmpD0;union Cyc_Absyn_Nmspace _tmpCF;struct _fat_ptr*_tmpCE;struct Cyc_Absyn_Tqual _tmpCD;void*_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;void**_tmpCA;struct Cyc_List_List*_tmpC9;_LL45: _tmpD0=_tmp9D->sc;_tmpCF=(_tmp9D->name)->f1;_tmpCE=(_tmp9D->name)->f2;_tmpCD=_tmp9D->tq;_tmpCC=_tmp9D->type;_tmpCB=_tmp9D->initializer;_tmpCA=(void**)& _tmp9D->rgn;_tmpC9=_tmp9D->attributes;_LL46:;{
void*_tmp9E=Cyc_Tcenv_curr_rgn(_tmp9A);
int is_local;
{enum Cyc_Absyn_Scope _tmp9F=_tmpD0;switch(_tmp9F){case Cyc_Absyn_Static: _LL48: _LL49:
 goto _LL4B;case Cyc_Absyn_Extern: _LL4A: _LL4B:
 goto _LL4D;case Cyc_Absyn_ExternC: _LL4C: _LL4D:
# 430
 _tmp119->escapes=1;
is_local=0;goto _LL47;case Cyc_Absyn_Abstract: _LL4E: _LL4F:
# 433
({void*_tmpA0=0U;({unsigned _tmp1B7=_tmp148->loc;struct _fat_ptr _tmp1B6=({const char*_tmpA1="bad abstract scope for local variable";_tag_fat(_tmpA1,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp1B7,_tmp1B6,_tag_fat(_tmpA0,sizeof(void*),0U));});});
goto _LL51;case Cyc_Absyn_Register: _LL50: _LL51:
 goto _LL53;case Cyc_Absyn_Public: _LL52: _LL53:
 goto _LL55;default: _LL54: _LL55:
 is_local=1;goto _LL47;}_LL47:;}
# 440
*_tmpCA=is_local?_tmp9E: Cyc_Absyn_heap_rgn_type;
# 442
{void*_tmpA2=Cyc_Tcutil_compress(_tmpCC);void*_tmpA3=_tmpA2;void*_tmpAF;struct Cyc_Absyn_Tqual _tmpAE;void*_tmpAD;unsigned _tmpAC;switch(*((int*)_tmpA3)){case 5U: _LL57: if(is_local){_LL58:
# 445
 _tmp119->escapes=1;
_tmpD0=3U;
is_local=0;
goto _LL56;}else{goto _LL5B;}case 4U: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA3)->f1).num_elts == 0){_LL59: _tmpAF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA3)->f1).elt_type;_tmpAE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA3)->f1).tq;_tmpAD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA3)->f1).zero_term;_tmpAC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA3)->f1).zt_loc;if(_tmp119->initializer != 0){_LL5A:
# 450
{void*_tmpA4=((struct Cyc_Absyn_Exp*)_check_null(_tmp119->initializer))->r;void*_tmpA5=_tmpA4;struct Cyc_List_List*_tmpAB;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;struct _fat_ptr _tmpA7;struct _fat_ptr _tmpA6;switch(*((int*)_tmpA5)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).Wstring_c).tag){case 8U: _LL5E: _tmpA6=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).String_c).val;_LL5F:
# 452
 _tmpCC=({void*_tmp1BC=({void*_tmp1BB=_tmpAF;struct Cyc_Absyn_Tqual _tmp1BA=_tmpAE;struct Cyc_Absyn_Exp*_tmp1B9=Cyc_Absyn_uint_exp(_get_fat_size(_tmpA6,sizeof(char)),0U);void*_tmp1B8=_tmpAD;Cyc_Absyn_array_type(_tmp1BB,_tmp1BA,_tmp1B9,_tmp1B8,_tmpAC);});_tmp119->type=_tmp1BC;});
goto _LL5D;case 9U: _LL60: _tmpA7=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA5)->f1).Wstring_c).val;_LL61:
# 455
 _tmpCC=({void*_tmp1C1=({void*_tmp1C0=_tmpAF;struct Cyc_Absyn_Tqual _tmp1BF=_tmpAE;struct Cyc_Absyn_Exp*_tmp1BE=Cyc_Absyn_uint_exp(1U,0U);void*_tmp1BD=_tmpAD;Cyc_Absyn_array_type(_tmp1C0,_tmp1BF,_tmp1BE,_tmp1BD,_tmpAC);});_tmp119->type=_tmp1C1;});
goto _LL5D;default: goto _LL6A;}case 27U: _LL62: _tmpA8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL63:
 _tmpA9=_tmpA8;goto _LL65;case 28U: _LL64: _tmpA9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL65:
# 460
 _tmpCC=({void*_tmp1C2=Cyc_Absyn_array_type(_tmpAF,_tmpAE,_tmpA9,_tmpAD,_tmpAC);_tmp119->type=_tmp1C2;});
goto _LL5D;case 36U: _LL66: _tmpAA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA5)->f2;_LL67:
 _tmpAB=_tmpAA;goto _LL69;case 26U: _LL68: _tmpAB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA5)->f1;_LL69:
# 464
 _tmpCC=({void*_tmp1C7=({void*_tmp1C6=_tmpAF;struct Cyc_Absyn_Tqual _tmp1C5=_tmpAE;struct Cyc_Absyn_Exp*_tmp1C4=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAB),0U);void*_tmp1C3=_tmpAD;Cyc_Absyn_array_type(_tmp1C6,_tmp1C5,_tmp1C4,_tmp1C3,_tmpAC);});_tmp119->type=_tmp1C7;});
goto _LL5D;default: _LL6A: _LL6B:
 goto _LL5D;}_LL5D:;}
# 468
goto _LL56;}else{goto _LL5B;}}else{goto _LL5B;}default: _LL5B: _LL5C:
 goto _LL56;}_LL56:;}{
# 475
struct Cyc_List_List*_tmpB0=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmp9A);
int _tmpB1=!is_local?0: 1;
Cyc_Tctyp_check_type(s0->loc,_tmp9A,_tmpB0,& Cyc_Tcutil_tmk,_tmpB1,1,_tmpCC);
({int _tmp1C8=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmp119->tq).print_const,_tmpCC);(_tmp119->tq).real_const=_tmp1C8;});{
# 480
struct Cyc_Tcenv_Tenv*_tmpB2=_tmp9A;
if((int)_tmpD0 == (int)3U ||(int)_tmpD0 == (int)4U)
({void*_tmpB3=0U;({unsigned _tmp1CA=_tmp148->loc;struct _fat_ptr _tmp1C9=({const char*_tmpB4="extern declarations are not yet supported within functions";_tag_fat(_tmpB4,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1CA,_tmp1C9,_tag_fat(_tmpB3,sizeof(void*),0U));});});
if(_tmpCB != 0){
Cyc_Tcexp_tcExpInitializer(_tmpB2,& _tmpCC,_tmpCB);
# 486
if(!is_local && !Cyc_Tcutil_is_const_exp(_tmpCB))
({void*_tmpB5=0U;({unsigned _tmp1CC=_tmp148->loc;struct _fat_ptr _tmp1CB=({const char*_tmpB6="initializer for static variable needs to be a constant expression";_tag_fat(_tmpB6,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1CC,_tmp1CB,_tag_fat(_tmpB5,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(_tmpB2,_tmpCB,_tmpCC)){
struct _fat_ptr _tmpB7=*_tmpCE;
struct _fat_ptr _tmpB8=Cyc_Absynpp_typ2string(_tmpCC);
struct _fat_ptr _tmpB9=Cyc_Absynpp_typ2string((void*)_check_null(_tmpCB->topt));
if(((_get_fat_size(_tmpB7,sizeof(char))+ _get_fat_size(_tmpB8,sizeof(char)))+ _get_fat_size(_tmpB9,sizeof(char)))+ (unsigned)50 < (unsigned)80)
({struct Cyc_String_pa_PrintArg_struct _tmpBC=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U,_tmp157.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB7);_tmp157;});struct Cyc_String_pa_PrintArg_struct _tmpBD=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U,_tmp156.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB8);_tmp156;});struct Cyc_String_pa_PrintArg_struct _tmpBE=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U,_tmp155.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB9);_tmp155;});void*_tmpBA[3U];_tmpBA[0]=& _tmpBC,_tmpBA[1]=& _tmpBD,_tmpBA[2]=& _tmpBE;({unsigned _tmp1CE=_tmp148->loc;struct _fat_ptr _tmp1CD=({const char*_tmpBB="%s was declared with type %s but initialized with type %s.";_tag_fat(_tmpBB,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1CE,_tmp1CD,_tag_fat(_tmpBA,sizeof(void*),3U));});});else{
# 495
if((_get_fat_size(_tmpB7,sizeof(char))+ _get_fat_size(_tmpB8,sizeof(char)))+ (unsigned)25 < (unsigned)80 &&
 _get_fat_size(_tmpB9,sizeof(char))+ (unsigned)25 < (unsigned)80)
({struct Cyc_String_pa_PrintArg_struct _tmpC1=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0U,_tmp15A.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB7);_tmp15A;});struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0U,_tmp159.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB8);_tmp159;});struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0U,_tmp158.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB9);_tmp158;});void*_tmpBF[3U];_tmpBF[0]=& _tmpC1,_tmpBF[1]=& _tmpC2,_tmpBF[2]=& _tmpC3;({unsigned _tmp1D0=_tmp148->loc;struct _fat_ptr _tmp1CF=({const char*_tmpC0="%s was declared with type %s\n but initialized with type %s.";_tag_fat(_tmpC0,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp1D0,_tmp1CF,_tag_fat(_tmpBF,sizeof(void*),3U));});});else{
# 500
({struct Cyc_String_pa_PrintArg_struct _tmpC6=({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0U,_tmp15D.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB7);_tmp15D;});struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0U,_tmp15C.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB8);_tmp15C;});struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0U,_tmp15B.f1=(struct _fat_ptr)((struct _fat_ptr)_tmpB9);_tmp15B;});void*_tmpC4[3U];_tmpC4[0]=& _tmpC6,_tmpC4[1]=& _tmpC7,_tmpC4[2]=& _tmpC8;({unsigned _tmp1D2=_tmp148->loc;struct _fat_ptr _tmp1D1=({const char*_tmpC5="%s declared with type \n%s\n but initialized with type \n%s.";_tag_fat(_tmpC5,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp1D2,_tmp1D1,_tag_fat(_tmpC4,sizeof(void*),3U));});});}}
# 502
Cyc_Tcutil_unify(_tmpCC,(void*)_check_null(_tmpCB->topt));
Cyc_Tcutil_explain_failure();}}
# 506
Cyc_Tcstmt_tcStmt(_tmpB2,_tmp147,0);
return;};};};}case 2U: _LL24: _tmp11D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9C)->f1;_tmp11C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9C)->f2;_tmp11B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9C)->f3;_tmp11A=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9C)->f4;_LL25: {
# 514
struct Cyc_Absyn_Exp*_tmpD1=*_tmp11B;
{void*_tmpD2=_tmp11D->r;void*_tmpD3=_tmpD2;switch(*((int*)_tmpD3)){case 1U: _LL6D: _LL6E:
 goto _LL70;case 2U: _LL6F: _LL70:
 goto _LL72;case 15U: _LL71: _LL72:
 goto _LL6C;default: _LL73: _LL74:
# 520
 if(!Cyc_Tcstmt_is_var_exp(_tmpD1)){
struct Cyc_Absyn_Vardecl*_tmpD4=({struct _tuple2*_tmp1D3=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1D3,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmpD5=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmpD6=({struct Cyc_Absyn_Pat*_tmp1D7=({void*_tmp1D5=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=1U,_tmpD9->f1=_tmpD4,({struct Cyc_Absyn_Pat*_tmp1D4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpD1->loc);_tmpD9->f2=_tmp1D4;});_tmpD9;});Cyc_Absyn_new_pat(_tmp1D5,_tmpD1->loc);});struct Cyc_Absyn_Exp*_tmp1D6=_tmpD1;Cyc_Absyn_let_decl(_tmp1D7,_tmp1D6,s0->loc);});
# 526
struct Cyc_Absyn_Stmt*_tmpD7=Cyc_Absyn_decl_stmt(_tmpD6,_tmpD5,s0->loc);
# 528
s0->r=_tmpD7->r;
({struct Cyc_Absyn_Exp*_tmp1D9=({void*_tmp1D8=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=4U,_tmpD8->f1=_tmpD4;_tmpD8;});Cyc_Absyn_varb_exp(_tmp1D8,_tmpD1->loc);});*_tmp11B=_tmp1D9;});
Cyc_Tcstmt_tcStmt(_tmp9A,s0,new_block);
return;}}_LL6C:;}
# 534
Cyc_Tcexp_tcExpInitializer(_tmp9A,0,_tmpD1);{
# 536
void*pat_type=(void*)_check_null(_tmpD1->topt);
# 538
struct Cyc_Tcpat_TcPatResult _tmpDA=Cyc_Tcpat_tcPat(_tmp9A,_tmp11D,& pat_type,_tmpD1);
({struct Cyc_Core_Opt*_tmp1DA=({struct Cyc_Core_Opt*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->v=_tmpDA.patvars;_tmpDB;});*_tmp11C=_tmp1DA;});
if(!({void*_tmp1DB=(void*)_check_null(_tmp11D->topt);Cyc_Tcutil_unify(_tmp1DB,(void*)_check_null(_tmpD1->topt));})&& !
Cyc_Tcutil_coerce_assign(_tmp9A,_tmpD1,(void*)_check_null(_tmp11D->topt))){
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0U,({
struct _fat_ptr _tmp1DC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp11D->topt)));_tmp15F.f1=_tmp1DC;});_tmp15F;});struct Cyc_String_pa_PrintArg_struct _tmpDF=({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0U,({struct _fat_ptr _tmp1DD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpD1->topt)));_tmp15E.f1=_tmp1DD;});_tmp15E;});void*_tmpDC[2U];_tmpDC[0]=& _tmpDE,_tmpDC[1]=& _tmpDF;({unsigned _tmp1DF=_tmp148->loc;struct _fat_ptr _tmp1DE=({const char*_tmpDD="pattern type %s does not match definition type %s";_tag_fat(_tmpDD,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp1DF,_tmp1DE,_tag_fat(_tmpDC,sizeof(void*),2U));});});
({void*_tmp1E0=(void*)_check_null(_tmp11D->topt);Cyc_Tcutil_unify(_tmp1E0,(void*)_check_null(_tmpD1->topt));});
Cyc_Tcutil_explain_failure();}else{
# 548
Cyc_Tcpat_check_pat_regions(_tmp9A,_tmp11D,_tmpDA.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmp11D->loc,_tmp9A,_tmp11D,_tmp11A);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmp9A,_tmpDA,_tmp147,0,0);
return;};}case 3U: _LL26: _tmp11E=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9C)->f1;_LL27: {
# 554
void*_tmpE0=Cyc_Tcenv_curr_rgn(_tmp9A);
struct Cyc_Tcenv_Tenv*_tmpE1=_tmp9A;
for(0;_tmp11E != 0;_tmp11E=_tmp11E->tl){
struct Cyc_Absyn_Vardecl*_tmpE2=(struct Cyc_Absyn_Vardecl*)_tmp11E->hd;
struct Cyc_Absyn_Vardecl*_tmpE3=_tmpE2;union Cyc_Absyn_Nmspace _tmpE6;void*_tmpE5;void**_tmpE4;_LL76: _tmpE6=(_tmpE3->name)->f1;_tmpE5=_tmpE3->type;_tmpE4=(void**)& _tmpE3->rgn;_LL77:;
*_tmpE4=_tmpE0;
({unsigned _tmp1E3=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1E2=_tmpE1;struct Cyc_List_List*_tmp1E1=Cyc_Tcenv_lookup_type_vars(_tmpE1);Cyc_Tctyp_check_type(_tmp1E3,_tmp1E2,_tmp1E1,& Cyc_Tcutil_tmk,1,1,_tmpE5);});}
# 562
Cyc_Tcstmt_tcStmt(_tmpE1,_tmp147,0);
return;}case 4U: _LL28: _tmp121=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9C)->f1;_tmp120=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9C)->f2;_tmp11F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9C)->f3;_LL29:
# 568
({void*_tmp1E7=({void*_tmp1E6=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1E5=Cyc_Absyn_new_stmt(_tmp147->r,_tmp147->loc);_tmpE7->f1=_tmp1E5;}),({struct Cyc_Absyn_Stmt*_tmp1E4=Cyc_Absyn_skip_stmt(_tmp147->loc);_tmpE7->f2=_tmp1E4;});_tmpE7;});Cyc_Absyn_new_stmt(_tmp1E6,_tmp147->loc);})->r;_tmp147->r=_tmp1E7;});{
# 570
struct Cyc_Tcenv_Tenv*_tmpE8=_tmp9A;
struct Cyc_Absyn_Vardecl*_tmpE9=_tmp120;void**_tmp103;void**_tmp102;_LL79: _tmp103=(void**)& _tmpE9->type;_tmp102=(void**)& _tmpE9->rgn;_LL7A:;{
void*_tmpEA=Cyc_Tcenv_curr_rgn(_tmp9A);
*_tmp102=_tmpEA;{
void*rgn_typ;
if((unsigned)_tmp11F){
# 578
struct _tuple2*drname=({struct _tuple2*_tmpFD=_cycalloc(sizeof(*_tmpFD));((_tmpFD->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp1EC=({struct _fat_ptr*_tmpF8[1U];({struct _fat_ptr*_tmp1EB=({struct _fat_ptr*_tmpFA=_cycalloc(sizeof(*_tmpFA));({struct _fat_ptr _tmp1EA=({const char*_tmpF9="Core";_tag_fat(_tmpF9,sizeof(char),5U);});*_tmpFA=_tmp1EA;});_tmpFA;});_tmpF8[0]=_tmp1EB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpF8,sizeof(struct _fat_ptr*),1U));});((_tmpFD->f1).Abs_n).val=_tmp1EC;}),({
struct _fat_ptr*_tmp1E9=({struct _fat_ptr*_tmpFC=_cycalloc(sizeof(*_tmpFC));({struct _fat_ptr _tmp1E8=({const char*_tmpFB="DynamicRegion";_tag_fat(_tmpFB,sizeof(char),14U);});*_tmpFC=_tmp1E8;});_tmpFC;});_tmpFD->f2=_tmp1E9;});_tmpFD;});
void*_tmpEB=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->v=& Cyc_Tcutil_trk;_tmpF7;}),0);
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->v=& Cyc_Tcutil_rk;_tmpEC;}),0);{
void*_tmpED=({
union Cyc_Absyn_AggrInfo _tmp1ED=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);Cyc_Absyn_aggr_type(_tmp1ED,({void*_tmpF6[1U];_tmpF6[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpF6,sizeof(void*),1U));}));});
void*_tmpEE=({void*_tmp1F0=_tmpED;void*_tmp1EF=_tmpEB;struct Cyc_Absyn_Tqual _tmp1EE=
Cyc_Absyn_empty_tqual(0U);
# 584
Cyc_Absyn_at_type(_tmp1F0,_tmp1EF,_tmp1EE,Cyc_Absyn_false_type);});
# 586
({unsigned _tmp1F3=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1F2=_tmp9A;struct Cyc_List_List*_tmp1F1=Cyc_Tcenv_lookup_type_vars(_tmp9A);Cyc_Tctyp_check_type(_tmp1F3,_tmp1F2,_tmp1F1,& Cyc_Tcutil_tmk,1,0,_tmpEE);});{
void*_tmpEF=Cyc_Tcexp_tcExp(_tmp9A,& _tmpEE,_tmp11F);
if(!Cyc_Tcutil_unify(_tmpEE,_tmpEF)&& !
Cyc_Tcutil_coerce_assign(_tmp9A,_tmp11F,_tmpEE))
({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0U,({
struct _fat_ptr _tmp1F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpEE));_tmp161.f1=_tmp1F4;});_tmp161;});struct Cyc_String_pa_PrintArg_struct _tmpF3=({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0U,({struct _fat_ptr _tmp1F5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpEF));_tmp160.f1=_tmp1F5;});_tmp160;});void*_tmpF0[2U];_tmpF0[0]=& _tmpF2,_tmpF0[1]=& _tmpF3;({unsigned _tmp1F7=s0->loc;struct _fat_ptr _tmp1F6=({const char*_tmpF1="expected %s but found %s";_tag_fat(_tmpF1,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp1F7,_tmp1F6,_tag_fat(_tmpF0,sizeof(void*),2U));});});
if(!Cyc_Tcutil_unify(_tmpEB,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Tcutil_unify(_tmpEB,Cyc_Absyn_refcnt_rgn_type))
({void*_tmpF4=0U;({unsigned _tmp1F9=s0->loc;struct _fat_ptr _tmp1F8=({const char*_tmpF5="open is only allowed on unique or reference-counted keys";_tag_fat(_tmpF5,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp1F9,_tmp1F8,_tag_fat(_tmpF4,sizeof(void*),0U));});});
({void*_tmp1FA=Cyc_Absyn_rgn_handle_type(rgn_typ);*_tmp103=_tmp1FA;});
_tmpE8=Cyc_Tcenv_add_region(_tmpE8,rgn_typ,1);};};}else{
# 598
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=2U,_tmpFE->f1=_tmp121;_tmpFE;});
_tmpE8=({unsigned _tmp1FC=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FB=_tmpE8;Cyc_Tcenv_add_type_vars(_tmp1FC,_tmp1FB,({struct Cyc_List_List*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->hd=_tmp121,_tmpFF->tl=0;_tmpFF;}));});
_tmpE8=Cyc_Tcenv_add_region(_tmpE8,rgn_typ,0);}
# 602
({unsigned _tmp1FF=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FE=_tmp9A;struct Cyc_List_List*_tmp1FD=Cyc_Tcenv_lookup_type_vars(_tmpE8);Cyc_Tctyp_check_type(_tmp1FF,_tmp1FE,_tmp1FD,& Cyc_Tcutil_bk,1,0,*_tmp103);});
if(!({void*_tmp200=*_tmp103;Cyc_Tcutil_unify(_tmp200,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmp100=0U;({struct _fat_ptr _tmp201=({const char*_tmp101="region stmt: type of region handle is wrong!";_tag_fat(_tmp101,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp201,_tag_fat(_tmp100,sizeof(void*),0U));});});
# 606
Cyc_Tcstmt_tcStmt(_tmpE8,_tmp147,0);
return;};};};case 1U: _LL2A: _tmp122=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9C)->f1;_LL2B: {
# 611
void*_tmp104=Cyc_Tcenv_curr_rgn(_tmp9A);
if((int)_tmp122->sc != (int)Cyc_Absyn_Public)
({void*_tmp105=0U;({unsigned _tmp203=_tmp148->loc;struct _fat_ptr _tmp202=({const char*_tmp106="bad storage class for inner function";_tag_fat(_tmp106,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp203,_tmp202,_tag_fat(_tmp105,sizeof(void*),0U));});});
# 615
{struct Cyc_List_List*atts=(_tmp122->i).attributes;for(0;(unsigned)atts;atts=atts->tl){
void*_tmp107=(void*)atts->hd;void*_tmp108=_tmp107;switch(*((int*)_tmp108)){case 7U: _LL7C: _LL7D:
 goto _LL7F;case 6U: _LL7E: _LL7F:
({struct Cyc_String_pa_PrintArg_struct _tmp10B=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0U,({
struct _fat_ptr _tmp204=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp162.f1=_tmp204;});_tmp162;});void*_tmp109[1U];_tmp109[0]=& _tmp10B;({unsigned _tmp206=_tmp148->loc;struct _fat_ptr _tmp205=({const char*_tmp10A="bad attribute %s for function";_tag_fat(_tmp10A,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp206,_tmp205,_tag_fat(_tmp109,sizeof(void*),1U));});});
goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}}
# 623
Cyc_Tctyp_check_fndecl_valid_type(_tmp148->loc,_tmp9A,_tmp122);{
void*t=Cyc_Tcutil_fndecl2type(_tmp122);
({struct Cyc_List_List*_tmp207=Cyc_Tcutil_transfer_fn_type_atts(t,(_tmp122->i).attributes);(_tmp122->i).attributes=_tmp207;});{
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->sc=_tmp122->sc,_tmp10C->name=_tmp122->name,_tmp10C->varloc=0U,({struct Cyc_Absyn_Tqual _tmp20C=Cyc_Absyn_const_tqual(0U);_tmp10C->tq=_tmp20C;}),({
void*_tmp20B=({void*_tmp20A=t;void*_tmp209=_tmp104;struct Cyc_Absyn_Tqual _tmp208=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp20A,_tmp209,_tmp208,Cyc_Absyn_false_type);});_tmp10C->type=_tmp20B;}),_tmp10C->initializer=0,_tmp10C->rgn=_tmp104,_tmp10C->attributes=0,_tmp10C->escapes=0;_tmp10C;});
# 630
_tmp122->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmp9A->le);
({struct Cyc_Tcenv_Fenv*_tmp20D=Cyc_Tcenv_nested_fenv(_tmp148->loc,old_fenv,_tmp122);_tmp9A->le=_tmp20D;});
Cyc_Tcstmt_tcStmt(_tmp9A,_tmp122->body,0);
Cyc_Tcenv_check_delayed_effects(_tmp9A);
Cyc_Tcenv_check_delayed_constraints(_tmp9A);
_tmp9A->le=old_fenv;
Cyc_Tcstmt_tcStmt(_tmp9A,_tmp147,0);
return;};};};}case 9U: _LL2C: _tmp124=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9C)->f1;_tmp123=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9C)->f2;_LL2D:
 unimp_msg_part=({const char*_tmp10D="namespace";_tag_fat(_tmp10D,sizeof(char),10U);});goto _LL21;case 10U: _LL2E: _tmp126=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9C)->f1;_tmp125=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9C)->f2;_LL2F:
 unimp_msg_part=({const char*_tmp10E="using";_tag_fat(_tmp10E,sizeof(char),6U);});goto _LL21;case 5U: _LL30: _LL31:
 unimp_msg_part=({const char*_tmp10F="type";_tag_fat(_tmp10F,sizeof(char),5U);});goto _LL21;case 6U: _LL32: _LL33:
 unimp_msg_part=({const char*_tmp110="datatype";_tag_fat(_tmp110,sizeof(char),9U);});goto _LL21;case 7U: _LL34: _LL35:
 unimp_msg_part=({const char*_tmp111="enum";_tag_fat(_tmp111,sizeof(char),5U);});goto _LL21;case 8U: _LL36: _LL37:
 unimp_msg_part=({const char*_tmp112="typedef";_tag_fat(_tmp112,sizeof(char),8U);});goto _LL21;case 11U: _LL38: _LL39:
 unimp_msg_part=({const char*_tmp113="extern \"C\"";_tag_fat(_tmp113,sizeof(char),11U);});goto _LL21;case 12U: _LL3A: _LL3B:
# 647
 unimp_msg_part=({const char*_tmp114="extern \"C include\"";_tag_fat(_tmp114,sizeof(char),19U);});goto _LL21;case 13U: _LL3C: _LL3D:
 unimp_msg_part=({const char*_tmp115="__cyclone_port_on__";_tag_fat(_tmp115,sizeof(char),20U);});goto _LL21;case 14U: _LL3E: _LL3F:
 unimp_msg_part=({const char*_tmp116="__cyclone_port_off__";_tag_fat(_tmp116,sizeof(char),21U);});goto _LL21;case 15U: _LL40: _LL41:
 unimp_msg_part=({const char*_tmp117="__tempest_on__";_tag_fat(_tmp117,sizeof(char),15U);});goto _LL21;default: _LL42: _LL43:
 unimp_msg_part=({const char*_tmp118="__tempest_off__";_tag_fat(_tmp118,sizeof(char),16U);});goto _LL21;}_LL21:;}
# 653
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp129=({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0U,_tmp163.f1=(struct _fat_ptr)((struct _fat_ptr)unimp_msg_part);_tmp163;});void*_tmp127[1U];_tmp127[0]=& _tmp129;({struct _fat_ptr _tmp20E=({const char*_tmp128="tcStmt: nested %s declarations unimplemented";_tag_fat(_tmp128,sizeof(char),45U);});((void*(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp20E,_tag_fat(_tmp127,sizeof(void*),1U));});}));}}_LL0:;}

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
void*Cyc_Core_fst(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 290
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 303
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 310
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 874 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 876
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 888
void*Cyc_Absyn_compress_kb(void*);
# 894
int Cyc_Absyn_type2bool(int def,void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 911
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 913
extern void*Cyc_Absyn_empty_effect;
# 915
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 917
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);
# 936
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar();
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar();
# 946
void*Cyc_Absyn_bounds_one();
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1127
struct _fat_ptr Cyc_Absyn_attribute2string(void*);
# 1129
int Cyc_Absyn_fntype_att(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 74
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 37
int Cyc_Tcutil_is_char_type(void*);
# 43
int Cyc_Tcutil_is_function_type(void*);
# 45
int Cyc_Tcutil_is_array_type(void*);
# 64
void*Cyc_Tcutil_pointer_elt_type(void*);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 87
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 94
void*Cyc_Tcutil_copy_type(void*);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 109
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 149
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
# 151
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 202
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 212
void Cyc_Tcutil_check_bitfield(unsigned,void*field_typ,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn);
# 214
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 258
int Cyc_Tcutil_new_tvar_id();
# 269
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 276
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 283
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 294
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 301
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 309
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 312
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 72 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 76
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 79
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 82
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 88
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 40 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tctyp_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;int fn_result;int generalize_evars;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;};
# 56 "tctyp.cyc"
static struct _fat_ptr*Cyc_Tctyp_fst_fdarg(struct _tuple9*t){return(struct _fat_ptr*)_check_null((*t).f1);}struct _tuple12{void*f1;int f2;};
# 62
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 64
void*_tmp0=Cyc_Tcutil_compress(e);void*_tmp1=_tmp0;int _tmpB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->tag == 1U){_LL1: _tmpB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->f3;_LL2:
# 66
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple12*_tmp2=(struct _tuple12*)es2->hd;struct _tuple12*_tmp3=_tmp2;void*_tmp8;int*_tmp7;_LL6: _tmp8=_tmp3->f1;_tmp7=(int*)& _tmp3->f2;_LL7:;{
void*_tmp4=Cyc_Tcutil_compress(_tmp8);void*_tmp5=_tmp4;int _tmp6;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5)->tag == 1U){_LL9: _tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5)->f3;_LLA:
# 70
 if(_tmpB == _tmp6){
if(b != *_tmp7)*_tmp7=1;
return es;}
# 74
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 78
return({struct Cyc_List_List*_tmpA=_region_malloc(r,sizeof(*_tmpA));({struct _tuple12*_tmp27B=({struct _tuple12*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->f1=e,_tmp9->f2=b;_tmp9;});_tmpA->hd=_tmp27B;}),_tmpA->tl=es;_tmpA;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 84
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp243;_tmp243.tag=0U,({struct _fat_ptr _tmp27C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(tv));_tmp243.f1=_tmp27C;});_tmp243;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({struct _fat_ptr _tmp27D=({const char*_tmpD="bound tvar id for %s is NULL";_tag_fat(_tmpD,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp27D,_tag_fat(_tmpC,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->hd=tv,_tmpF->tl=tvs;_tmpF;});}
# 89
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 92
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 100
if(!present)
r=({struct Cyc_List_List*_tmp10=_region_malloc(rgn,sizeof(*_tmp10));_tmp10->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp10->tl=r;_tmp10;});}
# 103
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}struct _tuple13{struct Cyc_Absyn_Tvar*f1;int f2;};
# 106
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 109
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple13 _tmp11=*((struct _tuple13*)tvs->hd);struct _tuple13 _tmp12=_tmp11;struct Cyc_Absyn_Tvar*_tmp15;int _tmp14;_LL1: _tmp15=_tmp12.f1;_tmp14=_tmp12.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp15->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 118
if(!present)
res=({struct Cyc_List_List*_tmp13=_region_malloc(r,sizeof(*_tmp13));_tmp13->hd=(struct _tuple13*)tvs->hd,_tmp13->tl=res;_tmp13;});};}
# 121
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 129
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
# 131
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
{void*_tmp16=Cyc_Tcutil_compress(e);void*_tmp17=_tmp16;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
 continue;}_LL0:;}{
# 137
struct Cyc_Absyn_Kind*_tmp18=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_tmp19=_tmp18;switch(((struct Cyc_Absyn_Kind*)_tmp19)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp19)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
# 139
 if(!Cyc_Unify_unify(e,Cyc_Absyn_unique_rgn_type))
({void*_tmp1A=0U;({struct _fat_ptr _tmp27E=({const char*_tmp1B="can't unify evar with unique region!";_tag_fat(_tmp1B,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp27E,_tag_fat(_tmp1A,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
# 144
 if(!Cyc_Unify_unify(e,Cyc_Absyn_heap_rgn_type))({void*_tmp1C=0U;({struct _fat_ptr _tmp27F=({const char*_tmp1D="can't unify evar with heap!";_tag_fat(_tmp1D,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp27F,_tag_fat(_tmp1C,sizeof(void*),0U));});});
goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 147
 if(!Cyc_Unify_unify(e,Cyc_Absyn_empty_effect))({void*_tmp1E=0U;({struct _fat_ptr _tmp280=({const char*_tmp1F="can't unify evar with {}!";_tag_fat(_tmp1F,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp280,_tag_fat(_tmp1E,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
# 150
 if(!Cyc_Unify_unify(e,Cyc_Absyn_false_type))({struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp244;_tmp244.tag=0U,({
struct _fat_ptr _tmp281=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e));_tmp244.f1=_tmp281;});_tmp244;});void*_tmp20[1U];_tmp20[0]=& _tmp22;({struct _fat_ptr _tmp282=({const char*_tmp21="can't unify evar %s with @false!";_tag_fat(_tmp21,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp282,_tag_fat(_tmp20,sizeof(void*),1U));});});
goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
# 154
 if(!({void*_tmp283=e;Cyc_Unify_unify(_tmp283,Cyc_Absyn_bounds_one());}))
({void*_tmp23=0U;({struct _fat_ptr _tmp284=({const char*_tmp24="can't unify evar with bounds_one!";_tag_fat(_tmp24,sizeof(char),34U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp284,_tag_fat(_tmp23,sizeof(void*),0U));});});
goto _LL5;default: _LL12: _LL13:
# 158
({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp246;_tmp246.tag=0U,({
struct _fat_ptr _tmp285=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e));_tmp246.f1=_tmp285;});_tmp246;});struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp245;_tmp245.tag=0U,({struct _fat_ptr _tmp286=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(in_typ));_tmp245.f1=_tmp286;});_tmp245;});void*_tmp25[2U];_tmp25[0]=& _tmp27,_tmp25[1]=& _tmp28;({unsigned _tmp288=loc;struct _fat_ptr _tmp287=({const char*_tmp26="hidden type variable %s isn't abstracted in type %s";_tag_fat(_tmp26,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp288,_tmp287,_tag_fat(_tmp25,sizeof(void*),2U));});});
goto _LL5;}_LL5:;};}}
# 169
static int Cyc_Tctyp_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp29=({struct _tuple0 _tmp247;_tmp247.f1=c1,_tmp247.f2=c2;_tmp247;});struct _tuple0 _tmp2A=_tmp29;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Absyn_Kind*_tmp3D;struct Cyc_Core_Opt**_tmp3C;struct Cyc_Absyn_Kind*_tmp3B;struct Cyc_Core_Opt**_tmp3A;struct Cyc_Absyn_Kind*_tmp39;struct Cyc_Absyn_Kind*_tmp38;struct Cyc_Core_Opt**_tmp37;struct Cyc_Core_Opt**_tmp36;struct Cyc_Absyn_Kind*_tmp35;struct Cyc_Core_Opt**_tmp34;struct Cyc_Absyn_Kind*_tmp33;struct Cyc_Absyn_Kind*_tmp32;struct Cyc_Absyn_Kind*_tmp31;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A.f1)->tag == 0U)switch(*((int*)_tmp2A.f2)){case 0U: _LL1: _tmp32=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A.f1)->f1;_tmp31=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A.f2)->f1;_LL2:
 return _tmp32 == _tmp31;case 1U: goto _LL3;default: _LL9: _tmp35=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A.f1)->f1;_tmp34=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f2)->f1;_tmp33=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f2)->f2;_LLA:
# 182
 if(Cyc_Tcutil_kind_leq(_tmp35,_tmp33)){
({struct Cyc_Core_Opt*_tmp289=({struct Cyc_Core_Opt*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->v=c1;_tmp2E;});*_tmp34=_tmp289;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2A.f2)->tag == 1U){_LL3: _tmp36=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2A.f2)->f1;_LL4:
# 175
({struct Cyc_Core_Opt*_tmp28A=({struct Cyc_Core_Opt*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->v=c1;_tmp2B;});*_tmp36=_tmp28A;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2A.f1)->tag == 1U){_LL5: _tmp37=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2A.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmp28B=({struct Cyc_Core_Opt*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->v=c2;_tmp2C;});*_tmp37=_tmp28B;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A.f2)->tag == 0U){_LL7: _tmp3A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f1)->f1;_tmp39=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f1)->f2;_tmp38=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2A.f2)->f1;_LL8:
# 178
 if(Cyc_Tcutil_kind_leq(_tmp38,_tmp39)){
({struct Cyc_Core_Opt*_tmp28C=({struct Cyc_Core_Opt*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->v=c2;_tmp2D;});*_tmp3A=_tmp28C;});return 1;}else{
return 0;}}else{_LLB: _tmp3E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f1)->f1;_tmp3D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f1)->f2;_tmp3C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f2)->f1;_tmp3B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2A.f2)->f2;_LLC:
# 186
 if(Cyc_Tcutil_kind_leq(_tmp3D,_tmp3B)){
({struct Cyc_Core_Opt*_tmp28D=({struct Cyc_Core_Opt*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->v=c1;_tmp2F;});*_tmp3C=_tmp28D;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp3B,_tmp3D)){
({struct Cyc_Core_Opt*_tmp28E=({struct Cyc_Core_Opt*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->v=c2;_tmp30;});*_tmp3E=_tmp28E;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 199
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 202
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tctyp_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp24A;_tmp24A.tag=0U,_tmp24A.f1=(struct _fat_ptr)((struct _fat_ptr)*tv->name);_tmp24A;});struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp249;_tmp249.tag=0U,({
struct _fat_ptr _tmp28F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp249.f1=_tmp28F;});_tmp249;});struct Cyc_String_pa_PrintArg_struct _tmp43=({struct Cyc_String_pa_PrintArg_struct _tmp248;_tmp248.tag=0U,({struct _fat_ptr _tmp290=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kindbound2string(k2));_tmp248.f1=_tmp290;});_tmp248;});void*_tmp3F[3U];_tmp3F[0]=& _tmp41,_tmp3F[1]=& _tmp42,_tmp3F[2]=& _tmp43;({unsigned _tmp292=loc;struct _fat_ptr _tmp291=({const char*_tmp40="type variable %s is used with inconsistent kinds %s and %s";_tag_fat(_tmp40,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp292,_tmp291,_tag_fat(_tmp3F,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp44=0U;({struct _fat_ptr _tmp293=({const char*_tmp45="same type variable has different identity!";_tag_fat(_tmp45,sizeof(char),43U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp293,_tag_fat(_tmp44,sizeof(void*),0U));});});}
return tvs;}}}
# 216
({int _tmp294=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp294;});
return({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=tv,_tmp46->tl=tvs;_tmp46;});}
# 222
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({void*_tmp47=0U;({struct _fat_ptr _tmp295=({const char*_tmp48="fast_add_free_tvar: bad identity in tv";_tag_fat(_tmp48,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp295,_tag_fat(_tmp47,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp49=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp49->identity == - 1)
({void*_tmp4A=0U;({struct _fat_ptr _tmp296=({const char*_tmp4B="fast_add_free_tvar: bad identity in tvs2";_tag_fat(_tmp4B,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp296,_tag_fat(_tmp4A,sizeof(void*),0U));});});
if(_tmp49->identity == tv->identity)
return tvs;}}
# 232
return({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=tv,_tmp4C->tl=tvs;_tmp4C;});}
# 238
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 241
if(tv->identity == - 1)
({void*_tmp4D=0U;({struct _fat_ptr _tmp297=({const char*_tmp4E="fast_add_free_tvar_bool: bad identity in tv";_tag_fat(_tmp4E,sizeof(char),44U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp297,_tag_fat(_tmp4D,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct _tuple13*_tmp4F=(struct _tuple13*)tvs2->hd;struct _tuple13*_tmp50=_tmp4F;struct Cyc_Absyn_Tvar*_tmp54;int*_tmp53;_LL1: _tmp54=_tmp50->f1;_tmp53=(int*)& _tmp50->f2;_LL2:;
if(_tmp54->identity == - 1)
({void*_tmp51=0U;({struct _fat_ptr _tmp298=({const char*_tmp52="fast_add_free_tvar_bool: bad identity in tvs2";_tag_fat(_tmp52,sizeof(char),46U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp298,_tag_fat(_tmp51,sizeof(void*),0U));});});
if(_tmp54->identity == tv->identity){
*_tmp53=*_tmp53 || b;
return tvs;}}}
# 252
return({struct Cyc_List_List*_tmp56=_region_malloc(r,sizeof(*_tmp56));({struct _tuple13*_tmp299=({struct _tuple13*_tmp55=_region_malloc(r,sizeof(*_tmp55));_tmp55->f1=tv,_tmp55->f2=b;_tmp55;});_tmp56->hd=_tmp299;}),_tmp56->tl=tvs;_tmp56;});}
# 255
static int Cyc_Tctyp_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp57=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp58=_tmp57;void*_tmp5C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58)->tag == 3U){_LL1: _tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58)->f1).ptr_atts).rgn;_LL2:
# 260
{void*_tmp59=Cyc_Tcutil_compress(_tmp5C);void*_tmp5A=_tmp59;struct Cyc_Absyn_Tvar*_tmp5B;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5A)->tag == 2U){_LL6: _tmp5B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5A)->f1;_LL7:
 return Cyc_Absyn_tvar_cmp(tvar,_tmp5B)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 264
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 270
return 1;}
return 0;}
# 274
static struct Cyc_Absyn_Kind*Cyc_Tctyp_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 277
void*_tmp5D=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp5E=_tmp5D;switch(*((int*)_tmp5E)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 286
 if((((int)expected_kind->kind == (int)Cyc_Absyn_BoxKind ||(int)expected_kind->kind == (int)Cyc_Absyn_MemKind)||(int)expected_kind->kind == (int)Cyc_Absyn_AnyKind)&&
# 289
 Cyc_Tctyp_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 295
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 300
static void Cyc_Tctyp_check_field_atts(unsigned loc,struct _fat_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp5F=(void*)atts->hd;void*_tmp60=_tmp5F;switch(*((int*)_tmp60)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0U,({
struct _fat_ptr _tmp29A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp24C.f1=_tmp29A;});_tmp24C;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp24B;_tmp24B.tag=0U,_tmp24B.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp24B;});void*_tmp61[2U];_tmp61[0]=& _tmp63,_tmp61[1]=& _tmp64;({unsigned _tmp29C=loc;struct _fat_ptr _tmp29B=({const char*_tmp62="bad attribute %s on member %s";_tag_fat(_tmp62,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp29C,_tmp29B,_tag_fat(_tmp61,sizeof(void*),2U));});});}_LL0:;}}
# 311
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*,struct Cyc_Tcenv_Tenv*,struct Cyc_Tctyp_CVTEnv);struct _tuple14{struct Cyc_Tctyp_CVTEnv f1;struct Cyc_List_List*f2;};
# 314
static struct _tuple14 Cyc_Tctyp_check_clause(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 317
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp24E;_tmp24E.tag=0U,_tmp24E.f1=(struct _fat_ptr)((struct _fat_ptr)clause_name);_tmp24E;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp24D;_tmp24D.tag=0U,({
struct _fat_ptr _tmp29D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp24D.f1=_tmp29D;});_tmp24D;});void*_tmp65[2U];_tmp65[0]=& _tmp67,_tmp65[1]=& _tmp68;({unsigned _tmp29F=loc;struct _fat_ptr _tmp29E=({const char*_tmp66="%s clause has type %s instead of integral type";_tag_fat(_tmp66,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp29F,_tmp29E,_tag_fat(_tmp65,sizeof(void*),2U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp250;_tmp250.tag=0U,_tmp250.f1=(struct _fat_ptr)((struct _fat_ptr)clause_name);_tmp250;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp24F;_tmp24F.tag=0U,({
struct _fat_ptr _tmp2A0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(clause));_tmp24F.f1=_tmp2A0;});_tmp24F;});void*_tmp69[2U];_tmp69[0]=& _tmp6B,_tmp69[1]=& _tmp6C;({unsigned _tmp2A2=clause->loc;struct _fat_ptr _tmp2A1=({const char*_tmp6A="%s clause '%s' may be unsatisfiable";_tag_fat(_tmp6A,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp2A2,_tmp2A1,_tag_fat(_tmp69,sizeof(void*),2U));});});}
# 329
return({struct _tuple14 _tmp251;_tmp251.f1=cvtenv,_tmp251.f2=relns;_tmp251;});}
# 359 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr);
# 364
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_aggr(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 370
{union Cyc_Absyn_AggrInfo _tmp6D=*info;union Cyc_Absyn_AggrInfo _tmp6E=_tmp6D;struct Cyc_Absyn_Aggrdecl*_tmp91;enum Cyc_Absyn_AggrKind _tmp90;struct _tuple1*_tmp8F;struct Cyc_Core_Opt*_tmp8E;if((_tmp6E.UnknownAggr).tag == 1){_LL1: _tmp90=((_tmp6E.UnknownAggr).val).f1;_tmp8F=((_tmp6E.UnknownAggr).val).f2;_tmp8E=((_tmp6E.UnknownAggr).val).f3;_LL2: {
# 372
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp6F;_push_handler(& _tmp6F);{int _tmp71=0;if(setjmp(_tmp6F.handler))_tmp71=1;if(!_tmp71){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8F);{
struct Cyc_Absyn_Aggrdecl*_tmp72=*adp;
if((int)_tmp72->kind != (int)_tmp90){
if((int)_tmp72->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp253;_tmp253.tag=0U,({
struct _fat_ptr _tmp2A3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp8F));_tmp253.f1=_tmp2A3;});_tmp253;});struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp252;_tmp252.tag=0U,({struct _fat_ptr _tmp2A4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp8F));_tmp252.f1=_tmp2A4;});_tmp252;});void*_tmp73[2U];_tmp73[0]=& _tmp75,_tmp73[1]=& _tmp76;({unsigned _tmp2A6=loc;struct _fat_ptr _tmp2A5=({const char*_tmp74="expecting struct %s instead of union %s";_tag_fat(_tmp74,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp2A6,_tmp2A5,_tag_fat(_tmp73,sizeof(void*),2U));});});else{
# 381
({struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp255;_tmp255.tag=0U,({
struct _fat_ptr _tmp2A7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp8F));_tmp255.f1=_tmp2A7;});_tmp255;});struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp254;_tmp254.tag=0U,({struct _fat_ptr _tmp2A8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp8F));_tmp254.f1=_tmp2A8;});_tmp254;});void*_tmp77[2U];_tmp77[0]=& _tmp79,_tmp77[1]=& _tmp7A;({unsigned _tmp2AA=loc;struct _fat_ptr _tmp2A9=({const char*_tmp78="expecting union %s instead of struct %s";_tag_fat(_tmp78,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp2AA,_tmp2A9,_tag_fat(_tmp77,sizeof(void*),2U));});});}}
# 384
if((unsigned)_tmp8E &&(int)_tmp8E->v){
if(!((unsigned)_tmp72->impl)|| !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp72->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp7D=({struct Cyc_String_pa_PrintArg_struct _tmp256;_tmp256.tag=0U,({
struct _fat_ptr _tmp2AB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp8F));_tmp256.f1=_tmp2AB;});_tmp256;});void*_tmp7B[1U];_tmp7B[0]=& _tmp7D;({unsigned _tmp2AD=loc;struct _fat_ptr _tmp2AC=({const char*_tmp7C="@tagged qualfiers don't agree on union %s";_tag_fat(_tmp7C,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp2AD,_tmp2AC,_tag_fat(_tmp7B,sizeof(void*),1U));});});}
# 390
({union Cyc_Absyn_AggrInfo _tmp2AE=Cyc_Absyn_KnownAggr(adp);*info=_tmp2AE;});};
# 374
;_pop_handler();}else{void*_tmp70=(void*)Cyc_Core_get_exn_thrown();void*_tmp7E=_tmp70;void*_tmp84;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7E)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 394
struct Cyc_Absyn_Aggrdecl*_tmp7F=({struct Cyc_Absyn_Aggrdecl*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->kind=_tmp90,_tmp83->sc=Cyc_Absyn_Extern,_tmp83->name=_tmp8F,_tmp83->tvs=0,_tmp83->impl=0,_tmp83->attributes=0,_tmp83->expected_mem_kind=0;_tmp83;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp7F);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8F);
({union Cyc_Absyn_AggrInfo _tmp2AF=Cyc_Absyn_KnownAggr(adp);*info=_tmp2AF;});
# 399
if(*targs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp82=({struct Cyc_String_pa_PrintArg_struct _tmp257;_tmp257.tag=0U,({struct _fat_ptr _tmp2B0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp8F));_tmp257.f1=_tmp2B0;});_tmp257;});void*_tmp80[1U];_tmp80[0]=& _tmp82;({unsigned _tmp2B2=loc;struct _fat_ptr _tmp2B1=({const char*_tmp81="declare parameterized type %s before using";_tag_fat(_tmp81,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp2B2,_tmp2B1,_tag_fat(_tmp80,sizeof(void*),1U));});});
return cvtenv;}
# 403
goto _LL5;}}else{_LL8: _tmp84=_tmp7E;_LL9:(int)_rethrow(_tmp84);}_LL5:;}};}
# 405
_tmp91=*adp;goto _LL4;}}else{_LL3: _tmp91=*(_tmp6E.KnownAggr).val;_LL4: {
# 407
struct Cyc_List_List*tvs=_tmp91->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0 && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp85=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp86=(void*)ts->hd;
# 415
{struct _tuple0 _tmp87=({struct _tuple0 _tmp258;({void*_tmp2B3=Cyc_Absyn_compress_kb(_tmp85->kind);_tmp258.f1=_tmp2B3;}),_tmp258.f2=_tmp86;_tmp258;});struct _tuple0 _tmp88=_tmp87;struct Cyc_Absyn_Tvar*_tmp89;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp88.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp88.f2)->tag == 2U){_LLB: _tmp89=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp88.f2)->f1;_LLC:
# 417
({struct Cyc_List_List*_tmp2B4=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,_tmp89);cvtenv.kind_env=_tmp2B4;});
({struct Cyc_List_List*_tmp2B5=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp89,1);cvtenv.free_vars=_tmp2B5;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 422
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 426
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8C=({struct Cyc_String_pa_PrintArg_struct _tmp259;_tmp259.tag=0U,({struct _fat_ptr _tmp2B6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp91->name));_tmp259.f1=_tmp2B6;});_tmp259;});void*_tmp8A[1U];_tmp8A[0]=& _tmp8C;({unsigned _tmp2B8=loc;struct _fat_ptr _tmp2B7=({const char*_tmp8B="too many parameters for type %s";_tag_fat(_tmp8B,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp2B8,_tmp2B7,_tag_fat(_tmp8A,sizeof(void*),1U));});});
if(tvs != 0){
# 430
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->hd=e,_tmp8D->tl=hidden_ts;_tmp8D;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 437
({struct Cyc_List_List*_tmp2BA=({struct Cyc_List_List*_tmp2B9=*targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp2B9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs=_tmp2BA;});}
# 439
if((allow_abs_aggr && _tmp91->impl == 0)&& !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 444
_tmp91->expected_mem_kind=1;}}_LL0:;}
# 447
return cvtenv;}
# 451
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_datatype(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 457
struct Cyc_List_List*_tmp92=*targsp;
{union Cyc_Absyn_DatatypeInfo _tmp93=*info;union Cyc_Absyn_DatatypeInfo _tmp94=_tmp93;struct Cyc_Absyn_Datatypedecl*_tmpAB;struct _tuple1*_tmpAA;int _tmpA9;if((_tmp94.UnknownDatatype).tag == 1){_LL1: _tmpAA=((_tmp94.UnknownDatatype).val).name;_tmpA9=((_tmp94.UnknownDatatype).val).is_extensible;_LL2: {
# 460
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp95;_push_handler(& _tmp95);{int _tmp97=0;if(setjmp(_tmp95.handler))_tmp97=1;if(!_tmp97){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpAA);;_pop_handler();}else{void*_tmp96=(void*)Cyc_Core_get_exn_thrown();void*_tmp98=_tmp96;void*_tmp9E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp98)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 464
struct Cyc_Absyn_Datatypedecl*_tmp99=({struct Cyc_Absyn_Datatypedecl*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->sc=Cyc_Absyn_Extern,_tmp9D->name=_tmpAA,_tmp9D->tvs=0,_tmp9D->fields=0,_tmp9D->is_extensible=_tmpA9;_tmp9D;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp99);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpAA);
# 468
if(_tmp92 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp9C=({struct Cyc_String_pa_PrintArg_struct _tmp25A;_tmp25A.tag=0U,({
struct _fat_ptr _tmp2BB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpAA));_tmp25A.f1=_tmp2BB;});_tmp25A;});void*_tmp9A[1U];_tmp9A[0]=& _tmp9C;({unsigned _tmp2BD=loc;struct _fat_ptr _tmp2BC=({const char*_tmp9B="declare parameterized datatype %s before using";_tag_fat(_tmp9B,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp2BD,_tmp2BC,_tag_fat(_tmp9A,sizeof(void*),1U));});});
return cvtenv;}
# 473
goto _LL5;}}else{_LL8: _tmp9E=_tmp98;_LL9:(int)_rethrow(_tmp9E);}_LL5:;}};}
# 477
if(_tmpA9 && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp25B;_tmp25B.tag=0U,({struct _fat_ptr _tmp2BE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpAA));_tmp25B.f1=_tmp2BE;});_tmp25B;});void*_tmp9F[1U];_tmp9F[0]=& _tmpA1;({unsigned _tmp2C0=loc;struct _fat_ptr _tmp2BF=({const char*_tmpA0="datatype %s was not declared @extensible";_tag_fat(_tmpA0,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp2C0,_tmp2BF,_tag_fat(_tmp9F,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfo _tmp2C1=Cyc_Absyn_KnownDatatype(tudp);*info=_tmp2C1;});
_tmpAB=*tudp;goto _LL4;}}else{_LL3: _tmpAB=*(_tmp94.KnownDatatype).val;_LL4: {
# 483
struct Cyc_List_List*tvs=_tmpAB->tvs;
for(0;_tmp92 != 0 && tvs != 0;(_tmp92=_tmp92->tl,tvs=tvs->tl)){
void*t=(void*)_tmp92->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 489
{struct _tuple0 _tmpA2=({struct _tuple0 _tmp25C;({void*_tmp2C2=Cyc_Absyn_compress_kb(tv->kind);_tmp25C.f1=_tmp2C2;}),_tmp25C.f2=t;_tmp25C;});struct _tuple0 _tmpA3=_tmpA2;struct Cyc_Absyn_Tvar*_tmpA4;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA3.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3.f2)->tag == 2U){_LLB: _tmpA4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3.f2)->f1;_LLC:
# 491
({struct Cyc_List_List*_tmp2C3=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,_tmpA4);cvtenv.kind_env=_tmp2C3;});
({struct Cyc_List_List*_tmp2C4=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA4,1);cvtenv.free_vars=_tmp2C4;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 496
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 500
if(_tmp92 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmp25D;_tmp25D.tag=0U,({
struct _fat_ptr _tmp2C5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpAB->name));_tmp25D.f1=_tmp2C5;});_tmp25D;});void*_tmpA5[1U];_tmpA5[0]=& _tmpA7;({unsigned _tmp2C7=loc;struct _fat_ptr _tmp2C6=({const char*_tmpA6="too many type arguments for datatype %s";_tag_fat(_tmpA6,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp2C7,_tmp2C6,_tag_fat(_tmpA5,sizeof(void*),1U));});});
if(tvs != 0){
# 505
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->hd=e,_tmpA8->tl=hidden_ts;_tmpA8;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 512
({struct Cyc_List_List*_tmp2C9=({struct Cyc_List_List*_tmp2C8=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp2C8,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmp2C9;});}
# 514
goto _LL0;}}_LL0:;}
# 516
return cvtenv;}
# 520
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_datatype_field(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 526
{union Cyc_Absyn_DatatypeFieldInfo _tmpAC=*info;union Cyc_Absyn_DatatypeFieldInfo _tmpAD=_tmpAC;struct Cyc_Absyn_Datatypedecl*_tmpC0;struct Cyc_Absyn_Datatypefield*_tmpBF;struct _tuple1*_tmpBE;struct _tuple1*_tmpBD;int _tmpBC;if((_tmpAD.UnknownDatatypefield).tag == 1){_LL1: _tmpBE=((_tmpAD.UnknownDatatypefield).val).datatype_name;_tmpBD=((_tmpAD.UnknownDatatypefield).val).field_name;_tmpBC=((_tmpAD.UnknownDatatypefield).val).is_extensible;_LL2: {
# 529
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBE);
struct Cyc_Absyn_Datatypefield*tuf;
# 534
{struct Cyc_List_List*_tmpAE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmpAE=_tmpAE->tl){
if(_tmpAE == 0)({void*_tmpAF=0U;({struct _fat_ptr _tmp2CA=({const char*_tmpB0="Tcutil found a bad datatypefield";_tag_fat(_tmpB0,sizeof(char),33U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2CA,_tag_fat(_tmpAF,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmpAE->hd)->name,_tmpBD)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmpAE->hd;
break;}}}
# 541
({union Cyc_Absyn_DatatypeFieldInfo _tmp2CB=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmp2CB;});
_tmpC0=tud;_tmpBF=tuf;goto _LL4;}}else{_LL3: _tmpC0=((_tmpAD.KnownDatatypefield).val).f1;_tmpBF=((_tmpAD.KnownDatatypefield).val).f2;_LL4: {
# 545
struct Cyc_List_List*tvs=_tmpC0->tvs;
for(0;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 551
{struct _tuple0 _tmpB1=({struct _tuple0 _tmp25E;({void*_tmp2CC=Cyc_Absyn_compress_kb(tv->kind);_tmp25E.f1=_tmp2CC;}),_tmp25E.f2=t;_tmp25E;});struct _tuple0 _tmpB2=_tmpB1;struct Cyc_Absyn_Tvar*_tmpB3;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB2.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB2.f2)->tag == 2U){_LL6: _tmpB3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB2.f2)->f1;_LL7:
# 553
({struct Cyc_List_List*_tmp2CD=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,_tmpB3);cvtenv.kind_env=_tmp2CD;});
({struct Cyc_List_List*_tmp2CE=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB3,1);cvtenv.free_vars=_tmp2CE;});
continue;}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}{
# 558
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 562
if(targs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpB6=({struct Cyc_String_pa_PrintArg_struct _tmp260;_tmp260.tag=0U,({
struct _fat_ptr _tmp2CF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpC0->name));_tmp260.f1=_tmp2CF;});_tmp260;});struct Cyc_String_pa_PrintArg_struct _tmpB7=({struct Cyc_String_pa_PrintArg_struct _tmp25F;_tmp25F.tag=0U,({struct _fat_ptr _tmp2D0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpBF->name));_tmp25F.f1=_tmp2D0;});_tmp25F;});void*_tmpB4[2U];_tmpB4[0]=& _tmpB6,_tmpB4[1]=& _tmpB7;({unsigned _tmp2D2=loc;struct _fat_ptr _tmp2D1=({const char*_tmpB5="too many type arguments for datatype %s.%s";_tag_fat(_tmpB5,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp2D2,_tmp2D1,_tag_fat(_tmpB4,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpBA=({struct Cyc_String_pa_PrintArg_struct _tmp262;_tmp262.tag=0U,({
struct _fat_ptr _tmp2D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpC0->name));_tmp262.f1=_tmp2D3;});_tmp262;});struct Cyc_String_pa_PrintArg_struct _tmpBB=({struct Cyc_String_pa_PrintArg_struct _tmp261;_tmp261.tag=0U,({struct _fat_ptr _tmp2D4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpBF->name));_tmp261.f1=_tmp2D4;});_tmp261;});void*_tmpB8[2U];_tmpB8[0]=& _tmpBA,_tmpB8[1]=& _tmpBB;({unsigned _tmp2D6=loc;struct _fat_ptr _tmp2D5=({const char*_tmpB9="too few type arguments for datatype %s.%s";_tag_fat(_tmpB9,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp2D6,_tmp2D5,_tag_fat(_tmpB8,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 570
return cvtenv;}
# 573
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_app(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 578
struct Cyc_List_List*_tmpC1=*targsp;
{void*_tmpC2=c;union Cyc_Absyn_DatatypeFieldInfo*_tmpE9;union Cyc_Absyn_DatatypeInfo*_tmpE8;union Cyc_Absyn_AggrInfo*_tmpE7;struct Cyc_List_List*_tmpE6;struct _tuple1*_tmpE5;struct Cyc_Absyn_Enumdecl**_tmpE4;switch(*((int*)_tmpC2)){case 1U: _LL1: _LL2:
# 581
 goto _LL4;case 2U: _LL3: _LL4: goto _LL6;case 0U: _LL5: _LL6: goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA: goto _LLC;case 5U: _LLB: _LLC: goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10: goto _LL12;case 14U: _LL11: _LL12: goto _LL14;case 17U: _LL13: _LL14:
# 585
 if(_tmpC1 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0U,({struct _fat_ptr _tmp2D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=0U,_tmpC6->f1=c,_tmpC6->f2=_tmpC1;_tmpC6;})));_tmp263.f1=_tmp2D7;});_tmp263;});void*_tmpC3[1U];_tmpC3[0]=& _tmpC5;({struct _fat_ptr _tmp2D8=({const char*_tmpC4="%s applied to argument(s)";_tag_fat(_tmpC4,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2D8,_tag_fat(_tmpC3,sizeof(void*),1U));});});
goto _LL0;case 9U: _LL15: _LL16:
# 589
 for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpC1->hd,1,1);}
goto _LL0;case 4U: _LL17: _LL18:
# 593
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1)!= 1)
({void*_tmpC7=0U;({struct _fat_ptr _tmp2D9=({const char*_tmpC8="tag_t applied to wrong number of arguments";_tag_fat(_tmpC8,sizeof(char),43U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2D9,_tag_fat(_tmpC7,sizeof(void*),0U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmpC1))->hd,0,1);
goto _LL0;case 15U: _LL19: _tmpE5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpC2)->f1;_tmpE4=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpC2)->f2;_LL1A:
# 598
 if(_tmpC1 != 0)({void*_tmpC9=0U;({struct _fat_ptr _tmp2DA=({const char*_tmpCA="enum applied to argument(s)";_tag_fat(_tmpCA,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2DA,_tag_fat(_tmpC9,sizeof(void*),0U));});});
if(*_tmpE4 == 0 ||((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpE4))->fields == 0){
struct _handler_cons _tmpCB;_push_handler(& _tmpCB);{int _tmpCD=0;if(setjmp(_tmpCB.handler))_tmpCD=1;if(!_tmpCD){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpE5);
*_tmpE4=*ed;}
# 601
;_pop_handler();}else{void*_tmpCC=(void*)Cyc_Core_get_exn_thrown();void*_tmpCE=_tmpCC;void*_tmpD1;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpCE)->tag == Cyc_Dict_Absent){_LL2C: _LL2D: {
# 605
struct Cyc_Absyn_Enumdecl*_tmpCF=({struct Cyc_Absyn_Enumdecl*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->sc=Cyc_Absyn_Extern,_tmpD0->name=_tmpE5,_tmpD0->fields=0;_tmpD0;});
Cyc_Tc_tcEnumdecl(te,loc,_tmpCF);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpE5);
*_tmpE4=*ed;
goto _LL2B;};}}else{_LL2E: _tmpD1=_tmpCE;_LL2F:(int)_rethrow(_tmpD1);}_LL2B:;}};}
# 611
goto _LL0;case 16U: _LL1B: _tmpE6=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmpC2)->f1;_LL1C:
# 613
 if(_tmpC1 != 0)({void*_tmpD2=0U;({struct _fat_ptr _tmp2DB=({const char*_tmpD3="enum applied to argument(s)";_tag_fat(_tmpD3,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2DB,_tag_fat(_tmpD2,sizeof(void*),0U));});});{
# 615
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
struct Cyc_Absyn_Enumfield*_tmpD4=(struct Cyc_Absyn_Enumfield*)_tmpE6->hd;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpD4->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmpD7=({struct Cyc_String_pa_PrintArg_struct _tmp264;_tmp264.tag=0U,_tmp264.f1=(struct _fat_ptr)((struct _fat_ptr)*(*_tmpD4->name).f2);_tmp264;});void*_tmpD5[1U];_tmpD5[0]=& _tmpD7;({unsigned _tmp2DD=_tmpD4->loc;struct _fat_ptr _tmp2DC=({const char*_tmpD6="duplicate enum field name %s";_tag_fat(_tmpD6,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp2DD,_tmp2DC,_tag_fat(_tmpD5,sizeof(void*),1U));});});else{
# 622
prev_fields=({struct Cyc_List_List*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->hd=(*_tmpD4->name).f2,_tmpD8->tl=prev_fields;_tmpD8;});}
if(_tmpD4->tag == 0)
({struct Cyc_Absyn_Exp*_tmp2DE=Cyc_Absyn_uint_exp(tag_count,_tmpD4->loc);_tmpD4->tag=_tmp2DE;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpD4->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp265;_tmp265.tag=0U,_tmp265.f1=(struct _fat_ptr)((struct _fat_ptr)*(*_tmpD4->name).f2);_tmp265;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({unsigned _tmp2E0=loc;struct _fat_ptr _tmp2DF=({const char*_tmpDA="enum field %s: expression is not constant";_tag_fat(_tmpDA,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp2E0,_tmp2DF,_tag_fat(_tmpD9,sizeof(void*),1U));});});}{
unsigned t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpD4->tag))).f1;
tag_count=t1 + (unsigned)1;};}
# 630
goto _LL0;};case 10U: _LL1D: _LL1E:
# 632
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1)!= 1)({void*_tmpDC=0U;({struct _fat_ptr _tmp2E1=({const char*_tmpDD="regions has wrong number of arguments";_tag_fat(_tmpDD,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2E1,_tag_fat(_tmpDC,sizeof(void*),0U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(_tmpC1))->hd,1,1);goto _LL0;case 3U: _LL1F: _LL20:
# 635
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1)!= 1)({void*_tmpDE=0U;({struct _fat_ptr _tmp2E2=({const char*_tmpDF="region_t has wrong number of arguments";_tag_fat(_tmpDF,sizeof(char),39U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2E2,_tag_fat(_tmpDE,sizeof(void*),0U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmpC1))->hd,1,1);
goto _LL0;case 13U: _LL21: _LL22:
# 639
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1)!= 1)({void*_tmpE0=0U;({struct _fat_ptr _tmp2E3=({const char*_tmpE1="@thin has wrong number of arguments";_tag_fat(_tmpE1,sizeof(char),36U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2E3,_tag_fat(_tmpE0,sizeof(void*),0U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmpC1))->hd,0,1);
goto _LL0;case 8U: _LL23: _LL24:
# 643
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1)!= 1)({void*_tmpE2=0U;({struct _fat_ptr _tmp2E4=({const char*_tmpE3="access has wrong number of arguments";_tag_fat(_tmpE3,sizeof(char),37U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp2E4,_tag_fat(_tmpE2,sizeof(void*),0U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmpC1))->hd,1,1);goto _LL0;case 20U: _LL25: _tmpE7=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpC2)->f1;_LL26:
# 646
 cvtenv=Cyc_Tctyp_i_check_valid_aggr(loc,te,cvtenv,expected_kind,_tmpE7,targsp,allow_abs_aggr);
# 648
goto _LL0;case 18U: _LL27: _tmpE8=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmpC2)->f1;_LL28:
# 650
 cvtenv=Cyc_Tctyp_i_check_valid_datatype(loc,te,cvtenv,expected_kind,_tmpE8,targsp,allow_abs_aggr);
# 652
goto _LL0;default: _LL29: _tmpE9=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpC2)->f1;_LL2A:
# 654
 cvtenv=Cyc_Tctyp_i_check_valid_datatype_field(loc,te,cvtenv,expected_kind,_tmpE9,_tmpC1,allow_abs_aggr);
# 656
goto _LL0;}_LL0:;}
# 658
return cvtenv;}struct _tuple15{enum Cyc_Absyn_Format_Type f1;void*f2;};struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 662
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 670
{void*_tmpEA=Cyc_Tcutil_compress(t);void*_tmpEB=_tmpEA;struct _tuple1*_tmp1EF;struct Cyc_List_List**_tmp1EE;struct Cyc_Absyn_Typedefdecl**_tmp1ED;void**_tmp1EC;enum Cyc_Absyn_AggrKind _tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List**_tmp1E8;void**_tmp1E7;struct Cyc_Absyn_Tqual*_tmp1E6;void*_tmp1E5;struct Cyc_List_List*_tmp1E4;int _tmp1E3;struct Cyc_Absyn_VarargInfo*_tmp1E2;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_List_List**_tmp1DE;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_List_List**_tmp1DC;void*_tmp1DB;struct Cyc_Absyn_Tqual*_tmp1DA;struct Cyc_Absyn_Exp**_tmp1D9;void*_tmp1D8;unsigned _tmp1D7;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D5;void*_tmp1D4;struct Cyc_Absyn_Tqual*_tmp1D3;void*_tmp1D2;void*_tmp1D1;void*_tmp1D0;void*_tmp1CF;void*_tmp1CE;void***_tmp1CD;struct Cyc_Absyn_Tvar*_tmp1CC;struct Cyc_Core_Opt**_tmp1CB;void**_tmp1CA;void*_tmp1C9;struct Cyc_List_List**_tmp1C8;switch(*((int*)_tmpEB)){case 0U: _LL1: _tmp1C9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEB)->f1;_tmp1C8=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEB)->f2;_LL2:
# 672
 cvtenv=Cyc_Tctyp_i_check_valid_type_app(loc,te,cvtenv,expected_kind,_tmp1C9,_tmp1C8,put_in_effect,allow_abs_aggr);
# 674
goto _LL0;case 1U: _LL3: _tmp1CB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpEB)->f1;_tmp1CA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpEB)->f2;_LL4:
# 677
 if(*_tmp1CB == 0 ||
 Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp1CB))->v)&& !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp1CB))->v,expected_kind))
({struct Cyc_Core_Opt*_tmp2E5=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp1CB=_tmp2E5;});
if(((cvtenv.fn_result && cvtenv.generalize_evars)&&(int)expected_kind->kind == (int)Cyc_Absyn_RgnKind)&& !te->tempest_generalize){
# 682
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*_tmp1CA=Cyc_Absyn_unique_rgn_type;else{
# 685
*_tmp1CA=Cyc_Absyn_heap_rgn_type;}}else{
if((cvtenv.generalize_evars &&(int)expected_kind->kind != (int)Cyc_Absyn_BoolKind)&&(int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 689
struct Cyc_Absyn_Tvar*_tmpEC=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->tag=2U,_tmpED->f1=0,_tmpED->f2=expected_kind;_tmpED;}));
({void*_tmp2E6=Cyc_Absyn_var_type(_tmpEC);*_tmp1CA=_tmp2E6;});
_tmp1CC=_tmpEC;goto _LL6;}else{
# 693
({struct Cyc_List_List*_tmp2E7=Cyc_Tctyp_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmp2E7;});}}
# 695
goto _LL0;case 2U: _LL5: _tmp1CC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEB)->f1;_LL6:
# 697
{void*_tmpEE=Cyc_Absyn_compress_kb(_tmp1CC->kind);void*_tmpEF=_tmpEE;struct Cyc_Core_Opt**_tmpF2;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpEF)->tag == 1U){_LL1A: _tmpF2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpEF)->f1;_LL1B:
({struct Cyc_Core_Opt*_tmp2E9=({struct Cyc_Core_Opt*_tmpF1=_cycalloc(sizeof(*_tmpF1));({void*_tmp2E8=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->tag=2U,_tmpF0->f1=0,_tmpF0->f2=expected_kind;_tmpF0;});_tmpF1->v=_tmp2E8;});_tmpF1;});*_tmpF2=_tmp2E9;});goto _LL19;}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 703
({struct Cyc_List_List*_tmp2EA=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,_tmp1CC);cvtenv.kind_env=_tmp2EA;});
# 706
({struct Cyc_List_List*_tmp2EB=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp1CC,put_in_effect);cvtenv.free_vars=_tmp2EB;});
# 708
{void*_tmpF3=Cyc_Absyn_compress_kb(_tmp1CC->kind);void*_tmpF4=_tmpF3;struct Cyc_Core_Opt**_tmpF8;struct Cyc_Absyn_Kind*_tmpF7;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF4)->tag == 2U){_LL1F: _tmpF8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF4)->f1;_tmpF7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF4)->f2;_LL20:
# 710
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpF7))
({struct Cyc_Core_Opt*_tmp2ED=({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(*_tmpF6));({void*_tmp2EC=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=2U,_tmpF5->f1=0,_tmpF5->f2=expected_kind;_tmpF5;});_tmpF6->v=_tmp2EC;});_tmpF6;});*_tmpF8=_tmp2ED;});
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 715
goto _LL0;case 10U: _LL7: _tmp1CE=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpEB)->f1)->r;_tmp1CD=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpEB)->f2;_LL8: {
# 721
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmpF9=_tmp1CE;struct Cyc_Absyn_Datatypedecl*_tmpFC;struct Cyc_Absyn_Enumdecl*_tmpFB;struct Cyc_Absyn_Aggrdecl*_tmpFA;switch(*((int*)_tmpF9)){case 0U: _LL24: _tmpFA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpF9)->f1;_LL25:
# 724
 if(te->in_extern_c_include)
_tmpFA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmpFA);goto _LL23;case 1U: _LL26: _tmpFB=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpF9)->f1;_LL27:
# 728
 if(te->in_extern_c_include)
_tmpFB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmpFB);goto _LL23;default: _LL28: _tmpFC=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpF9)->f1;_LL29:
# 732
 Cyc_Tc_tcDatatypedecl(te,loc,_tmpFC);goto _LL23;}_LL23:;}
# 734
({void**_tmp2EE=({void**_tmpFD=_cycalloc(sizeof(*_tmpFD));*_tmpFD=new_typ;_tmpFD;});*_tmp1CD=_tmp2EE;});
return Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3U: _LL9: _tmp1D4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEB)->f1).elt_type;_tmp1D3=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEB)->f1).elt_tq;_tmp1D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEB)->f1).ptr_atts).rgn;_tmp1D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEB)->f1).ptr_atts).nullable;_tmp1D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEB)->f1).ptr_atts).bounds;_tmp1CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEB)->f1).ptr_atts).zero_term;_LLA: {
# 740
int is_zero_terminated;
# 742
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp1D4,1,1);
({int _tmp2EF=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp1D3->print_const,_tmp1D4);_tmp1D3->real_const=_tmp2EF;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmpFE=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmpFF=_tmpFE;switch(_tmpFF){case Cyc_Absyn_Aliasable: _LL2B: _LL2C:
 k=& Cyc_Tcutil_rk;goto _LL2A;case Cyc_Absyn_Unique: _LL2D: _LL2E:
 k=& Cyc_Tcutil_urk;goto _LL2A;case Cyc_Absyn_Top: _LL2F: _LL30:
 goto _LL32;default: _LL31: _LL32:
 k=& Cyc_Tcutil_trk;goto _LL2A;}_LL2A:;}
# 751
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,_tmp1D2,1,1);
# 754
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp1CF,0,1);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp1D1,0,1);
({void*_tmp2F0=_tmp1CF;Cyc_Unify_unify(_tmp2F0,Cyc_Tcutil_is_char_type(_tmp1D4)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
is_zero_terminated=Cyc_Absyn_type2bool(0,_tmp1CF);
if(is_zero_terminated){
# 760
if(!Cyc_Tcutil_admits_zero(_tmp1D4))
({struct Cyc_String_pa_PrintArg_struct _tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0U,({
struct _fat_ptr _tmp2F1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp1D4));_tmp266.f1=_tmp2F1;});_tmp266;});void*_tmp100[1U];_tmp100[0]=& _tmp102;({unsigned _tmp2F3=loc;struct _fat_ptr _tmp2F2=({const char*_tmp101="cannot have a pointer to zero-terminated %s elements";_tag_fat(_tmp101,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp2F3,_tmp2F2,_tag_fat(_tmp100,sizeof(void*),1U));});});}
# 765
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ptrbk,_tmp1D0,0,allow_abs_aggr);{
struct Cyc_Absyn_Exp*_tmp103=({void*_tmp2F4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp2F4,_tmp1D0);});
if(_tmp103 != 0){
struct _tuple11 _tmp104=Cyc_Evexp_eval_const_uint_exp(_tmp103);struct _tuple11 _tmp105=_tmp104;unsigned _tmp109;int _tmp108;_LL34: _tmp109=_tmp105.f1;_tmp108=_tmp105.f2;_LL35:;
if(is_zero_terminated &&(!_tmp108 || _tmp109 < (unsigned)1))
({void*_tmp106=0U;({unsigned _tmp2F6=loc;struct _fat_ptr _tmp2F5=({const char*_tmp107="zero-terminated pointer cannot point to empty sequence";_tag_fat(_tmp107,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp2F6,_tmp2F5,_tag_fat(_tmp106,sizeof(void*),0U));});});}
# 772
goto _LL0;};};}case 9U: _LLB: _tmp1D5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpEB)->f1;_LLC:
# 777
({struct Cyc_Tcenv_Tenv*_tmp2F7=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp2F7,0,_tmp1D5);});
if(!Cyc_Tcutil_coerce_uint_type(_tmp1D5))
({void*_tmp10A=0U;({unsigned _tmp2F9=loc;struct _fat_ptr _tmp2F8=({const char*_tmp10B="valueof_t requires an int expression";_tag_fat(_tmp10B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp2F9,_tmp2F8,_tag_fat(_tmp10A,sizeof(void*),0U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp1D5,te,cvtenv);
goto _LL0;case 11U: _LLD: _tmp1D6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmpEB)->f1;_LLE:
# 786
({struct Cyc_Tcenv_Tenv*_tmp2FA=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp2FA,0,_tmp1D6);});
goto _LL0;case 4U: _LLF: _tmp1DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEB)->f1).elt_type;_tmp1DA=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEB)->f1).tq;_tmp1D9=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEB)->f1).num_elts;_tmp1D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEB)->f1).zero_term;_tmp1D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEB)->f1).zt_loc;_LL10: {
# 791
struct Cyc_Absyn_Exp*_tmp10C=*_tmp1D9;
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp1DB,1,allow_abs_aggr);
({int _tmp2FB=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp1DA->print_const,_tmp1DB);_tmp1DA->real_const=_tmp2FB;});{
# 795
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,_tmp1D8);
if(is_zero_terminated){
# 798
if(!Cyc_Tcutil_admits_zero(_tmp1DB))
({struct Cyc_String_pa_PrintArg_struct _tmp10F=({struct Cyc_String_pa_PrintArg_struct _tmp267;_tmp267.tag=0U,({
struct _fat_ptr _tmp2FC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp1DB));_tmp267.f1=_tmp2FC;});_tmp267;});void*_tmp10D[1U];_tmp10D[0]=& _tmp10F;({unsigned _tmp2FE=loc;struct _fat_ptr _tmp2FD=({const char*_tmp10E="cannot have a zero-terminated array of %s elements";_tag_fat(_tmp10E,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2FE,_tmp2FD,_tag_fat(_tmp10D,sizeof(void*),1U));});});}
# 804
if(_tmp10C == 0){
# 806
if(is_zero_terminated)
# 808
({struct Cyc_Absyn_Exp*_tmp2FF=_tmp10C=Cyc_Absyn_uint_exp(1U,0U);*_tmp1D9=_tmp2FF;});else{
# 811
({void*_tmp110=0U;({unsigned _tmp301=loc;struct _fat_ptr _tmp300=({const char*_tmp111="array bound defaults to 1 here";_tag_fat(_tmp111,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp301,_tmp300,_tag_fat(_tmp110,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp302=_tmp10C=Cyc_Absyn_uint_exp(1U,0U);*_tmp1D9=_tmp302;});}}
# 815
({struct Cyc_Tcenv_Tenv*_tmp303=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp303,0,_tmp10C);});
if(!Cyc_Tcutil_coerce_uint_type(_tmp10C))
({void*_tmp112=0U;({unsigned _tmp305=loc;struct _fat_ptr _tmp304=({const char*_tmp113="array bounds expression is not an unsigned int";_tag_fat(_tmp113,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp305,_tmp304,_tag_fat(_tmp112,sizeof(void*),0U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp10C,te,cvtenv);{
# 823
struct _tuple11 _tmp114=Cyc_Evexp_eval_const_uint_exp(_tmp10C);struct _tuple11 _tmp115=_tmp114;unsigned _tmp11B;int _tmp11A;_LL37: _tmp11B=_tmp115.f1;_tmp11A=_tmp115.f2;_LL38:;
# 825
if((is_zero_terminated && _tmp11A)&& _tmp11B < (unsigned)1)
({void*_tmp116=0U;({unsigned _tmp307=loc;struct _fat_ptr _tmp306=({const char*_tmp117="zero terminated array cannot have zero elements";_tag_fat(_tmp117,sizeof(char),48U);});Cyc_Tcutil_warn(_tmp307,_tmp306,_tag_fat(_tmp116,sizeof(void*),0U));});});
# 828
if((_tmp11A && _tmp11B < (unsigned)1)&& Cyc_Cyclone_tovc_r){
({void*_tmp118=0U;({unsigned _tmp309=loc;struct _fat_ptr _tmp308=({const char*_tmp119="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_fat(_tmp119,sizeof(char),75U);});Cyc_Tcutil_warn(_tmp309,_tmp308,_tag_fat(_tmp118,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp30A=Cyc_Absyn_uint_exp(1U,0U);*_tmp1D9=_tmp30A;});}
# 832
goto _LL0;};};}case 5U: _LL11: _tmp1E8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).tvars;_tmp1E7=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).effect;_tmp1E6=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).ret_tqual;_tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).ret_type;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).args;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).c_varargs;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).cyc_varargs;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).rgn_po;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).attributes;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).requires_clause;_tmp1DE=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).requires_relns;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).ensures_clause;_tmp1DC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB)->f1).ensures_relns;_LL12: {
# 839
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp1E0 != 0;_tmp1E0=_tmp1E0->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp1E0->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp11E=({struct Cyc_String_pa_PrintArg_struct _tmp268;_tmp268.tag=0U,({struct _fat_ptr _tmp30B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)_tmp1E0->hd));_tmp268.f1=_tmp30B;});_tmp268;});void*_tmp11C[1U];_tmp11C[0]=& _tmp11E;({unsigned _tmp30D=loc;struct _fat_ptr _tmp30C=({const char*_tmp11D="bad function type attribute %s";_tag_fat(_tmp11D,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp30D,_tmp30C,_tag_fat(_tmp11C,sizeof(void*),1U));});});{
void*_tmp11F=(void*)_tmp1E0->hd;void*_tmp120=_tmp11F;enum Cyc_Absyn_Format_Type _tmp125;int _tmp124;int _tmp123;switch(*((int*)_tmp120)){case 1U: _LL3A: _LL3B:
# 852
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL39;case 2U: _LL3C: _LL3D:
# 854
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL39;case 3U: _LL3E: _LL3F:
# 856
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL39;case 19U: _LL40: _tmp125=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp120)->f1;_tmp124=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp120)->f2;_tmp123=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp120)->f3;_LL41:
# 858
 if(!seen_format){
seen_format=1;
ft=_tmp125;
fmt_desc_arg=_tmp124;
fmt_arg_start=_tmp123;}else{
# 864
({void*_tmp121=0U;({unsigned _tmp30F=loc;struct _fat_ptr _tmp30E=({const char*_tmp122="function can't have multiple format attributes";_tag_fat(_tmp122,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp30F,_tmp30E,_tag_fat(_tmp121,sizeof(void*),0U));});});}
goto _LL39;default: _LL42: _LL43:
 goto _LL39;}_LL39:;};}
# 869
if(num_convs > 1)
({void*_tmp126=0U;({unsigned _tmp311=loc;struct _fat_ptr _tmp310=({const char*_tmp127="function can't have multiple calling conventions";_tag_fat(_tmp127,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp311,_tmp310,_tag_fat(_tmp126,sizeof(void*),0U));});});
# 874
Cyc_Tcutil_check_unique_tvars(loc,*_tmp1E8);
{struct Cyc_List_List*b=*_tmp1E8;for(0;b != 0;b=b->tl){
({int _tmp312=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmp312;});
({struct Cyc_List_List*_tmp313=Cyc_Tctyp_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmp313;});{
void*_tmp128=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp129=_tmp128;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp129)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp129)->f1)->kind == Cyc_Absyn_MemKind){_LL45: _LL46:
# 880
({struct Cyc_String_pa_PrintArg_struct _tmp12C=({struct Cyc_String_pa_PrintArg_struct _tmp269;_tmp269.tag=0U,_tmp269.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmp269;});void*_tmp12A[1U];_tmp12A[0]=& _tmp12C;({unsigned _tmp315=loc;struct _fat_ptr _tmp314=({const char*_tmp12B="function attempts to abstract Mem type variable %s";_tag_fat(_tmp12B,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp315,_tmp314,_tag_fat(_tmp12A,sizeof(void*),1U));});});
goto _LL44;}else{goto _LL47;}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;};}}{
# 888
struct Cyc_Tctyp_CVTEnv _tmp12D=({struct Cyc_Tctyp_CVTEnv _tmp26C;_tmp26C.r=Cyc_Core_heap_region,_tmp26C.kind_env=cvtenv.kind_env,_tmp26C.fn_result=1,_tmp26C.generalize_evars=cvtenv.generalize_evars,_tmp26C.free_vars=0,_tmp26C.free_evars=0;_tmp26C;});
# 890
_tmp12D=Cyc_Tctyp_i_check_valid_type(loc,te,_tmp12D,& Cyc_Tcutil_tmk,_tmp1E5,1,1);
({int _tmp316=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp1E6->print_const,_tmp1E5);_tmp1E6->real_const=_tmp316;});
_tmp12D.fn_result=0;
# 896
{struct Cyc_List_List*a=_tmp1E4;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp12E=(struct _tuple9*)a->hd;
void*_tmp12F=(*_tmp12E).f3;
_tmp12D=Cyc_Tctyp_i_check_valid_type(loc,te,_tmp12D,& Cyc_Tcutil_tmk,_tmp12F,1,1);{
int _tmp130=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp12E).f2).print_const,_tmp12F);
((*_tmp12E).f2).real_const=_tmp130;
# 904
if(Cyc_Tcutil_is_array_type(_tmp12F)){
# 906
void*_tmp131=Cyc_Absyn_new_evar(0,0);
_tmp12D=Cyc_Tctyp_i_check_valid_type(loc,te,_tmp12D,& Cyc_Tcutil_rk,_tmp131,1,1);
({void*_tmp317=Cyc_Tcutil_promote_array(_tmp12F,_tmp131,0);(*_tmp12E).f3=_tmp317;});}};}}
# 913
if(_tmp1E2 != 0){
if(_tmp1E3)({void*_tmp132=0U;({struct _fat_ptr _tmp318=({const char*_tmp133="both c_vararg and cyc_vararg";_tag_fat(_tmp133,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp318,_tag_fat(_tmp132,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp134=*_tmp1E2;struct Cyc_Absyn_VarargInfo _tmp135=_tmp134;void*_tmp146;int _tmp145;_LL4A: _tmp146=_tmp135.type;_tmp145=_tmp135.inject;_LL4B:;
_tmp12D=Cyc_Tctyp_i_check_valid_type(loc,te,_tmp12D,& Cyc_Tcutil_tmk,_tmp146,1,1);
({int _tmp319=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp1E2->tq).print_const,_tmp146);(_tmp1E2->tq).real_const=_tmp319;});
# 919
if(_tmp145){
void*_tmp136=Cyc_Tcutil_compress(_tmp146);void*_tmp137=_tmp136;void*_tmp144;void*_tmp143;void*_tmp142;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp137)->tag == 3U){_LL4D: _tmp144=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp137)->f1).elt_type;_tmp143=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp137)->f1).ptr_atts).bounds;_tmp142=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp137)->f1).ptr_atts).zero_term;_LL4E:
# 922
{void*_tmp138=Cyc_Tcutil_compress(_tmp144);void*_tmp139=_tmp138;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp139)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp139)->f1)->tag == 18U){_LL52: _LL53:
# 924
 if(Cyc_Tcutil_force_type2bool(0,_tmp142))
({void*_tmp13A=0U;({unsigned _tmp31B=loc;struct _fat_ptr _tmp31A=({const char*_tmp13B="can't inject into a zeroterm pointer";_tag_fat(_tmp13B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp31B,_tmp31A,_tag_fat(_tmp13A,sizeof(void*),0U));});});
if(!({void*_tmp31C=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp31C,_tmp143);}))
({void*_tmp13C=0U;({unsigned _tmp31E=loc;struct _fat_ptr _tmp31D=({const char*_tmp13D="can't inject into a fat pointer to datatype";_tag_fat(_tmp13D,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp31E,_tmp31D,_tag_fat(_tmp13C,sizeof(void*),0U));});});
goto _LL51;}else{goto _LL54;}}else{_LL54: _LL55:
({void*_tmp13E=0U;({unsigned _tmp320=loc;struct _fat_ptr _tmp31F=({const char*_tmp13F="can't inject a non-datatype type";_tag_fat(_tmp13F,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp320,_tmp31F,_tag_fat(_tmp13E,sizeof(void*),0U));});});goto _LL51;}_LL51:;}
# 931
goto _LL4C;}else{_LL4F: _LL50:
({void*_tmp140=0U;({unsigned _tmp322=loc;struct _fat_ptr _tmp321=({const char*_tmp141="expecting a datatype pointer type";_tag_fat(_tmp141,sizeof(char),34U);});Cyc_Tcutil_terr(_tmp322,_tmp321,_tag_fat(_tmp140,sizeof(void*),0U));});});goto _LL4C;}_LL4C:;}};}
# 937
if(seen_format){
int _tmp147=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1E4);
if((((fmt_desc_arg < 0 || fmt_desc_arg > _tmp147)|| fmt_arg_start < 0)||
# 941
(_tmp1E2 == 0 && !_tmp1E3)&& fmt_arg_start != 0)||
(_tmp1E2 != 0 || _tmp1E3)&& fmt_arg_start != _tmp147 + 1)
# 944
({void*_tmp148=0U;({unsigned _tmp324=loc;struct _fat_ptr _tmp323=({const char*_tmp149="bad format descriptor";_tag_fat(_tmp149,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp324,_tmp323,_tag_fat(_tmp148,sizeof(void*),0U));});});else{
# 947
struct _tuple9 _tmp14A=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp1E4,fmt_desc_arg - 1);struct _tuple9 _tmp14B=_tmp14A;void*_tmp160;_LL57: _tmp160=_tmp14B.f3;_LL58:;
# 949
{void*_tmp14C=Cyc_Tcutil_compress(_tmp160);void*_tmp14D=_tmp14C;void*_tmp159;void*_tmp158;void*_tmp157;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->tag == 3U){_LL5A: _tmp159=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).elt_type;_tmp158=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).ptr_atts).bounds;_tmp157=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).ptr_atts).zero_term;_LL5B:
# 952
 if(!Cyc_Tcutil_is_char_type(_tmp159))
({void*_tmp14E=0U;({unsigned _tmp326=loc;struct _fat_ptr _tmp325=({const char*_tmp14F="format descriptor is not a string";_tag_fat(_tmp14F,sizeof(char),34U);});Cyc_Tcutil_terr(_tmp326,_tmp325,_tag_fat(_tmp14E,sizeof(void*),0U));});});else{
# 955
struct Cyc_Absyn_Exp*_tmp150=({void*_tmp327=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp327,_tmp158);});
if(_tmp150 == 0 && _tmp1E3)
({void*_tmp151=0U;({unsigned _tmp329=loc;struct _fat_ptr _tmp328=({const char*_tmp152="format descriptor is not a char * type";_tag_fat(_tmp152,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp329,_tmp328,_tag_fat(_tmp151,sizeof(void*),0U));});});else{
if(_tmp150 != 0 && !_tmp1E3)
({void*_tmp153=0U;({unsigned _tmp32B=loc;struct _fat_ptr _tmp32A=({const char*_tmp154="format descriptor is not a char ? type";_tag_fat(_tmp154,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp32B,_tmp32A,_tag_fat(_tmp153,sizeof(void*),0U));});});}}
# 961
goto _LL59;}else{_LL5C: _LL5D:
({void*_tmp155=0U;({unsigned _tmp32D=loc;struct _fat_ptr _tmp32C=({const char*_tmp156="format descriptor is not a string type";_tag_fat(_tmp156,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp32D,_tmp32C,_tag_fat(_tmp155,sizeof(void*),0U));});});goto _LL59;}_LL59:;}
# 964
if(fmt_arg_start != 0 && !_tmp1E3){
# 968
int problem;
{struct _tuple15 _tmp15A=({struct _tuple15 _tmp26A;_tmp26A.f1=ft,({void*_tmp32E=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp1E2))->type));_tmp26A.f2=_tmp32E;});_tmp26A;});struct _tuple15 _tmp15B=_tmp15A;struct Cyc_Absyn_Datatypedecl*_tmp15D;struct Cyc_Absyn_Datatypedecl*_tmp15C;switch(_tmp15B.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5F: _tmp15C=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->f1)->f1).KnownDatatype).val;_LL60:
# 971
 problem=({struct _tuple1*_tmp32F=_tmp15C->name;Cyc_Absyn_qvar_cmp(_tmp32F,Cyc_Absyn_datatype_print_arg_qvar());})!= 0;
goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->f1)->f1).KnownDatatype).tag == 2){_LL61: _tmp15D=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B.f2)->f1)->f1).KnownDatatype).val;_LL62:
# 974
 problem=({struct _tuple1*_tmp330=_tmp15D->name;Cyc_Absyn_qvar_cmp(_tmp330,Cyc_Absyn_datatype_scanf_arg_qvar());})!= 0;
goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}default: _LL63: _LL64:
# 977
 problem=1;
goto _LL5E;}_LL5E:;}
# 980
if(problem)
({void*_tmp15E=0U;({unsigned _tmp332=loc;struct _fat_ptr _tmp331=({const char*_tmp15F="format attribute and vararg types don't match";_tag_fat(_tmp15F,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp332,_tmp331,_tag_fat(_tmp15E,sizeof(void*),0U));});});}}}
# 988
{struct Cyc_List_List*rpo=_tmp1E1;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp161=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp162=_tmp161;void*_tmp164;void*_tmp163;_LL66: _tmp164=_tmp162->f1;_tmp163=_tmp162->f2;_LL67:;
_tmp12D=Cyc_Tctyp_i_check_valid_type(loc,te,_tmp12D,& Cyc_Tcutil_ek,_tmp164,1,1);
_tmp12D=Cyc_Tctyp_i_check_valid_type(loc,te,_tmp12D,& Cyc_Tcutil_trk,_tmp163,1,1);}}{
# 996
struct Cyc_Tcenv_Fenv*_tmp165=Cyc_Tcenv_bogus_fenv(_tmp1E5,_tmp1E4);
struct Cyc_Tcenv_Tenv*_tmp166=({struct Cyc_Tcenv_Tenv*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->ns=te->ns,_tmp1A4->ae=te->ae,_tmp1A4->le=_tmp165,_tmp1A4->allow_valueof=1,_tmp1A4->in_extern_c_include=te->in_extern_c_include,_tmp1A4->in_tempest=te->in_tempest,_tmp1A4->tempest_generalize=te->tempest_generalize;_tmp1A4;});
struct _tuple14 _tmp167=({unsigned _tmp336=loc;struct Cyc_Tcenv_Tenv*_tmp335=_tmp166;struct Cyc_Tctyp_CVTEnv _tmp334=_tmp12D;struct _fat_ptr _tmp333=({const char*_tmp1A3="@requires";_tag_fat(_tmp1A3,sizeof(char),10U);});Cyc_Tctyp_check_clause(_tmp336,_tmp335,_tmp334,_tmp333,_tmp1DF);});struct _tuple14 _tmp168=_tmp167;struct Cyc_Tctyp_CVTEnv _tmp1A2;struct Cyc_List_List*_tmp1A1;_LL69: _tmp1A2=_tmp168.f1;_tmp1A1=_tmp168.f2;_LL6A:;
_tmp12D=_tmp1A2;
*_tmp1DE=_tmp1A1;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp1E4,_tmp1A1);{
struct _tuple14 _tmp169=({unsigned _tmp33A=loc;struct Cyc_Tcenv_Tenv*_tmp339=_tmp166;struct Cyc_Tctyp_CVTEnv _tmp338=_tmp12D;struct _fat_ptr _tmp337=({const char*_tmp1A0="@ensures";_tag_fat(_tmp1A0,sizeof(char),9U);});Cyc_Tctyp_check_clause(_tmp33A,_tmp339,_tmp338,_tmp337,_tmp1DD);});struct _tuple14 _tmp16A=_tmp169;struct Cyc_Tctyp_CVTEnv _tmp19F;struct Cyc_List_List*_tmp19E;_LL6C: _tmp19F=_tmp16A.f1;_tmp19E=_tmp16A.f2;_LL6D:;
_tmp12D=_tmp19F;
*_tmp1DC=_tmp19E;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp1E4,_tmp19E);
# 1007
if(*_tmp1E7 != 0)
_tmp12D=Cyc_Tctyp_i_check_valid_type(loc,te,_tmp12D,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp1E7),1,1);else{
# 1010
struct Cyc_List_List*effect=0;
# 1015
{struct Cyc_List_List*tvs=_tmp12D.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple13 _tmp16B=*((struct _tuple13*)tvs->hd);struct _tuple13 _tmp16C=_tmp16B;struct Cyc_Absyn_Tvar*_tmp17A;int _tmp179;_LL6F: _tmp17A=_tmp16C.f1;_tmp179=_tmp16C.f2;_LL70:;
if(!_tmp179)continue;{
void*_tmp16D=Cyc_Absyn_compress_kb(_tmp17A->kind);void*_tmp16E=_tmp16D;struct Cyc_Core_Opt**_tmp178;struct Cyc_Absyn_Kind*_tmp177;struct Cyc_Core_Opt**_tmp176;struct Cyc_Core_Opt**_tmp175;struct Cyc_Absyn_Kind*_tmp174;switch(*((int*)_tmp16E)){case 2U: _LL72: _tmp175=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E)->f1;_tmp174=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E)->f2;if((int)_tmp174->kind == (int)Cyc_Absyn_RgnKind){_LL73:
# 1021
 if((int)_tmp174->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmp33B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp175=_tmp33B;});_tmp177=_tmp174;goto _LL75;}
# 1024
({struct Cyc_Core_Opt*_tmp33C=Cyc_Tcutil_kind_to_bound_opt(_tmp174);*_tmp175=_tmp33C;});_tmp177=_tmp174;goto _LL75;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E)->f2)->kind){case Cyc_Absyn_BoolKind: _LL76: _LL77:
# 1027
 goto _LL79;case Cyc_Absyn_PtrBndKind: _LL78: _LL79:
 goto _LL7B;case Cyc_Absyn_IntKind: _LL7A: _LL7B:
 goto _LL7D;case Cyc_Absyn_EffKind: _LL82: _tmp176=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E)->f1;_LL83:
# 1034
({struct Cyc_Core_Opt*_tmp33D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp176=_tmp33D;});goto _LL85;default: goto _LL88;}}case 0U: _LL74: _tmp177=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E)->f1;if((int)_tmp177->kind == (int)Cyc_Absyn_RgnKind){_LL75:
# 1026
 effect=({struct Cyc_List_List*_tmp16F=_cycalloc(sizeof(*_tmp16F));({void*_tmp33E=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(_tmp17A));_tmp16F->hd=_tmp33E;}),_tmp16F->tl=effect;_tmp16F;});goto _LL71;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7C: _LL7D:
# 1030
 goto _LL7F;case Cyc_Absyn_PtrBndKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_IntKind: _LL80: _LL81:
 goto _LL71;case Cyc_Absyn_EffKind: _LL84: _LL85:
# 1036
 effect=({struct Cyc_List_List*_tmp170=_cycalloc(sizeof(*_tmp170));({void*_tmp33F=Cyc_Absyn_var_type(_tmp17A);_tmp170->hd=_tmp33F;}),_tmp170->tl=effect;_tmp170;});goto _LL71;default: _LL88: _LL89:
# 1041
 effect=({struct Cyc_List_List*_tmp173=_cycalloc(sizeof(*_tmp173));({void*_tmp340=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(_tmp17A));_tmp173->hd=_tmp340;}),_tmp173->tl=effect;_tmp173;});goto _LL71;}}default: _LL86: _tmp178=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16E)->f1;_LL87:
# 1038
({struct Cyc_Core_Opt*_tmp342=({struct Cyc_Core_Opt*_tmp172=_cycalloc(sizeof(*_tmp172));({void*_tmp341=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->tag=2U,_tmp171->f1=0,_tmp171->f2=& Cyc_Tcutil_ak;_tmp171;});_tmp172->v=_tmp341;});_tmp172;});*_tmp178=_tmp342;});goto _LL89;}_LL71:;};}}
# 1045
{struct Cyc_List_List*ts=_tmp12D.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple12 _tmp17B=*((struct _tuple12*)ts->hd);struct _tuple12 _tmp17C=_tmp17B;void*_tmp183;int _tmp182;_LL8B: _tmp183=_tmp17C.f1;_tmp182=_tmp17C.f2;_LL8C:;
if(!_tmp182)continue;{
struct Cyc_Absyn_Kind*_tmp17D=Cyc_Tcutil_type_kind(_tmp183);struct Cyc_Absyn_Kind*_tmp17E=_tmp17D;switch(((struct Cyc_Absyn_Kind*)_tmp17E)->kind){case Cyc_Absyn_RgnKind: _LL8E: _LL8F:
# 1050
 effect=({struct Cyc_List_List*_tmp17F=_cycalloc(sizeof(*_tmp17F));({void*_tmp343=Cyc_Absyn_access_eff(_tmp183);_tmp17F->hd=_tmp343;}),_tmp17F->tl=effect;_tmp17F;});goto _LL8D;case Cyc_Absyn_EffKind: _LL90: _LL91:
# 1052
 effect=({struct Cyc_List_List*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->hd=_tmp183,_tmp180->tl=effect;_tmp180;});goto _LL8D;case Cyc_Absyn_IntKind: _LL92: _LL93:
 goto _LL8D;default: _LL94: _LL95:
 effect=({struct Cyc_List_List*_tmp181=_cycalloc(sizeof(*_tmp181));({void*_tmp344=Cyc_Absyn_regionsof_eff(_tmp183);_tmp181->hd=_tmp344;}),_tmp181->tl=effect;_tmp181;});goto _LL8D;}_LL8D:;};}}
# 1057
({void*_tmp345=Cyc_Absyn_join_eff(effect);*_tmp1E7=_tmp345;});}
# 1063
if(*_tmp1E8 != 0){
struct Cyc_List_List*bs=*_tmp1E8;for(0;bs != 0;bs=bs->tl){
void*_tmp184=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp185=_tmp184;struct Cyc_Core_Opt**_tmp195;struct Cyc_Absyn_Kind*_tmp194;struct Cyc_Core_Opt**_tmp193;struct Cyc_Core_Opt**_tmp192;struct Cyc_Core_Opt**_tmp191;struct Cyc_Core_Opt**_tmp190;struct Cyc_Core_Opt**_tmp18F;struct Cyc_Core_Opt**_tmp18E;struct Cyc_Core_Opt**_tmp18D;struct Cyc_Core_Opt**_tmp18C;struct Cyc_Core_Opt**_tmp18B;switch(*((int*)_tmp185)){case 1U: _LL97: _tmp18B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LL98:
# 1067
({struct Cyc_String_pa_PrintArg_struct _tmp188=({struct Cyc_String_pa_PrintArg_struct _tmp26B;_tmp26B.tag=0U,_tmp26B.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmp26B;});void*_tmp186[1U];_tmp186[0]=& _tmp188;({unsigned _tmp347=loc;struct _fat_ptr _tmp346=({const char*_tmp187="Type variable %s unconstrained, assuming boxed";_tag_fat(_tmp187,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp347,_tmp346,_tag_fat(_tmp186,sizeof(void*),1U));});});
# 1069
_tmp18C=_tmp18B;goto _LL9A;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f2)->aliasqual == Cyc_Absyn_Top){_LL99: _tmp18C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LL9A:
 _tmp18D=_tmp18C;goto _LL9C;}else{goto _LLA9;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9B: _tmp18D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LL9C:
 _tmp18E=_tmp18D;goto _LL9E;case Cyc_Absyn_Aliasable: _LL9D: _tmp18E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LL9E:
 _tmp190=_tmp18E;goto _LLA0;case Cyc_Absyn_Unique: _LLA3: _tmp18F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LLA4:
# 1076
 _tmp192=_tmp18F;goto _LLA6;default: goto _LLA9;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9F: _tmp190=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LLA0:
# 1073
 _tmp191=_tmp190;goto _LLA2;case Cyc_Absyn_Aliasable: _LLA1: _tmp191=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LLA2:
# 1075
({struct Cyc_Core_Opt*_tmp348=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp191=_tmp348;});goto _LL96;case Cyc_Absyn_Unique: _LLA5: _tmp192=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LLA6:
# 1078
({struct Cyc_Core_Opt*_tmp349=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp192=_tmp349;});goto _LL96;default: goto _LLA9;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f2)->aliasqual == Cyc_Absyn_Top){_LLA7: _tmp193=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_LLA8:
# 1080
({struct Cyc_Core_Opt*_tmp34A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp193=_tmp34A;});goto _LL96;}else{goto _LLA9;}default: _LLA9: _tmp195=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f1;_tmp194=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp185)->f2;_LLAA:
# 1082
({struct Cyc_Core_Opt*_tmp34B=Cyc_Tcutil_kind_to_bound_opt(_tmp194);*_tmp195=_tmp34B;});goto _LL96;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp185)->f1)->kind == Cyc_Absyn_MemKind){_LLAB: _LLAC:
# 1084
({void*_tmp189=0U;({unsigned _tmp34D=loc;struct _fat_ptr _tmp34C=({const char*_tmp18A="functions can't abstract M types";_tag_fat(_tmp18A,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp34D,_tmp34C,_tag_fat(_tmp189,sizeof(void*),0U));});});goto _LL96;}else{_LLAD: _LLAE:
 goto _LL96;}}_LL96:;}}
# 1089
({struct Cyc_List_List*_tmp34E=Cyc_Tctyp_remove_bound_tvars(Cyc_Core_heap_region,_tmp12D.kind_env,*_tmp1E8);cvtenv.kind_env=_tmp34E;});
({struct Cyc_List_List*_tmp34F=Cyc_Tctyp_remove_bound_tvars_bool(_tmp12D.r,_tmp12D.free_vars,*_tmp1E8);_tmp12D.free_vars=_tmp34F;});
# 1092
{struct Cyc_List_List*tvs=_tmp12D.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple13 _tmp196=*((struct _tuple13*)tvs->hd);struct _tuple13 _tmp197=_tmp196;struct Cyc_Absyn_Tvar*_tmp199;int _tmp198;_LLB0: _tmp199=_tmp197.f1;_tmp198=_tmp197.f2;_LLB1:;
({struct Cyc_List_List*_tmp350=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp199,_tmp198);cvtenv.free_vars=_tmp350;});}}
# 1097
{struct Cyc_List_List*evs=_tmp12D.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple12 _tmp19A=*((struct _tuple12*)evs->hd);struct _tuple12 _tmp19B=_tmp19A;void*_tmp19D;int _tmp19C;_LLB3: _tmp19D=_tmp19B.f1;_tmp19C=_tmp19B.f2;_LLB4:;
({struct Cyc_List_List*_tmp351=Cyc_Tctyp_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp19D,_tmp19C);cvtenv.free_evars=_tmp351;});}}
# 1101
goto _LL0;};};};}case 6U: _LL13: _tmp1E9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpEB)->f1;_LL14:
# 1104
 for(0;_tmp1E9 != 0;_tmp1E9=_tmp1E9->tl){
struct _tuple16*_tmp1A5=(struct _tuple16*)_tmp1E9->hd;
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp1A5).f2,1,0);
({int _tmp352=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp1A5).f1).print_const,(*_tmp1A5).f2);
# 1107
((*_tmp1A5).f1).real_const=_tmp352;});}
# 1110
goto _LL0;case 7U: _LL15: _tmp1EB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEB)->f1;_tmp1EA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEB)->f2;_LL16: {
# 1114
struct Cyc_List_List*prev_fields=0;
for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
struct Cyc_Absyn_Aggrfield*_tmp1A6=(struct Cyc_Absyn_Aggrfield*)_tmp1EA->hd;struct Cyc_Absyn_Aggrfield*_tmp1A7=_tmp1A6;struct _fat_ptr*_tmp1B7;struct Cyc_Absyn_Tqual*_tmp1B6;void*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B2;_LLB6: _tmp1B7=_tmp1A7->name;_tmp1B6=(struct Cyc_Absyn_Tqual*)& _tmp1A7->tq;_tmp1B5=_tmp1A7->type;_tmp1B4=_tmp1A7->width;_tmp1B3=_tmp1A7->attributes;_tmp1B2=_tmp1A7->requires_clause;_LLB7:;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp1B7))
({struct Cyc_String_pa_PrintArg_struct _tmp1AA=({struct Cyc_String_pa_PrintArg_struct _tmp26D;_tmp26D.tag=0U,_tmp26D.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp1B7);_tmp26D;});void*_tmp1A8[1U];_tmp1A8[0]=& _tmp1AA;({unsigned _tmp354=loc;struct _fat_ptr _tmp353=({const char*_tmp1A9="duplicate field %s";_tag_fat(_tmp1A9,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp354,_tmp353,_tag_fat(_tmp1A8,sizeof(void*),1U));});});
if(({struct _fat_ptr _tmp355=(struct _fat_ptr)*_tmp1B7;Cyc_strcmp(_tmp355,({const char*_tmp1AB="";_tag_fat(_tmp1AB,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->hd=_tmp1B7,_tmp1AC->tl=prev_fields;_tmp1AC;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp1B5,1,0);
({int _tmp356=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp1B6->print_const,_tmp1B5);_tmp1B6->real_const=_tmp356;});
Cyc_Tcutil_check_bitfield(loc,_tmp1B5,_tmp1B4,_tmp1B7);
Cyc_Tctyp_check_field_atts(loc,_tmp1B7,_tmp1B3);
if(_tmp1B2 != 0){
# 1127
if((int)_tmp1EB != (int)1U)
({void*_tmp1AD=0U;({unsigned _tmp358=loc;struct _fat_ptr _tmp357=({const char*_tmp1AE="@requires clause is only allowed on union members";_tag_fat(_tmp1AE,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp358,_tmp357,_tag_fat(_tmp1AD,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp359=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp359,0,_tmp1B2);});
if(!Cyc_Tcutil_is_integral(_tmp1B2))
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp26E;_tmp26E.tag=0U,({
struct _fat_ptr _tmp35A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1B2->topt)));_tmp26E.f1=_tmp35A;});_tmp26E;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({unsigned _tmp35C=loc;struct _fat_ptr _tmp35B=({const char*_tmp1B0="@requires clause has type %s instead of integral type";_tag_fat(_tmp1B0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp35C,_tmp35B,_tag_fat(_tmp1AF,sizeof(void*),1U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp1B2,te,cvtenv);}}
# 1136
goto _LL0;}default: _LL17: _tmp1EF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpEB)->f1;_tmp1EE=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpEB)->f2;_tmp1ED=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpEB)->f3;_tmp1EC=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpEB)->f4;_LL18: {
# 1139
struct Cyc_List_List*targs=*_tmp1EE;
# 1141
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp1B8;_push_handler(& _tmp1B8);{int _tmp1BA=0;if(setjmp(_tmp1B8.handler))_tmp1BA=1;if(!_tmp1BA){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp1EF);;_pop_handler();}else{void*_tmp1B9=(void*)Cyc_Core_get_exn_thrown();void*_tmp1BB=_tmp1B9;void*_tmp1BF;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1BB)->tag == Cyc_Dict_Absent){_LLB9: _LLBA:
# 1144
({struct Cyc_String_pa_PrintArg_struct _tmp1BE=({struct Cyc_String_pa_PrintArg_struct _tmp26F;_tmp26F.tag=0U,({struct _fat_ptr _tmp35D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp1EF));_tmp26F.f1=_tmp35D;});_tmp26F;});void*_tmp1BC[1U];_tmp1BC[0]=& _tmp1BE;({unsigned _tmp35F=loc;struct _fat_ptr _tmp35E=({const char*_tmp1BD="unbound typedef name %s";_tag_fat(_tmp1BD,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp35F,_tmp35E,_tag_fat(_tmp1BC,sizeof(void*),1U));});});
return cvtenv;}else{_LLBB: _tmp1BF=_tmp1BB;_LLBC:(int)_rethrow(_tmp1BF);}_LLB8:;}};}
# 1147
*_tmp1ED=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 1152
for(0;ts != 0 && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 1157
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));({struct _tuple17*_tmp360=({struct _tuple17*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp1C0->f2=(void*)ts->hd;_tmp1C0;});_tmp1C1->hd=_tmp360;}),_tmp1C1->tl=inst;_tmp1C1;});}
# 1161
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1C4=({struct Cyc_String_pa_PrintArg_struct _tmp270;_tmp270.tag=0U,({struct _fat_ptr _tmp361=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp1EF));_tmp270.f1=_tmp361;});_tmp270;});void*_tmp1C2[1U];_tmp1C2[0]=& _tmp1C4;({unsigned _tmp363=loc;struct _fat_ptr _tmp362=({const char*_tmp1C3="too many parameters for typedef %s";_tag_fat(_tmp1C3,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp363,_tmp362,_tag_fat(_tmp1C2,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 1166
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->hd=e,_tmp1C5->tl=hidden_ts;_tmp1C5;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({struct _tuple17*_tmp364=({struct _tuple17*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp1C6->f2=e;_tmp1C6;});_tmp1C7->hd=_tmp364;}),_tmp1C7->tl=inst;_tmp1C7;});}
# 1174
({struct Cyc_List_List*_tmp366=({struct Cyc_List_List*_tmp365=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp365,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp1EE=_tmp366;});}
# 1176
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp1EC=new_typ;}
# 1182
goto _LL0;};}}_LL0:;}
# 1184
if(!({struct Cyc_Absyn_Kind*_tmp367=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmp367,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp1F2=({struct Cyc_String_pa_PrintArg_struct _tmp273;_tmp273.tag=0U,({
struct _fat_ptr _tmp368=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp273.f1=_tmp368;});_tmp273;});struct Cyc_String_pa_PrintArg_struct _tmp1F3=({struct Cyc_String_pa_PrintArg_struct _tmp272;_tmp272.tag=0U,({struct _fat_ptr _tmp369=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_type_kind(t)));_tmp272.f1=_tmp369;});_tmp272;});struct Cyc_String_pa_PrintArg_struct _tmp1F4=({struct Cyc_String_pa_PrintArg_struct _tmp271;_tmp271.tag=0U,({struct _fat_ptr _tmp36A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmp271.f1=_tmp36A;});_tmp271;});void*_tmp1F0[3U];_tmp1F0[0]=& _tmp1F2,_tmp1F0[1]=& _tmp1F3,_tmp1F0[2]=& _tmp1F4;({unsigned _tmp36C=loc;struct _fat_ptr _tmp36B=({const char*_tmp1F1="type %s has kind %s but as used here needs kind %s";_tag_fat(_tmp1F1,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp36C,_tmp36B,_tag_fat(_tmp1F0,sizeof(void*),3U));});});
# 1188
return cvtenv;}
# 1196
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv){
# 1198
{void*_tmp1F5=e->r;void*_tmp1F6=_tmp1F5;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*_tmp208;void*_tmp207;void*_tmp206;void*_tmp205;void*_tmp204;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_Absyn_Exp*_tmp202;struct Cyc_Absyn_Exp*_tmp201;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_List_List*_tmp1F9;switch(*((int*)_tmp1F6)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp1F9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1F6)->f2;_LLC:
# 1205
 for(0;_tmp1F9 != 0;_tmp1F9=_tmp1F9->tl){
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp1F9->hd,te,cvtenv);}
goto _LL0;case 6U: _LLD: _tmp1FC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_tmp1FB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1F6)->f2;_tmp1FA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1F6)->f3;_LLE:
# 1209
 cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp1FC,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp1FB,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp1FA,te,cvtenv);
goto _LL0;case 7U: _LLF: _tmp1FE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_tmp1FD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1F6)->f2;_LL10:
 _tmp200=_tmp1FE;_tmp1FF=_tmp1FD;goto _LL12;case 8U: _LL11: _tmp200=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_tmp1FF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1F6)->f2;_LL12:
 _tmp202=_tmp200;_tmp201=_tmp1FF;goto _LL14;case 9U: _LL13: _tmp202=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_tmp201=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1F6)->f2;_LL14:
# 1216
 cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp202,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp201,te,cvtenv);
goto _LL0;case 14U: _LL15: _tmp204=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_tmp203=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1F6)->f2;_LL16:
# 1220
 cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp203,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp204,1,0);
goto _LL0;case 19U: _LL17: _tmp205=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_LL18:
 _tmp206=_tmp205;goto _LL1A;case 17U: _LL19: _tmp206=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_LL1A:
# 1225
 cvtenv=Cyc_Tctyp_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp206,1,0);
goto _LL0;case 39U: _LL1B: _tmp207=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_LL1C:
# 1228
 cvtenv=Cyc_Tctyp_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp207,1,0);
goto _LL0;case 18U: _LL1D: _tmp208=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_LL1E:
# 1231
 cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp208,te,cvtenv);
goto _LL0;case 41U: _LL1F: _tmp209=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp1F6)->f1;_LL20:
# 1234
 cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(_tmp209,te,cvtenv);
goto _LL0;default: _LL21: _LL22:
# 1237
({void*_tmp1F7=0U;({struct _fat_ptr _tmp36D=({const char*_tmp1F8="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_fat(_tmp1F8,sizeof(char),66U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp36D,_tag_fat(_tmp1F7,sizeof(void*),0U));});});}_LL0:;}
# 1239
return cvtenv;}
# 1242
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1247
struct Cyc_List_List*_tmp20A=cvt.kind_env;
cvt=Cyc_Tctyp_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 1250
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
({struct Cyc_List_List*_tmp36E=Cyc_Tctyp_fast_add_free_tvar(_tmp20A,(*((struct _tuple13*)vs->hd)).f1);cvt.kind_env=_tmp36E;});}}
# 1256
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple12 _tmp20B=*((struct _tuple12*)evs->hd);struct _tuple12 _tmp20C=_tmp20B;void*_tmp215;_LL1: _tmp215=_tmp20C.f1;_LL2:;{
void*_tmp20D=Cyc_Tcutil_compress(_tmp215);void*_tmp20E=_tmp20D;struct Cyc_Core_Opt**_tmp214;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20E)->tag == 1U){_LL4: _tmp214=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20E)->f4;_LL5:
# 1260
 if(*_tmp214 == 0)
({struct Cyc_Core_Opt*_tmp36F=({struct Cyc_Core_Opt*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F->v=_tmp20A;_tmp20F;});*_tmp214=_tmp36F;});else{
# 1264
struct Cyc_List_List*_tmp210=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp214))->v;
struct Cyc_List_List*_tmp211=0;
for(0;_tmp210 != 0;_tmp210=_tmp210->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp20A,(struct Cyc_Absyn_Tvar*)_tmp210->hd))
_tmp211=({struct Cyc_List_List*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->hd=(struct Cyc_Absyn_Tvar*)_tmp210->hd,_tmp212->tl=_tmp211;_tmp212;});}
({struct Cyc_Core_Opt*_tmp370=({struct Cyc_Core_Opt*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->v=_tmp211;_tmp213;});*_tmp214=_tmp370;});}
# 1271
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}}
# 1275
return cvt;}
# 1282
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
if(te->in_tempest && !te->tempest_generalize)generalize_evars=0;{
struct Cyc_List_List*_tmp216=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tctyp_CVTEnv _tmp217=({unsigned _tmp374=loc;struct Cyc_Tcenv_Tenv*_tmp373=te;struct Cyc_Tctyp_CVTEnv _tmp372=({struct Cyc_Tctyp_CVTEnv _tmp277;_tmp277.r=Cyc_Core_heap_region,_tmp277.kind_env=_tmp216,_tmp277.fn_result=0,_tmp277.generalize_evars=generalize_evars,_tmp277.free_vars=0,_tmp277.free_evars=0;_tmp277;});struct Cyc_Absyn_Kind*_tmp371=expected_kind;Cyc_Tctyp_check_valid_type(_tmp374,_tmp373,_tmp372,_tmp371,1,t);});
# 1291
struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple13*))Cyc_Core_fst,_tmp217.free_vars);
struct Cyc_List_List*_tmp219=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_fst,_tmp217.free_evars);
# 1295
if(_tmp216 != 0){
struct Cyc_List_List*_tmp21A=0;
{struct Cyc_List_List*_tmp21B=_tmp218;for(0;(unsigned)_tmp21B;_tmp21B=_tmp21B->tl){
struct Cyc_Absyn_Tvar*_tmp21C=(struct Cyc_Absyn_Tvar*)_tmp21B->hd;
int found=0;
{struct Cyc_List_List*_tmp21D=_tmp216;for(0;(unsigned)_tmp21D;_tmp21D=_tmp21D->tl){
if(Cyc_Absyn_tvar_cmp(_tmp21C,(struct Cyc_Absyn_Tvar*)_tmp21D->hd)== 0){found=1;break;}}}
if(!found)
_tmp21A=({struct Cyc_List_List*_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E->hd=(struct Cyc_Absyn_Tvar*)_tmp21B->hd,_tmp21E->tl=_tmp21A;_tmp21E;});}}
# 1305
_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp21A);}
# 1311
{struct Cyc_List_List*x=_tmp218;for(0;x != 0;x=x->tl){
void*_tmp21F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp220=_tmp21F;enum Cyc_Absyn_AliasQual _tmp22E;struct Cyc_Core_Opt**_tmp22D;struct Cyc_Absyn_Kind*_tmp22C;struct Cyc_Core_Opt**_tmp22B;struct Cyc_Core_Opt**_tmp22A;struct Cyc_Core_Opt**_tmp229;struct Cyc_Core_Opt**_tmp228;struct Cyc_Core_Opt**_tmp227;struct Cyc_Core_Opt**_tmp226;switch(*((int*)_tmp220)){case 1U: _LL1: _tmp226=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp220)->f1;_LL2:
 _tmp227=_tmp226;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp227=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f1;_LL4:
 _tmp228=_tmp227;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp228=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f1;_LL6:
 _tmp229=_tmp228;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp229=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f1;_LL8:
# 1317
({struct Cyc_Core_Opt*_tmp375=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp229=_tmp375;});goto _LL0;case Cyc_Absyn_Unique: _LL9: _tmp22A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f1;_LLA:
# 1319
({struct Cyc_Core_Opt*_tmp376=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp22A=_tmp376;});goto _LL0;default: goto _LLD;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp22B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f1;_LLC:
# 1321
({struct Cyc_Core_Opt*_tmp377=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp22B=_tmp377;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp22D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f1;_tmp22C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp220)->f2;_LLE:
# 1323
({struct Cyc_Core_Opt*_tmp378=Cyc_Tcutil_kind_to_bound_opt(_tmp22C);*_tmp22D=_tmp378;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp220)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp22E=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp220)->f1)->aliasqual;_LL10:
# 1325
({struct Cyc_String_pa_PrintArg_struct _tmp223=({struct Cyc_String_pa_PrintArg_struct _tmp275;_tmp275.tag=0U,({
struct _fat_ptr _tmp379=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmp275.f1=_tmp379;});_tmp275;});struct Cyc_String_pa_PrintArg_struct _tmp224=({struct Cyc_String_pa_PrintArg_struct _tmp274;_tmp274.tag=0U,({struct _fat_ptr _tmp37A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->kind=Cyc_Absyn_MemKind,_tmp225->aliasqual=_tmp22E;_tmp225;})));_tmp274.f1=_tmp37A;});_tmp274;});void*_tmp221[2U];_tmp221[0]=& _tmp223,_tmp221[1]=& _tmp224;({unsigned _tmp37C=loc;struct _fat_ptr _tmp37B=({const char*_tmp222="type variable %s cannot have kind %s";_tag_fat(_tmp222,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp37C,_tmp37B,_tag_fat(_tmp221,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 1332
if(_tmp218 != 0 || _tmp219 != 0){
{void*_tmp22F=Cyc_Tcutil_compress(t);void*_tmp230=_tmp22F;struct Cyc_List_List**_tmp231;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp230)->tag == 5U){_LL14: _tmp231=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp230)->f1).tvars;_LL15:
# 1335
 if(*_tmp231 == 0){
# 1337
({struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp218);*_tmp231=_tmp37D;});
_tmp218=0;}
# 1340
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 1343
if(_tmp218 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp234=({struct Cyc_String_pa_PrintArg_struct _tmp276;_tmp276.tag=0U,_tmp276.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)_tmp218->hd)->name);_tmp276;});void*_tmp232[1U];_tmp232[0]=& _tmp234;({unsigned _tmp37F=loc;struct _fat_ptr _tmp37E=({const char*_tmp233="unbound type variable %s";_tag_fat(_tmp233,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp37F,_tmp37E,_tag_fat(_tmp232,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(_tmp219,t,loc);}};}
# 1354
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1357
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_tmp235=Cyc_Tcutil_compress(t);void*_tmp236=_tmp235;struct Cyc_Absyn_FnInfo _tmp23A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp236)->tag == 5U){_LL1: _tmp23A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp236)->f1;_LL2: {
# 1360
struct Cyc_List_List*_tmp237=(fd->i).attributes;
fd->i=_tmp23A;
(fd->i).attributes=_tmp237;
({int _tmp380=
Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp23A.ret_tqual).print_const,_tmp23A.ret_type);
# 1363
((fd->i).ret_tqual).real_const=_tmp380;});
# 1365
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp238=0U;({struct _fat_ptr _tmp381=({const char*_tmp239="check_fndecl_valid_type: not a FnType";_tag_fat(_tmp239,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp381,_tag_fat(_tmp238,sizeof(void*),0U));});});}_LL0:;}
# 1368
({struct Cyc_List_List*_tmp383=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)((struct _fat_ptr*(*)(struct _tuple9*t))Cyc_Tctyp_fst_fdarg,(fd->i).args);unsigned _tmp382=loc;Cyc_Tcutil_check_unique_vars(_tmp383,_tmp382,({const char*_tmp23B="function declaration has repeated parameter";_tag_fat(_tmp23B,sizeof(char),44U);}));});
# 1371
fd->cached_type=t;}
# 1376
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1379
struct Cyc_Tctyp_CVTEnv _tmp23C=({unsigned _tmp388=loc;struct Cyc_Tcenv_Tenv*_tmp387=te;struct Cyc_Tctyp_CVTEnv _tmp386=({struct Cyc_Tctyp_CVTEnv _tmp27A;_tmp27A.r=Cyc_Core_heap_region,_tmp27A.kind_env=bound_tvars,_tmp27A.fn_result=0,_tmp27A.generalize_evars=0,_tmp27A.free_vars=0,_tmp27A.free_evars=0;_tmp27A;});struct Cyc_Absyn_Kind*_tmp385=expected_kind;int _tmp384=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_tmp388,_tmp387,_tmp386,_tmp385,_tmp384,t);});
# 1383
struct Cyc_List_List*_tmp23D=({struct _RegionHandle*_tmp38A=Cyc_Core_heap_region;struct Cyc_List_List*_tmp389=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple13*))Cyc_Core_fst,_tmp23C.free_vars);Cyc_Tctyp_remove_bound_tvars(_tmp38A,_tmp389,bound_tvars);});
# 1385
struct Cyc_List_List*_tmp23E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_fst,_tmp23C.free_evars);
{struct Cyc_List_List*fs=_tmp23D;for(0;fs != 0;fs=fs->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp241=({struct Cyc_String_pa_PrintArg_struct _tmp279;_tmp279.tag=0U,_tmp279.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)fs->hd)->name);_tmp279;});struct Cyc_String_pa_PrintArg_struct _tmp242=({struct Cyc_String_pa_PrintArg_struct _tmp278;_tmp278.tag=0U,({struct _fat_ptr _tmp38B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp278.f1=_tmp38B;});_tmp278;});void*_tmp23F[2U];_tmp23F[0]=& _tmp241,_tmp23F[1]=& _tmp242;({unsigned _tmp38D=loc;struct _fat_ptr _tmp38C=({const char*_tmp240="unbound type variable %s in type %s";_tag_fat(_tmp240,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp38D,_tmp38C,_tag_fat(_tmp23F,sizeof(void*),2U));});});}}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(_tmp23E,t,loc);}

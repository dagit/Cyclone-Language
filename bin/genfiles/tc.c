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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 98 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 882
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 888
void*Cyc_Absyn_compress_kb(void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 939
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 962
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
# 970
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1043
struct _tuple0*Cyc_Absyn_uniquergn_qvar();
# 1127
struct _fat_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 69
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 27 "unify.h"
void Cyc_Unify_explain_failure();
# 29
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
# 43
int Cyc_Tcutil_is_function_type(void*);
# 45
int Cyc_Tcutil_is_array_type(void*);
# 52
int Cyc_Tcutil_is_bound_one(void*);
# 57
int Cyc_Tcutil_is_bits_only_type(void*);
# 87
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 107
void*Cyc_Tcutil_compress(void*);
# 114
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 174
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 202
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 212
void Cyc_Tcutil_check_bitfield(unsigned,void*field_typ,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn);
# 215
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 236
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 246
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 261
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 269
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 276
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 280
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 297
int Cyc_Tcutil_zeroable_type(void*);
# 301
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 304
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83 "dict.h"
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned,struct Cyc_Absyn_Fndecl*);
# 78
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple0*);
# 82
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*);
# 88
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 138
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 151
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);struct _tuple10{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 54 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*msg);
# 57
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*msg);
# 59
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*msg);
# 66
void*Cyc_Tcdecl_merge_binding(void*,void*,unsigned,struct _fat_ptr*msg);
# 74
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _fat_ptr*v,unsigned,struct _fat_ptr*msg);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 92 "graph.h"
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict g);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 40
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);static char _tmp0[1U]="";
# 41 "tc.cyc"
static struct _fat_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1U};
static struct _fat_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 45
int Cyc_Tc_aggressive_warn=0;struct _tuple11{unsigned f1;struct _tuple0*f2;int f3;};
# 47
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple11*_tmp1=(struct _tuple11*)exports->hd;struct _tuple11*p=_tmp1;
if(Cyc_Absyn_qvar_cmp(x,(*p).f2)== 0){
# 53
(*p).f3=1;
return 1;}}
# 57
return 0;}
# 60
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned loc,int i,int nargs,void*att){
if(i < 1 || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp1D3;_tmp1D3.tag=0U,({struct _fat_ptr _tmp211=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string(att));_tmp1D3.f1=_tmp211;});_tmp1D3;});void*_tmp2[1U];_tmp2[0]=& _tmp4;({unsigned _tmp213=loc;struct _fat_ptr _tmp212=({const char*_tmp3="%s has an out-of-range index";_tag_fat(_tmp3,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp213,_tmp212,_tag_fat(_tmp2,sizeof(void*),1U));});});
return 0;}
# 65
return 1;}struct _tuple12{struct Cyc_List_List*f1;struct _fat_ptr f2;};
# 67
static void Cyc_Tc_fnTypeAttsOverlap(unsigned loc,int i,struct _tuple12 lst1,struct _tuple12 lst2){
# 70
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp1D6;_tmp1D6.tag=0U,_tmp1D6.f1=(struct _fat_ptr)((struct _fat_ptr)lst1.f2);_tmp1D6;});struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp1D5;_tmp1D5.tag=0U,_tmp1D5.f1=(struct _fat_ptr)((struct _fat_ptr)lst2.f2);_tmp1D5;});struct Cyc_Int_pa_PrintArg_struct _tmp9=({struct Cyc_Int_pa_PrintArg_struct _tmp1D4;_tmp1D4.tag=1U,_tmp1D4.f1=(unsigned long)i;_tmp1D4;});void*_tmp5[3U];_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8,_tmp5[2]=& _tmp9;({unsigned _tmp215=loc;struct _fat_ptr _tmp214=({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_fat(_tmp6,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp215,_tmp214,_tag_fat(_tmp5,sizeof(void*),3U));});});}
# 74
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned loc){
struct _tuple12 init_params=({struct _tuple12 _tmp1DD;_tmp1DD.f1=0,({struct _fat_ptr _tmp216=({const char*_tmp3A="initializes";_tag_fat(_tmp3A,sizeof(char),12U);});_tmp1DD.f2=_tmp216;});_tmp1DD;});
struct _tuple12 nolive_unique_params=({struct _tuple12 _tmp1DC;_tmp1DC.f1=0,({struct _fat_ptr _tmp217=({const char*_tmp39="noliveunique";_tag_fat(_tmp39,sizeof(char),13U);});_tmp1DC.f2=_tmp217;});_tmp1DC;});
struct _tuple12 consume_params=({struct _tuple12 _tmp1DB;_tmp1DB.f1=0,({struct _fat_ptr _tmp218=({const char*_tmp38="consume";_tag_fat(_tmp38,sizeof(char),8U);});_tmp1DB.f2=_tmp218;});_tmp1DB;});
void*_tmpA=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmpA;void*_tmpB=_stmttmp0;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpC;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->tag == 5U){_LL1: _tmpC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).attributes;_tmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).args;_LL2: {struct Cyc_List_List*atts=_tmpC;struct Cyc_List_List*args=_tmpD;
# 80
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);
for(0;atts != 0;atts=atts->tl){
void*_tmpE=(void*)atts->hd;void*_stmttmp1=_tmpE;void*_tmpF=_stmttmp1;int _tmp10;int _tmp11;int _tmp12;switch(*((int*)_tmpF)){case 20U: _LL6: _tmp12=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpF)->f1;_LL7: {int i=_tmp12;
# 84
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,i,init_params,consume_params);{
struct _tuple8*_tmp13=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp2=_tmp13;struct _tuple8*_tmp14=_stmttmp2;void*_tmp15;_LLF: _tmp15=_tmp14->f3;_LL10: {void*t=_tmp15;
struct _fat_ptr s=({const char*_tmp29="initializes attribute allowed only on";_tag_fat(_tmp29,sizeof(char),38U);});
{void*_tmp16=Cyc_Tcutil_compress(t);void*_stmttmp3=_tmp16;void*_tmp17=_stmttmp3;void*_tmp1B;void*_tmp1A;void*_tmp19;void*_tmp18;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17)->tag == 3U){_LL12: _tmp18=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17)->f1).elt_type;_tmp19=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17)->f1).ptr_atts).nullable;_tmp1A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17)->f1).ptr_atts).bounds;_tmp1B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17)->f1).ptr_atts).zero_term;_LL13: {void*t=_tmp18;void*nullable=_tmp19;void*bd=_tmp1A;void*zt=_tmp1B;
# 91
if(Cyc_Tcutil_force_type2bool(0,nullable))
({struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp1D7;_tmp1D7.tag=0U,_tmp1D7.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp1D7;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1E;({unsigned _tmp21A=loc;struct _fat_ptr _tmp219=({const char*_tmp1D="%s non-null pointers";_tag_fat(_tmp1D,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp21A,_tmp219,_tag_fat(_tmp1C,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_bound_one(bd))
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp1D8;_tmp1D8.tag=0U,_tmp1D8.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp1D8;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({unsigned _tmp21C=loc;struct _fat_ptr _tmp21B=({const char*_tmp20="%s pointers of size 1";_tag_fat(_tmp20,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp21C,_tmp21B,_tag_fat(_tmp1F,sizeof(void*),1U));});});
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp1D9;_tmp1D9.tag=0U,_tmp1D9.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp1D9;});void*_tmp22[1U];_tmp22[0]=& _tmp24;({unsigned _tmp21E=loc;struct _fat_ptr _tmp21D=({const char*_tmp23="%s pointers to non-zero-terminated arrays";_tag_fat(_tmp23,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp21E,_tmp21D,_tag_fat(_tmp22,sizeof(void*),1U));});});
goto _LL11;}}else{_LL14: _LL15:
# 99
({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp1DA;_tmp1DA.tag=0U,_tmp1DA.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp1DA;});void*_tmp25[1U];_tmp25[0]=& _tmp27;({unsigned _tmp220=loc;struct _fat_ptr _tmp21F=({const char*_tmp26="%s pointers";_tag_fat(_tmp26,sizeof(char),12U);});Cyc_Tcutil_terr(_tmp220,_tmp21F,_tag_fat(_tmp25,sizeof(void*),1U));});});}_LL11:;}
# 101
({struct Cyc_List_List*_tmp221=({struct Cyc_List_List*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->hd=(void*)i,_tmp28->tl=init_params.f1;_tmp28;});init_params.f1=_tmp221;});
goto _LL5;}}}case 21U: _LL8: _tmp11=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpF)->f1;_LL9: {int i=_tmp11;
# 104
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,nolive_unique_params,init_params);{
# 107
struct _tuple8*_tmp2A=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp4=_tmp2A;struct _tuple8*_tmp2B=_stmttmp4;void*_tmp2C;_LL17: _tmp2C=_tmp2B->f3;_LL18: {void*t=_tmp2C;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp2D=0U;({unsigned _tmp223=loc;struct _fat_ptr _tmp222=({const char*_tmp2E="noliveunique attribute allowed only on unique pointers";_tag_fat(_tmp2E,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp223,_tmp222,_tag_fat(_tmp2D,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp224=({struct Cyc_List_List*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->hd=(void*)i,_tmp2F->tl=nolive_unique_params.f1;_tmp2F;});nolive_unique_params.f1=_tmp224;});
goto _LL5;}}}case 22U: _LLA: _tmp10=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpF)->f1;_LLB: {int i=_tmp10;
# 113
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,consume_params,init_params);{
# 116
struct _tuple8*_tmp30=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp5=_tmp30;struct _tuple8*_tmp31=_stmttmp5;void*_tmp32;_LL1A: _tmp32=_tmp31->f3;_LL1B: {void*t=_tmp32;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp33=0U;({unsigned _tmp226=loc;struct _fat_ptr _tmp225=({const char*_tmp34="consume attribute allowed only on unique pointers";_tag_fat(_tmp34,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp226,_tmp225,_tag_fat(_tmp33,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp227=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=(void*)i,_tmp35->tl=consume_params.f1;_tmp35;});consume_params.f1=_tmp227;});
goto _LL5;}}}default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 124
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp36=0U;({struct _fat_ptr _tmp228=({const char*_tmp37="fnTypeAttsOK: not a function type";_tag_fat(_tmp37,sizeof(char),34U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp228,_tag_fat(_tmp36,sizeof(void*),0U));});});}_LL0:;}struct _tuple13{void*f1;int f2;};
# 129
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 131
struct Cyc_Absyn_Vardecl*_tmp3B=vd;struct Cyc_List_List*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;void*_tmp3E;struct _tuple0*_tmp3D;enum Cyc_Absyn_Scope _tmp3C;_LL1: _tmp3C=_tmp3B->sc;_tmp3D=_tmp3B->name;_tmp3E=_tmp3B->type;_tmp3F=_tmp3B->initializer;_tmp40=_tmp3B->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp3C;struct _tuple0*q=_tmp3D;void*t=_tmp3E;struct Cyc_Absyn_Exp*initializer=_tmp3F;struct Cyc_List_List*atts=_tmp40;
# 138
{void*_tmp41=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmp41;void*_tmp42=_stmttmp6;unsigned _tmp46;void*_tmp45;struct Cyc_Absyn_Tqual _tmp44;void*_tmp43;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42)->tag == 4U){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42)->f1).num_elts == 0){_LL4: _tmp43=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42)->f1).elt_type;_tmp44=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42)->f1).tq;_tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42)->f1).zero_term;_tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42)->f1).zt_loc;if(initializer != 0){_LL5: {void*telt=_tmp43;struct Cyc_Absyn_Tqual tq=_tmp44;void*zt=_tmp45;unsigned ztl=_tmp46;
# 140
{void*_tmp47=initializer->r;void*_stmttmp7=_tmp47;void*_tmp48=_stmttmp7;struct Cyc_List_List*_tmp49;struct Cyc_List_List*_tmp4A;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Exp*_tmp4C;struct _fat_ptr _tmp4D;struct _fat_ptr _tmp4E;switch(*((int*)_tmp48)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp48)->f1).Wstring_c).tag){case 8U: _LL9: _tmp4E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp48)->f1).String_c).val;_LLA: {struct _fat_ptr s=_tmp4E;
# 142
t=({void*_tmp22D=({void*_tmp22C=telt;struct Cyc_Absyn_Tqual _tmp22B=tq;struct Cyc_Absyn_Exp*_tmp22A=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_tmp229=zt;Cyc_Absyn_array_type(_tmp22C,_tmp22B,_tmp22A,_tmp229,ztl);});vd->type=_tmp22D;});
goto _LL8;}case 9U: _LLB: _tmp4D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp48)->f1).Wstring_c).val;_LLC: {struct _fat_ptr s=_tmp4D;
# 145
t=({void*_tmp232=({void*_tmp231=telt;struct Cyc_Absyn_Tqual _tmp230=tq;struct Cyc_Absyn_Exp*_tmp22F=Cyc_Absyn_uint_exp(1U,0U);void*_tmp22E=zt;Cyc_Absyn_array_type(_tmp231,_tmp230,_tmp22F,_tmp22E,ztl);});vd->type=_tmp232;});
goto _LL8;}default: goto _LL15;}case 27U: _LLD: _tmp4C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp48)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp4C;
_tmp4B=e;goto _LL10;}case 28U: _LLF: _tmp4B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp48)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp4B;
# 150
t=({void*_tmp233=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_tmp233;});
goto _LL8;}case 36U: _LL11: _tmp4A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp48)->f2;_LL12: {struct Cyc_List_List*es=_tmp4A;
_tmp49=es;goto _LL14;}case 26U: _LL13: _tmp49=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp48)->f1;_LL14: {struct Cyc_List_List*es=_tmp49;
# 154
t=({void*_tmp238=({void*_tmp237=telt;struct Cyc_Absyn_Tqual _tmp236=tq;struct Cyc_Absyn_Exp*_tmp235=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);void*_tmp234=zt;Cyc_Absyn_array_type(_tmp237,_tmp236,_tmp235,_tmp234,ztl);});vd->type=_tmp238;});
goto _LL8;}default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 158
goto _LL3;}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 162
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
# 164
({int _tmp239=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp239;});
# 167
({int _tmp23A=!Cyc_Tcutil_is_array_type(t);vd->escapes=_tmp23A;});
# 169
if(Cyc_Tcutil_is_function_type(t)){
atts=Cyc_Tcutil_transfer_fn_type_atts(t,atts);
Cyc_Tc_fnTypeAttsOK(te,t,loc);}
# 174
if((int)sc == (int)3U ||(int)sc == (int)4U){
if(initializer != 0 && !in_cinclude)
({void*_tmp4F=0U;({unsigned _tmp23C=loc;struct _fat_ptr _tmp23B=({const char*_tmp50="extern declaration should not have initializer";_tag_fat(_tmp50,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp23C,_tmp23B,_tag_fat(_tmp4F,sizeof(void*),0U));});});}else{
if(!Cyc_Tcutil_is_function_type(t)){
# 181
for(0;atts != 0;atts=atts->tl){
void*_tmp51=(void*)atts->hd;void*_stmttmp8=_tmp51;void*_tmp52=_stmttmp8;switch(*((int*)_tmp52)){case 6U: _LL18: _LL19:
 goto _LL1B;case 8U: _LL1A: _LL1B:
# 186
 goto _LL1D;case 9U: _LL1C: _LL1D:
 goto _LL1F;case 10U: _LL1E: _LL1F:
 goto _LL21;case 11U: _LL20: _LL21:
 goto _LL23;case 12U: _LL22: _LL23:
 goto _LL25;case 13U: _LL24: _LL25:
 goto _LL27;case 14U: _LL26: _LL27:
 continue;default: _LL28: _LL29:
# 194
({struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp1DF;_tmp1DF.tag=0U,({
struct _fat_ptr _tmp23D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1DF.f1=_tmp23D;});_tmp1DF;});struct Cyc_String_pa_PrintArg_struct _tmp56=({struct Cyc_String_pa_PrintArg_struct _tmp1DE;_tmp1DE.tag=0U,({struct _fat_ptr _tmp23E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1DE.f1=_tmp23E;});_tmp1DE;});void*_tmp53[2U];_tmp53[0]=& _tmp55,_tmp53[1]=& _tmp56;({unsigned _tmp240=loc;struct _fat_ptr _tmp23F=({const char*_tmp54="bad attribute %s for variable %s";_tag_fat(_tmp54,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp240,_tmp23F,_tag_fat(_tmp53,sizeof(void*),2U));});});
goto _LL17;}_LL17:;}
# 199
if(initializer == 0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(t))
({struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0U,({
struct _fat_ptr _tmp241=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1E1.f1=_tmp241;});_tmp1E1;});struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp1E0;_tmp1E0.tag=0U,({struct _fat_ptr _tmp242=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp1E0.f1=_tmp242;});_tmp1E0;});void*_tmp57[2U];_tmp57[0]=& _tmp59,_tmp57[1]=& _tmp5A;({unsigned _tmp244=loc;struct _fat_ptr _tmp243=({const char*_tmp58="initializer required for variable %s of type %s";_tag_fat(_tmp58,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp244,_tmp243,_tag_fat(_tmp57,sizeof(void*),2U));});});}else{
# 204
struct Cyc_Absyn_Exp*_tmp5B=initializer;struct Cyc_Absyn_Exp*e=_tmp5B;
# 208
struct _handler_cons _tmp5C;_push_handler(& _tmp5C);{int _tmp5E=0;if(setjmp(_tmp5C.handler))_tmp5E=1;if(!_tmp5E){
{void*_tmp5F=Cyc_Tcexp_tcExpInitializer(te,& t,e);void*t2=_tmp5F;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp246=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp245=e;Cyc_Tcutil_coerce_assign(_tmp246,_tmp245,t);})){
struct _fat_ptr _tmp60=Cyc_Absynpp_qvar2string(vd->name);struct _fat_ptr s0=_tmp60;
const char*_tmp61=" declared with type ";const char*s1=_tmp61;
struct _fat_ptr _tmp62=Cyc_Absynpp_typ2string(t);struct _fat_ptr s2=_tmp62;
const char*_tmp63=" but initialized with type ";const char*s3=_tmp63;
struct _fat_ptr _tmp64=Cyc_Absynpp_typ2string(t2);struct _fat_ptr s4=_tmp64;
if(({unsigned long _tmp24A=({unsigned long _tmp249=({unsigned long _tmp248=({unsigned long _tmp247=Cyc_strlen((struct _fat_ptr)s0);_tmp247 + Cyc_strlen(({const char*_tmp65=s1;_tag_fat(_tmp65,sizeof(char),21U);}));});_tmp248 + Cyc_strlen((struct _fat_ptr)s2);});_tmp249 + Cyc_strlen(({const char*_tmp66=s3;_tag_fat(_tmp66,sizeof(char),28U);}));});_tmp24A + Cyc_strlen((struct _fat_ptr)s4);})> (unsigned long)70)
({struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0U,_tmp1E6.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp1E6;});struct Cyc_String_pa_PrintArg_struct _tmp6A=({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0U,({struct _fat_ptr _tmp24B=(struct _fat_ptr)({const char*_tmp6F=s1;_tag_fat(_tmp6F,sizeof(char),21U);});_tmp1E5.f1=_tmp24B;});_tmp1E5;});struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp1E4;_tmp1E4.tag=0U,_tmp1E4.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1E4;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp1E3;_tmp1E3.tag=0U,({struct _fat_ptr _tmp24C=(struct _fat_ptr)({const char*_tmp6E=s3;_tag_fat(_tmp6E,sizeof(char),28U);});_tmp1E3.f1=_tmp24C;});_tmp1E3;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp1E2;_tmp1E2.tag=0U,_tmp1E2.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp1E2;});void*_tmp67[5U];_tmp67[0]=& _tmp69,_tmp67[1]=& _tmp6A,_tmp67[2]=& _tmp6B,_tmp67[3]=& _tmp6C,_tmp67[4]=& _tmp6D;({unsigned _tmp24E=loc;struct _fat_ptr _tmp24D=({const char*_tmp68="%s%s\n\t%s\n%s\n\t%s";_tag_fat(_tmp68,sizeof(char),16U);});Cyc_Tcutil_terr(_tmp24E,_tmp24D,_tag_fat(_tmp67,sizeof(void*),5U));});});else{
# 219
({struct Cyc_String_pa_PrintArg_struct _tmp72=({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0U,_tmp1EB.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp1EB;});struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0U,({struct _fat_ptr _tmp24F=(struct _fat_ptr)({const char*_tmp78=s1;_tag_fat(_tmp78,sizeof(char),21U);});_tmp1EA.f1=_tmp24F;});_tmp1EA;});struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0U,_tmp1E9.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1E9;});struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp1E8;_tmp1E8.tag=0U,({struct _fat_ptr _tmp250=(struct _fat_ptr)({const char*_tmp77=s3;_tag_fat(_tmp77,sizeof(char),28U);});_tmp1E8.f1=_tmp250;});_tmp1E8;});struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0U,_tmp1E7.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp1E7;});void*_tmp70[5U];_tmp70[0]=& _tmp72,_tmp70[1]=& _tmp73,_tmp70[2]=& _tmp74,_tmp70[3]=& _tmp75,_tmp70[4]=& _tmp76;({unsigned _tmp252=loc;struct _fat_ptr _tmp251=({const char*_tmp71="%s%s%s%s%s";_tag_fat(_tmp71,sizeof(char),11U);});Cyc_Tcutil_terr(_tmp252,_tmp251,_tag_fat(_tmp70,sizeof(void*),5U));});});}
Cyc_Unify_explain_failure();}
# 223
if(!Cyc_Tcutil_is_const_exp(e))
({void*_tmp79=0U;({unsigned _tmp254=loc;struct _fat_ptr _tmp253=({const char*_tmp7A="initializer is not a constant expression";_tag_fat(_tmp7A,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp254,_tmp253,_tag_fat(_tmp79,sizeof(void*),0U));});});}
# 209
;_pop_handler();}else{void*_tmp5D=(void*)Cyc_Core_get_exn_thrown();void*_tmp7B=_tmp5D;void*_tmp7C;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp7B)->tag == Cyc_Tcenv_Env_error){_LL2B: _LL2C:
# 227
({void*_tmp7D=0U;({unsigned _tmp256=loc;struct _fat_ptr _tmp255=({const char*_tmp7E="initializer is not a constant expression";_tag_fat(_tmp7E,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp256,_tmp255,_tag_fat(_tmp7D,sizeof(void*),0U));});});
goto _LL2A;}else{_LL2D: _tmp7C=_tmp7B;_LL2E: {void*exn=_tmp7C;(int)_rethrow(exn);}}_LL2A:;}}}}else{
# 233
for(0;atts != 0;atts=atts->tl){
void*_tmp7F=(void*)atts->hd;void*_stmttmp9=_tmp7F;void*_tmp80=_stmttmp9;switch(*((int*)_tmp80)){case 0U: _LL30: _LL31:
# 236
 goto _LL33;case 1U: _LL32: _LL33:
 goto _LL35;case 2U: _LL34: _LL35:
 goto _LL37;case 3U: _LL36: _LL37:
 goto _LL39;case 4U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 goto _LL3D;case 20U: _LL3C: _LL3D:
 goto _LL3F;case 23U: _LL3E: _LL3F:
 goto _LL41;case 26U: _LL40: _LL41:
 goto _LL43;case 5U: _LL42: _LL43:
({void*_tmp81=0U;({struct _fat_ptr _tmp257=({const char*_tmp82="tcVardecl: fn type atts in function var decl";_tag_fat(_tmp82,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp257,_tag_fat(_tmp81,sizeof(void*),0U));});});case 6U: _LL44: _LL45:
# 247
 goto _LL47;case 7U: _LL46: _LL47:
# 249
({struct Cyc_String_pa_PrintArg_struct _tmp85=({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0U,({
struct _fat_ptr _tmp258=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1EC.f1=_tmp258;});_tmp1EC;});void*_tmp83[1U];_tmp83[0]=& _tmp85;({unsigned _tmp25A=loc;struct _fat_ptr _tmp259=({const char*_tmp84="bad attribute %s in function declaration";_tag_fat(_tmp84,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp25A,_tmp259,_tag_fat(_tmp83,sizeof(void*),1U));});});
goto _LL2F;default: _LL48: _LL49:
 continue;}_LL2F:;}}}
# 257
{struct _handler_cons _tmp86;_push_handler(& _tmp86);{int _tmp88=0;if(setjmp(_tmp86.handler))_tmp88=1;if(!_tmp88){
{struct _tuple13*_tmp89=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple13*ans=_tmp89;
void*_tmp8A=(*ans).f1;void*b0=_tmp8A;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8B=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=1U,_tmp8E->f1=vd;_tmp8E;});struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1=_tmp8B;
void*_tmp8C=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmp8C;
if(b == 0){_npop_handler(0U);return;}
# 264
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports)){
if(b != b0 ||(*ans).f2)
# 267
({struct Cyc_Dict_Dict _tmp25D=({struct Cyc_Dict_Dict _tmp25C=(te->ae)->ordinaries;struct _tuple0*_tmp25B=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp25C,_tmp25B,({struct _tuple13*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D->f1=b,_tmp8D->f2=(*ans).f2;_tmp8D;}));});
# 267
(te->ae)->ordinaries=_tmp25D;});}
# 269
_npop_handler(0U);return;}
# 258
;_pop_handler();}else{void*_tmp87=(void*)Cyc_Core_get_exn_thrown();void*_tmp8F=_tmp87;void*_tmp90;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8F)->tag == Cyc_Dict_Absent){_LL4B: _LL4C:
# 270
 goto _LL4A;}else{_LL4D: _tmp90=_tmp8F;_LL4E: {void*exn=_tmp90;(int)_rethrow(exn);}}_LL4A:;}}}
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp261=({struct Cyc_Dict_Dict _tmp260=(te->ae)->ordinaries;struct _tuple0*_tmp25F=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp260,_tmp25F,({struct _tuple13*_tmp92=_cycalloc(sizeof(*_tmp92));
({void*_tmp25E=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=1U,_tmp91->f1=vd;_tmp91;});_tmp92->f1=_tmp25E;}),_tmp92->f2=0;_tmp92;}));});
# 272
(te->ae)->ordinaries=_tmp261;});}}
# 276
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp93=n;struct _fat_ptr*_tmp95;union Cyc_Absyn_Nmspace _tmp94;_LL1: _tmp94=_tmp93->f1;_tmp95=_tmp93->f2;_LL2: {union Cyc_Absyn_Nmspace nms=_tmp94;struct _fat_ptr*v=_tmp95;
union Cyc_Absyn_Nmspace _tmp96=nms;if((_tmp96.Abs_n).tag == 2){if((_tmp96.Abs_n).val == 0){_LL4: _LL5:
# 280
 return({struct _fat_ptr _tmp262=(struct _fat_ptr)*v;Cyc_strcmp(_tmp262,({const char*_tmp97="main";_tag_fat(_tmp97,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;}}
# 285
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 287
struct _tuple0*q=fd->name;
# 291
if((int)fd->sc == (int)Cyc_Absyn_ExternC && !te->in_extern_c_include)
({void*_tmp98=0U;({unsigned _tmp264=loc;struct _fat_ptr _tmp263=({const char*_tmp99="extern \"C\" functions cannot be implemented in Cyclone";_tag_fat(_tmp99,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp264,_tmp263,_tag_fat(_tmp98,sizeof(void*),0U));});});
# 295
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp9A=(void*)atts->hd;void*_stmttmpA=_tmp9A;void*_tmp9B=_stmttmpA;switch(*((int*)_tmp9B)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
# 299
({struct Cyc_String_pa_PrintArg_struct _tmp9E=({struct Cyc_String_pa_PrintArg_struct _tmp1ED;_tmp1ED.tag=0U,({struct _fat_ptr _tmp265=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1ED.f1=_tmp265;});_tmp1ED;});void*_tmp9C[1U];_tmp9C[0]=& _tmp9E;({unsigned _tmp267=loc;struct _fat_ptr _tmp266=({const char*_tmp9D="bad attribute %s for function";_tag_fat(_tmp9D,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp267,_tmp266,_tag_fat(_tmp9C,sizeof(void*),1U));});});
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}}
# 305
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 308
({struct Cyc_List_List*_tmp268=Cyc_Tcutil_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp268;});
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 312
{struct _handler_cons _tmp9F;_push_handler(& _tmp9F);{int _tmpA1=0;if(setjmp(_tmp9F.handler))_tmpA1=1;if(!_tmpA1){
{struct _tuple13*_tmpA2=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple13*ans=_tmpA2;
void*_tmpA3=(*ans).f1;void*b0=_tmpA3;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA4=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=2U,_tmpA7->f1=fd;_tmpA7;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1=_tmpA4;
void*_tmpA5=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmpA5;
if(b != 0){
# 319
if(exports == 0 || Cyc_Tc_export_member(q,*exports)){
if(!(b == b0 &&(*ans).f2))
({struct Cyc_Dict_Dict _tmp26B=({struct Cyc_Dict_Dict _tmp26A=(te->ae)->ordinaries;struct _tuple0*_tmp269=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp26A,_tmp269,({struct _tuple13*_tmpA6=_cycalloc(sizeof(*_tmpA6));
_tmpA6->f1=b,_tmpA6->f2=(*ans).f2;_tmpA6;}));});
# 321
(te->ae)->ordinaries=_tmp26B;});}}}
# 313
;_pop_handler();}else{void*_tmpA0=(void*)Cyc_Core_get_exn_thrown();void*_tmpA8=_tmpA0;void*_tmpA9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA8)->tag == Cyc_Dict_Absent){_LL8: _LL9:
# 325
 if(exports == 0 || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAA=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=2U,_tmpAD->f1=fd;_tmpAD;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b=_tmpAA;
({struct Cyc_Dict_Dict _tmp26F=({struct Cyc_Dict_Dict _tmp26E=(te->ae)->ordinaries;struct _tuple0*_tmp26D=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp26E,_tmp26D,({struct _tuple13*_tmpAC=_cycalloc(sizeof(*_tmpAC));
({void*_tmp26C=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=2U,_tmpAB->f1=fd;_tmpAB;});_tmpAC->f1=_tmp26C;}),_tmpAC->f2=0;_tmpAC;}));});
# 327
(te->ae)->ordinaries=_tmp26F;});}
# 330
goto _LL7;}else{_LLA: _tmpA9=_tmpA8;_LLB: {void*exn=_tmpA9;(int)_rethrow(exn);}}_LL7:;}}}
# 334
if(te->in_extern_c_include)return;{
# 339
struct Cyc_Tcenv_Fenv*_tmpAE=Cyc_Tcenv_new_fenv(loc,fd);struct Cyc_Tcenv_Fenv*fenv=_tmpAE;
struct Cyc_Tcenv_Tenv*_tmpAF=({struct Cyc_Tcenv_Tenv*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->ns=te->ns,_tmpD3->ae=te->ae,_tmpD3->le=fenv,_tmpD3->allow_valueof=0,_tmpD3->in_extern_c_include=0,_tmpD3->in_tempest=te->in_tempest,_tmpD3->tempest_generalize=te->tempest_generalize;_tmpD3;});struct Cyc_Tcenv_Tenv*te=_tmpAF;
# 342
Cyc_Tcstmt_tcStmt(te,fd->body,0);
# 345
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
# 348
if(te->in_tempest){
te->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);
te->tempest_generalize=0;}
# 354
if(Cyc_Tc_is_main(q)){
# 356
{void*_tmpB0=Cyc_Tcutil_compress((fd->i).ret_type);void*_stmttmpB=_tmpB0;void*_tmpB1=_stmttmpB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB1)->f1)){case 0U: _LLD: _LLE:
({void*_tmpB2=0U;({unsigned _tmp271=loc;struct _fat_ptr _tmp270=({const char*_tmpB3="main declared with return type void";_tag_fat(_tmpB3,sizeof(char),36U);});Cyc_Tcutil_warn(_tmp271,_tmp270,_tag_fat(_tmpB2,sizeof(void*),0U));});});goto _LLC;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB1)->f1)->f2){case Cyc_Absyn_Int_sz: _LLF: _LL10:
 goto _LLC;case Cyc_Absyn_Long_sz: _LL11: _LL12:
 goto _LLC;default: goto _LL13;}default: goto _LL13;}else{_LL13: _LL14:
# 361
({struct Cyc_String_pa_PrintArg_struct _tmpB6=({struct Cyc_String_pa_PrintArg_struct _tmp1EE;_tmp1EE.tag=0U,({
struct _fat_ptr _tmp272=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((fd->i).ret_type));_tmp1EE.f1=_tmp272;});_tmp1EE;});void*_tmpB4[1U];_tmpB4[0]=& _tmpB6;({unsigned _tmp274=loc;struct _fat_ptr _tmp273=({const char*_tmpB5="main declared with return type %s instead of int or void";_tag_fat(_tmpB5,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp274,_tmp273,_tag_fat(_tmpB4,sizeof(void*),1U));});});
goto _LLC;}_LLC:;}
# 365
if((fd->i).c_varargs ||(fd->i).cyc_varargs != 0)
({void*_tmpB7=0U;({unsigned _tmp276=loc;struct _fat_ptr _tmp275=({const char*_tmpB8="main declared with varargs";_tag_fat(_tmpB8,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp276,_tmp275,_tag_fat(_tmpB7,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpB9=(fd->i).args;struct Cyc_List_List*args=_tmpB9;
if(args != 0){
struct _tuple8*_tmpBA=(struct _tuple8*)args->hd;struct _tuple8*_stmttmpC=_tmpBA;struct _tuple8*_tmpBB=_stmttmpC;void*_tmpBC;_LL16: _tmpBC=_tmpBB->f3;_LL17: {void*t1=_tmpBC;
{void*_tmpBD=Cyc_Tcutil_compress(t1);void*_stmttmpD=_tmpBD;void*_tmpBE=_stmttmpD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->f1)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->f1)->f2){case Cyc_Absyn_Int_sz: _LL19: _LL1A:
 goto _LL18;case Cyc_Absyn_Long_sz: _LL1B: _LL1C:
 goto _LL18;default: goto _LL1D;}else{goto _LL1D;}}else{_LL1D: _LL1E:
# 374
({struct Cyc_String_pa_PrintArg_struct _tmpC1=({struct Cyc_String_pa_PrintArg_struct _tmp1EF;_tmp1EF.tag=0U,({
struct _fat_ptr _tmp277=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp1EF.f1=_tmp277;});_tmp1EF;});void*_tmpBF[1U];_tmpBF[0]=& _tmpC1;({unsigned _tmp279=loc;struct _fat_ptr _tmp278=({const char*_tmpC0="main declared with first argument of type %s instead of int";_tag_fat(_tmpC0,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp279,_tmp278,_tag_fat(_tmpBF,sizeof(void*),1U));});});
goto _LL18;}_LL18:;}
# 378
args=args->tl;
if(args != 0){
struct _tuple8*_tmpC2=(struct _tuple8*)args->hd;struct _tuple8*_stmttmpE=_tmpC2;struct _tuple8*_tmpC3=_stmttmpE;void*_tmpC4;_LL20: _tmpC4=_tmpC3->f3;_LL21: {void*t2=_tmpC4;
args=args->tl;
if(args != 0)
({void*_tmpC5=0U;({unsigned _tmp27B=loc;struct _fat_ptr _tmp27A=({const char*_tmpC6="main declared with too many arguments";_tag_fat(_tmpC6,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp27B,_tmp27A,_tag_fat(_tmpC5,sizeof(void*),0U));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=(fd->i).tvars;_tmpD2;});
if(((!({void*_tmp293=t2;Cyc_Unify_unify(_tmp293,({void*_tmp292=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp28E=({struct Cyc_Core_Opt*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->v=& Cyc_Tcutil_rk;_tmpC7;});Cyc_Absyn_new_evar(_tmp28E,tvs);}));void*_tmp291=({
struct Cyc_Core_Opt*_tmp28F=({struct Cyc_Core_Opt*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->v=& Cyc_Tcutil_rk;_tmpC8;});Cyc_Absyn_new_evar(_tmp28F,tvs);});
# 385
struct Cyc_Absyn_Tqual _tmp290=
# 387
Cyc_Absyn_empty_tqual(0U);
# 385
Cyc_Absyn_fatptr_type(_tmp292,_tmp291,_tmp290,
# 387
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));})&& !({
void*_tmp28D=t2;Cyc_Unify_unify(_tmp28D,({void*_tmp28C=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp288=({struct Cyc_Core_Opt*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->v=& Cyc_Tcutil_rk;_tmpC9;});Cyc_Absyn_new_evar(_tmp288,tvs);}));void*_tmp28B=({
# 390
struct Cyc_Core_Opt*_tmp289=({struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=& Cyc_Tcutil_rk;_tmpCA;});Cyc_Absyn_new_evar(_tmp289,tvs);});
# 388
struct Cyc_Absyn_Tqual _tmp28A=
# 391
Cyc_Absyn_empty_tqual(0U);
# 388
Cyc_Absyn_fatptr_type(_tmp28C,_tmp28B,_tmp28A,
# 391
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp287=t2;Cyc_Unify_unify(_tmp287,({void*_tmp286=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp282=({struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->v=& Cyc_Tcutil_rk;_tmpCB;});Cyc_Absyn_new_evar(_tmp282,tvs);}));void*_tmp285=({
struct Cyc_Core_Opt*_tmp283=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=& Cyc_Tcutil_rk;_tmpCC;});Cyc_Absyn_new_evar(_tmp283,tvs);});
# 392
struct Cyc_Absyn_Tqual _tmp284=
# 394
Cyc_Absyn_const_tqual(0U);
# 392
Cyc_Absyn_fatptr_type(_tmp286,_tmp285,_tmp284,
# 394
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp281=t2;Cyc_Unify_unify(_tmp281,({void*_tmp280=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp27C=({struct Cyc_Core_Opt*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->v=& Cyc_Tcutil_rk;_tmpCD;});Cyc_Absyn_new_evar(_tmp27C,tvs);}));void*_tmp27F=({
# 397
struct Cyc_Core_Opt*_tmp27D=({struct Cyc_Core_Opt*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->v=& Cyc_Tcutil_rk;_tmpCE;});Cyc_Absyn_new_evar(_tmp27D,tvs);});
# 395
struct Cyc_Absyn_Tqual _tmp27E=
# 398
Cyc_Absyn_const_tqual(0U);
# 395
Cyc_Absyn_fatptr_type(_tmp280,_tmp27F,_tmp27E,
# 398
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))
({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0U,({
struct _fat_ptr _tmp294=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp1F0.f1=_tmp294;});_tmp1F0;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned _tmp296=loc;struct _fat_ptr _tmp295=({const char*_tmpD0="second argument of main has type %s instead of char??";_tag_fat(_tmpD0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp296,_tmp295,_tag_fat(_tmpCF,sizeof(void*),1U));});});}}}}}}}}}}
# 407
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 414
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0U,_tmp1F1.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp1F1;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({unsigned _tmp298=loc;struct _fat_ptr _tmp297=({const char*_tmpD5="redeclaration of typedef %s";_tag_fat(_tmpD5,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp298,_tmp297,_tag_fat(_tmpD4,sizeof(void*),1U));});});
return;}
# 419
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tctyp_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp299=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 423
(td->tq).real_const=_tmp299;});}
# 428
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpD7=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_stmttmpF=_tmpD7;void*_tmpD8=_stmttmpF;struct Cyc_Absyn_Kind*_tmpDA;struct Cyc_Core_Opt**_tmpD9;struct Cyc_Core_Opt**_tmpDB;switch(*((int*)_tmpD8)){case 1U: _LL1: _tmpDB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD8)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpDB;
# 431
if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0U,_tmp1F2.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);_tmp1F2;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({unsigned _tmp29B=loc;struct _fat_ptr _tmp29A=({const char*_tmpDD="type variable %s is not used in typedef definition";_tag_fat(_tmpDD,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp29B,_tmp29A,_tag_fat(_tmpDC,sizeof(void*),1U));});});
# 434
({struct Cyc_Core_Opt*_tmp29D=({struct Cyc_Core_Opt*_tmpDF=_cycalloc(sizeof(*_tmpDF));({void*_tmp29C=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpDF->v=_tmp29C;});_tmpDF;});*f=_tmp29D;});goto _LL0;}case 2U: _LL3: _tmpD9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD8)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmpD9;struct Cyc_Absyn_Kind*k=_tmpDA;
# 441
({struct Cyc_Core_Opt*_tmp29F=({struct Cyc_Core_Opt*_tmpE0=_cycalloc(sizeof(*_tmpE0));({void*_tmp29E=Cyc_Tcutil_kind_to_bound(k);_tmpE0->v=_tmp29E;});_tmpE0;});*f=_tmp29F;});
goto _LL0;}default: _LL5: _LL6:
 continue;}_LL0:;}}
# 447
({struct Cyc_Dict_Dict _tmp2A0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp2A0;});}struct _tuple14{void*f1;void*f2;};
# 450
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 457
struct _RegionHandle _tmpE1=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpE1;_push_region(uprev_rgn);
# 459
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple14*_tmpE2=(struct _tuple14*)rpo->hd;struct _tuple14*_stmttmp10=_tmpE2;struct _tuple14*_tmpE3=_stmttmp10;void*_tmpE5;void*_tmpE4;_LL1: _tmpE4=_tmpE3->f1;_tmpE5=_tmpE3->f2;_LL2: {void*e=_tmpE4;void*r=_tmpE5;
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,e);
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,r);}}{
# 465
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 468
struct Cyc_List_List*_tmpE6=fields;struct Cyc_List_List*fs=_tmpE6;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpE7=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp11=_tmpE7;struct Cyc_Absyn_Aggrfield*_tmpE8=_stmttmp11;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_List_List*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;void*_tmpEB;struct Cyc_Absyn_Tqual _tmpEA;struct _fat_ptr*_tmpE9;_LL4: _tmpE9=_tmpE8->name;_tmpEA=_tmpE8->tq;_tmpEB=_tmpE8->type;_tmpEC=_tmpE8->width;_tmpED=_tmpE8->attributes;_tmpEE=_tmpE8->requires_clause;_LL5: {struct _fat_ptr*fn=_tmpE9;struct Cyc_Absyn_Tqual tq=_tmpEA;void*t=_tmpEB;struct Cyc_Absyn_Exp*width=_tmpEC;struct Cyc_List_List*atts=_tmpED;struct Cyc_Absyn_Exp*requires_clause=_tmpEE;
# 471
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_String_pa_PrintArg_struct _tmpF1=({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0U,_tmp1F3.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp1F3;});void*_tmpEF[1U];_tmpEF[0]=& _tmpF1;({unsigned _tmp2A2=loc;struct _fat_ptr _tmp2A1=({const char*_tmpF0="duplicate member %s";_tag_fat(_tmpF0,sizeof(char),20U);});Cyc_Tcutil_terr(_tmp2A2,_tmp2A1,_tag_fat(_tmpEF,sizeof(void*),1U));});});
# 475
if(({struct _fat_ptr _tmp2A3=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp2A3,({const char*_tmpF2="";_tag_fat(_tmpF2,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmpF3=_region_malloc(uprev_rgn,sizeof(*_tmpF3));_tmpF3->hd=fn,_tmpF3->tl=prev_fields;_tmpF3;});{
# 478
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 482
if((int)str_or_union == (int)1U ||
 fs->tl == 0 &&(int)str_or_union == (int)0U)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tctyp_check_type(loc,te,tvs,field_kind,0,0,t);
# 487
({int _tmp2A4=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).print_const,t);(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).real_const=_tmp2A4;});
# 490
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
# 492
if((unsigned)requires_clause){
if((int)str_or_union != (int)1U)
({void*_tmpF4=0U;({unsigned _tmp2A6=loc;struct _fat_ptr _tmp2A5=({const char*_tmpF5="@requires clauses are only allowed on union members";_tag_fat(_tmpF5,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2A6,_tmp2A5,_tag_fat(_tmpF4,sizeof(void*),0U));});});{
struct Cyc_Tcenv_Tenv*_tmpF6=Cyc_Tcenv_allow_valueof(te);struct Cyc_Tcenv_Tenv*te2=_tmpF6;
Cyc_Tcexp_tcExp(te2,0,requires_clause);
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_String_pa_PrintArg_struct _tmpF9=({struct Cyc_String_pa_PrintArg_struct _tmp1F4;_tmp1F4.tag=0U,({
# 500
struct _fat_ptr _tmp2A7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(requires_clause->topt)));_tmp1F4.f1=_tmp2A7;});_tmp1F4;});void*_tmpF7[1U];_tmpF7[0]=& _tmpF9;({unsigned _tmp2A9=requires_clause->loc;struct _fat_ptr _tmp2A8=({const char*_tmpF8="@requires clause has type %s instead of integral type";_tag_fat(_tmpF8,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp2A9,_tmp2A8,_tag_fat(_tmpF7,sizeof(void*),1U));});});else{
# 502
({unsigned _tmp2AC=requires_clause->loc;struct Cyc_Tcenv_Tenv*_tmp2AB=te;struct Cyc_List_List*_tmp2AA=tvs;Cyc_Tctyp_check_type(_tmp2AC,_tmp2AB,_tmp2AA,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=9U,_tmpFA->f1=requires_clause;_tmpFA;}));});{
# 504
struct Cyc_List_List*_tmpFB=Cyc_Relations_exp2relns(uprev_rgn,requires_clause);struct Cyc_List_List*relns=_tmpFB;
# 511
if(!Cyc_Relations_consistent_relations(relns))
({void*_tmpFC=0U;({unsigned _tmp2AE=requires_clause->loc;struct _fat_ptr _tmp2AD=({const char*_tmpFD="@requires clause may be unsatisfiable";_tag_fat(_tmpFD,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp2AE,_tmp2AD,_tag_fat(_tmpFC,sizeof(void*),0U));});});
# 517
{struct Cyc_List_List*_tmpFE=prev_relations;struct Cyc_List_List*p=_tmpFE;for(0;p != 0;p=p->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,relns,(struct Cyc_List_List*)p->hd)))
({void*_tmpFF=0U;({unsigned _tmp2B0=requires_clause->loc;struct _fat_ptr _tmp2AF=({const char*_tmp100="@requires clause may overlap with previous clauses";_tag_fat(_tmp100,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2B0,_tmp2AF,_tag_fat(_tmpFF,sizeof(void*),0U));});});}}
# 522
prev_relations=({struct Cyc_List_List*_tmp101=_region_malloc(uprev_rgn,sizeof(*_tmp101));_tmp101->hd=relns,_tmp101->tl=prev_relations;_tmp101;});}}}}else{
# 525
if(prev_relations != 0)
({void*_tmp102=0U;({unsigned _tmp2B2=loc;struct _fat_ptr _tmp2B1=({const char*_tmp103="if one field has a @requires clause, they all must";_tag_fat(_tmp103,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2B2,_tmp2B1,_tag_fat(_tmp102,sizeof(void*),0U));});});}}}}}
# 459
;_pop_region(uprev_rgn);}
# 531
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 533
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp104=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmp12=_tmp104;void*_tmp105=_stmttmp12;enum Cyc_Absyn_AliasQual _tmp106;struct Cyc_Absyn_Kind*_tmp108;struct Cyc_Core_Opt**_tmp107;struct Cyc_Core_Opt**_tmp109;enum Cyc_Absyn_AliasQual _tmp10B;struct Cyc_Core_Opt**_tmp10A;struct Cyc_Core_Opt**_tmp10C;switch(*((int*)_tmp105)){case 1U: _LL1: _tmp10C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp105)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp10C;
# 536
({struct Cyc_Core_Opt*_tmp2B4=({struct Cyc_Core_Opt*_tmp10D=_cycalloc(sizeof(*_tmp10D));({void*_tmp2B3=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp10D->v=_tmp2B3;});_tmp10D;});*f=_tmp2B4;});continue;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmp10A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f1;_tmp10B=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2)->aliasqual;_LL4: {struct Cyc_Core_Opt**f=_tmp10A;enum Cyc_Absyn_AliasQual a=_tmp10B;
# 538
if(constrain_top_kind &&(int)a == (int)2U)
({struct Cyc_Core_Opt*_tmp2B6=({struct Cyc_Core_Opt*_tmp10F=_cycalloc(sizeof(*_tmp10F));({void*_tmp2B5=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->kind=Cyc_Absyn_BoxKind,_tmp10E->aliasqual=Cyc_Absyn_Aliasable;_tmp10E;}));_tmp10F->v=_tmp2B5;});_tmp10F;});*f=_tmp2B6;});else{
# 541
({struct Cyc_Core_Opt*_tmp2B8=({struct Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));({void*_tmp2B7=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->kind=Cyc_Absyn_BoxKind,_tmp110->aliasqual=a;_tmp110;}));_tmp111->v=_tmp2B7;});_tmp111;});*f=_tmp2B8;});}
continue;}case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmp109=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f1;if(constrain_top_kind){_LL6: {struct Cyc_Core_Opt**f=_tmp109;
# 544
({struct Cyc_Core_Opt*_tmp2BA=({struct Cyc_Core_Opt*_tmp113=_cycalloc(sizeof(*_tmp113));({void*_tmp2B9=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->kind=Cyc_Absyn_BoxKind,_tmp112->aliasqual=Cyc_Absyn_Aliasable;_tmp112;}));_tmp113->v=_tmp2B9;});_tmp113;});*f=_tmp2BA;});
continue;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmp107=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f1;_tmp108=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2;_LL8: {struct Cyc_Core_Opt**f=_tmp107;struct Cyc_Absyn_Kind*k=_tmp108;
# 547
({struct Cyc_Core_Opt*_tmp2BC=({struct Cyc_Core_Opt*_tmp114=_cycalloc(sizeof(*_tmp114));({void*_tmp2BB=Cyc_Tcutil_kind_to_bound(k);_tmp114->v=_tmp2BB;});_tmp114;});*f=_tmp2BC;});continue;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp105)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmp106=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp105)->f1)->aliasqual;_LLA: {enum Cyc_Absyn_AliasQual a=_tmp106;
# 549
({struct Cyc_String_pa_PrintArg_struct _tmp117=({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0U,_tmp1F7.f1=(struct _fat_ptr)((struct _fat_ptr)*(*n).f2);_tmp1F7;});struct Cyc_String_pa_PrintArg_struct _tmp118=({struct Cyc_String_pa_PrintArg_struct _tmp1F6;_tmp1F6.tag=0U,_tmp1F6.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp1F6;});struct Cyc_String_pa_PrintArg_struct _tmp119=({struct Cyc_String_pa_PrintArg_struct _tmp1F5;_tmp1F5.tag=0U,({
struct _fat_ptr _tmp2BD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->kind=Cyc_Absyn_MemKind,_tmp11A->aliasqual=a;_tmp11A;})));_tmp1F5.f1=_tmp2BD;});_tmp1F5;});void*_tmp115[3U];_tmp115[0]=& _tmp117,_tmp115[1]=& _tmp118,_tmp115[2]=& _tmp119;({unsigned _tmp2BF=loc;struct _fat_ptr _tmp2BE=({const char*_tmp116="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp116,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2BF,_tmp2BE,_tag_fat(_tmp115,sizeof(void*),3U));});});
continue;}}else{_LLB: _LLC:
 continue;}}_LL0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 557
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp11B=ad->name;struct _tuple0*q=_tmp11B;
# 564
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp11C=(void*)atts->hd;void*_stmttmp13=_tmp11C;void*_tmp11D=_stmttmp13;switch(*((int*)_tmp11D)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
# 569
({struct Cyc_String_pa_PrintArg_struct _tmp120=({struct Cyc_String_pa_PrintArg_struct _tmp1F9;_tmp1F9.tag=0U,({
struct _fat_ptr _tmp2C0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1F9.f1=_tmp2C0;});_tmp1F9;});struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp1F8;_tmp1F8.tag=0U,_tmp1F8.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp1F8;});void*_tmp11E[2U];_tmp11E[0]=& _tmp120,_tmp11E[1]=& _tmp121;({unsigned _tmp2C2=loc;struct _fat_ptr _tmp2C1=({const char*_tmp11F="bad attribute %s in %s definition";_tag_fat(_tmp11F,sizeof(char),34U);});Cyc_Tcutil_terr(_tmp2C2,_tmp2C1,_tag_fat(_tmp11E,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}}{
# 574
struct Cyc_List_List*_tmp122=ad->tvs;struct Cyc_List_List*tvs=_tmp122;
# 577
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 582
struct _tuple15 _tmp123=({struct _tuple15 _tmp1FD;_tmp1FD.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp2C3=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,q);_tmp1FD.f2=_tmp2C3;});_tmp1FD;});struct _tuple15 _stmttmp14=_tmp123;struct _tuple15 _tmp124=_stmttmp14;struct Cyc_Absyn_Aggrdecl**_tmp129;int _tmp128;struct Cyc_List_List*_tmp127;struct Cyc_List_List*_tmp126;struct Cyc_List_List*_tmp125;int _tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Aggrdecl**_tmp12E;if(_tmp124.f1 == 0){if(_tmp124.f2 == 0){_LL8: _LL9:
# 585
 Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 587
({struct Cyc_Dict_Dict _tmp2C6=({struct Cyc_Dict_Dict _tmp2C5=(te->ae)->aggrdecls;struct _tuple0*_tmp2C4=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp2C5,_tmp2C4,({struct Cyc_Absyn_Aggrdecl**_tmp12F=_cycalloc(sizeof(*_tmp12F));*_tmp12F=ad;_tmp12F;}));});(te->ae)->aggrdecls=_tmp2C6;});
goto _LL7;}else{_LLE: _tmp12E=*_tmp124.f2;_LLF: {struct Cyc_Absyn_Aggrdecl**adp=_tmp12E;
# 647
struct Cyc_Absyn_Aggrdecl*_tmp140=Cyc_Tcdecl_merge_aggrdecl(*adp,ad,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Aggrdecl*ad2=_tmp140;
if(ad2 == 0)
return;else{
# 651
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 654
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,q,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 657
*adp=ad2;
ad=ad2;
goto _LL7;}}}}else{if(_tmp124.f2 == 0){_LLA: _tmp12A=(_tmp124.f1)->exist_vars;_tmp12B=(_tmp124.f1)->rgn_po;_tmp12C=(_tmp124.f1)->fields;_tmp12D=(_tmp124.f1)->tagged;_LLB: {struct Cyc_List_List*exist_vars=_tmp12A;struct Cyc_List_List*rgn_po=_tmp12B;struct Cyc_List_List*fs=_tmp12C;int tagged=_tmp12D;
# 592
struct Cyc_Absyn_Aggrdecl**_tmp130=({struct Cyc_Absyn_Aggrdecl**_tmp139=_cycalloc(sizeof(*_tmp139));({struct Cyc_Absyn_Aggrdecl*_tmp2C7=({struct Cyc_Absyn_Aggrdecl*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->kind=ad->kind,_tmp138->sc=Cyc_Absyn_Extern,_tmp138->name=ad->name,_tmp138->tvs=tvs,_tmp138->impl=0,_tmp138->attributes=ad->attributes,_tmp138->expected_mem_kind=0;_tmp138;});*_tmp139=_tmp2C7;});_tmp139;});struct Cyc_Absyn_Aggrdecl**adp=_tmp130;
# 594
({struct Cyc_Dict_Dict _tmp2C8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,q,adp);(te->ae)->aggrdecls=_tmp2C8;});
# 599
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 603
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp131=0U;({unsigned _tmp2CA=loc;struct _fat_ptr _tmp2C9=({const char*_tmp132="@tagged qualifier is only allowed on union declarations";_tag_fat(_tmp132,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2CA,_tmp2C9,_tag_fat(_tmp131,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2CF=te;unsigned _tmp2CE=loc;enum Cyc_Absyn_AggrKind _tmp2CD=ad->kind;struct Cyc_List_List*_tmp2CC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp2CB=rgn_po;Cyc_Tc_tcAggrImpl(_tmp2CF,_tmp2CE,_tmp2CD,_tmp2CC,_tmp2CB,fs);});
# 607
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 610
Cyc_Tc_rule_out_memkind(loc,q,exist_vars,1);
# 612
if((int)ad->kind == (int)Cyc_Absyn_UnionA && !tagged){
# 615
struct Cyc_List_List*f=fs;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp135=({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0U,_tmp1FC.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);_tmp1FC;});struct Cyc_String_pa_PrintArg_struct _tmp136=({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0U,_tmp1FB.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp1FB;});struct Cyc_String_pa_PrintArg_struct _tmp137=({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0U,({
struct _fat_ptr _tmp2D0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));_tmp1FA.f1=_tmp2D0;});_tmp1FA;});void*_tmp133[3U];_tmp133[0]=& _tmp135,_tmp133[1]=& _tmp136,_tmp133[2]=& _tmp137;({unsigned _tmp2D2=loc;struct _fat_ptr _tmp2D1=({const char*_tmp134="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_fat(_tmp134,sizeof(char),98U);});Cyc_Tcutil_warn(_tmp2D2,_tmp2D1,_tag_fat(_tmp133,sizeof(void*),3U));});});}}
# 621
*adp=ad;
goto _LL7;}}else{_LLC: _tmp125=(_tmp124.f1)->exist_vars;_tmp126=(_tmp124.f1)->rgn_po;_tmp127=(_tmp124.f1)->fields;_tmp128=(_tmp124.f1)->tagged;_tmp129=*_tmp124.f2;_LLD: {struct Cyc_List_List*exist_vars=_tmp125;struct Cyc_List_List*rgn_po=_tmp126;struct Cyc_List_List*fs=_tmp127;int tagged=_tmp128;struct Cyc_Absyn_Aggrdecl**adp=_tmp129;
# 625
if((int)ad->kind != (int)(*adp)->kind)
({void*_tmp13A=0U;({unsigned _tmp2D4=loc;struct _fat_ptr _tmp2D3=({const char*_tmp13B="cannot reuse struct names for unions and vice-versa";_tag_fat(_tmp13B,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2D4,_tmp2D3,_tag_fat(_tmp13A,sizeof(void*),0U));});});{
# 628
struct Cyc_Absyn_Aggrdecl*_tmp13C=*adp;struct Cyc_Absyn_Aggrdecl*ad0=_tmp13C;
# 630
({struct Cyc_Absyn_Aggrdecl*_tmp2D5=({struct Cyc_Absyn_Aggrdecl*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->kind=ad->kind,_tmp13D->sc=Cyc_Absyn_Extern,_tmp13D->name=ad->name,_tmp13D->tvs=tvs,_tmp13D->impl=0,_tmp13D->attributes=ad->attributes,_tmp13D->expected_mem_kind=0;_tmp13D;});*adp=_tmp2D5;});
# 636
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 639
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp13E=0U;({unsigned _tmp2D7=loc;struct _fat_ptr _tmp2D6=({const char*_tmp13F="@tagged qualifier is only allowed on union declarations";_tag_fat(_tmp13F,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2D7,_tmp2D6,_tag_fat(_tmp13E,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2DC=te;unsigned _tmp2DB=loc;enum Cyc_Absyn_AggrKind _tmp2DA=ad->kind;struct Cyc_List_List*_tmp2D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp2D8=rgn_po;Cyc_Tc_tcAggrImpl(_tmp2DC,_tmp2DB,_tmp2DA,_tmp2D9,_tmp2D8,fs);});
# 643
*adp=ad0;
_tmp12E=adp;goto _LLF;}}}}_LL7:;}}}
# 664
static void Cyc_Tc_rule_out_mem_and_unique(unsigned loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp141=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmp15=_tmp141;void*_tmp142=_stmttmp15;enum Cyc_Absyn_AliasQual _tmp143;enum Cyc_Absyn_KindQual _tmp144;enum Cyc_Absyn_KindQual _tmp146;struct Cyc_Core_Opt**_tmp145;struct Cyc_Core_Opt**_tmp147;struct Cyc_Core_Opt**_tmp148;struct Cyc_Core_Opt**_tmp149;struct Cyc_Core_Opt**_tmp14A;struct Cyc_Core_Opt**_tmp14B;struct Cyc_Core_Opt**_tmp14C;switch(*((int*)_tmp142)){case 1U: _LL1: _tmp14C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp14C;
_tmp14B=f;goto _LL4;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmp14B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp14B;
# 669
_tmp14A=f;goto _LL6;}case Cyc_Absyn_Aliasable: _LL5: _tmp14A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp14A;
# 671
({struct Cyc_Core_Opt*_tmp2DE=({struct Cyc_Core_Opt*_tmp14D=_cycalloc(sizeof(*_tmp14D));({void*_tmp2DD=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp14D->v=_tmp2DD;});_tmp14D;});*f=_tmp2DE;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmp149=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp149;
_tmp148=f;goto _LLA;}case Cyc_Absyn_Aliasable: _LL9: _tmp148=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LLA: {struct Cyc_Core_Opt**f=_tmp148;
# 674
({struct Cyc_Core_Opt*_tmp2E0=({struct Cyc_Core_Opt*_tmp14E=_cycalloc(sizeof(*_tmp14E));({void*_tmp2DF=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp14E->v=_tmp2DF;});_tmp14E;});*f=_tmp2E0;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmp147=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LLC: {struct Cyc_Core_Opt**f=_tmp147;
# 676
({struct Cyc_Core_Opt*_tmp2E2=({struct Cyc_Core_Opt*_tmp14F=_cycalloc(sizeof(*_tmp14F));({void*_tmp2E1=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp14F->v=_tmp2E1;});_tmp14F;});*f=_tmp2E2;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmp145=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_tmp146=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->kind;_LL10: {struct Cyc_Core_Opt**f=_tmp145;enum Cyc_Absyn_KindQual k=_tmp146;
# 680
_tmp144=k;goto _LL12;}}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 678
({struct Cyc_String_pa_PrintArg_struct _tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0U,_tmp1FF.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp1FF;});struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0U,_tmp1FE.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp1FE;});void*_tmp150[2U];_tmp150[0]=& _tmp152,_tmp150[1]=& _tmp153;({unsigned _tmp2E4=loc;struct _fat_ptr _tmp2E3=({const char*_tmp151="type %s attempts to abstract type variable %s of kind TR";_tag_fat(_tmp151,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2E4,_tmp2E3,_tag_fat(_tmp150,sizeof(void*),2U));});});
goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmp144=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->kind;_LL12: {enum Cyc_Absyn_KindQual k=_tmp144;
# 682
({struct Cyc_String_pa_PrintArg_struct _tmp156=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0U,_tmp202.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp202;});struct Cyc_String_pa_PrintArg_struct _tmp157=({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0U,_tmp201.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp201;});struct Cyc_String_pa_PrintArg_struct _tmp158=({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0U,({
# 684
struct _fat_ptr _tmp2E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->kind=k,_tmp159->aliasqual=Cyc_Absyn_Unique;_tmp159;})));_tmp200.f1=_tmp2E5;});_tmp200;});void*_tmp154[3U];_tmp154[0]=& _tmp156,_tmp154[1]=& _tmp157,_tmp154[2]=& _tmp158;({unsigned _tmp2E7=loc;struct _fat_ptr _tmp2E6=({const char*_tmp155="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp155,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2E7,_tmp2E6,_tag_fat(_tmp154,sizeof(void*),3U));});});goto _LL0;}}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmp143=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->aliasqual;_LL14: {enum Cyc_Absyn_AliasQual a=_tmp143;
# 686
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0U,_tmp205.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp205;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0U,_tmp204.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp204;});struct Cyc_String_pa_PrintArg_struct _tmp15E=({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0U,({
# 688
struct _fat_ptr _tmp2E8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->kind=Cyc_Absyn_MemKind,_tmp15F->aliasqual=a;_tmp15F;})));_tmp203.f1=_tmp2E8;});_tmp203;});void*_tmp15A[3U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D,_tmp15A[2]=& _tmp15E;({unsigned _tmp2EA=loc;struct _fat_ptr _tmp2E9=({const char*_tmp15B="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp15B,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2EA,_tmp2E9,_tag_fat(_tmp15A,sizeof(void*),3U));});});goto _LL0;}}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 694
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 702
{struct Cyc_List_List*_tmp160=fields;struct Cyc_List_List*fs=_tmp160;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp161=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp161;
struct Cyc_List_List*typs=f->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tctyp_check_type(f->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 707
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0U,({
# 710
struct _fat_ptr _tmp2EB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp206.f1=_tmp2EB;});_tmp206;});void*_tmp162[1U];_tmp162[0]=& _tmp164;({unsigned _tmp2ED=f->loc;struct _fat_ptr _tmp2EC=({const char*_tmp163="noalias pointers in datatypes are not allowed (%s)";_tag_fat(_tmp163,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2ED,_tmp2EC,_tag_fat(_tmp162,sizeof(void*),1U));});});
({int _tmp2EE=
Cyc_Tcutil_extract_const_from_typedef(f->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 711
((*((struct _tuple16*)typs->hd)).f1).real_const=_tmp2EE;});}}}{
# 717
struct Cyc_List_List*fields2;
if(is_extensible){
# 720
int _tmp165=1;int res=_tmp165;
struct Cyc_List_List*_tmp166=Cyc_Tcdecl_sort_xdatatype_fields(fields,& res,(*name).f2,loc,Cyc_Tc_tc_msg);struct Cyc_List_List*fs=_tmp166;
if(res)
fields2=fs;else{
# 725
fields2=0;}}else{
# 727
struct _RegionHandle _tmp167=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp167;_push_region(uprev_rgn);
# 729
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp168=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp168;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp16B=({struct Cyc_String_pa_PrintArg_struct _tmp208;_tmp208.tag=0U,_tmp208.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp208;});struct Cyc_String_pa_PrintArg_struct _tmp16C=({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0U,_tmp207.f1=(struct _fat_ptr)((struct _fat_ptr)obj);_tmp207;});void*_tmp169[2U];_tmp169[0]=& _tmp16B,_tmp169[1]=& _tmp16C;({unsigned _tmp2F0=f->loc;struct _fat_ptr _tmp2EF=({const char*_tmp16A="duplicate field name %s in %s";_tag_fat(_tmp16A,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp2F0,_tmp2EF,_tag_fat(_tmp169,sizeof(void*),2U));});});else{
# 735
prev_fields=({struct Cyc_List_List*_tmp16D=_region_malloc(uprev_rgn,sizeof(*_tmp16D));_tmp16D->hd=(*f->name).f2,_tmp16D->tl=prev_fields;_tmp16D;});}
# 737
if((int)f->sc != (int)Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp170=({struct Cyc_String_pa_PrintArg_struct _tmp209;_tmp209.tag=0U,_tmp209.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp209;});void*_tmp16E[1U];_tmp16E[0]=& _tmp170;({unsigned _tmp2F2=loc;struct _fat_ptr _tmp2F1=({const char*_tmp16F="ignoring scope of field %s";_tag_fat(_tmp16F,sizeof(char),27U);});Cyc_Tcutil_warn(_tmp2F2,_tmp2F1,_tag_fat(_tmp16E,sizeof(void*),1U));});});
f->sc=Cyc_Absyn_Public;}}}
# 742
fields2=fields;}
# 729
;_pop_region(uprev_rgn);}
# 744
return fields2;}}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 747
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _fat_ptr obj=tud->is_extensible?({const char*_tmp186="@extensible datatype";_tag_fat(_tmp186,sizeof(char),21U);}):({const char*_tmp187="datatype";_tag_fat(_tmp187,sizeof(char),9U);});
# 754
struct Cyc_List_List*tvs=tud->tvs;
# 757
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 763
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 773 "tc.cyc"
{struct _handler_cons _tmp171;_push_handler(& _tmp171);{int _tmp173=0;if(setjmp(_tmp171.handler))_tmp173=1;if(!_tmp173){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt == 0 && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->tag=Cyc_Dict_Absent;_tmp174;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 779
({union Cyc_Absyn_Nmspace _tmp2F3=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp2F3;});}
# 774
;_pop_handler();}else{void*_tmp172=(void*)Cyc_Core_get_exn_thrown();void*_tmp175=_tmp172;void*_tmp176;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp175)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 783
struct Cyc_Absyn_Datatypedecl***_tmp177=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);struct Cyc_Absyn_Datatypedecl***tdopt=_tmp177;
tud_opt=(unsigned)tdopt?({struct Cyc_Absyn_Datatypedecl***_tmp178=_cycalloc(sizeof(*_tmp178));*_tmp178=*tdopt;_tmp178;}): 0;
goto _LL0;}}else{_LL3: _tmp176=_tmp175;_LL4: {void*exn=_tmp176;(int)_rethrow(exn);}}_LL0:;}}}{
# 790
struct _tuple17 _tmp179=({struct _tuple17 _tmp20A;_tmp20A.f1=tud->fields,_tmp20A.f2=tud_opt;_tmp20A;});struct _tuple17 _stmttmp16=_tmp179;struct _tuple17 _tmp17A=_stmttmp16;struct Cyc_Absyn_Datatypedecl**_tmp17C;struct Cyc_List_List**_tmp17B;struct Cyc_List_List**_tmp17D;struct Cyc_Absyn_Datatypedecl**_tmp17E;if(_tmp17A.f1 == 0){if(_tmp17A.f2 == 0){_LL6: _LL7:
# 793
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp2F6=({struct Cyc_Dict_Dict _tmp2F5=(te->ae)->datatypedecls;struct _tuple0*_tmp2F4=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp2F5,_tmp2F4,({struct Cyc_Absyn_Datatypedecl**_tmp17F=_cycalloc(sizeof(*_tmp17F));*_tmp17F=tud;_tmp17F;}));});(te->ae)->datatypedecls=_tmp2F6;});
goto _LL5;}else{_LLC: _tmp17E=*_tmp17A.f2;_LLD: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp17E;
# 826
struct Cyc_Absyn_Datatypedecl*_tmp185=Cyc_Tcdecl_merge_datatypedecl(*tudp,tud,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Datatypedecl*tud2=_tmp185;
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(tud2 == 0)
return;else{
# 831
*tudp=tud2;
goto _LL5;}}}}else{if(_tmp17A.f2 == 0){_LL8: _tmp17D=(struct Cyc_List_List**)&(_tmp17A.f1)->v;_LL9: {struct Cyc_List_List**fs=_tmp17D;
# 798
struct Cyc_Absyn_Datatypedecl**_tmp180=({struct Cyc_Absyn_Datatypedecl**_tmp182=_cycalloc(sizeof(*_tmp182));({struct Cyc_Absyn_Datatypedecl*_tmp2F7=({struct Cyc_Absyn_Datatypedecl*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->sc=Cyc_Absyn_Extern,_tmp181->name=tud->name,_tmp181->tvs=tvs,_tmp181->fields=0,_tmp181->is_extensible=tud->is_extensible;_tmp181;});*_tmp182=_tmp2F7;});_tmp182;});struct Cyc_Absyn_Datatypedecl**tudp=_tmp180;
# 800
({struct Cyc_Dict_Dict _tmp2F8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,tudp);(te->ae)->datatypedecls=_tmp2F8;});
# 803
({struct Cyc_List_List*_tmp2F9=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp2F9;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*tudp=tud;
goto _LL5;}}else{_LLA: _tmp17B=(struct Cyc_List_List**)&(_tmp17A.f1)->v;_tmp17C=*_tmp17A.f2;_LLB: {struct Cyc_List_List**fs=_tmp17B;struct Cyc_Absyn_Datatypedecl**tudp=_tmp17C;
# 808
struct Cyc_Absyn_Datatypedecl*_tmp183=*tudp;struct Cyc_Absyn_Datatypedecl*tud0=_tmp183;
# 811
if((!tud->is_extensible &&(unsigned)tud0)&& tud0->is_extensible)
tud->is_extensible=1;
# 814
({struct Cyc_Absyn_Datatypedecl*_tmp2FA=({struct Cyc_Absyn_Datatypedecl*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->sc=Cyc_Absyn_Extern,_tmp184->name=tud->name,_tmp184->tvs=tvs,_tmp184->fields=0,_tmp184->is_extensible=tud->is_extensible;_tmp184;});*tudp=_tmp2FA;});
# 818
({struct Cyc_List_List*_tmp2FB=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp2FB;});
# 821
*tudp=tud0;
_tmp17E=tudp;goto _LLD;}}}_LL5:;}}}
# 837
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 844
if(ed->fields != 0){struct _RegionHandle _tmp188=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp188;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp189=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp189;
# 850
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp18C=({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0U,_tmp20B.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp20B;});void*_tmp18A[1U];_tmp18A[0]=& _tmp18C;({unsigned _tmp2FD=f->loc;struct _fat_ptr _tmp2FC=({const char*_tmp18B="duplicate enum constructor %s";_tag_fat(_tmp18B,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp2FD,_tmp2FC,_tag_fat(_tmp18A,sizeof(void*),1U));});});else{
# 853
prev_fields=({struct Cyc_List_List*_tmp18D=_region_malloc(uprev_rgn,sizeof(*_tmp18D));_tmp18D->hd=(*f->name).f2,_tmp18D->tl=prev_fields;_tmp18D;});}
# 855
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,f->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp190=({struct Cyc_String_pa_PrintArg_struct _tmp20C;_tmp20C.tag=0U,_tmp20C.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp20C;});void*_tmp18E[1U];_tmp18E[0]=& _tmp190;({unsigned _tmp2FF=f->loc;struct _fat_ptr _tmp2FE=({const char*_tmp18F="enum field name %s shadows global name";_tag_fat(_tmp18F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp2FF,_tmp2FE,_tag_fat(_tmp18E,sizeof(void*),1U));});});
# 858
if(f->tag == 0){
({struct Cyc_Absyn_Exp*_tmp300=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp300;});
++ tag_count;}else{
# 863
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))){
struct _tuple10 _tmp191=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag));struct _tuple10 _stmttmp17=_tmp191;struct _tuple10 _tmp192=_stmttmp17;int _tmp194;unsigned _tmp193;_LL1: _tmp193=_tmp192.f1;_tmp194=_tmp192.f2;_LL2: {unsigned t1=_tmp193;int known=_tmp194;
if(known)tag_count=t1 + (unsigned)1;}}}}}
# 845
;_pop_region(uprev_rgn);}
# 871
{struct _handler_cons _tmp195;_push_handler(& _tmp195);{int _tmp197=0;if(setjmp(_tmp195.handler))_tmp197=1;if(!_tmp197){
{struct Cyc_Absyn_Enumdecl**_tmp198=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);struct Cyc_Absyn_Enumdecl**edp=_tmp198;
struct Cyc_Absyn_Enumdecl*_tmp199=Cyc_Tcdecl_merge_enumdecl(*edp,ed,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Enumdecl*ed2=_tmp199;
if(ed2 == 0){_npop_handler(0U);return;}
*edp=ed2;}
# 872
;_pop_handler();}else{void*_tmp196=(void*)Cyc_Core_get_exn_thrown();void*_tmp19A=_tmp196;void*_tmp19B;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp19A)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 877
struct Cyc_Absyn_Enumdecl**_tmp19C=({struct Cyc_Absyn_Enumdecl**_tmp19D=_cycalloc(sizeof(*_tmp19D));*_tmp19D=ed;_tmp19D;});struct Cyc_Absyn_Enumdecl**edp=_tmp19C;
({struct Cyc_Dict_Dict _tmp301=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,edp);(te->ae)->enumdecls=_tmp301;});
goto _LL3;}}else{_LL6: _tmp19B=_tmp19A;_LL7: {void*exn=_tmp19B;(int)_rethrow(exn);}}_LL3:;}}}
# 883
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp19E=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp19E;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1A1=({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0U,_tmp20E.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp20E;});struct Cyc_String_pa_PrintArg_struct _tmp1A2=({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0U,_tmp20D.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp20D;});void*_tmp19F[2U];_tmp19F[0]=& _tmp1A1,_tmp19F[1]=& _tmp1A2;({unsigned _tmp303=loc;struct _fat_ptr _tmp302=({const char*_tmp1A0="enum %s, field %s: expression is not constant";_tag_fat(_tmp1A0,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp303,_tmp302,_tag_fat(_tmp19F,sizeof(void*),2U));});});}}}
# 893
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1A3=sc;switch(_tmp1A3){case Cyc_Absyn_Static: _LL1: _LL2:
# 896
 if(!in_include)
({void*_tmp1A4=0U;({unsigned _tmp305=loc;struct _fat_ptr _tmp304=({const char*_tmp1A5="static declaration within extern \"C\"";_tag_fat(_tmp1A5,sizeof(char),37U);});Cyc_Tcutil_warn(_tmp305,_tmp304,_tag_fat(_tmp1A4,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 900
({void*_tmp1A6=0U;({unsigned _tmp307=loc;struct _fat_ptr _tmp306=({const char*_tmp1A7="abstract declaration within extern \"C\"";_tag_fat(_tmp1A7,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp307,_tmp306,_tag_fat(_tmp1A6,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;case Cyc_Absyn_ExternC: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
# 907
({void*_tmp1A8=0U;({unsigned _tmp309=loc;struct _fat_ptr _tmp308=({const char*_tmp1A9="nested extern \"C\" declaration";_tag_fat(_tmp1A9,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp309,_tmp308,_tag_fat(_tmp1A8,sizeof(void*),0U));});});
return 1;}_LL0:;}
# 917
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 921
struct Cyc_List_List*_tmp1AA=ds0;struct Cyc_List_List*ds=_tmp1AA;for(0;ds != 0;ds=ds->tl){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
unsigned loc=d->loc;
void*_tmp1AB=d->r;void*_stmttmp18=_tmp1AB;void*_tmp1AC=_stmttmp18;struct Cyc_List_List*_tmp1AF;struct Cyc_List_List*_tmp1AE;struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_List_List*_tmp1B3;struct _fat_ptr*_tmp1B2;struct Cyc_Absyn_Enumdecl*_tmp1B4;struct Cyc_Absyn_Datatypedecl*_tmp1B5;struct Cyc_Absyn_Aggrdecl*_tmp1B6;struct Cyc_Absyn_Typedefdecl*_tmp1B7;struct Cyc_Absyn_Fndecl*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1B9;switch(*((int*)_tmp1AC)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
# 927
({void*_tmp1BA=0U;({unsigned _tmp30B=loc;struct _fat_ptr _tmp30A=({const char*_tmp1BB="top level let-declarations are not implemented";_tag_fat(_tmp1BB,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp30B,_tmp30A,_tag_fat(_tmp1BA,sizeof(void*),0U));});});goto _LL0;case 4U: _LL5: _LL6:
# 929
({void*_tmp1BC=0U;({unsigned _tmp30D=loc;struct _fat_ptr _tmp30C=({const char*_tmp1BD="top level region declarations are not implemented";_tag_fat(_tmp1BD,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp30D,_tmp30C,_tag_fat(_tmp1BC,sizeof(void*),0U));});});goto _LL0;case 0U: _LL7: _tmp1B9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp1B9;
# 931
if(in_externC && Cyc_Tc_okay_externC(d->loc,vd->sc,te->in_extern_c_include))
vd->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,vd,check_var_init,te->in_extern_c_include,exports);
goto _LL0;}case 1U: _LL9: _tmp1B8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LLA: {struct Cyc_Absyn_Fndecl*fd=_tmp1B8;
# 936
if(in_externC && Cyc_Tc_okay_externC(d->loc,fd->sc,te->in_extern_c_include))
fd->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,fd,exports);
goto _LL0;}case 8U: _LLB: _tmp1B7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp1B7;
# 941
td->extern_c=te->in_extern_c_include;
# 945
Cyc_Tc_tcTypedefdecl(te,loc,td);
goto _LL0;}case 5U: _LLD: _tmp1B6=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1B6;
# 948
if(in_externC && Cyc_Tc_okay_externC(d->loc,ad->sc,te->in_extern_c_include))
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);
goto _LL0;}case 6U: _LLF: _tmp1B5=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL10: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1B5;
# 953
if(in_externC && Cyc_Tc_okay_externC(d->loc,tud->sc,te->in_extern_c_include))
tud->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
goto _LL0;}case 7U: _LL11: _tmp1B4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*ed=_tmp1B4;
# 958
if(in_externC && Cyc_Tc_okay_externC(d->loc,ed->sc,te->in_extern_c_include))
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);
goto _LL0;}case 13U: _LL13: _LL14:
({void*_tmp1BE=0U;({unsigned _tmp30F=d->loc;struct _fat_ptr _tmp30E=({const char*_tmp1BF="spurious __cyclone_port_on__";_tag_fat(_tmp1BF,sizeof(char),29U);});Cyc_Tcutil_warn(_tmp30F,_tmp30E,_tag_fat(_tmp1BE,sizeof(void*),0U));});});goto _LL0;case 14U: _LL15: _LL16:
 goto _LL0;case 15U: _LL17: _LL18:
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16U: _LL19: _LL1A:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9U: _LL1B: _tmp1B2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_tmp1B3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AC)->f2;_LL1C: {struct _fat_ptr*v=_tmp1B2;struct Cyc_List_List*ds2=_tmp1B3;
# 968
struct Cyc_List_List*_tmp1C0=te->ns;struct Cyc_List_List*ns=_tmp1C0;
({struct Cyc_List_List*_tmp311=({struct Cyc_List_List*_tmp310=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp310,({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=v,_tmp1C1->tl=0;_tmp1C1;}));});te->ns=_tmp311;});
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);
te->ns=ns;
goto _LL0;}case 10U: _LL1D: _tmp1B1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AC)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp1B1;
# 975
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11U: _LL1F: _tmp1B0=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp1B0;
# 978
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default: _LL21: _tmp1AD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_tmp1AE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AC)->f2;_tmp1AF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AC)->f3;_LL22: {struct Cyc_List_List*ds2=_tmp1AD;struct Cyc_List_List*ovrs=_tmp1AE;struct Cyc_List_List*exports2=_tmp1AF;
# 983
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(exports2,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1C2=Cyc_Tcenv_enter_extern_c_include(te);struct Cyc_Tcenv_Tenv*te2=_tmp1C2;
Cyc_Tc_tc_decls(te2,ds2,1,check_var_init,& newexs);
# 988
for(0;exports2 != 0;exports2=exports2->tl){
struct _tuple11*_tmp1C3=(struct _tuple11*)exports2->hd;struct _tuple11*exp=_tmp1C3;
if(!(*exp).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1C6=({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0U,({
struct _fat_ptr _tmp312=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string((*exp).f2));_tmp20F.f1=_tmp312;});_tmp20F;});void*_tmp1C4[1U];_tmp1C4[0]=& _tmp1C6;({unsigned _tmp314=(*exp).f1;struct _fat_ptr _tmp313=({const char*_tmp1C5="%s is exported but not defined";_tag_fat(_tmp1C5,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp314,_tmp313,_tag_fat(_tmp1C4,sizeof(void*),1U));});});}
# 994
goto _LL0;}}_LL0:;}}
# 999
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict _tmp1C7=Cyc_Callgraph_compute_callgraph(ds);struct Cyc_Dict_Dict cg=_tmp1C7;
# 1004
struct Cyc_Dict_Dict _tmp1C8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict g))Cyc_Graph_scc)(cg);struct Cyc_Dict_Dict scc=_tmp1C8;
# 1007
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1025 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1027
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1031
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){
if((int)vd->sc == (int)Cyc_Absyn_Extern ||(int)vd->sc == (int)Cyc_Absyn_ExternC)
return 1;{
void*_tmp1C9=Cyc_Tcutil_compress(vd->type);void*_stmttmp19=_tmp1C9;void*_tmp1CA=_stmttmp19;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CA)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1040
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1CB=d->r;void*_stmttmp1A=_tmp1CB;void*_tmp1CC=_stmttmp1A;struct Cyc_List_List**_tmp1CD;struct Cyc_List_List**_tmp1CE;struct Cyc_List_List**_tmp1CF;struct Cyc_List_List**_tmp1D0;struct Cyc_Absyn_Vardecl*_tmp1D1;switch(*((int*)_tmp1CC)){case 0U: _LL1: _tmp1D1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1CC)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1D1;
# 1045
if((env->in_cinclude || !Cyc_Tc_is_extern(vd))|| !({
struct _tuple0*_tmp315=vd->name;Cyc_Absyn_qvar_cmp(_tmp315,Cyc_Absyn_uniquergn_qvar());}))
# 1048
return 1;
# 1050
return(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,vd->name)).f2;}case 11U: _LL3: _tmp1D0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1CC)->f1;_LL4: {struct Cyc_List_List**ds2p=_tmp1D0;
_tmp1CF=ds2p;goto _LL6;}case 10U: _LL5: _tmp1CF=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1CC)->f2;_LL6: {struct Cyc_List_List**ds2p=_tmp1CF;
_tmp1CE=ds2p;goto _LL8;}case 9U: _LL7: _tmp1CE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1CC)->f2;_LL8: {struct Cyc_List_List**ds2p=_tmp1CE;
({struct Cyc_List_List*_tmp316=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp316;});return 1;}case 12U: _LL9: _tmp1CD=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1CC)->f1;_LLA: {struct Cyc_List_List**ds2p=_tmp1CD;
# 1055
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp317=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp317;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 1064
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1D2=({struct Cyc_Tc_TreeshakeEnv _tmp210;_tmp210.in_cinclude=0,_tmp210.ordinaries=(te->ae)->ordinaries;_tmp210;});struct Cyc_Tc_TreeshakeEnv env=_tmp1D2;
return Cyc_Tc_treeshake_f(& env,ds);}

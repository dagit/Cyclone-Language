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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 876 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 884
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 890
void*Cyc_Absyn_compress_kb(void*);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 941
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 964
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
# 972
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1046
struct _tuple0*Cyc_Absyn_uniquergn_qvar();
# 1130
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
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 70
void*Cyc_Warn_impos2(struct _fat_ptr);
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
# 273
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 280
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 284
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 301
int Cyc_Tcutil_zeroable_type(void*);
# 305
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 308
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
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
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
# 42 "tc.cyc"
static struct _fat_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1U};
static struct _fat_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 46
int Cyc_Tc_aggressive_warn=0;struct _tuple12{unsigned f1;struct _tuple0*f2;int f3;};
# 48
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;struct _tuple12*p=_tmp1;
if(Cyc_Absyn_qvar_cmp(x,(*p).f2)== 0){
# 54
(*p).f3=1;
return 1;}}
# 58
return 0;}
# 61
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned loc,int i,int nargs,void*att){
if(i < 1 || i > nargs){
({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp3=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp1E4;_tmp1E4.tag=9U,_tmp1E4.f1=(void*)att;_tmp1E4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E3;_tmp1E3.tag=0U,({struct _fat_ptr _tmp22E=({const char*_tmp5=" has an out-of-range index";_tag_fat(_tmp5,sizeof(char),27U);});_tmp1E3.f1=_tmp22E;});_tmp1E3;});void*_tmp2[2U];_tmp2[0]=& _tmp3,_tmp2[1]=& _tmp4;({unsigned _tmp22F=loc;Cyc_Warn_err2(_tmp22F,_tag_fat(_tmp2,sizeof(void*),2U));});});
return 0;}
# 66
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _fat_ptr f2;};
# 68
static void Cyc_Tc_fnTypeAttsOverlap(unsigned loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 71
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0U,_tmp1E7.f1=(struct _fat_ptr)((struct _fat_ptr)lst1.f2);_tmp1E7;});struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0U,_tmp1E6.f1=(struct _fat_ptr)((struct _fat_ptr)lst2.f2);_tmp1E6;});struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=1U,_tmp1E5.f1=(unsigned long)i;_tmp1E5;});void*_tmp6[3U];_tmp6[0]=& _tmp8,_tmp6[1]=& _tmp9,_tmp6[2]=& _tmpA;({unsigned _tmp231=loc;struct _fat_ptr _tmp230=({const char*_tmp7="incompatible %s() and %s() attributes on parameter %d";_tag_fat(_tmp7,sizeof(char),54U);});Cyc_Warn_err(_tmp231,_tmp230,_tag_fat(_tmp6,sizeof(void*),3U));});});}
# 75
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned loc){
struct _tuple13 init_params=({struct _tuple13 _tmp1F5;_tmp1F5.f1=0,({struct _fat_ptr _tmp232=({const char*_tmp42="initializes";_tag_fat(_tmp42,sizeof(char),12U);});_tmp1F5.f2=_tmp232;});_tmp1F5;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp1F4;_tmp1F4.f1=0,({struct _fat_ptr _tmp233=({const char*_tmp41="noliveunique";_tag_fat(_tmp41,sizeof(char),13U);});_tmp1F4.f2=_tmp233;});_tmp1F4;});
struct _tuple13 consume_params=({struct _tuple13 _tmp1F3;_tmp1F3.f1=0,({struct _fat_ptr _tmp234=({const char*_tmp40="consume";_tag_fat(_tmp40,sizeof(char),8U);});_tmp1F3.f2=_tmp234;});_tmp1F3;});
void*_tmpB=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpD;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->tag == 5U){_LL1: _tmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).attributes;_tmpE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).args;_LL2: {struct Cyc_List_List*atts=_tmpD;struct Cyc_List_List*args=_tmpE;
# 81
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);
for(0;atts != 0;atts=atts->tl){
void*_tmpF=(void*)atts->hd;void*_stmttmp1=_tmpF;void*_tmp10=_stmttmp1;int _tmp11;int _tmp12;int _tmp13;switch(*((int*)_tmp10)){case 20U: _LL6: _tmp13=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10)->f1;_LL7: {int i=_tmp13;
# 85
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,i,init_params,consume_params);{
struct _tuple8*_tmp14=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp2=_tmp14;struct _tuple8*_tmp15=_stmttmp2;void*_tmp16;_LLF: _tmp16=_tmp15->f3;_LL10: {void*t=_tmp16;
struct _fat_ptr s=({const char*_tmp2E="initializes attribute allowed only on";_tag_fat(_tmp2E,sizeof(char),38U);});
{void*_tmp17=Cyc_Tcutil_compress(t);void*_stmttmp3=_tmp17;void*_tmp18=_stmttmp3;void*_tmp1C;void*_tmp1B;void*_tmp1A;void*_tmp19;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->tag == 3U){_LL12: _tmp19=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).elt_type;_tmp1A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).nullable;_tmp1B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).bounds;_tmp1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).zero_term;_LL13: {void*t=_tmp19;void*nullable=_tmp1A;void*bd=_tmp1B;void*zt=_tmp1C;
# 92
if(Cyc_Tcutil_force_type2bool(0,nullable))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E9;_tmp1E9.tag=0U,_tmp1E9.f1=s;_tmp1E9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E8;_tmp1E8.tag=0U,({struct _fat_ptr _tmp235=({const char*_tmp20=" non-null pointers";_tag_fat(_tmp20,sizeof(char),19U);});_tmp1E8.f1=_tmp235;});_tmp1E8;});void*_tmp1D[2U];_tmp1D[0]=& _tmp1E,_tmp1D[1]=& _tmp1F;({unsigned _tmp236=loc;Cyc_Warn_err2(_tmp236,_tag_fat(_tmp1D,sizeof(void*),2U));});});
if(!Cyc_Tcutil_is_bound_one(bd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EB;_tmp1EB.tag=0U,_tmp1EB.f1=s;_tmp1EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EA;_tmp1EA.tag=0U,({struct _fat_ptr _tmp237=({const char*_tmp24=" pointers of size 1";_tag_fat(_tmp24,sizeof(char),20U);});_tmp1EA.f1=_tmp237;});_tmp1EA;});void*_tmp21[2U];_tmp21[0]=& _tmp22,_tmp21[1]=& _tmp23;({unsigned _tmp238=loc;Cyc_Warn_err2(_tmp238,_tag_fat(_tmp21,sizeof(void*),2U));});});
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1ED;_tmp1ED.tag=0U,_tmp1ED.f1=s;_tmp1ED;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EC;_tmp1EC.tag=0U,({struct _fat_ptr _tmp239=({const char*_tmp28=" pointers to non-zero-terminated arrays";_tag_fat(_tmp28,sizeof(char),40U);});_tmp1EC.f1=_tmp239;});_tmp1EC;});void*_tmp25[2U];_tmp25[0]=& _tmp26,_tmp25[1]=& _tmp27;({unsigned _tmp23A=loc;Cyc_Warn_err2(_tmp23A,_tag_fat(_tmp25,sizeof(void*),2U));});});
goto _LL11;}}else{_LL14: _LL15:
# 100
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EF;_tmp1EF.tag=0U,_tmp1EF.f1=s;_tmp1EF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EE;_tmp1EE.tag=0U,({struct _fat_ptr _tmp23B=({const char*_tmp2C=" pointers";_tag_fat(_tmp2C,sizeof(char),10U);});_tmp1EE.f1=_tmp23B;});_tmp1EE;});void*_tmp29[2U];_tmp29[0]=& _tmp2A,_tmp29[1]=& _tmp2B;({unsigned _tmp23C=loc;Cyc_Warn_err2(_tmp23C,_tag_fat(_tmp29,sizeof(void*),2U));});});}_LL11:;}
# 102
({struct Cyc_List_List*_tmp23D=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=(void*)i,_tmp2D->tl=init_params.f1;_tmp2D;});init_params.f1=_tmp23D;});
goto _LL5;}}}case 21U: _LL8: _tmp12=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp10)->f1;_LL9: {int i=_tmp12;
# 105
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,nolive_unique_params,init_params);{
# 108
struct _tuple8*_tmp2F=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp4=_tmp2F;struct _tuple8*_tmp30=_stmttmp4;void*_tmp31;_LL17: _tmp31=_tmp30->f3;_LL18: {void*t=_tmp31;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp33=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F0;_tmp1F0.tag=0U,({struct _fat_ptr _tmp23E=({const char*_tmp34="noliveunique allowed only on unique pointers";_tag_fat(_tmp34,sizeof(char),45U);});_tmp1F0.f1=_tmp23E;});_tmp1F0;});void*_tmp32[1U];_tmp32[0]=& _tmp33;({unsigned _tmp23F=loc;Cyc_Warn_err2(_tmp23F,_tag_fat(_tmp32,sizeof(void*),1U));});});
({struct Cyc_List_List*_tmp240=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=(void*)i,_tmp35->tl=nolive_unique_params.f1;_tmp35;});nolive_unique_params.f1=_tmp240;});
goto _LL5;}}}case 22U: _LLA: _tmp11=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp10)->f1;_LLB: {int i=_tmp11;
# 114
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,consume_params,init_params);{
# 117
struct _tuple8*_tmp36=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp5=_tmp36;struct _tuple8*_tmp37=_stmttmp5;void*_tmp38;_LL1A: _tmp38=_tmp37->f3;_LL1B: {void*t=_tmp38;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F1;_tmp1F1.tag=0U,({struct _fat_ptr _tmp241=({const char*_tmp3B="consume allowed only on unique pointers";_tag_fat(_tmp3B,sizeof(char),40U);});_tmp1F1.f1=_tmp241;});_tmp1F1;});void*_tmp39[1U];_tmp39[0]=& _tmp3A;({unsigned _tmp242=loc;Cyc_Warn_err2(_tmp242,_tag_fat(_tmp39,sizeof(void*),1U));});});
({struct Cyc_List_List*_tmp243=({struct Cyc_List_List*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->hd=(void*)i,_tmp3C->tl=consume_params.f1;_tmp3C;});consume_params.f1=_tmp243;});
goto _LL5;}}}default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 125
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F2;_tmp1F2.tag=0U,({struct _fat_ptr _tmp244=({const char*_tmp3F="fnTypeAttsOK: not a function type";_tag_fat(_tmp3F,sizeof(char),34U);});_tmp1F2.f1=_tmp244;});_tmp1F2;});void*_tmp3D[1U];_tmp3D[0]=& _tmp3E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3D,sizeof(void*),1U));});}_LL0:;}struct _tuple14{void*f1;int f2;};
# 130
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 132
struct Cyc_Absyn_Vardecl*_tmp43=vd;struct Cyc_List_List*_tmp48;struct Cyc_Absyn_Exp*_tmp47;void*_tmp46;struct _tuple0*_tmp45;enum Cyc_Absyn_Scope _tmp44;_LL1: _tmp44=_tmp43->sc;_tmp45=_tmp43->name;_tmp46=_tmp43->type;_tmp47=_tmp43->initializer;_tmp48=_tmp43->attributes;_LL2: {enum Cyc_Absyn_Scope sc=_tmp44;struct _tuple0*q=_tmp45;void*t=_tmp46;struct Cyc_Absyn_Exp*initializer=_tmp47;struct Cyc_List_List*atts=_tmp48;
# 139
{void*_tmp49=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmp49;void*_tmp4A=_stmttmp6;unsigned _tmp4E;void*_tmp4D;struct Cyc_Absyn_Tqual _tmp4C;void*_tmp4B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->tag == 4U){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).num_elts == 0){_LL4: _tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).elt_type;_tmp4C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).tq;_tmp4D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).zero_term;_tmp4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).zt_loc;if(initializer != 0){_LL5: {void*telt=_tmp4B;struct Cyc_Absyn_Tqual tq=_tmp4C;void*zt=_tmp4D;unsigned ztl=_tmp4E;
# 141
{void*_tmp4F=initializer->r;void*_stmttmp7=_tmp4F;void*_tmp50=_stmttmp7;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Exp*_tmp54;struct _fat_ptr _tmp55;struct _fat_ptr _tmp56;switch(*((int*)_tmp50)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp50)->f1).Wstring_c).tag){case 8U: _LL9: _tmp56=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp50)->f1).String_c).val;_LLA: {struct _fat_ptr s=_tmp56;
# 143
t=({void*_tmp249=({void*_tmp248=telt;struct Cyc_Absyn_Tqual _tmp247=tq;struct Cyc_Absyn_Exp*_tmp246=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_tmp245=zt;Cyc_Absyn_array_type(_tmp248,_tmp247,_tmp246,_tmp245,ztl);});vd->type=_tmp249;});
goto _LL8;}case 9U: _LLB: _tmp55=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp50)->f1).Wstring_c).val;_LLC: {struct _fat_ptr s=_tmp55;
# 146
t=({void*_tmp24E=({void*_tmp24D=telt;struct Cyc_Absyn_Tqual _tmp24C=tq;struct Cyc_Absyn_Exp*_tmp24B=Cyc_Absyn_uint_exp(1U,0U);void*_tmp24A=zt;Cyc_Absyn_array_type(_tmp24D,_tmp24C,_tmp24B,_tmp24A,ztl);});vd->type=_tmp24E;});
goto _LL8;}default: goto _LL15;}case 27U: _LLD: _tmp54=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp54;
_tmp53=e;goto _LL10;}case 28U: _LLF: _tmp53=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp53;
# 151
t=({void*_tmp24F=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_tmp24F;});
goto _LL8;}case 36U: _LL11: _tmp52=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL12: {struct Cyc_List_List*es=_tmp52;
_tmp51=es;goto _LL14;}case 26U: _LL13: _tmp51=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL14: {struct Cyc_List_List*es=_tmp51;
# 155
t=({void*_tmp254=({void*_tmp253=telt;struct Cyc_Absyn_Tqual _tmp252=tq;struct Cyc_Absyn_Exp*_tmp251=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);void*_tmp250=zt;Cyc_Absyn_array_type(_tmp253,_tmp252,_tmp251,_tmp250,ztl);});vd->type=_tmp254;});
goto _LL8;}default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 159
goto _LL3;}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 163
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
# 165
({int _tmp255=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp255;});
# 168
({int _tmp256=!Cyc_Tcutil_is_array_type(t);vd->escapes=_tmp256;});
# 170
if(Cyc_Tcutil_is_function_type(t)){
atts=Cyc_Tcutil_transfer_fn_type_atts(t,atts);
Cyc_Tc_fnTypeAttsOK(te,t,loc);}
# 175
if((int)sc == (int)3U ||(int)sc == (int)4U){
if(initializer != 0 && !in_cinclude)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp58=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F6;_tmp1F6.tag=0U,({struct _fat_ptr _tmp257=({const char*_tmp59="extern declaration should not have initializer";_tag_fat(_tmp59,sizeof(char),47U);});_tmp1F6.f1=_tmp257;});_tmp1F6;});void*_tmp57[1U];_tmp57[0]=& _tmp58;({unsigned _tmp258=loc;Cyc_Warn_err2(_tmp258,_tag_fat(_tmp57,sizeof(void*),1U));});});}else{
if(!Cyc_Tcutil_is_function_type(t)){
# 182
for(0;atts != 0;atts=atts->tl){
void*_tmp5A=(void*)atts->hd;void*_stmttmp8=_tmp5A;void*_tmp5B=_stmttmp8;switch(*((int*)_tmp5B)){case 6U: _LL18: _LL19:
 goto _LL1B;case 8U: _LL1A: _LL1B:
# 187
 goto _LL1D;case 9U: _LL1C: _LL1D:
 goto _LL1F;case 10U: _LL1E: _LL1F:
 goto _LL21;case 11U: _LL20: _LL21:
 goto _LL23;case 12U: _LL22: _LL23:
 goto _LL25;case 13U: _LL24: _LL25:
 goto _LL27;case 14U: _LL26: _LL27:
 continue;default: _LL28: _LL29:
# 195
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FA;_tmp1FA.tag=0U,({struct _fat_ptr _tmp259=({const char*_tmp62="bad attribute ";_tag_fat(_tmp62,sizeof(char),15U);});_tmp1FA.f1=_tmp259;});_tmp1FA;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp1F9;_tmp1F9.tag=9U,_tmp1F9.f1=(void*)atts->hd;_tmp1F9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F8;_tmp1F8.tag=0U,({struct _fat_ptr _tmp25A=({const char*_tmp61=" for variable ";_tag_fat(_tmp61,sizeof(char),15U);});_tmp1F8.f1=_tmp25A;});_tmp1F8;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp60=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp1F7;_tmp1F7.tag=10U,_tmp1F7.f1=vd;_tmp1F7;});void*_tmp5C[4U];_tmp5C[0]=& _tmp5D,_tmp5C[1]=& _tmp5E,_tmp5C[2]=& _tmp5F,_tmp5C[3]=& _tmp60;({unsigned _tmp25B=loc;Cyc_Warn_err2(_tmp25B,_tag_fat(_tmp5C,sizeof(void*),4U));});});
goto _LL17;}_LL17:;}
# 199
if(initializer == 0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FE;_tmp1FE.tag=0U,({struct _fat_ptr _tmp25C=({const char*_tmp69="initializer required for variable ";_tag_fat(_tmp69,sizeof(char),35U);});_tmp1FE.f1=_tmp25C;});_tmp1FE;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp65=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp1FD;_tmp1FD.tag=10U,_tmp1FD.f1=vd;_tmp1FD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp66=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FC;_tmp1FC.tag=0U,({struct _fat_ptr _tmp25D=({const char*_tmp68=" of type ";_tag_fat(_tmp68,sizeof(char),10U);});_tmp1FC.f1=_tmp25D;});_tmp1FC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp67=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1FB;_tmp1FB.tag=2U,_tmp1FB.f1=(void*)t;_tmp1FB;});void*_tmp63[4U];_tmp63[0]=& _tmp64,_tmp63[1]=& _tmp65,_tmp63[2]=& _tmp66,_tmp63[3]=& _tmp67;({unsigned _tmp25E=loc;Cyc_Warn_err2(_tmp25E,_tag_fat(_tmp63,sizeof(void*),4U));});});}else{
# 203
struct Cyc_Absyn_Exp*_tmp6A=initializer;struct Cyc_Absyn_Exp*e=_tmp6A;
# 207
struct _handler_cons _tmp6B;_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!_tmp6D){
{void*_tmp6E=Cyc_Tcexp_tcExpInitializer(te,& t,e);void*t2=_tmp6E;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp260=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp25F=e;Cyc_Tcutil_coerce_assign(_tmp260,_tmp25F,t);})){
struct _fat_ptr _tmp6F=Cyc_Absynpp_qvar2string(vd->name);struct _fat_ptr s0=_tmp6F;
const char*_tmp70=" declared with type ";const char*s1=_tmp70;
struct _fat_ptr _tmp71=Cyc_Absynpp_typ2string(t);struct _fat_ptr s2=_tmp71;
const char*_tmp72=" but initialized with type ";const char*s3=_tmp72;
struct _fat_ptr _tmp73=Cyc_Absynpp_typ2string(t2);struct _fat_ptr s4=_tmp73;
if(({unsigned long _tmp264=({unsigned long _tmp263=({unsigned long _tmp262=({unsigned long _tmp261=Cyc_strlen((struct _fat_ptr)s0);_tmp261 + Cyc_strlen(({const char*_tmp74=s1;_tag_fat(_tmp74,sizeof(char),21U);}));});_tmp262 + Cyc_strlen((struct _fat_ptr)s2);});_tmp263 + Cyc_strlen(({const char*_tmp75=s3;_tag_fat(_tmp75,sizeof(char),28U);}));});_tmp264 + Cyc_strlen((struct _fat_ptr)s4);})> (unsigned long)70)
({struct Cyc_String_pa_PrintArg_struct _tmp78=({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0U,_tmp203.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp203;});struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0U,({struct _fat_ptr _tmp265=(struct _fat_ptr)({const char*_tmp7E=s1;_tag_fat(_tmp7E,sizeof(char),21U);});_tmp202.f1=_tmp265;});_tmp202;});struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0U,_tmp201.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp201;});struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0U,({struct _fat_ptr _tmp266=(struct _fat_ptr)({const char*_tmp7D=s3;_tag_fat(_tmp7D,sizeof(char),28U);});_tmp200.f1=_tmp266;});_tmp200;});struct Cyc_String_pa_PrintArg_struct _tmp7C=({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0U,_tmp1FF.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp1FF;});void*_tmp76[5U];_tmp76[0]=& _tmp78,_tmp76[1]=& _tmp79,_tmp76[2]=& _tmp7A,_tmp76[3]=& _tmp7B,_tmp76[4]=& _tmp7C;({unsigned _tmp268=loc;struct _fat_ptr _tmp267=({const char*_tmp77="%s%s\n\t%s\n%s\n\t%s";_tag_fat(_tmp77,sizeof(char),16U);});Cyc_Tcutil_terr(_tmp268,_tmp267,_tag_fat(_tmp76,sizeof(void*),5U));});});else{
# 218
({struct Cyc_String_pa_PrintArg_struct _tmp81=({struct Cyc_String_pa_PrintArg_struct _tmp208;_tmp208.tag=0U,_tmp208.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp208;});struct Cyc_String_pa_PrintArg_struct _tmp82=({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0U,({struct _fat_ptr _tmp269=(struct _fat_ptr)({const char*_tmp87=s1;_tag_fat(_tmp87,sizeof(char),21U);});_tmp207.f1=_tmp269;});_tmp207;});struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0U,_tmp206.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp206;});struct Cyc_String_pa_PrintArg_struct _tmp84=({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0U,({struct _fat_ptr _tmp26A=(struct _fat_ptr)({const char*_tmp86=s3;_tag_fat(_tmp86,sizeof(char),28U);});_tmp205.f1=_tmp26A;});_tmp205;});struct Cyc_String_pa_PrintArg_struct _tmp85=({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0U,_tmp204.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp204;});void*_tmp7F[5U];_tmp7F[0]=& _tmp81,_tmp7F[1]=& _tmp82,_tmp7F[2]=& _tmp83,_tmp7F[3]=& _tmp84,_tmp7F[4]=& _tmp85;({unsigned _tmp26C=loc;struct _fat_ptr _tmp26B=({const char*_tmp80="%s%s%s%s%s";_tag_fat(_tmp80,sizeof(char),11U);});Cyc_Tcutil_terr(_tmp26C,_tmp26B,_tag_fat(_tmp7F,sizeof(void*),5U));});});}
Cyc_Unify_explain_failure();}
# 222
if(!Cyc_Tcutil_is_const_exp(e))
({void*_tmp88=0U;({unsigned _tmp26E=loc;struct _fat_ptr _tmp26D=({const char*_tmp89="initializer is not a constant expression";_tag_fat(_tmp89,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp26E,_tmp26D,_tag_fat(_tmp88,sizeof(void*),0U));});});}
# 208
;_pop_handler();}else{void*_tmp6C=(void*)Cyc_Core_get_exn_thrown();void*_tmp8A=_tmp6C;void*_tmp8B;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp8A)->tag == Cyc_Tcenv_Env_error){_LL2B: _LL2C:
# 226
({void*_tmp8C=0U;({unsigned _tmp270=loc;struct _fat_ptr _tmp26F=({const char*_tmp8D="initializer is not a constant expression";_tag_fat(_tmp8D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp270,_tmp26F,_tag_fat(_tmp8C,sizeof(void*),0U));});});
goto _LL2A;}else{_LL2D: _tmp8B=_tmp8A;_LL2E: {void*exn=_tmp8B;(int)_rethrow(exn);}}_LL2A:;}}}}else{
# 232
for(0;atts != 0;atts=atts->tl){
void*_tmp8E=(void*)atts->hd;void*_stmttmp9=_tmp8E;void*_tmp8F=_stmttmp9;switch(*((int*)_tmp8F)){case 0U: _LL30: _LL31:
# 235
 goto _LL33;case 1U: _LL32: _LL33:
 goto _LL35;case 2U: _LL34: _LL35:
 goto _LL37;case 3U: _LL36: _LL37:
 goto _LL39;case 4U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 goto _LL3D;case 20U: _LL3C: _LL3D:
 goto _LL3F;case 23U: _LL3E: _LL3F:
 goto _LL41;case 26U: _LL40: _LL41:
 goto _LL43;case 5U: _LL42: _LL43:
({void*_tmp90=0U;({struct _fat_ptr _tmp271=({const char*_tmp91="tcVardecl: fn type atts in function var decl";_tag_fat(_tmp91,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp271,_tag_fat(_tmp90,sizeof(void*),0U));});});case 6U: _LL44: _LL45:
# 246
 goto _LL47;case 7U: _LL46: _LL47:
# 248
({struct Cyc_String_pa_PrintArg_struct _tmp94=({struct Cyc_String_pa_PrintArg_struct _tmp209;_tmp209.tag=0U,({
struct _fat_ptr _tmp272=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp209.f1=_tmp272;});_tmp209;});void*_tmp92[1U];_tmp92[0]=& _tmp94;({unsigned _tmp274=loc;struct _fat_ptr _tmp273=({const char*_tmp93="bad attribute %s in function declaration";_tag_fat(_tmp93,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp274,_tmp273,_tag_fat(_tmp92,sizeof(void*),1U));});});
goto _LL2F;default: _LL48: _LL49:
 continue;}_LL2F:;}}}
# 256
{struct _handler_cons _tmp95;_push_handler(& _tmp95);{int _tmp97=0;if(setjmp(_tmp95.handler))_tmp97=1;if(!_tmp97){
{struct _tuple14*_tmp98=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple14*ans=_tmp98;
void*_tmp99=(*ans).f1;void*b0=_tmp99;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9A=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=1U,_tmp9D->f1=vd;_tmp9D;});struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1=_tmp9A;
void*_tmp9B=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmp9B;
if(b == 0){_npop_handler(0U);return;}
# 263
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports)){
if(b != b0 ||(*ans).f2)
# 266
({struct Cyc_Dict_Dict _tmp277=({struct Cyc_Dict_Dict _tmp276=(te->ae)->ordinaries;struct _tuple0*_tmp275=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp276,_tmp275,({struct _tuple14*_tmp9C=_cycalloc(sizeof(*_tmp9C));
_tmp9C->f1=b,_tmp9C->f2=(*ans).f2;_tmp9C;}));});
# 266
(te->ae)->ordinaries=_tmp277;});}
# 268
_npop_handler(0U);return;}
# 257
;_pop_handler();}else{void*_tmp96=(void*)Cyc_Core_get_exn_thrown();void*_tmp9E=_tmp96;void*_tmp9F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp9E)->tag == Cyc_Dict_Absent){_LL4B: _LL4C:
# 269
 goto _LL4A;}else{_LL4D: _tmp9F=_tmp9E;_LL4E: {void*exn=_tmp9F;(int)_rethrow(exn);}}_LL4A:;}}}
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp27B=({struct Cyc_Dict_Dict _tmp27A=(te->ae)->ordinaries;struct _tuple0*_tmp279=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp27A,_tmp279,({struct _tuple14*_tmpA1=_cycalloc(sizeof(*_tmpA1));
({void*_tmp278=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=1U,_tmpA0->f1=vd;_tmpA0;});_tmpA1->f1=_tmp278;}),_tmpA1->f2=0;_tmpA1;}));});
# 271
(te->ae)->ordinaries=_tmp27B;});}}
# 275
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmpA2=n;struct _fat_ptr*_tmpA4;union Cyc_Absyn_Nmspace _tmpA3;_LL1: _tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;_LL2: {union Cyc_Absyn_Nmspace nms=_tmpA3;struct _fat_ptr*v=_tmpA4;
union Cyc_Absyn_Nmspace _tmpA5=nms;if((_tmpA5.Abs_n).tag == 2){if((_tmpA5.Abs_n).val == 0){_LL4: _LL5:
# 279
 return({struct _fat_ptr _tmp27C=(struct _fat_ptr)*v;Cyc_strcmp(_tmp27C,({const char*_tmpA6="main";_tag_fat(_tmpA6,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;}}
# 284
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 286
struct _tuple0*q=fd->name;
# 290
if((int)fd->sc == (int)Cyc_Absyn_ExternC && !te->in_extern_c_include)
({void*_tmpA7=0U;({unsigned _tmp27E=loc;struct _fat_ptr _tmp27D=({const char*_tmpA8="extern \"C\" functions cannot be implemented in Cyclone";_tag_fat(_tmpA8,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp27E,_tmp27D,_tag_fat(_tmpA7,sizeof(void*),0U));});});
# 294
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpA9=(void*)atts->hd;void*_stmttmpA=_tmpA9;void*_tmpAA=_stmttmpA;switch(*((int*)_tmpAA)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
# 298
({struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmp20A;_tmp20A.tag=0U,({struct _fat_ptr _tmp27F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp20A.f1=_tmp27F;});_tmp20A;});void*_tmpAB[1U];_tmpAB[0]=& _tmpAD;({unsigned _tmp281=loc;struct _fat_ptr _tmp280=({const char*_tmpAC="bad attribute %s for function";_tag_fat(_tmpAC,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp281,_tmp280,_tag_fat(_tmpAB,sizeof(void*),1U));});});
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}}
# 304
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 307
({struct Cyc_List_List*_tmp282=Cyc_Tcutil_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp282;});
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 311
{struct _handler_cons _tmpAE;_push_handler(& _tmpAE);{int _tmpB0=0;if(setjmp(_tmpAE.handler))_tmpB0=1;if(!_tmpB0){
{struct _tuple14*_tmpB1=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple14*ans=_tmpB1;
void*_tmpB2=(*ans).f1;void*b0=_tmpB2;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB3=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->tag=2U,_tmpB6->f1=fd;_tmpB6;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1=_tmpB3;
void*_tmpB4=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmpB4;
if(b != 0){
# 318
if(exports == 0 || Cyc_Tc_export_member(q,*exports)){
if(!(b == b0 &&(*ans).f2))
({struct Cyc_Dict_Dict _tmp285=({struct Cyc_Dict_Dict _tmp284=(te->ae)->ordinaries;struct _tuple0*_tmp283=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp284,_tmp283,({struct _tuple14*_tmpB5=_cycalloc(sizeof(*_tmpB5));
_tmpB5->f1=b,_tmpB5->f2=(*ans).f2;_tmpB5;}));});
# 320
(te->ae)->ordinaries=_tmp285;});}}}
# 312
;_pop_handler();}else{void*_tmpAF=(void*)Cyc_Core_get_exn_thrown();void*_tmpB7=_tmpAF;void*_tmpB8;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB7)->tag == Cyc_Dict_Absent){_LL8: _LL9:
# 324
 if(exports == 0 || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB9=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->tag=2U,_tmpBC->f1=fd;_tmpBC;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b=_tmpB9;
({struct Cyc_Dict_Dict _tmp289=({struct Cyc_Dict_Dict _tmp288=(te->ae)->ordinaries;struct _tuple0*_tmp287=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp288,_tmp287,({struct _tuple14*_tmpBB=_cycalloc(sizeof(*_tmpBB));
({void*_tmp286=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->tag=2U,_tmpBA->f1=fd;_tmpBA;});_tmpBB->f1=_tmp286;}),_tmpBB->f2=0;_tmpBB;}));});
# 326
(te->ae)->ordinaries=_tmp289;});}
# 329
goto _LL7;}else{_LLA: _tmpB8=_tmpB7;_LLB: {void*exn=_tmpB8;(int)_rethrow(exn);}}_LL7:;}}}
# 333
if(te->in_extern_c_include)return;{
# 338
struct Cyc_Tcenv_Fenv*_tmpBD=Cyc_Tcenv_new_fenv(loc,fd);struct Cyc_Tcenv_Fenv*fenv=_tmpBD;
struct Cyc_Tcenv_Tenv*_tmpBE=({struct Cyc_Tcenv_Tenv*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->ns=te->ns,_tmpE2->ae=te->ae,_tmpE2->le=fenv,_tmpE2->allow_valueof=0,_tmpE2->in_extern_c_include=0,_tmpE2->in_tempest=te->in_tempest,_tmpE2->tempest_generalize=te->tempest_generalize;_tmpE2;});struct Cyc_Tcenv_Tenv*te=_tmpBE;
# 341
Cyc_Tcstmt_tcStmt(te,fd->body,0);
# 344
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
# 347
if(te->in_tempest){
te->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);
te->tempest_generalize=0;}
# 353
if(Cyc_Tc_is_main(q)){
# 355
{void*_tmpBF=Cyc_Tcutil_compress((fd->i).ret_type);void*_stmttmpB=_tmpBF;void*_tmpC0=_stmttmpB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)){case 0U: _LLD: _LLE:
({void*_tmpC1=0U;({unsigned _tmp28B=loc;struct _fat_ptr _tmp28A=({const char*_tmpC2="main declared with return type void";_tag_fat(_tmpC2,sizeof(char),36U);});Cyc_Tcutil_warn(_tmp28B,_tmp28A,_tag_fat(_tmpC1,sizeof(void*),0U));});});goto _LLC;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC0)->f1)->f2){case Cyc_Absyn_Int_sz: _LLF: _LL10:
 goto _LLC;case Cyc_Absyn_Long_sz: _LL11: _LL12:
 goto _LLC;default: goto _LL13;}default: goto _LL13;}else{_LL13: _LL14:
# 360
({struct Cyc_String_pa_PrintArg_struct _tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0U,({
struct _fat_ptr _tmp28C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((fd->i).ret_type));_tmp20B.f1=_tmp28C;});_tmp20B;});void*_tmpC3[1U];_tmpC3[0]=& _tmpC5;({unsigned _tmp28E=loc;struct _fat_ptr _tmp28D=({const char*_tmpC4="main declared with return type %s instead of int or void";_tag_fat(_tmpC4,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp28E,_tmp28D,_tag_fat(_tmpC3,sizeof(void*),1U));});});
goto _LLC;}_LLC:;}
# 364
if((fd->i).c_varargs ||(fd->i).cyc_varargs != 0)
({void*_tmpC6=0U;({unsigned _tmp290=loc;struct _fat_ptr _tmp28F=({const char*_tmpC7="main declared with varargs";_tag_fat(_tmpC7,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp290,_tmp28F,_tag_fat(_tmpC6,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpC8=(fd->i).args;struct Cyc_List_List*args=_tmpC8;
if(args != 0){
struct _tuple8*_tmpC9=(struct _tuple8*)args->hd;struct _tuple8*_stmttmpC=_tmpC9;struct _tuple8*_tmpCA=_stmttmpC;void*_tmpCB;_LL16: _tmpCB=_tmpCA->f3;_LL17: {void*t1=_tmpCB;
{void*_tmpCC=Cyc_Tcutil_compress(t1);void*_stmttmpD=_tmpCC;void*_tmpCD=_stmttmpD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCD)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCD)->f1)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCD)->f1)->f2){case Cyc_Absyn_Int_sz: _LL19: _LL1A:
 goto _LL18;case Cyc_Absyn_Long_sz: _LL1B: _LL1C:
 goto _LL18;default: goto _LL1D;}else{goto _LL1D;}}else{_LL1D: _LL1E:
# 373
({struct Cyc_String_pa_PrintArg_struct _tmpD0=({struct Cyc_String_pa_PrintArg_struct _tmp20C;_tmp20C.tag=0U,({
struct _fat_ptr _tmp291=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp20C.f1=_tmp291;});_tmp20C;});void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({unsigned _tmp293=loc;struct _fat_ptr _tmp292=({const char*_tmpCF="main declared with first argument of type %s instead of int";_tag_fat(_tmpCF,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp293,_tmp292,_tag_fat(_tmpCE,sizeof(void*),1U));});});
goto _LL18;}_LL18:;}
# 377
args=args->tl;
if(args != 0){
struct _tuple8*_tmpD1=(struct _tuple8*)args->hd;struct _tuple8*_stmttmpE=_tmpD1;struct _tuple8*_tmpD2=_stmttmpE;void*_tmpD3;_LL20: _tmpD3=_tmpD2->f3;_LL21: {void*t2=_tmpD3;
args=args->tl;
if(args != 0)
({void*_tmpD4=0U;({unsigned _tmp295=loc;struct _fat_ptr _tmp294=({const char*_tmpD5="main declared with too many arguments";_tag_fat(_tmpD5,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp295,_tmp294,_tag_fat(_tmpD4,sizeof(void*),0U));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=(fd->i).tvars;_tmpE1;});
if(((!({void*_tmp2AD=t2;Cyc_Unify_unify(_tmp2AD,({void*_tmp2AC=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp2A8=({struct Cyc_Core_Opt*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->v=& Cyc_Tcutil_rk;_tmpD6;});Cyc_Absyn_new_evar(_tmp2A8,tvs);}));void*_tmp2AB=({
struct Cyc_Core_Opt*_tmp2A9=({struct Cyc_Core_Opt*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->v=& Cyc_Tcutil_rk;_tmpD7;});Cyc_Absyn_new_evar(_tmp2A9,tvs);});
# 384
struct Cyc_Absyn_Tqual _tmp2AA=
# 386
Cyc_Absyn_empty_tqual(0U);
# 384
Cyc_Absyn_fatptr_type(_tmp2AC,_tmp2AB,_tmp2AA,
# 386
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));})&& !({
void*_tmp2A7=t2;Cyc_Unify_unify(_tmp2A7,({void*_tmp2A6=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp2A2=({struct Cyc_Core_Opt*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->v=& Cyc_Tcutil_rk;_tmpD8;});Cyc_Absyn_new_evar(_tmp2A2,tvs);}));void*_tmp2A5=({
# 389
struct Cyc_Core_Opt*_tmp2A3=({struct Cyc_Core_Opt*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->v=& Cyc_Tcutil_rk;_tmpD9;});Cyc_Absyn_new_evar(_tmp2A3,tvs);});
# 387
struct Cyc_Absyn_Tqual _tmp2A4=
# 390
Cyc_Absyn_empty_tqual(0U);
# 387
Cyc_Absyn_fatptr_type(_tmp2A6,_tmp2A5,_tmp2A4,
# 390
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp2A1=t2;Cyc_Unify_unify(_tmp2A1,({void*_tmp2A0=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp29C=({struct Cyc_Core_Opt*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->v=& Cyc_Tcutil_rk;_tmpDA;});Cyc_Absyn_new_evar(_tmp29C,tvs);}));void*_tmp29F=({
struct Cyc_Core_Opt*_tmp29D=({struct Cyc_Core_Opt*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->v=& Cyc_Tcutil_rk;_tmpDB;});Cyc_Absyn_new_evar(_tmp29D,tvs);});
# 391
struct Cyc_Absyn_Tqual _tmp29E=
# 393
Cyc_Absyn_const_tqual(0U);
# 391
Cyc_Absyn_fatptr_type(_tmp2A0,_tmp29F,_tmp29E,
# 393
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp29B=t2;Cyc_Unify_unify(_tmp29B,({void*_tmp29A=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp296=({struct Cyc_Core_Opt*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->v=& Cyc_Tcutil_rk;_tmpDC;});Cyc_Absyn_new_evar(_tmp296,tvs);}));void*_tmp299=({
# 396
struct Cyc_Core_Opt*_tmp297=({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->v=& Cyc_Tcutil_rk;_tmpDD;});Cyc_Absyn_new_evar(_tmp297,tvs);});
# 394
struct Cyc_Absyn_Tqual _tmp298=
# 397
Cyc_Absyn_const_tqual(0U);
# 394
Cyc_Absyn_fatptr_type(_tmp29A,_tmp299,_tmp298,
# 397
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))
({struct Cyc_String_pa_PrintArg_struct _tmpE0=({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0U,({
struct _fat_ptr _tmp2AE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp20D.f1=_tmp2AE;});_tmp20D;});void*_tmpDE[1U];_tmpDE[0]=& _tmpE0;({unsigned _tmp2B0=loc;struct _fat_ptr _tmp2AF=({const char*_tmpDF="second argument of main has type %s instead of char??";_tag_fat(_tmpDF,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp2B0,_tmp2AF,_tag_fat(_tmpDE,sizeof(void*),1U));});});}}}}}}}}}}
# 406
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 413
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpE5=({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0U,_tmp20E.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp20E;});void*_tmpE3[1U];_tmpE3[0]=& _tmpE5;({unsigned _tmp2B2=loc;struct _fat_ptr _tmp2B1=({const char*_tmpE4="redeclaration of typedef %s";_tag_fat(_tmpE4,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp2B2,_tmp2B1,_tag_fat(_tmpE3,sizeof(void*),1U));});});
return;}
# 418
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tctyp_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp2B3=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 422
(td->tq).real_const=_tmp2B3;});}
# 427
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpE6=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_stmttmpF=_tmpE6;void*_tmpE7=_stmttmpF;struct Cyc_Absyn_Kind*_tmpE9;struct Cyc_Core_Opt**_tmpE8;struct Cyc_Core_Opt**_tmpEA;switch(*((int*)_tmpE7)){case 1U: _LL1: _tmpEA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpE7)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpEA;
# 430
if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpED=({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0U,_tmp20F.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);_tmp20F;});void*_tmpEB[1U];_tmpEB[0]=& _tmpED;({unsigned _tmp2B5=loc;struct _fat_ptr _tmp2B4=({const char*_tmpEC="type variable %s is not used in typedef definition";_tag_fat(_tmpEC,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp2B5,_tmp2B4,_tag_fat(_tmpEB,sizeof(void*),1U));});});
# 433
({struct Cyc_Core_Opt*_tmp2B7=({struct Cyc_Core_Opt*_tmpEE=_cycalloc(sizeof(*_tmpEE));({void*_tmp2B6=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpEE->v=_tmp2B6;});_tmpEE;});*f=_tmp2B7;});goto _LL0;}case 2U: _LL3: _tmpE8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE7)->f1;_tmpE9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE7)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmpE8;struct Cyc_Absyn_Kind*k=_tmpE9;
# 440
({struct Cyc_Core_Opt*_tmp2B9=({struct Cyc_Core_Opt*_tmpEF=_cycalloc(sizeof(*_tmpEF));({void*_tmp2B8=Cyc_Tcutil_kind_to_bound(k);_tmpEF->v=_tmp2B8;});_tmpEF;});*f=_tmp2B9;});
goto _LL0;}default: _LL5: _LL6:
 continue;}_LL0:;}}
# 446
({struct Cyc_Dict_Dict _tmp2BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp2BA;});}struct _tuple15{void*f1;void*f2;};
# 449
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 456
struct _RegionHandle _tmpF0=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpF0;_push_region(uprev_rgn);
# 458
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple15*_tmpF1=(struct _tuple15*)rpo->hd;struct _tuple15*_stmttmp10=_tmpF1;struct _tuple15*_tmpF2=_stmttmp10;void*_tmpF4;void*_tmpF3;_LL1: _tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;_LL2: {void*e=_tmpF3;void*r=_tmpF4;
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,e);
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,r);}}{
# 464
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 467
struct Cyc_List_List*_tmpF5=fields;struct Cyc_List_List*fs=_tmpF5;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpF6=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp11=_tmpF6;struct Cyc_Absyn_Aggrfield*_tmpF7=_stmttmp11;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;void*_tmpFA;struct Cyc_Absyn_Tqual _tmpF9;struct _fat_ptr*_tmpF8;_LL4: _tmpF8=_tmpF7->name;_tmpF9=_tmpF7->tq;_tmpFA=_tmpF7->type;_tmpFB=_tmpF7->width;_tmpFC=_tmpF7->attributes;_tmpFD=_tmpF7->requires_clause;_LL5: {struct _fat_ptr*fn=_tmpF8;struct Cyc_Absyn_Tqual tq=_tmpF9;void*t=_tmpFA;struct Cyc_Absyn_Exp*width=_tmpFB;struct Cyc_List_List*atts=_tmpFC;struct Cyc_Absyn_Exp*requires_clause=_tmpFD;
# 470
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_String_pa_PrintArg_struct _tmp100=({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0U,_tmp210.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp210;});void*_tmpFE[1U];_tmpFE[0]=& _tmp100;({unsigned _tmp2BC=loc;struct _fat_ptr _tmp2BB=({const char*_tmpFF="duplicate member %s";_tag_fat(_tmpFF,sizeof(char),20U);});Cyc_Tcutil_terr(_tmp2BC,_tmp2BB,_tag_fat(_tmpFE,sizeof(void*),1U));});});
# 474
if(({struct _fat_ptr _tmp2BD=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp2BD,({const char*_tmp101="";_tag_fat(_tmp101,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp102=_region_malloc(uprev_rgn,sizeof(*_tmp102));_tmp102->hd=fn,_tmp102->tl=prev_fields;_tmp102;});{
# 477
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 481
if((int)str_or_union == (int)1U ||
 fs->tl == 0 &&(int)str_or_union == (int)0U)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tctyp_check_type(loc,te,tvs,field_kind,0,0,t);
# 486
({int _tmp2BE=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).print_const,t);(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).real_const=_tmp2BE;});
# 489
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
# 491
if((unsigned)requires_clause){
if((int)str_or_union != (int)1U)
({void*_tmp103=0U;({unsigned _tmp2C0=loc;struct _fat_ptr _tmp2BF=({const char*_tmp104="@requires clauses are only allowed on union members";_tag_fat(_tmp104,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2C0,_tmp2BF,_tag_fat(_tmp103,sizeof(void*),0U));});});{
struct Cyc_Tcenv_Tenv*_tmp105=Cyc_Tcenv_allow_valueof(te);struct Cyc_Tcenv_Tenv*te2=_tmp105;
Cyc_Tcexp_tcExp(te2,0,requires_clause);
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_String_pa_PrintArg_struct _tmp108=({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0U,({
# 499
struct _fat_ptr _tmp2C1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(requires_clause->topt)));_tmp211.f1=_tmp2C1;});_tmp211;});void*_tmp106[1U];_tmp106[0]=& _tmp108;({unsigned _tmp2C3=requires_clause->loc;struct _fat_ptr _tmp2C2=({const char*_tmp107="@requires clause has type %s instead of integral type";_tag_fat(_tmp107,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp2C3,_tmp2C2,_tag_fat(_tmp106,sizeof(void*),1U));});});else{
# 501
({unsigned _tmp2C6=requires_clause->loc;struct Cyc_Tcenv_Tenv*_tmp2C5=te;struct Cyc_List_List*_tmp2C4=tvs;Cyc_Tctyp_check_type(_tmp2C6,_tmp2C5,_tmp2C4,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->tag=9U,_tmp109->f1=requires_clause;_tmp109;}));});{
# 503
struct Cyc_List_List*_tmp10A=Cyc_Relations_exp2relns(uprev_rgn,requires_clause);struct Cyc_List_List*relns=_tmp10A;
# 510
if(!Cyc_Relations_consistent_relations(relns))
({void*_tmp10B=0U;({unsigned _tmp2C8=requires_clause->loc;struct _fat_ptr _tmp2C7=({const char*_tmp10C="@requires clause may be unsatisfiable";_tag_fat(_tmp10C,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp2C8,_tmp2C7,_tag_fat(_tmp10B,sizeof(void*),0U));});});
# 516
{struct Cyc_List_List*_tmp10D=prev_relations;struct Cyc_List_List*p=_tmp10D;for(0;p != 0;p=p->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,relns,(struct Cyc_List_List*)p->hd)))
({void*_tmp10E=0U;({unsigned _tmp2CA=requires_clause->loc;struct _fat_ptr _tmp2C9=({const char*_tmp10F="@requires clause may overlap with previous clauses";_tag_fat(_tmp10F,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2CA,_tmp2C9,_tag_fat(_tmp10E,sizeof(void*),0U));});});}}
# 521
prev_relations=({struct Cyc_List_List*_tmp110=_region_malloc(uprev_rgn,sizeof(*_tmp110));_tmp110->hd=relns,_tmp110->tl=prev_relations;_tmp110;});}}}}else{
# 524
if(prev_relations != 0)
({void*_tmp111=0U;({unsigned _tmp2CC=loc;struct _fat_ptr _tmp2CB=({const char*_tmp112="if one field has a @requires clause, they all must";_tag_fat(_tmp112,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2CC,_tmp2CB,_tag_fat(_tmp111,sizeof(void*),0U));});});}}}}}
# 458
;_pop_region(uprev_rgn);}
# 530
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 532
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp113=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmp12=_tmp113;void*_tmp114=_stmttmp12;enum Cyc_Absyn_AliasQual _tmp115;struct Cyc_Absyn_Kind*_tmp117;struct Cyc_Core_Opt**_tmp116;struct Cyc_Core_Opt**_tmp118;enum Cyc_Absyn_AliasQual _tmp11A;struct Cyc_Core_Opt**_tmp119;struct Cyc_Core_Opt**_tmp11B;switch(*((int*)_tmp114)){case 1U: _LL1: _tmp11B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp114)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp11B;
# 535
({struct Cyc_Core_Opt*_tmp2CE=({struct Cyc_Core_Opt*_tmp11C=_cycalloc(sizeof(*_tmp11C));({void*_tmp2CD=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp11C->v=_tmp2CD;});_tmp11C;});*f=_tmp2CE;});continue;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmp119=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114)->f1;_tmp11A=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114)->f2)->aliasqual;_LL4: {struct Cyc_Core_Opt**f=_tmp119;enum Cyc_Absyn_AliasQual a=_tmp11A;
# 537
if(constrain_top_kind &&(int)a == (int)2U)
({struct Cyc_Core_Opt*_tmp2D0=({struct Cyc_Core_Opt*_tmp11E=_cycalloc(sizeof(*_tmp11E));({void*_tmp2CF=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->kind=Cyc_Absyn_BoxKind,_tmp11D->aliasqual=Cyc_Absyn_Aliasable;_tmp11D;}));_tmp11E->v=_tmp2CF;});_tmp11E;});*f=_tmp2D0;});else{
# 540
({struct Cyc_Core_Opt*_tmp2D2=({struct Cyc_Core_Opt*_tmp120=_cycalloc(sizeof(*_tmp120));({void*_tmp2D1=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->kind=Cyc_Absyn_BoxKind,_tmp11F->aliasqual=a;_tmp11F;}));_tmp120->v=_tmp2D1;});_tmp120;});*f=_tmp2D2;});}
continue;}case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmp118=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114)->f1;if(constrain_top_kind){_LL6: {struct Cyc_Core_Opt**f=_tmp118;
# 543
({struct Cyc_Core_Opt*_tmp2D4=({struct Cyc_Core_Opt*_tmp122=_cycalloc(sizeof(*_tmp122));({void*_tmp2D3=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->kind=Cyc_Absyn_BoxKind,_tmp121->aliasqual=Cyc_Absyn_Aliasable;_tmp121;}));_tmp122->v=_tmp2D3;});_tmp122;});*f=_tmp2D4;});
continue;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmp116=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114)->f1;_tmp117=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114)->f2;_LL8: {struct Cyc_Core_Opt**f=_tmp116;struct Cyc_Absyn_Kind*k=_tmp117;
# 546
({struct Cyc_Core_Opt*_tmp2D6=({struct Cyc_Core_Opt*_tmp123=_cycalloc(sizeof(*_tmp123));({void*_tmp2D5=Cyc_Tcutil_kind_to_bound(k);_tmp123->v=_tmp2D5;});_tmp123;});*f=_tmp2D6;});continue;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp114)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmp115=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp114)->f1)->aliasqual;_LLA: {enum Cyc_Absyn_AliasQual a=_tmp115;
# 548
({struct Cyc_String_pa_PrintArg_struct _tmp126=({struct Cyc_String_pa_PrintArg_struct _tmp214;_tmp214.tag=0U,_tmp214.f1=(struct _fat_ptr)((struct _fat_ptr)*(*n).f2);_tmp214;});struct Cyc_String_pa_PrintArg_struct _tmp127=({struct Cyc_String_pa_PrintArg_struct _tmp213;_tmp213.tag=0U,_tmp213.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp213;});struct Cyc_String_pa_PrintArg_struct _tmp128=({struct Cyc_String_pa_PrintArg_struct _tmp212;_tmp212.tag=0U,({
struct _fat_ptr _tmp2D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->kind=Cyc_Absyn_MemKind,_tmp129->aliasqual=a;_tmp129;})));_tmp212.f1=_tmp2D7;});_tmp212;});void*_tmp124[3U];_tmp124[0]=& _tmp126,_tmp124[1]=& _tmp127,_tmp124[2]=& _tmp128;({unsigned _tmp2D9=loc;struct _fat_ptr _tmp2D8=({const char*_tmp125="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp125,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2D9,_tmp2D8,_tag_fat(_tmp124,sizeof(void*),3U));});});
continue;}}else{_LLB: _LLC:
 continue;}}_LL0:;}}struct _tuple16{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 556
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp12A=ad->name;struct _tuple0*q=_tmp12A;
# 563
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp12B=(void*)atts->hd;void*_stmttmp13=_tmp12B;void*_tmp12C=_stmttmp13;switch(*((int*)_tmp12C)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
# 568
({struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp216;_tmp216.tag=0U,({
struct _fat_ptr _tmp2DA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp216.f1=_tmp2DA;});_tmp216;});struct Cyc_String_pa_PrintArg_struct _tmp130=({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0U,_tmp215.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp215;});void*_tmp12D[2U];_tmp12D[0]=& _tmp12F,_tmp12D[1]=& _tmp130;({unsigned _tmp2DC=loc;struct _fat_ptr _tmp2DB=({const char*_tmp12E="bad attribute %s in %s definition";_tag_fat(_tmp12E,sizeof(char),34U);});Cyc_Tcutil_terr(_tmp2DC,_tmp2DB,_tag_fat(_tmp12D,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}}{
# 573
struct Cyc_List_List*_tmp131=ad->tvs;struct Cyc_List_List*tvs=_tmp131;
# 576
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 581
struct _tuple16 _tmp132=({struct _tuple16 _tmp21A;_tmp21A.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp2DD=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,q);_tmp21A.f2=_tmp2DD;});_tmp21A;});struct _tuple16 _stmttmp14=_tmp132;struct _tuple16 _tmp133=_stmttmp14;struct Cyc_Absyn_Aggrdecl**_tmp138;int _tmp137;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp134;int _tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp139;struct Cyc_Absyn_Aggrdecl**_tmp13D;if(_tmp133.f1 == 0){if(_tmp133.f2 == 0){_LL8: _LL9:
# 584
 Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 586
({struct Cyc_Dict_Dict _tmp2E0=({struct Cyc_Dict_Dict _tmp2DF=(te->ae)->aggrdecls;struct _tuple0*_tmp2DE=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp2DF,_tmp2DE,({struct Cyc_Absyn_Aggrdecl**_tmp13E=_cycalloc(sizeof(*_tmp13E));*_tmp13E=ad;_tmp13E;}));});(te->ae)->aggrdecls=_tmp2E0;});
goto _LL7;}else{_LLE: _tmp13D=*_tmp133.f2;_LLF: {struct Cyc_Absyn_Aggrdecl**adp=_tmp13D;
# 646
struct Cyc_Absyn_Aggrdecl*_tmp14F=Cyc_Tcdecl_merge_aggrdecl(*adp,ad,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Aggrdecl*ad2=_tmp14F;
if(ad2 == 0)
return;else{
# 650
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 653
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,q,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 656
*adp=ad2;
ad=ad2;
goto _LL7;}}}}else{if(_tmp133.f2 == 0){_LLA: _tmp139=(_tmp133.f1)->exist_vars;_tmp13A=(_tmp133.f1)->rgn_po;_tmp13B=(_tmp133.f1)->fields;_tmp13C=(_tmp133.f1)->tagged;_LLB: {struct Cyc_List_List*exist_vars=_tmp139;struct Cyc_List_List*rgn_po=_tmp13A;struct Cyc_List_List*fs=_tmp13B;int tagged=_tmp13C;
# 591
struct Cyc_Absyn_Aggrdecl**_tmp13F=({struct Cyc_Absyn_Aggrdecl**_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Aggrdecl*_tmp2E1=({struct Cyc_Absyn_Aggrdecl*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->kind=ad->kind,_tmp147->sc=Cyc_Absyn_Extern,_tmp147->name=ad->name,_tmp147->tvs=tvs,_tmp147->impl=0,_tmp147->attributes=ad->attributes,_tmp147->expected_mem_kind=0;_tmp147;});*_tmp148=_tmp2E1;});_tmp148;});struct Cyc_Absyn_Aggrdecl**adp=_tmp13F;
# 593
({struct Cyc_Dict_Dict _tmp2E2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,q,adp);(te->ae)->aggrdecls=_tmp2E2;});
# 598
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 602
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp140=0U;({unsigned _tmp2E4=loc;struct _fat_ptr _tmp2E3=({const char*_tmp141="@tagged qualifier is only allowed on union declarations";_tag_fat(_tmp141,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2E4,_tmp2E3,_tag_fat(_tmp140,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2E9=te;unsigned _tmp2E8=loc;enum Cyc_Absyn_AggrKind _tmp2E7=ad->kind;struct Cyc_List_List*_tmp2E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp2E5=rgn_po;Cyc_Tc_tcAggrImpl(_tmp2E9,_tmp2E8,_tmp2E7,_tmp2E6,_tmp2E5,fs);});
# 606
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 609
Cyc_Tc_rule_out_memkind(loc,q,exist_vars,1);
# 611
if((int)ad->kind == (int)Cyc_Absyn_UnionA && !tagged){
# 614
struct Cyc_List_List*f=fs;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp144=({struct Cyc_String_pa_PrintArg_struct _tmp219;_tmp219.tag=0U,_tmp219.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);_tmp219;});struct Cyc_String_pa_PrintArg_struct _tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp218;_tmp218.tag=0U,_tmp218.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp218;});struct Cyc_String_pa_PrintArg_struct _tmp146=({struct Cyc_String_pa_PrintArg_struct _tmp217;_tmp217.tag=0U,({
struct _fat_ptr _tmp2EA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));_tmp217.f1=_tmp2EA;});_tmp217;});void*_tmp142[3U];_tmp142[0]=& _tmp144,_tmp142[1]=& _tmp145,_tmp142[2]=& _tmp146;({unsigned _tmp2EC=loc;struct _fat_ptr _tmp2EB=({const char*_tmp143="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_fat(_tmp143,sizeof(char),98U);});Cyc_Tcutil_warn(_tmp2EC,_tmp2EB,_tag_fat(_tmp142,sizeof(void*),3U));});});}}
# 620
*adp=ad;
goto _LL7;}}else{_LLC: _tmp134=(_tmp133.f1)->exist_vars;_tmp135=(_tmp133.f1)->rgn_po;_tmp136=(_tmp133.f1)->fields;_tmp137=(_tmp133.f1)->tagged;_tmp138=*_tmp133.f2;_LLD: {struct Cyc_List_List*exist_vars=_tmp134;struct Cyc_List_List*rgn_po=_tmp135;struct Cyc_List_List*fs=_tmp136;int tagged=_tmp137;struct Cyc_Absyn_Aggrdecl**adp=_tmp138;
# 624
if((int)ad->kind != (int)(*adp)->kind)
({void*_tmp149=0U;({unsigned _tmp2EE=loc;struct _fat_ptr _tmp2ED=({const char*_tmp14A="cannot reuse struct names for unions and vice-versa";_tag_fat(_tmp14A,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2EE,_tmp2ED,_tag_fat(_tmp149,sizeof(void*),0U));});});{
# 627
struct Cyc_Absyn_Aggrdecl*_tmp14B=*adp;struct Cyc_Absyn_Aggrdecl*ad0=_tmp14B;
# 629
({struct Cyc_Absyn_Aggrdecl*_tmp2EF=({struct Cyc_Absyn_Aggrdecl*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->kind=ad->kind,_tmp14C->sc=Cyc_Absyn_Extern,_tmp14C->name=ad->name,_tmp14C->tvs=tvs,_tmp14C->impl=0,_tmp14C->attributes=ad->attributes,_tmp14C->expected_mem_kind=0;_tmp14C;});*adp=_tmp2EF;});
# 635
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 638
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp14D=0U;({unsigned _tmp2F1=loc;struct _fat_ptr _tmp2F0=({const char*_tmp14E="@tagged qualifier is only allowed on union declarations";_tag_fat(_tmp14E,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2F1,_tmp2F0,_tag_fat(_tmp14D,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2F6=te;unsigned _tmp2F5=loc;enum Cyc_Absyn_AggrKind _tmp2F4=ad->kind;struct Cyc_List_List*_tmp2F3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp2F2=rgn_po;Cyc_Tc_tcAggrImpl(_tmp2F6,_tmp2F5,_tmp2F4,_tmp2F3,_tmp2F2,fs);});
# 642
*adp=ad0;
_tmp13D=adp;goto _LLF;}}}}_LL7:;}}}
# 663
static void Cyc_Tc_rule_out_mem_and_unique(unsigned loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp150=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmp15=_tmp150;void*_tmp151=_stmttmp15;enum Cyc_Absyn_AliasQual _tmp152;enum Cyc_Absyn_KindQual _tmp153;enum Cyc_Absyn_KindQual _tmp155;struct Cyc_Core_Opt**_tmp154;struct Cyc_Core_Opt**_tmp156;struct Cyc_Core_Opt**_tmp157;struct Cyc_Core_Opt**_tmp158;struct Cyc_Core_Opt**_tmp159;struct Cyc_Core_Opt**_tmp15A;struct Cyc_Core_Opt**_tmp15B;switch(*((int*)_tmp151)){case 1U: _LL1: _tmp15B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp151)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp15B;
_tmp15A=f;goto _LL4;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmp15A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp15A;
# 668
_tmp159=f;goto _LL6;}case Cyc_Absyn_Aliasable: _LL5: _tmp159=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp159;
# 670
({struct Cyc_Core_Opt*_tmp2F8=({struct Cyc_Core_Opt*_tmp15C=_cycalloc(sizeof(*_tmp15C));({void*_tmp2F7=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp15C->v=_tmp2F7;});_tmp15C;});*f=_tmp2F8;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmp158=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp158;
_tmp157=f;goto _LLA;}case Cyc_Absyn_Aliasable: _LL9: _tmp157=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f1;_LLA: {struct Cyc_Core_Opt**f=_tmp157;
# 673
({struct Cyc_Core_Opt*_tmp2FA=({struct Cyc_Core_Opt*_tmp15D=_cycalloc(sizeof(*_tmp15D));({void*_tmp2F9=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp15D->v=_tmp2F9;});_tmp15D;});*f=_tmp2FA;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmp156=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f1;_LLC: {struct Cyc_Core_Opt**f=_tmp156;
# 675
({struct Cyc_Core_Opt*_tmp2FC=({struct Cyc_Core_Opt*_tmp15E=_cycalloc(sizeof(*_tmp15E));({void*_tmp2FB=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp15E->v=_tmp2FB;});_tmp15E;});*f=_tmp2FC;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmp154=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f1;_tmp155=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp151)->f2)->kind;_LL10: {struct Cyc_Core_Opt**f=_tmp154;enum Cyc_Absyn_KindQual k=_tmp155;
# 679
_tmp153=k;goto _LL12;}}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp151)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp151)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 677
({struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp21C;_tmp21C.tag=0U,_tmp21C.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp21C;});struct Cyc_String_pa_PrintArg_struct _tmp162=({struct Cyc_String_pa_PrintArg_struct _tmp21B;_tmp21B.tag=0U,_tmp21B.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp21B;});void*_tmp15F[2U];_tmp15F[0]=& _tmp161,_tmp15F[1]=& _tmp162;({unsigned _tmp2FE=loc;struct _fat_ptr _tmp2FD=({const char*_tmp160="type %s attempts to abstract type variable %s of kind TR";_tag_fat(_tmp160,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2FE,_tmp2FD,_tag_fat(_tmp15F,sizeof(void*),2U));});});
goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp151)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmp153=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp151)->f1)->kind;_LL12: {enum Cyc_Absyn_KindQual k=_tmp153;
# 681
({struct Cyc_String_pa_PrintArg_struct _tmp165=({struct Cyc_String_pa_PrintArg_struct _tmp21F;_tmp21F.tag=0U,_tmp21F.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp21F;});struct Cyc_String_pa_PrintArg_struct _tmp166=({struct Cyc_String_pa_PrintArg_struct _tmp21E;_tmp21E.tag=0U,_tmp21E.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp21E;});struct Cyc_String_pa_PrintArg_struct _tmp167=({struct Cyc_String_pa_PrintArg_struct _tmp21D;_tmp21D.tag=0U,({
# 683
struct _fat_ptr _tmp2FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->kind=k,_tmp168->aliasqual=Cyc_Absyn_Unique;_tmp168;})));_tmp21D.f1=_tmp2FF;});_tmp21D;});void*_tmp163[3U];_tmp163[0]=& _tmp165,_tmp163[1]=& _tmp166,_tmp163[2]=& _tmp167;({unsigned _tmp301=loc;struct _fat_ptr _tmp300=({const char*_tmp164="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp164,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp301,_tmp300,_tag_fat(_tmp163,sizeof(void*),3U));});});goto _LL0;}}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp151)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmp152=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp151)->f1)->aliasqual;_LL14: {enum Cyc_Absyn_AliasQual a=_tmp152;
# 685
({struct Cyc_String_pa_PrintArg_struct _tmp16B=({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0U,_tmp222.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp222;});struct Cyc_String_pa_PrintArg_struct _tmp16C=({struct Cyc_String_pa_PrintArg_struct _tmp221;_tmp221.tag=0U,_tmp221.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp221;});struct Cyc_String_pa_PrintArg_struct _tmp16D=({struct Cyc_String_pa_PrintArg_struct _tmp220;_tmp220.tag=0U,({
# 687
struct _fat_ptr _tmp302=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->kind=Cyc_Absyn_MemKind,_tmp16E->aliasqual=a;_tmp16E;})));_tmp220.f1=_tmp302;});_tmp220;});void*_tmp169[3U];_tmp169[0]=& _tmp16B,_tmp169[1]=& _tmp16C,_tmp169[2]=& _tmp16D;({unsigned _tmp304=loc;struct _fat_ptr _tmp303=({const char*_tmp16A="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp16A,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp304,_tmp303,_tag_fat(_tmp169,sizeof(void*),3U));});});goto _LL0;}}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 693
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 701
{struct Cyc_List_List*_tmp16F=fields;struct Cyc_List_List*fs=_tmp16F;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp170=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp170;
struct Cyc_List_List*typs=f->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tctyp_check_type(f->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple17*)typs->hd)).f2);
# 706
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple17*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp223;_tmp223.tag=0U,({
# 709
struct _fat_ptr _tmp305=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp223.f1=_tmp305;});_tmp223;});void*_tmp171[1U];_tmp171[0]=& _tmp173;({unsigned _tmp307=f->loc;struct _fat_ptr _tmp306=({const char*_tmp172="noalias pointers in datatypes are not allowed (%s)";_tag_fat(_tmp172,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp307,_tmp306,_tag_fat(_tmp171,sizeof(void*),1U));});});
({int _tmp308=
Cyc_Tcutil_extract_const_from_typedef(f->loc,((*((struct _tuple17*)typs->hd)).f1).print_const,(*((struct _tuple17*)typs->hd)).f2);
# 710
((*((struct _tuple17*)typs->hd)).f1).real_const=_tmp308;});}}}{
# 716
struct Cyc_List_List*fields2;
if(is_extensible){
# 719
int _tmp174=1;int res=_tmp174;
struct Cyc_List_List*_tmp175=Cyc_Tcdecl_sort_xdatatype_fields(fields,& res,(*name).f2,loc,Cyc_Tc_tc_msg);struct Cyc_List_List*fs=_tmp175;
if(res)
fields2=fs;else{
# 724
fields2=0;}}else{
# 726
struct _RegionHandle _tmp176=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp176;_push_region(uprev_rgn);
# 728
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp177=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp177;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp17A=({struct Cyc_String_pa_PrintArg_struct _tmp225;_tmp225.tag=0U,_tmp225.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp225;});struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp224;_tmp224.tag=0U,_tmp224.f1=(struct _fat_ptr)((struct _fat_ptr)obj);_tmp224;});void*_tmp178[2U];_tmp178[0]=& _tmp17A,_tmp178[1]=& _tmp17B;({unsigned _tmp30A=f->loc;struct _fat_ptr _tmp309=({const char*_tmp179="duplicate field name %s in %s";_tag_fat(_tmp179,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp30A,_tmp309,_tag_fat(_tmp178,sizeof(void*),2U));});});else{
# 734
prev_fields=({struct Cyc_List_List*_tmp17C=_region_malloc(uprev_rgn,sizeof(*_tmp17C));_tmp17C->hd=(*f->name).f2,_tmp17C->tl=prev_fields;_tmp17C;});}
# 736
if((int)f->sc != (int)Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0U,_tmp226.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp226;});void*_tmp17D[1U];_tmp17D[0]=& _tmp17F;({unsigned _tmp30C=loc;struct _fat_ptr _tmp30B=({const char*_tmp17E="ignoring scope of field %s";_tag_fat(_tmp17E,sizeof(char),27U);});Cyc_Tcutil_warn(_tmp30C,_tmp30B,_tag_fat(_tmp17D,sizeof(void*),1U));});});
f->sc=Cyc_Absyn_Public;}}}
# 741
fields2=fields;}
# 728
;_pop_region(uprev_rgn);}
# 743
return fields2;}}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 746
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _fat_ptr obj=tud->is_extensible?({const char*_tmp195="@extensible datatype";_tag_fat(_tmp195,sizeof(char),21U);}):({const char*_tmp196="datatype";_tag_fat(_tmp196,sizeof(char),9U);});
# 753
struct Cyc_List_List*tvs=tud->tvs;
# 756
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 762
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 772 "tc.cyc"
{struct _handler_cons _tmp180;_push_handler(& _tmp180);{int _tmp182=0;if(setjmp(_tmp180.handler))_tmp182=1;if(!_tmp182){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt == 0 && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->tag=Cyc_Dict_Absent;_tmp183;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 778
({union Cyc_Absyn_Nmspace _tmp30D=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp30D;});}
# 773
;_pop_handler();}else{void*_tmp181=(void*)Cyc_Core_get_exn_thrown();void*_tmp184=_tmp181;void*_tmp185;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp184)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 782
struct Cyc_Absyn_Datatypedecl***_tmp186=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);struct Cyc_Absyn_Datatypedecl***tdopt=_tmp186;
tud_opt=(unsigned)tdopt?({struct Cyc_Absyn_Datatypedecl***_tmp187=_cycalloc(sizeof(*_tmp187));*_tmp187=*tdopt;_tmp187;}): 0;
goto _LL0;}}else{_LL3: _tmp185=_tmp184;_LL4: {void*exn=_tmp185;(int)_rethrow(exn);}}_LL0:;}}}{
# 789
struct _tuple18 _tmp188=({struct _tuple18 _tmp227;_tmp227.f1=tud->fields,_tmp227.f2=tud_opt;_tmp227;});struct _tuple18 _stmttmp16=_tmp188;struct _tuple18 _tmp189=_stmttmp16;struct Cyc_Absyn_Datatypedecl**_tmp18B;struct Cyc_List_List**_tmp18A;struct Cyc_List_List**_tmp18C;struct Cyc_Absyn_Datatypedecl**_tmp18D;if(_tmp189.f1 == 0){if(_tmp189.f2 == 0){_LL6: _LL7:
# 792
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp310=({struct Cyc_Dict_Dict _tmp30F=(te->ae)->datatypedecls;struct _tuple0*_tmp30E=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp30F,_tmp30E,({struct Cyc_Absyn_Datatypedecl**_tmp18E=_cycalloc(sizeof(*_tmp18E));*_tmp18E=tud;_tmp18E;}));});(te->ae)->datatypedecls=_tmp310;});
goto _LL5;}else{_LLC: _tmp18D=*_tmp189.f2;_LLD: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp18D;
# 825
struct Cyc_Absyn_Datatypedecl*_tmp194=Cyc_Tcdecl_merge_datatypedecl(*tudp,tud,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Datatypedecl*tud2=_tmp194;
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(tud2 == 0)
return;else{
# 830
*tudp=tud2;
goto _LL5;}}}}else{if(_tmp189.f2 == 0){_LL8: _tmp18C=(struct Cyc_List_List**)&(_tmp189.f1)->v;_LL9: {struct Cyc_List_List**fs=_tmp18C;
# 797
struct Cyc_Absyn_Datatypedecl**_tmp18F=({struct Cyc_Absyn_Datatypedecl**_tmp191=_cycalloc(sizeof(*_tmp191));({struct Cyc_Absyn_Datatypedecl*_tmp311=({struct Cyc_Absyn_Datatypedecl*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->sc=Cyc_Absyn_Extern,_tmp190->name=tud->name,_tmp190->tvs=tvs,_tmp190->fields=0,_tmp190->is_extensible=tud->is_extensible;_tmp190;});*_tmp191=_tmp311;});_tmp191;});struct Cyc_Absyn_Datatypedecl**tudp=_tmp18F;
# 799
({struct Cyc_Dict_Dict _tmp312=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,tudp);(te->ae)->datatypedecls=_tmp312;});
# 802
({struct Cyc_List_List*_tmp313=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp313;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*tudp=tud;
goto _LL5;}}else{_LLA: _tmp18A=(struct Cyc_List_List**)&(_tmp189.f1)->v;_tmp18B=*_tmp189.f2;_LLB: {struct Cyc_List_List**fs=_tmp18A;struct Cyc_Absyn_Datatypedecl**tudp=_tmp18B;
# 807
struct Cyc_Absyn_Datatypedecl*_tmp192=*tudp;struct Cyc_Absyn_Datatypedecl*tud0=_tmp192;
# 810
if((!tud->is_extensible &&(unsigned)tud0)&& tud0->is_extensible)
tud->is_extensible=1;
# 813
({struct Cyc_Absyn_Datatypedecl*_tmp314=({struct Cyc_Absyn_Datatypedecl*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->sc=Cyc_Absyn_Extern,_tmp193->name=tud->name,_tmp193->tvs=tvs,_tmp193->fields=0,_tmp193->is_extensible=tud->is_extensible;_tmp193;});*tudp=_tmp314;});
# 817
({struct Cyc_List_List*_tmp315=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp315;});
# 820
*tudp=tud0;
_tmp18D=tudp;goto _LLD;}}}_LL5:;}}}
# 836
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 843
if(ed->fields != 0){struct _RegionHandle _tmp197=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp197;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp198=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp198;
# 849
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp19B=({struct Cyc_String_pa_PrintArg_struct _tmp228;_tmp228.tag=0U,_tmp228.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp228;});void*_tmp199[1U];_tmp199[0]=& _tmp19B;({unsigned _tmp317=f->loc;struct _fat_ptr _tmp316=({const char*_tmp19A="duplicate enum constructor %s";_tag_fat(_tmp19A,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp317,_tmp316,_tag_fat(_tmp199,sizeof(void*),1U));});});else{
# 852
prev_fields=({struct Cyc_List_List*_tmp19C=_region_malloc(uprev_rgn,sizeof(*_tmp19C));_tmp19C->hd=(*f->name).f2,_tmp19C->tl=prev_fields;_tmp19C;});}
# 854
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,f->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp19F=({struct Cyc_String_pa_PrintArg_struct _tmp229;_tmp229.tag=0U,_tmp229.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp229;});void*_tmp19D[1U];_tmp19D[0]=& _tmp19F;({unsigned _tmp319=f->loc;struct _fat_ptr _tmp318=({const char*_tmp19E="enum field name %s shadows global name";_tag_fat(_tmp19E,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp319,_tmp318,_tag_fat(_tmp19D,sizeof(void*),1U));});});
# 857
if(f->tag == 0){
({struct Cyc_Absyn_Exp*_tmp31A=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp31A;});
++ tag_count;}else{
# 862
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))){
struct _tuple11 _tmp1A0=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag));struct _tuple11 _stmttmp17=_tmp1A0;struct _tuple11 _tmp1A1=_stmttmp17;int _tmp1A3;unsigned _tmp1A2;_LL1: _tmp1A2=_tmp1A1.f1;_tmp1A3=_tmp1A1.f2;_LL2: {unsigned t1=_tmp1A2;int known=_tmp1A3;
if(known)tag_count=t1 + (unsigned)1;}}}}}
# 844
;_pop_region(uprev_rgn);}
# 870
{struct _handler_cons _tmp1A4;_push_handler(& _tmp1A4);{int _tmp1A6=0;if(setjmp(_tmp1A4.handler))_tmp1A6=1;if(!_tmp1A6){
{struct Cyc_Absyn_Enumdecl**_tmp1A7=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);struct Cyc_Absyn_Enumdecl**edp=_tmp1A7;
struct Cyc_Absyn_Enumdecl*_tmp1A8=Cyc_Tcdecl_merge_enumdecl(*edp,ed,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Enumdecl*ed2=_tmp1A8;
if(ed2 == 0){_npop_handler(0U);return;}
*edp=ed2;}
# 871
;_pop_handler();}else{void*_tmp1A5=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A9=_tmp1A5;void*_tmp1AA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1A9)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 876
struct Cyc_Absyn_Enumdecl**_tmp1AB=({struct Cyc_Absyn_Enumdecl**_tmp1AC=_cycalloc(sizeof(*_tmp1AC));*_tmp1AC=ed;_tmp1AC;});struct Cyc_Absyn_Enumdecl**edp=_tmp1AB;
({struct Cyc_Dict_Dict _tmp31B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,edp);(te->ae)->enumdecls=_tmp31B;});
goto _LL3;}}else{_LL6: _tmp1AA=_tmp1A9;_LL7: {void*exn=_tmp1AA;(int)_rethrow(exn);}}_LL3:;}}}
# 882
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1AD=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp1AD;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1B0=({struct Cyc_String_pa_PrintArg_struct _tmp22B;_tmp22B.tag=0U,_tmp22B.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp22B;});struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp22A;_tmp22A.tag=0U,_tmp22A.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp22A;});void*_tmp1AE[2U];_tmp1AE[0]=& _tmp1B0,_tmp1AE[1]=& _tmp1B1;({unsigned _tmp31D=loc;struct _fat_ptr _tmp31C=({const char*_tmp1AF="enum %s, field %s: expression is not constant";_tag_fat(_tmp1AF,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp31D,_tmp31C,_tag_fat(_tmp1AE,sizeof(void*),2U));});});}}}
# 892
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1B2=sc;switch(_tmp1B2){case Cyc_Absyn_Static: _LL1: _LL2:
# 895
 if(!in_include)
({void*_tmp1B3=0U;({unsigned _tmp31F=loc;struct _fat_ptr _tmp31E=({const char*_tmp1B4="static declaration within extern \"C\"";_tag_fat(_tmp1B4,sizeof(char),37U);});Cyc_Tcutil_warn(_tmp31F,_tmp31E,_tag_fat(_tmp1B3,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 899
({void*_tmp1B5=0U;({unsigned _tmp321=loc;struct _fat_ptr _tmp320=({const char*_tmp1B6="abstract declaration within extern \"C\"";_tag_fat(_tmp1B6,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp321,_tmp320,_tag_fat(_tmp1B5,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;case Cyc_Absyn_ExternC: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
# 906
({void*_tmp1B7=0U;({unsigned _tmp323=loc;struct _fat_ptr _tmp322=({const char*_tmp1B8="nested extern \"C\" declaration";_tag_fat(_tmp1B8,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp323,_tmp322,_tag_fat(_tmp1B7,sizeof(void*),0U));});});
return 1;}_LL0:;}
# 916
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 920
struct Cyc_List_List*_tmp1B9=ds0;struct Cyc_List_List*ds=_tmp1B9;for(0;ds != 0;ds=ds->tl){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
unsigned loc=d->loc;
void*_tmp1BA=d->r;void*_stmttmp18=_tmp1BA;void*_tmp1BB=_stmttmp18;struct _tuple10*_tmp1BF;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C3;struct _fat_ptr*_tmp1C2;struct Cyc_Absyn_Enumdecl*_tmp1C4;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Aggrdecl*_tmp1C6;struct Cyc_Absyn_Typedefdecl*_tmp1C7;struct Cyc_Absyn_Fndecl*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1C9;switch(*((int*)_tmp1BB)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
# 926
({void*_tmp1CA=0U;({unsigned _tmp325=loc;struct _fat_ptr _tmp324=({const char*_tmp1CB="top level let-declarations are not implemented";_tag_fat(_tmp1CB,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp325,_tmp324,_tag_fat(_tmp1CA,sizeof(void*),0U));});});goto _LL0;case 4U: _LL5: _LL6:
# 928
({void*_tmp1CC=0U;({unsigned _tmp327=loc;struct _fat_ptr _tmp326=({const char*_tmp1CD="top level region declarations are not implemented";_tag_fat(_tmp1CD,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp327,_tmp326,_tag_fat(_tmp1CC,sizeof(void*),0U));});});goto _LL0;case 0U: _LL7: _tmp1C9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp1C9;
# 930
if(in_externC && Cyc_Tc_okay_externC(d->loc,vd->sc,te->in_extern_c_include))
vd->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,vd,check_var_init,te->in_extern_c_include,exports);
goto _LL0;}case 1U: _LL9: _tmp1C8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_LLA: {struct Cyc_Absyn_Fndecl*fd=_tmp1C8;
# 935
if(in_externC && Cyc_Tc_okay_externC(d->loc,fd->sc,te->in_extern_c_include))
fd->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,fd,exports);
goto _LL0;}case 8U: _LLB: _tmp1C7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp1C7;
# 940
td->extern_c=te->in_extern_c_include;
# 944
Cyc_Tc_tcTypedefdecl(te,loc,td);
goto _LL0;}case 5U: _LLD: _tmp1C6=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1C6;
# 947
if(in_externC && Cyc_Tc_okay_externC(d->loc,ad->sc,te->in_extern_c_include))
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);
goto _LL0;}case 6U: _LLF: _tmp1C5=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_LL10: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1C5;
# 952
if(in_externC && Cyc_Tc_okay_externC(d->loc,tud->sc,te->in_extern_c_include))
tud->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
goto _LL0;}case 7U: _LL11: _tmp1C4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*ed=_tmp1C4;
# 957
if(in_externC && Cyc_Tc_okay_externC(d->loc,ed->sc,te->in_extern_c_include))
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);
goto _LL0;}case 13U: _LL13: _LL14:
({void*_tmp1CE=0U;({unsigned _tmp329=d->loc;struct _fat_ptr _tmp328=({const char*_tmp1CF="spurious __cyclone_port_on__";_tag_fat(_tmp1CF,sizeof(char),29U);});Cyc_Tcutil_warn(_tmp329,_tmp328,_tag_fat(_tmp1CE,sizeof(void*),0U));});});goto _LL0;case 14U: _LL15: _LL16:
 goto _LL0;case 15U: _LL17: _LL18:
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16U: _LL19: _LL1A:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9U: _LL1B: _tmp1C2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_tmp1C3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BB)->f2;_LL1C: {struct _fat_ptr*v=_tmp1C2;struct Cyc_List_List*ds2=_tmp1C3;
# 967
struct Cyc_List_List*_tmp1D0=te->ns;struct Cyc_List_List*ns=_tmp1D0;
({struct Cyc_List_List*_tmp32B=({struct Cyc_List_List*_tmp32A=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp32A,({struct Cyc_List_List*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->hd=v,_tmp1D1->tl=0;_tmp1D1;}));});te->ns=_tmp32B;});
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);
te->ns=ns;
goto _LL0;}case 10U: _LL1D: _tmp1C1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BB)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp1C1;
# 974
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11U: _LL1F: _tmp1C0=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp1C0;
# 977
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default: _LL21: _tmp1BC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f1;_tmp1BD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f2;_tmp1BE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f3;_tmp1BF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BB)->f4;_LL22: {struct Cyc_List_List*ds2=_tmp1BC;struct Cyc_List_List*ovrs=_tmp1BD;struct Cyc_List_List*exports2=_tmp1BE;struct _tuple10*wc=_tmp1BF;
# 982
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(exports2,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1D2=Cyc_Tcenv_enter_extern_c_include(te);struct Cyc_Tcenv_Tenv*te2=_tmp1D2;
Cyc_Tc_tc_decls(te2,ds2,1,check_var_init,& newexs);
# 987
for(0;exports2 != 0;exports2=exports2->tl){
struct _tuple12*_tmp1D3=(struct _tuple12*)exports2->hd;struct _tuple12*exp=_tmp1D3;
if(!(*exp).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1D6=({struct Cyc_String_pa_PrintArg_struct _tmp22C;_tmp22C.tag=0U,({
struct _fat_ptr _tmp32C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string((*exp).f2));_tmp22C.f1=_tmp32C;});_tmp22C;});void*_tmp1D4[1U];_tmp1D4[0]=& _tmp1D6;({unsigned _tmp32E=(*exp).f1;struct _fat_ptr _tmp32D=({const char*_tmp1D5="%s is exported but not defined";_tag_fat(_tmp1D5,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp32E,_tmp32D,_tag_fat(_tmp1D4,sizeof(void*),1U));});});}
# 993
goto _LL0;}}_LL0:;}}
# 998
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict _tmp1D7=Cyc_Callgraph_compute_callgraph(ds);struct Cyc_Dict_Dict cg=_tmp1D7;
# 1003
struct Cyc_Dict_Dict _tmp1D8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict g))Cyc_Graph_scc)(cg);struct Cyc_Dict_Dict scc=_tmp1D8;
# 1006
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1024 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1026
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1030
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){
if((int)vd->sc == (int)Cyc_Absyn_Extern ||(int)vd->sc == (int)Cyc_Absyn_ExternC)
return 1;{
void*_tmp1D9=Cyc_Tcutil_compress(vd->type);void*_stmttmp19=_tmp1D9;void*_tmp1DA=_stmttmp19;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1039
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1DB=d->r;void*_stmttmp1A=_tmp1DB;void*_tmp1DC=_stmttmp1A;struct Cyc_List_List**_tmp1DD;struct Cyc_List_List**_tmp1DE;struct Cyc_List_List**_tmp1DF;struct Cyc_List_List**_tmp1E0;struct Cyc_Absyn_Vardecl*_tmp1E1;switch(*((int*)_tmp1DC)){case 0U: _LL1: _tmp1E1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1DC)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1E1;
# 1043
if((env->in_cinclude || !Cyc_Tc_is_extern(vd))|| !({
struct _tuple0*_tmp32F=vd->name;Cyc_Absyn_qvar_cmp(_tmp32F,Cyc_Absyn_uniquergn_qvar());}))
return 1;
return(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,vd->name)).f2;}case 11U: _LL3: _tmp1E0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1DC)->f1;_LL4: {struct Cyc_List_List**ds2p=_tmp1E0;
_tmp1DF=ds2p;goto _LL6;}case 10U: _LL5: _tmp1DF=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1DC)->f2;_LL6: {struct Cyc_List_List**ds2p=_tmp1DF;
_tmp1DE=ds2p;goto _LL8;}case 9U: _LL7: _tmp1DE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1DC)->f2;_LL8: {struct Cyc_List_List**ds2p=_tmp1DE;
({struct Cyc_List_List*_tmp330=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp330;});return 1;}case 12U: _LL9: _tmp1DD=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1DC)->f1;_LLA: {struct Cyc_List_List**ds2p=_tmp1DD;
# 1051
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp331=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp331;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 1060
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1E2=({struct Cyc_Tc_TreeshakeEnv _tmp22D;_tmp22D.in_cinclude=0,_tmp22D.ordinaries=(te->ae)->ordinaries;_tmp22D;});struct Cyc_Tc_TreeshakeEnv env=_tmp1E2;
return Cyc_Tc_treeshake_f(& env,ds);}

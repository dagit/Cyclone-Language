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
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 890
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 896
void*Cyc_Absyn_compress_kb(void*);
# 900
void*Cyc_Absyn_compress(void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 954
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 975
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 983
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1059
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 43 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_EvalOrder =11U,Cyc_Flags_CCompiler =12U,Cyc_Flags_AllPasses =13U};
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_RgnOrder_RgnPO;
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 46
int Cyc_Tcutil_is_array_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 112
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 128
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 132
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 167
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 196
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 206
void Cyc_Tcutil_check_bitfield(unsigned,void*field_typ,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn);
# 209
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 240
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 255
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 260
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 264
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 281
int Cyc_Tcutil_zeroable_type(void*);
# 288
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 35 "attributes.h"
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 43
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int is_defn);
void Cyc_Atts_check_variable_atts(unsigned,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*);
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*fn,struct Cyc_List_List*);
# 47
void Cyc_Atts_fnTypeAttsOK(unsigned,void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83 "dict.h"
extern int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned,struct Cyc_Absyn_Fndecl*);
# 79
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple0*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 153
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv_dicts(struct Cyc_Tcenv_Tenv*);
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
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
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
extern struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict g);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);
# 36 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);
# 36 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs);struct Cyc_Hashtable_Table;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47 "toc.h"
void Cyc_Toc_init (void);
void Cyc_Toc_finish (void);static char _tmp0[1U]="";
# 45 "tc.cyc"
static struct _fat_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1U};
static struct _fat_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned f1;struct _tuple0*f2;int f3;};
# 48
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;struct _tuple12*p=_tmp1;
if(!Cyc_Absyn_qvar_cmp(x,(*p).f2)== 0)
continue;
# 55
(*p).f3=1;
return 1;}
# 58
return 0;}struct _tuple13{void*f1;int f2;};
# 61
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 63
struct Cyc_Absyn_Vardecl*_tmp2=vd;int*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_Absyn_Exp*_tmp6;void*_tmp5;struct _tuple0*_tmp4;enum Cyc_Absyn_Scope _tmp3;_LL1: _tmp3=_tmp2->sc;_tmp4=_tmp2->name;_tmp5=_tmp2->type;_tmp6=_tmp2->initializer;_tmp7=_tmp2->attributes;_tmp8=(int*)& _tmp2->is_proto;_LL2: {enum Cyc_Absyn_Scope sc=_tmp3;struct _tuple0*q=_tmp4;void*t=_tmp5;struct Cyc_Absyn_Exp*initializer=_tmp6;struct Cyc_List_List*atts=_tmp7;int*is_proto=_tmp8;
# 70
{void*_tmp9=Cyc_Absyn_compress(t);void*_stmttmp0=_tmp9;void*_tmpA=_stmttmp0;unsigned _tmpE;void*_tmpD;struct Cyc_Absyn_Tqual _tmpC;void*_tmpB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->tag == 4U){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).num_elts == 0){_LL4: _tmpB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).elt_type;_tmpC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).tq;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).zero_term;_tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).zt_loc;if(initializer != 0){_LL5: {void*telt=_tmpB;struct Cyc_Absyn_Tqual tq=_tmpC;void*zt=_tmpD;unsigned ztl=_tmpE;
# 72
{void*_tmpF=initializer->r;void*_stmttmp1=_tmpF;void*_tmp10=_stmttmp1;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp14;struct _fat_ptr _tmp15;struct _fat_ptr _tmp16;switch(*((int*)_tmp10)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Wstring_c).tag){case 8U: _LL9: _tmp16=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).String_c).val;_LLA: {struct _fat_ptr s=_tmp16;
# 74
t=({void*_tmp24A=({void*_tmp249=telt;struct Cyc_Absyn_Tqual _tmp248=tq;struct Cyc_Absyn_Exp*_tmp247=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_tmp246=zt;Cyc_Absyn_array_type(_tmp249,_tmp248,_tmp247,_tmp246,ztl);});vd->type=_tmp24A;});goto _LL8;}case 9U: _LLB: _tmp15=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Wstring_c).val;_LLC: {struct _fat_ptr s=_tmp15;
# 76
t=({void*_tmp24F=({void*_tmp24E=telt;struct Cyc_Absyn_Tqual _tmp24D=tq;struct Cyc_Absyn_Exp*_tmp24C=Cyc_Absyn_uint_exp(1U,0U);void*_tmp24B=zt;Cyc_Absyn_array_type(_tmp24E,_tmp24D,_tmp24C,_tmp24B,ztl);});vd->type=_tmp24F;});goto _LL8;}default: goto _LL15;}case 27U: _LLD: _tmp14=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp14;
_tmp13=e;goto _LL10;}case 28U: _LLF: _tmp13=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp13;
# 79
t=({void*_tmp250=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_tmp250;});goto _LL8;}case 36U: _LL11: _tmp12=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10)->f2;_LL12: {struct Cyc_List_List*es=_tmp12;
_tmp11=es;goto _LL14;}case 26U: _LL13: _tmp11=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10)->f1;_LL14: {struct Cyc_List_List*es=_tmp11;
# 82
t=({void*_tmp255=({void*_tmp254=telt;struct Cyc_Absyn_Tqual _tmp253=tq;struct Cyc_Absyn_Exp*_tmp252=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);void*_tmp251=zt;Cyc_Absyn_array_type(_tmp254,_tmp253,_tmp252,_tmp251,ztl);});vd->type=_tmp255;});
goto _LL8;}default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 86
goto _LL3;}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 90
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
# 92
({int _tmp256=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp256;});
# 95
({int _tmp257=!Cyc_Tcutil_is_array_type(t);vd->escapes=_tmp257;});
# 97
if(Cyc_Tcutil_is_function_type(t)){
*is_proto=0;
atts=Cyc_Atts_transfer_fn_type_atts(t,atts);
Cyc_Atts_fnTypeAttsOK(loc,t);}
# 103
if((int)sc == (int)3U ||(int)sc == (int)4U){
if(initializer != 0 && !in_cinclude)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DC;_tmp1DC.tag=0U,({struct _fat_ptr _tmp258=({const char*_tmp19="extern declaration should not have initializer";_tag_fat(_tmp19,sizeof(char),47U);});_tmp1DC.f1=_tmp258;});_tmp1DC;});void*_tmp17[1U];_tmp17[0]=& _tmp18;({unsigned _tmp259=loc;Cyc_Warn_err2(_tmp259,_tag_fat(_tmp17,sizeof(void*),1U));});});}else{
if(!Cyc_Tcutil_is_function_type(t)){
# 110
Cyc_Atts_check_variable_atts(loc,vd,atts);
if(initializer == 0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E0;_tmp1E0.tag=0U,({struct _fat_ptr _tmp25A=({const char*_tmp20="initializer required for variable ";_tag_fat(_tmp20,sizeof(char),35U);});_tmp1E0.f1=_tmp25A;});_tmp1E0;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp1DF;_tmp1DF.tag=11U,_tmp1DF.f1=vd;_tmp1DF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DE;_tmp1DE.tag=0U,({struct _fat_ptr _tmp25B=({const char*_tmp1F=" of type ";_tag_fat(_tmp1F,sizeof(char),10U);});_tmp1DE.f1=_tmp25B;});_tmp1DE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1DD;_tmp1DD.tag=2U,_tmp1DD.f1=(void*)t;_tmp1DD;});void*_tmp1A[4U];_tmp1A[0]=& _tmp1B,_tmp1A[1]=& _tmp1C,_tmp1A[2]=& _tmp1D,_tmp1A[3]=& _tmp1E;({unsigned _tmp25C=loc;Cyc_Warn_err2(_tmp25C,_tag_fat(_tmp1A,sizeof(void*),4U));});});}else{
# 118
struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){
{void*_tmp24=Cyc_Tcexp_tcExpInitializer(te,& t,initializer);void*t2=_tmp24;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp25E=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp25D=initializer;Cyc_Tcutil_coerce_assign(_tmp25E,_tmp25D,t);})){
struct _fat_ptr _tmp25=Cyc_Absynpp_qvar2string(vd->name);struct _fat_ptr s0=_tmp25;
const char*_tmp26=" declared with type ";const char*s1=_tmp26;
struct _fat_ptr _tmp27=Cyc_Absynpp_typ2string(t);struct _fat_ptr s2=_tmp27;
const char*_tmp28=" but initialized with type ";const char*s3=_tmp28;
struct _fat_ptr _tmp29=Cyc_Absynpp_typ2string(t2);struct _fat_ptr s4=_tmp29;
if(({unsigned long _tmp262=({unsigned long _tmp261=({unsigned long _tmp260=({unsigned long _tmp25F=Cyc_strlen((struct _fat_ptr)s0);_tmp25F + Cyc_strlen(({const char*_tmp2A=s1;_tag_fat(_tmp2A,sizeof(char),21U);}));});_tmp260 + Cyc_strlen((struct _fat_ptr)s2);});_tmp261 + Cyc_strlen(({const char*_tmp2B=s3;_tag_fat(_tmp2B,sizeof(char),28U);}));});_tmp262 + Cyc_strlen((struct _fat_ptr)s4);})> (unsigned long)70)
({struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0U,_tmp1E5.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp1E5;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp1E4;_tmp1E4.tag=0U,({struct _fat_ptr _tmp263=(struct _fat_ptr)({const char*_tmp34=s1;_tag_fat(_tmp34,sizeof(char),21U);});_tmp1E4.f1=_tmp263;});_tmp1E4;});struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp1E3;_tmp1E3.tag=0U,_tmp1E3.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1E3;});struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp1E2;_tmp1E2.tag=0U,({struct _fat_ptr _tmp264=(struct _fat_ptr)({const char*_tmp33=s3;_tag_fat(_tmp33,sizeof(char),28U);});_tmp1E2.f1=_tmp264;});_tmp1E2;});struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0U,_tmp1E1.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp1E1;});void*_tmp2C[5U];_tmp2C[0]=& _tmp2E,_tmp2C[1]=& _tmp2F,_tmp2C[2]=& _tmp30,_tmp2C[3]=& _tmp31,_tmp2C[4]=& _tmp32;({unsigned _tmp266=loc;struct _fat_ptr _tmp265=({const char*_tmp2D="%s%s\n\t%s\n%s\n\t%s";_tag_fat(_tmp2D,sizeof(char),16U);});Cyc_Warn_err(_tmp266,_tmp265,_tag_fat(_tmp2C,sizeof(void*),5U));});});else{
# 129
({struct Cyc_String_pa_PrintArg_struct _tmp37=({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0U,_tmp1EA.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp1EA;});struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0U,({struct _fat_ptr _tmp267=(struct _fat_ptr)({const char*_tmp3D=s1;_tag_fat(_tmp3D,sizeof(char),21U);});_tmp1E9.f1=_tmp267;});_tmp1E9;});struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp1E8;_tmp1E8.tag=0U,_tmp1E8.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1E8;});struct Cyc_String_pa_PrintArg_struct _tmp3A=({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0U,({struct _fat_ptr _tmp268=(struct _fat_ptr)({const char*_tmp3C=s3;_tag_fat(_tmp3C,sizeof(char),28U);});_tmp1E7.f1=_tmp268;});_tmp1E7;});struct Cyc_String_pa_PrintArg_struct _tmp3B=({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0U,_tmp1E6.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp1E6;});void*_tmp35[5U];_tmp35[0]=& _tmp37,_tmp35[1]=& _tmp38,_tmp35[2]=& _tmp39,_tmp35[3]=& _tmp3A,_tmp35[4]=& _tmp3B;({unsigned _tmp26A=loc;struct _fat_ptr _tmp269=({const char*_tmp36="%s%s%s%s%s";_tag_fat(_tmp36,sizeof(char),11U);});Cyc_Warn_err(_tmp26A,_tmp269,_tag_fat(_tmp35,sizeof(void*),5U));});});}
Cyc_Unify_explain_failure();}
# 133
if(!Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EB;_tmp1EB.tag=0U,({struct _fat_ptr _tmp26B=({const char*_tmp40="initializer is not a constant expression";_tag_fat(_tmp40,sizeof(char),41U);});_tmp1EB.f1=_tmp26B;});_tmp1EB;});void*_tmp3E[1U];_tmp3E[0]=& _tmp3F;({unsigned _tmp26C=loc;Cyc_Warn_err2(_tmp26C,_tag_fat(_tmp3E,sizeof(void*),1U));});});}
# 119
;_pop_handler();}else{void*_tmp22=(void*)Cyc_Core_get_exn_thrown();void*_tmp41=_tmp22;void*_tmp42;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp41)->tag == Cyc_Tcenv_Env_error){_LL18: _LL19:
# 136
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EC;_tmp1EC.tag=0U,({struct _fat_ptr _tmp26D=({const char*_tmp45="initializer is not a constant expression";_tag_fat(_tmp45,sizeof(char),41U);});_tmp1EC.f1=_tmp26D;});_tmp1EC;});void*_tmp43[1U];_tmp43[0]=& _tmp44;({unsigned _tmp26E=loc;Cyc_Warn_err2(_tmp26E,_tag_fat(_tmp43,sizeof(void*),1U));});});goto _LL17;}else{_LL1A: _tmp42=_tmp41;_LL1B: {void*exn=_tmp42;(int)_rethrow(exn);}}_LL17:;}}}}else{
# 140
Cyc_Atts_check_fndecl_atts(loc,atts,0);}}
# 143
{struct _handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler))_tmp48=1;if(!_tmp48){
{struct _tuple13*_tmp49=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple13*ans=_tmp49;
void*_tmp4A=(*ans).f1;void*b0=_tmp4A;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4B=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=1U,_tmp4E->f1=vd;_tmp4E;});struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1=_tmp4B;
void*_tmp4C=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmp4C;
if(b == 0){_npop_handler(0U);return;}
# 150
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports)){
if(b != b0 ||(*ans).f2)
# 153
({struct Cyc_Dict_Dict _tmp271=({struct Cyc_Dict_Dict _tmp270=(te->ae)->ordinaries;struct _tuple0*_tmp26F=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp270,_tmp26F,({struct _tuple13*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D->f1=b,_tmp4D->f2=(*ans).f2;_tmp4D;}));});
# 153
(te->ae)->ordinaries=_tmp271;});}
# 155
_npop_handler(0U);return;}
# 144
;_pop_handler();}else{void*_tmp47=(void*)Cyc_Core_get_exn_thrown();void*_tmp4F=_tmp47;void*_tmp50;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4F)->tag == Cyc_Dict_Absent){_LL1D: _LL1E:
# 156
 goto _LL1C;}else{_LL1F: _tmp50=_tmp4F;_LL20: {void*exn=_tmp50;(int)_rethrow(exn);}}_LL1C:;}}}
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp275=({struct Cyc_Dict_Dict _tmp274=(te->ae)->ordinaries;struct _tuple0*_tmp273=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp274,_tmp273,({struct _tuple13*_tmp52=_cycalloc(sizeof(*_tmp52));
({void*_tmp272=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=1U,_tmp51->f1=vd;_tmp51;});_tmp52->f1=_tmp272;}),_tmp52->f2=0;_tmp52;}));});
# 158
(te->ae)->ordinaries=_tmp275;});}}
# 162
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp53=n;struct _fat_ptr*_tmp55;union Cyc_Absyn_Nmspace _tmp54;_LL1: _tmp54=_tmp53->f1;_tmp55=_tmp53->f2;_LL2: {union Cyc_Absyn_Nmspace nms=_tmp54;struct _fat_ptr*v=_tmp55;
union Cyc_Absyn_Nmspace _tmp56=nms;if((_tmp56.Abs_n).tag == 2){if((_tmp56.Abs_n).val == 0){_LL4: _LL5:
 return({struct _fat_ptr _tmp276=(struct _fat_ptr)*v;Cyc_strcmp(_tmp276,({const char*_tmp57="main";_tag_fat(_tmp57,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;}}
# 170
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 172
struct _tuple0*q=fd->name;
# 176
if((int)fd->sc == (int)Cyc_Absyn_ExternC && !te->in_extern_c_include)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp59=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1ED;_tmp1ED.tag=0U,({struct _fat_ptr _tmp277=({const char*_tmp5A="extern \"C\" functions cannot be implemented in Cyclone";_tag_fat(_tmp5A,sizeof(char),54U);});_tmp1ED.f1=_tmp277;});_tmp1ED;});void*_tmp58[1U];_tmp58[0]=& _tmp59;({unsigned _tmp278=loc;Cyc_Warn_err2(_tmp278,_tag_fat(_tmp58,sizeof(void*),1U));});});
# 180
Cyc_Atts_check_fndecl_atts(loc,(fd->i).attributes,1);
# 183
if(te->in_extern_c_inc_repeat)
fd->cached_type=0;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 188
({struct Cyc_List_List*_tmp279=Cyc_Atts_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp279;});
Cyc_Atts_fnTypeAttsOK(loc,t);
# 192
{struct _handler_cons _tmp5B;_push_handler(& _tmp5B);{int _tmp5D=0;if(setjmp(_tmp5B.handler))_tmp5D=1;if(!_tmp5D){
{struct _tuple13*_tmp5E=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple13*ans=_tmp5E;
void*_tmp5F=(*ans).f1;void*b0=_tmp5F;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp60=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=2U,_tmp63->f1=fd;_tmp63;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1=_tmp60;
void*_tmp61=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmp61;
if(b != 0){
# 199
if(exports == 0 || Cyc_Tc_export_member(q,*exports)){
if(!(b == b0 &&(*ans).f2))
({struct Cyc_Dict_Dict _tmp27C=({struct Cyc_Dict_Dict _tmp27B=(te->ae)->ordinaries;struct _tuple0*_tmp27A=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp27B,_tmp27A,({struct _tuple13*_tmp62=_cycalloc(sizeof(*_tmp62));
_tmp62->f1=b,_tmp62->f2=(*ans).f2;_tmp62;}));});
# 201
(te->ae)->ordinaries=_tmp27C;});}}}
# 193
;_pop_handler();}else{void*_tmp5C=(void*)Cyc_Core_get_exn_thrown();void*_tmp64=_tmp5C;void*_tmp65;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp64)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 204
 if(exports == 0 || Cyc_Tc_export_member(fd->name,*exports))
({struct Cyc_Dict_Dict _tmp280=({struct Cyc_Dict_Dict _tmp27F=(te->ae)->ordinaries;struct _tuple0*_tmp27E=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp27F,_tmp27E,({struct _tuple13*_tmp67=_cycalloc(sizeof(*_tmp67));
({void*_tmp27D=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=2U,_tmp66->f1=fd;_tmp66;});_tmp67->f1=_tmp27D;}),_tmp67->f2=0;_tmp67;}));});
# 205
(te->ae)->ordinaries=_tmp280;});
# 207
goto _LL0;}else{_LL3: _tmp65=_tmp64;_LL4: {void*exn=_tmp65;(int)_rethrow(exn);}}_LL0:;}}}
# 211
if(te->in_extern_c_include)return;{
# 216
struct Cyc_Tcenv_Fenv*_tmp68=Cyc_Tcenv_new_fenv(loc,fd);struct Cyc_Tcenv_Fenv*fenv=_tmp68;
struct Cyc_Tcenv_Tenv*_tmp69=({struct Cyc_Tcenv_Tenv*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->ns=te->ns,_tmp99->ae=te->ae,_tmp99->le=fenv,_tmp99->allow_valueof=0,_tmp99->in_extern_c_include=0,_tmp99->in_tempest=te->in_tempest,_tmp99->tempest_generalize=te->tempest_generalize,_tmp99->in_extern_c_inc_repeat=0;_tmp99;});struct Cyc_Tcenv_Tenv*te=_tmp69;
# 219
Cyc_Tcstmt_tcStmt(te,fd->body,0);
# 222
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
# 225
if(te->in_tempest){
te->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);
te->tempest_generalize=0;}
# 231
if(Cyc_Tc_is_main(q)){
# 233
{void*_tmp6A=Cyc_Absyn_compress((fd->i).ret_type);void*_stmttmp2=_tmp6A;void*_tmp6B=_stmttmp2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B)->f1)){case 0U: _LL6: _LL7:
# 235
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EE;_tmp1EE.tag=0U,({struct _fat_ptr _tmp281=({const char*_tmp6E="main declared with return type void";_tag_fat(_tmp6E,sizeof(char),36U);});_tmp1EE.f1=_tmp281;});_tmp1EE;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6D;({unsigned _tmp282=loc;Cyc_Warn_warn2(_tmp282,_tag_fat(_tmp6C,sizeof(void*),1U));});});goto _LL5;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6B)->f1)->f2){case Cyc_Absyn_Int_sz: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Long_sz: _LLA: _LLB:
 goto _LL5;default: goto _LLC;}default: goto _LLC;}else{_LLC: _LLD:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F1;_tmp1F1.tag=0U,({struct _fat_ptr _tmp283=({const char*_tmp74="main declared with return type ";_tag_fat(_tmp74,sizeof(char),32U);});_tmp1F1.f1=_tmp283;});_tmp1F1;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp71=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F0;_tmp1F0.tag=2U,_tmp1F0.f1=(void*)(fd->i).ret_type;_tmp1F0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp72=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EF;_tmp1EF.tag=0U,({
struct _fat_ptr _tmp284=({const char*_tmp73=" instead of int or void";_tag_fat(_tmp73,sizeof(char),24U);});_tmp1EF.f1=_tmp284;});_tmp1EF;});void*_tmp6F[3U];_tmp6F[0]=& _tmp70,_tmp6F[1]=& _tmp71,_tmp6F[2]=& _tmp72;({unsigned _tmp285=loc;Cyc_Warn_err2(_tmp285,_tag_fat(_tmp6F,sizeof(void*),3U));});});}_LL5:;}
# 241
if((fd->i).c_varargs ||(fd->i).cyc_varargs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp76=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F2;_tmp1F2.tag=0U,({struct _fat_ptr _tmp286=({const char*_tmp77="main declared with varargs";_tag_fat(_tmp77,sizeof(char),27U);});_tmp1F2.f1=_tmp286;});_tmp1F2;});void*_tmp75[1U];_tmp75[0]=& _tmp76;({unsigned _tmp287=loc;Cyc_Warn_err2(_tmp287,_tag_fat(_tmp75,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp78=(fd->i).args;struct Cyc_List_List*args=_tmp78;
if(args != 0){
struct _tuple8*_tmp79=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp3=_tmp79;struct _tuple8*_tmp7A=_stmttmp3;void*_tmp7B;_LLF: _tmp7B=_tmp7A->f3;_LL10: {void*t1=_tmp7B;
{void*_tmp7C=Cyc_Absyn_compress(t1);void*_stmttmp4=_tmp7C;void*_tmp7D=_stmttmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7D)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7D)->f1)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp7D)->f1)->f2){case Cyc_Absyn_Int_sz: _LL12: _LL13:
 goto _LL15;case Cyc_Absyn_Long_sz: _LL14: _LL15:
 goto _LL11;default: goto _LL16;}else{goto _LL16;}}else{_LL16: _LL17:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F5;_tmp1F5.tag=0U,({struct _fat_ptr _tmp288=({const char*_tmp83="main declared with first argument of type ";_tag_fat(_tmp83,sizeof(char),43U);});_tmp1F5.f1=_tmp288;});_tmp1F5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp80=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F4;_tmp1F4.tag=2U,_tmp1F4.f1=(void*)t1;_tmp1F4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp81=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F3;_tmp1F3.tag=0U,({
struct _fat_ptr _tmp289=({const char*_tmp82=" instead of int";_tag_fat(_tmp82,sizeof(char),16U);});_tmp1F3.f1=_tmp289;});_tmp1F3;});void*_tmp7E[3U];_tmp7E[0]=& _tmp7F,_tmp7E[1]=& _tmp80,_tmp7E[2]=& _tmp81;({unsigned _tmp28A=loc;Cyc_Warn_err2(_tmp28A,_tag_fat(_tmp7E,sizeof(void*),3U));});});}_LL11:;}
# 252
args=args->tl;
if(args != 0){
struct _tuple8*_tmp84=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp5=_tmp84;struct _tuple8*_tmp85=_stmttmp5;void*_tmp86;_LL19: _tmp86=_tmp85->f3;_LL1A: {void*t2=_tmp86;
args=args->tl;
if(args != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F6;_tmp1F6.tag=0U,({struct _fat_ptr _tmp28B=({const char*_tmp89="main declared with too many arguments";_tag_fat(_tmp89,sizeof(char),38U);});_tmp1F6.f1=_tmp28B;});_tmp1F6;});void*_tmp87[1U];_tmp87[0]=& _tmp88;({unsigned _tmp28C=loc;Cyc_Warn_err2(_tmp28C,_tag_fat(_tmp87,sizeof(void*),1U));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->v=(fd->i).tvars;_tmp98;});
if(((!({void*_tmp2A4=t2;Cyc_Unify_unify(_tmp2A4,({void*_tmp2A3=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp29F=({struct Cyc_Core_Opt*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->v=& Cyc_Tcutil_rk;_tmp8A;});Cyc_Absyn_new_evar(_tmp29F,tvs);}));void*_tmp2A2=({
struct Cyc_Core_Opt*_tmp2A0=({struct Cyc_Core_Opt*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->v=& Cyc_Tcutil_rk;_tmp8B;});Cyc_Absyn_new_evar(_tmp2A0,tvs);});
# 259
struct Cyc_Absyn_Tqual _tmp2A1=
# 261
Cyc_Absyn_empty_tqual(0U);
# 259
Cyc_Absyn_fatptr_type(_tmp2A3,_tmp2A2,_tmp2A1,
# 261
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));})&& !({
void*_tmp29E=t2;Cyc_Unify_unify(_tmp29E,({void*_tmp29D=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp299=({struct Cyc_Core_Opt*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->v=& Cyc_Tcutil_rk;_tmp8C;});Cyc_Absyn_new_evar(_tmp299,tvs);}));void*_tmp29C=({
# 264
struct Cyc_Core_Opt*_tmp29A=({struct Cyc_Core_Opt*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->v=& Cyc_Tcutil_rk;_tmp8D;});Cyc_Absyn_new_evar(_tmp29A,tvs);});
# 262
struct Cyc_Absyn_Tqual _tmp29B=
# 265
Cyc_Absyn_empty_tqual(0U);
# 262
Cyc_Absyn_fatptr_type(_tmp29D,_tmp29C,_tmp29B,
# 265
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp298=t2;Cyc_Unify_unify(_tmp298,({void*_tmp297=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp293=({struct Cyc_Core_Opt*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->v=& Cyc_Tcutil_rk;_tmp8E;});Cyc_Absyn_new_evar(_tmp293,tvs);}));void*_tmp296=({
struct Cyc_Core_Opt*_tmp294=({struct Cyc_Core_Opt*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->v=& Cyc_Tcutil_rk;_tmp8F;});Cyc_Absyn_new_evar(_tmp294,tvs);});
# 266
struct Cyc_Absyn_Tqual _tmp295=
# 268
Cyc_Absyn_const_tqual(0U);
# 266
Cyc_Absyn_fatptr_type(_tmp297,_tmp296,_tmp295,
# 268
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp292=t2;Cyc_Unify_unify(_tmp292,({void*_tmp291=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp28D=({struct Cyc_Core_Opt*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->v=& Cyc_Tcutil_rk;_tmp90;});Cyc_Absyn_new_evar(_tmp28D,tvs);}));void*_tmp290=({
# 271
struct Cyc_Core_Opt*_tmp28E=({struct Cyc_Core_Opt*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->v=& Cyc_Tcutil_rk;_tmp91;});Cyc_Absyn_new_evar(_tmp28E,tvs);});
# 269
struct Cyc_Absyn_Tqual _tmp28F=
# 272
Cyc_Absyn_const_tqual(0U);
# 269
Cyc_Absyn_fatptr_type(_tmp291,_tmp290,_tmp28F,
# 272
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp93=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F9;_tmp1F9.tag=0U,({struct _fat_ptr _tmp2A5=({const char*_tmp97="second argument of main has type ";_tag_fat(_tmp97,sizeof(char),34U);});_tmp1F9.f1=_tmp2A5;});_tmp1F9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp94=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F8;_tmp1F8.tag=2U,_tmp1F8.f1=(void*)t2;_tmp1F8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp95=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F7;_tmp1F7.tag=0U,({
struct _fat_ptr _tmp2A6=({const char*_tmp96=" instead of char??";_tag_fat(_tmp96,sizeof(char),19U);});_tmp1F7.f1=_tmp2A6;});_tmp1F7;});void*_tmp92[3U];_tmp92[0]=& _tmp93,_tmp92[1]=& _tmp94,_tmp92[2]=& _tmp95;({unsigned _tmp2A7=loc;Cyc_Warn_err2(_tmp2A7,_tag_fat(_tmp92,sizeof(void*),3U));});});}}}}}}}}}}
# 281
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 287
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FB;_tmp1FB.tag=0U,({struct _fat_ptr _tmp2A8=({const char*_tmp9D="redeclaration of typedef ";_tag_fat(_tmp9D,sizeof(char),26U);});_tmp1FB.f1=_tmp2A8;});_tmp1FB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp9C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1FA;_tmp1FA.tag=1U,_tmp1FA.f1=q;_tmp1FA;});void*_tmp9A[2U];_tmp9A[0]=& _tmp9B,_tmp9A[1]=& _tmp9C;({unsigned _tmp2A9=loc;Cyc_Warn_err2(_tmp2A9,_tag_fat(_tmp9A,sizeof(void*),2U));});});
return;}
# 292
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tctyp_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp2AA=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 296
(td->tq).real_const=_tmp2AA;});}
# 301
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp9E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_stmttmp6=_tmp9E;void*_tmp9F=_stmttmp6;struct Cyc_Absyn_Kind*_tmpA1;struct Cyc_Core_Opt**_tmpA0;struct Cyc_Core_Opt**_tmpA2;switch(*((int*)_tmp9F)){case 1U: _LL1: _tmpA2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9F)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpA2;
# 304
if(td->defn != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FE;_tmp1FE.tag=0U,({struct _fat_ptr _tmp2AB=({const char*_tmpA8="type variable ";_tag_fat(_tmpA8,sizeof(char),15U);});_tmp1FE.f1=_tmp2AB;});_tmp1FE;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpA5=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp1FD;_tmp1FD.tag=7U,_tmp1FD.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp1FD;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FC;_tmp1FC.tag=0U,({struct _fat_ptr _tmp2AC=({const char*_tmpA7=" is not used in typedef";_tag_fat(_tmpA7,sizeof(char),24U);});_tmp1FC.f1=_tmp2AC;});_tmp1FC;});void*_tmpA3[3U];_tmpA3[0]=& _tmpA4,_tmpA3[1]=& _tmpA5,_tmpA3[2]=& _tmpA6;({unsigned _tmp2AD=loc;Cyc_Warn_warn2(_tmp2AD,_tag_fat(_tmpA3,sizeof(void*),3U));});});
({struct Cyc_Core_Opt*_tmp2AF=({struct Cyc_Core_Opt*_tmpA9=_cycalloc(sizeof(*_tmpA9));({void*_tmp2AE=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpA9->v=_tmp2AE;});_tmpA9;});*f=_tmp2AF;});goto _LL0;}case 2U: _LL3: _tmpA0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9F)->f1;_tmpA1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9F)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmpA0;struct Cyc_Absyn_Kind*k=_tmpA1;
# 313
({struct Cyc_Core_Opt*_tmp2B1=({struct Cyc_Core_Opt*_tmpAA=_cycalloc(sizeof(*_tmpAA));({void*_tmp2B0=Cyc_Tcutil_kind_to_bound(k);_tmpAA->v=_tmp2B0;});_tmpAA;});*f=_tmp2B1;});
goto _LL0;}default: _LL5: _LL6:
 continue;}_LL0:;}}
# 318
({struct Cyc_Dict_Dict _tmp2B2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp2B2;});}struct _tuple14{void*f1;void*f2;};
# 321
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 328
struct _RegionHandle _tmpAB=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpAB;_push_region(uprev_rgn);
# 330
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple14*_tmpAC=(struct _tuple14*)rpo->hd;struct _tuple14*_stmttmp7=_tmpAC;struct _tuple14*_tmpAD=_stmttmp7;void*_tmpAF;void*_tmpAE;_LL1: _tmpAE=_tmpAD->f1;_tmpAF=_tmpAD->f2;_LL2: {void*e=_tmpAE;void*r=_tmpAF;
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,e);
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,r);}}{
# 336
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 339
struct Cyc_List_List*_tmpB0=fields;struct Cyc_List_List*fs=_tmpB0;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpB1=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp8=_tmpB1;struct Cyc_Absyn_Aggrfield*_tmpB2=_stmttmp8;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_List_List*_tmpB7;struct Cyc_Absyn_Exp*_tmpB6;void*_tmpB5;struct Cyc_Absyn_Tqual _tmpB4;struct _fat_ptr*_tmpB3;_LL4: _tmpB3=_tmpB2->name;_tmpB4=_tmpB2->tq;_tmpB5=_tmpB2->type;_tmpB6=_tmpB2->width;_tmpB7=_tmpB2->attributes;_tmpB8=_tmpB2->requires_clause;_LL5: {struct _fat_ptr*fn=_tmpB3;struct Cyc_Absyn_Tqual tq=_tmpB4;void*t=_tmpB5;struct Cyc_Absyn_Exp*width=_tmpB6;struct Cyc_List_List*atts=_tmpB7;struct Cyc_Absyn_Exp*requires_clause=_tmpB8;
# 342
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp200;_tmp200.tag=0U,({struct _fat_ptr _tmp2B3=({const char*_tmpBC="duplicate member ";_tag_fat(_tmpBC,sizeof(char),18U);});_tmp200.f1=_tmp2B3;});_tmp200;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FF;_tmp1FF.tag=0U,_tmp1FF.f1=*fn;_tmp1FF;});void*_tmpB9[2U];_tmpB9[0]=& _tmpBA,_tmpB9[1]=& _tmpBB;({unsigned _tmp2B4=loc;Cyc_Warn_err2(_tmp2B4,_tag_fat(_tmpB9,sizeof(void*),2U));});});
# 346
if(({struct _fat_ptr _tmp2B5=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp2B5,({const char*_tmpBD="";_tag_fat(_tmpBD,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmpBE=_region_malloc(uprev_rgn,sizeof(*_tmpBE));_tmpBE->hd=fn,_tmpBE->tl=prev_fields;_tmpBE;});{
# 349
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 353
if((int)str_or_union == (int)1U ||
 fs->tl == 0 &&(int)str_or_union == (int)0U)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tctyp_check_type(loc,te,tvs,field_kind,0,0,t);
# 358
({int _tmp2B6=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).print_const,t);(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).real_const=_tmp2B6;});
# 361
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
# 363
if((unsigned)requires_clause){
if((int)str_or_union != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp201;_tmp201.tag=0U,({struct _fat_ptr _tmp2B7=({const char*_tmpC1="@requires clauses are allowed only on union members";_tag_fat(_tmpC1,sizeof(char),52U);});_tmp201.f1=_tmp2B7;});_tmp201;});void*_tmpBF[1U];_tmpBF[0]=& _tmpC0;({unsigned _tmp2B8=loc;Cyc_Warn_err2(_tmp2B8,_tag_fat(_tmpBF,sizeof(void*),1U));});});{
struct Cyc_Tcenv_Tenv*_tmpC2=Cyc_Tcenv_allow_valueof(te);struct Cyc_Tcenv_Tenv*te2=_tmpC2;
Cyc_Tcexp_tcExp(te2,0,requires_clause);
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp204;_tmp204.tag=0U,({
struct _fat_ptr _tmp2B9=({const char*_tmpC8="@requires clause has type ";_tag_fat(_tmpC8,sizeof(char),27U);});_tmp204.f1=_tmp2B9;});_tmp204;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp203;_tmp203.tag=2U,_tmp203.f1=(void*)_check_null(requires_clause->topt);_tmp203;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp202;_tmp202.tag=0U,({
struct _fat_ptr _tmp2BA=({const char*_tmpC7=" instead of integral type";_tag_fat(_tmpC7,sizeof(char),26U);});_tmp202.f1=_tmp2BA;});_tmp202;});void*_tmpC3[3U];_tmpC3[0]=& _tmpC4,_tmpC3[1]=& _tmpC5,_tmpC3[2]=& _tmpC6;({unsigned _tmp2BB=requires_clause->loc;Cyc_Warn_err2(_tmp2BB,_tag_fat(_tmpC3,sizeof(void*),3U));});});else{
# 373
({unsigned _tmp2BE=requires_clause->loc;struct Cyc_Tcenv_Tenv*_tmp2BD=te;struct Cyc_List_List*_tmp2BC=tvs;Cyc_Tctyp_check_type(_tmp2BE,_tmp2BD,_tmp2BC,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=9U,_tmpC9->f1=requires_clause;_tmpC9;}));});{
# 375
struct Cyc_List_List*_tmpCA=Cyc_Relations_exp2relns(uprev_rgn,requires_clause);struct Cyc_List_List*relns=_tmpCA;
# 378
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp205;_tmp205.tag=0U,({struct _fat_ptr _tmp2BF=({const char*_tmpCD="@requires clause may be unsatisfiable";_tag_fat(_tmpCD,sizeof(char),38U);});_tmp205.f1=_tmp2BF;});_tmp205;});void*_tmpCB[1U];_tmpCB[0]=& _tmpCC;({unsigned _tmp2C0=requires_clause->loc;Cyc_Warn_err2(_tmp2C0,_tag_fat(_tmpCB,sizeof(void*),1U));});});
# 384
{struct Cyc_List_List*_tmpCE=prev_relations;struct Cyc_List_List*p=_tmpCE;for(0;p != 0;p=p->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,relns,(struct Cyc_List_List*)p->hd)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp206;_tmp206.tag=0U,({
struct _fat_ptr _tmp2C1=({const char*_tmpD1="@requires clause may overlap with previous clauses";_tag_fat(_tmpD1,sizeof(char),51U);});_tmp206.f1=_tmp2C1;});_tmp206;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD0;({unsigned _tmp2C2=requires_clause->loc;Cyc_Warn_err2(_tmp2C2,_tag_fat(_tmpCF,sizeof(void*),1U));});});}}
prev_relations=({struct Cyc_List_List*_tmpD2=_region_malloc(uprev_rgn,sizeof(*_tmpD2));_tmpD2->hd=relns,_tmpD2->tl=prev_relations;_tmpD2;});}}}}else{
# 391
if(prev_relations != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp207;_tmp207.tag=0U,({struct _fat_ptr _tmp2C3=({const char*_tmpD5="if one field has a @requires clause, they all must";_tag_fat(_tmpD5,sizeof(char),51U);});_tmp207.f1=_tmp2C3;});_tmp207;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD4;({unsigned _tmp2C4=loc;Cyc_Warn_err2(_tmp2C4,_tag_fat(_tmpD3,sizeof(void*),1U));});});}}}}}
# 330
;_pop_region();}
# 396
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 398
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmpD6=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmp9=_tmpD6;void*_tmpD7=_stmttmp9;enum Cyc_Absyn_AliasQual _tmpD8;struct Cyc_Absyn_Kind*_tmpDA;struct Cyc_Core_Opt**_tmpD9;struct Cyc_Core_Opt**_tmpDB;enum Cyc_Absyn_AliasQual _tmpDD;struct Cyc_Core_Opt**_tmpDC;struct Cyc_Core_Opt**_tmpDE;switch(*((int*)_tmpD7)){case 1U: _LL1: _tmpDE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD7)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpDE;
# 401
({struct Cyc_Core_Opt*_tmp2C6=({struct Cyc_Core_Opt*_tmpDF=_cycalloc(sizeof(*_tmpDF));({void*_tmp2C5=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmpDF->v=_tmp2C5;});_tmpDF;});*f=_tmp2C6;});continue;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmpDC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7)->f1;_tmpDD=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7)->f2)->aliasqual;_LL4: {struct Cyc_Core_Opt**f=_tmpDC;enum Cyc_Absyn_AliasQual a=_tmpDD;
# 403
if(constrain_top_kind &&(int)a == (int)2U)
({struct Cyc_Core_Opt*_tmp2C8=({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));({void*_tmp2C7=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->kind=Cyc_Absyn_BoxKind,_tmpE0->aliasqual=Cyc_Absyn_Aliasable;_tmpE0;}));_tmpE1->v=_tmp2C7;});_tmpE1;});*f=_tmp2C8;});else{
# 406
({struct Cyc_Core_Opt*_tmp2CA=({struct Cyc_Core_Opt*_tmpE3=_cycalloc(sizeof(*_tmpE3));({void*_tmp2C9=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->kind=Cyc_Absyn_BoxKind,_tmpE2->aliasqual=a;_tmpE2;}));_tmpE3->v=_tmp2C9;});_tmpE3;});*f=_tmp2CA;});}
continue;}case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmpDB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7)->f1;if(constrain_top_kind){_LL6: {struct Cyc_Core_Opt**f=_tmpDB;
# 409
({struct Cyc_Core_Opt*_tmp2CC=({struct Cyc_Core_Opt*_tmpE5=_cycalloc(sizeof(*_tmpE5));({void*_tmp2CB=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->kind=Cyc_Absyn_BoxKind,_tmpE4->aliasqual=Cyc_Absyn_Aliasable;_tmpE4;}));_tmpE5->v=_tmp2CB;});_tmpE5;});*f=_tmp2CC;});
continue;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmpD9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7)->f1;_tmpDA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD7)->f2;_LL8: {struct Cyc_Core_Opt**f=_tmpD9;struct Cyc_Absyn_Kind*k=_tmpDA;
# 412
({struct Cyc_Core_Opt*_tmp2CE=({struct Cyc_Core_Opt*_tmpE6=_cycalloc(sizeof(*_tmpE6));({void*_tmp2CD=Cyc_Tcutil_kind_to_bound(k);_tmpE6->v=_tmp2CD;});_tmpE6;});*f=_tmp2CE;});continue;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpD7)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmpD8=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpD7)->f1)->aliasqual;_LLA: {enum Cyc_Absyn_AliasQual a=_tmpD8;
# 414
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20D;_tmp20D.tag=0U,({struct _fat_ptr _tmp2CF=({const char*_tmpF1="type ";_tag_fat(_tmpF1,sizeof(char),6U);});_tmp20D.f1=_tmp2CF;});_tmp20D;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpE9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp20C;_tmp20C.tag=1U,_tmp20C.f1=n;_tmp20C;});struct Cyc_Warn_String_Warn_Warg_struct _tmpEA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20B;_tmp20B.tag=0U,({struct _fat_ptr _tmp2D0=({const char*_tmpF0=" attempts to abstract type variable ";_tag_fat(_tmpF0,sizeof(char),37U);});_tmp20B.f1=_tmp2D0;});_tmp20B;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp20A;_tmp20A.tag=7U,_tmp20A.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp20A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpEC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp209;_tmp209.tag=0U,({
struct _fat_ptr _tmp2D1=({const char*_tmpEF=" of kind ";_tag_fat(_tmpEF,sizeof(char),10U);});_tmp209.f1=_tmp2D1;});_tmp209;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmpED=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp208;_tmp208.tag=9U,({struct Cyc_Absyn_Kind*_tmp2D2=({struct Cyc_Absyn_Kind*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->kind=Cyc_Absyn_MemKind,_tmpEE->aliasqual=a;_tmpEE;});_tmp208.f1=_tmp2D2;});_tmp208;});void*_tmpE7[6U];_tmpE7[0]=& _tmpE8,_tmpE7[1]=& _tmpE9,_tmpE7[2]=& _tmpEA,_tmpE7[3]=& _tmpEB,_tmpE7[4]=& _tmpEC,_tmpE7[5]=& _tmpED;({unsigned _tmp2D3=loc;Cyc_Warn_err2(_tmp2D3,_tag_fat(_tmpE7,sizeof(void*),6U));});});
continue;}}else{_LLB: _LLC:
 continue;}}_LL0:;}}
# 421
static void Cyc_Tc_rule_out_mem_and_unique(unsigned loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmpF2=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmpA=_tmpF2;void*_tmpF3=_stmttmpA;enum Cyc_Absyn_AliasQual _tmpF4;enum Cyc_Absyn_KindQual _tmpF5;enum Cyc_Absyn_KindQual _tmpF7;struct Cyc_Core_Opt**_tmpF6;struct Cyc_Core_Opt**_tmpF8;struct Cyc_Core_Opt**_tmpF9;struct Cyc_Core_Opt**_tmpFA;struct Cyc_Core_Opt**_tmpFB;struct Cyc_Core_Opt**_tmpFC;struct Cyc_Core_Opt**_tmpFD;switch(*((int*)_tmpF3)){case 1U: _LL1: _tmpFD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpF3)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpFD;
_tmpFC=f;goto _LL4;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmpFC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmpFC;
# 426
_tmpFB=f;goto _LL6;}case Cyc_Absyn_Aliasable: _LL5: _tmpFB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmpFB;
# 428
({struct Cyc_Core_Opt*_tmp2D5=({struct Cyc_Core_Opt*_tmpFE=_cycalloc(sizeof(*_tmpFE));({void*_tmp2D4=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmpFE->v=_tmp2D4;});_tmpFE;});*f=_tmp2D5;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmpFA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmpFA;
_tmpF9=f;goto _LLA;}case Cyc_Absyn_Aliasable: _LL9: _tmpF9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f1;_LLA: {struct Cyc_Core_Opt**f=_tmpF9;
# 431
({struct Cyc_Core_Opt*_tmp2D7=({struct Cyc_Core_Opt*_tmpFF=_cycalloc(sizeof(*_tmpFF));({void*_tmp2D6=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmpFF->v=_tmp2D6;});_tmpFF;});*f=_tmp2D7;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmpF8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f1;_LLC: {struct Cyc_Core_Opt**f=_tmpF8;
# 433
({struct Cyc_Core_Opt*_tmp2D9=({struct Cyc_Core_Opt*_tmp100=_cycalloc(sizeof(*_tmp100));({void*_tmp2D8=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp100->v=_tmp2D8;});_tmp100;});*f=_tmp2D9;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmpF6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f1;_tmpF7=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF3)->f2)->kind;_LL10: {struct Cyc_Core_Opt**f=_tmpF6;enum Cyc_Absyn_KindQual k=_tmpF7;
# 437
_tmpF5=k;goto _LL12;}}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpF3)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpF3)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 435
({struct Cyc_Warn_String_Warn_Warg_struct _tmp102=({struct Cyc_Warn_String_Warn_Warg_struct _tmp212;_tmp212.tag=0U,({struct _fat_ptr _tmp2DA=({const char*_tmp109="type ";_tag_fat(_tmp109,sizeof(char),6U);});_tmp212.f1=_tmp2DA;});_tmp212;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp103=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp211;_tmp211.tag=1U,_tmp211.f1=q;_tmp211;});struct Cyc_Warn_String_Warn_Warg_struct _tmp104=({struct Cyc_Warn_String_Warn_Warg_struct _tmp210;_tmp210.tag=0U,({struct _fat_ptr _tmp2DB=({const char*_tmp108=" attempts to abstract type variable ";_tag_fat(_tmp108,sizeof(char),37U);});_tmp210.f1=_tmp2DB;});_tmp210;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp105=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp20F;_tmp20F.tag=7U,_tmp20F.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp20F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp106=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20E;_tmp20E.tag=0U,({
struct _fat_ptr _tmp2DC=({const char*_tmp107=" of kind TR";_tag_fat(_tmp107,sizeof(char),12U);});_tmp20E.f1=_tmp2DC;});_tmp20E;});void*_tmp101[5U];_tmp101[0]=& _tmp102,_tmp101[1]=& _tmp103,_tmp101[2]=& _tmp104,_tmp101[3]=& _tmp105,_tmp101[4]=& _tmp106;({unsigned _tmp2DD=loc;Cyc_Warn_err2(_tmp2DD,_tag_fat(_tmp101,sizeof(void*),5U));});});goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpF3)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmpF5=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpF3)->f1)->kind;_LL12: {enum Cyc_Absyn_KindQual k=_tmpF5;
# 439
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp218;_tmp218.tag=0U,({struct _fat_ptr _tmp2DE=({const char*_tmp114="type ";_tag_fat(_tmp114,sizeof(char),6U);});_tmp218.f1=_tmp2DE;});_tmp218;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp217;_tmp217.tag=1U,_tmp217.f1=q;_tmp217;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp216;_tmp216.tag=0U,({struct _fat_ptr _tmp2DF=({const char*_tmp113=" attempts to abstract type variable ";_tag_fat(_tmp113,sizeof(char),37U);});_tmp216.f1=_tmp2DF;});_tmp216;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp215;_tmp215.tag=7U,_tmp215.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp215;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp214;_tmp214.tag=0U,({
struct _fat_ptr _tmp2E0=({const char*_tmp112=" of kind ";_tag_fat(_tmp112,sizeof(char),10U);});_tmp214.f1=_tmp2E0;});_tmp214;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp110=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp213;_tmp213.tag=9U,({struct Cyc_Absyn_Kind*_tmp2E1=({struct Cyc_Absyn_Kind*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->kind=k,_tmp111->aliasqual=Cyc_Absyn_Unique;_tmp111;});_tmp213.f1=_tmp2E1;});_tmp213;});void*_tmp10A[6U];_tmp10A[0]=& _tmp10B,_tmp10A[1]=& _tmp10C,_tmp10A[2]=& _tmp10D,_tmp10A[3]=& _tmp10E,_tmp10A[4]=& _tmp10F,_tmp10A[5]=& _tmp110;({unsigned _tmp2E2=loc;Cyc_Warn_err2(_tmp2E2,_tag_fat(_tmp10A,sizeof(void*),6U));});});goto _LL0;}}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpF3)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmpF4=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpF3)->f1)->aliasqual;_LL14: {enum Cyc_Absyn_AliasQual a=_tmpF4;
# 442
({struct Cyc_Warn_String_Warn_Warg_struct _tmp116=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21E;_tmp21E.tag=0U,({struct _fat_ptr _tmp2E3=({const char*_tmp11F="type ";_tag_fat(_tmp11F,sizeof(char),6U);});_tmp21E.f1=_tmp2E3;});_tmp21E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp117=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp21D;_tmp21D.tag=1U,_tmp21D.f1=q;_tmp21D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp118=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21C;_tmp21C.tag=0U,({struct _fat_ptr _tmp2E4=({const char*_tmp11E=" attempts to abstract type variable ";_tag_fat(_tmp11E,sizeof(char),37U);});_tmp21C.f1=_tmp2E4;});_tmp21C;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp119=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp21B;_tmp21B.tag=7U,_tmp21B.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp21B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21A;_tmp21A.tag=0U,({
struct _fat_ptr _tmp2E5=({const char*_tmp11D=" of kind ";_tag_fat(_tmp11D,sizeof(char),10U);});_tmp21A.f1=_tmp2E5;});_tmp21A;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp11B=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp219;_tmp219.tag=9U,({struct Cyc_Absyn_Kind*_tmp2E6=({struct Cyc_Absyn_Kind*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->kind=Cyc_Absyn_MemKind,_tmp11C->aliasqual=a;_tmp11C;});_tmp219.f1=_tmp2E6;});_tmp219;});void*_tmp115[6U];_tmp115[0]=& _tmp116,_tmp115[1]=& _tmp117,_tmp115[2]=& _tmp118,_tmp115[3]=& _tmp119,_tmp115[4]=& _tmp11A,_tmp115[5]=& _tmp11B;({unsigned _tmp2E7=loc;Cyc_Warn_err2(_tmp2E7,_tag_fat(_tmp115,sizeof(void*),6U));});});goto _LL0;}}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 450
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp120=ad->name;struct _tuple0*q=_tmp120;
# 456
Cyc_Atts_check_field_atts(loc,(*q).f2,ad->attributes);{
# 458
struct Cyc_List_List*_tmp121=ad->tvs;struct Cyc_List_List*tvs=_tmp121;
# 461
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 466
struct _tuple15 _tmp122=({struct _tuple15 _tmp229;_tmp229.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp2E8=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,q);_tmp229.f2=_tmp2E8;});_tmp229;});struct _tuple15 _stmttmpB=_tmp122;struct _tuple15 _tmp123=_stmttmpB;struct Cyc_Absyn_Aggrdecl**_tmp128;int _tmp127;struct Cyc_List_List*_tmp126;struct Cyc_List_List*_tmp125;struct Cyc_List_List*_tmp124;int _tmp12C;struct Cyc_List_List*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_Aggrdecl**_tmp12D;if(_tmp123.f1 == 0){if(_tmp123.f2 == 0){_LL1: _LL2:
# 469
 Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 471
({struct Cyc_Dict_Dict _tmp2EB=({struct Cyc_Dict_Dict _tmp2EA=(te->ae)->aggrdecls;struct _tuple0*_tmp2E9=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp2EA,_tmp2E9,({struct Cyc_Absyn_Aggrdecl**_tmp12E=_cycalloc(sizeof(*_tmp12E));*_tmp12E=ad;_tmp12E;}));});(te->ae)->aggrdecls=_tmp2EB;});
goto _LL0;}else{_LL7: _tmp12D=*_tmp123.f2;_LL8: {struct Cyc_Absyn_Aggrdecl**adp=_tmp12D;
# 530
struct Cyc_Absyn_Aggrdecl*_tmp149=Cyc_Tcdecl_merge_aggrdecl(*adp,ad,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Aggrdecl*ad2=_tmp149;
if(ad2 == 0)
return;
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 536
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,q,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
*adp=ad2;}}}else{if(_tmp123.f2 == 0){_LL3: _tmp129=(_tmp123.f1)->exist_vars;_tmp12A=(_tmp123.f1)->rgn_po;_tmp12B=(_tmp123.f1)->fields;_tmp12C=(_tmp123.f1)->tagged;_LL4: {struct Cyc_List_List*exist_vars=_tmp129;struct Cyc_List_List*rgn_po=_tmp12A;struct Cyc_List_List*fs=_tmp12B;int tagged=_tmp12C;
# 476
struct Cyc_Absyn_Aggrdecl**_tmp12F=({struct Cyc_Absyn_Aggrdecl**_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_Absyn_Aggrdecl*_tmp2EC=({struct Cyc_Absyn_Aggrdecl*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->kind=ad->kind,_tmp13F->sc=Cyc_Absyn_Extern,_tmp13F->name=ad->name,_tmp13F->tvs=tvs,_tmp13F->impl=0,_tmp13F->attributes=ad->attributes,_tmp13F->expected_mem_kind=0;_tmp13F;});*_tmp140=_tmp2EC;});_tmp140;});struct Cyc_Absyn_Aggrdecl**adp=_tmp12F;
# 478
({struct Cyc_Dict_Dict _tmp2ED=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,q,adp);(te->ae)->aggrdecls=_tmp2ED;});
# 483
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 487
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp131=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21F;_tmp21F.tag=0U,({struct _fat_ptr _tmp2EE=({const char*_tmp132="@tagged is allowed only on union declarations";_tag_fat(_tmp132,sizeof(char),46U);});_tmp21F.f1=_tmp2EE;});_tmp21F;});void*_tmp130[1U];_tmp130[0]=& _tmp131;({unsigned _tmp2EF=loc;Cyc_Warn_err2(_tmp2EF,_tag_fat(_tmp130,sizeof(void*),1U));});});
({struct Cyc_Tcenv_Tenv*_tmp2F4=te;unsigned _tmp2F3=loc;enum Cyc_Absyn_AggrKind _tmp2F2=ad->kind;struct Cyc_List_List*_tmp2F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp2F0=rgn_po;Cyc_Tc_tcAggrImpl(_tmp2F4,_tmp2F3,_tmp2F2,_tmp2F1,_tmp2F0,fs);});
# 491
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 494
Cyc_Tc_rule_out_memkind(loc,q,exist_vars,1);
# 496
if((int)ad->kind == (int)Cyc_Absyn_UnionA && !tagged){
# 499
struct Cyc_List_List*f=fs;for(0;f != 0;f=f->tl){
if((Cyc_Flags_tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp134=({struct Cyc_Warn_String_Warn_Warg_struct _tmp226;_tmp226.tag=0U,({struct _fat_ptr _tmp2F5=({const char*_tmp13E="member ";_tag_fat(_tmp13E,sizeof(char),8U);});_tmp226.f1=_tmp2F5;});_tmp226;});struct Cyc_Warn_String_Warn_Warg_struct _tmp135=({struct Cyc_Warn_String_Warn_Warg_struct _tmp225;_tmp225.tag=0U,_tmp225.f1=*((struct Cyc_Absyn_Aggrfield*)f->hd)->name;_tmp225;});struct Cyc_Warn_String_Warn_Warg_struct _tmp136=({struct Cyc_Warn_String_Warn_Warg_struct _tmp224;_tmp224.tag=0U,({struct _fat_ptr _tmp2F6=({const char*_tmp13D=" of union ";_tag_fat(_tmp13D,sizeof(char),11U);});_tmp224.f1=_tmp2F6;});_tmp224;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp137=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp223;_tmp223.tag=1U,_tmp223.f1=q;_tmp223;});struct Cyc_Warn_String_Warn_Warg_struct _tmp138=({struct Cyc_Warn_String_Warn_Warg_struct _tmp222;_tmp222.tag=0U,({struct _fat_ptr _tmp2F7=({const char*_tmp13C=" has type ";_tag_fat(_tmp13C,sizeof(char),11U);});_tmp222.f1=_tmp2F7;});_tmp222;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp139=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp221;_tmp221.tag=2U,_tmp221.f1=(void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type;_tmp221;});struct Cyc_Warn_String_Warn_Warg_struct _tmp13A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp220;_tmp220.tag=0U,({
struct _fat_ptr _tmp2F8=({const char*_tmp13B=" so it can only be written and not read";_tag_fat(_tmp13B,sizeof(char),40U);});_tmp220.f1=_tmp2F8;});_tmp220;});void*_tmp133[7U];_tmp133[0]=& _tmp134,_tmp133[1]=& _tmp135,_tmp133[2]=& _tmp136,_tmp133[3]=& _tmp137,_tmp133[4]=& _tmp138,_tmp133[5]=& _tmp139,_tmp133[6]=& _tmp13A;({unsigned _tmp2F9=loc;Cyc_Warn_warn2(_tmp2F9,_tag_fat(_tmp133,sizeof(void*),7U));});});}}
*adp=ad;
goto _LL0;}}else{_LL5: _tmp124=(_tmp123.f1)->exist_vars;_tmp125=(_tmp123.f1)->rgn_po;_tmp126=(_tmp123.f1)->fields;_tmp127=(_tmp123.f1)->tagged;_tmp128=*_tmp123.f2;_LL6: {struct Cyc_List_List*exist_vars=_tmp124;struct Cyc_List_List*rgn_po=_tmp125;struct Cyc_List_List*fs=_tmp126;int tagged=_tmp127;struct Cyc_Absyn_Aggrdecl**adp=_tmp128;
# 508
if((int)ad->kind != (int)(*adp)->kind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp142=({struct Cyc_Warn_String_Warn_Warg_struct _tmp227;_tmp227.tag=0U,({struct _fat_ptr _tmp2FA=({const char*_tmp143="cannot reuse struct names for unions and vice-versa";_tag_fat(_tmp143,sizeof(char),52U);});_tmp227.f1=_tmp2FA;});_tmp227;});void*_tmp141[1U];_tmp141[0]=& _tmp142;({unsigned _tmp2FB=loc;Cyc_Warn_err2(_tmp2FB,_tag_fat(_tmp141,sizeof(void*),1U));});});{
# 511
struct Cyc_Absyn_Aggrdecl*_tmp144=*adp;struct Cyc_Absyn_Aggrdecl*ad0=_tmp144;
# 513
({struct Cyc_Absyn_Aggrdecl*_tmp2FC=({struct Cyc_Absyn_Aggrdecl*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->kind=ad->kind,_tmp145->sc=Cyc_Absyn_Extern,_tmp145->name=ad->name,_tmp145->tvs=tvs,_tmp145->impl=0,_tmp145->attributes=ad->attributes,_tmp145->expected_mem_kind=0;_tmp145;});*adp=_tmp2FC;});
# 519
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 522
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp147=({struct Cyc_Warn_String_Warn_Warg_struct _tmp228;_tmp228.tag=0U,({struct _fat_ptr _tmp2FD=({const char*_tmp148="@tagged is allowed only on union declarations";_tag_fat(_tmp148,sizeof(char),46U);});_tmp228.f1=_tmp2FD;});_tmp228;});void*_tmp146[1U];_tmp146[0]=& _tmp147;({unsigned _tmp2FE=loc;Cyc_Warn_err2(_tmp2FE,_tag_fat(_tmp146,sizeof(void*),1U));});});
({struct Cyc_Tcenv_Tenv*_tmp303=te;unsigned _tmp302=loc;enum Cyc_Absyn_AggrKind _tmp301=ad->kind;struct Cyc_List_List*_tmp300=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp2FF=rgn_po;Cyc_Tc_tcAggrImpl(_tmp303,_tmp302,_tmp301,_tmp300,_tmp2FF,fs);});
# 526
*adp=ad0;
_tmp12D=adp;goto _LL8;}}}}_LL0:;}}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 542
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 550
{struct Cyc_List_List*_tmp14A=fields;struct Cyc_List_List*fs=_tmp14A;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp14B=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp14B;
struct Cyc_List_List*typs=f->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tctyp_check_type(f->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 555
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22B;_tmp22B.tag=0U,({struct _fat_ptr _tmp304=({const char*_tmp14F="noalias pointers in datatypes are not allowed: ";_tag_fat(_tmp14F,sizeof(char),48U);});_tmp22B.f1=_tmp304;});_tmp22B;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp22A;_tmp22A.tag=1U,_tmp22A.f1=f->name;_tmp22A;});void*_tmp14C[2U];_tmp14C[0]=& _tmp14D,_tmp14C[1]=& _tmp14E;({unsigned _tmp305=f->loc;Cyc_Warn_err2(_tmp305,_tag_fat(_tmp14C,sizeof(void*),2U));});});
# 558
({int _tmp306=
Cyc_Tcutil_extract_const_from_typedef(f->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 558
((*((struct _tuple16*)typs->hd)).f1).real_const=_tmp306;});}}}
# 564
if(is_extensible){
# 566
int _tmp150=1;int res=_tmp150;
struct Cyc_List_List*_tmp151=Cyc_Tcdecl_sort_xdatatype_fields(fields,& res,(*name).f2,loc,Cyc_Tc_tc_msg);struct Cyc_List_List*fs=_tmp151;
return res?fs: 0;}{
# 570
struct _RegionHandle _tmp152=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp152;_push_region(uprev_rgn);
# 572
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp153=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp153;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp155=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22F;_tmp22F.tag=0U,({struct _fat_ptr _tmp307=({const char*_tmp15A="duplicate field ";_tag_fat(_tmp15A,sizeof(char),17U);});_tmp22F.f1=_tmp307;});_tmp22F;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp156=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp22E;_tmp22E.tag=1U,_tmp22E.f1=f->name;_tmp22E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp157=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22D;_tmp22D.tag=0U,({struct _fat_ptr _tmp308=({const char*_tmp159=" in ";_tag_fat(_tmp159,sizeof(char),5U);});_tmp22D.f1=_tmp308;});_tmp22D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp158=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22C;_tmp22C.tag=0U,_tmp22C.f1=obj;_tmp22C;});void*_tmp154[4U];_tmp154[0]=& _tmp155,_tmp154[1]=& _tmp156,_tmp154[2]=& _tmp157,_tmp154[3]=& _tmp158;({unsigned _tmp309=f->loc;Cyc_Warn_err2(_tmp309,_tag_fat(_tmp154,sizeof(void*),4U));});});else{
# 578
prev_fields=({struct Cyc_List_List*_tmp15B=_region_malloc(uprev_rgn,sizeof(*_tmp15B));_tmp15B->hd=(*f->name).f2,_tmp15B->tl=prev_fields;_tmp15B;});}
# 580
if((int)f->sc != (int)Cyc_Absyn_Public){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp231;_tmp231.tag=0U,({struct _fat_ptr _tmp30A=({const char*_tmp15F="ignoring scope of field ";_tag_fat(_tmp15F,sizeof(char),25U);});_tmp231.f1=_tmp30A;});_tmp231;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp15E=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp230;_tmp230.tag=1U,_tmp230.f1=f->name;_tmp230;});void*_tmp15C[2U];_tmp15C[0]=& _tmp15D,_tmp15C[1]=& _tmp15E;({unsigned _tmp30B=loc;Cyc_Warn_warn2(_tmp30B,_tag_fat(_tmp15C,sizeof(void*),2U));});});
f->sc=Cyc_Absyn_Public;}}}{
# 585
struct Cyc_List_List*_tmp160=fields;_npop_handler(0U);return _tmp160;}}
# 572
;_pop_region();}}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 588
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _fat_ptr obj=tud->is_extensible?({const char*_tmp176="@extensible datatype";_tag_fat(_tmp176,sizeof(char),21U);}):({const char*_tmp177="datatype";_tag_fat(_tmp177,sizeof(char),9U);});
# 594
struct Cyc_List_List*tvs=tud->tvs;
# 597
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 603
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 613 "tc.cyc"
{struct _handler_cons _tmp161;_push_handler(& _tmp161);{int _tmp163=0;if(setjmp(_tmp161.handler))_tmp163=1;if(!_tmp163){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt == 0 && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->tag=Cyc_Dict_Absent;_tmp164;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 619
({union Cyc_Absyn_Nmspace _tmp30C=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp30C;});}
# 614
;_pop_handler();}else{void*_tmp162=(void*)Cyc_Core_get_exn_thrown();void*_tmp165=_tmp162;void*_tmp166;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp165)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 621
struct Cyc_Absyn_Datatypedecl***_tmp167=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);struct Cyc_Absyn_Datatypedecl***tdopt=_tmp167;
tud_opt=(unsigned)tdopt?({struct Cyc_Absyn_Datatypedecl***_tmp168=_cycalloc(sizeof(*_tmp168));*_tmp168=*tdopt;_tmp168;}): 0;
goto _LL0;}}else{_LL3: _tmp166=_tmp165;_LL4: {void*exn=_tmp166;(int)_rethrow(exn);}}_LL0:;}}}{
# 628
struct _tuple17 _tmp169=({struct _tuple17 _tmp232;_tmp232.f1=tud->fields,_tmp232.f2=tud_opt;_tmp232;});struct _tuple17 _stmttmpC=_tmp169;struct _tuple17 _tmp16A=_stmttmpC;struct Cyc_Absyn_Datatypedecl**_tmp16C;struct Cyc_List_List**_tmp16B;struct Cyc_List_List**_tmp16D;struct Cyc_Absyn_Datatypedecl**_tmp16E;if(_tmp16A.f1 == 0){if(_tmp16A.f2 == 0){_LL6: _LL7:
# 631
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp30F=({struct Cyc_Dict_Dict _tmp30E=(te->ae)->datatypedecls;struct _tuple0*_tmp30D=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp30E,_tmp30D,({struct Cyc_Absyn_Datatypedecl**_tmp16F=_cycalloc(sizeof(*_tmp16F));*_tmp16F=tud;_tmp16F;}));});(te->ae)->datatypedecls=_tmp30F;});
goto _LL5;}else{_LLC: _tmp16E=*_tmp16A.f2;_LLD: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp16E;
# 664
struct Cyc_Absyn_Datatypedecl*_tmp175=Cyc_Tcdecl_merge_datatypedecl(*tudp,tud,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Datatypedecl*tud2=_tmp175;
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(tud2 != 0)
*tudp=tud2;
goto _LL5;}}}else{if(_tmp16A.f2 == 0){_LL8: _tmp16D=(struct Cyc_List_List**)&(_tmp16A.f1)->v;_LL9: {struct Cyc_List_List**fs=_tmp16D;
# 636
struct Cyc_Absyn_Datatypedecl**_tmp170=({struct Cyc_Absyn_Datatypedecl**_tmp172=_cycalloc(sizeof(*_tmp172));({struct Cyc_Absyn_Datatypedecl*_tmp310=({struct Cyc_Absyn_Datatypedecl*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->sc=Cyc_Absyn_Extern,_tmp171->name=tud->name,_tmp171->tvs=tvs,_tmp171->fields=0,_tmp171->is_extensible=tud->is_extensible;_tmp171;});*_tmp172=_tmp310;});_tmp172;});struct Cyc_Absyn_Datatypedecl**tudp=_tmp170;
# 638
({struct Cyc_Dict_Dict _tmp311=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,tudp);(te->ae)->datatypedecls=_tmp311;});
# 641
({struct Cyc_List_List*_tmp312=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp312;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*tudp=tud;
goto _LL5;}}else{_LLA: _tmp16B=(struct Cyc_List_List**)&(_tmp16A.f1)->v;_tmp16C=*_tmp16A.f2;_LLB: {struct Cyc_List_List**fs=_tmp16B;struct Cyc_Absyn_Datatypedecl**tudp=_tmp16C;
# 646
struct Cyc_Absyn_Datatypedecl*_tmp173=*tudp;struct Cyc_Absyn_Datatypedecl*tud0=_tmp173;
# 649
if((!tud->is_extensible &&(unsigned)tud0)&& tud0->is_extensible)
tud->is_extensible=1;
# 652
({struct Cyc_Absyn_Datatypedecl*_tmp313=({struct Cyc_Absyn_Datatypedecl*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->sc=Cyc_Absyn_Extern,_tmp174->name=tud->name,_tmp174->tvs=tvs,_tmp174->fields=0,_tmp174->is_extensible=tud->is_extensible;_tmp174;});*tudp=_tmp313;});
# 656
({struct Cyc_List_List*_tmp314=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp314;});
# 659
*tudp=tud0;
_tmp16E=tudp;goto _LLD;}}}_LL5:;}}}
# 672
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 678
if(ed->fields != 0){
struct _RegionHandle _tmp178=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp178;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp179=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp179;
# 685
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp234;_tmp234.tag=0U,({struct _fat_ptr _tmp315=({const char*_tmp17D="duplicate enum constructor ";_tag_fat(_tmp17D,sizeof(char),28U);});_tmp234.f1=_tmp315;});_tmp234;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp233;_tmp233.tag=1U,_tmp233.f1=f->name;_tmp233;});void*_tmp17A[2U];_tmp17A[0]=& _tmp17B,_tmp17A[1]=& _tmp17C;({unsigned _tmp316=f->loc;Cyc_Warn_err2(_tmp316,_tag_fat(_tmp17A,sizeof(void*),2U));});});else{
# 688
prev_fields=({struct Cyc_List_List*_tmp17E=_region_malloc(uprev_rgn,sizeof(*_tmp17E));_tmp17E->hd=(*f->name).f2,_tmp17E->tl=prev_fields;_tmp17E;});}
# 690
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,f->name)!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp237;_tmp237.tag=0U,({struct _fat_ptr _tmp317=({const char*_tmp184="enum field name ";_tag_fat(_tmp184,sizeof(char),17U);});_tmp237.f1=_tmp317;});_tmp237;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp181=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp236;_tmp236.tag=1U,_tmp236.f1=f->name;_tmp236;});struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp235;_tmp235.tag=0U,({struct _fat_ptr _tmp318=({const char*_tmp183=" shadows global name";_tag_fat(_tmp183,sizeof(char),21U);});_tmp235.f1=_tmp318;});_tmp235;});void*_tmp17F[3U];_tmp17F[0]=& _tmp180,_tmp17F[1]=& _tmp181,_tmp17F[2]=& _tmp182;({unsigned _tmp319=f->loc;Cyc_Warn_err2(_tmp319,_tag_fat(_tmp17F,sizeof(void*),3U));});});
if(f->tag == 0)
({struct Cyc_Absyn_Exp*_tmp31A=Cyc_Absyn_uint_exp(tag_count ++,f->loc);f->tag=_tmp31A;});else{
# 695
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))){
struct _tuple11 _tmp185=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag));struct _tuple11 _stmttmpD=_tmp185;struct _tuple11 _tmp186=_stmttmpD;int _tmp188;unsigned _tmp187;_LL1: _tmp187=_tmp186.f1;_tmp188=_tmp186.f2;_LL2: {unsigned t1=_tmp187;int known=_tmp188;
if(known)
tag_count=t1 + (unsigned)1;}}}}}
# 680
;_pop_region();}
# 704
{struct _handler_cons _tmp189;_push_handler(& _tmp189);{int _tmp18B=0;if(setjmp(_tmp189.handler))_tmp18B=1;if(!_tmp18B){
{struct Cyc_Absyn_Enumdecl**_tmp18C=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);struct Cyc_Absyn_Enumdecl**edp=_tmp18C;
struct Cyc_Absyn_Enumdecl*_tmp18D=Cyc_Tcdecl_merge_enumdecl(*edp,ed,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Enumdecl*ed2=_tmp18D;
if(ed2 == 0){_npop_handler(0U);return;}
*edp=ed2;}
# 705
;_pop_handler();}else{void*_tmp18A=(void*)Cyc_Core_get_exn_thrown();void*_tmp18E=_tmp18A;void*_tmp18F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp18E)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 710
struct Cyc_Absyn_Enumdecl**_tmp190=({struct Cyc_Absyn_Enumdecl**_tmp191=_cycalloc(sizeof(*_tmp191));*_tmp191=ed;_tmp191;});struct Cyc_Absyn_Enumdecl**edp=_tmp190;
({struct Cyc_Dict_Dict _tmp31B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,edp);(te->ae)->enumdecls=_tmp31B;});
goto _LL3;}}else{_LL6: _tmp18F=_tmp18E;_LL7: {void*exn=_tmp18F;(int)_rethrow(exn);}}_LL3:;}}}
# 716
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp192=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp192;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp194=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23C;_tmp23C.tag=0U,({struct _fat_ptr _tmp31C=({const char*_tmp19B="enum ";_tag_fat(_tmp19B,sizeof(char),6U);});_tmp23C.f1=_tmp31C;});_tmp23C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp195=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp23B;_tmp23B.tag=1U,_tmp23B.f1=q;_tmp23B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp196=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23A;_tmp23A.tag=0U,({struct _fat_ptr _tmp31D=({const char*_tmp19A=", field ";_tag_fat(_tmp19A,sizeof(char),9U);});_tmp23A.f1=_tmp31D;});_tmp23A;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp197=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp239;_tmp239.tag=1U,_tmp239.f1=f->name;_tmp239;});struct Cyc_Warn_String_Warn_Warg_struct _tmp198=({struct Cyc_Warn_String_Warn_Warg_struct _tmp238;_tmp238.tag=0U,({
struct _fat_ptr _tmp31E=({const char*_tmp199=": expression is not constant";_tag_fat(_tmp199,sizeof(char),29U);});_tmp238.f1=_tmp31E;});_tmp238;});void*_tmp193[5U];_tmp193[0]=& _tmp194,_tmp193[1]=& _tmp195,_tmp193[2]=& _tmp196,_tmp193[3]=& _tmp197,_tmp193[4]=& _tmp198;({unsigned _tmp31F=loc;Cyc_Warn_err2(_tmp31F,_tag_fat(_tmp193,sizeof(void*),5U));});});}}}
# 726
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include,int in_inc_rep){
enum Cyc_Absyn_Scope _tmp19C=sc;switch(_tmp19C){case Cyc_Absyn_Static: _LL1: _LL2:
# 729
 if(!in_include)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23D;_tmp23D.tag=0U,({struct _fat_ptr _tmp320=({const char*_tmp19F="static declaration within extern \"C\"";_tag_fat(_tmp19F,sizeof(char),37U);});_tmp23D.f1=_tmp320;});_tmp23D;});void*_tmp19D[1U];_tmp19D[0]=& _tmp19E;({unsigned _tmp321=loc;Cyc_Warn_warn2(_tmp321,_tag_fat(_tmp19D,sizeof(void*),1U));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 733
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23E;_tmp23E.tag=0U,({struct _fat_ptr _tmp322=({const char*_tmp1A2="abstract declaration within extern \"C\"";_tag_fat(_tmp1A2,sizeof(char),39U);});_tmp23E.f1=_tmp322;});_tmp23E;});void*_tmp1A0[1U];_tmp1A0[0]=& _tmp1A1;({unsigned _tmp323=loc;Cyc_Warn_warn2(_tmp323,_tag_fat(_tmp1A0,sizeof(void*),1U));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;case Cyc_Absyn_ExternC: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
# 740
 if(!in_inc_rep)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23F;_tmp23F.tag=0U,({struct _fat_ptr _tmp324=({const char*_tmp1A5="nested extern \"C\" declaration";_tag_fat(_tmp1A5,sizeof(char),30U);});_tmp23F.f1=_tmp324;});_tmp23F;});void*_tmp1A3[1U];_tmp1A3[0]=& _tmp1A4;({unsigned _tmp325=loc;Cyc_Warn_warn2(_tmp325,_tag_fat(_tmp1A3,sizeof(void*),1U));});});
return 1;}_LL0:;}
# 746
static void Cyc_Tc_set_scopes(struct Cyc_List_List*ovrs,enum Cyc_Absyn_Scope sc){
for(0;ovrs != 0;ovrs=ovrs->tl){
void*_tmp1A6=((struct Cyc_Absyn_Decl*)ovrs->hd)->r;void*_stmttmpE=_tmp1A6;void*_tmp1A7=_stmttmpE;struct Cyc_Absyn_Enumdecl*_tmp1A8;struct Cyc_Absyn_Datatypedecl*_tmp1A9;struct Cyc_Absyn_Aggrdecl*_tmp1AA;struct Cyc_Absyn_Fndecl*_tmp1AB;struct Cyc_Absyn_Vardecl*_tmp1AC;switch(*((int*)_tmp1A7)){case 0U: _LL1: _tmp1AC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1A7)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1AC;
vd->sc=sc;goto _LL0;}case 1U: _LL3: _tmp1AB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1A7)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp1AB;
fd->sc=sc;goto _LL0;}case 5U: _LL5: _tmp1AA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1A7)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1AA;
ad->sc=sc;goto _LL0;}case 6U: _LL7: _tmp1A9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1A7)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1A9;
tud->sc=sc;goto _LL0;}case 7U: _LL9: _tmp1A8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1A7)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp1A8;
ed->sc=sc;goto _LL0;}default: _LLB: _LLC:
 goto _LL0;}_LL0:;}}
# 763
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 767
struct Cyc_List_List*_tmp1AD=ds0;struct Cyc_List_List*ds=_tmp1AD;for(0;ds != 0;ds=ds->tl){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
unsigned loc=d->loc;
void*_tmp1AE=d->r;void*_stmttmpF=_tmp1AE;void*_tmp1AF=_stmttmpF;struct _tuple10*_tmp1B3;struct Cyc_List_List*_tmp1B2;struct Cyc_List_List*_tmp1B1;struct Cyc_List_List**_tmp1B0;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_List_List*_tmp1B7;struct _fat_ptr*_tmp1B6;struct Cyc_Absyn_Enumdecl*_tmp1B8;struct Cyc_Absyn_Datatypedecl*_tmp1B9;struct Cyc_Absyn_Aggrdecl*_tmp1BA;struct Cyc_Absyn_Typedefdecl*_tmp1BB;struct Cyc_Absyn_Fndecl*_tmp1BC;struct Cyc_Absyn_Vardecl*_tmp1BD;switch(*((int*)_tmp1AF)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
# 773
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp240;_tmp240.tag=0U,({struct _fat_ptr _tmp326=({const char*_tmp1C0="top level let-declarations are not implemented";_tag_fat(_tmp1C0,sizeof(char),47U);});_tmp240.f1=_tmp326;});_tmp240;});void*_tmp1BE[1U];_tmp1BE[0]=& _tmp1BF;({unsigned _tmp327=loc;Cyc_Warn_err2(_tmp327,_tag_fat(_tmp1BE,sizeof(void*),1U));});});goto _LL0;case 4U: _LL5: _LL6:
# 775
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp241;_tmp241.tag=0U,({struct _fat_ptr _tmp328=({const char*_tmp1C3="top level region declarations are not implemented";_tag_fat(_tmp1C3,sizeof(char),50U);});_tmp241.f1=_tmp328;});_tmp241;});void*_tmp1C1[1U];_tmp1C1[0]=& _tmp1C2;({unsigned _tmp329=loc;Cyc_Warn_err2(_tmp329,_tag_fat(_tmp1C1,sizeof(void*),1U));});});goto _LL0;case 0U: _LL7: _tmp1BD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp1BD;
# 777
if(in_externC && Cyc_Tc_okay_externC(d->loc,vd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
vd->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,vd,check_var_init,te->in_extern_c_include,exports);
goto _LL0;}case 1U: _LL9: _tmp1BC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_LLA: {struct Cyc_Absyn_Fndecl*fd=_tmp1BC;
# 782
if(in_externC && Cyc_Tc_okay_externC(d->loc,fd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
fd->sc=Cyc_Absyn_ExternC;
if(te->in_extern_c_include)
fd->orig_scope=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,fd,exports);
goto _LL0;}case 8U: _LLB: _tmp1BB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp1BB;
# 789
td->extern_c=te->in_extern_c_include;
# 793
Cyc_Tc_tcTypedefdecl(te,loc,td);
goto _LL0;}case 5U: _LLD: _tmp1BA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1BA;
# 796
if(in_externC && Cyc_Tc_okay_externC(d->loc,ad->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);
goto _LL0;}case 6U: _LLF: _tmp1B9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_LL10: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1B9;
# 801
if(in_externC && Cyc_Tc_okay_externC(d->loc,tud->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
tud->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
goto _LL0;}case 7U: _LL11: _tmp1B8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*ed=_tmp1B8;
# 806
if(in_externC && Cyc_Tc_okay_externC(d->loc,ed->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);
goto _LL0;}case 13U: _LL13: _LL14:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp242;_tmp242.tag=0U,({struct _fat_ptr _tmp32A=({const char*_tmp1C6="spurious __cyclone_port_on__";_tag_fat(_tmp1C6,sizeof(char),29U);});_tmp242.f1=_tmp32A;});_tmp242;});void*_tmp1C4[1U];_tmp1C4[0]=& _tmp1C5;({unsigned _tmp32B=d->loc;Cyc_Warn_warn2(_tmp32B,_tag_fat(_tmp1C4,sizeof(void*),1U));});});goto _LL0;case 14U: _LL15: _LL16:
 goto _LL0;case 15U: _LL17: _LL18:
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16U: _LL19: _LL1A:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9U: _LL1B: _tmp1B6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_tmp1B7=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AF)->f2;_LL1C: {struct _fat_ptr*v=_tmp1B6;struct Cyc_List_List*ds2=_tmp1B7;
# 816
struct Cyc_List_List*_tmp1C7=te->ns;struct Cyc_List_List*ns=_tmp1C7;
({struct Cyc_List_List*_tmp32D=({struct Cyc_List_List*_tmp32C=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp32C,({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8->hd=v,_tmp1C8->tl=0;_tmp1C8;}));});te->ns=_tmp32D;});
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);
te->ns=ns;
goto _LL0;}case 10U: _LL1D: _tmp1B5=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AF)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp1B5;
# 823
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11U: _LL1F: _tmp1B4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp1B4;
# 826
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default: _LL21: _tmp1B0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AF)->f1;_tmp1B1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AF)->f2;_tmp1B2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AF)->f3;_tmp1B3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AF)->f4;_LL22: {struct Cyc_List_List**ds2=_tmp1B0;struct Cyc_List_List*ovrs=_tmp1B1;struct Cyc_List_List*exports2=_tmp1B2;struct _tuple10*wc=_tmp1B3;
# 831
if((unsigned)ovrs){
struct Cyc_Tcenv_Tenv*_tmp1C9=Cyc_Tcenv_copy_tenv_dicts(te);struct Cyc_Tcenv_Tenv*tecpy=_tmp1C9;
tecpy->in_extern_c_include=1;
Cyc_Tc_tc_decls(tecpy,*ds2,1,check_var_init,0);
Cyc_Toc_init();
Cyc_Tc_set_scopes(ovrs,Cyc_Absyn_ExternC);
Cyc_Cifc_user_overrides(d->loc,tecpy,ds2,ovrs);
Cyc_Toc_finish();}{
# 840
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(exports2,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1CA=Cyc_Tcenv_enter_extern_c_include(te);struct Cyc_Tcenv_Tenv*te2=_tmp1CA;
te2->in_extern_c_inc_repeat=ovrs != 0;
Cyc_Tc_tc_decls(te2,*ds2,1,check_var_init,& newexs);
# 847
for(0;exports2 != 0;exports2=exports2->tl){
struct _tuple12*_tmp1CB=(struct _tuple12*)exports2->hd;struct _tuple12*exp=_tmp1CB;
if(!(*exp).f3)
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1CD=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp244;_tmp244.tag=1U,_tmp244.f1=(*exp).f2;_tmp244;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp243;_tmp243.tag=0U,({struct _fat_ptr _tmp32E=({const char*_tmp1CF=" is exported but not defined";_tag_fat(_tmp1CF,sizeof(char),29U);});_tmp243.f1=_tmp32E;});_tmp243;});void*_tmp1CC[2U];_tmp1CC[0]=& _tmp1CD,_tmp1CC[1]=& _tmp1CE;({unsigned _tmp32F=(*exp).f1;Cyc_Warn_warn2(_tmp32F,_tag_fat(_tmp1CC,sizeof(void*),2U));});});}
# 852
goto _LL0;}}}_LL0:;}}
# 857
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict _tmp1D0=Cyc_Callgraph_compute_callgraph(ds);struct Cyc_Dict_Dict cg=_tmp1D0;
# 862
struct Cyc_Dict_Dict _tmp1D1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict g))Cyc_Graph_scc)(cg);struct Cyc_Dict_Dict scc=_tmp1D1;
# 865
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 883 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*);
# 885
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 889
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){
if((int)vd->sc == (int)Cyc_Absyn_Extern ||(int)vd->sc == (int)Cyc_Absyn_ExternC)
return 1;{
void*_tmp1D2=Cyc_Absyn_compress(vd->type);void*_stmttmp10=_tmp1D2;void*_tmp1D3=_stmttmp10;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D3)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 898
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1D4=d->r;void*_stmttmp11=_tmp1D4;void*_tmp1D5=_stmttmp11;struct Cyc_List_List**_tmp1D6;struct Cyc_List_List**_tmp1D7;struct Cyc_List_List**_tmp1D8;struct Cyc_List_List**_tmp1D9;struct Cyc_Absyn_Vardecl*_tmp1DA;switch(*((int*)_tmp1D5)){case 0U: _LL1: _tmp1DA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1D5)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1DA;
# 902
if((env->in_cinclude || !Cyc_Tc_is_extern(vd))|| !({
struct _tuple0*_tmp330=vd->name;Cyc_Absyn_qvar_cmp(_tmp330,Cyc_Absyn_uniquergn_qvar());}))
return 1;
return(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,vd->name)).f2;}case 11U: _LL3: _tmp1D9=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1D5)->f1;_LL4: {struct Cyc_List_List**ds2p=_tmp1D9;
_tmp1D8=ds2p;goto _LL6;}case 10U: _LL5: _tmp1D8=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1D5)->f2;_LL6: {struct Cyc_List_List**ds2p=_tmp1D8;
_tmp1D7=ds2p;goto _LL8;}case 9U: _LL7: _tmp1D7=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1D5)->f2;_LL8: {struct Cyc_List_List**ds2p=_tmp1D7;
({struct Cyc_List_List*_tmp331=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp331;});return 1;}case 12U: _LL9: _tmp1D6=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1D5)->f1;_LLA: {struct Cyc_List_List**ds2p=_tmp1D6;
# 910
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp332=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp332;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 919
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1DB=({struct Cyc_Tc_TreeshakeEnv _tmp245;_tmp245.in_cinclude=0,_tmp245.ordinaries=(te->ae)->ordinaries;_tmp245;});struct Cyc_Tc_TreeshakeEnv env=_tmp1DB;
return Cyc_Tc_treeshake_f(& env,ds);}

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
extern int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
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
# 907
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 947
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 968
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 976
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1052
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 129
extern int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 43
int Cyc_Tcutil_is_function_type(void*);
# 46
int Cyc_Tcutil_is_array_type(void*);
# 53
int Cyc_Tcutil_is_bound_one(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 88
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 108
void*Cyc_Tcutil_compress(void*);
# 115
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 140
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 145
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 175
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 203
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 213
void Cyc_Tcutil_check_bitfield(unsigned,void*field_typ,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn);
# 216
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 237
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 247
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 262
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 274
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 281
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 285
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 302
int Cyc_Tcutil_zeroable_type(void*);
# 306
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 309
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
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
# 139
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 152
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
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 40
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
static struct _fat_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 49
int Cyc_Tc_aggressive_warn=0;struct _tuple12{unsigned f1;struct _tuple0*f2;int f3;};
# 51
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;struct _tuple12*p=_tmp1;
if(Cyc_Absyn_qvar_cmp(x,(*p).f2)== 0){
# 57
(*p).f3=1;
return 1;}}
# 61
return 0;}
# 64
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned loc,int i,int nargs,void*att){
if(i < 1 || i > nargs){
({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp3=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp1F9;_tmp1F9.tag=10U,_tmp1F9.f1=(void*)att;_tmp1F9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F8;_tmp1F8.tag=0U,({struct _fat_ptr _tmp24A=({const char*_tmp5=" has an out-of-range index";_tag_fat(_tmp5,sizeof(char),27U);});_tmp1F8.f1=_tmp24A;});_tmp1F8;});void*_tmp2[2U];_tmp2[0]=& _tmp3,_tmp2[1]=& _tmp4;({unsigned _tmp24B=loc;Cyc_Warn_err2(_tmp24B,_tag_fat(_tmp2,sizeof(void*),2U));});});
return 0;}
# 69
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _fat_ptr f2;};
# 71
static void Cyc_Tc_fnTypeAttsOverlap(unsigned loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 74
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0U,_tmp1FC.f1=(struct _fat_ptr)((struct _fat_ptr)lst1.f2);_tmp1FC;});struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0U,_tmp1FB.f1=(struct _fat_ptr)((struct _fat_ptr)lst2.f2);_tmp1FB;});struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=1U,_tmp1FA.f1=(unsigned long)i;_tmp1FA;});void*_tmp6[3U];_tmp6[0]=& _tmp8,_tmp6[1]=& _tmp9,_tmp6[2]=& _tmpA;({unsigned _tmp24D=loc;struct _fat_ptr _tmp24C=({const char*_tmp7="incompatible %s() and %s() attributes on parameter %d";_tag_fat(_tmp7,sizeof(char),54U);});Cyc_Warn_err(_tmp24D,_tmp24C,_tag_fat(_tmp6,sizeof(void*),3U));});});}
# 78
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned loc){
struct _tuple13 init_params=({struct _tuple13 _tmp20A;_tmp20A.f1=0,({struct _fat_ptr _tmp24E=({const char*_tmp42="initializes";_tag_fat(_tmp42,sizeof(char),12U);});_tmp20A.f2=_tmp24E;});_tmp20A;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp209;_tmp209.f1=0,({struct _fat_ptr _tmp24F=({const char*_tmp41="noliveunique";_tag_fat(_tmp41,sizeof(char),13U);});_tmp209.f2=_tmp24F;});_tmp209;});
struct _tuple13 consume_params=({struct _tuple13 _tmp208;_tmp208.f1=0,({struct _fat_ptr _tmp250=({const char*_tmp40="consume";_tag_fat(_tmp40,sizeof(char),8U);});_tmp208.f2=_tmp250;});_tmp208;});
void*_tmpB=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpD;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->tag == 5U){_LL1: _tmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).attributes;_tmpE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).args;_LL2: {struct Cyc_List_List*atts=_tmpD;struct Cyc_List_List*args=_tmpE;
# 84
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);
for(0;atts != 0;atts=atts->tl){
void*_tmpF=(void*)atts->hd;void*_stmttmp1=_tmpF;void*_tmp10=_stmttmp1;int _tmp11;int _tmp12;int _tmp13;switch(*((int*)_tmp10)){case 20U: _LL6: _tmp13=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10)->f1;_LL7: {int i=_tmp13;
# 88
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,i,init_params,consume_params);{
struct _tuple8*_tmp14=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp2=_tmp14;struct _tuple8*_tmp15=_stmttmp2;void*_tmp16;_LLF: _tmp16=_tmp15->f3;_LL10: {void*t=_tmp16;
struct _fat_ptr s=({const char*_tmp2E="initializes attribute allowed only on";_tag_fat(_tmp2E,sizeof(char),38U);});
{void*_tmp17=Cyc_Tcutil_compress(t);void*_stmttmp3=_tmp17;void*_tmp18=_stmttmp3;void*_tmp1C;void*_tmp1B;void*_tmp1A;void*_tmp19;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->tag == 3U){_LL12: _tmp19=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).elt_type;_tmp1A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).nullable;_tmp1B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).bounds;_tmp1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18)->f1).ptr_atts).zero_term;_LL13: {void*t=_tmp19;void*nullable=_tmp1A;void*bd=_tmp1B;void*zt=_tmp1C;
# 95
if(Cyc_Tcutil_force_type2bool(0,nullable))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FE;_tmp1FE.tag=0U,_tmp1FE.f1=s;_tmp1FE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FD;_tmp1FD.tag=0U,({struct _fat_ptr _tmp251=({const char*_tmp20=" non-null pointers";_tag_fat(_tmp20,sizeof(char),19U);});_tmp1FD.f1=_tmp251;});_tmp1FD;});void*_tmp1D[2U];_tmp1D[0]=& _tmp1E,_tmp1D[1]=& _tmp1F;({unsigned _tmp252=loc;Cyc_Warn_err2(_tmp252,_tag_fat(_tmp1D,sizeof(void*),2U));});});
if(!Cyc_Tcutil_is_bound_one(bd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp200;_tmp200.tag=0U,_tmp200.f1=s;_tmp200;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FF;_tmp1FF.tag=0U,({struct _fat_ptr _tmp253=({const char*_tmp24=" pointers of size 1";_tag_fat(_tmp24,sizeof(char),20U);});_tmp1FF.f1=_tmp253;});_tmp1FF;});void*_tmp21[2U];_tmp21[0]=& _tmp22,_tmp21[1]=& _tmp23;({unsigned _tmp254=loc;Cyc_Warn_err2(_tmp254,_tag_fat(_tmp21,sizeof(void*),2U));});});
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26=({struct Cyc_Warn_String_Warn_Warg_struct _tmp202;_tmp202.tag=0U,_tmp202.f1=s;_tmp202;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27=({struct Cyc_Warn_String_Warn_Warg_struct _tmp201;_tmp201.tag=0U,({struct _fat_ptr _tmp255=({const char*_tmp28=" pointers to non-zero-terminated arrays";_tag_fat(_tmp28,sizeof(char),40U);});_tmp201.f1=_tmp255;});_tmp201;});void*_tmp25[2U];_tmp25[0]=& _tmp26,_tmp25[1]=& _tmp27;({unsigned _tmp256=loc;Cyc_Warn_err2(_tmp256,_tag_fat(_tmp25,sizeof(void*),2U));});});
goto _LL11;}}else{_LL14: _LL15:
# 103
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp204;_tmp204.tag=0U,_tmp204.f1=s;_tmp204;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp203;_tmp203.tag=0U,({struct _fat_ptr _tmp257=({const char*_tmp2C=" pointers";_tag_fat(_tmp2C,sizeof(char),10U);});_tmp203.f1=_tmp257;});_tmp203;});void*_tmp29[2U];_tmp29[0]=& _tmp2A,_tmp29[1]=& _tmp2B;({unsigned _tmp258=loc;Cyc_Warn_err2(_tmp258,_tag_fat(_tmp29,sizeof(void*),2U));});});}_LL11:;}
# 105
({struct Cyc_List_List*_tmp259=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=(void*)i,_tmp2D->tl=init_params.f1;_tmp2D;});init_params.f1=_tmp259;});
goto _LL5;}}}case 21U: _LL8: _tmp12=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp10)->f1;_LL9: {int i=_tmp12;
# 108
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,nolive_unique_params,init_params);{
# 111
struct _tuple8*_tmp2F=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp4=_tmp2F;struct _tuple8*_tmp30=_stmttmp4;void*_tmp31;_LL17: _tmp31=_tmp30->f3;_LL18: {void*t=_tmp31;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp33=({struct Cyc_Warn_String_Warn_Warg_struct _tmp205;_tmp205.tag=0U,({struct _fat_ptr _tmp25A=({const char*_tmp34="noliveunique allowed only on unique pointers";_tag_fat(_tmp34,sizeof(char),45U);});_tmp205.f1=_tmp25A;});_tmp205;});void*_tmp32[1U];_tmp32[0]=& _tmp33;({unsigned _tmp25B=loc;Cyc_Warn_err2(_tmp25B,_tag_fat(_tmp32,sizeof(void*),1U));});});
({struct Cyc_List_List*_tmp25C=({struct Cyc_List_List*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->hd=(void*)i,_tmp35->tl=nolive_unique_params.f1;_tmp35;});nolive_unique_params.f1=_tmp25C;});
goto _LL5;}}}case 22U: _LLA: _tmp11=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp10)->f1;_LLB: {int i=_tmp11;
# 117
if(!Cyc_Tc_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,i,consume_params,init_params);{
# 120
struct _tuple8*_tmp36=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp5=_tmp36;struct _tuple8*_tmp37=_stmttmp5;void*_tmp38;_LL1A: _tmp38=_tmp37->f3;_LL1B: {void*t=_tmp38;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp206;_tmp206.tag=0U,({struct _fat_ptr _tmp25D=({const char*_tmp3B="consume allowed only on unique pointers";_tag_fat(_tmp3B,sizeof(char),40U);});_tmp206.f1=_tmp25D;});_tmp206;});void*_tmp39[1U];_tmp39[0]=& _tmp3A;({unsigned _tmp25E=loc;Cyc_Warn_err2(_tmp25E,_tag_fat(_tmp39,sizeof(void*),1U));});});
({struct Cyc_List_List*_tmp25F=({struct Cyc_List_List*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->hd=(void*)i,_tmp3C->tl=consume_params.f1;_tmp3C;});consume_params.f1=_tmp25F;});
goto _LL5;}}}default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 128
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp207;_tmp207.tag=0U,({struct _fat_ptr _tmp260=({const char*_tmp3F="fnTypeAttsOK: not a function type";_tag_fat(_tmp3F,sizeof(char),34U);});_tmp207.f1=_tmp260;});_tmp207;});void*_tmp3D[1U];_tmp3D[0]=& _tmp3E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3D,sizeof(void*),1U));});}_LL0:;}struct _tuple14{void*f1;int f2;};
# 133
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 135
struct Cyc_Absyn_Vardecl*_tmp43=vd;int*_tmp49;struct Cyc_List_List*_tmp48;struct Cyc_Absyn_Exp*_tmp47;void*_tmp46;struct _tuple0*_tmp45;enum Cyc_Absyn_Scope _tmp44;_LL1: _tmp44=_tmp43->sc;_tmp45=_tmp43->name;_tmp46=_tmp43->type;_tmp47=_tmp43->initializer;_tmp48=_tmp43->attributes;_tmp49=(int*)& _tmp43->is_proto;_LL2: {enum Cyc_Absyn_Scope sc=_tmp44;struct _tuple0*q=_tmp45;void*t=_tmp46;struct Cyc_Absyn_Exp*initializer=_tmp47;struct Cyc_List_List*atts=_tmp48;int*is_proto=_tmp49;
# 142
{void*_tmp4A=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmp4A;void*_tmp4B=_stmttmp6;unsigned _tmp4F;void*_tmp4E;struct Cyc_Absyn_Tqual _tmp4D;void*_tmp4C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B)->tag == 4U){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B)->f1).num_elts == 0){_LL4: _tmp4C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B)->f1).elt_type;_tmp4D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B)->f1).tq;_tmp4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B)->f1).zero_term;_tmp4F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B)->f1).zt_loc;if(initializer != 0){_LL5: {void*telt=_tmp4C;struct Cyc_Absyn_Tqual tq=_tmp4D;void*zt=_tmp4E;unsigned ztl=_tmp4F;
# 144
{void*_tmp50=initializer->r;void*_stmttmp7=_tmp50;void*_tmp51=_stmttmp7;struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp55;struct _fat_ptr _tmp56;struct _fat_ptr _tmp57;switch(*((int*)_tmp51)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp51)->f1).Wstring_c).tag){case 8U: _LL9: _tmp57=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp51)->f1).String_c).val;_LLA: {struct _fat_ptr s=_tmp57;
# 146
t=({void*_tmp265=({void*_tmp264=telt;struct Cyc_Absyn_Tqual _tmp263=tq;struct Cyc_Absyn_Exp*_tmp262=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_tmp261=zt;Cyc_Absyn_array_type(_tmp264,_tmp263,_tmp262,_tmp261,ztl);});vd->type=_tmp265;});
goto _LL8;}case 9U: _LLB: _tmp56=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp51)->f1).Wstring_c).val;_LLC: {struct _fat_ptr s=_tmp56;
# 149
t=({void*_tmp26A=({void*_tmp269=telt;struct Cyc_Absyn_Tqual _tmp268=tq;struct Cyc_Absyn_Exp*_tmp267=Cyc_Absyn_uint_exp(1U,0U);void*_tmp266=zt;Cyc_Absyn_array_type(_tmp269,_tmp268,_tmp267,_tmp266,ztl);});vd->type=_tmp26A;});
goto _LL8;}default: goto _LL15;}case 27U: _LLD: _tmp55=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp51)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp55;
_tmp54=e;goto _LL10;}case 28U: _LLF: _tmp54=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp51)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp54;
# 154
t=({void*_tmp26B=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_tmp26B;});
goto _LL8;}case 36U: _LL11: _tmp53=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp51)->f2;_LL12: {struct Cyc_List_List*es=_tmp53;
_tmp52=es;goto _LL14;}case 26U: _LL13: _tmp52=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp51)->f1;_LL14: {struct Cyc_List_List*es=_tmp52;
# 158
t=({void*_tmp270=({void*_tmp26F=telt;struct Cyc_Absyn_Tqual _tmp26E=tq;struct Cyc_Absyn_Exp*_tmp26D=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);void*_tmp26C=zt;Cyc_Absyn_array_type(_tmp26F,_tmp26E,_tmp26D,_tmp26C,ztl);});vd->type=_tmp270;});
goto _LL8;}default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 162
goto _LL3;}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 166
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
# 168
({int _tmp271=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp271;});
# 171
({int _tmp272=!Cyc_Tcutil_is_array_type(t);vd->escapes=_tmp272;});
# 173
if(Cyc_Tcutil_is_function_type(t)){
*is_proto=0;
atts=Cyc_Tcutil_transfer_fn_type_atts(t,atts);
Cyc_Tc_fnTypeAttsOK(te,t,loc);}
# 179
if((int)sc == (int)3U ||(int)sc == (int)4U){
if(initializer != 0 && !in_cinclude)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp59=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20B;_tmp20B.tag=0U,({struct _fat_ptr _tmp273=({const char*_tmp5A="extern declaration should not have initializer";_tag_fat(_tmp5A,sizeof(char),47U);});_tmp20B.f1=_tmp273;});_tmp20B;});void*_tmp58[1U];_tmp58[0]=& _tmp59;({unsigned _tmp274=loc;Cyc_Warn_err2(_tmp274,_tag_fat(_tmp58,sizeof(void*),1U));});});}else{
if(!Cyc_Tcutil_is_function_type(t)){
# 186
for(0;atts != 0;atts=atts->tl){
void*_tmp5B=(void*)atts->hd;void*_stmttmp8=_tmp5B;void*_tmp5C=_stmttmp8;switch(*((int*)_tmp5C)){case 6U: _LL18: _LL19:
 goto _LL1B;case 8U: _LL1A: _LL1B:
# 191
 goto _LL1D;case 9U: _LL1C: _LL1D:
 goto _LL1F;case 10U: _LL1E: _LL1F:
 goto _LL21;case 11U: _LL20: _LL21:
 goto _LL23;case 12U: _LL22: _LL23:
 goto _LL25;case 13U: _LL24: _LL25:
 goto _LL27;case 14U: _LL26: _LL27:
 continue;default: _LL28: _LL29:
# 199
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20F;_tmp20F.tag=0U,({struct _fat_ptr _tmp275=({const char*_tmp63="bad attribute ";_tag_fat(_tmp63,sizeof(char),15U);});_tmp20F.f1=_tmp275;});_tmp20F;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp20E;_tmp20E.tag=10U,_tmp20E.f1=(void*)atts->hd;_tmp20E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp60=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20D;_tmp20D.tag=0U,({struct _fat_ptr _tmp276=({const char*_tmp62=" for variable ";_tag_fat(_tmp62,sizeof(char),15U);});_tmp20D.f1=_tmp276;});_tmp20D;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp61=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp20C;_tmp20C.tag=11U,_tmp20C.f1=vd;_tmp20C;});void*_tmp5D[4U];_tmp5D[0]=& _tmp5E,_tmp5D[1]=& _tmp5F,_tmp5D[2]=& _tmp60,_tmp5D[3]=& _tmp61;({unsigned _tmp277=loc;Cyc_Warn_err2(_tmp277,_tag_fat(_tmp5D,sizeof(void*),4U));});});
goto _LL17;}_LL17:;}
# 203
if(initializer == 0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp65=({struct Cyc_Warn_String_Warn_Warg_struct _tmp213;_tmp213.tag=0U,({struct _fat_ptr _tmp278=({const char*_tmp6A="initializer required for variable ";_tag_fat(_tmp6A,sizeof(char),35U);});_tmp213.f1=_tmp278;});_tmp213;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp66=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp212;_tmp212.tag=11U,_tmp212.f1=vd;_tmp212;});struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp211;_tmp211.tag=0U,({struct _fat_ptr _tmp279=({const char*_tmp69=" of type ";_tag_fat(_tmp69,sizeof(char),10U);});_tmp211.f1=_tmp279;});_tmp211;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp68=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp210;_tmp210.tag=2U,_tmp210.f1=(void*)t;_tmp210;});void*_tmp64[4U];_tmp64[0]=& _tmp65,_tmp64[1]=& _tmp66,_tmp64[2]=& _tmp67,_tmp64[3]=& _tmp68;({unsigned _tmp27A=loc;Cyc_Warn_err2(_tmp27A,_tag_fat(_tmp64,sizeof(void*),4U));});});}else{
# 207
struct Cyc_Absyn_Exp*_tmp6B=initializer;struct Cyc_Absyn_Exp*e=_tmp6B;
# 211
struct _handler_cons _tmp6C;_push_handler(& _tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!_tmp6E){
{void*_tmp6F=Cyc_Tcexp_tcExpInitializer(te,& t,e);void*t2=_tmp6F;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp27C=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp27B=e;Cyc_Tcutil_coerce_assign(_tmp27C,_tmp27B,t);})){
struct _fat_ptr _tmp70=Cyc_Absynpp_qvar2string(vd->name);struct _fat_ptr s0=_tmp70;
const char*_tmp71=" declared with type ";const char*s1=_tmp71;
struct _fat_ptr _tmp72=Cyc_Absynpp_typ2string(t);struct _fat_ptr s2=_tmp72;
const char*_tmp73=" but initialized with type ";const char*s3=_tmp73;
struct _fat_ptr _tmp74=Cyc_Absynpp_typ2string(t2);struct _fat_ptr s4=_tmp74;
if(({unsigned long _tmp280=({unsigned long _tmp27F=({unsigned long _tmp27E=({unsigned long _tmp27D=Cyc_strlen((struct _fat_ptr)s0);_tmp27D + Cyc_strlen(({const char*_tmp75=s1;_tag_fat(_tmp75,sizeof(char),21U);}));});_tmp27E + Cyc_strlen((struct _fat_ptr)s2);});_tmp27F + Cyc_strlen(({const char*_tmp76=s3;_tag_fat(_tmp76,sizeof(char),28U);}));});_tmp280 + Cyc_strlen((struct _fat_ptr)s4);})> (unsigned long)70)
({struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp218;_tmp218.tag=0U,_tmp218.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp218;});struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp217;_tmp217.tag=0U,({struct _fat_ptr _tmp281=(struct _fat_ptr)({const char*_tmp7F=s1;_tag_fat(_tmp7F,sizeof(char),21U);});_tmp217.f1=_tmp281;});_tmp217;});struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp216;_tmp216.tag=0U,_tmp216.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp216;});struct Cyc_String_pa_PrintArg_struct _tmp7C=({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0U,({struct _fat_ptr _tmp282=(struct _fat_ptr)({const char*_tmp7E=s3;_tag_fat(_tmp7E,sizeof(char),28U);});_tmp215.f1=_tmp282;});_tmp215;});struct Cyc_String_pa_PrintArg_struct _tmp7D=({struct Cyc_String_pa_PrintArg_struct _tmp214;_tmp214.tag=0U,_tmp214.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp214;});void*_tmp77[5U];_tmp77[0]=& _tmp79,_tmp77[1]=& _tmp7A,_tmp77[2]=& _tmp7B,_tmp77[3]=& _tmp7C,_tmp77[4]=& _tmp7D;({unsigned _tmp284=loc;struct _fat_ptr _tmp283=({const char*_tmp78="%s%s\n\t%s\n%s\n\t%s";_tag_fat(_tmp78,sizeof(char),16U);});Cyc_Tcutil_terr(_tmp284,_tmp283,_tag_fat(_tmp77,sizeof(void*),5U));});});else{
# 222
({struct Cyc_String_pa_PrintArg_struct _tmp82=({struct Cyc_String_pa_PrintArg_struct _tmp21D;_tmp21D.tag=0U,_tmp21D.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp21D;});struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp21C;_tmp21C.tag=0U,({struct _fat_ptr _tmp285=(struct _fat_ptr)({const char*_tmp88=s1;_tag_fat(_tmp88,sizeof(char),21U);});_tmp21C.f1=_tmp285;});_tmp21C;});struct Cyc_String_pa_PrintArg_struct _tmp84=({struct Cyc_String_pa_PrintArg_struct _tmp21B;_tmp21B.tag=0U,_tmp21B.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp21B;});struct Cyc_String_pa_PrintArg_struct _tmp85=({struct Cyc_String_pa_PrintArg_struct _tmp21A;_tmp21A.tag=0U,({struct _fat_ptr _tmp286=(struct _fat_ptr)({const char*_tmp87=s3;_tag_fat(_tmp87,sizeof(char),28U);});_tmp21A.f1=_tmp286;});_tmp21A;});struct Cyc_String_pa_PrintArg_struct _tmp86=({struct Cyc_String_pa_PrintArg_struct _tmp219;_tmp219.tag=0U,_tmp219.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp219;});void*_tmp80[5U];_tmp80[0]=& _tmp82,_tmp80[1]=& _tmp83,_tmp80[2]=& _tmp84,_tmp80[3]=& _tmp85,_tmp80[4]=& _tmp86;({unsigned _tmp288=loc;struct _fat_ptr _tmp287=({const char*_tmp81="%s%s%s%s%s";_tag_fat(_tmp81,sizeof(char),11U);});Cyc_Tcutil_terr(_tmp288,_tmp287,_tag_fat(_tmp80,sizeof(void*),5U));});});}
Cyc_Unify_explain_failure();}
# 226
if(!Cyc_Tcutil_is_const_exp(e))
({void*_tmp89=0U;({unsigned _tmp28A=loc;struct _fat_ptr _tmp289=({const char*_tmp8A="initializer is not a constant expression";_tag_fat(_tmp8A,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp28A,_tmp289,_tag_fat(_tmp89,sizeof(void*),0U));});});}
# 212
;_pop_handler();}else{void*_tmp6D=(void*)Cyc_Core_get_exn_thrown();void*_tmp8B=_tmp6D;void*_tmp8C;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp8B)->tag == Cyc_Tcenv_Env_error){_LL2B: _LL2C:
# 230
({void*_tmp8D=0U;({unsigned _tmp28C=loc;struct _fat_ptr _tmp28B=({const char*_tmp8E="initializer is not a constant expression";_tag_fat(_tmp8E,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp28C,_tmp28B,_tag_fat(_tmp8D,sizeof(void*),0U));});});
goto _LL2A;}else{_LL2D: _tmp8C=_tmp8B;_LL2E: {void*exn=_tmp8C;(int)_rethrow(exn);}}_LL2A:;}}}}else{
# 236
for(0;atts != 0;atts=atts->tl){
void*_tmp8F=(void*)atts->hd;void*_stmttmp9=_tmp8F;void*_tmp90=_stmttmp9;switch(*((int*)_tmp90)){case 0U: _LL30: _LL31:
# 239
 goto _LL33;case 1U: _LL32: _LL33:
 goto _LL35;case 2U: _LL34: _LL35:
 goto _LL37;case 3U: _LL36: _LL37:
 goto _LL39;case 4U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 goto _LL3D;case 20U: _LL3C: _LL3D:
 goto _LL3F;case 23U: _LL3E: _LL3F:
 goto _LL41;case 26U: _LL40: _LL41:
 goto _LL43;case 5U: _LL42: _LL43:
({void*_tmp91=0U;({struct _fat_ptr _tmp28D=({const char*_tmp92="tcVardecl: fn type atts in function var decl";_tag_fat(_tmp92,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp28D,_tag_fat(_tmp91,sizeof(void*),0U));});});case 6U: _LL44: _LL45:
# 250
 goto _LL47;case 7U: _LL46: _LL47:
# 252
({struct Cyc_Warn_String_Warn_Warg_struct _tmp94=({struct Cyc_Warn_String_Warn_Warg_struct _tmp220;_tmp220.tag=0U,({struct _fat_ptr _tmp28E=({const char*_tmp98="bad attribute ";_tag_fat(_tmp98,sizeof(char),15U);});_tmp220.f1=_tmp28E;});_tmp220;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp95=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp21F;_tmp21F.tag=10U,_tmp21F.f1=(void*)atts->hd;_tmp21F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp96=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21E;_tmp21E.tag=0U,({struct _fat_ptr _tmp28F=({const char*_tmp97=" in function declaration";_tag_fat(_tmp97,sizeof(char),25U);});_tmp21E.f1=_tmp28F;});_tmp21E;});void*_tmp93[3U];_tmp93[0]=& _tmp94,_tmp93[1]=& _tmp95,_tmp93[2]=& _tmp96;({unsigned _tmp290=loc;Cyc_Warn_err2(_tmp290,_tag_fat(_tmp93,sizeof(void*),3U));});});
goto _LL2F;default: _LL48: _LL49:
 continue;}_LL2F:;}}}
# 259
{struct _handler_cons _tmp99;_push_handler(& _tmp99);{int _tmp9B=0;if(setjmp(_tmp99.handler))_tmp9B=1;if(!_tmp9B){
{struct _tuple14*_tmp9C=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple14*ans=_tmp9C;
void*_tmp9D=(*ans).f1;void*b0=_tmp9D;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9E=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=1U,_tmpA1->f1=vd;_tmpA1;});struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1=_tmp9E;
void*_tmp9F=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmp9F;
if(b == 0){_npop_handler(0U);return;}
# 266
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports)){
if(b != b0 ||(*ans).f2)
# 269
({struct Cyc_Dict_Dict _tmp293=({struct Cyc_Dict_Dict _tmp292=(te->ae)->ordinaries;struct _tuple0*_tmp291=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp292,_tmp291,({struct _tuple14*_tmpA0=_cycalloc(sizeof(*_tmpA0));
_tmpA0->f1=b,_tmpA0->f2=(*ans).f2;_tmpA0;}));});
# 269
(te->ae)->ordinaries=_tmp293;});}
# 271
_npop_handler(0U);return;}
# 260
;_pop_handler();}else{void*_tmp9A=(void*)Cyc_Core_get_exn_thrown();void*_tmpA2=_tmp9A;void*_tmpA3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA2)->tag == Cyc_Dict_Absent){_LL4B: _LL4C:
# 272
 goto _LL4A;}else{_LL4D: _tmpA3=_tmpA2;_LL4E: {void*exn=_tmpA3;(int)_rethrow(exn);}}_LL4A:;}}}
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp297=({struct Cyc_Dict_Dict _tmp296=(te->ae)->ordinaries;struct _tuple0*_tmp295=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp296,_tmp295,({struct _tuple14*_tmpA5=_cycalloc(sizeof(*_tmpA5));
({void*_tmp294=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=1U,_tmpA4->f1=vd;_tmpA4;});_tmpA5->f1=_tmp294;}),_tmpA5->f2=0;_tmpA5;}));});
# 274
(te->ae)->ordinaries=_tmp297;});}}
# 278
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmpA6=n;struct _fat_ptr*_tmpA8;union Cyc_Absyn_Nmspace _tmpA7;_LL1: _tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;_LL2: {union Cyc_Absyn_Nmspace nms=_tmpA7;struct _fat_ptr*v=_tmpA8;
union Cyc_Absyn_Nmspace _tmpA9=nms;if((_tmpA9.Abs_n).tag == 2){if((_tmpA9.Abs_n).val == 0){_LL4: _LL5:
# 282
 return({struct _fat_ptr _tmp298=(struct _fat_ptr)*v;Cyc_strcmp(_tmp298,({const char*_tmpAA="main";_tag_fat(_tmpAA,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;}}
# 288
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 290
struct _tuple0*q=fd->name;
# 294
if((int)fd->sc == (int)Cyc_Absyn_ExternC && !te->in_extern_c_include)
({void*_tmpAB=0U;({unsigned _tmp29A=loc;struct _fat_ptr _tmp299=({const char*_tmpAC="extern \"C\" functions cannot be implemented in Cyclone";_tag_fat(_tmpAC,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp29A,_tmp299,_tag_fat(_tmpAB,sizeof(void*),0U));});});
# 298
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpAD=(void*)atts->hd;void*_stmttmpA=_tmpAD;void*_tmpAE=_stmttmpA;switch(*((int*)_tmpAE)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
# 302
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp223;_tmp223.tag=0U,({struct _fat_ptr _tmp29B=({const char*_tmpB4="bad attribute ";_tag_fat(_tmpB4,sizeof(char),15U);});_tmp223.f1=_tmp29B;});_tmp223;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp222;_tmp222.tag=10U,_tmp222.f1=(void*)atts->hd;_tmp222;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp221;_tmp221.tag=0U,({struct _fat_ptr _tmp29C=({const char*_tmpB3=" for function";_tag_fat(_tmpB3,sizeof(char),14U);});_tmp221.f1=_tmp29C;});_tmp221;});void*_tmpAF[3U];_tmpAF[0]=& _tmpB0,_tmpAF[1]=& _tmpB1,_tmpAF[2]=& _tmpB2;({unsigned _tmp29D=loc;Cyc_Warn_err2(_tmp29D,_tag_fat(_tmpAF,sizeof(void*),3U));});});
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}}
# 308
if(te->in_extern_c_inc_repeat)
fd->cached_type=0;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 313
({struct Cyc_List_List*_tmp29E=Cyc_Tcutil_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp29E;});
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 317
{struct _handler_cons _tmpB5;_push_handler(& _tmpB5);{int _tmpB7=0;if(setjmp(_tmpB5.handler))_tmpB7=1;if(!_tmpB7){
{struct _tuple14*_tmpB8=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple14*ans=_tmpB8;
void*_tmpB9=(*ans).f1;void*b0=_tmpB9;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBA=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=2U,_tmpBD->f1=fd;_tmpBD;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1=_tmpBA;
void*_tmpBB=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmpBB;
if(b != 0){
# 324
if(exports == 0 || Cyc_Tc_export_member(q,*exports)){
if(!(b == b0 &&(*ans).f2))
({struct Cyc_Dict_Dict _tmp2A1=({struct Cyc_Dict_Dict _tmp2A0=(te->ae)->ordinaries;struct _tuple0*_tmp29F=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp2A0,_tmp29F,({struct _tuple14*_tmpBC=_cycalloc(sizeof(*_tmpBC));
_tmpBC->f1=b,_tmpBC->f2=(*ans).f2;_tmpBC;}));});
# 326
(te->ae)->ordinaries=_tmp2A1;});}}}
# 318
;_pop_handler();}else{void*_tmpB6=(void*)Cyc_Core_get_exn_thrown();void*_tmpBE=_tmpB6;void*_tmpBF;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBE)->tag == Cyc_Dict_Absent){_LL8: _LL9:
# 333
 if(exports == 0 || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC0=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=2U,_tmpC3->f1=fd;_tmpC3;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b=_tmpC0;
({struct Cyc_Dict_Dict _tmp2A5=({struct Cyc_Dict_Dict _tmp2A4=(te->ae)->ordinaries;struct _tuple0*_tmp2A3=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp2A4,_tmp2A3,({struct _tuple14*_tmpC2=_cycalloc(sizeof(*_tmpC2));
({void*_tmp2A2=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->tag=2U,_tmpC1->f1=fd;_tmpC1;});_tmpC2->f1=_tmp2A2;}),_tmpC2->f2=0;_tmpC2;}));});
# 335
(te->ae)->ordinaries=_tmp2A5;});}
# 338
goto _LL7;}else{_LLA: _tmpBF=_tmpBE;_LLB: {void*exn=_tmpBF;(int)_rethrow(exn);}}_LL7:;}}}
# 342
if(te->in_extern_c_include)return;{
# 347
struct Cyc_Tcenv_Fenv*_tmpC4=Cyc_Tcenv_new_fenv(loc,fd);struct Cyc_Tcenv_Fenv*fenv=_tmpC4;
struct Cyc_Tcenv_Tenv*_tmpC5=({struct Cyc_Tcenv_Tenv*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->ns=te->ns,_tmpE9->ae=te->ae,_tmpE9->le=fenv,_tmpE9->allow_valueof=0,_tmpE9->in_extern_c_include=0,_tmpE9->in_tempest=te->in_tempest,_tmpE9->tempest_generalize=te->tempest_generalize,_tmpE9->in_extern_c_inc_repeat=0;_tmpE9;});struct Cyc_Tcenv_Tenv*te=_tmpC5;
# 350
Cyc_Tcstmt_tcStmt(te,fd->body,0);
# 353
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
# 356
if(te->in_tempest){
te->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);
te->tempest_generalize=0;}
# 362
if(Cyc_Tc_is_main(q)){
# 364
{void*_tmpC6=Cyc_Tcutil_compress((fd->i).ret_type);void*_stmttmpB=_tmpC6;void*_tmpC7=_stmttmpB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC7)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC7)->f1)){case 0U: _LLD: _LLE:
({void*_tmpC8=0U;({unsigned _tmp2A7=loc;struct _fat_ptr _tmp2A6=({const char*_tmpC9="main declared with return type void";_tag_fat(_tmpC9,sizeof(char),36U);});Cyc_Tcutil_warn(_tmp2A7,_tmp2A6,_tag_fat(_tmpC8,sizeof(void*),0U));});});goto _LLC;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpC7)->f1)->f2){case Cyc_Absyn_Int_sz: _LLF: _LL10:
 goto _LLC;case Cyc_Absyn_Long_sz: _LL11: _LL12:
 goto _LLC;default: goto _LL13;}default: goto _LL13;}else{_LL13: _LL14:
# 369
({struct Cyc_String_pa_PrintArg_struct _tmpCC=({struct Cyc_String_pa_PrintArg_struct _tmp224;_tmp224.tag=0U,({
struct _fat_ptr _tmp2A8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((fd->i).ret_type));_tmp224.f1=_tmp2A8;});_tmp224;});void*_tmpCA[1U];_tmpCA[0]=& _tmpCC;({unsigned _tmp2AA=loc;struct _fat_ptr _tmp2A9=({const char*_tmpCB="main declared with return type %s instead of int or void";_tag_fat(_tmpCB,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2AA,_tmp2A9,_tag_fat(_tmpCA,sizeof(void*),1U));});});
goto _LLC;}_LLC:;}
# 373
if((fd->i).c_varargs ||(fd->i).cyc_varargs != 0)
({void*_tmpCD=0U;({unsigned _tmp2AC=loc;struct _fat_ptr _tmp2AB=({const char*_tmpCE="main declared with varargs";_tag_fat(_tmpCE,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp2AC,_tmp2AB,_tag_fat(_tmpCD,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpCF=(fd->i).args;struct Cyc_List_List*args=_tmpCF;
if(args != 0){
struct _tuple8*_tmpD0=(struct _tuple8*)args->hd;struct _tuple8*_stmttmpC=_tmpD0;struct _tuple8*_tmpD1=_stmttmpC;void*_tmpD2;_LL16: _tmpD2=_tmpD1->f3;_LL17: {void*t1=_tmpD2;
{void*_tmpD3=Cyc_Tcutil_compress(t1);void*_stmttmpD=_tmpD3;void*_tmpD4=_stmttmpD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD4)->f1)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD4)->f1)->f2){case Cyc_Absyn_Int_sz: _LL19: _LL1A:
 goto _LL18;case Cyc_Absyn_Long_sz: _LL1B: _LL1C:
 goto _LL18;default: goto _LL1D;}else{goto _LL1D;}}else{_LL1D: _LL1E:
# 382
({struct Cyc_String_pa_PrintArg_struct _tmpD7=({struct Cyc_String_pa_PrintArg_struct _tmp225;_tmp225.tag=0U,({
struct _fat_ptr _tmp2AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp225.f1=_tmp2AD;});_tmp225;});void*_tmpD5[1U];_tmpD5[0]=& _tmpD7;({unsigned _tmp2AF=loc;struct _fat_ptr _tmp2AE=({const char*_tmpD6="main declared with first argument of type %s instead of int";_tag_fat(_tmpD6,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp2AF,_tmp2AE,_tag_fat(_tmpD5,sizeof(void*),1U));});});
goto _LL18;}_LL18:;}
# 386
args=args->tl;
if(args != 0){
struct _tuple8*_tmpD8=(struct _tuple8*)args->hd;struct _tuple8*_stmttmpE=_tmpD8;struct _tuple8*_tmpD9=_stmttmpE;void*_tmpDA;_LL20: _tmpDA=_tmpD9->f3;_LL21: {void*t2=_tmpDA;
args=args->tl;
if(args != 0)
({void*_tmpDB=0U;({unsigned _tmp2B1=loc;struct _fat_ptr _tmp2B0=({const char*_tmpDC="main declared with too many arguments";_tag_fat(_tmpDC,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp2B1,_tmp2B0,_tag_fat(_tmpDB,sizeof(void*),0U));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->v=(fd->i).tvars;_tmpE8;});
if(((!({void*_tmp2C9=t2;Cyc_Unify_unify(_tmp2C9,({void*_tmp2C8=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp2C4=({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->v=& Cyc_Tcutil_rk;_tmpDD;});Cyc_Absyn_new_evar(_tmp2C4,tvs);}));void*_tmp2C7=({
struct Cyc_Core_Opt*_tmp2C5=({struct Cyc_Core_Opt*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->v=& Cyc_Tcutil_rk;_tmpDE;});Cyc_Absyn_new_evar(_tmp2C5,tvs);});
# 393
struct Cyc_Absyn_Tqual _tmp2C6=
# 395
Cyc_Absyn_empty_tqual(0U);
# 393
Cyc_Absyn_fatptr_type(_tmp2C8,_tmp2C7,_tmp2C6,
# 395
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));})&& !({
void*_tmp2C3=t2;Cyc_Unify_unify(_tmp2C3,({void*_tmp2C2=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp2BE=({struct Cyc_Core_Opt*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->v=& Cyc_Tcutil_rk;_tmpDF;});Cyc_Absyn_new_evar(_tmp2BE,tvs);}));void*_tmp2C1=({
# 398
struct Cyc_Core_Opt*_tmp2BF=({struct Cyc_Core_Opt*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->v=& Cyc_Tcutil_rk;_tmpE0;});Cyc_Absyn_new_evar(_tmp2BF,tvs);});
# 396
struct Cyc_Absyn_Tqual _tmp2C0=
# 399
Cyc_Absyn_empty_tqual(0U);
# 396
Cyc_Absyn_fatptr_type(_tmp2C2,_tmp2C1,_tmp2C0,
# 399
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp2BD=t2;Cyc_Unify_unify(_tmp2BD,({void*_tmp2BC=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp2B8=({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=& Cyc_Tcutil_rk;_tmpE1;});Cyc_Absyn_new_evar(_tmp2B8,tvs);}));void*_tmp2BB=({
struct Cyc_Core_Opt*_tmp2B9=({struct Cyc_Core_Opt*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->v=& Cyc_Tcutil_rk;_tmpE2;});Cyc_Absyn_new_evar(_tmp2B9,tvs);});
# 400
struct Cyc_Absyn_Tqual _tmp2BA=
# 402
Cyc_Absyn_const_tqual(0U);
# 400
Cyc_Absyn_fatptr_type(_tmp2BC,_tmp2BB,_tmp2BA,
# 402
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))&& !({
void*_tmp2B7=t2;Cyc_Unify_unify(_tmp2B7,({void*_tmp2B6=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp2B2=({struct Cyc_Core_Opt*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->v=& Cyc_Tcutil_rk;_tmpE3;});Cyc_Absyn_new_evar(_tmp2B2,tvs);}));void*_tmp2B5=({
# 405
struct Cyc_Core_Opt*_tmp2B3=({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->v=& Cyc_Tcutil_rk;_tmpE4;});Cyc_Absyn_new_evar(_tmp2B3,tvs);});
# 403
struct Cyc_Absyn_Tqual _tmp2B4=
# 406
Cyc_Absyn_const_tqual(0U);
# 403
Cyc_Absyn_fatptr_type(_tmp2B6,_tmp2B5,_tmp2B4,
# 406
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v));}));}))
({struct Cyc_String_pa_PrintArg_struct _tmpE7=({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0U,({
struct _fat_ptr _tmp2CA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp226.f1=_tmp2CA;});_tmp226;});void*_tmpE5[1U];_tmpE5[0]=& _tmpE7;({unsigned _tmp2CC=loc;struct _fat_ptr _tmp2CB=({const char*_tmpE6="second argument of main has type %s instead of char??";_tag_fat(_tmpE6,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp2CC,_tmp2CB,_tag_fat(_tmpE5,sizeof(void*),1U));});});}}}}}}}}}}
# 415
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 422
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpEC=({struct Cyc_String_pa_PrintArg_struct _tmp227;_tmp227.tag=0U,_tmp227.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp227;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEC;({unsigned _tmp2CE=loc;struct _fat_ptr _tmp2CD=({const char*_tmpEB="redeclaration of typedef %s";_tag_fat(_tmpEB,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp2CE,_tmp2CD,_tag_fat(_tmpEA,sizeof(void*),1U));});});
return;}
# 427
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tctyp_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp2CF=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 431
(td->tq).real_const=_tmp2CF;});}
# 436
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpED=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_stmttmpF=_tmpED;void*_tmpEE=_stmttmpF;struct Cyc_Absyn_Kind*_tmpF0;struct Cyc_Core_Opt**_tmpEF;struct Cyc_Core_Opt**_tmpF1;switch(*((int*)_tmpEE)){case 1U: _LL1: _tmpF1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpEE)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpF1;
# 439
if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpF4=({struct Cyc_String_pa_PrintArg_struct _tmp228;_tmp228.tag=0U,_tmp228.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);_tmp228;});void*_tmpF2[1U];_tmpF2[0]=& _tmpF4;({unsigned _tmp2D1=loc;struct _fat_ptr _tmp2D0=({const char*_tmpF3="type variable %s is not used in typedef definition";_tag_fat(_tmpF3,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp2D1,_tmp2D0,_tag_fat(_tmpF2,sizeof(void*),1U));});});
# 442
({struct Cyc_Core_Opt*_tmp2D3=({struct Cyc_Core_Opt*_tmpF5=_cycalloc(sizeof(*_tmpF5));({void*_tmp2D2=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpF5->v=_tmp2D2;});_tmpF5;});*f=_tmp2D3;});goto _LL0;}case 2U: _LL3: _tmpEF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEE)->f1;_tmpF0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEE)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmpEF;struct Cyc_Absyn_Kind*k=_tmpF0;
# 449
({struct Cyc_Core_Opt*_tmp2D5=({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(*_tmpF6));({void*_tmp2D4=Cyc_Tcutil_kind_to_bound(k);_tmpF6->v=_tmp2D4;});_tmpF6;});*f=_tmp2D5;});
goto _LL0;}default: _LL5: _LL6:
 continue;}_LL0:;}}
# 455
({struct Cyc_Dict_Dict _tmp2D6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp2D6;});}struct _tuple15{void*f1;void*f2;};
# 458
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 465
struct _RegionHandle _tmpF7=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpF7;_push_region(uprev_rgn);
# 467
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple15*_tmpF8=(struct _tuple15*)rpo->hd;struct _tuple15*_stmttmp10=_tmpF8;struct _tuple15*_tmpF9=_stmttmp10;void*_tmpFB;void*_tmpFA;_LL1: _tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;_LL2: {void*e=_tmpFA;void*r=_tmpFB;
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,e);
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,r);}}{
# 473
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 476
struct Cyc_List_List*_tmpFC=fields;struct Cyc_List_List*fs=_tmpFC;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpFD=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp11=_tmpFD;struct Cyc_Absyn_Aggrfield*_tmpFE=_stmttmp11;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp102;void*_tmp101;struct Cyc_Absyn_Tqual _tmp100;struct _fat_ptr*_tmpFF;_LL4: _tmpFF=_tmpFE->name;_tmp100=_tmpFE->tq;_tmp101=_tmpFE->type;_tmp102=_tmpFE->width;_tmp103=_tmpFE->attributes;_tmp104=_tmpFE->requires_clause;_LL5: {struct _fat_ptr*fn=_tmpFF;struct Cyc_Absyn_Tqual tq=_tmp100;void*t=_tmp101;struct Cyc_Absyn_Exp*width=_tmp102;struct Cyc_List_List*atts=_tmp103;struct Cyc_Absyn_Exp*requires_clause=_tmp104;
# 479
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_String_pa_PrintArg_struct _tmp107=({struct Cyc_String_pa_PrintArg_struct _tmp229;_tmp229.tag=0U,_tmp229.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp229;});void*_tmp105[1U];_tmp105[0]=& _tmp107;({unsigned _tmp2D8=loc;struct _fat_ptr _tmp2D7=({const char*_tmp106="duplicate member %s";_tag_fat(_tmp106,sizeof(char),20U);});Cyc_Tcutil_terr(_tmp2D8,_tmp2D7,_tag_fat(_tmp105,sizeof(void*),1U));});});
# 483
if(({struct _fat_ptr _tmp2D9=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp2D9,({const char*_tmp108="";_tag_fat(_tmp108,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp109=_region_malloc(uprev_rgn,sizeof(*_tmp109));_tmp109->hd=fn,_tmp109->tl=prev_fields;_tmp109;});{
# 486
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 490
if((int)str_or_union == (int)1U ||
 fs->tl == 0 &&(int)str_or_union == (int)0U)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tctyp_check_type(loc,te,tvs,field_kind,0,0,t);
# 495
({int _tmp2DA=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).print_const,t);(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).real_const=_tmp2DA;});
# 498
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
# 500
if((unsigned)requires_clause){
if((int)str_or_union != (int)1U)
({void*_tmp10A=0U;({unsigned _tmp2DC=loc;struct _fat_ptr _tmp2DB=({const char*_tmp10B="@requires clauses are only allowed on union members";_tag_fat(_tmp10B,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2DC,_tmp2DB,_tag_fat(_tmp10A,sizeof(void*),0U));});});{
struct Cyc_Tcenv_Tenv*_tmp10C=Cyc_Tcenv_allow_valueof(te);struct Cyc_Tcenv_Tenv*te2=_tmp10C;
Cyc_Tcexp_tcExp(te2,0,requires_clause);
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_String_pa_PrintArg_struct _tmp10F=({struct Cyc_String_pa_PrintArg_struct _tmp22A;_tmp22A.tag=0U,({
# 508
struct _fat_ptr _tmp2DD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(requires_clause->topt)));_tmp22A.f1=_tmp2DD;});_tmp22A;});void*_tmp10D[1U];_tmp10D[0]=& _tmp10F;({unsigned _tmp2DF=requires_clause->loc;struct _fat_ptr _tmp2DE=({const char*_tmp10E="@requires clause has type %s instead of integral type";_tag_fat(_tmp10E,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp2DF,_tmp2DE,_tag_fat(_tmp10D,sizeof(void*),1U));});});else{
# 510
({unsigned _tmp2E2=requires_clause->loc;struct Cyc_Tcenv_Tenv*_tmp2E1=te;struct Cyc_List_List*_tmp2E0=tvs;Cyc_Tctyp_check_type(_tmp2E2,_tmp2E1,_tmp2E0,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->tag=9U,_tmp110->f1=requires_clause;_tmp110;}));});{
# 512
struct Cyc_List_List*_tmp111=Cyc_Relations_exp2relns(uprev_rgn,requires_clause);struct Cyc_List_List*relns=_tmp111;
# 519
if(!Cyc_Relations_consistent_relations(relns))
({void*_tmp112=0U;({unsigned _tmp2E4=requires_clause->loc;struct _fat_ptr _tmp2E3=({const char*_tmp113="@requires clause may be unsatisfiable";_tag_fat(_tmp113,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp2E4,_tmp2E3,_tag_fat(_tmp112,sizeof(void*),0U));});});
# 525
{struct Cyc_List_List*_tmp114=prev_relations;struct Cyc_List_List*p=_tmp114;for(0;p != 0;p=p->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,relns,(struct Cyc_List_List*)p->hd)))
({void*_tmp115=0U;({unsigned _tmp2E6=requires_clause->loc;struct _fat_ptr _tmp2E5=({const char*_tmp116="@requires clause may overlap with previous clauses";_tag_fat(_tmp116,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2E6,_tmp2E5,_tag_fat(_tmp115,sizeof(void*),0U));});});}}
# 530
prev_relations=({struct Cyc_List_List*_tmp117=_region_malloc(uprev_rgn,sizeof(*_tmp117));_tmp117->hd=relns,_tmp117->tl=prev_relations;_tmp117;});}}}}else{
# 533
if(prev_relations != 0)
({void*_tmp118=0U;({unsigned _tmp2E8=loc;struct _fat_ptr _tmp2E7=({const char*_tmp119="if one field has a @requires clause, they all must";_tag_fat(_tmp119,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2E8,_tmp2E7,_tag_fat(_tmp118,sizeof(void*),0U));});});}}}}}
# 467
;_pop_region();}
# 539
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 541
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp11A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmp12=_tmp11A;void*_tmp11B=_stmttmp12;enum Cyc_Absyn_AliasQual _tmp11C;struct Cyc_Absyn_Kind*_tmp11E;struct Cyc_Core_Opt**_tmp11D;struct Cyc_Core_Opt**_tmp11F;enum Cyc_Absyn_AliasQual _tmp121;struct Cyc_Core_Opt**_tmp120;struct Cyc_Core_Opt**_tmp122;switch(*((int*)_tmp11B)){case 1U: _LL1: _tmp122=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11B)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp122;
# 544
({struct Cyc_Core_Opt*_tmp2EA=({struct Cyc_Core_Opt*_tmp123=_cycalloc(sizeof(*_tmp123));({void*_tmp2E9=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp123->v=_tmp2E9;});_tmp123;});*f=_tmp2EA;});continue;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11B)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmp120=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11B)->f1;_tmp121=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11B)->f2)->aliasqual;_LL4: {struct Cyc_Core_Opt**f=_tmp120;enum Cyc_Absyn_AliasQual a=_tmp121;
# 546
if(constrain_top_kind &&(int)a == (int)2U)
({struct Cyc_Core_Opt*_tmp2EC=({struct Cyc_Core_Opt*_tmp125=_cycalloc(sizeof(*_tmp125));({void*_tmp2EB=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->kind=Cyc_Absyn_BoxKind,_tmp124->aliasqual=Cyc_Absyn_Aliasable;_tmp124;}));_tmp125->v=_tmp2EB;});_tmp125;});*f=_tmp2EC;});else{
# 549
({struct Cyc_Core_Opt*_tmp2EE=({struct Cyc_Core_Opt*_tmp127=_cycalloc(sizeof(*_tmp127));({void*_tmp2ED=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->kind=Cyc_Absyn_BoxKind,_tmp126->aliasqual=a;_tmp126;}));_tmp127->v=_tmp2ED;});_tmp127;});*f=_tmp2EE;});}
continue;}case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11B)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmp11F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11B)->f1;if(constrain_top_kind){_LL6: {struct Cyc_Core_Opt**f=_tmp11F;
# 552
({struct Cyc_Core_Opt*_tmp2F0=({struct Cyc_Core_Opt*_tmp129=_cycalloc(sizeof(*_tmp129));({void*_tmp2EF=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->kind=Cyc_Absyn_BoxKind,_tmp128->aliasqual=Cyc_Absyn_Aliasable;_tmp128;}));_tmp129->v=_tmp2EF;});_tmp129;});*f=_tmp2F0;});
continue;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmp11D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11B)->f1;_tmp11E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11B)->f2;_LL8: {struct Cyc_Core_Opt**f=_tmp11D;struct Cyc_Absyn_Kind*k=_tmp11E;
# 555
({struct Cyc_Core_Opt*_tmp2F2=({struct Cyc_Core_Opt*_tmp12A=_cycalloc(sizeof(*_tmp12A));({void*_tmp2F1=Cyc_Tcutil_kind_to_bound(k);_tmp12A->v=_tmp2F1;});_tmp12A;});*f=_tmp2F2;});continue;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11B)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmp11C=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11B)->f1)->aliasqual;_LLA: {enum Cyc_Absyn_AliasQual a=_tmp11C;
# 557
({struct Cyc_String_pa_PrintArg_struct _tmp12D=({struct Cyc_String_pa_PrintArg_struct _tmp22D;_tmp22D.tag=0U,_tmp22D.f1=(struct _fat_ptr)((struct _fat_ptr)*(*n).f2);_tmp22D;});struct Cyc_String_pa_PrintArg_struct _tmp12E=({struct Cyc_String_pa_PrintArg_struct _tmp22C;_tmp22C.tag=0U,_tmp22C.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp22C;});struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp22B;_tmp22B.tag=0U,({
struct _fat_ptr _tmp2F3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->kind=Cyc_Absyn_MemKind,_tmp130->aliasqual=a;_tmp130;})));_tmp22B.f1=_tmp2F3;});_tmp22B;});void*_tmp12B[3U];_tmp12B[0]=& _tmp12D,_tmp12B[1]=& _tmp12E,_tmp12B[2]=& _tmp12F;({unsigned _tmp2F5=loc;struct _fat_ptr _tmp2F4=({const char*_tmp12C="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp12C,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2F5,_tmp2F4,_tag_fat(_tmp12B,sizeof(void*),3U));});});
continue;}}else{_LLB: _LLC:
 continue;}}_LL0:;}}struct _tuple16{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 565
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp131=ad->name;struct _tuple0*q=_tmp131;
# 572
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp132=(void*)atts->hd;void*_stmttmp13=_tmp132;void*_tmp133=_stmttmp13;switch(*((int*)_tmp133)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
# 577
({struct Cyc_Warn_String_Warn_Warg_struct _tmp135=({struct Cyc_Warn_String_Warn_Warg_struct _tmp232;_tmp232.tag=0U,({struct _fat_ptr _tmp2F6=({const char*_tmp13C="bad attribute ";_tag_fat(_tmp13C,sizeof(char),15U);});_tmp232.f1=_tmp2F6;});_tmp232;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp136=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp231;_tmp231.tag=10U,_tmp231.f1=(void*)atts->hd;_tmp231;});struct Cyc_Warn_String_Warn_Warg_struct _tmp137=({struct Cyc_Warn_String_Warn_Warg_struct _tmp230;_tmp230.tag=0U,({struct _fat_ptr _tmp2F7=({const char*_tmp13B=" in ";_tag_fat(_tmp13B,sizeof(char),5U);});_tmp230.f1=_tmp2F7;});_tmp230;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp138=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp22F;_tmp22F.tag=1U,_tmp22F.f1=q;_tmp22F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp139=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22E;_tmp22E.tag=0U,({struct _fat_ptr _tmp2F8=({const char*_tmp13A=" definition";_tag_fat(_tmp13A,sizeof(char),12U);});_tmp22E.f1=_tmp2F8;});_tmp22E;});void*_tmp134[5U];_tmp134[0]=& _tmp135,_tmp134[1]=& _tmp136,_tmp134[2]=& _tmp137,_tmp134[3]=& _tmp138,_tmp134[4]=& _tmp139;({unsigned _tmp2F9=loc;Cyc_Warn_err2(_tmp2F9,_tag_fat(_tmp134,sizeof(void*),5U));});});
goto _LL0;}_LL0:;}}{
# 581
struct Cyc_List_List*_tmp13D=ad->tvs;struct Cyc_List_List*tvs=_tmp13D;
# 584
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 589
struct _tuple16 _tmp13E=({struct _tuple16 _tmp236;_tmp236.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp2FA=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,q);_tmp236.f2=_tmp2FA;});_tmp236;});struct _tuple16 _stmttmp14=_tmp13E;struct _tuple16 _tmp13F=_stmttmp14;struct Cyc_Absyn_Aggrdecl**_tmp144;int _tmp143;struct Cyc_List_List*_tmp142;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp140;int _tmp148;struct Cyc_List_List*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp145;struct Cyc_Absyn_Aggrdecl**_tmp149;if(_tmp13F.f1 == 0){if(_tmp13F.f2 == 0){_LL8: _LL9:
# 592
 Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 594
({struct Cyc_Dict_Dict _tmp2FD=({struct Cyc_Dict_Dict _tmp2FC=(te->ae)->aggrdecls;struct _tuple0*_tmp2FB=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp2FC,_tmp2FB,({struct Cyc_Absyn_Aggrdecl**_tmp14A=_cycalloc(sizeof(*_tmp14A));*_tmp14A=ad;_tmp14A;}));});(te->ae)->aggrdecls=_tmp2FD;});
goto _LL7;}else{_LLE: _tmp149=*_tmp13F.f2;_LLF: {struct Cyc_Absyn_Aggrdecl**adp=_tmp149;
# 654
struct Cyc_Absyn_Aggrdecl*_tmp15B=Cyc_Tcdecl_merge_aggrdecl(*adp,ad,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Aggrdecl*ad2=_tmp15B;
if(ad2 == 0)
return;else{
# 658
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 661
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,q,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 664
*adp=ad2;
ad=ad2;
goto _LL7;}}}}else{if(_tmp13F.f2 == 0){_LLA: _tmp145=(_tmp13F.f1)->exist_vars;_tmp146=(_tmp13F.f1)->rgn_po;_tmp147=(_tmp13F.f1)->fields;_tmp148=(_tmp13F.f1)->tagged;_LLB: {struct Cyc_List_List*exist_vars=_tmp145;struct Cyc_List_List*rgn_po=_tmp146;struct Cyc_List_List*fs=_tmp147;int tagged=_tmp148;
# 599
struct Cyc_Absyn_Aggrdecl**_tmp14B=({struct Cyc_Absyn_Aggrdecl**_tmp154=_cycalloc(sizeof(*_tmp154));({struct Cyc_Absyn_Aggrdecl*_tmp2FE=({struct Cyc_Absyn_Aggrdecl*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->kind=ad->kind,_tmp153->sc=Cyc_Absyn_Extern,_tmp153->name=ad->name,_tmp153->tvs=tvs,_tmp153->impl=0,_tmp153->attributes=ad->attributes,_tmp153->expected_mem_kind=0;_tmp153;});*_tmp154=_tmp2FE;});_tmp154;});struct Cyc_Absyn_Aggrdecl**adp=_tmp14B;
# 601
({struct Cyc_Dict_Dict _tmp2FF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,q,adp);(te->ae)->aggrdecls=_tmp2FF;});
# 606
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 610
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp14C=0U;({unsigned _tmp301=loc;struct _fat_ptr _tmp300=({const char*_tmp14D="@tagged qualifier is only allowed on union declarations";_tag_fat(_tmp14D,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp301,_tmp300,_tag_fat(_tmp14C,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp306=te;unsigned _tmp305=loc;enum Cyc_Absyn_AggrKind _tmp304=ad->kind;struct Cyc_List_List*_tmp303=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp302=rgn_po;Cyc_Tc_tcAggrImpl(_tmp306,_tmp305,_tmp304,_tmp303,_tmp302,fs);});
# 614
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 617
Cyc_Tc_rule_out_memkind(loc,q,exist_vars,1);
# 619
if((int)ad->kind == (int)Cyc_Absyn_UnionA && !tagged){
# 622
struct Cyc_List_List*f=fs;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp150=({struct Cyc_String_pa_PrintArg_struct _tmp235;_tmp235.tag=0U,_tmp235.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);_tmp235;});struct Cyc_String_pa_PrintArg_struct _tmp151=({struct Cyc_String_pa_PrintArg_struct _tmp234;_tmp234.tag=0U,_tmp234.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp234;});struct Cyc_String_pa_PrintArg_struct _tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp233;_tmp233.tag=0U,({
struct _fat_ptr _tmp307=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));_tmp233.f1=_tmp307;});_tmp233;});void*_tmp14E[3U];_tmp14E[0]=& _tmp150,_tmp14E[1]=& _tmp151,_tmp14E[2]=& _tmp152;({unsigned _tmp309=loc;struct _fat_ptr _tmp308=({const char*_tmp14F="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_fat(_tmp14F,sizeof(char),98U);});Cyc_Tcutil_warn(_tmp309,_tmp308,_tag_fat(_tmp14E,sizeof(void*),3U));});});}}
# 628
*adp=ad;
goto _LL7;}}else{_LLC: _tmp140=(_tmp13F.f1)->exist_vars;_tmp141=(_tmp13F.f1)->rgn_po;_tmp142=(_tmp13F.f1)->fields;_tmp143=(_tmp13F.f1)->tagged;_tmp144=*_tmp13F.f2;_LLD: {struct Cyc_List_List*exist_vars=_tmp140;struct Cyc_List_List*rgn_po=_tmp141;struct Cyc_List_List*fs=_tmp142;int tagged=_tmp143;struct Cyc_Absyn_Aggrdecl**adp=_tmp144;
# 632
if((int)ad->kind != (int)(*adp)->kind)
({void*_tmp155=0U;({unsigned _tmp30B=loc;struct _fat_ptr _tmp30A=({const char*_tmp156="cannot reuse struct names for unions and vice-versa";_tag_fat(_tmp156,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp30B,_tmp30A,_tag_fat(_tmp155,sizeof(void*),0U));});});{
# 635
struct Cyc_Absyn_Aggrdecl*_tmp157=*adp;struct Cyc_Absyn_Aggrdecl*ad0=_tmp157;
# 637
({struct Cyc_Absyn_Aggrdecl*_tmp30C=({struct Cyc_Absyn_Aggrdecl*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->kind=ad->kind,_tmp158->sc=Cyc_Absyn_Extern,_tmp158->name=ad->name,_tmp158->tvs=tvs,_tmp158->impl=0,_tmp158->attributes=ad->attributes,_tmp158->expected_mem_kind=0;_tmp158;});*adp=_tmp30C;});
# 643
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 646
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp159=0U;({unsigned _tmp30E=loc;struct _fat_ptr _tmp30D=({const char*_tmp15A="@tagged qualifier is only allowed on union declarations";_tag_fat(_tmp15A,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp30E,_tmp30D,_tag_fat(_tmp159,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp313=te;unsigned _tmp312=loc;enum Cyc_Absyn_AggrKind _tmp311=ad->kind;struct Cyc_List_List*_tmp310=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp30F=rgn_po;Cyc_Tc_tcAggrImpl(_tmp313,_tmp312,_tmp311,_tmp310,_tmp30F,fs);});
# 650
*adp=ad0;
_tmp149=adp;goto _LLF;}}}}_LL7:;}}}
# 671
static void Cyc_Tc_rule_out_mem_and_unique(unsigned loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp15C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmp15=_tmp15C;void*_tmp15D=_stmttmp15;enum Cyc_Absyn_AliasQual _tmp15E;enum Cyc_Absyn_KindQual _tmp15F;enum Cyc_Absyn_KindQual _tmp161;struct Cyc_Core_Opt**_tmp160;struct Cyc_Core_Opt**_tmp162;struct Cyc_Core_Opt**_tmp163;struct Cyc_Core_Opt**_tmp164;struct Cyc_Core_Opt**_tmp165;struct Cyc_Core_Opt**_tmp166;struct Cyc_Core_Opt**_tmp167;switch(*((int*)_tmp15D)){case 1U: _LL1: _tmp167=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15D)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp167;
_tmp166=f;goto _LL4;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmp166=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp166;
# 676
_tmp165=f;goto _LL6;}case Cyc_Absyn_Aliasable: _LL5: _tmp165=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp165;
# 678
({struct Cyc_Core_Opt*_tmp315=({struct Cyc_Core_Opt*_tmp168=_cycalloc(sizeof(*_tmp168));({void*_tmp314=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp168->v=_tmp314;});_tmp168;});*f=_tmp315;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmp164=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp164;
_tmp163=f;goto _LLA;}case Cyc_Absyn_Aliasable: _LL9: _tmp163=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f1;_LLA: {struct Cyc_Core_Opt**f=_tmp163;
# 681
({struct Cyc_Core_Opt*_tmp317=({struct Cyc_Core_Opt*_tmp169=_cycalloc(sizeof(*_tmp169));({void*_tmp316=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp169->v=_tmp316;});_tmp169;});*f=_tmp317;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmp162=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f1;_LLC: {struct Cyc_Core_Opt**f=_tmp162;
# 683
({struct Cyc_Core_Opt*_tmp319=({struct Cyc_Core_Opt*_tmp16A=_cycalloc(sizeof(*_tmp16A));({void*_tmp318=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp16A->v=_tmp318;});_tmp16A;});*f=_tmp319;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmp160=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f1;_tmp161=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15D)->f2)->kind;_LL10: {struct Cyc_Core_Opt**f=_tmp160;enum Cyc_Absyn_KindQual k=_tmp161;
# 687
_tmp15F=k;goto _LL12;}}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15D)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15D)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 685
({struct Cyc_String_pa_PrintArg_struct _tmp16D=({struct Cyc_String_pa_PrintArg_struct _tmp238;_tmp238.tag=0U,_tmp238.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp238;});struct Cyc_String_pa_PrintArg_struct _tmp16E=({struct Cyc_String_pa_PrintArg_struct _tmp237;_tmp237.tag=0U,_tmp237.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp237;});void*_tmp16B[2U];_tmp16B[0]=& _tmp16D,_tmp16B[1]=& _tmp16E;({unsigned _tmp31B=loc;struct _fat_ptr _tmp31A=({const char*_tmp16C="type %s attempts to abstract type variable %s of kind TR";_tag_fat(_tmp16C,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp31B,_tmp31A,_tag_fat(_tmp16B,sizeof(void*),2U));});});
goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15D)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmp15F=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15D)->f1)->kind;_LL12: {enum Cyc_Absyn_KindQual k=_tmp15F;
# 689
({struct Cyc_String_pa_PrintArg_struct _tmp171=({struct Cyc_String_pa_PrintArg_struct _tmp23B;_tmp23B.tag=0U,_tmp23B.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp23B;});struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp23A;_tmp23A.tag=0U,_tmp23A.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp23A;});struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp239;_tmp239.tag=0U,({
# 691
struct _fat_ptr _tmp31C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->kind=k,_tmp174->aliasqual=Cyc_Absyn_Unique;_tmp174;})));_tmp239.f1=_tmp31C;});_tmp239;});void*_tmp16F[3U];_tmp16F[0]=& _tmp171,_tmp16F[1]=& _tmp172,_tmp16F[2]=& _tmp173;({unsigned _tmp31E=loc;struct _fat_ptr _tmp31D=({const char*_tmp170="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp170,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp31E,_tmp31D,_tag_fat(_tmp16F,sizeof(void*),3U));});});goto _LL0;}}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15D)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmp15E=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15D)->f1)->aliasqual;_LL14: {enum Cyc_Absyn_AliasQual a=_tmp15E;
# 693
({struct Cyc_String_pa_PrintArg_struct _tmp177=({struct Cyc_String_pa_PrintArg_struct _tmp23E;_tmp23E.tag=0U,_tmp23E.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp23E;});struct Cyc_String_pa_PrintArg_struct _tmp178=({struct Cyc_String_pa_PrintArg_struct _tmp23D;_tmp23D.tag=0U,_tmp23D.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp23D;});struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp23C;_tmp23C.tag=0U,({
# 695
struct _fat_ptr _tmp31F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->kind=Cyc_Absyn_MemKind,_tmp17A->aliasqual=a;_tmp17A;})));_tmp23C.f1=_tmp31F;});_tmp23C;});void*_tmp175[3U];_tmp175[0]=& _tmp177,_tmp175[1]=& _tmp178,_tmp175[2]=& _tmp179;({unsigned _tmp321=loc;struct _fat_ptr _tmp320=({const char*_tmp176="type %s attempts to abstract type variable %s of kind %s";_tag_fat(_tmp176,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp321,_tmp320,_tag_fat(_tmp175,sizeof(void*),3U));});});goto _LL0;}}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 701
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 709
{struct Cyc_List_List*_tmp17B=fields;struct Cyc_List_List*fs=_tmp17B;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp17C=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp17C;
struct Cyc_List_List*typs=f->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tctyp_check_type(f->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple17*)typs->hd)).f2);
# 714
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple17*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp23F;_tmp23F.tag=0U,({
# 717
struct _fat_ptr _tmp322=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name));_tmp23F.f1=_tmp322;});_tmp23F;});void*_tmp17D[1U];_tmp17D[0]=& _tmp17F;({unsigned _tmp324=f->loc;struct _fat_ptr _tmp323=({const char*_tmp17E="noalias pointers in datatypes are not allowed (%s)";_tag_fat(_tmp17E,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp324,_tmp323,_tag_fat(_tmp17D,sizeof(void*),1U));});});
({int _tmp325=
Cyc_Tcutil_extract_const_from_typedef(f->loc,((*((struct _tuple17*)typs->hd)).f1).print_const,(*((struct _tuple17*)typs->hd)).f2);
# 718
((*((struct _tuple17*)typs->hd)).f1).real_const=_tmp325;});}}}{
# 724
struct Cyc_List_List*fields2;
if(is_extensible){
# 727
int _tmp180=1;int res=_tmp180;
struct Cyc_List_List*_tmp181=Cyc_Tcdecl_sort_xdatatype_fields(fields,& res,(*name).f2,loc,Cyc_Tc_tc_msg);struct Cyc_List_List*fs=_tmp181;
if(res)
fields2=fs;else{
# 732
fields2=0;}}else{
# 734
struct _RegionHandle _tmp182=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp182;_push_region(uprev_rgn);
# 736
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp183=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp183;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp186=({struct Cyc_String_pa_PrintArg_struct _tmp241;_tmp241.tag=0U,_tmp241.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp241;});struct Cyc_String_pa_PrintArg_struct _tmp187=({struct Cyc_String_pa_PrintArg_struct _tmp240;_tmp240.tag=0U,_tmp240.f1=(struct _fat_ptr)((struct _fat_ptr)obj);_tmp240;});void*_tmp184[2U];_tmp184[0]=& _tmp186,_tmp184[1]=& _tmp187;({unsigned _tmp327=f->loc;struct _fat_ptr _tmp326=({const char*_tmp185="duplicate field name %s in %s";_tag_fat(_tmp185,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp327,_tmp326,_tag_fat(_tmp184,sizeof(void*),2U));});});else{
# 742
prev_fields=({struct Cyc_List_List*_tmp188=_region_malloc(uprev_rgn,sizeof(*_tmp188));_tmp188->hd=(*f->name).f2,_tmp188->tl=prev_fields;_tmp188;});}
# 744
if((int)f->sc != (int)Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp18B=({struct Cyc_String_pa_PrintArg_struct _tmp242;_tmp242.tag=0U,_tmp242.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp242;});void*_tmp189[1U];_tmp189[0]=& _tmp18B;({unsigned _tmp329=loc;struct _fat_ptr _tmp328=({const char*_tmp18A="ignoring scope of field %s";_tag_fat(_tmp18A,sizeof(char),27U);});Cyc_Tcutil_warn(_tmp329,_tmp328,_tag_fat(_tmp189,sizeof(void*),1U));});});
f->sc=Cyc_Absyn_Public;}}}
# 749
fields2=fields;}
# 736
;_pop_region();}
# 751
return fields2;}}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 754
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _fat_ptr obj=tud->is_extensible?({const char*_tmp1A1="@extensible datatype";_tag_fat(_tmp1A1,sizeof(char),21U);}):({const char*_tmp1A2="datatype";_tag_fat(_tmp1A2,sizeof(char),9U);});
# 761
struct Cyc_List_List*tvs=tud->tvs;
# 764
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 770
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 780 "tc.cyc"
{struct _handler_cons _tmp18C;_push_handler(& _tmp18C);{int _tmp18E=0;if(setjmp(_tmp18C.handler))_tmp18E=1;if(!_tmp18E){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt == 0 && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->tag=Cyc_Dict_Absent;_tmp18F;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 786
({union Cyc_Absyn_Nmspace _tmp32A=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp32A;});}
# 781
;_pop_handler();}else{void*_tmp18D=(void*)Cyc_Core_get_exn_thrown();void*_tmp190=_tmp18D;void*_tmp191;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp190)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 790
struct Cyc_Absyn_Datatypedecl***_tmp192=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);struct Cyc_Absyn_Datatypedecl***tdopt=_tmp192;
tud_opt=(unsigned)tdopt?({struct Cyc_Absyn_Datatypedecl***_tmp193=_cycalloc(sizeof(*_tmp193));*_tmp193=*tdopt;_tmp193;}): 0;
goto _LL0;}}else{_LL3: _tmp191=_tmp190;_LL4: {void*exn=_tmp191;(int)_rethrow(exn);}}_LL0:;}}}{
# 797
struct _tuple18 _tmp194=({struct _tuple18 _tmp243;_tmp243.f1=tud->fields,_tmp243.f2=tud_opt;_tmp243;});struct _tuple18 _stmttmp16=_tmp194;struct _tuple18 _tmp195=_stmttmp16;struct Cyc_Absyn_Datatypedecl**_tmp197;struct Cyc_List_List**_tmp196;struct Cyc_List_List**_tmp198;struct Cyc_Absyn_Datatypedecl**_tmp199;if(_tmp195.f1 == 0){if(_tmp195.f2 == 0){_LL6: _LL7:
# 800
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp32D=({struct Cyc_Dict_Dict _tmp32C=(te->ae)->datatypedecls;struct _tuple0*_tmp32B=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp32C,_tmp32B,({struct Cyc_Absyn_Datatypedecl**_tmp19A=_cycalloc(sizeof(*_tmp19A));*_tmp19A=tud;_tmp19A;}));});(te->ae)->datatypedecls=_tmp32D;});
goto _LL5;}else{_LLC: _tmp199=*_tmp195.f2;_LLD: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp199;
# 833
struct Cyc_Absyn_Datatypedecl*_tmp1A0=Cyc_Tcdecl_merge_datatypedecl(*tudp,tud,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Datatypedecl*tud2=_tmp1A0;
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(tud2 == 0)
return;else{
# 838
*tudp=tud2;
goto _LL5;}}}}else{if(_tmp195.f2 == 0){_LL8: _tmp198=(struct Cyc_List_List**)&(_tmp195.f1)->v;_LL9: {struct Cyc_List_List**fs=_tmp198;
# 805
struct Cyc_Absyn_Datatypedecl**_tmp19B=({struct Cyc_Absyn_Datatypedecl**_tmp19D=_cycalloc(sizeof(*_tmp19D));({struct Cyc_Absyn_Datatypedecl*_tmp32E=({struct Cyc_Absyn_Datatypedecl*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->sc=Cyc_Absyn_Extern,_tmp19C->name=tud->name,_tmp19C->tvs=tvs,_tmp19C->fields=0,_tmp19C->is_extensible=tud->is_extensible;_tmp19C;});*_tmp19D=_tmp32E;});_tmp19D;});struct Cyc_Absyn_Datatypedecl**tudp=_tmp19B;
# 807
({struct Cyc_Dict_Dict _tmp32F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,tudp);(te->ae)->datatypedecls=_tmp32F;});
# 810
({struct Cyc_List_List*_tmp330=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp330;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*tudp=tud;
goto _LL5;}}else{_LLA: _tmp196=(struct Cyc_List_List**)&(_tmp195.f1)->v;_tmp197=*_tmp195.f2;_LLB: {struct Cyc_List_List**fs=_tmp196;struct Cyc_Absyn_Datatypedecl**tudp=_tmp197;
# 815
struct Cyc_Absyn_Datatypedecl*_tmp19E=*tudp;struct Cyc_Absyn_Datatypedecl*tud0=_tmp19E;
# 818
if((!tud->is_extensible &&(unsigned)tud0)&& tud0->is_extensible)
tud->is_extensible=1;
# 821
({struct Cyc_Absyn_Datatypedecl*_tmp331=({struct Cyc_Absyn_Datatypedecl*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->sc=Cyc_Absyn_Extern,_tmp19F->name=tud->name,_tmp19F->tvs=tvs,_tmp19F->fields=0,_tmp19F->is_extensible=tud->is_extensible;_tmp19F;});*tudp=_tmp331;});
# 825
({struct Cyc_List_List*_tmp332=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp332;});
# 828
*tudp=tud0;
_tmp199=tudp;goto _LLD;}}}_LL5:;}}}
# 844
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 851
if(ed->fields != 0){struct _RegionHandle _tmp1A3=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1A3;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1A4=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp1A4;
# 857
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp1A7=({struct Cyc_String_pa_PrintArg_struct _tmp244;_tmp244.tag=0U,_tmp244.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp244;});void*_tmp1A5[1U];_tmp1A5[0]=& _tmp1A7;({unsigned _tmp334=f->loc;struct _fat_ptr _tmp333=({const char*_tmp1A6="duplicate enum constructor %s";_tag_fat(_tmp1A6,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp334,_tmp333,_tag_fat(_tmp1A5,sizeof(void*),1U));});});else{
# 860
prev_fields=({struct Cyc_List_List*_tmp1A8=_region_malloc(uprev_rgn,sizeof(*_tmp1A8));_tmp1A8->hd=(*f->name).f2,_tmp1A8->tl=prev_fields;_tmp1A8;});}
# 862
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,f->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp245;_tmp245.tag=0U,_tmp245.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp245;});void*_tmp1A9[1U];_tmp1A9[0]=& _tmp1AB;({unsigned _tmp336=f->loc;struct _fat_ptr _tmp335=({const char*_tmp1AA="enum field name %s shadows global name";_tag_fat(_tmp1AA,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp336,_tmp335,_tag_fat(_tmp1A9,sizeof(void*),1U));});});
# 865
if(f->tag == 0){
({struct Cyc_Absyn_Exp*_tmp337=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp337;});
++ tag_count;}else{
# 870
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))){
struct _tuple11 _tmp1AC=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag));struct _tuple11 _stmttmp17=_tmp1AC;struct _tuple11 _tmp1AD=_stmttmp17;int _tmp1AF;unsigned _tmp1AE;_LL1: _tmp1AE=_tmp1AD.f1;_tmp1AF=_tmp1AD.f2;_LL2: {unsigned t1=_tmp1AE;int known=_tmp1AF;
if(known)tag_count=t1 + (unsigned)1;}}}}}
# 852
;_pop_region();}
# 878
{struct _handler_cons _tmp1B0;_push_handler(& _tmp1B0);{int _tmp1B2=0;if(setjmp(_tmp1B0.handler))_tmp1B2=1;if(!_tmp1B2){
{struct Cyc_Absyn_Enumdecl**_tmp1B3=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);struct Cyc_Absyn_Enumdecl**edp=_tmp1B3;
struct Cyc_Absyn_Enumdecl*_tmp1B4=Cyc_Tcdecl_merge_enumdecl(*edp,ed,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Enumdecl*ed2=_tmp1B4;
if(ed2 == 0){_npop_handler(0U);return;}
*edp=ed2;}
# 879
;_pop_handler();}else{void*_tmp1B1=(void*)Cyc_Core_get_exn_thrown();void*_tmp1B5=_tmp1B1;void*_tmp1B6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1B5)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 884
struct Cyc_Absyn_Enumdecl**_tmp1B7=({struct Cyc_Absyn_Enumdecl**_tmp1B8=_cycalloc(sizeof(*_tmp1B8));*_tmp1B8=ed;_tmp1B8;});struct Cyc_Absyn_Enumdecl**edp=_tmp1B7;
({struct Cyc_Dict_Dict _tmp338=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,edp);(te->ae)->enumdecls=_tmp338;});
goto _LL3;}}else{_LL6: _tmp1B6=_tmp1B5;_LL7: {void*exn=_tmp1B6;(int)_rethrow(exn);}}_LL3:;}}}
# 890
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1B9=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp1B9;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1BC=({struct Cyc_String_pa_PrintArg_struct _tmp247;_tmp247.tag=0U,_tmp247.f1=(struct _fat_ptr)((struct _fat_ptr)*(*q).f2);_tmp247;});struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp246;_tmp246.tag=0U,_tmp246.f1=(struct _fat_ptr)((struct _fat_ptr)*(*f->name).f2);_tmp246;});void*_tmp1BA[2U];_tmp1BA[0]=& _tmp1BC,_tmp1BA[1]=& _tmp1BD;({unsigned _tmp33A=loc;struct _fat_ptr _tmp339=({const char*_tmp1BB="enum %s, field %s: expression is not constant";_tag_fat(_tmp1BB,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp33A,_tmp339,_tag_fat(_tmp1BA,sizeof(void*),2U));});});}}}
# 900
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include,int in_inc_rep){
enum Cyc_Absyn_Scope _tmp1BE=sc;switch(_tmp1BE){case Cyc_Absyn_Static: _LL1: _LL2:
# 903
 if(!in_include)
({void*_tmp1BF=0U;({unsigned _tmp33C=loc;struct _fat_ptr _tmp33B=({const char*_tmp1C0="static declaration within extern \"C\"";_tag_fat(_tmp1C0,sizeof(char),37U);});Cyc_Tcutil_warn(_tmp33C,_tmp33B,_tag_fat(_tmp1BF,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 907
({void*_tmp1C1=0U;({unsigned _tmp33E=loc;struct _fat_ptr _tmp33D=({const char*_tmp1C2="abstract declaration within extern \"C\"";_tag_fat(_tmp1C2,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp33E,_tmp33D,_tag_fat(_tmp1C1,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;case Cyc_Absyn_ExternC: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
# 914
 if(!in_inc_rep)
({void*_tmp1C3=0U;({unsigned _tmp340=loc;struct _fat_ptr _tmp33F=({const char*_tmp1C4="nested extern \"C\" declaration";_tag_fat(_tmp1C4,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp340,_tmp33F,_tag_fat(_tmp1C3,sizeof(void*),0U));});});
return 1;}_LL0:;}
# 920
static void Cyc_Tc_set_scopes(struct Cyc_List_List*ovrs,enum Cyc_Absyn_Scope sc){
while((unsigned)ovrs){
struct Cyc_Absyn_Decl*_tmp1C5=(struct Cyc_Absyn_Decl*)ovrs->hd;struct Cyc_Absyn_Decl*d=_tmp1C5;
{void*_tmp1C6=d->r;void*_stmttmp18=_tmp1C6;void*_tmp1C7=_stmttmp18;struct Cyc_Absyn_Enumdecl*_tmp1C8;struct Cyc_Absyn_Datatypedecl*_tmp1C9;struct Cyc_Absyn_Aggrdecl*_tmp1CA;struct Cyc_Absyn_Fndecl*_tmp1CB;struct Cyc_Absyn_Vardecl*_tmp1CC;switch(*((int*)_tmp1C7)){case 0U: _LL1: _tmp1CC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1C7)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1CC;
vd->sc=sc;goto _LL0;}case 1U: _LL3: _tmp1CB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1C7)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp1CB;
fd->sc=sc;goto _LL0;}case 5U: _LL5: _tmp1CA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1C7)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1CA;
ad->sc=sc;goto _LL0;}case 6U: _LL7: _tmp1C9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1C7)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1C9;
tud->sc=sc;goto _LL0;}case 7U: _LL9: _tmp1C8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1C7)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp1C8;
ed->sc=sc;goto _LL0;}default: _LLB: _LLC:
# 930
 goto _LL0;}_LL0:;}
# 932
ovrs=ovrs->tl;}}
# 942
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 946
struct Cyc_List_List*_tmp1CD=ds0;struct Cyc_List_List*ds=_tmp1CD;for(0;ds != 0;ds=ds->tl){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
unsigned loc=d->loc;
void*_tmp1CE=d->r;void*_stmttmp19=_tmp1CE;void*_tmp1CF=_stmttmp19;struct _tuple10*_tmp1D3;struct Cyc_List_List*_tmp1D2;struct Cyc_List_List*_tmp1D1;struct Cyc_List_List**_tmp1D0;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D7;struct _fat_ptr*_tmp1D6;struct Cyc_Absyn_Enumdecl*_tmp1D8;struct Cyc_Absyn_Datatypedecl*_tmp1D9;struct Cyc_Absyn_Aggrdecl*_tmp1DA;struct Cyc_Absyn_Typedefdecl*_tmp1DB;struct Cyc_Absyn_Fndecl*_tmp1DC;struct Cyc_Absyn_Vardecl*_tmp1DD;switch(*((int*)_tmp1CF)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
# 952
({void*_tmp1DE=0U;({unsigned _tmp342=loc;struct _fat_ptr _tmp341=({const char*_tmp1DF="top level let-declarations are not implemented";_tag_fat(_tmp1DF,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp342,_tmp341,_tag_fat(_tmp1DE,sizeof(void*),0U));});});goto _LL0;case 4U: _LL5: _LL6:
# 954
({void*_tmp1E0=0U;({unsigned _tmp344=loc;struct _fat_ptr _tmp343=({const char*_tmp1E1="top level region declarations are not implemented";_tag_fat(_tmp1E1,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp344,_tmp343,_tag_fat(_tmp1E0,sizeof(void*),0U));});});goto _LL0;case 0U: _LL7: _tmp1DD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp1DD;
# 956
if(in_externC && Cyc_Tc_okay_externC(d->loc,vd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
vd->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,vd,check_var_init,te->in_extern_c_include,exports);
goto _LL0;}case 1U: _LL9: _tmp1DC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_LLA: {struct Cyc_Absyn_Fndecl*fd=_tmp1DC;
# 961
if(in_externC && Cyc_Tc_okay_externC(d->loc,fd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
fd->sc=Cyc_Absyn_ExternC;
if(te->in_extern_c_include)
fd->orig_scope=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,fd,exports);
goto _LL0;}case 8U: _LLB: _tmp1DB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp1DB;
# 968
td->extern_c=te->in_extern_c_include;
# 972
Cyc_Tc_tcTypedefdecl(te,loc,td);
goto _LL0;}case 5U: _LLD: _tmp1DA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1DA;
# 975
if(in_externC && Cyc_Tc_okay_externC(d->loc,ad->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);
goto _LL0;}case 6U: _LLF: _tmp1D9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_LL10: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1D9;
# 980
if(in_externC && Cyc_Tc_okay_externC(d->loc,tud->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
tud->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
goto _LL0;}case 7U: _LL11: _tmp1D8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*ed=_tmp1D8;
# 985
if(in_externC && Cyc_Tc_okay_externC(d->loc,ed->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);
goto _LL0;}case 13U: _LL13: _LL14:
({void*_tmp1E2=0U;({unsigned _tmp346=d->loc;struct _fat_ptr _tmp345=({const char*_tmp1E3="spurious __cyclone_port_on__";_tag_fat(_tmp1E3,sizeof(char),29U);});Cyc_Tcutil_warn(_tmp346,_tmp345,_tag_fat(_tmp1E2,sizeof(void*),0U));});});goto _LL0;case 14U: _LL15: _LL16:
 goto _LL0;case 15U: _LL17: _LL18:
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16U: _LL19: _LL1A:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9U: _LL1B: _tmp1D6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_tmp1D7=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1CF)->f2;_LL1C: {struct _fat_ptr*v=_tmp1D6;struct Cyc_List_List*ds2=_tmp1D7;
# 995
struct Cyc_List_List*_tmp1E4=te->ns;struct Cyc_List_List*ns=_tmp1E4;
({struct Cyc_List_List*_tmp348=({struct Cyc_List_List*_tmp347=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp347,({struct Cyc_List_List*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->hd=v,_tmp1E5->tl=0;_tmp1E5;}));});te->ns=_tmp348;});
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);
te->ns=ns;
goto _LL0;}case 10U: _LL1D: _tmp1D5=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1CF)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp1D5;
# 1002
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11U: _LL1F: _tmp1D4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp1D4;
# 1005
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default: _LL21: _tmp1D0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1CF)->f1;_tmp1D1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1CF)->f2;_tmp1D2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1CF)->f3;_tmp1D3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1CF)->f4;_LL22: {struct Cyc_List_List**ds2=_tmp1D0;struct Cyc_List_List*ovrs=_tmp1D1;struct Cyc_List_List*exports2=_tmp1D2;struct _tuple10*wc=_tmp1D3;
# 1010
if((unsigned)ovrs){
struct Cyc_Tcenv_Tenv*_tmp1E6=Cyc_Tcenv_copy_tenv_dicts(te);struct Cyc_Tcenv_Tenv*tecpy=_tmp1E6;
tecpy->in_extern_c_include=1;
Cyc_Tc_tc_decls(tecpy,*ds2,1,check_var_init,0);
Cyc_Toc_init();
Cyc_Tc_set_scopes(ovrs,Cyc_Absyn_ExternC);
Cyc_Cifc_user_overrides(d->loc,tecpy,ds2,ovrs);
Cyc_Toc_finish();}{
# 1019
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(exports2,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1E7=Cyc_Tcenv_enter_extern_c_include(te);struct Cyc_Tcenv_Tenv*te2=_tmp1E7;
te2->in_extern_c_inc_repeat=ovrs != 0;
Cyc_Tc_tc_decls(te2,*ds2,1,check_var_init,& newexs);
# 1026
for(0;exports2 != 0;exports2=exports2->tl){
struct _tuple12*_tmp1E8=(struct _tuple12*)exports2->hd;struct _tuple12*exp=_tmp1E8;
if(!(*exp).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1EB=({struct Cyc_String_pa_PrintArg_struct _tmp248;_tmp248.tag=0U,({
struct _fat_ptr _tmp349=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string((*exp).f2));_tmp248.f1=_tmp349;});_tmp248;});void*_tmp1E9[1U];_tmp1E9[0]=& _tmp1EB;({unsigned _tmp34B=(*exp).f1;struct _fat_ptr _tmp34A=({const char*_tmp1EA="%s is exported but not defined";_tag_fat(_tmp1EA,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp34B,_tmp34A,_tag_fat(_tmp1E9,sizeof(void*),1U));});});}
# 1032
goto _LL0;}}}_LL0:;}}
# 1037
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict _tmp1EC=Cyc_Callgraph_compute_callgraph(ds);struct Cyc_Dict_Dict cg=_tmp1EC;
# 1042
struct Cyc_Dict_Dict _tmp1ED=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict g))Cyc_Graph_scc)(cg);struct Cyc_Dict_Dict scc=_tmp1ED;
# 1045
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1063 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1065
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1069
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){
if((int)vd->sc == (int)Cyc_Absyn_Extern ||(int)vd->sc == (int)Cyc_Absyn_ExternC)
return 1;{
void*_tmp1EE=Cyc_Tcutil_compress(vd->type);void*_stmttmp1A=_tmp1EE;void*_tmp1EF=_stmttmp1A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1078
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1F0=d->r;void*_stmttmp1B=_tmp1F0;void*_tmp1F1=_stmttmp1B;struct Cyc_List_List**_tmp1F2;struct Cyc_List_List**_tmp1F3;struct Cyc_List_List**_tmp1F4;struct Cyc_List_List**_tmp1F5;struct Cyc_Absyn_Vardecl*_tmp1F6;switch(*((int*)_tmp1F1)){case 0U: _LL1: _tmp1F6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1F1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1F6;
# 1082
if((env->in_cinclude || !Cyc_Tc_is_extern(vd))|| !({
struct _tuple0*_tmp34C=vd->name;Cyc_Absyn_qvar_cmp(_tmp34C,Cyc_Absyn_uniquergn_qvar());}))
return 1;
return(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,vd->name)).f2;}case 11U: _LL3: _tmp1F5=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1F1)->f1;_LL4: {struct Cyc_List_List**ds2p=_tmp1F5;
_tmp1F4=ds2p;goto _LL6;}case 10U: _LL5: _tmp1F4=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1F1)->f2;_LL6: {struct Cyc_List_List**ds2p=_tmp1F4;
_tmp1F3=ds2p;goto _LL8;}case 9U: _LL7: _tmp1F3=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1F1)->f2;_LL8: {struct Cyc_List_List**ds2p=_tmp1F3;
({struct Cyc_List_List*_tmp34D=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp34D;});return 1;}case 12U: _LL9: _tmp1F2=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1F1)->f1;_LLA: {struct Cyc_List_List**ds2p=_tmp1F2;
# 1090
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp34E=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp34E;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 1099
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1F7=({struct Cyc_Tc_TreeshakeEnv _tmp249;_tmp249.in_cinclude=0,_tmp249.ordinaries=(te->ae)->ordinaries;_tmp249;});struct Cyc_Tc_TreeshakeEnv env=_tmp1F7;
return Cyc_Tc_treeshake_f(& env,ds);}

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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 93 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 859 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 868
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 875
void*Cyc_Absyn_compress(void*);
# 889
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 905
extern void*Cyc_Absyn_false_type;
# 929
void*Cyc_Absyn_string_type(void*);
void*Cyc_Absyn_const_string_type(void*);
# 950
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 958
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 976
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1034
struct _tuple0*Cyc_Absyn_uniquergn_qvar (void);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 43 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 43 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 46
int Cyc_Tcutil_is_array_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 107
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 156
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 159
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 205
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 214
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 231
int Cyc_Tcutil_zeroable_type(void*);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 62
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 70
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int);
void Cyc_Atts_check_variable_atts(unsigned,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*);
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*,struct Cyc_List_List*);
# 74
void Cyc_Atts_fnTypeAttsOK(unsigned,void*);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83 "dict.h"
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
int Cyc_Tcstmt_ensure_no_throws(struct Cyc_Absyn_Stmt*);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 54 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*);
# 57
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*);
# 59
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*);
# 66
void*Cyc_Tcdecl_merge_binding(void*,void*,unsigned,struct _fat_ptr*);
# 74
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*,int*,struct _fat_ptr*,unsigned,struct _fat_ptr*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 92 "graph.h"
extern struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);
# 36 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);
# 36 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List**,struct Cyc_List_List*);struct Cyc_Hashtable_Table;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47 "toc.h"
void Cyc_Toc_init (void);
void Cyc_Toc_finish (void);static char _tmp0[1U]="";
# 46 "tc.cyc"
static struct _fat_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1U};
static struct _fat_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned f1;struct _tuple0*f2;int f3;};
# 49
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(1;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;struct _tuple12*p=_tmp1;
if(!Cyc_Absyn_qvar_cmp(x,(*p).f2)== 0)
continue;
# 56
(*p).f3=1;
return 1;}
# 59
return 0;}struct _tuple13{void*f1;int f2;};
# 62
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 64
struct Cyc_Absyn_Vardecl*_tmp2=vd;int*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_Absyn_Exp*_tmp6;void*_tmp5;struct _tuple0*_tmp4;enum Cyc_Absyn_Scope _tmp3;_LL1: _tmp3=_tmp2->sc;_tmp4=_tmp2->name;_tmp5=_tmp2->type;_tmp6=_tmp2->initializer;_tmp7=_tmp2->attributes;_tmp8=(int*)& _tmp2->is_proto;_LL2: {enum Cyc_Absyn_Scope sc=_tmp3;struct _tuple0*q=_tmp4;void*t=_tmp5;struct Cyc_Absyn_Exp*initializer=_tmp6;struct Cyc_List_List*atts=_tmp7;int*is_proto=_tmp8;
# 71
{void*_tmp9=Cyc_Absyn_compress(t);void*_stmttmp0=_tmp9;void*_tmpA=_stmttmp0;unsigned _tmpE;void*_tmpD;struct Cyc_Absyn_Tqual _tmpC;void*_tmpB;if(*((int*)_tmpA)== 4){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).num_elts == 0){_LL4: _tmpB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).elt_type;_tmpC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).tq;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).zero_term;_tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA)->f1).zt_loc;if(initializer != 0){_LL5: {void*telt=_tmpB;struct Cyc_Absyn_Tqual tq=_tmpC;void*zt=_tmpD;unsigned ztl=_tmpE;
# 73
{void*_tmpF=initializer->r;void*_stmttmp1=_tmpF;void*_tmp10=_stmttmp1;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp14;struct _fat_ptr _tmp15;struct _fat_ptr _tmp16;switch(*((int*)_tmp10)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Wstring_c).tag){case 8: _LL9: _tmp16=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).String_c).val;_LLA: {struct _fat_ptr s=_tmp16;
# 75
t=({void*_tmp255=({void*_tmp254=telt;struct Cyc_Absyn_Tqual _tmp253=tq;struct Cyc_Absyn_Exp*_tmp252=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_tmp251=zt;Cyc_Absyn_array_type(_tmp254,_tmp253,_tmp252,_tmp251,ztl);});vd->type=_tmp255;});goto _LL8;}case 9: _LLB: _tmp15=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Wstring_c).val;_LLC: {struct _fat_ptr s=_tmp15;
# 77
t=({void*_tmp25A=({void*_tmp259=telt;struct Cyc_Absyn_Tqual _tmp258=tq;struct Cyc_Absyn_Exp*_tmp257=Cyc_Absyn_uint_exp(1U,0U);void*_tmp256=zt;Cyc_Absyn_array_type(_tmp259,_tmp258,_tmp257,_tmp256,ztl);});vd->type=_tmp25A;});goto _LL8;}default: goto _LL15;}case 27: _LLD: _tmp14=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp14;
_tmp13=e;goto _LL10;}case 28: _LLF: _tmp13=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmp13;
# 80
t=({void*_tmp25B=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_tmp25B;});goto _LL8;}case 36: _LL11: _tmp12=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10)->f2;_LL12: {struct Cyc_List_List*es=_tmp12;
_tmp11=es;goto _LL14;}case 26: _LL13: _tmp11=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10)->f1;_LL14: {struct Cyc_List_List*es=_tmp11;
# 83
t=({void*_tmp260=({void*_tmp25F=telt;struct Cyc_Absyn_Tqual _tmp25E=tq;struct Cyc_Absyn_Exp*_tmp25D=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*))Cyc_List_length)(es),0U);void*_tmp25C=zt;Cyc_Absyn_array_type(_tmp25F,_tmp25E,_tmp25D,_tmp25C,ztl);});vd->type=_tmp260;});
goto _LL8;}default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 87
goto _LL3;}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 91
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
# 93
({int _tmp261=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp261;});
# 96
({int _tmp262=!Cyc_Tcutil_is_array_type(t);vd->escapes=_tmp262;});
# 98
if(Cyc_Tcutil_is_function_type(t)){
*is_proto=0;
atts=Cyc_Atts_transfer_fn_type_atts(t,atts);
Cyc_Atts_fnTypeAttsOK(loc,t);}
# 104
if((int)sc == (int)3U ||(int)sc == (int)4U){
if(initializer != 0 && !in_cinclude)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E6;_tmp1E6.tag=0,({struct _fat_ptr _tmp263=({const char*_tmp19="extern declaration should not have initializer";_tag_fat(_tmp19,sizeof(char),47U);});_tmp1E6.f1=_tmp263;});_tmp1E6;});void*_tmp17[1];_tmp17[0]=& _tmp18;({unsigned _tmp264=loc;Cyc_Warn_err2(_tmp264,_tag_fat(_tmp17,sizeof(void*),1));});});}else{
if(!Cyc_Tcutil_is_function_type(t)){
# 111
Cyc_Atts_check_variable_atts(loc,vd,atts);
if(initializer == 0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EA;_tmp1EA.tag=0,({struct _fat_ptr _tmp265=({const char*_tmp20="initializer required for variable ";_tag_fat(_tmp20,sizeof(char),35U);});_tmp1EA.f1=_tmp265;});_tmp1EA;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp1E9;_tmp1E9.tag=11,_tmp1E9.f1=vd;_tmp1E9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E8;_tmp1E8.tag=0,({struct _fat_ptr _tmp266=({const char*_tmp1F=" of type ";_tag_fat(_tmp1F,sizeof(char),10U);});_tmp1E8.f1=_tmp266;});_tmp1E8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E7;_tmp1E7.tag=2,_tmp1E7.f1=(void*)t;_tmp1E7;});void*_tmp1A[4];_tmp1A[0]=& _tmp1B,_tmp1A[1]=& _tmp1C,_tmp1A[2]=& _tmp1D,_tmp1A[3]=& _tmp1E;({unsigned _tmp267=loc;Cyc_Warn_err2(_tmp267,_tag_fat(_tmp1A,sizeof(void*),4));});});}else{
# 119
struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){
{void*_tmp24=Cyc_Tcexp_tcExpInitializer(te,& t,initializer);void*t2=_tmp24;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp269=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp268=initializer;Cyc_Tcutil_coerce_assign(_tmp269,_tmp268,t);})){
struct _fat_ptr _tmp25=Cyc_Absynpp_qvar2string(vd->name);struct _fat_ptr s0=_tmp25;
const char*_tmp26=" declared with type ";const char*s1=_tmp26;
struct _fat_ptr _tmp27=Cyc_Absynpp_typ2string(t);struct _fat_ptr s2=_tmp27;
const char*_tmp28=" but initialized with type ";const char*s3=_tmp28;
struct _fat_ptr _tmp29=Cyc_Absynpp_typ2string(t2);struct _fat_ptr s4=_tmp29;
if(({unsigned long _tmp26D=({unsigned long _tmp26C=({unsigned long _tmp26B=({unsigned long _tmp26A=Cyc_strlen((struct _fat_ptr)s0);_tmp26A + Cyc_strlen(({const char*_tmp2A=s1;_tag_fat(_tmp2A,sizeof(char),21U);}));});_tmp26B + Cyc_strlen((struct _fat_ptr)s2);});_tmp26C + Cyc_strlen(({const char*_tmp2B=s3;_tag_fat(_tmp2B,sizeof(char),28U);}));});_tmp26D + Cyc_strlen((struct _fat_ptr)s4);})> (unsigned long)70)
({struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp1EF;_tmp1EF.tag=0,_tmp1EF.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp1EF;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp1EE;_tmp1EE.tag=0,({struct _fat_ptr _tmp26E=(struct _fat_ptr)({const char*_tmp34=s1;_tag_fat(_tmp34,sizeof(char),21U);});_tmp1EE.f1=_tmp26E;});_tmp1EE;});struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp1ED;_tmp1ED.tag=0,_tmp1ED.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1ED;});struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0,({struct _fat_ptr _tmp26F=(struct _fat_ptr)({const char*_tmp33=s3;_tag_fat(_tmp33,sizeof(char),28U);});_tmp1EC.f1=_tmp26F;});_tmp1EC;});struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0,_tmp1EB.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp1EB;});void*_tmp2C[5];_tmp2C[0]=& _tmp2E,_tmp2C[1]=& _tmp2F,_tmp2C[2]=& _tmp30,_tmp2C[3]=& _tmp31,_tmp2C[4]=& _tmp32;({unsigned _tmp271=loc;struct _fat_ptr _tmp270=({const char*_tmp2D="%s%s\n\t%s\n%s\n\t%s";_tag_fat(_tmp2D,sizeof(char),16U);});Cyc_Warn_err(_tmp271,_tmp270,_tag_fat(_tmp2C,sizeof(void*),5));});});else{
# 130
({struct Cyc_String_pa_PrintArg_struct _tmp37=({struct Cyc_String_pa_PrintArg_struct _tmp1F4;_tmp1F4.tag=0,_tmp1F4.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp1F4;});struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0,({struct _fat_ptr _tmp272=(struct _fat_ptr)({const char*_tmp3D=s1;_tag_fat(_tmp3D,sizeof(char),21U);});_tmp1F3.f1=_tmp272;});_tmp1F3;});struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0,_tmp1F2.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp1F2;});struct Cyc_String_pa_PrintArg_struct _tmp3A=({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0,({struct _fat_ptr _tmp273=(struct _fat_ptr)({const char*_tmp3C=s3;_tag_fat(_tmp3C,sizeof(char),28U);});_tmp1F1.f1=_tmp273;});_tmp1F1;});struct Cyc_String_pa_PrintArg_struct _tmp3B=({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0,_tmp1F0.f1=(struct _fat_ptr)((struct _fat_ptr)s4);_tmp1F0;});void*_tmp35[5];_tmp35[0]=& _tmp37,_tmp35[1]=& _tmp38,_tmp35[2]=& _tmp39,_tmp35[3]=& _tmp3A,_tmp35[4]=& _tmp3B;({unsigned _tmp275=loc;struct _fat_ptr _tmp274=({const char*_tmp36="%s%s%s%s%s";_tag_fat(_tmp36,sizeof(char),11U);});Cyc_Warn_err(_tmp275,_tmp274,_tag_fat(_tmp35,sizeof(void*),5));});});}
Cyc_Unify_explain_failure();}
# 134
if(!Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F5;_tmp1F5.tag=0,({struct _fat_ptr _tmp276=({const char*_tmp40="initializer is not a constant expression";_tag_fat(_tmp40,sizeof(char),41U);});_tmp1F5.f1=_tmp276;});_tmp1F5;});void*_tmp3E[1];_tmp3E[0]=& _tmp3F;({unsigned _tmp277=loc;Cyc_Warn_err2(_tmp277,_tag_fat(_tmp3E,sizeof(void*),1));});});}
# 120
;_pop_handler();}else{void*_tmp22=(void*)Cyc_Core_get_exn_thrown();void*_tmp41=_tmp22;void*_tmp42;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp41)->tag == Cyc_Tcenv_Env_error){_LL18: _LL19:
# 137
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F6;_tmp1F6.tag=0,({struct _fat_ptr _tmp278=({const char*_tmp45="initializer is not a constant expression";_tag_fat(_tmp45,sizeof(char),41U);});_tmp1F6.f1=_tmp278;});_tmp1F6;});void*_tmp43[1];_tmp43[0]=& _tmp44;({unsigned _tmp279=loc;Cyc_Warn_err2(_tmp279,_tag_fat(_tmp43,sizeof(void*),1));});});goto _LL17;}else{_LL1A: _tmp42=_tmp41;_LL1B: {void*exn=_tmp42;(int)_rethrow(exn);}}_LL17:;}}}}else{
# 141
Cyc_Atts_check_fndecl_atts(loc,atts,0);}}
# 144
{struct _handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler))_tmp48=1;if(!_tmp48){
{struct _tuple13*_tmp49=((struct _tuple13*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple13*ans=_tmp49;
void*_tmp4A=(*ans).f1;void*b0=_tmp4A;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4B=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=1,_tmp4E->f1=vd;_tmp4E;});struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1=_tmp4B;
void*_tmp4C=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmp4C;
if(b == 0){_npop_handler(0);return;}
# 151
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports)){
if(b != b0 ||(*ans).f2)
# 154
({struct Cyc_Dict_Dict _tmp27C=({struct Cyc_Dict_Dict _tmp27B=(te->ae)->ordinaries;struct _tuple0*_tmp27A=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert)(_tmp27B,_tmp27A,({struct _tuple13*_tmp4D=_cycalloc(sizeof(*_tmp4D));
_tmp4D->f1=b,_tmp4D->f2=(*ans).f2;_tmp4D;}));});
# 154
(te->ae)->ordinaries=_tmp27C;});}
# 156
_npop_handler(0);return;}
# 145
;_pop_handler();}else{void*_tmp47=(void*)Cyc_Core_get_exn_thrown();void*_tmp4F=_tmp47;void*_tmp50;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4F)->tag == Cyc_Dict_Absent){_LL1D: _LL1E:
# 157
 goto _LL1C;}else{_LL1F: _tmp50=_tmp4F;_LL20: {void*exn=_tmp50;(int)_rethrow(exn);}}_LL1C:;}}}
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp280=({struct Cyc_Dict_Dict _tmp27F=(te->ae)->ordinaries;struct _tuple0*_tmp27E=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert)(_tmp27F,_tmp27E,({struct _tuple13*_tmp52=_cycalloc(sizeof(*_tmp52));
({void*_tmp27D=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=1,_tmp51->f1=vd;_tmp51;});_tmp52->f1=_tmp27D;}),_tmp52->f2=0;_tmp52;}));});
# 159
(te->ae)->ordinaries=_tmp280;});}}
# 163
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp53=n;struct _fat_ptr*_tmp55;union Cyc_Absyn_Nmspace _tmp54;_LL1: _tmp54=_tmp53->f1;_tmp55=_tmp53->f2;_LL2: {union Cyc_Absyn_Nmspace nms=_tmp54;struct _fat_ptr*v=_tmp55;
union Cyc_Absyn_Nmspace _tmp56=nms;if((_tmp56.Abs_n).tag == 2){if((_tmp56.Abs_n).val == 0){_LL4: _LL5:
 return({struct _fat_ptr _tmp281=(struct _fat_ptr)*v;Cyc_strcmp(_tmp281,({const char*_tmp57="main";_tag_fat(_tmp57,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;}}
# 174
static int Cyc_Tc_check_catchall_handler(struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Stmt*_tmp58=fd->body;struct Cyc_Absyn_Stmt*b=_tmp58;
void*_tmp59=b->r;void*_stmttmp2=_tmp59;void*_tmp5A=_stmttmp2;struct Cyc_List_List*_tmp5B;if(*((int*)_tmp5A)== 15){_LL1: _tmp5B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5A)->f2;_LL2: {struct Cyc_List_List*case_list=_tmp5B;
# 178
int found_wild=0;
while((unsigned)case_list){
struct Cyc_Absyn_Switch_clause*_tmp5C=(struct Cyc_Absyn_Switch_clause*)case_list->hd;struct Cyc_Absyn_Switch_clause*clause=_tmp5C;
{void*_tmp5D=(clause->pattern)->r;void*_stmttmp3=_tmp5D;void*_tmp5E=_stmttmp3;if(*((int*)_tmp5E)== 0){_LL6: _LL7:
# 183
 found_wild=1;
goto _LL5;}else{_LL8: _LL9:
# 186
 goto _LL5;}_LL5:;}
# 188
if(!Cyc_Tcstmt_ensure_no_throws(clause->body))
return 0;
case_list=case_list->tl;}
# 192
return found_wild;}}else{_LL3: _LL4:
# 194
 return 0;}_LL0:;}
# 198
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 200
struct _tuple0*q=fd->name;
# 204
if((int)fd->sc == (int)Cyc_Absyn_ExternC && !te->in_extern_c_include)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp60=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F7;_tmp1F7.tag=0,({struct _fat_ptr _tmp282=({const char*_tmp61="extern \"C\" functions cannot be implemented in Cyclone";_tag_fat(_tmp61,sizeof(char),54U);});_tmp1F7.f1=_tmp282;});_tmp1F7;});void*_tmp5F[1];_tmp5F[0]=& _tmp60;({unsigned _tmp283=loc;Cyc_Warn_err2(_tmp283,_tag_fat(_tmp5F,sizeof(void*),1));});});
# 208
Cyc_Atts_check_fndecl_atts(loc,(fd->i).attributes,1);
# 211
if(te->in_extern_c_inc_repeat)
fd->cached_type=0;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 216
int nothrow=((int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem)(Cyc_Atts_attribute_cmp,(fd->i).attributes,(void*)& Cyc_Atts_No_throw_att_val);
# 219
({struct Cyc_List_List*_tmp284=Cyc_Atts_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp284;});
Cyc_Atts_fnTypeAttsOK(loc,t);
# 223
{struct _handler_cons _tmp62;_push_handler(& _tmp62);{int _tmp64=0;if(setjmp(_tmp62.handler))_tmp64=1;if(!_tmp64){
{struct _tuple13*_tmp65=((struct _tuple13*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple13*ans=_tmp65;
void*_tmp66=(*ans).f1;void*b0=_tmp66;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp67=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=2,_tmp6A->f1=fd;_tmp6A;});struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1=_tmp67;
void*_tmp68=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);void*b=_tmp68;
if(b != 0){
# 230
if(exports == 0 || Cyc_Tc_export_member(q,*exports)){
if(!(b == b0 &&(*ans).f2))
({struct Cyc_Dict_Dict _tmp287=({struct Cyc_Dict_Dict _tmp286=(te->ae)->ordinaries;struct _tuple0*_tmp285=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert)(_tmp286,_tmp285,({struct _tuple13*_tmp69=_cycalloc(sizeof(*_tmp69));
_tmp69->f1=b,_tmp69->f2=(*ans).f2;_tmp69;}));});
# 232
(te->ae)->ordinaries=_tmp287;});}}}
# 224
;_pop_handler();}else{void*_tmp63=(void*)Cyc_Core_get_exn_thrown();void*_tmp6B=_tmp63;void*_tmp6C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6B)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 235
 if(exports == 0 || Cyc_Tc_export_member(fd->name,*exports))
({struct Cyc_Dict_Dict _tmp28B=({struct Cyc_Dict_Dict _tmp28A=(te->ae)->ordinaries;struct _tuple0*_tmp289=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple13*))Cyc_Dict_insert)(_tmp28A,_tmp289,({struct _tuple13*_tmp6E=_cycalloc(sizeof(*_tmp6E));
({void*_tmp288=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=2,_tmp6D->f1=fd;_tmp6D;});_tmp6E->f1=_tmp288;}),_tmp6E->f2=0;_tmp6E;}));});
# 236
(te->ae)->ordinaries=_tmp28B;});
# 238
goto _LL0;}else{_LL3: _tmp6C=_tmp6B;_LL4: {void*exn=_tmp6C;(int)_rethrow(exn);}}_LL0:;}}}
# 242
if(te->in_extern_c_include)return;{
# 247
struct Cyc_Tcenv_Fenv*_tmp6F=Cyc_Tcenv_new_fenv(loc,fd);struct Cyc_Tcenv_Fenv*fenv=_tmp6F;
struct Cyc_Tcenv_Tenv*_tmp70=({struct Cyc_Tcenv_Tenv*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->ns=te->ns,_tmpA3->ae=te->ae,_tmpA3->le=fenv,_tmpA3->allow_valueof=0,_tmpA3->in_extern_c_include=0,_tmpA3->in_tempest=te->in_tempest,_tmpA3->tempest_generalize=te->tempest_generalize,_tmpA3->in_extern_c_inc_repeat=0;_tmpA3;});struct Cyc_Tcenv_Tenv*te=_tmp70;
# 250
Cyc_Tcstmt_tcStmt(te,fd->body,0);
# 253
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
# 256
if(te->in_tempest){
te->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);
te->tempest_generalize=0;}
# 261
if(nothrow && !Cyc_Tc_check_catchall_handler(fd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp72=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F8;_tmp1F8.tag=0,({struct _fat_ptr _tmp28C=({const char*_tmp73="Function with attribute no_throw must have a catch-all handler";_tag_fat(_tmp73,sizeof(char),63U);});_tmp1F8.f1=_tmp28C;});_tmp1F8;});void*_tmp71[1];_tmp71[0]=& _tmp72;({unsigned _tmp28D=loc;Cyc_Warn_err2(_tmp28D,_tag_fat(_tmp71,sizeof(void*),1));});});
# 264
if(Cyc_Tc_is_main(q)){
# 266
{void*_tmp74=Cyc_Absyn_compress((fd->i).ret_type);void*_stmttmp4=_tmp74;void*_tmp75=_stmttmp4;if(*((int*)_tmp75)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp75)->f1)){case 0: _LL6: _LL7:
# 268
({struct Cyc_Warn_String_Warn_Warg_struct _tmp77=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F9;_tmp1F9.tag=0,({struct _fat_ptr _tmp28E=({const char*_tmp78="main declared with return type void";_tag_fat(_tmp78,sizeof(char),36U);});_tmp1F9.f1=_tmp28E;});_tmp1F9;});void*_tmp76[1];_tmp76[0]=& _tmp77;({unsigned _tmp28F=loc;Cyc_Warn_warn2(_tmp28F,_tag_fat(_tmp76,sizeof(void*),1));});});goto _LL5;case 1: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp75)->f1)->f2){case Cyc_Absyn_Int_sz: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Long_sz: _LLA: _LLB:
 goto _LL5;default: goto _LLC;}default: goto _LLC;}else{_LLC: _LLD:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FC;_tmp1FC.tag=0,({struct _fat_ptr _tmp290=({const char*_tmp7E="main declared with return type ";_tag_fat(_tmp7E,sizeof(char),32U);});_tmp1FC.f1=_tmp290;});_tmp1FC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1FB;_tmp1FB.tag=2,_tmp1FB.f1=(void*)(fd->i).ret_type;_tmp1FB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FA;_tmp1FA.tag=0,({
struct _fat_ptr _tmp291=({const char*_tmp7D=" instead of int or void";_tag_fat(_tmp7D,sizeof(char),24U);});_tmp1FA.f1=_tmp291;});_tmp1FA;});void*_tmp79[3];_tmp79[0]=& _tmp7A,_tmp79[1]=& _tmp7B,_tmp79[2]=& _tmp7C;({unsigned _tmp292=loc;Cyc_Warn_err2(_tmp292,_tag_fat(_tmp79,sizeof(void*),3));});});}_LL5:;}
# 274
if((fd->i).c_varargs ||(fd->i).cyc_varargs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FD;_tmp1FD.tag=0,({struct _fat_ptr _tmp293=({const char*_tmp81="main declared with varargs";_tag_fat(_tmp81,sizeof(char),27U);});_tmp1FD.f1=_tmp293;});_tmp1FD;});void*_tmp7F[1];_tmp7F[0]=& _tmp80;({unsigned _tmp294=loc;Cyc_Warn_err2(_tmp294,_tag_fat(_tmp7F,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp82=(fd->i).args;struct Cyc_List_List*args=_tmp82;
if(args != 0){
struct _tuple8*_tmp83=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp5=_tmp83;struct _tuple8*_tmp84=_stmttmp5;void*_tmp85;_LLF: _tmp85=_tmp84->f3;_LL10: {void*t1=_tmp85;
{void*_tmp86=Cyc_Absyn_compress(t1);void*_stmttmp6=_tmp86;void*_tmp87=_stmttmp6;if(*((int*)_tmp87)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp87)->f1)== 1)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp87)->f1)->f2){case Cyc_Absyn_Int_sz: _LL12: _LL13:
 goto _LL15;case Cyc_Absyn_Long_sz: _LL14: _LL15:
 goto _LL11;default: goto _LL16;}else{goto _LL16;}}else{_LL16: _LL17:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp200;_tmp200.tag=0,({struct _fat_ptr _tmp295=({const char*_tmp8D="main declared with first argument of type ";_tag_fat(_tmp8D,sizeof(char),43U);});_tmp200.f1=_tmp295;});_tmp200;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1FF;_tmp1FF.tag=2,_tmp1FF.f1=(void*)t1;_tmp1FF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FE;_tmp1FE.tag=0,({
struct _fat_ptr _tmp296=({const char*_tmp8C=" instead of int";_tag_fat(_tmp8C,sizeof(char),16U);});_tmp1FE.f1=_tmp296;});_tmp1FE;});void*_tmp88[3];_tmp88[0]=& _tmp89,_tmp88[1]=& _tmp8A,_tmp88[2]=& _tmp8B;({unsigned _tmp297=loc;Cyc_Warn_err2(_tmp297,_tag_fat(_tmp88,sizeof(void*),3));});});}_LL11:;}
# 285
args=args->tl;
if(args != 0){
struct _tuple8*_tmp8E=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp7=_tmp8E;struct _tuple8*_tmp8F=_stmttmp7;void*_tmp90;_LL19: _tmp90=_tmp8F->f3;_LL1A: {void*t2=_tmp90;
args=args->tl;
if(args != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp92=({struct Cyc_Warn_String_Warn_Warg_struct _tmp201;_tmp201.tag=0,({struct _fat_ptr _tmp298=({const char*_tmp93="main declared with too many arguments";_tag_fat(_tmp93,sizeof(char),38U);});_tmp201.f1=_tmp298;});_tmp201;});void*_tmp91[1];_tmp91[0]=& _tmp92;({unsigned _tmp299=loc;Cyc_Warn_err2(_tmp299,_tag_fat(_tmp91,sizeof(void*),1));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->v=(fd->i).tvars;_tmpA2;});
if(((!({void*_tmp2B5=t2;Cyc_Unify_unify(_tmp2B5,({void*_tmp2B4=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp2AF=({struct Cyc_Core_Opt*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->v=& Cyc_Kinds_rk;_tmp94;});Cyc_Absyn_new_evar(_tmp2AF,tvs);}));void*_tmp2B3=({
struct Cyc_Core_Opt*_tmp2B0=({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->v=& Cyc_Kinds_rk;_tmp95;});Cyc_Absyn_new_evar(_tmp2B0,tvs);});
# 292
struct Cyc_Absyn_Tqual _tmp2B2=
# 294
Cyc_Absyn_empty_tqual(0U);
# 292
void*_tmp2B1=
# 294
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 292
Cyc_Absyn_fatptr_type(_tmp2B4,_tmp2B3,_tmp2B2,_tmp2B1,Cyc_Absyn_false_type);}));})&& !({
# 295
void*_tmp2AE=t2;Cyc_Unify_unify(_tmp2AE,({void*_tmp2AD=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp2A8=({struct Cyc_Core_Opt*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->v=& Cyc_Kinds_rk;_tmp96;});Cyc_Absyn_new_evar(_tmp2A8,tvs);}));void*_tmp2AC=({
# 297
struct Cyc_Core_Opt*_tmp2A9=({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->v=& Cyc_Kinds_rk;_tmp97;});Cyc_Absyn_new_evar(_tmp2A9,tvs);});
# 295
struct Cyc_Absyn_Tqual _tmp2AB=
# 298
Cyc_Absyn_empty_tqual(0U);
# 295
void*_tmp2AA=
# 298
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 295
Cyc_Absyn_fatptr_type(_tmp2AD,_tmp2AC,_tmp2AB,_tmp2AA,Cyc_Absyn_false_type);}));}))&& !({
# 299
void*_tmp2A7=t2;Cyc_Unify_unify(_tmp2A7,({void*_tmp2A6=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp2A1=({struct Cyc_Core_Opt*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->v=& Cyc_Kinds_rk;_tmp98;});Cyc_Absyn_new_evar(_tmp2A1,tvs);}));void*_tmp2A5=({
struct Cyc_Core_Opt*_tmp2A2=({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->v=& Cyc_Kinds_rk;_tmp99;});Cyc_Absyn_new_evar(_tmp2A2,tvs);});
# 299
struct Cyc_Absyn_Tqual _tmp2A4=
# 301
Cyc_Absyn_const_tqual(0U);
# 299
void*_tmp2A3=
# 301
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 299
Cyc_Absyn_fatptr_type(_tmp2A6,_tmp2A5,_tmp2A4,_tmp2A3,Cyc_Absyn_false_type);}));}))&& !({
# 302
void*_tmp2A0=t2;Cyc_Unify_unify(_tmp2A0,({void*_tmp29F=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp29A=({struct Cyc_Core_Opt*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->v=& Cyc_Kinds_rk;_tmp9A;});Cyc_Absyn_new_evar(_tmp29A,tvs);}));void*_tmp29E=({
# 304
struct Cyc_Core_Opt*_tmp29B=({struct Cyc_Core_Opt*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->v=& Cyc_Kinds_rk;_tmp9B;});Cyc_Absyn_new_evar(_tmp29B,tvs);});
# 302
struct Cyc_Absyn_Tqual _tmp29D=
# 305
Cyc_Absyn_const_tqual(0U);
# 302
void*_tmp29C=
# 305
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 302
Cyc_Absyn_fatptr_type(_tmp29F,_tmp29E,_tmp29D,_tmp29C,Cyc_Absyn_false_type);}));}))
# 306
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp204;_tmp204.tag=0,({struct _fat_ptr _tmp2B6=({const char*_tmpA1="second argument of main has type ";_tag_fat(_tmpA1,sizeof(char),34U);});_tmp204.f1=_tmp2B6;});_tmp204;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp9E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp203;_tmp203.tag=2,_tmp203.f1=(void*)t2;_tmp203;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp202;_tmp202.tag=0,({
struct _fat_ptr _tmp2B7=({const char*_tmpA0=" instead of char??";_tag_fat(_tmpA0,sizeof(char),19U);});_tmp202.f1=_tmp2B7;});_tmp202;});void*_tmp9C[3];_tmp9C[0]=& _tmp9D,_tmp9C[1]=& _tmp9E,_tmp9C[2]=& _tmp9F;({unsigned _tmp2B8=loc;Cyc_Warn_err2(_tmp2B8,_tag_fat(_tmp9C,sizeof(void*),3));});});}}}}}}}}}}
# 314
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 320
if(((int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp206;_tmp206.tag=0,({struct _fat_ptr _tmp2B9=({const char*_tmpA7="redeclaration of typedef ";_tag_fat(_tmpA7,sizeof(char),26U);});_tmp206.f1=_tmp2B9;});_tmp206;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp205;_tmp205.tag=1,_tmp205.f1=q;_tmp205;});void*_tmpA4[2];_tmpA4[0]=& _tmpA5,_tmpA4[1]=& _tmpA6;({unsigned _tmp2BA=loc;Cyc_Warn_err2(_tmp2BA,_tag_fat(_tmpA4,sizeof(void*),2));});});
return;}
# 325
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tctyp_check_type(loc,te,td->tvs,& Cyc_Kinds_tak,0,1,(void*)_check_null(td->defn));
({int _tmp2BB=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 329
(td->tq).real_const=_tmp2BB;});}
# 334
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpA8=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_stmttmp8=_tmpA8;void*_tmpA9=_stmttmp8;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Core_Opt**_tmpAA;struct Cyc_Core_Opt**_tmpAC;switch(*((int*)_tmpA9)){case 1: _LL1: _tmpAC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA9)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpAC;
# 337
if(td->defn != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp209;_tmp209.tag=0,({struct _fat_ptr _tmp2BC=({const char*_tmpB2="type variable ";_tag_fat(_tmpB2,sizeof(char),15U);});_tmp209.f1=_tmp2BC;});_tmp209;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp208;_tmp208.tag=7,_tmp208.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp208;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp207;_tmp207.tag=0,({struct _fat_ptr _tmp2BD=({const char*_tmpB1=" is not used in typedef";_tag_fat(_tmpB1,sizeof(char),24U);});_tmp207.f1=_tmp2BD;});_tmp207;});void*_tmpAD[3];_tmpAD[0]=& _tmpAE,_tmpAD[1]=& _tmpAF,_tmpAD[2]=& _tmpB0;({unsigned _tmp2BE=loc;Cyc_Warn_warn2(_tmp2BE,_tag_fat(_tmpAD,sizeof(void*),3));});});
({struct Cyc_Core_Opt*_tmp2C0=({struct Cyc_Core_Opt*_tmpB3=_cycalloc(sizeof(*_tmpB3));({void*_tmp2BF=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_tbk);_tmpB3->v=_tmp2BF;});_tmpB3;});*f=_tmp2C0;});goto _LL0;}case 2: _LL3: _tmpAA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA9)->f1;_tmpAB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA9)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmpAA;struct Cyc_Absyn_Kind*k=_tmpAB;
# 346
({struct Cyc_Core_Opt*_tmp2C2=({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));({void*_tmp2C1=Cyc_Kinds_kind_to_bound(k);_tmpB4->v=_tmp2C1;});_tmpB4;});*f=_tmp2C2;});
goto _LL0;}default: _LL5: _LL6:
 continue;}_LL0:;}}
# 351
({struct Cyc_Dict_Dict _tmp2C3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp2C3;});}struct _tuple14{void*f1;void*f2;};
# 354
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 361
struct _RegionHandle _tmpB5=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpB5;_push_region(uprev_rgn);
# 363
for(1;rpo != 0;rpo=rpo->tl){
struct _tuple14*_tmpB6=(struct _tuple14*)rpo->hd;struct _tuple14*_stmttmp9=_tmpB6;struct _tuple14*_tmpB7=_stmttmp9;void*_tmpB9;void*_tmpB8;_LL1: _tmpB8=_tmpB7->f1;_tmpB9=_tmpB7->f2;_LL2: {void*e=_tmpB8;void*r=_tmpB9;
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Kinds_ek,0,0,e);
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Kinds_trk,0,0,r);}}{
# 369
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 372
struct Cyc_List_List*_tmpBA=fields;struct Cyc_List_List*fs=_tmpBA;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpBB=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmpA=_tmpBB;struct Cyc_Absyn_Aggrfield*_tmpBC=_stmttmpA;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;void*_tmpBF;struct Cyc_Absyn_Tqual _tmpBE;struct _fat_ptr*_tmpBD;_LL4: _tmpBD=_tmpBC->name;_tmpBE=_tmpBC->tq;_tmpBF=_tmpBC->type;_tmpC0=_tmpBC->width;_tmpC1=_tmpBC->attributes;_tmpC2=_tmpBC->requires_clause;_LL5: {struct _fat_ptr*fn=_tmpBD;struct Cyc_Absyn_Tqual tq=_tmpBE;void*t=_tmpBF;struct Cyc_Absyn_Exp*width=_tmpC0;struct Cyc_List_List*atts=_tmpC1;struct Cyc_Absyn_Exp*requires_clause=_tmpC2;
# 375
if(((int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20B;_tmp20B.tag=0,({struct _fat_ptr _tmp2C4=({const char*_tmpC6="duplicate member ";_tag_fat(_tmpC6,sizeof(char),18U);});_tmp20B.f1=_tmp2C4;});_tmp20B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20A;_tmp20A.tag=0,_tmp20A.f1=*fn;_tmp20A;});void*_tmpC3[2];_tmpC3[0]=& _tmpC4,_tmpC3[1]=& _tmpC5;({unsigned _tmp2C5=loc;Cyc_Warn_err2(_tmp2C5,_tag_fat(_tmpC3,sizeof(void*),2));});});
# 379
if(({struct _fat_ptr _tmp2C6=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp2C6,({const char*_tmpC7="";_tag_fat(_tmpC7,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmpC8=_region_malloc(uprev_rgn,sizeof(*_tmpC8));_tmpC8->hd=fn,_tmpC8->tl=prev_fields;_tmpC8;});{
# 382
struct Cyc_Absyn_Kind*field_kind=& Cyc_Kinds_tmk;
# 386
if((int)str_or_union == (int)1U ||
 fs->tl == 0 &&(int)str_or_union == (int)0U)
field_kind=& Cyc_Kinds_tak;
Cyc_Tctyp_check_type(loc,te,tvs,field_kind,0,0,t);
# 391
({int _tmp2C7=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).print_const,t);(((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq).real_const=_tmp2C7;});
# 394
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
# 396
if((unsigned)requires_clause){
if((int)str_or_union != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20C;_tmp20C.tag=0,({struct _fat_ptr _tmp2C8=({const char*_tmpCB="@requires clauses are allowed only on union members";_tag_fat(_tmpCB,sizeof(char),52U);});_tmp20C.f1=_tmp2C8;});_tmp20C;});void*_tmpC9[1];_tmpC9[0]=& _tmpCA;({unsigned _tmp2C9=loc;Cyc_Warn_err2(_tmp2C9,_tag_fat(_tmpC9,sizeof(void*),1));});});{
struct Cyc_Tcenv_Tenv*_tmpCC=Cyc_Tcenv_allow_valueof(te);struct Cyc_Tcenv_Tenv*te2=_tmpCC;
Cyc_Tcexp_tcExp(te2,0,requires_clause);
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20F;_tmp20F.tag=0,({
struct _fat_ptr _tmp2CA=({const char*_tmpD2="@requires clause has type ";_tag_fat(_tmpD2,sizeof(char),27U);});_tmp20F.f1=_tmp2CA;});_tmp20F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp20E;_tmp20E.tag=2,_tmp20E.f1=(void*)_check_null(requires_clause->topt);_tmp20E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp20D;_tmp20D.tag=0,({
struct _fat_ptr _tmp2CB=({const char*_tmpD1=" instead of integral type";_tag_fat(_tmpD1,sizeof(char),26U);});_tmp20D.f1=_tmp2CB;});_tmp20D;});void*_tmpCD[3];_tmpCD[0]=& _tmpCE,_tmpCD[1]=& _tmpCF,_tmpCD[2]=& _tmpD0;({unsigned _tmp2CC=requires_clause->loc;Cyc_Warn_err2(_tmp2CC,_tag_fat(_tmpCD,sizeof(void*),3));});});else{
# 406
({unsigned _tmp2CF=requires_clause->loc;struct Cyc_Tcenv_Tenv*_tmp2CE=te;struct Cyc_List_List*_tmp2CD=tvs;Cyc_Tctyp_check_type(_tmp2CF,_tmp2CE,_tmp2CD,& Cyc_Kinds_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->tag=9,_tmpD3->f1=requires_clause;_tmpD3;}));});{
# 408
struct Cyc_List_List*_tmpD4=Cyc_Relations_exp2relns(uprev_rgn,requires_clause);struct Cyc_List_List*relns=_tmpD4;
# 411
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp210;_tmp210.tag=0,({struct _fat_ptr _tmp2D0=({const char*_tmpD7="@requires clause may be unsatisfiable";_tag_fat(_tmpD7,sizeof(char),38U);});_tmp210.f1=_tmp2D0;});_tmp210;});void*_tmpD5[1];_tmpD5[0]=& _tmpD6;({unsigned _tmp2D1=requires_clause->loc;Cyc_Warn_err2(_tmp2D1,_tag_fat(_tmpD5,sizeof(void*),1));});});
# 417
{struct Cyc_List_List*_tmpD8=prev_relations;struct Cyc_List_List*p=_tmpD8;for(0;p != 0;p=p->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rappend)(uprev_rgn,relns,(struct Cyc_List_List*)p->hd)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp211;_tmp211.tag=0,({
struct _fat_ptr _tmp2D2=({const char*_tmpDB="@requires clause may overlap with previous clauses";_tag_fat(_tmpDB,sizeof(char),51U);});_tmp211.f1=_tmp2D2;});_tmp211;});void*_tmpD9[1];_tmpD9[0]=& _tmpDA;({unsigned _tmp2D3=requires_clause->loc;Cyc_Warn_err2(_tmp2D3,_tag_fat(_tmpD9,sizeof(void*),1));});});}}
prev_relations=({struct Cyc_List_List*_tmpDC=_region_malloc(uprev_rgn,sizeof(*_tmpDC));_tmpDC->hd=relns,_tmpDC->tl=prev_relations;_tmpDC;});}}}}else{
# 424
if(prev_relations != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp212;_tmp212.tag=0,({struct _fat_ptr _tmp2D4=({const char*_tmpDF="if one field has a @requires clause, they all must";_tag_fat(_tmpDF,sizeof(char),51U);});_tmp212.f1=_tmp2D4;});_tmp212;});void*_tmpDD[1];_tmpDD[0]=& _tmpDE;({unsigned _tmp2D5=loc;Cyc_Warn_err2(_tmp2D5,_tag_fat(_tmpDD,sizeof(void*),1));});});}}}}}
# 363
;_pop_region();}
# 429
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 431
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmpE0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmpB=_tmpE0;void*_tmpE1=_stmttmpB;enum Cyc_Absyn_AliasQual _tmpE2;struct Cyc_Absyn_Kind*_tmpE4;struct Cyc_Core_Opt**_tmpE3;struct Cyc_Core_Opt**_tmpE5;enum Cyc_Absyn_AliasQual _tmpE7;struct Cyc_Core_Opt**_tmpE6;struct Cyc_Core_Opt**_tmpE8;switch(*((int*)_tmpE1)){case 1: _LL1: _tmpE8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpE1)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmpE8;
# 434
({struct Cyc_Core_Opt*_tmp2D7=({struct Cyc_Core_Opt*_tmpE9=_cycalloc(sizeof(*_tmpE9));({void*_tmp2D6=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_tmpE9->v=_tmp2D6;});_tmpE9;});*f=_tmp2D7;});continue;}case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE1)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmpE6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE1)->f1;_tmpE7=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE1)->f2)->aliasqual;_LL4: {struct Cyc_Core_Opt**f=_tmpE6;enum Cyc_Absyn_AliasQual a=_tmpE7;
# 436
if(constrain_top_kind &&(int)a == (int)2U)
({struct Cyc_Core_Opt*_tmp2D9=({struct Cyc_Core_Opt*_tmpEB=_cycalloc(sizeof(*_tmpEB));({void*_tmp2D8=Cyc_Kinds_kind_to_bound(({struct Cyc_Absyn_Kind*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->kind=Cyc_Absyn_BoxKind,_tmpEA->aliasqual=Cyc_Absyn_Aliasable;_tmpEA;}));_tmpEB->v=_tmp2D8;});_tmpEB;});*f=_tmp2D9;});else{
# 439
({struct Cyc_Core_Opt*_tmp2DB=({struct Cyc_Core_Opt*_tmpED=_cycalloc(sizeof(*_tmpED));({void*_tmp2DA=Cyc_Kinds_kind_to_bound(({struct Cyc_Absyn_Kind*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->kind=Cyc_Absyn_BoxKind,_tmpEC->aliasqual=a;_tmpEC;}));_tmpED->v=_tmp2DA;});_tmpED;});*f=_tmp2DB;});}
continue;}case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE1)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmpE5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE1)->f1;if(constrain_top_kind){_LL6: {struct Cyc_Core_Opt**f=_tmpE5;
# 442
({struct Cyc_Core_Opt*_tmp2DD=({struct Cyc_Core_Opt*_tmpEF=_cycalloc(sizeof(*_tmpEF));({void*_tmp2DC=Cyc_Kinds_kind_to_bound(({struct Cyc_Absyn_Kind*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->kind=Cyc_Absyn_BoxKind,_tmpEE->aliasqual=Cyc_Absyn_Aliasable;_tmpEE;}));_tmpEF->v=_tmp2DC;});_tmpEF;});*f=_tmp2DD;});
continue;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmpE3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE1)->f1;_tmpE4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE1)->f2;_LL8: {struct Cyc_Core_Opt**f=_tmpE3;struct Cyc_Absyn_Kind*k=_tmpE4;
# 445
({struct Cyc_Core_Opt*_tmp2DF=({struct Cyc_Core_Opt*_tmpF0=_cycalloc(sizeof(*_tmpF0));({void*_tmp2DE=Cyc_Kinds_kind_to_bound(k);_tmpF0->v=_tmp2DE;});_tmpF0;});*f=_tmp2DF;});continue;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpE1)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmpE2=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpE1)->f1)->aliasqual;_LLA: {enum Cyc_Absyn_AliasQual a=_tmpE2;
# 447
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp218;_tmp218.tag=0,({struct _fat_ptr _tmp2E0=({const char*_tmpFB="type ";_tag_fat(_tmpFB,sizeof(char),6U);});_tmp218.f1=_tmp2E0;});_tmp218;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp217;_tmp217.tag=1,_tmp217.f1=n;_tmp217;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp216;_tmp216.tag=0,({struct _fat_ptr _tmp2E1=({const char*_tmpFA=" attempts to abstract type variable ";_tag_fat(_tmpFA,sizeof(char),37U);});_tmp216.f1=_tmp2E1;});_tmp216;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp215;_tmp215.tag=7,_tmp215.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp215;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp214;_tmp214.tag=0,({
struct _fat_ptr _tmp2E2=({const char*_tmpF9=" of kind ";_tag_fat(_tmpF9,sizeof(char),10U);});_tmp214.f1=_tmp2E2;});_tmp214;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp213;_tmp213.tag=9,({struct Cyc_Absyn_Kind*_tmp2E3=({struct Cyc_Absyn_Kind*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->kind=Cyc_Absyn_MemKind,_tmpF8->aliasqual=a;_tmpF8;});_tmp213.f1=_tmp2E3;});_tmp213;});void*_tmpF1[6];_tmpF1[0]=& _tmpF2,_tmpF1[1]=& _tmpF3,_tmpF1[2]=& _tmpF4,_tmpF1[3]=& _tmpF5,_tmpF1[4]=& _tmpF6,_tmpF1[5]=& _tmpF7;({unsigned _tmp2E4=loc;Cyc_Warn_err2(_tmp2E4,_tag_fat(_tmpF1,sizeof(void*),6));});});
continue;}}else{_LLB: _LLC:
 continue;}}_LL0:;}}
# 453
static void Cyc_Tc_rule_out_mem_and_unique(unsigned loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmpFC=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_stmttmpC=_tmpFC;void*_tmpFD=_stmttmpC;enum Cyc_Absyn_AliasQual _tmpFE;enum Cyc_Absyn_KindQual _tmpFF;enum Cyc_Absyn_KindQual _tmp101;struct Cyc_Core_Opt**_tmp100;struct Cyc_Core_Opt**_tmp102;struct Cyc_Core_Opt**_tmp103;struct Cyc_Core_Opt**_tmp104;struct Cyc_Core_Opt**_tmp105;struct Cyc_Core_Opt**_tmp106;struct Cyc_Core_Opt**_tmp107;switch(*((int*)_tmpFD)){case 1: _LL1: _tmp107=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp107;
_tmp106=f;goto _LL4;}case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmp106=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp106;
# 458
_tmp105=f;goto _LL6;}case Cyc_Absyn_Aliasable: _LL5: _tmp105=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp105;
# 460
({struct Cyc_Core_Opt*_tmp2E6=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));({void*_tmp2E5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_tmp108->v=_tmp2E5;});_tmp108;});*f=_tmp2E6;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmp104=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp104;
_tmp103=f;goto _LLA;}case Cyc_Absyn_Aliasable: _LL9: _tmp103=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_LLA: {struct Cyc_Core_Opt**f=_tmp103;
# 463
({struct Cyc_Core_Opt*_tmp2E8=({struct Cyc_Core_Opt*_tmp109=_cycalloc(sizeof(*_tmp109));({void*_tmp2E7=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ak);_tmp109->v=_tmp2E7;});_tmp109;});*f=_tmp2E8;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmp102=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_LLC: {struct Cyc_Core_Opt**f=_tmp102;
# 465
({struct Cyc_Core_Opt*_tmp2EA=({struct Cyc_Core_Opt*_tmp10A=_cycalloc(sizeof(*_tmp10A));({void*_tmp2E9=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp10A->v=_tmp2E9;});_tmp10A;});*f=_tmp2EA;});goto _LL0;}case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmp100=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f1;_tmp101=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpFD)->f2)->kind;_LL10: {struct Cyc_Core_Opt**f=_tmp100;enum Cyc_Absyn_KindQual k=_tmp101;
# 469
_tmpFF=k;goto _LL12;}}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFD)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFD)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 467
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21D;_tmp21D.tag=0,({struct _fat_ptr _tmp2EB=({const char*_tmp113="type ";_tag_fat(_tmp113,sizeof(char),6U);});_tmp21D.f1=_tmp2EB;});_tmp21D;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp21C;_tmp21C.tag=1,_tmp21C.f1=q;_tmp21C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21B;_tmp21B.tag=0,({struct _fat_ptr _tmp2EC=({const char*_tmp112=" attempts to abstract type variable ";_tag_fat(_tmp112,sizeof(char),37U);});_tmp21B.f1=_tmp2EC;});_tmp21B;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp21A;_tmp21A.tag=7,_tmp21A.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp21A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp110=({struct Cyc_Warn_String_Warn_Warg_struct _tmp219;_tmp219.tag=0,({
struct _fat_ptr _tmp2ED=({const char*_tmp111=" of kind TR";_tag_fat(_tmp111,sizeof(char),12U);});_tmp219.f1=_tmp2ED;});_tmp219;});void*_tmp10B[5];_tmp10B[0]=& _tmp10C,_tmp10B[1]=& _tmp10D,_tmp10B[2]=& _tmp10E,_tmp10B[3]=& _tmp10F,_tmp10B[4]=& _tmp110;({unsigned _tmp2EE=loc;Cyc_Warn_err2(_tmp2EE,_tag_fat(_tmp10B,sizeof(void*),5));});});goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFD)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmpFF=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFD)->f1)->kind;_LL12: {enum Cyc_Absyn_KindQual k=_tmpFF;
# 471
({struct Cyc_Warn_String_Warn_Warg_struct _tmp115=({struct Cyc_Warn_String_Warn_Warg_struct _tmp223;_tmp223.tag=0,({struct _fat_ptr _tmp2EF=({const char*_tmp11E="type ";_tag_fat(_tmp11E,sizeof(char),6U);});_tmp223.f1=_tmp2EF;});_tmp223;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp116=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp222;_tmp222.tag=1,_tmp222.f1=q;_tmp222;});struct Cyc_Warn_String_Warn_Warg_struct _tmp117=({struct Cyc_Warn_String_Warn_Warg_struct _tmp221;_tmp221.tag=0,({struct _fat_ptr _tmp2F0=({const char*_tmp11D=" attempts to abstract type variable ";_tag_fat(_tmp11D,sizeof(char),37U);});_tmp221.f1=_tmp2F0;});_tmp221;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp118=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp220;_tmp220.tag=7,_tmp220.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp220;});struct Cyc_Warn_String_Warn_Warg_struct _tmp119=({struct Cyc_Warn_String_Warn_Warg_struct _tmp21F;_tmp21F.tag=0,({
struct _fat_ptr _tmp2F1=({const char*_tmp11C=" of kind ";_tag_fat(_tmp11C,sizeof(char),10U);});_tmp21F.f1=_tmp2F1;});_tmp21F;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp21E;_tmp21E.tag=9,({struct Cyc_Absyn_Kind*_tmp2F2=({struct Cyc_Absyn_Kind*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->kind=k,_tmp11B->aliasqual=Cyc_Absyn_Unique;_tmp11B;});_tmp21E.f1=_tmp2F2;});_tmp21E;});void*_tmp114[6];_tmp114[0]=& _tmp115,_tmp114[1]=& _tmp116,_tmp114[2]=& _tmp117,_tmp114[3]=& _tmp118,_tmp114[4]=& _tmp119,_tmp114[5]=& _tmp11A;({unsigned _tmp2F3=loc;Cyc_Warn_err2(_tmp2F3,_tag_fat(_tmp114,sizeof(void*),6));});});goto _LL0;}}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFD)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmpFE=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpFD)->f1)->aliasqual;_LL14: {enum Cyc_Absyn_AliasQual a=_tmpFE;
# 474
({struct Cyc_Warn_String_Warn_Warg_struct _tmp120=({struct Cyc_Warn_String_Warn_Warg_struct _tmp229;_tmp229.tag=0,({struct _fat_ptr _tmp2F4=({const char*_tmp129="type ";_tag_fat(_tmp129,sizeof(char),6U);});_tmp229.f1=_tmp2F4;});_tmp229;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp121=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp228;_tmp228.tag=1,_tmp228.f1=q;_tmp228;});struct Cyc_Warn_String_Warn_Warg_struct _tmp122=({struct Cyc_Warn_String_Warn_Warg_struct _tmp227;_tmp227.tag=0,({struct _fat_ptr _tmp2F5=({const char*_tmp128=" attempts to abstract type variable ";_tag_fat(_tmp128,sizeof(char),37U);});_tmp227.f1=_tmp2F5;});_tmp227;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp123=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp226;_tmp226.tag=7,_tmp226.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_tmp226;});struct Cyc_Warn_String_Warn_Warg_struct _tmp124=({struct Cyc_Warn_String_Warn_Warg_struct _tmp225;_tmp225.tag=0,({
struct _fat_ptr _tmp2F6=({const char*_tmp127=" of kind ";_tag_fat(_tmp127,sizeof(char),10U);});_tmp225.f1=_tmp2F6;});_tmp225;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp125=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp224;_tmp224.tag=9,({struct Cyc_Absyn_Kind*_tmp2F7=({struct Cyc_Absyn_Kind*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->kind=Cyc_Absyn_MemKind,_tmp126->aliasqual=a;_tmp126;});_tmp224.f1=_tmp2F7;});_tmp224;});void*_tmp11F[6];_tmp11F[0]=& _tmp120,_tmp11F[1]=& _tmp121,_tmp11F[2]=& _tmp122,_tmp11F[3]=& _tmp123,_tmp11F[4]=& _tmp124,_tmp11F[5]=& _tmp125;({unsigned _tmp2F8=loc;Cyc_Warn_err2(_tmp2F8,_tag_fat(_tmp11F,sizeof(void*),6));});});goto _LL0;}}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 482
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp12A=ad->name;struct _tuple0*q=_tmp12A;
# 488
Cyc_Atts_check_field_atts(loc,(*q).f2,ad->attributes);{
# 490
struct Cyc_List_List*_tmp12B=ad->tvs;struct Cyc_List_List*tvs=_tmp12B;
# 493
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 498
struct _tuple15 _tmp12C=({struct _tuple15 _tmp234;_tmp234.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp2F9=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,q);_tmp234.f2=_tmp2F9;});_tmp234;});struct _tuple15 _stmttmpD=_tmp12C;struct _tuple15 _tmp12D=_stmttmpD;struct Cyc_Absyn_Aggrdecl**_tmp132;int _tmp131;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp12E;int _tmp136;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp134;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_Aggrdecl**_tmp137;if(_tmp12D.f1 == 0){if(_tmp12D.f2 == 0){_LL1: _LL2:
# 501
 Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 503
({struct Cyc_Dict_Dict _tmp2FC=({struct Cyc_Dict_Dict _tmp2FB=(te->ae)->aggrdecls;struct _tuple0*_tmp2FA=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**))Cyc_Dict_insert)(_tmp2FB,_tmp2FA,({struct Cyc_Absyn_Aggrdecl**_tmp138=_cycalloc(sizeof(*_tmp138));*_tmp138=ad;_tmp138;}));});(te->ae)->aggrdecls=_tmp2FC;});
goto _LL0;}else{_LL7: _tmp137=*_tmp12D.f2;_LL8: {struct Cyc_Absyn_Aggrdecl**adp=_tmp137;
# 562
struct Cyc_Absyn_Aggrdecl*_tmp153=Cyc_Tcdecl_merge_aggrdecl(*adp,ad,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Aggrdecl*ad2=_tmp153;
if(ad2 == 0)
return;
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 568
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,q,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
*adp=ad2;}}}else{if(_tmp12D.f2 == 0){_LL3: _tmp133=(_tmp12D.f1)->exist_vars;_tmp134=(_tmp12D.f1)->rgn_po;_tmp135=(_tmp12D.f1)->fields;_tmp136=(_tmp12D.f1)->tagged;_LL4: {struct Cyc_List_List*exist_vars=_tmp133;struct Cyc_List_List*rgn_po=_tmp134;struct Cyc_List_List*fs=_tmp135;int tagged=_tmp136;
# 508
struct Cyc_Absyn_Aggrdecl**_tmp139=({struct Cyc_Absyn_Aggrdecl**_tmp14A=_cycalloc(sizeof(*_tmp14A));({struct Cyc_Absyn_Aggrdecl*_tmp2FD=({struct Cyc_Absyn_Aggrdecl*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->kind=ad->kind,_tmp149->sc=Cyc_Absyn_Extern,_tmp149->name=ad->name,_tmp149->tvs=tvs,_tmp149->impl=0,_tmp149->attributes=ad->attributes,_tmp149->expected_mem_kind=0;_tmp149;});*_tmp14A=_tmp2FD;});_tmp14A;});struct Cyc_Absyn_Aggrdecl**adp=_tmp139;
# 510
({struct Cyc_Dict_Dict _tmp2FE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**))Cyc_Dict_insert)((te->ae)->aggrdecls,q,adp);(te->ae)->aggrdecls=_tmp2FE;});
# 515
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 519
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22A;_tmp22A.tag=0,({struct _fat_ptr _tmp2FF=({const char*_tmp13C="@tagged is allowed only on union declarations";_tag_fat(_tmp13C,sizeof(char),46U);});_tmp22A.f1=_tmp2FF;});_tmp22A;});void*_tmp13A[1];_tmp13A[0]=& _tmp13B;({unsigned _tmp300=loc;Cyc_Warn_err2(_tmp300,_tag_fat(_tmp13A,sizeof(void*),1));});});
({struct Cyc_Tcenv_Tenv*_tmp305=te;unsigned _tmp304=loc;enum Cyc_Absyn_AggrKind _tmp303=ad->kind;struct Cyc_List_List*_tmp302=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp301=rgn_po;Cyc_Tc_tcAggrImpl(_tmp305,_tmp304,_tmp303,_tmp302,_tmp301,fs);});
# 523
Cyc_Tc_rule_out_memkind(loc,q,tvs,0);
# 526
Cyc_Tc_rule_out_memkind(loc,q,exist_vars,1);
# 528
if((int)ad->kind == (int)Cyc_Absyn_UnionA && !tagged){
# 531
struct Cyc_List_List*f=fs;for(0;f != 0;f=f->tl){
if((Cyc_Flags_tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp231;_tmp231.tag=0,({struct _fat_ptr _tmp306=({const char*_tmp148="member ";_tag_fat(_tmp148,sizeof(char),8U);});_tmp231.f1=_tmp306;});_tmp231;});struct Cyc_Warn_String_Warn_Warg_struct _tmp13F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp230;_tmp230.tag=0,_tmp230.f1=*((struct Cyc_Absyn_Aggrfield*)f->hd)->name;_tmp230;});struct Cyc_Warn_String_Warn_Warg_struct _tmp140=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22F;_tmp22F.tag=0,({struct _fat_ptr _tmp307=({const char*_tmp147=" of union ";_tag_fat(_tmp147,sizeof(char),11U);});_tmp22F.f1=_tmp307;});_tmp22F;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp141=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp22E;_tmp22E.tag=1,_tmp22E.f1=q;_tmp22E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp142=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22D;_tmp22D.tag=0,({struct _fat_ptr _tmp308=({const char*_tmp146=" has type ";_tag_fat(_tmp146,sizeof(char),11U);});_tmp22D.f1=_tmp308;});_tmp22D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp143=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp22C;_tmp22C.tag=2,_tmp22C.f1=(void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type;_tmp22C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp144=({struct Cyc_Warn_String_Warn_Warg_struct _tmp22B;_tmp22B.tag=0,({
struct _fat_ptr _tmp309=({const char*_tmp145=" so it can only be written and not read";_tag_fat(_tmp145,sizeof(char),40U);});_tmp22B.f1=_tmp309;});_tmp22B;});void*_tmp13D[7];_tmp13D[0]=& _tmp13E,_tmp13D[1]=& _tmp13F,_tmp13D[2]=& _tmp140,_tmp13D[3]=& _tmp141,_tmp13D[4]=& _tmp142,_tmp13D[5]=& _tmp143,_tmp13D[6]=& _tmp144;({unsigned _tmp30A=loc;Cyc_Warn_warn2(_tmp30A,_tag_fat(_tmp13D,sizeof(void*),7));});});}}
*adp=ad;
goto _LL0;}}else{_LL5: _tmp12E=(_tmp12D.f1)->exist_vars;_tmp12F=(_tmp12D.f1)->rgn_po;_tmp130=(_tmp12D.f1)->fields;_tmp131=(_tmp12D.f1)->tagged;_tmp132=*_tmp12D.f2;_LL6: {struct Cyc_List_List*exist_vars=_tmp12E;struct Cyc_List_List*rgn_po=_tmp12F;struct Cyc_List_List*fs=_tmp130;int tagged=_tmp131;struct Cyc_Absyn_Aggrdecl**adp=_tmp132;
# 540
if((int)ad->kind != (int)(*adp)->kind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp232;_tmp232.tag=0,({struct _fat_ptr _tmp30B=({const char*_tmp14D="cannot reuse struct names for unions and vice-versa";_tag_fat(_tmp14D,sizeof(char),52U);});_tmp232.f1=_tmp30B;});_tmp232;});void*_tmp14B[1];_tmp14B[0]=& _tmp14C;({unsigned _tmp30C=loc;Cyc_Warn_err2(_tmp30C,_tag_fat(_tmp14B,sizeof(void*),1));});});{
# 543
struct Cyc_Absyn_Aggrdecl*_tmp14E=*adp;struct Cyc_Absyn_Aggrdecl*ad0=_tmp14E;
# 545
({struct Cyc_Absyn_Aggrdecl*_tmp30D=({struct Cyc_Absyn_Aggrdecl*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->kind=ad->kind,_tmp14F->sc=Cyc_Absyn_Extern,_tmp14F->name=ad->name,_tmp14F->tvs=tvs,_tmp14F->impl=0,_tmp14F->attributes=ad->attributes,_tmp14F->expected_mem_kind=0;_tmp14F;});*adp=_tmp30D;});
# 551
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 554
if(tagged &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp151=({struct Cyc_Warn_String_Warn_Warg_struct _tmp233;_tmp233.tag=0,({struct _fat_ptr _tmp30E=({const char*_tmp152="@tagged is allowed only on union declarations";_tag_fat(_tmp152,sizeof(char),46U);});_tmp233.f1=_tmp30E;});_tmp233;});void*_tmp150[1];_tmp150[0]=& _tmp151;({unsigned _tmp30F=loc;Cyc_Warn_err2(_tmp30F,_tag_fat(_tmp150,sizeof(void*),1));});});
({struct Cyc_Tcenv_Tenv*_tmp314=te;unsigned _tmp313=loc;enum Cyc_Absyn_AggrKind _tmp312=ad->kind;struct Cyc_List_List*_tmp311=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(tvs,exist_vars);struct Cyc_List_List*_tmp310=rgn_po;Cyc_Tc_tcAggrImpl(_tmp314,_tmp313,_tmp312,_tmp311,_tmp310,fs);});
# 558
*adp=ad0;
_tmp137=adp;goto _LL8;}}}}_LL0:;}}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 574
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 582
{struct Cyc_List_List*_tmp154=fields;struct Cyc_List_List*fs=_tmp154;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp155=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp155;
struct Cyc_List_List*typs=f->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tctyp_check_type(f->loc,te,tvs,& Cyc_Kinds_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 587
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp157=({struct Cyc_Warn_String_Warn_Warg_struct _tmp236;_tmp236.tag=0,({struct _fat_ptr _tmp315=({const char*_tmp159="noalias pointers in datatypes are not allowed: ";_tag_fat(_tmp159,sizeof(char),48U);});_tmp236.f1=_tmp315;});_tmp236;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp158=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp235;_tmp235.tag=1,_tmp235.f1=f->name;_tmp235;});void*_tmp156[2];_tmp156[0]=& _tmp157,_tmp156[1]=& _tmp158;({unsigned _tmp316=f->loc;Cyc_Warn_err2(_tmp316,_tag_fat(_tmp156,sizeof(void*),2));});});
# 590
({int _tmp317=
Cyc_Tcutil_extract_const_from_typedef(f->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 590
((*((struct _tuple16*)typs->hd)).f1).real_const=_tmp317;});}}}
# 596
if(is_extensible){
# 598
int _tmp15A=1;int res=_tmp15A;
struct Cyc_List_List*_tmp15B=Cyc_Tcdecl_sort_xdatatype_fields(fields,& res,(*name).f2,loc,Cyc_Tc_tc_msg);struct Cyc_List_List*fs=_tmp15B;
return res?fs: 0;}{
# 602
struct _RegionHandle _tmp15C=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp15C;_push_region(uprev_rgn);
# 604
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp15D=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp15D;
if(((int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23A;_tmp23A.tag=0,({struct _fat_ptr _tmp318=({const char*_tmp164="duplicate field ";_tag_fat(_tmp164,sizeof(char),17U);});_tmp23A.f1=_tmp318;});_tmp23A;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp160=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp239;_tmp239.tag=1,_tmp239.f1=f->name;_tmp239;});struct Cyc_Warn_String_Warn_Warg_struct _tmp161=({struct Cyc_Warn_String_Warn_Warg_struct _tmp238;_tmp238.tag=0,({struct _fat_ptr _tmp319=({const char*_tmp163=" in ";_tag_fat(_tmp163,sizeof(char),5U);});_tmp238.f1=_tmp319;});_tmp238;});struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp237;_tmp237.tag=0,_tmp237.f1=obj;_tmp237;});void*_tmp15E[4];_tmp15E[0]=& _tmp15F,_tmp15E[1]=& _tmp160,_tmp15E[2]=& _tmp161,_tmp15E[3]=& _tmp162;({unsigned _tmp31A=f->loc;Cyc_Warn_err2(_tmp31A,_tag_fat(_tmp15E,sizeof(void*),4));});});else{
# 610
prev_fields=({struct Cyc_List_List*_tmp165=_region_malloc(uprev_rgn,sizeof(*_tmp165));_tmp165->hd=(*f->name).f2,_tmp165->tl=prev_fields;_tmp165;});}
# 612
if((int)f->sc != (int)Cyc_Absyn_Public){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp167=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23C;_tmp23C.tag=0,({struct _fat_ptr _tmp31B=({const char*_tmp169="ignoring scope of field ";_tag_fat(_tmp169,sizeof(char),25U);});_tmp23C.f1=_tmp31B;});_tmp23C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp168=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp23B;_tmp23B.tag=1,_tmp23B.f1=f->name;_tmp23B;});void*_tmp166[2];_tmp166[0]=& _tmp167,_tmp166[1]=& _tmp168;({unsigned _tmp31C=loc;Cyc_Warn_warn2(_tmp31C,_tag_fat(_tmp166,sizeof(void*),2));});});
f->sc=Cyc_Absyn_Public;}}}{
# 617
struct Cyc_List_List*_tmp16A=fields;_npop_handler(0);return _tmp16A;}}
# 604
;_pop_region();}}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 620
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _fat_ptr obj=tud->is_extensible?({const char*_tmp180="@extensible datatype";_tag_fat(_tmp180,sizeof(char),21U);}):({const char*_tmp181="datatype";_tag_fat(_tmp181,sizeof(char),9U);});
# 626
struct Cyc_List_List*tvs=tud->tvs;
# 629
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 635
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 645 "tc.cyc"
{struct _handler_cons _tmp16B;_push_handler(& _tmp16B);{int _tmp16D=0;if(setjmp(_tmp16B.handler))_tmp16D=1;if(!_tmp16D){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt == 0 && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->tag=Cyc_Dict_Absent;_tmp16E;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 651
({union Cyc_Absyn_Nmspace _tmp31D=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp31D;});}
# 646
;_pop_handler();}else{void*_tmp16C=(void*)Cyc_Core_get_exn_thrown();void*_tmp16F=_tmp16C;void*_tmp170;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16F)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 653
struct Cyc_Absyn_Datatypedecl***_tmp171=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);struct Cyc_Absyn_Datatypedecl***tdopt=_tmp171;
tud_opt=(unsigned)tdopt?({struct Cyc_Absyn_Datatypedecl***_tmp172=_cycalloc(sizeof(*_tmp172));*_tmp172=*tdopt;_tmp172;}): 0;
goto _LL0;}}else{_LL3: _tmp170=_tmp16F;_LL4: {void*exn=_tmp170;(int)_rethrow(exn);}}_LL0:;}}}{
# 660
struct _tuple17 _tmp173=({struct _tuple17 _tmp23D;_tmp23D.f1=tud->fields,_tmp23D.f2=tud_opt;_tmp23D;});struct _tuple17 _stmttmpE=_tmp173;struct _tuple17 _tmp174=_stmttmpE;struct Cyc_Absyn_Datatypedecl**_tmp176;struct Cyc_List_List**_tmp175;struct Cyc_List_List**_tmp177;struct Cyc_Absyn_Datatypedecl**_tmp178;if(_tmp174.f1 == 0){if(_tmp174.f2 == 0){_LL6: _LL7:
# 663
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp320=({struct Cyc_Dict_Dict _tmp31F=(te->ae)->datatypedecls;struct _tuple0*_tmp31E=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert)(_tmp31F,_tmp31E,({struct Cyc_Absyn_Datatypedecl**_tmp179=_cycalloc(sizeof(*_tmp179));*_tmp179=tud;_tmp179;}));});(te->ae)->datatypedecls=_tmp320;});
goto _LL5;}else{_LLC: _tmp178=*_tmp174.f2;_LLD: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp178;
# 696
struct Cyc_Absyn_Datatypedecl*_tmp17F=Cyc_Tcdecl_merge_datatypedecl(*tudp,tud,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Datatypedecl*tud2=_tmp17F;
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(tud2 != 0)
*tudp=tud2;
goto _LL5;}}}else{if(_tmp174.f2 == 0){_LL8: _tmp177=(struct Cyc_List_List**)&(_tmp174.f1)->v;_LL9: {struct Cyc_List_List**fs=_tmp177;
# 668
struct Cyc_Absyn_Datatypedecl**_tmp17A=({struct Cyc_Absyn_Datatypedecl**_tmp17C=_cycalloc(sizeof(*_tmp17C));({struct Cyc_Absyn_Datatypedecl*_tmp321=({struct Cyc_Absyn_Datatypedecl*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->sc=Cyc_Absyn_Extern,_tmp17B->name=tud->name,_tmp17B->tvs=tvs,_tmp17B->fields=0,_tmp17B->is_extensible=tud->is_extensible;_tmp17B;});*_tmp17C=_tmp321;});_tmp17C;});struct Cyc_Absyn_Datatypedecl**tudp=_tmp17A;
# 670
({struct Cyc_Dict_Dict _tmp322=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert)((te->ae)->datatypedecls,q,tudp);(te->ae)->datatypedecls=_tmp322;});
# 673
({struct Cyc_List_List*_tmp323=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp323;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*tudp=tud;
goto _LL5;}}else{_LLA: _tmp175=(struct Cyc_List_List**)&(_tmp174.f1)->v;_tmp176=*_tmp174.f2;_LLB: {struct Cyc_List_List**fs=_tmp175;struct Cyc_Absyn_Datatypedecl**tudp=_tmp176;
# 678
struct Cyc_Absyn_Datatypedecl*_tmp17D=*tudp;struct Cyc_Absyn_Datatypedecl*tud0=_tmp17D;
# 681
if((!tud->is_extensible &&(unsigned)tud0)&& tud0->is_extensible)
tud->is_extensible=1;
# 684
({struct Cyc_Absyn_Datatypedecl*_tmp324=({struct Cyc_Absyn_Datatypedecl*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->sc=Cyc_Absyn_Extern,_tmp17E->name=tud->name,_tmp17E->tvs=tvs,_tmp17E->fields=0,_tmp17E->is_extensible=tud->is_extensible;_tmp17E;});*tudp=_tmp324;});
# 688
({struct Cyc_List_List*_tmp325=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_tmp325;});
# 691
*tudp=tud0;
_tmp178=tudp;goto _LLD;}}}_LL5:;}}}
# 704
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 710
if(ed->fields != 0){
struct _RegionHandle _tmp182=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp182;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp183=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp183;
# 717
if(((int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp185=({struct Cyc_Warn_String_Warn_Warg_struct _tmp23F;_tmp23F.tag=0,({struct _fat_ptr _tmp326=({const char*_tmp187="duplicate enum constructor ";_tag_fat(_tmp187,sizeof(char),28U);});_tmp23F.f1=_tmp326;});_tmp23F;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp186=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp23E;_tmp23E.tag=1,_tmp23E.f1=f->name;_tmp23E;});void*_tmp184[2];_tmp184[0]=& _tmp185,_tmp184[1]=& _tmp186;({unsigned _tmp327=f->loc;Cyc_Warn_err2(_tmp327,_tag_fat(_tmp184,sizeof(void*),2));});});else{
# 720
prev_fields=({struct Cyc_List_List*_tmp188=_region_malloc(uprev_rgn,sizeof(*_tmp188));_tmp188->hd=(*f->name).f2,_tmp188->tl=prev_fields;_tmp188;});}
# 722
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,f->name)!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp242;_tmp242.tag=0,({struct _fat_ptr _tmp328=({const char*_tmp18E="enum field name ";_tag_fat(_tmp18E,sizeof(char),17U);});_tmp242.f1=_tmp328;});_tmp242;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp18B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp241;_tmp241.tag=1,_tmp241.f1=f->name;_tmp241;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp240;_tmp240.tag=0,({struct _fat_ptr _tmp329=({const char*_tmp18D=" shadows global name";_tag_fat(_tmp18D,sizeof(char),21U);});_tmp240.f1=_tmp329;});_tmp240;});void*_tmp189[3];_tmp189[0]=& _tmp18A,_tmp189[1]=& _tmp18B,_tmp189[2]=& _tmp18C;({unsigned _tmp32A=f->loc;Cyc_Warn_err2(_tmp32A,_tag_fat(_tmp189,sizeof(void*),3));});});
if(f->tag == 0)
({struct Cyc_Absyn_Exp*_tmp32B=Cyc_Absyn_uint_exp(tag_count ++,f->loc);f->tag=_tmp32B;});else{
# 727
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))){
struct _tuple11 _tmp18F=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag));struct _tuple11 _stmttmpF=_tmp18F;struct _tuple11 _tmp190=_stmttmpF;int _tmp192;unsigned _tmp191;_LL1: _tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_LL2: {unsigned t1=_tmp191;int known=_tmp192;
if(known)
tag_count=t1 + (unsigned)1;}}}}}
# 712
;_pop_region();}
# 736
{struct _handler_cons _tmp193;_push_handler(& _tmp193);{int _tmp195=0;if(setjmp(_tmp193.handler))_tmp195=1;if(!_tmp195){
{struct Cyc_Absyn_Enumdecl**_tmp196=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)((te->ae)->enumdecls,q);struct Cyc_Absyn_Enumdecl**edp=_tmp196;
struct Cyc_Absyn_Enumdecl*_tmp197=Cyc_Tcdecl_merge_enumdecl(*edp,ed,loc,Cyc_Tc_tc_msg);struct Cyc_Absyn_Enumdecl*ed2=_tmp197;
if(ed2 == 0){_npop_handler(0);return;}
*edp=ed2;}
# 737
;_pop_handler();}else{void*_tmp194=(void*)Cyc_Core_get_exn_thrown();void*_tmp198=_tmp194;void*_tmp199;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp198)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 742
struct Cyc_Absyn_Enumdecl**_tmp19A=({struct Cyc_Absyn_Enumdecl**_tmp19B=_cycalloc(sizeof(*_tmp19B));*_tmp19B=ed;_tmp19B;});struct Cyc_Absyn_Enumdecl**edp=_tmp19A;
({struct Cyc_Dict_Dict _tmp32C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Enumdecl**))Cyc_Dict_insert)((te->ae)->enumdecls,q,edp);(te->ae)->enumdecls=_tmp32C;});
goto _LL3;}}else{_LL6: _tmp199=_tmp198;_LL7: {void*exn=_tmp199;(int)_rethrow(exn);}}_LL3:;}}}
# 748
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp19C=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp19C;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp247;_tmp247.tag=0,({struct _fat_ptr _tmp32D=({const char*_tmp1A5="enum ";_tag_fat(_tmp1A5,sizeof(char),6U);});_tmp247.f1=_tmp32D;});_tmp247;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp246;_tmp246.tag=1,_tmp246.f1=q;_tmp246;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp245;_tmp245.tag=0,({struct _fat_ptr _tmp32E=({const char*_tmp1A4=", field ";_tag_fat(_tmp1A4,sizeof(char),9U);});_tmp245.f1=_tmp32E;});_tmp245;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp244;_tmp244.tag=1,_tmp244.f1=f->name;_tmp244;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp243;_tmp243.tag=0,({
struct _fat_ptr _tmp32F=({const char*_tmp1A3=": expression is not constant";_tag_fat(_tmp1A3,sizeof(char),29U);});_tmp243.f1=_tmp32F;});_tmp243;});void*_tmp19D[5];_tmp19D[0]=& _tmp19E,_tmp19D[1]=& _tmp19F,_tmp19D[2]=& _tmp1A0,_tmp19D[3]=& _tmp1A1,_tmp19D[4]=& _tmp1A2;({unsigned _tmp330=loc;Cyc_Warn_err2(_tmp330,_tag_fat(_tmp19D,sizeof(void*),5));});});}}}
# 758
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include,int in_inc_rep){
enum Cyc_Absyn_Scope _tmp1A6=sc;switch(_tmp1A6){case Cyc_Absyn_Static: _LL1: _LL2:
# 761
 if(!in_include)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp248;_tmp248.tag=0,({struct _fat_ptr _tmp331=({const char*_tmp1A9="static declaration within extern \"C\"";_tag_fat(_tmp1A9,sizeof(char),37U);});_tmp248.f1=_tmp331;});_tmp248;});void*_tmp1A7[1];_tmp1A7[0]=& _tmp1A8;({unsigned _tmp332=loc;Cyc_Warn_warn2(_tmp332,_tag_fat(_tmp1A7,sizeof(void*),1));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 765
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp249;_tmp249.tag=0,({struct _fat_ptr _tmp333=({const char*_tmp1AC="abstract declaration within extern \"C\"";_tag_fat(_tmp1AC,sizeof(char),39U);});_tmp249.f1=_tmp333;});_tmp249;});void*_tmp1AA[1];_tmp1AA[0]=& _tmp1AB;({unsigned _tmp334=loc;Cyc_Warn_warn2(_tmp334,_tag_fat(_tmp1AA,sizeof(void*),1));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;case Cyc_Absyn_ExternC: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
# 772
 if(!in_inc_rep)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24A;_tmp24A.tag=0,({struct _fat_ptr _tmp335=({const char*_tmp1AF="nested extern \"C\" declaration";_tag_fat(_tmp1AF,sizeof(char),30U);});_tmp24A.f1=_tmp335;});_tmp24A;});void*_tmp1AD[1];_tmp1AD[0]=& _tmp1AE;({unsigned _tmp336=loc;Cyc_Warn_warn2(_tmp336,_tag_fat(_tmp1AD,sizeof(void*),1));});});
return 1;}_LL0:;}
# 778
static void Cyc_Tc_set_scopes(struct Cyc_List_List*ovrs,enum Cyc_Absyn_Scope sc){
for(1;ovrs != 0;ovrs=ovrs->tl){
void*_tmp1B0=((struct Cyc_Absyn_Decl*)ovrs->hd)->r;void*_stmttmp10=_tmp1B0;void*_tmp1B1=_stmttmp10;struct Cyc_Absyn_Enumdecl*_tmp1B2;struct Cyc_Absyn_Datatypedecl*_tmp1B3;struct Cyc_Absyn_Aggrdecl*_tmp1B4;struct Cyc_Absyn_Fndecl*_tmp1B5;struct Cyc_Absyn_Vardecl*_tmp1B6;switch(*((int*)_tmp1B1)){case 0: _LL1: _tmp1B6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1B1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1B6;
vd->sc=sc;goto _LL0;}case 1: _LL3: _tmp1B5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1B1)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp1B5;
fd->sc=sc;goto _LL0;}case 5: _LL5: _tmp1B4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1B1)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1B4;
ad->sc=sc;goto _LL0;}case 6: _LL7: _tmp1B3=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1B1)->f1;_LL8: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1B3;
tud->sc=sc;goto _LL0;}case 7: _LL9: _tmp1B2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1B1)->f1;_LLA: {struct Cyc_Absyn_Enumdecl*ed=_tmp1B2;
ed->sc=sc;goto _LL0;}default: _LLB: _LLC:
 goto _LL0;}_LL0:;}}
# 795
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 799
struct Cyc_List_List*_tmp1B7=ds0;struct Cyc_List_List*ds=_tmp1B7;for(0;ds != 0;ds=ds->tl){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
unsigned loc=d->loc;
void*_tmp1B8=d->r;void*_stmttmp11=_tmp1B8;void*_tmp1B9=_stmttmp11;struct _tuple10*_tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List**_tmp1BA;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*_tmp1BF;struct Cyc_List_List*_tmp1C1;struct _fat_ptr*_tmp1C0;struct Cyc_Absyn_Enumdecl*_tmp1C2;struct Cyc_Absyn_Datatypedecl*_tmp1C3;struct Cyc_Absyn_Aggrdecl*_tmp1C4;struct Cyc_Absyn_Typedefdecl*_tmp1C5;struct Cyc_Absyn_Fndecl*_tmp1C6;struct Cyc_Absyn_Vardecl*_tmp1C7;switch(*((int*)_tmp1B9)){case 2: _LL1: _LL2:
 goto _LL4;case 3: _LL3: _LL4:
# 805
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24B;_tmp24B.tag=0,({struct _fat_ptr _tmp337=({const char*_tmp1CA="top level let-declarations are not implemented";_tag_fat(_tmp1CA,sizeof(char),47U);});_tmp24B.f1=_tmp337;});_tmp24B;});void*_tmp1C8[1];_tmp1C8[0]=& _tmp1C9;({unsigned _tmp338=loc;Cyc_Warn_err2(_tmp338,_tag_fat(_tmp1C8,sizeof(void*),1));});});goto _LL0;case 4: _LL5: _LL6:
# 807
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24C;_tmp24C.tag=0,({struct _fat_ptr _tmp339=({const char*_tmp1CD="top level region declarations are not implemented";_tag_fat(_tmp1CD,sizeof(char),50U);});_tmp24C.f1=_tmp339;});_tmp24C;});void*_tmp1CB[1];_tmp1CB[0]=& _tmp1CC;({unsigned _tmp33A=loc;Cyc_Warn_err2(_tmp33A,_tag_fat(_tmp1CB,sizeof(void*),1));});});goto _LL0;case 0: _LL7: _tmp1C7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp1C7;
# 809
if(in_externC && Cyc_Tc_okay_externC(d->loc,vd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
vd->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,vd,check_var_init,te->in_extern_c_include,exports);
goto _LL0;}case 1: _LL9: _tmp1C6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_LLA: {struct Cyc_Absyn_Fndecl*fd=_tmp1C6;
# 814
if(in_externC && Cyc_Tc_okay_externC(d->loc,fd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
fd->sc=Cyc_Absyn_ExternC;
if(te->in_extern_c_include)
fd->orig_scope=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,fd,exports);
goto _LL0;}case 8: _LLB: _tmp1C5=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp1C5;
# 821
td->extern_c=te->in_extern_c_include;
# 825
Cyc_Tc_tcTypedefdecl(te,loc,td);
goto _LL0;}case 5: _LLD: _tmp1C4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1C4;
# 828
if(in_externC && Cyc_Tc_okay_externC(d->loc,ad->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);
goto _LL0;}case 6: _LLF: _tmp1C3=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_LL10: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1C3;
# 833
if(in_externC && Cyc_Tc_okay_externC(d->loc,tud->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
tud->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
goto _LL0;}case 7: _LL11: _tmp1C2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*ed=_tmp1C2;
# 838
if(in_externC && Cyc_Tc_okay_externC(d->loc,ed->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);
goto _LL0;}case 13: _LL13: _LL14:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24D;_tmp24D.tag=0,({struct _fat_ptr _tmp33B=({const char*_tmp1D0="spurious __cyclone_port_on__";_tag_fat(_tmp1D0,sizeof(char),29U);});_tmp24D.f1=_tmp33B;});_tmp24D;});void*_tmp1CE[1];_tmp1CE[0]=& _tmp1CF;({unsigned _tmp33C=d->loc;Cyc_Warn_warn2(_tmp33C,_tag_fat(_tmp1CE,sizeof(void*),1));});});goto _LL0;case 14: _LL15: _LL16:
 goto _LL0;case 15: _LL17: _LL18:
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16: _LL19: _LL1A:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9: _LL1B: _tmp1C0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_tmp1C1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1B9)->f2;_LL1C: {struct _fat_ptr*v=_tmp1C0;struct Cyc_List_List*ds2=_tmp1C1;
# 848
struct Cyc_List_List*_tmp1D1=te->ns;struct Cyc_List_List*ns=_tmp1D1;
({struct Cyc_List_List*_tmp33E=({struct Cyc_List_List*_tmp33D=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp33D,({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->hd=v,_tmp1D2->tl=0;_tmp1D2;}));});te->ns=_tmp33E;});
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);
te->ns=ns;
goto _LL0;}case 10: _LL1D: _tmp1BF=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1B9)->f2;_LL1E: {struct Cyc_List_List*ds2=_tmp1BF;
# 855
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11: _LL1F: _tmp1BE=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_LL20: {struct Cyc_List_List*ds2=_tmp1BE;
# 858
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default: _LL21: _tmp1BA=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1B9)->f1;_tmp1BB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1B9)->f2;_tmp1BC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1B9)->f3;_tmp1BD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1B9)->f4;_LL22: {struct Cyc_List_List**ds2=_tmp1BA;struct Cyc_List_List*ovrs=_tmp1BB;struct Cyc_List_List*exports2=_tmp1BC;struct _tuple10*wc=_tmp1BD;
# 863
if((unsigned)ovrs){
struct Cyc_Tcenv_Tenv*_tmp1D3=Cyc_Tcenv_copy_tenv_dicts(te);struct Cyc_Tcenv_Tenv*tecpy=_tmp1D3;
tecpy->in_extern_c_include=1;
Cyc_Tc_tc_decls(tecpy,*ds2,1,check_var_init,0);
Cyc_Toc_init();
Cyc_Tc_set_scopes(ovrs,Cyc_Absyn_ExternC);
Cyc_Cifc_user_overrides(d->loc,tecpy,ds2,ovrs);
Cyc_Toc_finish();}{
# 872
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(exports2,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1D4=Cyc_Tcenv_enter_extern_c_include(te);struct Cyc_Tcenv_Tenv*te2=_tmp1D4;
te2->in_extern_c_inc_repeat=ovrs != 0;
Cyc_Tc_tc_decls(te2,*ds2,1,check_var_init,& newexs);
# 879
for(1;exports2 != 0;exports2=exports2->tl){
struct _tuple12*_tmp1D5=(struct _tuple12*)exports2->hd;struct _tuple12*exp=_tmp1D5;
if(!(*exp).f3)
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1D7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp24F;_tmp24F.tag=1,_tmp24F.f1=(*exp).f2;_tmp24F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24E;_tmp24E.tag=0,({struct _fat_ptr _tmp33F=({const char*_tmp1D9=" is exported but not defined";_tag_fat(_tmp1D9,sizeof(char),29U);});_tmp24E.f1=_tmp33F;});_tmp24E;});void*_tmp1D6[2];_tmp1D6[0]=& _tmp1D7,_tmp1D6[1]=& _tmp1D8;({unsigned _tmp340=(*exp).f1;Cyc_Warn_warn2(_tmp340,_tag_fat(_tmp1D6,sizeof(void*),2));});});}
# 884
goto _LL0;}}}_LL0:;}}
# 889
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict _tmp1DA=Cyc_Callgraph_compute_callgraph(ds);struct Cyc_Dict_Dict cg=_tmp1DA;
# 894
struct Cyc_Dict_Dict _tmp1DB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Graph_scc)(cg);struct Cyc_Dict_Dict scc=_tmp1DB;
# 897
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 915 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*);
# 917
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*,struct Cyc_List_List*))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 921
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){
if((int)vd->sc == (int)Cyc_Absyn_Extern ||(int)vd->sc == (int)Cyc_Absyn_ExternC)
return 1;{
void*_tmp1DC=Cyc_Absyn_compress(vd->type);void*_stmttmp12=_tmp1DC;void*_tmp1DD=_stmttmp12;if(*((int*)_tmp1DD)== 5){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 930
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1DE=d->r;void*_stmttmp13=_tmp1DE;void*_tmp1DF=_stmttmp13;struct Cyc_List_List**_tmp1E0;struct Cyc_List_List**_tmp1E1;struct Cyc_List_List**_tmp1E2;struct Cyc_List_List**_tmp1E3;struct Cyc_Absyn_Vardecl*_tmp1E4;switch(*((int*)_tmp1DF)){case 0: _LL1: _tmp1E4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1DF)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp1E4;
# 934
if((env->in_cinclude || !Cyc_Tc_is_extern(vd))|| !({
struct _tuple0*_tmp341=vd->name;Cyc_Absyn_qvar_cmp(_tmp341,Cyc_Absyn_uniquergn_qvar());}))
return 1;
return(*((struct _tuple13*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup)(env->ordinaries,vd->name)).f2;}case 11: _LL3: _tmp1E3=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1DF)->f1;_LL4: {struct Cyc_List_List**ds2p=_tmp1E3;
_tmp1E2=ds2p;goto _LL6;}case 10: _LL5: _tmp1E2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1DF)->f2;_LL6: {struct Cyc_List_List**ds2p=_tmp1E2;
_tmp1E1=ds2p;goto _LL8;}case 9: _LL7: _tmp1E1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1DF)->f2;_LL8: {struct Cyc_List_List**ds2p=_tmp1E1;
({struct Cyc_List_List*_tmp342=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp342;});return 1;}case 12: _LL9: _tmp1E0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1DF)->f1;_LLA: {struct Cyc_List_List**ds2p=_tmp1E0;
# 942
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp343=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_tmp343;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 951
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1E5=({struct Cyc_Tc_TreeshakeEnv _tmp250;_tmp250.in_cinclude=0,_tmp250.ordinaries=(te->ae)->ordinaries;_tmp250;});struct Cyc_Tc_TreeshakeEnv env=_tmp1E5;
return Cyc_Tc_treeshake_f(& env,ds);}

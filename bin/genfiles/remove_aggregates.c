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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153 "absyn.h"
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
# 894 "absyn.h"
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 953
void*Cyc_Absyn_strctq(struct _tuple0*);
void*Cyc_Absyn_unionq_type(struct _tuple0*);
# 968
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 975
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
# 977
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
# 982
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1040
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1048
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1051
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1135
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_Hashtable_Table;
# 41 "toc.h"
struct _tuple0*Cyc_Toc_temp_var (void);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 46 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 49 "remove_aggregates.cyc"
static void Cyc_RemoveAggrs_massage_toplevel_aggr(struct Cyc_Absyn_Exp*e){
if(e == 0)
return;}
# 57
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;if(*((int*)_tmp1)== 20){_tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{
return Cyc_Absyn_aggrmember_exp(e,f,loc);};}
# 66
enum Cyc_RemoveAggrs_ExpContext{Cyc_RemoveAggrs_Initializer =0U,Cyc_RemoveAggrs_NewDest =1U,Cyc_RemoveAggrs_AggrField =2U,Cyc_RemoveAggrs_Other =3U};struct Cyc_RemoveAggrs_Env{enum Cyc_RemoveAggrs_ExpContext ctxt;struct Cyc_Absyn_Exp*dest;};
# 77
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_other_env={Cyc_RemoveAggrs_Other,0};
# 79
enum Cyc_RemoveAggrs_ExpResult{Cyc_RemoveAggrs_WasArray =0U,Cyc_RemoveAggrs_OtherRes =1U};struct Cyc_RemoveAggrs_Result{enum Cyc_RemoveAggrs_ExpResult res;};
# 88
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env,struct Cyc_Absyn_Exp*);
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*);
# 91
static void Cyc_RemoveAggrs_noarray_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
struct Cyc_RemoveAggrs_Result _tmp3=Cyc_RemoveAggrs_remove_aggrs_exp(env,e);struct Cyc_RemoveAggrs_Result _stmttmp1=_tmp3;struct Cyc_RemoveAggrs_Result _tmp4=_stmttmp1;enum Cyc_RemoveAggrs_ExpResult _tmp5;_tmp5=_tmp4.res;{enum Cyc_RemoveAggrs_ExpResult r=_tmp5;
if((int)r != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp88;_tmp88.tag=0,({struct _fat_ptr _tmp93=({const char*_tmp9="remove_aggrs_exp -- unexpected array or comprehension: ";_tag_fat(_tmp9,sizeof(char),56U);});_tmp88.f1=_tmp93;});_tmp88;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp8=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp87;_tmp87.tag=4,_tmp87.f1=e;_tmp87;});void*_tmp6[2];_tmp6[0]=& _tmp7,_tmp6[1]=& _tmp8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp6,sizeof(void*),2));});}}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 97
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
enum Cyc_RemoveAggrs_ExpResult res=1U;
int did_assign=0;
# 101
{void*_tmpA=e->r;void*_stmttmp2=_tmpA;void*_tmpB=_stmttmp2;struct Cyc_Absyn_Stmt*_tmpC;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp26;int _tmp2A;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Vardecl*_tmp27;struct Cyc_List_List*_tmp2B;struct Cyc_Absyn_Aggrdecl*_tmp2E;struct Cyc_List_List*_tmp2D;struct _tuple0*_tmp2C;struct Cyc_Absyn_Exp*_tmp2F;struct _fat_ptr _tmp30;switch(*((int*)_tmpB)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB)->f1).String_c).tag == 8){_tmp30=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB)->f1).String_c).val;{struct _fat_ptr s=_tmp30;
# 104
if((int)env.ctxt == (int)Cyc_RemoveAggrs_AggrField && Cyc_Tcutil_is_array_type((void*)_check_null(e->topt))){
struct Cyc_List_List*dles=0;
unsigned _tmp31=_get_fat_size(s,sizeof(char));unsigned n=_tmp31;
{unsigned i=0U;for(0;i < n;++ i){
struct Cyc_Absyn_Exp*_tmp32=Cyc_Absyn_char_exp(((const char*)s.curr)[(int)i],0U);struct Cyc_Absyn_Exp*c=_tmp32;
c->topt=Cyc_Absyn_char_type;
dles=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));({struct _tuple11*_tmp94=({struct _tuple11*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=0,_tmp33->f2=c;_tmp33;});_tmp34->hd=_tmp94;}),_tmp34->tl=dles;_tmp34;});}}
# 112
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);
({void*_tmp95=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=26,_tmp35->f1=dles;_tmp35;});e->r=_tmp95;});
return Cyc_RemoveAggrs_remove_aggrs_exp(env,e);}
# 116
goto _LL0;}}else{
goto _LL6;}case 17: _LL6:
 goto _LL8;case 19: _LL8:
 goto _LLA;case 33: _LLA:
 goto _LLC;case 32: _LLC:
 goto _LLE;case 40: _LLE:
 goto _LL10;case 1: _LL10:
 goto _LL0;case 16: _tmp2F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e1=_tmp2F;
# 126
{void*_tmp36=e->annot;void*_stmttmp3=_tmp36;void*_tmp37=_stmttmp3;struct Cyc_Absyn_Exp*_tmp38;if(((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmp37)->tag == Cyc_Toc_Dest){_tmp38=((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmp37)->f1;{struct Cyc_Absyn_Exp*dest=_tmp38;
# 128
({struct Cyc_RemoveAggrs_Env _tmp96=({struct Cyc_RemoveAggrs_Env _tmp89;_tmp89.ctxt=Cyc_RemoveAggrs_NewDest,_tmp89.dest=dest;_tmp89;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp96,e1);});
*e=*e1;
goto _LL41;}}else{
({void*_tmp39=0U;({struct _fat_ptr _tmp97=({const char*_tmp3A="removeAggrs: toc did not set a new-destination";_tag_fat(_tmp3A,sizeof(char),47U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp97,_tag_fat(_tmp39,sizeof(void*),0));});});}_LL41:;}
# 133
goto _LL0;}case 29: _tmp2C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_tmp2E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f4;{struct _tuple0*tdn=_tmp2C;struct Cyc_List_List*dles=_tmp2D;struct Cyc_Absyn_Aggrdecl*sdopt=_tmp2E;
# 136
did_assign=1;{
int do_stmt_exp=(int)env.ctxt != (int)Cyc_RemoveAggrs_AggrField &&(int)env.ctxt != (int)Cyc_RemoveAggrs_NewDest;
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;
if(do_stmt_exp){
v=({struct _tuple0**_tmp3B=_cycalloc(sizeof(*_tmp3B));({struct _tuple0*_tmp98=Cyc_Toc_temp_var();*_tmp3B=_tmp98;});_tmp3B;});
dest=Cyc_Absyn_var_exp(*v,0U);}else{
# 144
v=0;
dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
if((int)env.ctxt == (int)Cyc_RemoveAggrs_NewDest)
dest=Cyc_Absyn_deref_exp(dest,0U);}
# 150
{struct Cyc_List_List*_tmp3C=dles;struct Cyc_List_List*dles2=_tmp3C;for(0;dles2 != 0;dles2=dles2->tl){
struct _tuple11*_tmp3D=(struct _tuple11*)dles2->hd;struct _tuple11*_stmttmp4=_tmp3D;struct _tuple11*_tmp3E=_stmttmp4;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_List_List*_tmp3F;_tmp3F=_tmp3E->f1;_tmp40=_tmp3E->f2;{struct Cyc_List_List*ds=_tmp3F;struct Cyc_Absyn_Exp*field_exp=_tmp40;
struct _fat_ptr*_tmp41=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp41;
struct Cyc_Absyn_Exp*_tmp42=({struct Cyc_Absyn_Exp*_tmp99=Cyc_Absyn_copy_exp(dest);Cyc_RemoveAggrs_member_exp(_tmp99,f,0U);});struct Cyc_Absyn_Exp*field_dest=_tmp42;
({struct Cyc_RemoveAggrs_Env _tmp9A=({struct Cyc_RemoveAggrs_Env _tmp8A;_tmp8A.ctxt=Cyc_RemoveAggrs_AggrField,_tmp8A.dest=field_dest;_tmp8A;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp9A,field_exp);});}}}
# 157
if(dles == 0)
({void*_tmp43=0U;({struct _fat_ptr _tmp9B=({const char*_tmp44="zero-field aggregate";_tag_fat(_tmp44,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp9B,_tag_fat(_tmp43,sizeof(void*),0));});});{
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple11*)dles->hd)).f2;
for(dles=dles->tl;dles != 0;dles=dles->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple11*)dles->hd)).f2,0U);}
if(do_stmt_exp){
void*(*_tmp45)(struct _tuple0*)=(unsigned)sdopt &&(int)sdopt->kind == (int)Cyc_Absyn_UnionA?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq;void*(*f)(struct _tuple0*)=_tmp45;
({void*_tmp9F=(Cyc_Absyn_stmt_exp(({struct _tuple0*_tmp9E=*((struct _tuple0**)_check_null(v));void*_tmp9D=f(tdn);Cyc_Absyn_declare_stmt(_tmp9E,_tmp9D,0,({
struct Cyc_Absyn_Stmt*_tmp9C=Cyc_Absyn_exp_stmt(init_e,0U);Cyc_Absyn_seq_stmt(_tmp9C,
Cyc_Absyn_exp_stmt(dest,0U),0U);}),0U);}),0U))->r;
# 164
e->r=_tmp9F;});}else{
# 168
e->r=init_e->r;
e->topt=0;}
# 171
goto _LL0;}}}case 26: _tmp2B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_List_List*dles=_tmp2B;
# 176
res=0U;
if(dles == 0){
({struct Cyc_Absyn_Exp _tmpA0=*Cyc_Absyn_signed_int_exp(0,0U);*e=_tmpA0;});
goto _LL0;}
# 181
{enum Cyc_RemoveAggrs_ExpContext _tmp46=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _stmttmp5=_tmp46;enum Cyc_RemoveAggrs_ExpContext _tmp47=_stmttmp5;if(_tmp47 == Cyc_RemoveAggrs_Other)
({void*_tmp48=0U;({struct _fat_ptr _tmpA1=({const char*_tmp49="remove-aggrs: Array_e in bad position";_tag_fat(_tmp49,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpA1,_tag_fat(_tmp48,sizeof(void*),0));});});else{
goto _LL49;}_LL49:;}
# 185
did_assign=1;{
struct Cyc_Absyn_Exp*dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
# 188
int i=0;
{struct Cyc_List_List*_tmp4A=dles;struct Cyc_List_List*dles2=_tmp4A;for(0;dles2 != 0;(dles2=dles2->tl,++ i)){
# 192
struct _tuple11*_tmp4B=(struct _tuple11*)dles2->hd;struct _tuple11*_stmttmp6=_tmp4B;struct _tuple11*_tmp4C=_stmttmp6;struct Cyc_Absyn_Exp*_tmp4D;_tmp4D=_tmp4C->f2;{struct Cyc_Absyn_Exp*field_exp=_tmp4D;
struct Cyc_Absyn_Exp*_tmp4E=({struct Cyc_Absyn_Exp*_tmpA2=Cyc_Absyn_copy_exp(dest);Cyc_Absyn_subscript_exp(_tmpA2,Cyc_Absyn_signed_int_exp(i,0U),0U);});struct Cyc_Absyn_Exp*fielddest=_tmp4E;
({struct Cyc_RemoveAggrs_Env _tmpA3=({struct Cyc_RemoveAggrs_Env _tmp8B;_tmp8B.ctxt=Cyc_RemoveAggrs_AggrField,_tmp8B.dest=fielddest;_tmp8B;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpA3,field_exp);});}}}{
# 196
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple11*)dles->hd)).f2;
for(dles=dles->tl;dles != 0;dles=dles->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple11*)dles->hd)).f2,0U);}
e->r=init_e->r;
e->topt=0;
goto _LL0;}}}case 27: _tmp27=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp28=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp29=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_tmp2A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmp27;struct Cyc_Absyn_Exp*bd=_tmp28;struct Cyc_Absyn_Exp*body=_tmp29;int zero_term=_tmp2A;
# 205
did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _tmp4F=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _stmttmp7=_tmp4F;enum Cyc_RemoveAggrs_ExpContext _tmp50=_stmttmp7;if(_tmp50 == Cyc_RemoveAggrs_Other)
({void*_tmp51=0U;({struct _fat_ptr _tmpA4=({const char*_tmp52="remove-aggrs: comprehension in bad position";_tag_fat(_tmp52,sizeof(char),44U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpA4,_tag_fat(_tmp51,sizeof(void*),0));});});else{
goto _LL51;}_LL51:;}{
# 211
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*i=vd->name;
struct Cyc_Absyn_Exp*ea=({struct Cyc_Absyn_Exp*_tmpA5=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_assign_exp(_tmpA5,Cyc_Absyn_signed_int_exp(0,0U),0U);});
struct Cyc_Absyn_Exp*eb=({struct Cyc_Absyn_Exp*_tmpA6=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_lt_exp(_tmpA6,Cyc_Absyn_var_exp(max,0U),0U);});
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_increment_exp(Cyc_Absyn_var_exp(i,0U),Cyc_Absyn_PreInc,0U);
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*_tmpA7=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_subscript_exp(_tmpA7,Cyc_Absyn_var_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _tmpA9=({struct Cyc_RemoveAggrs_Env _tmp8C;_tmp8C.ctxt=Cyc_RemoveAggrs_AggrField,({struct Cyc_Absyn_Exp*_tmpA8=Cyc_Absyn_copy_exp(lval);_tmp8C.dest=_tmpA8;});_tmp8C;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpA9,body);});{
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_tmpAC=ea;struct Cyc_Absyn_Exp*_tmpAB=eb;struct Cyc_Absyn_Exp*_tmpAA=ec;Cyc_Absyn_for_stmt(_tmpAC,_tmpAB,_tmpAA,Cyc_Absyn_exp_stmt(body,0U),0U);});
if(zero_term){
# 226
struct Cyc_Absyn_Exp*ex=({struct Cyc_Absyn_Exp*_tmpAE=({struct Cyc_Absyn_Exp*_tmpAD=Cyc_Absyn_copy_exp(env.dest);Cyc_Absyn_subscript_exp(_tmpAD,
Cyc_Absyn_var_exp(max,0U),0U);});
# 226
Cyc_Absyn_assign_exp(_tmpAE,
# 228
Cyc_Absyn_signed_int_exp(0,0U),0U);});
s=({struct Cyc_Absyn_Stmt*_tmpAF=s;Cyc_Absyn_seq_stmt(_tmpAF,Cyc_Absyn_exp_stmt(ex,0U),0U);});}
# 232
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,bd);
({void*_tmpB4=(Cyc_Absyn_stmt_exp(({struct Cyc_Absyn_Stmt*_tmpB3=({struct _tuple0*_tmpB2=max;void*_tmpB1=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmpB0=bd;Cyc_Absyn_declare_stmt(_tmpB2,_tmpB1,_tmpB0,
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_type,0,s,0U),0U);});
# 233
Cyc_Absyn_seq_stmt(_tmpB3,
# 235
Cyc_Absyn_exp_stmt(Cyc_Absyn_signed_int_exp(0,0U),0U),0U);}),0U))->r;
# 233
e->r=_tmpB4;});
# 236
e->topt=0;
goto _LL0;}}}case 28: _tmp26=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_Absyn_Exp*bd=_tmp26;
# 240
did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _tmp53=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _stmttmp8=_tmp53;enum Cyc_RemoveAggrs_ExpContext _tmp54=_stmttmp8;if(_tmp54 == Cyc_RemoveAggrs_Other)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56=({struct Cyc_Warn_String_Warn_Warg_struct _tmp8D;_tmp8D.tag=0,({struct _fat_ptr _tmpB5=({const char*_tmp57="remove-aggrs: no-init-comp in bad position";_tag_fat(_tmp57,sizeof(char),43U);});_tmp8D.f1=_tmpB5;});_tmp8D;});void*_tmp55[1];_tmp55[0]=& _tmp56;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp55,sizeof(void*),1));});else{
goto _LL56;}_LL56:;}
# 247
*e=*bd;
goto _LL0;}case 4: _tmp24=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp25=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB)->f3;{struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;
# 251
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
goto _LL0;}case 9: _tmp22=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp23=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e1=_tmp22;struct Cyc_Absyn_Exp*e2=_tmp23;
# 255
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,e2);
goto _LL0;}case 6: _tmp1F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp20=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp21=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f3;{struct Cyc_Absyn_Exp*e1=_tmp1F;struct Cyc_Absyn_Exp*e2=_tmp20;struct Cyc_Absyn_Exp*e3=_tmp21;
# 260
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,e2);{
struct Cyc_RemoveAggrs_Env _tmp58=({struct Cyc_RemoveAggrs_Env _tmp8E;_tmp8E.ctxt=env.ctxt,env.dest == 0?_tmp8E.dest=0:({struct Cyc_Absyn_Exp*_tmpB6=Cyc_Absyn_copy_exp(env.dest);_tmp8E.dest=_tmpB6;});_tmp8E;});struct Cyc_RemoveAggrs_Env env3=_tmp58;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env3,e3);
goto _LL0;}}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB)->f3 == 0){_tmp1D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp1E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e=_tmp1D;struct Cyc_List_List*es=_tmp1E;
# 268
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
_tmp1C=es;goto _LL24;}}else{goto _LL3F;}case 3: _tmp1C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL24: {struct Cyc_List_List*es=_tmp1C;
# 271
for(1;es != 0;es=es->tl){
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 23: _tmp1A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp1B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e1=_tmp1A;struct Cyc_Absyn_Exp*e2=_tmp1B;
# 275
_tmp18=e1;_tmp19=e2;goto _LL28;}case 7: _tmp18=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp19=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp18;struct Cyc_Absyn_Exp*e2=_tmp19;
_tmp16=e1;_tmp17=e2;goto _LL2A;}case 8: _tmp16=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp17=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp16;struct Cyc_Absyn_Exp*e2=_tmp17;
# 278
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
goto _LL0;}case 13: _tmp15=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_Absyn_Exp*e1=_tmp15;
# 283
_tmp14=e1;goto _LL2E;}case 12: _tmp14=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp14;
_tmp13=e1;goto _LL30;}case 14: _tmp13=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp13;
_tmp12=e1;goto _LL32;}case 15: _tmp12=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp12;
_tmp11=e1;goto _LL34;}case 20: _tmp11=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp11;
_tmp10=e1;goto _LL36;}case 21: _tmp10=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp10;
_tmpF=e1;goto _LL38;}case 22: _tmpF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_tmpF;
_tmpE=e1;goto _LL3A;}case 18: _tmpE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmpE;
_tmpD=e1;goto _LL3C;}case 5: _tmpD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmpD;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);goto _LL0;}case 37: _tmpC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_Absyn_Stmt*s=_tmpC;
# 294
Cyc_RemoveAggrs_remove_aggrs_stmt(s);goto _LL0;}default: _LL3F:
# 296
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp90;_tmp90.tag=0,({struct _fat_ptr _tmpB7=({const char*_tmp5C="bad exp after translation to C: ";_tag_fat(_tmp5C,sizeof(char),33U);});_tmp90.f1=_tmpB7;});_tmp90;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp5B=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp8F;_tmp8F.tag=4,_tmp8F.f1=e;_tmp8F;});void*_tmp59[2];_tmp59[0]=& _tmp5A,_tmp59[1]=& _tmp5B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp59,sizeof(void*),2));});}_LL0:;}
# 298
if((int)env.ctxt == (int)Cyc_RemoveAggrs_AggrField && !did_assign)
({void*_tmpB9=({struct Cyc_Absyn_Exp*_tmpB8=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_assign_exp(_tmpB8,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmpB9;});
if((int)env.ctxt == (int)Cyc_RemoveAggrs_NewDest && !did_assign)
({void*_tmpBB=({struct Cyc_Absyn_Exp*_tmpBA=Cyc_Absyn_deref_exp((struct Cyc_Absyn_Exp*)_check_null(env.dest),0U);Cyc_Absyn_assign_exp(_tmpBA,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmpBB;});
return({struct Cyc_RemoveAggrs_Result _tmp91;_tmp91.res=res;_tmp91;});}
# 309
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp5D=s->r;void*_stmttmp9=_tmp5D;void*_tmp5E=_stmttmp9;struct Cyc_Absyn_Stmt*_tmp60;struct Cyc_Absyn_Decl*_tmp5F;struct Cyc_List_List*_tmp62;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_Absyn_Stmt*_tmp66;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Stmt*_tmp68;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp6A;struct Cyc_Absyn_Stmt*_tmp69;struct Cyc_Absyn_Stmt*_tmp6D;struct Cyc_Absyn_Stmt*_tmp6C;struct Cyc_Absyn_Exp*_tmp6B;struct Cyc_Absyn_Stmt*_tmp6F;struct Cyc_Absyn_Stmt*_tmp6E;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Stmt*_tmp72;switch(*((int*)_tmp5E)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 13: _tmp72=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp72;
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);return;}case 3: _tmp71=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp71;
# 317
if(eopt == 0)
return;
_tmp70=eopt;goto _LLE;}case 1: _tmp70=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp70;
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);return;}case 2: _tmp6E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;_tmp6F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp6E;struct Cyc_Absyn_Stmt*s2=_tmp6F;
# 322
Cyc_RemoveAggrs_remove_aggrs_stmt(s1);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 4: _tmp6B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;_tmp6C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2;_tmp6D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E)->f3;{struct Cyc_Absyn_Exp*e=_tmp6B;struct Cyc_Absyn_Stmt*s1=_tmp6C;struct Cyc_Absyn_Stmt*s2=_tmp6D;
# 326
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
Cyc_RemoveAggrs_remove_aggrs_stmt(s1);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 14: _tmp69=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;_tmp6A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2).f1;{struct Cyc_Absyn_Stmt*s2=_tmp69;struct Cyc_Absyn_Exp*e=_tmp6A;
_tmp67=e;_tmp68=s2;goto _LL16;}case 5: _tmp67=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1).f1;_tmp68=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp67;struct Cyc_Absyn_Stmt*s2=_tmp68;
# 332
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 9: _tmp63=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;_tmp64=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2).f1;_tmp65=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E)->f3).f1;_tmp66=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E)->f4;{struct Cyc_Absyn_Exp*e1=_tmp63;struct Cyc_Absyn_Exp*e2=_tmp64;struct Cyc_Absyn_Exp*e3=_tmp65;struct Cyc_Absyn_Stmt*s2=_tmp66;
# 336
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e3);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 10: _tmp61=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;_tmp62=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Exp*e=_tmp61;struct Cyc_List_List*scs=_tmp62;
# 344
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
for(1;scs != 0;scs=scs->tl){
Cyc_RemoveAggrs_remove_aggrs_stmt(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _tmp5F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E)->f1;_tmp60=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E)->f2;{struct Cyc_Absyn_Decl*d=_tmp5F;struct Cyc_Absyn_Stmt*s2=_tmp60;
# 349
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);{
void*_tmp73=d->r;void*_stmttmpA=_tmp73;void*_tmp74=_stmttmpA;struct Cyc_Absyn_Fndecl*_tmp75;struct Cyc_Absyn_Vardecl*_tmp76;switch(*((int*)_tmp74)){case 0: _tmp76=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp74)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp76;
# 352
if((int)vd->sc == (int)Cyc_Absyn_Static){
Cyc_RemoveAggrs_massage_toplevel_aggr(vd->initializer);
return;}
# 363
if(vd->initializer != 0){
struct Cyc_RemoveAggrs_Result _tmp77=({struct Cyc_RemoveAggrs_Env _tmpBD=({struct Cyc_RemoveAggrs_Env _tmp92;_tmp92.ctxt=Cyc_RemoveAggrs_Initializer,({struct Cyc_Absyn_Exp*_tmpBC=Cyc_Absyn_var_exp(vd->name,0U);_tmp92.dest=_tmpBC;});_tmp92;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpBD,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));});struct Cyc_RemoveAggrs_Result _stmttmpB=_tmp77;struct Cyc_RemoveAggrs_Result _tmp78=_stmttmpB;enum Cyc_RemoveAggrs_ExpResult _tmp79;_tmp79=_tmp78.res;{enum Cyc_RemoveAggrs_ExpResult r=_tmp79;
# 366
if((int)r == (int)0U){
({void*_tmpC0=({struct Cyc_Absyn_Decl*_tmpBF=d;Cyc_Absyn_decl_stmt(_tmpBF,({struct Cyc_Absyn_Stmt*_tmpBE=Cyc_Absyn_exp_stmt((struct Cyc_Absyn_Exp*)_check_null(vd->initializer),0U);Cyc_Absyn_seq_stmt(_tmpBE,s2,0U);}),0U);})->r;s->r=_tmpC0;});
# 369
vd->initializer=0;}}}
# 372
return;}case 1: _tmp75=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp74)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp75;
Cyc_RemoveAggrs_remove_aggrs_stmt(fd->body);return;}default:
({void*_tmp7A=0U;({struct _fat_ptr _tmpC1=({const char*_tmp7B="bad local declaration after xlation to C";_tag_fat(_tmp7B,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpC1,_tag_fat(_tmp7A,sizeof(void*),0));});});};}}default:
# 376
({void*_tmp7C=0U;({struct _fat_ptr _tmpC2=({const char*_tmp7D="bad stmt after xlation to C";_tag_fat(_tmp7D,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpC2,_tag_fat(_tmp7C,sizeof(void*),0));});});};}
# 380
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*ds){
{struct Cyc_List_List*_tmp7E=ds;struct Cyc_List_List*ds2=_tmp7E;for(0;ds2 != 0;ds2=ds2->tl){
void*_tmp7F=((struct Cyc_Absyn_Decl*)ds2->hd)->r;void*_stmttmpC=_tmp7F;void*_tmp80=_stmttmpC;struct Cyc_Absyn_Vardecl*_tmp81;struct Cyc_Absyn_Fndecl*_tmp82;switch(*((int*)_tmp80)){case 1: _tmp82=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp80)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp82;
# 384
if((int)fd->orig_scope != (int)Cyc_Absyn_ExternC)
Cyc_RemoveAggrs_remove_aggrs_stmt(fd->body);
goto _LL0;}case 0: _tmp81=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp80)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp81;
Cyc_RemoveAggrs_massage_toplevel_aggr(vd->initializer);goto _LL0;}case 5:
 goto _LL8;case 7: _LL8:
 goto _LLA;case 8: _LLA:
 goto _LL0;case 4:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 2: _LL10:
 goto _LL12;case 3: _LL12:
({void*_tmp83=0U;({struct _fat_ptr _tmpC3=({const char*_tmp84="Cyclone decl after xlation to C";_tag_fat(_tmp84,sizeof(char),32U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpC3,_tag_fat(_tmp83,sizeof(void*),0));});});case 9:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
({void*_tmp85=0U;({struct _fat_ptr _tmpC4=({const char*_tmp86="translation unit after xlation to C";_tag_fat(_tmp86,sizeof(char),36U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmpC4,_tag_fat(_tmp85,sizeof(void*),0));});});}_LL0:;}}
# 404
return ds;}

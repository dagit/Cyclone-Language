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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
# 882 "absyn.h"
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 941
void*Cyc_Absyn_strctq(struct _tuple0*);
void*Cyc_Absyn_unionq_type(struct _tuple0*);
# 966
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 973
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
# 975
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1038
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1046
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1049
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1130
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
# 48 "remove_aggregates.cyc"
static void Cyc_RemoveAggrs_massage_toplevel_aggr(struct Cyc_Absyn_Exp*e){;}
# 54
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;void*_tmp2;if(*((int*)_tmp1)== 20){_tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{
return Cyc_Absyn_aggrmember_exp(e,f,loc);};}
# 63
enum Cyc_RemoveAggrs_ExpContext{Cyc_RemoveAggrs_Initializer =0U,Cyc_RemoveAggrs_NewDest =1U,Cyc_RemoveAggrs_AggrField =2U,Cyc_RemoveAggrs_Other =3U};struct Cyc_RemoveAggrs_Env{enum Cyc_RemoveAggrs_ExpContext ctxt;struct Cyc_Absyn_Exp*dest;};
# 74
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_other_env={Cyc_RemoveAggrs_Other,0};
# 76
enum Cyc_RemoveAggrs_ExpResult{Cyc_RemoveAggrs_WasArray =0U,Cyc_RemoveAggrs_OtherRes =1U};struct Cyc_RemoveAggrs_Result{enum Cyc_RemoveAggrs_ExpResult res;};
# 85
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env,struct Cyc_Absyn_Exp*);
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*);
# 88
static void Cyc_RemoveAggrs_noarray_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
struct Cyc_RemoveAggrs_Result _tmp3=Cyc_RemoveAggrs_remove_aggrs_exp(env,e);struct Cyc_RemoveAggrs_Result _stmttmp1=_tmp3;struct Cyc_RemoveAggrs_Result _tmp4=_stmttmp1;enum Cyc_RemoveAggrs_ExpResult _tmp5;_tmp5=_tmp4.res;{enum Cyc_RemoveAggrs_ExpResult r=_tmp5;
if((int)r != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56;_tmp56.tag=0,({struct _fat_ptr _tmp61=({const char*_tmp9="remove_aggrs_exp -- unexpected array or comprehension: ";_tag_fat(_tmp9,sizeof(char),56U);});_tmp56.f1=_tmp61;});_tmp56;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp8=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp55;_tmp55.tag=4,_tmp55.f1=e;_tmp55;});void*_tmp6[2];_tmp6[0]=& _tmp7,_tmp6[1]=& _tmp8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp6,sizeof(void*),2));});}}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 94
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
enum Cyc_RemoveAggrs_ExpResult res=1U;
int did_assign=0;
# 98
{void*_tmpA=e->r;void*_stmttmp2=_tmpA;void*_tmpB=_stmttmp2;int _tmpF;void*_tmpE;void*_tmpD;void*_tmpC;struct _fat_ptr _tmp10;switch(*((int*)_tmpB)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB)->f1).String_c).tag == 8){_tmp10=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB)->f1).String_c).val;{struct _fat_ptr s=_tmp10;
# 101
if((int)env.ctxt == (int)Cyc_RemoveAggrs_AggrField && Cyc_Tcutil_is_array_type((void*)_check_null(e->topt))){
struct Cyc_List_List*dles=0;
unsigned _tmp11=_get_fat_size(s,sizeof(char));unsigned n=_tmp11;
{unsigned i=0U;for(0;i < n;++ i){
struct Cyc_Absyn_Exp*_tmp12=Cyc_Absyn_char_exp(*((const char*)_check_fat_subscript(s,sizeof(char),(int)i)),0U);struct Cyc_Absyn_Exp*c=_tmp12;
c->topt=Cyc_Absyn_char_type;
dles=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));({struct _tuple11*_tmp62=({struct _tuple11*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->f1=0,_tmp13->f2=c;_tmp13;});_tmp14->hd=_tmp62;}),_tmp14->tl=dles;_tmp14;});}}
# 109
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(dles);
({void*_tmp63=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=26,_tmp15->f1=dles;_tmp15;});e->r=_tmp63;});
return Cyc_RemoveAggrs_remove_aggrs_exp(env,e);}
# 113
goto _LL0;}}else{
goto _LL6;}case 17: _LL6:
 goto _LL8;case 19: _LL8:
 goto _LLA;case 33: _LLA:
 goto _LLC;case 32: _LLC:
 goto _LLE;case 40: _LLE:
 goto _LL10;case 1: _LL10:
 goto _LL0;case 16: _tmpC=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC;
# 123
{void*_tmp16=e->annot;void*_stmttmp3=_tmp16;void*_tmp17=_stmttmp3;void*_tmp18;if(((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmp17)->tag == Cyc_Toc_Dest){_tmp18=((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmp17)->f1;{struct Cyc_Absyn_Exp*dest=_tmp18;
# 125
({struct Cyc_RemoveAggrs_Env _tmp64=({struct Cyc_RemoveAggrs_Env _tmp57;_tmp57.ctxt=Cyc_RemoveAggrs_NewDest,_tmp57.dest=dest;_tmp57;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp64,e1);});
*e=*e1;
goto _LL41;}}else{
({void*_tmp19=0U;({struct _fat_ptr _tmp65=({const char*_tmp1A="removeAggrs: toc did not set a new-destination";_tag_fat(_tmp1A,sizeof(char),47U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp65,_tag_fat(_tmp19,sizeof(void*),0));});});}_LL41:;}
# 130
goto _LL0;}case 29: _tmpC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_tmpE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f4;{struct _tuple0*tdn=_tmpC;struct Cyc_List_List*dles=_tmpD;struct Cyc_Absyn_Aggrdecl*sdopt=_tmpE;
# 133
did_assign=1;{
int do_stmt_exp=(int)env.ctxt != (int)Cyc_RemoveAggrs_AggrField &&(int)env.ctxt != (int)Cyc_RemoveAggrs_NewDest;
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;
if(do_stmt_exp){
v=({struct _tuple0**_tmp1B=_cycalloc(sizeof(*_tmp1B));({struct _tuple0*_tmp66=Cyc_Toc_temp_var();*_tmp1B=_tmp66;});_tmp1B;});
dest=Cyc_Absyn_var_exp(*v,0U);}else{
# 141
v=0;
dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
if((int)env.ctxt == (int)Cyc_RemoveAggrs_NewDest)
dest=Cyc_Absyn_deref_exp(dest,0U);}
# 147
{struct Cyc_List_List*_tmp1C=dles;struct Cyc_List_List*dles2=_tmp1C;for(0;dles2 != 0;dles2=dles2->tl){
struct _tuple11*_tmp1D=(struct _tuple11*)dles2->hd;struct _tuple11*_stmttmp4=_tmp1D;struct _tuple11*_tmp1E=_stmttmp4;void*_tmp20;void*_tmp1F;_tmp1F=_tmp1E->f1;_tmp20=_tmp1E->f2;{struct Cyc_List_List*ds=_tmp1F;struct Cyc_Absyn_Exp*field_exp=_tmp20;
struct _fat_ptr*_tmp21=Cyc_Absyn_designatorlist_to_fieldname(ds);struct _fat_ptr*f=_tmp21;
struct Cyc_Absyn_Exp*_tmp22=({struct Cyc_Absyn_Exp*_tmp67=Cyc_Absyn_copy_exp(dest);Cyc_RemoveAggrs_member_exp(_tmp67,f,0U);});struct Cyc_Absyn_Exp*field_dest=_tmp22;
({struct Cyc_RemoveAggrs_Env _tmp68=({struct Cyc_RemoveAggrs_Env _tmp58;_tmp58.ctxt=Cyc_RemoveAggrs_AggrField,_tmp58.dest=field_dest;_tmp58;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp68,field_exp);});}}}
# 154
if(dles == 0)
({void*_tmp23=0U;({struct _fat_ptr _tmp69=({const char*_tmp24="zero-field aggregate";_tag_fat(_tmp24,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp69,_tag_fat(_tmp23,sizeof(void*),0));});});{
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple11*)dles->hd)).f2;
for(dles=dles->tl;dles != 0;dles=dles->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple11*)dles->hd)).f2,0U);}
if(do_stmt_exp){
void*(*_tmp25)(struct _tuple0*)=(unsigned)sdopt &&(int)sdopt->kind == (int)Cyc_Absyn_UnionA?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq;void*(*f)(struct _tuple0*)=_tmp25;
({void*_tmp6D=(Cyc_Absyn_stmt_exp(({struct _tuple0*_tmp6C=*((struct _tuple0**)_check_null(v));void*_tmp6B=f(tdn);Cyc_Absyn_declare_stmt(_tmp6C,_tmp6B,0,({
struct Cyc_Absyn_Stmt*_tmp6A=Cyc_Absyn_exp_stmt(init_e,0U);Cyc_Absyn_seq_stmt(_tmp6A,
Cyc_Absyn_exp_stmt(dest,0U),0U);}),0U);}),0U))->r;
# 161
e->r=_tmp6D;});}else{
# 165
e->r=init_e->r;
e->topt=0;}
# 168
goto _LL0;}}}case 26: _tmpC=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_List_List*dles=_tmpC;
# 173
res=0U;
if(dles == 0){
({struct Cyc_Absyn_Exp _tmp6E=*Cyc_Absyn_signed_int_exp(0,0U);*e=_tmp6E;});
goto _LL0;}
# 178
{enum Cyc_RemoveAggrs_ExpContext _tmp26=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _stmttmp5=_tmp26;enum Cyc_RemoveAggrs_ExpContext _tmp27=_stmttmp5;if(_tmp27 == Cyc_RemoveAggrs_Other)
({void*_tmp28=0U;({struct _fat_ptr _tmp6F=({const char*_tmp29="remove-aggrs: Array_e in bad position";_tag_fat(_tmp29,sizeof(char),38U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp6F,_tag_fat(_tmp28,sizeof(void*),0));});});else{
goto _LL49;}_LL49:;}
# 182
did_assign=1;{
struct Cyc_Absyn_Exp*dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
# 185
int i=0;
{struct Cyc_List_List*_tmp2A=dles;struct Cyc_List_List*dles2=_tmp2A;for(0;dles2 != 0;(dles2=dles2->tl,++ i)){
# 189
struct _tuple11*_tmp2B=(struct _tuple11*)dles2->hd;struct _tuple11*_stmttmp6=_tmp2B;struct _tuple11*_tmp2C=_stmttmp6;void*_tmp2D;_tmp2D=_tmp2C->f2;{struct Cyc_Absyn_Exp*field_exp=_tmp2D;
struct Cyc_Absyn_Exp*_tmp2E=({struct Cyc_Absyn_Exp*_tmp70=Cyc_Absyn_copy_exp(dest);Cyc_Absyn_subscript_exp(_tmp70,Cyc_Absyn_signed_int_exp(i,0U),0U);});struct Cyc_Absyn_Exp*fielddest=_tmp2E;
({struct Cyc_RemoveAggrs_Env _tmp71=({struct Cyc_RemoveAggrs_Env _tmp59;_tmp59.ctxt=Cyc_RemoveAggrs_AggrField,_tmp59.dest=fielddest;_tmp59;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp71,field_exp);});}}}{
# 193
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple11*)dles->hd)).f2;
for(dles=dles->tl;dles != 0;dles=dles->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple11*)dles->hd)).f2,0U);}
e->r=init_e->r;
e->topt=0;
goto _LL0;}}}case 27: _tmpC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmpE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_tmpF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f4;{struct Cyc_Absyn_Vardecl*vd=_tmpC;struct Cyc_Absyn_Exp*bd=_tmpD;struct Cyc_Absyn_Exp*body=_tmpE;int zero_term=_tmpF;
# 202
did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _tmp2F=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _stmttmp7=_tmp2F;enum Cyc_RemoveAggrs_ExpContext _tmp30=_stmttmp7;if(_tmp30 == Cyc_RemoveAggrs_Other)
({void*_tmp31=0U;({struct _fat_ptr _tmp72=({const char*_tmp32="remove-aggrs: comprehension in bad position";_tag_fat(_tmp32,sizeof(char),44U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp72,_tag_fat(_tmp31,sizeof(void*),0));});});else{
goto _LL51;}_LL51:;}{
# 208
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*i=vd->name;
struct Cyc_Absyn_Exp*ea=({struct Cyc_Absyn_Exp*_tmp73=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_assign_exp(_tmp73,Cyc_Absyn_signed_int_exp(0,0U),0U);});
struct Cyc_Absyn_Exp*eb=({struct Cyc_Absyn_Exp*_tmp74=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_lt_exp(_tmp74,Cyc_Absyn_var_exp(max,0U),0U);});
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_increment_exp(Cyc_Absyn_var_exp(i,0U),Cyc_Absyn_PreInc,0U);
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*_tmp75=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_subscript_exp(_tmp75,Cyc_Absyn_var_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _tmp77=({struct Cyc_RemoveAggrs_Env _tmp5A;_tmp5A.ctxt=Cyc_RemoveAggrs_AggrField,({struct Cyc_Absyn_Exp*_tmp76=Cyc_Absyn_copy_exp(lval);_tmp5A.dest=_tmp76;});_tmp5A;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp77,body);});{
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_tmp7A=ea;struct Cyc_Absyn_Exp*_tmp79=eb;struct Cyc_Absyn_Exp*_tmp78=ec;Cyc_Absyn_for_stmt(_tmp7A,_tmp79,_tmp78,Cyc_Absyn_exp_stmt(body,0U),0U);});
if(zero_term){
# 223
struct Cyc_Absyn_Exp*ex=({struct Cyc_Absyn_Exp*_tmp7C=({struct Cyc_Absyn_Exp*_tmp7B=Cyc_Absyn_copy_exp(env.dest);Cyc_Absyn_subscript_exp(_tmp7B,
Cyc_Absyn_var_exp(max,0U),0U);});
# 223
Cyc_Absyn_assign_exp(_tmp7C,
# 225
Cyc_Absyn_signed_int_exp(0,0U),0U);});
s=({struct Cyc_Absyn_Stmt*_tmp7D=s;Cyc_Absyn_seq_stmt(_tmp7D,Cyc_Absyn_exp_stmt(ex,0U),0U);});}
# 229
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,bd);
({void*_tmp82=(Cyc_Absyn_stmt_exp(({struct Cyc_Absyn_Stmt*_tmp81=({struct _tuple0*_tmp80=max;void*_tmp7F=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp7E=bd;Cyc_Absyn_declare_stmt(_tmp80,_tmp7F,_tmp7E,
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_type,0,s,0U),0U);});
# 230
Cyc_Absyn_seq_stmt(_tmp81,
# 232
Cyc_Absyn_exp_stmt(Cyc_Absyn_signed_int_exp(0,0U),0U),0U);}),0U))->r;
# 230
e->r=_tmp82;});
# 233
e->topt=0;
goto _LL0;}}}case 28: _tmpC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_Absyn_Exp*bd=_tmpC;
# 237
did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _tmp33=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _stmttmp8=_tmp33;enum Cyc_RemoveAggrs_ExpContext _tmp34=_stmttmp8;if(_tmp34 == Cyc_RemoveAggrs_Other)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B;_tmp5B.tag=0,({struct _fat_ptr _tmp83=({const char*_tmp37="remove-aggrs: no-init-comp in bad position";_tag_fat(_tmp37,sizeof(char),43U);});_tmp5B.f1=_tmp83;});_tmp5B;});void*_tmp35[1];_tmp35[0]=& _tmp36;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp35,sizeof(void*),1));});else{
goto _LL56;}_LL56:;}
# 244
*e=*bd;
goto _LL0;}case 4: _tmpC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB)->f3;{struct Cyc_Absyn_Exp*e1=_tmpC;struct Cyc_Absyn_Exp*e2=_tmpD;
# 248
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
goto _LL0;}case 9: _tmpC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC;struct Cyc_Absyn_Exp*e2=_tmpD;
# 252
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,e2);
goto _LL0;}case 6: _tmpC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmpE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f3;{struct Cyc_Absyn_Exp*e1=_tmpC;struct Cyc_Absyn_Exp*e2=_tmpD;struct Cyc_Absyn_Exp*e3=_tmpE;
# 257
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,e2);{
struct Cyc_RemoveAggrs_Env _tmp38=({struct Cyc_RemoveAggrs_Env _tmp5C;_tmp5C.ctxt=env.ctxt,env.dest == 0?_tmp5C.dest=0:({struct Cyc_Absyn_Exp*_tmp84=Cyc_Absyn_copy_exp(env.dest);_tmp5C.dest=_tmp84;});_tmp5C;});struct Cyc_RemoveAggrs_Env env3=_tmp38;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env3,e3);
goto _LL0;}}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB)->f3 == 0){_tmpC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e=_tmpC;struct Cyc_List_List*es=_tmpD;
# 265
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
_tmpC=es;goto _LL24;}}else{goto _LL3F;}case 3: _tmpC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL24: {struct Cyc_List_List*es=_tmpC;
# 268
for(1;es != 0;es=es->tl){
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 23: _tmpC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f2;{struct Cyc_Absyn_Exp*e1=_tmpC;struct Cyc_Absyn_Exp*e2=_tmpD;
# 272
_tmpC=e1;_tmpD=e2;goto _LL28;}case 7: _tmpC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpC;struct Cyc_Absyn_Exp*e2=_tmpD;
_tmpC=e1;_tmpD=e2;goto _LL2A;}case 8: _tmpC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmpD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpC;struct Cyc_Absyn_Exp*e2=_tmpD;
# 275
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
goto _LL0;}case 13: _tmpC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_Absyn_Exp*e1=_tmpC;
# 280
_tmpC=e1;goto _LL2E;}case 12: _tmpC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpC;
_tmpC=e1;goto _LL30;}case 14: _tmpC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmpC;
_tmpC=e1;goto _LL32;}case 15: _tmpC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpC;
_tmpC=e1;goto _LL34;}case 20: _tmpC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpC;
_tmpC=e1;goto _LL36;}case 21: _tmpC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpC;
_tmpC=e1;goto _LL38;}case 22: _tmpC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_tmpC;
_tmpC=e1;goto _LL3A;}case 18: _tmpC=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmpC;
_tmpC=e1;goto _LL3C;}case 5: _tmpC=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmpC;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);goto _LL0;}case 37: _tmpC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;{struct Cyc_Absyn_Stmt*s=_tmpC;
# 291
Cyc_RemoveAggrs_remove_aggrs_stmt(s);goto _LL0;}default: _LL3F:
# 293
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E;_tmp5E.tag=0,({struct _fat_ptr _tmp85=({const char*_tmp3C="bad exp after translation to C: ";_tag_fat(_tmp3C,sizeof(char),33U);});_tmp5E.f1=_tmp85;});_tmp5E;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp3B=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp5D;_tmp5D.tag=4,_tmp5D.f1=e;_tmp5D;});void*_tmp39[2];_tmp39[0]=& _tmp3A,_tmp39[1]=& _tmp3B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp39,sizeof(void*),2));});}_LL0:;}
# 295
if((int)env.ctxt == (int)Cyc_RemoveAggrs_AggrField && !did_assign)
({void*_tmp87=({struct Cyc_Absyn_Exp*_tmp86=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_assign_exp(_tmp86,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmp87;});
if((int)env.ctxt == (int)Cyc_RemoveAggrs_NewDest && !did_assign)
({void*_tmp89=({struct Cyc_Absyn_Exp*_tmp88=Cyc_Absyn_deref_exp((struct Cyc_Absyn_Exp*)_check_null(env.dest),0U);Cyc_Absyn_assign_exp(_tmp88,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmp89;});
return({struct Cyc_RemoveAggrs_Result _tmp5F;_tmp5F.res=res;_tmp5F;});}
# 306
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3D=s->r;void*_stmttmp9=_tmp3D;void*_tmp3E=_stmttmp9;void*_tmp42;void*_tmp41;void*_tmp40;void*_tmp3F;switch(*((int*)_tmp3E)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 13: _tmp3F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2;{struct Cyc_Absyn_Stmt*s2=_tmp3F;
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);return;}case 3: _tmp3F=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;{struct Cyc_Absyn_Exp*eopt=_tmp3F;
# 314
if(eopt == 0)
return;
_tmp3F=eopt;goto _LLE;}case 1: _tmp3F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp3F;
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);return;}case 2: _tmp3F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;_tmp40=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2;{struct Cyc_Absyn_Stmt*s1=_tmp3F;struct Cyc_Absyn_Stmt*s2=_tmp40;
# 319
Cyc_RemoveAggrs_remove_aggrs_stmt(s1);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 4: _tmp3F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;_tmp40=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2;_tmp41=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3E)->f3;{struct Cyc_Absyn_Exp*e=_tmp3F;struct Cyc_Absyn_Stmt*s1=_tmp40;struct Cyc_Absyn_Stmt*s2=_tmp41;
# 323
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
Cyc_RemoveAggrs_remove_aggrs_stmt(s1);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 14: _tmp3F=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;_tmp40=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2).f1;{struct Cyc_Absyn_Stmt*s2=_tmp3F;struct Cyc_Absyn_Exp*e=_tmp40;
_tmp3F=e;_tmp40=s2;goto _LL16;}case 5: _tmp3F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1).f1;_tmp40=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2;_LL16: {struct Cyc_Absyn_Exp*e=_tmp3F;struct Cyc_Absyn_Stmt*s2=_tmp40;
# 329
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 9: _tmp3F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;_tmp40=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2).f1;_tmp41=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3E)->f3).f1;_tmp42=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3E)->f4;{struct Cyc_Absyn_Exp*e1=_tmp3F;struct Cyc_Absyn_Exp*e2=_tmp40;struct Cyc_Absyn_Exp*e3=_tmp41;struct Cyc_Absyn_Stmt*s2=_tmp42;
# 333
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e3);
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);
return;}case 10: _tmp3F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;_tmp40=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2;{struct Cyc_Absyn_Exp*e=_tmp3F;struct Cyc_List_List*scs=_tmp40;
# 341
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
for(1;scs != 0;scs=scs->tl){
Cyc_RemoveAggrs_remove_aggrs_stmt(((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _tmp3F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E)->f1;_tmp40=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3E)->f2;{struct Cyc_Absyn_Decl*d=_tmp3F;struct Cyc_Absyn_Stmt*s2=_tmp40;
# 346
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);{
void*_tmp43=d->r;void*_stmttmpA=_tmp43;void*_tmp44=_stmttmpA;void*_tmp45;switch(*((int*)_tmp44)){case 0: _tmp45=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp44)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp45;
# 349
if((int)vd->sc == (int)Cyc_Absyn_Static){
Cyc_RemoveAggrs_massage_toplevel_aggr(vd->initializer);
return;}
# 360
if(vd->initializer != 0){
struct Cyc_RemoveAggrs_Result _tmp46=({struct Cyc_RemoveAggrs_Env _tmp8B=({struct Cyc_RemoveAggrs_Env _tmp60;_tmp60.ctxt=Cyc_RemoveAggrs_Initializer,({struct Cyc_Absyn_Exp*_tmp8A=Cyc_Absyn_var_exp(vd->name,0U);_tmp60.dest=_tmp8A;});_tmp60;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp8B,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));});struct Cyc_RemoveAggrs_Result _stmttmpB=_tmp46;struct Cyc_RemoveAggrs_Result _tmp47=_stmttmpB;enum Cyc_RemoveAggrs_ExpResult _tmp48;_tmp48=_tmp47.res;{enum Cyc_RemoveAggrs_ExpResult r=_tmp48;
# 363
if((int)r == (int)0U){
({void*_tmp8E=({struct Cyc_Absyn_Decl*_tmp8D=d;Cyc_Absyn_decl_stmt(_tmp8D,({struct Cyc_Absyn_Stmt*_tmp8C=Cyc_Absyn_exp_stmt((struct Cyc_Absyn_Exp*)_check_null(vd->initializer),0U);Cyc_Absyn_seq_stmt(_tmp8C,s2,0U);}),0U);})->r;s->r=_tmp8E;});
# 366
vd->initializer=0;}}}
# 369
return;}case 1: _tmp45=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp44)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp45;
Cyc_RemoveAggrs_remove_aggrs_stmt(fd->body);return;}default:
({void*_tmp49=0U;({struct _fat_ptr _tmp8F=({const char*_tmp4A="bad local declaration after xlation to C";_tag_fat(_tmp4A,sizeof(char),41U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp8F,_tag_fat(_tmp49,sizeof(void*),0));});});};}}default:
# 373
({void*_tmp4B=0U;({struct _fat_ptr _tmp90=({const char*_tmp4C="bad stmt after xlation to C";_tag_fat(_tmp4C,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp90,_tag_fat(_tmp4B,sizeof(void*),0));});});};}
# 377
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*ds){
{struct Cyc_List_List*_tmp4D=ds;struct Cyc_List_List*ds2=_tmp4D;for(0;ds2 != 0;ds2=ds2->tl){
void*_tmp4E=((struct Cyc_Absyn_Decl*)ds2->hd)->r;void*_stmttmpC=_tmp4E;void*_tmp4F=_stmttmpC;void*_tmp50;switch(*((int*)_tmp4F)){case 1: _tmp50=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4F)->f1;{struct Cyc_Absyn_Fndecl*fd=_tmp50;
# 381
if((int)fd->orig_scope != (int)Cyc_Absyn_ExternC)
Cyc_RemoveAggrs_remove_aggrs_stmt(fd->body);
goto _LL0;}case 0: _tmp50=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4F)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp50;
Cyc_RemoveAggrs_massage_toplevel_aggr(vd->initializer);goto _LL0;}case 5:
 goto _LL8;case 7: _LL8:
 goto _LLA;case 8: _LLA:
 goto _LL0;case 4:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 2: _LL10:
 goto _LL12;case 3: _LL12:
({void*_tmp51=0U;({struct _fat_ptr _tmp91=({const char*_tmp52="Cyclone decl after xlation to C";_tag_fat(_tmp52,sizeof(char),32U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp91,_tag_fat(_tmp51,sizeof(void*),0));});});case 9:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
({void*_tmp53=0U;({struct _fat_ptr _tmp92=({const char*_tmp54="translation unit after xlation to C";_tag_fat(_tmp54,sizeof(char),36U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos)(_tmp92,_tag_fat(_tmp53,sizeof(void*),0));});});}_LL0:;}}
# 401
return ds;}

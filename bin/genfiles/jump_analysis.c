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
 struct Cyc_Core_Opt{void*v;};
# 126 "core.h"
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 50 "string.h"
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
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
# 1161 "absyn.h"
extern int Cyc_Absyn_no_regions;
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct _tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple11*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 86
int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*p);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Switch_clause*f1;};struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct{int tag;struct _fat_ptr*f1;};
# 54 "jump_analysis.cyc"
struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_NotLoop_j_val={0U};
struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_CaseEnd_j_val={1U};
struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_FnEnd_j_val={2U};
struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_Return_j_val={5U};struct Cyc_JumpAnalysis_SaveEnv{int try_depth;int pat_var_depth;struct Cyc_List_List*enclosers;};struct Cyc_JumpAnalysis_FnEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*tables;struct Cyc_Hashtable_Table*stmt_info;struct Cyc_List_List*jumpers;struct Cyc_Hashtable_Table*labels;};struct Cyc_JumpAnalysis_StmtEnv{struct Cyc_JumpAnalysis_FnEnv*fenv;void*continue_stmt;void*break_stmt;void*next_stmt;struct Cyc_JumpAnalysis_SaveEnv save_env;};
# 81
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Exp*e);
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd);
# 85
static int Cyc_JumpAnalysis_hash_ptr(void*s){
return(int)s;}
# 88
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_ptr_table(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_create)(33,Cyc_Core_ptrcmp,(int(*)(void*s))Cyc_JumpAnalysis_hash_ptr);}
# 91
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_var_table(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),int(*hash)(struct _fat_ptr*)))Cyc_Hashtable_create)(33,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 94
static struct Cyc_JumpAnalysis_StmtEnv Cyc_JumpAnalysis_add_encloser(struct Cyc_Absyn_Stmt*s,struct Cyc_JumpAnalysis_StmtEnv env){
struct Cyc_JumpAnalysis_StmtEnv _tmp0=env;struct Cyc_JumpAnalysis_StmtEnv env1=_tmp0;
({struct Cyc_List_List*_tmpBD=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=s,_tmp1->tl=(env1.save_env).enclosers;_tmp1;});(env1.save_env).enclosers=_tmpBD;});
return env1;}
# 99
static void Cyc_JumpAnalysis_anal_exps(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_JumpAnalysis_anal_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}}struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 103
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Exp*e){
void*_tmp2=e->r;void*_stmttmp0=_tmp2;void*_tmp3=_stmttmp0;struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp7;struct Cyc_Absyn_Exp*_tmp9;struct Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmpB;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_Absyn_Exp*_tmpC;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2D;struct Cyc_Absyn_Stmt*_tmp2E;switch(*((int*)_tmp3)){case 37U: _LL1: _tmp2E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2: {struct Cyc_Absyn_Stmt*s=_tmp2E;
({struct Cyc_JumpAnalysis_StmtEnv _tmpBE=Cyc_JumpAnalysis_add_encloser(s,env);Cyc_JumpAnalysis_anal_stmt(_tmpBE,s);});goto _LL0;}case 40U: _LL3: _LL4:
# 107
 goto _LL6;case 0U: _LL5: _LL6:
 goto _LL8;case 17U: _LL7: _LL8:
 goto _LLA;case 18U: _LL9: _LLA:
 goto _LLC;case 19U: _LLB: _LLC:
 goto _LLE;case 39U: _LLD: _LLE:
 goto _LL10;case 32U: _LLF: _LL10:
 goto _LL12;case 33U: _LL11: _LL12:
 goto _LL14;case 2U: _LL13: _LL14:
 goto _LL16;case 1U: _LL15: _LL16:
 goto _LL0;case 31U: _LL17: _tmp2D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL18: {struct Cyc_List_List*es=_tmp2D;
# 118
_tmp2C=es;goto _LL1A;}case 24U: _LL19: _tmp2C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL1A: {struct Cyc_List_List*es=_tmp2C;
_tmp2B=es;goto _LL1C;}case 3U: _LL1B: _tmp2B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL1C: {struct Cyc_List_List*es=_tmp2B;
Cyc_JumpAnalysis_anal_exps(env,es);goto _LL0;}case 10U: _LL1D: _tmp29=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp2A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp29;struct Cyc_List_List*es=_tmp2A;
# 122
Cyc_JumpAnalysis_anal_exp(env,e1);Cyc_JumpAnalysis_anal_exps(env,es);goto _LL0;}case 28U: _LL1F: _tmp28=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp28;
# 124
_tmp27=e1;goto _LL22;}case 41U: _LL21: _tmp27=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp27;
_tmp26=e1;goto _LL24;}case 38U: _LL23: _tmp26=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp26;
_tmp25=e1;goto _LL26;}case 15U: _LL25: _tmp25=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp25;
_tmp24=e1;goto _LL28;}case 20U: _LL27: _tmp24=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp24;
_tmp23=e1;goto _LL2A;}case 21U: _LL29: _tmp23=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmp23;
_tmp22=e1;goto _LL2C;}case 22U: _LL2B: _tmp22=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmp22;
_tmp21=e1;goto _LL2E;}case 14U: _LL2D: _tmp21=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp21;
_tmp20=e1;goto _LL30;}case 12U: _LL2F: _tmp20=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp20;
_tmp1F=e1;goto _LL32;}case 13U: _LL31: _tmp1F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp1F;
_tmp1E=e1;goto _LL34;}case 11U: _LL33: _tmp1E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp1E;
_tmp1D=e1;goto _LL36;}case 5U: _LL35: _tmp1D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_tmp1D;
Cyc_JumpAnalysis_anal_exp(env,e1);goto _LL0;}case 6U: _LL37: _tmp1A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp1B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_tmp1C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL38: {struct Cyc_Absyn_Exp*e1=_tmp1A;struct Cyc_Absyn_Exp*e2=_tmp1B;struct Cyc_Absyn_Exp*e3=_tmp1C;
# 138
Cyc_JumpAnalysis_anal_exp(env,e1);Cyc_JumpAnalysis_anal_exp(env,e2);Cyc_JumpAnalysis_anal_exp(env,e3);goto _LL0;}case 27U: _LL39: _tmp18=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_tmp19=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmp18;struct Cyc_Absyn_Exp*e2=_tmp19;
# 140
_tmp16=e1;_tmp17=e2;goto _LL3C;}case 4U: _LL3B: _tmp16=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp17=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmp16;struct Cyc_Absyn_Exp*e2=_tmp17;
_tmp14=e1;_tmp15=e2;goto _LL3E;}case 35U: _LL3D: _tmp14=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp15=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL3E: {struct Cyc_Absyn_Exp*e1=_tmp14;struct Cyc_Absyn_Exp*e2=_tmp15;
_tmp12=e1;_tmp13=e2;goto _LL40;}case 23U: _LL3F: _tmp12=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp13=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp12;struct Cyc_Absyn_Exp*e2=_tmp13;
_tmp10=e1;_tmp11=e2;goto _LL42;}case 7U: _LL41: _tmp10=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp11=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL42: {struct Cyc_Absyn_Exp*e1=_tmp10;struct Cyc_Absyn_Exp*e2=_tmp11;
_tmpE=e1;_tmpF=e2;goto _LL44;}case 8U: _LL43: _tmpE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmpF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL44: {struct Cyc_Absyn_Exp*e1=_tmpE;struct Cyc_Absyn_Exp*e2=_tmpF;
_tmpC=e1;_tmpD=e2;goto _LL46;}case 9U: _LL45: _tmpC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmpD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL46: {struct Cyc_Absyn_Exp*e1=_tmpC;struct Cyc_Absyn_Exp*e2=_tmpD;
Cyc_JumpAnalysis_anal_exp(env,e1);Cyc_JumpAnalysis_anal_exp(env,e2);goto _LL0;}case 34U: _LL47: _tmpA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3)->f1).rgn;_tmpB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3)->f1).num_elts;_LL48: {struct Cyc_Absyn_Exp*e1o=_tmpA;struct Cyc_Absyn_Exp*e2=_tmpB;
# 148
_tmp8=e1o;_tmp9=e2;goto _LL4A;}case 16U: _LL49: _tmp8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL4A: {struct Cyc_Absyn_Exp*e1o=_tmp8;struct Cyc_Absyn_Exp*e2=_tmp9;
# 150
if(e1o != 0)Cyc_JumpAnalysis_anal_exp(env,e1o);
Cyc_JumpAnalysis_anal_exp(env,e2);
goto _LL0;}case 30U: _LL4B: _tmp7=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL4C: {struct Cyc_List_List*dles=_tmp7;
# 154
_tmp6=dles;goto _LL4E;}case 29U: _LL4D: _tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL4E: {struct Cyc_List_List*dles=_tmp6;
_tmp5=dles;goto _LL50;}case 25U: _LL4F: _tmp5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL50: {struct Cyc_List_List*dles=_tmp5;
_tmp4=dles;goto _LL52;}case 26U: _LL51: _tmp4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL52: {struct Cyc_List_List*dles=_tmp4;
# 158
for(0;dles != 0;dles=dles->tl){
Cyc_JumpAnalysis_anal_exp(env,(*((struct _tuple12*)dles->hd)).f2);}
goto _LL0;}default: _LL53: _LL54:
# 162
({void*_tmp2F=0U;({struct _fat_ptr _tmpBF=({const char*_tmp30="jump_anaysis found UnresolvedMem_e";_tag_fat(_tmp30,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmpBF,_tag_fat(_tmp2F,sizeof(void*),0U));});});}_LL0:;}
# 165
static void Cyc_JumpAnalysis_anal_scs(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*scs){
# 168
env.break_stmt=env.next_stmt;
env.next_stmt=(void*)& Cyc_JumpAnalysis_CaseEnd_j_val;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp31=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*sc=_tmp31;
struct Cyc_Absyn_Exp*_tmp32=sc->where_clause;struct Cyc_Absyn_Exp*wc=_tmp32;
if(wc != 0)
Cyc_JumpAnalysis_anal_exp(env,wc);{
struct Cyc_JumpAnalysis_StmtEnv _tmp33=env;struct Cyc_JumpAnalysis_StmtEnv env1=_tmp33;
if(Cyc_Tcpat_has_vars(sc->pat_vars)){
env1=Cyc_JumpAnalysis_add_encloser(sc->body,env1);
++(env1.save_env).pat_var_depth;}
# 180
Cyc_JumpAnalysis_anal_stmt(env1,sc->body);}}}struct _tuple13{struct Cyc_Absyn_Stmt*f1;void*f2;};
# 183
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Hashtable_Table*_tmpC1=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpC0=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpC1,_tmpC0,({struct Cyc_JumpAnalysis_SaveEnv*_tmp34=_cycalloc(sizeof(*_tmp34));*_tmp34=env.save_env;_tmp34;}));});{
void*_tmp35=s->r;void*_stmttmp1=_tmp35;void*_tmp36=_stmttmp1;struct Cyc_Absyn_Stmt*_tmp38;struct Cyc_Absyn_Decl*_tmp37;struct Cyc_List_List*_tmp3A;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_List_List*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Switch_clause**_tmp3E;struct Cyc_List_List*_tmp3D;struct Cyc_Absyn_Stmt*_tmp40;struct _fat_ptr*_tmp3F;struct _fat_ptr*_tmp41;struct Cyc_Absyn_Stmt*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Stmt*_tmp42;struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Stmt*_tmp49;struct Cyc_Absyn_Exp*_tmp48;struct Cyc_Absyn_Stmt*_tmp47;struct Cyc_Absyn_Exp*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Stmt*_tmp4D;struct Cyc_Absyn_Stmt*_tmp4C;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Stmt*_tmp50;struct Cyc_Absyn_Exp*_tmp4F;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_Absyn_Stmt*_tmp52;struct Cyc_Absyn_Exp*_tmp54;switch(*((int*)_tmp36)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp54=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp54;
Cyc_JumpAnalysis_anal_exp(env,e);goto _LL0;}case 2U: _LL5: _tmp52=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp53=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp52;struct Cyc_Absyn_Stmt*s2=_tmp53;
# 189
struct Cyc_JumpAnalysis_StmtEnv _tmp55=env;struct Cyc_JumpAnalysis_StmtEnv env1=_tmp55;
({void*_tmpC2=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=3U,_tmp56->f1=s2;_tmp56;});env1.next_stmt=_tmpC2;});
Cyc_JumpAnalysis_anal_stmt(env1,s1);
Cyc_JumpAnalysis_anal_stmt(env,s2);
goto _LL0;}case 4U: _LL7: _tmp4F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp50=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_tmp51=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp36)->f3;_LL8: {struct Cyc_Absyn_Exp*e=_tmp4F;struct Cyc_Absyn_Stmt*s1=_tmp50;struct Cyc_Absyn_Stmt*s2=_tmp51;
# 195
Cyc_JumpAnalysis_anal_exp(env,e);
Cyc_JumpAnalysis_anal_stmt(env,s1);
Cyc_JumpAnalysis_anal_stmt(env,s2);
goto _LL0;}case 3U: _LL9: _tmp4E=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_LLA: {struct Cyc_Absyn_Exp*eopt=_tmp4E;
# 201
if(eopt != 0)Cyc_JumpAnalysis_anal_exp(env,eopt);
({struct Cyc_List_List*_tmpC4=({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({struct _tuple13*_tmpC3=({struct _tuple13*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=s,_tmp57->f2=(void*)& Cyc_JumpAnalysis_Return_j_val;_tmp57;});_tmp58->hd=_tmpC3;}),_tmp58->tl=(env.fenv)->jumpers;_tmp58;});(env.fenv)->jumpers=_tmpC4;});
goto _LL0;}case 5U: _LLB: _tmp4B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp36)->f1).f1;_tmp4C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp36)->f1).f2;_tmp4D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp4B;struct Cyc_Absyn_Stmt*cont_s=_tmp4C;struct Cyc_Absyn_Stmt*s=_tmp4D;
# 206
({struct Cyc_Hashtable_Table*_tmpC6=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpC5=cont_s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpC6,_tmpC5,({struct Cyc_JumpAnalysis_SaveEnv*_tmp59=_cycalloc(sizeof(*_tmp59));*_tmp59=env.save_env;_tmp59;}));});
Cyc_JumpAnalysis_anal_exp(env,e);{
struct Cyc_JumpAnalysis_StmtEnv _tmp5A=env;struct Cyc_JumpAnalysis_StmtEnv env_body=_tmp5A;
({void*_tmpC7=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->tag=3U,_tmp5B->f1=cont_s;_tmp5B;});env_body.continue_stmt=_tmpC7;});
env_body.next_stmt=env_body.continue_stmt;
env_body.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(env_body,s);
goto _LL0;}}case 9U: _LLD: _tmp45=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp46=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp36)->f2).f1;_tmp47=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp36)->f2).f2;_tmp48=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp36)->f3).f1;_tmp49=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp36)->f3).f2;_tmp4A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp36)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp45;struct Cyc_Absyn_Exp*e2=_tmp46;struct Cyc_Absyn_Stmt*guard_s=_tmp47;struct Cyc_Absyn_Exp*e3=_tmp48;struct Cyc_Absyn_Stmt*cont_s=_tmp49;struct Cyc_Absyn_Stmt*s=_tmp4A;
# 215
({struct Cyc_Hashtable_Table*_tmpC9=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpC8=guard_s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpC9,_tmpC8,({struct Cyc_JumpAnalysis_SaveEnv*_tmp5C=_cycalloc(sizeof(*_tmp5C));*_tmp5C=env.save_env;_tmp5C;}));});
({struct Cyc_Hashtable_Table*_tmpCB=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpCA=cont_s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpCB,_tmpCA,({struct Cyc_JumpAnalysis_SaveEnv*_tmp5D=_cycalloc(sizeof(*_tmp5D));*_tmp5D=env.save_env;_tmp5D;}));});
Cyc_JumpAnalysis_anal_exp(env,e1);
Cyc_JumpAnalysis_anal_exp(env,e2);{
# 220
struct Cyc_JumpAnalysis_StmtEnv _tmp5E=env;struct Cyc_JumpAnalysis_StmtEnv env_body=_tmp5E;
({void*_tmpCC=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=3U,_tmp5F->f1=cont_s;_tmp5F;});env_body.continue_stmt=_tmpCC;});
env_body.next_stmt=env_body.continue_stmt;
env_body.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(env_body,s);
Cyc_JumpAnalysis_anal_exp(env_body,e3);
goto _LL0;}}case 14U: _LLF: _tmp42=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp43=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp36)->f2).f1;_tmp44=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp36)->f2).f2;_LL10: {struct Cyc_Absyn_Stmt*s=_tmp42;struct Cyc_Absyn_Exp*e=_tmp43;struct Cyc_Absyn_Stmt*cont_s=_tmp44;
# 228
({struct Cyc_Hashtable_Table*_tmpCE=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpCD=cont_s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpCE,_tmpCD,({struct Cyc_JumpAnalysis_SaveEnv*_tmp60=_cycalloc(sizeof(*_tmp60));*_tmp60=env.save_env;_tmp60;}));});{
struct Cyc_JumpAnalysis_StmtEnv _tmp61=env;struct Cyc_JumpAnalysis_StmtEnv env_body=_tmp61;
({void*_tmpCF=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=3U,_tmp62->f1=cont_s;_tmp62;});env_body.continue_stmt=_tmpCF;});
env_body.next_stmt=env_body.continue_stmt;
env_body.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(env_body,s);
Cyc_JumpAnalysis_anal_exp(env,e);
goto _LL0;}}case 6U: _LL11: _LL12:
# 238
({struct Cyc_List_List*_tmpD1=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));({struct _tuple13*_tmpD0=({struct _tuple13*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->f1=s,_tmp63->f2=env.break_stmt;_tmp63;});_tmp64->hd=_tmpD0;}),_tmp64->tl=(env.fenv)->jumpers;_tmp64;});(env.fenv)->jumpers=_tmpD1;});
goto _LL0;case 7U: _LL13: _LL14:
# 241
({struct Cyc_List_List*_tmpD3=({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(*_tmp66));({struct _tuple13*_tmpD2=({struct _tuple13*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=s,_tmp65->f2=env.continue_stmt;_tmp65;});_tmp66->hd=_tmpD2;}),_tmp66->tl=(env.fenv)->jumpers;_tmp66;});(env.fenv)->jumpers=_tmpD3;});
goto _LL0;case 8U: _LL15: _tmp41=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_LL16: {struct _fat_ptr*lab=_tmp41;
# 244
({struct Cyc_List_List*_tmpD6=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _tuple13*_tmpD5=({struct _tuple13*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=s,({void*_tmpD4=(void*)({struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=6U,_tmp67->f1=lab;_tmp67;});_tmp68->f2=_tmpD4;});_tmp68;});_tmp69->hd=_tmpD5;}),_tmp69->tl=(env.fenv)->jumpers;_tmp69;});(env.fenv)->jumpers=_tmpD6;});
goto _LL0;}case 13U: _LL17: _tmp3F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp40=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_LL18: {struct _fat_ptr*lab=_tmp3F;struct Cyc_Absyn_Stmt*s1=_tmp40;
# 248
{struct _handler_cons _tmp6A;_push_handler(& _tmp6A);{int _tmp6C=0;if(setjmp(_tmp6A.handler))_tmp6C=1;if(!_tmp6C){
((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key))Cyc_Hashtable_lookup)((env.fenv)->labels,lab);
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB5;_tmpB5.tag=0U,({struct _fat_ptr _tmpD7=({const char*_tmp6F="repated label";_tag_fat(_tmp6F,sizeof(char),14U);});_tmpB5.f1=_tmpD7;});_tmpB5;});void*_tmp6D[1U];_tmp6D[0]=& _tmp6E;({unsigned _tmpD8=s->loc;Cyc_Warn_err2(_tmpD8,_tag_fat(_tmp6D,sizeof(void*),1U));});});
# 249
;_pop_handler();}else{void*_tmp6B=(void*)Cyc_Core_get_exn_thrown();void*_tmp70=_tmp6B;void*_tmp71;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp70)->tag == Cyc_Core_Not_found){_LL22: _LL23:
# 251
 goto _LL21;}else{_LL24: _tmp71=_tmp70;_LL25: {void*exn=_tmp71;(int)_rethrow(exn);}}_LL21:;}}}
((void(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)((env.fenv)->labels,lab,s);
Cyc_JumpAnalysis_anal_stmt(env,s1);
goto _LL0;}case 11U: _LL19: _tmp3D=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp3E=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_LL1A: {struct Cyc_List_List*es=_tmp3D;struct Cyc_Absyn_Switch_clause**clauseopt=_tmp3E;
# 258
Cyc_JumpAnalysis_anal_exps(env,es);
({struct Cyc_List_List*_tmpDB=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));({struct _tuple13*_tmpDA=({struct _tuple13*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->f1=s,({void*_tmpD9=(void*)({struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=4U,_tmp72->f1=*((struct Cyc_Absyn_Switch_clause**)_check_null(clauseopt));_tmp72;});_tmp73->f2=_tmpD9;});_tmp73;});_tmp74->hd=_tmpDA;}),_tmp74->tl=(env.fenv)->jumpers;_tmp74;});(env.fenv)->jumpers=_tmpDB;});
# 261
goto _LL0;}case 10U: _LL1B: _tmp3B=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp3C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_LL1C: {struct Cyc_Absyn_Exp*e=_tmp3B;struct Cyc_List_List*scs=_tmp3C;
# 263
Cyc_JumpAnalysis_anal_exp(env,e);
Cyc_JumpAnalysis_anal_scs(env,scs);
goto _LL0;}case 15U: _LL1D: _tmp39=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp3A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_LL1E: {struct Cyc_Absyn_Stmt*s=_tmp39;struct Cyc_List_List*scs=_tmp3A;
# 267
struct Cyc_JumpAnalysis_StmtEnv _tmp75=Cyc_JumpAnalysis_add_encloser(s,env);struct Cyc_JumpAnalysis_StmtEnv env1=_tmp75;
++(env1.save_env).try_depth;
Cyc_JumpAnalysis_anal_stmt(env1,s);
Cyc_JumpAnalysis_anal_scs(env,scs);
goto _LL0;}default: _LL1F: _tmp37=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp36)->f1;_tmp38=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp36)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp37;struct Cyc_Absyn_Stmt*s1=_tmp38;
# 276
env=Cyc_JumpAnalysis_add_encloser(s,env);
{void*_tmp76=d->r;void*_stmttmp2=_tmp76;void*_tmp77=_stmttmp2;struct Cyc_Absyn_Exp*_tmp78;struct Cyc_Absyn_Fndecl*_tmp79;struct Cyc_Absyn_Exp*_tmp7A;struct Cyc_Absyn_Exp*_tmp7B;switch(*((int*)_tmp77)){case 4U: _LL27: _tmp7B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp77)->f3;_LL28: {struct Cyc_Absyn_Exp*eo=_tmp7B;
# 280
if(eo != 0){
Cyc_JumpAnalysis_anal_exp(env,eo);
++(env.save_env).pat_var_depth;}
# 284
if(!Cyc_Absyn_no_regions)
++(env.save_env).try_depth;
goto _LL26;}case 0U: _LL29: _tmp7A=(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp77)->f1)->initializer;_LL2A: {struct Cyc_Absyn_Exp*eo=_tmp7A;
# 288
if(eo != 0)Cyc_JumpAnalysis_anal_exp(env,eo);
goto _LL26;}case 1U: _LL2B: _tmp79=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp77)->f1;_LL2C: {struct Cyc_Absyn_Fndecl*fd=_tmp79;
# 291
Cyc_JumpAnalysis_anal_fd((env.fenv)->tables,fd);
goto _LL26;}case 2U: _LL2D: _tmp78=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp77)->f3;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp78;
# 294
Cyc_JumpAnalysis_anal_exp(env,e);
++(env.save_env).pat_var_depth;
goto _LL26;}case 3U: _LL2F: _LL30:
# 298
 goto _LL32;case 8U: _LL31: _LL32:
 goto _LL34;case 5U: _LL33: _LL34:
 goto _LL36;case 6U: _LL35: _LL36:
 goto _LL38;case 7U: _LL37: _LL38:
 goto _LL3A;case 13U: _LL39: _LL3A:
 goto _LL3C;case 14U: _LL3B: _LL3C:
 goto _LL3E;case 15U: _LL3D: _LL3E:
 goto _LL40;case 16U: _LL3F: _LL40:
 goto _LL42;case 11U: _LL41: _LL42:
 goto _LL44;case 12U: _LL43: _LL44:
 goto _LL46;case 9U: _LL45: _LL46:
 goto _LL48;default: _LL47: _LL48:
 goto _LL26;}_LL26:;}
# 312
Cyc_JumpAnalysis_anal_stmt(env,s1);
goto _LL0;}}_LL0:;}}
# 318
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_JumpAnalysis_FnEnv*_tmp7C=({struct Cyc_JumpAnalysis_FnEnv*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tables=tables,({
struct Cyc_Hashtable_Table*_tmpDD=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpAD->stmt_info=_tmpDD;}),_tmpAD->jumpers=0,({
# 322
struct Cyc_Hashtable_Table*_tmpDC=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_var_table)();_tmpAD->labels=_tmpDC;});_tmpAD;});
# 319
struct Cyc_JumpAnalysis_FnEnv*fenv=_tmp7C;
# 323
struct Cyc_JumpAnalysis_StmtEnv _tmp7D=({struct Cyc_JumpAnalysis_StmtEnv _tmpBC;_tmpBC.fenv=fenv,_tmpBC.continue_stmt=(void*)& Cyc_JumpAnalysis_NotLoop_j_val,_tmpBC.break_stmt=(void*)& Cyc_JumpAnalysis_NotLoop_j_val,_tmpBC.next_stmt=(void*)& Cyc_JumpAnalysis_FnEnd_j_val,
# 327
(_tmpBC.save_env).try_depth=0,(_tmpBC.save_env).pat_var_depth=0,(_tmpBC.save_env).enclosers=0;_tmpBC;});
# 323
struct Cyc_JumpAnalysis_StmtEnv senv=_tmp7D;
# 330
Cyc_JumpAnalysis_anal_stmt(senv,fd->body);{
# 332
struct Cyc_Hashtable_Table*pop_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
struct Cyc_Hashtable_Table*succ_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
struct Cyc_Hashtable_Table*pat_pop_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
# 336
struct Cyc_Hashtable_Table*_tmp7E=fenv->stmt_info;struct Cyc_Hashtable_Table*stmt_info=_tmp7E;
{struct Cyc_List_List*_tmp7F=fenv->jumpers;struct Cyc_List_List*jumpers=_tmp7F;for(0;jumpers != 0;jumpers=jumpers->tl){
struct _tuple13*_tmp80=(struct _tuple13*)jumpers->hd;struct _tuple13*_stmttmp3=_tmp80;struct _tuple13*_tmp81=_stmttmp3;void*_tmp83;struct Cyc_Absyn_Stmt*_tmp82;_LL1: _tmp82=_tmp81->f1;_tmp83=_tmp81->f2;_LL2: {struct Cyc_Absyn_Stmt*src_stmt=_tmp82;void*dest=_tmp83;
struct Cyc_JumpAnalysis_SaveEnv*_tmp84=
# 341
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(stmt_info,src_stmt);
# 339
struct Cyc_JumpAnalysis_SaveEnv*_stmttmp4=_tmp84;struct Cyc_JumpAnalysis_SaveEnv*_tmp85=_stmttmp4;struct Cyc_List_List*_tmp88;int _tmp87;int _tmp86;_LL4: _tmp86=_tmp85->try_depth;_tmp87=_tmp85->pat_var_depth;_tmp88=_tmp85->enclosers;_LL5: {int src_trydepth=_tmp86;int src_pvdepth=_tmp87;struct Cyc_List_List*src_enclose=_tmp88;
# 342
void*_tmp89=dest;struct Cyc_Absyn_Switch_clause*_tmp8A;struct Cyc_Absyn_Stmt*_tmp8B;struct _fat_ptr*_tmp8C;switch(*((int*)_tmp89)){case 0U: _LL7: _LL8:
# 344
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB6;_tmpB6.tag=0U,({struct _fat_ptr _tmpDE=({const char*_tmp8F="jump has no target";_tag_fat(_tmp8F,sizeof(char),19U);});_tmpB6.f1=_tmpDE;});_tmpB6;});void*_tmp8D[1U];_tmp8D[0]=& _tmp8E;({unsigned _tmpDF=src_stmt->loc;Cyc_Warn_err2(_tmpDF,_tag_fat(_tmp8D,sizeof(void*),1U));});});
goto _LL6;case 1U: _LL9: _LLA:
# 347
({struct Cyc_Warn_String_Warn_Warg_struct _tmp91=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB7;_tmpB7.tag=0U,({
struct _fat_ptr _tmpE0=({const char*_tmp92="break causes outer switch to implicitly fallthru";_tag_fat(_tmp92,sizeof(char),49U);});_tmpB7.f1=_tmpE0;});_tmpB7;});void*_tmp90[1U];_tmp90[0]=& _tmp91;({unsigned _tmpE1=src_stmt->loc;Cyc_Warn_err2(_tmpE1,_tag_fat(_tmp90,sizeof(void*),1U));});});
goto _LL6;case 2U: _LLB: _LLC:
# 351
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,src_stmt,0);
goto _LLE;case 5U: _LLD: _LLE:
# 354
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,src_stmt,src_trydepth);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,src_stmt,src_pvdepth);
goto _LL6;case 6U: _LLF: _tmp8C=((struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*)_tmp89)->f1;_LL10: {struct _fat_ptr*lab=_tmp8C;
# 358
struct Cyc_Absyn_Stmt*dest_stmt;
{struct _handler_cons _tmp93;_push_handler(& _tmp93);{int _tmp95=0;if(setjmp(_tmp93.handler))_tmp95=1;if(!_tmp95){dest_stmt=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key))Cyc_Hashtable_lookup)(fenv->labels,lab);;_pop_handler();}else{void*_tmp94=(void*)Cyc_Core_get_exn_thrown();void*_tmp96=_tmp94;void*_tmp97;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp96)->tag == Cyc_Core_Not_found){_LL16: _LL17:
# 362
({struct Cyc_Warn_String_Warn_Warg_struct _tmp99=({struct Cyc_Warn_String_Warn_Warg_struct _tmpBA;_tmpBA.tag=0U,({struct _fat_ptr _tmpE2=({const char*_tmp9D="label ";_tag_fat(_tmp9D,sizeof(char),7U);});_tmpBA.f1=_tmpE2;});_tmpBA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9;_tmpB9.tag=0U,_tmpB9.f1=*lab;_tmpB9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_String_Warn_Warg_struct _tmpB8;_tmpB8.tag=0U,({struct _fat_ptr _tmpE3=({const char*_tmp9C=" undefined";_tag_fat(_tmp9C,sizeof(char),11U);});_tmpB8.f1=_tmpE3;});_tmpB8;});void*_tmp98[3U];_tmp98[0]=& _tmp99,_tmp98[1]=& _tmp9A,_tmp98[2]=& _tmp9B;({unsigned _tmpE4=src_stmt->loc;Cyc_Warn_err2(_tmpE4,_tag_fat(_tmp98,sizeof(void*),3U));});});
goto L;}else{_LL18: _tmp97=_tmp96;_LL19: {void*exn=_tmp97;(int)_rethrow(exn);}}_LL15:;}}}{
# 365
struct Cyc_JumpAnalysis_SaveEnv*_tmp9E=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(stmt_info,dest_stmt);
# 365
struct Cyc_JumpAnalysis_SaveEnv*_stmttmp5=_tmp9E;struct Cyc_JumpAnalysis_SaveEnv*_tmp9F=_stmttmp5;struct Cyc_List_List*_tmpA0;_LL1B: _tmpA0=_tmp9F->enclosers;_LL1C: {struct Cyc_List_List*dest_enclose=_tmpA0;
# 367
if(dest_enclose != 0){
struct Cyc_Absyn_Stmt*_tmpA1=(struct Cyc_Absyn_Stmt*)dest_enclose->hd;struct Cyc_Absyn_Stmt*dest_scope=_tmpA1;
for(0;src_enclose != 0;src_enclose=src_enclose->tl){
if((struct Cyc_Absyn_Stmt*)src_enclose->hd == dest_scope)
break;}
if(src_enclose == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA3=({struct Cyc_Warn_String_Warn_Warg_struct _tmpBB;_tmpBB.tag=0U,({
struct _fat_ptr _tmpE5=({const char*_tmpA4="goto enters local scope or exception handler";_tag_fat(_tmpA4,sizeof(char),45U);});_tmpBB.f1=_tmpE5;});_tmpBB;});void*_tmpA2[1U];_tmpA2[0]=& _tmpA3;({unsigned _tmpE6=src_stmt->loc;Cyc_Warn_err2(_tmpE6,_tag_fat(_tmpA2,sizeof(void*),1U));});});
goto _LL6;}}
# 378
_tmp8B=dest_stmt;goto _LL12;}}}case 3U: _LL11: _tmp8B=((struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_tmp89)->f1;_LL12: {struct Cyc_Absyn_Stmt*dest_stmt=_tmp8B;
# 380
struct Cyc_JumpAnalysis_SaveEnv*_tmpA5=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(stmt_info,dest_stmt);
# 380
struct Cyc_JumpAnalysis_SaveEnv*_stmttmp6=_tmpA5;struct Cyc_JumpAnalysis_SaveEnv*_tmpA6=_stmttmp6;int _tmpA8;int _tmpA7;_LL1E: _tmpA7=_tmpA6->try_depth;_tmpA8=_tmpA6->pat_var_depth;_LL1F: {int dest_trydepth=_tmpA7;int dest_pvdepth=_tmpA8;
# 382
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,src_stmt,src_trydepth - dest_trydepth);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,src_stmt,src_pvdepth - dest_pvdepth);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,src_stmt,dest_stmt);
goto _LL6;}}default: _LL13: _tmp8A=((struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*)_tmp89)->f1;_LL14: {struct Cyc_Absyn_Switch_clause*dest_clause=_tmp8A;
# 387
struct Cyc_JumpAnalysis_SaveEnv*_tmpA9=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(stmt_info,dest_clause->body);
# 387
struct Cyc_JumpAnalysis_SaveEnv*_stmttmp7=_tmpA9;struct Cyc_JumpAnalysis_SaveEnv*_tmpAA=_stmttmp7;int _tmpAC;int _tmpAB;_LL21: _tmpAB=_tmpAA->try_depth;_tmpAC=_tmpAA->pat_var_depth;_LL22: {int dest_trydepth=_tmpAB;int dest_pvdepth=_tmpAC;
# 389
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,src_stmt,src_trydepth - dest_trydepth);{
int pv_pop;
if(Cyc_Tcpat_has_vars(dest_clause->pat_vars))
pv_pop=(1 + src_pvdepth)- dest_pvdepth;else{
# 394
pv_pop=src_pvdepth - dest_pvdepth;}
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,src_stmt,pv_pop);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,src_stmt,dest_clause->body);
goto _LL6;}}}}_LL6:;}}}}
# 400
L:((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->pop_tables,fd,pop_table);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->succ_tables,fd,succ_table);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->pat_pop_tables,fd,pat_pop_table);}}
# 405
static void Cyc_JumpAnalysis_jump_analysis_iter(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 408
for(0;tds != 0;tds=tds->tl){
void*_tmpAE=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_stmttmp8=_tmpAE;void*_tmpAF=_stmttmp8;struct Cyc_Absyn_Fndecl*_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB2;switch(*((int*)_tmpAF)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 goto _LL8;case 0U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 15U: _LL15: _LL16:
 goto _LL18;case 16U: _LL17: _LL18:
 goto _LL1A;case 11U: _LL19: _LL1A:
 goto _LL1C;case 12U: _LL1B: _LL1C:
 continue;case 9U: _LL1D: _tmpB2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpAF)->f2;_LL1E: {struct Cyc_List_List*tds2=_tmpB2;
_tmpB1=tds2;goto _LL20;}case 10U: _LL1F: _tmpB1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpAF)->f2;_LL20: {struct Cyc_List_List*tds2=_tmpB1;
Cyc_JumpAnalysis_jump_analysis_iter(tables,tds2);goto _LL0;}default: _LL21: _tmpB0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpAF)->f1;_LL22: {struct Cyc_Absyn_Fndecl*fd=_tmpB0;
Cyc_JumpAnalysis_anal_fd(tables,fd);goto _LL0;}}_LL0:;}}
# 430
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds){
struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpB3=({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct Cyc_Hashtable_Table*_tmpE9=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpB4->pop_tables=_tmpE9;}),({
struct Cyc_Hashtable_Table*_tmpE8=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpB4->succ_tables=_tmpE8;}),({
struct Cyc_Hashtable_Table*_tmpE7=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpB4->pat_pop_tables=_tmpE7;});_tmpB4;});
# 431
struct Cyc_JumpAnalysis_Jump_Anal_Result*tables=_tmpB3;
# 434
Cyc_JumpAnalysis_jump_analysis_iter(tables,tds);
return tables;}

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
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "/tmp/cyclone/include/list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "/tmp/cyclone/include/cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 50 "/tmp/cyclone/include/string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "/tmp/cyclone/src/absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882 "/tmp/cyclone/src/absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 900
void*Cyc_Absyn_compress(void*);
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 928
extern void*Cyc_Absyn_empty_effect;
# 930
extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 947
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 975
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);struct Cyc_RgnOrder_RgnPO;
# 33 "/tmp/cyclone/src/rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned);
# 40
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
# 44
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*,void*rgn1,void*rgn2);
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "/tmp/cyclone/src/relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 99 "/tmp/cyclone/src/tcutil.h"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 104
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 128
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 130
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 167
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 169
struct _tuple11 Cyc_Tcutil_swap_kind(void*,void*);
# 190
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 194
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 209
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 250
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 252
int Cyc_Tcutil_new_tvar_id (void);
# 254
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "/tmp/cyclone/src/warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "/tmp/cyclone/include/dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 173 "/tmp/cyclone/include/dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 67 "/tmp/cyclone/src/tcenv.h"
void*Cyc_Tcenv_env_err(struct _fat_ptr msg);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 29 "/tmp/cyclone/src/currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 31
void*Cyc_CurRgn_curr_rgn_type (void);struct _tuple12{struct Cyc_List_List*f1;void*f2;};
# 33
struct _tuple12 Cyc_CurRgn_add_curr_tvar(struct Cyc_List_List*tvs,void*cap);char Cyc_Tcenv_Env_error[10U]="Env_error";
# 51 "/tmp/cyclone/src/tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 53
void*Cyc_Tcenv_env_err(struct _fat_ptr msg){
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct _tuple13{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{enum Cyc_Tcenv_NewStatus in_new;int in_notreadctxt: 1;int in_lhs: 1;int abstract_ok: 1;int in_stmt_exp: 1;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct _tuple13*ctrl_env;void*capability;void*curr_rgn;void*curr_lifo_rgn;struct Cyc_Tcenv_FenvFlags flags;};
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void){
# 117
struct Cyc_Tcenv_Genv*_tmp0=({struct Cyc_Tcenv_Genv*_tmp3=_cycalloc(sizeof(*_tmp3));({struct Cyc_Dict_Dict _tmp109=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->aggrdecls=_tmp109;}),({
struct Cyc_Dict_Dict _tmp108=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->datatypedecls=_tmp108;}),({
struct Cyc_Dict_Dict _tmp107=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->enumdecls=_tmp107;}),({
struct Cyc_Dict_Dict _tmp106=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->typedefs=_tmp106;}),({
struct Cyc_Dict_Dict _tmp105=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->ordinaries=_tmp105;});_tmp3;});
# 117
struct Cyc_Tcenv_Genv*ae=_tmp0;
# 122
({struct Cyc_Dict_Dict _tmp10D=({struct Cyc_Dict_Dict _tmp10C=ae->datatypedecls;struct _tuple0*_tmp10B=Cyc_Absyn_exn_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp10C,_tmp10B,({struct Cyc_Absyn_Datatypedecl**_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Datatypedecl*_tmp10A=Cyc_Absyn_exn_tud();*_tmp1=_tmp10A;});_tmp1;}));});ae->datatypedecls=_tmp10D;});
return({struct Cyc_Tcenv_Tenv*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->ns=0,_tmp2->ae=ae,_tmp2->le=0,_tmp2->allow_valueof=0,_tmp2->in_extern_c_include=0,_tmp2->in_tempest=0,_tmp2->tempest_generalize=0,_tmp2->in_extern_c_inc_repeat=0;_tmp2;});}struct _tuple14{void*f1;int f2;};
# 125
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q,int is_use){
struct _tuple14*_tmp4=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);struct _tuple14*ans=_tmp4;
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 131
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->aggrdecls,q);}
# 134
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);}
# 137
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmp5=_region_malloc(r,sizeof(*_tmp5));({struct Cyc_Absyn_Datatypedecl**_tmp10E=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);*_tmp5=_tmp10E;});_tmp5;});}
# 140
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);}
# 143
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->typedefs,q);}
# 149
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp6=te->le;struct Cyc_Tcenv_Fenv*le=_tmp6;
if(le == 0)((int(*)(struct _fat_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return le;}
# 154
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _fat_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp7="put_fenv";_tag_fat(_tmp7,sizeof(char),9U);}));{
struct Cyc_Tcenv_Tenv*_tmp8=({struct Cyc_Tcenv_Tenv*_tmp9=_cycalloc(sizeof(*_tmp9));*_tmp9=*te;_tmp9;});struct Cyc_Tcenv_Tenv*ans=_tmp8;
ans->le=fe;
return ans;}}
# 160
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmpA=({struct Cyc_Tcenv_Tenv*_tmpB=_cycalloc(sizeof(*_tmpB));*_tmpB=*te;_tmpB;});struct Cyc_Tcenv_Tenv*ans=_tmpA;
ans->le=0;
return ans;}
# 166
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
return(({struct Cyc_Tcenv_Tenv*_tmp10F=te;Cyc_Tcenv_get_fenv(_tmp10F,({const char*_tmpC="return_typ";_tag_fat(_tmpC,sizeof(char),11U);}));})->shared)->return_typ;}
# 170
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD=te->le;struct Cyc_Tcenv_Fenv*le=_tmpD;
if(te->le == 0)return 0;
return((struct Cyc_Tcenv_Fenv*)_check_null(le))->type_vars;}
# 175
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpE=_cycalloc(sizeof(*_tmpE));({struct Cyc_List_List*_tmp110=Cyc_Tcenv_lookup_type_vars(te);_tmpE->v=_tmp110;});_tmpE;});}
# 179
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
struct Cyc_Tcenv_Fenv*_tmpF=({struct Cyc_Tcenv_Fenv*_tmp12=_cycalloc(sizeof(*_tmp12));({struct Cyc_Tcenv_Fenv _tmp112=*({struct Cyc_Tcenv_Tenv*_tmp111=te;Cyc_Tcenv_get_fenv(_tmp111,({const char*_tmp11="add_type_vars";_tag_fat(_tmp11,sizeof(char),14U);}));});*_tmp12=_tmp112;});_tmp12;});struct Cyc_Tcenv_Fenv*fe=_tmpF;
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,fe->type_vars);struct Cyc_List_List*new_tvs=_tmp10;
Cyc_Tcutil_check_unique_tvars(loc,new_tvs);
fe->type_vars=new_tvs;
return Cyc_Tcenv_put_fenv(te,fe);}}
# 188
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp13=te->le;struct Cyc_Tcenv_Fenv*le=_tmp13;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp14=({struct Cyc_Tcenv_Fenv*_tmp15=_cycalloc(sizeof(*_tmp15));*_tmp15=*le;_tmp15;});struct Cyc_Tcenv_Fenv*ans=_tmp14;
(ans->flags).in_new=status;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 195
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16=te->le;struct Cyc_Tcenv_Fenv*le=_tmp16;
if(le == 0)return Cyc_Tcenv_NoneNew;
return(le->flags).in_new;}
# 200
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17=te->le;struct Cyc_Tcenv_Fenv*le=_tmp17;
if(le == 0)return 0;
return(le->flags).abstract_ok;}
# 205
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18=te->le;struct Cyc_Tcenv_Fenv*le=_tmp18;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp19=({struct Cyc_Tcenv_Fenv*_tmp1A=_cycalloc(sizeof(*_tmp1A));*_tmp1A=*le;_tmp1A;});struct Cyc_Tcenv_Fenv*ans=_tmp19;
(ans->flags).abstract_ok=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 212
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1B=te->le;struct Cyc_Tcenv_Fenv*le=_tmp1B;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1C=({struct Cyc_Tcenv_Fenv*_tmp1D=_cycalloc(sizeof(*_tmp1D));*_tmp1D=*le;_tmp1D;});struct Cyc_Tcenv_Fenv*ans=_tmp1C;
(ans->flags).abstract_ok=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 219
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1E=te->le;struct Cyc_Tcenv_Fenv*le=_tmp1E;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1F=({struct Cyc_Tcenv_Fenv*_tmp20=_cycalloc(sizeof(*_tmp20));*_tmp20=*le;_tmp20;});struct Cyc_Tcenv_Fenv*ans=_tmp1F;
(ans->flags).in_notreadctxt=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 226
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp21=te->le;struct Cyc_Tcenv_Fenv*le=_tmp21;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp22=({struct Cyc_Tcenv_Fenv*_tmp23=_cycalloc(sizeof(*_tmp23));*_tmp23=*le;_tmp23;});struct Cyc_Tcenv_Fenv*ans=_tmp22;
(ans->flags).in_notreadctxt=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 233
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp24=te->le;struct Cyc_Tcenv_Fenv*le=_tmp24;
if(le == 0)return 0;
return(le->flags).in_notreadctxt;}
# 238
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25=te->le;struct Cyc_Tcenv_Fenv*le=_tmp25;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp26=({struct Cyc_Tcenv_Fenv*_tmp27=_cycalloc(sizeof(*_tmp27));*_tmp27=*le;_tmp27;});struct Cyc_Tcenv_Fenv*ans=_tmp26;
(ans->flags).in_lhs=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 245
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp28=te->le;struct Cyc_Tcenv_Fenv*le=_tmp28;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp29=({struct Cyc_Tcenv_Fenv*_tmp2A=_cycalloc(sizeof(*_tmp2A));*_tmp2A=*le;_tmp2A;});struct Cyc_Tcenv_Fenv*ans=_tmp29;
(ans->flags).in_lhs=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 252
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B=te->le;struct Cyc_Tcenv_Fenv*le=_tmp2B;
if(le == 0)return 0;
return(le->flags).in_lhs;}
# 257
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2C=te->le;struct Cyc_Tcenv_Fenv*le=_tmp2C;
if(le == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp2D=({struct Cyc_Tcenv_Fenv*_tmp2E=_cycalloc(sizeof(*_tmp2E));*_tmp2E=*le;_tmp2E;});struct Cyc_Tcenv_Fenv*ans=_tmp2D;
(ans->flags).in_stmt_exp=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 264
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2F=te->le;struct Cyc_Tcenv_Fenv*le=_tmp2F;
if(le == 0)return 0;
return(le->flags).in_stmt_exp;}
# 271
const struct _tuple13*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 273
const struct _tuple13*_tmp30=({struct Cyc_Tcenv_Tenv*_tmp113=te;Cyc_Tcenv_get_fenv(_tmp113,({const char*_tmp32="process_fallthru";_tag_fat(_tmp32,sizeof(char),17U);}));})->ctrl_env;const struct _tuple13*ans=_tmp30;
if(ans != (const struct _tuple13*)0)
({struct Cyc_Absyn_Switch_clause**_tmp114=({struct Cyc_Absyn_Switch_clause**_tmp31=_cycalloc(sizeof(*_tmp31));*_tmp31=(*ans).f1;_tmp31;});*clauseopt=_tmp114;});
return ans;}
# 279
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 281
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,_tmp33->tl=ft_typ;_tmp33;});}{
struct _tuple13*_tmp34=({struct _tuple13*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->f1=clause,_tmp38->f2=new_tvs,({struct Cyc_List_List*_tmp115=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);_tmp38->f3=_tmp115;});_tmp38;});struct _tuple13*new_ctrl_env=_tmp34;
struct Cyc_Tcenv_Fenv*_tmp35=({struct Cyc_Tcenv_Fenv*_tmp37=_cycalloc(sizeof(*_tmp37));({struct Cyc_Tcenv_Fenv _tmp117=*({struct Cyc_Tcenv_Tenv*_tmp116=te;Cyc_Tcenv_get_fenv(_tmp116,({const char*_tmp36="set_fallthru";_tag_fat(_tmp36,sizeof(char),13U);}));});*_tmp37=_tmp117;});_tmp37;});struct Cyc_Tcenv_Fenv*new_fe=_tmp35;
new_fe->ctrl_env=(const struct _tuple13*)new_ctrl_env;
return Cyc_Tcenv_put_fenv(te,new_fe);}}
# 290
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Fenv*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct Cyc_Tcenv_Fenv _tmp119=*({struct Cyc_Tcenv_Tenv*_tmp118=te;Cyc_Tcenv_get_fenv(_tmp118,({const char*_tmp39="clear_fallthru";_tag_fat(_tmp39,sizeof(char),15U);}));});*_tmp3A=_tmp119;});_tmp3A;});
fe->ctrl_env=0;
return Cyc_Tcenv_put_fenv(te,fe);}
# 296
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3B=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp3E=_cycalloc(sizeof(*_tmp3E));*_tmp3E=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp3E;});struct Cyc_Tcenv_Fenv*fe=_tmp3B;
struct Cyc_Tcenv_Tenv*_tmp3C=({struct Cyc_Tcenv_Tenv*_tmp3D=_cycalloc(sizeof(*_tmp3D));*_tmp3D=*te;_tmp3D;});struct Cyc_Tcenv_Tenv*ans=_tmp3C;
ans->allow_valueof=1;
return ans;}
# 302
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3F=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp42=_cycalloc(sizeof(*_tmp42));*_tmp42=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp42;});struct Cyc_Tcenv_Fenv*fe=_tmp3F;
struct Cyc_Tcenv_Tenv*_tmp40=({struct Cyc_Tcenv_Tenv*_tmp41=_cycalloc(sizeof(*_tmp41));*_tmp41=*te;_tmp41;});struct Cyc_Tcenv_Tenv*ans=_tmp40;
ans->in_extern_c_include=1;
return ans;}
# 308
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp43=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp46;});struct Cyc_Tcenv_Fenv*fe=_tmp43;
struct Cyc_Tcenv_Tenv*_tmp44=({struct Cyc_Tcenv_Tenv*_tmp45=_cycalloc(sizeof(*_tmp45));*_tmp45=*te;_tmp45;});struct Cyc_Tcenv_Tenv*ans=_tmp44;
ans->in_tempest=1;
return ans;}
# 314
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp47=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp4A;});struct Cyc_Tcenv_Fenv*fe=_tmp47;
struct Cyc_Tcenv_Tenv*_tmp48=({struct Cyc_Tcenv_Tenv*_tmp49=_cycalloc(sizeof(*_tmp49));*_tmp49=*te;_tmp49;});struct Cyc_Tcenv_Tenv*ans=_tmp48;
ans->in_tempest=0;
return ans;}
# 320
int Cyc_Tcenv_in_tempest(struct Cyc_Tcenv_Tenv*te){
return te->in_tempest;}
# 323
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*rgn,int opened,int lifo){
struct Cyc_Tcenv_Fenv*_tmp4B=({struct Cyc_Tcenv_Fenv*_tmp53=_cycalloc(sizeof(*_tmp53));({struct Cyc_Tcenv_Fenv _tmp11B=*({struct Cyc_Tcenv_Tenv*_tmp11A=te;Cyc_Tcenv_get_fenv(_tmp11A,({const char*_tmp52="add_region";_tag_fat(_tmp52,sizeof(char),11U);}));});*_tmp53=_tmp11B;});_tmp53;});struct Cyc_Tcenv_Fenv*ans=_tmp4B;
struct Cyc_RgnOrder_RgnPO*_tmp4C=ans->region_order;struct Cyc_RgnOrder_RgnPO*region_order=_tmp4C;
{void*_tmp4D=Cyc_Absyn_compress(rgn);void*_stmttmp0=_tmp4D;void*_tmp4E=_stmttmp0;struct Cyc_Absyn_Tvar*_tmp4F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->tag == 2U){_LL1: _tmp4F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->f1;_LL2: {struct Cyc_Absyn_Tvar*x=_tmp4F;
region_order=Cyc_RgnOrder_add_youngest(region_order,x,opened);goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 330
ans->region_order=region_order;
({void*_tmp11E=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({void*_tmp11D=Cyc_Absyn_access_eff(rgn);_tmp51->hd=_tmp11D;}),({
struct Cyc_List_List*_tmp11C=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=ans->capability,_tmp50->tl=0;_tmp50;});_tmp51->tl=_tmp11C;});_tmp51;}));
# 331
ans->capability=_tmp11E;});
# 334
if(lifo)ans->curr_lifo_rgn=rgn;
return Cyc_Tcenv_put_fenv(te,ans);}
# 338
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
struct Cyc_Tcenv_Fenv*_tmp54=({struct Cyc_Tcenv_Fenv*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Tcenv_Fenv _tmp120=*({struct Cyc_Tcenv_Tenv*_tmp11F=te;Cyc_Tcenv_get_fenv(_tmp11F,({const char*_tmp5A="new_named_block";_tag_fat(_tmp5A,sizeof(char),16U);}));});*_tmp5B=_tmp120;});_tmp5B;});struct Cyc_Tcenv_Fenv*ans=_tmp54;
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp55=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=2U,_tmp59->f1=block_rgn;_tmp59;});struct Cyc_Absyn_VarType_Absyn_Type_struct*block_typ=_tmp55;
({struct Cyc_List_List*_tmp121=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->hd=block_rgn,_tmp56->tl=ans->type_vars;_tmp56;});ans->type_vars=_tmp121;});
Cyc_Tcutil_check_unique_tvars(loc,ans->type_vars);
({struct Cyc_RgnOrder_RgnPO*_tmp122=Cyc_RgnOrder_add_youngest(ans->region_order,block_rgn,0);ans->region_order=_tmp122;});
({void*_tmp125=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({void*_tmp124=Cyc_Absyn_access_eff((void*)block_typ);_tmp58->hd=_tmp124;}),({
struct Cyc_List_List*_tmp123=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=ans->capability,_tmp57->tl=0;_tmp57;});_tmp58->tl=_tmp123;});_tmp58;}));
# 344
ans->capability=_tmp125;});
# 346
ans->curr_rgn=(void*)block_typ;
return Cyc_Tcenv_put_fenv(te,ans);}
# 350
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0U,& Cyc_Tcutil_rk};
# 352
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(loc,te,t);}struct _tuple15{void*f1;void*f2;};
# 358
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned loc){
# 360
struct Cyc_Tcenv_Fenv*_tmp5C=({struct Cyc_Tcenv_Fenv*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct Cyc_Tcenv_Fenv _tmp127=*({struct Cyc_Tcenv_Tenv*_tmp126=te;Cyc_Tcenv_get_fenv(_tmp126,({const char*_tmp5D="new_outlives_constraints";_tag_fat(_tmp5D,sizeof(char),25U);}));});*_tmp5E=_tmp127;});_tmp5E;});struct Cyc_Tcenv_Fenv*ans=_tmp5C;
for(0;cs != 0;cs=cs->tl){
({struct Cyc_RgnOrder_RgnPO*_tmp128=
Cyc_RgnOrder_add_outlives_constraint(ans->region_order,(*((struct _tuple15*)cs->hd)).f1,(*((struct _tuple15*)cs->hd)).f2,loc);
# 362
ans->region_order=_tmp128;});}
# 364
return Cyc_Tcenv_put_fenv(te,ans);}
# 367
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned loc){
# 371
r1=Cyc_Absyn_compress(r1);
r2=Cyc_Absyn_compress(r2);{
struct Cyc_Absyn_Kind*_tmp5F=Cyc_Tcutil_type_kind(r1);struct Cyc_Absyn_Kind*r1k=_tmp5F;
struct Cyc_Absyn_Kind*_tmp60=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*r2k=_tmp60;
# 377
int r1_le_r2=Cyc_Tcutil_kind_leq(r1k,r2k);
int r2_le_r1=Cyc_Tcutil_kind_leq(r2k,r1k);
if(!r1_le_r2 && !r2_le_r1){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp62=({struct Cyc_Warn_String_Warn_Warg_struct _tmpF9;_tmpF9.tag=0U,({struct _fat_ptr _tmp129=({const char*_tmp6F="Cannot compare region handles for ";_tag_fat(_tmp6F,sizeof(char),35U);});_tmpF9.f1=_tmp129;});_tmpF9;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF8;_tmpF8.tag=2U,_tmpF8.f1=(void*)r1;_tmpF8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmpF7;_tmpF7.tag=0U,({struct _fat_ptr _tmp12A=({const char*_tmp6E=" and ";_tag_fat(_tmp6E,sizeof(char),6U);});_tmpF7.f1=_tmp12A;});_tmpF7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF6;_tmpF6.tag=2U,_tmpF6.f1=(void*)r2;_tmpF6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp66=({struct Cyc_Warn_String_Warn_Warg_struct _tmpF5;_tmpF5.tag=0U,({
struct _fat_ptr _tmp12B=({const char*_tmp6D="\n  kinds ";_tag_fat(_tmp6D,sizeof(char),10U);});_tmpF5.f1=_tmp12B;});_tmpF5;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp67=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmpF4;_tmpF4.tag=9U,_tmpF4.f1=r1k;_tmpF4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp68=({struct Cyc_Warn_String_Warn_Warg_struct _tmpF3;_tmpF3.tag=0U,({struct _fat_ptr _tmp12C=({const char*_tmp6C=" and ";_tag_fat(_tmp6C,sizeof(char),6U);});_tmpF3.f1=_tmp12C;});_tmpF3;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp69=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmpF2;_tmpF2.tag=9U,_tmpF2.f1=r2k;_tmpF2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmpF1;_tmpF1.tag=0U,({struct _fat_ptr _tmp12D=({const char*_tmp6B=" are not compatible";_tag_fat(_tmp6B,sizeof(char),20U);});_tmpF1.f1=_tmp12D;});_tmpF1;});void*_tmp61[9U];_tmp61[0]=& _tmp62,_tmp61[1]=& _tmp63,_tmp61[2]=& _tmp64,_tmp61[3]=& _tmp65,_tmp61[4]=& _tmp66,_tmp61[5]=& _tmp67,_tmp61[6]=& _tmp68,_tmp61[7]=& _tmp69,_tmp61[8]=& _tmp6A;({unsigned _tmp12E=loc;Cyc_Warn_err2(_tmp12E,_tag_fat(_tmp61,sizeof(void*),9U));});});
return Cyc_Tcenv_new_outlives_constraints(te,0,loc);}else{
# 384
if(r1_le_r2 && !r2_le_r1)
({struct _tuple11*_tmp131=({struct _tuple11*_tmp70=_cycalloc(sizeof(*_tmp70));({struct _tuple11 _tmp130=({void*_tmp12F=r2;Cyc_Tcutil_swap_kind(_tmp12F,Cyc_Tcutil_kind_to_bound(r1k));});*_tmp70=_tmp130;});_tmp70;});*oldtv=_tmp131;});else{
if(!r1_le_r2 && r2_le_r1)
({struct _tuple11*_tmp134=({struct _tuple11*_tmp71=_cycalloc(sizeof(*_tmp71));({struct _tuple11 _tmp133=({void*_tmp132=r1;Cyc_Tcutil_swap_kind(_tmp132,Cyc_Tcutil_kind_to_bound(r2k));});*_tmp71=_tmp133;});_tmp71;});*oldtv=_tmp134;});}}{
# 390
struct Cyc_List_List*_tmp72=0;struct Cyc_List_List*bds=_tmp72;
if((r1 != Cyc_Absyn_heap_rgn_type && r1 != Cyc_Absyn_unique_rgn_type)&& r1 != Cyc_Absyn_refcnt_rgn_type)
bds=({struct Cyc_List_List*_tmp74=_cycalloc(sizeof(*_tmp74));({struct _tuple15*_tmp136=({struct _tuple15*_tmp73=_cycalloc(sizeof(*_tmp73));({void*_tmp135=Cyc_Absyn_access_eff(r1);_tmp73->f1=_tmp135;}),_tmp73->f2=r2;_tmp73;});_tmp74->hd=_tmp136;}),_tmp74->tl=bds;_tmp74;});
if((r2 != Cyc_Absyn_heap_rgn_type && r2 != Cyc_Absyn_unique_rgn_type)&& r2 != Cyc_Absyn_refcnt_rgn_type)
bds=({struct Cyc_List_List*_tmp76=_cycalloc(sizeof(*_tmp76));({struct _tuple15*_tmp138=({struct _tuple15*_tmp75=_cycalloc(sizeof(*_tmp75));({void*_tmp137=Cyc_Absyn_access_eff(r2);_tmp75->f1=_tmp137;}),_tmp75->f2=r1;_tmp75;});_tmp76->hd=_tmp138;}),_tmp76->tl=bds;_tmp76;});
return Cyc_Tcenv_new_outlives_constraints(te,bds,loc);}}}
# 398
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp77=te->le;struct Cyc_Tcenv_Fenv*le=_tmp77;
if(le == 0)return Cyc_Absyn_heap_rgn_type;
return le->curr_rgn;}
# 403
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp78=te->le;struct Cyc_Tcenv_Fenv*le=_tmp78;
if(le == 0)return Cyc_Absyn_heap_rgn_type;
return le->curr_lifo_rgn;}
# 408
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp79=te->le;struct Cyc_Tcenv_Fenv*le=_tmp79;
if(le == 0)return 0;
return le->region_order;}
# 417
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Tenv*_tmp139=te;Cyc_Tcenv_get_fenv(_tmp139,({const char*_tmp7E="check_rgn_accessible";_tag_fat(_tmp7E,sizeof(char),21U);}));});
if(Cyc_Tcutil_region_in_effect(0,rgn,fe->capability)||
 Cyc_Tcutil_region_in_effect(1,rgn,fe->capability))
return;
if(({struct Cyc_RgnOrder_RgnPO*_tmp13B=fe->region_order;void*_tmp13A=Cyc_Absyn_access_eff(rgn);Cyc_RgnOrder_eff_outlives_eff(_tmp13B,_tmp13A,fe->capability);}))
return;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmpFB;_tmpFB.tag=0U,({struct _fat_ptr _tmp13C=({const char*_tmp7D="Expression accesses unavailable region ";_tag_fat(_tmp7D,sizeof(char),40U);});_tmpFB.f1=_tmp13C;});_tmpFB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpFA;_tmpFA.tag=2U,_tmpFA.f1=(void*)rgn;_tmpFA;});void*_tmp7A[2U];_tmp7A[0]=& _tmp7B,_tmp7A[1]=& _tmp7C;({unsigned _tmp13D=loc;Cyc_Warn_err2(_tmp13D,_tag_fat(_tmp7A,sizeof(void*),2U));});});}
# 430
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
return({struct Cyc_RgnOrder_RgnPO*_tmp13F=Cyc_Tcenv_curr_rgnpo(te);void*_tmp13E=rt_a;Cyc_RgnOrder_rgn_outlives_rgn(_tmp13F,_tmp13E,rt_b);});}struct _tuple16{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned f4;};
# 436
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp7F=({
struct Cyc_Tcenv_Tenv*_tmp140=te;Cyc_Tcenv_get_fenv(_tmp140,({const char*_tmp86="check_effect_accessible";_tag_fat(_tmp86,sizeof(char),24U);}));});
# 437
struct Cyc_Tcenv_Fenv*_stmttmp1=_tmp7F;struct Cyc_Tcenv_Fenv*_tmp80=_stmttmp1;struct Cyc_Tcenv_SharedFenv*_tmp83;struct Cyc_RgnOrder_RgnPO*_tmp82;void*_tmp81;_LL1: _tmp81=_tmp80->capability;_tmp82=_tmp80->region_order;_tmp83=_tmp80->shared;_LL2: {void*capability=_tmp81;struct Cyc_RgnOrder_RgnPO*ro=_tmp82;struct Cyc_Tcenv_SharedFenv*shared=_tmp83;
# 439
if(Cyc_Tcutil_subset_effect(0,eff,capability))
return;
if(Cyc_RgnOrder_eff_outlives_eff(ro,eff,capability))
return;
({struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));
({struct _tuple16*_tmp141=({struct _tuple16*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=capability,_tmp84->f2=eff,_tmp84->f3=ro,_tmp84->f4=loc;_tmp84;});_tmp85->hd=_tmp141;}),_tmp85->tl=shared->delayed_effect_checks;_tmp85;});
# 443
shared->delayed_effect_checks=_tmp142;});}}
# 446
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp87=(({
struct Cyc_Tcenv_Tenv*_tmp143=te;Cyc_Tcenv_get_fenv(_tmp143,({const char*_tmp95="check_delayed_constraints";_tag_fat(_tmp95,sizeof(char),26U);}));})->shared)->delayed_effect_checks;
# 447
struct Cyc_List_List*checks=_tmp87;
# 449
for(0;checks != 0;checks=checks->tl){
struct _tuple16*_tmp88=(struct _tuple16*)checks->hd;struct _tuple16*_stmttmp2=_tmp88;struct _tuple16*_tmp89=_stmttmp2;unsigned _tmp8D;struct Cyc_RgnOrder_RgnPO*_tmp8C;void*_tmp8B;void*_tmp8A;_LL1: _tmp8A=_tmp89->f1;_tmp8B=_tmp89->f2;_tmp8C=_tmp89->f3;_tmp8D=_tmp89->f4;_LL2: {void*capability=_tmp8A;void*eff=_tmp8B;struct Cyc_RgnOrder_RgnPO*rgn_order=_tmp8C;unsigned loc=_tmp8D;
if(Cyc_Tcutil_subset_effect(1,eff,capability))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(rgn_order,eff,capability))
continue;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_String_Warn_Warg_struct _tmpFF;_tmpFF.tag=0U,({struct _fat_ptr _tmp144=({const char*_tmp94="Capability \n";_tag_fat(_tmp94,sizeof(char),13U);});_tmpFF.f1=_tmp144;});_tmpFF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp90=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpFE;_tmpFE.tag=2U,_tmpFE.f1=(void*)capability;_tmpFE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp91=({struct Cyc_Warn_String_Warn_Warg_struct _tmpFD;_tmpFD.tag=0U,({
struct _fat_ptr _tmp145=({const char*_tmp93="\ndoes not cover function's effect\n";_tag_fat(_tmp93,sizeof(char),35U);});_tmpFD.f1=_tmp145;});_tmpFD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp92=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpFC;_tmpFC.tag=2U,_tmpFC.f1=(void*)eff;_tmpFC;});void*_tmp8E[4U];_tmp8E[0]=& _tmp8F,_tmp8E[1]=& _tmp90,_tmp8E[2]=& _tmp91,_tmp8E[3]=& _tmp92;({unsigned _tmp146=loc;Cyc_Warn_err2(_tmp146,_tag_fat(_tmp8E,sizeof(void*),4U));});});}}}struct _tuple17{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned f3;};
# 464
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*po){
# 466
struct Cyc_Tcenv_Fenv*_tmp96=te->le;struct Cyc_Tcenv_Fenv*le=_tmp96;
if(le == 0){
# 469
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple15*)po->hd)).f1,Cyc_Absyn_empty_effect)|| !({
void*_tmp147=Cyc_Absyn_access_eff((*((struct _tuple15*)po->hd)).f2);Cyc_Tcutil_subset_effect(1,_tmp147,Cyc_Absyn_empty_effect);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp100;_tmp100.tag=0U,({struct _fat_ptr _tmp148=({const char*_tmp99="the required region ordering is not satisfied here";_tag_fat(_tmp99,sizeof(char),51U);});_tmp100.f1=_tmp148;});_tmp100;});void*_tmp97[1U];_tmp97[0]=& _tmp98;({unsigned _tmp149=loc;Cyc_Warn_err2(_tmp149,_tag_fat(_tmp97,sizeof(void*),1U));});});}
return;}{
# 475
struct Cyc_Tcenv_Fenv*_tmp9A=le;struct Cyc_Tcenv_Fenv*_stmttmp3=_tmp9A;struct Cyc_Tcenv_Fenv*_tmp9B=_stmttmp3;struct Cyc_Tcenv_SharedFenv*_tmp9D;struct Cyc_RgnOrder_RgnPO*_tmp9C;_LL1: _tmp9C=_tmp9B->region_order;_tmp9D=_tmp9B->shared;_LL2: {struct Cyc_RgnOrder_RgnPO*ro=_tmp9C;struct Cyc_Tcenv_SharedFenv*shared=_tmp9D;
if(!Cyc_RgnOrder_satisfies_constraints(ro,po,Cyc_Absyn_heap_rgn_type,0))
({struct Cyc_List_List*_tmp14B=({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));
({struct _tuple17*_tmp14A=({struct _tuple17*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->f1=ro,_tmp9E->f2=po,_tmp9E->f3=loc;_tmp9E;});_tmp9F->hd=_tmp14A;}),_tmp9F->tl=shared->delayed_constraint_checks;_tmp9F;});
# 477
shared->delayed_constraint_checks=_tmp14B;});}}}
# 481
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmpA0=(({
struct Cyc_Tcenv_Tenv*_tmp14C=te;Cyc_Tcenv_get_fenv(_tmp14C,({const char*_tmpA9="check_delayed_constraints";_tag_fat(_tmpA9,sizeof(char),26U);}));})->shared)->delayed_constraint_checks;
# 482
struct Cyc_List_List*checks=_tmpA0;
# 484
for(0;checks != 0;checks=checks->tl){
struct _tuple17*_tmpA1=(struct _tuple17*)checks->hd;struct _tuple17*_stmttmp4=_tmpA1;struct _tuple17*_tmpA2=_stmttmp4;unsigned _tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_RgnOrder_RgnPO*_tmpA3;_LL1: _tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;_tmpA5=_tmpA2->f3;_LL2: {struct Cyc_RgnOrder_RgnPO*rgn_order=_tmpA3;struct Cyc_List_List*po=_tmpA4;unsigned loc=_tmpA5;
if(!Cyc_RgnOrder_satisfies_constraints(rgn_order,po,Cyc_Absyn_heap_rgn_type,1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp101;_tmp101.tag=0U,({struct _fat_ptr _tmp14D=({const char*_tmpA8="the required region ordering is not satisfied here";_tag_fat(_tmpA8,sizeof(char),51U);});_tmp101.f1=_tmp14D;});_tmp101;});void*_tmpA6[1U];_tmpA6[0]=& _tmpA7;({unsigned _tmp14E=loc;Cyc_Warn_err2(_tmp14E,_tag_fat(_tmpA6,sizeof(void*),1U));});});}}}
# 491
static struct Cyc_Tcenv_SharedFenv*Cyc_Tcenv_new_shared_fenv(void*ret){
return({struct Cyc_Tcenv_SharedFenv*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->return_typ=ret,_tmpAA->delayed_effect_checks=0,_tmpAA->delayed_constraint_checks=0;_tmpAA;});}
# 497
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpC3=_cycalloc(sizeof(*_tmpC3));
({struct _fat_ptr*_tmp152=({struct _fat_ptr*_tmpC2=_cycalloc(sizeof(*_tmpC2));({struct _fat_ptr _tmp151=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC1=({struct Cyc_String_pa_PrintArg_struct _tmp102;_tmp102.tag=0U,_tmp102.f1=(struct _fat_ptr)((struct _fat_ptr)*(*fd->name).f2);_tmp102;});void*_tmpBF[1U];_tmpBF[0]=& _tmpC1;({struct _fat_ptr _tmp150=({const char*_tmpC0="`%s";_tag_fat(_tmpC0,sizeof(char),4U);});Cyc_aprintf(_tmp150,_tag_fat(_tmpBF,sizeof(void*),1U));});});*_tmpC2=_tmp151;});_tmpC2;});_tmpC3->name=_tmp152;}),({
int _tmp14F=Cyc_Tcutil_new_tvar_id();_tmpC3->identity=_tmp14F;}),_tmpC3->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpC3;});
struct _tuple12 _tmpAB=Cyc_CurRgn_add_curr_tvar((fd->i).tvars,(void*)_check_null((fd->i).effect));struct _tuple12 _stmttmp5=_tmpAB;struct _tuple12 _tmpAC=_stmttmp5;void*_tmpAE;struct Cyc_List_List*_tmpAD;_LL1: _tmpAD=_tmpAC.f1;_tmpAE=_tmpAC.f2;_LL2: {struct Cyc_List_List*tvs1=_tmpAD;void*cap=_tmpAE;
struct Cyc_List_List*_tmpAF=({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=rgn0,_tmpBE->tl=tvs1;_tmpBE;});struct Cyc_List_List*tvs=_tmpAF;
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
struct Cyc_RgnOrder_RgnPO*_tmpB0=Cyc_RgnOrder_initial_fn_po(tvs1,(fd->i).rgn_po,cap,rgn0,loc);struct Cyc_RgnOrder_RgnPO*rgn_po=_tmpB0;
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=2U,_tmpBD->f1=rgn0;_tmpBD;});
{struct Cyc_List_List*_tmpB1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpB1;for(0;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpB2=*((struct Cyc_Absyn_VarargInfo*)_check_null((fd->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp6=_tmpB2;struct Cyc_Absyn_VarargInfo _tmpB3=_stmttmp6;int _tmpB7;void*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;struct _fat_ptr*_tmpB4;_LL4: _tmpB4=_tmpB3.name;_tmpB5=_tmpB3.tq;_tmpB6=_tmpB3.type;_tmpB7=_tmpB3.inject;_LL5: {struct _fat_ptr*n=_tmpB4;struct Cyc_Absyn_Tqual tq=_tmpB5;void*t=_tmpB6;int i=_tmpB7;
# 511
struct Cyc_List_List*_tmpB8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpB8;for(0;vds != 0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*_tmpB9=(struct Cyc_Absyn_Vardecl*)vds->hd;struct Cyc_Absyn_Vardecl*vd=_tmpB9;
if(Cyc_strptrcmp((*vd->name).f2,(struct _fat_ptr*)_check_null(n))== 0){
({void*_tmp153=Cyc_Absyn_fatptr_type(t,param_rgn,tq,Cyc_Absyn_false_type);vd->type=_tmp153;});
break;}}}}
# 519
return({struct Cyc_Tcenv_Fenv*_tmpBC=_cycalloc(sizeof(*_tmpBC));
({struct Cyc_Tcenv_SharedFenv*_tmp158=Cyc_Tcenv_new_shared_fenv((fd->i).ret_type);_tmpBC->shared=_tmp158;}),_tmpBC->type_vars=tvs,_tmpBC->region_order=rgn_po,_tmpBC->ctrl_env=0,({
# 524
void*_tmp157=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));({void*_tmp156=Cyc_Absyn_access_eff(param_rgn);_tmpBB->hd=_tmp156;}),({
struct Cyc_List_List*_tmp155=({struct Cyc_List_List*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->hd=cap,_tmpBA->tl=0;_tmpBA;});_tmpBB->tl=_tmp155;});_tmpBB;}));
# 524
_tmpBC->capability=_tmp157;}),_tmpBC->curr_rgn=param_rgn,({
# 527
void*_tmp154=Cyc_CurRgn_curr_rgn_type();_tmpBC->curr_lifo_rgn=_tmp154;}),
(_tmpBC->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpBC->flags).in_notreadctxt=0,(_tmpBC->flags).in_lhs=0,(_tmpBC->flags).abstract_ok=0,(_tmpBC->flags).in_stmt_exp=0;_tmpBC;});}}}
# 532
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmpC4=old_fenv;struct Cyc_Tcenv_SharedFenv*_tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_RgnOrder_RgnPO*_tmpC5;_LL1: _tmpC5=_tmpC4->region_order;_tmpC6=_tmpC4->type_vars;_tmpC7=_tmpC4->shared;_LL2: {struct Cyc_RgnOrder_RgnPO*rgn_po=_tmpC5;struct Cyc_List_List*type_vars=_tmpC6;struct Cyc_Tcenv_SharedFenv*shared=_tmpC7;
# 535
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpE4=_cycalloc(sizeof(*_tmpE4));
({struct _fat_ptr*_tmp15C=({struct _fat_ptr*_tmpE3=_cycalloc(sizeof(*_tmpE3));({struct _fat_ptr _tmp15B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE2=({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0U,_tmp104.f1=(struct _fat_ptr)((struct _fat_ptr)*(*fd->name).f2);_tmp104;});void*_tmpE0[1U];_tmpE0[0]=& _tmpE2;({struct _fat_ptr _tmp15A=({const char*_tmpE1="`%s";_tag_fat(_tmpE1,sizeof(char),4U);});Cyc_aprintf(_tmp15A,_tag_fat(_tmpE0,sizeof(void*),1U));});});*_tmpE3=_tmp15B;});_tmpE3;});_tmpE4->name=_tmp15C;}),({
int _tmp159=Cyc_Tcutil_new_tvar_id();_tmpE4->identity=_tmp159;}),_tmpE4->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpE4;});
{struct Cyc_List_List*_tmpC8=(fd->i).tvars;struct Cyc_List_List*tvars=_tmpC8;for(0;tvars != 0;tvars=tvars->tl){
struct Cyc_Absyn_Kind*_tmpC9=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp7=_tmpC9;struct Cyc_Absyn_Kind*_tmpCA=_stmttmp7;enum Cyc_Absyn_AliasQual _tmpCC;enum Cyc_Absyn_KindQual _tmpCB;_LL4: _tmpCB=_tmpCA->kind;_tmpCC=_tmpCA->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k=_tmpCB;enum Cyc_Absyn_AliasQual a=_tmpCC;
if((int)k == (int)3U){
if((int)a == (int)0U)
rgn_po=Cyc_RgnOrder_add_unordered(rgn_po,(struct Cyc_Absyn_Tvar*)tvars->hd);else{
# 544
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp103;_tmp103.tag=0U,({struct _fat_ptr _tmp15D=({const char*_tmpCF="non-intuitionistic tvar in nested_fenv";_tag_fat(_tmpCF,sizeof(char),39U);});_tmp103.f1=_tmp15D;});_tmp103;});void*_tmpCD[1U];_tmpCD[0]=& _tmpCE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpCD,sizeof(void*),1U));});}}}}}
# 546
rgn_po=Cyc_RgnOrder_add_youngest(rgn_po,rgn0,0);
{struct Cyc_List_List*_tmpD0=(fd->i).rgn_po;struct Cyc_List_List*po2=_tmpD0;for(0;po2 != 0;po2=po2->tl){
rgn_po=Cyc_RgnOrder_add_outlives_constraint(rgn_po,(*((struct _tuple15*)po2->hd)).f1,(*((struct _tuple15*)po2->hd)).f2,loc);}}{
struct Cyc_List_List*_tmpD1=({struct Cyc_List_List*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->hd=rgn0,({struct Cyc_List_List*_tmp15E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).tvars,type_vars);_tmpDF->tl=_tmp15E;});_tmpDF;});struct Cyc_List_List*tvs=_tmpD1;
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=2U,_tmpDE->f1=rgn0;_tmpDE;});
{struct Cyc_List_List*_tmpD2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpD2;for(0;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpD3=*((struct Cyc_Absyn_VarargInfo*)_check_null((fd->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _stmttmp8=_tmpD3;struct Cyc_Absyn_VarargInfo _tmpD4=_stmttmp8;int _tmpD8;void*_tmpD7;struct Cyc_Absyn_Tqual _tmpD6;struct _fat_ptr*_tmpD5;_LL7: _tmpD5=_tmpD4.name;_tmpD6=_tmpD4.tq;_tmpD7=_tmpD4.type;_tmpD8=_tmpD4.inject;_LL8: {struct _fat_ptr*n=_tmpD5;struct Cyc_Absyn_Tqual tq=_tmpD6;void*t=_tmpD7;int i=_tmpD8;
# 557
struct Cyc_List_List*_tmpD9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;struct Cyc_List_List*vds=_tmpD9;for(0;vds != 0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*_tmpDA=(struct Cyc_Absyn_Vardecl*)vds->hd;struct Cyc_Absyn_Vardecl*vd=_tmpDA;
if(Cyc_strptrcmp((*vd->name).f2,(struct _fat_ptr*)_check_null(n))== 0){
({void*_tmp15F=Cyc_Absyn_fatptr_type(t,param_rgn,tq,Cyc_Absyn_false_type);vd->type=_tmp15F;});
break;}}}}
# 565
return({struct Cyc_Tcenv_Fenv*_tmpDD=_cycalloc(sizeof(*_tmpDD));
({struct Cyc_Tcenv_SharedFenv*_tmp164=Cyc_Tcenv_new_shared_fenv((fd->i).ret_type);_tmpDD->shared=_tmp164;}),_tmpDD->type_vars=tvs,_tmpDD->region_order=rgn_po,_tmpDD->ctrl_env=0,({
# 570
void*_tmp163=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));({void*_tmp162=Cyc_Absyn_access_eff(param_rgn);_tmpDC->hd=_tmp162;}),({
struct Cyc_List_List*_tmp161=({struct Cyc_List_List*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->hd=(void*)_check_null((fd->i).effect),_tmpDB->tl=0;_tmpDB;});_tmpDC->tl=_tmp161;});_tmpDC;}));
# 570
_tmpDD->capability=_tmp163;}),_tmpDD->curr_rgn=param_rgn,({
# 573
void*_tmp160=Cyc_CurRgn_curr_rgn_type();_tmpDD->curr_lifo_rgn=_tmp160;}),
(_tmpDD->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpDD->flags).in_notreadctxt=0,(_tmpDD->flags).in_lhs=0,(_tmpDD->flags).abstract_ok=0,(_tmpDD->flags).in_stmt_exp=0;_tmpDD;});}}}}
# 578
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args){
# 580
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpED=_cycalloc(sizeof(*_tmpED));({struct _fat_ptr*_tmp167=({struct _fat_ptr*_tmpEC=_cycalloc(sizeof(*_tmpEC));({struct _fat_ptr _tmp166=({const char*_tmpEB="bogus";_tag_fat(_tmpEB,sizeof(char),6U);});*_tmpEC=_tmp166;});_tmpEC;});_tmpED->name=_tmp167;}),({int _tmp165=Cyc_Tcutil_new_tvar_id();_tmpED->identity=_tmp165;}),_tmpED->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpED;});
struct Cyc_List_List*_tmpE5=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=rgn0,_tmpEA->tl=0;_tmpEA;});struct Cyc_List_List*tvs=_tmpE5;
struct Cyc_RgnOrder_RgnPO*_tmpE6=({void*_tmp168=Cyc_Absyn_join_eff(0);Cyc_RgnOrder_initial_fn_po(0,0,_tmp168,rgn0,0U);});struct Cyc_RgnOrder_RgnPO*rgn_po=_tmpE6;
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->tag=2U,_tmpE9->f1=rgn0;_tmpE9;});
return({struct Cyc_Tcenv_Fenv*_tmpE8=_cycalloc(sizeof(*_tmpE8));
({struct Cyc_Tcenv_SharedFenv*_tmp16C=Cyc_Tcenv_new_shared_fenv(ret_type);_tmpE8->shared=_tmp16C;}),_tmpE8->type_vars=tvs,_tmpE8->region_order=rgn_po,_tmpE8->ctrl_env=0,({
# 589
void*_tmp16B=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));({void*_tmp16A=Cyc_Absyn_access_eff(param_rgn);_tmpE7->hd=_tmp16A;}),_tmpE7->tl=0;_tmpE7;}));_tmpE8->capability=_tmp16B;}),_tmpE8->curr_rgn=param_rgn,({
# 591
void*_tmp169=Cyc_CurRgn_curr_rgn_type();_tmpE8->curr_lifo_rgn=_tmp169;}),
(_tmpE8->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpE8->flags).in_notreadctxt=0,(_tmpE8->flags).in_lhs=0,(_tmpE8->flags).abstract_ok=0,(_tmpE8->flags).in_stmt_exp=0;_tmpE8;});}
# 596
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv_dicts(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmpEE=({struct Cyc_Tcenv_Tenv*_tmpF0=_cycalloc(sizeof(*_tmpF0));*_tmpF0=*te;_tmpF0;});struct Cyc_Tcenv_Tenv*ans=_tmpEE;
({struct Cyc_Tcenv_Genv*_tmp16D=({struct Cyc_Tcenv_Genv*_tmpEF=_cycalloc(sizeof(*_tmpEF));*_tmpEF=*te->ae;_tmpEF;});ans->ae=_tmp16D;});
({struct Cyc_Dict_Dict _tmp16E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy)((te->ae)->aggrdecls);(ans->ae)->aggrdecls=_tmp16E;});
({struct Cyc_Dict_Dict _tmp16F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy)((te->ae)->enumdecls);(ans->ae)->enumdecls=_tmp16F;});
({struct Cyc_Dict_Dict _tmp170=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy)((te->ae)->datatypedecls);(ans->ae)->datatypedecls=_tmp170;});
({struct Cyc_Dict_Dict _tmp171=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy)((te->ae)->typedefs);(ans->ae)->typedefs=_tmp171;});
({struct Cyc_Dict_Dict _tmp172=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict))Cyc_Dict_copy)((te->ae)->ordinaries);(ans->ae)->ordinaries=_tmp172;});
ans->le=0;
return ans;}

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
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 290 "cycboot.h"
extern int isdigit(int);struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 136 "string.h"
extern struct _fat_ptr Cyc_implode(struct Cyc_List_List*c);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 415 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 514
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 532
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 895 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 916
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 921
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;
# 923
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;
# 925
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 932
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 973
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term,void*rel);
# 977
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt,void*rel);
# 33 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 238 "tcutil.h"
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 47 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_rko;struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 31 "formatstr.cyc"
static void*Cyc_Formatstr_err_null(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 33
Cyc_Warn_verr(loc,fmt,ap);
return 0;}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;char f5;int f6;};
# 43
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _fat_ptr s,int i){
# 47
unsigned long _tmp0=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp0;
if(i < 0 ||(unsigned long)i >= len)return 0;{
# 51
struct Cyc_List_List*_tmp1=0;struct Cyc_List_List*flags=_tmp1;
char c=' ';
for(0;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmp2=c;switch(_tmp2){case 43U: _LL1: _LL2:
 goto _LL4;case 45U: _LL3: _LL4:
 goto _LL6;case 32U: _LL5: _LL6:
 goto _LL8;case 35U: _LL7: _LL8:
 goto _LLA;case 48U: _LL9: _LLA:
 flags=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=(void*)((int)c),_tmp3->tl=flags;_tmp3;});continue;default: _LLB: _LLC:
 goto _LL0;}_LL0:;}
# 63
break;}
# 65
if((unsigned long)i >= len)return 0;
flags=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(flags);{
# 69
struct Cyc_List_List*_tmp4=0;struct Cyc_List_List*width=_tmp4;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'*'){
width=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=(void*)((int)c),_tmp5->tl=width;_tmp5;});
++ i;}else{
# 75
for(0;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))width=({struct Cyc_List_List*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->hd=(void*)((int)c),_tmp6->tl=width;_tmp6;});else{
break;}}}
# 81
if((unsigned long)i >= len)return 0;
width=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(width);{
# 85
struct Cyc_List_List*_tmp7=0;struct Cyc_List_List*precision=_tmp7;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'.'){
precision=({struct Cyc_List_List*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->hd=(void*)((int)c),_tmp8->tl=precision;_tmp8;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'*'){
precision=({struct Cyc_List_List*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->hd=(void*)((int)c),_tmp9->tl=precision;_tmp9;});
++ i;}else{
# 95
for(0;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))precision=({struct Cyc_List_List*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->hd=(void*)((int)c),_tmpA->tl=precision;_tmpA;});else{
break;}}}}
# 101
if((unsigned long)i >= len)return 0;
precision=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(precision);{
# 106
struct Cyc_List_List*_tmpB=0;struct Cyc_List_List*lenmod=_tmpB;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmpC=c;switch(_tmpC){case 104U: _LLE: _LLF:
# 110
 lenmod=({struct Cyc_List_List*_tmpD=_region_malloc(r,sizeof(*_tmpD));_tmpD->hd=(void*)((int)c),_tmpD->tl=lenmod;_tmpD;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'h'){lenmod=({struct Cyc_List_List*_tmpE=_region_malloc(r,sizeof(*_tmpE));_tmpE->hd=(void*)((int)c),_tmpE->tl=lenmod;_tmpE;});++ i;}
goto _LLD;case 108U: _LL10: _LL11:
# 116
 lenmod=({struct Cyc_List_List*_tmpF=_region_malloc(r,sizeof(*_tmpF));_tmpF->hd=(void*)((int)c),_tmpF->tl=lenmod;_tmpF;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'l'){lenmod=({struct Cyc_List_List*_tmp10=_region_malloc(r,sizeof(*_tmp10));_tmp10->hd=(void*)((int)c),_tmp10->tl=lenmod;_tmp10;});++ i;}
goto _LLD;case 106U: _LL12: _LL13:
 goto _LL15;case 122U: _LL14: _LL15:
 goto _LL17;case 116U: _LL16: _LL17:
 goto _LL19;case 76U: _LL18: _LL19:
# 125
 lenmod=({struct Cyc_List_List*_tmp11=_region_malloc(r,sizeof(*_tmp11));_tmp11->hd=(void*)((int)c),_tmp11->tl=lenmod;_tmp11;});
++ i;
goto _LLD;default: _LL1A: _LL1B:
 goto _LLD;}_LLD:;}
# 130
if((unsigned long)i >= len)return 0;
lenmod=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(lenmod);
# 134
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmp12=c;switch(_tmp12){case 100U: _LL1D: _LL1E:
 goto _LL20;case 105U: _LL1F: _LL20:
 goto _LL22;case 111U: _LL21: _LL22:
 goto _LL24;case 117U: _LL23: _LL24:
 goto _LL26;case 120U: _LL25: _LL26:
 goto _LL28;case 88U: _LL27: _LL28:
 goto _LL2A;case 102U: _LL29: _LL2A:
 goto _LL2C;case 70U: _LL2B: _LL2C:
 goto _LL2E;case 101U: _LL2D: _LL2E:
 goto _LL30;case 69U: _LL2F: _LL30:
 goto _LL32;case 103U: _LL31: _LL32:
 goto _LL34;case 71U: _LL33: _LL34:
 goto _LL36;case 97U: _LL35: _LL36:
 goto _LL38;case 65U: _LL37: _LL38:
 goto _LL3A;case 99U: _LL39: _LL3A:
 goto _LL3C;case 115U: _LL3B: _LL3C:
 goto _LL3E;case 112U: _LL3D: _LL3E:
 goto _LL40;case 110U: _LL3F: _LL40:
 goto _LL42;case 37U: _LL41: _LL42:
 goto _LL1C;default: _LL43: _LL44:
 return 0;}_LL1C:;}
# 166 "formatstr.cyc"
return({struct Cyc_Core_Opt*_tmp14=_region_malloc(r,sizeof(*_tmp14));({struct _tuple11*_tmp123=({struct _tuple11*_tmp13=_region_malloc(r,sizeof(*_tmp13));_tmp13->f1=flags,_tmp13->f2=width,_tmp13->f3=precision,_tmp13->f4=lenmod,_tmp13->f5=c,_tmp13->f6=i + 1;_tmp13;});_tmp14->v=_tmp123;});_tmp14;});}}}}}
# 169
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){
# 172
unsigned long _tmp15=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp15;
struct Cyc_List_List*_tmp16=0;struct Cyc_List_List*typs=_tmp16;
int i;
struct _RegionHandle _tmp17=_new_region("temp");struct _RegionHandle*temp=& _tmp17;_push_region(temp);
for(i=0;(unsigned long)i < len;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'%')continue;{
struct Cyc_Core_Opt*_tmp18=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 1);struct Cyc_Core_Opt*cs=_tmp18;
if(cs == 0){
struct Cyc_List_List*_tmp1B=({void*_tmp19=0U;({unsigned _tmp125=loc;struct _fat_ptr _tmp124=({const char*_tmp1A="bad format string";_tag_fat(_tmp1A,sizeof(char),18U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp125,_tmp124,_tag_fat(_tmp19,sizeof(void*),0U));});});_npop_handler(0U);return _tmp1B;}{
struct _tuple11*_tmp1C=(struct _tuple11*)cs->v;struct _tuple11*_stmttmp0=_tmp1C;struct _tuple11*_tmp1D=_stmttmp0;int _tmp23;char _tmp22;struct Cyc_List_List*_tmp21;struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp1F;struct Cyc_List_List*_tmp1E;_LL1: _tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;_tmp20=_tmp1D->f3;_tmp21=_tmp1D->f4;_tmp22=_tmp1D->f5;_tmp23=_tmp1D->f6;_LL2: {struct Cyc_List_List*flags=_tmp1E;struct Cyc_List_List*width=_tmp1F;struct Cyc_List_List*precision=_tmp20;struct Cyc_List_List*lenmod=_tmp21;char c=_tmp22;int j=_tmp23;
i=j - 1;
{struct Cyc_List_List*_tmp24=lenmod;int _tmp25;if(_tmp24 != 0){if(((struct Cyc_List_List*)_tmp24)->tl == 0){_LL4: _tmp25=(int)_tmp24->hd;if(
(_tmp25 == (int)'j' || _tmp25 == (int)'z')|| _tmp25 == (int)'t'){_LL5: {int x=_tmp25;
# 187
struct Cyc_List_List*_tmp29=({struct Cyc_Int_pa_PrintArg_struct _tmp28=({struct Cyc_Int_pa_PrintArg_struct _tmp104;_tmp104.tag=1U,_tmp104.f1=(unsigned long)x;_tmp104;});void*_tmp26[1U];_tmp26[0]=& _tmp28;({unsigned _tmp127=loc;struct _fat_ptr _tmp126=({const char*_tmp27="length modifier '%c' is not supported";_tag_fat(_tmp27,sizeof(char),38U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp127,_tmp126,_tag_fat(_tmp26,sizeof(void*),1U));});});_npop_handler(0U);return _tmp29;}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 190
{struct Cyc_List_List*_tmp2A=width;int _tmp2B;if(_tmp2A != 0){if(((struct Cyc_List_List*)_tmp2A)->tl == 0){_LL9: _tmp2B=(int)_tmp2A->hd;if(_tmp2B == (int)'*'){_LLA: {int x=_tmp2B;
typs=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=Cyc_Absyn_sint_type,_tmp2C->tl=typs;_tmp2C;});goto _LL8;}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _LLC:
 goto _LL8;}_LL8:;}
# 194
{struct Cyc_List_List*_tmp2D=precision;int _tmp2F;int _tmp2E;if(_tmp2D != 0){if(((struct Cyc_List_List*)_tmp2D)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp2D)->tl)->tl == 0){_LLE: _tmp2E=(int)_tmp2D->hd;_tmp2F=(int)(_tmp2D->tl)->hd;if(
_tmp2E == (int)'.' && _tmp2F == (int)'*'){_LLF: {int x=_tmp2E;int y=_tmp2F;
typs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=Cyc_Absyn_sint_type,_tmp30->tl=typs;_tmp30;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}{
# 199
void*t;
char _tmp31=c;switch(_tmp31){case 100U: _LL13: _LL14:
 goto _LL16;case 105U: _LL15: _LL16:
# 203
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#'){
struct Cyc_List_List*_tmp35=({struct Cyc_Int_pa_PrintArg_struct _tmp34=({struct Cyc_Int_pa_PrintArg_struct _tmp105;_tmp105.tag=1U,_tmp105.f1=(unsigned long)((int)c);_tmp105;});void*_tmp32[1U];_tmp32[0]=& _tmp34;({unsigned _tmp129=loc;struct _fat_ptr _tmp128=({const char*_tmp33="flag '#' is not valid with %%%c";_tag_fat(_tmp33,sizeof(char),32U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp129,_tmp128,_tag_fat(_tmp32,sizeof(void*),1U));});});_npop_handler(0U);return _tmp35;}}}
{struct Cyc_List_List*_tmp36=lenmod;int _tmp38;int _tmp37;int _tmp39;int _tmp3A;if(_tmp36 == 0){_LL3C: _LL3D:
 t=Cyc_Absyn_sint_type;goto _LL3B;}else{if(((struct Cyc_List_List*)_tmp36)->tl == 0){_LL3E: _tmp3A=(int)_tmp36->hd;if(_tmp3A == (int)'l'){_LL3F: {int x=_tmp3A;
t=Cyc_Absyn_slong_type;goto _LL3B;}}else{_LL40: _tmp39=(int)_tmp36->hd;if(_tmp39 == (int)'h'){_LL41: {int x=_tmp39;
t=Cyc_Absyn_sshort_type;goto _LL3B;}}else{goto _LL44;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp36)->tl)->tl == 0){_LL42: _tmp37=(int)_tmp36->hd;_tmp38=(int)(_tmp36->tl)->hd;if(
_tmp37 == (int)'h' && _tmp38 == (int)'h'){_LL43: {int x=_tmp37;int y=_tmp38;
t=Cyc_Absyn_schar_type;goto _LL3B;}}else{goto _LL44;}}else{_LL44: _LL45: {
# 213
struct Cyc_List_List*_tmp3F=({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0U,({
struct _fat_ptr _tmp12A=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp107.f1=_tmp12A;});_tmp107;});struct Cyc_Int_pa_PrintArg_struct _tmp3E=({struct Cyc_Int_pa_PrintArg_struct _tmp106;_tmp106.tag=1U,_tmp106.f1=(unsigned long)((int)c);_tmp106;});void*_tmp3B[2U];_tmp3B[0]=& _tmp3D,_tmp3B[1]=& _tmp3E;({unsigned _tmp12C=loc;struct _fat_ptr _tmp12B=({const char*_tmp3C="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp3C,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp12C,_tmp12B,_tag_fat(_tmp3B,sizeof(void*),2U));});});_npop_handler(0U);return _tmp3F;}}}}_LL3B:;}
# 216
typs=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->hd=t,_tmp40->tl=typs;_tmp40;});
goto _LL12;case 117U: _LL17: _LL18:
# 219
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#'){
struct Cyc_List_List*_tmp43=({void*_tmp41=0U;({unsigned _tmp12E=loc;struct _fat_ptr _tmp12D=({const char*_tmp42="Flag '#' not valid with %%u";_tag_fat(_tmp42,sizeof(char),28U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp12E,_tmp12D,_tag_fat(_tmp41,sizeof(void*),0U));});});_npop_handler(0U);return _tmp43;}}}
goto _LL1A;case 111U: _LL19: _LL1A:
 goto _LL1C;case 120U: _LL1B: _LL1C:
 goto _LL1E;case 88U: _LL1D: _LL1E:
# 226
{struct Cyc_List_List*_tmp44=lenmod;int _tmp46;int _tmp45;int _tmp47;int _tmp48;if(_tmp44 == 0){_LL47: _LL48:
 t=Cyc_Absyn_uint_type;goto _LL46;}else{if(((struct Cyc_List_List*)_tmp44)->tl == 0){_LL49: _tmp48=(int)_tmp44->hd;if(_tmp48 == (int)'l'){_LL4A: {int x=_tmp48;
t=Cyc_Absyn_ulong_type;goto _LL46;}}else{_LL4B: _tmp47=(int)_tmp44->hd;if(_tmp47 == (int)'h'){_LL4C: {int x=_tmp47;
t=Cyc_Absyn_ushort_type;goto _LL46;}}else{goto _LL4F;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp44)->tl)->tl == 0){_LL4D: _tmp45=(int)_tmp44->hd;_tmp46=(int)(_tmp44->tl)->hd;if(
_tmp45 == (int)'h' && _tmp46 == (int)'h'){_LL4E: {int x=_tmp45;int y=_tmp46;
t=Cyc_Absyn_uchar_type;goto _LL46;}}else{goto _LL4F;}}else{_LL4F: _LL50: {
# 234
struct Cyc_List_List*_tmp4D=({struct Cyc_String_pa_PrintArg_struct _tmp4B=({struct Cyc_String_pa_PrintArg_struct _tmp109;_tmp109.tag=0U,({
struct _fat_ptr _tmp12F=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp109.f1=_tmp12F;});_tmp109;});struct Cyc_Int_pa_PrintArg_struct _tmp4C=({struct Cyc_Int_pa_PrintArg_struct _tmp108;_tmp108.tag=1U,_tmp108.f1=(unsigned long)((int)c);_tmp108;});void*_tmp49[2U];_tmp49[0]=& _tmp4B,_tmp49[1]=& _tmp4C;({unsigned _tmp131=loc;struct _fat_ptr _tmp130=({const char*_tmp4A="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp4A,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp131,_tmp130,_tag_fat(_tmp49,sizeof(void*),2U));});});_npop_handler(0U);return _tmp4D;}}}}_LL46:;}
# 237
typs=({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=t,_tmp4E->tl=typs;_tmp4E;});
goto _LL12;case 102U: _LL1F: _LL20:
 goto _LL22;case 70U: _LL21: _LL22:
 goto _LL24;case 101U: _LL23: _LL24:
 goto _LL26;case 69U: _LL25: _LL26:
 goto _LL28;case 103U: _LL27: _LL28:
 goto _LL2A;case 71U: _LL29: _LL2A:
 goto _LL2C;case 97U: _LL2B: _LL2C:
 goto _LL2E;case 65U: _LL2D: _LL2E:
# 253
{struct Cyc_List_List*_tmp4F=lenmod;int _tmp50;if(_tmp4F == 0){_LL52: _LL53:
# 255
 typs=({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->hd=Cyc_Absyn_double_type,_tmp51->tl=typs;_tmp51;});goto _LL51;}else{if(((struct Cyc_List_List*)_tmp4F)->tl == 0){_LL54: _tmp50=(int)_tmp4F->hd;if(_tmp50 == (int)'l'){_LL55: {int x=_tmp50;
# 257
typs=({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->hd=Cyc_Absyn_long_double_type,_tmp52->tl=typs;_tmp52;});goto _LL51;}}else{goto _LL56;}}else{_LL56: _LL57: {
# 259
struct Cyc_List_List*_tmp57=({struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp10B;_tmp10B.tag=0U,({
struct _fat_ptr _tmp132=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp10B.f1=_tmp132;});_tmp10B;});struct Cyc_Int_pa_PrintArg_struct _tmp56=({struct Cyc_Int_pa_PrintArg_struct _tmp10A;_tmp10A.tag=1U,_tmp10A.f1=(unsigned long)((int)c);_tmp10A;});void*_tmp53[2U];_tmp53[0]=& _tmp55,_tmp53[1]=& _tmp56;({unsigned _tmp134=loc;struct _fat_ptr _tmp133=({const char*_tmp54="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp54,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp134,_tmp133,_tag_fat(_tmp53,sizeof(void*),2U));});});_npop_handler(0U);return _tmp57;}}}_LL51:;}
# 262
goto _LL12;case 99U: _LL2F: _LL30:
# 264
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#' ||(int)f->hd == (int)'0'){
struct Cyc_List_List*_tmp5B=({struct Cyc_Int_pa_PrintArg_struct _tmp5A=({struct Cyc_Int_pa_PrintArg_struct _tmp10C;_tmp10C.tag=1U,_tmp10C.f1=(unsigned long)((int)f->hd);_tmp10C;});void*_tmp58[1U];_tmp58[0]=& _tmp5A;({unsigned _tmp136=loc;struct _fat_ptr _tmp135=({const char*_tmp59="flag '%c' not allowed with %%c";_tag_fat(_tmp59,sizeof(char),31U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp136,_tmp135,_tag_fat(_tmp58,sizeof(void*),1U));});});_npop_handler(0U);return _tmp5B;}}}
# 269
if(lenmod != 0){
struct Cyc_List_List*_tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp5E=({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0U,({
struct _fat_ptr _tmp137=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp10D.f1=_tmp137;});_tmp10D;});void*_tmp5C[1U];_tmp5C[0]=& _tmp5E;({unsigned _tmp139=loc;struct _fat_ptr _tmp138=({const char*_tmp5D="length modifier '%s' not allowed with %%c";_tag_fat(_tmp5D,sizeof(char),42U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp139,_tmp138,_tag_fat(_tmp5C,sizeof(void*),1U));});});_npop_handler(0U);return _tmp5F;}
if(precision != 0){
struct Cyc_List_List*_tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0U,({
struct _fat_ptr _tmp13A=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(precision));_tmp10E.f1=_tmp13A;});_tmp10E;});void*_tmp60[1U];_tmp60[0]=& _tmp62;({unsigned _tmp13C=loc;struct _fat_ptr _tmp13B=({const char*_tmp61="precision '%s' not allowed with %%c";_tag_fat(_tmp61,sizeof(char),36U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp13C,_tmp13B,_tag_fat(_tmp60,sizeof(void*),1U));});});_npop_handler(0U);return _tmp63;}
typs=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=Cyc_Absyn_sint_type,_tmp64->tl=typs;_tmp64;});
goto _LL12;case 115U: _LL31: _LL32:
# 279
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd != (int)'-'){
struct Cyc_List_List*_tmp67=({void*_tmp65=0U;({unsigned _tmp13E=loc;struct _fat_ptr _tmp13D=({const char*_tmp66="a flag not allowed with %%s";_tag_fat(_tmp66,sizeof(char),28U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp13E,_tmp13D,_tag_fat(_tmp65,sizeof(void*),0U));});});_npop_handler(0U);return _tmp67;}}}
# 284
if(lenmod != 0){
struct Cyc_List_List*_tmp6A=({void*_tmp68=0U;({unsigned _tmp140=loc;struct _fat_ptr _tmp13F=({const char*_tmp69="length modifiers not allowed with %%s";_tag_fat(_tmp69,sizeof(char),38U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp140,_tmp13F,_tag_fat(_tmp68,sizeof(void*),0U));});});_npop_handler(0U);return _tmp6A;}{
# 288
void*ptr;
struct Cyc_List_List*_tmp6B=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmp6B;
if(!isCproto)
ptr=({void*_tmp144=Cyc_Absyn_char_type;void*_tmp143=
Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->v=tvs;_tmp6C;}));
# 291
struct Cyc_Absyn_Tqual _tmp142=
# 294
Cyc_Absyn_const_tqual(0U);
# 291
void*_tmp141=Cyc_Absyn_false_type;Cyc_Absyn_fatptr_type(_tmp144,_tmp143,_tmp142,_tmp141,
# 294
Cyc_Tcutil_any_bool(tvs));});else{
# 296
ptr=({void*_tmp148=Cyc_Absyn_char_type;void*_tmp147=
Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->v=tvs;_tmp6D;}));
# 296
struct Cyc_Absyn_Tqual _tmp146=
# 299
Cyc_Absyn_const_tqual(0U);
# 296
void*_tmp145=Cyc_Absyn_true_type;Cyc_Absyn_at_type(_tmp148,_tmp147,_tmp146,_tmp145,
# 299
Cyc_Tcutil_any_bool(tvs));});}
typs=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=ptr,_tmp6E->tl=typs;_tmp6E;});
goto _LL12;}case 112U: _LL33: _LL34:
# 304
 typs=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->hd=Cyc_Absyn_uint_type,_tmp6F->tl=typs;_tmp6F;});
goto _LL12;case 110U: _LL35: _LL36:
# 307
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#' ||(int)f->hd == (int)'0'){
struct Cyc_List_List*_tmp73=({struct Cyc_Int_pa_PrintArg_struct _tmp72=({struct Cyc_Int_pa_PrintArg_struct _tmp10F;_tmp10F.tag=1U,_tmp10F.f1=(unsigned long)((int)f->hd);_tmp10F;});void*_tmp70[1U];_tmp70[0]=& _tmp72;({unsigned _tmp14A=loc;struct _fat_ptr _tmp149=({const char*_tmp71="flag '%c' not allowed with %%n";_tag_fat(_tmp71,sizeof(char),31U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp14A,_tmp149,_tag_fat(_tmp70,sizeof(void*),1U));});});_npop_handler(0U);return _tmp73;}}}
if(precision != 0){
struct Cyc_List_List*_tmp77=({struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp110;_tmp110.tag=0U,({
struct _fat_ptr _tmp14B=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(precision));_tmp110.f1=_tmp14B;});_tmp110;});void*_tmp74[1U];_tmp74[0]=& _tmp76;({unsigned _tmp14D=loc;struct _fat_ptr _tmp14C=({const char*_tmp75="precision '%s' not allowed with %%n";_tag_fat(_tmp75,sizeof(char),36U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp14D,_tmp14C,_tag_fat(_tmp74,sizeof(void*),1U));});});_npop_handler(0U);return _tmp77;}{
struct Cyc_List_List*_tmp78=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmp78;
{struct Cyc_List_List*_tmp79=lenmod;int _tmp7B;int _tmp7A;int _tmp7C;int _tmp7D;if(_tmp79 == 0){_LL59: _LL5A:
 t=Cyc_Absyn_sint_type;goto _LL58;}else{if(((struct Cyc_List_List*)_tmp79)->tl == 0){_LL5B: _tmp7D=(int)_tmp79->hd;if(_tmp7D == (int)'l'){_LL5C: {int x=_tmp7D;
# 317
t=Cyc_Absyn_ulong_type;goto _LL58;}}else{_LL5D: _tmp7C=(int)_tmp79->hd;if(_tmp7C == (int)'h'){_LL5E: {int x=_tmp7C;
t=Cyc_Absyn_sshort_type;goto _LL58;}}else{goto _LL61;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp79)->tl)->tl == 0){_LL5F: _tmp7A=(int)_tmp79->hd;_tmp7B=(int)(_tmp79->tl)->hd;if(
_tmp7A == (int)'h' && _tmp7B == (int)'h'){_LL60: {int x=_tmp7A;int y=_tmp7B;
t=Cyc_Absyn_schar_type;goto _LL58;}}else{goto _LL61;}}else{_LL61: _LL62: {
# 322
struct Cyc_List_List*_tmp82=({struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0U,({
struct _fat_ptr _tmp14E=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp112.f1=_tmp14E;});_tmp112;});struct Cyc_Int_pa_PrintArg_struct _tmp81=({struct Cyc_Int_pa_PrintArg_struct _tmp111;_tmp111.tag=1U,_tmp111.f1=(unsigned long)((int)c);_tmp111;});void*_tmp7E[2U];_tmp7E[0]=& _tmp80,_tmp7E[1]=& _tmp81;({unsigned _tmp150=loc;struct _fat_ptr _tmp14F=({const char*_tmp7F="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp7F,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp150,_tmp14F,_tag_fat(_tmp7E,sizeof(void*),2U));});});_npop_handler(0U);return _tmp82;}}}}_LL58:;}
# 325
t=({void*_tmp154=t;void*_tmp153=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->v=tvs;_tmp83;}));struct Cyc_Absyn_Tqual _tmp152=Cyc_Absyn_empty_tqual(0U);void*_tmp151=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp154,_tmp153,_tmp152,_tmp151,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->hd=t,_tmp84->tl=typs;_tmp84;});
goto _LL12;}case 37U: _LL37: _LL38:
# 330
 if(flags != 0){
struct Cyc_List_List*_tmp88=({struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0U,({
struct _fat_ptr _tmp155=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(flags));_tmp113.f1=_tmp155;});_tmp113;});void*_tmp85[1U];_tmp85[0]=& _tmp87;({unsigned _tmp157=loc;struct _fat_ptr _tmp156=({const char*_tmp86="flags '%s' not allowed with %%%%";_tag_fat(_tmp86,sizeof(char),33U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp157,_tmp156,_tag_fat(_tmp85,sizeof(void*),1U));});});_npop_handler(0U);return _tmp88;}
if(width != 0){
struct Cyc_List_List*_tmp8C=({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp114;_tmp114.tag=0U,({
struct _fat_ptr _tmp158=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(width));_tmp114.f1=_tmp158;});_tmp114;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({unsigned _tmp15A=loc;struct _fat_ptr _tmp159=({const char*_tmp8A="width '%s' not allowed with %%%%";_tag_fat(_tmp8A,sizeof(char),33U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp15A,_tmp159,_tag_fat(_tmp89,sizeof(void*),1U));});});_npop_handler(0U);return _tmp8C;}
if(precision != 0){
struct Cyc_List_List*_tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp8F=({struct Cyc_String_pa_PrintArg_struct _tmp115;_tmp115.tag=0U,({
struct _fat_ptr _tmp15B=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(precision));_tmp115.f1=_tmp15B;});_tmp115;});void*_tmp8D[1U];_tmp8D[0]=& _tmp8F;({unsigned _tmp15D=loc;struct _fat_ptr _tmp15C=({const char*_tmp8E="precision '%s' not allowed with %%%%";_tag_fat(_tmp8E,sizeof(char),37U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp15D,_tmp15C,_tag_fat(_tmp8D,sizeof(void*),1U));});});_npop_handler(0U);return _tmp90;}
if(lenmod != 0){
struct Cyc_List_List*_tmp94=({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0U,({
struct _fat_ptr _tmp15E=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp116.f1=_tmp15E;});_tmp116;});void*_tmp91[1U];_tmp91[0]=& _tmp93;({unsigned _tmp160=loc;struct _fat_ptr _tmp15F=({const char*_tmp92="length modifier '%s' not allowed with %%%%";_tag_fat(_tmp92,sizeof(char),43U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp160,_tmp15F,_tag_fat(_tmp91,sizeof(void*),1U));});});_npop_handler(0U);return _tmp94;}
goto _LL12;default: _LL39: _LL3A: {
struct Cyc_List_List*_tmp95=0;_npop_handler(0U);return _tmp95;}}_LL12:;}}}}}{
# 346
struct Cyc_List_List*_tmp96=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(typs);_npop_handler(0U);return _tmp96;}
# 176
;_pop_region();}struct _tuple12{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};
# 357 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _fat_ptr s,int i){
# 359
unsigned long _tmp97=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp97;
if(i < 0 ||(unsigned long)i >= len)return 0;{
# 362
int _tmp98=0;int suppress=_tmp98;
char _tmp99=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmp99;
if((int)c == (int)'*'){
suppress=1;
++ i;
if((unsigned long)i >= len)return 0;}{
# 370
struct Cyc_List_List*_tmp9A=0;struct Cyc_List_List*width=_tmp9A;
for(0;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))width=({struct Cyc_List_List*_tmp9B=_region_malloc(r,sizeof(*_tmp9B));_tmp9B->hd=(void*)((int)c),_tmp9B->tl=width;_tmp9B;});else{
break;}}
# 376
if((unsigned long)i >= len)return 0;
width=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(width);{
# 381
struct Cyc_List_List*_tmp9C=0;struct Cyc_List_List*lenmod=_tmp9C;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmp9D=c;switch(_tmp9D){case 104U: _LL1: _LL2:
# 385
 lenmod=({struct Cyc_List_List*_tmp9E=_region_malloc(r,sizeof(*_tmp9E));_tmp9E->hd=(void*)((int)c),_tmp9E->tl=lenmod;_tmp9E;});
++ i;
if((unsigned long)i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'h'){lenmod=({struct Cyc_List_List*_tmp9F=_region_malloc(r,sizeof(*_tmp9F));_tmp9F->hd=(void*)((int)c),_tmp9F->tl=lenmod;_tmp9F;});++ i;}
goto _LL0;case 108U: _LL3: _LL4:
# 392
 lenmod=({struct Cyc_List_List*_tmpA0=_region_malloc(r,sizeof(*_tmpA0));_tmpA0->hd=(void*)((int)c),_tmpA0->tl=lenmod;_tmpA0;});
++ i;
if((unsigned long)i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'l'){lenmod=({struct Cyc_List_List*_tmpA1=_region_malloc(r,sizeof(*_tmpA1));_tmpA1->hd=(void*)((int)c),_tmpA1->tl=lenmod;_tmpA1;});++ i;}
goto _LL0;case 106U: _LL5: _LL6:
 goto _LL8;case 122U: _LL7: _LL8:
 goto _LLA;case 116U: _LL9: _LLA:
 goto _LLC;case 76U: _LLB: _LLC:
# 402
 lenmod=({struct Cyc_List_List*_tmpA2=_region_malloc(r,sizeof(*_tmpA2));_tmpA2->hd=(void*)((int)c),_tmpA2->tl=lenmod;_tmpA2;});
++ i;
goto _LL0;default: _LLD: _LLE:
 goto _LL0;}_LL0:;}
# 407
if((unsigned long)i >= len)return 0;
lenmod=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(lenmod);
# 411
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmpA3=c;switch(_tmpA3){case 100U: _LL10: _LL11:
 goto _LL13;case 105U: _LL12: _LL13:
 goto _LL15;case 111U: _LL14: _LL15:
 goto _LL17;case 117U: _LL16: _LL17:
 goto _LL19;case 120U: _LL18: _LL19:
 goto _LL1B;case 88U: _LL1A: _LL1B:
 goto _LL1D;case 102U: _LL1C: _LL1D:
 goto _LL1F;case 70U: _LL1E: _LL1F:
 goto _LL21;case 101U: _LL20: _LL21:
 goto _LL23;case 69U: _LL22: _LL23:
 goto _LL25;case 103U: _LL24: _LL25:
 goto _LL27;case 71U: _LL26: _LL27:
 goto _LL29;case 97U: _LL28: _LL29:
 goto _LL2B;case 65U: _LL2A: _LL2B:
 goto _LL2D;case 99U: _LL2C: _LL2D:
 goto _LL2F;case 115U: _LL2E: _LL2F:
 goto _LL31;case 112U: _LL30: _LL31:
 goto _LL33;case 110U: _LL32: _LL33:
 goto _LL35;case 37U: _LL34: _LL35:
 goto _LLF;default: _LL36: _LL37:
 return 0;}_LLF:;}
# 434
return({struct Cyc_Core_Opt*_tmpA5=_region_malloc(r,sizeof(*_tmpA5));({struct _tuple12*_tmp161=({struct _tuple12*_tmpA4=_region_malloc(r,sizeof(*_tmpA4));_tmpA4->f1=suppress,_tmpA4->f2=width,_tmpA4->f3=lenmod,_tmpA4->f4=c,_tmpA4->f5=i + 1;_tmpA4;});_tmpA5->v=_tmp161;});_tmpA5;});}}}}
# 436
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){
# 439
unsigned long _tmpA6=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmpA6;
struct Cyc_List_List*_tmpA7=0;struct Cyc_List_List*typs=_tmpA7;
int i;
{struct _RegionHandle _tmpA8=_new_region("temp");struct _RegionHandle*temp=& _tmpA8;_push_region(temp);
for(i=0;(unsigned long)i < len;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'%')continue;{
struct Cyc_Core_Opt*_tmpA9=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);struct Cyc_Core_Opt*x=_tmpA9;
if(x == 0){
struct Cyc_List_List*_tmpAC=({void*_tmpAA=0U;({unsigned _tmp163=loc;struct _fat_ptr _tmp162=({const char*_tmpAB="bad format string";_tag_fat(_tmpAB,sizeof(char),18U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp163,_tmp162,_tag_fat(_tmpAA,sizeof(void*),0U));});});_npop_handler(0U);return _tmpAC;}{
struct _tuple12*_tmpAD=(struct _tuple12*)x->v;struct _tuple12*_stmttmp1=_tmpAD;struct _tuple12*_tmpAE=_stmttmp1;int _tmpB3;char _tmpB2;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;int _tmpAF;_LL1: _tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;_tmpB1=_tmpAE->f3;_tmpB2=_tmpAE->f4;_tmpB3=_tmpAE->f5;_LL2: {int suppress=_tmpAF;struct Cyc_List_List*width=_tmpB0;struct Cyc_List_List*lenmod=_tmpB1;char c=_tmpB2;int j=_tmpB3;
i=j - 1;
{struct Cyc_List_List*_tmpB4=lenmod;int _tmpB5;if(_tmpB4 != 0){if(((struct Cyc_List_List*)_tmpB4)->tl == 0){_LL4: _tmpB5=(int)_tmpB4->hd;if(
(_tmpB5 == (int)'j' || _tmpB5 == (int)'z')|| _tmpB5 == (int)'t'){_LL5: {int x=_tmpB5;
# 453
struct Cyc_List_List*_tmpB9=({struct Cyc_Int_pa_PrintArg_struct _tmpB8=({struct Cyc_Int_pa_PrintArg_struct _tmp117;_tmp117.tag=1U,_tmp117.f1=(unsigned long)x;_tmp117;});void*_tmpB6[1U];_tmpB6[0]=& _tmpB8;({unsigned _tmp165=loc;struct _fat_ptr _tmp164=({const char*_tmpB7="length modifier '%c' is not supported";_tag_fat(_tmpB7,sizeof(char),38U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp165,_tmp164,_tag_fat(_tmpB6,sizeof(void*),1U));});});_npop_handler(0U);return _tmpB9;}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 456
if(suppress)continue;{
void*t;
char _tmpBA=c;switch(_tmpBA){case 100U: _LL9: _LLA:
 goto _LLC;case 105U: _LLB: _LLC: {
# 461
struct Cyc_List_List*_tmpBB=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmpBB;
{struct Cyc_List_List*_tmpBC=lenmod;int _tmpBE;int _tmpBD;int _tmpBF;int _tmpC0;if(_tmpBC == 0){_LL34: _LL35:
 t=Cyc_Absyn_sint_type;goto _LL33;}else{if(((struct Cyc_List_List*)_tmpBC)->tl == 0){_LL36: _tmpC0=(int)_tmpBC->hd;if(_tmpC0 == (int)'l'){_LL37: {int x=_tmpC0;
t=Cyc_Absyn_slong_type;goto _LL33;}}else{_LL38: _tmpBF=(int)_tmpBC->hd;if(_tmpBF == (int)'h'){_LL39: {int x=_tmpBF;
t=Cyc_Absyn_sshort_type;goto _LL33;}}else{goto _LL3C;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpBC)->tl)->tl == 0){_LL3A: _tmpBD=(int)_tmpBC->hd;_tmpBE=(int)(_tmpBC->tl)->hd;if(
_tmpBD == (int)'h' && _tmpBE == (int)'h'){_LL3B: {int x=_tmpBD;int y=_tmpBE;t=Cyc_Absyn_schar_type;goto _LL33;}}else{goto _LL3C;}}else{_LL3C: _LL3D: {
# 468
struct Cyc_List_List*_tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp119;_tmp119.tag=0U,({
struct _fat_ptr _tmp166=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp119.f1=_tmp166;});_tmp119;});struct Cyc_Int_pa_PrintArg_struct _tmpC4=({struct Cyc_Int_pa_PrintArg_struct _tmp118;_tmp118.tag=1U,_tmp118.f1=(unsigned long)((int)c);_tmp118;});void*_tmpC1[2U];_tmpC1[0]=& _tmpC3,_tmpC1[1]=& _tmpC4;({unsigned _tmp168=loc;struct _fat_ptr _tmp167=({const char*_tmpC2="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpC2,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp168,_tmp167,_tag_fat(_tmpC1,sizeof(void*),2U));});});_npop_handler(0U);return _tmpC5;}}}}_LL33:;}
# 471
t=({void*_tmp16C=t;void*_tmp16B=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->v=tvs;_tmpC6;}));struct Cyc_Absyn_Tqual _tmp16A=Cyc_Absyn_empty_tqual(0U);void*_tmp169=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp16C,_tmp16B,_tmp16A,_tmp169,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->hd=t,_tmpC7->tl=typs;_tmpC7;});
goto _LL8;}case 117U: _LLD: _LLE:
 goto _LL10;case 111U: _LLF: _LL10:
 goto _LL12;case 120U: _LL11: _LL12:
 goto _LL14;case 88U: _LL13: _LL14: {
# 479
struct Cyc_List_List*_tmpC8=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmpC8;
{struct Cyc_List_List*_tmpC9=lenmod;int _tmpCB;int _tmpCA;int _tmpCC;int _tmpCD;if(_tmpC9 == 0){_LL3F: _LL40:
 t=Cyc_Absyn_uint_type;goto _LL3E;}else{if(((struct Cyc_List_List*)_tmpC9)->tl == 0){_LL41: _tmpCD=(int)_tmpC9->hd;if(_tmpCD == (int)'l'){_LL42: {int x=_tmpCD;
t=Cyc_Absyn_ulong_type;goto _LL3E;}}else{_LL43: _tmpCC=(int)_tmpC9->hd;if(_tmpCC == (int)'h'){_LL44: {int x=_tmpCC;
t=Cyc_Absyn_ushort_type;goto _LL3E;}}else{goto _LL47;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpC9)->tl)->tl == 0){_LL45: _tmpCA=(int)_tmpC9->hd;_tmpCB=(int)(_tmpC9->tl)->hd;if(
_tmpCA == (int)'h' && _tmpCB == (int)'h'){_LL46: {int x=_tmpCA;int y=_tmpCB;t=Cyc_Absyn_uchar_type;goto _LL3E;}}else{goto _LL47;}}else{_LL47: _LL48: {
# 486
struct Cyc_List_List*_tmpD2=({struct Cyc_String_pa_PrintArg_struct _tmpD0=({struct Cyc_String_pa_PrintArg_struct _tmp11B;_tmp11B.tag=0U,({
struct _fat_ptr _tmp16D=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp11B.f1=_tmp16D;});_tmp11B;});struct Cyc_Int_pa_PrintArg_struct _tmpD1=({struct Cyc_Int_pa_PrintArg_struct _tmp11A;_tmp11A.tag=1U,_tmp11A.f1=(unsigned long)((int)c);_tmp11A;});void*_tmpCE[2U];_tmpCE[0]=& _tmpD0,_tmpCE[1]=& _tmpD1;({unsigned _tmp16F=loc;struct _fat_ptr _tmp16E=({const char*_tmpCF="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpCF,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp16F,_tmp16E,_tag_fat(_tmpCE,sizeof(void*),2U));});});_npop_handler(0U);return _tmpD2;}}}}_LL3E:;}
# 489
t=({void*_tmp173=t;void*_tmp172=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->v=tvs;_tmpD3;}));struct Cyc_Absyn_Tqual _tmp171=Cyc_Absyn_empty_tqual(0U);void*_tmp170=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp173,_tmp172,_tmp171,_tmp170,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=t,_tmpD4->tl=typs;_tmpD4;});
goto _LL8;}case 102U: _LL15: _LL16:
 goto _LL18;case 70U: _LL17: _LL18:
 goto _LL1A;case 101U: _LL19: _LL1A:
 goto _LL1C;case 69U: _LL1B: _LL1C:
 goto _LL1E;case 103U: _LL1D: _LL1E:
 goto _LL20;case 71U: _LL1F: _LL20:
 goto _LL22;case 97U: _LL21: _LL22:
 goto _LL24;case 65U: _LL23: _LL24: {
# 501
struct Cyc_List_List*_tmpD5=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmpD5;
{struct Cyc_List_List*_tmpD6=lenmod;int _tmpD7;if(_tmpD6 == 0){_LL4A: _LL4B:
 t=Cyc_Absyn_float_type;goto _LL49;}else{if(((struct Cyc_List_List*)_tmpD6)->tl == 0){_LL4C: _tmpD7=(int)_tmpD6->hd;if(_tmpD7 == (int)'l'){_LL4D: {int x=_tmpD7;
# 505
t=Cyc_Absyn_double_type;goto _LL49;}}else{goto _LL4E;}}else{_LL4E: _LL4F: {
# 507
struct Cyc_List_List*_tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmpDA=({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0U,({
struct _fat_ptr _tmp174=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp11D.f1=_tmp174;});_tmp11D;});struct Cyc_Int_pa_PrintArg_struct _tmpDB=({struct Cyc_Int_pa_PrintArg_struct _tmp11C;_tmp11C.tag=1U,_tmp11C.f1=(unsigned long)((int)c);_tmp11C;});void*_tmpD8[2U];_tmpD8[0]=& _tmpDA,_tmpD8[1]=& _tmpDB;({unsigned _tmp176=loc;struct _fat_ptr _tmp175=({const char*_tmpD9="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpD9,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp176,_tmp175,_tag_fat(_tmpD8,sizeof(void*),2U));});});_npop_handler(0U);return _tmpDC;}}}_LL49:;}
# 510
t=({void*_tmp17A=t;void*_tmp179=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->v=tvs;_tmpDD;}));struct Cyc_Absyn_Tqual _tmp178=Cyc_Absyn_empty_tqual(0U);void*_tmp177=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp17A,_tmp179,_tmp178,_tmp177,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->hd=t,_tmpDE->tl=typs;_tmpDE;});
goto _LL8;}case 99U: _LL25: _LL26: {
# 516
struct Cyc_List_List*_tmpDF=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmpDF;
void*ptr;
if(!isCproto)
ptr=({void*_tmp17E=Cyc_Absyn_char_type;void*_tmp17D=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->v=tvs;_tmpE0;}));struct Cyc_Absyn_Tqual _tmp17C=
Cyc_Absyn_empty_tqual(0U);
# 519
void*_tmp17B=Cyc_Absyn_false_type;Cyc_Absyn_fatptr_type(_tmp17E,_tmp17D,_tmp17C,_tmp17B,
# 521
Cyc_Tcutil_any_bool(tvs));});else{
# 523
ptr=({void*_tmp182=Cyc_Absyn_char_type;void*_tmp181=
Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=tvs;_tmpE1;}));
# 523
struct Cyc_Absyn_Tqual _tmp180=
# 525
Cyc_Absyn_empty_tqual(0U);
# 523
void*_tmp17F=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp182,_tmp181,_tmp180,_tmp17F,
# 525
Cyc_Tcutil_any_bool(tvs));});}
typs=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=ptr,_tmpE2->tl=typs;_tmpE2;});
goto _LL8;}case 115U: _LL27: _LL28: {
# 529
struct Cyc_List_List*_tmpE3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmpE3;
# 531
void*ptr;
if(!isCproto)
ptr=({void*_tmp186=Cyc_Absyn_char_type;void*_tmp185=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->v=tvs;_tmpE4;}));struct Cyc_Absyn_Tqual _tmp184=
Cyc_Absyn_empty_tqual(0U);
# 533
void*_tmp183=Cyc_Absyn_false_type;Cyc_Absyn_fatptr_type(_tmp186,_tmp185,_tmp184,_tmp183,
Cyc_Tcutil_any_bool(tvs));});else{
# 536
ptr=({void*_tmp18A=Cyc_Absyn_char_type;void*_tmp189=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->v=tvs;_tmpE5;}));struct Cyc_Absyn_Tqual _tmp188=
Cyc_Absyn_empty_tqual(0U);
# 536
void*_tmp187=Cyc_Absyn_true_type;Cyc_Absyn_at_type(_tmp18A,_tmp189,_tmp188,_tmp187,
Cyc_Tcutil_any_bool(tvs));});}
typs=({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=ptr,_tmpE6->tl=typs;_tmpE6;});
goto _LL8;}case 91U: _LL29: _LL2A:
 goto _LL2C;case 112U: _LL2B: _LL2C: {
# 542
struct Cyc_List_List*_tmpEA=({struct Cyc_Int_pa_PrintArg_struct _tmpE9=({struct Cyc_Int_pa_PrintArg_struct _tmp11E;_tmp11E.tag=1U,_tmp11E.f1=(unsigned long)((int)c);_tmp11E;});void*_tmpE7[1U];_tmpE7[0]=& _tmpE9;({unsigned _tmp18C=loc;struct _fat_ptr _tmp18B=({const char*_tmpE8="%%%c is not supported";_tag_fat(_tmpE8,sizeof(char),22U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp18C,_tmp18B,_tag_fat(_tmpE7,sizeof(void*),1U));});});_npop_handler(0U);return _tmpEA;}case 110U: _LL2D: _LL2E: {
# 544
struct Cyc_List_List*_tmpEB=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tvs=_tmpEB;
{struct Cyc_List_List*_tmpEC=lenmod;int _tmpEE;int _tmpED;int _tmpEF;int _tmpF0;if(_tmpEC == 0){_LL51: _LL52:
 t=Cyc_Absyn_sint_type;goto _LL50;}else{if(((struct Cyc_List_List*)_tmpEC)->tl == 0){_LL53: _tmpF0=(int)_tmpEC->hd;if(_tmpF0 == (int)'l'){_LL54: {int x=_tmpF0;
t=Cyc_Absyn_ulong_type;goto _LL50;}}else{_LL55: _tmpEF=(int)_tmpEC->hd;if(_tmpEF == (int)'h'){_LL56: {int x=_tmpEF;
t=Cyc_Absyn_sshort_type;goto _LL50;}}else{goto _LL59;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpEC)->tl)->tl == 0){_LL57: _tmpED=(int)_tmpEC->hd;_tmpEE=(int)(_tmpEC->tl)->hd;if(
_tmpED == (int)'h' && _tmpEE == (int)'h'){_LL58: {int x=_tmpED;int y=_tmpEE;t=Cyc_Absyn_schar_type;goto _LL50;}}else{goto _LL59;}}else{_LL59: _LL5A: {
# 551
struct Cyc_List_List*_tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmpF3=({struct Cyc_String_pa_PrintArg_struct _tmp120;_tmp120.tag=0U,({
struct _fat_ptr _tmp18D=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp120.f1=_tmp18D;});_tmp120;});struct Cyc_Int_pa_PrintArg_struct _tmpF4=({struct Cyc_Int_pa_PrintArg_struct _tmp11F;_tmp11F.tag=1U,_tmp11F.f1=(unsigned long)((int)c);_tmp11F;});void*_tmpF1[2U];_tmpF1[0]=& _tmpF3,_tmpF1[1]=& _tmpF4;({unsigned _tmp18F=loc;struct _fat_ptr _tmp18E=({const char*_tmpF2="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpF2,sizeof(char),46U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp18F,_tmp18E,_tag_fat(_tmpF1,sizeof(void*),2U));});});_npop_handler(0U);return _tmpF5;}}}}_LL50:;}
# 554
t=({void*_tmp193=t;void*_tmp192=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->v=tvs;_tmpF6;}));struct Cyc_Absyn_Tqual _tmp191=Cyc_Absyn_empty_tqual(0U);void*_tmp190=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_tmp193,_tmp192,_tmp191,_tmp190,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->hd=t,_tmpF7->tl=typs;_tmpF7;});
goto _LL8;}case 37U: _LL2F: _LL30:
# 559
 if(suppress){
struct Cyc_List_List*_tmpFA=({void*_tmpF8=0U;({unsigned _tmp195=loc;struct _fat_ptr _tmp194=({const char*_tmpF9="Assignment suppression (*) is not allowed with %%%%";_tag_fat(_tmpF9,sizeof(char),52U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp195,_tmp194,_tag_fat(_tmpF8,sizeof(void*),0U));});});_npop_handler(0U);return _tmpFA;}
if(width != 0){
struct Cyc_List_List*_tmpFE=({struct Cyc_String_pa_PrintArg_struct _tmpFD=({struct Cyc_String_pa_PrintArg_struct _tmp121;_tmp121.tag=0U,({struct _fat_ptr _tmp196=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(width));_tmp121.f1=_tmp196;});_tmp121;});void*_tmpFB[1U];_tmpFB[0]=& _tmpFD;({unsigned _tmp198=loc;struct _fat_ptr _tmp197=({const char*_tmpFC="width '%s' not allowed with %%%%";_tag_fat(_tmpFC,sizeof(char),33U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp198,_tmp197,_tag_fat(_tmpFB,sizeof(void*),1U));});});_npop_handler(0U);return _tmpFE;}
if(lenmod != 0){
struct Cyc_List_List*_tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp101=({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0U,({
struct _fat_ptr _tmp199=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp122.f1=_tmp199;});_tmp122;});void*_tmpFF[1U];_tmpFF[0]=& _tmp101;({unsigned _tmp19B=loc;struct _fat_ptr _tmp19A=({const char*_tmp100="length modifier '%s' not allowed with %%%%";_tag_fat(_tmp100,sizeof(char),43U);});((struct Cyc_List_List*(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Formatstr_err_null)(_tmp19B,_tmp19A,_tag_fat(_tmpFF,sizeof(void*),1U));});});_npop_handler(0U);return _tmp102;}
goto _LL8;default: _LL31: _LL32: {
struct Cyc_List_List*_tmp103=0;_npop_handler(0U);return _tmp103;}}_LL8:;}}}}}
# 443
;_pop_region();}
# 571
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(typs);}

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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 890 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 916
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;
# 918
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;
# 920
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 927
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 968
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 972
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 31 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 155
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct _tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;char f5;int f6;};
# 36 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _fat_ptr s,int i){
# 40
unsigned long _tmp0=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp0;
if(i < 0 ||(unsigned long)i >= len)return 0;{
# 44
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
# 56
break;}
# 58
if((unsigned long)i >= len)return 0;
flags=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(flags);{
# 62
struct Cyc_List_List*_tmp4=0;struct Cyc_List_List*width=_tmp4;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'*'){
width=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->hd=(void*)((int)c),_tmp5->tl=width;_tmp5;});
++ i;}else{
# 68
for(0;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))width=({struct Cyc_List_List*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->hd=(void*)((int)c),_tmp6->tl=width;_tmp6;});else{
break;}}}
# 74
if((unsigned long)i >= len)return 0;
width=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(width);{
# 78
struct Cyc_List_List*_tmp7=0;struct Cyc_List_List*precision=_tmp7;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'.'){
precision=({struct Cyc_List_List*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->hd=(void*)((int)c),_tmp8->tl=precision;_tmp8;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'*'){
precision=({struct Cyc_List_List*_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9->hd=(void*)((int)c),_tmp9->tl=precision;_tmp9;});
++ i;}else{
# 88
for(0;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))precision=({struct Cyc_List_List*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->hd=(void*)((int)c),_tmpA->tl=precision;_tmpA;});else{
break;}}}}
# 94
if((unsigned long)i >= len)return 0;
precision=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(precision);{
# 99
struct Cyc_List_List*_tmpB=0;struct Cyc_List_List*lenmod=_tmpB;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmpC=c;switch(_tmpC){case 104U: _LLE: _LLF:
# 103
 lenmod=({struct Cyc_List_List*_tmpD=_region_malloc(r,sizeof(*_tmpD));_tmpD->hd=(void*)((int)c),_tmpD->tl=lenmod;_tmpD;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'h'){lenmod=({struct Cyc_List_List*_tmpE=_region_malloc(r,sizeof(*_tmpE));_tmpE->hd=(void*)((int)c),_tmpE->tl=lenmod;_tmpE;});++ i;}
goto _LLD;case 108U: _LL10: _LL11:
# 109
 lenmod=({struct Cyc_List_List*_tmpF=_region_malloc(r,sizeof(*_tmpF));_tmpF->hd=(void*)((int)c),_tmpF->tl=lenmod;_tmpF;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'l'){lenmod=({struct Cyc_List_List*_tmp10=_region_malloc(r,sizeof(*_tmp10));_tmp10->hd=(void*)((int)c),_tmp10->tl=lenmod;_tmp10;});++ i;}
goto _LLD;case 106U: _LL12: _LL13:
 goto _LL15;case 122U: _LL14: _LL15:
 goto _LL17;case 116U: _LL16: _LL17:
 goto _LL19;case 76U: _LL18: _LL19:
# 118
 lenmod=({struct Cyc_List_List*_tmp11=_region_malloc(r,sizeof(*_tmp11));_tmp11->hd=(void*)((int)c),_tmp11->tl=lenmod;_tmp11;});
++ i;
goto _LLD;default: _LL1A: _LL1B:
 goto _LLD;}_LLD:;}
# 123
if((unsigned long)i >= len)return 0;
lenmod=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(lenmod);
# 127
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
# 159 "formatstr.cyc"
return({struct Cyc_Core_Opt*_tmp14=_region_malloc(r,sizeof(*_tmp14));({struct _tuple11*_tmp120=({struct _tuple11*_tmp13=_region_malloc(r,sizeof(*_tmp13));_tmp13->f1=flags,_tmp13->f2=width,_tmp13->f3=precision,_tmp13->f4=lenmod,_tmp13->f5=c,_tmp13->f6=i + 1;_tmp13;});_tmp14->v=_tmp120;});_tmp14;});}}}}}
# 162
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){
# 165
unsigned long _tmp15=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp15;
struct Cyc_List_List*_tmp16=0;struct Cyc_List_List*typs=_tmp16;
int i;
struct _RegionHandle _tmp17=_new_region("temp");struct _RegionHandle*temp=& _tmp17;_push_region(temp);
for(i=0;(unsigned long)i < len;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'%')continue;{
struct Cyc_Core_Opt*_tmp18=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 1);struct Cyc_Core_Opt*cs=_tmp18;
if(cs == 0){
({void*_tmp19=0U;({unsigned _tmp122=loc;struct _fat_ptr _tmp121=({const char*_tmp1A="bad format string";_tag_fat(_tmp1A,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp122,_tmp121,_tag_fat(_tmp19,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp1B=0;_npop_handler(0U);return _tmp1B;}}{
# 176
struct _tuple11*_tmp1C=(struct _tuple11*)cs->v;struct _tuple11*_stmttmp0=_tmp1C;struct _tuple11*_tmp1D=_stmttmp0;int _tmp23;char _tmp22;struct Cyc_List_List*_tmp21;struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp1F;struct Cyc_List_List*_tmp1E;_LL1: _tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;_tmp20=_tmp1D->f3;_tmp21=_tmp1D->f4;_tmp22=_tmp1D->f5;_tmp23=_tmp1D->f6;_LL2: {struct Cyc_List_List*flags=_tmp1E;struct Cyc_List_List*width=_tmp1F;struct Cyc_List_List*precision=_tmp20;struct Cyc_List_List*lenmod=_tmp21;char c=_tmp22;int j=_tmp23;
i=j - 1;
{struct Cyc_List_List*_tmp24=lenmod;int _tmp25;if(_tmp24 != 0){if(((struct Cyc_List_List*)_tmp24)->tl == 0){_LL4: _tmp25=(int)_tmp24->hd;if(
(_tmp25 == (int)'j' || _tmp25 == (int)'z')|| _tmp25 == (int)'t'){_LL5: {int x=_tmp25;
# 182
({struct Cyc_Int_pa_PrintArg_struct _tmp28=({struct Cyc_Int_pa_PrintArg_struct _tmp101;_tmp101.tag=1U,_tmp101.f1=(unsigned long)x;_tmp101;});void*_tmp26[1U];_tmp26[0]=& _tmp28;({unsigned _tmp124=loc;struct _fat_ptr _tmp123=({const char*_tmp27="length modifier '%c' is not supported";_tag_fat(_tmp27,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp124,_tmp123,_tag_fat(_tmp26,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp29=0;_npop_handler(0U);return _tmp29;}}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 186
{struct Cyc_List_List*_tmp2A=width;int _tmp2B;if(_tmp2A != 0){if(((struct Cyc_List_List*)_tmp2A)->tl == 0){_LL9: _tmp2B=(int)_tmp2A->hd;if(_tmp2B == (int)'*'){_LLA: {int x=_tmp2B;
typs=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=Cyc_Absyn_sint_type,_tmp2C->tl=typs;_tmp2C;});goto _LL8;}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _LLC:
 goto _LL8;}_LL8:;}
# 190
{struct Cyc_List_List*_tmp2D=precision;int _tmp2F;int _tmp2E;if(_tmp2D != 0){if(((struct Cyc_List_List*)_tmp2D)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp2D)->tl)->tl == 0){_LLE: _tmp2E=(int)_tmp2D->hd;_tmp2F=(int)(_tmp2D->tl)->hd;if(
_tmp2E == (int)'.' && _tmp2F == (int)'*'){_LLF: {int x=_tmp2E;int y=_tmp2F;
typs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->hd=Cyc_Absyn_sint_type,_tmp30->tl=typs;_tmp30;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}{
# 195
void*t;
char _tmp31=c;switch(_tmp31){case 100U: _LL13: _LL14:
 goto _LL16;case 105U: _LL15: _LL16:
# 199
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#'){
({struct Cyc_Int_pa_PrintArg_struct _tmp34=({struct Cyc_Int_pa_PrintArg_struct _tmp102;_tmp102.tag=1U,_tmp102.f1=(unsigned long)((int)c);_tmp102;});void*_tmp32[1U];_tmp32[0]=& _tmp34;({unsigned _tmp126=loc;struct _fat_ptr _tmp125=({const char*_tmp33="flag '#' is not valid with %%%c";_tag_fat(_tmp33,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp126,_tmp125,_tag_fat(_tmp32,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp35=0;_npop_handler(0U);return _tmp35;}}}}
# 204
{struct Cyc_List_List*_tmp36=lenmod;int _tmp38;int _tmp37;int _tmp39;int _tmp3A;if(_tmp36 == 0){_LL3C: _LL3D:
 t=Cyc_Absyn_sint_type;goto _LL3B;}else{if(((struct Cyc_List_List*)_tmp36)->tl == 0){_LL3E: _tmp3A=(int)_tmp36->hd;if(_tmp3A == (int)'l'){_LL3F: {int x=_tmp3A;
t=Cyc_Absyn_slong_type;goto _LL3B;}}else{_LL40: _tmp39=(int)_tmp36->hd;if(_tmp39 == (int)'h'){_LL41: {int x=_tmp39;
t=Cyc_Absyn_sshort_type;goto _LL3B;}}else{goto _LL44;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp36)->tl)->tl == 0){_LL42: _tmp37=(int)_tmp36->hd;_tmp38=(int)(_tmp36->tl)->hd;if(
_tmp37 == (int)'h' && _tmp38 == (int)'h'){_LL43: {int x=_tmp37;int y=_tmp38;
t=Cyc_Absyn_schar_type;goto _LL3B;}}else{goto _LL44;}}else{_LL44: _LL45:
# 211
({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0U,({
struct _fat_ptr _tmp127=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp104.f1=_tmp127;});_tmp104;});struct Cyc_Int_pa_PrintArg_struct _tmp3E=({struct Cyc_Int_pa_PrintArg_struct _tmp103;_tmp103.tag=1U,_tmp103.f1=(unsigned long)((int)c);_tmp103;});void*_tmp3B[2U];_tmp3B[0]=& _tmp3D,_tmp3B[1]=& _tmp3E;({unsigned _tmp129=loc;struct _fat_ptr _tmp128=({const char*_tmp3C="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp3C,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp129,_tmp128,_tag_fat(_tmp3B,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmp3F=0;_npop_handler(0U);return _tmp3F;}}}}_LL3B:;}
# 215
typs=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->hd=t,_tmp40->tl=typs;_tmp40;});
goto _LL12;case 117U: _LL17: _LL18:
# 218
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#'){
({void*_tmp41=0U;({unsigned _tmp12B=loc;struct _fat_ptr _tmp12A=({const char*_tmp42="Flag '#' not valid with %%u";_tag_fat(_tmp42,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp12B,_tmp12A,_tag_fat(_tmp41,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp43=0;_npop_handler(0U);return _tmp43;}}}}
# 223
goto _LL1A;case 111U: _LL19: _LL1A:
 goto _LL1C;case 120U: _LL1B: _LL1C:
 goto _LL1E;case 88U: _LL1D: _LL1E:
# 227
{struct Cyc_List_List*_tmp44=lenmod;int _tmp46;int _tmp45;int _tmp47;int _tmp48;if(_tmp44 == 0){_LL47: _LL48:
 t=Cyc_Absyn_uint_type;goto _LL46;}else{if(((struct Cyc_List_List*)_tmp44)->tl == 0){_LL49: _tmp48=(int)_tmp44->hd;if(_tmp48 == (int)'l'){_LL4A: {int x=_tmp48;
t=Cyc_Absyn_ulong_type;goto _LL46;}}else{_LL4B: _tmp47=(int)_tmp44->hd;if(_tmp47 == (int)'h'){_LL4C: {int x=_tmp47;
t=Cyc_Absyn_ushort_type;goto _LL46;}}else{goto _LL4F;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp44)->tl)->tl == 0){_LL4D: _tmp45=(int)_tmp44->hd;_tmp46=(int)(_tmp44->tl)->hd;if(
_tmp45 == (int)'h' && _tmp46 == (int)'h'){_LL4E: {int x=_tmp45;int y=_tmp46;
t=Cyc_Absyn_uchar_type;goto _LL46;}}else{goto _LL4F;}}else{_LL4F: _LL50:
# 235
({struct Cyc_String_pa_PrintArg_struct _tmp4B=({struct Cyc_String_pa_PrintArg_struct _tmp106;_tmp106.tag=0U,({
struct _fat_ptr _tmp12C=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp106.f1=_tmp12C;});_tmp106;});struct Cyc_Int_pa_PrintArg_struct _tmp4C=({struct Cyc_Int_pa_PrintArg_struct _tmp105;_tmp105.tag=1U,_tmp105.f1=(unsigned long)((int)c);_tmp105;});void*_tmp49[2U];_tmp49[0]=& _tmp4B,_tmp49[1]=& _tmp4C;({unsigned _tmp12E=loc;struct _fat_ptr _tmp12D=({const char*_tmp4A="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp4A,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp12E,_tmp12D,_tag_fat(_tmp49,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmp4D=0;_npop_handler(0U);return _tmp4D;}}}}_LL46:;}
# 239
typs=({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=t,_tmp4E->tl=typs;_tmp4E;});
goto _LL12;case 102U: _LL1F: _LL20:
 goto _LL22;case 70U: _LL21: _LL22:
 goto _LL24;case 101U: _LL23: _LL24:
 goto _LL26;case 69U: _LL25: _LL26:
 goto _LL28;case 103U: _LL27: _LL28:
 goto _LL2A;case 71U: _LL29: _LL2A:
 goto _LL2C;case 97U: _LL2B: _LL2C:
 goto _LL2E;case 65U: _LL2D: _LL2E:
# 255
{struct Cyc_List_List*_tmp4F=lenmod;int _tmp50;if(_tmp4F == 0){_LL52: _LL53:
# 257
 typs=({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->hd=Cyc_Absyn_double_type,_tmp51->tl=typs;_tmp51;});
goto _LL51;}else{if(((struct Cyc_List_List*)_tmp4F)->tl == 0){_LL54: _tmp50=(int)_tmp4F->hd;if(_tmp50 == (int)'l'){_LL55: {int x=_tmp50;
# 260
typs=({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->hd=Cyc_Absyn_long_double_type,_tmp52->tl=typs;_tmp52;});goto _LL51;}}else{goto _LL56;}}else{_LL56: _LL57:
# 262
({struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0U,({
struct _fat_ptr _tmp12F=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp108.f1=_tmp12F;});_tmp108;});struct Cyc_Int_pa_PrintArg_struct _tmp56=({struct Cyc_Int_pa_PrintArg_struct _tmp107;_tmp107.tag=1U,_tmp107.f1=(unsigned long)((int)c);_tmp107;});void*_tmp53[2U];_tmp53[0]=& _tmp55,_tmp53[1]=& _tmp56;({unsigned _tmp131=loc;struct _fat_ptr _tmp130=({const char*_tmp54="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp54,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp131,_tmp130,_tag_fat(_tmp53,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmp57=0;_npop_handler(0U);return _tmp57;}}}_LL51:;}
# 266
goto _LL12;case 99U: _LL2F: _LL30:
# 268
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#' ||(int)f->hd == (int)'0'){
({struct Cyc_Int_pa_PrintArg_struct _tmp5A=({struct Cyc_Int_pa_PrintArg_struct _tmp109;_tmp109.tag=1U,_tmp109.f1=(unsigned long)((int)f->hd);_tmp109;});void*_tmp58[1U];_tmp58[0]=& _tmp5A;({unsigned _tmp133=loc;struct _fat_ptr _tmp132=({const char*_tmp59="flag '%c' not allowed with %%c";_tag_fat(_tmp59,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp133,_tmp132,_tag_fat(_tmp58,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp5B=0;_npop_handler(0U);return _tmp5B;}}}}
# 275
if(lenmod != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp5E=({struct Cyc_String_pa_PrintArg_struct _tmp10A;_tmp10A.tag=0U,({
struct _fat_ptr _tmp134=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp10A.f1=_tmp134;});_tmp10A;});void*_tmp5C[1U];_tmp5C[0]=& _tmp5E;({unsigned _tmp136=loc;struct _fat_ptr _tmp135=({const char*_tmp5D="length modifier '%s' not allowed with %%c";_tag_fat(_tmp5D,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp136,_tmp135,_tag_fat(_tmp5C,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp5F=0;_npop_handler(0U);return _tmp5F;}}
# 280
if(precision != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp10B;_tmp10B.tag=0U,({
struct _fat_ptr _tmp137=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(precision));_tmp10B.f1=_tmp137;});_tmp10B;});void*_tmp60[1U];_tmp60[0]=& _tmp62;({unsigned _tmp139=loc;struct _fat_ptr _tmp138=({const char*_tmp61="precision '%s' not allowed with %%c";_tag_fat(_tmp61,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp139,_tmp138,_tag_fat(_tmp60,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp63=0;_npop_handler(0U);return _tmp63;}}
# 285
typs=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=Cyc_Absyn_sint_type,_tmp64->tl=typs;_tmp64;});
goto _LL12;case 115U: _LL31: _LL32:
# 289
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd != (int)'-'){
({void*_tmp65=0U;({unsigned _tmp13B=loc;struct _fat_ptr _tmp13A=({const char*_tmp66="a flag not allowed with %%s";_tag_fat(_tmp66,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp13B,_tmp13A,_tag_fat(_tmp65,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp67=0;_npop_handler(0U);return _tmp67;}}}}
# 296
if(lenmod != 0){
({void*_tmp68=0U;({unsigned _tmp13D=loc;struct _fat_ptr _tmp13C=({const char*_tmp69="length modifiers not allowed with %%s";_tag_fat(_tmp69,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp13D,_tmp13C,_tag_fat(_tmp68,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp6A=0;_npop_handler(0U);return _tmp6A;}}{
# 302
void*ptr;
if(!isCproto)
ptr=({void*_tmp141=Cyc_Absyn_char_type;void*_tmp140=
Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp6B=_cycalloc(sizeof(*_tmp6B));({
struct Cyc_List_List*_tmp13E=Cyc_Tcenv_lookup_type_vars(te);_tmp6B->v=_tmp13E;});_tmp6B;}));
# 304
struct Cyc_Absyn_Tqual _tmp13F=
# 307
Cyc_Absyn_const_tqual(0U);
# 304
Cyc_Absyn_fatptr_type(_tmp141,_tmp140,_tmp13F,Cyc_Absyn_false_type);});else{
# 309
ptr=({void*_tmp145=Cyc_Absyn_char_type;void*_tmp144=
Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp6C=_cycalloc(sizeof(*_tmp6C));({
struct Cyc_List_List*_tmp142=Cyc_Tcenv_lookup_type_vars(te);_tmp6C->v=_tmp142;});_tmp6C;}));
# 309
struct Cyc_Absyn_Tqual _tmp143=
# 312
Cyc_Absyn_const_tqual(0U);
# 309
Cyc_Absyn_at_type(_tmp145,_tmp144,_tmp143,Cyc_Absyn_true_type);});}
# 313
typs=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=ptr,_tmp6D->tl=typs;_tmp6D;});
goto _LL12;}case 112U: _LL33: _LL34:
# 317
 typs=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absyn_uint_type,_tmp6E->tl=typs;_tmp6E;});
goto _LL12;case 110U: _LL35: _LL36:
# 320
{struct Cyc_List_List*f=flags;for(0;f != 0;f=f->tl){
if((int)f->hd == (int)'#' ||(int)f->hd == (int)'0'){
({struct Cyc_Int_pa_PrintArg_struct _tmp71=({struct Cyc_Int_pa_PrintArg_struct _tmp10C;_tmp10C.tag=1U,_tmp10C.f1=(unsigned long)((int)f->hd);_tmp10C;});void*_tmp6F[1U];_tmp6F[0]=& _tmp71;({unsigned _tmp147=loc;struct _fat_ptr _tmp146=({const char*_tmp70="flag '%c' not allowed with %%n";_tag_fat(_tmp70,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp147,_tmp146,_tag_fat(_tmp6F,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp72=0;_npop_handler(0U);return _tmp72;}}}}
# 325
if(precision != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0U,({
struct _fat_ptr _tmp148=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(precision));_tmp10D.f1=_tmp148;});_tmp10D;});void*_tmp73[1U];_tmp73[0]=& _tmp75;({unsigned _tmp14A=loc;struct _fat_ptr _tmp149=({const char*_tmp74="precision '%s' not allowed with %%n";_tag_fat(_tmp74,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp14A,_tmp149,_tag_fat(_tmp73,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp76=0;_npop_handler(0U);return _tmp76;}}{
# 330
struct Cyc_Core_Opt*_tmp77=({struct Cyc_Core_Opt*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_List_List*_tmp14B=Cyc_Tcenv_lookup_type_vars(te);_tmp83->v=_tmp14B;});_tmp83;});struct Cyc_Core_Opt*tvs=_tmp77;
{struct Cyc_List_List*_tmp78=lenmod;int _tmp7A;int _tmp79;int _tmp7B;int _tmp7C;if(_tmp78 == 0){_LL59: _LL5A:
 t=Cyc_Absyn_sint_type;goto _LL58;}else{if(((struct Cyc_List_List*)_tmp78)->tl == 0){_LL5B: _tmp7C=(int)_tmp78->hd;if(_tmp7C == (int)'l'){_LL5C: {int x=_tmp7C;
# 334
t=Cyc_Absyn_ulong_type;goto _LL58;}}else{_LL5D: _tmp7B=(int)_tmp78->hd;if(_tmp7B == (int)'h'){_LL5E: {int x=_tmp7B;
t=Cyc_Absyn_sshort_type;goto _LL58;}}else{goto _LL61;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmp78)->tl)->tl == 0){_LL5F: _tmp79=(int)_tmp78->hd;_tmp7A=(int)(_tmp78->tl)->hd;if(
_tmp79 == (int)'h' && _tmp7A == (int)'h'){_LL60: {int x=_tmp79;int y=_tmp7A;
t=Cyc_Absyn_schar_type;goto _LL58;}}else{goto _LL61;}}else{_LL61: _LL62:
# 339
({struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmp10F;_tmp10F.tag=0U,({
struct _fat_ptr _tmp14C=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp10F.f1=_tmp14C;});_tmp10F;});struct Cyc_Int_pa_PrintArg_struct _tmp80=({struct Cyc_Int_pa_PrintArg_struct _tmp10E;_tmp10E.tag=1U,_tmp10E.f1=(unsigned long)((int)c);_tmp10E;});void*_tmp7D[2U];_tmp7D[0]=& _tmp7F,_tmp7D[1]=& _tmp80;({unsigned _tmp14E=loc;struct _fat_ptr _tmp14D=({const char*_tmp7E="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmp7E,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp14E,_tmp14D,_tag_fat(_tmp7D,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmp81=0;_npop_handler(0U);return _tmp81;}}}}_LL58:;}
# 343
t=({void*_tmp151=t;void*_tmp150=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp14F=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp151,_tmp150,_tmp14F,Cyc_Absyn_false_type);});
typs=({struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=t,_tmp82->tl=typs;_tmp82;});
goto _LL12;}case 37U: _LL37: _LL38:
# 347
 if(flags != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp86=({struct Cyc_String_pa_PrintArg_struct _tmp110;_tmp110.tag=0U,({struct _fat_ptr _tmp152=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(flags));_tmp110.f1=_tmp152;});_tmp110;});void*_tmp84[1U];_tmp84[0]=& _tmp86;({unsigned _tmp154=loc;struct _fat_ptr _tmp153=({const char*_tmp85="flags '%s' not allowed with %%%%";_tag_fat(_tmp85,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp154,_tmp153,_tag_fat(_tmp84,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp87=0;_npop_handler(0U);return _tmp87;}}
# 351
if(width != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0U,({struct _fat_ptr _tmp155=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(width));_tmp111.f1=_tmp155;});_tmp111;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({unsigned _tmp157=loc;struct _fat_ptr _tmp156=({const char*_tmp89="width '%s' not allowed with %%%%";_tag_fat(_tmp89,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp157,_tmp156,_tag_fat(_tmp88,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp8B=0;_npop_handler(0U);return _tmp8B;}}
# 355
if(precision != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8E=({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0U,({struct _fat_ptr _tmp158=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(precision));_tmp112.f1=_tmp158;});_tmp112;});void*_tmp8C[1U];_tmp8C[0]=& _tmp8E;({unsigned _tmp15A=loc;struct _fat_ptr _tmp159=({const char*_tmp8D="precision '%s' not allowed with %%%%";_tag_fat(_tmp8D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp15A,_tmp159,_tag_fat(_tmp8C,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp8F=0;_npop_handler(0U);return _tmp8F;}}
# 359
if(lenmod != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0U,({struct _fat_ptr _tmp15B=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp113.f1=_tmp15B;});_tmp113;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({unsigned _tmp15D=loc;struct _fat_ptr _tmp15C=({const char*_tmp91="length modifier '%s' not allowed with %%%%";_tag_fat(_tmp91,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp15D,_tmp15C,_tag_fat(_tmp90,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp93=0;_npop_handler(0U);return _tmp93;}}
# 363
goto _LL12;default: _LL39: _LL3A: {
# 365
struct Cyc_List_List*_tmp94=0;_npop_handler(0U);return _tmp94;}}_LL12:;}}}}}{
# 368
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(typs);_npop_handler(0U);return _tmp95;}
# 169
;_pop_region();}struct _tuple12{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};
# 379 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _fat_ptr s,int i){
# 381
unsigned long _tmp96=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp96;
if(i < 0 ||(unsigned long)i >= len)return 0;{
# 384
int _tmp97=0;int suppress=_tmp97;
char _tmp98=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmp98;
if((int)c == (int)'*'){
suppress=1;
++ i;
if((unsigned long)i >= len)return 0;}{
# 392
struct Cyc_List_List*_tmp99=0;struct Cyc_List_List*width=_tmp99;
for(0;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))width=({struct Cyc_List_List*_tmp9A=_region_malloc(r,sizeof(*_tmp9A));_tmp9A->hd=(void*)((int)c),_tmp9A->tl=width;_tmp9A;});else{
break;}}
# 398
if((unsigned long)i >= len)return 0;
width=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(width);{
# 403
struct Cyc_List_List*_tmp9B=0;struct Cyc_List_List*lenmod=_tmp9B;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmp9C=c;switch(_tmp9C){case 104U: _LL1: _LL2:
# 407
 lenmod=({struct Cyc_List_List*_tmp9D=_region_malloc(r,sizeof(*_tmp9D));_tmp9D->hd=(void*)((int)c),_tmp9D->tl=lenmod;_tmp9D;});
++ i;
if((unsigned long)i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'h'){lenmod=({struct Cyc_List_List*_tmp9E=_region_malloc(r,sizeof(*_tmp9E));_tmp9E->hd=(void*)((int)c),_tmp9E->tl=lenmod;_tmp9E;});++ i;}
goto _LL0;case 108U: _LL3: _LL4:
# 414
 lenmod=({struct Cyc_List_List*_tmp9F=_region_malloc(r,sizeof(*_tmp9F));_tmp9F->hd=(void*)((int)c),_tmp9F->tl=lenmod;_tmp9F;});
++ i;
if((unsigned long)i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == (int)'l'){lenmod=({struct Cyc_List_List*_tmpA0=_region_malloc(r,sizeof(*_tmpA0));_tmpA0->hd=(void*)((int)c),_tmpA0->tl=lenmod;_tmpA0;});++ i;}
goto _LL0;case 106U: _LL5: _LL6:
 goto _LL8;case 122U: _LL7: _LL8:
 goto _LLA;case 116U: _LL9: _LLA:
 goto _LLC;case 76U: _LLB: _LLC:
# 424
 lenmod=({struct Cyc_List_List*_tmpA1=_region_malloc(r,sizeof(*_tmpA1));_tmpA1->hd=(void*)((int)c),_tmpA1->tl=lenmod;_tmpA1;});
++ i;
goto _LL0;default: _LLD: _LLE:
 goto _LL0;}_LL0:;}
# 429
if((unsigned long)i >= len)return 0;
lenmod=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(lenmod);
# 433
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
{char _tmpA2=c;switch(_tmpA2){case 100U: _LL10: _LL11:
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
# 456
return({struct Cyc_Core_Opt*_tmpA4=_region_malloc(r,sizeof(*_tmpA4));({struct _tuple12*_tmp15E=({struct _tuple12*_tmpA3=_region_malloc(r,sizeof(*_tmpA3));_tmpA3->f1=suppress,_tmpA3->f2=width,_tmpA3->f3=lenmod,_tmpA3->f4=c,_tmpA3->f5=i + 1;_tmpA3;});_tmpA4->v=_tmp15E;});_tmpA4;});}}}}
# 458
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){
# 461
unsigned long _tmpA5=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmpA5;
struct Cyc_List_List*_tmpA6=0;struct Cyc_List_List*typs=_tmpA6;
int i;
{struct _RegionHandle _tmpA7=_new_region("temp");struct _RegionHandle*temp=& _tmpA7;_push_region(temp);
for(i=0;(unsigned long)i < len;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'%')continue;{
struct Cyc_Core_Opt*_tmpA8=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);struct Cyc_Core_Opt*x=_tmpA8;
if(x == 0){
({void*_tmpA9=0U;({unsigned _tmp160=loc;struct _fat_ptr _tmp15F=({const char*_tmpAA="bad format string";_tag_fat(_tmpAA,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp160,_tmp15F,_tag_fat(_tmpA9,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpAB=0;_npop_handler(0U);return _tmpAB;}}{
# 472
struct _tuple12*_tmpAC=(struct _tuple12*)x->v;struct _tuple12*_stmttmp1=_tmpAC;struct _tuple12*_tmpAD=_stmttmp1;int _tmpB2;char _tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;int _tmpAE;_LL1: _tmpAE=_tmpAD->f1;_tmpAF=_tmpAD->f2;_tmpB0=_tmpAD->f3;_tmpB1=_tmpAD->f4;_tmpB2=_tmpAD->f5;_LL2: {int suppress=_tmpAE;struct Cyc_List_List*width=_tmpAF;struct Cyc_List_List*lenmod=_tmpB0;char c=_tmpB1;int j=_tmpB2;
i=j - 1;
{struct Cyc_List_List*_tmpB3=lenmod;int _tmpB4;if(_tmpB3 != 0){if(((struct Cyc_List_List*)_tmpB3)->tl == 0){_LL4: _tmpB4=(int)_tmpB3->hd;if(
(_tmpB4 == (int)'j' || _tmpB4 == (int)'z')|| _tmpB4 == (int)'t'){_LL5: {int x=_tmpB4;
# 477
({struct Cyc_Int_pa_PrintArg_struct _tmpB7=({struct Cyc_Int_pa_PrintArg_struct _tmp114;_tmp114.tag=1U,_tmp114.f1=(unsigned long)x;_tmp114;});void*_tmpB5[1U];_tmpB5[0]=& _tmpB7;({unsigned _tmp162=loc;struct _fat_ptr _tmp161=({const char*_tmpB6="length modifier '%c' is not supported";_tag_fat(_tmpB6,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp162,_tmp161,_tag_fat(_tmpB5,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmpB8=0;_npop_handler(0U);return _tmpB8;}}}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 481
if(suppress)continue;{
void*t;
char _tmpB9=c;switch(_tmpB9){case 100U: _LL9: _LLA:
 goto _LLC;case 105U: _LLB: _LLC: {
# 486
struct Cyc_Core_Opt*_tmpBA=({struct Cyc_Core_Opt*_tmpC6=_cycalloc(sizeof(*_tmpC6));({struct Cyc_List_List*_tmp163=Cyc_Tcenv_lookup_type_vars(te);_tmpC6->v=_tmp163;});_tmpC6;});struct Cyc_Core_Opt*tvs=_tmpBA;
{struct Cyc_List_List*_tmpBB=lenmod;int _tmpBD;int _tmpBC;int _tmpBE;int _tmpBF;if(_tmpBB == 0){_LL34: _LL35:
 t=Cyc_Absyn_sint_type;goto _LL33;}else{if(((struct Cyc_List_List*)_tmpBB)->tl == 0){_LL36: _tmpBF=(int)_tmpBB->hd;if(_tmpBF == (int)'l'){_LL37: {int x=_tmpBF;
t=Cyc_Absyn_slong_type;goto _LL33;}}else{_LL38: _tmpBE=(int)_tmpBB->hd;if(_tmpBE == (int)'h'){_LL39: {int x=_tmpBE;
t=Cyc_Absyn_sshort_type;goto _LL33;}}else{goto _LL3C;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpBB)->tl)->tl == 0){_LL3A: _tmpBC=(int)_tmpBB->hd;_tmpBD=(int)(_tmpBB->tl)->hd;if(
_tmpBC == (int)'h' && _tmpBD == (int)'h'){_LL3B: {int x=_tmpBC;int y=_tmpBD;t=Cyc_Absyn_schar_type;goto _LL33;}}else{goto _LL3C;}}else{_LL3C: _LL3D:
# 493
({struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0U,({
struct _fat_ptr _tmp164=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp116.f1=_tmp164;});_tmp116;});struct Cyc_Int_pa_PrintArg_struct _tmpC3=({struct Cyc_Int_pa_PrintArg_struct _tmp115;_tmp115.tag=1U,_tmp115.f1=(unsigned long)((int)c);_tmp115;});void*_tmpC0[2U];_tmpC0[0]=& _tmpC2,_tmpC0[1]=& _tmpC3;({unsigned _tmp166=loc;struct _fat_ptr _tmp165=({const char*_tmpC1="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpC1,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp166,_tmp165,_tag_fat(_tmpC0,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmpC4=0;_npop_handler(0U);return _tmpC4;}}}}_LL33:;}
# 497
t=({void*_tmp169=t;void*_tmp168=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp167=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp169,_tmp168,_tmp167,Cyc_Absyn_false_type);});
typs=({struct Cyc_List_List*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->hd=t,_tmpC5->tl=typs;_tmpC5;});
goto _LL8;}case 117U: _LLD: _LLE:
 goto _LL10;case 111U: _LLF: _LL10:
 goto _LL12;case 120U: _LL11: _LL12:
 goto _LL14;case 88U: _LL13: _LL14: {
# 504
struct Cyc_Core_Opt*_tmpC7=({struct Cyc_Core_Opt*_tmpD3=_cycalloc(sizeof(*_tmpD3));({struct Cyc_List_List*_tmp16A=Cyc_Tcenv_lookup_type_vars(te);_tmpD3->v=_tmp16A;});_tmpD3;});struct Cyc_Core_Opt*tvs=_tmpC7;
{struct Cyc_List_List*_tmpC8=lenmod;int _tmpCA;int _tmpC9;int _tmpCB;int _tmpCC;if(_tmpC8 == 0){_LL3F: _LL40:
 t=Cyc_Absyn_uint_type;goto _LL3E;}else{if(((struct Cyc_List_List*)_tmpC8)->tl == 0){_LL41: _tmpCC=(int)_tmpC8->hd;if(_tmpCC == (int)'l'){_LL42: {int x=_tmpCC;
t=Cyc_Absyn_ulong_type;goto _LL3E;}}else{_LL43: _tmpCB=(int)_tmpC8->hd;if(_tmpCB == (int)'h'){_LL44: {int x=_tmpCB;
t=Cyc_Absyn_ushort_type;goto _LL3E;}}else{goto _LL47;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpC8)->tl)->tl == 0){_LL45: _tmpC9=(int)_tmpC8->hd;_tmpCA=(int)(_tmpC8->tl)->hd;if(
_tmpC9 == (int)'h' && _tmpCA == (int)'h'){_LL46: {int x=_tmpC9;int y=_tmpCA;t=Cyc_Absyn_uchar_type;goto _LL3E;}}else{goto _LL47;}}else{_LL47: _LL48:
# 511
({struct Cyc_String_pa_PrintArg_struct _tmpCF=({struct Cyc_String_pa_PrintArg_struct _tmp118;_tmp118.tag=0U,({
struct _fat_ptr _tmp16B=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp118.f1=_tmp16B;});_tmp118;});struct Cyc_Int_pa_PrintArg_struct _tmpD0=({struct Cyc_Int_pa_PrintArg_struct _tmp117;_tmp117.tag=1U,_tmp117.f1=(unsigned long)((int)c);_tmp117;});void*_tmpCD[2U];_tmpCD[0]=& _tmpCF,_tmpCD[1]=& _tmpD0;({unsigned _tmp16D=loc;struct _fat_ptr _tmp16C=({const char*_tmpCE="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpCE,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp16D,_tmp16C,_tag_fat(_tmpCD,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmpD1=0;_npop_handler(0U);return _tmpD1;}}}}_LL3E:;}
# 515
t=({void*_tmp170=t;void*_tmp16F=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp16E=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp170,_tmp16F,_tmp16E,Cyc_Absyn_false_type);});
typs=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=t,_tmpD2->tl=typs;_tmpD2;});
goto _LL8;}case 102U: _LL15: _LL16:
 goto _LL18;case 70U: _LL17: _LL18:
 goto _LL1A;case 101U: _LL19: _LL1A:
 goto _LL1C;case 69U: _LL1B: _LL1C:
 goto _LL1E;case 103U: _LL1D: _LL1E:
 goto _LL20;case 71U: _LL1F: _LL20:
 goto _LL22;case 97U: _LL21: _LL22:
 goto _LL24;case 65U: _LL23: _LL24: {
# 526
struct Cyc_Core_Opt*_tmpD4=({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_List_List*_tmp171=Cyc_Tcenv_lookup_type_vars(te);_tmpDD->v=_tmp171;});_tmpDD;});struct Cyc_Core_Opt*tvs=_tmpD4;
{struct Cyc_List_List*_tmpD5=lenmod;int _tmpD6;if(_tmpD5 == 0){_LL4A: _LL4B:
 t=Cyc_Absyn_float_type;goto _LL49;}else{if(((struct Cyc_List_List*)_tmpD5)->tl == 0){_LL4C: _tmpD6=(int)_tmpD5->hd;if(_tmpD6 == (int)'l'){_LL4D: {int x=_tmpD6;
# 530
t=Cyc_Absyn_double_type;goto _LL49;}}else{goto _LL4E;}}else{_LL4E: _LL4F:
# 532
({struct Cyc_String_pa_PrintArg_struct _tmpD9=({struct Cyc_String_pa_PrintArg_struct _tmp11A;_tmp11A.tag=0U,({
struct _fat_ptr _tmp172=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp11A.f1=_tmp172;});_tmp11A;});struct Cyc_Int_pa_PrintArg_struct _tmpDA=({struct Cyc_Int_pa_PrintArg_struct _tmp119;_tmp119.tag=1U,_tmp119.f1=(unsigned long)((int)c);_tmp119;});void*_tmpD7[2U];_tmpD7[0]=& _tmpD9,_tmpD7[1]=& _tmpDA;({unsigned _tmp174=loc;struct _fat_ptr _tmp173=({const char*_tmpD8="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpD8,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp174,_tmp173,_tag_fat(_tmpD7,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmpDB=0;_npop_handler(0U);return _tmpDB;}}}_LL49:;}
# 536
t=({void*_tmp177=t;void*_tmp176=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp175=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp177,_tmp176,_tmp175,Cyc_Absyn_false_type);});
typs=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=t,_tmpDC->tl=typs;_tmpDC;});
goto _LL8;}case 99U: _LL25: _LL26: {
# 541
struct Cyc_Core_Opt*_tmpDE=({struct Cyc_Core_Opt*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct Cyc_List_List*_tmp178=Cyc_Tcenv_lookup_type_vars(te);_tmpE0->v=_tmp178;});_tmpE0;});struct Cyc_Core_Opt*tvs=_tmpDE;
void*ptr;
if(!isCproto)
ptr=({void*_tmp17B=Cyc_Absyn_char_type;void*_tmp17A=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp179=
Cyc_Absyn_empty_tqual(0U);
# 544
Cyc_Absyn_fatptr_type(_tmp17B,_tmp17A,_tmp179,Cyc_Absyn_false_type);});else{
# 548
ptr=({void*_tmp17E=Cyc_Absyn_char_type;void*_tmp17D=
Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);
# 548
struct Cyc_Absyn_Tqual _tmp17C=
# 550
Cyc_Absyn_empty_tqual(0U);
# 548
Cyc_Absyn_at_type(_tmp17E,_tmp17D,_tmp17C,Cyc_Absyn_false_type);});}
# 551
typs=({struct Cyc_List_List*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->hd=ptr,_tmpDF->tl=typs;_tmpDF;});
goto _LL8;}case 115U: _LL27: _LL28: {
# 554
struct Cyc_Core_Opt*_tmpE1=({struct Cyc_Core_Opt*_tmpE3=_cycalloc(sizeof(*_tmpE3));({struct Cyc_List_List*_tmp17F=Cyc_Tcenv_lookup_type_vars(te);_tmpE3->v=_tmp17F;});_tmpE3;});struct Cyc_Core_Opt*tvs=_tmpE1;
# 556
void*ptr;
if(!isCproto)
ptr=({void*_tmp182=Cyc_Absyn_char_type;void*_tmp181=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp180=
Cyc_Absyn_empty_tqual(0U);
# 558
Cyc_Absyn_fatptr_type(_tmp182,_tmp181,_tmp180,Cyc_Absyn_false_type);});else{
# 561
ptr=({void*_tmp185=Cyc_Absyn_char_type;void*_tmp184=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp183=
Cyc_Absyn_empty_tqual(0U);
# 561
Cyc_Absyn_at_type(_tmp185,_tmp184,_tmp183,Cyc_Absyn_true_type);});}
# 563
typs=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=ptr,_tmpE2->tl=typs;_tmpE2;});
goto _LL8;}case 91U: _LL29: _LL2A:
 goto _LL2C;case 112U: _LL2B: _LL2C:
# 567
({struct Cyc_Int_pa_PrintArg_struct _tmpE6=({struct Cyc_Int_pa_PrintArg_struct _tmp11B;_tmp11B.tag=1U,_tmp11B.f1=(unsigned long)((int)c);_tmp11B;});void*_tmpE4[1U];_tmpE4[0]=& _tmpE6;({unsigned _tmp187=loc;struct _fat_ptr _tmp186=({const char*_tmpE5="%%%c is not supported";_tag_fat(_tmpE5,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp187,_tmp186,_tag_fat(_tmpE4,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmpE7=0;_npop_handler(0U);return _tmpE7;}case 110U: _LL2D: _LL2E: {
# 570
struct Cyc_Core_Opt*_tmpE8=({struct Cyc_Core_Opt*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct Cyc_List_List*_tmp188=Cyc_Tcenv_lookup_type_vars(te);_tmpF4->v=_tmp188;});_tmpF4;});struct Cyc_Core_Opt*tvs=_tmpE8;
{struct Cyc_List_List*_tmpE9=lenmod;int _tmpEB;int _tmpEA;int _tmpEC;int _tmpED;if(_tmpE9 == 0){_LL51: _LL52:
 t=Cyc_Absyn_sint_type;goto _LL50;}else{if(((struct Cyc_List_List*)_tmpE9)->tl == 0){_LL53: _tmpED=(int)_tmpE9->hd;if(_tmpED == (int)'l'){_LL54: {int x=_tmpED;
t=Cyc_Absyn_ulong_type;goto _LL50;}}else{_LL55: _tmpEC=(int)_tmpE9->hd;if(_tmpEC == (int)'h'){_LL56: {int x=_tmpEC;
t=Cyc_Absyn_sshort_type;goto _LL50;}}else{goto _LL59;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)_tmpE9)->tl)->tl == 0){_LL57: _tmpEA=(int)_tmpE9->hd;_tmpEB=(int)(_tmpE9->tl)->hd;if(
_tmpEA == (int)'h' && _tmpEB == (int)'h'){_LL58: {int x=_tmpEA;int y=_tmpEB;t=Cyc_Absyn_schar_type;goto _LL50;}}else{goto _LL59;}}else{_LL59: _LL5A:
# 577
({struct Cyc_String_pa_PrintArg_struct _tmpF0=({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0U,({
struct _fat_ptr _tmp189=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp11D.f1=_tmp189;});_tmp11D;});struct Cyc_Int_pa_PrintArg_struct _tmpF1=({struct Cyc_Int_pa_PrintArg_struct _tmp11C;_tmp11C.tag=1U,_tmp11C.f1=(unsigned long)((int)c);_tmp11C;});void*_tmpEE[2U];_tmpEE[0]=& _tmpF0,_tmpEE[1]=& _tmpF1;({unsigned _tmp18B=loc;struct _fat_ptr _tmp18A=({const char*_tmpEF="length modifier '%s' is not allowed with %%%c";_tag_fat(_tmpEF,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp18B,_tmp18A,_tag_fat(_tmpEE,sizeof(void*),2U));});});{
struct Cyc_List_List*_tmpF2=0;_npop_handler(0U);return _tmpF2;}}}}_LL50:;}
# 581
t=({void*_tmp18E=t;void*_tmp18D=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,tvs);struct Cyc_Absyn_Tqual _tmp18C=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp18E,_tmp18D,_tmp18C,Cyc_Absyn_false_type);});
typs=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->hd=t,_tmpF3->tl=typs;_tmpF3;});
goto _LL8;}case 37U: _LL2F: _LL30:
# 585
 if(suppress){
({void*_tmpF5=0U;({unsigned _tmp190=loc;struct _fat_ptr _tmp18F=({const char*_tmpF6="Assignment suppression (*) is not allowed with %%%%";_tag_fat(_tmpF6,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp190,_tmp18F,_tag_fat(_tmpF5,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpF7=0;_npop_handler(0U);return _tmpF7;}}
# 589
if(width != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpFA=({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0U,({struct _fat_ptr _tmp191=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(width));_tmp11E.f1=_tmp191;});_tmp11E;});void*_tmpF8[1U];_tmpF8[0]=& _tmpFA;({unsigned _tmp193=loc;struct _fat_ptr _tmp192=({const char*_tmpF9="width '%s' not allowed with %%%%";_tag_fat(_tmpF9,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp193,_tmp192,_tag_fat(_tmpF8,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmpFB=0;_npop_handler(0U);return _tmpFB;}}
# 593
if(lenmod != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpFE=({struct Cyc_String_pa_PrintArg_struct _tmp11F;_tmp11F.tag=0U,({struct _fat_ptr _tmp194=(struct _fat_ptr)((struct _fat_ptr)Cyc_implode(lenmod));_tmp11F.f1=_tmp194;});_tmp11F;});void*_tmpFC[1U];_tmpFC[0]=& _tmpFE;({unsigned _tmp196=loc;struct _fat_ptr _tmp195=({const char*_tmpFD="length modifier '%s' not allowed with %%%%";_tag_fat(_tmpFD,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp196,_tmp195,_tag_fat(_tmpFC,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmpFF=0;_npop_handler(0U);return _tmpFF;}}
# 597
goto _LL8;default: _LL31: _LL32: {
# 599
struct Cyc_List_List*_tmp100=0;_npop_handler(0U);return _tmp100;}}_LL8:;}}}}}
# 465
;_pop_region();}
# 603
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(typs);}

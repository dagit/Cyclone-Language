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
# 86 "list.h"
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 917 "absyn.h"
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 942
extern void*Cyc_Absyn_fat_bound_type;
# 946
void*Cyc_Absyn_bounds_one();
# 29 "warn.h"
void Cyc_Warn_warn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 35
void Cyc_Warn_err(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RType(void*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple10{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 64
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 68
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 100
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 106
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 120
struct _fat_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r);
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_RgnOrder_RgnPO;
# 40 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 107
void*Cyc_Tcutil_compress(void*);
# 140
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 174
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 269
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 287
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 8 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
struct Cyc_AP_T*Cyc_AP_fromint(long x);
# 14
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x);
# 16
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned val;};struct _union_Node_NParamNumelts{int tag;unsigned val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 61 "pratt-ap.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
# 63
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NType(void*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NReturn();struct Cyc_Pratt_Graph;
# 76
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph();
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*);
# 81
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,struct Cyc_AP_T*a);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 64 "string.h"
struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 34 "relations-ap.cyc"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned i){return({union Cyc_Relations_RelnOp _tmpE4;(_tmpE4.RConst).tag=1U,(_tmpE4.RConst).val=i;_tmpE4;});}
union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*v){return({union Cyc_Relations_RelnOp _tmpE5;(_tmpE5.RVar).tag=2U,(_tmpE5.RVar).val=v;_tmpE5;});}
union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*v){return({union Cyc_Relations_RelnOp _tmpE6;(_tmpE6.RNumelts).tag=3U,(_tmpE6.RNumelts).val=v;_tmpE6;});}
union Cyc_Relations_RelnOp Cyc_Relations_RType(void*t){return({union Cyc_Relations_RelnOp _tmpE7;(_tmpE7.RType).tag=4U,({void*_tmpFC=Cyc_Tcutil_compress(t);(_tmpE7.RType).val=_tmpFC;});_tmpE7;});}
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned i){return({union Cyc_Relations_RelnOp _tmpE8;(_tmpE8.RParam).tag=5U,(_tmpE8.RParam).val=i;_tmpE8;});}
union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned i){return({union Cyc_Relations_RelnOp _tmpE9;(_tmpE9.RParamNumelts).tag=6U,(_tmpE9.RParamNumelts).val=i;_tmpE9;});}
union Cyc_Relations_RelnOp Cyc_Relations_RReturn(){return({union Cyc_Relations_RelnOp _tmpEA;(_tmpEA.RReturn).tag=7U,(_tmpEA.RReturn).val=0U;_tmpEA;});}struct _tuple12{union Cyc_Relations_RelnOp f1;union Cyc_Relations_RelnOp f2;};
# 42
static int Cyc_Relations_same_relop(union Cyc_Relations_RelnOp r1,union Cyc_Relations_RelnOp r2){
struct _tuple12 _tmp0=({struct _tuple12 _tmpEB;_tmpEB.f1=r1,_tmpEB.f2=r2;_tmpEB;});struct _tuple12 _tmp1=_tmp0;unsigned _tmpD;unsigned _tmpC;unsigned _tmpB;unsigned _tmpA;void*_tmp9;void*_tmp8;struct Cyc_Absyn_Vardecl*_tmp7;struct Cyc_Absyn_Vardecl*_tmp6;struct Cyc_Absyn_Vardecl*_tmp5;struct Cyc_Absyn_Vardecl*_tmp4;unsigned _tmp3;unsigned _tmp2;switch(((_tmp1.f1).RParamNumelts).tag){case 1U: if(((_tmp1.f2).RConst).tag == 1){_LL1: _tmp3=((_tmp1.f1).RConst).val;_tmp2=((_tmp1.f2).RConst).val;_LL2:
 return _tmp3 == _tmp2;}else{goto _LLF;}case 2U: if(((_tmp1.f2).RVar).tag == 2){_LL3: _tmp5=((_tmp1.f1).RVar).val;_tmp4=((_tmp1.f2).RVar).val;_LL4:
 return _tmp5 == _tmp4;}else{goto _LLF;}case 3U: if(((_tmp1.f2).RNumelts).tag == 3){_LL5: _tmp7=((_tmp1.f1).RNumelts).val;_tmp6=((_tmp1.f2).RNumelts).val;_LL6:
 return _tmp7 == _tmp6;}else{goto _LLF;}case 4U: if(((_tmp1.f2).RType).tag == 4){_LL7: _tmp9=((_tmp1.f1).RType).val;_tmp8=((_tmp1.f2).RType).val;_LL8:
 return Cyc_Unify_unify(_tmp9,_tmp8);}else{goto _LLF;}case 5U: if(((_tmp1.f2).RParam).tag == 5){_LL9: _tmpB=((_tmp1.f1).RParam).val;_tmpA=((_tmp1.f2).RParam).val;_LLA:
 return _tmpB == _tmpA;}else{goto _LLF;}case 6U: if(((_tmp1.f2).RParamNumelts).tag == 6){_LLB: _tmpD=((_tmp1.f1).RParamNumelts).val;_tmpC=((_tmp1.f2).RParamNumelts).val;_LLC:
 return _tmpD == _tmpC;}else{goto _LLF;}default: if(((_tmp1.f2).RReturn).tag == 7){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
 return 0;}}_LL0:;}
# 55
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation relation,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns){
# 59
{struct Cyc_List_List*_tmpE=relns;for(0;_tmpE != 0;_tmpE=_tmpE->tl){
struct Cyc_Relations_Reln*_tmpF=(struct Cyc_Relations_Reln*)_tmpE->hd;
if((Cyc_Relations_same_relop(_tmpF->rop1,rop1)&&(int)_tmpF->relation == (int)relation)&&
 Cyc_Relations_same_relop(_tmpF->rop2,rop2))return relns;}}
# 64
return({struct Cyc_List_List*_tmp11=_region_malloc(rgn,sizeof(*_tmp11));({struct Cyc_Relations_Reln*_tmpFD=({struct Cyc_Relations_Reln*_tmp10=_region_malloc(rgn,sizeof(*_tmp10));_tmp10->rop1=rop1,_tmp10->relation=relation,_tmp10->rop2=rop2;_tmp10;});_tmp11->hd=_tmpFD;}),_tmp11->tl=relns;_tmp11;});}
# 67
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns){
# 70
union Cyc_Relations_RelnOp rop_old=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop_new=Cyc_Relations_RConst(0U);
if(!Cyc_Relations_exp2relnop(e_old,& rop_old))return relns;
if(!Cyc_Relations_exp2relnop(e_new,& rop_new)){
({void*_tmp12=0U;({unsigned _tmpFF=e_new->loc;struct _fat_ptr _tmpFE=({const char*_tmp13="Could not construct relation";_tag_fat(_tmp13,sizeof(char),29U);});Cyc_Warn_warn(_tmpFF,_tmpFE,_tag_fat(_tmp12,sizeof(void*),0U));});});
return relns;}
# 77
{struct Cyc_List_List*_tmp14=relns;for(0;_tmp14 != 0;_tmp14=_tmp14->tl){
struct Cyc_Relations_Reln*_tmp15=(struct Cyc_Relations_Reln*)_tmp14->hd;
union Cyc_Relations_RelnOp _tmp16=_tmp15->rop1;
union Cyc_Relations_RelnOp _tmp17=_tmp15->rop2;
int addIt=0;
if(Cyc_Relations_same_relop(_tmp16,rop_old)){
addIt=1;
_tmp16=rop_new;}
# 86
if(Cyc_Relations_same_relop(_tmp17,rop_old)){
addIt=1;
_tmp17=rop_new;}
# 90
if(addIt)
relns=({struct Cyc_List_List*_tmp19=_region_malloc(rgn,sizeof(*_tmp19));({struct Cyc_Relations_Reln*_tmp100=({struct Cyc_Relations_Reln*_tmp18=_region_malloc(rgn,sizeof(*_tmp18));_tmp18->rop1=_tmp16,_tmp18->relation=_tmp15->relation,_tmp18->rop2=_tmp17;_tmp18;});_tmp19->hd=_tmp100;}),_tmp19->tl=relns;_tmp19;});}}
# 93
return relns;}
# 96
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 100
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_Relations_Reln*_tmp1A=(struct Cyc_Relations_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp1B=r2s;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){
struct Cyc_Relations_Reln*_tmp1C=(struct Cyc_Relations_Reln*)_tmp1B->hd;
if(_tmp1A == _tmp1C ||(Cyc_Relations_same_relop(_tmp1A->rop1,_tmp1C->rop1)&&(int)_tmp1A->relation == (int)_tmp1C->relation)&&
# 107
 Cyc_Relations_same_relop(_tmp1A->rop2,_tmp1C->rop2)){
found=1;
break;}}}
# 112
if(!found)
return 0;}
# 115
return 1;}
# 118
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp1D=r1s;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
struct Cyc_Relations_Reln*_tmp1E=(struct Cyc_Relations_Reln*)_tmp1D->hd;
int found=0;
{struct Cyc_List_List*_tmp1F=r2s;for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){
struct Cyc_Relations_Reln*_tmp20=(struct Cyc_Relations_Reln*)_tmp1F->hd;
if(_tmp1E == _tmp20 ||(Cyc_Relations_same_relop(_tmp1E->rop1,_tmp20->rop1)&&(int)_tmp1E->relation == (int)_tmp20->relation)&&
# 129
 Cyc_Relations_same_relop(_tmp1E->rop2,_tmp20->rop2)){
res=({struct Cyc_List_List*_tmp21=_region_malloc(r,sizeof(*_tmp21));_tmp21->hd=_tmp1E,_tmp21->tl=res;_tmp21;});
found=1;
break;}}}
# 135
if(!found)diff=1;}}
# 137
if(!diff)res=r1s;
return res;};}
# 141
static int Cyc_Relations_rop_contains_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmp22=r;struct Cyc_Absyn_Vardecl*_tmp24;struct Cyc_Absyn_Vardecl*_tmp23;switch((_tmp22.RReturn).tag){case 2U: _LL1: _tmp23=(_tmp22.RVar).val;_LL2:
 return v == _tmp23;case 3U: _LL3: _tmp24=(_tmp22.RNumelts).val;_LL4:
 return v == _tmp24;case 4U: _LL5: _LL6:
 goto _LL8;case 5U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
 return 0;}_LL0:;}
# 153
union Cyc_Relations_RelnOp Cyc_Relations_subst_rop_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
union Cyc_Relations_RelnOp _tmp25=r;struct Cyc_Absyn_Vardecl*_tmp27;struct Cyc_Absyn_Vardecl*_tmp26;switch((_tmp25.RReturn).tag){case 2U: _LL1: _tmp26=(_tmp25.RVar).val;_LL2:
 return v == _tmp26?new_rop: r;case 3U: _LL3: _tmp27=(_tmp25.RNumelts).val;_LL4:
 return v == _tmp27?new_rop: r;case 4U: _LL5: _LL6:
 goto _LL8;case 5U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
 return r;}_LL0:;}
# 165
struct Cyc_Relations_Reln*Cyc_Relations_substitute_rop_var(struct _RegionHandle*rgn,struct Cyc_Relations_Reln*reln,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
# 167
return({struct Cyc_Relations_Reln*_tmp28=_region_malloc(rgn,sizeof(*_tmp28));({union Cyc_Relations_RelnOp _tmp102=Cyc_Relations_subst_rop_var(reln->rop1,v,new_rop);_tmp28->rop1=_tmp102;}),_tmp28->relation=reln->relation,({
union Cyc_Relations_RelnOp _tmp101=Cyc_Relations_subst_rop_var(reln->rop2,v,new_rop);_tmp28->rop2=_tmp101;});_tmp28;});}
# 170
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found && p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp29=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp29->rop1,v)|| Cyc_Relations_rop_contains_var(_tmp29->rop2,v)){
found=1;
break;}}
# 180
if(!found)return rs;{
# 182
struct Cyc_Absyn_Tvar*_tmp2A=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
union Cyc_Relations_RelnOp _tmp2B=Cyc_Relations_RType(Cyc_Absyn_var_type(_tmp2A));
struct Cyc_List_List*_tmp2C=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp2D=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp2D->rop1,v)|| Cyc_Relations_rop_contains_var(_tmp2D->rop2,v))
_tmp2C=({struct Cyc_List_List*_tmp2E=_region_malloc(rgn,sizeof(*_tmp2E));({struct Cyc_Relations_Reln*_tmp103=Cyc_Relations_substitute_rop_var(rgn,_tmp2D,v,_tmp2B);_tmp2E->hd=_tmp103;}),_tmp2E->tl=_tmp2C;_tmp2E;});else{
# 190
_tmp2C=({struct Cyc_List_List*_tmp2F=_region_malloc(rgn,sizeof(*_tmp2F));_tmp2F->hd=_tmp2D,_tmp2F->tl=_tmp2C;_tmp2F;});}}
# 193
return _tmp2C;};}
# 199
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p){
# 202
RETRY:
 if(e->topt != 0){
void*_tmp30=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp31=_tmp30;struct Cyc_Absyn_Exp*_tmp33;void*_tmp32;switch(*((int*)_tmp31)){case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2 != 0){_LL1: _tmp32=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2)->hd;_LL2:
# 206
({union Cyc_Relations_RelnOp _tmp104=Cyc_Relations_RType(_tmp32);*p=_tmp104;});return 1;
goto _LL0;}else{goto _LL5;}}else{goto _LL5;}case 9U: _LL3: _tmp33=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp31)->f1;_LL4:
# 209
 e=_tmp33;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 214
{void*_tmp34=e->r;void*_tmp35=_tmp34;void*_tmp3F;void*_tmp3E;int _tmp3D;void*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;switch(*((int*)_tmp35)){case 14U: _LL8: _tmp3B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp35)->f2;_LL9:
 return Cyc_Relations_exp2relnop(_tmp3B,p);case 1U: _LLA: _tmp3C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35)->f1;_LLB: {
# 217
struct Cyc_Absyn_Vardecl*_tmp36=Cyc_Tcutil_nonesc_vardecl(_tmp3C);
if(_tmp36 != 0){
({union Cyc_Relations_RelnOp _tmp105=Cyc_Relations_RVar(_tmp36);*p=_tmp105;});
return 1;}
# 222
goto _LL7;}case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35)->f1).Int_c).tag == 5){_LLC: _tmp3D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35)->f1).Int_c).val).f2;_LLD:
# 224
({union Cyc_Relations_RelnOp _tmp106=Cyc_Relations_RConst((unsigned)_tmp3D);*p=_tmp106;});
return 1;}else{goto _LL12;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f2 != 0){if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f2)->hd)->r)->tag == 1U){_LLE: _tmp3E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f2)->hd)->r)->f1;_LLF: {
# 227
struct Cyc_Absyn_Vardecl*_tmp37=Cyc_Tcutil_nonesc_vardecl(_tmp3E);
if(_tmp37 != 0){
({union Cyc_Relations_RelnOp _tmp107=Cyc_Relations_RNumelts(_tmp37);*p=_tmp107;});
return 1;}
# 232
goto _LL7;}}else{goto _LL12;}}else{goto _LL12;}}else{goto _LL12;}case 39U: _LL10: _tmp3F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp35)->f1;_LL11:
# 234
{void*_tmp38=Cyc_Tcutil_compress(_tmp3F);void*_tmp39=_tmp38;struct Cyc_Absyn_Exp*_tmp3A;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp39)->tag == 9U){_LL15: _tmp3A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp39)->f1;_LL16:
# 236
 e=_tmp3A;
goto RETRY;}else{_LL17: _LL18:
# 239
({union Cyc_Relations_RelnOp _tmp108=Cyc_Relations_RType(_tmp3F);*p=_tmp108;});
return 1;}_LL14:;}
# 242
goto _LL7;default: _LL12: _LL13:
 goto _LL7;}_LL7:;}
# 246
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmp40=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmp41=_tmp40;unsigned _tmp43;int _tmp42;_LL1A: _tmp43=_tmp41.f1;_tmp42=_tmp41.f2;_LL1B:;
if(_tmp42){
({union Cyc_Relations_RelnOp _tmp109=Cyc_Relations_RConst(_tmp43);*p=_tmp109;});
return 1;}}
# 253
return 0;}
# 256
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp44=e->r;void*_tmp45=_tmp44;void*_tmp47;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp45)->tag == 1U){_LL1: _tmp47=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp45)->f1;_LL2: {
# 259
struct Cyc_Absyn_Vardecl*_tmp46=Cyc_Tcutil_nonesc_vardecl(_tmp47);
if(_tmp46 != 0)
return Cyc_Relations_reln_kill_var(rgn,r,_tmp46);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 267
struct Cyc_Relations_Reln*Cyc_Relations_copy_reln(struct _RegionHandle*r2,struct Cyc_Relations_Reln*r){
return({struct Cyc_Relations_Reln*_tmp48=_region_malloc(r2,sizeof(*_tmp48));*_tmp48=*r;_tmp48;});}
# 271
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*(*f)(struct _RegionHandle*,struct Cyc_Relations_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_Relations_copy_reln,r2,relns);}
# 275
int Cyc_Relations_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0 && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_Relations_Reln*_tmp49=(struct Cyc_Relations_Reln*)r1->hd;
struct Cyc_Relations_Reln*_tmp4A=(struct Cyc_Relations_Reln*)r2->hd;
if((!Cyc_Relations_same_relop(_tmp49->rop1,_tmp4A->rop1)||(int)_tmp49->relation != (int)_tmp4A->relation)|| !
# 281
Cyc_Relations_same_relop(_tmp49->rop2,_tmp4A->rop2))return 0;}
# 283
if(r1 != 0 || r2 != 0)return 0;else{
return 1;}}
# 287
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 291
if(v->escapes)return r;
# 294
r=Cyc_Relations_reln_kill_var(rgn,r,v);
# 299
{void*_tmp4B=e->r;void*_tmp4C=_tmp4B;struct Cyc_Absyn_Exp*_tmp52;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C)->tag == 34U){if((((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C)->f1).fat_result == 1){_LL1: _tmp52=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C)->f1).num_elts;_LL2:
# 302
 malloc_loop: {
void*_tmp4D=_tmp52->r;void*_tmp4E=_tmp4D;void*_tmp51;struct Cyc_Absyn_Exp*_tmp50;switch(*((int*)_tmp4E)){case 14U: _LL6: _tmp50=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LL7:
 _tmp52=_tmp50;goto malloc_loop;case 1U: _LL8: _tmp51=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_LL9: {
# 306
struct Cyc_Absyn_Vardecl*_tmp4F=Cyc_Tcutil_nonesc_vardecl(_tmp51);
if(_tmp4F != 0)
return({struct _RegionHandle*_tmp10C=rgn;union Cyc_Relations_RelnOp _tmp10B=Cyc_Relations_RVar(_tmp4F);union Cyc_Relations_RelnOp _tmp10A=Cyc_Relations_RNumelts(v);Cyc_Relations_add_relation(_tmp10C,_tmp10B,Cyc_Relations_Req,_tmp10A,r);});else{
return r;}}default: _LLA: _LLB:
 return r;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 313
 goto _LL0;}_LL0:;}
# 316
if(!Cyc_Tcutil_is_integral_type(v->type))return r;{
# 318
union Cyc_Relations_RelnOp eop=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e,& eop))
return({struct _RegionHandle*_tmp10F=rgn;union Cyc_Relations_RelnOp _tmp10E=Cyc_Relations_RVar(v);union Cyc_Relations_RelnOp _tmp10D=eop;Cyc_Relations_add_relation(_tmp10F,_tmp10E,Cyc_Relations_Req,_tmp10D,r);});
# 322
return r;};}
# 325
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp53=e1->r;void*_tmp54=_tmp53;void*_tmp56;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54)->tag == 1U){_LL1: _tmp56=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54)->f1;_LL2: {
# 328
struct Cyc_Absyn_Vardecl*_tmp55=Cyc_Tcutil_nonesc_vardecl(_tmp56);
if(_tmp55 != 0)
return Cyc_Relations_reln_assign_var(rgn,r,_tmp55,e2);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 337
static union Cyc_Pratt_Node Cyc_Relations_rop2node(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp57=r;unsigned _tmp5E;unsigned _tmp5D;void*_tmp5C;struct Cyc_Absyn_Vardecl*_tmp5B;struct Cyc_Absyn_Vardecl*_tmp5A;switch((_tmp57.RReturn).tag){case 2U: _LL1: _tmp5A=(_tmp57.RVar).val;_LL2:
 return Cyc_Pratt_NVar(_tmp5A);case 3U: _LL3: _tmp5B=(_tmp57.RNumelts).val;_LL4:
 return Cyc_Pratt_NNumelts(_tmp5B);case 4U: _LL5: _tmp5C=(_tmp57.RType).val;_LL6:
 return Cyc_Pratt_NType(_tmp5C);case 5U: _LL7: _tmp5D=(_tmp57.RParam).val;_LL8:
 return Cyc_Pratt_NParam(_tmp5D);case 6U: _LL9: _tmp5E=(_tmp57.RParamNumelts).val;_LLA:
 return Cyc_Pratt_NParamNumelts(_tmp5E);case 7U: _LLB: _LLC:
 return Cyc_Pratt_NReturn();default: _LLD: _LLE:
({void*_tmp58=0U;({struct _fat_ptr _tmp110=({const char*_tmp59="rop2node";_tag_fat(_tmp59,sizeof(char),9U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp110,_tag_fat(_tmp58,sizeof(void*),0U));});});}_LL0:;}
# 349
struct _fat_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmp5F=r;switch(_tmp5F){case Cyc_Relations_Req: _LL1: _LL2:
 return({const char*_tmp60="==";_tag_fat(_tmp60,sizeof(char),3U);});case Cyc_Relations_Rneq: _LL3: _LL4:
 return({const char*_tmp61="!=";_tag_fat(_tmp61,sizeof(char),3U);});case Cyc_Relations_Rlt: _LL5: _LL6:
 return({const char*_tmp62="<";_tag_fat(_tmp62,sizeof(char),2U);});case Cyc_Relations_Rlte: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA: return({const char*_tmp63="<=";_tag_fat(_tmp63,sizeof(char),3U);});}_LL0:;}
# 358
struct _fat_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp64=r;unsigned _tmp7B;unsigned _tmp7A;struct Cyc_Absyn_Vardecl*_tmp79;void*_tmp78;struct Cyc_Absyn_Vardecl*_tmp77;unsigned _tmp76;switch((_tmp64.RParamNumelts).tag){case 1U: _LL1: _tmp76=(_tmp64.RConst).val;_LL2:
 return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp67=({struct Cyc_Int_pa_PrintArg_struct _tmpEC;_tmpEC.tag=1U,_tmpEC.f1=(unsigned long)((int)_tmp76);_tmpEC;});void*_tmp65[1U];_tmp65[0]=& _tmp67;({struct _fat_ptr _tmp111=({const char*_tmp66="%d";_tag_fat(_tmp66,sizeof(char),3U);});Cyc_aprintf(_tmp111,_tag_fat(_tmp65,sizeof(void*),1U));});});case 2U: _LL3: _tmp77=(_tmp64.RVar).val;_LL4:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6A=({struct Cyc_String_pa_PrintArg_struct _tmpED;_tmpED.tag=0U,({struct _fat_ptr _tmp112=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp77->name));_tmpED.f1=_tmp112;});_tmpED;});void*_tmp68[1U];_tmp68[0]=& _tmp6A;({struct _fat_ptr _tmp113=({const char*_tmp69="%s";_tag_fat(_tmp69,sizeof(char),3U);});Cyc_aprintf(_tmp113,_tag_fat(_tmp68,sizeof(void*),1U));});});case 4U: _LL5: _tmp78=(_tmp64.RType).val;_LL6:
 return Cyc_Absynpp_typ2string(_tmp78);case 3U: _LL7: _tmp79=(_tmp64.RNumelts).val;_LL8:
 return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmpEE;_tmpEE.tag=0U,({struct _fat_ptr _tmp114=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp79->name));_tmpEE.f1=_tmp114;});_tmpEE;});void*_tmp6B[1U];_tmp6B[0]=& _tmp6D;({struct _fat_ptr _tmp115=({const char*_tmp6C="numelts(%s)";_tag_fat(_tmp6C,sizeof(char),12U);});Cyc_aprintf(_tmp115,_tag_fat(_tmp6B,sizeof(void*),1U));});});case 5U: _LL9: _tmp7A=(_tmp64.RParam).val;_LLA:
 return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp70=({struct Cyc_Int_pa_PrintArg_struct _tmpEF;_tmpEF.tag=1U,_tmpEF.f1=(unsigned long)((int)_tmp7A);_tmpEF;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({struct _fat_ptr _tmp116=({const char*_tmp6F="param(%d)";_tag_fat(_tmp6F,sizeof(char),10U);});Cyc_aprintf(_tmp116,_tag_fat(_tmp6E,sizeof(void*),1U));});});case 6U: _LLB: _tmp7B=(_tmp64.RParamNumelts).val;_LLC:
 return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp73=({struct Cyc_Int_pa_PrintArg_struct _tmpF0;_tmpF0.tag=1U,_tmpF0.f1=(unsigned long)((int)_tmp7B);_tmpF0;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({struct _fat_ptr _tmp117=({const char*_tmp72="numelts(param(%d))";_tag_fat(_tmp72,sizeof(char),19U);});Cyc_aprintf(_tmp117,_tag_fat(_tmp71,sizeof(void*),1U));});});default: _LLD: _LLE:
 return(struct _fat_ptr)({void*_tmp74=0U;({struct _fat_ptr _tmp118=({const char*_tmp75="return_value";_tag_fat(_tmp75,sizeof(char),13U);});Cyc_aprintf(_tmp118,_tag_fat(_tmp74,sizeof(void*),0U));});});}_LL0:;}
# 370
struct _fat_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r){
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmpF3;_tmpF3.tag=0U,({struct _fat_ptr _tmp119=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_rop2string(r->rop1));_tmpF3.f1=_tmp119;});_tmpF3;});struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmpF2;_tmpF2.tag=0U,({struct _fat_ptr _tmp11A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_relation2string(r->relation));_tmpF2.f1=_tmp11A;});_tmpF2;});struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0U,({
struct _fat_ptr _tmp11B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_rop2string(r->rop2));_tmpF1.f1=_tmp11B;});_tmpF1;});void*_tmp7C[3U];_tmp7C[0]=& _tmp7E,_tmp7C[1]=& _tmp7F,_tmp7C[2]=& _tmp80;({struct _fat_ptr _tmp11C=({const char*_tmp7D="%s %s %s";_tag_fat(_tmp7D,sizeof(char),9U);});Cyc_aprintf(_tmp11C,_tag_fat(_tmp7C,sizeof(void*),3U));});});}
# 375
struct _fat_ptr Cyc_Relations_relns2string(struct Cyc_List_List*rs){
if(rs == 0)return({const char*_tmp81="true";_tag_fat(_tmp81,sizeof(char),5U);});{
struct Cyc_List_List*ss=0;
for(0;rs != 0;rs=rs->tl){
ss=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));({struct _fat_ptr*_tmp11E=({struct _fat_ptr*_tmp82=_cycalloc(sizeof(*_tmp82));({struct _fat_ptr _tmp11D=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)rs->hd);*_tmp82=_tmp11D;});_tmp82;});_tmp83->hd=_tmp11E;}),_tmp83->tl=ss;_tmp83;});
if(rs->tl != 0)ss=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*_tmp86));({struct _fat_ptr*_tmp120=({struct _fat_ptr*_tmp85=_cycalloc(sizeof(*_tmp85));({struct _fat_ptr _tmp11F=({const char*_tmp84=" && ";_tag_fat(_tmp84,sizeof(char),5U);});*_tmp85=_tmp11F;});_tmp85;});_tmp86->hd=_tmp120;}),_tmp86->tl=ss;_tmp86;});}
# 382
return(struct _fat_ptr)Cyc_strconcat_l(ss);};}
# 385
void Cyc_Relations_print_relns(struct Cyc___cycFILE*f,struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0U,({struct _fat_ptr _tmp121=(struct _fat_ptr)((struct _fat_ptr)Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)r->hd));_tmpF4.f1=_tmp121;});_tmpF4;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({struct Cyc___cycFILE*_tmp123=f;struct _fat_ptr _tmp122=({const char*_tmp88="%s";_tag_fat(_tmp88,sizeof(char),3U);});Cyc_fprintf(_tmp123,_tmp122,_tag_fat(_tmp87,sizeof(void*),1U));});});
if(r->tl != 0)({void*_tmp8A=0U;({struct Cyc___cycFILE*_tmp125=f;struct _fat_ptr _tmp124=({const char*_tmp8B=",";_tag_fat(_tmp8B,sizeof(char),2U);});Cyc_fprintf(_tmp125,_tmp124,_tag_fat(_tmp8A,sizeof(void*),0U));});});}}
# 392
static union Cyc_Relations_RelnOp Cyc_Relations_reduce_relnop(union Cyc_Relations_RelnOp rop){
{union Cyc_Relations_RelnOp _tmp8C=rop;struct Cyc_Absyn_Vardecl*_tmp96;void*_tmp95;switch((_tmp8C.RNumelts).tag){case 4U: _LL1: _tmp95=(_tmp8C.RType).val;_LL2:
# 395
{void*_tmp8D=Cyc_Tcutil_compress(_tmp95);void*_tmp8E=_tmp8D;struct Cyc_Absyn_Exp*_tmp8F;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8E)->tag == 9U){_LL8: _tmp8F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8E)->f1;_LL9: {
# 397
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp8F,& rop2))return rop2;
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 402
goto _LL0;case 3U: _LL3: _tmp96=(_tmp8C.RNumelts).val;_LL4:
# 404
{void*_tmp90=Cyc_Tcutil_compress(_tmp96->type);void*_tmp91=_tmp90;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 3U){_LLD: _tmp94=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).bounds;_LLE:
# 408
 if(({void*_tmp126=_tmp94;_tmp126 == Cyc_Absyn_bounds_one();}))goto _LLC;{
struct Cyc_Absyn_Exp*_tmp92=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp94);
if(_tmp92 != 0){
struct Cyc_Absyn_Exp*_tmp93=_tmp92;
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp93,& rop2))return rop2;}
# 415
goto _LLC;};}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 418
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 421
return rop;}
# 426
static int Cyc_Relations_consist_relations(struct Cyc_Pratt_Graph*G,struct Cyc_List_List*rlns){
for(0;(unsigned)G && rlns != 0;rlns=rlns->tl){
struct Cyc_Relations_Reln*_tmp97=(struct Cyc_Relations_Reln*)rlns->hd;
({union Cyc_Relations_RelnOp _tmp127=Cyc_Relations_reduce_relnop(_tmp97->rop1);_tmp97->rop1=_tmp127;});
({union Cyc_Relations_RelnOp _tmp128=Cyc_Relations_reduce_relnop(_tmp97->rop2);_tmp97->rop2=_tmp128;});{
# 436
struct Cyc_Relations_Reln*_tmp98=_tmp97;union Cyc_Relations_RelnOp _tmpB2;enum Cyc_Relations_Relation _tmpB1;union Cyc_Relations_RelnOp _tmpB0;union Cyc_Relations_RelnOp _tmpAF;enum Cyc_Relations_Relation _tmpAE;unsigned _tmpAD;union Cyc_Relations_RelnOp _tmpAC;union Cyc_Relations_RelnOp _tmpAB;union Cyc_Relations_RelnOp _tmpAA;unsigned _tmpA9;unsigned _tmpA8;enum Cyc_Relations_Relation _tmpA7;union Cyc_Relations_RelnOp _tmpA6;unsigned _tmpA5;union Cyc_Relations_RelnOp _tmpA4;unsigned _tmpA3;enum Cyc_Relations_Relation _tmpA2;unsigned _tmpA1;if(((((struct Cyc_Relations_Reln*)_tmp98)->rop1).RConst).tag == 1){if(((((struct Cyc_Relations_Reln*)_tmp98)->rop2).RConst).tag == 1){_LL1: _tmpA3=((_tmp98->rop1).RConst).val;_tmpA2=_tmp98->relation;_tmpA1=((_tmp98->rop2).RConst).val;_LL2:
# 438
{enum Cyc_Relations_Relation _tmp99=_tmpA2;switch(_tmp99){case Cyc_Relations_Req: _LL10: _LL11:
 if(_tmpA3 != _tmpA1)return 0;goto _LLF;case Cyc_Relations_Rneq: _LL12: _LL13:
 if(_tmpA3 == _tmpA1)return 0;goto _LLF;case Cyc_Relations_Rlt: _LL14: _LL15:
 if(_tmpA3 >= _tmpA1)return 0;goto _LLF;case Cyc_Relations_Rlte: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19: if(_tmpA3 > _tmpA1)return 0;goto _LLF;}_LLF:;}
# 444
goto _LL0;}else{if(((struct Cyc_Relations_Reln*)_tmp98)->relation == Cyc_Relations_Rneq){_LL5: _tmpA5=((_tmp98->rop1).RConst).val;_tmpA4=_tmp98->rop2;_LL6: {
# 451
union Cyc_Pratt_Node _tmp9A=Cyc_Relations_rop2node(_tmpA4);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_tmp12C=G2;union Cyc_Pratt_Node _tmp12B=_tmp9A;union Cyc_Pratt_Node _tmp12A=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp12C,_tmp12B,_tmp12A,({struct Cyc_AP_T*_tmp129=Cyc_AP_fromint((long)_tmpA5);Cyc_AP_sub(_tmp129,Cyc_AP_one);}));});
G=({struct Cyc_Pratt_Graph*_tmp130=G;union Cyc_Pratt_Node _tmp12F=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp12E=_tmp9A;Cyc_Pratt_add_edge(_tmp130,_tmp12F,_tmp12E,Cyc_AP_neg(({struct Cyc_AP_T*_tmp12D=Cyc_AP_fromint((long)_tmpA5);Cyc_AP_add(_tmp12D,Cyc_AP_one);})));});
# 461
if(G != 0 && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}else{_LLB: _tmpA8=((_tmp98->rop1).RConst).val;_tmpA7=_tmp98->relation;_tmpA6=_tmp98->rop2;_LLC: {
# 495
union Cyc_Pratt_Node _tmp9E=Cyc_Relations_rop2node(_tmpA6);
if((int)_tmpA7 == (int)3U)_tmpA8=_tmpA8 + (unsigned)1;
G=({struct Cyc_Pratt_Graph*_tmp133=G;union Cyc_Pratt_Node _tmp132=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp131=_tmp9E;Cyc_Pratt_add_edge(_tmp133,_tmp132,_tmp131,Cyc_AP_neg(Cyc_AP_fromint((long)_tmpA8)));});
# 500
if((unsigned)G &&(int)_tmpA7 == (int)0U)
# 505
G=({struct Cyc_Pratt_Graph*_tmp136=G;union Cyc_Pratt_Node _tmp135=_tmp9E;union Cyc_Pratt_Node _tmp134=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp136,_tmp135,_tmp134,Cyc_AP_fromint((long)_tmpA8));});
# 512
goto _LL0;}}}}else{if(((struct Cyc_Relations_Reln*)_tmp98)->relation == Cyc_Relations_Rneq){if(((((struct Cyc_Relations_Reln*)_tmp98)->rop2).RConst).tag == 1){_LL3: _tmpAA=_tmp98->rop1;_tmpA9=((_tmp98->rop2).RConst).val;_LL4:
# 449
 _tmpA5=_tmpA9;_tmpA4=_tmpAA;goto _LL6;}else{_LL7: _tmpAC=_tmp98->rop1;_tmpAB=_tmp98->rop2;_LL8: {
# 468
union Cyc_Pratt_Node _tmp9B=Cyc_Relations_rop2node(_tmpAC);
union Cyc_Pratt_Node _tmp9C=Cyc_Relations_rop2node(_tmpAB);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_tmp139=G2;union Cyc_Pratt_Node _tmp138=_tmp9B;union Cyc_Pratt_Node _tmp137=_tmp9C;Cyc_Pratt_add_edge(_tmp139,_tmp138,_tmp137,Cyc_AP_neg(Cyc_AP_one));});
G=({struct Cyc_Pratt_Graph*_tmp13C=G;union Cyc_Pratt_Node _tmp13B=_tmp9C;union Cyc_Pratt_Node _tmp13A=_tmp9B;Cyc_Pratt_add_edge(_tmp13C,_tmp13B,_tmp13A,Cyc_AP_neg(Cyc_AP_one));});
if(G != 0 && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}}else{if(((((struct Cyc_Relations_Reln*)_tmp98)->rop2).RConst).tag == 1){_LL9: _tmpAF=_tmp98->rop1;_tmpAE=_tmp98->relation;_tmpAD=((_tmp98->rop2).RConst).val;_LLA: {
# 482
union Cyc_Pratt_Node _tmp9D=Cyc_Relations_rop2node(_tmpAF);
if((int)_tmpAE == (int)3U)_tmpAD=_tmpAD - (unsigned)1;
G=({struct Cyc_Pratt_Graph*_tmp13F=G;union Cyc_Pratt_Node _tmp13E=_tmp9D;union Cyc_Pratt_Node _tmp13D=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp13F,_tmp13E,_tmp13D,Cyc_AP_fromint((long)_tmpAD));});
# 486
if((unsigned)G &&(int)_tmpAE == (int)0U)
G=({struct Cyc_Pratt_Graph*_tmp142=G;union Cyc_Pratt_Node _tmp141=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp140=_tmp9D;Cyc_Pratt_add_edge(_tmp142,_tmp141,_tmp140,Cyc_AP_neg(Cyc_AP_fromint((long)_tmpAD)));});
goto _LL0;}}else{_LLD: _tmpB2=_tmp98->rop1;_tmpB1=_tmp98->relation;_tmpB0=_tmp98->rop2;_LLE: {
# 515
union Cyc_Pratt_Node _tmp9F=Cyc_Relations_rop2node(_tmpB2);
union Cyc_Pratt_Node _tmpA0=Cyc_Relations_rop2node(_tmpB0);
int i=(int)_tmpB1 == (int)3U?- 1: 0;
# 520
G=({struct Cyc_Pratt_Graph*_tmp145=G;union Cyc_Pratt_Node _tmp144=_tmp9F;union Cyc_Pratt_Node _tmp143=_tmpA0;Cyc_Pratt_add_edge(_tmp145,_tmp144,_tmp143,Cyc_AP_fromint(i));});
# 522
if((unsigned)G &&(int)_tmpB1 == (int)0U)
G=({struct Cyc_Pratt_Graph*_tmp148=G;union Cyc_Pratt_Node _tmp147=_tmpA0;union Cyc_Pratt_Node _tmp146=_tmp9F;Cyc_Pratt_add_edge(_tmp148,_tmp147,_tmp146,Cyc_AP_fromint(i));});
goto _LL0;}}}}_LL0:;};}
# 527
if(G != 0)return 1;else{return 0;}}
# 535
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns){
# 541
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
return Cyc_Relations_consist_relations(G,rlns);}
# 547
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmpB3=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmpB4=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->hd=_tmpB3,_tmpB5->tl=r1;_tmpB5;});
if(Cyc_Relations_consistent_relations(_tmpB4))return 0;}
# 553
return 1;}
# 559
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 561
enum Cyc_Absyn_Primop _tmpB6=p;switch(_tmpB6){case Cyc_Absyn_Eq: _LL1: _LL2:
 return({struct _tuple10 _tmpF5;_tmpF5.f1=e1,_tmpF5.f2=Cyc_Relations_Req,_tmpF5.f3=e2;_tmpF5;});case Cyc_Absyn_Neq: _LL3: _LL4:
 return({struct _tuple10 _tmpF6;_tmpF6.f1=e1,_tmpF6.f2=Cyc_Relations_Rneq,_tmpF6.f3=e2;_tmpF6;});case Cyc_Absyn_Lt: _LL5: _LL6:
 return({struct _tuple10 _tmpF7;_tmpF7.f1=e1,_tmpF7.f2=Cyc_Relations_Rlt,_tmpF7.f3=e2;_tmpF7;});case Cyc_Absyn_Lte: _LL7: _LL8:
 return({struct _tuple10 _tmpF8;_tmpF8.f1=e1,_tmpF8.f2=Cyc_Relations_Rlte,_tmpF8.f3=e2;_tmpF8;});case Cyc_Absyn_Gt: _LL9: _LLA:
 return({struct _tuple10 _tmpF9;_tmpF9.f1=e2,_tmpF9.f2=Cyc_Relations_Rlt,_tmpF9.f3=e1;_tmpF9;});case Cyc_Absyn_Gte: _LLB: _LLC:
 return({struct _tuple10 _tmpFA;_tmpFA.f1=e2,_tmpFA.f2=Cyc_Relations_Rlte,_tmpFA.f3=e1;_tmpFA;});default: _LLD: _LLE:
({void*_tmpB7=0U;({struct _fat_ptr _tmp149=({const char*_tmpB8="primop2relation";_tag_fat(_tmpB8,sizeof(char),16U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp149,_tag_fat(_tmpB7,sizeof(void*),0U));});});}_LL0:;}
# 573
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmpB9=r;switch(_tmpB9){case Cyc_Relations_Req: _LL1: _LL2:
 return Cyc_Relations_Rneq;case Cyc_Relations_Rneq: _LL3: _LL4:
 return Cyc_Relations_Req;case Cyc_Relations_Rlt: _LL5: _LL6:
 return Cyc_Relations_Rlte;case Cyc_Relations_Rlte: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA: return Cyc_Relations_Rlt;}_LL0:;}
# 582
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*r,struct Cyc_Relations_Reln*rln){
return({struct Cyc_Relations_Reln*_tmpBA=_region_malloc(r,sizeof(*_tmpBA));_tmpBA->rop1=rln->rop2,({enum Cyc_Relations_Relation _tmp14A=Cyc_Relations_flip_relation(rln->relation);_tmpBA->relation=_tmp14A;}),_tmpBA->rop2=rln->rop1;_tmpBA;});}
# 593
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 595
{void*_tmpBB=e->r;void*_tmpBC=_tmpBB;enum Cyc_Absyn_Primop _tmpD8;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;enum Cyc_Absyn_Primop _tmpD5;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD1;void*_tmpD0;struct Cyc_Absyn_Exp*_tmpCF;switch(*((int*)_tmpBC)){case 14U: _LL1: _tmpD0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBC)->f1;_tmpCF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBC)->f2;_LL2:
# 598
{void*_tmpBD=Cyc_Tcutil_compress(_tmpD0);void*_tmpBE=_tmpBD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE)->f1)->f2 == Cyc_Absyn_Int_sz){_LLC: _LLD:
 return Cyc_Relations_exp2relns(r,_tmpCF);}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _LLF:
 goto _LLB;}_LLB:;}
# 602
goto _LL0;case 7U: _LL3: _tmpD2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBC)->f1;_tmpD1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBC)->f2;_LL4:
 return({struct _RegionHandle*_tmp14C=r;struct Cyc_List_List*_tmp14B=Cyc_Relations_exp2relns(r,_tmpD2);((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp14C,_tmp14B,Cyc_Relations_exp2relns(r,_tmpD1));});case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2 != 0){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd)->r)->tag == 3U){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd)->r)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd)->r)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd)->r)->f2)->tl)->tl == 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl == 0){_LL5: _tmpD5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd)->r)->f1;_tmpD4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd)->r)->f2)->hd;_tmpD3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd)->r)->f2)->tl)->hd;_LL6:
# 605
{enum Cyc_Absyn_Primop _tmpBF=_tmpD5;switch(_tmpBF){case Cyc_Absyn_Eq: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Neq: _LL13: _LL14: goto _LL16;case Cyc_Absyn_Lt: _LL15: _LL16: goto _LL18;case Cyc_Absyn_Lte: _LL17: _LL18: goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A: goto _LL1C;case Cyc_Absyn_Gte: _LL1B: _LL1C: {
struct _tuple10 _tmpC0=Cyc_Relations_primop2relation(_tmpD4,_tmpD5,_tmpD3);struct _tuple10 _tmpC1=_tmpC0;struct Cyc_Absyn_Exp*_tmpC6;enum Cyc_Relations_Relation _tmpC5;struct Cyc_Absyn_Exp*_tmpC4;_LL20: _tmpC6=_tmpC1.f1;_tmpC5=_tmpC1.f2;_tmpC4=_tmpC1.f3;_LL21:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpC6,& rop1)&& Cyc_Relations_exp2relnop(_tmpC4,& rop2))
return({struct Cyc_List_List*_tmpC3=_region_malloc(r,sizeof(*_tmpC3));({struct Cyc_Relations_Reln*_tmp14E=({struct Cyc_Relations_Reln*_tmpC2=_region_malloc(r,sizeof(*_tmpC2));_tmpC2->rop1=rop2,({enum Cyc_Relations_Relation _tmp14D=Cyc_Relations_flip_relation(_tmpC5);_tmpC2->relation=_tmp14D;}),_tmpC2->rop2=rop1;_tmpC2;});_tmpC3->hd=_tmp14E;}),_tmpC3->tl=0;_tmpC3;});
goto _LL10;};}default: _LL1D: _LL1E:
 goto _LL10;}_LL10:;}
# 615
goto _LL0;}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl)->tl == 0){_LL7: _tmpD8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f1;_tmpD7=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->hd;_tmpD6=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBC)->f2)->tl)->hd;_LL8:
# 617
{enum Cyc_Absyn_Primop _tmpC7=_tmpD8;switch(_tmpC7){case Cyc_Absyn_Eq: _LL23: _LL24:
 goto _LL26;case Cyc_Absyn_Neq: _LL25: _LL26: goto _LL28;case Cyc_Absyn_Lt: _LL27: _LL28: goto _LL2A;case Cyc_Absyn_Lte: _LL29: _LL2A: goto _LL2C;case Cyc_Absyn_Gt: _LL2B: _LL2C: goto _LL2E;case Cyc_Absyn_Gte: _LL2D: _LL2E: {
struct _tuple10 _tmpC8=Cyc_Relations_primop2relation(_tmpD7,_tmpD8,_tmpD6);struct _tuple10 _tmpC9=_tmpC8;struct Cyc_Absyn_Exp*_tmpCE;enum Cyc_Relations_Relation _tmpCD;struct Cyc_Absyn_Exp*_tmpCC;_LL32: _tmpCE=_tmpC9.f1;_tmpCD=_tmpC9.f2;_tmpCC=_tmpC9.f3;_LL33:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpCE,& rop1)&& Cyc_Relations_exp2relnop(_tmpCC,& rop2))
return({struct Cyc_List_List*_tmpCB=_region_malloc(r,sizeof(*_tmpCB));({struct Cyc_Relations_Reln*_tmp14F=({struct Cyc_Relations_Reln*_tmpCA=_region_malloc(r,sizeof(*_tmpCA));_tmpCA->rop1=rop1,_tmpCA->relation=_tmpCD,_tmpCA->rop2=rop2;_tmpCA;});_tmpCB->hd=_tmp14F;}),_tmpCB->tl=0;_tmpCB;});
goto _LL22;};}default: _LL2F: _LL30:
 goto _LL22;}_LL22:;}
# 627
goto _LL0;}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}}default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 631
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmpD9=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmpDA=_tmpD9;unsigned _tmpE1;int _tmpE0;_LL35: _tmpE1=_tmpDA.f1;_tmpE0=_tmpDA.f2;_LL36:;
if(_tmpE0){
if((int)_tmpE1)return 0;else{
# 636
return({struct Cyc_List_List*_tmpDC=_region_malloc(r,sizeof(*_tmpDC));({struct Cyc_Relations_Reln*_tmp152=({struct Cyc_Relations_Reln*_tmpDB=_region_malloc(r,sizeof(*_tmpDB));({union Cyc_Relations_RelnOp _tmp151=Cyc_Relations_RConst(0U);_tmpDB->rop1=_tmp151;}),_tmpDB->relation=Cyc_Relations_Rlt,({union Cyc_Relations_RelnOp _tmp150=Cyc_Relations_RConst(0U);_tmpDB->rop2=_tmp150;});_tmpDB;});_tmpDC->hd=_tmp152;}),_tmpDC->tl=0;_tmpDC;});}}
# 638
({struct Cyc_String_pa_PrintArg_struct _tmpDF=({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0U,({
struct _fat_ptr _tmp153=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmpFB.f1=_tmp153;});_tmpFB;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({unsigned _tmp155=e->loc;struct _fat_ptr _tmp154=({const char*_tmpDE="unable to convert `%s' to static relation";_tag_fat(_tmpDE,sizeof(char),42U);});Cyc_Warn_err(_tmp155,_tmp154,_tag_fat(_tmpDD,sizeof(void*),1U));});});}
# 642
return({struct Cyc_List_List*_tmpE3=_region_malloc(r,sizeof(*_tmpE3));({struct Cyc_Relations_Reln*_tmp158=({struct Cyc_Relations_Reln*_tmpE2=_region_malloc(r,sizeof(*_tmpE2));({union Cyc_Relations_RelnOp _tmp157=Cyc_Relations_RConst(0U);_tmpE2->rop1=_tmp157;}),_tmpE2->relation=Cyc_Relations_Rlt,({union Cyc_Relations_RelnOp _tmp156=Cyc_Relations_RConst(0U);_tmpE2->rop2=_tmp156;});_tmpE2;});_tmpE3->hd=_tmp158;}),_tmpE3->tl=0;_tmpE3;});}

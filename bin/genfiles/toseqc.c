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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 978 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1048
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1060
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 269 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table;
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple1*Cyc_Toc_temp_var();extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 35 "toseqc.cyc"
enum Cyc_Toseqc_SideEffect{Cyc_Toseqc_Const =0U,Cyc_Toseqc_NoEffect =1U,Cyc_Toseqc_ExnEffect =2U,Cyc_Toseqc_AnyEffect =3U};struct _tuple11{enum Cyc_Toseqc_SideEffect f1;enum Cyc_Toseqc_SideEffect f2;};
# 48 "toseqc.cyc"
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_join_side_effect(enum Cyc_Toseqc_SideEffect e1,enum Cyc_Toseqc_SideEffect e2){
struct _tuple11 _tmp0=({struct _tuple11 _tmpCB;_tmpCB.f1=e1,_tmpCB.f2=e2;_tmpCB;});struct _tuple11 _tmp1=_tmp0;if(_tmp1.f1 == Cyc_Toseqc_AnyEffect){_LL1: _LL2:
 goto _LL4;}else{if(_tmp1.f2 == Cyc_Toseqc_AnyEffect){_LL3: _LL4:
 return Cyc_Toseqc_AnyEffect;}else{if(_tmp1.f1 == Cyc_Toseqc_ExnEffect){if(_tmp1.f2 == Cyc_Toseqc_ExnEffect){_LL5: _LL6:
# 53
 return Cyc_Toseqc_AnyEffect;}else{_LL7: _LL8:
 goto _LLA;}}else{if(_tmp1.f2 == Cyc_Toseqc_ExnEffect){_LL9: _LLA:
 return Cyc_Toseqc_ExnEffect;}else{if(_tmp1.f1 == Cyc_Toseqc_NoEffect){_LLB: _LLC:
 goto _LLE;}else{if(_tmp1.f2 == Cyc_Toseqc_NoEffect){_LLD: _LLE:
 return Cyc_Toseqc_NoEffect;}else{if(_tmp1.f1 == Cyc_Toseqc_Const){if(_tmp1.f2 == Cyc_Toseqc_Const){_LLF: _LL10:
 return Cyc_Toseqc_Const;}else{goto _LL11;}}else{_LL11: _LL12:
({void*_tmp2=0U;({struct _fat_ptr _tmpD2=({const char*_tmp3="join_side_effect";_tag_fat(_tmp3,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpD2,_tag_fat(_tmp2,sizeof(void*),0U));});});}}}}}}}_LL0:;}static char _tmp4[14U]="_get_fat_size";static char _tmp5[24U]="_get_zero_arr_size_char";static char _tmp6[25U]="_get_zero_arr_size_short";static char _tmp7[23U]="_get_zero_arr_size_int";static char _tmp8[25U]="_get_zero_arr_size_float";static char _tmp9[26U]="_get_zero_arr_size_double";static char _tmpA[30U]="_get_zero_arr_size_longdouble";static char _tmpB[28U]="_get_zero_arr_size_voidstar";
# 64
static struct _fat_ptr Cyc_Toseqc_pure_funs[8U]={{_tmp4,_tmp4,_tmp4 + 14U},{_tmp5,_tmp5,_tmp5 + 24U},{_tmp6,_tmp6,_tmp6 + 25U},{_tmp7,_tmp7,_tmp7 + 23U},{_tmp8,_tmp8,_tmp8 + 25U},{_tmp9,_tmp9,_tmp9 + 26U},{_tmpA,_tmpA,_tmpA + 30U},{_tmpB,_tmpB,_tmpB + 28U}};static char _tmpC[12U]="_check_null";static char _tmpD[28U]="_check_known_subscript_null";static char _tmpE[31U]="_check_known_subscript_notnull";static char _tmpF[21U]="_check_fat_subscript";static char _tmp10[15U]="_untag_fat_ptr";static char _tmp11[15U]="_region_malloc";
# 75
static struct _fat_ptr Cyc_Toseqc_exn_effect_funs[6U]={{_tmpC,_tmpC,_tmpC + 12U},{_tmpD,_tmpD,_tmpD + 28U},{_tmpE,_tmpE,_tmpE + 31U},{_tmpF,_tmpF,_tmpF + 21U},{_tmp10,_tmp10,_tmp10 + 15U},{_tmp11,_tmp11,_tmp11 + 15U}};
# 83
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_fun_effect(struct _fat_ptr fn){
{int i=0;for(0;(unsigned)i < 8U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)Cyc_Toseqc_pure_funs[i],(struct _fat_ptr)fn))
return Cyc_Toseqc_NoEffect;}}
{int i=0;for(0;(unsigned)i < 6U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)Cyc_Toseqc_exn_effect_funs[i],(struct _fat_ptr)fn))
return Cyc_Toseqc_ExnEffect;}}
return Cyc_Toseqc_AnyEffect;}
# 93
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exps_effect(struct Cyc_List_List*es){
enum Cyc_Toseqc_SideEffect res=0U;
# 99
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
enum Cyc_Toseqc_SideEffect _tmp12=Cyc_Toseqc_exp_effect((struct Cyc_Absyn_Exp*)x->hd);
res=Cyc_Toseqc_join_side_effect(_tmp12,res);
# 104
if((int)res == (int)3U)
# 106
return res;}}
# 110
return res;}
# 114
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e){
struct _fat_ptr bad_form=({const char*_tmp4A="";_tag_fat(_tmp4A,sizeof(char),1U);});
{void*_tmp13=e->r;void*_tmp14=_tmp13;struct Cyc_Absyn_Stmt*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Exp*_tmp3A;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Exp*_tmp34;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Exp*_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_List_List*_tmp2F;struct Cyc_Absyn_Exp*_tmp2E;switch(*((int*)_tmp14)){case 0U: _LL1: _LL2:
 goto _LL4;case 18U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 goto _LLA;case 33U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 return Cyc_Toseqc_Const;case 1U: _LLD: _LLE:
 return Cyc_Toseqc_NoEffect;case 4U: _LLF: _LL10:
# 125
 goto _LL12;case 40U: _LL11: _LL12:
 return Cyc_Toseqc_AnyEffect;case 10U: _LL13: _tmp2E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL14: {
# 129
void*_tmp15=_tmp2E->r;void*_tmp16=_tmp15;struct _tuple1*_tmp19;struct Cyc_Absyn_Vardecl*_tmp18;struct Cyc_Absyn_Fndecl*_tmp17;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16)->tag == 1U)switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16)->f1)){case 2U: _LL56: _tmp17=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16)->f1)->f1;_LL57:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp17->name));case 1U: _LL58: _tmp18=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16)->f1)->f1;_LL59:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp18->name));case 0U: _LL5A: _tmp19=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16)->f1)->f1;_LL5B:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp19));default: goto _LL5C;}else{_LL5C: _LL5D:
# 134
 return Cyc_Toseqc_AnyEffect;}_LL55:;}case 36U: _LL15: _tmp2F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_LL16: {
# 138
struct _tuple0 _tmp1A=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp2F);struct _tuple0 _tmp1B=_tmp1A;struct Cyc_List_List*_tmp1C;_LL5F: _tmp1C=_tmp1B.f2;_LL60:;
_tmp30=_tmp1C;goto _LL18;}case 3U: _LL17: _tmp30=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_LL18:
# 142
 return Cyc_Toseqc_exps_effect(_tmp30);case 6U: _LL19: _tmp33=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_tmp32=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_tmp31=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp14)->f3;_LL1A:
# 145
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp1D[3U];_tmp1D[0]=_tmp33,_tmp1D[1]=_tmp32,_tmp1D[2]=_tmp31;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D,sizeof(struct Cyc_Absyn_Exp*),3U));}));case 23U: _LL1B: _tmp35=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_tmp34=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_LL1C:
# 147
 _tmp37=_tmp35;_tmp36=_tmp34;goto _LL1E;case 7U: _LL1D: _tmp37=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_tmp36=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_LL1E:
 _tmp39=_tmp37;_tmp38=_tmp36;goto _LL20;case 8U: _LL1F: _tmp39=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_tmp38=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_LL20:
 _tmp3B=_tmp39;_tmp3A=_tmp38;goto _LL22;case 9U: _LL21: _tmp3B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_tmp3A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_LL22:
# 151
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp1E[2U];_tmp1E[0]=_tmp3B,_tmp1E[1]=_tmp3A;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E,sizeof(struct Cyc_Absyn_Exp*),2U));}));case 41U: _LL23: _tmp3C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL24:
# 153
 _tmp3D=_tmp3C;goto _LL26;case 12U: _LL25: _tmp3D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL26:
 _tmp3E=_tmp3D;goto _LL28;case 13U: _LL27: _tmp3E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL28:
 _tmp3F=_tmp3E;goto _LL2A;case 14U: _LL29: _tmp3F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp14)->f2;_LL2A:
 _tmp40=_tmp3F;goto _LL2C;case 15U: _LL2B: _tmp40=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL2C:
 _tmp41=_tmp40;goto _LL2E;case 20U: _LL2D: _tmp41=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL2E:
 _tmp42=_tmp41;goto _LL30;case 21U: _LL2F: _tmp42=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL30:
 _tmp43=_tmp42;goto _LL32;case 22U: _LL31: _tmp43=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL32:
 _tmp44=_tmp43;goto _LL34;case 5U: _LL33: _tmp44=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL34:
# 162
 return Cyc_Toseqc_exp_effect(_tmp44);case 37U: _LL35: _tmp45=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp14)->f1;_LL36:
# 164
 return Cyc_Toseqc_stmt_effect(_tmp45);case 2U: _LL37: _LL38:
# 166
 bad_form=({const char*_tmp1F="Pragma_e";_tag_fat(_tmp1F,sizeof(char),9U);});goto _LL0;case 11U: _LL39: _LL3A:
 bad_form=({const char*_tmp20="Throw_e";_tag_fat(_tmp20,sizeof(char),8U);});goto _LL0;case 16U: _LL3B: _LL3C:
 bad_form=({const char*_tmp21="New_e";_tag_fat(_tmp21,sizeof(char),6U);});goto _LL0;case 24U: _LL3D: _LL3E:
 bad_form=({const char*_tmp22="Tuple_e";_tag_fat(_tmp22,sizeof(char),8U);});goto _LL0;case 25U: _LL3F: _LL40:
 bad_form=({const char*_tmp23="CompoundLit_e";_tag_fat(_tmp23,sizeof(char),14U);});goto _LL0;case 26U: _LL41: _LL42:
 bad_form=({const char*_tmp24="Array_e";_tag_fat(_tmp24,sizeof(char),8U);});goto _LL0;case 27U: _LL43: _LL44:
 bad_form=({const char*_tmp25="Comprehension_e";_tag_fat(_tmp25,sizeof(char),16U);});goto _LL0;case 28U: _LL45: _LL46:
 bad_form=({const char*_tmp26="ComprehensionNoinit_e";_tag_fat(_tmp26,sizeof(char),22U);});goto _LL0;case 29U: _LL47: _LL48:
 bad_form=({const char*_tmp27="Aggregate_e";_tag_fat(_tmp27,sizeof(char),12U);});goto _LL0;case 30U: _LL49: _LL4A:
 bad_form=({const char*_tmp28="AnonStruct_e";_tag_fat(_tmp28,sizeof(char),13U);});goto _LL0;case 31U: _LL4B: _LL4C:
 bad_form=({const char*_tmp29="Datatype_e";_tag_fat(_tmp29,sizeof(char),11U);});goto _LL0;case 34U: _LL4D: _LL4E:
 bad_form=({const char*_tmp2A="Malloc_e";_tag_fat(_tmp2A,sizeof(char),9U);});goto _LL0;case 35U: _LL4F: _LL50:
 bad_form=({const char*_tmp2B="Swap_e";_tag_fat(_tmp2B,sizeof(char),7U);});goto _LL0;case 38U: _LL51: _LL52:
 bad_form=({const char*_tmp2C="Tagcheck_e";_tag_fat(_tmp2C,sizeof(char),11U);});goto _LL0;default: _LL53: _LL54:
 bad_form=({const char*_tmp2D="Valueof_e";_tag_fat(_tmp2D,sizeof(char),10U);});goto _LL0;}_LL0:;}
# 182
({struct Cyc_String_pa_PrintArg_struct _tmp48=({struct Cyc_String_pa_PrintArg_struct _tmpCD;_tmpCD.tag=0U,_tmpCD.f1=(struct _fat_ptr)((struct _fat_ptr)bad_form);_tmpCD;});struct Cyc_String_pa_PrintArg_struct _tmp49=({struct Cyc_String_pa_PrintArg_struct _tmpCC;_tmpCC.tag=0U,({
struct _fat_ptr _tmpD3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmpCC.f1=_tmpD3;});_tmpCC;});void*_tmp46[2U];_tmp46[0]=& _tmp48,_tmp46[1]=& _tmp49;({struct _fat_ptr _tmpD4=({const char*_tmp47="bad exp form %s (exp=|%s|) after xlation to C";_tag_fat(_tmp47,sizeof(char),46U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpD4,_tag_fat(_tmp46,sizeof(void*),2U));});});}
# 185
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s){
enum Cyc_Toseqc_SideEffect res=0U;
while(1){
void*_tmp4B=s->r;void*_tmp4C=_tmp4B;struct Cyc_Absyn_Stmt*_tmp52;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Stmt*_tmp50;struct Cyc_Absyn_Exp*_tmp4F;switch(*((int*)_tmp4C)){case 0U: _LL1: _LL2:
 return res;case 1U: _LL3: _tmp4F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4C)->f1;_LL4:
 return({enum Cyc_Toseqc_SideEffect _tmpD5=res;Cyc_Toseqc_join_side_effect(_tmpD5,Cyc_Toseqc_exp_effect(_tmp4F));});case 13U: _LL5: _tmp50=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp4C)->f2;_LL6:
# 192
 s=_tmp50;continue;case 2U: _LL7: _tmp52=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4C)->f1;_tmp51=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4C)->f2;_LL8:
# 194
 res=({enum Cyc_Toseqc_SideEffect _tmpD6=res;Cyc_Toseqc_join_side_effect(_tmpD6,Cyc_Toseqc_stmt_effect(_tmp52));});
s=_tmp51;
continue;case 6U: _LL9: _LLA:
# 198
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 8U: _LLD: _LLE:
 goto _LL10;case 3U: _LLF: _LL10:
 goto _LL12;case 4U: _LL11: _LL12:
 goto _LL14;case 5U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 14U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 10U: _LL1B: _LL1C:
 return Cyc_Toseqc_AnyEffect;default: _LL1D: _LL1E:
({void*_tmp4D=0U;({struct _fat_ptr _tmpD7=({const char*_tmp4E="bad stmt after xlation to C";_tag_fat(_tmp4E,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpD7,_tag_fat(_tmp4D,sizeof(void*),0U));});});}_LL0:;}}
# 215
static int Cyc_Toseqc_is_toc_var(struct Cyc_Absyn_Exp*e){
void*_tmp53=e->r;void*_tmp54=_tmp53;struct _tuple1*_tmp55;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54)->f1)->tag == 0U){_LL1: _tmp55=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54)->f1)->f1;_LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 222
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*s);
# 240 "toseqc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toseqc_exps_to_seqc(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
# 242
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toseqc_exp_to_seqc,es);
# 245
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)<= 1 ||(int)Cyc_Toseqc_exps_effect(es)!= (int)Cyc_Toseqc_AnyEffect)return 0;{
# 249
struct Cyc_Absyn_Stmt*stmt=({struct Cyc_Absyn_Exp*_tmpD8=Cyc_Absyn_copy_exp(e);Cyc_Absyn_exp_stmt(_tmpD8,e->loc);});
struct Cyc_Absyn_Stmt*laststmt=stmt;
int did_skip=0;
int did_convert=0;
{struct Cyc_List_List*x=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Exp*_tmp56=(struct Cyc_Absyn_Exp*)x->hd;
# 257
if(Cyc_Tcutil_is_const_exp(_tmp56)|| Cyc_Toseqc_is_toc_var(_tmp56))continue;
# 262
if(!did_skip){
did_skip=1;
continue;}
# 266
did_convert=1;{
# 269
struct _tuple1*_tmp57=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp58=Cyc_Absyn_var_exp(_tmp57,_tmp56->loc);
# 273
struct Cyc_Absyn_Exp*_tmp59=Cyc_Absyn_new_exp(_tmp56->r,_tmp56->loc);
_tmp59->annot=_tmp56->annot;
_tmp59->topt=_tmp56->topt;{
# 277
void*t=_tmp59->topt == 0?(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=11U,_tmp5A->f1=_tmp59;_tmp5A;}): Cyc_Toc_typ_to_c((void*)_check_null(_tmp59->topt));
# 281
stmt=Cyc_Absyn_declare_stmt(_tmp57,t,_tmp59,stmt,e->loc);
# 284
_tmp56->r=_tmp58->r;};};}}
# 288
if(did_convert){
({void*_tmpD9=(Cyc_Absyn_stmt_exp(stmt,e->loc))->r;e->r=_tmpD9;});
return laststmt;}else{
# 292
return 0;}};}
# 298
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*e){
struct _fat_ptr bad_form=({const char*_tmp95="";_tag_fat(_tmp95,sizeof(char),1U);});
{void*_tmp5B=e->r;void*_tmp5C=_tmp5B;struct Cyc_Absyn_Stmt*_tmp90;struct Cyc_List_List*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp80;struct Cyc_Absyn_Exp*_tmp7F;struct Cyc_Absyn_Exp*_tmp7E;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Exp*_tmp7B;struct Cyc_Core_Opt*_tmp7A;struct Cyc_Absyn_Exp*_tmp79;struct Cyc_Absyn_Exp*_tmp78;struct Cyc_Absyn_Exp*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp74;switch(*((int*)_tmp5C)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return;case 10U: _LL5: _tmp75=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_tmp74=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LL6:
# 304
 _tmp76=({struct Cyc_List_List*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->hd=_tmp75,_tmp5D->tl=_tmp74;_tmp5D;});goto _LL8;case 3U: _LL7: _tmp76=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LL8:
 Cyc_Toseqc_exps_to_seqc(e,_tmp76);return;case 23U: _LL9: _tmp78=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_tmp77=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LLA:
({struct Cyc_Absyn_Exp*_tmpDA=e;Cyc_Toseqc_exps_to_seqc(_tmpDA,({struct Cyc_Absyn_Exp*_tmp5E[2U];_tmp5E[0]=_tmp78,_tmp5E[1]=_tmp77;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp5E,sizeof(struct Cyc_Absyn_Exp*),2U));}));});return;case 4U: _LLB: _tmp7B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_tmp7A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_tmp79=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5C)->f3;_LLC:
# 309
 if(_tmp7A == 0){
void*_tmp5F=_tmp7B->r;void*_tmp60=_tmp5F;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp60)->tag == 1U){_LL56: _LL57:
 Cyc_Toseqc_exp_to_seqc(_tmp79);return;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 315
({struct Cyc_Absyn_Exp*_tmpDB=e;Cyc_Toseqc_exps_to_seqc(_tmpDB,({struct Cyc_Absyn_Exp*_tmp61[2U];_tmp61[0]=_tmp79,_tmp61[1]=_tmp7B;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp61,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
return;case 6U: _LLD: _tmp7E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_tmp7D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_tmp7C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C)->f3;_LLE:
# 318
 Cyc_Toseqc_exp_to_seqc(_tmp7C);_tmp80=_tmp7E;_tmp7F=_tmp7D;goto _LL10;case 7U: _LLF: _tmp80=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_tmp7F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LL10:
 _tmp82=_tmp80;_tmp81=_tmp7F;goto _LL12;case 8U: _LL11: _tmp82=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_tmp81=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LL12:
 _tmp84=_tmp82;_tmp83=_tmp81;goto _LL14;case 9U: _LL13: _tmp84=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_tmp83=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LL14:
 Cyc_Toseqc_exp_to_seqc(_tmp83);_tmp85=_tmp84;goto _LL16;case 41U: _LL15: _tmp85=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL16:
# 323
 _tmp86=_tmp85;goto _LL18;case 12U: _LL17: _tmp86=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL18:
 _tmp87=_tmp86;goto _LL1A;case 13U: _LL19: _tmp87=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL1A:
 _tmp88=_tmp87;goto _LL1C;case 14U: _LL1B: _tmp88=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LL1C:
 _tmp89=_tmp88;goto _LL1E;case 15U: _LL1D: _tmp89=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL1E:
 _tmp8A=_tmp89;goto _LL20;case 18U: _LL1F: _tmp8A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL20:
 _tmp8B=_tmp8A;goto _LL22;case 20U: _LL21: _tmp8B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL22:
 _tmp8C=_tmp8B;goto _LL24;case 21U: _LL23: _tmp8C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL24:
 _tmp8D=_tmp8C;goto _LL26;case 22U: _LL25: _tmp8D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL26:
 _tmp8E=_tmp8D;goto _LL28;case 5U: _LL27: _tmp8E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL28:
 Cyc_Toseqc_exp_to_seqc(_tmp8E);return;case 17U: _LL29: _LL2A:
# 334
 goto _LL2C;case 19U: _LL2B: _LL2C:
 goto _LL2E;case 33U: _LL2D: _LL2E:
 goto _LL30;case 32U: _LL2F: _LL30:
 return;case 36U: _LL31: _tmp8F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5C)->f2;_LL32: {
# 340
struct _tuple0 _tmp62=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp8F);struct _tuple0 _tmp63=_tmp62;struct Cyc_List_List*_tmp64;_LL5B: _tmp64=_tmp63.f2;_LL5C:;
Cyc_Toseqc_exps_to_seqc(e,_tmp64);
return;}case 37U: _LL33: _tmp90=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5C)->f1;_LL34:
# 344
 Cyc_Toseqc_stmt_to_seqc(_tmp90);return;case 40U: _LL35: _LL36:
# 346
 return;case 2U: _LL37: _LL38:
# 348
 bad_form=({const char*_tmp65="Pragma_e";_tag_fat(_tmp65,sizeof(char),9U);});goto _LL0;case 11U: _LL39: _LL3A:
 bad_form=({const char*_tmp66="Throw_e";_tag_fat(_tmp66,sizeof(char),8U);});goto _LL0;case 16U: _LL3B: _LL3C:
 bad_form=({const char*_tmp67="New_e";_tag_fat(_tmp67,sizeof(char),6U);});goto _LL0;case 24U: _LL3D: _LL3E:
 bad_form=({const char*_tmp68="Tuple_e";_tag_fat(_tmp68,sizeof(char),8U);});goto _LL0;case 25U: _LL3F: _LL40:
 bad_form=({const char*_tmp69="CompoundLit_e";_tag_fat(_tmp69,sizeof(char),14U);});goto _LL0;case 26U: _LL41: _LL42:
 bad_form=({const char*_tmp6A="Array_e";_tag_fat(_tmp6A,sizeof(char),8U);});goto _LL0;case 27U: _LL43: _LL44:
 bad_form=({const char*_tmp6B="Comprehension_e";_tag_fat(_tmp6B,sizeof(char),16U);});goto _LL0;case 28U: _LL45: _LL46:
 bad_form=({const char*_tmp6C="ComprehensionNoinit_e";_tag_fat(_tmp6C,sizeof(char),22U);});goto _LL0;case 29U: _LL47: _LL48:
 bad_form=({const char*_tmp6D="Aggregate_e";_tag_fat(_tmp6D,sizeof(char),12U);});goto _LL0;case 30U: _LL49: _LL4A:
 bad_form=({const char*_tmp6E="AnonStruct_e";_tag_fat(_tmp6E,sizeof(char),13U);});goto _LL0;case 31U: _LL4B: _LL4C:
 bad_form=({const char*_tmp6F="Datatype_e";_tag_fat(_tmp6F,sizeof(char),11U);});goto _LL0;case 34U: _LL4D: _LL4E:
 bad_form=({const char*_tmp70="Malloc_e";_tag_fat(_tmp70,sizeof(char),9U);});goto _LL0;case 35U: _LL4F: _LL50:
 bad_form=({const char*_tmp71="Swap_e";_tag_fat(_tmp71,sizeof(char),7U);});goto _LL0;case 38U: _LL51: _LL52:
 bad_form=({const char*_tmp72="Tagcheck_e";_tag_fat(_tmp72,sizeof(char),11U);});goto _LL0;default: _LL53: _LL54:
 bad_form=({const char*_tmp73="Valueof_e";_tag_fat(_tmp73,sizeof(char),10U);});goto _LL0;}_LL0:;}
# 364
({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmpCF;_tmpCF.tag=0U,_tmpCF.f1=(struct _fat_ptr)((struct _fat_ptr)bad_form);_tmpCF;});struct Cyc_String_pa_PrintArg_struct _tmp94=({struct Cyc_String_pa_PrintArg_struct _tmpCE;_tmpCE.tag=0U,({
struct _fat_ptr _tmpDC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmpCE.f1=_tmpDC;});_tmpCE;});void*_tmp91[2U];_tmp91[0]=& _tmp93,_tmp91[1]=& _tmp94;({struct _fat_ptr _tmpDD=({const char*_tmp92="bad exp form %s (exp=|%s|) after xlation to C";_tag_fat(_tmp92,sizeof(char),46U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpDD,_tag_fat(_tmp91,sizeof(void*),2U));});});}
# 368
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s){
# 370
while(1){
void*_tmp96=s->r;void*_tmp97=_tmp96;struct Cyc_Absyn_Decl*_tmpC3;struct Cyc_Absyn_Stmt*_tmpC2;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_List_List*_tmpC0;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Stmt*_tmpBC;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Stmt*_tmpBA;struct Cyc_Absyn_Stmt*_tmpB9;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Stmt*_tmpB7;struct Cyc_Absyn_Stmt*_tmpB6;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Stmt*_tmpB4;struct Cyc_Absyn_Stmt*_tmpB3;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Stmt*_tmpB0;switch(*((int*)_tmp97)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 goto _LL8;case 8U: _LL7: _LL8:
 return;case 13U: _LL9: _tmpB0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LLA:
 s=_tmpB0;continue;case 3U: _LLB: _tmpB1=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LLC:
# 378
 if(_tmpB1 == 0)
return;
_tmpB2=_tmpB1;goto _LLE;case 1U: _LLD: _tmpB2=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_LLE:
# 382
 Cyc_Toseqc_exp_to_seqc(_tmpB2);
# 390
return;case 4U: _LLF: _tmpB5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpB4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_tmpB3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp97)->f3;_LL10:
# 392
 Cyc_Toseqc_exp_to_seqc(_tmpB5);
_tmpB7=_tmpB4;_tmpB6=_tmpB3;goto _LL12;case 2U: _LL11: _tmpB7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpB6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL12:
# 395
 Cyc_Toseqc_stmt_to_seqc(_tmpB7);
s=_tmpB6;
continue;case 14U: _LL13: _tmpB9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpB8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp97)->f2).f1;_LL14:
 _tmpBB=_tmpB8;_tmpBA=_tmpB9;goto _LL16;case 5U: _LL15: _tmpBB=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp97)->f1).f1;_tmpBA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL16:
# 400
 Cyc_Toseqc_exp_to_seqc(_tmpBB);
s=_tmpBA;
continue;case 9U: _LL17: _tmpBF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpBE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f2).f1;_tmpBD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f3).f1;_tmpBC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp97)->f4;_LL18:
# 404
 Cyc_Toseqc_exp_to_seqc(_tmpBF);
Cyc_Toseqc_exp_to_seqc(_tmpBE);
Cyc_Toseqc_exp_to_seqc(_tmpBD);
s=_tmpBC;
continue;case 10U: _LL19: _tmpC1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpC0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL1A:
# 412
 Cyc_Toseqc_exp_to_seqc(_tmpC1);
for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){
Cyc_Toseqc_stmt_to_seqc(((struct Cyc_Absyn_Switch_clause*)_tmpC0->hd)->body);}
return;case 12U: _LL1B: _tmpC3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp97)->f1;_tmpC2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp97)->f2;_LL1C:
# 417
{void*_tmp98=_tmpC3->r;void*_tmp99=_tmp98;struct Cyc_Absyn_Vardecl*_tmpAD;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp99)->tag == 0U){_LL20: _tmpAD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp99)->f1;_LL21:
# 419
 if(_tmpAD->initializer != 0){
# 422
void*_tmp9A=((struct Cyc_Absyn_Exp*)_check_null(_tmpAD->initializer))->r;void*_tmp9B=_tmp9A;struct Cyc_List_List*_tmpAC;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9B)->tag == 36U){_LL25: _tmpAC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9B)->f2;_LL26: {
# 435 "toseqc.cyc"
struct _tuple0 _tmp9C=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpAC);struct _tuple0 _tmp9D=_tmp9C;struct Cyc_List_List*_tmpAB;_LL2A: _tmpAB=_tmp9D.f2;_LL2B:;{
struct Cyc_Absyn_Stmt*_tmp9E=Cyc_Toseqc_exps_to_seqc((struct Cyc_Absyn_Exp*)_check_null(_tmpAD->initializer),_tmpAB);
# 438
if(_tmp9E == 0)goto _LL24;
# 444
{void*_tmp9F=_tmp9E->r;void*_tmpA0=_tmp9F;struct Cyc_Absyn_Exp*_tmpAA;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA0)->tag == 1U){_LL2D: _tmpAA=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA0)->f1;_LL2E:
# 446
{void*_tmpA1=((struct Cyc_Absyn_Exp*)_check_null(_tmpAD->initializer))->r;void*_tmpA2=_tmpA1;struct Cyc_Absyn_Stmt*_tmpA6;if(((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA2)->tag == 37U){_LL32: _tmpA6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA2)->f1;_LL33:
# 448
 _tmpAD->initializer=_tmpAA;
_tmp9E->r=s->r;
s->r=_tmpA6->r;
goto _LL31;}else{_LL34: _LL35:
# 453
({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmpD0;_tmpD0.tag=0U,({
struct _fat_ptr _tmpDE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmpAD->initializer)));_tmpD0.f1=_tmpDE;});_tmpD0;});void*_tmpA3[1U];_tmpA3[0]=& _tmpA5;({struct _fat_ptr _tmpDF=({const char*_tmpA4="exps_to_seqc updated to non-stmt-exp |%s|";_tag_fat(_tmpA4,sizeof(char),42U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpDF,_tag_fat(_tmpA3,sizeof(void*),1U));});});}_LL31:;}
# 456
goto _LL2C;}else{_LL2F: _LL30:
# 458
({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmpD1;_tmpD1.tag=0U,({
struct _fat_ptr _tmpE0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(_tmp9E));_tmpD1.f1=_tmpE0;});_tmpD1;});void*_tmpA7[1U];_tmpA7[0]=& _tmpA9;({struct _fat_ptr _tmpE1=({const char*_tmpA8="exps_to_seqc returned non-exp-stmt |%s|";_tag_fat(_tmpA8,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpE1,_tag_fat(_tmpA7,sizeof(void*),1U));});});}_LL2C:;}
# 461
goto _LL24;};}}else{_LL27: _LL28:
# 464
 Cyc_Toseqc_exp_to_seqc((struct Cyc_Absyn_Exp*)_check_null(_tmpAD->initializer));
goto _LL24;}_LL24:;}
# 468
s=_tmpC2;
continue;}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 472
goto _LL1E;default: _LL1D: _LL1E:
({void*_tmpAE=0U;({struct _fat_ptr _tmpE2=({const char*_tmpAF="bad stmt after xlation to C";_tag_fat(_tmpAF,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpE2,_tag_fat(_tmpAE,sizeof(void*),0U));});});}_LL0:;}}
# 478
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmpC4=ds;
# 481
for(0;_tmpC4 != 0;_tmpC4=_tmpC4->tl){
struct Cyc_Absyn_Decl*_tmpC5=(struct Cyc_Absyn_Decl*)_tmpC4->hd;
void*_tmpC6=_tmpC5->r;void*_tmpC7=_tmpC6;struct Cyc_Absyn_Fndecl*_tmpCA;switch(*((int*)_tmpC7)){case 1U: _LL1: _tmpCA=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpC7)->f1;_LL2:
 Cyc_Toseqc_stmt_to_seqc(_tmpCA->body);goto _LL0;case 0U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 8U: _LL11: _LL12:
 goto _LL0;case 9U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
({void*_tmpC8=0U;({struct _fat_ptr _tmpE3=({const char*_tmpC9="unexpected toplevel decl in toseqc";_tag_fat(_tmpC9,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmpE3,_tag_fat(_tmpC8,sizeof(void*),0U));});});}_LL0:;}
# 503
return ds;}

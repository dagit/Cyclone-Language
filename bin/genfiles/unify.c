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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190 "list.h"
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;
# 47 "position.h"
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 888 "absyn.h"
void*Cyc_Absyn_compress_kb(void*);
# 917
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 946
void*Cyc_Absyn_bounds_one();struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 74
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 101
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 106
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 191
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 200
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 261
void*Cyc_Tcutil_normalize_effect(void*e);
# 276
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 323
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 327
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*,struct Cyc_List_List*);
# 330
int Cyc_Tcutil_tycon_cmp(void*,void*);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);char Cyc_Unify_Unify[6U]="Unify";struct Cyc_Unify_Unify_exn_struct{char*tag;};
# 34 "unify.cyc"
struct Cyc_Unify_Unify_exn_struct Cyc_Unify_Unify_val={Cyc_Unify_Unify};
# 37
static void*Cyc_Unify_t1_failure=0;
static int Cyc_Unify_tq1_const=0;
static void*Cyc_Unify_t2_failure=0;
static int Cyc_Unify_tq2_const=0;
# 42
struct _fat_ptr Cyc_Unify_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 46
void Cyc_Unify_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 51
if(({struct _fat_ptr _tmp11D=({const char*_tmp0="(qualifiers don't match)";_tag_fat(_tmp0,sizeof(char),25U);});Cyc_strcmp(_tmp11D,(struct _fat_ptr)Cyc_Unify_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0U,_tmp111.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Unify_failure_reason);_tmp111;});void*_tmp1[1U];_tmp1[0]=& _tmp3;({struct Cyc___cycFILE*_tmp11F=Cyc_stderr;struct _fat_ptr _tmp11E=({const char*_tmp2="  %s\n";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_fprintf(_tmp11F,_tmp11E,_tag_fat(_tmp1,sizeof(void*),1U));});});
return;}
# 56
if(({struct _fat_ptr _tmp120=({const char*_tmp4="(function effects do not match)";_tag_fat(_tmp4,sizeof(char),32U);});Cyc_strcmp(_tmp120,(struct _fat_ptr)Cyc_Unify_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp5=Cyc_Absynpp_tc_params_r;
_tmp5.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp5);}{
# 61
void*_tmp6=Cyc_Unify_t1_failure;
void*_tmp7=Cyc_Unify_t2_failure;
struct _fat_ptr s1=(unsigned)_tmp6?Cyc_Absynpp_typ2string(_tmp6):({const char*_tmp22="<?>";_tag_fat(_tmp22,sizeof(char),4U);});
struct _fat_ptr s2=(unsigned)_tmp7?Cyc_Absynpp_typ2string(_tmp7):({const char*_tmp21="<?>";_tag_fat(_tmp21,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0U,_tmp112.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp112;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp122=Cyc_stderr;struct _fat_ptr _tmp121=({const char*_tmp9="  %s";_tag_fat(_tmp9,sizeof(char),5U);});Cyc_fprintf(_tmp122,_tmp121,_tag_fat(_tmp8,sizeof(void*),1U));});});
pos +=_get_fat_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpB=0U;({struct Cyc___cycFILE*_tmp124=Cyc_stderr;struct _fat_ptr _tmp123=({const char*_tmpC="\n\t";_tag_fat(_tmpC,sizeof(char),3U);});Cyc_fprintf(_tmp124,_tmp123,_tag_fat(_tmpB,sizeof(void*),0U));});});
pos=8;}else{
# 72
({void*_tmpD=0U;({struct Cyc___cycFILE*_tmp126=Cyc_stderr;struct _fat_ptr _tmp125=({const char*_tmpE=" ";_tag_fat(_tmpE,sizeof(char),2U);});Cyc_fprintf(_tmp126,_tmp125,_tag_fat(_tmpD,sizeof(void*),0U));});});
++ pos;}
# 75
({void*_tmpF=0U;({struct Cyc___cycFILE*_tmp128=Cyc_stderr;struct _fat_ptr _tmp127=({const char*_tmp10="and ";_tag_fat(_tmp10,sizeof(char),5U);});Cyc_fprintf(_tmp128,_tmp127,_tag_fat(_tmpF,sizeof(void*),0U));});});
pos +=4;
if((unsigned)pos + _get_fat_size(s2,sizeof(char))>= (unsigned)80){
({void*_tmp11=0U;({struct Cyc___cycFILE*_tmp12A=Cyc_stderr;struct _fat_ptr _tmp129=({const char*_tmp12="\n\t";_tag_fat(_tmp12,sizeof(char),3U);});Cyc_fprintf(_tmp12A,_tmp129,_tag_fat(_tmp11,sizeof(void*),0U));});});
pos=8;}
# 81
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0U,_tmp113.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp113;});void*_tmp13[1U];_tmp13[0]=& _tmp15;({struct Cyc___cycFILE*_tmp12C=Cyc_stderr;struct _fat_ptr _tmp12B=({const char*_tmp14="%s ";_tag_fat(_tmp14,sizeof(char),4U);});Cyc_fprintf(_tmp12C,_tmp12B,_tag_fat(_tmp13,sizeof(void*),1U));});});
pos +=_get_fat_size(s2,sizeof(char))+ (unsigned)1;
if(pos + 17 >= 80){
({void*_tmp16=0U;({struct Cyc___cycFILE*_tmp12E=Cyc_stderr;struct _fat_ptr _tmp12D=({const char*_tmp17="\n\t";_tag_fat(_tmp17,sizeof(char),3U);});Cyc_fprintf(_tmp12E,_tmp12D,_tag_fat(_tmp16,sizeof(void*),0U));});});
pos=8;}
# 87
({void*_tmp18=0U;({struct Cyc___cycFILE*_tmp130=Cyc_stderr;struct _fat_ptr _tmp12F=({const char*_tmp19="are not compatible. ";_tag_fat(_tmp19,sizeof(char),21U);});Cyc_fprintf(_tmp130,_tmp12F,_tag_fat(_tmp18,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmp131=(char*)Cyc_Unify_failure_reason.curr;_tmp131 != (char*)(_tag_fat(0,0,0)).curr;})){
if(({unsigned long _tmp132=(unsigned long)pos;_tmp132 + Cyc_strlen((struct _fat_ptr)Cyc_Unify_failure_reason);})>= (unsigned long)80)
({void*_tmp1A=0U;({struct Cyc___cycFILE*_tmp134=Cyc_stderr;struct _fat_ptr _tmp133=({const char*_tmp1B="\n\t";_tag_fat(_tmp1B,sizeof(char),3U);});Cyc_fprintf(_tmp134,_tmp133,_tag_fat(_tmp1A,sizeof(void*),0U));});});
# 93
({struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp114;_tmp114.tag=0U,_tmp114.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Unify_failure_reason);_tmp114;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1E;({struct Cyc___cycFILE*_tmp136=Cyc_stderr;struct _fat_ptr _tmp135=({const char*_tmp1D="%s";_tag_fat(_tmp1D,sizeof(char),3U);});Cyc_fprintf(_tmp136,_tmp135,_tag_fat(_tmp1C,sizeof(void*),1U));});});}
# 95
({void*_tmp1F=0U;({struct Cyc___cycFILE*_tmp138=Cyc_stderr;struct _fat_ptr _tmp137=({const char*_tmp20="\n";_tag_fat(_tmp20,sizeof(char),2U);});Cyc_fprintf(_tmp138,_tmp137,_tag_fat(_tmp1F,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 101
static int Cyc_Unify_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2)&& Cyc_Tcutil_check_logical_implication(r2,r1);}struct _tuple10{void*f1;void*f2;};
# 106
int Cyc_Unify_unify_kindbound(void*kb1,void*kb2){
struct _tuple10 _tmp23=({struct _tuple10 _tmp115;({void*_tmp13A=Cyc_Absyn_compress_kb(kb1);_tmp115.f1=_tmp13A;}),({void*_tmp139=Cyc_Absyn_compress_kb(kb2);_tmp115.f2=_tmp139;});_tmp115;});struct _tuple10 _tmp24=_tmp23;struct Cyc_Core_Opt**_tmp39;void*_tmp38;void*_tmp37;struct Cyc_Core_Opt**_tmp36;struct Cyc_Core_Opt**_tmp35;struct Cyc_Absyn_Kind*_tmp34;struct Cyc_Core_Opt**_tmp33;struct Cyc_Absyn_Kind*_tmp32;struct Cyc_Core_Opt**_tmp31;struct Cyc_Absyn_Kind*_tmp30;struct Cyc_Absyn_Kind*_tmp2F;struct Cyc_Absyn_Kind*_tmp2E;struct Cyc_Core_Opt**_tmp2D;struct Cyc_Absyn_Kind*_tmp2C;struct Cyc_Absyn_Kind*_tmp2B;struct Cyc_Absyn_Kind*_tmp2A;switch(*((int*)_tmp24.f1)){case 0U: switch(*((int*)_tmp24.f2)){case 0U: _LL1: _tmp2B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp24.f1)->f1;_tmp2A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp24.f2)->f1;_LL2:
 return _tmp2B == _tmp2A;case 2U: _LL5: _tmp2E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp24.f1)->f1;_tmp2D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f2)->f1;_tmp2C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f2)->f2;_LL6:
# 115
 if(Cyc_Tcutil_kind_leq(_tmp2E,_tmp2C)){
({struct Cyc_Core_Opt*_tmp13B=({struct Cyc_Core_Opt*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->v=kb1;_tmp26;});*_tmp2D=_tmp13B;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp24.f2)){case 0U: _LL3: _tmp31=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f1)->f1;_tmp30=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f1)->f2;_tmp2F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp24.f2)->f1;_LL4:
# 110
 if(Cyc_Tcutil_kind_leq(_tmp2F,_tmp30)){
({struct Cyc_Core_Opt*_tmp13C=({struct Cyc_Core_Opt*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->v=kb2;_tmp25;});*_tmp31=_tmp13C;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp35=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f1)->f1;_tmp34=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f1)->f2;_tmp33=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f2)->f1;_tmp32=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp24.f2)->f2;_LL8:
# 120
 if(Cyc_Tcutil_kind_leq(_tmp34,_tmp32)){
({struct Cyc_Core_Opt*_tmp13D=({struct Cyc_Core_Opt*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->v=kb1;_tmp27;});*_tmp33=_tmp13D;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp32,_tmp34)){
({struct Cyc_Core_Opt*_tmp13E=({struct Cyc_Core_Opt*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->v=kb2;_tmp28;});*_tmp35=_tmp13E;});
return 1;}else{
return 0;}}default: _LLB: _tmp37=_tmp24.f1;_tmp36=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp24.f2)->f1;_LLC:
# 129
({struct Cyc_Core_Opt*_tmp13F=({struct Cyc_Core_Opt*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->v=_tmp37;_tmp29;});*_tmp36=_tmp13F;});
return 1;}default: _LL9: _tmp39=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp24.f1)->f1;_tmp38=_tmp24.f2;_LLA:
# 127
 _tmp37=_tmp38;_tmp36=_tmp39;goto _LLC;}_LL0:;}
# 134
static void Cyc_Unify_unify_it(void*t1,void*t2);
# 137
int Cyc_Unify_unify(void*t1,void*t2){
struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=0;if(setjmp(_tmp3A.handler))_tmp3C=1;if(!_tmp3C){
Cyc_Unify_unify_it(t1,t2);{
int _tmp3D=1;_npop_handler(0U);return _tmp3D;};
# 139
;_pop_handler();}else{void*_tmp3B=(void*)Cyc_Core_get_exn_thrown();void*_tmp3E=_tmp3B;void*_tmp3F;if(((struct Cyc_Unify_Unify_exn_struct*)_tmp3E)->tag == Cyc_Unify_Unify){_LL1: _LL2:
# 141
 return 0;}else{_LL3: _tmp3F=_tmp3E;_LL4:(int)_rethrow(_tmp3F);}_LL0:;}};}
# 146
static void Cyc_Unify_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp40=t;struct Cyc_List_List*_tmp60;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp5E;struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5C;void*_tmp5B;struct Cyc_Absyn_Tqual _tmp5A;void*_tmp59;struct Cyc_List_List*_tmp58;int _tmp57;struct Cyc_Absyn_VarargInfo*_tmp56;struct Cyc_List_List*_tmp55;struct Cyc_List_List*_tmp54;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_List_List*_tmp52;struct Cyc_Absyn_Exp*_tmp51;struct Cyc_List_List*_tmp50;void*_tmp4F;void*_tmp4E;struct Cyc_Absyn_PtrInfo _tmp4D;void*_tmp4C;struct Cyc_Core_Opt**_tmp4B;struct Cyc_Absyn_Tvar*_tmp4A;switch(*((int*)_tmp40)){case 2U: _LL1: _tmp4A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp40)->f1;_LL2:
# 151
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp4A)){
Cyc_Unify_failure_reason=({const char*_tmp41="(type variable would escape scope)";_tag_fat(_tmp41,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 155
goto _LL0;case 1U: _LL3: _tmp4C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40)->f2;_tmp4B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40)->f4;_LL4:
# 157
 if(t == evar){
Cyc_Unify_failure_reason=({const char*_tmp42="(occurs check)";_tag_fat(_tmp42,sizeof(char),15U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}else{
# 161
if(_tmp4C != 0)Cyc_Unify_occurs(evar,r,env,_tmp4C);else{
# 164
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp4B))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 171
if(problem){
struct Cyc_List_List*_tmp43=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp4B))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp43=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp44->tl=_tmp43;_tmp44;});}}
# 177
({struct Cyc_Core_Opt*_tmp140=({struct Cyc_Core_Opt*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->v=_tmp43;_tmp45;});*_tmp4B=_tmp140;});}}}
# 180
goto _LL0;case 3U: _LL5: _tmp4D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40)->f1;_LL6:
# 182
 Cyc_Unify_occurs(evar,r,env,_tmp4D.elt_type);
Cyc_Unify_occurs(evar,r,env,(_tmp4D.ptr_atts).rgn);
Cyc_Unify_occurs(evar,r,env,(_tmp4D.ptr_atts).nullable);
Cyc_Unify_occurs(evar,r,env,(_tmp4D.ptr_atts).bounds);
Cyc_Unify_occurs(evar,r,env,(_tmp4D.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp4F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40)->f1).elt_type;_tmp4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40)->f1).zero_term;_LL8:
# 190
 Cyc_Unify_occurs(evar,r,env,_tmp4F);
Cyc_Unify_occurs(evar,r,env,_tmp4E);
goto _LL0;case 5U: _LL9: _tmp5C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).tvars;_tmp5B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).effect;_tmp5A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).ret_tqual;_tmp59=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).ret_type;_tmp58=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).args;_tmp57=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).c_varargs;_tmp56=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).cyc_varargs;_tmp55=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).rgn_po;_tmp54=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).attributes;_tmp53=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).requires_clause;_tmp52=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).requires_relns;_tmp51=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).ensures_clause;_tmp50=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40)->f1).ensures_relns;_LLA:
# 195
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp5C,env);
if(_tmp5B != 0)Cyc_Unify_occurs(evar,r,env,_tmp5B);
Cyc_Unify_occurs(evar,r,env,_tmp59);
for(0;_tmp58 != 0;_tmp58=_tmp58->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple8*)_tmp58->hd)).f3);}
if(_tmp56 != 0)
Cyc_Unify_occurs(evar,r,env,_tmp56->type);
for(0;_tmp55 != 0;_tmp55=_tmp55->tl){
struct _tuple10*_tmp46=(struct _tuple10*)_tmp55->hd;struct _tuple10*_tmp47=_tmp46;void*_tmp49;void*_tmp48;_LL16: _tmp49=_tmp47->f1;_tmp48=_tmp47->f2;_LL17:;
Cyc_Unify_occurs(evar,r,env,_tmp49);
Cyc_Unify_occurs(evar,r,env,_tmp48);}
# 207
goto _LL0;case 6U: _LLB: _tmp5D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40)->f1;_LLC:
# 209
 for(0;_tmp5D != 0;_tmp5D=_tmp5D->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple11*)_tmp5D->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp5E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp40)->f2;_LLE:
# 214
 for(0;_tmp5E != 0;_tmp5E=_tmp5E->tl){
Cyc_Unify_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp5E->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp5F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp40)->f2;_LL10:
 _tmp60=_tmp5F;goto _LL12;case 0U: _LL11: _tmp60=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40)->f2;_LL12:
# 219
 Cyc_Unify_occurslist(evar,r,env,_tmp60);goto _LL0;default: _LL13: _LL14:
# 222
 goto _LL0;}_LL0:;};}
# 225
static void Cyc_Unify_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 227
for(0;ts != 0;ts=ts->tl){
Cyc_Unify_occurs(evar,r,env,(void*)ts->hd);}}
# 232
static void Cyc_Unify_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0 && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Unify_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0 || t2 != 0)
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 240
static void Cyc_Unify_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const && !tq1.real_const)
({void*_tmp61=0U;({struct _fat_ptr _tmp141=({const char*_tmp62="tq1 real_const not set.";_tag_fat(_tmp62,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp141,_tag_fat(_tmp61,sizeof(void*),0U));});});
if(tq2.print_const && !tq2.real_const)
({void*_tmp63=0U;({struct _fat_ptr _tmp142=({const char*_tmp64="tq2 real_const not set.";_tag_fat(_tmp64,sizeof(char),24U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp142,_tag_fat(_tmp63,sizeof(void*),0U));});});
# 246
if((tq1.real_const != tq2.real_const || tq1.q_volatile != tq2.q_volatile)|| tq1.q_restrict != tq2.q_restrict){
# 249
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_tq1_const=tq1.real_const;
Cyc_Unify_tq2_const=tq2.real_const;
Cyc_Unify_failure_reason=({const char*_tmp65="(qualifiers don't match)";_tag_fat(_tmp65,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 257
Cyc_Unify_tq1_const=0;
Cyc_Unify_tq2_const=0;}
# 261
static void Cyc_Unify_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmp66=({struct _tuple10 _tmp116;_tmp116.f1=e1->r,_tmp116.f2=e2->r;_tmp116;});struct _tuple10 _tmp67=_tmp66;void*_tmp71;void*_tmp70;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp67.f1)->tag == 14U){_LL1: _tmp6E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp67.f1)->f2;_LL2:
# 266
 Cyc_Unify_unify_cmp_exp(_tmp6E,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp67.f2)->tag == 14U){_LL3: _tmp6F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp67.f2)->f2;_LL4:
 Cyc_Unify_unify_cmp_exp(e1,_tmp6F);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp67.f1)->tag == 39U){_LL5: _tmp70=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp67.f1)->f1;_LL6: {
# 269
void*_tmp68=Cyc_Tcutil_compress(_tmp70);void*_tmp69=_tmp68;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp69)->tag == 1U){_LLC: _LLD:
({void*_tmp143=_tmp70;Cyc_Unify_unify_it(_tmp143,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=9U,_tmp6A->f1=e2;_tmp6A;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Unify_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp67.f2)->tag == 39U){_LL7: _tmp71=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp67.f2)->f1;_LL8: {
# 274
void*_tmp6B=Cyc_Tcutil_compress(_tmp71);void*_tmp6C=_tmp6B;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6C)->tag == 1U){_LL11: _LL12:
({void*_tmp144=_tmp71;Cyc_Unify_unify_it(_tmp144,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=9U,_tmp6D->f1=e1;_tmp6D;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Unify_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 278
(int)_throw((void*)& Cyc_Unify_Unify_val);}}}}_LL0:;}
# 293 "unify.cyc"
static int Cyc_Unify_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2)&& Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2)&& Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple13{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 304
static void Cyc_Unify_unify_it(void*t1,void*t2){
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=_tag_fat(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp72=t1;struct Cyc_Core_Opt*_tmp80;void**_tmp7F;struct Cyc_Core_Opt*_tmp7E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp72)->tag == 1U){_LL1: _tmp80=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp72)->f1;_tmp7F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp72)->f2;_tmp7E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp72)->f4;_LL2:
# 315
 Cyc_Unify_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7E))->v,t2);{
struct Cyc_Absyn_Kind*_tmp73=Cyc_Tcutil_type_kind(t2);
# 320
if(Cyc_Tcutil_kind_leq(_tmp73,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp80))->v)){
*_tmp7F=t2;
return;}else{
# 324
{void*_tmp74=t2;struct Cyc_Absyn_PtrInfo _tmp7C;void**_tmp7B;struct Cyc_Core_Opt*_tmp7A;switch(*((int*)_tmp74)){case 1U: _LL6: _tmp7B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f2;_tmp7A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f4;_LL7: {
# 327
struct Cyc_List_List*_tmp75=(struct Cyc_List_List*)_tmp7E->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7A))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp75,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Unify_failure_reason=({const char*_tmp76="(type variable would escape scope)";_tag_fat(_tmp76,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}}}
# 334
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp80->v,_tmp73)){
*_tmp7B=t1;return;}
# 337
Cyc_Unify_failure_reason=({const char*_tmp77="(kinds are incompatible)";_tag_fat(_tmp77,sizeof(char),25U);});
goto _LL5;}case 3U: _LL8: _tmp7C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74)->f1;if((int)((struct Cyc_Absyn_Kind*)_tmp80->v)->kind == (int)Cyc_Absyn_BoxKind){_LL9: {
# 343
void*_tmp78=Cyc_Tcutil_compress((_tmp7C.ptr_atts).bounds);
{void*_tmp79=_tmp78;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp79)->tag == 1U){_LLD: _LLE:
# 346
({void*_tmp145=_tmp78;Cyc_Unify_unify(_tmp145,Cyc_Absyn_bounds_one());});
*_tmp7F=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 351
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 354
Cyc_Unify_failure_reason=({const char*_tmp7D="(kinds are incompatible)";_tag_fat(_tmp7D,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}};}else{_LL3: _LL4:
# 357
 goto _LL0;}_LL0:;}
# 362
{struct _tuple10 _tmp81=({struct _tuple10 _tmp11C;_tmp11C.f1=t2,_tmp11C.f2=t1;_tmp11C;});struct _tuple10 _tmp82=_tmp81;struct Cyc_List_List*_tmp110;struct Cyc_Absyn_Typedefdecl*_tmp10F;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Typedefdecl*_tmp10D;enum Cyc_Absyn_AggrKind _tmp10C;struct Cyc_List_List*_tmp10B;enum Cyc_Absyn_AggrKind _tmp10A;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp106;void*_tmp105;struct Cyc_Absyn_Tqual _tmp104;void*_tmp103;struct Cyc_List_List*_tmp102;int _tmp101;struct Cyc_Absyn_VarargInfo*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpF9;void*_tmpF8;struct Cyc_Absyn_Tqual _tmpF7;void*_tmpF6;struct Cyc_List_List*_tmpF5;int _tmpF4;struct Cyc_Absyn_VarargInfo*_tmpF3;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_List_List*_tmpED;void*_tmpEC;struct Cyc_Absyn_Tqual _tmpEB;struct Cyc_Absyn_Exp*_tmpEA;void*_tmpE9;void*_tmpE8;struct Cyc_Absyn_Tqual _tmpE7;struct Cyc_Absyn_Exp*_tmpE6;void*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;void*_tmpE2;struct Cyc_Absyn_Tqual _tmpE1;void*_tmpE0;void*_tmpDF;void*_tmpDE;void*_tmpDD;void*_tmpDC;struct Cyc_Absyn_Tqual _tmpDB;void*_tmpDA;void*_tmpD9;void*_tmpD8;void*_tmpD7;struct Cyc_Absyn_Tvar*_tmpD6;struct Cyc_Absyn_Tvar*_tmpD5;void*_tmpD4;struct Cyc_List_List*_tmpD3;void*_tmpD2;struct Cyc_List_List*_tmpD1;switch(*((int*)_tmp82.f1)){case 1U: _LL12: _LL13:
# 365
 Cyc_Unify_unify_it(t2,t1);return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f1)->f1)->tag == 9U){_LL14: _LL15:
# 367
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmpD4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f1)->f1;_tmpD3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f1)->f2;_tmpD2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->f1;_tmpD1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->f2;_LL21:
# 378
 if(Cyc_Tcutil_tycon_cmp(_tmpD4,_tmpD2)== 0){
Cyc_Unify_unify_list(_tmpD3,_tmpD1);
return;}else{
# 382
Cyc_Unify_failure_reason=({const char*_tmp84="(different type constructors)";_tag_fat(_tmp84,sizeof(char),30U);});}
goto _LL11;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f1)->f1)){case 8U: _LL18: _LL19:
# 369
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 371
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp82.f2)->f1)){case 9U: _LL16: _LL17:
# 368
 goto _LL19;case 8U: _LL1A: _LL1B:
# 370
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 373
 if(Cyc_Unify_unify_effect(t1,t2))return;
Cyc_Unify_failure_reason=({const char*_tmp83="(effects don't unify)";_tag_fat(_tmp83,sizeof(char),22U);});
goto _LL11;default: switch(*((int*)_tmp82.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp82.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp82.f2)->tag == 2U){_LL22: _tmpD6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp82.f1)->f1;_tmpD5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp82.f2)->f1;_LL23: {
# 386
struct _fat_ptr*_tmp85=_tmpD6->name;
struct _fat_ptr*_tmp86=_tmpD5->name;
# 389
int _tmp87=_tmpD6->identity;
int _tmp88=_tmpD5->identity;
if(_tmp88 == _tmp87)return;
Cyc_Unify_failure_reason=({const char*_tmp89="(variable types are not the same)";_tag_fat(_tmp89,sizeof(char),34U);});
goto _LL11;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f2)->tag == 3U){_LL24: _tmpE2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f1)->f1).elt_type;_tmpE1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f1)->f1).elt_tq;_tmpE0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f1)->f1).ptr_atts).rgn;_tmpDF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f1)->f1).ptr_atts).nullable;_tmpDE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f1)->f1).ptr_atts).bounds;_tmpDD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f1)->f1).ptr_atts).zero_term;_tmpDC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f2)->f1).elt_type;_tmpDB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f2)->f1).elt_tq;_tmpDA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f2)->f1).ptr_atts).rgn;_tmpD9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f2)->f1).ptr_atts).nullable;_tmpD8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f2)->f1).ptr_atts).bounds;_tmpD7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82.f2)->f1).ptr_atts).zero_term;_LL25:
# 397
 Cyc_Unify_unify_it(_tmpDC,_tmpE2);
Cyc_Unify_unify_it(_tmpE0,_tmpDA);
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;{
struct _fat_ptr _tmp8A=Cyc_Unify_failure_reason;
if(!Cyc_Unify_unify(_tmpD7,_tmpDD)){
Cyc_Unify_t1_failure=t1;Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmp8B="(not both zero terminated)";_tag_fat(_tmp8B,sizeof(char),27U);});
goto _LL11;}
# 407
Cyc_Unify_unify_tqual(_tmpDB,_tmpDC,_tmpE1,_tmpE2);
if(!Cyc_Unify_unify(_tmpD8,_tmpDE)){
Cyc_Unify_t1_failure=t1;Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmp8C="(different pointer bounds)";_tag_fat(_tmp8C,sizeof(char),27U);});
goto _LL11;}{
# 414
void*_tmp8D=Cyc_Tcutil_compress(_tmpD8);void*_tmp8E=_tmp8D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E)->f1)->tag == 14U){_LL35: _LL36:
# 416
 Cyc_Unify_failure_reason=_tmp8A;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 419
 Cyc_Unify_failure_reason=({const char*_tmp8F="(incompatible pointer types)";_tag_fat(_tmp8F,sizeof(char),29U);});
Cyc_Unify_unify_it(_tmpD9,_tmpDF);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp82.f2)->tag == 9U){_LL26: _tmpE4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp82.f1)->f1;_tmpE3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp82.f2)->f1;_LL27:
# 425
 if(!Cyc_Evexp_same_const_exp(_tmpE4,_tmpE3)){
Cyc_Unify_failure_reason=({const char*_tmp90="(cannot prove expressions are the same)";_tag_fat(_tmp90,sizeof(char),40U);});
goto _LL11;}
# 429
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f2)->tag == 4U){_LL28: _tmpEC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f1)->f1).elt_type;_tmpEB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f1)->f1).tq;_tmpEA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f1)->f1).num_elts;_tmpE9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f1)->f1).zero_term;_tmpE8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f2)->f1).elt_type;_tmpE7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f2)->f1).tq;_tmpE6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f2)->f1).num_elts;_tmpE5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp82.f2)->f1).zero_term;_LL29:
# 433
 Cyc_Unify_unify_it(_tmpE8,_tmpEC);
Cyc_Unify_unify_tqual(_tmpE7,_tmpE8,_tmpEB,_tmpEC);
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmp91="(not both zero terminated)";_tag_fat(_tmp91,sizeof(char),27U);});
Cyc_Unify_unify_it(_tmpE9,_tmpE5);
if(_tmpEA == _tmpE6)return;
if(_tmpEA == 0 || _tmpE6 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmpEA,_tmpE6))
return;
Cyc_Unify_failure_reason=({const char*_tmp92="(different array sizes)";_tag_fat(_tmp92,sizeof(char),24U);});
goto _LL11;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->tag == 5U){_LL2A: _tmp106=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).tvars;_tmp105=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).effect;_tmp104=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).ret_tqual;_tmp103=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).ret_type;_tmp102=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).args;_tmp101=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).c_varargs;_tmp100=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).cyc_varargs;_tmpFF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).rgn_po;_tmpFE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).attributes;_tmpFD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).requires_clause;_tmpFC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).requires_relns;_tmpFB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).ensures_clause;_tmpFA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f1)->f1).ensures_relns;_tmpF9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).tvars;_tmpF8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).effect;_tmpF7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).ret_tqual;_tmpF6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).ret_type;_tmpF5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).args;_tmpF4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).c_varargs;_tmpF3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).cyc_varargs;_tmpF2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).rgn_po;_tmpF1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).attributes;_tmpF0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).requires_clause;_tmpEF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).requires_relns;_tmpEE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).ensures_clause;_tmpED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp82.f2)->f1).ensures_relns;_LL2B: {
# 448
int done=0;
{struct _RegionHandle _tmp93=_new_region("rgn");struct _RegionHandle*rgn=& _tmp93;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmpF9 != 0){
if(_tmp106 == 0){
Cyc_Unify_failure_reason=({const char*_tmp94="(second function type has too few type variables)";_tag_fat(_tmp94,sizeof(char),50U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}{
# 456
void*_tmp95=((struct Cyc_Absyn_Tvar*)_tmpF9->hd)->kind;
void*_tmp96=((struct Cyc_Absyn_Tvar*)_tmp106->hd)->kind;
if(!Cyc_Unify_unify_kindbound(_tmp95,_tmp96)){
Cyc_Unify_failure_reason=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp99=({struct Cyc_String_pa_PrintArg_struct _tmp119;_tmp119.tag=0U,({
struct _fat_ptr _tmp146=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmpF9->hd));_tmp119.f1=_tmp146;});_tmp119;});struct Cyc_String_pa_PrintArg_struct _tmp9A=({struct Cyc_String_pa_PrintArg_struct _tmp118;_tmp118.tag=0U,({
struct _fat_ptr _tmp147=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmpF9->hd,& Cyc_Tcutil_bk)));_tmp118.f1=_tmp147;});_tmp118;});struct Cyc_String_pa_PrintArg_struct _tmp9B=({struct Cyc_String_pa_PrintArg_struct _tmp117;_tmp117.tag=0U,({
struct _fat_ptr _tmp148=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp106->hd,& Cyc_Tcutil_bk)));_tmp117.f1=_tmp148;});_tmp117;});void*_tmp97[3U];_tmp97[0]=& _tmp99,_tmp97[1]=& _tmp9A,_tmp97[2]=& _tmp9B;({struct _fat_ptr _tmp149=({const char*_tmp98="(type var %s has different kinds %s and %s)";_tag_fat(_tmp98,sizeof(char),44U);});Cyc_aprintf(_tmp149,_tag_fat(_tmp97,sizeof(void*),3U));});});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 465
inst=({struct Cyc_List_List*_tmp9D=_region_malloc(rgn,sizeof(*_tmp9D));({struct _tuple12*_tmp14B=({struct _tuple12*_tmp9C=_region_malloc(rgn,sizeof(*_tmp9C));_tmp9C->f1=(struct Cyc_Absyn_Tvar*)_tmp106->hd,({void*_tmp14A=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmpF9->hd);_tmp9C->f2=_tmp14A;});_tmp9C;});_tmp9D->hd=_tmp14B;}),_tmp9D->tl=inst;_tmp9D;});
_tmpF9=_tmpF9->tl;
_tmp106=_tmp106->tl;};}
# 469
if(_tmp106 != 0){
Cyc_Unify_failure_reason=({const char*_tmp9E="(second function type has too many type variables)";_tag_fat(_tmp9E,sizeof(char),51U);});
_npop_handler(0U);goto _LL11;}
# 473
if(inst != 0){
({void*_tmp14E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=5U,(_tmp9F->f1).tvars=0,(_tmp9F->f1).effect=_tmpF8,(_tmp9F->f1).ret_tqual=_tmpF7,(_tmp9F->f1).ret_type=_tmpF6,(_tmp9F->f1).args=_tmpF5,(_tmp9F->f1).c_varargs=_tmpF4,(_tmp9F->f1).cyc_varargs=_tmpF3,(_tmp9F->f1).rgn_po=_tmpF2,(_tmp9F->f1).attributes=_tmpF1,(_tmp9F->f1).requires_clause=_tmpFD,(_tmp9F->f1).requires_relns=_tmpFC,(_tmp9F->f1).ensures_clause=_tmpFB,(_tmp9F->f1).ensures_relns=_tmpFA;_tmp9F;});Cyc_Unify_unify_it(_tmp14E,({
# 477
struct _RegionHandle*_tmp14D=rgn;struct Cyc_List_List*_tmp14C=inst;Cyc_Tcutil_rsubstitute(_tmp14D,_tmp14C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));
_tmpA0->tag=5U,(_tmpA0->f1).tvars=0,(_tmpA0->f1).effect=_tmp105,(_tmpA0->f1).ret_tqual=_tmp104,(_tmpA0->f1).ret_type=_tmp103,(_tmpA0->f1).args=_tmp102,(_tmpA0->f1).c_varargs=_tmp101,(_tmpA0->f1).cyc_varargs=_tmp100,(_tmpA0->f1).rgn_po=_tmpFF,(_tmpA0->f1).attributes=_tmpFE,(_tmpA0->f1).requires_clause=_tmpF0,(_tmpA0->f1).requires_relns=_tmpEF,(_tmpA0->f1).ensures_clause=_tmpEE,(_tmpA0->f1).ensures_relns=_tmpED;_tmpA0;}));}));});
# 482
done=1;}}
# 450
;_pop_region(rgn);}
# 485
if(done)
return;
Cyc_Unify_unify_it(_tmpF6,_tmp103);
Cyc_Unify_unify_tqual(_tmpF7,_tmpF6,_tmp104,_tmp103);
for(0;_tmpF5 != 0 && _tmp102 != 0;(_tmpF5=_tmpF5->tl,_tmp102=_tmp102->tl)){
struct _tuple8 _tmpA1=*((struct _tuple8*)_tmpF5->hd);struct _tuple8 _tmpA2=_tmpA1;struct Cyc_Absyn_Tqual _tmpA8;void*_tmpA7;_LL3A: _tmpA8=_tmpA2.f2;_tmpA7=_tmpA2.f3;_LL3B:;{
struct _tuple8 _tmpA3=*((struct _tuple8*)_tmp102->hd);struct _tuple8 _tmpA4=_tmpA3;struct Cyc_Absyn_Tqual _tmpA6;void*_tmpA5;_LL3D: _tmpA6=_tmpA4.f2;_tmpA5=_tmpA4.f3;_LL3E:;
Cyc_Unify_unify_it(_tmpA7,_tmpA5);
Cyc_Unify_unify_tqual(_tmpA8,_tmpA7,_tmpA6,_tmpA5);};}
# 495
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
if(_tmpF5 != 0 || _tmp102 != 0){
Cyc_Unify_failure_reason=({const char*_tmpA9="(function types have different number of arguments)";_tag_fat(_tmpA9,sizeof(char),52U);});
goto _LL11;}
# 501
if(_tmpF4 != _tmp101){
Cyc_Unify_failure_reason=({const char*_tmpAA="(only one function type takes C varargs)";_tag_fat(_tmpAA,sizeof(char),41U);});
goto _LL11;}{
# 506
int bad_cyc_vararg=0;
{struct _tuple13 _tmpAB=({struct _tuple13 _tmp11A;_tmp11A.f1=_tmpF3,_tmp11A.f2=_tmp100;_tmp11A;});struct _tuple13 _tmpAC=_tmpAB;struct _fat_ptr*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;void*_tmpB4;int _tmpB3;struct _fat_ptr*_tmpB2;struct Cyc_Absyn_Tqual _tmpB1;void*_tmpB0;int _tmpAF;if(_tmpAC.f1 == 0){if(_tmpAC.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmpAC.f2 == 0){_LL44: _LL45:
# 511
 bad_cyc_vararg=1;
Cyc_Unify_failure_reason=({const char*_tmpAD="(only one function type takes varargs)";_tag_fat(_tmpAD,sizeof(char),39U);});
goto _LL3F;}else{_LL46: _tmpB6=(_tmpAC.f1)->name;_tmpB5=(_tmpAC.f1)->tq;_tmpB4=(_tmpAC.f1)->type;_tmpB3=(_tmpAC.f1)->inject;_tmpB2=(_tmpAC.f2)->name;_tmpB1=(_tmpAC.f2)->tq;_tmpB0=(_tmpAC.f2)->type;_tmpAF=(_tmpAC.f2)->inject;_LL47:
# 515
 Cyc_Unify_unify_it(_tmpB4,_tmpB0);
Cyc_Unify_unify_tqual(_tmpB5,_tmpB4,_tmpB1,_tmpB0);
if(_tmpB3 != _tmpAF){
bad_cyc_vararg=1;
Cyc_Unify_failure_reason=({const char*_tmpAE="(only one function type injects varargs)";_tag_fat(_tmpAE,sizeof(char),41U);});}
# 521
goto _LL3F;}}_LL3F:;}
# 523
if(bad_cyc_vararg)goto _LL11;{
# 526
int bad_effect=0;
{struct _tuple10 _tmpB7=({struct _tuple10 _tmp11B;_tmp11B.f1=_tmpF8,_tmp11B.f2=_tmp105;_tmp11B;});struct _tuple10 _tmpB8=_tmpB7;if(_tmpB8.f1 == 0){if(_tmpB8.f2 == 0){_LL49: _LL4A:
 goto _LL48;}else{_LL4B: _LL4C:
 goto _LL4E;}}else{if(_tmpB8.f2 == 0){_LL4D: _LL4E:
 bad_effect=1;goto _LL48;}else{_LL4F: _LL50:
 bad_effect=!({void*_tmp14F=(void*)_check_null(_tmpF8);Cyc_Unify_unify_effect(_tmp14F,(void*)_check_null(_tmp105));});goto _LL48;}}_LL48:;}
# 533
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
if(bad_effect){
Cyc_Unify_failure_reason=({const char*_tmpB9="(function effects do not match)";_tag_fat(_tmpB9,sizeof(char),32U);});
goto _LL11;}
# 539
if(!Cyc_Tcutil_same_atts(_tmpFE,_tmpF1)){
Cyc_Unify_failure_reason=({const char*_tmpBA="(function types have different attributes)";_tag_fat(_tmpBA,sizeof(char),43U);});
goto _LL11;}
# 543
if(!Cyc_Tcutil_same_rgn_po(_tmpFF,_tmpF2)){
Cyc_Unify_failure_reason=({const char*_tmpBB="(function types have different region lifetime orderings)";_tag_fat(_tmpBB,sizeof(char),58U);});
goto _LL11;}
# 547
if(!Cyc_Unify_check_logical_equivalence(_tmpFC,_tmpEF)){
Cyc_Unify_failure_reason=({const char*_tmpBC="(@requires clauses not equivalent)";_tag_fat(_tmpBC,sizeof(char),35U);});
goto _LL11;}
# 551
if(!Cyc_Unify_check_logical_equivalence(_tmpFA,_tmpED)){
Cyc_Unify_failure_reason=({const char*_tmpBD="(@ensures clauses not equivalent)";_tag_fat(_tmpBD,sizeof(char),34U);});
goto _LL11;}
# 555
return;};};}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp82.f2)->tag == 6U){_LL2C: _tmp108=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp82.f1)->f1;_tmp107=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp82.f2)->f1;_LL2D:
# 558
 for(0;_tmp107 != 0 && _tmp108 != 0;(_tmp107=_tmp107->tl,_tmp108=_tmp108->tl)){
struct _tuple11 _tmpBE=*((struct _tuple11*)_tmp107->hd);struct _tuple11 _tmpBF=_tmpBE;struct Cyc_Absyn_Tqual _tmpC5;void*_tmpC4;_LL52: _tmpC5=_tmpBF.f1;_tmpC4=_tmpBF.f2;_LL53:;{
struct _tuple11 _tmpC0=*((struct _tuple11*)_tmp108->hd);struct _tuple11 _tmpC1=_tmpC0;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC2;_LL55: _tmpC3=_tmpC1.f1;_tmpC2=_tmpC1.f2;_LL56:;
Cyc_Unify_unify_it(_tmpC4,_tmpC2);
Cyc_Unify_unify_tqual(_tmpC5,_tmpC4,_tmpC3,_tmpC2);};}
# 564
if(_tmp107 == 0 && _tmp108 == 0)return;
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmpC6="(tuple types have different numbers of components)";_tag_fat(_tmpC6,sizeof(char),51U);});
goto _LL11;}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp82.f2)->tag == 7U){_LL2E: _tmp10C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp82.f1)->f1;_tmp10B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp82.f1)->f2;_tmp10A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp82.f2)->f1;_tmp109=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp82.f2)->f2;_LL2F:
# 571
 if((int)_tmp10A != (int)_tmp10C){Cyc_Unify_failure_reason=({const char*_tmpC7="(struct and union type)";_tag_fat(_tmpC7,sizeof(char),24U);});goto _LL11;}
for(0;_tmp109 != 0 && _tmp10B != 0;(_tmp109=_tmp109->tl,_tmp10B=_tmp10B->tl)){
struct Cyc_Absyn_Aggrfield*_tmpC8=(struct Cyc_Absyn_Aggrfield*)_tmp109->hd;
struct Cyc_Absyn_Aggrfield*_tmpC9=(struct Cyc_Absyn_Aggrfield*)_tmp10B->hd;
if(Cyc_strptrcmp(_tmpC8->name,_tmpC9->name)!= 0){
Cyc_Unify_failure_reason=({const char*_tmpCA="(different member names)";_tag_fat(_tmpCA,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 579
Cyc_Unify_unify_it(_tmpC8->type,_tmpC9->type);
Cyc_Unify_unify_tqual(_tmpC8->tq,_tmpC8->type,_tmpC9->tq,_tmpC9->type);
if(!Cyc_Tcutil_same_atts(_tmpC8->attributes,_tmpC9->attributes)){
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmpCB="(different attributes on member)";_tag_fat(_tmpCB,sizeof(char),33U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 587
if((_tmpC8->width != 0 && _tmpC9->width == 0 ||
 _tmpC9->width != 0 && _tmpC8->width == 0)||
(_tmpC8->width != 0 && _tmpC9->width != 0)&& !({
struct Cyc_Absyn_Exp*_tmp150=(struct Cyc_Absyn_Exp*)_check_null(_tmpC8->width);Cyc_Evexp_same_const_exp(_tmp150,(struct Cyc_Absyn_Exp*)_check_null(_tmpC9->width));})){
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmpCC="(different bitfield widths on member)";_tag_fat(_tmpCC,sizeof(char),38U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 596
if((_tmpC8->requires_clause != 0 && _tmpC9->requires_clause == 0 ||
 _tmpC8->requires_clause == 0 && _tmpC9->requires_clause != 0)||
(_tmpC8->requires_clause == 0 && _tmpC9->requires_clause != 0)&& !({
struct Cyc_Absyn_Exp*_tmp151=(struct Cyc_Absyn_Exp*)_check_null(_tmpC8->requires_clause);Cyc_Evexp_same_const_exp(_tmp151,(struct Cyc_Absyn_Exp*)_check_null(_tmpC9->requires_clause));})){
# 601
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmpCD="(different @requires clauses on member)";_tag_fat(_tmpCD,sizeof(char),40U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}}
# 607
if(_tmp109 == 0 && _tmp10B == 0)return;
Cyc_Unify_t1_failure=t1;
Cyc_Unify_t2_failure=t2;
Cyc_Unify_failure_reason=({const char*_tmpCE="(different number of members)";_tag_fat(_tmpCE,sizeof(char),30U);});
goto _LL11;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp82.f2)->tag == 8U){_LL30: _tmp110=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp82.f1)->f2;_tmp10F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp82.f1)->f3;_tmp10E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp82.f2)->f2;_tmp10D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp82.f2)->f3;_LL31:
# 613
 if(_tmp10F != _tmp10D){
Cyc_Unify_failure_reason=({const char*_tmpCF="(different abstract typedefs)";_tag_fat(_tmpCF,sizeof(char),30U);});
goto _LL11;}
# 617
Cyc_Unify_failure_reason=({const char*_tmpD0="(type parameters to typedef differ)";_tag_fat(_tmpD0,sizeof(char),36U);});
Cyc_Unify_unify_list(_tmp110,_tmp10E);
return;}else{goto _LL32;}default: _LL32: _LL33:
 goto _LL11;}}}_LL11:;}
# 622
(int)_throw((void*)& Cyc_Unify_Unify_val);}

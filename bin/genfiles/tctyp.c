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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 117
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 292
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 305
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 312
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 879 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 881
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 893
void*Cyc_Absyn_compress_kb(void*);
# 899
int Cyc_Absyn_type2bool(int def,void*);
# 904
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 916
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 918
extern void*Cyc_Absyn_empty_effect;
# 920
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 922
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);
# 941
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar();
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar();
# 951
void*Cyc_Absyn_bounds_one();
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1137
int Cyc_Absyn_fntype_att(void*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_RgnOrder_RgnPO;
# 37 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
# 43
int Cyc_Tcutil_is_function_type(void*);
# 45
int Cyc_Tcutil_is_array_type(void*);
# 64
void*Cyc_Tcutil_pointer_elt_type(void*);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 87
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 94
void*Cyc_Tcutil_copy_type(void*);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
# 109
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 147
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 149
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
# 151
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 202
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 212
void Cyc_Tcutil_check_bitfield(unsigned,void*field_typ,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn);
# 214
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 258
int Cyc_Tcutil_new_tvar_id();
# 273
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 280
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 287
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 298
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 305
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 313
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 316
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 73 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 77
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 80
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 40 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tctyp_CVTEnv{unsigned loc;struct Cyc_Tcenv_Tenv*te;struct Cyc_List_List*kind_env;int fn_result;int generalize_evars;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;};
# 81 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*,void*,int,int);struct _tuple13{void*f1;int f2;};
# 87
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct Cyc_List_List*es,void*e,int b){
void*_tmp0=Cyc_Tcutil_compress(e);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;int _tmp2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->tag == 1U){_LL1: _tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->f3;_LL2: {int i=_tmp2;
# 90
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple13*_tmp3=(struct _tuple13*)es2->hd;struct _tuple13*_stmttmp1=_tmp3;struct _tuple13*_tmp4=_stmttmp1;int*_tmp6;void*_tmp5;_LL6: _tmp5=_tmp4->f1;_tmp6=(int*)& _tmp4->f2;_LL7: {void*t=_tmp5;int*b2=_tmp6;
void*_tmp7=Cyc_Tcutil_compress(t);void*_stmttmp2=_tmp7;void*_tmp8=_stmttmp2;int _tmp9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8)->tag == 1U){_LL9: _tmp9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8)->f3;_LLA: {int j=_tmp9;
# 94
if(i == j){
if(b != *b2)*b2=1;
return es;}
# 98
goto _LL8;}}else{_LLB: _LLC:
 goto _LL8;}_LL8:;}}}
# 102
return({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));({struct _tuple13*_tmp319=({struct _tuple13*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=e,_tmpA->f2=b;_tmpA;});_tmpB->hd=_tmp319;}),_tmpB->tl=es;_tmpB;});}}else{_LL3: _LL4:
 return es;}_LL0:;}
# 108
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp292;_tmp292.tag=0U,({struct _fat_ptr _tmp31A=({const char*_tmp11="bound tvar id for ";_tag_fat(_tmp11,sizeof(char),19U);});_tmp292.f1=_tmp31A;});_tmp292;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpE=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp291;_tmp291.tag=7U,_tmp291.f1=tv;_tmp291;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp290;_tmp290.tag=0U,({struct _fat_ptr _tmp31B=({const char*_tmp10=" is NULL";_tag_fat(_tmp10,sizeof(char),9U);});_tmp290.f1=_tmp31B;});_tmp290;});void*_tmpC[3U];_tmpC[0]=& _tmpD,_tmpC[1]=& _tmpE,_tmpC[2]=& _tmpF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC,sizeof(void*),3U));});
return({struct Cyc_List_List*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=tv,_tmp12->tl=tvs;_tmp12;});}
# 113
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(void*)){
# 115
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_List_List*b=btvs;
for(0;b != 0;b=b->tl){
if(({int _tmp31C=(get(tvs->hd))->identity;_tmp31C == ((struct Cyc_Absyn_Tvar*)b->hd)->identity;}))
break;}
if(b == 0)
r=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=tvs->hd,_tmp13->tl=r;_tmp13;});}
# 124
r=Cyc_List_imp_rev(r);
return r;}
# 132
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(*((struct _tuple13*)free_evars->hd)).f1;
{void*_tmp14=Cyc_Tcutil_compress(e);void*_stmttmp3=_tmp14;void*_tmp15=_stmttmp3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
 continue;}_LL0:;}{
# 139
void*t;
struct _fat_ptr s;
{struct Cyc_Absyn_Kind*_tmp16=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_stmttmp4=_tmp16;struct Cyc_Absyn_Kind*_tmp17=_stmttmp4;switch(((struct Cyc_Absyn_Kind*)_tmp17)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp17)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
 t=Cyc_Absyn_unique_rgn_type;s=({const char*_tmp18="unique region";_tag_fat(_tmp18,sizeof(char),14U);});goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
 t=Cyc_Absyn_heap_rgn_type;s=({const char*_tmp19="heap";_tag_fat(_tmp19,sizeof(char),5U);});goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
 t=Cyc_Absyn_empty_effect;s=({const char*_tmp1A="{}";_tag_fat(_tmp1A,sizeof(char),3U);});goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
 t=Cyc_Absyn_false_type;s=({const char*_tmp1B="@false";_tag_fat(_tmp1B,sizeof(char),7U);});goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
 t=Cyc_Absyn_bounds_one();s=({const char*_tmp1C="bounds_one";_tag_fat(_tmp1C,sizeof(char),11U);});goto _LL5;default: _LL12: _LL13:
# 149
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp296;_tmp296.tag=0U,({struct _fat_ptr _tmp31D=({const char*_tmp23="hidden type variable ";_tag_fat(_tmp23,sizeof(char),22U);});_tmp296.f1=_tmp31D;});_tmp296;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp295;_tmp295.tag=2U,_tmp295.f1=(void*)e;_tmp295;});struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp294;_tmp294.tag=0U,({struct _fat_ptr _tmp31E=({const char*_tmp22=" isn't abstracted in type ";_tag_fat(_tmp22,sizeof(char),27U);});_tmp294.f1=_tmp31E;});_tmp294;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp293;_tmp293.tag=2U,_tmp293.f1=(void*)in_typ;_tmp293;});void*_tmp1D[4U];_tmp1D[0]=& _tmp1E,_tmp1D[1]=& _tmp1F,_tmp1D[2]=& _tmp20,_tmp1D[3]=& _tmp21;({unsigned _tmp31F=loc;Cyc_Warn_err2(_tmp31F,_tag_fat(_tmp1D,sizeof(void*),4U));});});
return;}_LL5:;}
# 152
if(!Cyc_Unify_unify(e,t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29B;_tmp29B.tag=0U,({struct _fat_ptr _tmp320=({const char*_tmp2C="can't unify evar ";_tag_fat(_tmp2C,sizeof(char),18U);});_tmp29B.f1=_tmp320;});_tmp29B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp26=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp29A;_tmp29A.tag=2U,_tmp29A.f1=(void*)e;_tmp29A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27=({struct Cyc_Warn_String_Warn_Warg_struct _tmp299;_tmp299.tag=0U,({struct _fat_ptr _tmp321=({const char*_tmp2B=" with ";_tag_fat(_tmp2B,sizeof(char),7U);});_tmp299.f1=_tmp321;});_tmp299;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28=({struct Cyc_Warn_String_Warn_Warg_struct _tmp298;_tmp298.tag=0U,_tmp298.f1=s;_tmp298;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmp297;_tmp297.tag=0U,({struct _fat_ptr _tmp322=({const char*_tmp2A="!";_tag_fat(_tmp2A,sizeof(char),2U);});_tmp297.f1=_tmp322;});_tmp297;});void*_tmp24[5U];_tmp24[0]=& _tmp25,_tmp24[1]=& _tmp26,_tmp24[2]=& _tmp27,_tmp24[3]=& _tmp28,_tmp24[4]=& _tmp29;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp24,sizeof(void*),5U));});}}}
# 161
static int Cyc_Tctyp_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp2D=({struct _tuple0 _tmp29C;_tmp29C.f1=c1,_tmp29C.f2=c2;_tmp29C;});struct _tuple0 _stmttmp5=_tmp2D;struct _tuple0 _tmp2E=_stmttmp5;struct Cyc_Absyn_Kind*_tmp32;struct Cyc_Core_Opt**_tmp31;struct Cyc_Absyn_Kind*_tmp30;struct Cyc_Core_Opt**_tmp2F;struct Cyc_Absyn_Kind*_tmp35;struct Cyc_Absyn_Kind*_tmp34;struct Cyc_Core_Opt**_tmp33;struct Cyc_Core_Opt**_tmp36;struct Cyc_Core_Opt**_tmp37;struct Cyc_Absyn_Kind*_tmp3A;struct Cyc_Core_Opt**_tmp39;struct Cyc_Absyn_Kind*_tmp38;struct Cyc_Absyn_Kind*_tmp3C;struct Cyc_Absyn_Kind*_tmp3B;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f1)->tag == 0U)switch(*((int*)_tmp2E.f2)){case 0U: _LL1: _tmp3B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp3C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_LL2: {struct Cyc_Absyn_Kind*k1=_tmp3B;struct Cyc_Absyn_Kind*k2=_tmp3C;
return k1 == k2;}case 1U: goto _LL3;default: _LL9: _tmp38=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp39=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_tmp3A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f2;_LLA: {struct Cyc_Absyn_Kind*k1=_tmp38;struct Cyc_Core_Opt**f=_tmp39;struct Cyc_Absyn_Kind*k2=_tmp3A;
# 175
if(!Cyc_Tcutil_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_tmp323=({struct Cyc_Core_Opt*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->v=c1;_tmp40;});*f=_tmp323;});
return 1;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f2)->tag == 1U){_LL3: _tmp37=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp37;
# 167
({struct Cyc_Core_Opt*_tmp324=({struct Cyc_Core_Opt*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->v=c1;_tmp3D;});*f=_tmp324;});return 1;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f1)->tag == 1U){_LL5: _tmp36=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp36;
({struct Cyc_Core_Opt*_tmp325=({struct Cyc_Core_Opt*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->v=c2;_tmp3E;});*f=_tmp325;});return 1;}}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f2)->tag == 0U){_LL7: _tmp33=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp34=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f2;_tmp35=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp33;struct Cyc_Absyn_Kind*k1=_tmp34;struct Cyc_Absyn_Kind*k2=_tmp35;
# 170
if(!Cyc_Tcutil_kind_leq(k2,k1))
return 0;
({struct Cyc_Core_Opt*_tmp326=({struct Cyc_Core_Opt*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->v=c2;_tmp3F;});*f=_tmp326;});
return 1;}}else{_LLB: _tmp2F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp30=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f2;_tmp31=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_tmp32=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f2;_LLC: {struct Cyc_Core_Opt**f1=_tmp2F;struct Cyc_Absyn_Kind*k1=_tmp30;struct Cyc_Core_Opt**f2=_tmp31;struct Cyc_Absyn_Kind*k2=_tmp32;
# 180
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp327=({struct Cyc_Core_Opt*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->v=c1;_tmp41;});*f2=_tmp327;});
return 1;}
# 184
if(Cyc_Tcutil_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp328=({struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=c2;_tmp42;});*f1=_tmp328;});
return 1;}
# 188
return 0;}}}}}_LL0:;}}
# 197
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tctyp_constrain_kinds(k1,k2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A2;_tmp2A2.tag=0U,({struct _fat_ptr _tmp329=({const char*_tmp4C="type variable ";_tag_fat(_tmp4C,sizeof(char),15U);});_tmp2A2.f1=_tmp329;});_tmp2A2;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp45=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2A1;_tmp2A1.tag=7U,_tmp2A1.f1=tv;_tmp2A1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp46=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A0;_tmp2A0.tag=0U,({struct _fat_ptr _tmp32A=({const char*_tmp4B=" used with inconsistent kinds ";_tag_fat(_tmp4B,sizeof(char),31U);});_tmp2A0.f1=_tmp32A;});_tmp2A0;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp47=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp29F;_tmp29F.tag=8U,_tmp29F.f1=(void*)k1;_tmp29F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp48=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E;_tmp29E.tag=0U,({
struct _fat_ptr _tmp32B=({const char*_tmp4A=" and ";_tag_fat(_tmp4A,sizeof(char),6U);});_tmp29E.f1=_tmp32B;});_tmp29E;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp49=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp29D;_tmp29D.tag=8U,_tmp29D.f1=(void*)k2;_tmp29D;});void*_tmp43[6U];_tmp43[0]=& _tmp44,_tmp43[1]=& _tmp45,_tmp43[2]=& _tmp46,_tmp43[3]=& _tmp47,_tmp43[4]=& _tmp48,_tmp43[5]=& _tmp49;({unsigned _tmp32C=loc;Cyc_Warn_err2(_tmp32C,_tag_fat(_tmp43,sizeof(void*),6U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A3;_tmp2A3.tag=0U,({struct _fat_ptr _tmp32D=({const char*_tmp4F="same type variable has different identity!";_tag_fat(_tmp4F,sizeof(char),43U);});_tmp2A3.f1=_tmp32D;});_tmp2A3;});void*_tmp4D[1U];_tmp4D[0]=& _tmp4E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4D,sizeof(void*),1U));});}
return tvs;}}}
# 211
({int _tmp32E=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp32E;});
return({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=tv,_tmp50->tl=tvs;_tmp50;});}
# 217
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A4;_tmp2A4.tag=0U,({struct _fat_ptr _tmp32F=({const char*_tmp53="fast_add_free_tvar: bad identity in tv";_tag_fat(_tmp53,sizeof(char),39U);});_tmp2A4.f1=_tmp32F;});_tmp2A4;});void*_tmp51[1U];_tmp51[0]=& _tmp52;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp51,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp54=(struct Cyc_Absyn_Tvar*)tvs2->hd;struct Cyc_Absyn_Tvar*tv2=_tmp54;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5;_tmp2A5.tag=0U,({struct _fat_ptr _tmp330=({const char*_tmp57="fast_add_free_tvar: bad identity in tvs2";_tag_fat(_tmp57,sizeof(char),41U);});_tmp2A5.f1=_tmp330;});_tmp2A5;});void*_tmp55[1U];_tmp55[0]=& _tmp56;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp55,sizeof(void*),1U));});
if(tv2->identity == tv->identity)
return tvs;}}
# 227
return({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->hd=tv,_tmp58->tl=tvs;_tmp58;});}struct _tuple14{struct Cyc_Absyn_Tvar*f1;int f2;};
# 233
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A6;_tmp2A6.tag=0U,({struct _fat_ptr _tmp331=({const char*_tmp5B="fast_add_free_tvar_bool: bad identity in tv";_tag_fat(_tmp5B,sizeof(char),44U);});_tmp2A6.f1=_tmp331;});_tmp2A6;});void*_tmp59[1U];_tmp59[0]=& _tmp5A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp59,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct _tuple14*_tmp5C=(struct _tuple14*)tvs2->hd;struct _tuple14*_stmttmp6=_tmp5C;struct _tuple14*_tmp5D=_stmttmp6;int*_tmp5F;struct Cyc_Absyn_Tvar*_tmp5E;_LL1: _tmp5E=_tmp5D->f1;_tmp5F=(int*)& _tmp5D->f2;_LL2: {struct Cyc_Absyn_Tvar*tv2=_tmp5E;int*b2=_tmp5F;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp61=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A7;_tmp2A7.tag=0U,({struct _fat_ptr _tmp332=({const char*_tmp62="fast_add_free_tvar_bool: bad identity in tvs2";_tag_fat(_tmp62,sizeof(char),46U);});_tmp2A7.f1=_tmp332;});_tmp2A7;});void*_tmp60[1U];_tmp60[0]=& _tmp61;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp60,sizeof(void*),1U));});
if(tv2->identity == tv->identity){
*b2=*b2 || b;
return tvs;}}}}
# 245
return({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));({struct _tuple14*_tmp333=({struct _tuple14*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->f1=tv,_tmp63->f2=b;_tmp63;});_tmp64->hd=_tmp333;}),_tmp64->tl=tvs;_tmp64;});}
# 252
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tctyp_CVTEnv env){
env.loc=e->loc;
{void*_tmp65=e->r;void*_stmttmp7=_tmp65;void*_tmp66=_stmttmp7;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp68;void*_tmp69;void*_tmp6A;void*_tmp6B;struct Cyc_Absyn_Exp*_tmp6D;void*_tmp6C;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*_tmp73;struct Cyc_Absyn_Exp*_tmp72;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_List_List*_tmp77;switch(*((int*)_tmp66)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp77=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LLC: {struct Cyc_List_List*es=_tmp77;
# 261
for(0;es != 0;es=es->tl){
env=Cyc_Tctyp_i_check_type_level_exp((struct Cyc_Absyn_Exp*)es->hd,env);}
goto _LL0;}case 6U: _LLD: _tmp74=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp75=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_tmp76=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp74;struct Cyc_Absyn_Exp*e2=_tmp75;struct Cyc_Absyn_Exp*e3=_tmp76;
# 265
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
_tmp72=e2;_tmp73=e3;goto _LL10;}case 7U: _LLF: _tmp72=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp73=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp72;struct Cyc_Absyn_Exp*e2=_tmp73;
_tmp70=e1;_tmp71=e2;goto _LL12;}case 8U: _LL11: _tmp70=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp71=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp70;struct Cyc_Absyn_Exp*e2=_tmp71;
_tmp6E=e1;_tmp6F=e2;goto _LL14;}case 9U: _LL13: _tmp6E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp6F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp6E;struct Cyc_Absyn_Exp*e2=_tmp6F;
# 270
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type_level_exp(e2,env);
goto _LL0;}case 14U: _LL15: _tmp6C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp6D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL16: {void*t=_tmp6C;struct Cyc_Absyn_Exp*e1=_tmp6D;
# 274
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tak,t,1,0);
goto _LL0;}case 19U: _LL17: _tmp6B=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL18: {void*t=_tmp6B;
_tmp6A=t;goto _LL1A;}case 17U: _LL19: _tmp6A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL1A: {void*t=_tmp6A;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tak,t,1,0);goto _LL0;}case 39U: _LL1B: _tmp69=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL1C: {void*t=_tmp69;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_ik,t,1,0);goto _LL0;}case 18U: _LL1D: _tmp68=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp68;
_tmp67=e;goto _LL20;}case 41U: _LL1F: _tmp67=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp67;
env=Cyc_Tctyp_i_check_type_level_exp(e,env);goto _LL0;}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp79=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A8;_tmp2A8.tag=0U,({struct _fat_ptr _tmp334=({const char*_tmp7A="bad expression in Tctyp::i_check_type_level_exp";_tag_fat(_tmp7A,sizeof(char),48U);});_tmp2A8.f1=_tmp334;});_tmp2A8;});void*_tmp78[1U];_tmp78[0]=& _tmp79;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp78,sizeof(void*),1U));});}_LL0:;}
# 284
return env;}
# 287
static int Cyc_Tctyp_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td == 0)
return 1;
if(td->defn == 0)
return 0;{
void*_tmp7B=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_stmttmp8=_tmp7B;void*_tmp7C=_stmttmp8;void*_tmp7D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->tag == 3U){_LL1: _tmp7D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp7D;
# 294
void*_tmp7E=Cyc_Tcutil_compress(r);void*_stmttmp9=_tmp7E;void*_tmp7F=_stmttmp9;struct Cyc_Absyn_Tvar*_tmp80;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->tag == 2U){_LL6: _tmp80=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->f1;_LL7: {struct Cyc_Absyn_Tvar*tv=_tmp80;
return Cyc_Absyn_tvar_cmp(tvar,tv)== 0;}}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 298
 return 0;}_LL0:;}}
# 302
static struct Cyc_Absyn_Kind*Cyc_Tctyp_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 304
void*_tmp81=Cyc_Absyn_compress_kb(tvar->kind);void*_stmttmpA=_tmp81;void*_tmp82=_stmttmpA;switch(*((int*)_tmp82)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp82)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp82)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp82)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp82)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4: {
# 313
enum Cyc_Absyn_KindQual _tmp83=expected_kind->kind;enum Cyc_Absyn_KindQual k=_tmp83;
if((((int)k == (int)2U ||(int)k == (int)1U)||(int)k == (int)0U)&&
 Cyc_Tctyp_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}
# 321
return& Cyc_Tcutil_trk;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 325
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,struct _tuple1*typname,struct Cyc_Absyn_Typedefdecl*td){
# 330
struct Cyc_List_List*_tmp84=*targsp;struct Cyc_List_List*targs=_tmp84;
for(0;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 336
{struct _tuple0 _tmp85=({struct _tuple0 _tmp2A9;({void*_tmp335=Cyc_Absyn_compress_kb(tv->kind);_tmp2A9.f1=_tmp335;}),_tmp2A9.f2=t;_tmp2A9;});struct _tuple0 _stmttmpB=_tmp85;struct _tuple0 _tmp86=_stmttmpB;struct Cyc_Absyn_Tvar*_tmp87;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp86.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp86.f2)->tag == 2U){_LL1: _tmp87=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp86.f2)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv2=_tmp87;
# 338
({struct Cyc_List_List*_tmp336=Cyc_Tctyp_add_free_tvar(env.loc,env.kind_env,tv2);env.kind_env=_tmp336;});
({struct Cyc_List_List*_tmp337=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_tmp337;});
continue;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 343
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
env=Cyc_Tctyp_i_check_type(env,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(env.loc,t);}}
# 347
if(targs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AB;_tmp2AB.tag=0U,({struct _fat_ptr _tmp338=({const char*_tmp8B="too many type arguments for ";_tag_fat(_tmp8B,sizeof(char),29U);});_tmp2AB.f1=_tmp338;});_tmp2AB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2AA;_tmp2AA.tag=1U,_tmp2AA.f1=typname;_tmp2AA;});void*_tmp88[2U];_tmp88[0]=& _tmp89,_tmp88[1]=& _tmp8A;({unsigned _tmp339=env.loc;Cyc_Warn_err2(_tmp339,_tag_fat(_tmp88,sizeof(void*),2U));});});{
# 350
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=e,_tmp8C->tl=hidden_ts;_tmp8C;});{
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
env=Cyc_Tctyp_i_check_type(env,k,e,1,allow_abs_aggr);}}
# 357
({struct Cyc_List_List*_tmp33B=({struct Cyc_List_List*_tmp33A=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp33A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmp33B;});
return env;}}
# 361
static void Cyc_Tctyp_check_field_atts(unsigned loc,struct _fat_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp8D=(void*)atts->hd;void*_stmttmpC=_tmp8D;void*_tmp8E=_stmttmpC;switch(*((int*)_tmp8E)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AF;_tmp2AF.tag=0U,({struct _fat_ptr _tmp33C=({const char*_tmp95="bad attribute ";_tag_fat(_tmp95,sizeof(char),15U);});_tmp2AF.f1=_tmp33C;});_tmp2AF;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp91=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp2AE;_tmp2AE.tag=10U,_tmp2AE.f1=(void*)atts->hd;_tmp2AE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp92=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AD;_tmp2AD.tag=0U,({struct _fat_ptr _tmp33D=({const char*_tmp94=" on member ";_tag_fat(_tmp94,sizeof(char),12U);});_tmp2AD.f1=_tmp33D;});_tmp2AD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp93=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AC;_tmp2AC.tag=0U,_tmp2AC.f1=*fn;_tmp2AC;});void*_tmp8F[4U];_tmp8F[0]=& _tmp90,_tmp8F[1]=& _tmp91,_tmp8F[2]=& _tmp92,_tmp8F[3]=& _tmp93;({unsigned _tmp33E=loc;Cyc_Warn_err2(_tmp33E,_tag_fat(_tmp8F,sizeof(void*),4U));});});}_LL0:;}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f1;struct Cyc_List_List*f2;};
# 371
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 374
if(clause == 0)
return({struct _tuple15 _tmp2B0;_tmp2B0.f1=env,_tmp2B0.f2=0;_tmp2B0;});
Cyc_Tcexp_tcExp(env.te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp97=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4;_tmp2B4.tag=0U,_tmp2B4.f1=clause_name;_tmp2B4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B3;_tmp2B3.tag=0U,({struct _fat_ptr _tmp33F=({const char*_tmp9C=" clause has type ";_tag_fat(_tmp9C,sizeof(char),18U);});_tmp2B3.f1=_tmp33F;});_tmp2B3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp99=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp2B2;_tmp2B2.tag=3U,_tmp2B2.f1=(void*)clause->topt;_tmp2B2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B1;_tmp2B1.tag=0U,({
struct _fat_ptr _tmp340=({const char*_tmp9B=" instead of integral type";_tag_fat(_tmp9B,sizeof(char),26U);});_tmp2B1.f1=_tmp340;});_tmp2B1;});void*_tmp96[4U];_tmp96[0]=& _tmp97,_tmp96[1]=& _tmp98,_tmp96[2]=& _tmp99,_tmp96[3]=& _tmp9A;({unsigned _tmp341=env.loc;Cyc_Warn_err2(_tmp341,_tag_fat(_tmp96,sizeof(void*),4U));});});
env=Cyc_Tctyp_i_check_type_level_exp(clause,env);{
struct Cyc_List_List*_tmp9D=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);struct Cyc_List_List*relns=_tmp9D;
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B8;_tmp2B8.tag=0U,_tmp2B8.f1=clause_name;_tmp2B8;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B7;_tmp2B7.tag=0U,({struct _fat_ptr _tmp342=({const char*_tmpA4=" clause '";_tag_fat(_tmpA4,sizeof(char),10U);});_tmp2B7.f1=_tmp342;});_tmp2B7;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2B6;_tmp2B6.tag=4U,_tmp2B6.f1=clause;_tmp2B6;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B5;_tmp2B5.tag=0U,({struct _fat_ptr _tmp343=({const char*_tmpA3="' may be unsatisfiable";_tag_fat(_tmpA3,sizeof(char),23U);});_tmp2B5.f1=_tmp343;});_tmp2B5;});void*_tmp9E[4U];_tmp9E[0]=& _tmp9F,_tmp9E[1]=& _tmpA0,_tmp9E[2]=& _tmpA1,_tmp9E[3]=& _tmpA2;({unsigned _tmp344=clause->loc;Cyc_Warn_err2(_tmp344,_tag_fat(_tmp9E,sizeof(void*),4U));});});
return({struct _tuple15 _tmp2B9;_tmp2B9.f1=env,_tmp2B9.f2=relns;_tmp2B9;});}}
# 388
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 391
struct Cyc_Tctyp_CVTEnv _tmpA5=env;struct Cyc_Tcenv_Tenv*_tmpA7;unsigned _tmpA6;_LL1: _tmpA6=_tmpA5.loc;_tmpA7=_tmpA5.te;_LL2: {unsigned loc=_tmpA6;struct Cyc_Tcenv_Tenv*te=_tmpA7;
union Cyc_Absyn_AggrInfo _tmpA8=*info;union Cyc_Absyn_AggrInfo _stmttmpD=_tmpA8;union Cyc_Absyn_AggrInfo _tmpA9=_stmttmpD;struct Cyc_Absyn_Aggrdecl*_tmpAA;struct Cyc_Core_Opt*_tmpAD;struct _tuple1*_tmpAC;enum Cyc_Absyn_AggrKind _tmpAB;if((_tmpA9.UnknownAggr).tag == 1){_LL4: _tmpAB=((_tmpA9.UnknownAggr).val).f1;_tmpAC=((_tmpA9.UnknownAggr).val).f2;_tmpAD=((_tmpA9.UnknownAggr).val).f3;_LL5: {enum Cyc_Absyn_AggrKind k=_tmpAB;struct _tuple1*n=_tmpAC;struct Cyc_Core_Opt*tgd=_tmpAD;
# 394
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAE;_push_handler(& _tmpAE);{int _tmpB0=0;if(setjmp(_tmpAE.handler))_tmpB0=1;if(!_tmpB0){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);{
struct Cyc_Absyn_Aggrdecl*_tmpB1=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmpB1;
if((int)ad->kind != (int)k){
if((int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BD;_tmp2BD.tag=0U,({struct _fat_ptr _tmp345=({const char*_tmpB8="expecting struct ";_tag_fat(_tmpB8,sizeof(char),18U);});_tmp2BD.f1=_tmp345;});_tmp2BD;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2BC;_tmp2BC.tag=1U,_tmp2BC.f1=n;_tmp2BC;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB;_tmp2BB.tag=0U,({struct _fat_ptr _tmp346=({const char*_tmpB7="instead of union ";_tag_fat(_tmpB7,sizeof(char),18U);});_tmp2BB.f1=_tmp346;});_tmp2BB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2BA;_tmp2BA.tag=1U,_tmp2BA.f1=n;_tmp2BA;});void*_tmpB2[4U];_tmpB2[0]=& _tmpB3,_tmpB2[1]=& _tmpB4,_tmpB2[2]=& _tmpB5,_tmpB2[3]=& _tmpB6;({unsigned _tmp347=loc;Cyc_Warn_err2(_tmp347,_tag_fat(_tmpB2,sizeof(void*),4U));});});else{
# 402
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp348=({const char*_tmpBF="expecting union ";_tag_fat(_tmpBF,sizeof(char),17U);});_tmp2C1.f1=_tmp348;});_tmp2C1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpBB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C0;_tmp2C0.tag=1U,_tmp2C0.f1=n;_tmp2C0;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BF;_tmp2BF.tag=0U,({struct _fat_ptr _tmp349=({const char*_tmpBE="instead of struct ";_tag_fat(_tmpBE,sizeof(char),19U);});_tmp2BF.f1=_tmp349;});_tmp2BF;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2BE;_tmp2BE.tag=1U,_tmp2BE.f1=n;_tmp2BE;});void*_tmpB9[4U];_tmpB9[0]=& _tmpBA,_tmpB9[1]=& _tmpBB,_tmpB9[2]=& _tmpBC,_tmpB9[3]=& _tmpBD;({unsigned _tmp34A=loc;Cyc_Warn_err2(_tmp34A,_tag_fat(_tmpB9,sizeof(void*),4U));});});}}
# 404
if(((unsigned)tgd &&(int)tgd->v)&&(!((unsigned)ad->impl)|| !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3;_tmp2C3.tag=0U,({struct _fat_ptr _tmp34B=({const char*_tmpC3="@tagged qualfiers don't agree on union ";_tag_fat(_tmpC3,sizeof(char),40U);});_tmp2C3.f1=_tmp34B;});_tmp2C3;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpC2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C2;_tmp2C2.tag=1U,_tmp2C2.f1=n;_tmp2C2;});void*_tmpC0[2U];_tmpC0[0]=& _tmpC1,_tmpC0[1]=& _tmpC2;({unsigned _tmp34C=loc;Cyc_Warn_err2(_tmp34C,_tag_fat(_tmpC0,sizeof(void*),2U));});});
# 407
({union Cyc_Absyn_AggrInfo _tmp34D=Cyc_Absyn_KnownAggr(adp);*info=_tmp34D;});}
# 396
;_pop_handler();}else{void*_tmpAF=(void*)Cyc_Core_get_exn_thrown();void*_tmpC4=_tmpAF;void*_tmpC5;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC4)->tag == Cyc_Dict_Absent){_LL9: _LLA:
# 411
({struct Cyc_Tcenv_Tenv*_tmp34F=te;unsigned _tmp34E=loc;Cyc_Tc_tcAggrdecl(_tmp34F,_tmp34E,({struct Cyc_Absyn_Aggrdecl*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->kind=k,_tmpC6->sc=Cyc_Absyn_Extern,_tmpC6->name=n,_tmpC6->tvs=0,_tmpC6->impl=0,_tmpC6->attributes=0,_tmpC6->expected_mem_kind=0;_tmpC6;}));});
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);
({union Cyc_Absyn_AggrInfo _tmp350=Cyc_Absyn_KnownAggr(adp);*info=_tmp350;});
# 415
if(*targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C6;_tmp2C6.tag=0U,({struct _fat_ptr _tmp351=({const char*_tmpCC="declare parameterized type ";_tag_fat(_tmpCC,sizeof(char),28U);});_tmp2C6.f1=_tmp351;});_tmp2C6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpC9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C5;_tmp2C5.tag=1U,_tmp2C5.f1=n;_tmp2C5;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C4;_tmp2C4.tag=0U,({struct _fat_ptr _tmp352=({const char*_tmpCB=" before using";_tag_fat(_tmpCB,sizeof(char),14U);});_tmp2C4.f1=_tmp352;});_tmp2C4;});void*_tmpC7[3U];_tmpC7[0]=& _tmpC8,_tmpC7[1]=& _tmpC9,_tmpC7[2]=& _tmpCA;({unsigned _tmp353=loc;Cyc_Warn_err2(_tmp353,_tag_fat(_tmpC7,sizeof(void*),3U));});});
return env;}
# 419
goto _LL8;}else{_LLB: _tmpC5=_tmpC4;_LLC: {void*exn=_tmpC5;(int)_rethrow(exn);}}_LL8:;}}}
# 421
_tmpAA=*adp;goto _LL7;}}else{_LL6: _tmpAA=*(_tmpA9.KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmpAA;
# 423
env=Cyc_Tctyp_check_type_inst(env,targs,ad->tvs,expected_kind,allow_abs_aggr,ad->name,0);
# 425
if((allow_abs_aggr && ad->impl == 0)&& !Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 429
ad->expected_mem_kind=1;
return env;}}_LL3:;}}
# 435
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 438
struct Cyc_Tctyp_CVTEnv _tmpCD=env;struct Cyc_Tcenv_Tenv*_tmpCF;unsigned _tmpCE;_LL1: _tmpCE=_tmpCD.loc;_tmpCF=_tmpCD.te;_LL2: {unsigned loc=_tmpCE;struct Cyc_Tcenv_Tenv*te=_tmpCF;
struct Cyc_List_List*_tmpD0=*targsp;struct Cyc_List_List*targs=_tmpD0;
union Cyc_Absyn_DatatypeInfo _tmpD1=*info;union Cyc_Absyn_DatatypeInfo _stmttmpE=_tmpD1;union Cyc_Absyn_DatatypeInfo _tmpD2=_stmttmpE;struct Cyc_Absyn_Datatypedecl*_tmpD3;int _tmpD5;struct _tuple1*_tmpD4;if((_tmpD2.UnknownDatatype).tag == 1){_LL4: _tmpD4=((_tmpD2.UnknownDatatype).val).name;_tmpD5=((_tmpD2.UnknownDatatype).val).is_extensible;_LL5: {struct _tuple1*n=_tmpD4;int is_x=_tmpD5;
# 442
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpD6;_push_handler(& _tmpD6);{int _tmpD8=0;if(setjmp(_tmpD6.handler))_tmpD8=1;if(!_tmpD8){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);;_pop_handler();}else{void*_tmpD7=(void*)Cyc_Core_get_exn_thrown();void*_tmpD9=_tmpD7;void*_tmpDA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpD9)->tag == Cyc_Dict_Absent){_LL9: _LLA:
# 446
({struct Cyc_Tcenv_Tenv*_tmp355=te;unsigned _tmp354=loc;Cyc_Tc_tcDatatypedecl(_tmp355,_tmp354,({struct Cyc_Absyn_Datatypedecl*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->sc=Cyc_Absyn_Extern,_tmpDB->name=n,_tmpDB->tvs=0,_tmpDB->fields=0,_tmpDB->is_extensible=is_x;_tmpDB;}));});
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 449
if(targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C9;_tmp2C9.tag=0U,({struct _fat_ptr _tmp356=({const char*_tmpE1="declare parameterized datatype ";_tag_fat(_tmpE1,sizeof(char),32U);});_tmp2C9.f1=_tmp356;});_tmp2C9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C8;_tmp2C8.tag=1U,_tmp2C8.f1=n;_tmp2C8;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C7;_tmp2C7.tag=0U,({struct _fat_ptr _tmp357=({const char*_tmpE0=" before using";_tag_fat(_tmpE0,sizeof(char),14U);});_tmp2C7.f1=_tmp357;});_tmp2C7;});void*_tmpDC[3U];_tmpDC[0]=& _tmpDD,_tmpDC[1]=& _tmpDE,_tmpDC[2]=& _tmpDF;({unsigned _tmp358=loc;Cyc_Warn_err2(_tmp358,_tag_fat(_tmpDC,sizeof(void*),3U));});});
return env;}
# 453
goto _LL8;}else{_LLB: _tmpDA=_tmpD9;_LLC: {void*exn=_tmpDA;(int)_rethrow(exn);}}_LL8:;}}}
# 457
if(is_x && !(*tudp)->is_extensible)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC;_tmp2CC.tag=0U,({struct _fat_ptr _tmp359=({const char*_tmpE7="datatype ";_tag_fat(_tmpE7,sizeof(char),10U);});_tmp2CC.f1=_tmp359;});_tmp2CC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpE4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2CB;_tmp2CB.tag=1U,_tmp2CB.f1=n;_tmp2CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA;_tmp2CA.tag=0U,({struct _fat_ptr _tmp35A=({const char*_tmpE6=" was not declared @extensible";_tag_fat(_tmpE6,sizeof(char),30U);});_tmp2CA.f1=_tmp35A;});_tmp2CA;});void*_tmpE2[3U];_tmpE2[0]=& _tmpE3,_tmpE2[1]=& _tmpE4,_tmpE2[2]=& _tmpE5;({unsigned _tmp35B=loc;Cyc_Warn_err2(_tmp35B,_tag_fat(_tmpE2,sizeof(void*),3U));});});
({union Cyc_Absyn_DatatypeInfo _tmp35C=Cyc_Absyn_KnownDatatype(tudp);*info=_tmp35C;});
_tmpD3=*tudp;goto _LL7;}}else{_LL6: _tmpD3=*(_tmpD2.KnownDatatype).val;_LL7: {struct Cyc_Absyn_Datatypedecl*tud=_tmpD3;
# 462
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tud->name,0);}}_LL3:;}}
# 468
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 471
union Cyc_Absyn_DatatypeFieldInfo _tmpE8=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmpF=_tmpE8;union Cyc_Absyn_DatatypeFieldInfo _tmpE9=_stmttmpF;struct Cyc_Absyn_Datatypefield*_tmpEB;struct Cyc_Absyn_Datatypedecl*_tmpEA;int _tmpEE;struct _tuple1*_tmpED;struct _tuple1*_tmpEC;if((_tmpE9.UnknownDatatypefield).tag == 1){_LL1: _tmpEC=((_tmpE9.UnknownDatatypefield).val).datatype_name;_tmpED=((_tmpE9.UnknownDatatypefield).val).field_name;_tmpEE=((_tmpE9.UnknownDatatypefield).val).is_extensible;_LL2: {struct _tuple1*tname=_tmpEC;struct _tuple1*fname=_tmpED;int is_x=_tmpEE;
# 474
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(env.te,env.loc,tname);
struct Cyc_Absyn_Datatypefield*tuf;
# 479
{struct Cyc_List_List*_tmpEF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmpEF;for(0;1;fs=fs->tl){
if(fs == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD;_tmp2CD.tag=0U,({struct _fat_ptr _tmp35D=({const char*_tmpF2="Tcutil found a bad datatypefield";_tag_fat(_tmpF2,sizeof(char),33U);});_tmp2CD.f1=_tmp35D;});_tmp2CD;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpF0,sizeof(void*),1U));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)fs->hd)->name,fname)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 486
({union Cyc_Absyn_DatatypeFieldInfo _tmp35E=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmp35E;});
_tmpEA=tud;_tmpEB=tuf;goto _LL4;}}else{_LL3: _tmpEA=((_tmpE9.KnownDatatypefield).val).f1;_tmpEB=((_tmpE9.KnownDatatypefield).val).f2;_LL4: {struct Cyc_Absyn_Datatypedecl*tud=_tmpEA;struct Cyc_Absyn_Datatypefield*tuf=_tmpEB;
# 489
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tuf->name,0);}}_LL0:;}
# 494
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_unary_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*k,struct Cyc_List_List*ts,int put_in_effect,struct _fat_ptr s){
# 496
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF;_tmp2CF.tag=0U,_tmp2CF.f1=s;_tmp2CF;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CE;_tmp2CE.tag=0U,({struct _fat_ptr _tmp35F=({const char*_tmpF6=" has wrong number of arguments";_tag_fat(_tmpF6,sizeof(char),31U);});_tmp2CE.f1=_tmp35F;});_tmp2CE;});void*_tmpF3[2U];_tmpF3[0]=& _tmpF4,_tmpF3[1]=& _tmpF5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpF3,sizeof(void*),2U));});
return Cyc_Tctyp_i_check_type(env,k,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,put_in_effect,1);}
# 501
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 504
struct Cyc_Tctyp_CVTEnv _tmpF7=env;struct Cyc_Tcenv_Tenv*_tmpF9;unsigned _tmpF8;_LL1: _tmpF8=_tmpF7.loc;_tmpF9=_tmpF7.te;_LL2: {unsigned loc=_tmpF8;struct Cyc_Tcenv_Tenv*te=_tmpF9;
struct Cyc_List_List*_tmpFA=*targsp;struct Cyc_List_List*ts=_tmpFA;
void*_tmpFB=c;union Cyc_Absyn_DatatypeFieldInfo*_tmpFC;union Cyc_Absyn_DatatypeInfo*_tmpFD;union Cyc_Absyn_AggrInfo*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Enumdecl**_tmp101;struct _tuple1*_tmp100;switch(*((int*)_tmpFB)){case 1U: _LL4: _LL5:
# 508
 goto _LL7;case 2U: _LL6: _LL7: goto _LL9;case 0U: _LL8: _LL9: goto _LLB;case 7U: _LLA: _LLB:
 goto _LLD;case 6U: _LLC: _LLD: goto _LLF;case 5U: _LLE: _LLF: goto _LL11;case 12U: _LL10: _LL11:
 goto _LL13;case 11U: _LL12: _LL13: goto _LL15;case 14U: _LL14: _LL15: goto _LL17;case 17U: _LL16: _LL17:
# 512
 if(ts != 0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp103=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2D1;_tmp2D1.tag=2U,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=0U,_tmp106->f1=c,_tmp106->f2=ts;_tmp106;})))_tmp360=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=0U,_tmp106->f1=c,_tmp106->f2=ts;_tmp106;}));_tmp2D1.f1=_tmp360;});_tmp2D1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp104=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D0;_tmp2D0.tag=0U,({struct _fat_ptr _tmp361=({const char*_tmp105=" applied to argument(s)";_tag_fat(_tmp105,sizeof(char),24U);});_tmp2D0.f1=_tmp361;});_tmp2D0;});void*_tmp102[2U];_tmp102[0]=& _tmp103,_tmp102[1]=& _tmp104;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp102,sizeof(void*),2U));});
return env;case 9U: _LL18: _LL19:
# 516
 for(0;ts != 0;ts=ts->tl){
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_ek,(void*)ts->hd,1,1);}
return env;case 15U: _LL1A: _tmp100=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpFB)->f1;_tmp101=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpFB)->f2;_LL1B: {struct _tuple1*n=_tmp100;struct Cyc_Absyn_Enumdecl**edp=_tmp101;
# 520
if(ts != 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp108=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D2;_tmp2D2.tag=0U,({struct _fat_ptr _tmp362=({const char*_tmp109="enum applied to argument(s)";_tag_fat(_tmp109,sizeof(char),28U);});_tmp2D2.f1=_tmp362;});_tmp2D2;});void*_tmp107[1U];_tmp107[0]=& _tmp108;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp107,sizeof(void*),1U));});
if(*edp == 0 ||((struct Cyc_Absyn_Enumdecl*)_check_null(*edp))->fields == 0){
struct _handler_cons _tmp10A;_push_handler(& _tmp10A);{int _tmp10C=0;if(setjmp(_tmp10A.handler))_tmp10C=1;if(!_tmp10C){({struct Cyc_Absyn_Enumdecl*_tmp363=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp363;});;_pop_handler();}else{void*_tmp10B=(void*)Cyc_Core_get_exn_thrown();void*_tmp10D=_tmp10B;void*_tmp10E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp10D)->tag == Cyc_Dict_Absent){_LL2F: _LL30:
# 524
({struct Cyc_Tcenv_Tenv*_tmp365=te;unsigned _tmp364=loc;Cyc_Tc_tcEnumdecl(_tmp365,_tmp364,({struct Cyc_Absyn_Enumdecl*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->sc=Cyc_Absyn_Extern,_tmp10F->name=n,_tmp10F->fields=0;_tmp10F;}));});
({struct Cyc_Absyn_Enumdecl*_tmp366=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp366;});
goto _LL2E;}else{_LL31: _tmp10E=_tmp10D;_LL32: {void*exn=_tmp10E;(int)_rethrow(exn);}}_LL2E:;}}}
# 529
return env;}case 16U: _LL1C: _tmpFF=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL1D: {struct Cyc_List_List*fs=_tmpFF;
# 531
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp111=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3;_tmp2D3.tag=0U,({struct _fat_ptr _tmp367=({const char*_tmp112="enum applied to argument(s)";_tag_fat(_tmp112,sizeof(char),28U);});_tmp2D3.f1=_tmp367;});_tmp2D3;});void*_tmp110[1U];_tmp110[0]=& _tmp111;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp110,sizeof(void*),1U));});{
# 534
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp113=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp113;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp115=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5;_tmp2D5.tag=0U,({struct _fat_ptr _tmp368=({const char*_tmp117="duplicate enum field name ";_tag_fat(_tmp117,sizeof(char),27U);});_tmp2D5.f1=_tmp368;});_tmp2D5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp116=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D4;_tmp2D4.tag=1U,_tmp2D4.f1=f->name;_tmp2D4;});void*_tmp114[2U];_tmp114[0]=& _tmp115,_tmp114[1]=& _tmp116;({unsigned _tmp369=f->loc;Cyc_Warn_err2(_tmp369,_tag_fat(_tmp114,sizeof(void*),2U));});});else{
# 541
prev_fields=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->hd=(*f->name).f2,_tmp118->tl=prev_fields;_tmp118;});}
if(f->tag == 0)
({struct Cyc_Absyn_Exp*_tmp36A=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp36A;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D8;_tmp2D8.tag=0U,({struct _fat_ptr _tmp36B=({const char*_tmp11E="enum field ";_tag_fat(_tmp11E,sizeof(char),12U);});_tmp2D8.f1=_tmp36B;});_tmp2D8;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp11B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2D7;_tmp2D7.tag=1U,_tmp2D7.f1=f->name;_tmp2D7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp11C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6;_tmp2D6.tag=0U,({struct _fat_ptr _tmp36C=({const char*_tmp11D=": expression is not constant";_tag_fat(_tmp11D,sizeof(char),29U);});_tmp2D6.f1=_tmp36C;});_tmp2D6;});void*_tmp119[3U];_tmp119[0]=& _tmp11A,_tmp119[1]=& _tmp11B,_tmp119[2]=& _tmp11C;({unsigned _tmp36D=loc;Cyc_Warn_err2(_tmp36D,_tag_fat(_tmp119,sizeof(void*),3U));});});}
tag_count=(unsigned)1 + (Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))).f1;}
# 548
return env;}}case 10U: _LL1E: _LL1F:
 return({struct Cyc_Tctyp_CVTEnv _tmp36F=env;struct Cyc_List_List*_tmp36E=ts;Cyc_Tctyp_i_check_unary_app(_tmp36F,& Cyc_Tcutil_tak,_tmp36E,1,({const char*_tmp11F="regions";_tag_fat(_tmp11F,sizeof(char),8U);}));});case 3U: _LL20: _LL21:
 return({struct Cyc_Tctyp_CVTEnv _tmp371=env;struct Cyc_List_List*_tmp370=ts;Cyc_Tctyp_i_check_unary_app(_tmp371,& Cyc_Tcutil_trk,_tmp370,1,({const char*_tmp120="region_t";_tag_fat(_tmp120,sizeof(char),9U);}));});case 8U: _LL22: _LL23:
 return({struct Cyc_Tctyp_CVTEnv _tmp373=env;struct Cyc_List_List*_tmp372=ts;Cyc_Tctyp_i_check_unary_app(_tmp373,& Cyc_Tcutil_trk,_tmp372,1,({const char*_tmp121="access";_tag_fat(_tmp121,sizeof(char),7U);}));});case 4U: _LL24: _LL25:
 return({struct Cyc_Tctyp_CVTEnv _tmp375=env;struct Cyc_List_List*_tmp374=ts;Cyc_Tctyp_i_check_unary_app(_tmp375,& Cyc_Tcutil_ik,_tmp374,0,({const char*_tmp122="tag_t";_tag_fat(_tmp122,sizeof(char),6U);}));});case 13U: _LL26: _LL27:
 return({struct Cyc_Tctyp_CVTEnv _tmp377=env;struct Cyc_List_List*_tmp376=ts;Cyc_Tctyp_i_check_unary_app(_tmp377,& Cyc_Tcutil_ik,_tmp376,0,({const char*_tmp123="@thin";_tag_fat(_tmp123,sizeof(char),6U);}));});case 20U: _LL28: _tmpFE=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL29: {union Cyc_Absyn_AggrInfo*info=_tmpFE;
# 555
return Cyc_Tctyp_i_check_aggr(env,expected_kind,info,targsp,allow_abs_aggr);}case 18U: _LL2A: _tmpFD=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL2B: {union Cyc_Absyn_DatatypeInfo*info=_tmpFD;
# 557
return Cyc_Tctyp_i_check_datatype(env,expected_kind,info,targsp,allow_abs_aggr);}default: _LL2C: _tmpFC=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL2D: {union Cyc_Absyn_DatatypeFieldInfo*info=_tmpFC;
# 559
return Cyc_Tctyp_i_check_datatype_field(env,expected_kind,info,targsp,allow_abs_aggr);}}_LL3:;}}struct _tuple16{enum Cyc_Absyn_Format_Type f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 564
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 566
struct Cyc_Tctyp_CVTEnv _tmp124=env;struct Cyc_Tcenv_Tenv*_tmp126;unsigned _tmp125;_LL1: _tmp125=_tmp124.loc;_tmp126=_tmp124.te;_LL2: {unsigned loc=_tmp125;struct Cyc_Tcenv_Tenv*te=_tmp126;
{void*_tmp127=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp127;void*_tmp128=_stmttmp10;void**_tmp12C;struct Cyc_Absyn_Typedefdecl**_tmp12B;struct Cyc_List_List**_tmp12A;struct _tuple1*_tmp129;struct Cyc_List_List*_tmp12E;enum Cyc_Absyn_AggrKind _tmp12D;struct Cyc_List_List*_tmp12F;struct Cyc_List_List**_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_List_List**_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_List_List*_tmp137;struct Cyc_Absyn_VarargInfo*_tmp136;int _tmp135;struct Cyc_List_List*_tmp134;void*_tmp133;struct Cyc_Absyn_Tqual*_tmp132;void**_tmp131;struct Cyc_List_List**_tmp130;unsigned _tmp141;void*_tmp140;struct Cyc_Absyn_Exp**_tmp13F;struct Cyc_Absyn_Tqual*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp143;void*_tmp149;void*_tmp148;void*_tmp147;void*_tmp146;struct Cyc_Absyn_Tqual*_tmp145;void*_tmp144;void***_tmp14B;void*_tmp14A;struct Cyc_Absyn_Tvar*_tmp14C;void**_tmp14E;struct Cyc_Core_Opt**_tmp14D;struct Cyc_List_List**_tmp150;void*_tmp14F;switch(*((int*)_tmp128)){case 0U: _LL4: _tmp14F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp128)->f1;_tmp150=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp128)->f2;_LL5: {void*c=_tmp14F;struct Cyc_List_List**targsp=_tmp150;
# 569
env=Cyc_Tctyp_i_check_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr);
# 571
goto _LL3;}case 1U: _LL6: _tmp14D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp128)->f1;_tmp14E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp128)->f2;_LL7: {struct Cyc_Core_Opt**k=_tmp14D;void**ref=_tmp14E;
# 574
if(*k == 0 ||
 Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v)&& !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v,expected_kind))
({struct Cyc_Core_Opt*_tmp378=Cyc_Tcutil_kind_to_opt(expected_kind);*k=_tmp378;});
if(((env.fn_result && env.generalize_evars)&&(int)expected_kind->kind == (int)Cyc_Absyn_RgnKind)&& !te->tempest_generalize){
# 579
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*ref=Cyc_Absyn_unique_rgn_type;else{
# 582
*ref=Cyc_Absyn_heap_rgn_type;}}else{
if((env.generalize_evars &&(int)expected_kind->kind != (int)Cyc_Absyn_BoolKind)&&(int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 586
struct Cyc_Absyn_Tvar*_tmp151=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->tag=2U,_tmp152->f1=0,_tmp152->f2=expected_kind;_tmp152;}));struct Cyc_Absyn_Tvar*v=_tmp151;
({void*_tmp379=Cyc_Absyn_var_type(v);*ref=_tmp379;});
_tmp14C=v;goto _LL9;}else{
# 590
({struct Cyc_List_List*_tmp37A=Cyc_Tctyp_add_free_evar(env.free_evars,t,put_in_effect);env.free_evars=_tmp37A;});}}
# 592
goto _LL3;}case 2U: _LL8: _tmp14C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp128)->f1;_LL9: {struct Cyc_Absyn_Tvar*v=_tmp14C;
# 594
{void*_tmp153=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp11=_tmp153;void*_tmp154=_stmttmp11;struct Cyc_Core_Opt**_tmp155;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp154)->tag == 1U){_LL1D: _tmp155=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp154)->f1;_LL1E: {struct Cyc_Core_Opt**f=_tmp155;
({struct Cyc_Core_Opt*_tmp37C=({struct Cyc_Core_Opt*_tmp157=_cycalloc(sizeof(*_tmp157));({void*_tmp37B=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->tag=2U,_tmp156->f1=0,_tmp156->f2=expected_kind;_tmp156;});_tmp157->v=_tmp37B;});_tmp157;});*f=_tmp37C;});goto _LL1C;}}else{_LL1F: _LL20:
 goto _LL1C;}_LL1C:;}
# 600
({struct Cyc_List_List*_tmp37D=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,v);env.kind_env=_tmp37D;});
# 603
({struct Cyc_List_List*_tmp37E=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,v,put_in_effect);env.free_vars=_tmp37E;});
# 605
{void*_tmp158=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp12=_tmp158;void*_tmp159=_stmttmp12;struct Cyc_Absyn_Kind*_tmp15B;struct Cyc_Core_Opt**_tmp15A;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp159)->tag == 2U){_LL22: _tmp15A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp159)->f1;_tmp15B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp159)->f2;_LL23: {struct Cyc_Core_Opt**f=_tmp15A;struct Cyc_Absyn_Kind*k=_tmp15B;
# 607
if(Cyc_Tcutil_kind_leq(expected_kind,k))
({struct Cyc_Core_Opt*_tmp380=({struct Cyc_Core_Opt*_tmp15D=_cycalloc(sizeof(*_tmp15D));({void*_tmp37F=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->tag=2U,_tmp15C->f1=0,_tmp15C->f2=expected_kind;_tmp15C;});_tmp15D->v=_tmp37F;});_tmp15D;});*f=_tmp380;});
goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 612
goto _LL3;}case 10U: _LLA: _tmp14A=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp128)->f1)->r;_tmp14B=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp128)->f2;_LLB: {void*td=_tmp14A;void***topt=_tmp14B;
# 617
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp15E=td;struct Cyc_Absyn_Datatypedecl*_tmp15F;struct Cyc_Absyn_Enumdecl*_tmp160;struct Cyc_Absyn_Aggrdecl*_tmp161;switch(*((int*)_tmp15E)){case 0U: _LL27: _tmp161=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp15E)->f1;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp161;
# 620
if((env.te)->in_extern_c_include)
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL26;}case 1U: _LL29: _tmp160=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp15E)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp160;
# 624
if((env.te)->in_extern_c_include)
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL26;}default: _LL2B: _tmp15F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp15E)->f1;_LL2C: {struct Cyc_Absyn_Datatypedecl*dd=_tmp15F;
# 628
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL26;}}_LL26:;}
# 630
({void**_tmp381=({void**_tmp162=_cycalloc(sizeof(*_tmp162));*_tmp162=new_typ;_tmp162;});*topt=_tmp381;});
return Cyc_Tctyp_i_check_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3U: _LLC: _tmp144=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->f1).elt_type;_tmp145=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->f1).elt_tq;_tmp146=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->f1).ptr_atts).rgn;_tmp147=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->f1).ptr_atts).nullable;_tmp148=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->f1).ptr_atts).bounds;_tmp149=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->f1).ptr_atts).zero_term;_LLD: {void*t1=_tmp144;struct Cyc_Absyn_Tqual*tqp=_tmp145;void*rgn_type=_tmp146;void*nullable=_tmp147;void*b=_tmp148;void*zt=_tmp149;
# 635
struct Cyc_Absyn_Kind*rgnk;
{enum Cyc_Absyn_AliasQual _tmp163=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp13=_tmp163;enum Cyc_Absyn_AliasQual _tmp164=_stmttmp13;switch(_tmp164){case Cyc_Absyn_Aliasable: _LL2E: _LL2F:
 rgnk=& Cyc_Tcutil_rk;goto _LL2D;case Cyc_Absyn_Unique: _LL30: _LL31:
 rgnk=& Cyc_Tcutil_urk;goto _LL2D;case Cyc_Absyn_Top: _LL32: _LL33:
 goto _LL35;default: _LL34: _LL35: rgnk=& Cyc_Tcutil_trk;goto _LL2D;}_LL2D:;}
# 641
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tak,t1,1,1);
env=Cyc_Tctyp_i_check_type(env,rgnk,rgn_type,1,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_boolk,zt,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_boolk,nullable,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_ptrbk,b,0,allow_abs_aggr);
# 647
({int _tmp382=Cyc_Tcutil_extract_const_from_typedef(env.loc,tqp->print_const,t1);tqp->real_const=_tmp382;});
# 651
({void*_tmp383=zt;Cyc_Unify_unify(_tmp383,Cyc_Tcutil_is_char_type(t1)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});{
int is_zero_terminated=Cyc_Absyn_type2bool(0,zt);
if(is_zero_terminated && !Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp166=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DB;_tmp2DB.tag=0U,({struct _fat_ptr _tmp384=({const char*_tmp16A="cannot have a pointer to zero-terminated ";_tag_fat(_tmp16A,sizeof(char),42U);});_tmp2DB.f1=_tmp384;});_tmp2DB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp167=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DA;_tmp2DA.tag=2U,_tmp2DA.f1=(void*)t1;_tmp2DA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp168=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9;_tmp2D9.tag=0U,({struct _fat_ptr _tmp385=({const char*_tmp169=" elements";_tag_fat(_tmp169,sizeof(char),10U);});_tmp2D9.f1=_tmp385;});_tmp2D9;});void*_tmp165[3U];_tmp165[0]=& _tmp166,_tmp165[1]=& _tmp167,_tmp165[2]=& _tmp168;({unsigned _tmp386=loc;Cyc_Warn_err2(_tmp386,_tag_fat(_tmp165,sizeof(void*),3U));});});{
# 657
struct Cyc_Absyn_Exp*_tmp16B=({void*_tmp387=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp387,b);});struct Cyc_Absyn_Exp*e=_tmp16B;
if(e != 0){
struct _tuple12 _tmp16C=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp14=_tmp16C;struct _tuple12 _tmp16D=_stmttmp14;int _tmp16F;unsigned _tmp16E;_LL37: _tmp16E=_tmp16D.f1;_tmp16F=_tmp16D.f2;_LL38: {unsigned sz=_tmp16E;int known=_tmp16F;
if(is_zero_terminated &&(!known || sz < (unsigned)1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp171=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC;_tmp2DC.tag=0U,({struct _fat_ptr _tmp388=({const char*_tmp172="zero-terminated pointer cannot point to empty sequence";_tag_fat(_tmp172,sizeof(char),55U);});_tmp2DC.f1=_tmp388;});_tmp2DC;});void*_tmp170[1U];_tmp170[0]=& _tmp171;({unsigned _tmp389=loc;Cyc_Warn_err2(_tmp389,_tag_fat(_tmp170,sizeof(void*),1U));});});}}
# 663
goto _LL3;}}}case 9U: _LLE: _tmp143=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp128)->f1;_LLF: {struct Cyc_Absyn_Exp*e=_tmp143;
# 668
({struct Cyc_Tcenv_Tenv*_tmp38A=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp38A,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp174=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DD;_tmp2DD.tag=0U,({struct _fat_ptr _tmp38B=({const char*_tmp175="valueof_t requires an int expression";_tag_fat(_tmp175,sizeof(char),37U);});_tmp2DD.f1=_tmp38B;});_tmp2DD;});void*_tmp173[1U];_tmp173[0]=& _tmp174;({unsigned _tmp38C=loc;Cyc_Warn_err2(_tmp38C,_tag_fat(_tmp173,sizeof(void*),1U));});});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);
goto _LL3;}case 11U: _LL10: _tmp142=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp128)->f1;_LL11: {struct Cyc_Absyn_Exp*e=_tmp142;
# 677
({struct Cyc_Tcenv_Tenv*_tmp38D=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp38D,0,e);});
goto _LL3;}case 4U: _LL12: _tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp128)->f1).elt_type;_tmp13E=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp128)->f1).tq;_tmp13F=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp128)->f1).num_elts;_tmp140=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp128)->f1).zero_term;_tmp141=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp128)->f1).zt_loc;_LL13: {void*t1=_tmp13D;struct Cyc_Absyn_Tqual*tqp=_tmp13E;struct Cyc_Absyn_Exp**eptr=_tmp13F;void*zt=_tmp140;unsigned ztl=_tmp141;
# 682
struct Cyc_Absyn_Exp*_tmp176=*eptr;struct Cyc_Absyn_Exp*e=_tmp176;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tmk,t1,1,allow_abs_aggr);
({int _tmp38E=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_tmp38E;});{
# 686
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);
if(is_zero_terminated){
# 689
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp178=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0;_tmp2E0.tag=0U,({struct _fat_ptr _tmp38F=({const char*_tmp17C="cannot have a zero-terminated array of ";_tag_fat(_tmp17C,sizeof(char),40U);});_tmp2E0.f1=_tmp38F;});_tmp2E0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp179=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DF;_tmp2DF.tag=2U,_tmp2DF.f1=(void*)t1;_tmp2DF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE;_tmp2DE.tag=0U,({struct _fat_ptr _tmp390=({const char*_tmp17B=" elements";_tag_fat(_tmp17B,sizeof(char),10U);});_tmp2DE.f1=_tmp390;});_tmp2DE;});void*_tmp177[3U];_tmp177[0]=& _tmp178,_tmp177[1]=& _tmp179,_tmp177[2]=& _tmp17A;({unsigned _tmp391=loc;Cyc_Warn_err2(_tmp391,_tag_fat(_tmp177,sizeof(void*),3U));});});}
# 694
if(e == 0){
# 697
if(!is_zero_terminated)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1;_tmp2E1.tag=0U,({struct _fat_ptr _tmp392=({const char*_tmp17F="array bound defaults to 1 here";_tag_fat(_tmp17F,sizeof(char),31U);});_tmp2E1.f1=_tmp392;});_tmp2E1;});void*_tmp17D[1U];_tmp17D[0]=& _tmp17E;({unsigned _tmp393=loc;Cyc_Warn_warn2(_tmp393,_tag_fat(_tmp17D,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp394=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp394;});}
# 701
({struct Cyc_Tcenv_Tenv*_tmp395=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp395,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp181=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2;_tmp2E2.tag=0U,({struct _fat_ptr _tmp396=({const char*_tmp182="array bounds expression is not an unsigned int";_tag_fat(_tmp182,sizeof(char),47U);});_tmp2E2.f1=_tmp396;});_tmp2E2;});void*_tmp180[1U];_tmp180[0]=& _tmp181;({unsigned _tmp397=loc;Cyc_Warn_err2(_tmp397,_tag_fat(_tmp180,sizeof(void*),1U));});});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);{
struct _tuple12 _tmp183=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp15=_tmp183;struct _tuple12 _tmp184=_stmttmp15;int _tmp186;unsigned _tmp185;_LL3A: _tmp185=_tmp184.f1;_tmp186=_tmp184.f2;_LL3B: {unsigned sz=_tmp185;int known=_tmp186;
# 707
if((is_zero_terminated && known)&& sz < (unsigned)1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp188=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=0U,({struct _fat_ptr _tmp398=({const char*_tmp189="zero terminated array cannot have zero elements";_tag_fat(_tmp189,sizeof(char),48U);});_tmp2E3.f1=_tmp398;});_tmp2E3;});void*_tmp187[1U];_tmp187[0]=& _tmp188;({unsigned _tmp399=loc;Cyc_Warn_warn2(_tmp399,_tag_fat(_tmp187,sizeof(void*),1U));});});
# 710
if((known && sz < (unsigned)1)&& Cyc_Cyclone_tovc_r){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0U,({struct _fat_ptr _tmp39A=({const char*_tmp18C="0-element arrays are supported only with gcc, changing to 1";_tag_fat(_tmp18C,sizeof(char),60U);});_tmp2E4.f1=_tmp39A;});_tmp2E4;});void*_tmp18A[1U];_tmp18A[0]=& _tmp18B;({unsigned _tmp39B=loc;Cyc_Warn_warn2(_tmp39B,_tag_fat(_tmp18A,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp39C=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp39C;});}
# 714
goto _LL3;}}}}case 5U: _LL14: _tmp130=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).tvars;_tmp131=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).effect;_tmp132=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).ret_tqual;_tmp133=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).ret_type;_tmp134=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).args;_tmp135=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).c_varargs;_tmp136=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).cyc_varargs;_tmp137=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).rgn_po;_tmp138=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).attributes;_tmp139=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).requires_clause;_tmp13A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).requires_relns;_tmp13B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).ensures_clause;_tmp13C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp128)->f1).ensures_relns;_LL15: {struct Cyc_List_List**btvs=_tmp130;void**eff=_tmp131;struct Cyc_Absyn_Tqual*rtq=_tmp132;void*tr=_tmp133;struct Cyc_List_List*args=_tmp134;int c_vararg=_tmp135;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp136;struct Cyc_List_List*rgn_po=_tmp137;struct Cyc_List_List*atts=_tmp138;struct Cyc_Absyn_Exp*req=_tmp139;struct Cyc_List_List**req_relns=_tmp13A;struct Cyc_Absyn_Exp*ens=_tmp13B;struct Cyc_List_List**ens_relns=_tmp13C;
# 721
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;atts != 0;atts=atts->tl){
void*_tmp18D=(void*)atts->hd;void*_stmttmp16=_tmp18D;void*_tmp18E=_stmttmp16;int _tmp191;int _tmp190;enum Cyc_Absyn_Format_Type _tmp18F;switch(*((int*)_tmp18E)){case 1U: _LL3D: _LL3E:
 is_stdcall=1;goto _LL3C;case 2U: _LL3F: _LL40:
 is_cdecl=1;goto _LL3C;case 3U: _LL41: _LL42:
 is_fastcall=1;goto _LL3C;case 19U: _LL43: _tmp18F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp18E)->f1;_tmp190=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp18E)->f2;_tmp191=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp18E)->f3;_LL44: {enum Cyc_Absyn_Format_Type fmttype=_tmp18F;int i=_tmp190;int j=_tmp191;
# 734
++ num_formats;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;
goto _LL3C;}default: _LL45: _LL46:
# 740
 if(!Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp193=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=0U,({struct _fat_ptr _tmp39D=({const char*_tmp195="bad function type attribute ";_tag_fat(_tmp195,sizeof(char),29U);});_tmp2E6.f1=_tmp39D;});_tmp2E6;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp194=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp2E5;_tmp2E5.tag=10U,_tmp2E5.f1=(void*)atts->hd;_tmp2E5;});void*_tmp192[2U];_tmp192[0]=& _tmp193,_tmp192[1]=& _tmp194;({unsigned _tmp39E=loc;Cyc_Warn_err2(_tmp39E,_tag_fat(_tmp192,sizeof(void*),2U));});});}_LL3C:;}
# 743
if((is_stdcall + is_cdecl)+ is_fastcall > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp197=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=0U,({struct _fat_ptr _tmp39F=({const char*_tmp198="function can't have multiple calling conventions";_tag_fat(_tmp198,sizeof(char),49U);});_tmp2E7.f1=_tmp39F;});_tmp2E7;});void*_tmp196[1U];_tmp196[0]=& _tmp197;({unsigned _tmp3A0=loc;Cyc_Warn_err2(_tmp3A0,_tag_fat(_tmp196,sizeof(void*),1U));});});
if(num_formats > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E8;_tmp2E8.tag=0U,({struct _fat_ptr _tmp3A1=({const char*_tmp19B="function can't have multiple format attributes";_tag_fat(_tmp19B,sizeof(char),47U);});_tmp2E8.f1=_tmp3A1;});_tmp2E8;});void*_tmp199[1U];_tmp199[0]=& _tmp19A;({unsigned _tmp3A2=loc;Cyc_Warn_err2(_tmp3A2,_tag_fat(_tmp199,sizeof(void*),1U));});});
# 750
Cyc_Tcutil_check_unique_tvars(loc,*btvs);
{struct Cyc_List_List*b=*btvs;for(0;b != 0;b=b->tl){
({int _tmp3A3=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmp3A3;});
({struct Cyc_List_List*_tmp3A4=Cyc_Tctyp_add_bound_tvar(env.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);env.kind_env=_tmp3A4;});{
void*_tmp19C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_stmttmp17=_tmp19C;void*_tmp19D=_stmttmp17;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp19D)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp19D)->f1)->kind == Cyc_Absyn_MemKind){_LL48: _LL49:
# 756
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=0U,({struct _fat_ptr _tmp3A5=({const char*_tmp1A1="function abstracts Mem type variable ";_tag_fat(_tmp1A1,sizeof(char),38U);});_tmp2EA.f1=_tmp3A5;});_tmp2EA;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=7U,_tmp2E9.f1=(struct Cyc_Absyn_Tvar*)b->hd;_tmp2E9;});void*_tmp19E[2U];_tmp19E[0]=& _tmp19F,_tmp19E[1]=& _tmp1A0;({unsigned _tmp3A6=loc;Cyc_Warn_err2(_tmp3A6,_tag_fat(_tmp19E,sizeof(void*),2U));});});goto _LL47;}else{goto _LL4A;}}else{_LL4A: _LL4B:
 goto _LL47;}_LL47:;}}}{
# 762
struct Cyc_Tctyp_CVTEnv new_env=({struct Cyc_Tctyp_CVTEnv _tmp2FB;_tmp2FB.loc=loc,_tmp2FB.te=te,_tmp2FB.kind_env=env.kind_env,_tmp2FB.fn_result=1,_tmp2FB.generalize_evars=env.generalize_evars,_tmp2FB.free_vars=0,_tmp2FB.free_evars=0;_tmp2FB;});
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_tmk,tr,1,1);
({int _tmp3A7=Cyc_Tcutil_extract_const_from_typedef(loc,rtq->print_const,tr);rtq->real_const=_tmp3A7;});
new_env.fn_result=0;
# 768
{struct Cyc_List_List*a=args;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp1A2=(struct _tuple9*)a->hd;struct _tuple9*trip=_tmp1A2;
void*_tmp1A3=(*trip).f3;void*t=_tmp1A3;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_tmk,t,1,1);{
int _tmp1A4=Cyc_Tcutil_extract_const_from_typedef(loc,((*trip).f2).print_const,t);int ec=_tmp1A4;
((*trip).f2).real_const=ec;
# 776
if(Cyc_Tcutil_is_array_type(t)){
# 778
void*_tmp1A5=Cyc_Absyn_new_evar(0,0);void*ptr_rgn=_tmp1A5;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_rk,ptr_rgn,1,1);
({void*_tmp3A8=Cyc_Tcutil_promote_array(t,ptr_rgn,0);(*trip).f3=_tmp3A8;});}}}}
# 784
if(cyc_vararg != 0){
if(c_vararg)({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=0U,({struct _fat_ptr _tmp3A9=({const char*_tmp1A8="both c_vararg and cyc_vararg";_tag_fat(_tmp1A8,sizeof(char),29U);});_tmp2EB.f1=_tmp3A9;});_tmp2EB;});void*_tmp1A6[1U];_tmp1A6[0]=& _tmp1A7;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1A6,sizeof(void*),1U));});{
struct Cyc_Absyn_VarargInfo _tmp1A9=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp18=_tmp1A9;struct Cyc_Absyn_VarargInfo _tmp1AA=_stmttmp18;int _tmp1AC;void*_tmp1AB;_LL4D: _tmp1AB=_tmp1AA.type;_tmp1AC=_tmp1AA.inject;_LL4E: {void*vt=_tmp1AB;int vi=_tmp1AC;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_tmk,vt,1,1);
({int _tmp3AA=Cyc_Tcutil_extract_const_from_typedef(loc,(cyc_vararg->tq).print_const,vt);(cyc_vararg->tq).real_const=_tmp3AA;});
# 790
if(vi){
void*_tmp1AD=Cyc_Tcutil_compress(vt);void*_stmttmp19=_tmp1AD;void*_tmp1AE=_stmttmp19;void*_tmp1B1;void*_tmp1B0;void*_tmp1AF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AE)->tag == 3U){_LL50: _tmp1AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AE)->f1).elt_type;_tmp1B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AE)->f1).ptr_atts).bounds;_tmp1B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AE)->f1).ptr_atts).zero_term;_LL51: {void*et=_tmp1AF;void*bs=_tmp1B0;void*zt=_tmp1B1;
# 793
{void*_tmp1B2=Cyc_Tcutil_compress(et);void*_stmttmp1A=_tmp1B2;void*_tmp1B3=_stmttmp1A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B3)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B3)->f1)->tag == 18U){_LL55: _LL56:
# 795
 if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC;_tmp2EC.tag=0U,({struct _fat_ptr _tmp3AB=({const char*_tmp1B6="can't inject into a zeroterm pointer";_tag_fat(_tmp1B6,sizeof(char),37U);});_tmp2EC.f1=_tmp3AB;});_tmp2EC;});void*_tmp1B4[1U];_tmp1B4[0]=& _tmp1B5;({unsigned _tmp3AC=loc;Cyc_Warn_err2(_tmp3AC,_tag_fat(_tmp1B4,sizeof(void*),1U));});});
if(!({void*_tmp3AD=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp3AD,bs);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=0U,({struct _fat_ptr _tmp3AE=({const char*_tmp1B9="can't inject into a fat pointer to datatype";_tag_fat(_tmp1B9,sizeof(char),44U);});_tmp2ED.f1=_tmp3AE;});_tmp2ED;});void*_tmp1B7[1U];_tmp1B7[0]=& _tmp1B8;({unsigned _tmp3AF=loc;Cyc_Warn_err2(_tmp3AF,_tag_fat(_tmp1B7,sizeof(void*),1U));});});
goto _LL54;}else{goto _LL57;}}else{_LL57: _LL58:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE;_tmp2EE.tag=0U,({struct _fat_ptr _tmp3B0=({const char*_tmp1BC="can't inject a non-datatype type";_tag_fat(_tmp1BC,sizeof(char),33U);});_tmp2EE.f1=_tmp3B0;});_tmp2EE;});void*_tmp1BA[1U];_tmp1BA[0]=& _tmp1BB;({unsigned _tmp3B1=loc;Cyc_Warn_err2(_tmp3B1,_tag_fat(_tmp1BA,sizeof(void*),1U));});});goto _LL54;}_LL54:;}
# 802
goto _LL4F;}}else{_LL52: _LL53:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0U,({struct _fat_ptr _tmp3B2=({const char*_tmp1BF="expecting a datatype pointer type";_tag_fat(_tmp1BF,sizeof(char),34U);});_tmp2EF.f1=_tmp3B2;});_tmp2EF;});void*_tmp1BD[1U];_tmp1BD[0]=& _tmp1BE;({unsigned _tmp3B3=loc;Cyc_Warn_err2(_tmp3B3,_tag_fat(_tmp1BD,sizeof(void*),1U));});});goto _LL4F;}_LL4F:;}}}}
# 807
if(num_formats > 0){
int _tmp1C0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);int num_args=_tmp1C0;
if((((fmt_desc_arg < 0 || fmt_desc_arg > num_args)|| fmt_arg_start < 0)||
# 811
(cyc_vararg == 0 && !c_vararg)&& fmt_arg_start != 0)||
(cyc_vararg != 0 || c_vararg)&& fmt_arg_start != num_args + 1)
# 814
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=0U,({struct _fat_ptr _tmp3B4=({const char*_tmp1C3="bad format descriptor";_tag_fat(_tmp1C3,sizeof(char),22U);});_tmp2F0.f1=_tmp3B4;});_tmp2F0;});void*_tmp1C1[1U];_tmp1C1[0]=& _tmp1C2;({unsigned _tmp3B5=loc;Cyc_Warn_err2(_tmp3B5,_tag_fat(_tmp1C1,sizeof(void*),1U));});});else{
# 817
struct _tuple9 _tmp1C4=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_desc_arg - 1);struct _tuple9 _stmttmp1B=_tmp1C4;struct _tuple9 _tmp1C5=_stmttmp1B;void*_tmp1C6;_LL5A: _tmp1C6=_tmp1C5.f3;_LL5B: {void*t=_tmp1C6;
# 819
{void*_tmp1C7=Cyc_Tcutil_compress(t);void*_stmttmp1C=_tmp1C7;void*_tmp1C8=_stmttmp1C;void*_tmp1CA;void*_tmp1C9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->tag == 3U){_LL5D: _tmp1C9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->f1).elt_type;_tmp1CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->f1).ptr_atts).bounds;_LL5E: {void*et=_tmp1C9;void*b=_tmp1CA;
# 822
if(!Cyc_Tcutil_is_char_type(et))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=0U,({struct _fat_ptr _tmp3B6=({const char*_tmp1CD="format descriptor is not a string";_tag_fat(_tmp1CD,sizeof(char),34U);});_tmp2F1.f1=_tmp3B6;});_tmp2F1;});void*_tmp1CB[1U];_tmp1CB[0]=& _tmp1CC;({unsigned _tmp3B7=loc;Cyc_Warn_err2(_tmp3B7,_tag_fat(_tmp1CB,sizeof(void*),1U));});});else{
# 825
struct Cyc_Absyn_Exp*_tmp1CE=({void*_tmp3B8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp3B8,b);});struct Cyc_Absyn_Exp*e=_tmp1CE;
if(e == 0 && c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2;_tmp2F2.tag=0U,({struct _fat_ptr _tmp3B9=({const char*_tmp1D1="format descriptor is not a char * type";_tag_fat(_tmp1D1,sizeof(char),39U);});_tmp2F2.f1=_tmp3B9;});_tmp2F2;});void*_tmp1CF[1U];_tmp1CF[0]=& _tmp1D0;({unsigned _tmp3BA=loc;Cyc_Warn_err2(_tmp3BA,_tag_fat(_tmp1CF,sizeof(void*),1U));});});else{
if(e != 0 && !c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F3;_tmp2F3.tag=0U,({struct _fat_ptr _tmp3BB=({const char*_tmp1D4="format descriptor is not a char ? type";_tag_fat(_tmp1D4,sizeof(char),39U);});_tmp2F3.f1=_tmp3BB;});_tmp2F3;});void*_tmp1D2[1U];_tmp1D2[0]=& _tmp1D3;({unsigned _tmp3BC=loc;Cyc_Warn_err2(_tmp3BC,_tag_fat(_tmp1D2,sizeof(void*),1U));});});}}
# 831
goto _LL5C;}}else{_LL5F: _LL60:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F4;_tmp2F4.tag=0U,({struct _fat_ptr _tmp3BD=({const char*_tmp1D7="format descriptor is not a string type";_tag_fat(_tmp1D7,sizeof(char),39U);});_tmp2F4.f1=_tmp3BD;});_tmp2F4;});void*_tmp1D5[1U];_tmp1D5[0]=& _tmp1D6;({unsigned _tmp3BE=loc;Cyc_Warn_err2(_tmp3BE,_tag_fat(_tmp1D5,sizeof(void*),1U));});});goto _LL5C;}_LL5C:;}
# 834
if(fmt_arg_start != 0 && !c_vararg){
# 838
int problem;
{struct _tuple16 _tmp1D8=({struct _tuple16 _tmp2F5;_tmp2F5.f1=ft,({void*_tmp3BF=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_vararg))->type));_tmp2F5.f2=_tmp3BF;});_tmp2F5;});struct _tuple16 _stmttmp1D=_tmp1D8;struct _tuple16 _tmp1D9=_stmttmp1D;struct Cyc_Absyn_Datatypedecl*_tmp1DA;struct Cyc_Absyn_Datatypedecl*_tmp1DB;switch(_tmp1D9.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->f1)->f1).KnownDatatype).tag == 2){_LL62: _tmp1DB=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->f1)->f1).KnownDatatype).val;_LL63: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1DB;
# 841
problem=({struct _tuple1*_tmp3C0=tud->name;Cyc_Absyn_qvar_cmp(_tmp3C0,Cyc_Absyn_datatype_print_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->f1)->f1).KnownDatatype).tag == 2){_LL64: _tmp1DA=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9.f2)->f1)->f1).KnownDatatype).val;_LL65: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1DA;
# 844
problem=({struct _tuple1*_tmp3C1=tud->name;Cyc_Absyn_qvar_cmp(_tmp3C1,Cyc_Absyn_datatype_scanf_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}default: _LL66: _LL67:
# 847
 problem=1;
goto _LL61;}_LL61:;}
# 850
if(problem)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6;_tmp2F6.tag=0U,({struct _fat_ptr _tmp3C2=({const char*_tmp1DE="format attribute and vararg types don't match";_tag_fat(_tmp1DE,sizeof(char),46U);});_tmp2F6.f1=_tmp3C2;});_tmp2F6;});void*_tmp1DC[1U];_tmp1DC[0]=& _tmp1DD;({unsigned _tmp3C3=loc;Cyc_Warn_err2(_tmp3C3,_tag_fat(_tmp1DC,sizeof(void*),1U));});});}}}}
# 858
{struct Cyc_List_List*rpo=rgn_po;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp1DF=(struct _tuple0*)rpo->hd;struct _tuple0*_stmttmp1E=_tmp1DF;struct _tuple0*_tmp1E0=_stmttmp1E;void*_tmp1E2;void*_tmp1E1;_LL69: _tmp1E1=_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;_LL6A: {void*r1=_tmp1E1;void*r2=_tmp1E2;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_ek,r1,1,1);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_trk,r2,1,1);}}}{
# 866
struct Cyc_Tcenv_Fenv*_tmp1E3=Cyc_Tcenv_bogus_fenv(tr,args);struct Cyc_Tcenv_Fenv*fenv=_tmp1E3;
struct Cyc_Tcenv_Tenv*_tmp1E4=new_env.te;struct Cyc_Tcenv_Tenv*old_te=_tmp1E4;
({struct Cyc_Tcenv_Tenv*_tmp3C4=({struct Cyc_Tcenv_Tenv*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->ns=te->ns,_tmp1E5->ae=te->ae,_tmp1E5->le=fenv,_tmp1E5->allow_valueof=1,_tmp1E5->in_extern_c_include=te->in_extern_c_include,_tmp1E5->in_tempest=te->in_tempest,_tmp1E5->tempest_generalize=te->tempest_generalize,_tmp1E5->in_extern_c_inc_repeat=te->in_extern_c_inc_repeat;_tmp1E5;});new_env.te=_tmp3C4;});{
# 872
struct _tuple15 _tmp1E6=({struct Cyc_Tctyp_CVTEnv _tmp3C6=new_env;struct _fat_ptr _tmp3C5=({const char*_tmp21F="@requires";_tag_fat(_tmp21F,sizeof(char),10U);});Cyc_Tctyp_check_clause(_tmp3C6,_tmp3C5,req);});struct _tuple15 _stmttmp1F=_tmp1E6;struct _tuple15 _tmp1E7=_stmttmp1F;struct Cyc_List_List*_tmp1E9;struct Cyc_Tctyp_CVTEnv _tmp1E8;_LL6C: _tmp1E8=_tmp1E7.f1;_tmp1E9=_tmp1E7.f2;_LL6D: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1E8;struct Cyc_List_List*req_rs=_tmp1E9;
new_env=nenv;
*req_relns=req_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,req_rs);{
struct _tuple15 _tmp1EA=({struct Cyc_Tctyp_CVTEnv _tmp3C8=new_env;struct _fat_ptr _tmp3C7=({const char*_tmp21E="@ensures";_tag_fat(_tmp21E,sizeof(char),9U);});Cyc_Tctyp_check_clause(_tmp3C8,_tmp3C7,ens);});struct _tuple15 _stmttmp20=_tmp1EA;struct _tuple15 _tmp1EB=_stmttmp20;struct Cyc_List_List*_tmp1ED;struct Cyc_Tctyp_CVTEnv _tmp1EC;_LL6F: _tmp1EC=_tmp1EB.f1;_tmp1ED=_tmp1EB.f2;_LL70: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1EC;struct Cyc_List_List*ens_rs=_tmp1ED;
new_env=nenv;
*ens_relns=ens_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,ens_rs);
new_env.te=old_te;
# 882
if(*eff != 0)
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_ek,(void*)_check_null(*eff),1,1);else{
# 885
struct Cyc_List_List*effect=0;
# 890
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp1EE=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp21=_tmp1EE;struct _tuple14 _tmp1EF=_stmttmp21;int _tmp1F1;struct Cyc_Absyn_Tvar*_tmp1F0;_LL72: _tmp1F0=_tmp1EF.f1;_tmp1F1=_tmp1EF.f2;_LL73: {struct Cyc_Absyn_Tvar*tv=_tmp1F0;int put_in_eff=_tmp1F1;
if(!put_in_eff)continue;{
void*_tmp1F2=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp22=_tmp1F2;void*_tmp1F3=_stmttmp22;struct Cyc_Core_Opt**_tmp1F4;struct Cyc_Absyn_Kind*_tmp1F5;struct Cyc_Core_Opt**_tmp1F6;struct Cyc_Absyn_Kind*_tmp1F8;struct Cyc_Core_Opt**_tmp1F7;switch(*((int*)_tmp1F3)){case 2U: _LL75: _tmp1F7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1F3)->f1;_tmp1F8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1F3)->f2;if((int)_tmp1F8->kind == (int)Cyc_Absyn_RgnKind){_LL76: {struct Cyc_Core_Opt**f=_tmp1F7;struct Cyc_Absyn_Kind*r=_tmp1F8;
# 896
if((int)r->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmp3C9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp3C9;});_tmp1F5=r;goto _LL78;}
# 899
({struct Cyc_Core_Opt*_tmp3CA=Cyc_Tcutil_kind_to_bound_opt(r);*f=_tmp3CA;});_tmp1F5=r;goto _LL78;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1F3)->f2)->kind){case Cyc_Absyn_BoolKind: _LL79: _LL7A:
# 902
 goto _LL7C;case Cyc_Absyn_PtrBndKind: _LL7B: _LL7C:
 goto _LL7E;case Cyc_Absyn_IntKind: _LL7D: _LL7E:
 goto _LL80;case Cyc_Absyn_EffKind: _LL85: _tmp1F6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1F3)->f1;_LL86: {struct Cyc_Core_Opt**f=_tmp1F6;
# 909
({struct Cyc_Core_Opt*_tmp3CB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*f=_tmp3CB;});goto _LL88;}default: goto _LL8B;}}case 0U: _LL77: _tmp1F5=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1F3)->f1;if((int)_tmp1F5->kind == (int)Cyc_Absyn_RgnKind){_LL78: {struct Cyc_Absyn_Kind*r=_tmp1F5;
# 901
effect=({struct Cyc_List_List*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));({void*_tmp3CC=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(tv));_tmp1F9->hd=_tmp3CC;}),_tmp1F9->tl=effect;_tmp1F9;});goto _LL74;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1F3)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7F: _LL80:
# 905
 goto _LL82;case Cyc_Absyn_PtrBndKind: _LL81: _LL82:
 goto _LL84;case Cyc_Absyn_IntKind: _LL83: _LL84:
 goto _LL74;case Cyc_Absyn_EffKind: _LL87: _LL88:
# 911
 effect=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));({void*_tmp3CD=Cyc_Absyn_var_type(tv);_tmp1FA->hd=_tmp3CD;}),_tmp1FA->tl=effect;_tmp1FA;});goto _LL74;default: _LL8B: _LL8C:
# 916
 effect=({struct Cyc_List_List*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));({void*_tmp3CE=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(tv));_tmp1FD->hd=_tmp3CE;}),_tmp1FD->tl=effect;_tmp1FD;});goto _LL74;}}default: _LL89: _tmp1F4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1F3)->f1;_LL8A: {struct Cyc_Core_Opt**f=_tmp1F4;
# 913
({struct Cyc_Core_Opt*_tmp3D0=({struct Cyc_Core_Opt*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));({void*_tmp3CF=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->tag=2U,_tmp1FB->f1=0,_tmp1FB->f2=& Cyc_Tcutil_ak;_tmp1FB;});_tmp1FC->v=_tmp3CF;});_tmp1FC;});*f=_tmp3D0;});goto _LL8C;}}_LL74:;}}}}
# 920
{struct Cyc_List_List*ts=new_env.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple13 _tmp1FE=*((struct _tuple13*)ts->hd);struct _tuple13 _stmttmp23=_tmp1FE;struct _tuple13 _tmp1FF=_stmttmp23;int _tmp201;void*_tmp200;_LL8E: _tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;_LL8F: {void*tv=_tmp200;int put_in_eff=_tmp201;
if(!put_in_eff)continue;{
struct Cyc_Absyn_Kind*_tmp202=Cyc_Tcutil_type_kind(tv);struct Cyc_Absyn_Kind*_stmttmp24=_tmp202;struct Cyc_Absyn_Kind*_tmp203=_stmttmp24;switch(((struct Cyc_Absyn_Kind*)_tmp203)->kind){case Cyc_Absyn_RgnKind: _LL91: _LL92:
 effect=({struct Cyc_List_List*_tmp204=_cycalloc(sizeof(*_tmp204));({void*_tmp3D1=Cyc_Absyn_access_eff(tv);_tmp204->hd=_tmp3D1;}),_tmp204->tl=effect;_tmp204;});goto _LL90;case Cyc_Absyn_EffKind: _LL93: _LL94:
 effect=({struct Cyc_List_List*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->hd=tv,_tmp205->tl=effect;_tmp205;});goto _LL90;case Cyc_Absyn_IntKind: _LL95: _LL96:
 goto _LL90;default: _LL97: _LL98:
 effect=({struct Cyc_List_List*_tmp206=_cycalloc(sizeof(*_tmp206));({void*_tmp3D2=Cyc_Absyn_regionsof_eff(tv);_tmp206->hd=_tmp3D2;}),_tmp206->tl=effect;_tmp206;});goto _LL90;}_LL90:;}}}}
# 930
({void*_tmp3D3=Cyc_Absyn_join_eff(effect);*eff=_tmp3D3;});}
# 936
if(*btvs != 0){
struct Cyc_List_List*bs=*btvs;for(0;bs != 0;bs=bs->tl){
void*_tmp207=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_stmttmp25=_tmp207;void*_tmp208=_stmttmp25;struct Cyc_Absyn_Kind*_tmp20A;struct Cyc_Core_Opt**_tmp209;struct Cyc_Core_Opt**_tmp20B;switch(*((int*)_tmp208)){case 1U: _LL9A: _tmp20B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp208)->f1;_LL9B: {struct Cyc_Core_Opt**f=_tmp20B;
# 940
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F9;_tmp2F9.tag=0U,({struct _fat_ptr _tmp3D4=({const char*_tmp211="type variable ";_tag_fat(_tmp211,sizeof(char),15U);});_tmp2F9.f1=_tmp3D4;});_tmp2F9;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp20E=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2F8;_tmp2F8.tag=7U,_tmp2F8.f1=(struct Cyc_Absyn_Tvar*)bs->hd;_tmp2F8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp20F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7;_tmp2F7.tag=0U,({struct _fat_ptr _tmp3D5=({const char*_tmp210=" unconstrained, assuming boxed";_tag_fat(_tmp210,sizeof(char),31U);});_tmp2F7.f1=_tmp3D5;});_tmp2F7;});void*_tmp20C[3U];_tmp20C[0]=& _tmp20D,_tmp20C[1]=& _tmp20E,_tmp20C[2]=& _tmp20F;({unsigned _tmp3D6=loc;Cyc_Warn_warn2(_tmp3D6,_tag_fat(_tmp20C,sizeof(void*),3U));});});
({struct Cyc_Core_Opt*_tmp3D7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3D7;});
goto _LL99;}case 2U: _LL9C: _tmp209=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp208)->f1;_tmp20A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp208)->f2;_LL9D: {struct Cyc_Core_Opt**f=_tmp209;struct Cyc_Absyn_Kind*k=_tmp20A;
# 944
{struct Cyc_Absyn_Kind*_tmp212=k;switch(((struct Cyc_Absyn_Kind*)_tmp212)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp212)->aliasqual == Cyc_Absyn_Top){_LLA3: _LLA4:
 goto _LLA6;}else{goto _LLB3;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)_tmp212)->aliasqual){case Cyc_Absyn_Top: _LLA5: _LLA6:
 goto _LLA8;case Cyc_Absyn_Aliasable: _LLA7: _LLA8:
 goto _LLAA;case Cyc_Absyn_Unique: _LLAD: _LLAE:
# 950
 goto _LLB0;default: goto _LLB3;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)_tmp212)->aliasqual){case Cyc_Absyn_Top: _LLA9: _LLAA:
# 948
 goto _LLAC;case Cyc_Absyn_Aliasable: _LLAB: _LLAC:
({struct Cyc_Core_Opt*_tmp3D8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3D8;});goto _LLA2;case Cyc_Absyn_Unique: _LLAF: _LLB0:
# 951
({struct Cyc_Core_Opt*_tmp3D9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp3D9;});goto _LLA2;default: goto _LLB3;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp212)->aliasqual == Cyc_Absyn_Top){_LLB1: _LLB2:
({struct Cyc_Core_Opt*_tmp3DA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp3DA;});goto _LLA2;}else{goto _LLB3;}default: _LLB3: _LLB4:
({struct Cyc_Core_Opt*_tmp3DB=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp3DB;});goto _LLA2;}_LLA2:;}
# 955
goto _LL99;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp208)->f1)->kind == Cyc_Absyn_MemKind){_LL9E: _LL9F:
# 957
({struct Cyc_Warn_String_Warn_Warg_struct _tmp214=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FA;_tmp2FA.tag=0U,({struct _fat_ptr _tmp3DC=({const char*_tmp215="functions can't abstract M types";_tag_fat(_tmp215,sizeof(char),33U);});_tmp2FA.f1=_tmp3DC;});_tmp2FA;});void*_tmp213[1U];_tmp213[0]=& _tmp214;({unsigned _tmp3DD=loc;Cyc_Warn_err2(_tmp3DD,_tag_fat(_tmp213,sizeof(void*),1U));});});goto _LL99;}else{_LLA0: _LLA1:
 goto _LL99;}}_LL99:;}}
# 962
({struct Cyc_List_List*_tmp3DE=((struct Cyc_List_List*(*)(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(struct Cyc_Absyn_Tvar*)))Cyc_Tctyp_remove_bound_tvars)(new_env.kind_env,*btvs,(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))Cyc_Core_identity);env.kind_env=_tmp3DE;});
({struct Cyc_List_List*_tmp3DF=((struct Cyc_List_List*(*)(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars)(new_env.free_vars,*btvs,(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst);new_env.free_vars=_tmp3DF;});
# 965
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp216=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp26=_tmp216;struct _tuple14 _tmp217=_stmttmp26;int _tmp219;struct Cyc_Absyn_Tvar*_tmp218;_LLB6: _tmp218=_tmp217.f1;_tmp219=_tmp217.f2;_LLB7: {struct Cyc_Absyn_Tvar*t=_tmp218;int b=_tmp219;
({struct Cyc_List_List*_tmp3E0=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,t,b);env.free_vars=_tmp3E0;});}}}
# 970
{struct Cyc_List_List*evs=new_env.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp21A=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp27=_tmp21A;struct _tuple13 _tmp21B=_stmttmp27;int _tmp21D;void*_tmp21C;_LLB9: _tmp21C=_tmp21B.f1;_tmp21D=_tmp21B.f2;_LLBA: {void*e=_tmp21C;int b=_tmp21D;
({struct Cyc_List_List*_tmp3E1=Cyc_Tctyp_add_free_evar(env.free_evars,e,b);env.free_evars=_tmp3E1;});}}}
# 974
goto _LL3;}}}}}}}case 6U: _LL16: _tmp12F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp128)->f1;_LL17: {struct Cyc_List_List*tq_ts=_tmp12F;
# 977
for(0;tq_ts != 0;tq_ts=tq_ts->tl){
struct _tuple17*_tmp220=(struct _tuple17*)tq_ts->hd;struct _tuple17*p=_tmp220;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tmk,(*p).f2,1,0);
({int _tmp3E2=
Cyc_Tcutil_extract_const_from_typedef(loc,((*p).f1).print_const,(*p).f2);
# 980
((*p).f1).real_const=_tmp3E2;});}
# 983
goto _LL3;}case 7U: _LL18: _tmp12D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp128)->f1;_tmp12E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp128)->f2;_LL19: {enum Cyc_Absyn_AggrKind k=_tmp12D;struct Cyc_List_List*fs=_tmp12E;
# 987
struct Cyc_List_List*prev_fields=0;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp221=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp28=_tmp221;struct Cyc_Absyn_Aggrfield*_tmp222=_stmttmp28;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_List_List*_tmp227;struct Cyc_Absyn_Exp*_tmp226;void*_tmp225;struct Cyc_Absyn_Tqual*_tmp224;struct _fat_ptr*_tmp223;_LLBC: _tmp223=_tmp222->name;_tmp224=(struct Cyc_Absyn_Tqual*)& _tmp222->tq;_tmp225=_tmp222->type;_tmp226=_tmp222->width;_tmp227=_tmp222->attributes;_tmp228=_tmp222->requires_clause;_LLBD: {struct _fat_ptr*fn=_tmp223;struct Cyc_Absyn_Tqual*tqp=_tmp224;void*t=_tmp225;struct Cyc_Absyn_Exp*width=_tmp226;struct Cyc_List_List*atts=_tmp227;struct Cyc_Absyn_Exp*requires_clause=_tmp228;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FD;_tmp2FD.tag=0U,({struct _fat_ptr _tmp3E3=({const char*_tmp22C="duplicate field ";_tag_fat(_tmp22C,sizeof(char),17U);});_tmp2FD.f1=_tmp3E3;});_tmp2FD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FC;_tmp2FC.tag=0U,_tmp2FC.f1=*fn;_tmp2FC;});void*_tmp229[2U];_tmp229[0]=& _tmp22A,_tmp229[1]=& _tmp22B;({unsigned _tmp3E4=loc;Cyc_Warn_err2(_tmp3E4,_tag_fat(_tmp229,sizeof(void*),2U));});});
if(({struct _fat_ptr _tmp3E5=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp3E5,({const char*_tmp22D="";_tag_fat(_tmp22D,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E->hd=fn,_tmp22E->tl=prev_fields;_tmp22E;});
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tmk,t,1,0);
({int _tmp3E6=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t);tqp->real_const=_tmp3E6;});
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Tctyp_check_field_atts(loc,fn,atts);
if(requires_clause != 0){
# 1000
if((int)k != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp230=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FE;_tmp2FE.tag=0U,({struct _fat_ptr _tmp3E7=({const char*_tmp231="@requires clause is only allowed on union members";_tag_fat(_tmp231,sizeof(char),50U);});_tmp2FE.f1=_tmp3E7;});_tmp2FE;});void*_tmp22F[1U];_tmp22F[0]=& _tmp230;({unsigned _tmp3E8=loc;Cyc_Warn_err2(_tmp3E8,_tag_fat(_tmp22F,sizeof(void*),1U));});});
({struct Cyc_Tcenv_Tenv*_tmp3E9=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3E9,0,requires_clause);});
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp233=({struct Cyc_Warn_String_Warn_Warg_struct _tmp301;_tmp301.tag=0U,({struct _fat_ptr _tmp3EA=({const char*_tmp237="@requires clause has type ";_tag_fat(_tmp237,sizeof(char),27U);});_tmp301.f1=_tmp3EA;});_tmp301;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp234=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp300;_tmp300.tag=3U,_tmp300.f1=(void*)requires_clause->topt;_tmp300;});struct Cyc_Warn_String_Warn_Warg_struct _tmp235=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FF;_tmp2FF.tag=0U,({
struct _fat_ptr _tmp3EB=({const char*_tmp236=" instead of integral type";_tag_fat(_tmp236,sizeof(char),26U);});_tmp2FF.f1=_tmp3EB;});_tmp2FF;});void*_tmp232[3U];_tmp232[0]=& _tmp233,_tmp232[1]=& _tmp234,_tmp232[2]=& _tmp235;({unsigned _tmp3EC=loc;Cyc_Warn_err2(_tmp3EC,_tag_fat(_tmp232,sizeof(void*),3U));});});
env=Cyc_Tctyp_i_check_type_level_exp(requires_clause,env);}}}
# 1009
goto _LL3;}default: _LL1A: _tmp129=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp128)->f1;_tmp12A=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp128)->f2;_tmp12B=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp128)->f3;_tmp12C=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp128)->f4;_LL1B: {struct _tuple1*tdn=_tmp129;struct Cyc_List_List**targs_ref=_tmp12A;struct Cyc_Absyn_Typedefdecl**tdopt=_tmp12B;void**toptp=_tmp12C;
# 1013
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp238;_push_handler(& _tmp238);{int _tmp23A=0;if(setjmp(_tmp238.handler))_tmp23A=1;if(!_tmp23A){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);;_pop_handler();}else{void*_tmp239=(void*)Cyc_Core_get_exn_thrown();void*_tmp23B=_tmp239;void*_tmp23C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp23B)->tag == Cyc_Dict_Absent){_LLBF: _LLC0:
# 1016
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp303;_tmp303.tag=0U,({struct _fat_ptr _tmp3ED=({const char*_tmp240="unbound typedef name ";_tag_fat(_tmp240,sizeof(char),22U);});_tmp303.f1=_tmp3ED;});_tmp303;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp23F=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp302;_tmp302.tag=1U,_tmp302.f1=tdn;_tmp302;});void*_tmp23D[2U];_tmp23D[0]=& _tmp23E,_tmp23D[1]=& _tmp23F;({unsigned _tmp3EE=loc;Cyc_Warn_err2(_tmp3EE,_tag_fat(_tmp23D,sizeof(void*),2U));});});
return env;}else{_LLC1: _tmp23C=_tmp23B;_LLC2: {void*exn=_tmp23C;(int)_rethrow(exn);}}_LLBE:;}}}
# 1019
*tdopt=td;
env=Cyc_Tctyp_check_type_inst(env,targs_ref,td->tvs,expected_kind,allow_abs_aggr,tdn,td);
# 1022
if(td->defn != 0){
struct Cyc_List_List*inst=0;
struct Cyc_List_List*_tmp241=td->tvs;struct Cyc_List_List*tvs=_tmp241;
struct Cyc_List_List*_tmp242=*targs_ref;struct Cyc_List_List*ts=_tmp242;
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
inst=({struct Cyc_List_List*_tmp244=_cycalloc(sizeof(*_tmp244));({struct _tuple18*_tmp3EF=({struct _tuple18*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp243->f2=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;_tmp243;});_tmp244->hd=_tmp3EF;}),_tmp244->tl=inst;_tmp244;});}
({void*_tmp3F0=Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));*toptp=_tmp3F0;});}
# 1030
goto _LL3;}}_LL3:;}
# 1032
if(!({struct Cyc_Absyn_Kind*_tmp3F1=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmp3F1,expected_kind);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp246=({struct Cyc_Warn_String_Warn_Warg_struct _tmp309;_tmp309.tag=0U,({struct _fat_ptr _tmp3F2=({const char*_tmp24E="type ";_tag_fat(_tmp24E,sizeof(char),6U);});_tmp309.f1=_tmp3F2;});_tmp309;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp247=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp308;_tmp308.tag=2U,_tmp308.f1=(void*)t;_tmp308;});struct Cyc_Warn_String_Warn_Warg_struct _tmp248=({struct Cyc_Warn_String_Warn_Warg_struct _tmp307;_tmp307.tag=0U,({struct _fat_ptr _tmp3F3=({const char*_tmp24D=" has kind ";_tag_fat(_tmp24D,sizeof(char),11U);});_tmp307.f1=_tmp3F3;});_tmp307;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp249=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp306;_tmp306.tag=9U,({struct Cyc_Absyn_Kind*_tmp3F4=Cyc_Tcutil_type_kind(t);_tmp306.f1=_tmp3F4;});_tmp306;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp305;_tmp305.tag=0U,({
struct _fat_ptr _tmp3F5=({const char*_tmp24C=" but as used here needs kind ";_tag_fat(_tmp24C,sizeof(char),30U);});_tmp305.f1=_tmp3F5;});_tmp305;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp24B=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp304;_tmp304.tag=9U,_tmp304.f1=expected_kind;_tmp304;});void*_tmp245[6U];_tmp245[0]=& _tmp246,_tmp245[1]=& _tmp247,_tmp245[2]=& _tmp248,_tmp245[3]=& _tmp249,_tmp245[4]=& _tmp24A,_tmp245[5]=& _tmp24B;({unsigned _tmp3F6=loc;Cyc_Warn_err2(_tmp3F6,_tag_fat(_tmp245,sizeof(void*),6U));});});
return env;}}
# 1038
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1040
struct Cyc_List_List*_tmp24F=cvt.kind_env;struct Cyc_List_List*kind_env=_tmp24F;
cvt=Cyc_Tctyp_i_check_type(cvt,expected_kind,t,1,allow_abs_aggr);
# 1043
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
({struct Cyc_List_List*_tmp3F7=Cyc_Tctyp_fast_add_free_tvar(kind_env,(*((struct _tuple14*)vs->hd)).f1);cvt.kind_env=_tmp3F7;});}}
# 1049
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp250=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp29=_tmp250;struct _tuple13 _tmp251=_stmttmp29;void*_tmp252;_LL1: _tmp252=_tmp251.f1;_LL2: {void*e=_tmp252;
void*_tmp253=Cyc_Tcutil_compress(e);void*_stmttmp2A=_tmp253;void*_tmp254=_stmttmp2A;struct Cyc_Core_Opt**_tmp255;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp254)->tag == 1U){_LL4: _tmp255=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp254)->f4;_LL5: {struct Cyc_Core_Opt**s=_tmp255;
# 1053
if(*s == 0)
({struct Cyc_Core_Opt*_tmp3F8=({struct Cyc_Core_Opt*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->v=kind_env;_tmp256;});*s=_tmp3F8;});else{
# 1057
struct Cyc_List_List*_tmp257=0;struct Cyc_List_List*result=_tmp257;
{struct Cyc_List_List*_tmp258=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*s))->v;struct Cyc_List_List*tvs=_tmp258;for(0;tvs != 0;tvs=tvs->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)tvs->hd))
result=({struct Cyc_List_List*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp259->tl=result;_tmp259;});}}
({struct Cyc_Core_Opt*_tmp3F9=({struct Cyc_Core_Opt*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->v=result;_tmp25A;});*s=_tmp3F9;});}
# 1063
goto _LL3;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}}}
# 1067
return cvt;}
# 1074
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=
Cyc_Tcutil_is_function_type(t)&&(!te->in_tempest || te->tempest_generalize);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_List_List*_tmp25B=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*te_vars=_tmp25B;
struct Cyc_Tctyp_CVTEnv _tmp25C=({struct Cyc_Tctyp_CVTEnv _tmp3FB=({struct Cyc_Tctyp_CVTEnv _tmp312;_tmp312.loc=loc,_tmp312.te=te,_tmp312.kind_env=te_vars,_tmp312.fn_result=0,_tmp312.generalize_evars=generalize_evars,_tmp312.free_vars=0,_tmp312.free_evars=0;_tmp312;});struct Cyc_Absyn_Kind*_tmp3FA=expected_kind;Cyc_Tctyp_check_valid_type(_tmp3FB,_tmp3FA,1,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp25C;
# 1082
struct Cyc_List_List*_tmp25D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst,cvt.free_vars);struct Cyc_List_List*free_tvars=_tmp25D;
struct Cyc_List_List*_tmp25E=cvt.free_evars;struct Cyc_List_List*free_evars=_tmp25E;
# 1085
if(te_vars != 0){
struct Cyc_List_List*_tmp25F=0;struct Cyc_List_List*res=_tmp25F;
{struct Cyc_List_List*_tmp260=free_tvars;struct Cyc_List_List*fs=_tmp260;for(0;(unsigned)fs;fs=fs->tl){
struct Cyc_List_List*_tmp261=te_vars;struct Cyc_List_List*ts=_tmp261;
for(0;ts != 0;ts=ts->tl){
if(Cyc_Absyn_tvar_cmp((struct Cyc_Absyn_Tvar*)fs->hd,(struct Cyc_Absyn_Tvar*)ts->hd)== 0)
break;}
if(ts == 0)
res=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=(struct Cyc_Absyn_Tvar*)fs->hd,_tmp262->tl=res;_tmp262;});}}
# 1095
free_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);}
# 1100
{struct Cyc_List_List*x=free_tvars;for(0;x != 0;x=x->tl){
void*_tmp263=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_stmttmp2B=_tmp263;void*_tmp264=_stmttmp2B;enum Cyc_Absyn_AliasQual _tmp265;struct Cyc_Absyn_Kind*_tmp267;struct Cyc_Core_Opt**_tmp266;struct Cyc_Core_Opt**_tmp268;switch(*((int*)_tmp264)){case 1U: _LL1: _tmp268=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp264)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp268;
({struct Cyc_Core_Opt*_tmp3FC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3FC;});goto _LL0;}case 2U: _LL3: _tmp266=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp264)->f1;_tmp267=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp264)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmp266;struct Cyc_Absyn_Kind*k=_tmp267;
# 1104
{struct Cyc_Absyn_Kind*_tmp269=k;switch(((struct Cyc_Absyn_Kind*)_tmp269)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp269)->aliasqual == Cyc_Absyn_Top){_LLA: _LLB:
 goto _LLD;}else{goto _LL14;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)_tmp269)->aliasqual){case Cyc_Absyn_Top: _LLC: _LLD:
 goto _LLF;case Cyc_Absyn_Aliasable: _LLE: _LLF:
({struct Cyc_Core_Opt*_tmp3FD=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3FD;});goto _LL9;case Cyc_Absyn_Unique: _LL10: _LL11:
({struct Cyc_Core_Opt*_tmp3FE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp3FE;});goto _LL9;default: goto _LL14;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp269)->aliasqual == Cyc_Absyn_Top){_LL12: _LL13:
({struct Cyc_Core_Opt*_tmp3FF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp3FF;});goto _LL9;}else{goto _LL14;}default: _LL14: _LL15:
({struct Cyc_Core_Opt*_tmp400=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp400;});goto _LL9;}_LL9:;}
# 1112
goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp264)->f1)->kind == Cyc_Absyn_MemKind){_LL5: _tmp265=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp264)->f1)->aliasqual;_LL6: {enum Cyc_Absyn_AliasQual a=_tmp265;
# 1114
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30D;_tmp30D.tag=0U,({struct _fat_ptr _tmp401=({const char*_tmp271="type variable ";_tag_fat(_tmp271,sizeof(char),15U);});_tmp30D.f1=_tmp401;});_tmp30D;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp26C=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp30C;_tmp30C.tag=7U,_tmp30C.f1=(struct Cyc_Absyn_Tvar*)x->hd;_tmp30C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp26D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30B;_tmp30B.tag=0U,({struct _fat_ptr _tmp402=({const char*_tmp270=" cannot have kind ";_tag_fat(_tmp270,sizeof(char),19U);});_tmp30B.f1=_tmp402;});_tmp30B;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp26E=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp30A;_tmp30A.tag=9U,({struct Cyc_Absyn_Kind*_tmp403=({struct Cyc_Absyn_Kind*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->kind=Cyc_Absyn_MemKind,_tmp26F->aliasqual=a;_tmp26F;});_tmp30A.f1=_tmp403;});_tmp30A;});void*_tmp26A[4U];_tmp26A[0]=& _tmp26B,_tmp26A[1]=& _tmp26C,_tmp26A[2]=& _tmp26D,_tmp26A[3]=& _tmp26E;({unsigned _tmp404=loc;Cyc_Warn_err2(_tmp404,_tag_fat(_tmp26A,sizeof(void*),4U));});});
goto _LL0;}}else{_LL7: _LL8:
 goto _LL0;}}_LL0:;}}
# 1120
if(free_tvars != 0 || free_evars != 0){
{void*_tmp272=Cyc_Tcutil_compress(t);void*_stmttmp2C=_tmp272;void*_tmp273=_stmttmp2C;struct Cyc_List_List**_tmp274;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp273)->tag == 5U){_LL17: _tmp274=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp273)->f1).tvars;_LL18: {struct Cyc_List_List**btvs=_tmp274;
# 1123
if(*btvs == 0){
# 1125
({struct Cyc_List_List*_tmp405=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(free_tvars);*btvs=_tmp405;});
free_tvars=0;}
# 1128
goto _LL16;}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 1131
if(free_tvars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp276=({struct Cyc_Warn_String_Warn_Warg_struct _tmp311;_tmp311.tag=0U,({struct _fat_ptr _tmp406=({const char*_tmp27B="unbound type variable ";_tag_fat(_tmp27B,sizeof(char),23U);});_tmp311.f1=_tmp406;});_tmp311;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp277=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp310;_tmp310.tag=7U,_tmp310.f1=(struct Cyc_Absyn_Tvar*)free_tvars->hd;_tmp310;});struct Cyc_Warn_String_Warn_Warg_struct _tmp278=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30F;_tmp30F.tag=0U,({struct _fat_ptr _tmp407=({const char*_tmp27A=" when checking ";_tag_fat(_tmp27A,sizeof(char),16U);});_tmp30F.f1=_tmp407;});_tmp30F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp279=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp30E;_tmp30E.tag=2U,_tmp30E.f1=(void*)t;_tmp30E;});void*_tmp275[4U];_tmp275[0]=& _tmp276,_tmp275[1]=& _tmp277,_tmp275[2]=& _tmp278,_tmp275[3]=& _tmp279;({unsigned _tmp408=loc;Cyc_Warn_err2(_tmp408,_tag_fat(_tmp275,sizeof(void*),4U));});});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}}
# 1142
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1145
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_tmp27C=Cyc_Tcutil_compress(t);void*_stmttmp2D=_tmp27C;void*_tmp27D=_stmttmp2D;struct Cyc_Absyn_FnInfo _tmp27E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp27D)->tag == 5U){_LL1: _tmp27E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp27D)->f1;_LL2: {struct Cyc_Absyn_FnInfo i=_tmp27E;
# 1148
struct Cyc_List_List*_tmp27F=(fd->i).attributes;struct Cyc_List_List*atts=_tmp27F;
fd->i=i;
(fd->i).attributes=atts;
({int _tmp409=
Cyc_Tcutil_extract_const_from_typedef(loc,(i.ret_tqual).print_const,i.ret_type);
# 1151
((fd->i).ret_tqual).real_const=_tmp409;});
# 1153
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp281=({struct Cyc_Warn_String_Warn_Warg_struct _tmp313;_tmp313.tag=0U,({struct _fat_ptr _tmp40A=({const char*_tmp282="check_fndecl_valid_type: not a FnType";_tag_fat(_tmp282,sizeof(char),38U);});_tmp313.f1=_tmp40A;});_tmp313;});void*_tmp280[1U];_tmp280[0]=& _tmp281;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp280,sizeof(void*),1U));});}_LL0:;}{
# 1156
struct Cyc_List_List*_tmp283=0;struct Cyc_List_List*argnames=_tmp283;
{struct Cyc_List_List*_tmp284=(fd->i).args;struct Cyc_List_List*args=_tmp284;for(0;args != 0;args=args->tl){
argnames=({struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=(struct _fat_ptr*)_check_null((*((struct _tuple9*)args->hd)).f1),_tmp285->tl=argnames;_tmp285;});}}
({struct Cyc_List_List*_tmp40C=argnames;unsigned _tmp40B=loc;Cyc_Tcutil_check_unique_vars(_tmp40C,_tmp40B,({const char*_tmp286="function declaration has repeated parameter";_tag_fat(_tmp286,sizeof(char),44U);}));});
# 1161
fd->cached_type=t;}}
# 1166
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1169
struct Cyc_Tctyp_CVTEnv _tmp287=({struct Cyc_Tctyp_CVTEnv _tmp40F=({struct Cyc_Tctyp_CVTEnv _tmp318;_tmp318.loc=loc,_tmp318.te=te,_tmp318.kind_env=bound_tvars,_tmp318.fn_result=0,_tmp318.generalize_evars=0,_tmp318.free_vars=0,_tmp318.free_evars=0;_tmp318;});struct Cyc_Absyn_Kind*_tmp40E=expected_kind;int _tmp40D=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_tmp40F,_tmp40E,_tmp40D,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp287;
# 1171
struct Cyc_List_List*_tmp288=((struct Cyc_List_List*(*)(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars)(cvt.free_vars,bound_tvars,(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst);struct Cyc_List_List*free_tvs=_tmp288;
for(0;free_tvs != 0;free_tvs=free_tvs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp317;_tmp317.tag=0U,({struct _fat_ptr _tmp410=({const char*_tmp28F="unbound type variable ";_tag_fat(_tmp28F,sizeof(char),23U);});_tmp317.f1=_tmp410;});_tmp317;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp28B=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp316;_tmp316.tag=7U,_tmp316.f1=(*((struct _tuple14*)free_tvs->hd)).f1;_tmp316;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp315;_tmp315.tag=0U,({struct _fat_ptr _tmp411=({const char*_tmp28E=" in type ";_tag_fat(_tmp28E,sizeof(char),10U);});_tmp315.f1=_tmp411;});_tmp315;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp314;_tmp314.tag=2U,_tmp314.f1=(void*)t;_tmp314;});void*_tmp289[4U];_tmp289[0]=& _tmp28A,_tmp289[1]=& _tmp28B,_tmp289[2]=& _tmp28C,_tmp289[3]=& _tmp28D;({unsigned _tmp412=loc;Cyc_Warn_err2(_tmp412,_tag_fat(_tmp289,sizeof(void*),4U));});});}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(cvt.free_evars,t,loc);}

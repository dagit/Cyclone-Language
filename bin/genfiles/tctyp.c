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
void*Cyc_Core_fst(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 290
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 303
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 310
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 876 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 878
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 890
void*Cyc_Absyn_compress_kb(void*);
# 896
int Cyc_Absyn_type2bool(int def,void*);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 915
extern void*Cyc_Absyn_empty_effect;
# 917
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 919
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);
# 938
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar();
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar();
# 948
void*Cyc_Absyn_bounds_one();
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1132
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
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
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
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 68
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 70
void*Cyc_Warn_impos2(struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tctyp_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;int fn_result;int generalize_evars;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;};
# 56 "tctyp.cyc"
static struct _fat_ptr*Cyc_Tctyp_fst_fdarg(struct _tuple9*t){return(struct _fat_ptr*)_check_null((*t).f1);}struct _tuple13{void*f1;int f2;};
# 62
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 64
void*_tmp0=Cyc_Tcutil_compress(e);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;int _tmp2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->tag == 1U){_LL1: _tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->f3;_LL2: {int i=_tmp2;
# 66
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple13*_tmp3=(struct _tuple13*)es2->hd;struct _tuple13*_stmttmp1=_tmp3;struct _tuple13*_tmp4=_stmttmp1;int*_tmp6;void*_tmp5;_LL6: _tmp5=_tmp4->f1;_tmp6=(int*)& _tmp4->f2;_LL7: {void*t=_tmp5;int*b2=_tmp6;
void*_tmp7=Cyc_Tcutil_compress(t);void*_stmttmp2=_tmp7;void*_tmp8=_stmttmp2;int _tmp9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8)->tag == 1U){_LL9: _tmp9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8)->f3;_LLA: {int j=_tmp9;
# 70
if(i == j){
if(b != *b2)*b2=1;
return es;}
# 74
goto _LL8;}}else{_LLB: _LLC:
 goto _LL8;}_LL8:;}}}
# 78
return({struct Cyc_List_List*_tmpB=_region_malloc(r,sizeof(*_tmpB));({struct _tuple13*_tmp35A=({struct _tuple13*_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA->f1=e,_tmpA->f2=b;_tmpA;});_tmpB->hd=_tmp35A;}),_tmpB->tl=es;_tmpB;});}}else{_LL3: _LL4:
 return es;}_LL0:;}
# 84
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp35B=({const char*_tmp11="bound tvar id for ";_tag_fat(_tmp11,sizeof(char),19U);});_tmp2C1.f1=_tmp35B;});_tmp2C1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpE=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2C0;_tmp2C0.tag=6U,_tmp2C0.f1=tv;_tmp2C0;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BF;_tmp2BF.tag=0U,({struct _fat_ptr _tmp35C=({const char*_tmp10=" is NULL";_tag_fat(_tmp10,sizeof(char),9U);});_tmp2BF.f1=_tmp35C;});_tmp2BF;});void*_tmpC[3U];_tmpC[0]=& _tmpD,_tmpC[1]=& _tmpE,_tmpC[2]=& _tmpF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC,sizeof(void*),3U));});
return({struct Cyc_List_List*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=tv,_tmp12->tl=tvs;_tmp12;});}
# 89
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 92
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 100
if(!present)
r=({struct Cyc_List_List*_tmp13=_region_malloc(rgn,sizeof(*_tmp13));_tmp13->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp13->tl=r;_tmp13;});}
# 103
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}struct _tuple14{struct Cyc_Absyn_Tvar*f1;int f2;};
# 106
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 109
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp14=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp3=_tmp14;struct _tuple14 _tmp15=_stmttmp3;int _tmp17;struct Cyc_Absyn_Tvar*_tmp16;_LL1: _tmp16=_tmp15.f1;_tmp17=_tmp15.f2;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp16;int b=_tmp17;
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(tv->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 118
if(!present)
res=({struct Cyc_List_List*_tmp18=_region_malloc(r,sizeof(*_tmp18));_tmp18->hd=(struct _tuple14*)tvs->hd,_tmp18->tl=res;_tmp18;});}}
# 121
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 129
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
# 131
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
{void*_tmp19=Cyc_Tcutil_compress(e);void*_stmttmp4=_tmp19;void*_tmp1A=_stmttmp4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
 continue;}_LL0:;}{
# 137
struct Cyc_Absyn_Kind*_tmp1B=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_stmttmp5=_tmp1B;struct Cyc_Absyn_Kind*_tmp1C=_stmttmp5;switch(((struct Cyc_Absyn_Kind*)_tmp1C)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp1C)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
# 139
 if(!Cyc_Unify_unify(e,Cyc_Absyn_unique_rgn_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2;_tmp2C2.tag=0U,({struct _fat_ptr _tmp35D=({const char*_tmp1F="can't unify evar with unique region!";_tag_fat(_tmp1F,sizeof(char),37U);});_tmp2C2.f1=_tmp35D;});_tmp2C2;});void*_tmp1D[1U];_tmp1D[0]=& _tmp1E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1D,sizeof(void*),1U));});
goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
# 144
 if(!Cyc_Unify_unify(e,Cyc_Absyn_heap_rgn_type))({struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3;_tmp2C3.tag=0U,({struct _fat_ptr _tmp35E=({const char*_tmp22="can't unify evar with heap!";_tag_fat(_tmp22,sizeof(char),28U);});_tmp2C3.f1=_tmp35E;});_tmp2C3;});void*_tmp20[1U];_tmp20[0]=& _tmp21;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp20,sizeof(void*),1U));});
goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 147
 if(!Cyc_Unify_unify(e,Cyc_Absyn_empty_effect))({struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C4;_tmp2C4.tag=0U,({struct _fat_ptr _tmp35F=({const char*_tmp25="can't unify evar with {}!";_tag_fat(_tmp25,sizeof(char),26U);});_tmp2C4.f1=_tmp35F;});_tmp2C4;});void*_tmp23[1U];_tmp23[0]=& _tmp24;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp23,sizeof(void*),1U));});
goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
# 150
 if(!Cyc_Unify_unify(e,Cyc_Absyn_false_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C7;_tmp2C7.tag=0U,({struct _fat_ptr _tmp360=({const char*_tmp2B="can't unify evar ";_tag_fat(_tmp2B,sizeof(char),18U);});_tmp2C7.f1=_tmp360;});_tmp2C7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C6;_tmp2C6.tag=2U,_tmp2C6.f1=(void*)e;_tmp2C6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5;_tmp2C5.tag=0U,({struct _fat_ptr _tmp361=({const char*_tmp2A=" with @false!";_tag_fat(_tmp2A,sizeof(char),14U);});_tmp2C5.f1=_tmp361;});_tmp2C5;});void*_tmp26[3U];_tmp26[0]=& _tmp27,_tmp26[1]=& _tmp28,_tmp26[2]=& _tmp29;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp26,sizeof(void*),3U));});
goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
# 154
 if(!({void*_tmp362=e;Cyc_Unify_unify(_tmp362,Cyc_Absyn_bounds_one());}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8;_tmp2C8.tag=0U,({struct _fat_ptr _tmp363=({const char*_tmp2E="can't unify evar with bounds_one!";_tag_fat(_tmp2E,sizeof(char),34U);});_tmp2C8.f1=_tmp363;});_tmp2C8;});void*_tmp2C[1U];_tmp2C[0]=& _tmp2D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C,sizeof(void*),1U));});
goto _LL5;default: _LL12: _LL13:
# 158
({struct Cyc_Warn_String_Warn_Warg_struct _tmp30=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC;_tmp2CC.tag=0U,({struct _fat_ptr _tmp364=({const char*_tmp35="hidden type variable ";_tag_fat(_tmp35,sizeof(char),22U);});_tmp2CC.f1=_tmp364;});_tmp2CC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp31=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2CB;_tmp2CB.tag=2U,_tmp2CB.f1=(void*)e;_tmp2CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA;_tmp2CA.tag=0U,({struct _fat_ptr _tmp365=({const char*_tmp34=" isn't abstracted in type ";_tag_fat(_tmp34,sizeof(char),27U);});_tmp2CA.f1=_tmp365;});_tmp2CA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp33=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C9;_tmp2C9.tag=2U,_tmp2C9.f1=(void*)in_typ;_tmp2C9;});void*_tmp2F[4U];_tmp2F[0]=& _tmp30,_tmp2F[1]=& _tmp31,_tmp2F[2]=& _tmp32,_tmp2F[3]=& _tmp33;({unsigned _tmp366=loc;Cyc_Warn_err2(_tmp366,_tag_fat(_tmp2F,sizeof(void*),4U));});});
goto _LL5;}_LL5:;}}}
# 168
static int Cyc_Tctyp_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp36=({struct _tuple0 _tmp2CD;_tmp2CD.f1=c1,_tmp2CD.f2=c2;_tmp2CD;});struct _tuple0 _stmttmp6=_tmp36;struct _tuple0 _tmp37=_stmttmp6;struct Cyc_Absyn_Kind*_tmp3B;struct Cyc_Core_Opt**_tmp3A;struct Cyc_Absyn_Kind*_tmp39;struct Cyc_Core_Opt**_tmp38;struct Cyc_Absyn_Kind*_tmp3E;struct Cyc_Absyn_Kind*_tmp3D;struct Cyc_Core_Opt**_tmp3C;struct Cyc_Core_Opt**_tmp3F;struct Cyc_Core_Opt**_tmp40;struct Cyc_Absyn_Kind*_tmp43;struct Cyc_Core_Opt**_tmp42;struct Cyc_Absyn_Kind*_tmp41;struct Cyc_Absyn_Kind*_tmp45;struct Cyc_Absyn_Kind*_tmp44;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp37.f1)->tag == 0U)switch(*((int*)_tmp37.f2)){case 0U: _LL1: _tmp44=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp37.f1)->f1;_tmp45=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp37.f2)->f1;_LL2: {struct Cyc_Absyn_Kind*k1=_tmp44;struct Cyc_Absyn_Kind*k2=_tmp45;
return k1 == k2;}case 1U: goto _LL3;default: _LL9: _tmp41=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp37.f1)->f1;_tmp42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f2)->f1;_tmp43=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f2)->f2;_LLA: {struct Cyc_Absyn_Kind*k1=_tmp41;struct Cyc_Core_Opt**f=_tmp42;struct Cyc_Absyn_Kind*k2=_tmp43;
# 181
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp367=({struct Cyc_Core_Opt*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->v=c1;_tmp49;});*f=_tmp367;});return 1;}else{
return 0;}}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp37.f2)->tag == 1U){_LL3: _tmp40=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp37.f2)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp40;
# 174
({struct Cyc_Core_Opt*_tmp368=({struct Cyc_Core_Opt*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->v=c1;_tmp46;});*f=_tmp368;});return 1;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp37.f1)->tag == 1U){_LL5: _tmp3F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp37.f1)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp3F;
({struct Cyc_Core_Opt*_tmp369=({struct Cyc_Core_Opt*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->v=c2;_tmp47;});*f=_tmp369;});return 1;}}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp37.f2)->tag == 0U){_LL7: _tmp3C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f1)->f1;_tmp3D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f1)->f2;_tmp3E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp37.f2)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp3C;struct Cyc_Absyn_Kind*k1=_tmp3D;struct Cyc_Absyn_Kind*k2=_tmp3E;
# 177
if(Cyc_Tcutil_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp36A=({struct Cyc_Core_Opt*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->v=c2;_tmp48;});*f=_tmp36A;});return 1;}else{
return 0;}}}else{_LLB: _tmp38=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f1)->f1;_tmp39=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f1)->f2;_tmp3A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f2)->f1;_tmp3B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp37.f2)->f2;_LLC: {struct Cyc_Core_Opt**f1=_tmp38;struct Cyc_Absyn_Kind*k1=_tmp39;struct Cyc_Core_Opt**f2=_tmp3A;struct Cyc_Absyn_Kind*k2=_tmp3B;
# 185
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp36B=({struct Cyc_Core_Opt*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->v=c1;_tmp4A;});*f2=_tmp36B;});return 1;}else{
if(Cyc_Tcutil_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp36C=({struct Cyc_Core_Opt*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->v=c2;_tmp4B;});*f1=_tmp36C;});return 1;}else{
return 0;}}}}}}}_LL0:;}}
# 198
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 201
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tctyp_constrain_kinds(k1,k2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3;_tmp2D3.tag=0U,({struct _fat_ptr _tmp36D=({const char*_tmp55="type variable ";_tag_fat(_tmp55,sizeof(char),15U);});_tmp2D3.f1=_tmp36D;});_tmp2D3;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp4E=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2D2;_tmp2D2.tag=6U,_tmp2D2.f1=tv;_tmp2D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D1;_tmp2D1.tag=0U,({struct _fat_ptr _tmp36E=({const char*_tmp54=" is used with inconsistent kinds ";_tag_fat(_tmp54,sizeof(char),34U);});_tmp2D1.f1=_tmp36E;});_tmp2D1;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp50=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp2D0;_tmp2D0.tag=7U,_tmp2D0.f1=(void*)k1;_tmp2D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp51=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF;_tmp2CF.tag=0U,({
struct _fat_ptr _tmp36F=({const char*_tmp53=" and ";_tag_fat(_tmp53,sizeof(char),6U);});_tmp2CF.f1=_tmp36F;});_tmp2CF;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp52=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp2CE;_tmp2CE.tag=7U,_tmp2CE.f1=(void*)k2;_tmp2CE;});void*_tmp4C[6U];_tmp4C[0]=& _tmp4D,_tmp4C[1]=& _tmp4E,_tmp4C[2]=& _tmp4F,_tmp4C[3]=& _tmp50,_tmp4C[4]=& _tmp51,_tmp4C[5]=& _tmp52;({unsigned _tmp370=loc;Cyc_Warn_err2(_tmp370,_tag_fat(_tmp4C,sizeof(void*),6U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp57=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4;_tmp2D4.tag=0U,({struct _fat_ptr _tmp371=({const char*_tmp58="same type variable has different identity!";_tag_fat(_tmp58,sizeof(char),43U);});_tmp2D4.f1=_tmp371;});_tmp2D4;});void*_tmp56[1U];_tmp56[0]=& _tmp57;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp56,sizeof(void*),1U));});}
return tvs;}}}
# 215
({int _tmp372=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp372;});
return({struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->hd=tv,_tmp59->tl=tvs;_tmp59;});}
# 221
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5;_tmp2D5.tag=0U,({struct _fat_ptr _tmp373=({const char*_tmp5C="fast_add_free_tvar: bad identity in tv";_tag_fat(_tmp5C,sizeof(char),39U);});_tmp2D5.f1=_tmp373;});_tmp2D5;});void*_tmp5A[1U];_tmp5A[0]=& _tmp5B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp5A,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp5D=(struct Cyc_Absyn_Tvar*)tvs2->hd;struct Cyc_Absyn_Tvar*tv2=_tmp5D;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6;_tmp2D6.tag=0U,({struct _fat_ptr _tmp374=({const char*_tmp60="fast_add_free_tvar: bad identity in tvs2";_tag_fat(_tmp60,sizeof(char),41U);});_tmp2D6.f1=_tmp374;});_tmp2D6;});void*_tmp5E[1U];_tmp5E[0]=& _tmp5F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp5E,sizeof(void*),1U));});
if(tv2->identity == tv->identity)
return tvs;}}
# 231
return({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=tv,_tmp61->tl=tvs;_tmp61;});}
# 237
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 240
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp63=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D7;_tmp2D7.tag=0U,({struct _fat_ptr _tmp375=({const char*_tmp64="fast_add_free_tvar_bool: bad identity in tv";_tag_fat(_tmp64,sizeof(char),44U);});_tmp2D7.f1=_tmp375;});_tmp2D7;});void*_tmp62[1U];_tmp62[0]=& _tmp63;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp62,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct _tuple14*_tmp65=(struct _tuple14*)tvs2->hd;struct _tuple14*_stmttmp7=_tmp65;struct _tuple14*_tmp66=_stmttmp7;int*_tmp68;struct Cyc_Absyn_Tvar*_tmp67;_LL1: _tmp67=_tmp66->f1;_tmp68=(int*)& _tmp66->f2;_LL2: {struct Cyc_Absyn_Tvar*tv2=_tmp67;int*b2=_tmp68;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D8;_tmp2D8.tag=0U,({struct _fat_ptr _tmp376=({const char*_tmp6B="fast_add_free_tvar_bool: bad identity in tvs2";_tag_fat(_tmp6B,sizeof(char),46U);});_tmp2D8.f1=_tmp376;});_tmp2D8;});void*_tmp69[1U];_tmp69[0]=& _tmp6A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp69,sizeof(void*),1U));});
if(tv2->identity == tv->identity){
*b2=*b2 || b;
return tvs;}}}}
# 251
return({struct Cyc_List_List*_tmp6D=_region_malloc(r,sizeof(*_tmp6D));({struct _tuple14*_tmp377=({struct _tuple14*_tmp6C=_region_malloc(r,sizeof(*_tmp6C));_tmp6C->f1=tv,_tmp6C->f2=b;_tmp6C;});_tmp6D->hd=_tmp377;}),_tmp6D->tl=tvs;_tmp6D;});}
# 254
static int Cyc_Tctyp_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp6E=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_stmttmp8=_tmp6E;void*_tmp6F=_stmttmp8;void*_tmp70;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->tag == 3U){_LL1: _tmp70=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp70;
# 259
{void*_tmp71=Cyc_Tcutil_compress(r);void*_stmttmp9=_tmp71;void*_tmp72=_stmttmp9;struct Cyc_Absyn_Tvar*_tmp73;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72)->tag == 2U){_LL6: _tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp72)->f1;_LL7: {struct Cyc_Absyn_Tvar*tv=_tmp73;
return Cyc_Absyn_tvar_cmp(tvar,tv)== 0;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 263
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 269
return 1;}
return 0;}
# 273
static struct Cyc_Absyn_Kind*Cyc_Tctyp_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 276
void*_tmp74=Cyc_Absyn_compress_kb(tvar->kind);void*_stmttmpA=_tmp74;void*_tmp75=_stmttmpA;switch(*((int*)_tmp75)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 285
 if((((int)expected_kind->kind == (int)Cyc_Absyn_BoxKind ||(int)expected_kind->kind == (int)Cyc_Absyn_MemKind)||(int)expected_kind->kind == (int)Cyc_Absyn_AnyKind)&&
# 288
 Cyc_Tctyp_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 294
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 299
static void Cyc_Tctyp_check_field_atts(unsigned loc,struct _fat_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp76=(void*)atts->hd;void*_stmttmpB=_tmp76;void*_tmp77=_stmttmpB;switch(*((int*)_tmp77)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp79=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC;_tmp2DC.tag=0U,({struct _fat_ptr _tmp378=({const char*_tmp7E="bad attribute ";_tag_fat(_tmp7E,sizeof(char),15U);});_tmp2DC.f1=_tmp378;});_tmp2DC;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp7A=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp2DB;_tmp2DB.tag=9U,_tmp2DB.f1=(void*)atts->hd;_tmp2DB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA;_tmp2DA.tag=0U,({struct _fat_ptr _tmp379=({const char*_tmp7D=" on member ";_tag_fat(_tmp7D,sizeof(char),12U);});_tmp2DA.f1=_tmp379;});_tmp2DA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9;_tmp2D9.tag=0U,_tmp2D9.f1=*fn;_tmp2D9;});void*_tmp78[4U];_tmp78[0]=& _tmp79,_tmp78[1]=& _tmp7A,_tmp78[2]=& _tmp7B,_tmp78[3]=& _tmp7C;({unsigned _tmp37A=loc;Cyc_Warn_err2(_tmp37A,_tag_fat(_tmp78,sizeof(void*),4U));});});}_LL0:;}}
# 309
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*,struct Cyc_Tcenv_Tenv*,struct Cyc_Tctyp_CVTEnv);struct _tuple15{struct Cyc_Tctyp_CVTEnv f1;struct Cyc_List_List*f2;};
# 312
static struct _tuple15 Cyc_Tctyp_check_clause(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 315
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0;_tmp2E0.tag=0U,_tmp2E0.f1=clause_name;_tmp2E0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp81=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF;_tmp2DF.tag=0U,({struct _fat_ptr _tmp37B=({const char*_tmp85=" clause has type ";_tag_fat(_tmp85,sizeof(char),18U);});_tmp2DF.f1=_tmp37B;});_tmp2DF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp82=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DE;_tmp2DE.tag=2U,_tmp2DE.f1=(void*)_check_null(clause->topt);_tmp2DE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp83=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DD;_tmp2DD.tag=0U,({
struct _fat_ptr _tmp37C=({const char*_tmp84=" instead of integral type";_tag_fat(_tmp84,sizeof(char),26U);});_tmp2DD.f1=_tmp37C;});_tmp2DD;});void*_tmp7F[4U];_tmp7F[0]=& _tmp80,_tmp7F[1]=& _tmp81,_tmp7F[2]=& _tmp82,_tmp7F[3]=& _tmp83;({unsigned _tmp37D=loc;Cyc_Warn_err2(_tmp37D,_tag_fat(_tmp7F,sizeof(void*),4U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0U,_tmp2E4.f1=clause_name;_tmp2E4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=0U,({
struct _fat_ptr _tmp37E=({const char*_tmp8C=" clause '";_tag_fat(_tmp8C,sizeof(char),10U);});_tmp2E3.f1=_tmp37E;});_tmp2E3;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp89=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2E2;_tmp2E2.tag=3U,_tmp2E2.f1=clause;_tmp2E2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1;_tmp2E1.tag=0U,({struct _fat_ptr _tmp37F=({const char*_tmp8B="' may be unsatisfiable";_tag_fat(_tmp8B,sizeof(char),23U);});_tmp2E1.f1=_tmp37F;});_tmp2E1;});void*_tmp86[4U];_tmp86[0]=& _tmp87,_tmp86[1]=& _tmp88,_tmp86[2]=& _tmp89,_tmp86[3]=& _tmp8A;({unsigned _tmp380=clause->loc;Cyc_Warn_err2(_tmp380,_tag_fat(_tmp86,sizeof(void*),4U));});});}
# 327
return({struct _tuple15 _tmp2E5;_tmp2E5.f1=cvtenv,_tmp2E5.f2=relns;_tmp2E5;});}
# 357 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr);
# 362
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_aggr(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 368
{union Cyc_Absyn_AggrInfo _tmp8D=*info;union Cyc_Absyn_AggrInfo _stmttmpC=_tmp8D;union Cyc_Absyn_AggrInfo _tmp8E=_stmttmpC;struct Cyc_Absyn_Aggrdecl*_tmp8F;struct Cyc_Core_Opt*_tmp92;struct _tuple1*_tmp91;enum Cyc_Absyn_AggrKind _tmp90;if((_tmp8E.UnknownAggr).tag == 1){_LL1: _tmp90=((_tmp8E.UnknownAggr).val).f1;_tmp91=((_tmp8E.UnknownAggr).val).f2;_tmp92=((_tmp8E.UnknownAggr).val).f3;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp90;struct _tuple1*n=_tmp91;struct Cyc_Core_Opt*tgd=_tmp92;
# 370
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp93;_push_handler(& _tmp93);{int _tmp95=0;if(setjmp(_tmp93.handler))_tmp95=1;if(!_tmp95){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);{
struct Cyc_Absyn_Aggrdecl*_tmp96=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp96;
if((int)ad->kind != (int)k){
if((int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=0U,({struct _fat_ptr _tmp381=({const char*_tmp9D="expecting struct ";_tag_fat(_tmp9D,sizeof(char),18U);});_tmp2E9.f1=_tmp381;});_tmp2E9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp99=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E8;_tmp2E8.tag=1U,_tmp2E8.f1=n;_tmp2E8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=0U,({struct _fat_ptr _tmp382=({const char*_tmp9C="instead of union ";_tag_fat(_tmp9C,sizeof(char),18U);});_tmp2E7.f1=_tmp382;});_tmp2E7;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=1U,_tmp2E6.f1=n;_tmp2E6;});void*_tmp97[4U];_tmp97[0]=& _tmp98,_tmp97[1]=& _tmp99,_tmp97[2]=& _tmp9A,_tmp97[3]=& _tmp9B;({unsigned _tmp383=loc;Cyc_Warn_err2(_tmp383,_tag_fat(_tmp97,sizeof(void*),4U));});});else{
# 378
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=0U,({struct _fat_ptr _tmp384=({const char*_tmpA4="expecting union ";_tag_fat(_tmpA4,sizeof(char),17U);});_tmp2ED.f1=_tmp384;});_tmp2ED;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2EC;_tmp2EC.tag=1U,_tmp2EC.f1=n;_tmp2EC;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=0U,({struct _fat_ptr _tmp385=({const char*_tmpA3="instead of struct ";_tag_fat(_tmpA3,sizeof(char),19U);});_tmp2EB.f1=_tmp385;});_tmp2EB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=1U,_tmp2EA.f1=n;_tmp2EA;});void*_tmp9E[4U];_tmp9E[0]=& _tmp9F,_tmp9E[1]=& _tmpA0,_tmp9E[2]=& _tmpA1,_tmp9E[3]=& _tmpA2;({unsigned _tmp386=loc;Cyc_Warn_err2(_tmp386,_tag_fat(_tmp9E,sizeof(void*),4U));});});}}
# 380
if((unsigned)tgd &&(int)tgd->v){
if(!((unsigned)ad->impl)|| !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0U,({struct _fat_ptr _tmp387=({const char*_tmpA8="@tagged qualfiers don't agree on union ";_tag_fat(_tmpA8,sizeof(char),40U);});_tmp2EF.f1=_tmp387;});_tmp2EF;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2EE;_tmp2EE.tag=1U,_tmp2EE.f1=n;_tmp2EE;});void*_tmpA5[2U];_tmpA5[0]=& _tmpA6,_tmpA5[1]=& _tmpA7;({unsigned _tmp388=loc;Cyc_Warn_err2(_tmp388,_tag_fat(_tmpA5,sizeof(void*),2U));});});}
# 385
({union Cyc_Absyn_AggrInfo _tmp389=Cyc_Absyn_KnownAggr(adp);*info=_tmp389;});}
# 372
;_pop_handler();}else{void*_tmp94=(void*)Cyc_Core_get_exn_thrown();void*_tmpA9=_tmp94;void*_tmpAA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA9)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 389
struct Cyc_Absyn_Aggrdecl*_tmpAB=({struct Cyc_Absyn_Aggrdecl*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->kind=k,_tmpB2->sc=Cyc_Absyn_Extern,_tmpB2->name=n,_tmpB2->tvs=0,_tmpB2->impl=0,_tmpB2->attributes=0,_tmpB2->expected_mem_kind=0;_tmpB2;});struct Cyc_Absyn_Aggrdecl*ad=_tmpAB;
Cyc_Tc_tcAggrdecl(te,loc,ad);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);
({union Cyc_Absyn_AggrInfo _tmp38A=Cyc_Absyn_KnownAggr(adp);*info=_tmp38A;});
# 394
if(*targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2;_tmp2F2.tag=0U,({struct _fat_ptr _tmp38B=({const char*_tmpB1="declare parameterized type ";_tag_fat(_tmpB1,sizeof(char),28U);});_tmp2F2.f1=_tmp38B;});_tmp2F2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=1U,_tmp2F1.f1=n;_tmp2F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=0U,({struct _fat_ptr _tmp38C=({const char*_tmpB0=" before using";_tag_fat(_tmpB0,sizeof(char),14U);});_tmp2F0.f1=_tmp38C;});_tmp2F0;});void*_tmpAC[3U];_tmpAC[0]=& _tmpAD,_tmpAC[1]=& _tmpAE,_tmpAC[2]=& _tmpAF;({unsigned _tmp38D=loc;Cyc_Warn_err2(_tmp38D,_tag_fat(_tmpAC,sizeof(void*),3U));});});
return cvtenv;}
# 398
goto _LL5;}}else{_LL8: _tmpAA=_tmpA9;_LL9: {void*exn=_tmpAA;(int)_rethrow(exn);}}_LL5:;}}}
# 400
_tmp8F=*adp;goto _LL4;}}else{_LL3: _tmp8F=*(_tmp8E.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp8F;
# 402
struct Cyc_List_List*tvs=ad->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0 && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpB3=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmpB3;
void*_tmpB4=(void*)ts->hd;void*t=_tmpB4;
# 410
{struct _tuple0 _tmpB5=({struct _tuple0 _tmp2F3;({void*_tmp38E=Cyc_Absyn_compress_kb(tv->kind);_tmp2F3.f1=_tmp38E;}),_tmp2F3.f2=t;_tmp2F3;});struct _tuple0 _stmttmpD=_tmpB5;struct _tuple0 _tmpB6=_stmttmpD;struct Cyc_Absyn_Tvar*_tmpB7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB6.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB6.f2)->tag == 2U){_LLB: _tmpB7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB6.f2)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv2=_tmpB7;
# 412
({struct Cyc_List_List*_tmp38F=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,tv2);cvtenv.kind_env=_tmp38F;});
({struct Cyc_List_List*_tmp390=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,tv2,1);cvtenv.free_vars=_tmp390;});
continue;}}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 417
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);}}
# 421
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F5;_tmp2F5.tag=0U,({struct _fat_ptr _tmp391=({const char*_tmpBB="too many parameters for ";_tag_fat(_tmpBB,sizeof(char),25U);});_tmp2F5.f1=_tmp391;});_tmp2F5;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2F4;_tmp2F4.tag=5U,_tmp2F4.f1=ad;_tmp2F4;});void*_tmpB8[2U];_tmpB8[0]=& _tmpB9,_tmpB8[1]=& _tmpBA;({unsigned _tmp392=loc;Cyc_Warn_err2(_tmp392,_tag_fat(_tmpB8,sizeof(void*),2U));});});
if(tvs != 0){
# 425
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->hd=e,_tmpBC->tl=hidden_ts;_tmpBC;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 432
({struct Cyc_List_List*_tmp394=({struct Cyc_List_List*_tmp393=*targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp393,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs=_tmp394;});}
# 434
if((allow_abs_aggr && ad->impl == 0)&& !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 439
ad->expected_mem_kind=1;}}_LL0:;}
# 442
return cvtenv;}
# 446
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_datatype(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 452
struct Cyc_List_List*_tmpBD=*targsp;struct Cyc_List_List*targs=_tmpBD;
{union Cyc_Absyn_DatatypeInfo _tmpBE=*info;union Cyc_Absyn_DatatypeInfo _stmttmpE=_tmpBE;union Cyc_Absyn_DatatypeInfo _tmpBF=_stmttmpE;struct Cyc_Absyn_Datatypedecl*_tmpC0;int _tmpC2;struct _tuple1*_tmpC1;if((_tmpBF.UnknownDatatype).tag == 1){_LL1: _tmpC1=((_tmpBF.UnknownDatatype).val).name;_tmpC2=((_tmpBF.UnknownDatatype).val).is_extensible;_LL2: {struct _tuple1*n=_tmpC1;int is_x=_tmpC2;
# 455
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpC3;_push_handler(& _tmpC3);{int _tmpC5=0;if(setjmp(_tmpC3.handler))_tmpC5=1;if(!_tmpC5){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);;_pop_handler();}else{void*_tmpC4=(void*)Cyc_Core_get_exn_thrown();void*_tmpC6=_tmpC4;void*_tmpC7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC6)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 459
struct Cyc_Absyn_Datatypedecl*_tmpC8=({struct Cyc_Absyn_Datatypedecl*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->sc=Cyc_Absyn_Extern,_tmpCF->name=n,_tmpCF->tvs=0,_tmpCF->fields=0,_tmpCF->is_extensible=is_x;_tmpCF;});struct Cyc_Absyn_Datatypedecl*tud=_tmpC8;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 463
if(targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F8;_tmp2F8.tag=0U,({struct _fat_ptr _tmp395=({const char*_tmpCE="declare parameterized datatype ";_tag_fat(_tmpCE,sizeof(char),32U);});_tmp2F8.f1=_tmp395;});_tmp2F8;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F7;_tmp2F7.tag=1U,_tmp2F7.f1=n;_tmp2F7;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6;_tmp2F6.tag=0U,({struct _fat_ptr _tmp396=({const char*_tmpCD=" before using";_tag_fat(_tmpCD,sizeof(char),14U);});_tmp2F6.f1=_tmp396;});_tmp2F6;});void*_tmpC9[3U];_tmpC9[0]=& _tmpCA,_tmpC9[1]=& _tmpCB,_tmpC9[2]=& _tmpCC;({unsigned _tmp397=loc;Cyc_Warn_err2(_tmp397,_tag_fat(_tmpC9,sizeof(void*),3U));});});
return cvtenv;}
# 467
goto _LL5;}}else{_LL8: _tmpC7=_tmpC6;_LL9: {void*exn=_tmpC7;(int)_rethrow(exn);}}_LL5:;}}}
# 471
if(is_x && !(*tudp)->is_extensible)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB;_tmp2FB.tag=0U,({struct _fat_ptr _tmp398=({const char*_tmpD5="datatype ";_tag_fat(_tmpD5,sizeof(char),10U);});_tmp2FB.f1=_tmp398;});_tmp2FB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpD2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FA;_tmp2FA.tag=1U,_tmp2FA.f1=n;_tmp2FA;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F9;_tmp2F9.tag=0U,({struct _fat_ptr _tmp399=({const char*_tmpD4=" was not declared @extensible";_tag_fat(_tmpD4,sizeof(char),30U);});_tmp2F9.f1=_tmp399;});_tmp2F9;});void*_tmpD0[3U];_tmpD0[0]=& _tmpD1,_tmpD0[1]=& _tmpD2,_tmpD0[2]=& _tmpD3;({unsigned _tmp39A=loc;Cyc_Warn_err2(_tmp39A,_tag_fat(_tmpD0,sizeof(void*),3U));});});
({union Cyc_Absyn_DatatypeInfo _tmp39B=Cyc_Absyn_KnownDatatype(tudp);*info=_tmp39B;});
_tmpC0=*tudp;goto _LL4;}}else{_LL3: _tmpC0=*(_tmpBF.KnownDatatype).val;_LL4: {struct Cyc_Absyn_Datatypedecl*tud=_tmpC0;
# 477
struct Cyc_List_List*tvs=tud->tvs;
for(0;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 483
{struct _tuple0 _tmpD6=({struct _tuple0 _tmp2FC;({void*_tmp39C=Cyc_Absyn_compress_kb(tv->kind);_tmp2FC.f1=_tmp39C;}),_tmp2FC.f2=t;_tmp2FC;});struct _tuple0 _stmttmpF=_tmpD6;struct _tuple0 _tmpD7=_stmttmpF;struct Cyc_Absyn_Tvar*_tmpD8;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD7.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD7.f2)->tag == 2U){_LLB: _tmpD8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD7.f2)->f1;_LLC: {struct Cyc_Absyn_Tvar*tv2=_tmpD8;
# 485
({struct Cyc_List_List*_tmp39D=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,tv2);cvtenv.kind_env=_tmp39D;});
({struct Cyc_List_List*_tmp39E=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,tv2,1);cvtenv.free_vars=_tmp39E;});
continue;}}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 490
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);}}
# 494
if(targs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FE;_tmp2FE.tag=0U,({struct _fat_ptr _tmp39F=({const char*_tmpDC="too many type arguments for datatype ";_tag_fat(_tmpDC,sizeof(char),38U);});_tmp2FE.f1=_tmp39F;});_tmp2FE;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpDB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FD;_tmp2FD.tag=1U,_tmp2FD.f1=tud->name;_tmp2FD;});void*_tmpD9[2U];_tmpD9[0]=& _tmpDA,_tmpD9[1]=& _tmpDB;({unsigned _tmp3A0=loc;Cyc_Warn_err2(_tmp3A0,_tag_fat(_tmpD9,sizeof(void*),2U));});});
if(tvs != 0){
# 498
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=e,_tmpDD->tl=hidden_ts;_tmpDD;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 505
({struct Cyc_List_List*_tmp3A2=({struct Cyc_List_List*_tmp3A1=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp3A1,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmp3A2;});}
# 507
goto _LL0;}}_LL0:;}
# 509
return cvtenv;}
# 513
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_datatype_field(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 519
{union Cyc_Absyn_DatatypeFieldInfo _tmpDE=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmp10=_tmpDE;union Cyc_Absyn_DatatypeFieldInfo _tmpDF=_stmttmp10;struct Cyc_Absyn_Datatypefield*_tmpE1;struct Cyc_Absyn_Datatypedecl*_tmpE0;int _tmpE4;struct _tuple1*_tmpE3;struct _tuple1*_tmpE2;if((_tmpDF.UnknownDatatypefield).tag == 1){_LL1: _tmpE2=((_tmpDF.UnknownDatatypefield).val).datatype_name;_tmpE3=((_tmpDF.UnknownDatatypefield).val).field_name;_tmpE4=((_tmpDF.UnknownDatatypefield).val).is_extensible;_LL2: {struct _tuple1*tname=_tmpE2;struct _tuple1*fname=_tmpE3;int is_x=_tmpE4;
# 522
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,tname);
struct Cyc_Absyn_Datatypefield*tuf;
# 527
{struct Cyc_List_List*_tmpE5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmpE5;for(0;1;fs=fs->tl){
if(fs == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FF;_tmp2FF.tag=0U,({struct _fat_ptr _tmp3A3=({const char*_tmpE8="Tcutil found a bad datatypefield";_tag_fat(_tmpE8,sizeof(char),33U);});_tmp2FF.f1=_tmp3A3;});_tmp2FF;});void*_tmpE6[1U];_tmpE6[0]=& _tmpE7;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE6,sizeof(void*),1U));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)fs->hd)->name,fname)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 534
({union Cyc_Absyn_DatatypeFieldInfo _tmp3A4=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmp3A4;});
_tmpE0=tud;_tmpE1=tuf;goto _LL4;}}else{_LL3: _tmpE0=((_tmpDF.KnownDatatypefield).val).f1;_tmpE1=((_tmpDF.KnownDatatypefield).val).f2;_LL4: {struct Cyc_Absyn_Datatypedecl*tud=_tmpE0;struct Cyc_Absyn_Datatypefield*tuf=_tmpE1;
# 538
struct Cyc_List_List*tvs=tud->tvs;
for(0;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 544
{struct _tuple0 _tmpE9=({struct _tuple0 _tmp300;({void*_tmp3A5=Cyc_Absyn_compress_kb(tv->kind);_tmp300.f1=_tmp3A5;}),_tmp300.f2=t;_tmp300;});struct _tuple0 _stmttmp11=_tmpE9;struct _tuple0 _tmpEA=_stmttmp11;struct Cyc_Absyn_Tvar*_tmpEB;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpEA.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEA.f2)->tag == 2U){_LL6: _tmpEB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEA.f2)->f1;_LL7: {struct Cyc_Absyn_Tvar*tv2=_tmpEB;
# 546
({struct Cyc_List_List*_tmp3A6=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,tv2);cvtenv.kind_env=_tmp3A6;});
({struct Cyc_List_List*_tmp3A7=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,tv2,1);cvtenv.free_vars=_tmp3A7;});
continue;}}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}{
# 551
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);}}
# 555
if(targs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp304;_tmp304.tag=0U,({struct _fat_ptr _tmp3A8=({const char*_tmpF2="too many type arguments for datatype ";_tag_fat(_tmpF2,sizeof(char),38U);});_tmp304.f1=_tmp3A8;});_tmp304;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpEE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp303;_tmp303.tag=1U,_tmp303.f1=tud->name;_tmp303;});struct Cyc_Warn_String_Warn_Warg_struct _tmpEF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp302;_tmp302.tag=0U,({struct _fat_ptr _tmp3A9=({const char*_tmpF1=".";_tag_fat(_tmpF1,sizeof(char),2U);});_tmp302.f1=_tmp3A9;});_tmp302;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp301;_tmp301.tag=1U,_tmp301.f1=tuf->name;_tmp301;});void*_tmpEC[4U];_tmpEC[0]=& _tmpED,_tmpEC[1]=& _tmpEE,_tmpEC[2]=& _tmpEF,_tmpEC[3]=& _tmpF0;({unsigned _tmp3AA=loc;Cyc_Warn_err2(_tmp3AA,_tag_fat(_tmpEC,sizeof(void*),4U));});});
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp308;_tmp308.tag=0U,({struct _fat_ptr _tmp3AB=({const char*_tmpF9="too few type arguments for datatype ";_tag_fat(_tmpF9,sizeof(char),37U);});_tmp308.f1=_tmp3AB;});_tmp308;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp307;_tmp307.tag=1U,_tmp307.f1=tud->name;_tmp307;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp306;_tmp306.tag=0U,({struct _fat_ptr _tmp3AC=({const char*_tmpF8=".";_tag_fat(_tmpF8,sizeof(char),2U);});_tmp306.f1=_tmp3AC;});_tmp306;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp305;_tmp305.tag=1U,_tmp305.f1=tuf->name;_tmp305;});void*_tmpF3[4U];_tmpF3[0]=& _tmpF4,_tmpF3[1]=& _tmpF5,_tmpF3[2]=& _tmpF6,_tmpF3[3]=& _tmpF7;({unsigned _tmp3AD=loc;Cyc_Warn_err2(_tmp3AD,_tag_fat(_tmpF3,sizeof(void*),4U));});});
goto _LL0;}}_LL0:;}
# 561
return cvtenv;}
# 564
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_app(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 569
struct Cyc_List_List*_tmpFA=*targsp;struct Cyc_List_List*ts=_tmpFA;
{void*_tmpFB=c;union Cyc_Absyn_DatatypeFieldInfo*_tmpFC;union Cyc_Absyn_DatatypeInfo*_tmpFD;union Cyc_Absyn_AggrInfo*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Enumdecl**_tmp101;struct _tuple1*_tmp100;switch(*((int*)_tmpFB)){case 1U: _LL1: _LL2:
# 572
 goto _LL4;case 2U: _LL3: _LL4: goto _LL6;case 0U: _LL5: _LL6: goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA: goto _LLC;case 5U: _LLB: _LLC: goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10: goto _LL12;case 14U: _LL11: _LL12: goto _LL14;case 17U: _LL13: _LL14:
# 576
 if(ts != 0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp103=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp30A;_tmp30A.tag=2U,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=0U,_tmp106->f1=c,_tmp106->f2=ts;_tmp106;})))_tmp3AE=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=0U,_tmp106->f1=c,_tmp106->f2=ts;_tmp106;}));_tmp30A.f1=_tmp3AE;});_tmp30A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp104=({struct Cyc_Warn_String_Warn_Warg_struct _tmp309;_tmp309.tag=0U,({struct _fat_ptr _tmp3AF=({const char*_tmp105=" applied to argument(s)";_tag_fat(_tmp105,sizeof(char),24U);});_tmp309.f1=_tmp3AF;});_tmp309;});void*_tmp102[2U];_tmp102[0]=& _tmp103,_tmp102[1]=& _tmp104;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp102,sizeof(void*),2U));});
goto _LL0;case 9U: _LL15: _LL16:
# 580
 for(0;ts != 0;ts=ts->tl){
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)ts->hd,1,1);}
goto _LL0;case 4U: _LL17: _LL18:
# 584
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp108=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30B;_tmp30B.tag=0U,({struct _fat_ptr _tmp3B0=({const char*_tmp109="tag_t applied to wrong number of arguments";_tag_fat(_tmp109,sizeof(char),43U);});_tmp30B.f1=_tmp3B0;});_tmp30B;});void*_tmp107[1U];_tmp107[0]=& _tmp108;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp107,sizeof(void*),1U));});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,0,1);
goto _LL0;case 15U: _LL19: _tmp100=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpFB)->f1;_tmp101=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpFB)->f2;_LL1A: {struct _tuple1*n=_tmp100;struct Cyc_Absyn_Enumdecl**edp=_tmp101;
# 589
if(ts != 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30C;_tmp30C.tag=0U,({struct _fat_ptr _tmp3B1=({const char*_tmp10C="enum applied to argument(s)";_tag_fat(_tmp10C,sizeof(char),28U);});_tmp30C.f1=_tmp3B1;});_tmp30C;});void*_tmp10A[1U];_tmp10A[0]=& _tmp10B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp10A,sizeof(void*),1U));});
if(*edp == 0 ||((struct Cyc_Absyn_Enumdecl*)_check_null(*edp))->fields == 0){
struct _handler_cons _tmp10D;_push_handler(& _tmp10D);{int _tmp10F=0;if(setjmp(_tmp10D.handler))_tmp10F=1;if(!_tmp10F){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,n);
*edp=*ed;}
# 592
;_pop_handler();}else{void*_tmp10E=(void*)Cyc_Core_get_exn_thrown();void*_tmp110=_tmp10E;void*_tmp111;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp110)->tag == Cyc_Dict_Absent){_LL2C: _LL2D: {
# 596
struct Cyc_Absyn_Enumdecl*_tmp112=({struct Cyc_Absyn_Enumdecl*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->sc=Cyc_Absyn_Extern,_tmp113->name=n,_tmp113->fields=0;_tmp113;});struct Cyc_Absyn_Enumdecl*ed_orig=_tmp112;
Cyc_Tc_tcEnumdecl(te,loc,ed_orig);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,n);
*edp=*ed;
goto _LL2B;}}}else{_LL2E: _tmp111=_tmp110;_LL2F: {void*exn=_tmp111;(int)_rethrow(exn);}}_LL2B:;}}}
# 602
goto _LL0;}case 16U: _LL1B: _tmpFF=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL1C: {struct Cyc_List_List*fs=_tmpFF;
# 604
if(ts != 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp115=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30D;_tmp30D.tag=0U,({struct _fat_ptr _tmp3B2=({const char*_tmp116="enum applied to argument(s)";_tag_fat(_tmp116,sizeof(char),28U);});_tmp30D.f1=_tmp3B2;});_tmp30D;});void*_tmp114[1U];_tmp114[0]=& _tmp115;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp114,sizeof(void*),1U));});{
# 606
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp117=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp117;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp119=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30F;_tmp30F.tag=0U,({struct _fat_ptr _tmp3B3=({const char*_tmp11B="duplicate enum field name ";_tag_fat(_tmp11B,sizeof(char),27U);});_tmp30F.f1=_tmp3B3;});_tmp30F;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp30E;_tmp30E.tag=1U,_tmp30E.f1=f->name;_tmp30E;});void*_tmp118[2U];_tmp118[0]=& _tmp119,_tmp118[1]=& _tmp11A;({unsigned _tmp3B4=f->loc;Cyc_Warn_err2(_tmp3B4,_tag_fat(_tmp118,sizeof(void*),2U));});});else{
# 613
prev_fields=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->hd=(*f->name).f2,_tmp11C->tl=prev_fields;_tmp11C;});}
if(f->tag == 0)
({struct Cyc_Absyn_Exp*_tmp3B5=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp3B5;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp312;_tmp312.tag=0U,({struct _fat_ptr _tmp3B6=({const char*_tmp122="enum field ";_tag_fat(_tmp122,sizeof(char),12U);});_tmp312.f1=_tmp3B6;});_tmp312;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp11F=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp311;_tmp311.tag=1U,_tmp311.f1=f->name;_tmp311;});struct Cyc_Warn_String_Warn_Warg_struct _tmp120=({struct Cyc_Warn_String_Warn_Warg_struct _tmp310;_tmp310.tag=0U,({struct _fat_ptr _tmp3B7=({const char*_tmp121=": expression is not constant";_tag_fat(_tmp121,sizeof(char),29U);});_tmp310.f1=_tmp3B7;});_tmp310;});void*_tmp11D[3U];_tmp11D[0]=& _tmp11E,_tmp11D[1]=& _tmp11F,_tmp11D[2]=& _tmp120;({unsigned _tmp3B8=loc;Cyc_Warn_err2(_tmp3B8,_tag_fat(_tmp11D,sizeof(void*),3U));});});}{
unsigned t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))).f1;
tag_count=t1 + (unsigned)1;}}
# 621
goto _LL0;}}case 10U: _LL1D: _LL1E:
# 623
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp124=({struct Cyc_Warn_String_Warn_Warg_struct _tmp313;_tmp313.tag=0U,({struct _fat_ptr _tmp3B9=({const char*_tmp125="regions has wrong number of arguments";_tag_fat(_tmp125,sizeof(char),38U);});_tmp313.f1=_tmp3B9;});_tmp313;});void*_tmp123[1U];_tmp123[0]=& _tmp124;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp123,sizeof(void*),1U));});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,1,1);goto _LL0;case 3U: _LL1F: _LL20:
# 626
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp127=({struct Cyc_Warn_String_Warn_Warg_struct _tmp314;_tmp314.tag=0U,({struct _fat_ptr _tmp3BA=({const char*_tmp128="region_t has wrong number of arguments";_tag_fat(_tmp128,sizeof(char),39U);});_tmp314.f1=_tmp3BA;});_tmp314;});void*_tmp126[1U];_tmp126[0]=& _tmp127;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp126,sizeof(void*),1U));});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,1,1);
goto _LL0;case 13U: _LL21: _LL22:
# 630
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp12A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp315;_tmp315.tag=0U,({struct _fat_ptr _tmp3BB=({const char*_tmp12B="@thin has wrong number of arguments";_tag_fat(_tmp12B,sizeof(char),36U);});_tmp315.f1=_tmp3BB;});_tmp315;});void*_tmp129[1U];_tmp129[0]=& _tmp12A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp129,sizeof(void*),1U));});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,0,1);
goto _LL0;case 8U: _LL23: _LL24:
# 634
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp12D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp316;_tmp316.tag=0U,({struct _fat_ptr _tmp3BC=({const char*_tmp12E="access has wrong number of arguments";_tag_fat(_tmp12E,sizeof(char),37U);});_tmp316.f1=_tmp3BC;});_tmp316;});void*_tmp12C[1U];_tmp12C[0]=& _tmp12D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp12C,sizeof(void*),1U));});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,1,1);goto _LL0;case 20U: _LL25: _tmpFE=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL26: {union Cyc_Absyn_AggrInfo*info=_tmpFE;
# 637
cvtenv=Cyc_Tctyp_i_check_valid_aggr(loc,te,cvtenv,expected_kind,info,targsp,allow_abs_aggr);
# 639
goto _LL0;}case 18U: _LL27: _tmpFD=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL28: {union Cyc_Absyn_DatatypeInfo*info=_tmpFD;
# 641
cvtenv=Cyc_Tctyp_i_check_valid_datatype(loc,te,cvtenv,expected_kind,info,targsp,allow_abs_aggr);
# 643
goto _LL0;}default: _LL29: _tmpFC=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpFB)->f1;_LL2A: {union Cyc_Absyn_DatatypeFieldInfo*info=_tmpFC;
# 645
cvtenv=Cyc_Tctyp_i_check_valid_datatype_field(loc,te,cvtenv,expected_kind,info,ts,allow_abs_aggr);
# 647
goto _LL0;}}_LL0:;}
# 649
return cvtenv;}struct _tuple16{enum Cyc_Absyn_Format_Type f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 653
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 658
{void*_tmp12F=Cyc_Tcutil_compress(t);void*_stmttmp12=_tmp12F;void*_tmp130=_stmttmp12;void**_tmp134;struct Cyc_Absyn_Typedefdecl**_tmp133;struct Cyc_List_List**_tmp132;struct _tuple1*_tmp131;struct Cyc_List_List*_tmp136;enum Cyc_Absyn_AggrKind _tmp135;struct Cyc_List_List*_tmp137;struct Cyc_List_List**_tmp144;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_List_List**_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_VarargInfo*_tmp13E;int _tmp13D;struct Cyc_List_List*_tmp13C;void*_tmp13B;struct Cyc_Absyn_Tqual*_tmp13A;void**_tmp139;struct Cyc_List_List**_tmp138;unsigned _tmp149;void*_tmp148;struct Cyc_Absyn_Exp**_tmp147;struct Cyc_Absyn_Tqual*_tmp146;void*_tmp145;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*_tmp14B;void*_tmp151;void*_tmp150;void*_tmp14F;void*_tmp14E;struct Cyc_Absyn_Tqual*_tmp14D;void*_tmp14C;void***_tmp153;void*_tmp152;struct Cyc_Absyn_Tvar*_tmp154;void**_tmp156;struct Cyc_Core_Opt**_tmp155;struct Cyc_List_List**_tmp158;void*_tmp157;switch(*((int*)_tmp130)){case 0U: _LL1: _tmp157=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f1;_tmp158=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp130)->f2;_LL2: {void*c=_tmp157;struct Cyc_List_List**targsp=_tmp158;
# 660
cvtenv=Cyc_Tctyp_i_check_valid_type_app(loc,te,cvtenv,expected_kind,c,targsp,put_in_effect,allow_abs_aggr);
# 662
goto _LL0;}case 1U: _LL3: _tmp155=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp130)->f1;_tmp156=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp130)->f2;_LL4: {struct Cyc_Core_Opt**k=_tmp155;void**ref=_tmp156;
# 665
if(*k == 0 ||
 Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v)&& !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v,expected_kind))
({struct Cyc_Core_Opt*_tmp3BD=Cyc_Tcutil_kind_to_opt(expected_kind);*k=_tmp3BD;});
if(((cvtenv.fn_result && cvtenv.generalize_evars)&&(int)expected_kind->kind == (int)Cyc_Absyn_RgnKind)&& !te->tempest_generalize){
# 670
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*ref=Cyc_Absyn_unique_rgn_type;else{
# 673
*ref=Cyc_Absyn_heap_rgn_type;}}else{
if((cvtenv.generalize_evars &&(int)expected_kind->kind != (int)Cyc_Absyn_BoolKind)&&(int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 677
struct Cyc_Absyn_Tvar*_tmp159=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A->tag=2U,_tmp15A->f1=0,_tmp15A->f2=expected_kind;_tmp15A;}));struct Cyc_Absyn_Tvar*v=_tmp159;
({void*_tmp3BE=Cyc_Absyn_var_type(v);*ref=_tmp3BE;});
_tmp154=v;goto _LL6;}else{
# 681
({struct Cyc_List_List*_tmp3BF=Cyc_Tctyp_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmp3BF;});}}
# 683
goto _LL0;}case 2U: _LL5: _tmp154=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp130)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp154;
# 685
{void*_tmp15B=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp13=_tmp15B;void*_tmp15C=_stmttmp13;struct Cyc_Core_Opt**_tmp15D;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15C)->tag == 1U){_LL1A: _tmp15D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15C)->f1;_LL1B: {struct Cyc_Core_Opt**f=_tmp15D;
({struct Cyc_Core_Opt*_tmp3C1=({struct Cyc_Core_Opt*_tmp15F=_cycalloc(sizeof(*_tmp15F));({void*_tmp3C0=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->tag=2U,_tmp15E->f1=0,_tmp15E->f2=expected_kind;_tmp15E;});_tmp15F->v=_tmp3C0;});_tmp15F;});*f=_tmp3C1;});goto _LL19;}}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 691
({struct Cyc_List_List*_tmp3C2=Cyc_Tctyp_add_free_tvar(loc,cvtenv.kind_env,v);cvtenv.kind_env=_tmp3C2;});
# 694
({struct Cyc_List_List*_tmp3C3=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,v,put_in_effect);cvtenv.free_vars=_tmp3C3;});
# 696
{void*_tmp160=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp14=_tmp160;void*_tmp161=_stmttmp14;struct Cyc_Absyn_Kind*_tmp163;struct Cyc_Core_Opt**_tmp162;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp161)->tag == 2U){_LL1F: _tmp162=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp161)->f1;_tmp163=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp161)->f2;_LL20: {struct Cyc_Core_Opt**f=_tmp162;struct Cyc_Absyn_Kind*k=_tmp163;
# 698
if(Cyc_Tcutil_kind_leq(expected_kind,k))
({struct Cyc_Core_Opt*_tmp3C5=({struct Cyc_Core_Opt*_tmp165=_cycalloc(sizeof(*_tmp165));({void*_tmp3C4=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->tag=2U,_tmp164->f1=0,_tmp164->f2=expected_kind;_tmp164;});_tmp165->v=_tmp3C4;});_tmp165;});*f=_tmp3C5;});
goto _LL1E;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 703
goto _LL0;}case 10U: _LL7: _tmp152=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp130)->f1)->r;_tmp153=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp130)->f2;_LL8: {void*td=_tmp152;void***topt=_tmp153;
# 709
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp166=td;struct Cyc_Absyn_Datatypedecl*_tmp167;struct Cyc_Absyn_Enumdecl*_tmp168;struct Cyc_Absyn_Aggrdecl*_tmp169;switch(*((int*)_tmp166)){case 0U: _LL24: _tmp169=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp166)->f1;_LL25: {struct Cyc_Absyn_Aggrdecl*ad=_tmp169;
# 712
if(te->in_extern_c_include)
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL23;}case 1U: _LL26: _tmp168=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp166)->f1;_LL27: {struct Cyc_Absyn_Enumdecl*ed=_tmp168;
# 716
if(te->in_extern_c_include)
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL23;}default: _LL28: _tmp167=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp166)->f1;_LL29: {struct Cyc_Absyn_Datatypedecl*dd=_tmp167;
# 720
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL23;}}_LL23:;}
# 722
({void**_tmp3C6=({void**_tmp16A=_cycalloc(sizeof(*_tmp16A));*_tmp16A=new_typ;_tmp16A;});*topt=_tmp3C6;});
return Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3U: _LL9: _tmp14C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).elt_type;_tmp14D=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).elt_tq;_tmp14E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).ptr_atts).rgn;_tmp14F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).ptr_atts).nullable;_tmp150=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).ptr_atts).bounds;_tmp151=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).ptr_atts).zero_term;_LLA: {void*t1=_tmp14C;struct Cyc_Absyn_Tqual*tqp=_tmp14D;void*rgn_type=_tmp14E;void*nullable=_tmp14F;void*b=_tmp150;void*zt=_tmp151;
# 728
int is_zero_terminated;
# 730
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,t1,1,1);
({int _tmp3C7=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_tmp3C7;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp16B=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp15=_tmp16B;enum Cyc_Absyn_AliasQual _tmp16C=_stmttmp15;switch(_tmp16C){case Cyc_Absyn_Aliasable: _LL2B: _LL2C:
 k=& Cyc_Tcutil_rk;goto _LL2A;case Cyc_Absyn_Unique: _LL2D: _LL2E:
 k=& Cyc_Tcutil_urk;goto _LL2A;case Cyc_Absyn_Top: _LL2F: _LL30:
 goto _LL32;default: _LL31: _LL32:
 k=& Cyc_Tcutil_trk;goto _LL2A;}_LL2A:;}
# 739
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,rgn_type,1,1);
# 742
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,zt,0,1);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,nullable,0,1);
({void*_tmp3C8=zt;Cyc_Unify_unify(_tmp3C8,Cyc_Tcutil_is_char_type(t1)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
is_zero_terminated=Cyc_Absyn_type2bool(0,zt);
if(is_zero_terminated){
# 748
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp319;_tmp319.tag=0U,({struct _fat_ptr _tmp3C9=({const char*_tmp172="cannot have a pointer to zero-terminated ";_tag_fat(_tmp172,sizeof(char),42U);});_tmp319.f1=_tmp3C9;});_tmp319;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp318;_tmp318.tag=2U,_tmp318.f1=(void*)t1;_tmp318;});struct Cyc_Warn_String_Warn_Warg_struct _tmp170=({struct Cyc_Warn_String_Warn_Warg_struct _tmp317;_tmp317.tag=0U,({struct _fat_ptr _tmp3CA=({const char*_tmp171=" elements";_tag_fat(_tmp171,sizeof(char),10U);});_tmp317.f1=_tmp3CA;});_tmp317;});void*_tmp16D[3U];_tmp16D[0]=& _tmp16E,_tmp16D[1]=& _tmp16F,_tmp16D[2]=& _tmp170;({unsigned _tmp3CB=loc;Cyc_Warn_err2(_tmp3CB,_tag_fat(_tmp16D,sizeof(void*),3U));});});}
# 752
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ptrbk,b,0,allow_abs_aggr);{
struct Cyc_Absyn_Exp*_tmp173=({void*_tmp3CC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp3CC,b);});struct Cyc_Absyn_Exp*e=_tmp173;
if(e != 0){
struct _tuple12 _tmp174=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp16=_tmp174;struct _tuple12 _tmp175=_stmttmp16;int _tmp177;unsigned _tmp176;_LL34: _tmp176=_tmp175.f1;_tmp177=_tmp175.f2;_LL35: {unsigned sz=_tmp176;int known=_tmp177;
if(is_zero_terminated &&(!known || sz < (unsigned)1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp179=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31A;_tmp31A.tag=0U,({struct _fat_ptr _tmp3CD=({const char*_tmp17A="zero-terminated pointer cannot point to empty sequence";_tag_fat(_tmp17A,sizeof(char),55U);});_tmp31A.f1=_tmp3CD;});_tmp31A;});void*_tmp178[1U];_tmp178[0]=& _tmp179;({unsigned _tmp3CE=loc;Cyc_Warn_err2(_tmp3CE,_tag_fat(_tmp178,sizeof(void*),1U));});});}}
# 759
goto _LL0;}}}case 9U: _LLB: _tmp14B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp130)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp14B;
# 764
({struct Cyc_Tcenv_Tenv*_tmp3CF=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3CF,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31B;_tmp31B.tag=0U,({struct _fat_ptr _tmp3D0=({const char*_tmp17D="valueof_t requires an int expression";_tag_fat(_tmp17D,sizeof(char),37U);});_tmp31B.f1=_tmp3D0;});_tmp31B;});void*_tmp17B[1U];_tmp17B[0]=& _tmp17C;({unsigned _tmp3D1=loc;Cyc_Warn_err2(_tmp3D1,_tag_fat(_tmp17B,sizeof(void*),1U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e,te,cvtenv);
goto _LL0;}case 11U: _LLD: _tmp14A=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp130)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_tmp14A;
# 773
({struct Cyc_Tcenv_Tenv*_tmp3D2=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3D2,0,e);});
goto _LL0;}case 4U: _LLF: _tmp145=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp130)->f1).elt_type;_tmp146=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp130)->f1).tq;_tmp147=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp130)->f1).num_elts;_tmp148=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp130)->f1).zero_term;_tmp149=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp130)->f1).zt_loc;_LL10: {void*t1=_tmp145;struct Cyc_Absyn_Tqual*tqp=_tmp146;struct Cyc_Absyn_Exp**eptr=_tmp147;void*zt=_tmp148;unsigned ztl=_tmp149;
# 778
struct Cyc_Absyn_Exp*_tmp17E=*eptr;struct Cyc_Absyn_Exp*e=_tmp17E;
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,t1,1,allow_abs_aggr);
({int _tmp3D3=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_tmp3D3;});{
# 782
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);
if(is_zero_terminated){
# 785
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31E;_tmp31E.tag=0U,({struct _fat_ptr _tmp3D4=({const char*_tmp184="cannot have a zero-terminated array of ";_tag_fat(_tmp184,sizeof(char),40U);});_tmp31E.f1=_tmp3D4;});_tmp31E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp181=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp31D;_tmp31D.tag=2U,_tmp31D.f1=(void*)t1;_tmp31D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31C;_tmp31C.tag=0U,({struct _fat_ptr _tmp3D5=({const char*_tmp183=" elements";_tag_fat(_tmp183,sizeof(char),10U);});_tmp31C.f1=_tmp3D5;});_tmp31C;});void*_tmp17F[3U];_tmp17F[0]=& _tmp180,_tmp17F[1]=& _tmp181,_tmp17F[2]=& _tmp182;({unsigned _tmp3D6=loc;Cyc_Warn_err2(_tmp3D6,_tag_fat(_tmp17F,sizeof(void*),3U));});});}
# 790
if(e == 0){
# 792
if(is_zero_terminated)
# 794
({struct Cyc_Absyn_Exp*_tmp3D7=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp3D7;});else{
# 797
({struct Cyc_Warn_String_Warn_Warg_struct _tmp186=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31F;_tmp31F.tag=0U,({struct _fat_ptr _tmp3D8=({const char*_tmp187="array bound defaults to 1 here";_tag_fat(_tmp187,sizeof(char),31U);});_tmp31F.f1=_tmp3D8;});_tmp31F;});void*_tmp185[1U];_tmp185[0]=& _tmp186;({unsigned _tmp3D9=loc;Cyc_Warn_warn2(_tmp3D9,_tag_fat(_tmp185,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp3DA=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp3DA;});}}
# 801
({struct Cyc_Tcenv_Tenv*_tmp3DB=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3DB,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp189=({struct Cyc_Warn_String_Warn_Warg_struct _tmp320;_tmp320.tag=0U,({struct _fat_ptr _tmp3DC=({const char*_tmp18A="array bounds expression is not an unsigned int";_tag_fat(_tmp18A,sizeof(char),47U);});_tmp320.f1=_tmp3DC;});_tmp320;});void*_tmp188[1U];_tmp188[0]=& _tmp189;({unsigned _tmp3DD=loc;Cyc_Warn_err2(_tmp3DD,_tag_fat(_tmp188,sizeof(void*),1U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e,te,cvtenv);{
struct _tuple12 _tmp18B=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp17=_tmp18B;struct _tuple12 _tmp18C=_stmttmp17;int _tmp18E;unsigned _tmp18D;_LL37: _tmp18D=_tmp18C.f1;_tmp18E=_tmp18C.f2;_LL38: {unsigned sz=_tmp18D;int known=_tmp18E;
# 807
if((is_zero_terminated && known)&& sz < (unsigned)1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp190=({struct Cyc_Warn_String_Warn_Warg_struct _tmp321;_tmp321.tag=0U,({struct _fat_ptr _tmp3DE=({const char*_tmp191="zero terminated array cannot have zero elements";_tag_fat(_tmp191,sizeof(char),48U);});_tmp321.f1=_tmp3DE;});_tmp321;});void*_tmp18F[1U];_tmp18F[0]=& _tmp190;({unsigned _tmp3DF=loc;Cyc_Warn_warn2(_tmp3DF,_tag_fat(_tmp18F,sizeof(void*),1U));});});
# 810
if((known && sz < (unsigned)1)&& Cyc_Cyclone_tovc_r){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp193=({struct Cyc_Warn_String_Warn_Warg_struct _tmp322;_tmp322.tag=0U,({struct _fat_ptr _tmp3E0=({const char*_tmp194="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_fat(_tmp194,sizeof(char),75U);});_tmp322.f1=_tmp3E0;});_tmp322;});void*_tmp192[1U];_tmp192[0]=& _tmp193;({unsigned _tmp3E1=loc;Cyc_Warn_warn2(_tmp3E1,_tag_fat(_tmp192,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp3E2=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp3E2;});}
# 814
goto _LL0;}}}}case 5U: _LL11: _tmp138=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).tvars;_tmp139=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).effect;_tmp13A=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).ret_tqual;_tmp13B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).ret_type;_tmp13C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).args;_tmp13D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).c_varargs;_tmp13E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).cyc_varargs;_tmp13F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).rgn_po;_tmp140=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).attributes;_tmp141=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).requires_clause;_tmp142=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).requires_relns;_tmp143=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).ensures_clause;_tmp144=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp130)->f1).ensures_relns;_LL12: {struct Cyc_List_List**btvs=_tmp138;void**eff=_tmp139;struct Cyc_Absyn_Tqual*rtq=_tmp13A;void*tr=_tmp13B;struct Cyc_List_List*args=_tmp13C;int c_vararg=_tmp13D;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp13E;struct Cyc_List_List*rgn_po=_tmp13F;struct Cyc_List_List*atts=_tmp140;struct Cyc_Absyn_Exp*req=_tmp141;struct Cyc_List_List**req_relns=_tmp142;struct Cyc_Absyn_Exp*ens=_tmp143;struct Cyc_List_List**ens_relns=_tmp144;
# 821
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;atts != 0;atts=atts->tl){
if(!Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp196=({struct Cyc_Warn_String_Warn_Warg_struct _tmp324;_tmp324.tag=0U,({struct _fat_ptr _tmp3E3=({const char*_tmp198="bad function type attribute ";_tag_fat(_tmp198,sizeof(char),29U);});_tmp324.f1=_tmp3E3;});_tmp324;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp197=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp323;_tmp323.tag=9U,_tmp323.f1=(void*)atts->hd;_tmp323;});void*_tmp195[2U];_tmp195[0]=& _tmp196,_tmp195[1]=& _tmp197;({unsigned _tmp3E4=loc;Cyc_Warn_err2(_tmp3E4,_tag_fat(_tmp195,sizeof(void*),2U));});});{
void*_tmp199=(void*)atts->hd;void*_stmttmp18=_tmp199;void*_tmp19A=_stmttmp18;int _tmp19D;int _tmp19C;enum Cyc_Absyn_Format_Type _tmp19B;switch(*((int*)_tmp19A)){case 1U: _LL3A: _LL3B:
# 834
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL39;case 2U: _LL3C: _LL3D:
# 836
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL39;case 3U: _LL3E: _LL3F:
# 838
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL39;case 19U: _LL40: _tmp19B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp19A)->f1;_tmp19C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp19A)->f2;_tmp19D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp19A)->f3;_LL41: {enum Cyc_Absyn_Format_Type fmttype=_tmp19B;int i=_tmp19C;int j=_tmp19D;
# 840
if(!seen_format){
seen_format=1;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;}else{
# 846
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp325;_tmp325.tag=0U,({struct _fat_ptr _tmp3E5=({const char*_tmp1A0="function can't have multiple format attributes";_tag_fat(_tmp1A0,sizeof(char),47U);});_tmp325.f1=_tmp3E5;});_tmp325;});void*_tmp19E[1U];_tmp19E[0]=& _tmp19F;({unsigned _tmp3E6=loc;Cyc_Warn_err2(_tmp3E6,_tag_fat(_tmp19E,sizeof(void*),1U));});});}
goto _LL39;}default: _LL42: _LL43:
 goto _LL39;}_LL39:;}}
# 851
if(num_convs > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp326;_tmp326.tag=0U,({struct _fat_ptr _tmp3E7=({const char*_tmp1A3="function can't have multiple calling conventions";_tag_fat(_tmp1A3,sizeof(char),49U);});_tmp326.f1=_tmp3E7;});_tmp326;});void*_tmp1A1[1U];_tmp1A1[0]=& _tmp1A2;({unsigned _tmp3E8=loc;Cyc_Warn_err2(_tmp3E8,_tag_fat(_tmp1A1,sizeof(void*),1U));});});
# 856
Cyc_Tcutil_check_unique_tvars(loc,*btvs);
{struct Cyc_List_List*b=*btvs;for(0;b != 0;b=b->tl){
({int _tmp3E9=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmp3E9;});
({struct Cyc_List_List*_tmp3EA=Cyc_Tctyp_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmp3EA;});{
void*_tmp1A4=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_stmttmp19=_tmp1A4;void*_tmp1A5=_stmttmp19;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1A5)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1A5)->f1)->kind == Cyc_Absyn_MemKind){_LL45: _LL46:
# 862
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp328;_tmp328.tag=0U,({struct _fat_ptr _tmp3EB=({const char*_tmp1A9="function abstracts Mem type variable ";_tag_fat(_tmp1A9,sizeof(char),38U);});_tmp328.f1=_tmp3EB;});_tmp328;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp327;_tmp327.tag=0U,_tmp327.f1=*((struct Cyc_Absyn_Tvar*)b->hd)->name;_tmp327;});void*_tmp1A6[2U];_tmp1A6[0]=& _tmp1A7,_tmp1A6[1]=& _tmp1A8;({unsigned _tmp3EC=loc;Cyc_Warn_err2(_tmp3EC,_tag_fat(_tmp1A6,sizeof(void*),2U));});});goto _LL44;}else{goto _LL47;}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}}}{
# 868
struct Cyc_Tctyp_CVTEnv _tmp1AA=({struct Cyc_Tctyp_CVTEnv _tmp339;_tmp339.r=Cyc_Core_heap_region,_tmp339.kind_env=cvtenv.kind_env,_tmp339.fn_result=1,_tmp339.generalize_evars=cvtenv.generalize_evars,_tmp339.free_vars=0,_tmp339.free_evars=0;_tmp339;});struct Cyc_Tctyp_CVTEnv new_cvtenv=_tmp1AA;
# 870
new_cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,new_cvtenv,& Cyc_Tcutil_tmk,tr,1,1);
({int _tmp3ED=Cyc_Tcutil_extract_const_from_typedef(loc,rtq->print_const,tr);rtq->real_const=_tmp3ED;});
new_cvtenv.fn_result=0;
# 878
{struct Cyc_List_List*a=args;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp1AB=(struct _tuple9*)a->hd;struct _tuple9*trip=_tmp1AB;
void*_tmp1AC=(*trip).f3;void*t=_tmp1AC;
new_cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,new_cvtenv,& Cyc_Tcutil_tmk,t,1,1);{
int _tmp1AD=Cyc_Tcutil_extract_const_from_typedef(loc,((*trip).f2).print_const,t);int ec=_tmp1AD;
((*trip).f2).real_const=ec;
# 886
if(Cyc_Tcutil_is_array_type(t)){
# 888
void*_tmp1AE=Cyc_Absyn_new_evar(0,0);void*ptr_rgn=_tmp1AE;
new_cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,new_cvtenv,& Cyc_Tcutil_rk,ptr_rgn,1,1);
({void*_tmp3EE=Cyc_Tcutil_promote_array(t,ptr_rgn,0);(*trip).f3=_tmp3EE;});}}}}
# 894
if(cyc_vararg != 0){
if(c_vararg)({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp329;_tmp329.tag=0U,({struct _fat_ptr _tmp3EF=({const char*_tmp1B1="both c_vararg and cyc_vararg";_tag_fat(_tmp1B1,sizeof(char),29U);});_tmp329.f1=_tmp3EF;});_tmp329;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1AF,sizeof(void*),1U));});{
struct Cyc_Absyn_VarargInfo _tmp1B2=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1A=_tmp1B2;struct Cyc_Absyn_VarargInfo _tmp1B3=_stmttmp1A;int _tmp1B5;void*_tmp1B4;_LL4A: _tmp1B4=_tmp1B3.type;_tmp1B5=_tmp1B3.inject;_LL4B: {void*vt=_tmp1B4;int vi=_tmp1B5;
new_cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,new_cvtenv,& Cyc_Tcutil_tmk,vt,1,1);
({int _tmp3F0=Cyc_Tcutil_extract_const_from_typedef(loc,(cyc_vararg->tq).print_const,vt);(cyc_vararg->tq).real_const=_tmp3F0;});
# 900
if(vi){
void*_tmp1B6=Cyc_Tcutil_compress(vt);void*_stmttmp1B=_tmp1B6;void*_tmp1B7=_stmttmp1B;void*_tmp1BA;void*_tmp1B9;void*_tmp1B8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7)->tag == 3U){_LL4D: _tmp1B8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7)->f1).elt_type;_tmp1B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7)->f1).ptr_atts).bounds;_tmp1BA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7)->f1).ptr_atts).zero_term;_LL4E: {void*et=_tmp1B8;void*bs=_tmp1B9;void*zt=_tmp1BA;
# 903
{void*_tmp1BB=Cyc_Tcutil_compress(et);void*_stmttmp1C=_tmp1BB;void*_tmp1BC=_stmttmp1C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f1)->tag == 18U){_LL52: _LL53:
# 905
 if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32A;_tmp32A.tag=0U,({struct _fat_ptr _tmp3F1=({const char*_tmp1BF="can't inject into a zeroterm pointer";_tag_fat(_tmp1BF,sizeof(char),37U);});_tmp32A.f1=_tmp3F1;});_tmp32A;});void*_tmp1BD[1U];_tmp1BD[0]=& _tmp1BE;({unsigned _tmp3F2=loc;Cyc_Warn_err2(_tmp3F2,_tag_fat(_tmp1BD,sizeof(void*),1U));});});
if(!({void*_tmp3F3=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp3F3,bs);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32B;_tmp32B.tag=0U,({struct _fat_ptr _tmp3F4=({const char*_tmp1C2="can't inject into a fat pointer to datatype";_tag_fat(_tmp1C2,sizeof(char),44U);});_tmp32B.f1=_tmp3F4;});_tmp32B;});void*_tmp1C0[1U];_tmp1C0[0]=& _tmp1C1;({unsigned _tmp3F5=loc;Cyc_Warn_err2(_tmp3F5,_tag_fat(_tmp1C0,sizeof(void*),1U));});});
goto _LL51;}else{goto _LL54;}}else{_LL54: _LL55:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32C;_tmp32C.tag=0U,({struct _fat_ptr _tmp3F6=({const char*_tmp1C5="can't inject a non-datatype type";_tag_fat(_tmp1C5,sizeof(char),33U);});_tmp32C.f1=_tmp3F6;});_tmp32C;});void*_tmp1C3[1U];_tmp1C3[0]=& _tmp1C4;({unsigned _tmp3F7=loc;Cyc_Warn_err2(_tmp3F7,_tag_fat(_tmp1C3,sizeof(void*),1U));});});goto _LL51;}_LL51:;}
# 912
goto _LL4C;}}else{_LL4F: _LL50:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32D;_tmp32D.tag=0U,({struct _fat_ptr _tmp3F8=({const char*_tmp1C8="expecting a datatype pointer type";_tag_fat(_tmp1C8,sizeof(char),34U);});_tmp32D.f1=_tmp3F8;});_tmp32D;});void*_tmp1C6[1U];_tmp1C6[0]=& _tmp1C7;({unsigned _tmp3F9=loc;Cyc_Warn_err2(_tmp3F9,_tag_fat(_tmp1C6,sizeof(void*),1U));});});goto _LL4C;}_LL4C:;}}}}
# 917
if(seen_format){
int _tmp1C9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);int num_args=_tmp1C9;
if((((fmt_desc_arg < 0 || fmt_desc_arg > num_args)|| fmt_arg_start < 0)||
# 921
(cyc_vararg == 0 && !c_vararg)&& fmt_arg_start != 0)||
(cyc_vararg != 0 || c_vararg)&& fmt_arg_start != num_args + 1)
# 924
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32E;_tmp32E.tag=0U,({struct _fat_ptr _tmp3FA=({const char*_tmp1CC="bad format descriptor";_tag_fat(_tmp1CC,sizeof(char),22U);});_tmp32E.f1=_tmp3FA;});_tmp32E;});void*_tmp1CA[1U];_tmp1CA[0]=& _tmp1CB;({unsigned _tmp3FB=loc;Cyc_Warn_err2(_tmp3FB,_tag_fat(_tmp1CA,sizeof(void*),1U));});});else{
# 927
struct _tuple9 _tmp1CD=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_desc_arg - 1);struct _tuple9 _stmttmp1D=_tmp1CD;struct _tuple9 _tmp1CE=_stmttmp1D;void*_tmp1CF;_LL57: _tmp1CF=_tmp1CE.f3;_LL58: {void*t=_tmp1CF;
# 929
{void*_tmp1D0=Cyc_Tcutil_compress(t);void*_stmttmp1E=_tmp1D0;void*_tmp1D1=_stmttmp1E;void*_tmp1D4;void*_tmp1D3;void*_tmp1D2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D1)->tag == 3U){_LL5A: _tmp1D2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D1)->f1).elt_type;_tmp1D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D1)->f1).ptr_atts).bounds;_tmp1D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D1)->f1).ptr_atts).zero_term;_LL5B: {void*et=_tmp1D2;void*b=_tmp1D3;void*zt=_tmp1D4;
# 932
if(!Cyc_Tcutil_is_char_type(et))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32F;_tmp32F.tag=0U,({struct _fat_ptr _tmp3FC=({const char*_tmp1D7="format descriptor is not a string";_tag_fat(_tmp1D7,sizeof(char),34U);});_tmp32F.f1=_tmp3FC;});_tmp32F;});void*_tmp1D5[1U];_tmp1D5[0]=& _tmp1D6;({unsigned _tmp3FD=loc;Cyc_Warn_err2(_tmp3FD,_tag_fat(_tmp1D5,sizeof(void*),1U));});});else{
# 935
struct Cyc_Absyn_Exp*_tmp1D8=({void*_tmp3FE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp3FE,b);});struct Cyc_Absyn_Exp*e=_tmp1D8;
if(e == 0 && c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp330;_tmp330.tag=0U,({struct _fat_ptr _tmp3FF=({const char*_tmp1DB="format descriptor is not a char * type";_tag_fat(_tmp1DB,sizeof(char),39U);});_tmp330.f1=_tmp3FF;});_tmp330;});void*_tmp1D9[1U];_tmp1D9[0]=& _tmp1DA;({unsigned _tmp400=loc;Cyc_Warn_err2(_tmp400,_tag_fat(_tmp1D9,sizeof(void*),1U));});});else{
if(e != 0 && !c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp331;_tmp331.tag=0U,({struct _fat_ptr _tmp401=({const char*_tmp1DE="format descriptor is not a char ? type";_tag_fat(_tmp1DE,sizeof(char),39U);});_tmp331.f1=_tmp401;});_tmp331;});void*_tmp1DC[1U];_tmp1DC[0]=& _tmp1DD;({unsigned _tmp402=loc;Cyc_Warn_err2(_tmp402,_tag_fat(_tmp1DC,sizeof(void*),1U));});});}}
# 941
goto _LL59;}}else{_LL5C: _LL5D:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp332;_tmp332.tag=0U,({struct _fat_ptr _tmp403=({const char*_tmp1E1="format descriptor is not a string type";_tag_fat(_tmp1E1,sizeof(char),39U);});_tmp332.f1=_tmp403;});_tmp332;});void*_tmp1DF[1U];_tmp1DF[0]=& _tmp1E0;({unsigned _tmp404=loc;Cyc_Warn_err2(_tmp404,_tag_fat(_tmp1DF,sizeof(void*),1U));});});goto _LL59;}_LL59:;}
# 944
if(fmt_arg_start != 0 && !c_vararg){
# 948
int problem;
{struct _tuple16 _tmp1E2=({struct _tuple16 _tmp333;_tmp333.f1=ft,({void*_tmp405=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_vararg))->type));_tmp333.f2=_tmp405;});_tmp333;});struct _tuple16 _stmttmp1F=_tmp1E2;struct _tuple16 _tmp1E3=_stmttmp1F;struct Cyc_Absyn_Datatypedecl*_tmp1E4;struct Cyc_Absyn_Datatypedecl*_tmp1E5;switch(_tmp1E3.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5F: _tmp1E5=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->f1)->f1).KnownDatatype).val;_LL60: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1E5;
# 951
problem=({struct _tuple1*_tmp406=tud->name;Cyc_Absyn_qvar_cmp(_tmp406,Cyc_Absyn_datatype_print_arg_qvar());})!= 0;
goto _LL5E;}}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->f1)->f1).KnownDatatype).tag == 2){_LL61: _tmp1E4=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3.f2)->f1)->f1).KnownDatatype).val;_LL62: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1E4;
# 954
problem=({struct _tuple1*_tmp407=tud->name;Cyc_Absyn_qvar_cmp(_tmp407,Cyc_Absyn_datatype_scanf_arg_qvar());})!= 0;
goto _LL5E;}}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}default: _LL63: _LL64:
# 957
 problem=1;
goto _LL5E;}_LL5E:;}
# 960
if(problem)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp334;_tmp334.tag=0U,({struct _fat_ptr _tmp408=({const char*_tmp1E8="format attribute and vararg types don't match";_tag_fat(_tmp1E8,sizeof(char),46U);});_tmp334.f1=_tmp408;});_tmp334;});void*_tmp1E6[1U];_tmp1E6[0]=& _tmp1E7;({unsigned _tmp409=loc;Cyc_Warn_err2(_tmp409,_tag_fat(_tmp1E6,sizeof(void*),1U));});});}}}}
# 968
{struct Cyc_List_List*rpo=rgn_po;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp1E9=(struct _tuple0*)rpo->hd;struct _tuple0*_stmttmp20=_tmp1E9;struct _tuple0*_tmp1EA=_stmttmp20;void*_tmp1EC;void*_tmp1EB;_LL66: _tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;_LL67: {void*r1=_tmp1EB;void*r2=_tmp1EC;
new_cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,new_cvtenv,& Cyc_Tcutil_ek,r1,1,1);
new_cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,new_cvtenv,& Cyc_Tcutil_trk,r2,1,1);}}}{
# 976
struct Cyc_Tcenv_Fenv*_tmp1ED=Cyc_Tcenv_bogus_fenv(tr,args);struct Cyc_Tcenv_Fenv*fenv=_tmp1ED;
struct Cyc_Tcenv_Tenv*_tmp1EE=({struct Cyc_Tcenv_Tenv*_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->ns=te->ns,_tmp230->ae=te->ae,_tmp230->le=fenv,_tmp230->allow_valueof=1,_tmp230->in_extern_c_include=te->in_extern_c_include,_tmp230->in_tempest=te->in_tempest,_tmp230->tempest_generalize=te->tempest_generalize,_tmp230->in_extern_c_inc_repeat=te->in_extern_c_inc_repeat;_tmp230;});struct Cyc_Tcenv_Tenv*ftenv=_tmp1EE;
# 979
struct _tuple15 _tmp1EF=({unsigned _tmp40D=loc;struct Cyc_Tcenv_Tenv*_tmp40C=ftenv;struct Cyc_Tctyp_CVTEnv _tmp40B=new_cvtenv;struct _fat_ptr _tmp40A=({const char*_tmp22F="@requires";_tag_fat(_tmp22F,sizeof(char),10U);});Cyc_Tctyp_check_clause(_tmp40D,_tmp40C,_tmp40B,_tmp40A,req);});struct _tuple15 _stmttmp21=_tmp1EF;struct _tuple15 _tmp1F0=_stmttmp21;struct Cyc_List_List*_tmp1F2;struct Cyc_Tctyp_CVTEnv _tmp1F1;_LL69: _tmp1F1=_tmp1F0.f1;_tmp1F2=_tmp1F0.f2;_LL6A: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1F1;struct Cyc_List_List*req_rs=_tmp1F2;
new_cvtenv=nenv;
*req_relns=req_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,req_rs);{
struct _tuple15 _tmp1F3=({unsigned _tmp411=loc;struct Cyc_Tcenv_Tenv*_tmp410=ftenv;struct Cyc_Tctyp_CVTEnv _tmp40F=new_cvtenv;struct _fat_ptr _tmp40E=({const char*_tmp22E="@ensures";_tag_fat(_tmp22E,sizeof(char),9U);});Cyc_Tctyp_check_clause(_tmp411,_tmp410,_tmp40F,_tmp40E,ens);});struct _tuple15 _stmttmp22=_tmp1F3;struct _tuple15 _tmp1F4=_stmttmp22;struct Cyc_List_List*_tmp1F6;struct Cyc_Tctyp_CVTEnv _tmp1F5;_LL6C: _tmp1F5=_tmp1F4.f1;_tmp1F6=_tmp1F4.f2;_LL6D: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1F5;struct Cyc_List_List*ens_rs=_tmp1F6;
new_cvtenv=nenv;
*ens_relns=ens_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,ens_rs);
# 988
if(*eff != 0)
new_cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,new_cvtenv,& Cyc_Tcutil_ek,(void*)_check_null(*eff),1,1);else{
# 991
struct Cyc_List_List*effect=0;
# 996
{struct Cyc_List_List*tvs=new_cvtenv.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp1F7=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp23=_tmp1F7;struct _tuple14 _tmp1F8=_stmttmp23;int _tmp1FA;struct Cyc_Absyn_Tvar*_tmp1F9;_LL6F: _tmp1F9=_tmp1F8.f1;_tmp1FA=_tmp1F8.f2;_LL70: {struct Cyc_Absyn_Tvar*tv=_tmp1F9;int put_in_eff=_tmp1FA;
if(!put_in_eff)continue;{
void*_tmp1FB=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp24=_tmp1FB;void*_tmp1FC=_stmttmp24;struct Cyc_Core_Opt**_tmp1FD;struct Cyc_Absyn_Kind*_tmp1FE;struct Cyc_Core_Opt**_tmp1FF;struct Cyc_Absyn_Kind*_tmp201;struct Cyc_Core_Opt**_tmp200;switch(*((int*)_tmp1FC)){case 2U: _LL72: _tmp200=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1FC)->f1;_tmp201=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1FC)->f2;if((int)_tmp201->kind == (int)Cyc_Absyn_RgnKind){_LL73: {struct Cyc_Core_Opt**f=_tmp200;struct Cyc_Absyn_Kind*r=_tmp201;
# 1002
if((int)r->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmp412=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp412;});_tmp1FE=r;goto _LL75;}
# 1005
({struct Cyc_Core_Opt*_tmp413=Cyc_Tcutil_kind_to_bound_opt(r);*f=_tmp413;});_tmp1FE=r;goto _LL75;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1FC)->f2)->kind){case Cyc_Absyn_BoolKind: _LL76: _LL77:
# 1008
 goto _LL79;case Cyc_Absyn_PtrBndKind: _LL78: _LL79:
 goto _LL7B;case Cyc_Absyn_IntKind: _LL7A: _LL7B:
 goto _LL7D;case Cyc_Absyn_EffKind: _LL82: _tmp1FF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1FC)->f1;_LL83: {struct Cyc_Core_Opt**f=_tmp1FF;
# 1015
({struct Cyc_Core_Opt*_tmp414=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*f=_tmp414;});goto _LL85;}default: goto _LL88;}}case 0U: _LL74: _tmp1FE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1FC)->f1;if((int)_tmp1FE->kind == (int)Cyc_Absyn_RgnKind){_LL75: {struct Cyc_Absyn_Kind*r=_tmp1FE;
# 1007
effect=({struct Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));({void*_tmp415=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(tv));_tmp202->hd=_tmp415;}),_tmp202->tl=effect;_tmp202;});goto _LL71;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1FC)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7C: _LL7D:
# 1011
 goto _LL7F;case Cyc_Absyn_PtrBndKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_IntKind: _LL80: _LL81:
 goto _LL71;case Cyc_Absyn_EffKind: _LL84: _LL85:
# 1017
 effect=({struct Cyc_List_List*_tmp203=_cycalloc(sizeof(*_tmp203));({void*_tmp416=Cyc_Absyn_var_type(tv);_tmp203->hd=_tmp416;}),_tmp203->tl=effect;_tmp203;});goto _LL71;default: _LL88: _LL89:
# 1022
 effect=({struct Cyc_List_List*_tmp206=_cycalloc(sizeof(*_tmp206));({void*_tmp417=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(tv));_tmp206->hd=_tmp417;}),_tmp206->tl=effect;_tmp206;});goto _LL71;}}default: _LL86: _tmp1FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1FC)->f1;_LL87: {struct Cyc_Core_Opt**f=_tmp1FD;
# 1019
({struct Cyc_Core_Opt*_tmp419=({struct Cyc_Core_Opt*_tmp205=_cycalloc(sizeof(*_tmp205));({void*_tmp418=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->tag=2U,_tmp204->f1=0,_tmp204->f2=& Cyc_Tcutil_ak;_tmp204;});_tmp205->v=_tmp418;});_tmp205;});*f=_tmp419;});goto _LL89;}}_LL71:;}}}}
# 1026
{struct Cyc_List_List*ts=new_cvtenv.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple13 _tmp207=*((struct _tuple13*)ts->hd);struct _tuple13 _stmttmp25=_tmp207;struct _tuple13 _tmp208=_stmttmp25;int _tmp20A;void*_tmp209;_LL8B: _tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;_LL8C: {void*tv=_tmp209;int put_in_eff=_tmp20A;
if(!put_in_eff)continue;{
struct Cyc_Absyn_Kind*_tmp20B=Cyc_Tcutil_type_kind(tv);struct Cyc_Absyn_Kind*_stmttmp26=_tmp20B;struct Cyc_Absyn_Kind*_tmp20C=_stmttmp26;switch(((struct Cyc_Absyn_Kind*)_tmp20C)->kind){case Cyc_Absyn_RgnKind: _LL8E: _LL8F:
# 1031
 effect=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));({void*_tmp41A=Cyc_Absyn_access_eff(tv);_tmp20D->hd=_tmp41A;}),_tmp20D->tl=effect;_tmp20D;});goto _LL8D;case Cyc_Absyn_EffKind: _LL90: _LL91:
# 1033
 effect=({struct Cyc_List_List*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->hd=tv,_tmp20E->tl=effect;_tmp20E;});goto _LL8D;case Cyc_Absyn_IntKind: _LL92: _LL93:
 goto _LL8D;default: _LL94: _LL95:
 effect=({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*_tmp20F));({void*_tmp41B=Cyc_Absyn_regionsof_eff(tv);_tmp20F->hd=_tmp41B;}),_tmp20F->tl=effect;_tmp20F;});goto _LL8D;}_LL8D:;}}}}
# 1038
({void*_tmp41C=Cyc_Absyn_join_eff(effect);*eff=_tmp41C;});}
# 1044
if(*btvs != 0){
struct Cyc_List_List*bs=*btvs;for(0;bs != 0;bs=bs->tl){
void*_tmp210=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_stmttmp27=_tmp210;void*_tmp211=_stmttmp27;struct Cyc_Absyn_Kind*_tmp213;struct Cyc_Core_Opt**_tmp212;struct Cyc_Core_Opt**_tmp214;struct Cyc_Core_Opt**_tmp215;struct Cyc_Core_Opt**_tmp216;struct Cyc_Core_Opt**_tmp217;struct Cyc_Core_Opt**_tmp218;struct Cyc_Core_Opt**_tmp219;struct Cyc_Core_Opt**_tmp21A;struct Cyc_Core_Opt**_tmp21B;struct Cyc_Core_Opt**_tmp21C;switch(*((int*)_tmp211)){case 1U: _LL97: _tmp21C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LL98: {struct Cyc_Core_Opt**f=_tmp21C;
# 1048
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp337;_tmp337.tag=0U,({struct _fat_ptr _tmp41D=({const char*_tmp222="Type variable ";_tag_fat(_tmp222,sizeof(char),15U);});_tmp337.f1=_tmp41D;});_tmp337;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp21F=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp336;_tmp336.tag=6U,_tmp336.f1=(struct Cyc_Absyn_Tvar*)bs->hd;_tmp336;});struct Cyc_Warn_String_Warn_Warg_struct _tmp220=({struct Cyc_Warn_String_Warn_Warg_struct _tmp335;_tmp335.tag=0U,({struct _fat_ptr _tmp41E=({const char*_tmp221=" unconstrained, assuming boxed";_tag_fat(_tmp221,sizeof(char),31U);});_tmp335.f1=_tmp41E;});_tmp335;});void*_tmp21D[3U];_tmp21D[0]=& _tmp21E,_tmp21D[1]=& _tmp21F,_tmp21D[2]=& _tmp220;({unsigned _tmp41F=loc;Cyc_Warn_warn2(_tmp41F,_tag_fat(_tmp21D,sizeof(void*),3U));});});
_tmp21B=f;goto _LL9A;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f2)->aliasqual == Cyc_Absyn_Top){_LL99: _tmp21B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LL9A: {struct Cyc_Core_Opt**f=_tmp21B;
_tmp21A=f;goto _LL9C;}}else{goto _LLA9;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9B: _tmp21A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LL9C: {struct Cyc_Core_Opt**f=_tmp21A;
_tmp219=f;goto _LL9E;}case Cyc_Absyn_Aliasable: _LL9D: _tmp219=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LL9E: {struct Cyc_Core_Opt**f=_tmp219;
_tmp217=f;goto _LLA0;}case Cyc_Absyn_Unique: _LLA3: _tmp218=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LLA4: {struct Cyc_Core_Opt**f=_tmp218;
# 1056
_tmp215=f;goto _LLA6;}default: goto _LLA9;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9F: _tmp217=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LLA0: {struct Cyc_Core_Opt**f=_tmp217;
# 1053
_tmp216=f;goto _LLA2;}case Cyc_Absyn_Aliasable: _LLA1: _tmp216=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LLA2: {struct Cyc_Core_Opt**f=_tmp216;
# 1055
({struct Cyc_Core_Opt*_tmp420=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp420;});goto _LL96;}case Cyc_Absyn_Unique: _LLA5: _tmp215=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LLA6: {struct Cyc_Core_Opt**f=_tmp215;
# 1058
({struct Cyc_Core_Opt*_tmp421=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp421;});goto _LL96;}default: goto _LLA9;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f2)->aliasqual == Cyc_Absyn_Top){_LLA7: _tmp214=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_LLA8: {struct Cyc_Core_Opt**f=_tmp214;
# 1060
({struct Cyc_Core_Opt*_tmp422=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp422;});goto _LL96;}}else{goto _LLA9;}default: _LLA9: _tmp212=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f1;_tmp213=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp211)->f2;_LLAA: {struct Cyc_Core_Opt**f=_tmp212;struct Cyc_Absyn_Kind*k=_tmp213;
# 1062
({struct Cyc_Core_Opt*_tmp423=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp423;});goto _LL96;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp211)->f1)->kind == Cyc_Absyn_MemKind){_LLAB: _LLAC:
# 1064
({struct Cyc_Warn_String_Warn_Warg_struct _tmp224=({struct Cyc_Warn_String_Warn_Warg_struct _tmp338;_tmp338.tag=0U,({struct _fat_ptr _tmp424=({const char*_tmp225="functions can't abstract M types";_tag_fat(_tmp225,sizeof(char),33U);});_tmp338.f1=_tmp424;});_tmp338;});void*_tmp223[1U];_tmp223[0]=& _tmp224;({unsigned _tmp425=loc;Cyc_Warn_err2(_tmp425,_tag_fat(_tmp223,sizeof(void*),1U));});});goto _LL96;}else{_LLAD: _LLAE:
 goto _LL96;}}_LL96:;}}
# 1069
({struct Cyc_List_List*_tmp426=Cyc_Tctyp_remove_bound_tvars(Cyc_Core_heap_region,new_cvtenv.kind_env,*btvs);cvtenv.kind_env=_tmp426;});
({struct Cyc_List_List*_tmp427=Cyc_Tctyp_remove_bound_tvars_bool(new_cvtenv.r,new_cvtenv.free_vars,*btvs);new_cvtenv.free_vars=_tmp427;});
# 1072
{struct Cyc_List_List*tvs=new_cvtenv.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp226=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp28=_tmp226;struct _tuple14 _tmp227=_stmttmp28;int _tmp229;struct Cyc_Absyn_Tvar*_tmp228;_LLB0: _tmp228=_tmp227.f1;_tmp229=_tmp227.f2;_LLB1: {struct Cyc_Absyn_Tvar*t=_tmp228;int b=_tmp229;
({struct Cyc_List_List*_tmp428=Cyc_Tctyp_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,t,b);cvtenv.free_vars=_tmp428;});}}}
# 1077
{struct Cyc_List_List*evs=new_cvtenv.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp22A=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp29=_tmp22A;struct _tuple13 _tmp22B=_stmttmp29;int _tmp22D;void*_tmp22C;_LLB3: _tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;_LLB4: {void*e=_tmp22C;int b=_tmp22D;
({struct Cyc_List_List*_tmp429=Cyc_Tctyp_add_free_evar(cvtenv.r,cvtenv.free_evars,e,b);cvtenv.free_evars=_tmp429;});}}}
# 1081
goto _LL0;}}}}}}case 6U: _LL13: _tmp137=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp130)->f1;_LL14: {struct Cyc_List_List*tq_ts=_tmp137;
# 1084
for(0;tq_ts != 0;tq_ts=tq_ts->tl){
struct _tuple17*_tmp231=(struct _tuple17*)tq_ts->hd;struct _tuple17*p=_tmp231;
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*p).f2,1,0);
({int _tmp42A=
Cyc_Tcutil_extract_const_from_typedef(loc,((*p).f1).print_const,(*p).f2);
# 1087
((*p).f1).real_const=_tmp42A;});}
# 1090
goto _LL0;}case 7U: _LL15: _tmp135=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp130)->f1;_tmp136=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp130)->f2;_LL16: {enum Cyc_Absyn_AggrKind k=_tmp135;struct Cyc_List_List*fs=_tmp136;
# 1094
struct Cyc_List_List*prev_fields=0;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp232=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp2A=_tmp232;struct Cyc_Absyn_Aggrfield*_tmp233=_stmttmp2A;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Exp*_tmp237;void*_tmp236;struct Cyc_Absyn_Tqual*_tmp235;struct _fat_ptr*_tmp234;_LLB6: _tmp234=_tmp233->name;_tmp235=(struct Cyc_Absyn_Tqual*)& _tmp233->tq;_tmp236=_tmp233->type;_tmp237=_tmp233->width;_tmp238=_tmp233->attributes;_tmp239=_tmp233->requires_clause;_LLB7: {struct _fat_ptr*fn=_tmp234;struct Cyc_Absyn_Tqual*tqp=_tmp235;void*t=_tmp236;struct Cyc_Absyn_Exp*width=_tmp237;struct Cyc_List_List*atts=_tmp238;struct Cyc_Absyn_Exp*requires_clause=_tmp239;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33B;_tmp33B.tag=0U,({struct _fat_ptr _tmp42B=({const char*_tmp23D="duplicate field ";_tag_fat(_tmp23D,sizeof(char),17U);});_tmp33B.f1=_tmp42B;});_tmp33B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33A;_tmp33A.tag=0U,_tmp33A.f1=*fn;_tmp33A;});void*_tmp23A[2U];_tmp23A[0]=& _tmp23B,_tmp23A[1]=& _tmp23C;({unsigned _tmp42C=loc;Cyc_Warn_err2(_tmp42C,_tag_fat(_tmp23A,sizeof(void*),2U));});});
if(({struct _fat_ptr _tmp42D=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp42D,({const char*_tmp23E="";_tag_fat(_tmp23E,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F->hd=fn,_tmp23F->tl=prev_fields;_tmp23F;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,t,1,0);
({int _tmp42E=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t);tqp->real_const=_tmp42E;});
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Tctyp_check_field_atts(loc,fn,atts);
if(requires_clause != 0){
# 1107
if((int)k != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp241=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33C;_tmp33C.tag=0U,({struct _fat_ptr _tmp42F=({const char*_tmp242="@requires clause is only allowed on union members";_tag_fat(_tmp242,sizeof(char),50U);});_tmp33C.f1=_tmp42F;});_tmp33C;});void*_tmp240[1U];_tmp240[0]=& _tmp241;({unsigned _tmp430=loc;Cyc_Warn_err2(_tmp430,_tag_fat(_tmp240,sizeof(void*),1U));});});
({struct Cyc_Tcenv_Tenv*_tmp431=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp431,0,requires_clause);});
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp244=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33F;_tmp33F.tag=0U,({struct _fat_ptr _tmp432=({const char*_tmp248="@requires clause has type ";_tag_fat(_tmp248,sizeof(char),27U);});_tmp33F.f1=_tmp432;});_tmp33F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp245=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp33E;_tmp33E.tag=2U,_tmp33E.f1=(void*)_check_null(requires_clause->topt);_tmp33E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp246=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33D;_tmp33D.tag=0U,({
struct _fat_ptr _tmp433=({const char*_tmp247=" instead of integral type";_tag_fat(_tmp247,sizeof(char),26U);});_tmp33D.f1=_tmp433;});_tmp33D;});void*_tmp243[3U];_tmp243[0]=& _tmp244,_tmp243[1]=& _tmp245,_tmp243[2]=& _tmp246;({unsigned _tmp434=loc;Cyc_Warn_err2(_tmp434,_tag_fat(_tmp243,sizeof(void*),3U));});});
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(requires_clause,te,cvtenv);}}}
# 1116
goto _LL0;}default: _LL17: _tmp131=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp130)->f1;_tmp132=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp130)->f2;_tmp133=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp130)->f3;_tmp134=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp130)->f4;_LL18: {struct _tuple1*tdn=_tmp131;struct Cyc_List_List**targs_ref=_tmp132;struct Cyc_Absyn_Typedefdecl**tdopt=_tmp133;void**toptp=_tmp134;
# 1119
struct Cyc_List_List*targs=*targs_ref;
# 1121
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp249;_push_handler(& _tmp249);{int _tmp24B=0;if(setjmp(_tmp249.handler))_tmp24B=1;if(!_tmp24B){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);;_pop_handler();}else{void*_tmp24A=(void*)Cyc_Core_get_exn_thrown();void*_tmp24C=_tmp24A;void*_tmp24D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp24C)->tag == Cyc_Dict_Absent){_LLB9: _LLBA:
# 1124
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp341;_tmp341.tag=0U,({struct _fat_ptr _tmp435=({const char*_tmp251="unbound typedef name ";_tag_fat(_tmp251,sizeof(char),22U);});_tmp341.f1=_tmp435;});_tmp341;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp250=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp340;_tmp340.tag=1U,_tmp340.f1=tdn;_tmp340;});void*_tmp24E[2U];_tmp24E[0]=& _tmp24F,_tmp24E[1]=& _tmp250;({unsigned _tmp436=loc;Cyc_Warn_err2(_tmp436,_tag_fat(_tmp24E,sizeof(void*),2U));});});
return cvtenv;}else{_LLBB: _tmp24D=_tmp24C;_LLBC: {void*exn=_tmp24D;(int)_rethrow(exn);}}_LLB8:;}}}
# 1127
*tdopt=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 1132
for(0;ts != 0 && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp253=_cycalloc(sizeof(*_tmp253));({struct _tuple18*_tmp437=({struct _tuple18*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp252->f2=(void*)ts->hd;_tmp252;});_tmp253->hd=_tmp437;}),_tmp253->tl=inst;_tmp253;});}
# 1138
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp255=({struct Cyc_Warn_String_Warn_Warg_struct _tmp343;_tmp343.tag=0U,({struct _fat_ptr _tmp438=({const char*_tmp257="too many parameters for typedef ";_tag_fat(_tmp257,sizeof(char),33U);});_tmp343.f1=_tmp438;});_tmp343;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp256=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp342;_tmp342.tag=1U,_tmp342.f1=tdn;_tmp342;});void*_tmp254[2U];_tmp254[0]=& _tmp255,_tmp254[1]=& _tmp256;({unsigned _tmp439=loc;Cyc_Warn_err2(_tmp439,_tag_fat(_tmp254,sizeof(void*),2U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 1143
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258->hd=e,_tmp258->tl=hidden_ts;_tmp258;});
cvtenv=Cyc_Tctyp_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp25A=_cycalloc(sizeof(*_tmp25A));({struct _tuple18*_tmp43A=({struct _tuple18*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp259->f2=e;_tmp259;});_tmp25A->hd=_tmp43A;}),_tmp25A->tl=inst;_tmp25A;});}
# 1151
({struct Cyc_List_List*_tmp43C=({struct Cyc_List_List*_tmp43B=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp43B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs_ref=_tmp43C;});}
# 1153
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*toptp=new_typ;}
# 1159
goto _LL0;}}}_LL0:;}
# 1161
if(!({struct Cyc_Absyn_Kind*_tmp43D=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmp43D,expected_kind);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp349;_tmp349.tag=0U,({struct _fat_ptr _tmp43E=({const char*_tmp264="type ";_tag_fat(_tmp264,sizeof(char),6U);});_tmp349.f1=_tmp43E;});_tmp349;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp25D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp348;_tmp348.tag=2U,_tmp348.f1=(void*)t;_tmp348;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp347;_tmp347.tag=0U,({struct _fat_ptr _tmp43F=({const char*_tmp263=" has kind ";_tag_fat(_tmp263,sizeof(char),11U);});_tmp347.f1=_tmp43F;});_tmp347;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp25F=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp346;_tmp346.tag=8U,({struct Cyc_Absyn_Kind*_tmp440=Cyc_Tcutil_type_kind(t);_tmp346.f1=_tmp440;});_tmp346;});struct Cyc_Warn_String_Warn_Warg_struct _tmp260=({struct Cyc_Warn_String_Warn_Warg_struct _tmp345;_tmp345.tag=0U,({
struct _fat_ptr _tmp441=({const char*_tmp262=" but as used here needs kind ";_tag_fat(_tmp262,sizeof(char),30U);});_tmp345.f1=_tmp441;});_tmp345;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp261=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp344;_tmp344.tag=8U,_tmp344.f1=expected_kind;_tmp344;});void*_tmp25B[6U];_tmp25B[0]=& _tmp25C,_tmp25B[1]=& _tmp25D,_tmp25B[2]=& _tmp25E,_tmp25B[3]=& _tmp25F,_tmp25B[4]=& _tmp260,_tmp25B[5]=& _tmp261;({unsigned _tmp442=loc;Cyc_Warn_err2(_tmp442,_tag_fat(_tmp25B,sizeof(void*),6U));});});
# 1165
return cvtenv;}
# 1173
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvtenv){
# 1175
{void*_tmp265=e->r;void*_stmttmp2B=_tmp265;void*_tmp266=_stmttmp2B;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*_tmp268;void*_tmp269;void*_tmp26A;void*_tmp26B;struct Cyc_Absyn_Exp*_tmp26D;void*_tmp26C;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_List_List*_tmp277;switch(*((int*)_tmp266)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp277=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp266)->f2;_LLC: {struct Cyc_List_List*es=_tmp277;
# 1182
for(0;es != 0;es=es->tl){
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)es->hd,te,cvtenv);}
goto _LL0;}case 6U: _LLD: _tmp274=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_tmp275=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp266)->f2;_tmp276=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp266)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp274;struct Cyc_Absyn_Exp*e2=_tmp275;struct Cyc_Absyn_Exp*e3=_tmp276;
# 1186
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e1,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e2,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e3,te,cvtenv);
goto _LL0;}case 7U: _LLF: _tmp272=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_tmp273=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp266)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp272;struct Cyc_Absyn_Exp*e2=_tmp273;
_tmp270=e1;_tmp271=e2;goto _LL12;}case 8U: _LL11: _tmp270=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_tmp271=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp266)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp270;struct Cyc_Absyn_Exp*e2=_tmp271;
_tmp26E=e1;_tmp26F=e2;goto _LL14;}case 9U: _LL13: _tmp26E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_tmp26F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp266)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp26E;struct Cyc_Absyn_Exp*e2=_tmp26F;
# 1193
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e1,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e2,te,cvtenv);
goto _LL0;}case 14U: _LL15: _tmp26C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_tmp26D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp266)->f2;_LL16: {void*t=_tmp26C;struct Cyc_Absyn_Exp*e1=_tmp26D;
# 1197
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e1,te,cvtenv);
cvtenv=Cyc_Tctyp_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,t,1,0);
goto _LL0;}case 19U: _LL17: _tmp26B=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_LL18: {void*t=_tmp26B;
_tmp26A=t;goto _LL1A;}case 17U: _LL19: _tmp26A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_LL1A: {void*t=_tmp26A;
# 1202
cvtenv=Cyc_Tctyp_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,t,1,0);
goto _LL0;}case 39U: _LL1B: _tmp269=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_LL1C: {void*t=_tmp269;
# 1205
cvtenv=Cyc_Tctyp_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,t,1,0);
goto _LL0;}case 18U: _LL1D: _tmp268=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp268;
# 1208
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e,te,cvtenv);
goto _LL0;}case 41U: _LL1F: _tmp267=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp266)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp267;
# 1211
cvtenv=Cyc_Tctyp_i_check_valid_type_level_exp(e,te,cvtenv);
goto _LL0;}default: _LL21: _LL22:
# 1214
({struct Cyc_Warn_String_Warn_Warg_struct _tmp279=({struct Cyc_Warn_String_Warn_Warg_struct _tmp34A;_tmp34A.tag=0U,({struct _fat_ptr _tmp443=({const char*_tmp27A="non-type-level-expression in Tctyp::i_check_valid_type_level_exp";_tag_fat(_tmp27A,sizeof(char),65U);});_tmp34A.f1=_tmp443;});_tmp34A;});void*_tmp278[1U];_tmp278[0]=& _tmp279;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp278,sizeof(void*),1U));});}_LL0:;}
# 1216
return cvtenv;}
# 1219
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1224
struct Cyc_List_List*_tmp27B=cvt.kind_env;struct Cyc_List_List*kind_env=_tmp27B;
cvt=Cyc_Tctyp_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 1227
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
({struct Cyc_List_List*_tmp444=Cyc_Tctyp_fast_add_free_tvar(kind_env,(*((struct _tuple14*)vs->hd)).f1);cvt.kind_env=_tmp444;});}}
# 1233
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp27C=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp2C=_tmp27C;struct _tuple13 _tmp27D=_stmttmp2C;void*_tmp27E;_LL1: _tmp27E=_tmp27D.f1;_LL2: {void*e=_tmp27E;
void*_tmp27F=Cyc_Tcutil_compress(e);void*_stmttmp2D=_tmp27F;void*_tmp280=_stmttmp2D;struct Cyc_Core_Opt**_tmp281;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->tag == 1U){_LL4: _tmp281=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f4;_LL5: {struct Cyc_Core_Opt**s=_tmp281;
# 1237
if(*s == 0)
({struct Cyc_Core_Opt*_tmp445=({struct Cyc_Core_Opt*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->v=kind_env;_tmp282;});*s=_tmp445;});else{
# 1241
struct Cyc_List_List*_tmp283=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*s))->v;struct Cyc_List_List*tvs=_tmp283;
struct Cyc_List_List*_tmp284=0;struct Cyc_List_List*result=_tmp284;
for(0;tvs != 0;tvs=tvs->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)tvs->hd))
result=({struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp285->tl=result;_tmp285;});}
({struct Cyc_Core_Opt*_tmp446=({struct Cyc_Core_Opt*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->v=result;_tmp286;});*s=_tmp446;});}
# 1248
goto _LL3;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}}}
# 1252
return cvt;}
# 1261
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
if(te->in_tempest && !te->tempest_generalize)generalize_evars=0;{
struct Cyc_List_List*_tmp287=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*te_vars=_tmp287;
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tctyp_CVTEnv _tmp288=({unsigned _tmp44A=loc;struct Cyc_Tcenv_Tenv*_tmp449=te;struct Cyc_Tctyp_CVTEnv _tmp448=({struct Cyc_Tctyp_CVTEnv _tmp353;_tmp353.r=Cyc_Core_heap_region,_tmp353.kind_env=te_vars,_tmp353.fn_result=0,_tmp353.generalize_evars=generalize_evars,_tmp353.free_vars=0,_tmp353.free_evars=0;_tmp353;});struct Cyc_Absyn_Kind*_tmp447=expected_kind;Cyc_Tctyp_check_valid_type(_tmp44A,_tmp449,_tmp448,_tmp447,1,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp288;
# 1270
struct Cyc_List_List*_tmp289=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst,cvt.free_vars);struct Cyc_List_List*free_tvars=_tmp289;
struct Cyc_List_List*_tmp28A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_fst,cvt.free_evars);struct Cyc_List_List*free_evars=_tmp28A;
# 1273
if(te_vars != 0){
struct Cyc_List_List*_tmp28B=0;struct Cyc_List_List*res=_tmp28B;
{struct Cyc_List_List*_tmp28C=free_tvars;struct Cyc_List_List*fs=_tmp28C;for(0;(unsigned)fs;fs=fs->tl){
struct Cyc_Absyn_Tvar*_tmp28D=(struct Cyc_Absyn_Tvar*)fs->hd;struct Cyc_Absyn_Tvar*f=_tmp28D;
int found=0;
{struct Cyc_List_List*_tmp28E=te_vars;struct Cyc_List_List*ts=_tmp28E;for(0;(unsigned)ts;ts=ts->tl){
if(Cyc_Absyn_tvar_cmp(f,(struct Cyc_Absyn_Tvar*)ts->hd)== 0){found=1;break;}}}
if(!found)
res=({struct Cyc_List_List*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->hd=(struct Cyc_Absyn_Tvar*)fs->hd,_tmp28F->tl=res;_tmp28F;});}}
# 1283
free_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);}
# 1288
{struct Cyc_List_List*x=free_tvars;for(0;x != 0;x=x->tl){
void*_tmp290=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_stmttmp2E=_tmp290;void*_tmp291=_stmttmp2E;enum Cyc_Absyn_AliasQual _tmp292;struct Cyc_Absyn_Kind*_tmp294;struct Cyc_Core_Opt**_tmp293;struct Cyc_Core_Opt**_tmp295;struct Cyc_Core_Opt**_tmp296;struct Cyc_Core_Opt**_tmp297;struct Cyc_Core_Opt**_tmp298;struct Cyc_Core_Opt**_tmp299;struct Cyc_Core_Opt**_tmp29A;switch(*((int*)_tmp291)){case 1U: _LL1: _tmp29A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp291)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp29A;
_tmp299=f;goto _LL4;}case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp299=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp299;
_tmp298=f;goto _LL6;}}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp298=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp298;
_tmp297=f;goto _LL8;}case Cyc_Absyn_Aliasable: _LL7: _tmp297=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp297;
# 1294
({struct Cyc_Core_Opt*_tmp44B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp44B;});goto _LL0;}case Cyc_Absyn_Unique: _LL9: _tmp296=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f1;_LLA: {struct Cyc_Core_Opt**f=_tmp296;
# 1296
({struct Cyc_Core_Opt*_tmp44C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp44C;});goto _LL0;}default: goto _LLD;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp295=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f1;_LLC: {struct Cyc_Core_Opt**f=_tmp295;
# 1298
({struct Cyc_Core_Opt*_tmp44D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp44D;});goto _LL0;}}else{goto _LLD;}default: _LLD: _tmp293=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f1;_tmp294=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f2;_LLE: {struct Cyc_Core_Opt**f=_tmp293;struct Cyc_Absyn_Kind*k=_tmp294;
# 1300
({struct Cyc_Core_Opt*_tmp44E=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp44E;});goto _LL0;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp291)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp292=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp291)->f1)->aliasqual;_LL10: {enum Cyc_Absyn_AliasQual a=_tmp292;
# 1302
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp34E;_tmp34E.tag=0U,({struct _fat_ptr _tmp44F=({const char*_tmp2A2="type variable ";_tag_fat(_tmp2A2,sizeof(char),15U);});_tmp34E.f1=_tmp44F;});_tmp34E;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp29D=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp34D;_tmp34D.tag=6U,_tmp34D.f1=(struct Cyc_Absyn_Tvar*)x->hd;_tmp34D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp34C;_tmp34C.tag=0U,({struct _fat_ptr _tmp450=({const char*_tmp2A1=" cannot have kind ";_tag_fat(_tmp2A1,sizeof(char),19U);});_tmp34C.f1=_tmp450;});_tmp34C;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp29F=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp34B;_tmp34B.tag=8U,({struct Cyc_Absyn_Kind*_tmp451=({struct Cyc_Absyn_Kind*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->kind=Cyc_Absyn_MemKind,_tmp2A0->aliasqual=a;_tmp2A0;});_tmp34B.f1=_tmp451;});_tmp34B;});void*_tmp29B[4U];_tmp29B[0]=& _tmp29C,_tmp29B[1]=& _tmp29D,_tmp29B[2]=& _tmp29E,_tmp29B[3]=& _tmp29F;({unsigned _tmp452=loc;Cyc_Warn_err2(_tmp452,_tag_fat(_tmp29B,sizeof(void*),4U));});});
goto _LL0;}}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 1308
if(free_tvars != 0 || free_evars != 0){
{void*_tmp2A3=Cyc_Tcutil_compress(t);void*_stmttmp2F=_tmp2A3;void*_tmp2A4=_stmttmp2F;struct Cyc_List_List**_tmp2A5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2A4)->tag == 5U){_LL14: _tmp2A5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2A4)->f1).tvars;_LL15: {struct Cyc_List_List**btvs=_tmp2A5;
# 1311
if(*btvs == 0){
# 1313
({struct Cyc_List_List*_tmp453=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(free_tvars);*btvs=_tmp453;});
free_tvars=0;}
# 1316
goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 1319
if(free_tvars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp352;_tmp352.tag=0U,({struct _fat_ptr _tmp454=({const char*_tmp2AC="unbound type variable ";_tag_fat(_tmp2AC,sizeof(char),23U);});_tmp352.f1=_tmp454;});_tmp352;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2A8=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp351;_tmp351.tag=6U,_tmp351.f1=(struct Cyc_Absyn_Tvar*)free_tvars->hd;_tmp351;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp350;_tmp350.tag=0U,({struct _fat_ptr _tmp455=({const char*_tmp2AB=" when checking ";_tag_fat(_tmp2AB,sizeof(char),16U);});_tmp350.f1=_tmp455;});_tmp350;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2AA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp34F;_tmp34F.tag=2U,_tmp34F.f1=(void*)t;_tmp34F;});void*_tmp2A6[4U];_tmp2A6[0]=& _tmp2A7,_tmp2A6[1]=& _tmp2A8,_tmp2A6[2]=& _tmp2A9,_tmp2A6[3]=& _tmp2AA;({unsigned _tmp456=loc;Cyc_Warn_err2(_tmp456,_tag_fat(_tmp2A6,sizeof(void*),4U));});});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}}}
# 1330
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1333
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_tmp2AD=Cyc_Tcutil_compress(t);void*_stmttmp30=_tmp2AD;void*_tmp2AE=_stmttmp30;struct Cyc_Absyn_FnInfo _tmp2AF;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2AE)->tag == 5U){_LL1: _tmp2AF=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2AE)->f1;_LL2: {struct Cyc_Absyn_FnInfo i=_tmp2AF;
# 1336
struct Cyc_List_List*_tmp2B0=(fd->i).attributes;struct Cyc_List_List*atts=_tmp2B0;
fd->i=i;
(fd->i).attributes=atts;
({int _tmp457=
Cyc_Tcutil_extract_const_from_typedef(loc,(i.ret_tqual).print_const,i.ret_type);
# 1339
((fd->i).ret_tqual).real_const=_tmp457;});
# 1341
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp354;_tmp354.tag=0U,({struct _fat_ptr _tmp458=({const char*_tmp2B3="check_fndecl_valid_type: not a FnType";_tag_fat(_tmp2B3,sizeof(char),38U);});_tmp354.f1=_tmp458;});_tmp354;});void*_tmp2B1[1U];_tmp2B1[0]=& _tmp2B2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2B1,sizeof(void*),1U));});}_LL0:;}
# 1344
({struct Cyc_List_List*_tmp45A=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)((struct _fat_ptr*(*)(struct _tuple9*t))Cyc_Tctyp_fst_fdarg,(fd->i).args);unsigned _tmp459=loc;Cyc_Tcutil_check_unique_vars(_tmp45A,_tmp459,({const char*_tmp2B4="function declaration has repeated parameter";_tag_fat(_tmp2B4,sizeof(char),44U);}));});
# 1347
fd->cached_type=t;}
# 1352
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1355
struct Cyc_Tctyp_CVTEnv _tmp2B5=({unsigned _tmp45F=loc;struct Cyc_Tcenv_Tenv*_tmp45E=te;struct Cyc_Tctyp_CVTEnv _tmp45D=({struct Cyc_Tctyp_CVTEnv _tmp359;_tmp359.r=Cyc_Core_heap_region,_tmp359.kind_env=bound_tvars,_tmp359.fn_result=0,_tmp359.generalize_evars=0,_tmp359.free_vars=0,_tmp359.free_evars=0;_tmp359;});struct Cyc_Absyn_Kind*_tmp45C=expected_kind;int _tmp45B=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_tmp45F,_tmp45E,_tmp45D,_tmp45C,_tmp45B,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp2B5;
# 1359
struct Cyc_List_List*_tmp2B6=({struct _RegionHandle*_tmp461=Cyc_Core_heap_region;struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst,cvt.free_vars);Cyc_Tctyp_remove_bound_tvars(_tmp461,_tmp460,bound_tvars);});struct Cyc_List_List*free_tvars=_tmp2B6;
# 1361
struct Cyc_List_List*_tmp2B7=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_fst,cvt.free_evars);struct Cyc_List_List*free_evars=_tmp2B7;
{struct Cyc_List_List*fs=free_tvars;for(0;fs != 0;fs=fs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp358;_tmp358.tag=0U,({struct _fat_ptr _tmp462=({const char*_tmp2BE="unbound type variable ";_tag_fat(_tmp2BE,sizeof(char),23U);});_tmp358.f1=_tmp462;});_tmp358;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2BA=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp357;_tmp357.tag=6U,_tmp357.f1=(struct Cyc_Absyn_Tvar*)fs->hd;_tmp357;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp356;_tmp356.tag=0U,({struct _fat_ptr _tmp463=({const char*_tmp2BD=" in type ";_tag_fat(_tmp2BD,sizeof(char),10U);});_tmp356.f1=_tmp463;});_tmp356;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp355;_tmp355.tag=2U,_tmp355.f1=(void*)t;_tmp355;});void*_tmp2B8[4U];_tmp2B8[0]=& _tmp2B9,_tmp2B8[1]=& _tmp2BA,_tmp2B8[2]=& _tmp2BB,_tmp2B8[3]=& _tmp2BC;({unsigned _tmp464=loc;Cyc_Warn_err2(_tmp464,_tag_fat(_tmp2B8,sizeof(void*),4U));});});}}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}

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
# 1134
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
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tctyp_CVTEnv{unsigned loc;struct Cyc_Tcenv_Tenv*te;struct Cyc_List_List*kind_env;int fn_result;int generalize_evars;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;};
# 57 "tctyp.cyc"
static struct _fat_ptr*Cyc_Tctyp_fst_fdarg(struct _tuple9*t){return(struct _fat_ptr*)_check_null((*t).f1);}struct _tuple13{void*f1;int f2;};
# 63
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct Cyc_List_List*es,void*e,int b){
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
return({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));({struct _tuple13*_tmp353=({struct _tuple13*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=e,_tmpA->f2=b;_tmpA;});_tmpB->hd=_tmp353;}),_tmpB->tl=es;_tmpB;});}}else{_LL3: _LL4:
 return es;}_LL0:;}
# 84
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BC;_tmp2BC.tag=0U,({struct _fat_ptr _tmp354=({const char*_tmp11="bound tvar id for ";_tag_fat(_tmp11,sizeof(char),19U);});_tmp2BC.f1=_tmp354;});_tmp2BC;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpE=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2BB;_tmp2BB.tag=7U,_tmp2BB.f1=tv;_tmp2BB;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA;_tmp2BA.tag=0U,({struct _fat_ptr _tmp355=({const char*_tmp10=" is NULL";_tag_fat(_tmp10,sizeof(char),9U);});_tmp2BA.f1=_tmp355;});_tmp2BA;});void*_tmpC[3U];_tmpC[0]=& _tmpD,_tmpC[1]=& _tmpE,_tmpC[2]=& _tmpF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC,sizeof(void*),3U));});
return({struct Cyc_List_List*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=tv,_tmp12->tl=tvs;_tmp12;});}
# 89
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 91
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_List_List*b=btvs;
for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity)
break;}
if(b == 0)
r=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp13->tl=r;_tmp13;});}
# 100
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}struct _tuple14{struct Cyc_Absyn_Tvar*f1;int f2;};
# 103
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars_bool(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 105
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp14=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp3=_tmp14;struct _tuple14 _tmp15=_stmttmp3;struct Cyc_Absyn_Tvar*_tmp16;_LL1: _tmp16=_tmp15.f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp16;
struct Cyc_List_List*b=btvs;
for(0;b != 0;b=b->tl){
if(tv->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity)
break;}
if(b == 0)
res=({struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=(struct _tuple14*)tvs->hd,_tmp17->tl=res;_tmp17;});}}
# 115
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 123
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(*((struct _tuple13*)free_evars->hd)).f1;
{void*_tmp18=Cyc_Tcutil_compress(e);void*_stmttmp4=_tmp18;void*_tmp19=_stmttmp4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
 continue;}_LL0:;}{
# 130
void*t;
struct _fat_ptr s;
{struct Cyc_Absyn_Kind*_tmp1A=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_stmttmp5=_tmp1A;struct Cyc_Absyn_Kind*_tmp1B=_stmttmp5;switch(((struct Cyc_Absyn_Kind*)_tmp1B)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp1B)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
 t=Cyc_Absyn_unique_rgn_type;s=({const char*_tmp1C="unique region";_tag_fat(_tmp1C,sizeof(char),14U);});goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
 t=Cyc_Absyn_heap_rgn_type;s=({const char*_tmp1D="heap";_tag_fat(_tmp1D,sizeof(char),5U);});goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
 t=Cyc_Absyn_empty_effect;s=({const char*_tmp1E="{}";_tag_fat(_tmp1E,sizeof(char),3U);});goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
 t=Cyc_Absyn_false_type;s=({const char*_tmp1F="@false";_tag_fat(_tmp1F,sizeof(char),7U);});goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
 t=Cyc_Absyn_bounds_one();s=({const char*_tmp20="bounds_one";_tag_fat(_tmp20,sizeof(char),11U);});goto _LL5;default: _LL12: _LL13:
# 140
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0;_tmp2C0.tag=0U,({struct _fat_ptr _tmp356=({const char*_tmp27="hidden type variable ";_tag_fat(_tmp27,sizeof(char),22U);});_tmp2C0.f1=_tmp356;});_tmp2C0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp23=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BF;_tmp2BF.tag=2U,_tmp2BF.f1=(void*)e;_tmp2BF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BE;_tmp2BE.tag=0U,({struct _fat_ptr _tmp357=({const char*_tmp26=" isn't abstracted in type ";_tag_fat(_tmp26,sizeof(char),27U);});_tmp2BE.f1=_tmp357;});_tmp2BE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp25=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BD;_tmp2BD.tag=2U,_tmp2BD.f1=(void*)in_typ;_tmp2BD;});void*_tmp21[4U];_tmp21[0]=& _tmp22,_tmp21[1]=& _tmp23,_tmp21[2]=& _tmp24,_tmp21[3]=& _tmp25;({unsigned _tmp358=loc;Cyc_Warn_err2(_tmp358,_tag_fat(_tmp21,sizeof(void*),4U));});});
return;}_LL5:;}
# 143
if(!Cyc_Unify_unify(e,t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5;_tmp2C5.tag=0U,({struct _fat_ptr _tmp359=({const char*_tmp30="can't unify evar ";_tag_fat(_tmp30,sizeof(char),18U);});_tmp2C5.f1=_tmp359;});_tmp2C5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C4;_tmp2C4.tag=2U,_tmp2C4.f1=(void*)e;_tmp2C4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3;_tmp2C3.tag=0U,({struct _fat_ptr _tmp35A=({const char*_tmp2F=" with ";_tag_fat(_tmp2F,sizeof(char),7U);});_tmp2C3.f1=_tmp35A;});_tmp2C3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2;_tmp2C2.tag=0U,_tmp2C2.f1=s;_tmp2C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp35B=({const char*_tmp2E="!";_tag_fat(_tmp2E,sizeof(char),2U);});_tmp2C1.f1=_tmp35B;});_tmp2C1;});void*_tmp28[5U];_tmp28[0]=& _tmp29,_tmp28[1]=& _tmp2A,_tmp28[2]=& _tmp2B,_tmp28[3]=& _tmp2C,_tmp28[4]=& _tmp2D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp28,sizeof(void*),5U));});}}}
# 152
static int Cyc_Tctyp_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp31=({struct _tuple0 _tmp2C6;_tmp2C6.f1=c1,_tmp2C6.f2=c2;_tmp2C6;});struct _tuple0 _stmttmp6=_tmp31;struct _tuple0 _tmp32=_stmttmp6;struct Cyc_Absyn_Kind*_tmp36;struct Cyc_Core_Opt**_tmp35;struct Cyc_Absyn_Kind*_tmp34;struct Cyc_Core_Opt**_tmp33;struct Cyc_Absyn_Kind*_tmp39;struct Cyc_Absyn_Kind*_tmp38;struct Cyc_Core_Opt**_tmp37;struct Cyc_Core_Opt**_tmp3A;struct Cyc_Core_Opt**_tmp3B;struct Cyc_Absyn_Kind*_tmp3E;struct Cyc_Core_Opt**_tmp3D;struct Cyc_Absyn_Kind*_tmp3C;struct Cyc_Absyn_Kind*_tmp40;struct Cyc_Absyn_Kind*_tmp3F;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp32.f1)->tag == 0U)switch(*((int*)_tmp32.f2)){case 0U: _LL1: _tmp3F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp32.f1)->f1;_tmp40=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp32.f2)->f1;_LL2: {struct Cyc_Absyn_Kind*k1=_tmp3F;struct Cyc_Absyn_Kind*k2=_tmp40;
return k1 == k2;}case 1U: goto _LL3;default: _LL9: _tmp3C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp32.f1)->f1;_tmp3D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f2)->f1;_tmp3E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f2)->f2;_LLA: {struct Cyc_Absyn_Kind*k1=_tmp3C;struct Cyc_Core_Opt**f=_tmp3D;struct Cyc_Absyn_Kind*k2=_tmp3E;
# 166
if(!Cyc_Tcutil_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_tmp35C=({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->v=c1;_tmp44;});*f=_tmp35C;});
return 1;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp32.f2)->tag == 1U){_LL3: _tmp3B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp32.f2)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp3B;
# 158
({struct Cyc_Core_Opt*_tmp35D=({struct Cyc_Core_Opt*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->v=c1;_tmp41;});*f=_tmp35D;});return 1;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp32.f1)->tag == 1U){_LL5: _tmp3A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp32.f1)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp3A;
({struct Cyc_Core_Opt*_tmp35E=({struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=c2;_tmp42;});*f=_tmp35E;});return 1;}}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp32.f2)->tag == 0U){_LL7: _tmp37=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f1)->f1;_tmp38=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f1)->f2;_tmp39=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp32.f2)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp37;struct Cyc_Absyn_Kind*k1=_tmp38;struct Cyc_Absyn_Kind*k2=_tmp39;
# 161
if(!Cyc_Tcutil_kind_leq(k2,k1))
return 0;
({struct Cyc_Core_Opt*_tmp35F=({struct Cyc_Core_Opt*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->v=c2;_tmp43;});*f=_tmp35F;});
return 1;}}else{_LLB: _tmp33=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f1)->f1;_tmp34=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f1)->f2;_tmp35=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f2)->f1;_tmp36=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32.f2)->f2;_LLC: {struct Cyc_Core_Opt**f1=_tmp33;struct Cyc_Absyn_Kind*k1=_tmp34;struct Cyc_Core_Opt**f2=_tmp35;struct Cyc_Absyn_Kind*k2=_tmp36;
# 171
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp360=({struct Cyc_Core_Opt*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->v=c1;_tmp45;});*f2=_tmp360;});
return 1;}
# 175
if(Cyc_Tcutil_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp361=({struct Cyc_Core_Opt*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->v=c2;_tmp46;});*f1=_tmp361;});
return 1;}
# 179
return 0;}}}}}_LL0:;}}
# 188
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tctyp_constrain_kinds(k1,k2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp48=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC;_tmp2CC.tag=0U,({struct _fat_ptr _tmp362=({const char*_tmp50="type variable ";_tag_fat(_tmp50,sizeof(char),15U);});_tmp2CC.f1=_tmp362;});_tmp2CC;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp49=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2CB;_tmp2CB.tag=7U,_tmp2CB.f1=tv;_tmp2CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA;_tmp2CA.tag=0U,({struct _fat_ptr _tmp363=({const char*_tmp4F=" is used with inconsistent kinds ";_tag_fat(_tmp4F,sizeof(char),34U);});_tmp2CA.f1=_tmp363;});_tmp2CA;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp4B=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp2C9;_tmp2C9.tag=8U,_tmp2C9.f1=(void*)k1;_tmp2C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8;_tmp2C8.tag=0U,({
struct _fat_ptr _tmp364=({const char*_tmp4E=" and ";_tag_fat(_tmp4E,sizeof(char),6U);});_tmp2C8.f1=_tmp364;});_tmp2C8;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp4D=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp2C7;_tmp2C7.tag=8U,_tmp2C7.f1=(void*)k2;_tmp2C7;});void*_tmp47[6U];_tmp47[0]=& _tmp48,_tmp47[1]=& _tmp49,_tmp47[2]=& _tmp4A,_tmp47[3]=& _tmp4B,_tmp47[4]=& _tmp4C,_tmp47[5]=& _tmp4D;({unsigned _tmp365=loc;Cyc_Warn_err2(_tmp365,_tag_fat(_tmp47,sizeof(void*),6U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD;_tmp2CD.tag=0U,({struct _fat_ptr _tmp366=({const char*_tmp53="same type variable has different identity!";_tag_fat(_tmp53,sizeof(char),43U);});_tmp2CD.f1=_tmp366;});_tmp2CD;});void*_tmp51[1U];_tmp51[0]=& _tmp52;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp51,sizeof(void*),1U));});}
return tvs;}}}
# 203
({int _tmp367=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp367;});
return({struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=tv,_tmp54->tl=tvs;_tmp54;});}
# 209
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CE;_tmp2CE.tag=0U,({struct _fat_ptr _tmp368=({const char*_tmp57="fast_add_free_tvar: bad identity in tv";_tag_fat(_tmp57,sizeof(char),39U);});_tmp2CE.f1=_tmp368;});_tmp2CE;});void*_tmp55[1U];_tmp55[0]=& _tmp56;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp55,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp58=(struct Cyc_Absyn_Tvar*)tvs2->hd;struct Cyc_Absyn_Tvar*tv2=_tmp58;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF;_tmp2CF.tag=0U,({struct _fat_ptr _tmp369=({const char*_tmp5B="fast_add_free_tvar: bad identity in tvs2";_tag_fat(_tmp5B,sizeof(char),41U);});_tmp2CF.f1=_tmp369;});_tmp2CF;});void*_tmp59[1U];_tmp59[0]=& _tmp5A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp59,sizeof(void*),1U));});
if(tv2->identity == tv->identity)
return tvs;}}
# 219
return({struct Cyc_List_List*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->hd=tv,_tmp5C->tl=tvs;_tmp5C;});}
# 225
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D0;_tmp2D0.tag=0U,({struct _fat_ptr _tmp36A=({const char*_tmp5F="fast_add_free_tvar_bool: bad identity in tv";_tag_fat(_tmp5F,sizeof(char),44U);});_tmp2D0.f1=_tmp36A;});_tmp2D0;});void*_tmp5D[1U];_tmp5D[0]=& _tmp5E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp5D,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct _tuple14*_tmp60=(struct _tuple14*)tvs2->hd;struct _tuple14*_stmttmp7=_tmp60;struct _tuple14*_tmp61=_stmttmp7;int*_tmp63;struct Cyc_Absyn_Tvar*_tmp62;_LL1: _tmp62=_tmp61->f1;_tmp63=(int*)& _tmp61->f2;_LL2: {struct Cyc_Absyn_Tvar*tv2=_tmp62;int*b2=_tmp63;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp65=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D1;_tmp2D1.tag=0U,({struct _fat_ptr _tmp36B=({const char*_tmp66="fast_add_free_tvar_bool: bad identity in tvs2";_tag_fat(_tmp66,sizeof(char),46U);});_tmp2D1.f1=_tmp36B;});_tmp2D1;});void*_tmp64[1U];_tmp64[0]=& _tmp65;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp64,sizeof(void*),1U));});
if(tv2->identity == tv->identity){
*b2=*b2 || b;
return tvs;}}}}
# 237
return({struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _tuple14*_tmp36C=({struct _tuple14*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->f1=tv,_tmp67->f2=b;_tmp67;});_tmp68->hd=_tmp36C;}),_tmp68->tl=tvs;_tmp68;});}
# 240
static int Cyc_Tctyp_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td == 0)
return 1;
if(td->defn == 0)
return 0;{
void*_tmp69=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_stmttmp8=_tmp69;void*_tmp6A=_stmttmp8;void*_tmp6B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->tag == 3U){_LL1: _tmp6B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp6B;
# 247
void*_tmp6C=Cyc_Tcutil_compress(r);void*_stmttmp9=_tmp6C;void*_tmp6D=_stmttmp9;struct Cyc_Absyn_Tvar*_tmp6E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->tag == 2U){_LL6: _tmp6E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1;_LL7: {struct Cyc_Absyn_Tvar*tv=_tmp6E;
return Cyc_Absyn_tvar_cmp(tvar,tv)== 0;}}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 251
 return 0;}_LL0:;}}
# 255
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*,struct Cyc_Tctyp_CVTEnv);
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type(struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*,void*,int,int);
# 258
static struct Cyc_Absyn_Kind*Cyc_Tctyp_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 261
void*_tmp6F=Cyc_Absyn_compress_kb(tvar->kind);void*_stmttmpA=_tmp6F;void*_tmp70=_stmttmpA;switch(*((int*)_tmp70)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp70)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp70)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp70)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp70)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4: {
# 270
enum Cyc_Absyn_KindQual _tmp71=expected_kind->kind;enum Cyc_Absyn_KindQual k=_tmp71;
if((((int)k == (int)2U ||(int)k == (int)1U)||(int)k == (int)0U)&&
 Cyc_Tctyp_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}
# 278
return& Cyc_Tcutil_trk;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 282
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_complete_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr){
# 286
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->hd=e,_tmp72->tl=hidden_ts;_tmp72;});{
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
env=Cyc_Tctyp_i_check_valid_type(env,k,e,1,allow_abs_aggr);}}
# 293
({struct Cyc_List_List*_tmp36E=({struct Cyc_List_List*_tmp36D=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp36D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmp36E;});
return env;}
# 297
static void Cyc_Tctyp_check_field_atts(unsigned loc,struct _fat_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp73=(void*)atts->hd;void*_stmttmpB=_tmp73;void*_tmp74=_stmttmpB;switch(*((int*)_tmp74)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp76=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5;_tmp2D5.tag=0U,({struct _fat_ptr _tmp36F=({const char*_tmp7B="bad attribute ";_tag_fat(_tmp7B,sizeof(char),15U);});_tmp2D5.f1=_tmp36F;});_tmp2D5;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp77=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp2D4;_tmp2D4.tag=10U,_tmp2D4.f1=(void*)atts->hd;_tmp2D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp78=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3;_tmp2D3.tag=0U,({struct _fat_ptr _tmp370=({const char*_tmp7A=" on member ";_tag_fat(_tmp7A,sizeof(char),12U);});_tmp2D3.f1=_tmp370;});_tmp2D3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp79=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D2;_tmp2D2.tag=0U,_tmp2D2.f1=*fn;_tmp2D2;});void*_tmp75[4U];_tmp75[0]=& _tmp76,_tmp75[1]=& _tmp77,_tmp75[2]=& _tmp78,_tmp75[3]=& _tmp79;({unsigned _tmp371=loc;Cyc_Warn_err2(_tmp371,_tag_fat(_tmp75,sizeof(void*),4U));});});}_LL0:;}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f1;struct Cyc_List_List*f2;};
# 307
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 310
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(env.te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9;_tmp2D9.tag=0U,_tmp2D9.f1=clause_name;_tmp2D9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D8;_tmp2D8.tag=0U,({struct _fat_ptr _tmp372=({const char*_tmp82=" clause has type ";_tag_fat(_tmp82,sizeof(char),18U);});_tmp2D8.f1=_tmp372;});_tmp2D8;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp2D7;_tmp2D7.tag=3U,_tmp2D7.f1=(void*)clause->topt;_tmp2D7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6;_tmp2D6.tag=0U,({
struct _fat_ptr _tmp373=({const char*_tmp81=" instead of integral type";_tag_fat(_tmp81,sizeof(char),26U);});_tmp2D6.f1=_tmp373;});_tmp2D6;});void*_tmp7C[4U];_tmp7C[0]=& _tmp7D,_tmp7C[1]=& _tmp7E,_tmp7C[2]=& _tmp7F,_tmp7C[3]=& _tmp80;({unsigned _tmp374=env.loc;Cyc_Warn_err2(_tmp374,_tag_fat(_tmp7C,sizeof(void*),4U));});});
env=Cyc_Tctyp_i_check_valid_type_level_exp(clause,env);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DD;_tmp2DD.tag=0U,_tmp2DD.f1=clause_name;_tmp2DD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp85=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC;_tmp2DC.tag=0U,({
struct _fat_ptr _tmp375=({const char*_tmp89=" clause '";_tag_fat(_tmp89,sizeof(char),10U);});_tmp2DC.f1=_tmp375;});_tmp2DC;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp86=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2DB;_tmp2DB.tag=4U,_tmp2DB.f1=clause;_tmp2DB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA;_tmp2DA.tag=0U,({struct _fat_ptr _tmp376=({const char*_tmp88="' may be unsatisfiable";_tag_fat(_tmp88,sizeof(char),23U);});_tmp2DA.f1=_tmp376;});_tmp2DA;});void*_tmp83[4U];_tmp83[0]=& _tmp84,_tmp83[1]=& _tmp85,_tmp83[2]=& _tmp86,_tmp83[3]=& _tmp87;({unsigned _tmp377=clause->loc;Cyc_Warn_err2(_tmp377,_tag_fat(_tmp83,sizeof(void*),4U));});});}
# 322
return({struct _tuple15 _tmp2DE;_tmp2DE.f1=env,_tmp2DE.f2=relns;_tmp2DE;});}
# 354 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 359
struct Cyc_Tctyp_CVTEnv _tmp8A=env;struct Cyc_Tcenv_Tenv*_tmp8C;unsigned _tmp8B;_LL1: _tmp8B=_tmp8A.loc;_tmp8C=_tmp8A.te;_LL2: {unsigned loc=_tmp8B;struct Cyc_Tcenv_Tenv*te=_tmp8C;
{union Cyc_Absyn_AggrInfo _tmp8D=*info;union Cyc_Absyn_AggrInfo _stmttmpC=_tmp8D;union Cyc_Absyn_AggrInfo _tmp8E=_stmttmpC;struct Cyc_Absyn_Aggrdecl*_tmp8F;struct Cyc_Core_Opt*_tmp92;struct _tuple1*_tmp91;enum Cyc_Absyn_AggrKind _tmp90;if((_tmp8E.UnknownAggr).tag == 1){_LL4: _tmp90=((_tmp8E.UnknownAggr).val).f1;_tmp91=((_tmp8E.UnknownAggr).val).f2;_tmp92=((_tmp8E.UnknownAggr).val).f3;_LL5: {enum Cyc_Absyn_AggrKind k=_tmp90;struct _tuple1*n=_tmp91;struct Cyc_Core_Opt*tgd=_tmp92;
# 362
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp93;_push_handler(& _tmp93);{int _tmp95=0;if(setjmp(_tmp93.handler))_tmp95=1;if(!_tmp95){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);{
struct Cyc_Absyn_Aggrdecl*_tmp96=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp96;
if((int)ad->kind != (int)k){
if((int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp98=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2;_tmp2E2.tag=0U,({struct _fat_ptr _tmp378=({const char*_tmp9D="expecting struct ";_tag_fat(_tmp9D,sizeof(char),18U);});_tmp2E2.f1=_tmp378;});_tmp2E2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp99=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E1;_tmp2E1.tag=1U,_tmp2E1.f1=n;_tmp2E1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0;_tmp2E0.tag=0U,({struct _fat_ptr _tmp379=({const char*_tmp9C="instead of union ";_tag_fat(_tmp9C,sizeof(char),18U);});_tmp2E0.f1=_tmp379;});_tmp2E0;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2DF;_tmp2DF.tag=1U,_tmp2DF.f1=n;_tmp2DF;});void*_tmp97[4U];_tmp97[0]=& _tmp98,_tmp97[1]=& _tmp99,_tmp97[2]=& _tmp9A,_tmp97[3]=& _tmp9B;({unsigned _tmp37A=loc;Cyc_Warn_err2(_tmp37A,_tag_fat(_tmp97,sizeof(void*),4U));});});else{
# 370
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=0U,({struct _fat_ptr _tmp37B=({const char*_tmpA4="expecting union ";_tag_fat(_tmpA4,sizeof(char),17U);});_tmp2E6.f1=_tmp37B;});_tmp2E6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E5;_tmp2E5.tag=1U,_tmp2E5.f1=n;_tmp2E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0U,({struct _fat_ptr _tmp37C=({const char*_tmpA3="instead of struct ";_tag_fat(_tmpA3,sizeof(char),19U);});_tmp2E4.f1=_tmp37C;});_tmp2E4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=1U,_tmp2E3.f1=n;_tmp2E3;});void*_tmp9E[4U];_tmp9E[0]=& _tmp9F,_tmp9E[1]=& _tmpA0,_tmp9E[2]=& _tmpA1,_tmp9E[3]=& _tmpA2;({unsigned _tmp37D=loc;Cyc_Warn_err2(_tmp37D,_tag_fat(_tmp9E,sizeof(void*),4U));});});}}
# 372
if((unsigned)tgd &&(int)tgd->v){
if(!((unsigned)ad->impl)|| !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E8;_tmp2E8.tag=0U,({struct _fat_ptr _tmp37E=({const char*_tmpA8="@tagged qualfiers don't agree on union ";_tag_fat(_tmpA8,sizeof(char),40U);});_tmp2E8.f1=_tmp37E;});_tmp2E8;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpA7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=1U,_tmp2E7.f1=n;_tmp2E7;});void*_tmpA5[2U];_tmpA5[0]=& _tmpA6,_tmpA5[1]=& _tmpA7;({unsigned _tmp37F=loc;Cyc_Warn_err2(_tmp37F,_tag_fat(_tmpA5,sizeof(void*),2U));});});}
# 377
({union Cyc_Absyn_AggrInfo _tmp380=Cyc_Absyn_KnownAggr(adp);*info=_tmp380;});}
# 364
;_pop_handler();}else{void*_tmp94=(void*)Cyc_Core_get_exn_thrown();void*_tmpA9=_tmp94;void*_tmpAA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA9)->tag == Cyc_Dict_Absent){_LL9: _LLA: {
# 381
struct Cyc_Absyn_Aggrdecl*_tmpAB=({struct Cyc_Absyn_Aggrdecl*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->kind=k,_tmpB2->sc=Cyc_Absyn_Extern,_tmpB2->name=n,_tmpB2->tvs=0,_tmpB2->impl=0,_tmpB2->attributes=0,_tmpB2->expected_mem_kind=0;_tmpB2;});struct Cyc_Absyn_Aggrdecl*ad=_tmpAB;
Cyc_Tc_tcAggrdecl(te,loc,ad);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);
({union Cyc_Absyn_AggrInfo _tmp381=Cyc_Absyn_KnownAggr(adp);*info=_tmp381;});
# 386
if(*targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=0U,({struct _fat_ptr _tmp382=({const char*_tmpB1="declare parameterized type ";_tag_fat(_tmpB1,sizeof(char),28U);});_tmp2EB.f1=_tmp382;});_tmp2EB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=1U,_tmp2EA.f1=n;_tmp2EA;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=0U,({struct _fat_ptr _tmp383=({const char*_tmpB0=" before using";_tag_fat(_tmpB0,sizeof(char),14U);});_tmp2E9.f1=_tmp383;});_tmp2E9;});void*_tmpAC[3U];_tmpAC[0]=& _tmpAD,_tmpAC[1]=& _tmpAE,_tmpAC[2]=& _tmpAF;({unsigned _tmp384=loc;Cyc_Warn_err2(_tmp384,_tag_fat(_tmpAC,sizeof(void*),3U));});});
return env;}
# 390
goto _LL8;}}else{_LLB: _tmpAA=_tmpA9;_LLC: {void*exn=_tmpAA;(int)_rethrow(exn);}}_LL8:;}}}
# 392
_tmp8F=*adp;goto _LL7;}}else{_LL6: _tmp8F=*(_tmp8E.KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp8F;
# 394
struct Cyc_List_List*tvs=ad->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0 && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpB3=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmpB3;
void*_tmpB4=(void*)ts->hd;void*t=_tmpB4;
# 402
{struct _tuple0 _tmpB5=({struct _tuple0 _tmp2EC;({void*_tmp385=Cyc_Absyn_compress_kb(tv->kind);_tmp2EC.f1=_tmp385;}),_tmp2EC.f2=t;_tmp2EC;});struct _tuple0 _stmttmpD=_tmpB5;struct _tuple0 _tmpB6=_stmttmpD;struct Cyc_Absyn_Tvar*_tmpB7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB6.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB6.f2)->tag == 2U){_LLE: _tmpB7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB6.f2)->f1;_LLF: {struct Cyc_Absyn_Tvar*tv2=_tmpB7;
# 404
({struct Cyc_List_List*_tmp386=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,tv2);env.kind_env=_tmp386;});
({struct Cyc_List_List*_tmp387=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_tmp387;});
continue;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}{
# 409
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
env=Cyc_Tctyp_i_check_valid_type(env,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);}}
# 413
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE;_tmp2EE.tag=0U,({struct _fat_ptr _tmp388=({const char*_tmpBB="too many parameters for ";_tag_fat(_tmpBB,sizeof(char),25U);});_tmp2EE.f1=_tmp388;});_tmp2EE;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=6U,_tmp2ED.f1=ad;_tmp2ED;});void*_tmpB8[2U];_tmpB8[0]=& _tmpB9,_tmpB8[1]=& _tmpBA;({unsigned _tmp389=loc;Cyc_Warn_err2(_tmp389,_tag_fat(_tmpB8,sizeof(void*),2U));});});
env=Cyc_Tctyp_complete_type_inst(env,targs,tvs,expected_kind,allow_abs_aggr);
if((allow_abs_aggr && ad->impl == 0)&& !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 421
ad->expected_mem_kind=1;}}_LL3:;}
# 424
return env;}}
# 428
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 433
struct Cyc_Tctyp_CVTEnv _tmpBC=env;struct Cyc_Tcenv_Tenv*_tmpBE;unsigned _tmpBD;_LL1: _tmpBD=_tmpBC.loc;_tmpBE=_tmpBC.te;_LL2: {unsigned loc=_tmpBD;struct Cyc_Tcenv_Tenv*te=_tmpBE;
struct Cyc_List_List*_tmpBF=*targsp;struct Cyc_List_List*targs=_tmpBF;
{union Cyc_Absyn_DatatypeInfo _tmpC0=*info;union Cyc_Absyn_DatatypeInfo _stmttmpE=_tmpC0;union Cyc_Absyn_DatatypeInfo _tmpC1=_stmttmpE;struct Cyc_Absyn_Datatypedecl*_tmpC2;int _tmpC4;struct _tuple1*_tmpC3;if((_tmpC1.UnknownDatatype).tag == 1){_LL4: _tmpC3=((_tmpC1.UnknownDatatype).val).name;_tmpC4=((_tmpC1.UnknownDatatype).val).is_extensible;_LL5: {struct _tuple1*n=_tmpC3;int is_x=_tmpC4;
# 437
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpC5;_push_handler(& _tmpC5);{int _tmpC7=0;if(setjmp(_tmpC5.handler))_tmpC7=1;if(!_tmpC7){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);;_pop_handler();}else{void*_tmpC6=(void*)Cyc_Core_get_exn_thrown();void*_tmpC8=_tmpC6;void*_tmpC9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC8)->tag == Cyc_Dict_Absent){_LL9: _LLA: {
# 441
struct Cyc_Absyn_Datatypedecl*_tmpCA=({struct Cyc_Absyn_Datatypedecl*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->sc=Cyc_Absyn_Extern,_tmpD1->name=n,_tmpD1->tvs=0,_tmpD1->fields=0,_tmpD1->is_extensible=is_x;_tmpD1;});struct Cyc_Absyn_Datatypedecl*tud=_tmpCA;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 445
if(targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=0U,({struct _fat_ptr _tmp38A=({const char*_tmpD0="declare parameterized datatype ";_tag_fat(_tmpD0,sizeof(char),32U);});_tmp2F1.f1=_tmp38A;});_tmp2F1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpCD=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=1U,_tmp2F0.f1=n;_tmp2F0;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0U,({struct _fat_ptr _tmp38B=({const char*_tmpCF=" before using";_tag_fat(_tmpCF,sizeof(char),14U);});_tmp2EF.f1=_tmp38B;});_tmp2EF;});void*_tmpCB[3U];_tmpCB[0]=& _tmpCC,_tmpCB[1]=& _tmpCD,_tmpCB[2]=& _tmpCE;({unsigned _tmp38C=loc;Cyc_Warn_err2(_tmp38C,_tag_fat(_tmpCB,sizeof(void*),3U));});});
return env;}
# 449
goto _LL8;}}else{_LLB: _tmpC9=_tmpC8;_LLC: {void*exn=_tmpC9;(int)_rethrow(exn);}}_LL8:;}}}
# 453
if(is_x && !(*tudp)->is_extensible)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F4;_tmp2F4.tag=0U,({struct _fat_ptr _tmp38D=({const char*_tmpD7="datatype ";_tag_fat(_tmpD7,sizeof(char),10U);});_tmp2F4.f1=_tmp38D;});_tmp2F4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpD4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F3;_tmp2F3.tag=1U,_tmp2F3.f1=n;_tmp2F3;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2;_tmp2F2.tag=0U,({struct _fat_ptr _tmp38E=({const char*_tmpD6=" was not declared @extensible";_tag_fat(_tmpD6,sizeof(char),30U);});_tmp2F2.f1=_tmp38E;});_tmp2F2;});void*_tmpD2[3U];_tmpD2[0]=& _tmpD3,_tmpD2[1]=& _tmpD4,_tmpD2[2]=& _tmpD5;({unsigned _tmp38F=loc;Cyc_Warn_err2(_tmp38F,_tag_fat(_tmpD2,sizeof(void*),3U));});});
({union Cyc_Absyn_DatatypeInfo _tmp390=Cyc_Absyn_KnownDatatype(tudp);*info=_tmp390;});
_tmpC2=*tudp;goto _LL7;}}else{_LL6: _tmpC2=*(_tmpC1.KnownDatatype).val;_LL7: {struct Cyc_Absyn_Datatypedecl*tud=_tmpC2;
# 459
struct Cyc_List_List*tvs=tud->tvs;
for(0;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 465
{struct _tuple0 _tmpD8=({struct _tuple0 _tmp2F5;({void*_tmp391=Cyc_Absyn_compress_kb(tv->kind);_tmp2F5.f1=_tmp391;}),_tmp2F5.f2=t;_tmp2F5;});struct _tuple0 _stmttmpF=_tmpD8;struct _tuple0 _tmpD9=_stmttmpF;struct Cyc_Absyn_Tvar*_tmpDA;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD9.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD9.f2)->tag == 2U){_LLE: _tmpDA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD9.f2)->f1;_LLF: {struct Cyc_Absyn_Tvar*tv2=_tmpDA;
# 467
({struct Cyc_List_List*_tmp392=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,tv2);env.kind_env=_tmp392;});
({struct Cyc_List_List*_tmp393=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_tmp393;});
continue;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}{
# 472
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
env=Cyc_Tctyp_i_check_valid_type(env,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);}}
# 476
if(targs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F7;_tmp2F7.tag=0U,({struct _fat_ptr _tmp394=({const char*_tmpDE="too many type arguments for datatype ";_tag_fat(_tmpDE,sizeof(char),38U);});_tmp2F7.f1=_tmp394;});_tmp2F7;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpDD=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F6;_tmp2F6.tag=1U,_tmp2F6.f1=tud->name;_tmp2F6;});void*_tmpDB[2U];_tmpDB[0]=& _tmpDC,_tmpDB[1]=& _tmpDD;({unsigned _tmp395=loc;Cyc_Warn_err2(_tmp395,_tag_fat(_tmpDB,sizeof(void*),2U));});});
env=Cyc_Tctyp_complete_type_inst(env,targsp,tvs,expected_kind,allow_abs_aggr);
goto _LL3;}}_LL3:;}
# 481
return env;}}
# 485
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 490
struct Cyc_Tctyp_CVTEnv _tmpDF=env;struct Cyc_Tcenv_Tenv*_tmpE1;unsigned _tmpE0;_LL1: _tmpE0=_tmpDF.loc;_tmpE1=_tmpDF.te;_LL2: {unsigned loc=_tmpE0;struct Cyc_Tcenv_Tenv*te=_tmpE1;
{union Cyc_Absyn_DatatypeFieldInfo _tmpE2=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmp10=_tmpE2;union Cyc_Absyn_DatatypeFieldInfo _tmpE3=_stmttmp10;struct Cyc_Absyn_Datatypefield*_tmpE5;struct Cyc_Absyn_Datatypedecl*_tmpE4;int _tmpE8;struct _tuple1*_tmpE7;struct _tuple1*_tmpE6;if((_tmpE3.UnknownDatatypefield).tag == 1){_LL4: _tmpE6=((_tmpE3.UnknownDatatypefield).val).datatype_name;_tmpE7=((_tmpE3.UnknownDatatypefield).val).field_name;_tmpE8=((_tmpE3.UnknownDatatypefield).val).is_extensible;_LL5: {struct _tuple1*tname=_tmpE6;struct _tuple1*fname=_tmpE7;int is_x=_tmpE8;
# 494
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,tname);
struct Cyc_Absyn_Datatypefield*tuf;
# 499
{struct Cyc_List_List*_tmpE9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmpE9;for(0;1;fs=fs->tl){
if(fs == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F8;_tmp2F8.tag=0U,({struct _fat_ptr _tmp396=({const char*_tmpEC="Tcutil found a bad datatypefield";_tag_fat(_tmpEC,sizeof(char),33U);});_tmp2F8.f1=_tmp396;});_tmp2F8;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpEA,sizeof(void*),1U));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)fs->hd)->name,fname)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 506
({union Cyc_Absyn_DatatypeFieldInfo _tmp397=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmp397;});
_tmpE4=tud;_tmpE5=tuf;goto _LL7;}}else{_LL6: _tmpE4=((_tmpE3.KnownDatatypefield).val).f1;_tmpE5=((_tmpE3.KnownDatatypefield).val).f2;_LL7: {struct Cyc_Absyn_Datatypedecl*tud=_tmpE4;struct Cyc_Absyn_Datatypefield*tuf=_tmpE5;
# 510
struct Cyc_List_List*tvs=tud->tvs;
for(0;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 516
{struct _tuple0 _tmpED=({struct _tuple0 _tmp2F9;({void*_tmp398=Cyc_Absyn_compress_kb(tv->kind);_tmp2F9.f1=_tmp398;}),_tmp2F9.f2=t;_tmp2F9;});struct _tuple0 _stmttmp11=_tmpED;struct _tuple0 _tmpEE=_stmttmp11;struct Cyc_Absyn_Tvar*_tmpEF;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpEE.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEE.f2)->tag == 2U){_LL9: _tmpEF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEE.f2)->f1;_LLA: {struct Cyc_Absyn_Tvar*tv2=_tmpEF;
# 518
({struct Cyc_List_List*_tmp399=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,tv2);env.kind_env=_tmp399;});
({struct Cyc_List_List*_tmp39A=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_tmp39A;});
continue;}}else{goto _LLB;}}else{_LLB: _LLC:
 goto _LL8;}_LL8:;}{
# 523
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
env=Cyc_Tctyp_i_check_valid_type(env,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);}}
# 527
if(targs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FD;_tmp2FD.tag=0U,({struct _fat_ptr _tmp39B=({const char*_tmpF6="too many type arguments for datatype ";_tag_fat(_tmpF6,sizeof(char),38U);});_tmp2FD.f1=_tmp39B;});_tmp2FD;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FC;_tmp2FC.tag=1U,_tmp2FC.f1=tud->name;_tmp2FC;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB;_tmp2FB.tag=0U,({struct _fat_ptr _tmp39C=({const char*_tmpF5=".";_tag_fat(_tmpF5,sizeof(char),2U);});_tmp2FB.f1=_tmp39C;});_tmp2FB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FA;_tmp2FA.tag=1U,_tmp2FA.f1=tuf->name;_tmp2FA;});void*_tmpF0[4U];_tmpF0[0]=& _tmpF1,_tmpF0[1]=& _tmpF2,_tmpF0[2]=& _tmpF3,_tmpF0[3]=& _tmpF4;({unsigned _tmp39D=loc;Cyc_Warn_err2(_tmp39D,_tag_fat(_tmpF0,sizeof(void*),4U));});});
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp301;_tmp301.tag=0U,({struct _fat_ptr _tmp39E=({const char*_tmpFD="too few type arguments for datatype ";_tag_fat(_tmpFD,sizeof(char),37U);});_tmp301.f1=_tmp39E;});_tmp301;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpF9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp300;_tmp300.tag=1U,_tmp300.f1=tud->name;_tmp300;});struct Cyc_Warn_String_Warn_Warg_struct _tmpFA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FF;_tmp2FF.tag=0U,({struct _fat_ptr _tmp39F=({const char*_tmpFC=".";_tag_fat(_tmpFC,sizeof(char),2U);});_tmp2FF.f1=_tmp39F;});_tmp2FF;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpFB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FE;_tmp2FE.tag=1U,_tmp2FE.f1=tuf->name;_tmp2FE;});void*_tmpF7[4U];_tmpF7[0]=& _tmpF8,_tmpF7[1]=& _tmpF9,_tmpF7[2]=& _tmpFA,_tmpF7[3]=& _tmpFB;({unsigned _tmp3A0=loc;Cyc_Warn_err2(_tmp3A0,_tag_fat(_tmpF7,sizeof(void*),4U));});});
goto _LL3;}}_LL3:;}
# 533
return env;}}
# 536
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 540
struct Cyc_Tctyp_CVTEnv _tmpFE=env;struct Cyc_Tcenv_Tenv*_tmp100;unsigned _tmpFF;_LL1: _tmpFF=_tmpFE.loc;_tmp100=_tmpFE.te;_LL2: {unsigned loc=_tmpFF;struct Cyc_Tcenv_Tenv*te=_tmp100;
struct Cyc_List_List*_tmp101=*targsp;struct Cyc_List_List*ts=_tmp101;
void*_tmp102=c;union Cyc_Absyn_DatatypeFieldInfo*_tmp103;union Cyc_Absyn_DatatypeInfo*_tmp104;union Cyc_Absyn_AggrInfo*_tmp105;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Enumdecl**_tmp108;struct _tuple1*_tmp107;switch(*((int*)_tmp102)){case 1U: _LL4: _LL5:
# 544
 goto _LL7;case 2U: _LL6: _LL7: goto _LL9;case 0U: _LL8: _LL9: goto _LLB;case 7U: _LLA: _LLB:
 goto _LLD;case 6U: _LLC: _LLD: goto _LLF;case 5U: _LLE: _LLF: goto _LL11;case 12U: _LL10: _LL11:
 goto _LL13;case 11U: _LL12: _LL13: goto _LL15;case 14U: _LL14: _LL15: goto _LL17;case 17U: _LL16: _LL17:
# 548
 if(ts != 0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp10A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp303;_tmp303.tag=2U,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->tag=0U,_tmp10D->f1=c,_tmp10D->f2=ts;_tmp10D;})))_tmp3A1=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->tag=0U,_tmp10D->f1=c,_tmp10D->f2=ts;_tmp10D;}));_tmp303.f1=_tmp3A1;});_tmp303;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp302;_tmp302.tag=0U,({struct _fat_ptr _tmp3A2=({const char*_tmp10C=" applied to argument(s)";_tag_fat(_tmp10C,sizeof(char),24U);});_tmp302.f1=_tmp3A2;});_tmp302;});void*_tmp109[2U];_tmp109[0]=& _tmp10A,_tmp109[1]=& _tmp10B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp109,sizeof(void*),2U));});
return env;case 9U: _LL18: _LL19:
# 552
 for(0;ts != 0;ts=ts->tl){
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_ek,(void*)ts->hd,1,1);}
return env;case 15U: _LL1A: _tmp107=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp102)->f1;_tmp108=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp102)->f2;_LL1B: {struct _tuple1*n=_tmp107;struct Cyc_Absyn_Enumdecl**edp=_tmp108;
# 556
if(ts != 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp304;_tmp304.tag=0U,({struct _fat_ptr _tmp3A3=({const char*_tmp110="enum applied to argument(s)";_tag_fat(_tmp110,sizeof(char),28U);});_tmp304.f1=_tmp3A3;});_tmp304;});void*_tmp10E[1U];_tmp10E[0]=& _tmp10F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp10E,sizeof(void*),1U));});
if(*edp == 0 ||((struct Cyc_Absyn_Enumdecl*)_check_null(*edp))->fields == 0){
struct _handler_cons _tmp111;_push_handler(& _tmp111);{int _tmp113=0;if(setjmp(_tmp111.handler))_tmp113=1;if(!_tmp113){({struct Cyc_Absyn_Enumdecl*_tmp3A4=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp3A4;});;_pop_handler();}else{void*_tmp112=(void*)Cyc_Core_get_exn_thrown();void*_tmp114=_tmp112;void*_tmp115;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp114)->tag == Cyc_Dict_Absent){_LL2F: _LL30:
# 560
({struct Cyc_Tcenv_Tenv*_tmp3A6=te;unsigned _tmp3A5=loc;Cyc_Tc_tcEnumdecl(_tmp3A6,_tmp3A5,({struct Cyc_Absyn_Enumdecl*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->sc=Cyc_Absyn_Extern,_tmp116->name=n,_tmp116->fields=0;_tmp116;}));});
({struct Cyc_Absyn_Enumdecl*_tmp3A7=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp3A7;});
goto _LL2E;}else{_LL31: _tmp115=_tmp114;_LL32: {void*exn=_tmp115;(int)_rethrow(exn);}}_LL2E:;}}}
# 565
return env;}case 16U: _LL1C: _tmp106=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp102)->f1;_LL1D: {struct Cyc_List_List*fs=_tmp106;
# 567
if(ts != 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmp118=({struct Cyc_Warn_String_Warn_Warg_struct _tmp305;_tmp305.tag=0U,({struct _fat_ptr _tmp3A8=({const char*_tmp119="enum applied to argument(s)";_tag_fat(_tmp119,sizeof(char),28U);});_tmp305.f1=_tmp3A8;});_tmp305;});void*_tmp117[1U];_tmp117[0]=& _tmp118;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp117,sizeof(void*),1U));});{
# 569
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp11A=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp11A;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp307;_tmp307.tag=0U,({struct _fat_ptr _tmp3A9=({const char*_tmp11E="duplicate enum field name ";_tag_fat(_tmp11E,sizeof(char),27U);});_tmp307.f1=_tmp3A9;});_tmp307;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp11D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp306;_tmp306.tag=1U,_tmp306.f1=f->name;_tmp306;});void*_tmp11B[2U];_tmp11B[0]=& _tmp11C,_tmp11B[1]=& _tmp11D;({unsigned _tmp3AA=f->loc;Cyc_Warn_err2(_tmp3AA,_tag_fat(_tmp11B,sizeof(void*),2U));});});else{
# 576
prev_fields=({struct Cyc_List_List*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->hd=(*f->name).f2,_tmp11F->tl=prev_fields;_tmp11F;});}
if(f->tag == 0)
({struct Cyc_Absyn_Exp*_tmp3AB=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp3AB;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp121=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30A;_tmp30A.tag=0U,({struct _fat_ptr _tmp3AC=({const char*_tmp125="enum field ";_tag_fat(_tmp125,sizeof(char),12U);});_tmp30A.f1=_tmp3AC;});_tmp30A;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp122=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp309;_tmp309.tag=1U,_tmp309.f1=f->name;_tmp309;});struct Cyc_Warn_String_Warn_Warg_struct _tmp123=({struct Cyc_Warn_String_Warn_Warg_struct _tmp308;_tmp308.tag=0U,({struct _fat_ptr _tmp3AD=({const char*_tmp124=": expression is not constant";_tag_fat(_tmp124,sizeof(char),29U);});_tmp308.f1=_tmp3AD;});_tmp308;});void*_tmp120[3U];_tmp120[0]=& _tmp121,_tmp120[1]=& _tmp122,_tmp120[2]=& _tmp123;({unsigned _tmp3AE=loc;Cyc_Warn_err2(_tmp3AE,_tag_fat(_tmp120,sizeof(void*),3U));});});}
tag_count=(unsigned)1 + (Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))).f1;}
# 583
return env;}}case 10U: _LL1E: _LL1F:
# 585
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp127=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30B;_tmp30B.tag=0U,({struct _fat_ptr _tmp3AF=({const char*_tmp128="regions has wrong number of arguments";_tag_fat(_tmp128,sizeof(char),38U);});_tmp30B.f1=_tmp3AF;});_tmp30B;});void*_tmp126[1U];_tmp126[0]=& _tmp127;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp126,sizeof(void*),1U));});
return Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,1,1);case 3U: _LL20: _LL21:
# 588
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp12A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30C;_tmp30C.tag=0U,({struct _fat_ptr _tmp3B0=({const char*_tmp12B="region_t has wrong number of arguments";_tag_fat(_tmp12B,sizeof(char),39U);});_tmp30C.f1=_tmp3B0;});_tmp30C;});void*_tmp129[1U];_tmp129[0]=& _tmp12A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp129,sizeof(void*),1U));});
return Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,1,1);case 8U: _LL22: _LL23:
# 591
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp12D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30D;_tmp30D.tag=0U,({struct _fat_ptr _tmp3B1=({const char*_tmp12E="access has wrong number of arguments";_tag_fat(_tmp12E,sizeof(char),37U);});_tmp30D.f1=_tmp3B1;});_tmp30D;});void*_tmp12C[1U];_tmp12C[0]=& _tmp12D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp12C,sizeof(void*),1U));});
return Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,1,1);case 4U: _LL24: _LL25:
# 594
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp130=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30E;_tmp30E.tag=0U,({struct _fat_ptr _tmp3B2=({const char*_tmp131="tag_t has wrong number of arguments";_tag_fat(_tmp131,sizeof(char),36U);});_tmp30E.f1=_tmp3B2;});_tmp30E;});void*_tmp12F[1U];_tmp12F[0]=& _tmp130;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp12F,sizeof(void*),1U));});
return Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,0,1);case 13U: _LL26: _LL27:
# 597
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)({struct Cyc_Warn_String_Warn_Warg_struct _tmp133=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30F;_tmp30F.tag=0U,({struct _fat_ptr _tmp3B3=({const char*_tmp134="@thin has wrong number of arguments";_tag_fat(_tmp134,sizeof(char),36U);});_tmp30F.f1=_tmp3B3;});_tmp30F;});void*_tmp132[1U];_tmp132[0]=& _tmp133;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp132,sizeof(void*),1U));});
return Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,0,1);case 20U: _LL28: _tmp105=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp102)->f1;_LL29: {union Cyc_Absyn_AggrInfo*info=_tmp105;
# 600
return Cyc_Tctyp_i_check_valid_aggr(env,expected_kind,info,targsp,allow_abs_aggr);}case 18U: _LL2A: _tmp104=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp102)->f1;_LL2B: {union Cyc_Absyn_DatatypeInfo*info=_tmp104;
# 602
return Cyc_Tctyp_i_check_valid_datatype(env,expected_kind,info,targsp,allow_abs_aggr);}default: _LL2C: _tmp103=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp102)->f1;_LL2D: {union Cyc_Absyn_DatatypeFieldInfo*info=_tmp103;
# 604
return Cyc_Tctyp_i_check_valid_datatype_field(env,expected_kind,info,ts,allow_abs_aggr);}}_LL3:;}}struct _tuple16{enum Cyc_Absyn_Format_Type f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 609
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 611
struct Cyc_Tctyp_CVTEnv _tmp135=env;struct Cyc_Tcenv_Tenv*_tmp137;unsigned _tmp136;_LL1: _tmp136=_tmp135.loc;_tmp137=_tmp135.te;_LL2: {unsigned loc=_tmp136;struct Cyc_Tcenv_Tenv*te=_tmp137;
{void*_tmp138=Cyc_Tcutil_compress(t);void*_stmttmp12=_tmp138;void*_tmp139=_stmttmp12;void**_tmp13D;struct Cyc_Absyn_Typedefdecl**_tmp13C;struct Cyc_List_List**_tmp13B;struct _tuple1*_tmp13A;struct Cyc_List_List*_tmp13F;enum Cyc_Absyn_AggrKind _tmp13E;struct Cyc_List_List*_tmp140;struct Cyc_List_List**_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_List_List**_tmp14B;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_VarargInfo*_tmp147;int _tmp146;struct Cyc_List_List*_tmp145;void*_tmp144;struct Cyc_Absyn_Tqual*_tmp143;void**_tmp142;struct Cyc_List_List**_tmp141;unsigned _tmp152;void*_tmp151;struct Cyc_Absyn_Exp**_tmp150;struct Cyc_Absyn_Tqual*_tmp14F;void*_tmp14E;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp154;void*_tmp15A;void*_tmp159;void*_tmp158;void*_tmp157;struct Cyc_Absyn_Tqual*_tmp156;void*_tmp155;void***_tmp15C;void*_tmp15B;struct Cyc_Absyn_Tvar*_tmp15D;void**_tmp15F;struct Cyc_Core_Opt**_tmp15E;struct Cyc_List_List**_tmp161;void*_tmp160;switch(*((int*)_tmp139)){case 0U: _LL4: _tmp160=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp139)->f1;_tmp161=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp139)->f2;_LL5: {void*c=_tmp160;struct Cyc_List_List**targsp=_tmp161;
# 614
env=Cyc_Tctyp_i_check_valid_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr);
# 616
goto _LL3;}case 1U: _LL6: _tmp15E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp139)->f1;_tmp15F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp139)->f2;_LL7: {struct Cyc_Core_Opt**k=_tmp15E;void**ref=_tmp15F;
# 619
if(*k == 0 ||
 Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v)&& !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v,expected_kind))
({struct Cyc_Core_Opt*_tmp3B4=Cyc_Tcutil_kind_to_opt(expected_kind);*k=_tmp3B4;});
if(((env.fn_result && env.generalize_evars)&&(int)expected_kind->kind == (int)Cyc_Absyn_RgnKind)&& !te->tempest_generalize){
# 624
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*ref=Cyc_Absyn_unique_rgn_type;else{
# 627
*ref=Cyc_Absyn_heap_rgn_type;}}else{
if((env.generalize_evars &&(int)expected_kind->kind != (int)Cyc_Absyn_BoolKind)&&(int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 631
struct Cyc_Absyn_Tvar*_tmp162=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->tag=2U,_tmp163->f1=0,_tmp163->f2=expected_kind;_tmp163;}));struct Cyc_Absyn_Tvar*v=_tmp162;
({void*_tmp3B5=Cyc_Absyn_var_type(v);*ref=_tmp3B5;});
_tmp15D=v;goto _LL9;}else{
# 635
({struct Cyc_List_List*_tmp3B6=Cyc_Tctyp_add_free_evar(env.free_evars,t,put_in_effect);env.free_evars=_tmp3B6;});}}
# 637
goto _LL3;}case 2U: _LL8: _tmp15D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp139)->f1;_LL9: {struct Cyc_Absyn_Tvar*v=_tmp15D;
# 639
{void*_tmp164=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp13=_tmp164;void*_tmp165=_stmttmp13;struct Cyc_Core_Opt**_tmp166;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp165)->tag == 1U){_LL1D: _tmp166=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp165)->f1;_LL1E: {struct Cyc_Core_Opt**f=_tmp166;
({struct Cyc_Core_Opt*_tmp3B8=({struct Cyc_Core_Opt*_tmp168=_cycalloc(sizeof(*_tmp168));({void*_tmp3B7=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->tag=2U,_tmp167->f1=0,_tmp167->f2=expected_kind;_tmp167;});_tmp168->v=_tmp3B7;});_tmp168;});*f=_tmp3B8;});goto _LL1C;}}else{_LL1F: _LL20:
 goto _LL1C;}_LL1C:;}
# 645
({struct Cyc_List_List*_tmp3B9=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,v);env.kind_env=_tmp3B9;});
# 648
({struct Cyc_List_List*_tmp3BA=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,v,put_in_effect);env.free_vars=_tmp3BA;});
# 650
{void*_tmp169=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp14=_tmp169;void*_tmp16A=_stmttmp14;struct Cyc_Absyn_Kind*_tmp16C;struct Cyc_Core_Opt**_tmp16B;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16A)->tag == 2U){_LL22: _tmp16B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16A)->f1;_tmp16C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16A)->f2;_LL23: {struct Cyc_Core_Opt**f=_tmp16B;struct Cyc_Absyn_Kind*k=_tmp16C;
# 652
if(Cyc_Tcutil_kind_leq(expected_kind,k))
({struct Cyc_Core_Opt*_tmp3BC=({struct Cyc_Core_Opt*_tmp16E=_cycalloc(sizeof(*_tmp16E));({void*_tmp3BB=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->tag=2U,_tmp16D->f1=0,_tmp16D->f2=expected_kind;_tmp16D;});_tmp16E->v=_tmp3BB;});_tmp16E;});*f=_tmp3BC;});
goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 657
goto _LL3;}case 10U: _LLA: _tmp15B=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp139)->f1)->r;_tmp15C=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp139)->f2;_LLB: {void*td=_tmp15B;void***topt=_tmp15C;
# 662
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp16F=td;struct Cyc_Absyn_Datatypedecl*_tmp170;struct Cyc_Absyn_Enumdecl*_tmp171;struct Cyc_Absyn_Aggrdecl*_tmp172;switch(*((int*)_tmp16F)){case 0U: _LL27: _tmp172=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp16F)->f1;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp172;
# 665
if((env.te)->in_extern_c_include)
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL26;}case 1U: _LL29: _tmp171=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp16F)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp171;
# 669
if((env.te)->in_extern_c_include)
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL26;}default: _LL2B: _tmp170=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp16F)->f1;_LL2C: {struct Cyc_Absyn_Datatypedecl*dd=_tmp170;
# 673
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL26;}}_LL26:;}
# 675
({void**_tmp3BD=({void**_tmp173=_cycalloc(sizeof(*_tmp173));*_tmp173=new_typ;_tmp173;});*topt=_tmp3BD;});
return Cyc_Tctyp_i_check_valid_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3U: _LLC: _tmp155=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).elt_type;_tmp156=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).elt_tq;_tmp157=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).ptr_atts).rgn;_tmp158=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).ptr_atts).nullable;_tmp159=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).ptr_atts).bounds;_tmp15A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp139)->f1).ptr_atts).zero_term;_LLD: {void*t1=_tmp155;struct Cyc_Absyn_Tqual*tqp=_tmp156;void*rgn_type=_tmp157;void*nullable=_tmp158;void*b=_tmp159;void*zt=_tmp15A;
# 681
int is_zero_terminated;
# 683
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_tak,t1,1,1);
({int _tmp3BE=Cyc_Tcutil_extract_const_from_typedef(env.loc,tqp->print_const,t1);tqp->real_const=_tmp3BE;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp174=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp15=_tmp174;enum Cyc_Absyn_AliasQual _tmp175=_stmttmp15;switch(_tmp175){case Cyc_Absyn_Aliasable: _LL2E: _LL2F:
 k=& Cyc_Tcutil_rk;goto _LL2D;case Cyc_Absyn_Unique: _LL30: _LL31:
 k=& Cyc_Tcutil_urk;goto _LL2D;case Cyc_Absyn_Top: _LL32: _LL33:
 goto _LL35;default: _LL34: _LL35:
 k=& Cyc_Tcutil_trk;goto _LL2D;}_LL2D:;}
# 692
env=Cyc_Tctyp_i_check_valid_type(env,k,rgn_type,1,1);
# 695
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_boolk,zt,0,1);
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_boolk,nullable,0,1);
({void*_tmp3BF=zt;Cyc_Unify_unify(_tmp3BF,Cyc_Tcutil_is_char_type(t1)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
is_zero_terminated=Cyc_Absyn_type2bool(0,zt);
if(is_zero_terminated){
# 701
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp177=({struct Cyc_Warn_String_Warn_Warg_struct _tmp312;_tmp312.tag=0U,({struct _fat_ptr _tmp3C0=({const char*_tmp17B="cannot have a pointer to zero-terminated ";_tag_fat(_tmp17B,sizeof(char),42U);});_tmp312.f1=_tmp3C0;});_tmp312;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp178=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp311;_tmp311.tag=2U,_tmp311.f1=(void*)t1;_tmp311;});struct Cyc_Warn_String_Warn_Warg_struct _tmp179=({struct Cyc_Warn_String_Warn_Warg_struct _tmp310;_tmp310.tag=0U,({struct _fat_ptr _tmp3C1=({const char*_tmp17A=" elements";_tag_fat(_tmp17A,sizeof(char),10U);});_tmp310.f1=_tmp3C1;});_tmp310;});void*_tmp176[3U];_tmp176[0]=& _tmp177,_tmp176[1]=& _tmp178,_tmp176[2]=& _tmp179;({unsigned _tmp3C2=loc;Cyc_Warn_err2(_tmp3C2,_tag_fat(_tmp176,sizeof(void*),3U));});});}
# 705
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_ptrbk,b,0,allow_abs_aggr);{
struct Cyc_Absyn_Exp*_tmp17C=({void*_tmp3C3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp3C3,b);});struct Cyc_Absyn_Exp*e=_tmp17C;
if(e != 0){
struct _tuple12 _tmp17D=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp16=_tmp17D;struct _tuple12 _tmp17E=_stmttmp16;int _tmp180;unsigned _tmp17F;_LL37: _tmp17F=_tmp17E.f1;_tmp180=_tmp17E.f2;_LL38: {unsigned sz=_tmp17F;int known=_tmp180;
if(is_zero_terminated &&(!known || sz < (unsigned)1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp313;_tmp313.tag=0U,({struct _fat_ptr _tmp3C4=({const char*_tmp183="zero-terminated pointer cannot point to empty sequence";_tag_fat(_tmp183,sizeof(char),55U);});_tmp313.f1=_tmp3C4;});_tmp313;});void*_tmp181[1U];_tmp181[0]=& _tmp182;({unsigned _tmp3C5=loc;Cyc_Warn_err2(_tmp3C5,_tag_fat(_tmp181,sizeof(void*),1U));});});}}
# 712
goto _LL3;}}}case 9U: _LLE: _tmp154=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp139)->f1;_LLF: {struct Cyc_Absyn_Exp*e=_tmp154;
# 717
({struct Cyc_Tcenv_Tenv*_tmp3C6=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3C6,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp185=({struct Cyc_Warn_String_Warn_Warg_struct _tmp314;_tmp314.tag=0U,({struct _fat_ptr _tmp3C7=({const char*_tmp186="valueof_t requires an int expression";_tag_fat(_tmp186,sizeof(char),37U);});_tmp314.f1=_tmp3C7;});_tmp314;});void*_tmp184[1U];_tmp184[0]=& _tmp185;({unsigned _tmp3C8=loc;Cyc_Warn_err2(_tmp3C8,_tag_fat(_tmp184,sizeof(void*),1U));});});
env=Cyc_Tctyp_i_check_valid_type_level_exp(e,env);
goto _LL3;}case 11U: _LL10: _tmp153=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp139)->f1;_LL11: {struct Cyc_Absyn_Exp*e=_tmp153;
# 726
({struct Cyc_Tcenv_Tenv*_tmp3C9=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3C9,0,e);});
goto _LL3;}case 4U: _LL12: _tmp14E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp139)->f1).elt_type;_tmp14F=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp139)->f1).tq;_tmp150=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp139)->f1).num_elts;_tmp151=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp139)->f1).zero_term;_tmp152=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp139)->f1).zt_loc;_LL13: {void*t1=_tmp14E;struct Cyc_Absyn_Tqual*tqp=_tmp14F;struct Cyc_Absyn_Exp**eptr=_tmp150;void*zt=_tmp151;unsigned ztl=_tmp152;
# 731
struct Cyc_Absyn_Exp*_tmp187=*eptr;struct Cyc_Absyn_Exp*e=_tmp187;
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_tmk,t1,1,allow_abs_aggr);
({int _tmp3CA=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_tmp3CA;});{
# 735
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);
if(is_zero_terminated){
# 738
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp189=({struct Cyc_Warn_String_Warn_Warg_struct _tmp317;_tmp317.tag=0U,({struct _fat_ptr _tmp3CB=({const char*_tmp18D="cannot have a zero-terminated array of ";_tag_fat(_tmp18D,sizeof(char),40U);});_tmp317.f1=_tmp3CB;});_tmp317;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp18A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp316;_tmp316.tag=2U,_tmp316.f1=(void*)t1;_tmp316;});struct Cyc_Warn_String_Warn_Warg_struct _tmp18B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp315;_tmp315.tag=0U,({struct _fat_ptr _tmp3CC=({const char*_tmp18C=" elements";_tag_fat(_tmp18C,sizeof(char),10U);});_tmp315.f1=_tmp3CC;});_tmp315;});void*_tmp188[3U];_tmp188[0]=& _tmp189,_tmp188[1]=& _tmp18A,_tmp188[2]=& _tmp18B;({unsigned _tmp3CD=loc;Cyc_Warn_err2(_tmp3CD,_tag_fat(_tmp188,sizeof(void*),3U));});});}
# 743
if(e == 0){
# 746
if(!is_zero_terminated)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp318;_tmp318.tag=0U,({struct _fat_ptr _tmp3CE=({const char*_tmp190="array bound defaults to 1 here";_tag_fat(_tmp190,sizeof(char),31U);});_tmp318.f1=_tmp3CE;});_tmp318;});void*_tmp18E[1U];_tmp18E[0]=& _tmp18F;({unsigned _tmp3CF=loc;Cyc_Warn_warn2(_tmp3CF,_tag_fat(_tmp18E,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp3D0=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp3D0;});}
# 750
({struct Cyc_Tcenv_Tenv*_tmp3D1=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3D1,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp192=({struct Cyc_Warn_String_Warn_Warg_struct _tmp319;_tmp319.tag=0U,({struct _fat_ptr _tmp3D2=({const char*_tmp193="array bounds expression is not an unsigned int";_tag_fat(_tmp193,sizeof(char),47U);});_tmp319.f1=_tmp3D2;});_tmp319;});void*_tmp191[1U];_tmp191[0]=& _tmp192;({unsigned _tmp3D3=loc;Cyc_Warn_err2(_tmp3D3,_tag_fat(_tmp191,sizeof(void*),1U));});});
env=Cyc_Tctyp_i_check_valid_type_level_exp(e,env);{
struct _tuple12 _tmp194=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp17=_tmp194;struct _tuple12 _tmp195=_stmttmp17;int _tmp197;unsigned _tmp196;_LL3A: _tmp196=_tmp195.f1;_tmp197=_tmp195.f2;_LL3B: {unsigned sz=_tmp196;int known=_tmp197;
# 756
if((is_zero_terminated && known)&& sz < (unsigned)1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp199=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31A;_tmp31A.tag=0U,({struct _fat_ptr _tmp3D4=({const char*_tmp19A="zero terminated array cannot have zero elements";_tag_fat(_tmp19A,sizeof(char),48U);});_tmp31A.f1=_tmp3D4;});_tmp31A;});void*_tmp198[1U];_tmp198[0]=& _tmp199;({unsigned _tmp3D5=loc;Cyc_Warn_warn2(_tmp3D5,_tag_fat(_tmp198,sizeof(void*),1U));});});
# 759
if((known && sz < (unsigned)1)&& Cyc_Cyclone_tovc_r){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31B;_tmp31B.tag=0U,({struct _fat_ptr _tmp3D6=({const char*_tmp19D="0-element arrays are supported only with gcc -- changing to 1";_tag_fat(_tmp19D,sizeof(char),62U);});_tmp31B.f1=_tmp3D6;});_tmp31B;});void*_tmp19B[1U];_tmp19B[0]=& _tmp19C;({unsigned _tmp3D7=loc;Cyc_Warn_warn2(_tmp3D7,_tag_fat(_tmp19B,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp3D8=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp3D8;});}
# 763
goto _LL3;}}}}case 5U: _LL14: _tmp141=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).tvars;_tmp142=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).effect;_tmp143=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).ret_tqual;_tmp144=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).ret_type;_tmp145=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).args;_tmp146=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).c_varargs;_tmp147=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).cyc_varargs;_tmp148=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).rgn_po;_tmp149=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).attributes;_tmp14A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).requires_clause;_tmp14B=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).requires_relns;_tmp14C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).ensures_clause;_tmp14D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp139)->f1).ensures_relns;_LL15: {struct Cyc_List_List**btvs=_tmp141;void**eff=_tmp142;struct Cyc_Absyn_Tqual*rtq=_tmp143;void*tr=_tmp144;struct Cyc_List_List*args=_tmp145;int c_vararg=_tmp146;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp147;struct Cyc_List_List*rgn_po=_tmp148;struct Cyc_List_List*atts=_tmp149;struct Cyc_Absyn_Exp*req=_tmp14A;struct Cyc_List_List**req_relns=_tmp14B;struct Cyc_Absyn_Exp*ens=_tmp14C;struct Cyc_List_List**ens_relns=_tmp14D;
# 770
int num_convs=0;
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;atts != 0;atts=atts->tl){
if(!Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31D;_tmp31D.tag=0U,({struct _fat_ptr _tmp3D9=({const char*_tmp1A1="bad function type attribute ";_tag_fat(_tmp1A1,sizeof(char),29U);});_tmp31D.f1=_tmp3D9;});_tmp31D;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp1A0=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp31C;_tmp31C.tag=10U,_tmp31C.f1=(void*)atts->hd;_tmp31C;});void*_tmp19E[2U];_tmp19E[0]=& _tmp19F,_tmp19E[1]=& _tmp1A0;({unsigned _tmp3DA=loc;Cyc_Warn_err2(_tmp3DA,_tag_fat(_tmp19E,sizeof(void*),2U));});});{
void*_tmp1A2=(void*)atts->hd;void*_stmttmp18=_tmp1A2;void*_tmp1A3=_stmttmp18;int _tmp1A6;int _tmp1A5;enum Cyc_Absyn_Format_Type _tmp1A4;switch(*((int*)_tmp1A3)){case 1U: _LL3D: _LL3E:
 if(!is_stdcall){is_stdcall=1;++ num_convs;}goto _LL3C;case 2U: _LL3F: _LL40:
 if(!is_cdecl){is_cdecl=1;++ num_convs;}goto _LL3C;case 3U: _LL41: _LL42:
 if(!is_fastcall){is_fastcall=1;++ num_convs;}goto _LL3C;case 19U: _LL43: _tmp1A4=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f1;_tmp1A5=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f2;_tmp1A6=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f3;_LL44: {enum Cyc_Absyn_Format_Type fmttype=_tmp1A4;int i=_tmp1A5;int j=_tmp1A6;
# 786
++ num_formats;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;
goto _LL3C;}default: _LL45: _LL46:
 goto _LL3C;}_LL3C:;}}
# 794
if(num_convs > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31E;_tmp31E.tag=0U,({struct _fat_ptr _tmp3DB=({const char*_tmp1A9="function can't have multiple calling conventions";_tag_fat(_tmp1A9,sizeof(char),49U);});_tmp31E.f1=_tmp3DB;});_tmp31E;});void*_tmp1A7[1U];_tmp1A7[0]=& _tmp1A8;({unsigned _tmp3DC=loc;Cyc_Warn_err2(_tmp3DC,_tag_fat(_tmp1A7,sizeof(void*),1U));});});
if(num_formats > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp31F;_tmp31F.tag=0U,({struct _fat_ptr _tmp3DD=({const char*_tmp1AC="function can't have multiple format attributes";_tag_fat(_tmp1AC,sizeof(char),47U);});_tmp31F.f1=_tmp3DD;});_tmp31F;});void*_tmp1AA[1U];_tmp1AA[0]=& _tmp1AB;({unsigned _tmp3DE=loc;Cyc_Warn_err2(_tmp3DE,_tag_fat(_tmp1AA,sizeof(void*),1U));});});
# 801
Cyc_Tcutil_check_unique_tvars(loc,*btvs);
{struct Cyc_List_List*b=*btvs;for(0;b != 0;b=b->tl){
({int _tmp3DF=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmp3DF;});
({struct Cyc_List_List*_tmp3E0=Cyc_Tctyp_add_bound_tvar(env.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);env.kind_env=_tmp3E0;});{
void*_tmp1AD=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_stmttmp19=_tmp1AD;void*_tmp1AE=_stmttmp19;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1AE)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1AE)->f1)->kind == Cyc_Absyn_MemKind){_LL48: _LL49:
# 807
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp321;_tmp321.tag=0U,({struct _fat_ptr _tmp3E1=({const char*_tmp1B2="function abstracts Mem type variable ";_tag_fat(_tmp1B2,sizeof(char),38U);});_tmp321.f1=_tmp3E1;});_tmp321;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp320;_tmp320.tag=0U,_tmp320.f1=*((struct Cyc_Absyn_Tvar*)b->hd)->name;_tmp320;});void*_tmp1AF[2U];_tmp1AF[0]=& _tmp1B0,_tmp1AF[1]=& _tmp1B1;({unsigned _tmp3E2=loc;Cyc_Warn_err2(_tmp3E2,_tag_fat(_tmp1AF,sizeof(void*),2U));});});goto _LL47;}else{goto _LL4A;}}else{_LL4A: _LL4B:
 goto _LL47;}_LL47:;}}}{
# 813
struct Cyc_Tctyp_CVTEnv _tmp1B3=({struct Cyc_Tctyp_CVTEnv _tmp332;_tmp332.loc=env.loc,_tmp332.te=env.te,_tmp332.kind_env=env.kind_env,_tmp332.fn_result=1,_tmp332.generalize_evars=env.generalize_evars,_tmp332.free_vars=0,_tmp332.free_evars=0;_tmp332;});struct Cyc_Tctyp_CVTEnv new_env=_tmp1B3;
# 815
new_env=Cyc_Tctyp_i_check_valid_type(new_env,& Cyc_Tcutil_tmk,tr,1,1);
({int _tmp3E3=Cyc_Tcutil_extract_const_from_typedef(loc,rtq->print_const,tr);rtq->real_const=_tmp3E3;});
new_env.fn_result=0;
# 820
{struct Cyc_List_List*a=args;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp1B4=(struct _tuple9*)a->hd;struct _tuple9*trip=_tmp1B4;
void*_tmp1B5=(*trip).f3;void*t=_tmp1B5;
new_env=Cyc_Tctyp_i_check_valid_type(new_env,& Cyc_Tcutil_tmk,t,1,1);{
int _tmp1B6=Cyc_Tcutil_extract_const_from_typedef(loc,((*trip).f2).print_const,t);int ec=_tmp1B6;
((*trip).f2).real_const=ec;
# 828
if(Cyc_Tcutil_is_array_type(t)){
# 830
void*_tmp1B7=Cyc_Absyn_new_evar(0,0);void*ptr_rgn=_tmp1B7;
new_env=Cyc_Tctyp_i_check_valid_type(new_env,& Cyc_Tcutil_rk,ptr_rgn,1,1);
({void*_tmp3E4=Cyc_Tcutil_promote_array(t,ptr_rgn,0);(*trip).f3=_tmp3E4;});}}}}
# 836
if(cyc_vararg != 0){
if(c_vararg)({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp322;_tmp322.tag=0U,({struct _fat_ptr _tmp3E5=({const char*_tmp1BA="both c_vararg and cyc_vararg";_tag_fat(_tmp1BA,sizeof(char),29U);});_tmp322.f1=_tmp3E5;});_tmp322;});void*_tmp1B8[1U];_tmp1B8[0]=& _tmp1B9;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1B8,sizeof(void*),1U));});{
struct Cyc_Absyn_VarargInfo _tmp1BB=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1A=_tmp1BB;struct Cyc_Absyn_VarargInfo _tmp1BC=_stmttmp1A;int _tmp1BE;void*_tmp1BD;_LL4D: _tmp1BD=_tmp1BC.type;_tmp1BE=_tmp1BC.inject;_LL4E: {void*vt=_tmp1BD;int vi=_tmp1BE;
new_env=Cyc_Tctyp_i_check_valid_type(new_env,& Cyc_Tcutil_tmk,vt,1,1);
({int _tmp3E6=Cyc_Tcutil_extract_const_from_typedef(loc,(cyc_vararg->tq).print_const,vt);(cyc_vararg->tq).real_const=_tmp3E6;});
# 842
if(vi){
void*_tmp1BF=Cyc_Tcutil_compress(vt);void*_stmttmp1B=_tmp1BF;void*_tmp1C0=_stmttmp1B;void*_tmp1C3;void*_tmp1C2;void*_tmp1C1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C0)->tag == 3U){_LL50: _tmp1C1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C0)->f1).elt_type;_tmp1C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C0)->f1).ptr_atts).bounds;_tmp1C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C0)->f1).ptr_atts).zero_term;_LL51: {void*et=_tmp1C1;void*bs=_tmp1C2;void*zt=_tmp1C3;
# 845
{void*_tmp1C4=Cyc_Tcutil_compress(et);void*_stmttmp1C=_tmp1C4;void*_tmp1C5=_stmttmp1C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C5)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C5)->f1)->tag == 18U){_LL55: _LL56:
# 847
 if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp323;_tmp323.tag=0U,({struct _fat_ptr _tmp3E7=({const char*_tmp1C8="can't inject into a zeroterm pointer";_tag_fat(_tmp1C8,sizeof(char),37U);});_tmp323.f1=_tmp3E7;});_tmp323;});void*_tmp1C6[1U];_tmp1C6[0]=& _tmp1C7;({unsigned _tmp3E8=loc;Cyc_Warn_err2(_tmp3E8,_tag_fat(_tmp1C6,sizeof(void*),1U));});});
if(!({void*_tmp3E9=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp3E9,bs);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp324;_tmp324.tag=0U,({struct _fat_ptr _tmp3EA=({const char*_tmp1CB="can't inject into a fat pointer to datatype";_tag_fat(_tmp1CB,sizeof(char),44U);});_tmp324.f1=_tmp3EA;});_tmp324;});void*_tmp1C9[1U];_tmp1C9[0]=& _tmp1CA;({unsigned _tmp3EB=loc;Cyc_Warn_err2(_tmp3EB,_tag_fat(_tmp1C9,sizeof(void*),1U));});});
goto _LL54;}else{goto _LL57;}}else{_LL57: _LL58:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp325;_tmp325.tag=0U,({struct _fat_ptr _tmp3EC=({const char*_tmp1CE="can't inject a non-datatype type";_tag_fat(_tmp1CE,sizeof(char),33U);});_tmp325.f1=_tmp3EC;});_tmp325;});void*_tmp1CC[1U];_tmp1CC[0]=& _tmp1CD;({unsigned _tmp3ED=loc;Cyc_Warn_err2(_tmp3ED,_tag_fat(_tmp1CC,sizeof(void*),1U));});});goto _LL54;}_LL54:;}
# 854
goto _LL4F;}}else{_LL52: _LL53:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp326;_tmp326.tag=0U,({struct _fat_ptr _tmp3EE=({const char*_tmp1D1="expecting a datatype pointer type";_tag_fat(_tmp1D1,sizeof(char),34U);});_tmp326.f1=_tmp3EE;});_tmp326;});void*_tmp1CF[1U];_tmp1CF[0]=& _tmp1D0;({unsigned _tmp3EF=loc;Cyc_Warn_err2(_tmp3EF,_tag_fat(_tmp1CF,sizeof(void*),1U));});});goto _LL4F;}_LL4F:;}}}}
# 859
if(num_formats > 0){
int _tmp1D2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);int num_args=_tmp1D2;
if((((fmt_desc_arg < 0 || fmt_desc_arg > num_args)|| fmt_arg_start < 0)||
# 863
(cyc_vararg == 0 && !c_vararg)&& fmt_arg_start != 0)||
(cyc_vararg != 0 || c_vararg)&& fmt_arg_start != num_args + 1)
# 866
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp327;_tmp327.tag=0U,({struct _fat_ptr _tmp3F0=({const char*_tmp1D5="bad format descriptor";_tag_fat(_tmp1D5,sizeof(char),22U);});_tmp327.f1=_tmp3F0;});_tmp327;});void*_tmp1D3[1U];_tmp1D3[0]=& _tmp1D4;({unsigned _tmp3F1=loc;Cyc_Warn_err2(_tmp3F1,_tag_fat(_tmp1D3,sizeof(void*),1U));});});else{
# 869
struct _tuple9 _tmp1D6=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_desc_arg - 1);struct _tuple9 _stmttmp1D=_tmp1D6;struct _tuple9 _tmp1D7=_stmttmp1D;void*_tmp1D8;_LL5A: _tmp1D8=_tmp1D7.f3;_LL5B: {void*t=_tmp1D8;
# 871
{void*_tmp1D9=Cyc_Tcutil_compress(t);void*_stmttmp1E=_tmp1D9;void*_tmp1DA=_stmttmp1E;void*_tmp1DC;void*_tmp1DB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DA)->tag == 3U){_LL5D: _tmp1DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DA)->f1).elt_type;_tmp1DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DA)->f1).ptr_atts).bounds;_LL5E: {void*et=_tmp1DB;void*b=_tmp1DC;
# 874
if(!Cyc_Tcutil_is_char_type(et))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp328;_tmp328.tag=0U,({struct _fat_ptr _tmp3F2=({const char*_tmp1DF="format descriptor is not a string";_tag_fat(_tmp1DF,sizeof(char),34U);});_tmp328.f1=_tmp3F2;});_tmp328;});void*_tmp1DD[1U];_tmp1DD[0]=& _tmp1DE;({unsigned _tmp3F3=loc;Cyc_Warn_err2(_tmp3F3,_tag_fat(_tmp1DD,sizeof(void*),1U));});});else{
# 877
struct Cyc_Absyn_Exp*_tmp1E0=({void*_tmp3F4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp3F4,b);});struct Cyc_Absyn_Exp*e=_tmp1E0;
if(e == 0 && c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp329;_tmp329.tag=0U,({struct _fat_ptr _tmp3F5=({const char*_tmp1E3="format descriptor is not a char * type";_tag_fat(_tmp1E3,sizeof(char),39U);});_tmp329.f1=_tmp3F5;});_tmp329;});void*_tmp1E1[1U];_tmp1E1[0]=& _tmp1E2;({unsigned _tmp3F6=loc;Cyc_Warn_err2(_tmp3F6,_tag_fat(_tmp1E1,sizeof(void*),1U));});});else{
if(e != 0 && !c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32A;_tmp32A.tag=0U,({struct _fat_ptr _tmp3F7=({const char*_tmp1E6="format descriptor is not a char ? type";_tag_fat(_tmp1E6,sizeof(char),39U);});_tmp32A.f1=_tmp3F7;});_tmp32A;});void*_tmp1E4[1U];_tmp1E4[0]=& _tmp1E5;({unsigned _tmp3F8=loc;Cyc_Warn_err2(_tmp3F8,_tag_fat(_tmp1E4,sizeof(void*),1U));});});}}
# 883
goto _LL5C;}}else{_LL5F: _LL60:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32B;_tmp32B.tag=0U,({struct _fat_ptr _tmp3F9=({const char*_tmp1E9="format descriptor is not a string type";_tag_fat(_tmp1E9,sizeof(char),39U);});_tmp32B.f1=_tmp3F9;});_tmp32B;});void*_tmp1E7[1U];_tmp1E7[0]=& _tmp1E8;({unsigned _tmp3FA=loc;Cyc_Warn_err2(_tmp3FA,_tag_fat(_tmp1E7,sizeof(void*),1U));});});goto _LL5C;}_LL5C:;}
# 886
if(fmt_arg_start != 0 && !c_vararg){
# 890
int problem;
{struct _tuple16 _tmp1EA=({struct _tuple16 _tmp32C;_tmp32C.f1=ft,({void*_tmp3FB=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_vararg))->type));_tmp32C.f2=_tmp3FB;});_tmp32C;});struct _tuple16 _stmttmp1F=_tmp1EA;struct _tuple16 _tmp1EB=_stmttmp1F;struct Cyc_Absyn_Datatypedecl*_tmp1EC;struct Cyc_Absyn_Datatypedecl*_tmp1ED;switch(_tmp1EB.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->f1)->f1).KnownDatatype).tag == 2){_LL62: _tmp1ED=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->f1)->f1).KnownDatatype).val;_LL63: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1ED;
# 893
problem=({struct _tuple1*_tmp3FC=tud->name;Cyc_Absyn_qvar_cmp(_tmp3FC,Cyc_Absyn_datatype_print_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->f1)->f1).KnownDatatype).tag == 2){_LL64: _tmp1EC=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB.f2)->f1)->f1).KnownDatatype).val;_LL65: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1EC;
# 896
problem=({struct _tuple1*_tmp3FD=tud->name;Cyc_Absyn_qvar_cmp(_tmp3FD,Cyc_Absyn_datatype_scanf_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}default: _LL66: _LL67:
# 899
 problem=1;
goto _LL61;}_LL61:;}
# 902
if(problem)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32D;_tmp32D.tag=0U,({struct _fat_ptr _tmp3FE=({const char*_tmp1F0="format attribute and vararg types don't match";_tag_fat(_tmp1F0,sizeof(char),46U);});_tmp32D.f1=_tmp3FE;});_tmp32D;});void*_tmp1EE[1U];_tmp1EE[0]=& _tmp1EF;({unsigned _tmp3FF=loc;Cyc_Warn_err2(_tmp3FF,_tag_fat(_tmp1EE,sizeof(void*),1U));});});}}}}
# 910
{struct Cyc_List_List*rpo=rgn_po;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp1F1=(struct _tuple0*)rpo->hd;struct _tuple0*_stmttmp20=_tmp1F1;struct _tuple0*_tmp1F2=_stmttmp20;void*_tmp1F4;void*_tmp1F3;_LL69: _tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;_LL6A: {void*r1=_tmp1F3;void*r2=_tmp1F4;
new_env=Cyc_Tctyp_i_check_valid_type(new_env,& Cyc_Tcutil_ek,r1,1,1);
new_env=Cyc_Tctyp_i_check_valid_type(new_env,& Cyc_Tcutil_trk,r2,1,1);}}}{
# 918
struct Cyc_Tcenv_Fenv*_tmp1F5=Cyc_Tcenv_bogus_fenv(tr,args);struct Cyc_Tcenv_Fenv*fenv=_tmp1F5;
struct Cyc_Tcenv_Tenv*_tmp1F6=new_env.te;struct Cyc_Tcenv_Tenv*old_te=_tmp1F6;
({struct Cyc_Tcenv_Tenv*_tmp400=({struct Cyc_Tcenv_Tenv*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->ns=te->ns,_tmp1F7->ae=te->ae,_tmp1F7->le=fenv,_tmp1F7->allow_valueof=1,_tmp1F7->in_extern_c_include=te->in_extern_c_include,_tmp1F7->in_tempest=te->in_tempest,_tmp1F7->tempest_generalize=te->tempest_generalize,_tmp1F7->in_extern_c_inc_repeat=te->in_extern_c_inc_repeat;_tmp1F7;});new_env.te=_tmp400;});{
# 923
struct _tuple15 _tmp1F8=({struct Cyc_Tctyp_CVTEnv _tmp402=new_env;struct _fat_ptr _tmp401=({const char*_tmp231="@requires";_tag_fat(_tmp231,sizeof(char),10U);});Cyc_Tctyp_check_clause(_tmp402,_tmp401,req);});struct _tuple15 _stmttmp21=_tmp1F8;struct _tuple15 _tmp1F9=_stmttmp21;struct Cyc_List_List*_tmp1FB;struct Cyc_Tctyp_CVTEnv _tmp1FA;_LL6C: _tmp1FA=_tmp1F9.f1;_tmp1FB=_tmp1F9.f2;_LL6D: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1FA;struct Cyc_List_List*req_rs=_tmp1FB;
new_env=nenv;
*req_relns=req_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,req_rs);{
struct _tuple15 _tmp1FC=({struct Cyc_Tctyp_CVTEnv _tmp404=new_env;struct _fat_ptr _tmp403=({const char*_tmp230="@ensures";_tag_fat(_tmp230,sizeof(char),9U);});Cyc_Tctyp_check_clause(_tmp404,_tmp403,ens);});struct _tuple15 _stmttmp22=_tmp1FC;struct _tuple15 _tmp1FD=_stmttmp22;struct Cyc_List_List*_tmp1FF;struct Cyc_Tctyp_CVTEnv _tmp1FE;_LL6F: _tmp1FE=_tmp1FD.f1;_tmp1FF=_tmp1FD.f2;_LL70: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1FE;struct Cyc_List_List*ens_rs=_tmp1FF;
new_env=nenv;
*ens_relns=ens_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,ens_rs);
new_env.te=old_te;
# 933
if(*eff != 0)
new_env=Cyc_Tctyp_i_check_valid_type(new_env,& Cyc_Tcutil_ek,(void*)_check_null(*eff),1,1);else{
# 936
struct Cyc_List_List*effect=0;
# 941
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp200=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp23=_tmp200;struct _tuple14 _tmp201=_stmttmp23;int _tmp203;struct Cyc_Absyn_Tvar*_tmp202;_LL72: _tmp202=_tmp201.f1;_tmp203=_tmp201.f2;_LL73: {struct Cyc_Absyn_Tvar*tv=_tmp202;int put_in_eff=_tmp203;
if(!put_in_eff)continue;{
void*_tmp204=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp24=_tmp204;void*_tmp205=_stmttmp24;struct Cyc_Core_Opt**_tmp206;struct Cyc_Absyn_Kind*_tmp207;struct Cyc_Core_Opt**_tmp208;struct Cyc_Absyn_Kind*_tmp20A;struct Cyc_Core_Opt**_tmp209;switch(*((int*)_tmp205)){case 2U: _LL75: _tmp209=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp205)->f1;_tmp20A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp205)->f2;if((int)_tmp20A->kind == (int)Cyc_Absyn_RgnKind){_LL76: {struct Cyc_Core_Opt**f=_tmp209;struct Cyc_Absyn_Kind*r=_tmp20A;
# 947
if((int)r->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmp405=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp405;});_tmp207=r;goto _LL78;}
# 950
({struct Cyc_Core_Opt*_tmp406=Cyc_Tcutil_kind_to_bound_opt(r);*f=_tmp406;});_tmp207=r;goto _LL78;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp205)->f2)->kind){case Cyc_Absyn_BoolKind: _LL79: _LL7A:
# 953
 goto _LL7C;case Cyc_Absyn_PtrBndKind: _LL7B: _LL7C:
 goto _LL7E;case Cyc_Absyn_IntKind: _LL7D: _LL7E:
 goto _LL80;case Cyc_Absyn_EffKind: _LL85: _tmp208=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp205)->f1;_LL86: {struct Cyc_Core_Opt**f=_tmp208;
# 960
({struct Cyc_Core_Opt*_tmp407=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*f=_tmp407;});goto _LL88;}default: goto _LL8B;}}case 0U: _LL77: _tmp207=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp205)->f1;if((int)_tmp207->kind == (int)Cyc_Absyn_RgnKind){_LL78: {struct Cyc_Absyn_Kind*r=_tmp207;
# 952
effect=({struct Cyc_List_List*_tmp20B=_cycalloc(sizeof(*_tmp20B));({void*_tmp408=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(tv));_tmp20B->hd=_tmp408;}),_tmp20B->tl=effect;_tmp20B;});goto _LL74;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp205)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7F: _LL80:
# 956
 goto _LL82;case Cyc_Absyn_PtrBndKind: _LL81: _LL82:
 goto _LL84;case Cyc_Absyn_IntKind: _LL83: _LL84:
 goto _LL74;case Cyc_Absyn_EffKind: _LL87: _LL88:
# 962
 effect=({struct Cyc_List_List*_tmp20C=_cycalloc(sizeof(*_tmp20C));({void*_tmp409=Cyc_Absyn_var_type(tv);_tmp20C->hd=_tmp409;}),_tmp20C->tl=effect;_tmp20C;});goto _LL74;default: _LL8B: _LL8C:
# 967
 effect=({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*_tmp20F));({void*_tmp40A=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(tv));_tmp20F->hd=_tmp40A;}),_tmp20F->tl=effect;_tmp20F;});goto _LL74;}}default: _LL89: _tmp206=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp205)->f1;_LL8A: {struct Cyc_Core_Opt**f=_tmp206;
# 964
({struct Cyc_Core_Opt*_tmp40C=({struct Cyc_Core_Opt*_tmp20E=_cycalloc(sizeof(*_tmp20E));({void*_tmp40B=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->tag=2U,_tmp20D->f1=0,_tmp20D->f2=& Cyc_Tcutil_ak;_tmp20D;});_tmp20E->v=_tmp40B;});_tmp20E;});*f=_tmp40C;});goto _LL8C;}}_LL74:;}}}}
# 971
{struct Cyc_List_List*ts=new_env.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple13 _tmp210=*((struct _tuple13*)ts->hd);struct _tuple13 _stmttmp25=_tmp210;struct _tuple13 _tmp211=_stmttmp25;int _tmp213;void*_tmp212;_LL8E: _tmp212=_tmp211.f1;_tmp213=_tmp211.f2;_LL8F: {void*tv=_tmp212;int put_in_eff=_tmp213;
if(!put_in_eff)continue;{
struct Cyc_Absyn_Kind*_tmp214=Cyc_Tcutil_type_kind(tv);struct Cyc_Absyn_Kind*_stmttmp26=_tmp214;struct Cyc_Absyn_Kind*_tmp215=_stmttmp26;switch(((struct Cyc_Absyn_Kind*)_tmp215)->kind){case Cyc_Absyn_RgnKind: _LL91: _LL92:
# 976
 effect=({struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*_tmp216));({void*_tmp40D=Cyc_Absyn_access_eff(tv);_tmp216->hd=_tmp40D;}),_tmp216->tl=effect;_tmp216;});goto _LL90;case Cyc_Absyn_EffKind: _LL93: _LL94:
# 978
 effect=({struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->hd=tv,_tmp217->tl=effect;_tmp217;});goto _LL90;case Cyc_Absyn_IntKind: _LL95: _LL96:
 goto _LL90;default: _LL97: _LL98:
 effect=({struct Cyc_List_List*_tmp218=_cycalloc(sizeof(*_tmp218));({void*_tmp40E=Cyc_Absyn_regionsof_eff(tv);_tmp218->hd=_tmp40E;}),_tmp218->tl=effect;_tmp218;});goto _LL90;}_LL90:;}}}}
# 983
({void*_tmp40F=Cyc_Absyn_join_eff(effect);*eff=_tmp40F;});}
# 989
if(*btvs != 0){
struct Cyc_List_List*bs=*btvs;for(0;bs != 0;bs=bs->tl){
void*_tmp219=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_stmttmp27=_tmp219;void*_tmp21A=_stmttmp27;struct Cyc_Absyn_Kind*_tmp21C;struct Cyc_Core_Opt**_tmp21B;struct Cyc_Core_Opt**_tmp21D;switch(*((int*)_tmp21A)){case 1U: _LL9A: _tmp21D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp21A)->f1;_LL9B: {struct Cyc_Core_Opt**f=_tmp21D;
# 993
({struct Cyc_Warn_String_Warn_Warg_struct _tmp21F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp330;_tmp330.tag=0U,({struct _fat_ptr _tmp410=({const char*_tmp223="type variable ";_tag_fat(_tmp223,sizeof(char),15U);});_tmp330.f1=_tmp410;});_tmp330;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp220=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp32F;_tmp32F.tag=7U,_tmp32F.f1=(struct Cyc_Absyn_Tvar*)bs->hd;_tmp32F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp221=({struct Cyc_Warn_String_Warn_Warg_struct _tmp32E;_tmp32E.tag=0U,({struct _fat_ptr _tmp411=({const char*_tmp222=" unconstrained, assuming boxed";_tag_fat(_tmp222,sizeof(char),31U);});_tmp32E.f1=_tmp411;});_tmp32E;});void*_tmp21E[3U];_tmp21E[0]=& _tmp21F,_tmp21E[1]=& _tmp220,_tmp21E[2]=& _tmp221;({unsigned _tmp412=loc;Cyc_Warn_warn2(_tmp412,_tag_fat(_tmp21E,sizeof(void*),3U));});});
({struct Cyc_Core_Opt*_tmp413=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp413;});
goto _LL99;}case 2U: _LL9C: _tmp21B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21A)->f1;_tmp21C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp21A)->f2;_LL9D: {struct Cyc_Core_Opt**f=_tmp21B;struct Cyc_Absyn_Kind*k=_tmp21C;
# 997
{struct Cyc_Absyn_Kind*_tmp224=k;switch(((struct Cyc_Absyn_Kind*)_tmp224)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp224)->aliasqual == Cyc_Absyn_Top){_LLA3: _LLA4:
 goto _LLA6;}else{goto _LLB3;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)_tmp224)->aliasqual){case Cyc_Absyn_Top: _LLA5: _LLA6:
 goto _LLA8;case Cyc_Absyn_Aliasable: _LLA7: _LLA8:
 goto _LLAA;case Cyc_Absyn_Unique: _LLAD: _LLAE:
# 1003
 goto _LLB0;default: goto _LLB3;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)_tmp224)->aliasqual){case Cyc_Absyn_Top: _LLA9: _LLAA:
# 1001
 goto _LLAC;case Cyc_Absyn_Aliasable: _LLAB: _LLAC:
({struct Cyc_Core_Opt*_tmp414=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp414;});goto _LLA2;case Cyc_Absyn_Unique: _LLAF: _LLB0:
# 1004
({struct Cyc_Core_Opt*_tmp415=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp415;});goto _LLA2;default: goto _LLB3;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp224)->aliasqual == Cyc_Absyn_Top){_LLB1: _LLB2:
({struct Cyc_Core_Opt*_tmp416=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp416;});goto _LLA2;}else{goto _LLB3;}default: _LLB3: _LLB4:
({struct Cyc_Core_Opt*_tmp417=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp417;});goto _LLA2;}_LLA2:;}
# 1008
goto _LL99;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp21A)->f1)->kind == Cyc_Absyn_MemKind){_LL9E: _LL9F:
# 1010
({struct Cyc_Warn_String_Warn_Warg_struct _tmp226=({struct Cyc_Warn_String_Warn_Warg_struct _tmp331;_tmp331.tag=0U,({struct _fat_ptr _tmp418=({const char*_tmp227="functions can't abstract M types";_tag_fat(_tmp227,sizeof(char),33U);});_tmp331.f1=_tmp418;});_tmp331;});void*_tmp225[1U];_tmp225[0]=& _tmp226;({unsigned _tmp419=loc;Cyc_Warn_err2(_tmp419,_tag_fat(_tmp225,sizeof(void*),1U));});});goto _LL99;}else{_LLA0: _LLA1:
 goto _LL99;}}_LL99:;}}
# 1015
({struct Cyc_List_List*_tmp41A=Cyc_Tctyp_remove_bound_tvars(new_env.kind_env,*btvs);env.kind_env=_tmp41A;});
({struct Cyc_List_List*_tmp41B=Cyc_Tctyp_remove_bound_tvars_bool(new_env.free_vars,*btvs);new_env.free_vars=_tmp41B;});
# 1018
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp228=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp28=_tmp228;struct _tuple14 _tmp229=_stmttmp28;int _tmp22B;struct Cyc_Absyn_Tvar*_tmp22A;_LLB6: _tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_LLB7: {struct Cyc_Absyn_Tvar*t=_tmp22A;int b=_tmp22B;
({struct Cyc_List_List*_tmp41C=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,t,b);env.free_vars=_tmp41C;});}}}
# 1023
{struct Cyc_List_List*evs=new_env.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp22C=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp29=_tmp22C;struct _tuple13 _tmp22D=_stmttmp29;int _tmp22F;void*_tmp22E;_LLB9: _tmp22E=_tmp22D.f1;_tmp22F=_tmp22D.f2;_LLBA: {void*e=_tmp22E;int b=_tmp22F;
({struct Cyc_List_List*_tmp41D=Cyc_Tctyp_add_free_evar(env.free_evars,e,b);env.free_evars=_tmp41D;});}}}
# 1027
goto _LL3;}}}}}}}case 6U: _LL16: _tmp140=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp139)->f1;_LL17: {struct Cyc_List_List*tq_ts=_tmp140;
# 1030
for(0;tq_ts != 0;tq_ts=tq_ts->tl){
struct _tuple17*_tmp232=(struct _tuple17*)tq_ts->hd;struct _tuple17*p=_tmp232;
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_tmk,(*p).f2,1,0);
({int _tmp41E=
Cyc_Tcutil_extract_const_from_typedef(loc,((*p).f1).print_const,(*p).f2);
# 1033
((*p).f1).real_const=_tmp41E;});}
# 1036
goto _LL3;}case 7U: _LL18: _tmp13E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp139)->f1;_tmp13F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp139)->f2;_LL19: {enum Cyc_Absyn_AggrKind k=_tmp13E;struct Cyc_List_List*fs=_tmp13F;
# 1040
struct Cyc_List_List*prev_fields=0;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp233=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp2A=_tmp233;struct Cyc_Absyn_Aggrfield*_tmp234=_stmttmp2A;struct Cyc_Absyn_Exp*_tmp23A;struct Cyc_List_List*_tmp239;struct Cyc_Absyn_Exp*_tmp238;void*_tmp237;struct Cyc_Absyn_Tqual*_tmp236;struct _fat_ptr*_tmp235;_LLBC: _tmp235=_tmp234->name;_tmp236=(struct Cyc_Absyn_Tqual*)& _tmp234->tq;_tmp237=_tmp234->type;_tmp238=_tmp234->width;_tmp239=_tmp234->attributes;_tmp23A=_tmp234->requires_clause;_LLBD: {struct _fat_ptr*fn=_tmp235;struct Cyc_Absyn_Tqual*tqp=_tmp236;void*t=_tmp237;struct Cyc_Absyn_Exp*width=_tmp238;struct Cyc_List_List*atts=_tmp239;struct Cyc_Absyn_Exp*requires_clause=_tmp23A;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp334;_tmp334.tag=0U,({struct _fat_ptr _tmp41F=({const char*_tmp23E="duplicate field ";_tag_fat(_tmp23E,sizeof(char),17U);});_tmp334.f1=_tmp41F;});_tmp334;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp333;_tmp333.tag=0U,_tmp333.f1=*fn;_tmp333;});void*_tmp23B[2U];_tmp23B[0]=& _tmp23C,_tmp23B[1]=& _tmp23D;({unsigned _tmp420=loc;Cyc_Warn_err2(_tmp420,_tag_fat(_tmp23B,sizeof(void*),2U));});});
if(({struct _fat_ptr _tmp421=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp421,({const char*_tmp23F="";_tag_fat(_tmp23F,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240->hd=fn,_tmp240->tl=prev_fields;_tmp240;});
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_tmk,t,1,0);
({int _tmp422=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t);tqp->real_const=_tmp422;});
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Tctyp_check_field_atts(loc,fn,atts);
if(requires_clause != 0){
# 1053
if((int)k != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp242=({struct Cyc_Warn_String_Warn_Warg_struct _tmp335;_tmp335.tag=0U,({struct _fat_ptr _tmp423=({const char*_tmp243="@requires clause is only allowed on union members";_tag_fat(_tmp243,sizeof(char),50U);});_tmp335.f1=_tmp423;});_tmp335;});void*_tmp241[1U];_tmp241[0]=& _tmp242;({unsigned _tmp424=loc;Cyc_Warn_err2(_tmp424,_tag_fat(_tmp241,sizeof(void*),1U));});});
({struct Cyc_Tcenv_Tenv*_tmp425=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp425,0,requires_clause);});
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp245=({struct Cyc_Warn_String_Warn_Warg_struct _tmp338;_tmp338.tag=0U,({struct _fat_ptr _tmp426=({const char*_tmp249="@requires clause has type ";_tag_fat(_tmp249,sizeof(char),27U);});_tmp338.f1=_tmp426;});_tmp338;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp246=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp337;_tmp337.tag=3U,_tmp337.f1=(void*)requires_clause->topt;_tmp337;});struct Cyc_Warn_String_Warn_Warg_struct _tmp247=({struct Cyc_Warn_String_Warn_Warg_struct _tmp336;_tmp336.tag=0U,({
struct _fat_ptr _tmp427=({const char*_tmp248=" instead of integral type";_tag_fat(_tmp248,sizeof(char),26U);});_tmp336.f1=_tmp427;});_tmp336;});void*_tmp244[3U];_tmp244[0]=& _tmp245,_tmp244[1]=& _tmp246,_tmp244[2]=& _tmp247;({unsigned _tmp428=loc;Cyc_Warn_err2(_tmp428,_tag_fat(_tmp244,sizeof(void*),3U));});});
env=Cyc_Tctyp_i_check_valid_type_level_exp(requires_clause,env);}}}
# 1062
goto _LL3;}default: _LL1A: _tmp13A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp139)->f1;_tmp13B=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp139)->f2;_tmp13C=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp139)->f3;_tmp13D=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp139)->f4;_LL1B: {struct _tuple1*tdn=_tmp13A;struct Cyc_List_List**targs_ref=_tmp13B;struct Cyc_Absyn_Typedefdecl**tdopt=_tmp13C;void**toptp=_tmp13D;
# 1066
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp24A;_push_handler(& _tmp24A);{int _tmp24C=0;if(setjmp(_tmp24A.handler))_tmp24C=1;if(!_tmp24C){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);;_pop_handler();}else{void*_tmp24B=(void*)Cyc_Core_get_exn_thrown();void*_tmp24D=_tmp24B;void*_tmp24E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp24D)->tag == Cyc_Dict_Absent){_LLBF: _LLC0:
# 1069
({struct Cyc_Warn_String_Warn_Warg_struct _tmp250=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33A;_tmp33A.tag=0U,({struct _fat_ptr _tmp429=({const char*_tmp252="unbound typedef name ";_tag_fat(_tmp252,sizeof(char),22U);});_tmp33A.f1=_tmp429;});_tmp33A;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp251=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp339;_tmp339.tag=1U,_tmp339.f1=tdn;_tmp339;});void*_tmp24F[2U];_tmp24F[0]=& _tmp250,_tmp24F[1]=& _tmp251;({unsigned _tmp42A=loc;Cyc_Warn_err2(_tmp42A,_tag_fat(_tmp24F,sizeof(void*),2U));});});
return env;}else{_LLC1: _tmp24E=_tmp24D;_LLC2: {void*exn=_tmp24E;(int)_rethrow(exn);}}_LLBE:;}}}
# 1072
*tdopt=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=*targs_ref;
struct Cyc_List_List*inst=0;
# 1077
for(0;ts != 0 && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
env=Cyc_Tctyp_i_check_valid_type(env,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp254=_cycalloc(sizeof(*_tmp254));({struct _tuple18*_tmp42B=({struct _tuple18*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp253->f2=(void*)ts->hd;_tmp253;});_tmp254->hd=_tmp42B;}),_tmp254->tl=inst;_tmp254;});}
# 1083
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp256=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33C;_tmp33C.tag=0U,({struct _fat_ptr _tmp42C=({const char*_tmp258="too many parameters for typedef ";_tag_fat(_tmp258,sizeof(char),33U);});_tmp33C.f1=_tmp42C;});_tmp33C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp257=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp33B;_tmp33B.tag=1U,_tmp33B.f1=tdn;_tmp33B;});void*_tmp255[2U];_tmp255[0]=& _tmp256,_tmp255[1]=& _tmp257;({unsigned _tmp42D=loc;Cyc_Warn_err2(_tmp42D,_tag_fat(_tmp255,sizeof(void*),2U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 1088
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->hd=e,_tmp259->tl=hidden_ts;_tmp259;});
env=Cyc_Tctyp_i_check_valid_type(env,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp25B=_cycalloc(sizeof(*_tmp25B));({struct _tuple18*_tmp42E=({struct _tuple18*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp25A->f2=e;_tmp25A;});_tmp25B->hd=_tmp42E;}),_tmp25B->tl=inst;_tmp25B;});}
# 1096
({struct Cyc_List_List*_tmp430=({struct Cyc_List_List*_tmp42F=*targs_ref;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp42F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs_ref=_tmp430;});}
# 1098
if(td->defn != 0)
({void*_tmp431=Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));*toptp=_tmp431;});
goto _LL3;}}}_LL3:;}
# 1102
if(!({struct Cyc_Absyn_Kind*_tmp432=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmp432,expected_kind);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp342;_tmp342.tag=0U,({struct _fat_ptr _tmp433=({const char*_tmp265="type ";_tag_fat(_tmp265,sizeof(char),6U);});_tmp342.f1=_tmp433;});_tmp342;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp25E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp341;_tmp341.tag=2U,_tmp341.f1=(void*)t;_tmp341;});struct Cyc_Warn_String_Warn_Warg_struct _tmp25F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp340;_tmp340.tag=0U,({struct _fat_ptr _tmp434=({const char*_tmp264=" has kind ";_tag_fat(_tmp264,sizeof(char),11U);});_tmp340.f1=_tmp434;});_tmp340;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp260=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp33F;_tmp33F.tag=9U,({struct Cyc_Absyn_Kind*_tmp435=Cyc_Tcutil_type_kind(t);_tmp33F.f1=_tmp435;});_tmp33F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp261=({struct Cyc_Warn_String_Warn_Warg_struct _tmp33E;_tmp33E.tag=0U,({
struct _fat_ptr _tmp436=({const char*_tmp263=" but as used here needs kind ";_tag_fat(_tmp263,sizeof(char),30U);});_tmp33E.f1=_tmp436;});_tmp33E;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp262=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp33D;_tmp33D.tag=9U,_tmp33D.f1=expected_kind;_tmp33D;});void*_tmp25C[6U];_tmp25C[0]=& _tmp25D,_tmp25C[1]=& _tmp25E,_tmp25C[2]=& _tmp25F,_tmp25C[3]=& _tmp260,_tmp25C[4]=& _tmp261,_tmp25C[5]=& _tmp262;({unsigned _tmp437=loc;Cyc_Warn_err2(_tmp437,_tag_fat(_tmp25C,sizeof(void*),6U));});});
# 1106
return env;}}
# 1114
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tctyp_CVTEnv env){
env.loc=e->loc;
{void*_tmp266=e->r;void*_stmttmp2B=_tmp266;void*_tmp267=_stmttmp2B;struct Cyc_Absyn_Exp*_tmp268;struct Cyc_Absyn_Exp*_tmp269;void*_tmp26A;void*_tmp26B;void*_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;void*_tmp26D;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_List_List*_tmp278;switch(*((int*)_tmp267)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp278=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp267)->f2;_LLC: {struct Cyc_List_List*es=_tmp278;
# 1123
for(0;es != 0;es=es->tl){
env=Cyc_Tctyp_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)es->hd,env);}
goto _LL0;}case 6U: _LLD: _tmp275=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_tmp276=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp267)->f2;_tmp277=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp267)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp275;struct Cyc_Absyn_Exp*e2=_tmp276;struct Cyc_Absyn_Exp*e3=_tmp277;
# 1127
env=Cyc_Tctyp_i_check_valid_type_level_exp(e1,env);
_tmp273=e2;_tmp274=e3;goto _LL10;}case 7U: _LLF: _tmp273=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_tmp274=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp267)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp273;struct Cyc_Absyn_Exp*e2=_tmp274;
_tmp271=e1;_tmp272=e2;goto _LL12;}case 8U: _LL11: _tmp271=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_tmp272=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp267)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp271;struct Cyc_Absyn_Exp*e2=_tmp272;
_tmp26F=e1;_tmp270=e2;goto _LL14;}case 9U: _LL13: _tmp26F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_tmp270=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp267)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp26F;struct Cyc_Absyn_Exp*e2=_tmp270;
# 1132
env=Cyc_Tctyp_i_check_valid_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_valid_type_level_exp(e2,env);
goto _LL0;}case 14U: _LL15: _tmp26D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_tmp26E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp267)->f2;_LL16: {void*t=_tmp26D;struct Cyc_Absyn_Exp*e1=_tmp26E;
# 1136
env=Cyc_Tctyp_i_check_valid_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_tak,t,1,0);
goto _LL0;}case 19U: _LL17: _tmp26C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_LL18: {void*t=_tmp26C;
_tmp26B=t;goto _LL1A;}case 17U: _LL19: _tmp26B=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_LL1A: {void*t=_tmp26B;
# 1141
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_tak,t,1,0);goto _LL0;}case 39U: _LL1B: _tmp26A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_LL1C: {void*t=_tmp26A;
# 1143
env=Cyc_Tctyp_i_check_valid_type(env,& Cyc_Tcutil_ik,t,1,0);goto _LL0;}case 18U: _LL1D: _tmp269=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp269;
_tmp268=e;goto _LL20;}case 41U: _LL1F: _tmp268=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp267)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp268;
# 1146
env=Cyc_Tctyp_i_check_valid_type_level_exp(e,env);goto _LL0;}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp343;_tmp343.tag=0U,({struct _fat_ptr _tmp438=({const char*_tmp27B="bad expression in Tctyp::i_check_valid_type_level_exp";_tag_fat(_tmp27B,sizeof(char),54U);});_tmp343.f1=_tmp438;});_tmp343;});void*_tmp279[1U];_tmp279[0]=& _tmp27A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp279,sizeof(void*),1U));});}_LL0:;}
# 1149
return env;}
# 1152
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1154
struct Cyc_List_List*_tmp27C=cvt.kind_env;struct Cyc_List_List*kind_env=_tmp27C;
cvt=Cyc_Tctyp_i_check_valid_type(cvt,expected_kind,t,1,allow_abs_aggr);
# 1157
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
({struct Cyc_List_List*_tmp439=Cyc_Tctyp_fast_add_free_tvar(kind_env,(*((struct _tuple14*)vs->hd)).f1);cvt.kind_env=_tmp439;});}}
# 1163
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp27D=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp2C=_tmp27D;struct _tuple13 _tmp27E=_stmttmp2C;void*_tmp27F;_LL1: _tmp27F=_tmp27E.f1;_LL2: {void*e=_tmp27F;
void*_tmp280=Cyc_Tcutil_compress(e);void*_stmttmp2D=_tmp280;void*_tmp281=_stmttmp2D;struct Cyc_Core_Opt**_tmp282;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp281)->tag == 1U){_LL4: _tmp282=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp281)->f4;_LL5: {struct Cyc_Core_Opt**s=_tmp282;
# 1167
if(*s == 0)
({struct Cyc_Core_Opt*_tmp43A=({struct Cyc_Core_Opt*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->v=kind_env;_tmp283;});*s=_tmp43A;});else{
# 1171
struct Cyc_List_List*_tmp284=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*s))->v;struct Cyc_List_List*tvs=_tmp284;
struct Cyc_List_List*_tmp285=0;struct Cyc_List_List*result=_tmp285;
for(0;tvs != 0;tvs=tvs->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)tvs->hd))
result=({struct Cyc_List_List*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp286->tl=result;_tmp286;});}
({struct Cyc_Core_Opt*_tmp43B=({struct Cyc_Core_Opt*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287->v=result;_tmp287;});*s=_tmp43B;});}
# 1178
goto _LL3;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}}}
# 1182
return cvt;}
# 1191
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=
Cyc_Tcutil_is_function_type(t)&&(!te->in_tempest || te->tempest_generalize);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_List_List*_tmp288=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*te_vars=_tmp288;
struct Cyc_Tctyp_CVTEnv _tmp289=({struct Cyc_Tctyp_CVTEnv _tmp43D=({struct Cyc_Tctyp_CVTEnv _tmp34C;_tmp34C.loc=loc,_tmp34C.te=te,_tmp34C.kind_env=te_vars,_tmp34C.fn_result=0,_tmp34C.generalize_evars=generalize_evars,_tmp34C.free_vars=0,_tmp34C.free_evars=0;_tmp34C;});struct Cyc_Absyn_Kind*_tmp43C=expected_kind;Cyc_Tctyp_check_valid_type(_tmp43D,_tmp43C,1,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp289;
# 1199
struct Cyc_List_List*_tmp28A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst,cvt.free_vars);struct Cyc_List_List*free_tvars=_tmp28A;
struct Cyc_List_List*_tmp28B=cvt.free_evars;struct Cyc_List_List*free_evars=_tmp28B;
# 1202
if(te_vars != 0){
struct Cyc_List_List*_tmp28C=0;struct Cyc_List_List*res=_tmp28C;
{struct Cyc_List_List*_tmp28D=free_tvars;struct Cyc_List_List*fs=_tmp28D;for(0;(unsigned)fs;fs=fs->tl){
struct Cyc_List_List*_tmp28E=te_vars;struct Cyc_List_List*ts=_tmp28E;
for(0;ts != 0;ts=ts->tl){
if(Cyc_Absyn_tvar_cmp((struct Cyc_Absyn_Tvar*)fs->hd,(struct Cyc_Absyn_Tvar*)ts->hd)== 0)
break;}
if(ts == 0)
res=({struct Cyc_List_List*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F->hd=(struct Cyc_Absyn_Tvar*)fs->hd,_tmp28F->tl=res;_tmp28F;});}}
# 1212
free_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);}
# 1217
{struct Cyc_List_List*x=free_tvars;for(0;x != 0;x=x->tl){
void*_tmp290=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_stmttmp2E=_tmp290;void*_tmp291=_stmttmp2E;enum Cyc_Absyn_AliasQual _tmp292;struct Cyc_Absyn_Kind*_tmp294;struct Cyc_Core_Opt**_tmp293;struct Cyc_Core_Opt**_tmp295;switch(*((int*)_tmp291)){case 1U: _LL1: _tmp295=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp291)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp295;
({struct Cyc_Core_Opt*_tmp43E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp43E;});goto _LL0;}case 2U: _LL3: _tmp293=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f1;_tmp294=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp291)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmp293;struct Cyc_Absyn_Kind*k=_tmp294;
# 1221
{struct Cyc_Absyn_Kind*_tmp296=k;switch(((struct Cyc_Absyn_Kind*)_tmp296)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp296)->aliasqual == Cyc_Absyn_Top){_LLA: _LLB:
 goto _LLD;}else{goto _LL14;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)_tmp296)->aliasqual){case Cyc_Absyn_Top: _LLC: _LLD:
 goto _LLF;case Cyc_Absyn_Aliasable: _LLE: _LLF:
({struct Cyc_Core_Opt*_tmp43F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp43F;});goto _LL9;case Cyc_Absyn_Unique: _LL10: _LL11:
({struct Cyc_Core_Opt*_tmp440=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp440;});goto _LL9;default: goto _LL14;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp296)->aliasqual == Cyc_Absyn_Top){_LL12: _LL13:
({struct Cyc_Core_Opt*_tmp441=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp441;});goto _LL9;}else{goto _LL14;}default: _LL14: _LL15:
({struct Cyc_Core_Opt*_tmp442=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp442;});goto _LL9;}_LL9:;}
# 1229
goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp291)->f1)->kind == Cyc_Absyn_MemKind){_LL5: _tmp292=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp291)->f1)->aliasqual;_LL6: {enum Cyc_Absyn_AliasQual a=_tmp292;
# 1231
({struct Cyc_Warn_String_Warn_Warg_struct _tmp298=({struct Cyc_Warn_String_Warn_Warg_struct _tmp347;_tmp347.tag=0U,({struct _fat_ptr _tmp443=({const char*_tmp29E="type variable ";_tag_fat(_tmp29E,sizeof(char),15U);});_tmp347.f1=_tmp443;});_tmp347;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp299=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp346;_tmp346.tag=7U,_tmp346.f1=(struct Cyc_Absyn_Tvar*)x->hd;_tmp346;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp345;_tmp345.tag=0U,({struct _fat_ptr _tmp444=({const char*_tmp29D=" cannot have kind ";_tag_fat(_tmp29D,sizeof(char),19U);});_tmp345.f1=_tmp444;});_tmp345;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp29B=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp344;_tmp344.tag=9U,({struct Cyc_Absyn_Kind*_tmp445=({struct Cyc_Absyn_Kind*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->kind=Cyc_Absyn_MemKind,_tmp29C->aliasqual=a;_tmp29C;});_tmp344.f1=_tmp445;});_tmp344;});void*_tmp297[4U];_tmp297[0]=& _tmp298,_tmp297[1]=& _tmp299,_tmp297[2]=& _tmp29A,_tmp297[3]=& _tmp29B;({unsigned _tmp446=loc;Cyc_Warn_err2(_tmp446,_tag_fat(_tmp297,sizeof(void*),4U));});});
goto _LL0;}}else{_LL7: _LL8:
 goto _LL0;}}_LL0:;}}
# 1237
if(free_tvars != 0 || free_evars != 0){
{void*_tmp29F=Cyc_Tcutil_compress(t);void*_stmttmp2F=_tmp29F;void*_tmp2A0=_stmttmp2F;struct Cyc_List_List**_tmp2A1;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2A0)->tag == 5U){_LL17: _tmp2A1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2A0)->f1).tvars;_LL18: {struct Cyc_List_List**btvs=_tmp2A1;
# 1240
if(*btvs == 0){
# 1242
({struct Cyc_List_List*_tmp447=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(free_tvars);*btvs=_tmp447;});
free_tvars=0;}
# 1245
goto _LL16;}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 1248
if(free_tvars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp34B;_tmp34B.tag=0U,({struct _fat_ptr _tmp448=({const char*_tmp2A8="unbound type variable ";_tag_fat(_tmp2A8,sizeof(char),23U);});_tmp34B.f1=_tmp448;});_tmp34B;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2A4=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp34A;_tmp34A.tag=7U,_tmp34A.f1=(struct Cyc_Absyn_Tvar*)free_tvars->hd;_tmp34A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp349;_tmp349.tag=0U,({struct _fat_ptr _tmp449=({const char*_tmp2A7=" when checking ";_tag_fat(_tmp2A7,sizeof(char),16U);});_tmp349.f1=_tmp449;});_tmp349;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp348;_tmp348.tag=2U,_tmp348.f1=(void*)t;_tmp348;});void*_tmp2A2[4U];_tmp2A2[0]=& _tmp2A3,_tmp2A2[1]=& _tmp2A4,_tmp2A2[2]=& _tmp2A5,_tmp2A2[3]=& _tmp2A6;({unsigned _tmp44A=loc;Cyc_Warn_err2(_tmp44A,_tag_fat(_tmp2A2,sizeof(void*),4U));});});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}}
# 1259
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1262
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_tmp2A9=Cyc_Tcutil_compress(t);void*_stmttmp30=_tmp2A9;void*_tmp2AA=_stmttmp30;struct Cyc_Absyn_FnInfo _tmp2AB;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2AA)->tag == 5U){_LL1: _tmp2AB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2AA)->f1;_LL2: {struct Cyc_Absyn_FnInfo i=_tmp2AB;
# 1265
struct Cyc_List_List*_tmp2AC=(fd->i).attributes;struct Cyc_List_List*atts=_tmp2AC;
fd->i=i;
(fd->i).attributes=atts;
({int _tmp44B=
Cyc_Tcutil_extract_const_from_typedef(loc,(i.ret_tqual).print_const,i.ret_type);
# 1268
((fd->i).ret_tqual).real_const=_tmp44B;});
# 1270
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp34D;_tmp34D.tag=0U,({struct _fat_ptr _tmp44C=({const char*_tmp2AF="check_fndecl_valid_type: not a FnType";_tag_fat(_tmp2AF,sizeof(char),38U);});_tmp34D.f1=_tmp44C;});_tmp34D;});void*_tmp2AD[1U];_tmp2AD[0]=& _tmp2AE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2AD,sizeof(void*),1U));});}_LL0:;}
# 1273
({struct Cyc_List_List*_tmp44E=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)((struct _fat_ptr*(*)(struct _tuple9*t))Cyc_Tctyp_fst_fdarg,(fd->i).args);unsigned _tmp44D=loc;Cyc_Tcutil_check_unique_vars(_tmp44E,_tmp44D,({const char*_tmp2B0="function declaration has repeated parameter";_tag_fat(_tmp2B0,sizeof(char),44U);}));});
# 1276
fd->cached_type=t;}
# 1281
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1284
struct Cyc_Tctyp_CVTEnv _tmp2B1=({struct Cyc_Tctyp_CVTEnv _tmp451=({struct Cyc_Tctyp_CVTEnv _tmp352;_tmp352.loc=loc,_tmp352.te=te,_tmp352.kind_env=bound_tvars,_tmp352.fn_result=0,_tmp352.generalize_evars=0,_tmp352.free_vars=0,_tmp352.free_evars=0;_tmp352;});struct Cyc_Absyn_Kind*_tmp450=expected_kind;int _tmp44F=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_tmp451,_tmp450,_tmp44F,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp2B1;
# 1286
struct Cyc_List_List*_tmp2B2=({struct Cyc_List_List*_tmp452=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst,cvt.free_vars);Cyc_Tctyp_remove_bound_tvars(_tmp452,bound_tvars);});struct Cyc_List_List*free_tvars=_tmp2B2;
{struct Cyc_List_List*fs=free_tvars;for(0;fs != 0;fs=fs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp351;_tmp351.tag=0U,({struct _fat_ptr _tmp453=({const char*_tmp2B9="unbound type variable ";_tag_fat(_tmp2B9,sizeof(char),23U);});_tmp351.f1=_tmp453;});_tmp351;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2B5=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp350;_tmp350.tag=7U,_tmp350.f1=(struct Cyc_Absyn_Tvar*)fs->hd;_tmp350;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp34F;_tmp34F.tag=0U,({struct _fat_ptr _tmp454=({const char*_tmp2B8=" in type ";_tag_fat(_tmp2B8,sizeof(char),10U);});_tmp34F.f1=_tmp454;});_tmp34F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2B7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp34E;_tmp34E.tag=2U,_tmp34E.f1=(void*)t;_tmp34E;});void*_tmp2B3[4U];_tmp2B3[0]=& _tmp2B4,_tmp2B3[1]=& _tmp2B5,_tmp2B3[2]=& _tmp2B6,_tmp2B3[3]=& _tmp2B7;({unsigned _tmp455=loc;Cyc_Warn_err2(_tmp455,_tag_fat(_tmp2B3,sizeof(void*),4U));});});}}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(cvt.free_evars,t,loc);}

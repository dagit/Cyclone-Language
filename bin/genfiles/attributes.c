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
 struct Cyc_Core_Opt{void*v;};
# 116 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 52
extern int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 109 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 875 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 53 "tcutil.h"
int Cyc_Tcutil_is_bound_one(void*);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*,int);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 25 "attributes.h"
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Atts_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Atts_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Atts_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Atts_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Atts_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Atts_Packed_att_val;
# 32
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Atts_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Atts_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Atts_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Atts_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Atts_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Atts_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Atts_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Atts_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Atts_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Atts_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Atts_Always_inline_att_val;
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 52
int Cyc_Atts_is_noreturn_fn_type(void*);
# 68
int Cyc_Atts_fntype_att(void*);
# 77
int Cyc_Atts_equal_att(void*,void*);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);
# 28 "attributes.cyc"
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Atts_Stdcall_att_val={1};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Atts_Cdecl_att_val={2};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Atts_Fastcall_att_val={3};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Atts_Noreturn_att_val={4};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Atts_Const_att_val={5};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Atts_Packed_att_val={7};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Atts_Nocommon_att_val={9};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Atts_Shared_att_val={10};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Atts_Unused_att_val={11};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Atts_Weak_att_val={12};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Atts_Dllimport_att_val={13};
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Atts_Dllexport_att_val={14};
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Atts_No_instrument_function_att_val={15};
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Atts_Constructor_att_val={16};
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Atts_Destructor_att_val={17};
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Atts_No_check_memory_usage_att_val={18};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Atts_Pure_att_val={23};
struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Atts_Always_inline_att_val={26};
struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val={27};
# 48
static void*Cyc_Atts_bad_attribute(unsigned loc){
({void*_tmp0=0U;({unsigned _tmpEB=loc;struct _fat_ptr _tmpEA=({const char*_tmp1="unrecognized attribute";_tag_fat(_tmp1,sizeof(char),23U);});Cyc_Warn_err(_tmpEB,_tmpEA,_tag_fat(_tmp0,sizeof(void*),0));});});
return(void*)& Cyc_Atts_Cdecl_att_val;}struct _tuple11{struct _fat_ptr f1;void*f2;};static char _tmp2[8U]="stdcall";static char _tmp3[6U]="cdecl";static char _tmp4[9U]="fastcall";static char _tmp5[9U]="noreturn";static char _tmp6[6U]="const";static char _tmp7[8U]="aligned";static char _tmp8[7U]="packed";static char _tmp9[7U]="shared";static char _tmpA[7U]="unused";static char _tmpB[5U]="weak";static char _tmpC[10U]="dllimport";static char _tmpD[10U]="dllexport";static char _tmpE[23U]="no_instrument_function";static char _tmpF[12U]="constructor";static char _tmp10[11U]="destructor";static char _tmp11[22U]="no_check_memory_usage";static char _tmp12[5U]="pure";static char _tmp13[14U]="always_inline";static char _tmp14[9U]="no_throw";
# 53
void*Cyc_Atts_parse_nullary_att(unsigned loc,struct _fat_ptr s){
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple11 att_map[19U]={{{_tmp2,_tmp2,_tmp2 + 8U},(void*)& Cyc_Atts_Stdcall_att_val},{{_tmp3,_tmp3,_tmp3 + 6U},(void*)& Cyc_Atts_Cdecl_att_val},{{_tmp4,_tmp4,_tmp4 + 9U},(void*)& Cyc_Atts_Fastcall_att_val},{{_tmp5,_tmp5,_tmp5 + 9U},(void*)& Cyc_Atts_Noreturn_att_val},{{_tmp6,_tmp6,_tmp6 + 6U},(void*)& Cyc_Atts_Const_att_val},{{_tmp7,_tmp7,_tmp7 + 8U},(void*)& att_aligned},{{_tmp8,_tmp8,_tmp8 + 7U},(void*)& Cyc_Atts_Packed_att_val},{{_tmp9,_tmp9,_tmp9 + 7U},(void*)& Cyc_Atts_Shared_att_val},{{_tmpA,_tmpA,_tmpA + 7U},(void*)& Cyc_Atts_Unused_att_val},{{_tmpB,_tmpB,_tmpB + 5U},(void*)& Cyc_Atts_Weak_att_val},{{_tmpC,_tmpC,_tmpC + 10U},(void*)& Cyc_Atts_Dllimport_att_val},{{_tmpD,_tmpD,_tmpD + 10U},(void*)& Cyc_Atts_Dllexport_att_val},{{_tmpE,_tmpE,_tmpE + 23U},(void*)& Cyc_Atts_No_instrument_function_att_val},{{_tmpF,_tmpF,_tmpF + 12U},(void*)& Cyc_Atts_Constructor_att_val},{{_tmp10,_tmp10,_tmp10 + 11U},(void*)& Cyc_Atts_Destructor_att_val},{{_tmp11,_tmp11,_tmp11 + 22U},(void*)& Cyc_Atts_No_check_memory_usage_att_val},{{_tmp12,_tmp12,_tmp12 + 5U},(void*)& Cyc_Atts_Pure_att_val},{{_tmp13,_tmp13,_tmp13 + 14U},(void*)& Cyc_Atts_Always_inline_att_val},{{_tmp14,_tmp14,_tmp14 + 9U},(void*)& Cyc_Atts_No_throw_att_val}};
# 77
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 79
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(1;(unsigned)i < 19U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple11*)_check_known_subscript_notnull(att_map,19U,sizeof(struct _tuple11),i))).f1)== 0)
return(att_map[i]).f2;}
return Cyc_Atts_bad_attribute(loc);}}
# 87
static int Cyc_Atts_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp15=e->r;void*_stmttmp0=_tmp15;void*_tmp16=_stmttmp0;int _tmp17;if(*((int*)_tmp16)== 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16)->f1).Int_c).tag == 5){_LL1: _tmp17=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16)->f1).Int_c).val).f2;_LL2: {int i=_tmp17;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 91
({void*_tmp18=0U;({unsigned _tmpED=loc;struct _fat_ptr _tmpEC=({const char*_tmp19="expecting integer constant";_tag_fat(_tmp19,sizeof(char),27U);});Cyc_Warn_err(_tmpED,_tmpEC,_tag_fat(_tmp18,sizeof(void*),0));});});
return 0;}_LL0:;}
# 95
static struct _fat_ptr Cyc_Atts_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp1A=e->r;void*_stmttmp1=_tmp1A;void*_tmp1B=_stmttmp1;struct _fat_ptr _tmp1C;if(*((int*)_tmp1B)== 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1B)->f1).String_c).tag == 8){_LL1: _tmp1C=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1B)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp1C;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 99
({void*_tmp1D=0U;({unsigned _tmpEF=loc;struct _fat_ptr _tmpEE=({const char*_tmp1E="expecting string constant";_tag_fat(_tmp1E,sizeof(char),26U);});Cyc_Warn_err(_tmpEF,_tmpEE,_tag_fat(_tmp1D,sizeof(void*),0));});});
return _tag_fat(0,0,0);}_LL0:;}
# 104
void*Cyc_Atts_parse_unary_att(unsigned sloc,struct _fat_ptr s,unsigned eloc,struct Cyc_Absyn_Exp*e){
void*a;
if(({struct _fat_ptr _tmpF1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpF1,({const char*_tmp1F="aligned";_tag_fat(_tmp1F,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmpF0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpF0,({const char*_tmp20="__aligned__";_tag_fat(_tmp20,sizeof(char),12U);}));})== 0)
return(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=6,_tmp21->f1=e;_tmp21;});
if(({struct _fat_ptr _tmpF3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpF3,({const char*_tmp22="section";_tag_fat(_tmp22,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmpF2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpF2,({const char*_tmp23="__section__";_tag_fat(_tmp23,sizeof(char),12U);}));})== 0)
return(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=8,({struct _fat_ptr _tmpF4=Cyc_Atts_exp2string(eloc,e);_tmp24->f1=_tmpF4;});_tmp24;});
if(({struct _fat_ptr _tmpF5=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpF5,({const char*_tmp25="__mode__";_tag_fat(_tmp25,sizeof(char),9U);}));})== 0)
return(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=24,({struct _fat_ptr _tmpF6=Cyc_Atts_exp2string(eloc,e);_tmp26->f1=_tmpF6;});_tmp26;});
if(({struct _fat_ptr _tmpF7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpF7,({const char*_tmp27="alias";_tag_fat(_tmp27,sizeof(char),6U);}));})== 0)
return(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=25,({struct _fat_ptr _tmpF8=Cyc_Atts_exp2string(eloc,e);_tmp28->f1=_tmpF8;});_tmp28;});{
int n=Cyc_Atts_exp2int(eloc,e);
if(({struct _fat_ptr _tmpFA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpFA,({const char*_tmp29="regparm";_tag_fat(_tmp29,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmpF9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpF9,({const char*_tmp2A="__regparm__";_tag_fat(_tmp2A,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp2B=0U;({unsigned _tmpFC=eloc;struct _fat_ptr _tmpFB=({const char*_tmp2C="regparm requires value between 0 and 3";_tag_fat(_tmp2C,sizeof(char),39U);});Cyc_Warn_err(_tmpFC,_tmpFB,_tag_fat(_tmp2B,sizeof(void*),0));});});
return(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=0,_tmp2D->f1=n;_tmp2D;});}
# 120
if(({struct _fat_ptr _tmpFE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpFE,({const char*_tmp2E="initializes";_tag_fat(_tmp2E,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmpFD=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpFD,({const char*_tmp2F="__initializes__";_tag_fat(_tmp2F,sizeof(char),16U);}));})== 0)
return(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=20,_tmp30->f1=n;_tmp30;});
if(({struct _fat_ptr _tmp100=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp100,({const char*_tmp31="noliveunique";_tag_fat(_tmp31,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmpFF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmpFF,({const char*_tmp32="__noliveunique__";_tag_fat(_tmp32,sizeof(char),17U);}));})== 0)
return(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=21,_tmp33->f1=n;_tmp33;});
if(({struct _fat_ptr _tmp102=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp102,({const char*_tmp34="consume";_tag_fat(_tmp34,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp101=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp101,({const char*_tmp35="__consume__";_tag_fat(_tmp35,sizeof(char),12U);}));})== 0)
return(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->tag=22,_tmp36->f1=n;_tmp36;});
({void*_tmp37=0U;({unsigned _tmp104=sloc;struct _fat_ptr _tmp103=({const char*_tmp38="unrecognized attribute";_tag_fat(_tmp38,sizeof(char),23U);});Cyc_Warn_err(_tmp104,_tmp103,_tag_fat(_tmp37,sizeof(void*),0));});});
return Cyc_Atts_bad_attribute(sloc);}}
# 130
void*Cyc_Atts_parse_format_att(unsigned loc,unsigned s2loc,struct _fat_ptr s1,struct _fat_ptr s2,unsigned u1,unsigned u2){
# 132
if(!(({struct _fat_ptr _tmp106=(struct _fat_ptr)s1;Cyc_zstrcmp(_tmp106,({const char*_tmp39="format";_tag_fat(_tmp39,sizeof(char),7U);}));})== 0)&& !(({struct _fat_ptr _tmp105=(struct _fat_ptr)s1;Cyc_zstrcmp(_tmp105,({const char*_tmp3A="__format__";_tag_fat(_tmp3A,sizeof(char),11U);}));})== 0))
return Cyc_Atts_bad_attribute(loc);
if(({struct _fat_ptr _tmp108=(struct _fat_ptr)s2;Cyc_zstrcmp(_tmp108,({const char*_tmp3B="printf";_tag_fat(_tmp3B,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp107=(struct _fat_ptr)s2;Cyc_zstrcmp(_tmp107,({const char*_tmp3C="__printf__";_tag_fat(_tmp3C,sizeof(char),11U);}));})== 0)
return(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->tag=19,_tmp3D->f1=Cyc_Absyn_Printf_ft,_tmp3D->f2=(int)u1,_tmp3D->f3=(int)u2;_tmp3D;});
if(({struct _fat_ptr _tmp10A=(struct _fat_ptr)s2;Cyc_zstrcmp(_tmp10A,({const char*_tmp3E="scanf";_tag_fat(_tmp3E,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp109=(struct _fat_ptr)s2;Cyc_zstrcmp(_tmp109,({const char*_tmp3F="__scanf__";_tag_fat(_tmp3F,sizeof(char),10U);}));})== 0)
return(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->tag=19,_tmp40->f1=Cyc_Absyn_Scanf_ft,_tmp40->f2=(int)u1,_tmp40->f3=(int)u2;_tmp40;});
({void*_tmp41=0U;({unsigned _tmp10C=loc;struct _fat_ptr _tmp10B=({const char*_tmp42="unrecognized format type";_tag_fat(_tmp42,sizeof(char),25U);});Cyc_Warn_err(_tmp10C,_tmp10B,_tag_fat(_tmp41,sizeof(void*),0));});});
return(void*)& Cyc_Atts_Cdecl_att_val;}
# 144
int Cyc_Atts_is_noreturn_fn_type(void*t){
void*_tmp43=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp43;void*_tmp44=_stmttmp2;struct Cyc_List_List*_tmp45;void*_tmp46;switch(*((int*)_tmp44)){case 3: _LL1: _tmp46=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44)->f1).elt_type;_LL2: {void*elt=_tmp46;
return Cyc_Atts_is_noreturn_fn_type(elt);}case 5: _LL3: _tmp45=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44)->f1).attributes;_LL4: {struct Cyc_List_List*atts=_tmp45;
# 148
for(1;atts != 0;atts=atts->tl){
void*_tmp47=(void*)atts->hd;void*_stmttmp3=_tmp47;void*_tmp48=_stmttmp3;if(*((int*)_tmp48)== 4){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 153
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 158
int Cyc_Atts_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a2))
return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a1))
return 0;}}
return 1;}
# 168
static int Cyc_Atts_is_regparm0_att(void*a){
void*_tmp49=a;if(*((int*)_tmp49)== 0){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp49)->f1 == 0){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 175
int Cyc_Atts_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a2)&& !Cyc_Atts_is_regparm0_att((void*)a->hd))
return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a1)&& !Cyc_Atts_is_regparm0_att((void*)a->hd))
return 0;}}
return 1;}
# 187
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp4A=Cyc_Absyn_compress(t);void*_stmttmp4=_tmp4A;void*_tmp4B=_stmttmp4;struct Cyc_List_List**_tmp4C;if(*((int*)_tmp4B)== 5){_LL1: _tmp4C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4B)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp4C;
# 190
struct Cyc_List_List*_tmp4D=0;struct Cyc_List_List*res_atts=_tmp4D;
for(1;atts != 0;atts=atts->tl){
if(!Cyc_Atts_fntype_att((void*)atts->hd))
res_atts=({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=(void*)atts->hd,_tmp4E->tl=res_atts;_tmp4E;});else{
if(!((int(*)(int(*)(void*,void*),struct Cyc_List_List*,void*))Cyc_List_mem)(Cyc_Atts_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmp10D=({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->hd=(void*)atts->hd,_tmp4F->tl=*fnatts;_tmp4F;});*fnatts=_tmp10D;});}}
return res_atts;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp51=({struct Cyc_Warn_String_Warn_Warg_struct _tmpC9;_tmpC9.tag=0,({struct _fat_ptr _tmp10E=({const char*_tmp52="transfer_fn_type_atts";_tag_fat(_tmp52,sizeof(char),22U);});_tmpC9.f1=_tmp10E;});_tmpC9;});void*_tmp50[1];_tmp50[0]=& _tmp51;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp50,sizeof(void*),1));});}_LL0:;}
# 201
static int Cyc_Atts_fnTypeAttsRangeOK(unsigned loc,int i,int nargs,void*att){
if(i < 1 || i > nargs){
({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp54=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpCB;_tmpCB.tag=10,_tmpCB.f1=(void*)att;_tmpCB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp55=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA;_tmpCA.tag=0,({struct _fat_ptr _tmp10F=({const char*_tmp56=" has an out-of-range index";_tag_fat(_tmp56,sizeof(char),27U);});_tmpCA.f1=_tmp10F;});_tmpCA;});void*_tmp53[2];_tmp53[0]=& _tmp54,_tmp53[1]=& _tmp55;({unsigned _tmp110=loc;Cyc_Warn_err2(_tmp110,_tag_fat(_tmp53,sizeof(void*),2));});});
return 0;}
# 206
return 1;}struct _tuple12{struct Cyc_List_List*f1;struct _fat_ptr f2;};
# 208
static void Cyc_Atts_fnTypeAttsOverlap(unsigned loc,int i,struct _tuple12 lst1,struct _tuple12 lst2){
# 211
if(((int(*)(struct Cyc_List_List*,int))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmpCE;_tmpCE.tag=0,_tmpCE.f1=(struct _fat_ptr)((struct _fat_ptr)lst1.f2);_tmpCE;});struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmpCD;_tmpCD.tag=0,_tmpCD.f1=(struct _fat_ptr)((struct _fat_ptr)lst2.f2);_tmpCD;});struct Cyc_Int_pa_PrintArg_struct _tmp5B=({struct Cyc_Int_pa_PrintArg_struct _tmpCC;_tmpCC.tag=1,_tmpCC.f1=(unsigned long)i;_tmpCC;});void*_tmp57[3];_tmp57[0]=& _tmp59,_tmp57[1]=& _tmp5A,_tmp57[2]=& _tmp5B;({unsigned _tmp112=loc;struct _fat_ptr _tmp111=({const char*_tmp58="incompatible %s() and %s() attributes on parameter %d";_tag_fat(_tmp58,sizeof(char),54U);});Cyc_Warn_err(_tmp112,_tmp111,_tag_fat(_tmp57,sizeof(void*),3));});});}
# 215
void Cyc_Atts_fnTypeAttsOK(unsigned loc,void*t){
struct _tuple12 init_params=({struct _tuple12 _tmpDC;_tmpDC.f1=0,({struct _fat_ptr _tmp113=({const char*_tmp8F="initializes";_tag_fat(_tmp8F,sizeof(char),12U);});_tmpDC.f2=_tmp113;});_tmpDC;});
struct _tuple12 nolive_unique_params=({struct _tuple12 _tmpDB;_tmpDB.f1=0,({struct _fat_ptr _tmp114=({const char*_tmp8E="noliveunique";_tag_fat(_tmp8E,sizeof(char),13U);});_tmpDB.f2=_tmp114;});_tmpDB;});
struct _tuple12 consume_params=({struct _tuple12 _tmpDA;_tmpDA.f1=0,({struct _fat_ptr _tmp115=({const char*_tmp8D="consume";_tag_fat(_tmp8D,sizeof(char),8U);});_tmpDA.f2=_tmp115;});_tmpDA;});
void*_tmp5C=Cyc_Absyn_compress(t);void*_stmttmp5=_tmp5C;void*_tmp5D=_stmttmp5;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp5E;if(*((int*)_tmp5D)== 5){_LL1: _tmp5E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D)->f1).attributes;_tmp5F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D)->f1).args;_LL2: {struct Cyc_List_List*atts=_tmp5E;struct Cyc_List_List*args=_tmp5F;
# 221
int nargs=((int(*)(struct Cyc_List_List*))Cyc_List_length)(args);
for(1;atts != 0;atts=atts->tl){
void*_tmp60=(void*)atts->hd;void*_stmttmp6=_tmp60;void*_tmp61=_stmttmp6;int _tmp62;int _tmp63;int _tmp64;switch(*((int*)_tmp61)){case 20: _LL6: _tmp64=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp61)->f1;_LL7: {int i=_tmp64;
# 225
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))
goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,init_params,nolive_unique_params);
Cyc_Atts_fnTypeAttsOverlap(loc,i,init_params,consume_params);{
struct _fat_ptr s=({const char*_tmp7B="initializes attribute allowed only on";_tag_fat(_tmp7B,sizeof(char),38U);});
{void*_tmp65=Cyc_Absyn_compress((*((struct _tuple8*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(args,i - 1)).f3);void*_stmttmp7=_tmp65;void*_tmp66=_stmttmp7;void*_tmp69;void*_tmp68;void*_tmp67;if(*((int*)_tmp66)== 3){_LLF: _tmp67=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66)->f1).ptr_atts).nullable;_tmp68=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66)->f1).ptr_atts).bounds;_tmp69=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66)->f1).ptr_atts).zero_term;_LL10: {void*nullable=_tmp67;void*bd=_tmp68;void*zt=_tmp69;
# 232
if(Cyc_Tcutil_force_type2bool(0,nullable))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD0;_tmpD0.tag=0,_tmpD0.f1=s;_tmpD0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmpCF;_tmpCF.tag=0,({struct _fat_ptr _tmp116=({const char*_tmp6D=" non-null pointers";_tag_fat(_tmp6D,sizeof(char),19U);});_tmpCF.f1=_tmp116;});_tmpCF;});void*_tmp6A[2];_tmp6A[0]=& _tmp6B,_tmp6A[1]=& _tmp6C;({unsigned _tmp117=loc;Cyc_Warn_err2(_tmp117,_tag_fat(_tmp6A,sizeof(void*),2));});});
if(!Cyc_Tcutil_is_bound_one(bd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6F=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD2;_tmpD2.tag=0,_tmpD2.f1=s;_tmpD2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD1;_tmpD1.tag=0,({struct _fat_ptr _tmp118=({const char*_tmp71=" pointers of size 1";_tag_fat(_tmp71,sizeof(char),20U);});_tmpD1.f1=_tmp118;});_tmpD1;});void*_tmp6E[2];_tmp6E[0]=& _tmp6F,_tmp6E[1]=& _tmp70;({unsigned _tmp119=loc;Cyc_Warn_err2(_tmp119,_tag_fat(_tmp6E,sizeof(void*),2));});});
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp73=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD4;_tmpD4.tag=0,_tmpD4.f1=s;_tmpD4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD3;_tmpD3.tag=0,({struct _fat_ptr _tmp11A=({const char*_tmp75=" pointers to non-zero-terminated arrays";_tag_fat(_tmp75,sizeof(char),40U);});_tmpD3.f1=_tmp11A;});_tmpD3;});void*_tmp72[2];_tmp72[0]=& _tmp73,_tmp72[1]=& _tmp74;({unsigned _tmp11B=loc;Cyc_Warn_err2(_tmp11B,_tag_fat(_tmp72,sizeof(void*),2));});});
goto _LLE;}}else{_LL11: _LL12:
# 240
({struct Cyc_Warn_String_Warn_Warg_struct _tmp77=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD6;_tmpD6.tag=0,_tmpD6.f1=s;_tmpD6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp78=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD5;_tmpD5.tag=0,({struct _fat_ptr _tmp11C=({const char*_tmp79=" pointers";_tag_fat(_tmp79,sizeof(char),10U);});_tmpD5.f1=_tmp11C;});_tmpD5;});void*_tmp76[2];_tmp76[0]=& _tmp77,_tmp76[1]=& _tmp78;({unsigned _tmp11D=loc;Cyc_Warn_err2(_tmp11D,_tag_fat(_tmp76,sizeof(void*),2));});});}_LLE:;}
# 242
({struct Cyc_List_List*_tmp11E=({struct Cyc_List_List*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->hd=(void*)i,_tmp7A->tl=init_params.f1;_tmp7A;});init_params.f1=_tmp11E;});
goto _LL5;}}case 21: _LL8: _tmp63=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp61)->f1;_LL9: {int i=_tmp63;
# 245
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,nolive_unique_params,init_params);{
# 248
struct _tuple8*_tmp7C=((struct _tuple8*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp8=_tmp7C;struct _tuple8*_tmp7D=_stmttmp8;void*_tmp7E;_LL14: _tmp7E=_tmp7D->f3;_LL15: {void*t=_tmp7E;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD7;_tmpD7.tag=0,({struct _fat_ptr _tmp11F=({const char*_tmp81="noliveunique allowed only on unique pointers";_tag_fat(_tmp81,sizeof(char),45U);});_tmpD7.f1=_tmp11F;});_tmpD7;});void*_tmp7F[1];_tmp7F[0]=& _tmp80;({unsigned _tmp120=loc;Cyc_Warn_err2(_tmp120,_tag_fat(_tmp7F,sizeof(void*),1));});});
({struct Cyc_List_List*_tmp121=({struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=(void*)i,_tmp82->tl=nolive_unique_params.f1;_tmp82;});nolive_unique_params.f1=_tmp121;});
goto _LL5;}}}case 22: _LLA: _tmp62=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmp61)->f1;_LLB: {int i=_tmp62;
# 254
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,consume_params,init_params);{
# 257
struct _tuple8*_tmp83=((struct _tuple8*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(args,i - 1);struct _tuple8*_stmttmp9=_tmp83;struct _tuple8*_tmp84=_stmttmp9;void*_tmp85;_LL17: _tmp85=_tmp84->f3;_LL18: {void*t=_tmp85;
if(!Cyc_Tcutil_is_noalias_pointer(t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD8;_tmpD8.tag=0,({struct _fat_ptr _tmp122=({const char*_tmp88="consume allowed only on unique pointers";_tag_fat(_tmp88,sizeof(char),40U);});_tmpD8.f1=_tmp122;});_tmpD8;});void*_tmp86[1];_tmp86[0]=& _tmp87;({unsigned _tmp123=loc;Cyc_Warn_err2(_tmp123,_tag_fat(_tmp86,sizeof(void*),1));});});
({struct Cyc_List_List*_tmp124=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=(void*)i,_tmp89->tl=consume_params.f1;_tmp89;});consume_params.f1=_tmp124;});
goto _LL5;}}}default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 265
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8B=({struct Cyc_Warn_String_Warn_Warg_struct _tmpD9;_tmpD9.tag=0,({struct _fat_ptr _tmp125=({const char*_tmp8C="fnTypeAttsOK: not a function type";_tag_fat(_tmp8C,sizeof(char),34U);});_tmpD9.f1=_tmp125;});_tmpD9;});void*_tmp8A[1];_tmp8A[0]=& _tmp8B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp8A,sizeof(void*),1));});}_LL0:;}
# 271
int Cyc_Atts_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp90=(void*)t->hd;void*_stmttmpA=_tmp90;void*_tmp91=_stmttmpA;switch(*((int*)_tmp91)){case 23: _LL1: _LL2:
 goto _LL4;case 4: _LL3: _LL4:
 goto _LL6;case 20: _LL5: _LL6:
 continue;default: _LL7: _LL8:
 if(!((int(*)(int(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 279
for(1;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a2->hd,a1))
return 0;}
return 1;}
# 285
static int Cyc_Atts_attribute_case_number(void*att){
void*_tmp92=att;switch(*((int*)_tmp92)){case 0: _LL1: _LL2:
 return 0;case 1: _LL3: _LL4:
 return 1;case 2: _LL5: _LL6:
 return 2;case 3: _LL7: _LL8:
 return 3;case 4: _LL9: _LLA:
 return 4;case 5: _LLB: _LLC:
 return 5;case 6: _LLD: _LLE:
 return 6;case 7: _LLF: _LL10:
 return 7;case 8: _LL11: _LL12:
 return 8;case 9: _LL13: _LL14:
 return 9;case 10: _LL15: _LL16:
 return 10;case 11: _LL17: _LL18:
 return 11;case 12: _LL19: _LL1A:
 return 12;case 13: _LL1B: _LL1C:
 return 13;case 14: _LL1D: _LL1E:
 return 14;case 15: _LL1F: _LL20:
 return 15;case 16: _LL21: _LL22:
 return 16;case 17: _LL23: _LL24:
 return 17;case 18: _LL25: _LL26:
 return 18;case 19: _LL27: _LL28:
 return 19;case 20: _LL29: _LL2A:
 return 20;case 21: _LL2B: _LL2C:
 return 21;case 22: _LL2D: _LL2E:
 return 22;case 23: _LL2F: _LL30:
 return 23;case 24: _LL31: _LL32:
 return 24;case 25: _LL33: _LL34:
 return 25;case 26: _LL35: _LL36:
 return 26;default: _LL37: _LL38:
 return 27;}_LL0:;}struct _tuple13{void*f1;void*f2;};
# 317
int Cyc_Atts_attribute_cmp(void*att1,void*att2){
struct _tuple13 _tmp93=({struct _tuple13 _tmpDD;_tmpDD.f1=att1,_tmpDD.f2=att2;_tmpDD;});struct _tuple13 _stmttmpB=_tmp93;struct _tuple13 _tmp94=_stmttmpB;int _tmp9A;int _tmp99;enum Cyc_Absyn_Format_Type _tmp98;int _tmp97;int _tmp96;enum Cyc_Absyn_Format_Type _tmp95;struct _fat_ptr _tmp9C;struct _fat_ptr _tmp9B;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;int _tmpA0;int _tmp9F;int _tmpA2;int _tmpA1;switch(*((int*)_tmp94.f1)){case 0: if(*((int*)_tmp94.f2)== 0){_LL1: _tmpA1=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp94.f1)->f1;_tmpA2=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp94.f2)->f1;_LL2: {int i1=_tmpA1;int i2=_tmpA2;
_tmp9F=i1;_tmpA0=i2;goto _LL4;}}else{goto _LLB;}case 20: if(*((int*)_tmp94.f2)== 20){_LL3: _tmp9F=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp94.f1)->f1;_tmpA0=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp94.f2)->f1;_LL4: {int i1=_tmp9F;int i2=_tmpA0;
return Cyc_Core_intcmp(i1,i2);}}else{goto _LLB;}case 6: if(*((int*)_tmp94.f2)== 6){_LL5: _tmp9D=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp94.f1)->f1;_tmp9E=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp94.f2)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp9D;struct Cyc_Absyn_Exp*e2=_tmp9E;
# 322
if(e1 == e2)return 0;
if(e1 == 0)return - 1;
if(e2 == 0)return 1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LLB;}case 8: if(*((int*)_tmp94.f2)== 8){_LL7: _tmp9B=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp94.f1)->f1;_tmp9C=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp94.f2)->f1;_LL8: {struct _fat_ptr s1=_tmp9B;struct _fat_ptr s2=_tmp9C;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLB;}case 19: if(*((int*)_tmp94.f2)== 19){_LL9: _tmp95=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp94.f1)->f1;_tmp96=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp94.f1)->f2;_tmp97=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp94.f1)->f3;_tmp98=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp94.f2)->f1;_tmp99=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp94.f2)->f2;_tmp9A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp94.f2)->f3;_LLA: {enum Cyc_Absyn_Format_Type ft1=_tmp95;int i1=_tmp96;int j1=_tmp97;enum Cyc_Absyn_Format_Type ft2=_tmp98;int i2=_tmp99;int j2=_tmp9A;
# 328
int _tmpA3=Cyc_Core_intcmp((int)((unsigned)ft1),(int)((unsigned)ft2));int ftc=_tmpA3;
if(ftc != 0)return ftc;{
int _tmpA4=Cyc_Core_intcmp(i1,i2);int ic=_tmpA4;
if(ic != 0)return ic;
return Cyc_Core_intcmp(j1,j2);}}}else{goto _LLB;}default: _LLB: _LLC:
# 334
 return({int _tmp126=Cyc_Atts_attribute_case_number(att1);Cyc_Core_intcmp(_tmp126,
Cyc_Atts_attribute_case_number(att2));});}_LL0:;}
# 338
int Cyc_Atts_equal_att(void*a1,void*a2){
return Cyc_Atts_attribute_cmp(a1,a2)== 0;}
# 343
int Cyc_Atts_fntype_att(void*a){
void*_tmpA5=a;int _tmpA6;switch(*((int*)_tmpA5)){case 0: _LL1: _tmpA6=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpA5)->f1;_LL2: {int i=_tmpA6;
goto _LL4;}case 3: _LL3: _LL4:
 goto _LL6;case 1: _LL5: _LL6:
 goto _LL8;case 2: _LL7: _LL8:
 goto _LLA;case 4: _LL9: _LLA:
 goto _LLC;case 23: _LLB: _LLC:
 goto _LLE;case 26: _LLD: _LLE:
 goto _LL10;case 19: _LLF: _LL10:
 goto _LL12;case 5: _LL11: _LL12:
 goto _LL14;case 21: _LL13: _LL14:
 goto _LL16;case 20: _LL15: _LL16:
 goto _LL18;case 22: _LL17: _LL18:
 goto _LL1A;case 27: _LL19: _LL1A:
 return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 362
void Cyc_Atts_check_fndecl_atts(unsigned loc,struct Cyc_List_List*atts,int is_defn){
for(1;atts != 0;atts=atts->tl){
void*_tmpA7=(void*)atts->hd;void*_stmttmpC=_tmpA7;void*_tmpA8=_stmttmpC;switch(*((int*)_tmpA8)){case 6: _LL1: _LL2:
 goto _LL4;case 7: _LL3: _LL4:
# 367
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmpE0;_tmpE0.tag=0,({struct _fat_ptr _tmp127=({const char*_tmpAE="bad attribute ";_tag_fat(_tmpAE,sizeof(char),15U);});_tmpE0.f1=_tmp127;});_tmpE0;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpAB=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpDF;_tmpDF.tag=10,_tmpDF.f1=(void*)atts->hd;_tmpDF;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAC=({struct Cyc_Warn_String_Warn_Warg_struct _tmpDE;_tmpDE.tag=0,({struct _fat_ptr _tmp128=({const char*_tmpAD=" in function declaration";_tag_fat(_tmpAD,sizeof(char),25U);});_tmpDE.f1=_tmp128;});_tmpDE;});void*_tmpA9[3];_tmpA9[0]=& _tmpAA,_tmpA9[1]=& _tmpAB,_tmpA9[2]=& _tmpAC;({unsigned _tmp129=loc;Cyc_Warn_err2(_tmp129,_tag_fat(_tmpA9,sizeof(void*),3));});});
return;default: _LL5: _LL6:
# 370
 if(!is_defn && Cyc_Atts_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmpE1;_tmpE1.tag=0,({struct _fat_ptr _tmp12A=({const char*_tmpB1="function-type att in function variable declaration";_tag_fat(_tmpB1,sizeof(char),51U);});_tmpE1.f1=_tmp12A;});_tmpE1;});void*_tmpAF[1];_tmpAF[0]=& _tmpB0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpAF,sizeof(void*),1));});}_LL0:;}}
# 375
void Cyc_Atts_check_variable_atts(unsigned loc,struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List*atts){
for(1;atts != 0;atts=atts->tl){
void*_tmpB2=(void*)atts->hd;void*_stmttmpD=_tmpB2;void*_tmpB3=_stmttmpD;switch(*((int*)_tmpB3)){case 6: _LL1: _LL2:
 goto _LL4;case 8: _LL3: _LL4:
# 381
 goto _LL6;case 9: _LL5: _LL6:
 goto _LL8;case 10: _LL7: _LL8:
 goto _LLA;case 11: _LL9: _LLA:
 goto _LLC;case 12: _LLB: _LLC:
 goto _LLE;case 13: _LLD: _LLE:
 goto _LL10;case 14: _LLF: _LL10:
 continue;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_String_Warn_Warg_struct _tmpE5;_tmpE5.tag=0,({struct _fat_ptr _tmp12B=({const char*_tmpBA="bad attribute ";_tag_fat(_tmpBA,sizeof(char),15U);});_tmpE5.f1=_tmp12B;});_tmpE5;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpE4;_tmpE4.tag=10,_tmpE4.f1=(void*)atts->hd;_tmpE4;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB7=({struct Cyc_Warn_String_Warn_Warg_struct _tmpE3;_tmpE3.tag=0,({struct _fat_ptr _tmp12C=({const char*_tmpB9=" for variable ";_tag_fat(_tmpB9,sizeof(char),15U);});_tmpE3.f1=_tmp12C;});_tmpE3;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmpB8=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmpE2;_tmpE2.tag=11,_tmpE2.f1=vd;_tmpE2;});void*_tmpB4[4];_tmpB4[0]=& _tmpB5,_tmpB4[1]=& _tmpB6,_tmpB4[2]=& _tmpB7,_tmpB4[3]=& _tmpB8;({unsigned _tmp12D=loc;Cyc_Warn_err2(_tmp12D,_tag_fat(_tmpB4,sizeof(void*),4));});});}_LL0:;}}
# 392
void Cyc_Atts_check_field_atts(unsigned loc,struct _fat_ptr*fn,struct Cyc_List_List*atts){
for(1;atts != 0;atts=atts->tl){
void*_tmpBB=(void*)atts->hd;void*_stmttmpE=_tmpBB;void*_tmpBC=_stmttmpE;switch(*((int*)_tmpBC)){case 7: _LL1: _LL2:
 goto _LL4;case 6: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmpE9;_tmpE9.tag=0,({struct _fat_ptr _tmp12E=({const char*_tmpC3="bad attribute ";_tag_fat(_tmpC3,sizeof(char),15U);});_tmpE9.f1=_tmp12E;});_tmpE9;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpBF=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmpE8;_tmpE8.tag=10,_tmpE8.f1=(void*)atts->hd;_tmpE8;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC0=({struct Cyc_Warn_String_Warn_Warg_struct _tmpE7;_tmpE7.tag=0,({struct _fat_ptr _tmp12F=({const char*_tmpC2=" on ";_tag_fat(_tmpC2,sizeof(char),5U);});_tmpE7.f1=_tmp12F;});_tmpE7;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC1=({struct Cyc_Warn_String_Warn_Warg_struct _tmpE6;_tmpE6.tag=0,_tmpE6.f1=*fn;_tmpE6;});void*_tmpBD[4];_tmpBD[0]=& _tmpBE,_tmpBD[1]=& _tmpBF,_tmpBD[2]=& _tmpC0,_tmpBD[3]=& _tmpC1;({unsigned _tmp130=loc;Cyc_Warn_err2(_tmp130,_tag_fat(_tmpBD,sizeof(void*),4));});});}_LL0:;}}
# 402
struct Cyc_List_List*Cyc_Atts_merge_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
struct Cyc_List_List*x=0;
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_exists_c)(Cyc_Atts_equal_att,(void*)a->hd,a2))
x=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->hd=(void*)a->hd,_tmpC4->tl=x;_tmpC4;});}}
return({struct Cyc_List_List*_tmp131=x;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp131,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_copy)(a2));});}
# 410
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*atts){
# 413
struct Cyc_List_List*_tmpC5=0;struct Cyc_List_List*ans=_tmpC5;
for(1;atts != 0;atts=atts->tl){
void*_tmpC6=(void*)atts->hd;void*_stmttmpF=_tmpC6;void*_tmpC7=_stmttmpF;switch(*((int*)_tmpC7)){case 4: _LL1: _LL2:
 goto _LL4;case 5: _LL3: _LL4:
 goto _LL6;case 19: _LL5: _LL6:
 continue;case 22: _LL7: _LL8:
 continue;case 21: _LL9: _LLA:
 continue;case 20: _LLB: _LLC:
 continue;case 27: _LLD: _LLE:
 continue;default: _LLF: _LL10:
 ans=({struct Cyc_List_List*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->hd=(void*)atts->hd,_tmpC8->tl=ans;_tmpC8;});goto _LL0;}_LL0:;}
# 425
return((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(ans);}

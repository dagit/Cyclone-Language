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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 114
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 284
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 297
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 304
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 844 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 847
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 864
void*Cyc_Absyn_compress(void*);
# 868
int Cyc_Absyn_type2bool(int,void*);
# 877
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 889
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 891
extern void*Cyc_Absyn_empty_effect;
# 893
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);
# 914
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar (void);
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar (void);
# 924
void*Cyc_Absyn_bounds_one (void);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 37 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
# 43
int Cyc_Tcutil_is_function_type(void*);
# 46
int Cyc_Tcutil_is_array_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 102
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 156
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 158
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 214
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 217
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 228
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 243
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 246
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
# 40
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 42
extern struct Cyc_Absyn_Kind Cyc_Kinds_urk;
# 44
extern struct Cyc_Absyn_Kind Cyc_Kinds_ubk;
# 66
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 86
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_bound_opt(struct Cyc_Absyn_Kind*);
int Cyc_Kinds_constrain_kinds(void*,void*);
# 68 "attributes.h"
int Cyc_Atts_fntype_att(void*);
# 72
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*,struct Cyc_List_List*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 73 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*,struct Cyc_List_List*);
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
# 36 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 77 "flags.h"
extern int Cyc_Flags_tovc_r;
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 35
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*);
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*);struct Cyc_Tctyp_CVTEnv{unsigned loc;struct Cyc_Tcenv_Tenv*te;struct Cyc_List_List*kind_env;int fn_result;int generalize_evars;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;};
# 84 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*,void*,int,int);struct _tuple13{void*f1;int f2;};
# 90
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct Cyc_List_List*es,void*e,int b){
void*_tmp0=Cyc_Absyn_compress(e);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;int _tmp2;if(*((int*)_tmp1)== 1){_tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->f3;{int i=_tmp2;
# 93
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple13*_tmp3=(struct _tuple13*)es2->hd;struct _tuple13*_stmttmp1=_tmp3;struct _tuple13*_tmp4=_stmttmp1;void*_tmp6;void*_tmp5;_tmp5=_tmp4->f1;_tmp6=(int*)& _tmp4->f2;{void*t=_tmp5;int*b2=(int*)_tmp6;
void*_tmp7=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp7;void*_tmp8=_stmttmp2;int _tmp9;if(*((int*)_tmp8)== 1){_tmp9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8)->f3;{int j=_tmp9;
# 97
if(i == j){
if(b != *b2)*b2=1;
return es;}
# 101
goto _LL8;}}else{
goto _LL8;}_LL8:;}}}
# 105
return({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));({struct _tuple13*_tmp2CC=({struct _tuple13*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=e,_tmpA->f2=b;_tmpA;});_tmpB->hd=_tmp2CC;}),_tmpB->tl=es;_tmpB;});}}else{
return es;};}
# 111
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == -1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp246;_tmp246.tag=0,({struct _fat_ptr _tmp2CD=({const char*_tmp11="bound tvar id for ";_tag_fat(_tmp11,sizeof(char),19U);});_tmp246.f1=_tmp2CD;});_tmp246;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpE=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp245;_tmp245.tag=7,_tmp245.f1=tv;_tmp245;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp244;_tmp244.tag=0,({struct _fat_ptr _tmp2CE=({const char*_tmp10=" is NULL";_tag_fat(_tmp10,sizeof(char),9U);});_tmp244.f1=_tmp2CE;});_tmp244;});void*_tmpC[3];_tmpC[0]=& _tmpD,_tmpC[1]=& _tmpE,_tmpC[2]=& _tmpF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC,sizeof(void*),3));});
return({struct Cyc_List_List*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=tv,_tmp12->tl=tvs;_tmp12;});}
# 116
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(void*)){
# 118
struct Cyc_List_List*r=0;
for(1;tvs != 0;tvs=tvs->tl){
struct Cyc_List_List*b=btvs;
for(1;b != 0;b=b->tl){
if(({int _tmp2CF=(get(tvs->hd))->identity;_tmp2CF == ((struct Cyc_Absyn_Tvar*)b->hd)->identity;}))
break;}
if(b == 0)
r=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=tvs->hd,_tmp13->tl=r;_tmp13;});}
# 127
r=Cyc_List_imp_rev(r);
return r;}
# 135
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
for(1;free_evars != 0;free_evars=free_evars->tl){
void*e=(*((struct _tuple13*)free_evars->hd)).f1;
{void*_tmp14=Cyc_Absyn_compress(e);void*_stmttmp3=_tmp14;void*_tmp15=_stmttmp3;if(*((int*)_tmp15)== 1)
goto _LL0;else{
continue;}_LL0:;}{
# 142
void*t;
struct _fat_ptr s;
{struct Cyc_Absyn_Kind*_tmp16=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_stmttmp4=_tmp16;struct Cyc_Absyn_Kind*_tmp17=_stmttmp4;switch((int)((struct Cyc_Absyn_Kind*)_tmp17)->kind){case Cyc_Absyn_RgnKind: switch((int)((struct Cyc_Absyn_Kind*)_tmp17)->aliasqual){case Cyc_Absyn_Unique:
 t=Cyc_Absyn_unique_rgn_type;s=({const char*_tmp18="unique region";_tag_fat(_tmp18,sizeof(char),14U);});goto _LL5;case Cyc_Absyn_Aliasable:
 goto _LLB;case Cyc_Absyn_Top: _LLB:
 t=Cyc_Absyn_heap_rgn_type;s=({const char*_tmp19="heap";_tag_fat(_tmp19,sizeof(char),5U);});goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind:
 t=Cyc_Absyn_empty_effect;s=({const char*_tmp1A="{}";_tag_fat(_tmp1A,sizeof(char),3U);});goto _LL5;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_false_type;s=({const char*_tmp1B="@false";_tag_fat(_tmp1B,sizeof(char),7U);});goto _LL5;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_bounds_one();s=({const char*_tmp1C="bounds_one";_tag_fat(_tmp1C,sizeof(char),11U);});goto _LL5;default: _LL12:
# 152
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24A;_tmp24A.tag=0,({struct _fat_ptr _tmp2D0=({const char*_tmp23="hidden type variable ";_tag_fat(_tmp23,sizeof(char),22U);});_tmp24A.f1=_tmp2D0;});_tmp24A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp249;_tmp249.tag=2,_tmp249.f1=(void*)e;_tmp249;});struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp248;_tmp248.tag=0,({struct _fat_ptr _tmp2D1=({const char*_tmp22=" isn't abstracted in type ";_tag_fat(_tmp22,sizeof(char),27U);});_tmp248.f1=_tmp2D1;});_tmp248;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp247;_tmp247.tag=2,_tmp247.f1=(void*)in_typ;_tmp247;});void*_tmp1D[4];_tmp1D[0]=& _tmp1E,_tmp1D[1]=& _tmp1F,_tmp1D[2]=& _tmp20,_tmp1D[3]=& _tmp21;({unsigned _tmp2D2=loc;Cyc_Warn_err2(_tmp2D2,_tag_fat(_tmp1D,sizeof(void*),4));});});
return;}_LL5:;}
# 155
if(!Cyc_Unify_unify(e,t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24F;_tmp24F.tag=0,({struct _fat_ptr _tmp2D3=({const char*_tmp2C="can't unify evar ";_tag_fat(_tmp2C,sizeof(char),18U);});_tmp24F.f1=_tmp2D3;});_tmp24F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp26=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp24E;_tmp24E.tag=2,_tmp24E.f1=(void*)e;_tmp24E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24D;_tmp24D.tag=0,({struct _fat_ptr _tmp2D4=({const char*_tmp2B=" with ";_tag_fat(_tmp2B,sizeof(char),7U);});_tmp24D.f1=_tmp2D4;});_tmp24D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24C;_tmp24C.tag=0,_tmp24C.f1=s;_tmp24C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmp24B;_tmp24B.tag=0,({struct _fat_ptr _tmp2D5=({const char*_tmp2A="!";_tag_fat(_tmp2A,sizeof(char),2U);});_tmp24B.f1=_tmp2D5;});_tmp24B;});void*_tmp24[5];_tmp24[0]=& _tmp25,_tmp24[1]=& _tmp26,_tmp24[2]=& _tmp27,_tmp24[3]=& _tmp28,_tmp24[4]=& _tmp29;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp24,sizeof(void*),5));});}}}
# 166
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Kinds_constrain_kinds(k1,k2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp255;_tmp255.tag=0,({struct _fat_ptr _tmp2D6=({const char*_tmp36="type variable ";_tag_fat(_tmp36,sizeof(char),15U);});_tmp255.f1=_tmp2D6;});_tmp255;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2F=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp254;_tmp254.tag=7,_tmp254.f1=tv;_tmp254;});struct Cyc_Warn_String_Warn_Warg_struct _tmp30=({struct Cyc_Warn_String_Warn_Warg_struct _tmp253;_tmp253.tag=0,({struct _fat_ptr _tmp2D7=({const char*_tmp35=" used with inconsistent kinds ";_tag_fat(_tmp35,sizeof(char),31U);});_tmp253.f1=_tmp2D7;});_tmp253;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp31=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp252;_tmp252.tag=8,_tmp252.f1=(void*)k1;_tmp252;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32=({struct Cyc_Warn_String_Warn_Warg_struct _tmp251;_tmp251.tag=0,({
struct _fat_ptr _tmp2D8=({const char*_tmp34=" and ";_tag_fat(_tmp34,sizeof(char),6U);});_tmp251.f1=_tmp2D8;});_tmp251;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp33=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp250;_tmp250.tag=8,_tmp250.f1=(void*)k2;_tmp250;});void*_tmp2D[6];_tmp2D[0]=& _tmp2E,_tmp2D[1]=& _tmp2F,_tmp2D[2]=& _tmp30,_tmp2D[3]=& _tmp31,_tmp2D[4]=& _tmp32,_tmp2D[5]=& _tmp33;({unsigned _tmp2D9=loc;Cyc_Warn_err2(_tmp2D9,_tag_fat(_tmp2D,sizeof(void*),6));});});
if(tv->identity == -1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp38=({struct Cyc_Warn_String_Warn_Warg_struct _tmp256;_tmp256.tag=0,({struct _fat_ptr _tmp2DA=({const char*_tmp39="same type variable has different identity!";_tag_fat(_tmp39,sizeof(char),43U);});_tmp256.f1=_tmp2DA;});_tmp256;});void*_tmp37[1];_tmp37[0]=& _tmp38;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp37,sizeof(void*),1));});}
return tvs;}}}
# 180
({int _tmp2DB=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp2DB;});
return({struct Cyc_List_List*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->hd=tv,_tmp3A->tl=tvs;_tmp3A;});}
# 186
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == -1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp257;_tmp257.tag=0,({struct _fat_ptr _tmp2DC=({const char*_tmp3D="fast_add_free_tvar: bad identity in tv";_tag_fat(_tmp3D,sizeof(char),39U);});_tmp257.f1=_tmp2DC;});_tmp257;});void*_tmp3B[1];_tmp3B[0]=& _tmp3C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3B,sizeof(void*),1));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp3E=(struct Cyc_Absyn_Tvar*)tvs2->hd;struct Cyc_Absyn_Tvar*tv2=_tmp3E;
if(tv2->identity == -1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp40=({struct Cyc_Warn_String_Warn_Warg_struct _tmp258;_tmp258.tag=0,({struct _fat_ptr _tmp2DD=({const char*_tmp41="fast_add_free_tvar: bad identity in tvs2";_tag_fat(_tmp41,sizeof(char),41U);});_tmp258.f1=_tmp2DD;});_tmp258;});void*_tmp3F[1];_tmp3F[0]=& _tmp40;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3F,sizeof(void*),1));});
if(tv2->identity == tv->identity)
return tvs;}}
# 196
return({struct Cyc_List_List*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->hd=tv,_tmp42->tl=tvs;_tmp42;});}struct _tuple14{struct Cyc_Absyn_Tvar*f1;int f2;};
# 202
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
if(tv->identity == -1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp259;_tmp259.tag=0,({struct _fat_ptr _tmp2DE=({const char*_tmp45="fast_add_free_tvar_bool: bad identity in tv";_tag_fat(_tmp45,sizeof(char),44U);});_tmp259.f1=_tmp2DE;});_tmp259;});void*_tmp43[1];_tmp43[0]=& _tmp44;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp43,sizeof(void*),1));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct _tuple14*_tmp46=(struct _tuple14*)tvs2->hd;struct _tuple14*_stmttmp5=_tmp46;struct _tuple14*_tmp47=_stmttmp5;void*_tmp49;void*_tmp48;_tmp48=_tmp47->f1;_tmp49=(int*)& _tmp47->f2;{struct Cyc_Absyn_Tvar*tv2=_tmp48;int*b2=(int*)_tmp49;
if(tv2->identity == -1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25A;_tmp25A.tag=0,({struct _fat_ptr _tmp2DF=({const char*_tmp4C="fast_add_free_tvar_bool: bad identity in tvs2";_tag_fat(_tmp4C,sizeof(char),46U);});_tmp25A.f1=_tmp2DF;});_tmp25A;});void*_tmp4A[1];_tmp4A[0]=& _tmp4B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4A,sizeof(void*),1));});
if(tv2->identity == tv->identity){
*b2=*b2 || b;
return tvs;}}}}
# 214
return({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));({struct _tuple14*_tmp2E0=({struct _tuple14*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->f1=tv,_tmp4D->f2=b;_tmp4D;});_tmp4E->hd=_tmp2E0;}),_tmp4E->tl=tvs;_tmp4E;});}
# 221
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tctyp_CVTEnv env){
env.loc=e->loc;
{void*_tmp4F=e->r;void*_stmttmp6=_tmp4F;void*_tmp50=_stmttmp6;void*_tmp53;void*_tmp52;void*_tmp51;switch(*((int*)_tmp50)){case 0:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 33: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 1: _LLA:
 goto _LL0;case 3: _tmp51=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{struct Cyc_List_List*es=_tmp51;
# 230
for(1;es != 0;es=es->tl){
env=Cyc_Tctyp_i_check_type_level_exp((struct Cyc_Absyn_Exp*)es->hd,env);}
goto _LL0;}case 6: _tmp51=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_tmp53=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50)->f3;{struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;struct Cyc_Absyn_Exp*e3=_tmp53;
# 234
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
_tmp51=e2;_tmp52=e3;goto _LL10;}case 7: _tmp51=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
_tmp51=e1;_tmp52=e2;goto _LL12;}case 8: _tmp51=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
_tmp51=e1;_tmp52=e2;goto _LL14;}case 9: _tmp51=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp50)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp51;struct Cyc_Absyn_Exp*e2=_tmp52;
# 239
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type_level_exp(e2,env);
goto _LL0;}case 14: _tmp51=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_tmp52=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50)->f2;{void*t=_tmp51;struct Cyc_Absyn_Exp*e1=_tmp52;
# 243
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tak,t,1,0);
goto _LL0;}case 19: _tmp51=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp50)->f1;{void*t=_tmp51;
_tmp51=t;goto _LL1A;}case 17: _tmp51=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL1A: {void*t=_tmp51;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tak,t,1,0);goto _LL0;}case 39: _tmp51=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp50)->f1;{void*t=_tmp51;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ik,t,1,0);goto _LL0;}case 18: _tmp51=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp50)->f1;{struct Cyc_Absyn_Exp*e=_tmp51;
_tmp51=e;goto _LL20;}case 41: _tmp51=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp50)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp51;
env=Cyc_Tctyp_i_check_type_level_exp(e,env);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp55=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25B;_tmp25B.tag=0,({struct _fat_ptr _tmp2E1=({const char*_tmp56="bad expression in Tctyp::i_check_type_level_exp";_tag_fat(_tmp56,sizeof(char),48U);});_tmp25B.f1=_tmp2E1;});_tmp25B;});void*_tmp54[1];_tmp54[0]=& _tmp55;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp54,sizeof(void*),1));});}_LL0:;}
# 253
return env;}
# 256
static int Cyc_Tctyp_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td == 0)
return 1;
if(td->defn == 0)
return 0;{
void*_tmp57=Cyc_Absyn_compress((void*)_check_null(td->defn));void*_stmttmp7=_tmp57;void*_tmp58=_stmttmp7;void*_tmp59;if(*((int*)_tmp58)== 3){_tmp59=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58)->f1).ptr_atts).rgn;{void*r=_tmp59;
# 263
void*_tmp5A=Cyc_Absyn_compress(r);void*_stmttmp8=_tmp5A;void*_tmp5B=_stmttmp8;void*_tmp5C;if(*((int*)_tmp5B)== 2){_tmp5C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5B)->f1;{struct Cyc_Absyn_Tvar*tv=_tmp5C;
return Cyc_Absyn_tvar_cmp(tvar,tv)== 0;}}else{
return 0;};}}else{
# 267
return 0;};}}
# 271
static struct Cyc_Absyn_Kind*Cyc_Tctyp_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 273
void*_tmp5D=Cyc_Kinds_compress_kb(tvar->kind);void*_stmttmp9=_tmp5D;void*_tmp5E=_stmttmp9;switch(*((int*)_tmp5E)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E)->f2)->aliasqual == Cyc_Absyn_Top)
goto _LL4;else{goto _LL5;}}else{goto _LL5;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E)->f1)->aliasqual == Cyc_Absyn_Top){_LL4: {
# 282
enum Cyc_Absyn_KindQual _tmp5F=expected_kind->kind;enum Cyc_Absyn_KindQual k=_tmp5F;
if((((int)k == 2 ||(int)k == 1)||(int)k == 0)&&
 Cyc_Tctyp_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == 0)
return& Cyc_Kinds_rk;
if((int)expected_kind->aliasqual == 1)
return& Cyc_Kinds_urk;}
# 290
return& Cyc_Kinds_trk;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 return Cyc_Kinds_tvar_kind(tvar,def_kind);};}
# 294
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,struct _tuple1*typname,struct Cyc_Absyn_Typedefdecl*td){
# 299
struct Cyc_List_List*_tmp60=*targsp;struct Cyc_List_List*targs=_tmp60;
for(1;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 305
{struct _tuple0 _tmp61=({struct _tuple0 _tmp25C;({void*_tmp2E2=Cyc_Kinds_compress_kb(tv->kind);_tmp25C.f1=_tmp2E2;}),_tmp25C.f2=t;_tmp25C;});struct _tuple0 _stmttmpA=_tmp61;struct _tuple0 _tmp62=_stmttmpA;void*_tmp63;if(*((int*)_tmp62.f1)== 1){if(*((int*)_tmp62.f2)== 2){_tmp63=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp62.f2)->f1;{struct Cyc_Absyn_Tvar*tv2=_tmp63;
# 307
({struct Cyc_List_List*_tmp2E3=Cyc_Tctyp_add_free_tvar(env.loc,env.kind_env,tv2);env.kind_env=_tmp2E3;});
({struct Cyc_List_List*_tmp2E4=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_tmp2E4;});
continue;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 312
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);
env=Cyc_Tctyp_i_check_type(env,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(env.loc,t);}}
# 316
if(targs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp65=({struct Cyc_Warn_String_Warn_Warg_struct _tmp25E;_tmp25E.tag=0,({struct _fat_ptr _tmp2E5=({const char*_tmp67="too many type arguments for ";_tag_fat(_tmp67,sizeof(char),29U);});_tmp25E.f1=_tmp2E5;});_tmp25E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp66=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp25D;_tmp25D.tag=1,_tmp25D.f1=typname;_tmp25D;});void*_tmp64[2];_tmp64[0]=& _tmp65,_tmp64[1]=& _tmp66;({unsigned _tmp2E6=env.loc;Cyc_Warn_err2(_tmp2E6,_tag_fat(_tmp64,sizeof(void*),2));});});{
# 319
struct Cyc_List_List*hidden_ts=0;
for(1;tvs != 0;tvs=tvs->tl){
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=e,_tmp68->tl=hidden_ts;_tmp68;});{
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Kinds_bk,expected_kind,td);
env=Cyc_Tctyp_i_check_type(env,k,e,1,allow_abs_aggr);}}
# 326
({struct Cyc_List_List*_tmp2E8=({struct Cyc_List_List*_tmp2E7=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp2E7,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmp2E8;});
return env;}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f1;struct Cyc_List_List*f2;};
# 331
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 334
if(clause == 0)
return({struct _tuple15 _tmp25F;_tmp25F.f1=env,_tmp25F.f2=0;_tmp25F;});
Cyc_Tcexp_tcExp(env.te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp263;_tmp263.tag=0,_tmp263.f1=clause_name;_tmp263;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp262;_tmp262.tag=0,({struct _fat_ptr _tmp2E9=({const char*_tmp6F=" clause has type ";_tag_fat(_tmp6F,sizeof(char),18U);});_tmp262.f1=_tmp2E9;});_tmp262;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp261;_tmp261.tag=3,_tmp261.f1=(void*)clause->topt;_tmp261;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp260;_tmp260.tag=0,({
struct _fat_ptr _tmp2EA=({const char*_tmp6E=" instead of integral type";_tag_fat(_tmp6E,sizeof(char),26U);});_tmp260.f1=_tmp2EA;});_tmp260;});void*_tmp69[4];_tmp69[0]=& _tmp6A,_tmp69[1]=& _tmp6B,_tmp69[2]=& _tmp6C,_tmp69[3]=& _tmp6D;({unsigned _tmp2EB=env.loc;Cyc_Warn_err2(_tmp2EB,_tag_fat(_tmp69,sizeof(void*),4));});});
env=Cyc_Tctyp_i_check_type_level_exp(clause,env);{
struct Cyc_List_List*_tmp70=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);struct Cyc_List_List*relns=_tmp70;
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp72=({struct Cyc_Warn_String_Warn_Warg_struct _tmp267;_tmp267.tag=0,_tmp267.f1=clause_name;_tmp267;});struct Cyc_Warn_String_Warn_Warg_struct _tmp73=({struct Cyc_Warn_String_Warn_Warg_struct _tmp266;_tmp266.tag=0,({struct _fat_ptr _tmp2EC=({const char*_tmp77=" clause '";_tag_fat(_tmp77,sizeof(char),10U);});_tmp266.f1=_tmp2EC;});_tmp266;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp74=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp265;_tmp265.tag=4,_tmp265.f1=clause;_tmp265;});struct Cyc_Warn_String_Warn_Warg_struct _tmp75=({struct Cyc_Warn_String_Warn_Warg_struct _tmp264;_tmp264.tag=0,({struct _fat_ptr _tmp2ED=({const char*_tmp76="' may be unsatisfiable";_tag_fat(_tmp76,sizeof(char),23U);});_tmp264.f1=_tmp2ED;});_tmp264;});void*_tmp71[4];_tmp71[0]=& _tmp72,_tmp71[1]=& _tmp73,_tmp71[2]=& _tmp74,_tmp71[3]=& _tmp75;({unsigned _tmp2EE=clause->loc;Cyc_Warn_err2(_tmp2EE,_tag_fat(_tmp71,sizeof(void*),4));});});
return({struct _tuple15 _tmp268;_tmp268.f1=env,_tmp268.f2=relns;_tmp268;});}}
# 348
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 351
struct Cyc_Tctyp_CVTEnv _tmp78=env;void*_tmp7A;unsigned _tmp79;_tmp79=_tmp78.loc;_tmp7A=_tmp78.te;{unsigned loc=_tmp79;struct Cyc_Tcenv_Tenv*te=_tmp7A;
union Cyc_Absyn_AggrInfo _tmp7B=*info;union Cyc_Absyn_AggrInfo _stmttmpB=_tmp7B;union Cyc_Absyn_AggrInfo _tmp7C=_stmttmpB;void*_tmp7F;void*_tmp7D;enum Cyc_Absyn_AggrKind _tmp7E;if((_tmp7C.UnknownAggr).tag == 1){_tmp7E=((_tmp7C.UnknownAggr).val).f1;_tmp7D=((_tmp7C.UnknownAggr).val).f2;_tmp7F=((_tmp7C.UnknownAggr).val).f3;{enum Cyc_Absyn_AggrKind k=_tmp7E;struct _tuple1*n=_tmp7D;struct Cyc_Core_Opt*tgd=_tmp7F;
# 354
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp80;_push_handler(& _tmp80);{int _tmp82=0;if(setjmp(_tmp80.handler))_tmp82=1;if(!_tmp82){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);{
struct Cyc_Absyn_Aggrdecl*_tmp83=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmp83;
if((int)ad->kind != (int)k){
if((int)ad->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp85=({struct Cyc_Warn_String_Warn_Warg_struct _tmp26C;_tmp26C.tag=0,({struct _fat_ptr _tmp2EF=({const char*_tmp8A="expecting struct ";_tag_fat(_tmp8A,sizeof(char),18U);});_tmp26C.f1=_tmp2EF;});_tmp26C;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp86=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp26B;_tmp26B.tag=1,_tmp26B.f1=n;_tmp26B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp87=({struct Cyc_Warn_String_Warn_Warg_struct _tmp26A;_tmp26A.tag=0,({struct _fat_ptr _tmp2F0=({const char*_tmp89="instead of union ";_tag_fat(_tmp89,sizeof(char),18U);});_tmp26A.f1=_tmp2F0;});_tmp26A;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp88=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp269;_tmp269.tag=1,_tmp269.f1=n;_tmp269;});void*_tmp84[4];_tmp84[0]=& _tmp85,_tmp84[1]=& _tmp86,_tmp84[2]=& _tmp87,_tmp84[3]=& _tmp88;({unsigned _tmp2F1=loc;Cyc_Warn_err2(_tmp2F1,_tag_fat(_tmp84,sizeof(void*),4));});});else{
# 362
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp270;_tmp270.tag=0,({struct _fat_ptr _tmp2F2=({const char*_tmp91="expecting union ";_tag_fat(_tmp91,sizeof(char),17U);});_tmp270.f1=_tmp2F2;});_tmp270;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp8D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp26F;_tmp26F.tag=1,_tmp26F.f1=n;_tmp26F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp26E;_tmp26E.tag=0,({struct _fat_ptr _tmp2F3=({const char*_tmp90="instead of struct ";_tag_fat(_tmp90,sizeof(char),19U);});_tmp26E.f1=_tmp2F3;});_tmp26E;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp26D;_tmp26D.tag=1,_tmp26D.f1=n;_tmp26D;});void*_tmp8B[4];_tmp8B[0]=& _tmp8C,_tmp8B[1]=& _tmp8D,_tmp8B[2]=& _tmp8E,_tmp8B[3]=& _tmp8F;({unsigned _tmp2F4=loc;Cyc_Warn_err2(_tmp2F4,_tag_fat(_tmp8B,sizeof(void*),4));});});}}
# 364
if(((unsigned)tgd &&(int)tgd->v)&&(!((unsigned)ad->impl)|| !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp93=({struct Cyc_Warn_String_Warn_Warg_struct _tmp272;_tmp272.tag=0,({struct _fat_ptr _tmp2F5=({const char*_tmp95="@tagged qualfiers don't agree on union ";_tag_fat(_tmp95,sizeof(char),40U);});_tmp272.f1=_tmp2F5;});_tmp272;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp94=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp271;_tmp271.tag=1,_tmp271.f1=n;_tmp271;});void*_tmp92[2];_tmp92[0]=& _tmp93,_tmp92[1]=& _tmp94;({unsigned _tmp2F6=loc;Cyc_Warn_err2(_tmp2F6,_tag_fat(_tmp92,sizeof(void*),2));});});
# 367
({union Cyc_Absyn_AggrInfo _tmp2F7=Cyc_Absyn_KnownAggr(adp);*info=_tmp2F7;});}
# 356
;_pop_handler();}else{void*_tmp81=(void*)Cyc_Core_get_exn_thrown();void*_tmp96=_tmp81;void*_tmp97;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp96)->tag == Cyc_Dict_Absent){
# 371
({struct Cyc_Tcenv_Tenv*_tmp2F9=te;unsigned _tmp2F8=loc;Cyc_Tc_tcAggrdecl(_tmp2F9,_tmp2F8,({struct Cyc_Absyn_Aggrdecl*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->kind=k,_tmp98->sc=3U,_tmp98->name=n,_tmp98->tvs=0,_tmp98->impl=0,_tmp98->attributes=0,_tmp98->expected_mem_kind=0;_tmp98;}));});
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);
({union Cyc_Absyn_AggrInfo _tmp2FA=Cyc_Absyn_KnownAggr(adp);*info=_tmp2FA;});
# 375
if(*targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp275;_tmp275.tag=0,({struct _fat_ptr _tmp2FB=({const char*_tmp9E="declare parameterized type ";_tag_fat(_tmp9E,sizeof(char),28U);});_tmp275.f1=_tmp2FB;});_tmp275;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp274;_tmp274.tag=1,_tmp274.f1=n;_tmp274;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp273;_tmp273.tag=0,({struct _fat_ptr _tmp2FC=({const char*_tmp9D=" before using";_tag_fat(_tmp9D,sizeof(char),14U);});_tmp273.f1=_tmp2FC;});_tmp273;});void*_tmp99[3];_tmp99[0]=& _tmp9A,_tmp99[1]=& _tmp9B,_tmp99[2]=& _tmp9C;({unsigned _tmp2FD=loc;Cyc_Warn_err2(_tmp2FD,_tag_fat(_tmp99,sizeof(void*),3));});});
return env;}
# 379
goto _LL8;}else{_tmp97=_tmp96;{void*exn=_tmp97;(int)_rethrow(exn);}}_LL8:;}}}
# 381
_tmp7D=*adp;goto _LL7;}}else{_tmp7D=*(_tmp7C.KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp7D;
# 383
env=Cyc_Tctyp_check_type_inst(env,targs,ad->tvs,expected_kind,allow_abs_aggr,ad->name,0);
# 385
if((allow_abs_aggr && ad->impl == 0)&& !
Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,expected_kind))
# 390
ad->expected_mem_kind=1;
return env;}};}}
# 396
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 399
struct Cyc_Tctyp_CVTEnv _tmp9F=env;void*_tmpA1;unsigned _tmpA0;_tmpA0=_tmp9F.loc;_tmpA1=_tmp9F.te;{unsigned loc=_tmpA0;struct Cyc_Tcenv_Tenv*te=_tmpA1;
struct Cyc_List_List*_tmpA2=*targsp;struct Cyc_List_List*targs=_tmpA2;
union Cyc_Absyn_DatatypeInfo _tmpA3=*info;union Cyc_Absyn_DatatypeInfo _stmttmpC=_tmpA3;union Cyc_Absyn_DatatypeInfo _tmpA4=_stmttmpC;int _tmpA6;void*_tmpA5;if((_tmpA4.UnknownDatatype).tag == 1){_tmpA5=((_tmpA4.UnknownDatatype).val).name;_tmpA6=((_tmpA4.UnknownDatatype).val).is_extensible;{struct _tuple1*n=_tmpA5;int is_x=_tmpA6;
# 403
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpA7;_push_handler(& _tmpA7);{int _tmpA9=0;if(setjmp(_tmpA7.handler))_tmpA9=1;if(!_tmpA9){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);;_pop_handler();}else{void*_tmpA8=(void*)Cyc_Core_get_exn_thrown();void*_tmpAA=_tmpA8;void*_tmpAB;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpAA)->tag == Cyc_Dict_Absent){
# 407
({struct Cyc_Tcenv_Tenv*_tmp2FF=te;unsigned _tmp2FE=loc;Cyc_Tc_tcDatatypedecl(_tmp2FF,_tmp2FE,({struct Cyc_Absyn_Datatypedecl*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->sc=3U,_tmpAC->name=n,_tmpAC->tvs=0,_tmpAC->fields=0,_tmpAC->is_extensible=is_x;_tmpAC;}));});
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 410
if(targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp278;_tmp278.tag=0,({struct _fat_ptr _tmp300=({const char*_tmpB2="declare parameterized datatype ";_tag_fat(_tmpB2,sizeof(char),32U);});_tmp278.f1=_tmp300;});_tmp278;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp277;_tmp277.tag=1,_tmp277.f1=n;_tmp277;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp276;_tmp276.tag=0,({struct _fat_ptr _tmp301=({const char*_tmpB1=" before using";_tag_fat(_tmpB1,sizeof(char),14U);});_tmp276.f1=_tmp301;});_tmp276;});void*_tmpAD[3];_tmpAD[0]=& _tmpAE,_tmpAD[1]=& _tmpAF,_tmpAD[2]=& _tmpB0;({unsigned _tmp302=loc;Cyc_Warn_err2(_tmp302,_tag_fat(_tmpAD,sizeof(void*),3));});});
return env;}
# 414
goto _LL8;}else{_tmpAB=_tmpAA;{void*exn=_tmpAB;(int)_rethrow(exn);}}_LL8:;}}}
# 418
if(is_x && !(*tudp)->is_extensible)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp27B;_tmp27B.tag=0,({struct _fat_ptr _tmp303=({const char*_tmpB8="datatype ";_tag_fat(_tmpB8,sizeof(char),10U);});_tmp27B.f1=_tmp303;});_tmp27B;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp27A;_tmp27A.tag=1,_tmp27A.f1=n;_tmp27A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp279;_tmp279.tag=0,({struct _fat_ptr _tmp304=({const char*_tmpB7=" was not declared @extensible";_tag_fat(_tmpB7,sizeof(char),30U);});_tmp279.f1=_tmp304;});_tmp279;});void*_tmpB3[3];_tmpB3[0]=& _tmpB4,_tmpB3[1]=& _tmpB5,_tmpB3[2]=& _tmpB6;({unsigned _tmp305=loc;Cyc_Warn_err2(_tmp305,_tag_fat(_tmpB3,sizeof(void*),3));});});
({union Cyc_Absyn_DatatypeInfo _tmp306=Cyc_Absyn_KnownDatatype(tudp);*info=_tmp306;});
_tmpA5=*tudp;goto _LL7;}}else{_tmpA5=*(_tmpA4.KnownDatatype).val;_LL7: {struct Cyc_Absyn_Datatypedecl*tud=_tmpA5;
# 423
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tud->name,0);}};}}
# 429
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 432
union Cyc_Absyn_DatatypeFieldInfo _tmpB9=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmpD=_tmpB9;union Cyc_Absyn_DatatypeFieldInfo _tmpBA=_stmttmpD;int _tmpBD;void*_tmpBC;void*_tmpBB;if((_tmpBA.UnknownDatatypefield).tag == 1){_tmpBB=((_tmpBA.UnknownDatatypefield).val).datatype_name;_tmpBC=((_tmpBA.UnknownDatatypefield).val).field_name;_tmpBD=((_tmpBA.UnknownDatatypefield).val).is_extensible;{struct _tuple1*tname=_tmpBB;struct _tuple1*fname=_tmpBC;int is_x=_tmpBD;
# 435
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(env.te,env.loc,tname);
struct Cyc_Absyn_Datatypefield*tuf;
# 440
{struct Cyc_List_List*_tmpBE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmpBE;for(0;1;fs=fs->tl){
if(fs == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmpC0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp27C;_tmp27C.tag=0,({struct _fat_ptr _tmp307=({const char*_tmpC1="Tcutil found a bad datatypefield";_tag_fat(_tmpC1,sizeof(char),33U);});_tmp27C.f1=_tmp307;});_tmp27C;});void*_tmpBF[1];_tmpBF[0]=& _tmpC0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpBF,sizeof(void*),1));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)fs->hd)->name,fname)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 447
({union Cyc_Absyn_DatatypeFieldInfo _tmp308=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmp308;});
_tmpBB=tud;_tmpBC=tuf;goto _LL4;}}else{_tmpBB=((_tmpBA.KnownDatatypefield).val).f1;_tmpBC=((_tmpBA.KnownDatatypefield).val).f2;_LL4: {struct Cyc_Absyn_Datatypedecl*tud=_tmpBB;struct Cyc_Absyn_Datatypefield*tuf=_tmpBC;
# 450
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tuf->name,0);}};}
# 455
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_unary_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*k,struct Cyc_List_List*ts,int put_in_effect,struct _fat_ptr s){
# 457
if(((int(*)(struct Cyc_List_List*))Cyc_List_length)(ts)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp27E;_tmp27E.tag=0,_tmp27E.f1=s;_tmp27E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp27D;_tmp27D.tag=0,({struct _fat_ptr _tmp309=({const char*_tmpC5=" has wrong number of arguments";_tag_fat(_tmpC5,sizeof(char),31U);});_tmp27D.f1=_tmp309;});_tmp27D;});void*_tmpC2[2];_tmpC2[0]=& _tmpC3,_tmpC2[1]=& _tmpC4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC2,sizeof(void*),2));});
return Cyc_Tctyp_i_check_type(env,k,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,put_in_effect,1);}
# 462
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 465
struct Cyc_Tctyp_CVTEnv _tmpC6=env;void*_tmpC8;unsigned _tmpC7;_tmpC7=_tmpC6.loc;_tmpC8=_tmpC6.te;{unsigned loc=_tmpC7;struct Cyc_Tcenv_Tenv*te=_tmpC8;
struct Cyc_List_List*_tmpC9=*targsp;struct Cyc_List_List*ts=_tmpC9;
void*_tmpCA=c;void*_tmpCC;void*_tmpCB;switch(*((int*)_tmpCA)){case 1:
# 469
 goto _LL7;case 2: _LL7: goto _LL9;case 0: _LL9: goto _LLB;case 7: _LLB:
 goto _LLD;case 6: _LLD: goto _LLF;case 5: _LLF: goto _LL11;case 12: _LL11:
 goto _LL13;case 11: _LL13: goto _LL15;case 14: _LL15: goto _LL17;case 17: _LL17:
# 473
 if(ts != 0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp280;_tmp280.tag=2,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=0,_tmpD1->f1=c,_tmpD1->f2=ts;_tmpD1;})))_tmp30A=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=0,_tmpD1->f1=c,_tmpD1->f2=ts;_tmpD1;}));_tmp280.f1=_tmp30A;});_tmp280;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp27F;_tmp27F.tag=0,({struct _fat_ptr _tmp30B=({const char*_tmpD0=" applied to argument(s)";_tag_fat(_tmpD0,sizeof(char),24U);});_tmp27F.f1=_tmp30B;});_tmp27F;});void*_tmpCD[2];_tmpCD[0]=& _tmpCE,_tmpCD[1]=& _tmpCF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpCD,sizeof(void*),2));});
return env;case 9:
# 477
 for(1;ts != 0;ts=ts->tl){
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ek,(void*)ts->hd,1,1);}
return env;case 15: _tmpCB=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpCA)->f1;_tmpCC=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpCA)->f2;{struct _tuple1*n=_tmpCB;struct Cyc_Absyn_Enumdecl**edp=(struct Cyc_Absyn_Enumdecl**)_tmpCC;
# 481
if(ts != 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmpD3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp281;_tmp281.tag=0,({struct _fat_ptr _tmp30C=({const char*_tmpD4="enum applied to argument(s)";_tag_fat(_tmpD4,sizeof(char),28U);});_tmp281.f1=_tmp30C;});_tmp281;});void*_tmpD2[1];_tmpD2[0]=& _tmpD3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpD2,sizeof(void*),1));});
if(*edp == 0 ||((struct Cyc_Absyn_Enumdecl*)_check_null(*edp))->fields == 0){
struct _handler_cons _tmpD5;_push_handler(& _tmpD5);{int _tmpD7=0;if(setjmp(_tmpD5.handler))_tmpD7=1;if(!_tmpD7){({struct Cyc_Absyn_Enumdecl*_tmp30D=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp30D;});;_pop_handler();}else{void*_tmpD6=(void*)Cyc_Core_get_exn_thrown();void*_tmpD8=_tmpD6;void*_tmpD9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpD8)->tag == Cyc_Dict_Absent){
# 485
({struct Cyc_Tcenv_Tenv*_tmp30F=te;unsigned _tmp30E=loc;Cyc_Tc_tcEnumdecl(_tmp30F,_tmp30E,({struct Cyc_Absyn_Enumdecl*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->sc=3U,_tmpDA->name=n,_tmpDA->fields=0;_tmpDA;}));});
({struct Cyc_Absyn_Enumdecl*_tmp310=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp310;});
goto _LL2E;}else{_tmpD9=_tmpD8;{void*exn=_tmpD9;(int)_rethrow(exn);}}_LL2E:;}}}
# 490
return env;}case 16: _tmpCB=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmpCA)->f1;{struct Cyc_List_List*fs=_tmpCB;
# 492
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp282;_tmp282.tag=0,({struct _fat_ptr _tmp311=({const char*_tmpDD="enum applied to argument(s)";_tag_fat(_tmpDD,sizeof(char),28U);});_tmp282.f1=_tmp311;});_tmp282;});void*_tmpDB[1];_tmpDB[0]=& _tmpDC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpDB,sizeof(void*),1));});{
# 495
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(1;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmpDE=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmpDE;
if(((int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp284;_tmp284.tag=0,({struct _fat_ptr _tmp312=({const char*_tmpE2="duplicate enum field name ";_tag_fat(_tmpE2,sizeof(char),27U);});_tmp284.f1=_tmp312;});_tmp284;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpE1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp283;_tmp283.tag=1,_tmp283.f1=f->name;_tmp283;});void*_tmpDF[2];_tmpDF[0]=& _tmpE0,_tmpDF[1]=& _tmpE1;({unsigned _tmp313=f->loc;Cyc_Warn_err2(_tmp313,_tag_fat(_tmpDF,sizeof(void*),2));});});else{
# 502
prev_fields=({struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->hd=(*f->name).f2,_tmpE3->tl=prev_fields;_tmpE3;});}
if(f->tag == 0)
({struct Cyc_Absyn_Exp*_tmp314=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp314;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp287;_tmp287.tag=0,({struct _fat_ptr _tmp315=({const char*_tmpE9="enum field ";_tag_fat(_tmpE9,sizeof(char),12U);});_tmp287.f1=_tmp315;});_tmp287;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpE6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp286;_tmp286.tag=1,_tmp286.f1=f->name;_tmp286;});struct Cyc_Warn_String_Warn_Warg_struct _tmpE7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp285;_tmp285.tag=0,({struct _fat_ptr _tmp316=({const char*_tmpE8=": expression is not constant";_tag_fat(_tmpE8,sizeof(char),29U);});_tmp285.f1=_tmp316;});_tmp285;});void*_tmpE4[3];_tmpE4[0]=& _tmpE5,_tmpE4[1]=& _tmpE6,_tmpE4[2]=& _tmpE7;({unsigned _tmp317=loc;Cyc_Warn_err2(_tmp317,_tag_fat(_tmpE4,sizeof(void*),3));});});}
tag_count=1U + (Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))).f1;}
# 509
return env;}}case 10:
 return({struct Cyc_Tctyp_CVTEnv _tmp319=env;struct Cyc_List_List*_tmp318=ts;Cyc_Tctyp_i_check_unary_app(_tmp319,& Cyc_Kinds_tak,_tmp318,1,({const char*_tmpEA="regions";_tag_fat(_tmpEA,sizeof(char),8U);}));});case 3:
 return({struct Cyc_Tctyp_CVTEnv _tmp31B=env;struct Cyc_List_List*_tmp31A=ts;Cyc_Tctyp_i_check_unary_app(_tmp31B,& Cyc_Kinds_trk,_tmp31A,1,({const char*_tmpEB="region_t";_tag_fat(_tmpEB,sizeof(char),9U);}));});case 8:
 return({struct Cyc_Tctyp_CVTEnv _tmp31D=env;struct Cyc_List_List*_tmp31C=ts;Cyc_Tctyp_i_check_unary_app(_tmp31D,& Cyc_Kinds_trk,_tmp31C,1,({const char*_tmpEC="access";_tag_fat(_tmpEC,sizeof(char),7U);}));});case 4:
 return({struct Cyc_Tctyp_CVTEnv _tmp31F=env;struct Cyc_List_List*_tmp31E=ts;Cyc_Tctyp_i_check_unary_app(_tmp31F,& Cyc_Kinds_ik,_tmp31E,0,({const char*_tmpED="tag_t";_tag_fat(_tmpED,sizeof(char),6U);}));});case 13:
 return({struct Cyc_Tctyp_CVTEnv _tmp321=env;struct Cyc_List_List*_tmp320=ts;Cyc_Tctyp_i_check_unary_app(_tmp321,& Cyc_Kinds_ik,_tmp320,0,({const char*_tmpEE="@thin";_tag_fat(_tmpEE,sizeof(char),6U);}));});case 20: _tmpCB=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpCA)->f1;{union Cyc_Absyn_AggrInfo*info=_tmpCB;
# 516
return Cyc_Tctyp_i_check_aggr(env,expected_kind,info,targsp,allow_abs_aggr);}case 18: _tmpCB=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmpCA)->f1;{union Cyc_Absyn_DatatypeInfo*info=_tmpCB;
# 518
return Cyc_Tctyp_i_check_datatype(env,expected_kind,info,targsp,allow_abs_aggr);}default: _tmpCB=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpCA)->f1;{union Cyc_Absyn_DatatypeFieldInfo*info=(union Cyc_Absyn_DatatypeFieldInfo*)_tmpCB;
# 520
return Cyc_Tctyp_i_check_datatype_field(env,expected_kind,info,targsp,allow_abs_aggr);}};}}struct _tuple16{enum Cyc_Absyn_Format_Type f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 525
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 527
struct Cyc_Tctyp_CVTEnv _tmpEF=env;void*_tmpF1;unsigned _tmpF0;_tmpF0=_tmpEF.loc;_tmpF1=_tmpEF.te;{unsigned loc=_tmpF0;struct Cyc_Tcenv_Tenv*te=_tmpF1;
{void*_tmpF2=Cyc_Absyn_compress(t);void*_stmttmpE=_tmpF2;void*_tmpF3=_stmttmpE;enum Cyc_Absyn_AggrKind _tmpF8;void*_tmp101;void*_tmp100;void*_tmpFF;void*_tmpFE;void*_tmpFD;int _tmpFA;unsigned _tmp102;void*_tmpFC;void*_tmpFB;void*_tmpF9;void*_tmpF7;void*_tmpF6;void*_tmpF5;void*_tmpF4;switch(*((int*)_tmpF3)){case 0: _tmpF4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF3)->f1;_tmpF5=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF3)->f2;{void*c=_tmpF4;struct Cyc_List_List**targsp=_tmpF5;
# 530
env=Cyc_Tctyp_i_check_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr);
# 532
goto _LL3;}case 1: _tmpF4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF3)->f1;_tmpF5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF3)->f2;{struct Cyc_Core_Opt**k=_tmpF4;void**ref=_tmpF5;
# 535
if(*k == 0 ||
 Cyc_Kinds_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v)&& !
Cyc_Kinds_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v,expected_kind))
({struct Cyc_Core_Opt*_tmp322=Cyc_Kinds_kind_to_opt(expected_kind);*k=_tmp322;});
if(((env.fn_result && env.generalize_evars)&&(int)expected_kind->kind == 3)&& !te->tempest_generalize){
# 541
if((int)expected_kind->aliasqual == 1)
*ref=Cyc_Absyn_unique_rgn_type;else{
# 544
*ref=Cyc_Absyn_heap_rgn_type;}}else{
if((env.generalize_evars &&(int)expected_kind->kind != 6)&&(int)expected_kind->kind != 7){
# 548
struct Cyc_Absyn_Tvar*_tmp103=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=2,_tmp104->f1=0,_tmp104->f2=expected_kind;_tmp104;}));struct Cyc_Absyn_Tvar*v=_tmp103;
({void*_tmp323=Cyc_Absyn_var_type(v);*ref=_tmp323;});
_tmpF4=v;goto _LL9;}else{
# 552
({struct Cyc_List_List*_tmp324=Cyc_Tctyp_add_free_evar(env.free_evars,t,put_in_effect);env.free_evars=_tmp324;});}}
# 554
goto _LL3;}case 2: _tmpF4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpF3)->f1;_LL9: {struct Cyc_Absyn_Tvar*v=_tmpF4;
# 556
{void*_tmp105=Cyc_Kinds_compress_kb(v->kind);void*_stmttmpF=_tmp105;void*_tmp106=_stmttmpF;void*_tmp107;if(*((int*)_tmp106)== 1){_tmp107=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp106)->f1;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_tmp107;
({struct Cyc_Core_Opt*_tmp326=({struct Cyc_Core_Opt*_tmp109=_cycalloc(sizeof(*_tmp109));({void*_tmp325=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->tag=2,_tmp108->f1=0,_tmp108->f2=expected_kind;_tmp108;});_tmp109->v=_tmp325;});_tmp109;});*f=_tmp326;});goto _LL1C;}}else{
goto _LL1C;}_LL1C:;}
# 562
({struct Cyc_List_List*_tmp327=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,v);env.kind_env=_tmp327;});
# 565
({struct Cyc_List_List*_tmp328=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,v,put_in_effect);env.free_vars=_tmp328;});
# 567
{void*_tmp10A=Cyc_Kinds_compress_kb(v->kind);void*_stmttmp10=_tmp10A;void*_tmp10B=_stmttmp10;void*_tmp10D;void*_tmp10C;if(*((int*)_tmp10B)== 2){_tmp10C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10B)->f1;_tmp10D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10B)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_tmp10C;struct Cyc_Absyn_Kind*k=_tmp10D;
# 569
if(Cyc_Kinds_kind_leq(expected_kind,k))
({struct Cyc_Core_Opt*_tmp32A=({struct Cyc_Core_Opt*_tmp10F=_cycalloc(sizeof(*_tmp10F));({void*_tmp329=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=2,_tmp10E->f1=0,_tmp10E->f2=expected_kind;_tmp10E;});_tmp10F->v=_tmp329;});_tmp10F;});*f=_tmp32A;});
goto _LL21;}}else{
goto _LL21;}_LL21:;}
# 574
goto _LL3;}case 10: _tmpF4=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpF3)->f1)->r;_tmpF5=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpF3)->f2;{void*td=_tmpF4;void***topt=_tmpF5;
# 579
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Absyn_compress(t));
{void*_tmp110=td;void*_tmp111;switch(*((int*)_tmp110)){case 0: _tmp111=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp110)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp111;
# 582
if((env.te)->in_extern_c_include)
ad->sc=4U;
Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL26;}case 1: _tmp111=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp110)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp111;
# 586
if((env.te)->in_extern_c_include)
ed->sc=4U;
Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL26;}default: _tmp111=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp110)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp111;
# 590
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL26;}}_LL26:;}
# 592
({void**_tmp32B=({void**_tmp112=_cycalloc(sizeof(*_tmp112));*_tmp112=new_typ;_tmp112;});*topt=_tmp32B;});
return Cyc_Tctyp_i_check_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3: _tmpF4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3)->f1).elt_type;_tmpF5=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3)->f1).elt_tq;_tmpF6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3)->f1).ptr_atts).rgn;_tmpF7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3)->f1).ptr_atts).nullable;_tmpF9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3)->f1).ptr_atts).bounds;_tmpFB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3)->f1).ptr_atts).zero_term;_tmpFC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3)->f1).ptr_atts).released;{void*t1=_tmpF4;struct Cyc_Absyn_Tqual*tqp=_tmpF5;void*rgn_type=_tmpF6;void*nullable=_tmpF7;void*b=_tmpF9;void*zt=_tmpFB;void*rel=_tmpFC;
# 597
struct Cyc_Absyn_Kind*rgnk;
{enum Cyc_Absyn_AliasQual _tmp113=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp11=_tmp113;enum Cyc_Absyn_AliasQual _tmp114=_stmttmp11;switch((int)_tmp114){case Cyc_Absyn_Aliasable:
 rgnk=& Cyc_Kinds_rk;goto _LL2D;case Cyc_Absyn_Unique:
 rgnk=& Cyc_Kinds_urk;goto _LL2D;case Cyc_Absyn_Top:
 goto _LL35;default: _LL35: rgnk=& Cyc_Kinds_trk;goto _LL2D;}_LL2D:;}
# 603
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tak,t1,1,1);
env=Cyc_Tctyp_i_check_type(env,rgnk,rgn_type,1,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,zt,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,nullable,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ptrbk,b,0,allow_abs_aggr);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,rel,0,1);
# 610
({int _tmp32C=Cyc_Tcutil_extract_const_from_typedef(env.loc,tqp->print_const,t1);tqp->real_const=_tmp32C;});
# 614
({void*_tmp32D=zt;Cyc_Unify_unify(_tmp32D,Cyc_Tcutil_is_char_type(t1)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});{
int is_zero_terminated=Cyc_Absyn_type2bool(0,zt);
if(is_zero_terminated && !Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp116=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28A;_tmp28A.tag=0,({struct _fat_ptr _tmp32E=({const char*_tmp11A="cannot have a pointer to zero-terminated ";_tag_fat(_tmp11A,sizeof(char),42U);});_tmp28A.f1=_tmp32E;});_tmp28A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp117=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp289;_tmp289.tag=2,_tmp289.f1=(void*)t1;_tmp289;});struct Cyc_Warn_String_Warn_Warg_struct _tmp118=({struct Cyc_Warn_String_Warn_Warg_struct _tmp288;_tmp288.tag=0,({struct _fat_ptr _tmp32F=({const char*_tmp119=" elements";_tag_fat(_tmp119,sizeof(char),10U);});_tmp288.f1=_tmp32F;});_tmp288;});void*_tmp115[3];_tmp115[0]=& _tmp116,_tmp115[1]=& _tmp117,_tmp115[2]=& _tmp118;({unsigned _tmp330=loc;Cyc_Warn_err2(_tmp330,_tag_fat(_tmp115,sizeof(void*),3));});});{
# 620
struct Cyc_Absyn_Exp*_tmp11B=({void*_tmp331=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp331,b);});struct Cyc_Absyn_Exp*e=_tmp11B;
if(e != 0){
struct _tuple12 _tmp11C=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp12=_tmp11C;struct _tuple12 _tmp11D=_stmttmp12;int _tmp11F;unsigned _tmp11E;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;{unsigned sz=_tmp11E;int known=_tmp11F;
if(is_zero_terminated &&(!known || sz < 1U))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp121=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28B;_tmp28B.tag=0,({struct _fat_ptr _tmp332=({const char*_tmp122="zero-terminated pointer cannot point to empty sequence";_tag_fat(_tmp122,sizeof(char),55U);});_tmp28B.f1=_tmp332;});_tmp28B;});void*_tmp120[1];_tmp120[0]=& _tmp121;({unsigned _tmp333=loc;Cyc_Warn_err2(_tmp333,_tag_fat(_tmp120,sizeof(void*),1));});});}}
# 626
goto _LL3;}}}case 9: _tmpF4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpF3)->f1;{struct Cyc_Absyn_Exp*e=_tmpF4;
# 631
({struct Cyc_Tcenv_Tenv*_tmp334=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp334,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp124=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28C;_tmp28C.tag=0,({struct _fat_ptr _tmp335=({const char*_tmp125="valueof_t requires an int expression";_tag_fat(_tmp125,sizeof(char),37U);});_tmp28C.f1=_tmp335;});_tmp28C;});void*_tmp123[1];_tmp123[0]=& _tmp124;({unsigned _tmp336=loc;Cyc_Warn_err2(_tmp336,_tag_fat(_tmp123,sizeof(void*),1));});});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);
goto _LL3;}case 11: _tmpF4=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmpF3)->f1;{struct Cyc_Absyn_Exp*e=_tmpF4;
# 640
({struct Cyc_Tcenv_Tenv*_tmp337=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp337,0,e);});
goto _LL3;}case 4: _tmpF4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3)->f1).elt_type;_tmpF5=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3)->f1).tq;_tmpF6=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3)->f1).num_elts;_tmpF7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3)->f1).zero_term;_tmp102=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3)->f1).zt_loc;{void*t1=_tmpF4;struct Cyc_Absyn_Tqual*tqp=_tmpF5;struct Cyc_Absyn_Exp**eptr=_tmpF6;void*zt=_tmpF7;unsigned ztl=_tmp102;
# 645
struct Cyc_Absyn_Exp*_tmp126=*eptr;struct Cyc_Absyn_Exp*e=_tmp126;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tmk,t1,1,allow_abs_aggr);
({int _tmp338=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_tmp338;});{
# 649
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);
if(is_zero_terminated){
# 652
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp128=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28F;_tmp28F.tag=0,({struct _fat_ptr _tmp339=({const char*_tmp12C="cannot have a zero-terminated array of ";_tag_fat(_tmp12C,sizeof(char),40U);});_tmp28F.f1=_tmp339;});_tmp28F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp129=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28E;_tmp28E.tag=2,_tmp28E.f1=(void*)t1;_tmp28E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28D;_tmp28D.tag=0,({struct _fat_ptr _tmp33A=({const char*_tmp12B=" elements";_tag_fat(_tmp12B,sizeof(char),10U);});_tmp28D.f1=_tmp33A;});_tmp28D;});void*_tmp127[3];_tmp127[0]=& _tmp128,_tmp127[1]=& _tmp129,_tmp127[2]=& _tmp12A;({unsigned _tmp33B=loc;Cyc_Warn_err2(_tmp33B,_tag_fat(_tmp127,sizeof(void*),3));});});}
# 657
if(e == 0){
# 660
if(!is_zero_terminated)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp290;_tmp290.tag=0,({struct _fat_ptr _tmp33C=({const char*_tmp12F="array bound defaults to 1 here";_tag_fat(_tmp12F,sizeof(char),31U);});_tmp290.f1=_tmp33C;});_tmp290;});void*_tmp12D[1];_tmp12D[0]=& _tmp12E;({unsigned _tmp33D=loc;Cyc_Warn_warn2(_tmp33D,_tag_fat(_tmp12D,sizeof(void*),1));});});
({struct Cyc_Absyn_Exp*_tmp33E=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp33E;});}
# 664
({struct Cyc_Tcenv_Tenv*_tmp33F=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp33F,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp131=({struct Cyc_Warn_String_Warn_Warg_struct _tmp291;_tmp291.tag=0,({struct _fat_ptr _tmp340=({const char*_tmp132="array bounds expression is not an unsigned int";_tag_fat(_tmp132,sizeof(char),47U);});_tmp291.f1=_tmp340;});_tmp291;});void*_tmp130[1];_tmp130[0]=& _tmp131;({unsigned _tmp341=loc;Cyc_Warn_err2(_tmp341,_tag_fat(_tmp130,sizeof(void*),1));});});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);{
struct _tuple12 _tmp133=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp13=_tmp133;struct _tuple12 _tmp134=_stmttmp13;int _tmp136;unsigned _tmp135;_tmp135=_tmp134.f1;_tmp136=_tmp134.f2;{unsigned sz=_tmp135;int known=_tmp136;
# 670
if((is_zero_terminated && known)&& sz < 1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp138=({struct Cyc_Warn_String_Warn_Warg_struct _tmp292;_tmp292.tag=0,({struct _fat_ptr _tmp342=({const char*_tmp139="zero terminated array cannot have zero elements";_tag_fat(_tmp139,sizeof(char),48U);});_tmp292.f1=_tmp342;});_tmp292;});void*_tmp137[1];_tmp137[0]=& _tmp138;({unsigned _tmp343=loc;Cyc_Warn_warn2(_tmp343,_tag_fat(_tmp137,sizeof(void*),1));});});
# 673
if((known && sz < 1U)&& Cyc_Flags_tovc_r){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp293;_tmp293.tag=0,({struct _fat_ptr _tmp344=({const char*_tmp13C="0-element arrays are supported only with gcc, changing to 1";_tag_fat(_tmp13C,sizeof(char),60U);});_tmp293.f1=_tmp344;});_tmp293;});void*_tmp13A[1];_tmp13A[0]=& _tmp13B;({unsigned _tmp345=loc;Cyc_Warn_warn2(_tmp345,_tag_fat(_tmp13A,sizeof(void*),1));});});
({struct Cyc_Absyn_Exp*_tmp346=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp346;});}
# 677
goto _LL3;}}}}case 5: _tmpF4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).tvars;_tmpF5=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).effect;_tmpF6=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).ret_tqual;_tmpF7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).ret_type;_tmpF9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).args;_tmpFA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).c_varargs;_tmpFB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).cyc_varargs;_tmpFC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).rgn_po;_tmpFD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).attributes;_tmpFE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).requires_clause;_tmpFF=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).requires_relns;_tmp100=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).ensures_clause;_tmp101=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF3)->f1).ensures_relns;{struct Cyc_List_List**btvs=_tmpF4;void**eff=_tmpF5;struct Cyc_Absyn_Tqual*rtq=_tmpF6;void*tr=_tmpF7;struct Cyc_List_List*args=_tmpF9;int c_vararg=_tmpFA;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpFB;struct Cyc_List_List*rgn_po=_tmpFC;struct Cyc_List_List*atts=_tmpFD;struct Cyc_Absyn_Exp*req=_tmpFE;struct Cyc_List_List**req_relns=(struct Cyc_List_List**)_tmpFF;struct Cyc_Absyn_Exp*ens=_tmp100;struct Cyc_List_List**ens_relns=(struct Cyc_List_List**)_tmp101;
# 685
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(1;atts != 0;atts=atts->tl){
void*_tmp13D=(void*)atts->hd;void*_stmttmp14=_tmp13D;void*_tmp13E=_stmttmp14;int _tmp141;int _tmp140;enum Cyc_Absyn_Format_Type _tmp13F;switch(*((int*)_tmp13E)){case 1:
 is_stdcall=1;goto _LL3C;case 2:
 is_cdecl=1;goto _LL3C;case 3:
 is_fastcall=1;goto _LL3C;case 19: _tmp13F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp13E)->f1;_tmp140=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp13E)->f2;_tmp141=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp13E)->f3;{enum Cyc_Absyn_Format_Type fmttype=_tmp13F;int i=_tmp140;int j=_tmp141;
# 698
++ num_formats;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;
goto _LL3C;}default:
# 704
 if(!Cyc_Atts_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp143=({struct Cyc_Warn_String_Warn_Warg_struct _tmp295;_tmp295.tag=0,({struct _fat_ptr _tmp347=({const char*_tmp145="bad function type attribute ";_tag_fat(_tmp145,sizeof(char),29U);});_tmp295.f1=_tmp347;});_tmp295;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp144=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp294;_tmp294.tag=10,_tmp294.f1=(void*)atts->hd;_tmp294;});void*_tmp142[2];_tmp142[0]=& _tmp143,_tmp142[1]=& _tmp144;({unsigned _tmp348=loc;Cyc_Warn_err2(_tmp348,_tag_fat(_tmp142,sizeof(void*),2));});});}_LL3C:;}
# 707
if((is_stdcall + is_cdecl)+ is_fastcall > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp147=({struct Cyc_Warn_String_Warn_Warg_struct _tmp296;_tmp296.tag=0,({struct _fat_ptr _tmp349=({const char*_tmp148="function can't have multiple calling conventions";_tag_fat(_tmp148,sizeof(char),49U);});_tmp296.f1=_tmp349;});_tmp296;});void*_tmp146[1];_tmp146[0]=& _tmp147;({unsigned _tmp34A=loc;Cyc_Warn_err2(_tmp34A,_tag_fat(_tmp146,sizeof(void*),1));});});
if(num_formats > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp297;_tmp297.tag=0,({struct _fat_ptr _tmp34B=({const char*_tmp14B="function can't have multiple format attributes";_tag_fat(_tmp14B,sizeof(char),47U);});_tmp297.f1=_tmp34B;});_tmp297;});void*_tmp149[1];_tmp149[0]=& _tmp14A;({unsigned _tmp34C=loc;Cyc_Warn_err2(_tmp34C,_tag_fat(_tmp149,sizeof(void*),1));});});
# 714
Cyc_Tcutil_check_unique_tvars(loc,*btvs);
{struct Cyc_List_List*b=*btvs;for(0;b != 0;b=b->tl){
({int _tmp34D=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmp34D;});
({struct Cyc_List_List*_tmp34E=Cyc_Tctyp_add_bound_tvar(env.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);env.kind_env=_tmp34E;});{
void*_tmp14C=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_stmttmp15=_tmp14C;void*_tmp14D=_stmttmp15;if(*((int*)_tmp14D)== 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14D)->f1)->kind == Cyc_Absyn_MemKind){
# 720
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp299;_tmp299.tag=0,({struct _fat_ptr _tmp34F=({const char*_tmp151="function abstracts Mem type variable ";_tag_fat(_tmp151,sizeof(char),38U);});_tmp299.f1=_tmp34F;});_tmp299;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp150=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp298;_tmp298.tag=7,_tmp298.f1=(struct Cyc_Absyn_Tvar*)b->hd;_tmp298;});void*_tmp14E[2];_tmp14E[0]=& _tmp14F,_tmp14E[1]=& _tmp150;({unsigned _tmp350=loc;Cyc_Warn_err2(_tmp350,_tag_fat(_tmp14E,sizeof(void*),2));});});goto _LL47;}else{goto _LL4A;}}else{_LL4A:
 goto _LL47;}_LL47:;}}}{
# 726
struct Cyc_Tctyp_CVTEnv new_env=({struct Cyc_Tctyp_CVTEnv _tmp2AA;_tmp2AA.loc=loc,_tmp2AA.te=te,_tmp2AA.kind_env=env.kind_env,_tmp2AA.fn_result=1,_tmp2AA.generalize_evars=env.generalize_evars,_tmp2AA.free_vars=0,_tmp2AA.free_evars=0;_tmp2AA;});
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_tmk,tr,1,1);
({int _tmp351=Cyc_Tcutil_extract_const_from_typedef(loc,rtq->print_const,tr);rtq->real_const=_tmp351;});
new_env.fn_result=0;
# 732
{struct Cyc_List_List*a=args;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp152=(struct _tuple9*)a->hd;struct _tuple9*trip=_tmp152;
void*_tmp153=(*trip).f3;void*t=_tmp153;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_tmk,t,1,1);{
int _tmp154=Cyc_Tcutil_extract_const_from_typedef(loc,((*trip).f2).print_const,t);int ec=_tmp154;
((*trip).f2).real_const=ec;
# 740
if(Cyc_Tcutil_is_array_type(t)){
# 742
void*_tmp155=Cyc_Absyn_new_evar(0,0);void*ptr_rgn=_tmp155;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_rk,ptr_rgn,1,1);
({void*_tmp352=Cyc_Tcutil_promote_array(t,ptr_rgn,0);(*trip).f3=_tmp352;});}}}}
# 748
if(cyc_vararg != 0){
if(c_vararg)({struct Cyc_Warn_String_Warn_Warg_struct _tmp157=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29A;_tmp29A.tag=0,({struct _fat_ptr _tmp353=({const char*_tmp158="both c_vararg and cyc_vararg";_tag_fat(_tmp158,sizeof(char),29U);});_tmp29A.f1=_tmp353;});_tmp29A;});void*_tmp156[1];_tmp156[0]=& _tmp157;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp156,sizeof(void*),1));});{
struct Cyc_Absyn_VarargInfo _tmp159=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp16=_tmp159;struct Cyc_Absyn_VarargInfo _tmp15A=_stmttmp16;int _tmp15C;void*_tmp15B;_tmp15B=_tmp15A.type;_tmp15C=_tmp15A.inject;{void*vt=_tmp15B;int vi=_tmp15C;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_tmk,vt,1,1);
({int _tmp354=Cyc_Tcutil_extract_const_from_typedef(loc,(cyc_vararg->tq).print_const,vt);(cyc_vararg->tq).real_const=_tmp354;});
# 754
if(vi){
void*_tmp15D=Cyc_Absyn_compress(vt);void*_stmttmp17=_tmp15D;void*_tmp15E=_stmttmp17;void*_tmp161;void*_tmp160;void*_tmp15F;if(*((int*)_tmp15E)== 3){_tmp15F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15E)->f1).elt_type;_tmp160=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15E)->f1).ptr_atts).bounds;_tmp161=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15E)->f1).ptr_atts).zero_term;{void*et=_tmp15F;void*bs=_tmp160;void*zt=_tmp161;
# 757
{void*_tmp162=Cyc_Absyn_compress(et);void*_stmttmp18=_tmp162;void*_tmp163=_stmttmp18;if(*((int*)_tmp163)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp163)->f1)== 18){
# 759
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp165=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29B;_tmp29B.tag=0,({struct _fat_ptr _tmp355=({const char*_tmp166="can't inject into a zeroterm pointer";_tag_fat(_tmp166,sizeof(char),37U);});_tmp29B.f1=_tmp355;});_tmp29B;});void*_tmp164[1];_tmp164[0]=& _tmp165;({unsigned _tmp356=loc;Cyc_Warn_err2(_tmp356,_tag_fat(_tmp164,sizeof(void*),1));});});
if(!({void*_tmp357=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp357,bs);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp168=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29C;_tmp29C.tag=0,({struct _fat_ptr _tmp358=({const char*_tmp169="can't inject into a fat pointer to datatype";_tag_fat(_tmp169,sizeof(char),44U);});_tmp29C.f1=_tmp358;});_tmp29C;});void*_tmp167[1];_tmp167[0]=& _tmp168;({unsigned _tmp359=loc;Cyc_Warn_err2(_tmp359,_tag_fat(_tmp167,sizeof(void*),1));});});
goto _LL54;}else{goto _LL57;}}else{_LL57:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29D;_tmp29D.tag=0,({struct _fat_ptr _tmp35A=({const char*_tmp16C="can't inject a non-datatype type";_tag_fat(_tmp16C,sizeof(char),33U);});_tmp29D.f1=_tmp35A;});_tmp29D;});void*_tmp16A[1];_tmp16A[0]=& _tmp16B;({unsigned _tmp35B=loc;Cyc_Warn_err2(_tmp35B,_tag_fat(_tmp16A,sizeof(void*),1));});});goto _LL54;}_LL54:;}
# 766
goto _LL4F;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E;_tmp29E.tag=0,({struct _fat_ptr _tmp35C=({const char*_tmp16F="expecting a datatype pointer type";_tag_fat(_tmp16F,sizeof(char),34U);});_tmp29E.f1=_tmp35C;});_tmp29E;});void*_tmp16D[1];_tmp16D[0]=& _tmp16E;({unsigned _tmp35D=loc;Cyc_Warn_err2(_tmp35D,_tag_fat(_tmp16D,sizeof(void*),1));});});goto _LL4F;}_LL4F:;}}}}
# 771
if(num_formats > 0){
int _tmp170=((int(*)(struct Cyc_List_List*))Cyc_List_length)(args);int num_args=_tmp170;
if((((fmt_desc_arg < 0 || fmt_desc_arg > num_args)|| fmt_arg_start < 0)||
# 775
(cyc_vararg == 0 && !c_vararg)&& fmt_arg_start != 0)||
(cyc_vararg != 0 || c_vararg)&& fmt_arg_start != num_args + 1)
# 778
({struct Cyc_Warn_String_Warn_Warg_struct _tmp172=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29F;_tmp29F.tag=0,({struct _fat_ptr _tmp35E=({const char*_tmp173="bad format descriptor";_tag_fat(_tmp173,sizeof(char),22U);});_tmp29F.f1=_tmp35E;});_tmp29F;});void*_tmp171[1];_tmp171[0]=& _tmp172;({unsigned _tmp35F=loc;Cyc_Warn_err2(_tmp35F,_tag_fat(_tmp171,sizeof(void*),1));});});else{
# 781
struct _tuple9 _tmp174=*((struct _tuple9*(*)(struct Cyc_List_List*,int))Cyc_List_nth)(args,fmt_desc_arg - 1);struct _tuple9 _stmttmp19=_tmp174;struct _tuple9 _tmp175=_stmttmp19;void*_tmp176;_tmp176=_tmp175.f3;{void*t=_tmp176;
# 783
{void*_tmp177=Cyc_Absyn_compress(t);void*_stmttmp1A=_tmp177;void*_tmp178=_stmttmp1A;void*_tmp17A;void*_tmp179;if(*((int*)_tmp178)== 3){_tmp179=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).elt_type;_tmp17A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).ptr_atts).bounds;{void*et=_tmp179;void*b=_tmp17A;
# 786
if(!Cyc_Tcutil_is_char_type(et))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A0;_tmp2A0.tag=0,({struct _fat_ptr _tmp360=({const char*_tmp17D="format descriptor is not a string";_tag_fat(_tmp17D,sizeof(char),34U);});_tmp2A0.f1=_tmp360;});_tmp2A0;});void*_tmp17B[1];_tmp17B[0]=& _tmp17C;({unsigned _tmp361=loc;Cyc_Warn_err2(_tmp361,_tag_fat(_tmp17B,sizeof(void*),1));});});else{
# 789
struct Cyc_Absyn_Exp*_tmp17E=({void*_tmp362=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp362,b);});struct Cyc_Absyn_Exp*e=_tmp17E;
if(e == 0 && c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp180=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A1;_tmp2A1.tag=0,({struct _fat_ptr _tmp363=({const char*_tmp181="format descriptor is not a char * type";_tag_fat(_tmp181,sizeof(char),39U);});_tmp2A1.f1=_tmp363;});_tmp2A1;});void*_tmp17F[1];_tmp17F[0]=& _tmp180;({unsigned _tmp364=loc;Cyc_Warn_err2(_tmp364,_tag_fat(_tmp17F,sizeof(void*),1));});});else{
if(e != 0 && !c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp183=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A2;_tmp2A2.tag=0,({struct _fat_ptr _tmp365=({const char*_tmp184="format descriptor is not a char ? type";_tag_fat(_tmp184,sizeof(char),39U);});_tmp2A2.f1=_tmp365;});_tmp2A2;});void*_tmp182[1];_tmp182[0]=& _tmp183;({unsigned _tmp366=loc;Cyc_Warn_err2(_tmp366,_tag_fat(_tmp182,sizeof(void*),1));});});}}
# 795
goto _LL5C;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp186=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A3;_tmp2A3.tag=0,({struct _fat_ptr _tmp367=({const char*_tmp187="format descriptor is not a string type";_tag_fat(_tmp187,sizeof(char),39U);});_tmp2A3.f1=_tmp367;});_tmp2A3;});void*_tmp185[1];_tmp185[0]=& _tmp186;({unsigned _tmp368=loc;Cyc_Warn_err2(_tmp368,_tag_fat(_tmp185,sizeof(void*),1));});});goto _LL5C;}_LL5C:;}
# 798
if(fmt_arg_start != 0 && !c_vararg){
# 802
int problem;
{struct _tuple16 _tmp188=({struct _tuple16 _tmp2A4;_tmp2A4.f1=ft,({void*_tmp369=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_vararg))->type));_tmp2A4.f2=_tmp369;});_tmp2A4;});struct _tuple16 _stmttmp1B=_tmp188;struct _tuple16 _tmp189=_stmttmp1B;void*_tmp18A;switch((int)_tmp189.f1){case Cyc_Absyn_Printf_ft: if(*((int*)_tmp189.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp189.f2)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp189.f2)->f1)->f1).KnownDatatype).tag == 2){_tmp18A=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp189.f2)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*tud=_tmp18A;
# 805
problem=({struct _tuple1*_tmp36A=tud->name;Cyc_Absyn_qvar_cmp(_tmp36A,Cyc_Absyn_datatype_print_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}case Cyc_Absyn_Scanf_ft: if(*((int*)_tmp189.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp189.f2)->f1)== 18){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp189.f2)->f1)->f1).KnownDatatype).tag == 2){_tmp18A=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp189.f2)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl*tud=_tmp18A;
# 808
problem=({struct _tuple1*_tmp36B=tud->name;Cyc_Absyn_qvar_cmp(_tmp36B,Cyc_Absyn_datatype_scanf_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}default: _LL66:
# 811
 problem=1;
goto _LL61;}_LL61:;}
# 814
if(problem)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5;_tmp2A5.tag=0,({struct _fat_ptr _tmp36C=({const char*_tmp18D="format attribute and vararg types don't match";_tag_fat(_tmp18D,sizeof(char),46U);});_tmp2A5.f1=_tmp36C;});_tmp2A5;});void*_tmp18B[1];_tmp18B[0]=& _tmp18C;({unsigned _tmp36D=loc;Cyc_Warn_err2(_tmp36D,_tag_fat(_tmp18B,sizeof(void*),1));});});}}}}
# 822
{struct Cyc_List_List*rpo=rgn_po;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp18E=(struct _tuple0*)rpo->hd;struct _tuple0*_stmttmp1C=_tmp18E;struct _tuple0*_tmp18F=_stmttmp1C;void*_tmp191;void*_tmp190;_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;{void*r1=_tmp190;void*r2=_tmp191;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,r1,1,1);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_trk,r2,1,1);}}}{
# 830
struct Cyc_Tcenv_Fenv*_tmp192=Cyc_Tcenv_bogus_fenv(tr,args);struct Cyc_Tcenv_Fenv*fenv=_tmp192;
struct Cyc_Tcenv_Tenv*_tmp193=new_env.te;struct Cyc_Tcenv_Tenv*old_te=_tmp193;
({struct Cyc_Tcenv_Tenv*_tmp36E=({struct Cyc_Tcenv_Tenv*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->ns=te->ns,_tmp194->ae=te->ae,_tmp194->le=fenv,_tmp194->allow_valueof=1,_tmp194->in_extern_c_include=te->in_extern_c_include,_tmp194->in_tempest=te->in_tempest,_tmp194->tempest_generalize=te->tempest_generalize,_tmp194->in_extern_c_inc_repeat=te->in_extern_c_inc_repeat;_tmp194;});new_env.te=_tmp36E;});{
# 836
struct _tuple15 _tmp195=({struct Cyc_Tctyp_CVTEnv _tmp370=new_env;struct _fat_ptr _tmp36F=({const char*_tmp1CA="@requires";_tag_fat(_tmp1CA,sizeof(char),10U);});Cyc_Tctyp_check_clause(_tmp370,_tmp36F,req);});struct _tuple15 _stmttmp1D=_tmp195;struct _tuple15 _tmp196=_stmttmp1D;void*_tmp198;struct Cyc_Tctyp_CVTEnv _tmp197;_tmp197=_tmp196.f1;_tmp198=_tmp196.f2;{struct Cyc_Tctyp_CVTEnv nenv=_tmp197;struct Cyc_List_List*req_rs=_tmp198;
new_env=nenv;
*req_relns=req_rs;
((void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,req_rs);{
struct _tuple15 _tmp199=({struct Cyc_Tctyp_CVTEnv _tmp372=new_env;struct _fat_ptr _tmp371=({const char*_tmp1C9="@ensures";_tag_fat(_tmp1C9,sizeof(char),9U);});Cyc_Tctyp_check_clause(_tmp372,_tmp371,ens);});struct _tuple15 _stmttmp1E=_tmp199;struct _tuple15 _tmp19A=_stmttmp1E;void*_tmp19C;struct Cyc_Tctyp_CVTEnv _tmp19B;_tmp19B=_tmp19A.f1;_tmp19C=_tmp19A.f2;{struct Cyc_Tctyp_CVTEnv nenv=_tmp19B;struct Cyc_List_List*ens_rs=_tmp19C;
new_env=nenv;
*ens_relns=ens_rs;
((void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,ens_rs);
new_env.te=old_te;
# 846
if(*eff != 0)
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,(void*)_check_null(*eff),1,1);else{
# 849
struct Cyc_List_List*effect=0;
# 854
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp19D=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp1F=_tmp19D;struct _tuple14 _tmp19E=_stmttmp1F;int _tmp1A0;void*_tmp19F;_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;{struct Cyc_Absyn_Tvar*tv=_tmp19F;int put_in_eff=_tmp1A0;
if(!put_in_eff)continue;{
void*_tmp1A1=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp20=_tmp1A1;void*_tmp1A2=_stmttmp20;void*_tmp1A4;void*_tmp1A3;switch(*((int*)_tmp1A2)){case 2: _tmp1A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A2)->f1;_tmp1A4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A2)->f2;if((int)((struct Cyc_Absyn_Kind*)_tmp1A4)->kind == 3){struct Cyc_Core_Opt**f=_tmp1A3;struct Cyc_Absyn_Kind*r=_tmp1A4;
# 860
if((int)r->aliasqual == 2)
({struct Cyc_Core_Opt*_tmp373=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_rk);*f=_tmp373;});else{
# 863
({struct Cyc_Core_Opt*_tmp374=Cyc_Kinds_kind_to_bound_opt(r);*f=_tmp374;});}
_tmp1A3=r;goto _LL78;}else{switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A2)->f2)->kind){case Cyc_Absyn_BoolKind:
# 867
 goto _LL7C;case Cyc_Absyn_PtrBndKind: _LL7C:
 goto _LL7E;case Cyc_Absyn_IntKind: _LL7E:
 goto _LL80;case Cyc_Absyn_EffKind: _tmp1A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A2)->f1;{struct Cyc_Core_Opt**f=_tmp1A3;
# 874
({struct Cyc_Core_Opt*_tmp375=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ek);*f=_tmp375;});goto _LL88;}default: goto _LL8B;}}case 0: _tmp1A3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1A2)->f1;if((int)((struct Cyc_Absyn_Kind*)_tmp1A3)->kind == 3){_LL78: {struct Cyc_Absyn_Kind*r=_tmp1A3;
# 866
effect=({struct Cyc_List_List*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));({void*_tmp376=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(tv));_tmp1A5->hd=_tmp376;}),_tmp1A5->tl=effect;_tmp1A5;});goto _LL74;}}else{switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1A2)->f1)->kind){case Cyc_Absyn_BoolKind: _LL80:
# 870
 goto _LL82;case Cyc_Absyn_PtrBndKind: _LL82:
 goto _LL84;case Cyc_Absyn_IntKind: _LL84:
 goto _LL74;case Cyc_Absyn_EffKind: _LL88:
# 876
 effect=({struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));({void*_tmp377=Cyc_Absyn_var_type(tv);_tmp1A6->hd=_tmp377;}),_tmp1A6->tl=effect;_tmp1A6;});goto _LL74;default: _LL8B: _LL8C:
# 881
 effect=({struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));({void*_tmp378=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(tv));_tmp1A9->hd=_tmp378;}),_tmp1A9->tl=effect;_tmp1A9;});goto _LL74;}}default: _tmp1A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1A2)->f1;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_tmp1A3;
# 878
({struct Cyc_Core_Opt*_tmp37A=({struct Cyc_Core_Opt*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));({void*_tmp379=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->tag=2,_tmp1A7->f1=0,_tmp1A7->f2=& Cyc_Kinds_ak;_tmp1A7;});_tmp1A8->v=_tmp379;});_tmp1A8;});*f=_tmp37A;});goto _LL8C;}}_LL74:;}}}}
# 885
{struct Cyc_List_List*ts=new_env.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple13 _tmp1AA=*((struct _tuple13*)ts->hd);struct _tuple13 _stmttmp21=_tmp1AA;struct _tuple13 _tmp1AB=_stmttmp21;int _tmp1AD;void*_tmp1AC;_tmp1AC=_tmp1AB.f1;_tmp1AD=_tmp1AB.f2;{void*tv=_tmp1AC;int put_in_eff=_tmp1AD;
if(!put_in_eff)continue;{
struct Cyc_Absyn_Kind*_tmp1AE=Cyc_Tcutil_type_kind(tv);struct Cyc_Absyn_Kind*_stmttmp22=_tmp1AE;struct Cyc_Absyn_Kind*_tmp1AF=_stmttmp22;switch((int)((struct Cyc_Absyn_Kind*)_tmp1AF)->kind){case Cyc_Absyn_RgnKind:
 effect=({struct Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({void*_tmp37B=Cyc_Absyn_access_eff(tv);_tmp1B0->hd=_tmp37B;}),_tmp1B0->tl=effect;_tmp1B0;});goto _LL90;case Cyc_Absyn_EffKind:
 effect=({struct Cyc_List_List*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->hd=tv,_tmp1B1->tl=effect;_tmp1B1;});goto _LL90;case Cyc_Absyn_IntKind:
 goto _LL90;default:
 effect=({struct Cyc_List_List*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));({void*_tmp37C=Cyc_Absyn_regionsof_eff(tv);_tmp1B2->hd=_tmp37C;}),_tmp1B2->tl=effect;_tmp1B2;});goto _LL90;}_LL90:;}}}}
# 895
({void*_tmp37D=Cyc_Absyn_join_eff(effect);*eff=_tmp37D;});}
# 901
if(*btvs != 0){
struct Cyc_List_List*bs=*btvs;for(0;bs != 0;bs=bs->tl){
void*_tmp1B3=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_stmttmp23=_tmp1B3;void*_tmp1B4=_stmttmp23;void*_tmp1B6;void*_tmp1B5;switch(*((int*)_tmp1B4)){case 1: _tmp1B5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1B4)->f1;{struct Cyc_Core_Opt**f=_tmp1B5;
# 905
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A8;_tmp2A8.tag=0,({struct _fat_ptr _tmp37E=({const char*_tmp1BC="type variable ";_tag_fat(_tmp1BC,sizeof(char),15U);});_tmp2A8.f1=_tmp37E;});_tmp2A8;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp1B9=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2A7;_tmp2A7.tag=7,_tmp2A7.f1=(struct Cyc_Absyn_Tvar*)bs->hd;_tmp2A7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A6;_tmp2A6.tag=0,({struct _fat_ptr _tmp37F=({const char*_tmp1BB=" unconstrained, assuming boxed";_tag_fat(_tmp1BB,sizeof(char),31U);});_tmp2A6.f1=_tmp37F;});_tmp2A6;});void*_tmp1B7[3];_tmp1B7[0]=& _tmp1B8,_tmp1B7[1]=& _tmp1B9,_tmp1B7[2]=& _tmp1BA;({unsigned _tmp380=loc;Cyc_Warn_warn2(_tmp380,_tag_fat(_tmp1B7,sizeof(void*),3));});});
({struct Cyc_Core_Opt*_tmp381=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_tmp381;});
goto _LL99;}case 2: _tmp1B5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1B4)->f1;_tmp1B6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1B4)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_tmp1B5;struct Cyc_Absyn_Kind*k=_tmp1B6;
# 909
{struct Cyc_Absyn_Kind*_tmp1BD=k;switch((int)((struct Cyc_Absyn_Kind*)_tmp1BD)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp1BD)->aliasqual == Cyc_Absyn_Top)
goto _LLA6;else{goto _LLB3;}case Cyc_Absyn_MemKind: switch((int)((struct Cyc_Absyn_Kind*)_tmp1BD)->aliasqual){case Cyc_Absyn_Top: _LLA6:
 goto _LLA8;case Cyc_Absyn_Aliasable: _LLA8:
 goto _LLAA;case Cyc_Absyn_Unique:
# 916
 goto _LLB0;default: goto _LLB3;}case Cyc_Absyn_AnyKind: switch((int)((struct Cyc_Absyn_Kind*)_tmp1BD)->aliasqual){case Cyc_Absyn_Top: _LLAA:
# 913
 goto _LLAC;case Cyc_Absyn_Aliasable: _LLAC:
# 915
({struct Cyc_Core_Opt*_tmp382=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_tmp382;});goto _LLA2;case Cyc_Absyn_Unique: _LLB0:
# 918
({struct Cyc_Core_Opt*_tmp383=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ubk);*f=_tmp383;});goto _LLA2;default: goto _LLB3;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp1BD)->aliasqual == Cyc_Absyn_Top){
# 920
({struct Cyc_Core_Opt*_tmp384=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_rk);*f=_tmp384;});goto _LLA2;}else{goto _LLB3;}default: _LLB3:
# 922
({struct Cyc_Core_Opt*_tmp385=Cyc_Kinds_kind_to_bound_opt(k);*f=_tmp385;});goto _LLA2;}_LLA2:;}
# 924
goto _LL99;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1B4)->f1)->kind == Cyc_Absyn_MemKind){
# 926
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A9;_tmp2A9.tag=0,({struct _fat_ptr _tmp386=({const char*_tmp1C0="functions can't abstract M types";_tag_fat(_tmp1C0,sizeof(char),33U);});_tmp2A9.f1=_tmp386;});_tmp2A9;});void*_tmp1BE[1];_tmp1BE[0]=& _tmp1BF;({unsigned _tmp387=loc;Cyc_Warn_err2(_tmp387,_tag_fat(_tmp1BE,sizeof(void*),1));});});goto _LL99;}else{
goto _LL99;}}_LL99:;}}
# 930
({struct Cyc_List_List*_tmp388=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*)))Cyc_Tctyp_remove_bound_tvars)(new_env.kind_env,*btvs,(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))Cyc_Core_identity);env.kind_env=_tmp388;});
({struct Cyc_List_List*_tmp389=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars)(new_env.free_vars,*btvs,(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst);new_env.free_vars=_tmp389;});
# 933
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp1C1=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp24=_tmp1C1;struct _tuple14 _tmp1C2=_stmttmp24;int _tmp1C4;void*_tmp1C3;_tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;{struct Cyc_Absyn_Tvar*t=_tmp1C3;int b=_tmp1C4;
({struct Cyc_List_List*_tmp38A=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,t,b);env.free_vars=_tmp38A;});}}}
# 938
{struct Cyc_List_List*evs=new_env.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp1C5=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp25=_tmp1C5;struct _tuple13 _tmp1C6=_stmttmp25;int _tmp1C8;void*_tmp1C7;_tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;{void*e=_tmp1C7;int b=_tmp1C8;
({struct Cyc_List_List*_tmp38B=Cyc_Tctyp_add_free_evar(env.free_evars,e,b);env.free_evars=_tmp38B;});}}}
# 942
goto _LL3;}}}}}}}case 6: _tmpF4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF3)->f1;{struct Cyc_List_List*tq_ts=_tmpF4;
# 945
for(1;tq_ts != 0;tq_ts=tq_ts->tl){
struct _tuple17*_tmp1CB=(struct _tuple17*)tq_ts->hd;struct _tuple17*p=_tmp1CB;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tmk,(*p).f2,1,0);
({int _tmp38C=
Cyc_Tcutil_extract_const_from_typedef(loc,((*p).f1).print_const,(*p).f2);
# 948
((*p).f1).real_const=_tmp38C;});}
# 951
goto _LL3;}case 7: _tmpF8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF3)->f1;_tmpF4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF3)->f2;{enum Cyc_Absyn_AggrKind k=_tmpF8;struct Cyc_List_List*fs=_tmpF4;
# 955
struct Cyc_List_List*prev_fields=0;
for(1;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp1CC=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp26=_tmp1CC;struct Cyc_Absyn_Aggrfield*_tmp1CD=_stmttmp26;void*_tmp1D3;void*_tmp1D2;void*_tmp1D1;void*_tmp1D0;void*_tmp1CF;void*_tmp1CE;_tmp1CE=_tmp1CD->name;_tmp1CF=(struct Cyc_Absyn_Tqual*)& _tmp1CD->tq;_tmp1D0=_tmp1CD->type;_tmp1D1=_tmp1CD->width;_tmp1D2=_tmp1CD->attributes;_tmp1D3=_tmp1CD->requires_clause;{struct _fat_ptr*fn=_tmp1CE;struct Cyc_Absyn_Tqual*tqp=(struct Cyc_Absyn_Tqual*)_tmp1CF;void*t=_tmp1D0;struct Cyc_Absyn_Exp*width=_tmp1D1;struct Cyc_List_List*atts=_tmp1D2;struct Cyc_Absyn_Exp*requires_clause=_tmp1D3;
if(((int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AC;_tmp2AC.tag=0,({struct _fat_ptr _tmp38D=({const char*_tmp1D7="duplicate field ";_tag_fat(_tmp1D7,sizeof(char),17U);});_tmp2AC.f1=_tmp38D;});_tmp2AC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AB;_tmp2AB.tag=0,_tmp2AB.f1=*fn;_tmp2AB;});void*_tmp1D4[2];_tmp1D4[0]=& _tmp1D5,_tmp1D4[1]=& _tmp1D6;({unsigned _tmp38E=loc;Cyc_Warn_err2(_tmp38E,_tag_fat(_tmp1D4,sizeof(void*),2));});});
if(({struct _fat_ptr _tmp38F=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp38F,({const char*_tmp1D8="";_tag_fat(_tmp1D8,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->hd=fn,_tmp1D9->tl=prev_fields;_tmp1D9;});
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tmk,t,1,0);
({int _tmp390=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t);tqp->real_const=_tmp390;});
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Atts_check_field_atts(loc,fn,atts);
if(requires_clause != 0){
# 968
if((int)k != 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AD;_tmp2AD.tag=0,({struct _fat_ptr _tmp391=({const char*_tmp1DC="@requires clause is only allowed on union members";_tag_fat(_tmp1DC,sizeof(char),50U);});_tmp2AD.f1=_tmp391;});_tmp2AD;});void*_tmp1DA[1];_tmp1DA[0]=& _tmp1DB;({unsigned _tmp392=loc;Cyc_Warn_err2(_tmp392,_tag_fat(_tmp1DA,sizeof(void*),1));});});
({struct Cyc_Tcenv_Tenv*_tmp393=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp393,0,requires_clause);});
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B0;_tmp2B0.tag=0,({struct _fat_ptr _tmp394=({const char*_tmp1E2="@requires clause has type ";_tag_fat(_tmp1E2,sizeof(char),27U);});_tmp2B0.f1=_tmp394;});_tmp2B0;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1DF=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp2AF;_tmp2AF.tag=3,_tmp2AF.f1=(void*)requires_clause->topt;_tmp2AF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AE;_tmp2AE.tag=0,({
struct _fat_ptr _tmp395=({const char*_tmp1E1=" instead of integral type";_tag_fat(_tmp1E1,sizeof(char),26U);});_tmp2AE.f1=_tmp395;});_tmp2AE;});void*_tmp1DD[3];_tmp1DD[0]=& _tmp1DE,_tmp1DD[1]=& _tmp1DF,_tmp1DD[2]=& _tmp1E0;({unsigned _tmp396=loc;Cyc_Warn_err2(_tmp396,_tag_fat(_tmp1DD,sizeof(void*),3));});});
env=Cyc_Tctyp_i_check_type_level_exp(requires_clause,env);}}}
# 977
goto _LL3;}default: _tmpF4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpF3)->f1;_tmpF5=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpF3)->f2;_tmpF6=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpF3)->f3;_tmpF7=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpF3)->f4;{struct _tuple1*tdn=_tmpF4;struct Cyc_List_List**targs_ref=(struct Cyc_List_List**)_tmpF5;struct Cyc_Absyn_Typedefdecl**tdopt=(struct Cyc_Absyn_Typedefdecl**)_tmpF6;void**toptp=(void**)_tmpF7;
# 981
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp1E3;_push_handler(& _tmp1E3);{int _tmp1E5=0;if(setjmp(_tmp1E3.handler))_tmp1E5=1;if(!_tmp1E5){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);;_pop_handler();}else{void*_tmp1E4=(void*)Cyc_Core_get_exn_thrown();void*_tmp1E6=_tmp1E4;void*_tmp1E7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1E6)->tag == Cyc_Dict_Absent){
# 984
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B2;_tmp2B2.tag=0,({struct _fat_ptr _tmp397=({const char*_tmp1EB="unbound typedef name ";_tag_fat(_tmp1EB,sizeof(char),22U);});_tmp2B2.f1=_tmp397;});_tmp2B2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1EA=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2B1;_tmp2B1.tag=1,_tmp2B1.f1=tdn;_tmp2B1;});void*_tmp1E8[2];_tmp1E8[0]=& _tmp1E9,_tmp1E8[1]=& _tmp1EA;({unsigned _tmp398=loc;Cyc_Warn_err2(_tmp398,_tag_fat(_tmp1E8,sizeof(void*),2));});});
return env;}else{_tmp1E7=_tmp1E6;{void*exn=_tmp1E7;(int)_rethrow(exn);}};}}}
# 987
*tdopt=td;
env=Cyc_Tctyp_check_type_inst(env,targs_ref,td->tvs,expected_kind,allow_abs_aggr,tdn,td);
# 990
if(td->defn != 0){
struct Cyc_List_List*inst=0;
struct Cyc_List_List*_tmp1EC=td->tvs;struct Cyc_List_List*tvs=_tmp1EC;
struct Cyc_List_List*_tmp1ED=*targs_ref;struct Cyc_List_List*ts=_tmp1ED;
for(1;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
inst=({struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));({struct _tuple18*_tmp399=({struct _tuple18*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp1EE->f2=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;_tmp1EE;});_tmp1EF->hd=_tmp399;}),_tmp1EF->tl=inst;_tmp1EF;});}
({void*_tmp39A=Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));*toptp=_tmp39A;});}
# 998
goto _LL3;}}_LL3:;}
# 1000
if(!({struct Cyc_Absyn_Kind*_tmp39B=Cyc_Tcutil_type_kind(t);Cyc_Kinds_kind_leq(_tmp39B,expected_kind);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B8;_tmp2B8.tag=0,({struct _fat_ptr _tmp39C=({const char*_tmp1F9="type ";_tag_fat(_tmp1F9,sizeof(char),6U);});_tmp2B8.f1=_tmp39C;});_tmp2B8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2B7;_tmp2B7.tag=2,_tmp2B7.f1=(void*)t;_tmp2B7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B6;_tmp2B6.tag=0,({struct _fat_ptr _tmp39D=({const char*_tmp1F8=" has kind ";_tag_fat(_tmp1F8,sizeof(char),11U);});_tmp2B6.f1=_tmp39D;});_tmp2B6;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp1F4=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp2B5;_tmp2B5.tag=9,({struct Cyc_Absyn_Kind*_tmp39E=Cyc_Tcutil_type_kind(t);_tmp2B5.f1=_tmp39E;});_tmp2B5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4;_tmp2B4.tag=0,({
struct _fat_ptr _tmp39F=({const char*_tmp1F7=" but as used here needs kind ";_tag_fat(_tmp1F7,sizeof(char),30U);});_tmp2B4.f1=_tmp39F;});_tmp2B4;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp1F6=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp2B3;_tmp2B3.tag=9,_tmp2B3.f1=expected_kind;_tmp2B3;});void*_tmp1F0[6];_tmp1F0[0]=& _tmp1F1,_tmp1F0[1]=& _tmp1F2,_tmp1F0[2]=& _tmp1F3,_tmp1F0[3]=& _tmp1F4,_tmp1F0[4]=& _tmp1F5,_tmp1F0[5]=& _tmp1F6;({unsigned _tmp3A0=loc;Cyc_Warn_err2(_tmp3A0,_tag_fat(_tmp1F0,sizeof(void*),6));});});
return env;}}
# 1006
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1008
struct Cyc_List_List*_tmp1FA=cvt.kind_env;struct Cyc_List_List*kind_env=_tmp1FA;
cvt=Cyc_Tctyp_i_check_type(cvt,expected_kind,t,1,allow_abs_aggr);
# 1011
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
({struct Cyc_List_List*_tmp3A1=Cyc_Tctyp_fast_add_free_tvar(kind_env,(*((struct _tuple14*)vs->hd)).f1);cvt.kind_env=_tmp3A1;});}}
# 1017
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp1FB=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp27=_tmp1FB;struct _tuple13 _tmp1FC=_stmttmp27;void*_tmp1FD;_tmp1FD=_tmp1FC.f1;{void*e=_tmp1FD;
void*_tmp1FE=Cyc_Absyn_compress(e);void*_stmttmp28=_tmp1FE;void*_tmp1FF=_stmttmp28;void*_tmp200;if(*((int*)_tmp1FF)== 1){_tmp200=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1FF)->f4;{struct Cyc_Core_Opt**s=(struct Cyc_Core_Opt**)_tmp200;
# 1021
if(*s == 0)
({struct Cyc_Core_Opt*_tmp3A2=({struct Cyc_Core_Opt*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->v=kind_env;_tmp201;});*s=_tmp3A2;});else{
# 1025
struct Cyc_List_List*_tmp202=0;struct Cyc_List_List*result=_tmp202;
{struct Cyc_List_List*_tmp203=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*s))->v;struct Cyc_List_List*tvs=_tmp203;for(0;tvs != 0;tvs=tvs->tl){
if(((int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)tvs->hd))
result=({struct Cyc_List_List*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp204->tl=result;_tmp204;});}}
({struct Cyc_Core_Opt*_tmp3A3=({struct Cyc_Core_Opt*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->v=result;_tmp205;});*s=_tmp3A3;});}
# 1031
goto _LL3;}}else{
goto _LL3;}_LL3:;}}}
# 1035
return cvt;}
# 1042
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=
Cyc_Tcutil_is_function_type(t)&&(!te->in_tempest || te->tempest_generalize);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Kinds_tak:& Cyc_Kinds_tmk;
struct Cyc_List_List*_tmp206=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*te_vars=_tmp206;
struct Cyc_Tctyp_CVTEnv _tmp207=({struct Cyc_Tctyp_CVTEnv _tmp3A5=({struct Cyc_Tctyp_CVTEnv _tmp2C5;_tmp2C5.loc=loc,_tmp2C5.te=te,_tmp2C5.kind_env=te_vars,_tmp2C5.fn_result=0,_tmp2C5.generalize_evars=generalize_evars,_tmp2C5.free_vars=0,_tmp2C5.free_evars=0;_tmp2C5;});struct Cyc_Absyn_Kind*_tmp3A4=expected_kind;Cyc_Tctyp_check_valid_type(_tmp3A5,_tmp3A4,1,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp207;
# 1050
struct Cyc_List_List*_tmp208=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst,cvt.free_vars);struct Cyc_List_List*free_tvars=_tmp208;
struct Cyc_List_List*_tmp209=cvt.free_evars;struct Cyc_List_List*free_evars=_tmp209;
# 1053
if(te_vars != 0){
struct Cyc_List_List*_tmp20A=0;struct Cyc_List_List*res=_tmp20A;
{struct Cyc_List_List*_tmp20B=free_tvars;struct Cyc_List_List*fs=_tmp20B;for(0;(unsigned)fs;fs=fs->tl){
struct Cyc_List_List*_tmp20C=te_vars;struct Cyc_List_List*ts=_tmp20C;
for(1;ts != 0;ts=ts->tl){
if(Cyc_Absyn_tvar_cmp((struct Cyc_Absyn_Tvar*)fs->hd,(struct Cyc_Absyn_Tvar*)ts->hd)== 0)
break;}
if(ts == 0)
res=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->hd=(struct Cyc_Absyn_Tvar*)fs->hd,_tmp20D->tl=res;_tmp20D;});}}
# 1063
free_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(res);}
# 1068
{struct Cyc_List_List*x=free_tvars;for(0;x != 0;x=x->tl){
void*_tmp20E=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_stmttmp29=_tmp20E;void*_tmp20F=_stmttmp29;enum Cyc_Absyn_AliasQual _tmp210;void*_tmp212;void*_tmp211;switch(*((int*)_tmp20F)){case 1: _tmp211=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp20F)->f1;{struct Cyc_Core_Opt**f=_tmp211;
({struct Cyc_Core_Opt*_tmp3A6=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_tmp3A6;});goto _LL0;}case 2: _tmp211=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp20F)->f1;_tmp212=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp20F)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_tmp211;struct Cyc_Absyn_Kind*k=_tmp212;
# 1072
{struct Cyc_Absyn_Kind*_tmp213=k;switch((int)((struct Cyc_Absyn_Kind*)_tmp213)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp213)->aliasqual == Cyc_Absyn_Top)
goto _LLD;else{goto _LL14;}case Cyc_Absyn_MemKind: switch((int)((struct Cyc_Absyn_Kind*)_tmp213)->aliasqual){case Cyc_Absyn_Top: _LLD:
 goto _LLF;case Cyc_Absyn_Aliasable: _LLF:
# 1076
({struct Cyc_Core_Opt*_tmp3A7=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_tmp3A7;});goto _LL9;case Cyc_Absyn_Unique:
# 1078
({struct Cyc_Core_Opt*_tmp3A8=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ubk);*f=_tmp3A8;});goto _LL9;default: goto _LL14;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp213)->aliasqual == Cyc_Absyn_Top){
# 1080
({struct Cyc_Core_Opt*_tmp3A9=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_rk);*f=_tmp3A9;});goto _LL9;}else{goto _LL14;}default: _LL14:
# 1082
({struct Cyc_Core_Opt*_tmp3AA=Cyc_Kinds_kind_to_bound_opt(k);*f=_tmp3AA;});goto _LL9;}_LL9:;}
# 1084
goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp20F)->f1)->kind == Cyc_Absyn_MemKind){_tmp210=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp20F)->f1)->aliasqual;{enum Cyc_Absyn_AliasQual a=_tmp210;
# 1086
({struct Cyc_Warn_String_Warn_Warg_struct _tmp215=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BC;_tmp2BC.tag=0,({struct _fat_ptr _tmp3AB=({const char*_tmp21B="type variable ";_tag_fat(_tmp21B,sizeof(char),15U);});_tmp2BC.f1=_tmp3AB;});_tmp2BC;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp216=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2BB;_tmp2BB.tag=7,_tmp2BB.f1=(struct Cyc_Absyn_Tvar*)x->hd;_tmp2BB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp217=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA;_tmp2BA.tag=0,({struct _fat_ptr _tmp3AC=({const char*_tmp21A=" cannot have kind ";_tag_fat(_tmp21A,sizeof(char),19U);});_tmp2BA.f1=_tmp3AC;});_tmp2BA;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp218=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp2B9;_tmp2B9.tag=9,({struct Cyc_Absyn_Kind*_tmp3AD=({struct Cyc_Absyn_Kind*_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219->kind=1U,_tmp219->aliasqual=a;_tmp219;});_tmp2B9.f1=_tmp3AD;});_tmp2B9;});void*_tmp214[4];_tmp214[0]=& _tmp215,_tmp214[1]=& _tmp216,_tmp214[2]=& _tmp217,_tmp214[3]=& _tmp218;({unsigned _tmp3AE=loc;Cyc_Warn_err2(_tmp3AE,_tag_fat(_tmp214,sizeof(void*),4));});});
goto _LL0;}}else{
goto _LL0;}}_LL0:;}}
# 1092
if(free_tvars != 0 || free_evars != 0){
{void*_tmp21C=Cyc_Absyn_compress(t);void*_stmttmp2A=_tmp21C;void*_tmp21D=_stmttmp2A;void*_tmp21E;if(*((int*)_tmp21D)== 5){_tmp21E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21D)->f1).tvars;{struct Cyc_List_List**btvs=(struct Cyc_List_List**)_tmp21E;
# 1095
if(*btvs == 0){
# 1097
({struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_filter)(Cyc_CurRgn_is_not_curr_rgn,free_tvars);*btvs=_tmp3AF;});
free_tvars=0;}
# 1100
goto _LL16;}}else{
goto _LL16;}_LL16:;}
# 1103
if(free_tvars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp220=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0;_tmp2C0.tag=0,({struct _fat_ptr _tmp3B0=({const char*_tmp225="unbound type variable ";_tag_fat(_tmp225,sizeof(char),23U);});_tmp2C0.f1=_tmp3B0;});_tmp2C0;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp221=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2BF;_tmp2BF.tag=7,_tmp2BF.f1=(struct Cyc_Absyn_Tvar*)free_tvars->hd;_tmp2BF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp222=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BE;_tmp2BE.tag=0,({struct _fat_ptr _tmp3B1=({const char*_tmp224=" when checking ";_tag_fat(_tmp224,sizeof(char),16U);});_tmp2BE.f1=_tmp3B1;});_tmp2BE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp223=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BD;_tmp2BD.tag=2,_tmp2BD.f1=(void*)t;_tmp2BD;});void*_tmp21F[4];_tmp21F[0]=& _tmp220,_tmp21F[1]=& _tmp221,_tmp21F[2]=& _tmp222,_tmp21F[3]=& _tmp223;({unsigned _tmp3B2=loc;Cyc_Warn_err2(_tmp3B2,_tag_fat(_tmp21F,sizeof(void*),4));});});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}{
# 1109
void*_tmp226=Cyc_Absyn_compress(t);void*_stmttmp2B=_tmp226;void*_tmp227=_stmttmp2B;void*_tmp228;if(*((int*)_tmp227)== 5){_tmp228=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp227)->f1).tvars;{struct Cyc_List_List*btvs=_tmp228;
# 1111
if(((int(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_exists)(Cyc_CurRgn_is_curr_rgn,btvs))
# 1113
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C4;_tmp2C4.tag=0,({struct _fat_ptr _tmp3B3=({const char*_tmp22F="cannot abstract current region ";_tag_fat(_tmp22F,sizeof(char),32U);});_tmp2C4.f1=_tmp3B3;});_tmp2C4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3;_tmp2C3.tag=0,_tmp2C3.f1=Cyc_CurRgn_curr_rgn_name;_tmp2C3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2;_tmp2C2.tag=0,({struct _fat_ptr _tmp3B4=({const char*_tmp22E=" in ";_tag_fat(_tmp22E,sizeof(char),5U);});_tmp2C2.f1=_tmp3B4;});_tmp2C2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp22D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C1;_tmp2C1.tag=2,_tmp2C1.f1=(void*)t;_tmp2C1;});void*_tmp229[4];_tmp229[0]=& _tmp22A,_tmp229[1]=& _tmp22B,_tmp229[2]=& _tmp22C,_tmp229[3]=& _tmp22D;({unsigned _tmp3B5=loc;Cyc_Warn_err2(_tmp3B5,_tag_fat(_tmp229,sizeof(void*),4));});});
goto _LL1B;}}else{
goto _LL1B;}_LL1B:;}}
# 1123
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1126
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_tmp230=Cyc_Absyn_compress(t);void*_stmttmp2C=_tmp230;void*_tmp231=_stmttmp2C;struct Cyc_Absyn_FnInfo _tmp232;if(*((int*)_tmp231)== 5){_tmp232=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp231)->f1;{struct Cyc_Absyn_FnInfo i=_tmp232;
# 1129
struct Cyc_List_List*_tmp233=(fd->i).attributes;struct Cyc_List_List*atts=_tmp233;
fd->i=i;
(fd->i).attributes=atts;
({int _tmp3B6=
Cyc_Tcutil_extract_const_from_typedef(loc,(i.ret_tqual).print_const,i.ret_type);
# 1132
((fd->i).ret_tqual).real_const=_tmp3B6;});
# 1134
goto _LL0;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp235=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C6;_tmp2C6.tag=0,({struct _fat_ptr _tmp3B7=({const char*_tmp236="check_fndecl_valid_type: not a FnType";_tag_fat(_tmp236,sizeof(char),38U);});_tmp2C6.f1=_tmp3B7;});_tmp2C6;});void*_tmp234[1];_tmp234[0]=& _tmp235;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp234,sizeof(void*),1));});}_LL0:;}{
# 1137
struct Cyc_List_List*_tmp237=0;struct Cyc_List_List*argnames=_tmp237;
{struct Cyc_List_List*_tmp238=(fd->i).args;struct Cyc_List_List*args=_tmp238;for(0;args != 0;args=args->tl){
argnames=({struct Cyc_List_List*_tmp239=_cycalloc(sizeof(*_tmp239));_tmp239->hd=(struct _fat_ptr*)_check_null((*((struct _tuple9*)args->hd)).f1),_tmp239->tl=argnames;_tmp239;});}}
({struct Cyc_List_List*_tmp3B9=argnames;unsigned _tmp3B8=loc;Cyc_Tcutil_check_unique_vars(_tmp3B9,_tmp3B8,({const char*_tmp23A="function declaration has repeated parameter";_tag_fat(_tmp23A,sizeof(char),44U);}));});
# 1142
fd->cached_type=t;}}
# 1147
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1150
struct Cyc_Tctyp_CVTEnv _tmp23B=({struct Cyc_Tctyp_CVTEnv _tmp3BC=({struct Cyc_Tctyp_CVTEnv _tmp2CB;_tmp2CB.loc=loc,_tmp2CB.te=te,_tmp2CB.kind_env=bound_tvars,_tmp2CB.fn_result=0,_tmp2CB.generalize_evars=0,_tmp2CB.free_vars=0,_tmp2CB.free_evars=0;_tmp2CB;});struct Cyc_Absyn_Kind*_tmp3BB=expected_kind;int _tmp3BA=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_tmp3BC,_tmp3BB,_tmp3BA,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp23B;
# 1152
struct Cyc_List_List*_tmp23C=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars)(cvt.free_vars,bound_tvars,(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst);struct Cyc_List_List*free_tvs=_tmp23C;
for(1;free_tvs != 0;free_tvs=free_tvs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA;_tmp2CA.tag=0,({struct _fat_ptr _tmp3BD=({const char*_tmp243="unbound type variable ";_tag_fat(_tmp243,sizeof(char),23U);});_tmp2CA.f1=_tmp3BD;});_tmp2CA;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp23F=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2C9;_tmp2C9.tag=7,_tmp2C9.f1=(*((struct _tuple14*)free_tvs->hd)).f1;_tmp2C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp240=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8;_tmp2C8.tag=0,({struct _fat_ptr _tmp3BE=({const char*_tmp242=" in type ";_tag_fat(_tmp242,sizeof(char),10U);});_tmp2C8.f1=_tmp3BE;});_tmp2C8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp241=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C7;_tmp2C7.tag=2,_tmp2C7.f1=(void*)t;_tmp2C7;});void*_tmp23D[4];_tmp23D[0]=& _tmp23E,_tmp23D[1]=& _tmp23F,_tmp23D[2]=& _tmp240,_tmp23D[3]=& _tmp241;({unsigned _tmp3BF=loc;Cyc_Warn_err2(_tmp3BF,_tag_fat(_tmp23D,sizeof(void*),4));});});}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(cvt.free_evars,t,loc);}

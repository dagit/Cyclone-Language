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
extern void*Cyc_Core_fst(struct _tuple0*);
# 117
extern void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 884
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 896
void*Cyc_Absyn_compress_kb(void*);
# 900
void*Cyc_Absyn_compress(void*);
# 905
int Cyc_Absyn_type2bool(int def,void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 928
extern void*Cyc_Absyn_empty_effect;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);
# 951
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar (void);
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar (void);
# 961
void*Cyc_Absyn_bounds_one (void);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
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
# 46
int Cyc_Tcutil_is_array_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 99
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 104
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 107
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
# 128
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 132
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 140
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
# 166
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
# 178
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 196
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 206
void Cyc_Tcutil_check_bitfield(unsigned,void*field_typ,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn);
# 208
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 250
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 252
int Cyc_Tcutil_new_tvar_id (void);
# 260
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 264
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 267
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 278
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 285
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 293
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 296
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 41 "attributes.h"
int Cyc_Atts_fntype_att(void*);
# 45
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*fn,struct Cyc_List_List*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
# 72 "flags.h"
extern int Cyc_Flags_tovc_r;
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_EvalOrder =11U,Cyc_Flags_CCompiler =12U,Cyc_Flags_AllPasses =13U};
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tctyp_CVTEnv{unsigned loc;struct Cyc_Tcenv_Tenv*te;struct Cyc_List_List*kind_env;int fn_result;int generalize_evars;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;};
# 82 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*,void*,int,int);struct _tuple13{void*f1;int f2;};
# 88
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct Cyc_List_List*es,void*e,int b){
void*_tmp0=Cyc_Absyn_compress(e);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;int _tmp2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->tag == 1U){_LL1: _tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1)->f3;_LL2: {int i=_tmp2;
# 91
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple13*_tmp3=(struct _tuple13*)es2->hd;struct _tuple13*_stmttmp1=_tmp3;struct _tuple13*_tmp4=_stmttmp1;int*_tmp6;void*_tmp5;_LL6: _tmp5=_tmp4->f1;_tmp6=(int*)& _tmp4->f2;_LL7: {void*t=_tmp5;int*b2=_tmp6;
void*_tmp7=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp7;void*_tmp8=_stmttmp2;int _tmp9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8)->tag == 1U){_LL9: _tmp9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8)->f3;_LLA: {int j=_tmp9;
# 95
if(i == j){
if(b != *b2)*b2=1;
return es;}
# 99
goto _LL8;}}else{_LLB: _LLC:
 goto _LL8;}_LL8:;}}}
# 103
return({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));({struct _tuple13*_tmp30C=({struct _tuple13*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=e,_tmpA->f2=b;_tmpA;});_tmpB->hd=_tmp30C;}),_tmpB->tl=es;_tmpB;});}}else{_LL3: _LL4:
 return es;}_LL0:;}
# 109
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp289;_tmp289.tag=0U,({struct _fat_ptr _tmp30D=({const char*_tmp11="bound tvar id for ";_tag_fat(_tmp11,sizeof(char),19U);});_tmp289.f1=_tmp30D;});_tmp289;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmpE=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp288;_tmp288.tag=7U,_tmp288.f1=tv;_tmp288;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp287;_tmp287.tag=0U,({struct _fat_ptr _tmp30E=({const char*_tmp10=" is NULL";_tag_fat(_tmp10,sizeof(char),9U);});_tmp287.f1=_tmp30E;});_tmp287;});void*_tmpC[3U];_tmpC[0]=& _tmpD,_tmpC[1]=& _tmpE,_tmpC[2]=& _tmpF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpC,sizeof(void*),3U));});
return({struct Cyc_List_List*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=tv,_tmp12->tl=tvs;_tmp12;});}
# 114
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(void*)){
# 116
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_List_List*b=btvs;
for(0;b != 0;b=b->tl){
if(({int _tmp30F=(get(tvs->hd))->identity;_tmp30F == ((struct Cyc_Absyn_Tvar*)b->hd)->identity;}))
break;}
if(b == 0)
r=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=tvs->hd,_tmp13->tl=r;_tmp13;});}
# 125
r=Cyc_List_imp_rev(r);
return r;}
# 133
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(*((struct _tuple13*)free_evars->hd)).f1;
{void*_tmp14=Cyc_Absyn_compress(e);void*_stmttmp3=_tmp14;void*_tmp15=_stmttmp3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
 continue;}_LL0:;}{
# 140
void*t;
struct _fat_ptr s;
{struct Cyc_Absyn_Kind*_tmp16=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_stmttmp4=_tmp16;struct Cyc_Absyn_Kind*_tmp17=_stmttmp4;switch(((struct Cyc_Absyn_Kind*)_tmp17)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp17)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
 t=Cyc_Absyn_unique_rgn_type;s=({const char*_tmp18="unique region";_tag_fat(_tmp18,sizeof(char),14U);});goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
 t=Cyc_Absyn_heap_rgn_type;s=({const char*_tmp19="heap";_tag_fat(_tmp19,sizeof(char),5U);});goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
 t=Cyc_Absyn_empty_effect;s=({const char*_tmp1A="{}";_tag_fat(_tmp1A,sizeof(char),3U);});goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
 t=Cyc_Absyn_false_type;s=({const char*_tmp1B="@false";_tag_fat(_tmp1B,sizeof(char),7U);});goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
 t=Cyc_Absyn_bounds_one();s=({const char*_tmp1C="bounds_one";_tag_fat(_tmp1C,sizeof(char),11U);});goto _LL5;default: _LL12: _LL13:
# 150
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28D;_tmp28D.tag=0U,({struct _fat_ptr _tmp310=({const char*_tmp23="hidden type variable ";_tag_fat(_tmp23,sizeof(char),22U);});_tmp28D.f1=_tmp310;});_tmp28D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28C;_tmp28C.tag=2U,_tmp28C.f1=(void*)e;_tmp28C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28B;_tmp28B.tag=0U,({struct _fat_ptr _tmp311=({const char*_tmp22=" isn't abstracted in type ";_tag_fat(_tmp22,sizeof(char),27U);});_tmp28B.f1=_tmp311;});_tmp28B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28A;_tmp28A.tag=2U,_tmp28A.f1=(void*)in_typ;_tmp28A;});void*_tmp1D[4U];_tmp1D[0]=& _tmp1E,_tmp1D[1]=& _tmp1F,_tmp1D[2]=& _tmp20,_tmp1D[3]=& _tmp21;({unsigned _tmp312=loc;Cyc_Warn_err2(_tmp312,_tag_fat(_tmp1D,sizeof(void*),4U));});});
return;}_LL5:;}
# 153
if(!Cyc_Unify_unify(e,t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25=({struct Cyc_Warn_String_Warn_Warg_struct _tmp292;_tmp292.tag=0U,({struct _fat_ptr _tmp313=({const char*_tmp2C="can't unify evar ";_tag_fat(_tmp2C,sizeof(char),18U);});_tmp292.f1=_tmp313;});_tmp292;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp26=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp291;_tmp291.tag=2U,_tmp291.f1=(void*)e;_tmp291;});struct Cyc_Warn_String_Warn_Warg_struct _tmp27=({struct Cyc_Warn_String_Warn_Warg_struct _tmp290;_tmp290.tag=0U,({struct _fat_ptr _tmp314=({const char*_tmp2B=" with ";_tag_fat(_tmp2B,sizeof(char),7U);});_tmp290.f1=_tmp314;});_tmp290;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28F;_tmp28F.tag=0U,_tmp28F.f1=s;_tmp28F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp29=({struct Cyc_Warn_String_Warn_Warg_struct _tmp28E;_tmp28E.tag=0U,({struct _fat_ptr _tmp315=({const char*_tmp2A="!";_tag_fat(_tmp2A,sizeof(char),2U);});_tmp28E.f1=_tmp315;});_tmp28E;});void*_tmp24[5U];_tmp24[0]=& _tmp25,_tmp24[1]=& _tmp26,_tmp24[2]=& _tmp27,_tmp24[3]=& _tmp28,_tmp24[4]=& _tmp29;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp24,sizeof(void*),5U));});}}}
# 162
static int Cyc_Tctyp_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp2D=({struct _tuple0 _tmp293;_tmp293.f1=c1,_tmp293.f2=c2;_tmp293;});struct _tuple0 _stmttmp5=_tmp2D;struct _tuple0 _tmp2E=_stmttmp5;struct Cyc_Absyn_Kind*_tmp32;struct Cyc_Core_Opt**_tmp31;struct Cyc_Absyn_Kind*_tmp30;struct Cyc_Core_Opt**_tmp2F;struct Cyc_Absyn_Kind*_tmp35;struct Cyc_Absyn_Kind*_tmp34;struct Cyc_Core_Opt**_tmp33;struct Cyc_Core_Opt**_tmp36;struct Cyc_Core_Opt**_tmp37;struct Cyc_Absyn_Kind*_tmp3A;struct Cyc_Core_Opt**_tmp39;struct Cyc_Absyn_Kind*_tmp38;struct Cyc_Absyn_Kind*_tmp3C;struct Cyc_Absyn_Kind*_tmp3B;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f1)->tag == 0U)switch(*((int*)_tmp2E.f2)){case 0U: _LL1: _tmp3B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp3C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_LL2: {struct Cyc_Absyn_Kind*k1=_tmp3B;struct Cyc_Absyn_Kind*k2=_tmp3C;
return k1 == k2;}case 1U: goto _LL3;default: _LL9: _tmp38=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp39=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_tmp3A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f2;_LLA: {struct Cyc_Absyn_Kind*k1=_tmp38;struct Cyc_Core_Opt**f=_tmp39;struct Cyc_Absyn_Kind*k2=_tmp3A;
# 176
if(!Cyc_Tcutil_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_tmp316=({struct Cyc_Core_Opt*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->v=c1;_tmp40;});*f=_tmp316;});
return 1;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f2)->tag == 1U){_LL3: _tmp37=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_LL4: {struct Cyc_Core_Opt**f=_tmp37;
# 168
({struct Cyc_Core_Opt*_tmp317=({struct Cyc_Core_Opt*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->v=c1;_tmp3D;});*f=_tmp317;});return 1;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f1)->tag == 1U){_LL5: _tmp36=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_LL6: {struct Cyc_Core_Opt**f=_tmp36;
({struct Cyc_Core_Opt*_tmp318=({struct Cyc_Core_Opt*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->v=c2;_tmp3E;});*f=_tmp318;});return 1;}}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f2)->tag == 0U){_LL7: _tmp33=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp34=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f2;_tmp35=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_LL8: {struct Cyc_Core_Opt**f=_tmp33;struct Cyc_Absyn_Kind*k1=_tmp34;struct Cyc_Absyn_Kind*k2=_tmp35;
# 171
if(!Cyc_Tcutil_kind_leq(k2,k1))
return 0;
({struct Cyc_Core_Opt*_tmp319=({struct Cyc_Core_Opt*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->v=c2;_tmp3F;});*f=_tmp319;});
return 1;}}else{_LLB: _tmp2F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f1;_tmp30=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f1)->f2;_tmp31=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f1;_tmp32=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp2E.f2)->f2;_LLC: {struct Cyc_Core_Opt**f1=_tmp2F;struct Cyc_Absyn_Kind*k1=_tmp30;struct Cyc_Core_Opt**f2=_tmp31;struct Cyc_Absyn_Kind*k2=_tmp32;
# 181
if(Cyc_Tcutil_kind_leq(k1,k2)){
({struct Cyc_Core_Opt*_tmp31A=({struct Cyc_Core_Opt*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->v=c1;_tmp41;});*f2=_tmp31A;});
return 1;}
# 185
if(Cyc_Tcutil_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_tmp31B=({struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=c2;_tmp42;});*f1=_tmp31B;});
return 1;}
# 189
return 0;}}}}}_LL0:;}}
# 198
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tctyp_constrain_kinds(k1,k2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp44=({struct Cyc_Warn_String_Warn_Warg_struct _tmp299;_tmp299.tag=0U,({struct _fat_ptr _tmp31C=({const char*_tmp4C="type variable ";_tag_fat(_tmp4C,sizeof(char),15U);});_tmp299.f1=_tmp31C;});_tmp299;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp45=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp298;_tmp298.tag=7U,_tmp298.f1=tv;_tmp298;});struct Cyc_Warn_String_Warn_Warg_struct _tmp46=({struct Cyc_Warn_String_Warn_Warg_struct _tmp297;_tmp297.tag=0U,({struct _fat_ptr _tmp31D=({const char*_tmp4B=" used with inconsistent kinds ";_tag_fat(_tmp4B,sizeof(char),31U);});_tmp297.f1=_tmp31D;});_tmp297;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp47=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp296;_tmp296.tag=8U,_tmp296.f1=(void*)k1;_tmp296;});struct Cyc_Warn_String_Warn_Warg_struct _tmp48=({struct Cyc_Warn_String_Warn_Warg_struct _tmp295;_tmp295.tag=0U,({
struct _fat_ptr _tmp31E=({const char*_tmp4A=" and ";_tag_fat(_tmp4A,sizeof(char),6U);});_tmp295.f1=_tmp31E;});_tmp295;});struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp49=({struct Cyc_Warn_KindBound_Warn_Warg_struct _tmp294;_tmp294.tag=8U,_tmp294.f1=(void*)k2;_tmp294;});void*_tmp43[6U];_tmp43[0]=& _tmp44,_tmp43[1]=& _tmp45,_tmp43[2]=& _tmp46,_tmp43[3]=& _tmp47,_tmp43[4]=& _tmp48,_tmp43[5]=& _tmp49;({unsigned _tmp31F=loc;Cyc_Warn_err2(_tmp31F,_tag_fat(_tmp43,sizeof(void*),6U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29A;_tmp29A.tag=0U,({struct _fat_ptr _tmp320=({const char*_tmp4F="same type variable has different identity!";_tag_fat(_tmp4F,sizeof(char),43U);});_tmp29A.f1=_tmp320;});_tmp29A;});void*_tmp4D[1U];_tmp4D[0]=& _tmp4E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4D,sizeof(void*),1U));});}
return tvs;}}}
# 212
({int _tmp321=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp321;});
return({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=tv,_tmp50->tl=tvs;_tmp50;});}
# 218
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29B;_tmp29B.tag=0U,({struct _fat_ptr _tmp322=({const char*_tmp53="fast_add_free_tvar: bad identity in tv";_tag_fat(_tmp53,sizeof(char),39U);});_tmp29B.f1=_tmp322;});_tmp29B;});void*_tmp51[1U];_tmp51[0]=& _tmp52;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp51,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*_tmp54=(struct Cyc_Absyn_Tvar*)tvs2->hd;struct Cyc_Absyn_Tvar*tv2=_tmp54;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp56=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29C;_tmp29C.tag=0U,({struct _fat_ptr _tmp323=({const char*_tmp57="fast_add_free_tvar: bad identity in tvs2";_tag_fat(_tmp57,sizeof(char),41U);});_tmp29C.f1=_tmp323;});_tmp29C;});void*_tmp55[1U];_tmp55[0]=& _tmp56;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp55,sizeof(void*),1U));});
if(tv2->identity == tv->identity)
return tvs;}}
# 228
return({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->hd=tv,_tmp58->tl=tvs;_tmp58;});}struct _tuple14{struct Cyc_Absyn_Tvar*f1;int f2;};
# 234
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
if(tv->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29D;_tmp29D.tag=0U,({struct _fat_ptr _tmp324=({const char*_tmp5B="fast_add_free_tvar_bool: bad identity in tv";_tag_fat(_tmp5B,sizeof(char),44U);});_tmp29D.f1=_tmp324;});_tmp29D;});void*_tmp59[1U];_tmp59[0]=& _tmp5A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp59,sizeof(void*),1U));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
struct _tuple14*_tmp5C=(struct _tuple14*)tvs2->hd;struct _tuple14*_stmttmp6=_tmp5C;struct _tuple14*_tmp5D=_stmttmp6;int*_tmp5F;struct Cyc_Absyn_Tvar*_tmp5E;_LL1: _tmp5E=_tmp5D->f1;_tmp5F=(int*)& _tmp5D->f2;_LL2: {struct Cyc_Absyn_Tvar*tv2=_tmp5E;int*b2=_tmp5F;
if(tv2->identity == - 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp61=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E;_tmp29E.tag=0U,({struct _fat_ptr _tmp325=({const char*_tmp62="fast_add_free_tvar_bool: bad identity in tvs2";_tag_fat(_tmp62,sizeof(char),46U);});_tmp29E.f1=_tmp325;});_tmp29E;});void*_tmp60[1U];_tmp60[0]=& _tmp61;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp60,sizeof(void*),1U));});
if(tv2->identity == tv->identity){
*b2=*b2 || b;
return tvs;}}}}
# 246
return({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));({struct _tuple14*_tmp326=({struct _tuple14*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->f1=tv,_tmp63->f2=b;_tmp63;});_tmp64->hd=_tmp326;}),_tmp64->tl=tvs;_tmp64;});}
# 253
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tctyp_CVTEnv env){
env.loc=e->loc;
{void*_tmp65=e->r;void*_stmttmp7=_tmp65;void*_tmp66=_stmttmp7;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp68;void*_tmp69;void*_tmp6A;void*_tmp6B;struct Cyc_Absyn_Exp*_tmp6D;void*_tmp6C;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Exp*_tmp71;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*_tmp73;struct Cyc_Absyn_Exp*_tmp72;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_List_List*_tmp77;switch(*((int*)_tmp66)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp77=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LLC: {struct Cyc_List_List*es=_tmp77;
# 262
for(0;es != 0;es=es->tl){
env=Cyc_Tctyp_i_check_type_level_exp((struct Cyc_Absyn_Exp*)es->hd,env);}
goto _LL0;}case 6U: _LLD: _tmp74=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp75=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_tmp76=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp66)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp74;struct Cyc_Absyn_Exp*e2=_tmp75;struct Cyc_Absyn_Exp*e3=_tmp76;
# 266
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
_tmp72=e2;_tmp73=e3;goto _LL10;}case 7U: _LLF: _tmp72=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp73=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp72;struct Cyc_Absyn_Exp*e2=_tmp73;
_tmp70=e1;_tmp71=e2;goto _LL12;}case 8U: _LL11: _tmp70=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp71=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp70;struct Cyc_Absyn_Exp*e2=_tmp71;
_tmp6E=e1;_tmp6F=e2;goto _LL14;}case 9U: _LL13: _tmp6E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp6F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp6E;struct Cyc_Absyn_Exp*e2=_tmp6F;
# 271
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type_level_exp(e2,env);
goto _LL0;}case 14U: _LL15: _tmp6C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_tmp6D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f2;_LL16: {void*t=_tmp6C;struct Cyc_Absyn_Exp*e1=_tmp6D;
# 275
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tak,t,1,0);
goto _LL0;}case 19U: _LL17: _tmp6B=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL18: {void*t=_tmp6B;
_tmp6A=t;goto _LL1A;}case 17U: _LL19: _tmp6A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL1A: {void*t=_tmp6A;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tak,t,1,0);goto _LL0;}case 39U: _LL1B: _tmp69=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL1C: {void*t=_tmp69;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_ik,t,1,0);goto _LL0;}case 18U: _LL1D: _tmp68=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp68;
_tmp67=e;goto _LL20;}case 41U: _LL1F: _tmp67=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp66)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp67;
env=Cyc_Tctyp_i_check_type_level_exp(e,env);goto _LL0;}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp79=({struct Cyc_Warn_String_Warn_Warg_struct _tmp29F;_tmp29F.tag=0U,({struct _fat_ptr _tmp327=({const char*_tmp7A="bad expression in Tctyp::i_check_type_level_exp";_tag_fat(_tmp7A,sizeof(char),48U);});_tmp29F.f1=_tmp327;});_tmp29F;});void*_tmp78[1U];_tmp78[0]=& _tmp79;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp78,sizeof(void*),1U));});}_LL0:;}
# 285
return env;}
# 288
static int Cyc_Tctyp_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td == 0)
return 1;
if(td->defn == 0)
return 0;{
void*_tmp7B=Cyc_Absyn_compress((void*)_check_null(td->defn));void*_stmttmp8=_tmp7B;void*_tmp7C=_stmttmp8;void*_tmp7D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->tag == 3U){_LL1: _tmp7D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp7D;
# 295
void*_tmp7E=Cyc_Absyn_compress(r);void*_stmttmp9=_tmp7E;void*_tmp7F=_stmttmp9;struct Cyc_Absyn_Tvar*_tmp80;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->tag == 2U){_LL6: _tmp80=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F)->f1;_LL7: {struct Cyc_Absyn_Tvar*tv=_tmp80;
return Cyc_Absyn_tvar_cmp(tvar,tv)== 0;}}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 299
 return 0;}_LL0:;}}
# 303
static struct Cyc_Absyn_Kind*Cyc_Tctyp_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 305
void*_tmp81=Cyc_Absyn_compress_kb(tvar->kind);void*_stmttmpA=_tmp81;void*_tmp82=_stmttmpA;switch(*((int*)_tmp82)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp82)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp82)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp82)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp82)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4: {
# 314
enum Cyc_Absyn_KindQual _tmp83=expected_kind->kind;enum Cyc_Absyn_KindQual k=_tmp83;
if((((int)k == (int)2U ||(int)k == (int)1U)||(int)k == (int)0U)&&
 Cyc_Tctyp_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}
# 322
return& Cyc_Tcutil_trk;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 326
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,struct _tuple1*typname,struct Cyc_Absyn_Typedefdecl*td){
# 331
struct Cyc_List_List*_tmp84=*targsp;struct Cyc_List_List*targs=_tmp84;
for(0;targs != 0 && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 337
{struct _tuple0 _tmp85=({struct _tuple0 _tmp2A0;({void*_tmp328=Cyc_Absyn_compress_kb(tv->kind);_tmp2A0.f1=_tmp328;}),_tmp2A0.f2=t;_tmp2A0;});struct _tuple0 _stmttmpB=_tmp85;struct _tuple0 _tmp86=_stmttmpB;struct Cyc_Absyn_Tvar*_tmp87;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp86.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp86.f2)->tag == 2U){_LL1: _tmp87=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp86.f2)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv2=_tmp87;
# 339
({struct Cyc_List_List*_tmp329=Cyc_Tctyp_add_free_tvar(env.loc,env.kind_env,tv2);env.kind_env=_tmp329;});
({struct Cyc_List_List*_tmp32A=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_tmp32A;});
continue;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 344
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
env=Cyc_Tctyp_i_check_type(env,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(env.loc,t);}}
# 348
if(targs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp89=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A2;_tmp2A2.tag=0U,({struct _fat_ptr _tmp32B=({const char*_tmp8B="too many type arguments for ";_tag_fat(_tmp8B,sizeof(char),29U);});_tmp2A2.f1=_tmp32B;});_tmp2A2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2A1;_tmp2A1.tag=1U,_tmp2A1.f1=typname;_tmp2A1;});void*_tmp88[2U];_tmp88[0]=& _tmp89,_tmp88[1]=& _tmp8A;({unsigned _tmp32C=env.loc;Cyc_Warn_err2(_tmp32C,_tag_fat(_tmp88,sizeof(void*),2U));});});{
# 351
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->hd=e,_tmp8C->tl=hidden_ts;_tmp8C;});{
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
env=Cyc_Tctyp_i_check_type(env,k,e,1,allow_abs_aggr);}}
# 358
({struct Cyc_List_List*_tmp32E=({struct Cyc_List_List*_tmp32D=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp32D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmp32E;});
return env;}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f1;struct Cyc_List_List*f2;};
# 363
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 366
if(clause == 0)
return({struct _tuple15 _tmp2A3;_tmp2A3.f1=env,_tmp2A3.f2=0;_tmp2A3;});
Cyc_Tcexp_tcExp(env.te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A7;_tmp2A7.tag=0U,_tmp2A7.f1=clause_name;_tmp2A7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A6;_tmp2A6.tag=0U,({struct _fat_ptr _tmp32F=({const char*_tmp93=" clause has type ";_tag_fat(_tmp93,sizeof(char),18U);});_tmp2A6.f1=_tmp32F;});_tmp2A6;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp90=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp2A5;_tmp2A5.tag=3U,_tmp2A5.f1=(void*)clause->topt;_tmp2A5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp91=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A4;_tmp2A4.tag=0U,({
struct _fat_ptr _tmp330=({const char*_tmp92=" instead of integral type";_tag_fat(_tmp92,sizeof(char),26U);});_tmp2A4.f1=_tmp330;});_tmp2A4;});void*_tmp8D[4U];_tmp8D[0]=& _tmp8E,_tmp8D[1]=& _tmp8F,_tmp8D[2]=& _tmp90,_tmp8D[3]=& _tmp91;({unsigned _tmp331=env.loc;Cyc_Warn_err2(_tmp331,_tag_fat(_tmp8D,sizeof(void*),4U));});});
env=Cyc_Tctyp_i_check_type_level_exp(clause,env);{
struct Cyc_List_List*_tmp94=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);struct Cyc_List_List*relns=_tmp94;
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp96=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AB;_tmp2AB.tag=0U,_tmp2AB.f1=clause_name;_tmp2AB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp97=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AA;_tmp2AA.tag=0U,({struct _fat_ptr _tmp332=({const char*_tmp9B=" clause '";_tag_fat(_tmp9B,sizeof(char),10U);});_tmp2AA.f1=_tmp332;});_tmp2AA;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp98=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp2A9;_tmp2A9.tag=4U,_tmp2A9.f1=clause;_tmp2A9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp99=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A8;_tmp2A8.tag=0U,({struct _fat_ptr _tmp333=({const char*_tmp9A="' may be unsatisfiable";_tag_fat(_tmp9A,sizeof(char),23U);});_tmp2A8.f1=_tmp333;});_tmp2A8;});void*_tmp95[4U];_tmp95[0]=& _tmp96,_tmp95[1]=& _tmp97,_tmp95[2]=& _tmp98,_tmp95[3]=& _tmp99;({unsigned _tmp334=clause->loc;Cyc_Warn_err2(_tmp334,_tag_fat(_tmp95,sizeof(void*),4U));});});
return({struct _tuple15 _tmp2AC;_tmp2AC.f1=env,_tmp2AC.f2=relns;_tmp2AC;});}}
# 380
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 383
struct Cyc_Tctyp_CVTEnv _tmp9C=env;struct Cyc_Tcenv_Tenv*_tmp9E;unsigned _tmp9D;_LL1: _tmp9D=_tmp9C.loc;_tmp9E=_tmp9C.te;_LL2: {unsigned loc=_tmp9D;struct Cyc_Tcenv_Tenv*te=_tmp9E;
union Cyc_Absyn_AggrInfo _tmp9F=*info;union Cyc_Absyn_AggrInfo _stmttmpC=_tmp9F;union Cyc_Absyn_AggrInfo _tmpA0=_stmttmpC;struct Cyc_Absyn_Aggrdecl*_tmpA1;struct Cyc_Core_Opt*_tmpA4;struct _tuple1*_tmpA3;enum Cyc_Absyn_AggrKind _tmpA2;if((_tmpA0.UnknownAggr).tag == 1){_LL4: _tmpA2=((_tmpA0.UnknownAggr).val).f1;_tmpA3=((_tmpA0.UnknownAggr).val).f2;_tmpA4=((_tmpA0.UnknownAggr).val).f3;_LL5: {enum Cyc_Absyn_AggrKind k=_tmpA2;struct _tuple1*n=_tmpA3;struct Cyc_Core_Opt*tgd=_tmpA4;
# 386
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpA5;_push_handler(& _tmpA5);{int _tmpA7=0;if(setjmp(_tmpA5.handler))_tmpA7=1;if(!_tmpA7){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);{
struct Cyc_Absyn_Aggrdecl*_tmpA8=*adp;struct Cyc_Absyn_Aggrdecl*ad=_tmpA8;
if((int)ad->kind != (int)k){
if((int)ad->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B0;_tmp2B0.tag=0U,({struct _fat_ptr _tmp335=({const char*_tmpAF="expecting struct ";_tag_fat(_tmpAF,sizeof(char),18U);});_tmp2B0.f1=_tmp335;});_tmp2B0;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpAB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2AF;_tmp2AF.tag=1U,_tmp2AF.f1=n;_tmp2AF;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AE;_tmp2AE.tag=0U,({struct _fat_ptr _tmp336=({const char*_tmpAE="instead of union ";_tag_fat(_tmpAE,sizeof(char),18U);});_tmp2AE.f1=_tmp336;});_tmp2AE;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpAD=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2AD;_tmp2AD.tag=1U,_tmp2AD.f1=n;_tmp2AD;});void*_tmpA9[4U];_tmpA9[0]=& _tmpAA,_tmpA9[1]=& _tmpAB,_tmpA9[2]=& _tmpAC,_tmpA9[3]=& _tmpAD;({unsigned _tmp337=loc;Cyc_Warn_err2(_tmp337,_tag_fat(_tmpA9,sizeof(void*),4U));});});else{
# 394
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4;_tmp2B4.tag=0U,({struct _fat_ptr _tmp338=({const char*_tmpB6="expecting union ";_tag_fat(_tmpB6,sizeof(char),17U);});_tmp2B4.f1=_tmp338;});_tmp2B4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2B3;_tmp2B3.tag=1U,_tmp2B3.f1=n;_tmp2B3;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B2;_tmp2B2.tag=0U,({struct _fat_ptr _tmp339=({const char*_tmpB5="instead of struct ";_tag_fat(_tmpB5,sizeof(char),19U);});_tmp2B2.f1=_tmp339;});_tmp2B2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2B1;_tmp2B1.tag=1U,_tmp2B1.f1=n;_tmp2B1;});void*_tmpB0[4U];_tmpB0[0]=& _tmpB1,_tmpB0[1]=& _tmpB2,_tmpB0[2]=& _tmpB3,_tmpB0[3]=& _tmpB4;({unsigned _tmp33A=loc;Cyc_Warn_err2(_tmp33A,_tag_fat(_tmpB0,sizeof(void*),4U));});});}}
# 396
if(((unsigned)tgd &&(int)tgd->v)&&(!((unsigned)ad->impl)|| !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B6;_tmp2B6.tag=0U,({struct _fat_ptr _tmp33B=({const char*_tmpBA="@tagged qualfiers don't agree on union ";_tag_fat(_tmpBA,sizeof(char),40U);});_tmp2B6.f1=_tmp33B;});_tmp2B6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2B5;_tmp2B5.tag=1U,_tmp2B5.f1=n;_tmp2B5;});void*_tmpB7[2U];_tmpB7[0]=& _tmpB8,_tmpB7[1]=& _tmpB9;({unsigned _tmp33C=loc;Cyc_Warn_err2(_tmp33C,_tag_fat(_tmpB7,sizeof(void*),2U));});});
# 399
({union Cyc_Absyn_AggrInfo _tmp33D=Cyc_Absyn_KnownAggr(adp);*info=_tmp33D;});}
# 388
;_pop_handler();}else{void*_tmpA6=(void*)Cyc_Core_get_exn_thrown();void*_tmpBB=_tmpA6;void*_tmpBC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBB)->tag == Cyc_Dict_Absent){_LL9: _LLA:
# 403
({struct Cyc_Tcenv_Tenv*_tmp33F=te;unsigned _tmp33E=loc;Cyc_Tc_tcAggrdecl(_tmp33F,_tmp33E,({struct Cyc_Absyn_Aggrdecl*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->kind=k,_tmpBD->sc=Cyc_Absyn_Extern,_tmpBD->name=n,_tmpBD->tvs=0,_tmpBD->impl=0,_tmpBD->attributes=0,_tmpBD->expected_mem_kind=0;_tmpBD;}));});
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);
({union Cyc_Absyn_AggrInfo _tmp340=Cyc_Absyn_KnownAggr(adp);*info=_tmp340;});
# 407
if(*targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpBF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B9;_tmp2B9.tag=0U,({struct _fat_ptr _tmp341=({const char*_tmpC3="declare parameterized type ";_tag_fat(_tmpC3,sizeof(char),28U);});_tmp2B9.f1=_tmp341;});_tmp2B9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpC0=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2B8;_tmp2B8.tag=1U,_tmp2B8.f1=n;_tmp2B8;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B7;_tmp2B7.tag=0U,({struct _fat_ptr _tmp342=({const char*_tmpC2=" before using";_tag_fat(_tmpC2,sizeof(char),14U);});_tmp2B7.f1=_tmp342;});_tmp2B7;});void*_tmpBE[3U];_tmpBE[0]=& _tmpBF,_tmpBE[1]=& _tmpC0,_tmpBE[2]=& _tmpC1;({unsigned _tmp343=loc;Cyc_Warn_err2(_tmp343,_tag_fat(_tmpBE,sizeof(void*),3U));});});
return env;}
# 411
goto _LL8;}else{_LLB: _tmpBC=_tmpBB;_LLC: {void*exn=_tmpBC;(int)_rethrow(exn);}}_LL8:;}}}
# 413
_tmpA1=*adp;goto _LL7;}}else{_LL6: _tmpA1=*(_tmpA0.KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmpA1;
# 415
env=Cyc_Tctyp_check_type_inst(env,targs,ad->tvs,expected_kind,allow_abs_aggr,ad->name,0);
# 417
if((allow_abs_aggr && ad->impl == 0)&& !Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 421
ad->expected_mem_kind=1;
return env;}}_LL3:;}}
# 427
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 430
struct Cyc_Tctyp_CVTEnv _tmpC4=env;struct Cyc_Tcenv_Tenv*_tmpC6;unsigned _tmpC5;_LL1: _tmpC5=_tmpC4.loc;_tmpC6=_tmpC4.te;_LL2: {unsigned loc=_tmpC5;struct Cyc_Tcenv_Tenv*te=_tmpC6;
struct Cyc_List_List*_tmpC7=*targsp;struct Cyc_List_List*targs=_tmpC7;
union Cyc_Absyn_DatatypeInfo _tmpC8=*info;union Cyc_Absyn_DatatypeInfo _stmttmpD=_tmpC8;union Cyc_Absyn_DatatypeInfo _tmpC9=_stmttmpD;struct Cyc_Absyn_Datatypedecl*_tmpCA;int _tmpCC;struct _tuple1*_tmpCB;if((_tmpC9.UnknownDatatype).tag == 1){_LL4: _tmpCB=((_tmpC9.UnknownDatatype).val).name;_tmpCC=((_tmpC9.UnknownDatatype).val).is_extensible;_LL5: {struct _tuple1*n=_tmpCB;int is_x=_tmpCC;
# 434
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpCD;_push_handler(& _tmpCD);{int _tmpCF=0;if(setjmp(_tmpCD.handler))_tmpCF=1;if(!_tmpCF){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);;_pop_handler();}else{void*_tmpCE=(void*)Cyc_Core_get_exn_thrown();void*_tmpD0=_tmpCE;void*_tmpD1;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpD0)->tag == Cyc_Dict_Absent){_LL9: _LLA:
# 438
({struct Cyc_Tcenv_Tenv*_tmp345=te;unsigned _tmp344=loc;Cyc_Tc_tcDatatypedecl(_tmp345,_tmp344,({struct Cyc_Absyn_Datatypedecl*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->sc=Cyc_Absyn_Extern,_tmpD2->name=n,_tmpD2->tvs=0,_tmpD2->fields=0,_tmpD2->is_extensible=is_x;_tmpD2;}));});
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 441
if(targs != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BC;_tmp2BC.tag=0U,({struct _fat_ptr _tmp346=({const char*_tmpD8="declare parameterized datatype ";_tag_fat(_tmpD8,sizeof(char),32U);});_tmp2BC.f1=_tmp346;});_tmp2BC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2BB;_tmp2BB.tag=1U,_tmp2BB.f1=n;_tmp2BB;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA;_tmp2BA.tag=0U,({struct _fat_ptr _tmp347=({const char*_tmpD7=" before using";_tag_fat(_tmpD7,sizeof(char),14U);});_tmp2BA.f1=_tmp347;});_tmp2BA;});void*_tmpD3[3U];_tmpD3[0]=& _tmpD4,_tmpD3[1]=& _tmpD5,_tmpD3[2]=& _tmpD6;({unsigned _tmp348=loc;Cyc_Warn_err2(_tmp348,_tag_fat(_tmpD3,sizeof(void*),3U));});});
return env;}
# 445
goto _LL8;}else{_LLB: _tmpD1=_tmpD0;_LLC: {void*exn=_tmpD1;(int)_rethrow(exn);}}_LL8:;}}}
# 449
if(is_x && !(*tudp)->is_extensible)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BF;_tmp2BF.tag=0U,({struct _fat_ptr _tmp349=({const char*_tmpDE="datatype ";_tag_fat(_tmpDE,sizeof(char),10U);});_tmp2BF.f1=_tmp349;});_tmp2BF;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmpDB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2BE;_tmp2BE.tag=1U,_tmp2BE.f1=n;_tmp2BE;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BD;_tmp2BD.tag=0U,({struct _fat_ptr _tmp34A=({const char*_tmpDD=" was not declared @extensible";_tag_fat(_tmpDD,sizeof(char),30U);});_tmp2BD.f1=_tmp34A;});_tmp2BD;});void*_tmpD9[3U];_tmpD9[0]=& _tmpDA,_tmpD9[1]=& _tmpDB,_tmpD9[2]=& _tmpDC;({unsigned _tmp34B=loc;Cyc_Warn_err2(_tmp34B,_tag_fat(_tmpD9,sizeof(void*),3U));});});
({union Cyc_Absyn_DatatypeInfo _tmp34C=Cyc_Absyn_KnownDatatype(tudp);*info=_tmp34C;});
_tmpCA=*tudp;goto _LL7;}}else{_LL6: _tmpCA=*(_tmpC9.KnownDatatype).val;_LL7: {struct Cyc_Absyn_Datatypedecl*tud=_tmpCA;
# 454
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tud->name,0);}}_LL3:;}}
# 460
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 463
union Cyc_Absyn_DatatypeFieldInfo _tmpDF=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmpE=_tmpDF;union Cyc_Absyn_DatatypeFieldInfo _tmpE0=_stmttmpE;struct Cyc_Absyn_Datatypefield*_tmpE2;struct Cyc_Absyn_Datatypedecl*_tmpE1;int _tmpE5;struct _tuple1*_tmpE4;struct _tuple1*_tmpE3;if((_tmpE0.UnknownDatatypefield).tag == 1){_LL1: _tmpE3=((_tmpE0.UnknownDatatypefield).val).datatype_name;_tmpE4=((_tmpE0.UnknownDatatypefield).val).field_name;_tmpE5=((_tmpE0.UnknownDatatypefield).val).is_extensible;_LL2: {struct _tuple1*tname=_tmpE3;struct _tuple1*fname=_tmpE4;int is_x=_tmpE5;
# 466
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(env.te,env.loc,tname);
struct Cyc_Absyn_Datatypefield*tuf;
# 471
{struct Cyc_List_List*_tmpE6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmpE6;for(0;1;fs=fs->tl){
if(fs == 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmpE8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0;_tmp2C0.tag=0U,({struct _fat_ptr _tmp34D=({const char*_tmpE9="Tcutil found a bad datatypefield";_tag_fat(_tmpE9,sizeof(char),33U);});_tmp2C0.f1=_tmp34D;});_tmp2C0;});void*_tmpE7[1U];_tmpE7[0]=& _tmpE8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE7,sizeof(void*),1U));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)fs->hd)->name,fname)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 478
({union Cyc_Absyn_DatatypeFieldInfo _tmp34E=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmp34E;});
_tmpE1=tud;_tmpE2=tuf;goto _LL4;}}else{_LL3: _tmpE1=((_tmpE0.KnownDatatypefield).val).f1;_tmpE2=((_tmpE0.KnownDatatypefield).val).f2;_LL4: {struct Cyc_Absyn_Datatypedecl*tud=_tmpE1;struct Cyc_Absyn_Datatypefield*tuf=_tmpE2;
# 481
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tuf->name,0);}}_LL0:;}
# 486
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_unary_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*k,struct Cyc_List_List*ts,int put_in_effect,struct _fat_ptr s){
# 488
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2;_tmp2C2.tag=0U,_tmp2C2.f1=s;_tmp2C2;});struct Cyc_Warn_String_Warn_Warg_struct _tmpEC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp34F=({const char*_tmpED=" has wrong number of arguments";_tag_fat(_tmpED,sizeof(char),31U);});_tmp2C1.f1=_tmp34F;});_tmp2C1;});void*_tmpEA[2U];_tmpEA[0]=& _tmpEB,_tmpEA[1]=& _tmpEC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpEA,sizeof(void*),2U));});
return Cyc_Tctyp_i_check_type(env,k,(void*)((struct Cyc_List_List*)_check_null(ts))->hd,put_in_effect,1);}
# 493
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 496
struct Cyc_Tctyp_CVTEnv _tmpEE=env;struct Cyc_Tcenv_Tenv*_tmpF0;unsigned _tmpEF;_LL1: _tmpEF=_tmpEE.loc;_tmpF0=_tmpEE.te;_LL2: {unsigned loc=_tmpEF;struct Cyc_Tcenv_Tenv*te=_tmpF0;
struct Cyc_List_List*_tmpF1=*targsp;struct Cyc_List_List*ts=_tmpF1;
void*_tmpF2=c;union Cyc_Absyn_DatatypeFieldInfo*_tmpF3;union Cyc_Absyn_DatatypeInfo*_tmpF4;union Cyc_Absyn_AggrInfo*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_Absyn_Enumdecl**_tmpF8;struct _tuple1*_tmpF7;switch(*((int*)_tmpF2)){case 1U: _LL4: _LL5:
# 500
 goto _LL7;case 2U: _LL6: _LL7: goto _LL9;case 0U: _LL8: _LL9: goto _LLB;case 7U: _LLA: _LLB:
 goto _LLD;case 6U: _LLC: _LLD: goto _LLF;case 5U: _LLE: _LLF: goto _LL11;case 12U: _LL10: _LL11:
 goto _LL13;case 11U: _LL12: _LL13: goto _LL15;case 14U: _LL14: _LL15: goto _LL17;case 17U: _LL16: _LL17:
# 504
 if(ts != 0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmpFA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C4;_tmp2C4.tag=2U,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->tag=0U,_tmpFD->f1=c,_tmpFD->f2=ts;_tmpFD;})))_tmp350=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->tag=0U,_tmpFD->f1=c,_tmpFD->f2=ts;_tmpFD;}));_tmp2C4.f1=_tmp350;});_tmp2C4;});struct Cyc_Warn_String_Warn_Warg_struct _tmpFB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3;_tmp2C3.tag=0U,({struct _fat_ptr _tmp351=({const char*_tmpFC=" applied to argument(s)";_tag_fat(_tmpFC,sizeof(char),24U);});_tmp2C3.f1=_tmp351;});_tmp2C3;});void*_tmpF9[2U];_tmpF9[0]=& _tmpFA,_tmpF9[1]=& _tmpFB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpF9,sizeof(void*),2U));});
return env;case 9U: _LL18: _LL19:
# 508
 for(0;ts != 0;ts=ts->tl){
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_ek,(void*)ts->hd,1,1);}
return env;case 15U: _LL1A: _tmpF7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpF2)->f1;_tmpF8=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmpF2)->f2;_LL1B: {struct _tuple1*n=_tmpF7;struct Cyc_Absyn_Enumdecl**edp=_tmpF8;
# 512
if(ts != 0)({struct Cyc_Warn_String_Warn_Warg_struct _tmpFF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5;_tmp2C5.tag=0U,({struct _fat_ptr _tmp352=({const char*_tmp100="enum applied to argument(s)";_tag_fat(_tmp100,sizeof(char),28U);});_tmp2C5.f1=_tmp352;});_tmp2C5;});void*_tmpFE[1U];_tmpFE[0]=& _tmpFF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpFE,sizeof(void*),1U));});
if(*edp == 0 ||((struct Cyc_Absyn_Enumdecl*)_check_null(*edp))->fields == 0){
struct _handler_cons _tmp101;_push_handler(& _tmp101);{int _tmp103=0;if(setjmp(_tmp101.handler))_tmp103=1;if(!_tmp103){({struct Cyc_Absyn_Enumdecl*_tmp353=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp353;});;_pop_handler();}else{void*_tmp102=(void*)Cyc_Core_get_exn_thrown();void*_tmp104=_tmp102;void*_tmp105;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp104)->tag == Cyc_Dict_Absent){_LL2F: _LL30:
# 516
({struct Cyc_Tcenv_Tenv*_tmp355=te;unsigned _tmp354=loc;Cyc_Tc_tcEnumdecl(_tmp355,_tmp354,({struct Cyc_Absyn_Enumdecl*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->sc=Cyc_Absyn_Extern,_tmp106->name=n,_tmp106->fields=0;_tmp106;}));});
({struct Cyc_Absyn_Enumdecl*_tmp356=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_tmp356;});
goto _LL2E;}else{_LL31: _tmp105=_tmp104;_LL32: {void*exn=_tmp105;(int)_rethrow(exn);}}_LL2E:;}}}
# 521
return env;}case 16U: _LL1C: _tmpF6=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmpF2)->f1;_LL1D: {struct Cyc_List_List*fs=_tmpF6;
# 523
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp108=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C6;_tmp2C6.tag=0U,({struct _fat_ptr _tmp357=({const char*_tmp109="enum applied to argument(s)";_tag_fat(_tmp109,sizeof(char),28U);});_tmp2C6.f1=_tmp357;});_tmp2C6;});void*_tmp107[1U];_tmp107[0]=& _tmp108;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp107,sizeof(void*),1U));});{
# 526
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp10A=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp10A;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8;_tmp2C8.tag=0U,({struct _fat_ptr _tmp358=({const char*_tmp10E="duplicate enum field name ";_tag_fat(_tmp10E,sizeof(char),27U);});_tmp2C8.f1=_tmp358;});_tmp2C8;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10D=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2C7;_tmp2C7.tag=1U,_tmp2C7.f1=f->name;_tmp2C7;});void*_tmp10B[2U];_tmp10B[0]=& _tmp10C,_tmp10B[1]=& _tmp10D;({unsigned _tmp359=f->loc;Cyc_Warn_err2(_tmp359,_tag_fat(_tmp10B,sizeof(void*),2U));});});else{
# 533
prev_fields=({struct Cyc_List_List*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->hd=(*f->name).f2,_tmp10F->tl=prev_fields;_tmp10F;});}
if(f->tag == 0)
({struct Cyc_Absyn_Exp*_tmp35A=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_tmp35A;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp111=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CB;_tmp2CB.tag=0U,({struct _fat_ptr _tmp35B=({const char*_tmp115="enum field ";_tag_fat(_tmp115,sizeof(char),12U);});_tmp2CB.f1=_tmp35B;});_tmp2CB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp112=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2CA;_tmp2CA.tag=1U,_tmp2CA.f1=f->name;_tmp2CA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp113=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C9;_tmp2C9.tag=0U,({struct _fat_ptr _tmp35C=({const char*_tmp114=": expression is not constant";_tag_fat(_tmp114,sizeof(char),29U);});_tmp2C9.f1=_tmp35C;});_tmp2C9;});void*_tmp110[3U];_tmp110[0]=& _tmp111,_tmp110[1]=& _tmp112,_tmp110[2]=& _tmp113;({unsigned _tmp35D=loc;Cyc_Warn_err2(_tmp35D,_tag_fat(_tmp110,sizeof(void*),3U));});});}
tag_count=(unsigned)1 + (Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(f->tag))).f1;}
# 540
return env;}}case 10U: _LL1E: _LL1F:
 return({struct Cyc_Tctyp_CVTEnv _tmp35F=env;struct Cyc_List_List*_tmp35E=ts;Cyc_Tctyp_i_check_unary_app(_tmp35F,& Cyc_Tcutil_tak,_tmp35E,1,({const char*_tmp116="regions";_tag_fat(_tmp116,sizeof(char),8U);}));});case 3U: _LL20: _LL21:
 return({struct Cyc_Tctyp_CVTEnv _tmp361=env;struct Cyc_List_List*_tmp360=ts;Cyc_Tctyp_i_check_unary_app(_tmp361,& Cyc_Tcutil_trk,_tmp360,1,({const char*_tmp117="region_t";_tag_fat(_tmp117,sizeof(char),9U);}));});case 8U: _LL22: _LL23:
 return({struct Cyc_Tctyp_CVTEnv _tmp363=env;struct Cyc_List_List*_tmp362=ts;Cyc_Tctyp_i_check_unary_app(_tmp363,& Cyc_Tcutil_trk,_tmp362,1,({const char*_tmp118="access";_tag_fat(_tmp118,sizeof(char),7U);}));});case 4U: _LL24: _LL25:
 return({struct Cyc_Tctyp_CVTEnv _tmp365=env;struct Cyc_List_List*_tmp364=ts;Cyc_Tctyp_i_check_unary_app(_tmp365,& Cyc_Tcutil_ik,_tmp364,0,({const char*_tmp119="tag_t";_tag_fat(_tmp119,sizeof(char),6U);}));});case 13U: _LL26: _LL27:
 return({struct Cyc_Tctyp_CVTEnv _tmp367=env;struct Cyc_List_List*_tmp366=ts;Cyc_Tctyp_i_check_unary_app(_tmp367,& Cyc_Tcutil_ik,_tmp366,0,({const char*_tmp11A="@thin";_tag_fat(_tmp11A,sizeof(char),6U);}));});case 20U: _LL28: _tmpF5=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpF2)->f1;_LL29: {union Cyc_Absyn_AggrInfo*info=_tmpF5;
# 547
return Cyc_Tctyp_i_check_aggr(env,expected_kind,info,targsp,allow_abs_aggr);}case 18U: _LL2A: _tmpF4=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmpF2)->f1;_LL2B: {union Cyc_Absyn_DatatypeInfo*info=_tmpF4;
# 549
return Cyc_Tctyp_i_check_datatype(env,expected_kind,info,targsp,allow_abs_aggr);}default: _LL2C: _tmpF3=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpF2)->f1;_LL2D: {union Cyc_Absyn_DatatypeFieldInfo*info=_tmpF3;
# 551
return Cyc_Tctyp_i_check_datatype_field(env,expected_kind,info,targsp,allow_abs_aggr);}}_LL3:;}}struct _tuple16{enum Cyc_Absyn_Format_Type f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 556
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 558
struct Cyc_Tctyp_CVTEnv _tmp11B=env;struct Cyc_Tcenv_Tenv*_tmp11D;unsigned _tmp11C;_LL1: _tmp11C=_tmp11B.loc;_tmp11D=_tmp11B.te;_LL2: {unsigned loc=_tmp11C;struct Cyc_Tcenv_Tenv*te=_tmp11D;
{void*_tmp11E=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp11E;void*_tmp11F=_stmttmpF;void**_tmp123;struct Cyc_Absyn_Typedefdecl**_tmp122;struct Cyc_List_List**_tmp121;struct _tuple1*_tmp120;struct Cyc_List_List*_tmp125;enum Cyc_Absyn_AggrKind _tmp124;struct Cyc_List_List*_tmp126;struct Cyc_List_List**_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_List_List**_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp12E;struct Cyc_Absyn_VarargInfo*_tmp12D;int _tmp12C;struct Cyc_List_List*_tmp12B;void*_tmp12A;struct Cyc_Absyn_Tqual*_tmp129;void**_tmp128;struct Cyc_List_List**_tmp127;unsigned _tmp138;void*_tmp137;struct Cyc_Absyn_Exp**_tmp136;struct Cyc_Absyn_Tqual*_tmp135;void*_tmp134;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;void*_tmp140;void*_tmp13F;void*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Tqual*_tmp13C;void*_tmp13B;void***_tmp142;void*_tmp141;struct Cyc_Absyn_Tvar*_tmp143;void**_tmp145;struct Cyc_Core_Opt**_tmp144;struct Cyc_List_List**_tmp147;void*_tmp146;switch(*((int*)_tmp11F)){case 0U: _LL4: _tmp146=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11F)->f1;_tmp147=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11F)->f2;_LL5: {void*c=_tmp146;struct Cyc_List_List**targsp=_tmp147;
# 561
env=Cyc_Tctyp_i_check_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr);
# 563
goto _LL3;}case 1U: _LL6: _tmp144=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp11F)->f1;_tmp145=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp11F)->f2;_LL7: {struct Cyc_Core_Opt**k=_tmp144;void**ref=_tmp145;
# 566
if(*k == 0 ||
 Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v)&& !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*k))->v,expected_kind))
({struct Cyc_Core_Opt*_tmp368=Cyc_Tcutil_kind_to_opt(expected_kind);*k=_tmp368;});
if(((env.fn_result && env.generalize_evars)&&(int)expected_kind->kind == (int)Cyc_Absyn_RgnKind)&& !te->tempest_generalize){
# 571
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*ref=Cyc_Absyn_unique_rgn_type;else{
# 574
*ref=Cyc_Absyn_heap_rgn_type;}}else{
if((env.generalize_evars &&(int)expected_kind->kind != (int)Cyc_Absyn_BoolKind)&&(int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 578
struct Cyc_Absyn_Tvar*_tmp148=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->tag=2U,_tmp149->f1=0,_tmp149->f2=expected_kind;_tmp149;}));struct Cyc_Absyn_Tvar*v=_tmp148;
({void*_tmp369=Cyc_Absyn_var_type(v);*ref=_tmp369;});
_tmp143=v;goto _LL9;}else{
# 582
({struct Cyc_List_List*_tmp36A=Cyc_Tctyp_add_free_evar(env.free_evars,t,put_in_effect);env.free_evars=_tmp36A;});}}
# 584
goto _LL3;}case 2U: _LL8: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp11F)->f1;_LL9: {struct Cyc_Absyn_Tvar*v=_tmp143;
# 586
{void*_tmp14A=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp10=_tmp14A;void*_tmp14B=_stmttmp10;struct Cyc_Core_Opt**_tmp14C;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp14B)->tag == 1U){_LL1D: _tmp14C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp14B)->f1;_LL1E: {struct Cyc_Core_Opt**f=_tmp14C;
({struct Cyc_Core_Opt*_tmp36C=({struct Cyc_Core_Opt*_tmp14E=_cycalloc(sizeof(*_tmp14E));({void*_tmp36B=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->tag=2U,_tmp14D->f1=0,_tmp14D->f2=expected_kind;_tmp14D;});_tmp14E->v=_tmp36B;});_tmp14E;});*f=_tmp36C;});goto _LL1C;}}else{_LL1F: _LL20:
 goto _LL1C;}_LL1C:;}
# 592
({struct Cyc_List_List*_tmp36D=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,v);env.kind_env=_tmp36D;});
# 595
({struct Cyc_List_List*_tmp36E=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,v,put_in_effect);env.free_vars=_tmp36E;});
# 597
{void*_tmp14F=Cyc_Absyn_compress_kb(v->kind);void*_stmttmp11=_tmp14F;void*_tmp150=_stmttmp11;struct Cyc_Absyn_Kind*_tmp152;struct Cyc_Core_Opt**_tmp151;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp150)->tag == 2U){_LL22: _tmp151=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp150)->f1;_tmp152=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp150)->f2;_LL23: {struct Cyc_Core_Opt**f=_tmp151;struct Cyc_Absyn_Kind*k=_tmp152;
# 599
if(Cyc_Tcutil_kind_leq(expected_kind,k))
({struct Cyc_Core_Opt*_tmp370=({struct Cyc_Core_Opt*_tmp154=_cycalloc(sizeof(*_tmp154));({void*_tmp36F=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->tag=2U,_tmp153->f1=0,_tmp153->f2=expected_kind;_tmp153;});_tmp154->v=_tmp36F;});_tmp154;});*f=_tmp370;});
goto _LL21;}}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 604
goto _LL3;}case 10U: _LLA: _tmp141=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11F)->f1)->r;_tmp142=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11F)->f2;_LLB: {void*td=_tmp141;void***topt=_tmp142;
# 609
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Absyn_compress(t));
{void*_tmp155=td;struct Cyc_Absyn_Datatypedecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;struct Cyc_Absyn_Aggrdecl*_tmp158;switch(*((int*)_tmp155)){case 0U: _LL27: _tmp158=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp155)->f1;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp158;
# 612
if((env.te)->in_extern_c_include)
ad->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL26;}case 1U: _LL29: _tmp157=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp155)->f1;_LL2A: {struct Cyc_Absyn_Enumdecl*ed=_tmp157;
# 616
if((env.te)->in_extern_c_include)
ed->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL26;}default: _LL2B: _tmp156=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp155)->f1;_LL2C: {struct Cyc_Absyn_Datatypedecl*dd=_tmp156;
# 620
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL26;}}_LL26:;}
# 622
({void**_tmp371=({void**_tmp159=_cycalloc(sizeof(*_tmp159));*_tmp159=new_typ;_tmp159;});*topt=_tmp371;});
return Cyc_Tctyp_i_check_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3U: _LLC: _tmp13B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11F)->f1).elt_type;_tmp13C=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11F)->f1).elt_tq;_tmp13D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11F)->f1).ptr_atts).rgn;_tmp13E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11F)->f1).ptr_atts).nullable;_tmp13F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11F)->f1).ptr_atts).bounds;_tmp140=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11F)->f1).ptr_atts).zero_term;_LLD: {void*t1=_tmp13B;struct Cyc_Absyn_Tqual*tqp=_tmp13C;void*rgn_type=_tmp13D;void*nullable=_tmp13E;void*b=_tmp13F;void*zt=_tmp140;
# 627
struct Cyc_Absyn_Kind*rgnk;
{enum Cyc_Absyn_AliasQual _tmp15A=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp12=_tmp15A;enum Cyc_Absyn_AliasQual _tmp15B=_stmttmp12;switch(_tmp15B){case Cyc_Absyn_Aliasable: _LL2E: _LL2F:
 rgnk=& Cyc_Tcutil_rk;goto _LL2D;case Cyc_Absyn_Unique: _LL30: _LL31:
 rgnk=& Cyc_Tcutil_urk;goto _LL2D;case Cyc_Absyn_Top: _LL32: _LL33:
 goto _LL35;default: _LL34: _LL35: rgnk=& Cyc_Tcutil_trk;goto _LL2D;}_LL2D:;}
# 633
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tak,t1,1,1);
env=Cyc_Tctyp_i_check_type(env,rgnk,rgn_type,1,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_boolk,zt,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_boolk,nullable,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_ptrbk,b,0,allow_abs_aggr);
# 639
({int _tmp372=Cyc_Tcutil_extract_const_from_typedef(env.loc,tqp->print_const,t1);tqp->real_const=_tmp372;});
# 643
({void*_tmp373=zt;Cyc_Unify_unify(_tmp373,Cyc_Tcutil_is_char_type(t1)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});{
int is_zero_terminated=Cyc_Absyn_type2bool(0,zt);
if(is_zero_terminated && !Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CE;_tmp2CE.tag=0U,({struct _fat_ptr _tmp374=({const char*_tmp161="cannot have a pointer to zero-terminated ";_tag_fat(_tmp161,sizeof(char),42U);});_tmp2CE.f1=_tmp374;});_tmp2CE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2CD;_tmp2CD.tag=2U,_tmp2CD.f1=(void*)t1;_tmp2CD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp15F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC;_tmp2CC.tag=0U,({struct _fat_ptr _tmp375=({const char*_tmp160=" elements";_tag_fat(_tmp160,sizeof(char),10U);});_tmp2CC.f1=_tmp375;});_tmp2CC;});void*_tmp15C[3U];_tmp15C[0]=& _tmp15D,_tmp15C[1]=& _tmp15E,_tmp15C[2]=& _tmp15F;({unsigned _tmp376=loc;Cyc_Warn_err2(_tmp376,_tag_fat(_tmp15C,sizeof(void*),3U));});});{
# 649
struct Cyc_Absyn_Exp*_tmp162=({void*_tmp377=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp377,b);});struct Cyc_Absyn_Exp*e=_tmp162;
if(e != 0){
struct _tuple12 _tmp163=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp13=_tmp163;struct _tuple12 _tmp164=_stmttmp13;int _tmp166;unsigned _tmp165;_LL37: _tmp165=_tmp164.f1;_tmp166=_tmp164.f2;_LL38: {unsigned sz=_tmp165;int known=_tmp166;
if(is_zero_terminated &&(!known || sz < (unsigned)1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp168=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF;_tmp2CF.tag=0U,({struct _fat_ptr _tmp378=({const char*_tmp169="zero-terminated pointer cannot point to empty sequence";_tag_fat(_tmp169,sizeof(char),55U);});_tmp2CF.f1=_tmp378;});_tmp2CF;});void*_tmp167[1U];_tmp167[0]=& _tmp168;({unsigned _tmp379=loc;Cyc_Warn_err2(_tmp379,_tag_fat(_tmp167,sizeof(void*),1U));});});}}
# 655
goto _LL3;}}}case 9U: _LLE: _tmp13A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp11F)->f1;_LLF: {struct Cyc_Absyn_Exp*e=_tmp13A;
# 660
({struct Cyc_Tcenv_Tenv*_tmp37A=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp37A,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D0;_tmp2D0.tag=0U,({struct _fat_ptr _tmp37B=({const char*_tmp16C="valueof_t requires an int expression";_tag_fat(_tmp16C,sizeof(char),37U);});_tmp2D0.f1=_tmp37B;});_tmp2D0;});void*_tmp16A[1U];_tmp16A[0]=& _tmp16B;({unsigned _tmp37C=loc;Cyc_Warn_err2(_tmp37C,_tag_fat(_tmp16A,sizeof(void*),1U));});});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);
goto _LL3;}case 11U: _LL10: _tmp139=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp11F)->f1;_LL11: {struct Cyc_Absyn_Exp*e=_tmp139;
# 669
({struct Cyc_Tcenv_Tenv*_tmp37D=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp37D,0,e);});
goto _LL3;}case 4U: _LL12: _tmp134=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11F)->f1).elt_type;_tmp135=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11F)->f1).tq;_tmp136=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11F)->f1).num_elts;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11F)->f1).zero_term;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11F)->f1).zt_loc;_LL13: {void*t1=_tmp134;struct Cyc_Absyn_Tqual*tqp=_tmp135;struct Cyc_Absyn_Exp**eptr=_tmp136;void*zt=_tmp137;unsigned ztl=_tmp138;
# 674
struct Cyc_Absyn_Exp*_tmp16D=*eptr;struct Cyc_Absyn_Exp*e=_tmp16D;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tmk,t1,1,allow_abs_aggr);
({int _tmp37E=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_tmp37E;});{
# 678
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);
if(is_zero_terminated){
# 681
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3;_tmp2D3.tag=0U,({struct _fat_ptr _tmp37F=({const char*_tmp173="cannot have a zero-terminated array of ";_tag_fat(_tmp173,sizeof(char),40U);});_tmp2D3.f1=_tmp37F;});_tmp2D3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp170=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2D2;_tmp2D2.tag=2U,_tmp2D2.f1=(void*)t1;_tmp2D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp171=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D1;_tmp2D1.tag=0U,({struct _fat_ptr _tmp380=({const char*_tmp172=" elements";_tag_fat(_tmp172,sizeof(char),10U);});_tmp2D1.f1=_tmp380;});_tmp2D1;});void*_tmp16E[3U];_tmp16E[0]=& _tmp16F,_tmp16E[1]=& _tmp170,_tmp16E[2]=& _tmp171;({unsigned _tmp381=loc;Cyc_Warn_err2(_tmp381,_tag_fat(_tmp16E,sizeof(void*),3U));});});}
# 686
if(e == 0){
# 689
if(!is_zero_terminated)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp175=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4;_tmp2D4.tag=0U,({struct _fat_ptr _tmp382=({const char*_tmp176="array bound defaults to 1 here";_tag_fat(_tmp176,sizeof(char),31U);});_tmp2D4.f1=_tmp382;});_tmp2D4;});void*_tmp174[1U];_tmp174[0]=& _tmp175;({unsigned _tmp383=loc;Cyc_Warn_warn2(_tmp383,_tag_fat(_tmp174,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp384=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp384;});}
# 693
({struct Cyc_Tcenv_Tenv*_tmp385=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp385,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp178=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5;_tmp2D5.tag=0U,({struct _fat_ptr _tmp386=({const char*_tmp179="array bounds expression is not an unsigned int";_tag_fat(_tmp179,sizeof(char),47U);});_tmp2D5.f1=_tmp386;});_tmp2D5;});void*_tmp177[1U];_tmp177[0]=& _tmp178;({unsigned _tmp387=loc;Cyc_Warn_err2(_tmp387,_tag_fat(_tmp177,sizeof(void*),1U));});});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);{
struct _tuple12 _tmp17A=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp14=_tmp17A;struct _tuple12 _tmp17B=_stmttmp14;int _tmp17D;unsigned _tmp17C;_LL3A: _tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;_LL3B: {unsigned sz=_tmp17C;int known=_tmp17D;
# 699
if((is_zero_terminated && known)&& sz < (unsigned)1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D6;_tmp2D6.tag=0U,({struct _fat_ptr _tmp388=({const char*_tmp180="zero terminated array cannot have zero elements";_tag_fat(_tmp180,sizeof(char),48U);});_tmp2D6.f1=_tmp388;});_tmp2D6;});void*_tmp17E[1U];_tmp17E[0]=& _tmp17F;({unsigned _tmp389=loc;Cyc_Warn_warn2(_tmp389,_tag_fat(_tmp17E,sizeof(void*),1U));});});
# 702
if((known && sz < (unsigned)1)&& Cyc_Flags_tovc_r){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp182=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D7;_tmp2D7.tag=0U,({struct _fat_ptr _tmp38A=({const char*_tmp183="0-element arrays are supported only with gcc, changing to 1";_tag_fat(_tmp183,sizeof(char),60U);});_tmp2D7.f1=_tmp38A;});_tmp2D7;});void*_tmp181[1U];_tmp181[0]=& _tmp182;({unsigned _tmp38B=loc;Cyc_Warn_warn2(_tmp38B,_tag_fat(_tmp181,sizeof(void*),1U));});});
({struct Cyc_Absyn_Exp*_tmp38C=Cyc_Absyn_uint_exp(1U,0U);*eptr=_tmp38C;});}
# 706
goto _LL3;}}}}case 5U: _LL14: _tmp127=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).tvars;_tmp128=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).effect;_tmp129=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).ret_tqual;_tmp12A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).ret_type;_tmp12B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).args;_tmp12C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).c_varargs;_tmp12D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).cyc_varargs;_tmp12E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).rgn_po;_tmp12F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).attributes;_tmp130=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).requires_clause;_tmp131=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).requires_relns;_tmp132=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).ensures_clause;_tmp133=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11F)->f1).ensures_relns;_LL15: {struct Cyc_List_List**btvs=_tmp127;void**eff=_tmp128;struct Cyc_Absyn_Tqual*rtq=_tmp129;void*tr=_tmp12A;struct Cyc_List_List*args=_tmp12B;int c_vararg=_tmp12C;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp12D;struct Cyc_List_List*rgn_po=_tmp12E;struct Cyc_List_List*atts=_tmp12F;struct Cyc_Absyn_Exp*req=_tmp130;struct Cyc_List_List**req_relns=_tmp131;struct Cyc_Absyn_Exp*ens=_tmp132;struct Cyc_List_List**ens_relns=_tmp133;
# 714
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;atts != 0;atts=atts->tl){
void*_tmp184=(void*)atts->hd;void*_stmttmp15=_tmp184;void*_tmp185=_stmttmp15;int _tmp188;int _tmp187;enum Cyc_Absyn_Format_Type _tmp186;switch(*((int*)_tmp185)){case 1U: _LL3D: _LL3E:
 is_stdcall=1;goto _LL3C;case 2U: _LL3F: _LL40:
 is_cdecl=1;goto _LL3C;case 3U: _LL41: _LL42:
 is_fastcall=1;goto _LL3C;case 19U: _LL43: _tmp186=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp185)->f1;_tmp187=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp185)->f2;_tmp188=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp185)->f3;_LL44: {enum Cyc_Absyn_Format_Type fmttype=_tmp186;int i=_tmp187;int j=_tmp188;
# 727
++ num_formats;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;
goto _LL3C;}default: _LL45: _LL46:
# 733
 if(!Cyc_Atts_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9;_tmp2D9.tag=0U,({struct _fat_ptr _tmp38D=({const char*_tmp18C="bad function type attribute ";_tag_fat(_tmp18C,sizeof(char),29U);});_tmp2D9.f1=_tmp38D;});_tmp2D9;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp18B=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp2D8;_tmp2D8.tag=10U,_tmp2D8.f1=(void*)atts->hd;_tmp2D8;});void*_tmp189[2U];_tmp189[0]=& _tmp18A,_tmp189[1]=& _tmp18B;({unsigned _tmp38E=loc;Cyc_Warn_err2(_tmp38E,_tag_fat(_tmp189,sizeof(void*),2U));});});}_LL3C:;}
# 736
if((is_stdcall + is_cdecl)+ is_fastcall > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA;_tmp2DA.tag=0U,({struct _fat_ptr _tmp38F=({const char*_tmp18F="function can't have multiple calling conventions";_tag_fat(_tmp18F,sizeof(char),49U);});_tmp2DA.f1=_tmp38F;});_tmp2DA;});void*_tmp18D[1U];_tmp18D[0]=& _tmp18E;({unsigned _tmp390=loc;Cyc_Warn_err2(_tmp390,_tag_fat(_tmp18D,sizeof(void*),1U));});});
if(num_formats > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp191=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DB;_tmp2DB.tag=0U,({struct _fat_ptr _tmp391=({const char*_tmp192="function can't have multiple format attributes";_tag_fat(_tmp192,sizeof(char),47U);});_tmp2DB.f1=_tmp391;});_tmp2DB;});void*_tmp190[1U];_tmp190[0]=& _tmp191;({unsigned _tmp392=loc;Cyc_Warn_err2(_tmp392,_tag_fat(_tmp190,sizeof(void*),1U));});});
# 743
Cyc_Tcutil_check_unique_tvars(loc,*btvs);
{struct Cyc_List_List*b=*btvs;for(0;b != 0;b=b->tl){
({int _tmp393=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmp393;});
({struct Cyc_List_List*_tmp394=Cyc_Tctyp_add_bound_tvar(env.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);env.kind_env=_tmp394;});{
void*_tmp193=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_stmttmp16=_tmp193;void*_tmp194=_stmttmp16;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp194)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp194)->f1)->kind == Cyc_Absyn_MemKind){_LL48: _LL49:
# 749
({struct Cyc_Warn_String_Warn_Warg_struct _tmp196=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DD;_tmp2DD.tag=0U,({struct _fat_ptr _tmp395=({const char*_tmp198="function abstracts Mem type variable ";_tag_fat(_tmp198,sizeof(char),38U);});_tmp2DD.f1=_tmp395;});_tmp2DD;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp197=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2DC;_tmp2DC.tag=7U,_tmp2DC.f1=(struct Cyc_Absyn_Tvar*)b->hd;_tmp2DC;});void*_tmp195[2U];_tmp195[0]=& _tmp196,_tmp195[1]=& _tmp197;({unsigned _tmp396=loc;Cyc_Warn_err2(_tmp396,_tag_fat(_tmp195,sizeof(void*),2U));});});goto _LL47;}else{goto _LL4A;}}else{_LL4A: _LL4B:
 goto _LL47;}_LL47:;}}}{
# 755
struct Cyc_Tctyp_CVTEnv new_env=({struct Cyc_Tctyp_CVTEnv _tmp2EE;_tmp2EE.loc=loc,_tmp2EE.te=te,_tmp2EE.kind_env=env.kind_env,_tmp2EE.fn_result=1,_tmp2EE.generalize_evars=env.generalize_evars,_tmp2EE.free_vars=0,_tmp2EE.free_evars=0;_tmp2EE;});
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_tmk,tr,1,1);
({int _tmp397=Cyc_Tcutil_extract_const_from_typedef(loc,rtq->print_const,tr);rtq->real_const=_tmp397;});
new_env.fn_result=0;
# 761
{struct Cyc_List_List*a=args;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp199=(struct _tuple9*)a->hd;struct _tuple9*trip=_tmp199;
void*_tmp19A=(*trip).f3;void*t=_tmp19A;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_tmk,t,1,1);{
int _tmp19B=Cyc_Tcutil_extract_const_from_typedef(loc,((*trip).f2).print_const,t);int ec=_tmp19B;
((*trip).f2).real_const=ec;
# 769
if(Cyc_Tcutil_is_array_type(t)){
# 771
void*_tmp19C=Cyc_Absyn_new_evar(0,0);void*ptr_rgn=_tmp19C;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_rk,ptr_rgn,1,1);
({void*_tmp398=Cyc_Tcutil_promote_array(t,ptr_rgn,0);(*trip).f3=_tmp398;});}}}}
# 777
if(cyc_vararg != 0){
if(c_vararg)({struct Cyc_Warn_String_Warn_Warg_struct _tmp19E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE;_tmp2DE.tag=0U,({struct _fat_ptr _tmp399=({const char*_tmp19F="both c_vararg and cyc_vararg";_tag_fat(_tmp19F,sizeof(char),29U);});_tmp2DE.f1=_tmp399;});_tmp2DE;});void*_tmp19D[1U];_tmp19D[0]=& _tmp19E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp19D,sizeof(void*),1U));});{
struct Cyc_Absyn_VarargInfo _tmp1A0=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp17=_tmp1A0;struct Cyc_Absyn_VarargInfo _tmp1A1=_stmttmp17;int _tmp1A3;void*_tmp1A2;_LL4D: _tmp1A2=_tmp1A1.type;_tmp1A3=_tmp1A1.inject;_LL4E: {void*vt=_tmp1A2;int vi=_tmp1A3;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_tmk,vt,1,1);
({int _tmp39A=Cyc_Tcutil_extract_const_from_typedef(loc,(cyc_vararg->tq).print_const,vt);(cyc_vararg->tq).real_const=_tmp39A;});
# 783
if(vi){
void*_tmp1A4=Cyc_Absyn_compress(vt);void*_stmttmp18=_tmp1A4;void*_tmp1A5=_stmttmp18;void*_tmp1A8;void*_tmp1A7;void*_tmp1A6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->tag == 3U){_LL50: _tmp1A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_type;_tmp1A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).ptr_atts).bounds;_tmp1A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).ptr_atts).zero_term;_LL51: {void*et=_tmp1A6;void*bs=_tmp1A7;void*zt=_tmp1A8;
# 786
{void*_tmp1A9=Cyc_Absyn_compress(et);void*_stmttmp19=_tmp1A9;void*_tmp1AA=_stmttmp19;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 18U){_LL55: _LL56:
# 788
 if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF;_tmp2DF.tag=0U,({struct _fat_ptr _tmp39B=({const char*_tmp1AD="can't inject into a zeroterm pointer";_tag_fat(_tmp1AD,sizeof(char),37U);});_tmp2DF.f1=_tmp39B;});_tmp2DF;});void*_tmp1AB[1U];_tmp1AB[0]=& _tmp1AC;({unsigned _tmp39C=loc;Cyc_Warn_err2(_tmp39C,_tag_fat(_tmp1AB,sizeof(void*),1U));});});
if(!({void*_tmp39D=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp39D,bs);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0;_tmp2E0.tag=0U,({struct _fat_ptr _tmp39E=({const char*_tmp1B0="can't inject into a fat pointer to datatype";_tag_fat(_tmp1B0,sizeof(char),44U);});_tmp2E0.f1=_tmp39E;});_tmp2E0;});void*_tmp1AE[1U];_tmp1AE[0]=& _tmp1AF;({unsigned _tmp39F=loc;Cyc_Warn_err2(_tmp39F,_tag_fat(_tmp1AE,sizeof(void*),1U));});});
goto _LL54;}else{goto _LL57;}}else{_LL57: _LL58:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1;_tmp2E1.tag=0U,({struct _fat_ptr _tmp3A0=({const char*_tmp1B3="can't inject a non-datatype type";_tag_fat(_tmp1B3,sizeof(char),33U);});_tmp2E1.f1=_tmp3A0;});_tmp2E1;});void*_tmp1B1[1U];_tmp1B1[0]=& _tmp1B2;({unsigned _tmp3A1=loc;Cyc_Warn_err2(_tmp3A1,_tag_fat(_tmp1B1,sizeof(void*),1U));});});goto _LL54;}_LL54:;}
# 795
goto _LL4F;}}else{_LL52: _LL53:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2;_tmp2E2.tag=0U,({struct _fat_ptr _tmp3A2=({const char*_tmp1B6="expecting a datatype pointer type";_tag_fat(_tmp1B6,sizeof(char),34U);});_tmp2E2.f1=_tmp3A2;});_tmp2E2;});void*_tmp1B4[1U];_tmp1B4[0]=& _tmp1B5;({unsigned _tmp3A3=loc;Cyc_Warn_err2(_tmp3A3,_tag_fat(_tmp1B4,sizeof(void*),1U));});});goto _LL4F;}_LL4F:;}}}}
# 800
if(num_formats > 0){
int _tmp1B7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args);int num_args=_tmp1B7;
if((((fmt_desc_arg < 0 || fmt_desc_arg > num_args)|| fmt_arg_start < 0)||
# 804
(cyc_vararg == 0 && !c_vararg)&& fmt_arg_start != 0)||
(cyc_vararg != 0 || c_vararg)&& fmt_arg_start != num_args + 1)
# 807
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=0U,({struct _fat_ptr _tmp3A4=({const char*_tmp1BA="bad format descriptor";_tag_fat(_tmp1BA,sizeof(char),22U);});_tmp2E3.f1=_tmp3A4;});_tmp2E3;});void*_tmp1B8[1U];_tmp1B8[0]=& _tmp1B9;({unsigned _tmp3A5=loc;Cyc_Warn_err2(_tmp3A5,_tag_fat(_tmp1B8,sizeof(void*),1U));});});else{
# 810
struct _tuple9 _tmp1BB=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_desc_arg - 1);struct _tuple9 _stmttmp1A=_tmp1BB;struct _tuple9 _tmp1BC=_stmttmp1A;void*_tmp1BD;_LL5A: _tmp1BD=_tmp1BC.f3;_LL5B: {void*t=_tmp1BD;
# 812
{void*_tmp1BE=Cyc_Absyn_compress(t);void*_stmttmp1B=_tmp1BE;void*_tmp1BF=_stmttmp1B;void*_tmp1C1;void*_tmp1C0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BF)->tag == 3U){_LL5D: _tmp1C0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BF)->f1).elt_type;_tmp1C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BF)->f1).ptr_atts).bounds;_LL5E: {void*et=_tmp1C0;void*b=_tmp1C1;
# 815
if(!Cyc_Tcutil_is_char_type(et))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0U,({struct _fat_ptr _tmp3A6=({const char*_tmp1C4="format descriptor is not a string";_tag_fat(_tmp1C4,sizeof(char),34U);});_tmp2E4.f1=_tmp3A6;});_tmp2E4;});void*_tmp1C2[1U];_tmp1C2[0]=& _tmp1C3;({unsigned _tmp3A7=loc;Cyc_Warn_err2(_tmp3A7,_tag_fat(_tmp1C2,sizeof(void*),1U));});});else{
# 818
struct Cyc_Absyn_Exp*_tmp1C5=({void*_tmp3A8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp3A8,b);});struct Cyc_Absyn_Exp*e=_tmp1C5;
if(e == 0 && c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5;_tmp2E5.tag=0U,({struct _fat_ptr _tmp3A9=({const char*_tmp1C8="format descriptor is not a char * type";_tag_fat(_tmp1C8,sizeof(char),39U);});_tmp2E5.f1=_tmp3A9;});_tmp2E5;});void*_tmp1C6[1U];_tmp1C6[0]=& _tmp1C7;({unsigned _tmp3AA=loc;Cyc_Warn_err2(_tmp3AA,_tag_fat(_tmp1C6,sizeof(void*),1U));});});else{
if(e != 0 && !c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=0U,({struct _fat_ptr _tmp3AB=({const char*_tmp1CB="format descriptor is not a char ? type";_tag_fat(_tmp1CB,sizeof(char),39U);});_tmp2E6.f1=_tmp3AB;});_tmp2E6;});void*_tmp1C9[1U];_tmp1C9[0]=& _tmp1CA;({unsigned _tmp3AC=loc;Cyc_Warn_err2(_tmp3AC,_tag_fat(_tmp1C9,sizeof(void*),1U));});});}}
# 824
goto _LL5C;}}else{_LL5F: _LL60:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=0U,({struct _fat_ptr _tmp3AD=({const char*_tmp1CE="format descriptor is not a string type";_tag_fat(_tmp1CE,sizeof(char),39U);});_tmp2E7.f1=_tmp3AD;});_tmp2E7;});void*_tmp1CC[1U];_tmp1CC[0]=& _tmp1CD;({unsigned _tmp3AE=loc;Cyc_Warn_err2(_tmp3AE,_tag_fat(_tmp1CC,sizeof(void*),1U));});});goto _LL5C;}_LL5C:;}
# 827
if(fmt_arg_start != 0 && !c_vararg){
# 831
int problem;
{struct _tuple16 _tmp1CF=({struct _tuple16 _tmp2E8;_tmp2E8.f1=ft,({void*_tmp3AF=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_vararg))->type));_tmp2E8.f2=_tmp3AF;});_tmp2E8;});struct _tuple16 _stmttmp1C=_tmp1CF;struct _tuple16 _tmp1D0=_stmttmp1C;struct Cyc_Absyn_Datatypedecl*_tmp1D1;struct Cyc_Absyn_Datatypedecl*_tmp1D2;switch(_tmp1D0.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->f1)->f1).KnownDatatype).tag == 2){_LL62: _tmp1D2=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->f1)->f1).KnownDatatype).val;_LL63: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1D2;
# 834
problem=({struct _tuple1*_tmp3B0=tud->name;Cyc_Absyn_qvar_cmp(_tmp3B0,Cyc_Absyn_datatype_print_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->f1)->f1).KnownDatatype).tag == 2){_LL64: _tmp1D1=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0.f2)->f1)->f1).KnownDatatype).val;_LL65: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1D1;
# 837
problem=({struct _tuple1*_tmp3B1=tud->name;Cyc_Absyn_qvar_cmp(_tmp3B1,Cyc_Absyn_datatype_scanf_arg_qvar());})!= 0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}default: _LL66: _LL67:
# 840
 problem=1;
goto _LL61;}_LL61:;}
# 843
if(problem)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=0U,({struct _fat_ptr _tmp3B2=({const char*_tmp1D5="format attribute and vararg types don't match";_tag_fat(_tmp1D5,sizeof(char),46U);});_tmp2E9.f1=_tmp3B2;});_tmp2E9;});void*_tmp1D3[1U];_tmp1D3[0]=& _tmp1D4;({unsigned _tmp3B3=loc;Cyc_Warn_err2(_tmp3B3,_tag_fat(_tmp1D3,sizeof(void*),1U));});});}}}}
# 851
{struct Cyc_List_List*rpo=rgn_po;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp1D6=(struct _tuple0*)rpo->hd;struct _tuple0*_stmttmp1D=_tmp1D6;struct _tuple0*_tmp1D7=_stmttmp1D;void*_tmp1D9;void*_tmp1D8;_LL69: _tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D7->f2;_LL6A: {void*r1=_tmp1D8;void*r2=_tmp1D9;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_ek,r1,1,1);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_trk,r2,1,1);}}}{
# 859
struct Cyc_Tcenv_Fenv*_tmp1DA=Cyc_Tcenv_bogus_fenv(tr,args);struct Cyc_Tcenv_Fenv*fenv=_tmp1DA;
struct Cyc_Tcenv_Tenv*_tmp1DB=new_env.te;struct Cyc_Tcenv_Tenv*old_te=_tmp1DB;
({struct Cyc_Tcenv_Tenv*_tmp3B4=({struct Cyc_Tcenv_Tenv*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->ns=te->ns,_tmp1DC->ae=te->ae,_tmp1DC->le=fenv,_tmp1DC->allow_valueof=1,_tmp1DC->in_extern_c_include=te->in_extern_c_include,_tmp1DC->in_tempest=te->in_tempest,_tmp1DC->tempest_generalize=te->tempest_generalize,_tmp1DC->in_extern_c_inc_repeat=te->in_extern_c_inc_repeat;_tmp1DC;});new_env.te=_tmp3B4;});{
# 865
struct _tuple15 _tmp1DD=({struct Cyc_Tctyp_CVTEnv _tmp3B6=new_env;struct _fat_ptr _tmp3B5=({const char*_tmp216="@requires";_tag_fat(_tmp216,sizeof(char),10U);});Cyc_Tctyp_check_clause(_tmp3B6,_tmp3B5,req);});struct _tuple15 _stmttmp1E=_tmp1DD;struct _tuple15 _tmp1DE=_stmttmp1E;struct Cyc_List_List*_tmp1E0;struct Cyc_Tctyp_CVTEnv _tmp1DF;_LL6C: _tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;_LL6D: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1DF;struct Cyc_List_List*req_rs=_tmp1E0;
new_env=nenv;
*req_relns=req_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,req_rs);{
struct _tuple15 _tmp1E1=({struct Cyc_Tctyp_CVTEnv _tmp3B8=new_env;struct _fat_ptr _tmp3B7=({const char*_tmp215="@ensures";_tag_fat(_tmp215,sizeof(char),9U);});Cyc_Tctyp_check_clause(_tmp3B8,_tmp3B7,ens);});struct _tuple15 _stmttmp1F=_tmp1E1;struct _tuple15 _tmp1E2=_stmttmp1F;struct Cyc_List_List*_tmp1E4;struct Cyc_Tctyp_CVTEnv _tmp1E3;_LL6F: _tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;_LL70: {struct Cyc_Tctyp_CVTEnv nenv=_tmp1E3;struct Cyc_List_List*ens_rs=_tmp1E4;
new_env=nenv;
*ens_relns=ens_rs;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,ens_rs);
new_env.te=old_te;
# 875
if(*eff != 0)
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Tcutil_ek,(void*)_check_null(*eff),1,1);else{
# 878
struct Cyc_List_List*effect=0;
# 883
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp1E5=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp20=_tmp1E5;struct _tuple14 _tmp1E6=_stmttmp20;int _tmp1E8;struct Cyc_Absyn_Tvar*_tmp1E7;_LL72: _tmp1E7=_tmp1E6.f1;_tmp1E8=_tmp1E6.f2;_LL73: {struct Cyc_Absyn_Tvar*tv=_tmp1E7;int put_in_eff=_tmp1E8;
if(!put_in_eff)continue;{
void*_tmp1E9=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp21=_tmp1E9;void*_tmp1EA=_stmttmp21;struct Cyc_Core_Opt**_tmp1EB;struct Cyc_Absyn_Kind*_tmp1EC;struct Cyc_Core_Opt**_tmp1ED;struct Cyc_Absyn_Kind*_tmp1EF;struct Cyc_Core_Opt**_tmp1EE;switch(*((int*)_tmp1EA)){case 2U: _LL75: _tmp1EE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1EA)->f1;_tmp1EF=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1EA)->f2;if((int)_tmp1EF->kind == (int)Cyc_Absyn_RgnKind){_LL76: {struct Cyc_Core_Opt**f=_tmp1EE;struct Cyc_Absyn_Kind*r=_tmp1EF;
# 889
if((int)r->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmp3B9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp3B9;});_tmp1EC=r;goto _LL78;}
# 892
({struct Cyc_Core_Opt*_tmp3BA=Cyc_Tcutil_kind_to_bound_opt(r);*f=_tmp3BA;});_tmp1EC=r;goto _LL78;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1EA)->f2)->kind){case Cyc_Absyn_BoolKind: _LL79: _LL7A:
# 895
 goto _LL7C;case Cyc_Absyn_PtrBndKind: _LL7B: _LL7C:
 goto _LL7E;case Cyc_Absyn_IntKind: _LL7D: _LL7E:
 goto _LL80;case Cyc_Absyn_EffKind: _LL85: _tmp1ED=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1EA)->f1;_LL86: {struct Cyc_Core_Opt**f=_tmp1ED;
# 902
({struct Cyc_Core_Opt*_tmp3BB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*f=_tmp3BB;});goto _LL88;}default: goto _LL8B;}}case 0U: _LL77: _tmp1EC=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1EA)->f1;if((int)_tmp1EC->kind == (int)Cyc_Absyn_RgnKind){_LL78: {struct Cyc_Absyn_Kind*r=_tmp1EC;
# 894
effect=({struct Cyc_List_List*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));({void*_tmp3BC=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(tv));_tmp1F0->hd=_tmp3BC;}),_tmp1F0->tl=effect;_tmp1F0;});goto _LL74;}}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1EA)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7F: _LL80:
# 898
 goto _LL82;case Cyc_Absyn_PtrBndKind: _LL81: _LL82:
 goto _LL84;case Cyc_Absyn_IntKind: _LL83: _LL84:
 goto _LL74;case Cyc_Absyn_EffKind: _LL87: _LL88:
# 904
 effect=({struct Cyc_List_List*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));({void*_tmp3BD=Cyc_Absyn_var_type(tv);_tmp1F1->hd=_tmp3BD;}),_tmp1F1->tl=effect;_tmp1F1;});goto _LL74;default: _LL8B: _LL8C:
# 909
 effect=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));({void*_tmp3BE=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(tv));_tmp1F4->hd=_tmp3BE;}),_tmp1F4->tl=effect;_tmp1F4;});goto _LL74;}}default: _LL89: _tmp1EB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1EA)->f1;_LL8A: {struct Cyc_Core_Opt**f=_tmp1EB;
# 906
({struct Cyc_Core_Opt*_tmp3C0=({struct Cyc_Core_Opt*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));({void*_tmp3BF=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->tag=2U,_tmp1F2->f1=0,_tmp1F2->f2=& Cyc_Tcutil_ak;_tmp1F2;});_tmp1F3->v=_tmp3BF;});_tmp1F3;});*f=_tmp3C0;});goto _LL8C;}}_LL74:;}}}}
# 913
{struct Cyc_List_List*ts=new_env.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple13 _tmp1F5=*((struct _tuple13*)ts->hd);struct _tuple13 _stmttmp22=_tmp1F5;struct _tuple13 _tmp1F6=_stmttmp22;int _tmp1F8;void*_tmp1F7;_LL8E: _tmp1F7=_tmp1F6.f1;_tmp1F8=_tmp1F6.f2;_LL8F: {void*tv=_tmp1F7;int put_in_eff=_tmp1F8;
if(!put_in_eff)continue;{
struct Cyc_Absyn_Kind*_tmp1F9=Cyc_Tcutil_type_kind(tv);struct Cyc_Absyn_Kind*_stmttmp23=_tmp1F9;struct Cyc_Absyn_Kind*_tmp1FA=_stmttmp23;switch(((struct Cyc_Absyn_Kind*)_tmp1FA)->kind){case Cyc_Absyn_RgnKind: _LL91: _LL92:
 effect=({struct Cyc_List_List*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));({void*_tmp3C1=Cyc_Absyn_access_eff(tv);_tmp1FB->hd=_tmp3C1;}),_tmp1FB->tl=effect;_tmp1FB;});goto _LL90;case Cyc_Absyn_EffKind: _LL93: _LL94:
 effect=({struct Cyc_List_List*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->hd=tv,_tmp1FC->tl=effect;_tmp1FC;});goto _LL90;case Cyc_Absyn_IntKind: _LL95: _LL96:
 goto _LL90;default: _LL97: _LL98:
 effect=({struct Cyc_List_List*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));({void*_tmp3C2=Cyc_Absyn_regionsof_eff(tv);_tmp1FD->hd=_tmp3C2;}),_tmp1FD->tl=effect;_tmp1FD;});goto _LL90;}_LL90:;}}}}
# 923
({void*_tmp3C3=Cyc_Absyn_join_eff(effect);*eff=_tmp3C3;});}
# 929
if(*btvs != 0){
struct Cyc_List_List*bs=*btvs;for(0;bs != 0;bs=bs->tl){
void*_tmp1FE=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_stmttmp24=_tmp1FE;void*_tmp1FF=_stmttmp24;struct Cyc_Absyn_Kind*_tmp201;struct Cyc_Core_Opt**_tmp200;struct Cyc_Core_Opt**_tmp202;switch(*((int*)_tmp1FF)){case 1U: _LL9A: _tmp202=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1FF)->f1;_LL9B: {struct Cyc_Core_Opt**f=_tmp202;
# 933
({struct Cyc_Warn_String_Warn_Warg_struct _tmp204=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC;_tmp2EC.tag=0U,({struct _fat_ptr _tmp3C4=({const char*_tmp208="type variable ";_tag_fat(_tmp208,sizeof(char),15U);});_tmp2EC.f1=_tmp3C4;});_tmp2EC;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp205=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=7U,_tmp2EB.f1=(struct Cyc_Absyn_Tvar*)bs->hd;_tmp2EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp206=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=0U,({struct _fat_ptr _tmp3C5=({const char*_tmp207=" unconstrained, assuming boxed";_tag_fat(_tmp207,sizeof(char),31U);});_tmp2EA.f1=_tmp3C5;});_tmp2EA;});void*_tmp203[3U];_tmp203[0]=& _tmp204,_tmp203[1]=& _tmp205,_tmp203[2]=& _tmp206;({unsigned _tmp3C6=loc;Cyc_Warn_warn2(_tmp3C6,_tag_fat(_tmp203,sizeof(void*),3U));});});
({struct Cyc_Core_Opt*_tmp3C7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3C7;});
goto _LL99;}case 2U: _LL9C: _tmp200=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1FF)->f1;_tmp201=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1FF)->f2;_LL9D: {struct Cyc_Core_Opt**f=_tmp200;struct Cyc_Absyn_Kind*k=_tmp201;
# 937
{struct Cyc_Absyn_Kind*_tmp209=k;switch(((struct Cyc_Absyn_Kind*)_tmp209)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp209)->aliasqual == Cyc_Absyn_Top){_LLA3: _LLA4:
 goto _LLA6;}else{goto _LLB3;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)_tmp209)->aliasqual){case Cyc_Absyn_Top: _LLA5: _LLA6:
 goto _LLA8;case Cyc_Absyn_Aliasable: _LLA7: _LLA8:
 goto _LLAA;case Cyc_Absyn_Unique: _LLAD: _LLAE:
# 943
 goto _LLB0;default: goto _LLB3;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)_tmp209)->aliasqual){case Cyc_Absyn_Top: _LLA9: _LLAA:
# 941
 goto _LLAC;case Cyc_Absyn_Aliasable: _LLAB: _LLAC:
({struct Cyc_Core_Opt*_tmp3C8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3C8;});goto _LLA2;case Cyc_Absyn_Unique: _LLAF: _LLB0:
# 944
({struct Cyc_Core_Opt*_tmp3C9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp3C9;});goto _LLA2;default: goto _LLB3;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp209)->aliasqual == Cyc_Absyn_Top){_LLB1: _LLB2:
({struct Cyc_Core_Opt*_tmp3CA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp3CA;});goto _LLA2;}else{goto _LLB3;}default: _LLB3: _LLB4:
({struct Cyc_Core_Opt*_tmp3CB=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp3CB;});goto _LLA2;}_LLA2:;}
# 948
goto _LL99;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1FF)->f1)->kind == Cyc_Absyn_MemKind){_LL9E: _LL9F:
# 950
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=0U,({struct _fat_ptr _tmp3CC=({const char*_tmp20C="functions can't abstract M types";_tag_fat(_tmp20C,sizeof(char),33U);});_tmp2ED.f1=_tmp3CC;});_tmp2ED;});void*_tmp20A[1U];_tmp20A[0]=& _tmp20B;({unsigned _tmp3CD=loc;Cyc_Warn_err2(_tmp3CD,_tag_fat(_tmp20A,sizeof(void*),1U));});});goto _LL99;}else{_LLA0: _LLA1:
 goto _LL99;}}_LL99:;}}
# 955
({struct Cyc_List_List*_tmp3CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(struct Cyc_Absyn_Tvar*)))Cyc_Tctyp_remove_bound_tvars)(new_env.kind_env,*btvs,(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))Cyc_Core_identity);env.kind_env=_tmp3CE;});
({struct Cyc_List_List*_tmp3CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars)(new_env.free_vars,*btvs,(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst);new_env.free_vars=_tmp3CF;});
# 958
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple14 _tmp20D=*((struct _tuple14*)tvs->hd);struct _tuple14 _stmttmp25=_tmp20D;struct _tuple14 _tmp20E=_stmttmp25;int _tmp210;struct Cyc_Absyn_Tvar*_tmp20F;_LLB6: _tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;_LLB7: {struct Cyc_Absyn_Tvar*t=_tmp20F;int b=_tmp210;
({struct Cyc_List_List*_tmp3D0=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,t,b);env.free_vars=_tmp3D0;});}}}
# 963
{struct Cyc_List_List*evs=new_env.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp211=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp26=_tmp211;struct _tuple13 _tmp212=_stmttmp26;int _tmp214;void*_tmp213;_LLB9: _tmp213=_tmp212.f1;_tmp214=_tmp212.f2;_LLBA: {void*e=_tmp213;int b=_tmp214;
({struct Cyc_List_List*_tmp3D1=Cyc_Tctyp_add_free_evar(env.free_evars,e,b);env.free_evars=_tmp3D1;});}}}
# 967
goto _LL3;}}}}}}}case 6U: _LL16: _tmp126=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11F)->f1;_LL17: {struct Cyc_List_List*tq_ts=_tmp126;
# 970
for(0;tq_ts != 0;tq_ts=tq_ts->tl){
struct _tuple17*_tmp217=(struct _tuple17*)tq_ts->hd;struct _tuple17*p=_tmp217;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tmk,(*p).f2,1,0);
({int _tmp3D2=
Cyc_Tcutil_extract_const_from_typedef(loc,((*p).f1).print_const,(*p).f2);
# 973
((*p).f1).real_const=_tmp3D2;});}
# 976
goto _LL3;}case 7U: _LL18: _tmp124=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11F)->f1;_tmp125=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11F)->f2;_LL19: {enum Cyc_Absyn_AggrKind k=_tmp124;struct Cyc_List_List*fs=_tmp125;
# 980
struct Cyc_List_List*prev_fields=0;
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp218=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp27=_tmp218;struct Cyc_Absyn_Aggrfield*_tmp219=_stmttmp27;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_List_List*_tmp21E;struct Cyc_Absyn_Exp*_tmp21D;void*_tmp21C;struct Cyc_Absyn_Tqual*_tmp21B;struct _fat_ptr*_tmp21A;_LLBC: _tmp21A=_tmp219->name;_tmp21B=(struct Cyc_Absyn_Tqual*)& _tmp219->tq;_tmp21C=_tmp219->type;_tmp21D=_tmp219->width;_tmp21E=_tmp219->attributes;_tmp21F=_tmp219->requires_clause;_LLBD: {struct _fat_ptr*fn=_tmp21A;struct Cyc_Absyn_Tqual*tqp=_tmp21B;void*t=_tmp21C;struct Cyc_Absyn_Exp*width=_tmp21D;struct Cyc_List_List*atts=_tmp21E;struct Cyc_Absyn_Exp*requires_clause=_tmp21F;
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp221=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=0U,({struct _fat_ptr _tmp3D3=({const char*_tmp223="duplicate field ";_tag_fat(_tmp223,sizeof(char),17U);});_tmp2F0.f1=_tmp3D3;});_tmp2F0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp222=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0U,_tmp2EF.f1=*fn;_tmp2EF;});void*_tmp220[2U];_tmp220[0]=& _tmp221,_tmp220[1]=& _tmp222;({unsigned _tmp3D4=loc;Cyc_Warn_err2(_tmp3D4,_tag_fat(_tmp220,sizeof(void*),2U));});});
if(({struct _fat_ptr _tmp3D5=(struct _fat_ptr)*fn;Cyc_strcmp(_tmp3D5,({const char*_tmp224="";_tag_fat(_tmp224,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->hd=fn,_tmp225->tl=prev_fields;_tmp225;});
env=Cyc_Tctyp_i_check_type(env,& Cyc_Tcutil_tmk,t,1,0);
({int _tmp3D6=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t);tqp->real_const=_tmp3D6;});
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Atts_check_field_atts(loc,fn,atts);
if(requires_clause != 0){
# 993
if((int)k != (int)1U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp227=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=0U,({struct _fat_ptr _tmp3D7=({const char*_tmp228="@requires clause is only allowed on union members";_tag_fat(_tmp228,sizeof(char),50U);});_tmp2F1.f1=_tmp3D7;});_tmp2F1;});void*_tmp226[1U];_tmp226[0]=& _tmp227;({unsigned _tmp3D8=loc;Cyc_Warn_err2(_tmp3D8,_tag_fat(_tmp226,sizeof(void*),1U));});});
({struct Cyc_Tcenv_Tenv*_tmp3D9=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmp3D9,0,requires_clause);});
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp22A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F4;_tmp2F4.tag=0U,({struct _fat_ptr _tmp3DA=({const char*_tmp22E="@requires clause has type ";_tag_fat(_tmp22E,sizeof(char),27U);});_tmp2F4.f1=_tmp3DA;});_tmp2F4;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp22B=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp2F3;_tmp2F3.tag=3U,_tmp2F3.f1=(void*)requires_clause->topt;_tmp2F3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2;_tmp2F2.tag=0U,({
struct _fat_ptr _tmp3DB=({const char*_tmp22D=" instead of integral type";_tag_fat(_tmp22D,sizeof(char),26U);});_tmp2F2.f1=_tmp3DB;});_tmp2F2;});void*_tmp229[3U];_tmp229[0]=& _tmp22A,_tmp229[1]=& _tmp22B,_tmp229[2]=& _tmp22C;({unsigned _tmp3DC=loc;Cyc_Warn_err2(_tmp3DC,_tag_fat(_tmp229,sizeof(void*),3U));});});
env=Cyc_Tctyp_i_check_type_level_exp(requires_clause,env);}}}
# 1002
goto _LL3;}default: _LL1A: _tmp120=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11F)->f1;_tmp121=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11F)->f2;_tmp122=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11F)->f3;_tmp123=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11F)->f4;_LL1B: {struct _tuple1*tdn=_tmp120;struct Cyc_List_List**targs_ref=_tmp121;struct Cyc_Absyn_Typedefdecl**tdopt=_tmp122;void**toptp=_tmp123;
# 1006
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp22F;_push_handler(& _tmp22F);{int _tmp231=0;if(setjmp(_tmp22F.handler))_tmp231=1;if(!_tmp231){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);;_pop_handler();}else{void*_tmp230=(void*)Cyc_Core_get_exn_thrown();void*_tmp232=_tmp230;void*_tmp233;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp232)->tag == Cyc_Dict_Absent){_LLBF: _LLC0:
# 1009
({struct Cyc_Warn_String_Warn_Warg_struct _tmp235=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6;_tmp2F6.tag=0U,({struct _fat_ptr _tmp3DD=({const char*_tmp237="unbound typedef name ";_tag_fat(_tmp237,sizeof(char),22U);});_tmp2F6.f1=_tmp3DD;});_tmp2F6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp236=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F5;_tmp2F5.tag=1U,_tmp2F5.f1=tdn;_tmp2F5;});void*_tmp234[2U];_tmp234[0]=& _tmp235,_tmp234[1]=& _tmp236;({unsigned _tmp3DE=loc;Cyc_Warn_err2(_tmp3DE,_tag_fat(_tmp234,sizeof(void*),2U));});});
return env;}else{_LLC1: _tmp233=_tmp232;_LLC2: {void*exn=_tmp233;(int)_rethrow(exn);}}_LLBE:;}}}
# 1012
*tdopt=td;
env=Cyc_Tctyp_check_type_inst(env,targs_ref,td->tvs,expected_kind,allow_abs_aggr,tdn,td);
# 1015
if(td->defn != 0){
struct Cyc_List_List*inst=0;
struct Cyc_List_List*_tmp238=td->tvs;struct Cyc_List_List*tvs=_tmp238;
struct Cyc_List_List*_tmp239=*targs_ref;struct Cyc_List_List*ts=_tmp239;
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
inst=({struct Cyc_List_List*_tmp23B=_cycalloc(sizeof(*_tmp23B));({struct _tuple18*_tmp3DF=({struct _tuple18*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp23A->f2=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;_tmp23A;});_tmp23B->hd=_tmp3DF;}),_tmp23B->tl=inst;_tmp23B;});}
({void*_tmp3E0=Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));*toptp=_tmp3E0;});}
# 1023
goto _LL3;}}_LL3:;}
# 1025
if(!({struct Cyc_Absyn_Kind*_tmp3E1=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmp3E1,expected_kind);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FC;_tmp2FC.tag=0U,({struct _fat_ptr _tmp3E2=({const char*_tmp245="type ";_tag_fat(_tmp245,sizeof(char),6U);});_tmp2FC.f1=_tmp3E2;});_tmp2FC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp23E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FB;_tmp2FB.tag=2U,_tmp2FB.f1=(void*)t;_tmp2FB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp23F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FA;_tmp2FA.tag=0U,({struct _fat_ptr _tmp3E3=({const char*_tmp244=" has kind ";_tag_fat(_tmp244,sizeof(char),11U);});_tmp2FA.f1=_tmp3E3;});_tmp2FA;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp240=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp2F9;_tmp2F9.tag=9U,({struct Cyc_Absyn_Kind*_tmp3E4=Cyc_Tcutil_type_kind(t);_tmp2F9.f1=_tmp3E4;});_tmp2F9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp241=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F8;_tmp2F8.tag=0U,({
struct _fat_ptr _tmp3E5=({const char*_tmp243=" but as used here needs kind ";_tag_fat(_tmp243,sizeof(char),30U);});_tmp2F8.f1=_tmp3E5;});_tmp2F8;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp242=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp2F7;_tmp2F7.tag=9U,_tmp2F7.f1=expected_kind;_tmp2F7;});void*_tmp23C[6U];_tmp23C[0]=& _tmp23D,_tmp23C[1]=& _tmp23E,_tmp23C[2]=& _tmp23F,_tmp23C[3]=& _tmp240,_tmp23C[4]=& _tmp241,_tmp23C[5]=& _tmp242;({unsigned _tmp3E6=loc;Cyc_Warn_err2(_tmp3E6,_tag_fat(_tmp23C,sizeof(void*),6U));});});
return env;}}
# 1031
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1033
struct Cyc_List_List*_tmp246=cvt.kind_env;struct Cyc_List_List*kind_env=_tmp246;
cvt=Cyc_Tctyp_i_check_type(cvt,expected_kind,t,1,allow_abs_aggr);
# 1036
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
({struct Cyc_List_List*_tmp3E7=Cyc_Tctyp_fast_add_free_tvar(kind_env,(*((struct _tuple14*)vs->hd)).f1);cvt.kind_env=_tmp3E7;});}}
# 1042
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple13 _tmp247=*((struct _tuple13*)evs->hd);struct _tuple13 _stmttmp28=_tmp247;struct _tuple13 _tmp248=_stmttmp28;void*_tmp249;_LL1: _tmp249=_tmp248.f1;_LL2: {void*e=_tmp249;
void*_tmp24A=Cyc_Absyn_compress(e);void*_stmttmp29=_tmp24A;void*_tmp24B=_stmttmp29;struct Cyc_Core_Opt**_tmp24C;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24B)->tag == 1U){_LL4: _tmp24C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24B)->f4;_LL5: {struct Cyc_Core_Opt**s=_tmp24C;
# 1046
if(*s == 0)
({struct Cyc_Core_Opt*_tmp3E8=({struct Cyc_Core_Opt*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->v=kind_env;_tmp24D;});*s=_tmp3E8;});else{
# 1050
struct Cyc_List_List*_tmp24E=0;struct Cyc_List_List*result=_tmp24E;
{struct Cyc_List_List*_tmp24F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*s))->v;struct Cyc_List_List*tvs=_tmp24F;for(0;tvs != 0;tvs=tvs->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)tvs->hd))
result=({struct Cyc_List_List*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp250->tl=result;_tmp250;});}}
({struct Cyc_Core_Opt*_tmp3E9=({struct Cyc_Core_Opt*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251->v=result;_tmp251;});*s=_tmp3E9;});}
# 1056
goto _LL3;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}}}
# 1060
return cvt;}
# 1067
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=
Cyc_Tcutil_is_function_type(t)&&(!te->in_tempest || te->tempest_generalize);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_List_List*_tmp252=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*te_vars=_tmp252;
struct Cyc_Tctyp_CVTEnv _tmp253=({struct Cyc_Tctyp_CVTEnv _tmp3EB=({struct Cyc_Tctyp_CVTEnv _tmp305;_tmp305.loc=loc,_tmp305.te=te,_tmp305.kind_env=te_vars,_tmp305.fn_result=0,_tmp305.generalize_evars=generalize_evars,_tmp305.free_vars=0,_tmp305.free_evars=0;_tmp305;});struct Cyc_Absyn_Kind*_tmp3EA=expected_kind;Cyc_Tctyp_check_valid_type(_tmp3EB,_tmp3EA,1,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp253;
# 1075
struct Cyc_List_List*_tmp254=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst,cvt.free_vars);struct Cyc_List_List*free_tvars=_tmp254;
struct Cyc_List_List*_tmp255=cvt.free_evars;struct Cyc_List_List*free_evars=_tmp255;
# 1078
if(te_vars != 0){
struct Cyc_List_List*_tmp256=0;struct Cyc_List_List*res=_tmp256;
{struct Cyc_List_List*_tmp257=free_tvars;struct Cyc_List_List*fs=_tmp257;for(0;(unsigned)fs;fs=fs->tl){
struct Cyc_List_List*_tmp258=te_vars;struct Cyc_List_List*ts=_tmp258;
for(0;ts != 0;ts=ts->tl){
if(Cyc_Absyn_tvar_cmp((struct Cyc_Absyn_Tvar*)fs->hd,(struct Cyc_Absyn_Tvar*)ts->hd)== 0)
break;}
if(ts == 0)
res=({struct Cyc_List_List*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->hd=(struct Cyc_Absyn_Tvar*)fs->hd,_tmp259->tl=res;_tmp259;});}}
# 1088
free_tvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);}
# 1093
{struct Cyc_List_List*x=free_tvars;for(0;x != 0;x=x->tl){
void*_tmp25A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_stmttmp2A=_tmp25A;void*_tmp25B=_stmttmp2A;enum Cyc_Absyn_AliasQual _tmp25C;struct Cyc_Absyn_Kind*_tmp25E;struct Cyc_Core_Opt**_tmp25D;struct Cyc_Core_Opt**_tmp25F;switch(*((int*)_tmp25B)){case 1U: _LL1: _tmp25F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp25B)->f1;_LL2: {struct Cyc_Core_Opt**f=_tmp25F;
({struct Cyc_Core_Opt*_tmp3EC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3EC;});goto _LL0;}case 2U: _LL3: _tmp25D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25B)->f1;_tmp25E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp25B)->f2;_LL4: {struct Cyc_Core_Opt**f=_tmp25D;struct Cyc_Absyn_Kind*k=_tmp25E;
# 1097
{struct Cyc_Absyn_Kind*_tmp260=k;switch(((struct Cyc_Absyn_Kind*)_tmp260)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)_tmp260)->aliasqual == Cyc_Absyn_Top){_LLA: _LLB:
 goto _LLD;}else{goto _LL14;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)_tmp260)->aliasqual){case Cyc_Absyn_Top: _LLC: _LLD:
 goto _LLF;case Cyc_Absyn_Aliasable: _LLE: _LLF:
({struct Cyc_Core_Opt*_tmp3ED=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*f=_tmp3ED;});goto _LL9;case Cyc_Absyn_Unique: _LL10: _LL11:
({struct Cyc_Core_Opt*_tmp3EE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*f=_tmp3EE;});goto _LL9;default: goto _LL14;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)_tmp260)->aliasqual == Cyc_Absyn_Top){_LL12: _LL13:
({struct Cyc_Core_Opt*_tmp3EF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*f=_tmp3EF;});goto _LL9;}else{goto _LL14;}default: _LL14: _LL15:
({struct Cyc_Core_Opt*_tmp3F0=Cyc_Tcutil_kind_to_bound_opt(k);*f=_tmp3F0;});goto _LL9;}_LL9:;}
# 1105
goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25B)->f1)->kind == Cyc_Absyn_MemKind){_LL5: _tmp25C=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp25B)->f1)->aliasqual;_LL6: {enum Cyc_Absyn_AliasQual a=_tmp25C;
# 1107
({struct Cyc_Warn_String_Warn_Warg_struct _tmp262=({struct Cyc_Warn_String_Warn_Warg_struct _tmp300;_tmp300.tag=0U,({struct _fat_ptr _tmp3F1=({const char*_tmp268="type variable ";_tag_fat(_tmp268,sizeof(char),15U);});_tmp300.f1=_tmp3F1;});_tmp300;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp263=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp2FF;_tmp2FF.tag=7U,_tmp2FF.f1=(struct Cyc_Absyn_Tvar*)x->hd;_tmp2FF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp264=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FE;_tmp2FE.tag=0U,({struct _fat_ptr _tmp3F2=({const char*_tmp267=" cannot have kind ";_tag_fat(_tmp267,sizeof(char),19U);});_tmp2FE.f1=_tmp3F2;});_tmp2FE;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp265=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp2FD;_tmp2FD.tag=9U,({struct Cyc_Absyn_Kind*_tmp3F3=({struct Cyc_Absyn_Kind*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->kind=Cyc_Absyn_MemKind,_tmp266->aliasqual=a;_tmp266;});_tmp2FD.f1=_tmp3F3;});_tmp2FD;});void*_tmp261[4U];_tmp261[0]=& _tmp262,_tmp261[1]=& _tmp263,_tmp261[2]=& _tmp264,_tmp261[3]=& _tmp265;({unsigned _tmp3F4=loc;Cyc_Warn_err2(_tmp3F4,_tag_fat(_tmp261,sizeof(void*),4U));});});
goto _LL0;}}else{_LL7: _LL8:
 goto _LL0;}}_LL0:;}}
# 1113
if(free_tvars != 0 || free_evars != 0){
{void*_tmp269=Cyc_Absyn_compress(t);void*_stmttmp2B=_tmp269;void*_tmp26A=_stmttmp2B;struct Cyc_List_List**_tmp26B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26A)->tag == 5U){_LL17: _tmp26B=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26A)->f1).tvars;_LL18: {struct Cyc_List_List**btvs=_tmp26B;
# 1116
if(*btvs == 0){
# 1118
({struct Cyc_List_List*_tmp3F5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(free_tvars);*btvs=_tmp3F5;});
free_tvars=0;}
# 1121
goto _LL16;}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 1124
if(free_tvars != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp304;_tmp304.tag=0U,({struct _fat_ptr _tmp3F6=({const char*_tmp272="unbound type variable ";_tag_fat(_tmp272,sizeof(char),23U);});_tmp304.f1=_tmp3F6;});_tmp304;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp26E=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp303;_tmp303.tag=7U,_tmp303.f1=(struct Cyc_Absyn_Tvar*)free_tvars->hd;_tmp303;});struct Cyc_Warn_String_Warn_Warg_struct _tmp26F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp302;_tmp302.tag=0U,({struct _fat_ptr _tmp3F7=({const char*_tmp271=" when checking ";_tag_fat(_tmp271,sizeof(char),16U);});_tmp302.f1=_tmp3F7;});_tmp302;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp270=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp301;_tmp301.tag=2U,_tmp301.f1=(void*)t;_tmp301;});void*_tmp26C[4U];_tmp26C[0]=& _tmp26D,_tmp26C[1]=& _tmp26E,_tmp26C[2]=& _tmp26F,_tmp26C[3]=& _tmp270;({unsigned _tmp3F8=loc;Cyc_Warn_err2(_tmp3F8,_tag_fat(_tmp26C,sizeof(void*),4U));});});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}}
# 1135
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1138
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_tmp273=Cyc_Absyn_compress(t);void*_stmttmp2C=_tmp273;void*_tmp274=_stmttmp2C;struct Cyc_Absyn_FnInfo _tmp275;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp274)->tag == 5U){_LL1: _tmp275=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp274)->f1;_LL2: {struct Cyc_Absyn_FnInfo i=_tmp275;
# 1141
struct Cyc_List_List*_tmp276=(fd->i).attributes;struct Cyc_List_List*atts=_tmp276;
fd->i=i;
(fd->i).attributes=atts;
({int _tmp3F9=
Cyc_Tcutil_extract_const_from_typedef(loc,(i.ret_tqual).print_const,i.ret_type);
# 1144
((fd->i).ret_tqual).real_const=_tmp3F9;});
# 1146
goto _LL0;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp278=({struct Cyc_Warn_String_Warn_Warg_struct _tmp306;_tmp306.tag=0U,({struct _fat_ptr _tmp3FA=({const char*_tmp279="check_fndecl_valid_type: not a FnType";_tag_fat(_tmp279,sizeof(char),38U);});_tmp306.f1=_tmp3FA;});_tmp306;});void*_tmp277[1U];_tmp277[0]=& _tmp278;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp277,sizeof(void*),1U));});}_LL0:;}{
# 1149
struct Cyc_List_List*_tmp27A=0;struct Cyc_List_List*argnames=_tmp27A;
{struct Cyc_List_List*_tmp27B=(fd->i).args;struct Cyc_List_List*args=_tmp27B;for(0;args != 0;args=args->tl){
argnames=({struct Cyc_List_List*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->hd=(struct _fat_ptr*)_check_null((*((struct _tuple9*)args->hd)).f1),_tmp27C->tl=argnames;_tmp27C;});}}
({struct Cyc_List_List*_tmp3FC=argnames;unsigned _tmp3FB=loc;Cyc_Tcutil_check_unique_vars(_tmp3FC,_tmp3FB,({const char*_tmp27D="function declaration has repeated parameter";_tag_fat(_tmp27D,sizeof(char),44U);}));});
# 1154
fd->cached_type=t;}}
# 1159
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1162
struct Cyc_Tctyp_CVTEnv _tmp27E=({struct Cyc_Tctyp_CVTEnv _tmp3FF=({struct Cyc_Tctyp_CVTEnv _tmp30B;_tmp30B.loc=loc,_tmp30B.te=te,_tmp30B.kind_env=bound_tvars,_tmp30B.fn_result=0,_tmp30B.generalize_evars=0,_tmp30B.free_vars=0,_tmp30B.free_evars=0;_tmp30B;});struct Cyc_Absyn_Kind*_tmp3FE=expected_kind;int _tmp3FD=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_tmp3FF,_tmp3FE,_tmp3FD,t);});struct Cyc_Tctyp_CVTEnv cvt=_tmp27E;
# 1164
struct Cyc_List_List*_tmp27F=((struct Cyc_List_List*(*)(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars)(cvt.free_vars,bound_tvars,(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst);struct Cyc_List_List*free_tvs=_tmp27F;
for(0;free_tvs != 0;free_tvs=free_tvs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp281=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30A;_tmp30A.tag=0U,({struct _fat_ptr _tmp400=({const char*_tmp286="unbound type variable ";_tag_fat(_tmp286,sizeof(char),23U);});_tmp30A.f1=_tmp400;});_tmp30A;});struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp282=({struct Cyc_Warn_Tvar_Warn_Warg_struct _tmp309;_tmp309.tag=7U,_tmp309.f1=(*((struct _tuple14*)free_tvs->hd)).f1;_tmp309;});struct Cyc_Warn_String_Warn_Warg_struct _tmp283=({struct Cyc_Warn_String_Warn_Warg_struct _tmp308;_tmp308.tag=0U,({struct _fat_ptr _tmp401=({const char*_tmp285=" in type ";_tag_fat(_tmp285,sizeof(char),10U);});_tmp308.f1=_tmp401;});_tmp308;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp284=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp307;_tmp307.tag=2U,_tmp307.f1=(void*)t;_tmp307;});void*_tmp280[4U];_tmp280[0]=& _tmp281,_tmp280[1]=& _tmp282,_tmp280[2]=& _tmp283,_tmp280[3]=& _tmp284;({unsigned _tmp402=loc;Cyc_Warn_err2(_tmp402,_tag_fat(_tmp280,sizeof(void*),4U));});});}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(cvt.free_evars,t,loc);}

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
# 111
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Position_Error;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 693 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple12{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple12*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 884
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 917
extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_rgn_handle_type(void*);
# 936
void*Cyc_Absyn_exn_type();
# 960
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 972
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 977
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1049
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1062
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1071
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1076
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1080
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple2*,void*,struct Cyc_Absyn_Exp*init);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 68
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 27 "unify.h"
void Cyc_Unify_explain_failure();
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 36
int Cyc_Tcutil_is_void_type(void*);
# 38
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 107
void*Cyc_Tcutil_compress(void*);
# 113
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 147
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 174
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 202
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 206
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 241
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 246
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 258
int Cyc_Tcutil_new_tvar_id();
# 273
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 280
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 284
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 294
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 71 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 97
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 101
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 103
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 107
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 125
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple14{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 128
const struct _tuple14*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 141
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 152
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
# 35 "tctyp.h"
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr msg_part);
void Cyc_Tcexp_unique_consume_err(unsigned);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 107
void Cyc_Tcpat_check_switch_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 109
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 111
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 43 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp3;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3;
({void*_tmp174=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=5U,_tmp4->f1=e1,_tmp4->f2=Cyc_Absyn_PreInc;_tmp4;});e->r=_tmp174;});goto _LL0;}case Cyc_Absyn_PostDec: _LL3: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2;
({void*_tmp175=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=5U,_tmp5->f1=e1,_tmp5->f2=Cyc_Absyn_PreDec;_tmp5;});e->r=_tmp175;});goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 52
static int Cyc_Tcstmt_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp6=e->r;void*_stmttmp1=_tmp6;void*_tmp7=_stmttmp1;struct Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmp9;switch(*((int*)_tmp7)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL4: {struct Cyc_Absyn_Exp*e2=_tmp9;
_tmp8=e2;goto _LL6;}case 13U: _LL5: _tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL6: {struct Cyc_Absyn_Exp*e2=_tmp8;
e=e2;continue;}default: _LL7: _LL8:
# 59
 return 0;}_LL0:;}}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 69
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmpA=e->r;void*_stmttmp2=_tmpA;void*_tmpB=_stmttmp2;struct Cyc_List_List*_tmpC;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_Absyn_Exp*_tmp2E;switch(*((int*)_tmpB)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 78
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp2E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp30=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp2E;struct Cyc_Absyn_Exp*e2=_tmp2F;struct Cyc_Absyn_Exp*e3=_tmp30;
# 88
if(!Cyc_Tcstmt_noassign_exp(e1))return 0;
_tmp2C=e2;_tmp2D=e3;goto _LL20;}case 27U: _LL1F: _tmp2C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp2D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp2C;struct Cyc_Absyn_Exp*e2=_tmp2D;
_tmp2A=e1;_tmp2B=e2;goto _LL22;}case 7U: _LL21: _tmp2A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp2A;struct Cyc_Absyn_Exp*e2=_tmp2B;
_tmp28=e1;_tmp29=e2;goto _LL24;}case 8U: _LL23: _tmp28=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp29=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp28;struct Cyc_Absyn_Exp*e2=_tmp29;
_tmp26=e1;_tmp27=e2;goto _LL26;}case 23U: _LL25: _tmp26=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp27=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp26;struct Cyc_Absyn_Exp*e2=_tmp27;
_tmp24=e1;_tmp25=e2;goto _LL28;}case 9U: _LL27: _tmp24=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp25=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;
return Cyc_Tcstmt_noassign_exp(e1)&& Cyc_Tcstmt_noassign_exp(e2);}case 41U: _LL29: _tmp23=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp23;
# 96
_tmp22=e;goto _LL2C;}case 38U: _LL2B: _tmp22=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2C: {struct Cyc_Absyn_Exp*e=_tmp22;
_tmp21=e;goto _LL2E;}case 11U: _LL2D: _tmp21=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2E: {struct Cyc_Absyn_Exp*e=_tmp21;
_tmp20=e;goto _LL30;}case 12U: _LL2F: _tmp20=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL30: {struct Cyc_Absyn_Exp*e=_tmp20;
_tmp1F=e;goto _LL32;}case 13U: _LL31: _tmp1F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL32: {struct Cyc_Absyn_Exp*e=_tmp1F;
_tmp1E=e;goto _LL34;}case 14U: _LL33: _tmp1E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL34: {struct Cyc_Absyn_Exp*e=_tmp1E;
_tmp1D=e;goto _LL36;}case 18U: _LL35: _tmp1D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_tmp1D;
_tmp1C=e;goto _LL38;}case 20U: _LL37: _tmp1C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_tmp1C;
_tmp1B=e;goto _LL3A;}case 21U: _LL39: _tmp1B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3A: {struct Cyc_Absyn_Exp*e=_tmp1B;
_tmp1A=e;goto _LL3C;}case 22U: _LL3B: _tmp1A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_tmp1A;
_tmp19=e;goto _LL3E;}case 28U: _LL3D: _tmp19=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL3E: {struct Cyc_Absyn_Exp*e=_tmp19;
_tmp18=e;goto _LL40;}case 15U: _LL3F: _tmp18=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL40: {struct Cyc_Absyn_Exp*e=_tmp18;
return Cyc_Tcstmt_noassign_exp(e);}case 34U: _LL41: _tmp16=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB)->f1).rgn;_tmp17=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB)->f1).num_elts;_LL42: {struct Cyc_Absyn_Exp*eopt=_tmp16;struct Cyc_Absyn_Exp*e=_tmp17;
# 110
_tmp14=eopt;_tmp15=e;goto _LL44;}case 16U: _LL43: _tmp14=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp15=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp14;struct Cyc_Absyn_Exp*e=_tmp15;
# 112
if(!Cyc_Tcstmt_noassign_exp(e))return 0;
if(eopt != 0)return Cyc_Tcstmt_noassign_exp(eopt);
return 1;}case 3U: _LL45: _tmp13=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL46: {struct Cyc_List_List*es=_tmp13;
# 116
_tmp12=es;goto _LL48;}case 31U: _LL47: _tmp12=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL48: {struct Cyc_List_List*es=_tmp12;
_tmp11=es;goto _LL4A;}case 24U: _LL49: _tmp11=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL4A: {struct Cyc_List_List*es=_tmp11;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,es);}case 36U: _LL4B: _tmp10=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL4C: {struct Cyc_List_List*dles=_tmp10;
# 120
_tmpF=dles;goto _LL4E;}case 29U: _LL4D: _tmpF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL4E: {struct Cyc_List_List*dles=_tmpF;
_tmpE=dles;goto _LL50;}case 30U: _LL4F: _tmpE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL50: {struct Cyc_List_List*dles=_tmpE;
_tmpD=dles;goto _LL52;}case 26U: _LL51: _tmpD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL52: {struct Cyc_List_List*dles=_tmpD;
_tmpC=dles;goto _LL54;}default: _LL53: _tmpC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL54: {struct Cyc_List_List*dles=_tmpC;
# 125
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Tcstmt_noassign_exp((*((struct _tuple16*)dles->hd)).f2))
return 0;}
return 1;}}_LL0:;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple18{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 133
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 135
struct Cyc_Tcpat_TcPatResult _tmp31=pat_res;struct Cyc_List_List*_tmp33;struct _tuple1*_tmp32;_LL1: _tmp32=_tmp31.tvars_and_bounds_opt;_tmp33=_tmp31.patvars;_LL2: {struct _tuple1*p=_tmp32;struct Cyc_List_List*vse=_tmp33;
struct Cyc_List_List*_tmp34=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple18*))Cyc_Core_fst,vse);struct Cyc_List_List*vs=_tmp34;
struct Cyc_List_List*_tmp35=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple17*))Cyc_Core_fst,(*p).f1);struct Cyc_List_List*tvs=_tmp35;
struct Cyc_List_List*_tmp36=p == 0?0:(*p).f2;struct Cyc_List_List*bds=_tmp36;
struct Cyc_List_List*_tmp37=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple17*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple17*))Cyc_Core_snd,(*p).f1));struct Cyc_List_List*rgns=_tmp37;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(0;rgns != 0;rgns=rgns->tl){
te2=({struct Cyc_Tcenv_Tenv*_tmp176=te2;Cyc_Tcenv_add_region(_tmp176,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->tag=2U,_tmp38->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_tmp38;}),0);});}
te2=Cyc_Tcenv_new_outlives_constraints(te2,bds,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(loc,te2);{
void*_tmp39=Cyc_Tcenv_curr_rgn(te2);void*ropt=_tmp39;
{struct Cyc_List_List*_tmp3A=vs;struct Cyc_List_List*vs2=_tmp3A;for(0;vs2 != 0;vs2=vs2->tl){
if((struct Cyc_Absyn_Vardecl**)vs2->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)vs2->hd)))->rgn=ropt;}}
# 151
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp178=te2;struct Cyc_Absyn_Exp*_tmp177=where_opt;Cyc_Tcexp_tcTest(_tmp178,_tmp177,({const char*_tmp3B="switch clause guard";_tag_fat(_tmp3B,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp3C=0U;({unsigned _tmp17A=where_opt->loc;struct _fat_ptr _tmp179=({const char*_tmp3D="cannot show &&-clause in pattern has no effects";_tag_fat(_tmp3D,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp17A,_tmp179,_tag_fat(_tmp3C,sizeof(void*),0U));});});}
# 156
Cyc_Tcstmt_tcStmt(te2,s,0);}}}
# 159
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp3E=Cyc_Tcstmt_stmt_temp_var_counter ++;int i=_tmp3E;
struct _tuple2*res=({struct _tuple2*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp17D=({struct _fat_ptr*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _fat_ptr _tmp17C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp41=({struct Cyc_Int_pa_PrintArg_struct _tmp14D;_tmp14D.tag=1U,_tmp14D.f1=(unsigned)i;_tmp14D;});void*_tmp3F[1U];_tmp3F[0]=& _tmp41;({struct _fat_ptr _tmp17B=({const char*_tmp40="_stmttmp%X";_tag_fat(_tmp40,sizeof(char),11U);});Cyc_aprintf(_tmp17B,_tag_fat(_tmp3F,sizeof(void*),1U));});});*_tmp42=_tmp17C;});_tmp42;});_tmp43->f2=_tmp17D;});_tmp43;});
return res;}
# 173 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 175
void*_tmp44=s0->r;void*_stmttmp3=_tmp44;void*_tmp45=_stmttmp3;struct Cyc_Absyn_Stmt*_tmp47;struct Cyc_Absyn_Decl*_tmp46;void**_tmp4A;struct Cyc_List_List**_tmp49;struct Cyc_Absyn_Stmt*_tmp48;void**_tmp4D;struct Cyc_List_List*_tmp4C;struct Cyc_Absyn_Exp**_tmp4B;struct Cyc_Absyn_Stmt*_tmp4F;struct _fat_ptr*_tmp4E;struct Cyc_Absyn_Switch_clause***_tmp51;struct Cyc_List_List*_tmp50;struct Cyc_Absyn_Stmt*_tmp54;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Stmt*_tmp52;struct Cyc_Absyn_Stmt*_tmp5A;struct Cyc_Absyn_Stmt*_tmp59;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Stmt*_tmp57;struct Cyc_Absyn_Exp*_tmp56;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Stmt*_tmp5D;struct Cyc_Absyn_Stmt*_tmp5C;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Stmt*_tmp60;struct Cyc_Absyn_Stmt*_tmp5F;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_Absyn_Stmt*_tmp63;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Exp*_tmp64;switch(*((int*)_tmp45)){case 0U: _LL1: _LL2:
# 177
 return;case 1U: _LL3: _tmp64=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp64;
# 180
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2U: _LL5: _tmp62=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp63=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp62;struct Cyc_Absyn_Stmt*s2=_tmp63;
# 186
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3U: _LL7: _tmp61=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp61;
# 191
void*t=Cyc_Tcenv_return_typ(te);
if(eopt == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp66=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14F;_tmp14F.tag=0U,({struct _fat_ptr _tmp17E=({const char*_tmp68="should return a value of type ";_tag_fat(_tmp68,sizeof(char),31U);});_tmp14F.f1=_tmp17E;});_tmp14F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp67=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14E;_tmp14E.tag=2U,_tmp14E.f1=(void*)t;_tmp14E;});void*_tmp65[2U];_tmp65[0]=& _tmp66,_tmp65[1]=& _tmp67;({unsigned _tmp17F=s0->loc;Cyc_Warn_warn2(_tmp17F,_tag_fat(_tmp65,sizeof(void*),2U));});});else{
# 197
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp151;_tmp151.tag=0U,({struct _fat_ptr _tmp180=({const char*_tmp6C="must return a value of type ";_tag_fat(_tmp6C,sizeof(char),29U);});_tmp151.f1=_tmp180;});_tmp151;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp150;_tmp150.tag=2U,_tmp150.f1=(void*)t;_tmp150;});void*_tmp69[2U];_tmp69[0]=& _tmp6A,_tmp69[1]=& _tmp6B;({unsigned _tmp181=s0->loc;Cyc_Warn_err2(_tmp181,_tag_fat(_tmp69,sizeof(void*),2U));});});}}}else{
# 201
int bogus=0;
struct Cyc_Absyn_Exp*e=eopt;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp183=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp182=e;Cyc_Tcutil_coerce_arg(_tmp183,_tmp182,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp155;_tmp155.tag=0U,({struct _fat_ptr _tmp184=({const char*_tmp73="returns value of type ";_tag_fat(_tmp73,sizeof(char),23U);});_tmp155.f1=_tmp184;});_tmp155;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp154;_tmp154.tag=2U,_tmp154.f1=(void*)_check_null(e->topt);_tmp154;});struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp153;_tmp153.tag=0U,({
struct _fat_ptr _tmp185=({const char*_tmp72="but requires ";_tag_fat(_tmp72,sizeof(char),14U);});_tmp153.f1=_tmp185;});_tmp153;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp71=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp152;_tmp152.tag=2U,_tmp152.f1=(void*)t;_tmp152;});void*_tmp6D[4U];_tmp6D[0]=& _tmp6E,_tmp6D[1]=& _tmp6F,_tmp6D[2]=& _tmp70,_tmp6D[3]=& _tmp71;({unsigned _tmp186=s0->loc;Cyc_Warn_err2(_tmp186,_tag_fat(_tmp6D,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 209
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(e->loc);}
# 212
return;}case 4U: _LL9: _tmp5E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp5F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_tmp60=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp5E;struct Cyc_Absyn_Stmt*s1=_tmp5F;struct Cyc_Absyn_Stmt*s2=_tmp60;
# 215
({struct Cyc_Tcenv_Tenv*_tmp188=te;struct Cyc_Absyn_Exp*_tmp187=e;Cyc_Tcexp_tcTest(_tmp188,_tmp187,({const char*_tmp74="if statement";_tag_fat(_tmp74,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5U: _LLB: _tmp5B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f1).f1;_tmp5C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f1).f2;_tmp5D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5B;struct Cyc_Absyn_Stmt*cont_s=_tmp5C;struct Cyc_Absyn_Stmt*s=_tmp5D;
# 221
({struct Cyc_Tcenv_Tenv*_tmp18A=te;struct Cyc_Absyn_Exp*_tmp189=e;Cyc_Tcexp_tcTest(_tmp18A,_tmp189,({const char*_tmp75="while loop";_tag_fat(_tmp75,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9U: _LLD: _tmp55=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp56=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f1;_tmp57=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f2;_tmp58=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f3).f1;_tmp59=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f3).f2;_tmp5A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp55;struct Cyc_Absyn_Exp*e2=_tmp56;struct Cyc_Absyn_Stmt*guard_s=_tmp57;struct Cyc_Absyn_Exp*e3=_tmp58;struct Cyc_Absyn_Stmt*cont_s=_tmp59;struct Cyc_Absyn_Stmt*s=_tmp5A;
# 226
Cyc_Tcexp_tcExp(te,0,e1);
({struct Cyc_Tcenv_Tenv*_tmp18C=te;struct Cyc_Absyn_Exp*_tmp18B=e2;Cyc_Tcexp_tcTest(_tmp18C,_tmp18B,({const char*_tmp76="for loop";_tag_fat(_tmp76,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14U: _LLF: _tmp52=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp53=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f1;_tmp54=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f2;_LL10: {struct Cyc_Absyn_Stmt*s=_tmp52;struct Cyc_Absyn_Exp*e=_tmp53;struct Cyc_Absyn_Stmt*cont_s=_tmp54;
# 234
Cyc_Tcstmt_tcStmt(te,s,1);
({struct Cyc_Tcenv_Tenv*_tmp18E=te;struct Cyc_Absyn_Exp*_tmp18D=e;Cyc_Tcexp_tcTest(_tmp18E,_tmp18D,({const char*_tmp77="do loop";_tag_fat(_tmp77,sizeof(char),8U);}));});
return;}case 6U: _LL11: _LL12:
# 238
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp50=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp51=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL18: {struct Cyc_List_List*es=_tmp50;struct Cyc_Absyn_Switch_clause***clauseopt=_tmp51;
# 243
const struct _tuple14*_tmp78=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);const struct _tuple14*trip_opt=_tmp78;
if(trip_opt == (const struct _tuple14*)0){
({void*_tmp79=0U;({unsigned _tmp190=s0->loc;struct _fat_ptr _tmp18F=({const char*_tmp7A="fallthru not in a non-last case";_tag_fat(_tmp7A,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp190,_tmp18F,_tag_fat(_tmp79,sizeof(void*),0U));});});
return;}{
# 248
struct Cyc_List_List*_tmp7B=(*trip_opt).f2;struct Cyc_List_List*tvs=_tmp7B;
struct Cyc_List_List*_tmp7C=(*trip_opt).f3;struct Cyc_List_List*ts_orig=_tmp7C;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp191=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp191,tvs);});
struct Cyc_List_List*_tmp7D=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,ts_orig);struct Cyc_List_List*ts=_tmp7D;
for(0;ts != 0 && es != 0;(ts=ts->tl,es=es->tl)){
# 255
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp193=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp192=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp193,_tmp192,(void*)ts->hd,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp159;_tmp159.tag=0U,({struct _fat_ptr _tmp194=({const char*_tmp84="fallthru argument has type ";_tag_fat(_tmp84,sizeof(char),28U);});_tmp159.f1=_tmp194;});_tmp159;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp80=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp158;_tmp158.tag=2U,_tmp158.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp158;});struct Cyc_Warn_String_Warn_Warg_struct _tmp81=({struct Cyc_Warn_String_Warn_Warg_struct _tmp157;_tmp157.tag=0U,({
struct _fat_ptr _tmp195=({const char*_tmp83="but pattern variable has type ";_tag_fat(_tmp83,sizeof(char),31U);});_tmp157.f1=_tmp195;});_tmp157;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp82=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp156;_tmp156.tag=2U,_tmp156.f1=(void*)ts->hd;_tmp156;});void*_tmp7E[4U];_tmp7E[0]=& _tmp7F,_tmp7E[1]=& _tmp80,_tmp7E[2]=& _tmp81,_tmp7E[3]=& _tmp82;({unsigned _tmp196=s0->loc;Cyc_Warn_err2(_tmp196,_tag_fat(_tmp7E,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 263
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)ts->hd)&& !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)es->hd))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)es->hd)->loc);}
# 266
if(es != 0)
({void*_tmp85=0U;({unsigned _tmp198=s0->loc;struct _fat_ptr _tmp197=({const char*_tmp86="too many arguments in explicit fallthru";_tag_fat(_tmp86,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp198,_tmp197,_tag_fat(_tmp85,sizeof(void*),0U));});});
if(ts != 0)
({void*_tmp87=0U;({unsigned _tmp19A=s0->loc;struct _fat_ptr _tmp199=({const char*_tmp88="too few arguments in explicit fallthru";_tag_fat(_tmp88,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp19A,_tmp199,_tag_fat(_tmp87,sizeof(void*),0U));});});
return;}}case 13U: _LL19: _tmp4E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp4F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL1A: {struct _fat_ptr*l=_tmp4E;struct Cyc_Absyn_Stmt*s=_tmp4F;
# 276
({struct Cyc_Tcenv_Tenv*_tmp1A2=({unsigned _tmp1A1=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1A0=te;Cyc_Tcenv_new_named_block(_tmp1A1,_tmp1A0,({struct Cyc_Absyn_Tvar*_tmp8D=_cycalloc(sizeof(*_tmp8D));
({struct _fat_ptr*_tmp19F=({struct _fat_ptr*_tmp8C=_cycalloc(sizeof(*_tmp8C));({struct _fat_ptr _tmp19E=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0U,_tmp15A.f1=(struct _fat_ptr)((struct _fat_ptr)*l);_tmp15A;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _fat_ptr _tmp19D=({const char*_tmp8A="`%s";_tag_fat(_tmp8A,sizeof(char),4U);});Cyc_aprintf(_tmp19D,_tag_fat(_tmp89,sizeof(void*),1U));});});*_tmp8C=_tmp19E;});_tmp8C;});_tmp8D->name=_tmp19F;}),({
int _tmp19C=Cyc_Tcutil_new_tvar_id();_tmp8D->identity=_tmp19C;}),({
void*_tmp19B=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp8D->kind=_tmp19B;});_tmp8D;}));});
# 276
Cyc_Tcstmt_tcStmt(_tmp1A2,s,0);});
# 280
return;}case 10U: _LL1B: _tmp4B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp4C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_tmp4D=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f3;_LL1C: {struct Cyc_Absyn_Exp**exp=_tmp4B;struct Cyc_List_List*scs0=_tmp4C;void**dtp=_tmp4D;
# 285
struct Cyc_Absyn_Exp*_tmp8E=*exp;struct Cyc_Absyn_Exp*e=_tmp8E;
if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmp8F=({struct _tuple2*_tmp1A3=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1A3,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp8F;
struct Cyc_Absyn_Stmt*_tmp90=Cyc_Absyn_new_stmt(s0->r,s0->loc);struct Cyc_Absyn_Stmt*s1=_tmp90;
struct Cyc_Absyn_Decl*_tmp91=({struct Cyc_Absyn_Pat*_tmp1A7=({void*_tmp1A5=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=1U,_tmp94->f1=vd,({struct Cyc_Absyn_Pat*_tmp1A4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp94->f2=_tmp1A4;});_tmp94;});Cyc_Absyn_new_pat(_tmp1A5,e->loc);});struct Cyc_Absyn_Exp*_tmp1A6=e;Cyc_Absyn_let_decl(_tmp1A7,_tmp1A6,s0->loc);});struct Cyc_Absyn_Decl*d=_tmp91;
# 292
struct Cyc_Absyn_Stmt*_tmp92=Cyc_Absyn_decl_stmt(d,s1,s0->loc);struct Cyc_Absyn_Stmt*s2=_tmp92;
s0->r=s2->r;
({struct Cyc_Absyn_Exp*_tmp1A9=({void*_tmp1A8=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=4U,_tmp93->f1=vd;_tmp93;});Cyc_Absyn_varb_exp(_tmp1A8,e->loc);});*exp=_tmp1A9;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 299
void*_tmp95=Cyc_Tcexp_tcExp(te,0,e);void*t=_tmp95;
# 301
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(e->loc);{
# 306
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp96=scs0;struct Cyc_List_List*scs=_tmp96;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 314
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 316
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp97=_cycalloc(sizeof(*_tmp97));({struct Cyc_Tcpat_TcPatResult _tmp1AA=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& t,e);*_tmp97=_tmp1AA;});_tmp97;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 320
te=Cyc_Tcenv_clear_fallthru(te);else{
# 322
struct Cyc_List_List*_tmp98=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp98;
struct Cyc_List_List*_tmp99=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple17*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 323
struct Cyc_List_List*new_tvs=_tmp99;
# 325
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 328
te=Cyc_Tcenv_clear_fallthru(te);}{
# 330
struct Cyc_Absyn_Pat*_tmp9A=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmp9A;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15E;_tmp15E.tag=0U,({struct _fat_ptr _tmp1AB=({const char*_tmpA1="switch on type ";_tag_fat(_tmpA1,sizeof(char),16U);});_tmp15E.f1=_tmp1AB;});_tmp15E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp9D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15D;_tmp15D.tag=2U,_tmp15D.f1=(void*)t;_tmp15D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp9E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15C;_tmp15C.tag=0U,({struct _fat_ptr _tmp1AC=({const char*_tmpA0=" but case expects type ";_tag_fat(_tmpA0,sizeof(char),24U);});_tmp15C.f1=_tmp1AC;});_tmp15C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp9F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15B;_tmp15B.tag=2U,_tmp15B.f1=(void*)_check_null(p->topt);_tmp15B;});void*_tmp9B[4U];_tmp9B[0]=& _tmp9C,_tmp9B[1]=& _tmp9D,_tmp9B[2]=& _tmp9E,_tmp9B[3]=& _tmp9F;({unsigned _tmp1AD=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp1AD,_tag_fat(_tmp9B,sizeof(void*),4U));});});
# 334
Cyc_Unify_explain_failure();}else{
# 337
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 339
({struct Cyc_Core_Opt*_tmp1AE=({struct Cyc_Core_Opt*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->v=this_pattern.patvars;_tmpA2;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1AE;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 343
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15U: _LL1D: _tmp48=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp49=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_tmp4A=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp45)->f3;_LL1E: {struct Cyc_Absyn_Stmt*s=_tmp48;struct Cyc_List_List**scs0=_tmp49;void**dtp=_tmp4A;
# 351
({void*_tmp1B2=({void*_tmp1B1=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1B0=Cyc_Absyn_new_stmt(s->r,s->loc);_tmpA3->f1=_tmp1B0;}),({struct Cyc_Absyn_Stmt*_tmp1AF=Cyc_Absyn_skip_stmt(s->loc);_tmpA3->f2=_tmp1AF;});_tmpA3;});Cyc_Absyn_new_stmt(_tmp1B1,s->loc);})->r;s->r=_tmp1B2;});
# 353
Cyc_Tcstmt_tcStmt(te,s,1);{
# 356
struct _tuple2*def_v=({struct _tuple2*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp1B4=({struct _fat_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _fat_ptr _tmp1B3=({const char*_tmpBB="exn";_tag_fat(_tmpBB,sizeof(char),4U);});*_tmpBC=_tmp1B3;});_tmpBC;});_tmpBD->f2=_tmp1B4;});_tmpBD;});
struct Cyc_Absyn_Vardecl*_tmpA4=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmpA4;
struct Cyc_Absyn_Pat*_tmpA5=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->tag=1U,_tmpBA->f1=vd,({struct Cyc_Absyn_Pat*_tmp1B5=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmpBA->f2=_tmp1B5;});_tmpBA;}),0U);struct Cyc_Absyn_Pat*def_pat=_tmpA5;
struct Cyc_Absyn_Stmt*_tmpA6=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->tag=11U,({struct Cyc_Absyn_Exp*_tmp1B6=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->tag=4U,_tmpB8->f1=vd;_tmpB8;}),0U);_tmpB9->f1=_tmp1B6;}),_tmpB9->f2=1;_tmpB9;}),0U),0U);
# 359
struct Cyc_Absyn_Stmt*def_stmt=_tmpA6;
# 361
struct Cyc_Absyn_Switch_clause*_tmpA7=({struct Cyc_Absyn_Switch_clause*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->pattern=def_pat,_tmpB7->pat_vars=0,_tmpB7->where_clause=0,_tmpB7->body=def_stmt,_tmpB7->loc=0U;_tmpB7;});struct Cyc_Absyn_Switch_clause*def_clause=_tmpA7;
({struct Cyc_List_List*_tmp1B8=({struct Cyc_List_List*_tmp1B7=*scs0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1B7,({struct Cyc_Absyn_Switch_clause*_tmpA8[1U];_tmpA8[0]=def_clause;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpA8,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*scs0=_tmp1B8;});{
# 368
void*_tmpA9=Cyc_Absyn_exn_type();void*exception_type=_tmpA9;
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmpAA=*scs0;struct Cyc_List_List*scs=_tmpAA;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 377
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 379
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct Cyc_Tcpat_TcPatResult _tmp1B9=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& exception_type,0);*_tmpAB=_tmp1B9;});_tmpAB;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 383
te=Cyc_Tcenv_clear_fallthru(te);else{
# 385
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmpAC;
struct Cyc_List_List*_tmpAD=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple17*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 386
struct Cyc_List_List*new_tvs=_tmpAD;
# 388
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 391
te=Cyc_Tcenv_clear_fallthru(te);}{
# 393
struct Cyc_Absyn_Pat*_tmpAE=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmpAE;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),exception_type)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp162;_tmp162.tag=0U,({struct _fat_ptr _tmp1BA=({const char*_tmpB5="switch on type ";_tag_fat(_tmpB5,sizeof(char),16U);});_tmp162.f1=_tmp1BA;});_tmp162;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp161;_tmp161.tag=2U,_tmp161.f1=(void*)exception_type;_tmp161;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp160;_tmp160.tag=0U,({
struct _fat_ptr _tmp1BB=({const char*_tmpB4=" but case expects type ";_tag_fat(_tmpB4,sizeof(char),24U);});_tmp160.f1=_tmp1BB;});_tmp160;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15F;_tmp15F.tag=2U,_tmp15F.f1=(void*)_check_null(p->topt);_tmp15F;});void*_tmpAF[4U];_tmpAF[0]=& _tmpB0,_tmpAF[1]=& _tmpB1,_tmpAF[2]=& _tmpB2,_tmpAF[3]=& _tmpB3;({unsigned _tmp1BC=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp1BC,_tag_fat(_tmpAF,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}else{
# 400
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 402
({struct Cyc_Core_Opt*_tmp1BD=({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=this_pattern.patvars;_tmpB6;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1BD;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 406
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}}default: _LL1F: _tmp46=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp47=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp46;struct Cyc_Absyn_Stmt*s=_tmp47;
# 410
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmpBE=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_tmpBE;
{void*_tmpBF=d->r;void*_stmttmp4=_tmpBF;void*_tmpC0=_stmttmp4;struct Cyc_List_List*_tmpC2;struct _tuple2*_tmpC1;struct Cyc_List_List*_tmpC4;struct _fat_ptr*_tmpC3;struct Cyc_Absyn_Fndecl*_tmpC5;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC7;struct Cyc_Absyn_Tvar*_tmpC6;struct Cyc_List_List*_tmpC9;void**_tmpCD;struct Cyc_Absyn_Exp**_tmpCC;struct Cyc_Core_Opt**_tmpCB;struct Cyc_Absyn_Pat*_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCE;switch(*((int*)_tmpC0)){case 0U: _LL22: _tmpCE=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC0)->f1;_LL23: {struct Cyc_Absyn_Vardecl*vd=_tmpCE;
# 414
struct Cyc_Absyn_Vardecl*_tmpCF=vd;struct Cyc_List_List*_tmpD7;void**_tmpD6;struct Cyc_Absyn_Exp*_tmpD5;void*_tmpD4;struct Cyc_Absyn_Tqual _tmpD3;struct _fat_ptr*_tmpD2;union Cyc_Absyn_Nmspace _tmpD1;enum Cyc_Absyn_Scope _tmpD0;_LL45: _tmpD0=_tmpCF->sc;_tmpD1=(_tmpCF->name)->f1;_tmpD2=(_tmpCF->name)->f2;_tmpD3=_tmpCF->tq;_tmpD4=_tmpCF->type;_tmpD5=_tmpCF->initializer;_tmpD6=(void**)& _tmpCF->rgn;_tmpD7=_tmpCF->attributes;_LL46: {enum Cyc_Absyn_Scope sc=_tmpD0;union Cyc_Absyn_Nmspace nsl=_tmpD1;struct _fat_ptr*x=_tmpD2;struct Cyc_Absyn_Tqual tq=_tmpD3;void*t=_tmpD4;struct Cyc_Absyn_Exp*initializer=_tmpD5;void**rgn_ptr=_tmpD6;struct Cyc_List_List*atts=_tmpD7;
void*_tmpD8=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpD8;
int is_local;
{enum Cyc_Absyn_Scope _tmpD9=sc;switch(_tmpD9){case Cyc_Absyn_Static: _LL48: _LL49:
 goto _LL4B;case Cyc_Absyn_Extern: _LL4A: _LL4B:
 goto _LL4D;case Cyc_Absyn_ExternC: _LL4C: _LL4D:
# 421
 vd->escapes=1;
is_local=0;goto _LL47;case Cyc_Absyn_Abstract: _LL4E: _LL4F:
# 424
({void*_tmpDA=0U;({unsigned _tmp1BF=d->loc;struct _fat_ptr _tmp1BE=({const char*_tmpDB="bad abstract scope for local variable";_tag_fat(_tmpDB,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp1BF,_tmp1BE,_tag_fat(_tmpDA,sizeof(void*),0U));});});
goto _LL51;case Cyc_Absyn_Register: _LL50: _LL51:
 goto _LL53;case Cyc_Absyn_Public: _LL52: _LL53:
 goto _LL55;default: _LL54: _LL55:
 is_local=1;goto _LL47;}_LL47:;}
# 431
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
# 433
{void*_tmpDC=Cyc_Tcutil_compress(t);void*_stmttmp5=_tmpDC;void*_tmpDD=_stmttmp5;unsigned _tmpE1;void*_tmpE0;struct Cyc_Absyn_Tqual _tmpDF;void*_tmpDE;switch(*((int*)_tmpDD)){case 5U: _LL57: if(is_local){_LL58:
# 436
 vd->escapes=1;
sc=3U;
is_local=0;
goto _LL56;}else{goto _LL5B;}case 4U: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD)->f1).num_elts == 0){_LL59: _tmpDE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD)->f1).elt_type;_tmpDF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD)->f1).tq;_tmpE0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD)->f1).zero_term;_tmpE1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD)->f1).zt_loc;if(vd->initializer != 0){_LL5A: {void*telt=_tmpDE;struct Cyc_Absyn_Tqual tq=_tmpDF;void*zt=_tmpE0;unsigned ztl=_tmpE1;
# 441
{void*_tmpE2=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmp6=_tmpE2;void*_tmpE3=_stmttmp6;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct _fat_ptr _tmpE8;struct _fat_ptr _tmpE9;switch(*((int*)_tmpE3)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).Wstring_c).tag){case 8U: _LL5E: _tmpE9=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).String_c).val;_LL5F: {struct _fat_ptr s=_tmpE9;
# 443
t=({void*_tmp1C4=({void*_tmp1C3=telt;struct Cyc_Absyn_Tqual _tmp1C2=tq;struct Cyc_Absyn_Exp*_tmp1C1=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_tmp1C0=zt;Cyc_Absyn_array_type(_tmp1C3,_tmp1C2,_tmp1C1,_tmp1C0,ztl);});vd->type=_tmp1C4;});
goto _LL5D;}case 9U: _LL60: _tmpE8=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).Wstring_c).val;_LL61: {struct _fat_ptr s=_tmpE8;
# 446
t=({void*_tmp1C9=({void*_tmp1C8=telt;struct Cyc_Absyn_Tqual _tmp1C7=tq;struct Cyc_Absyn_Exp*_tmp1C6=Cyc_Absyn_uint_exp(1U,0U);void*_tmp1C5=zt;Cyc_Absyn_array_type(_tmp1C8,_tmp1C7,_tmp1C6,_tmp1C5,ztl);});vd->type=_tmp1C9;});
goto _LL5D;}default: goto _LL6A;}case 27U: _LL62: _tmpE7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL63: {struct Cyc_Absyn_Exp*e=_tmpE7;
_tmpE6=e;goto _LL65;}case 28U: _LL64: _tmpE6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL65: {struct Cyc_Absyn_Exp*e=_tmpE6;
# 451
t=({void*_tmp1CA=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_tmp1CA;});
goto _LL5D;}case 36U: _LL66: _tmpE5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL67: {struct Cyc_List_List*es=_tmpE5;
_tmpE4=es;goto _LL69;}case 26U: _LL68: _tmpE4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL69: {struct Cyc_List_List*es=_tmpE4;
# 455
t=({void*_tmp1CF=({void*_tmp1CE=telt;struct Cyc_Absyn_Tqual _tmp1CD=tq;struct Cyc_Absyn_Exp*_tmp1CC=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);void*_tmp1CB=zt;Cyc_Absyn_array_type(_tmp1CE,_tmp1CD,_tmp1CC,_tmp1CB,ztl);});vd->type=_tmp1CF;});
goto _LL5D;}default: _LL6A: _LL6B:
 goto _LL5D;}_LL5D:;}
# 459
goto _LL56;}}else{goto _LL5B;}}else{goto _LL5B;}default: _LL5B: _LL5C:
 goto _LL56;}_LL56:;}{
# 466
struct Cyc_List_List*_tmpEA=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*bound_vars=_tmpEA;
int _tmpEB=!is_local?0: 1;int allow_evars=_tmpEB;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Tcutil_tmk,allow_evars,1,t);
({int _tmp1D0=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp1D0;});{
# 471
struct Cyc_Tcenv_Tenv*_tmpEC=te;struct Cyc_Tcenv_Tenv*te2=_tmpEC;
if((int)sc == (int)3U ||(int)sc == (int)4U)
({void*_tmpED=0U;({unsigned _tmp1D2=d->loc;struct _fat_ptr _tmp1D1=({const char*_tmpEE="extern declarations are not yet supported within functions";_tag_fat(_tmpEE,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1D2,_tmp1D1,_tag_fat(_tmpED,sizeof(void*),0U));});});
if(initializer != 0){
Cyc_Tcexp_tcExpInitializer(te2,& t,initializer);
# 477
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({void*_tmpEF=0U;({unsigned _tmp1D4=d->loc;struct _fat_ptr _tmp1D3=({const char*_tmpF0="initializer for static variable needs to be a constant expression";_tag_fat(_tmpF0,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1D4,_tmp1D3,_tag_fat(_tmpEF,sizeof(void*),0U));});});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp1D6=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp1D5=initializer;Cyc_Tcutil_coerce_assign(_tmp1D6,_tmp1D5,t);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpF2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp167;_tmp167.tag=0U,_tmp167.f1=*x;_tmp167;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp166;_tmp166.tag=0U,({struct _fat_ptr _tmp1D7=({const char*_tmpF8=" was declared with type ";_tag_fat(_tmpF8,sizeof(char),25U);});_tmp166.f1=_tmp1D7;});_tmp166;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp165;_tmp165.tag=2U,_tmp165.f1=(void*)t;_tmp165;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp164;_tmp164.tag=0U,({
struct _fat_ptr _tmp1D8=({const char*_tmpF7=" but initialized with type ";_tag_fat(_tmpF7,sizeof(char),28U);});_tmp164.f1=_tmp1D8;});_tmp164;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp163;_tmp163.tag=2U,_tmp163.f1=(void*)_check_null(initializer->topt);_tmp163;});void*_tmpF1[5U];_tmpF1[0]=& _tmpF2,_tmpF1[1]=& _tmpF3,_tmpF1[2]=& _tmpF4,_tmpF1[3]=& _tmpF5,_tmpF1[4]=& _tmpF6;({unsigned _tmp1D9=d->loc;Cyc_Warn_err2(_tmp1D9,_tag_fat(_tmpF1,sizeof(void*),5U));});});
Cyc_Unify_unify(t,(void*)_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 486
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 2U: _LL24: _tmpCA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC0)->f1;_tmpCB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC0)->f2;_tmpCC=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC0)->f3;_tmpCD=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC0)->f4;_LL25: {struct Cyc_Absyn_Pat*p=_tmpCA;struct Cyc_Core_Opt**vds=_tmpCB;struct Cyc_Absyn_Exp**exp=_tmpCC;void**dtp=_tmpCD;
# 494
struct Cyc_Absyn_Exp*_tmpF9=*exp;struct Cyc_Absyn_Exp*e=_tmpF9;
{void*_tmpFA=p->r;void*_stmttmp7=_tmpFA;void*_tmpFB=_stmttmp7;switch(*((int*)_tmpFB)){case 1U: _LL6D: _LL6E:
 goto _LL70;case 2U: _LL6F: _LL70:
 goto _LL72;case 15U: _LL71: _LL72:
 goto _LL6C;default: _LL73: _LL74:
# 500
 if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmpFC=({struct _tuple2*_tmp1DA=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1DA,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmpFC;
struct Cyc_Absyn_Stmt*_tmpFD=Cyc_Absyn_new_stmt(s0->r,s0->loc);struct Cyc_Absyn_Stmt*s1=_tmpFD;
struct Cyc_Absyn_Decl*_tmpFE=({struct Cyc_Absyn_Pat*_tmp1DE=({void*_tmp1DC=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=1U,_tmp101->f1=vd,({struct Cyc_Absyn_Pat*_tmp1DB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp101->f2=_tmp1DB;});_tmp101;});Cyc_Absyn_new_pat(_tmp1DC,e->loc);});struct Cyc_Absyn_Exp*_tmp1DD=e;Cyc_Absyn_let_decl(_tmp1DE,_tmp1DD,s0->loc);});struct Cyc_Absyn_Decl*d=_tmpFE;
# 506
struct Cyc_Absyn_Stmt*_tmpFF=Cyc_Absyn_decl_stmt(d,s1,s0->loc);struct Cyc_Absyn_Stmt*s2=_tmpFF;
# 508
s0->r=s2->r;
({struct Cyc_Absyn_Exp*_tmp1E0=({void*_tmp1DF=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->tag=4U,_tmp100->f1=vd;_tmp100;});Cyc_Absyn_varb_exp(_tmp1DF,e->loc);});*exp=_tmp1E0;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL6C:;}
# 514
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 516
void*pat_type=(void*)_check_null(e->topt);
# 518
struct Cyc_Tcpat_TcPatResult _tmp102=Cyc_Tcpat_tcPat(te,p,& pat_type,e);struct Cyc_Tcpat_TcPatResult pat_res=_tmp102;
({struct Cyc_Core_Opt*_tmp1E1=({struct Cyc_Core_Opt*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->v=pat_res.patvars;_tmp103;});*vds=_tmp1E1;});
if(!({void*_tmp1E4=(void*)_check_null(p->topt);Cyc_Unify_unify(_tmp1E4,(void*)_check_null(e->topt));})&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1E3=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1E2=e;Cyc_Tcutil_coerce_assign(_tmp1E3,_tmp1E2,(void*)_check_null(p->topt));})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp105=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B;_tmp16B.tag=0U,({struct _fat_ptr _tmp1E5=({const char*_tmp10A="pattern type ";_tag_fat(_tmp10A,sizeof(char),14U);});_tmp16B.f1=_tmp1E5;});_tmp16B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp106=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16A;_tmp16A.tag=2U,_tmp16A.f1=(void*)_check_null(p->topt);_tmp16A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp107=({struct Cyc_Warn_String_Warn_Warg_struct _tmp169;_tmp169.tag=0U,({
struct _fat_ptr _tmp1E6=({const char*_tmp109=" does not match definition type ";_tag_fat(_tmp109,sizeof(char),33U);});_tmp169.f1=_tmp1E6;});_tmp169;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp108=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp168;_tmp168.tag=2U,_tmp168.f1=(void*)_check_null(e->topt);_tmp168;});void*_tmp104[4U];_tmp104[0]=& _tmp105,_tmp104[1]=& _tmp106,_tmp104[2]=& _tmp107,_tmp104[3]=& _tmp108;({unsigned _tmp1E7=d->loc;Cyc_Warn_err2(_tmp1E7,_tag_fat(_tmp104,sizeof(void*),4U));});});
({void*_tmp1E8=(void*)_check_null(p->topt);Cyc_Unify_unify(_tmp1E8,(void*)_check_null(e->topt));});
Cyc_Unify_explain_failure();}else{
# 528
Cyc_Tcpat_check_pat_regions(te,p,pat_res.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}case 3U: _LL26: _tmpC9=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpC0)->f1;_LL27: {struct Cyc_List_List*vds=_tmpC9;
# 534
void*_tmp10B=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp10B;
struct Cyc_Tcenv_Tenv*_tmp10C=te;struct Cyc_Tcenv_Tenv*te2=_tmp10C;
for(0;vds != 0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*_tmp10D=(struct Cyc_Absyn_Vardecl*)vds->hd;struct Cyc_Absyn_Vardecl*vd=_tmp10D;
struct Cyc_Absyn_Vardecl*_tmp10E=vd;void**_tmp111;void*_tmp110;union Cyc_Absyn_Nmspace _tmp10F;_LL76: _tmp10F=(_tmp10E->name)->f1;_tmp110=_tmp10E->type;_tmp111=(void**)& _tmp10E->rgn;_LL77: {union Cyc_Absyn_Nmspace nsl=_tmp10F;void*t=_tmp110;void**rgn_ptr=_tmp111;
*rgn_ptr=curr_bl;
({unsigned _tmp1EB=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1EA=te2;struct Cyc_List_List*_tmp1E9=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp1EB,_tmp1EA,_tmp1E9,& Cyc_Tcutil_tmk,1,1,t);});}}
# 542
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}case 4U: _LL28: _tmpC6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpC0)->f1;_tmpC7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpC0)->f2;_tmpC8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpC0)->f3;_LL29: {struct Cyc_Absyn_Tvar*tv=_tmpC6;struct Cyc_Absyn_Vardecl*vd=_tmpC7;struct Cyc_Absyn_Exp*open_exp_opt=_tmpC8;
# 548
({void*_tmp1EF=({void*_tmp1EE=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1ED=Cyc_Absyn_new_stmt(s->r,s->loc);_tmp112->f1=_tmp1ED;}),({struct Cyc_Absyn_Stmt*_tmp1EC=Cyc_Absyn_skip_stmt(s->loc);_tmp112->f2=_tmp1EC;});_tmp112;});Cyc_Absyn_new_stmt(_tmp1EE,s->loc);})->r;s->r=_tmp1EF;});{
# 550
struct Cyc_Tcenv_Tenv*_tmp113=te;struct Cyc_Tcenv_Tenv*te2=_tmp113;
struct Cyc_Absyn_Vardecl*_tmp114=vd;void**_tmp116;void**_tmp115;_LL79: _tmp115=(void**)& _tmp114->type;_tmp116=(void**)& _tmp114->rgn;_LL7A: {void**t=_tmp115;void**rgn_ptr=_tmp116;
void*_tmp117=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp117;
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 558
struct _tuple2*drname=({struct _tuple2*_tmp12D=_cycalloc(sizeof(*_tmp12D));((_tmp12D->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp1F4=({struct _fat_ptr*_tmp128[1U];({struct _fat_ptr*_tmp1F3=({struct _fat_ptr*_tmp12A=_cycalloc(sizeof(*_tmp12A));({struct _fat_ptr _tmp1F2=({const char*_tmp129="Core";_tag_fat(_tmp129,sizeof(char),5U);});*_tmp12A=_tmp1F2;});_tmp12A;});_tmp128[0]=_tmp1F3;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp128,sizeof(struct _fat_ptr*),1U));});((_tmp12D->f1).Abs_n).val=_tmp1F4;}),({
struct _fat_ptr*_tmp1F1=({struct _fat_ptr*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct _fat_ptr _tmp1F0=({const char*_tmp12B="DynamicRegion";_tag_fat(_tmp12B,sizeof(char),14U);});*_tmp12C=_tmp1F0;});_tmp12C;});_tmp12D->f2=_tmp1F1;});_tmp12D;});
void*_tmp118=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->v=& Cyc_Tcutil_trk;_tmp127;}),0);void*outer_rgn=_tmp118;
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->v=& Cyc_Tcutil_rk;_tmp119;}),0);{
void*_tmp11A=({
union Cyc_Absyn_AggrInfo _tmp1F5=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);Cyc_Absyn_aggr_type(_tmp1F5,({void*_tmp126[1U];_tmp126[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp126,sizeof(void*),1U));}));});
# 562
void*dr_type=_tmp11A;
# 564
void*_tmp11B=({void*_tmp1F8=dr_type;void*_tmp1F7=outer_rgn;struct Cyc_Absyn_Tqual _tmp1F6=
Cyc_Absyn_empty_tqual(0U);
# 564
Cyc_Absyn_at_type(_tmp1F8,_tmp1F7,_tmp1F6,Cyc_Absyn_false_type);});void*exp_type=_tmp11B;
# 566
({unsigned _tmp1FB=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FA=te;struct Cyc_List_List*_tmp1F9=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1FB,_tmp1FA,_tmp1F9,& Cyc_Tcutil_tmk,1,0,exp_type);});{
void*_tmp11C=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);void*key_typ=_tmp11C;
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1FD=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1FC=open_exp_opt;Cyc_Tcutil_coerce_assign(_tmp1FD,_tmp1FC,exp_type);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16F;_tmp16F.tag=0U,({struct _fat_ptr _tmp1FE=({const char*_tmp123="expected ";_tag_fat(_tmp123,sizeof(char),10U);});_tmp16F.f1=_tmp1FE;});_tmp16F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp11F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16E;_tmp16E.tag=2U,_tmp16E.f1=(void*)exp_type;_tmp16E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp120=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16D;_tmp16D.tag=0U,({struct _fat_ptr _tmp1FF=({const char*_tmp122=" but found ";_tag_fat(_tmp122,sizeof(char),12U);});_tmp16D.f1=_tmp1FF;});_tmp16D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp121=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16C;_tmp16C.tag=2U,_tmp16C.f1=(void*)key_typ;_tmp16C;});void*_tmp11D[4U];_tmp11D[0]=& _tmp11E,_tmp11D[1]=& _tmp11F,_tmp11D[2]=& _tmp120,_tmp11D[3]=& _tmp121;({unsigned _tmp200=s0->loc;Cyc_Warn_err2(_tmp200,_tag_fat(_tmp11D,sizeof(void*),4U));});});
if(!Cyc_Unify_unify(outer_rgn,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Unify_unify(outer_rgn,Cyc_Absyn_refcnt_rgn_type))
({void*_tmp124=0U;({unsigned _tmp202=s0->loc;struct _fat_ptr _tmp201=({const char*_tmp125="open is only allowed on unique or reference-counted keys";_tag_fat(_tmp125,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp202,_tmp201,_tag_fat(_tmp124,sizeof(void*),0U));});});
({void*_tmp203=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_tmp203;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1);}}}else{
# 577
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=2U,_tmp12E->f1=tv;_tmp12E;});
te2=({unsigned _tmp205=s0->loc;struct Cyc_Tcenv_Tenv*_tmp204=te2;Cyc_Tcenv_add_type_vars(_tmp205,_tmp204,({struct Cyc_List_List*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->hd=tv,_tmp12F->tl=0;_tmp12F;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0);}
# 581
({unsigned _tmp208=s0->loc;struct Cyc_Tcenv_Tenv*_tmp207=te;struct Cyc_List_List*_tmp206=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp208,_tmp207,_tmp206,& Cyc_Tcutil_bk,1,0,*t);});
if(!({void*_tmp209=*t;Cyc_Unify_unify(_tmp209,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmp130=0U;({struct _fat_ptr _tmp20A=({const char*_tmp131="region stmt: type of region handle is wrong!";_tag_fat(_tmp131,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp20A,_tag_fat(_tmp130,sizeof(void*),0U));});});
# 585
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1U: _LL2A: _tmpC5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpC0)->f1;_LL2B: {struct Cyc_Absyn_Fndecl*fd=_tmpC5;
# 590
void*_tmp132=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp132;
if((int)fd->sc != (int)Cyc_Absyn_Public)
({void*_tmp133=0U;({unsigned _tmp20C=d->loc;struct _fat_ptr _tmp20B=({const char*_tmp134="bad storage class for inner function";_tag_fat(_tmp134,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp20C,_tmp20B,_tag_fat(_tmp133,sizeof(void*),0U));});});
# 594
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;(unsigned)atts;atts=atts->tl){
void*_tmp135=(void*)atts->hd;void*_stmttmp8=_tmp135;void*_tmp136=_stmttmp8;switch(*((int*)_tmp136)){case 7U: _LL7C: _LL7D:
 goto _LL7F;case 6U: _LL7E: _LL7F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp138=({struct Cyc_Warn_String_Warn_Warg_struct _tmp172;_tmp172.tag=0U,({struct _fat_ptr _tmp20D=({const char*_tmp13C="bad attribute ";_tag_fat(_tmp13C,sizeof(char),15U);});_tmp172.f1=_tmp20D;});_tmp172;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp139=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp171;_tmp171.tag=9U,_tmp171.f1=(void*)atts->hd;_tmp171;});struct Cyc_Warn_String_Warn_Warg_struct _tmp13A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp170;_tmp170.tag=0U,({struct _fat_ptr _tmp20E=({const char*_tmp13B=" for function";_tag_fat(_tmp13B,sizeof(char),14U);});_tmp170.f1=_tmp20E;});_tmp170;});void*_tmp137[3U];_tmp137[0]=& _tmp138,_tmp137[1]=& _tmp139,_tmp137[2]=& _tmp13A;({unsigned _tmp20F=d->loc;Cyc_Warn_err2(_tmp20F,_tag_fat(_tmp137,sizeof(void*),3U));});});
goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}}
# 601
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_tmp210=Cyc_Tcutil_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp210;});{
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->sc=fd->sc,_tmp13D->name=fd->name,_tmp13D->varloc=0U,({struct Cyc_Absyn_Tqual _tmp215=Cyc_Absyn_const_tqual(0U);_tmp13D->tq=_tmp215;}),({
void*_tmp214=({void*_tmp213=t;void*_tmp212=curr_bl;struct Cyc_Absyn_Tqual _tmp211=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp213,_tmp212,_tmp211,Cyc_Absyn_false_type);});_tmp13D->type=_tmp214;}),_tmp13D->initializer=0,_tmp13D->rgn=curr_bl,_tmp13D->attributes=0,_tmp13D->escapes=0;_tmp13D;});
# 608
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_tmp216=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_tmp216;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9U: _LL2C: _tmpC3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC0)->f1;_tmpC4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC0)->f2;_LL2D: {struct _fat_ptr*n=_tmpC3;struct Cyc_List_List*tds=_tmpC4;
unimp_msg_part=({const char*_tmp13E="namespace";_tag_fat(_tmp13E,sizeof(char),10U);});goto _LL21;}case 10U: _LL2E: _tmpC1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpC0)->f1;_tmpC2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpC0)->f2;_LL2F: {struct _tuple2*q=_tmpC1;struct Cyc_List_List*tds=_tmpC2;
unimp_msg_part=({const char*_tmp13F="using";_tag_fat(_tmp13F,sizeof(char),6U);});goto _LL21;}case 5U: _LL30: _LL31:
 unimp_msg_part=({const char*_tmp140="type";_tag_fat(_tmp140,sizeof(char),5U);});goto _LL21;case 6U: _LL32: _LL33:
 unimp_msg_part=({const char*_tmp141="datatype";_tag_fat(_tmp141,sizeof(char),9U);});goto _LL21;case 7U: _LL34: _LL35:
 unimp_msg_part=({const char*_tmp142="enum";_tag_fat(_tmp142,sizeof(char),5U);});goto _LL21;case 8U: _LL36: _LL37:
 unimp_msg_part=({const char*_tmp143="typedef";_tag_fat(_tmp143,sizeof(char),8U);});goto _LL21;case 11U: _LL38: _LL39:
 unimp_msg_part=({const char*_tmp144="extern \"C\"";_tag_fat(_tmp144,sizeof(char),11U);});goto _LL21;case 12U: _LL3A: _LL3B:
# 625
 unimp_msg_part=({const char*_tmp145="extern \"C include\"";_tag_fat(_tmp145,sizeof(char),19U);});goto _LL21;case 13U: _LL3C: _LL3D:
 unimp_msg_part=({const char*_tmp146="__cyclone_port_on__";_tag_fat(_tmp146,sizeof(char),20U);});goto _LL21;case 14U: _LL3E: _LL3F:
 unimp_msg_part=({const char*_tmp147="__cyclone_port_off__";_tag_fat(_tmp147,sizeof(char),21U);});goto _LL21;case 15U: _LL40: _LL41:
 unimp_msg_part=({const char*_tmp148="__tempest_on__";_tag_fat(_tmp148,sizeof(char),15U);});goto _LL21;default: _LL42: _LL43:
 unimp_msg_part=({const char*_tmp149="__tempest_off__";_tag_fat(_tmp149,sizeof(char),16U);});goto _LL21;}_LL21:;}
# 631
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0U,_tmp173.f1=(struct _fat_ptr)((struct _fat_ptr)unimp_msg_part);_tmp173;});void*_tmp14A[1U];_tmp14A[0]=& _tmp14C;({struct _fat_ptr _tmp217=({const char*_tmp14B="tcStmt: nested %s declarations unimplemented";_tag_fat(_tmp14B,sizeof(char),45U);});((void*(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp217,_tag_fat(_tmp14A,sizeof(void*),1U));});}));}}_LL0:;}

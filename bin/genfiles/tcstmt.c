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
# 977
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1050
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1063
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1072
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1077
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1081
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple2*,void*,struct Cyc_Absyn_Exp*init);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
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
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 40 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp3;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2: {struct Cyc_Absyn_Exp*e1=_tmp3;
({void*_tmp17A=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=5U,_tmp4->f1=e1,_tmp4->f2=Cyc_Absyn_PreInc;_tmp4;});e->r=_tmp17A;});goto _LL0;}case Cyc_Absyn_PostDec: _LL3: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp2;
({void*_tmp17B=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=5U,_tmp5->f1=e1,_tmp5->f2=Cyc_Absyn_PreDec;_tmp5;});e->r=_tmp17B;});goto _LL0;}default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 49
static int Cyc_Tcstmt_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp6=e->r;void*_stmttmp1=_tmp6;void*_tmp7=_stmttmp1;struct Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmp9;switch(*((int*)_tmp7)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL4: {struct Cyc_Absyn_Exp*e2=_tmp9;
_tmp8=e2;goto _LL6;}case 13U: _LL5: _tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL6: {struct Cyc_Absyn_Exp*e2=_tmp8;
e=e2;continue;}default: _LL7: _LL8:
# 56
 return 0;}_LL0:;}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 66
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmpA=e->r;void*_stmttmp2=_tmpA;void*_tmpB=_stmttmp2;struct Cyc_List_List*_tmpC;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_Absyn_Exp*_tmp2E;switch(*((int*)_tmpB)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 75
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmp2E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp30=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp2E;struct Cyc_Absyn_Exp*e2=_tmp2F;struct Cyc_Absyn_Exp*e3=_tmp30;
# 85
if(!Cyc_Tcstmt_noassign_exp(e1))return 0;
_tmp2C=e2;_tmp2D=e3;goto _LL20;}case 27U: _LL1F: _tmp2C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_tmp2D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp2C;struct Cyc_Absyn_Exp*e2=_tmp2D;
_tmp2A=e1;_tmp2B=e2;goto _LL22;}case 7U: _LL21: _tmp2A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp2B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp2A;struct Cyc_Absyn_Exp*e2=_tmp2B;
_tmp28=e1;_tmp29=e2;goto _LL24;}case 8U: _LL23: _tmp28=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp29=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp28;struct Cyc_Absyn_Exp*e2=_tmp29;
_tmp26=e1;_tmp27=e2;goto _LL26;}case 23U: _LL25: _tmp26=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp27=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL26: {struct Cyc_Absyn_Exp*e1=_tmp26;struct Cyc_Absyn_Exp*e2=_tmp27;
_tmp24=e1;_tmp25=e2;goto _LL28;}case 9U: _LL27: _tmp24=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp25=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp24;struct Cyc_Absyn_Exp*e2=_tmp25;
return Cyc_Tcstmt_noassign_exp(e1)&& Cyc_Tcstmt_noassign_exp(e2);}case 41U: _LL29: _tmp23=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL2A: {struct Cyc_Absyn_Exp*e=_tmp23;
# 93
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
# 106
_tmp14=eopt;_tmp15=e;goto _LL44;}case 16U: _LL43: _tmp14=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_tmp15=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL44: {struct Cyc_Absyn_Exp*eopt=_tmp14;struct Cyc_Absyn_Exp*e=_tmp15;
# 108
if(!Cyc_Tcstmt_noassign_exp(e))return 0;
if(eopt != 0)return Cyc_Tcstmt_noassign_exp(eopt);
return 1;}case 3U: _LL45: _tmp13=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL46: {struct Cyc_List_List*es=_tmp13;
# 112
_tmp12=es;goto _LL48;}case 31U: _LL47: _tmp12=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL48: {struct Cyc_List_List*es=_tmp12;
_tmp11=es;goto _LL4A;}case 24U: _LL49: _tmp11=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL4A: {struct Cyc_List_List*es=_tmp11;
return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,es);}case 36U: _LL4B: _tmp10=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL4C: {struct Cyc_List_List*dles=_tmp10;
# 116
_tmpF=dles;goto _LL4E;}case 29U: _LL4D: _tmpF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB)->f3;_LL4E: {struct Cyc_List_List*dles=_tmpF;
_tmpE=dles;goto _LL50;}case 30U: _LL4F: _tmpE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL50: {struct Cyc_List_List*dles=_tmpE;
_tmpD=dles;goto _LL52;}case 26U: _LL51: _tmpD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB)->f1;_LL52: {struct Cyc_List_List*dles=_tmpD;
_tmpC=dles;goto _LL54;}default: _LL53: _tmpC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB)->f2;_LL54: {struct Cyc_List_List*dles=_tmpC;
# 121
for(0;dles != 0;dles=dles->tl){
if(!Cyc_Tcstmt_noassign_exp((*((struct _tuple15*)dles->hd)).f2))
return 0;}
return 1;}}_LL0:;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 129
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 131
struct Cyc_Tcpat_TcPatResult _tmp31=pat_res;struct Cyc_List_List*_tmp33;struct _tuple1*_tmp32;_LL1: _tmp32=_tmp31.tvars_and_bounds_opt;_tmp33=_tmp31.patvars;_LL2: {struct _tuple1*p=_tmp32;struct Cyc_List_List*vse=_tmp33;
struct Cyc_List_List*_tmp34=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple17*))Cyc_Core_fst,vse);struct Cyc_List_List*vs=_tmp34;
struct Cyc_List_List*_tmp35=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*p).f1);struct Cyc_List_List*tvs=_tmp35;
struct Cyc_List_List*_tmp36=p == 0?0:(*p).f2;struct Cyc_List_List*bds=_tmp36;
struct Cyc_List_List*_tmp37=p == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple16*))Cyc_Core_snd,(*p).f1));struct Cyc_List_List*rgns=_tmp37;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(0;rgns != 0;rgns=rgns->tl){
te2=({struct Cyc_Tcenv_Tenv*_tmp17C=te2;Cyc_Tcenv_add_region(_tmp17C,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->tag=2U,_tmp38->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_tmp38;}),0);});}
te2=Cyc_Tcenv_new_outlives_constraints(te2,bds,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(loc,te2);{
void*_tmp39=Cyc_Tcenv_curr_rgn(te2);void*ropt=_tmp39;
{struct Cyc_List_List*_tmp3A=vs;struct Cyc_List_List*vs2=_tmp3A;for(0;vs2 != 0;vs2=vs2->tl){
if((struct Cyc_Absyn_Vardecl**)vs2->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)vs2->hd)))->rgn=ropt;}}
# 147
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp17E=te2;struct Cyc_Absyn_Exp*_tmp17D=where_opt;Cyc_Tcexp_tcTest(_tmp17E,_tmp17D,({const char*_tmp3B="switch clause guard";_tag_fat(_tmp3B,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp3C=0U;({unsigned _tmp180=where_opt->loc;struct _fat_ptr _tmp17F=({const char*_tmp3D="cannot show &&-clause in pattern has no effects";_tag_fat(_tmp3D,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp180,_tmp17F,_tag_fat(_tmp3C,sizeof(void*),0U));});});}
# 152
Cyc_Tcstmt_tcStmt(te2,s,0);}}}
# 155
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
static int counter=0;
struct _tuple2*res=({struct _tuple2*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp183=({struct _fat_ptr*_tmp41=_cycalloc(sizeof(*_tmp41));({struct _fat_ptr _tmp182=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp40=({struct Cyc_Int_pa_PrintArg_struct _tmp14C;_tmp14C.tag=1U,_tmp14C.f1=(unsigned)counter ++;_tmp14C;});void*_tmp3E[1U];_tmp3E[0]=& _tmp40;({struct _fat_ptr _tmp181=({const char*_tmp3F="_stmttmp%X";_tag_fat(_tmp3F,sizeof(char),11U);});Cyc_aprintf(_tmp181,_tag_fat(_tmp3E,sizeof(void*),1U));});});*_tmp41=_tmp182;});_tmp41;});_tmp42->f2=_tmp183;});_tmp42;});
return res;}
# 168 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 170
void*_tmp43=s0->r;void*_stmttmp3=_tmp43;void*_tmp44=_stmttmp3;struct Cyc_Absyn_Stmt*_tmp46;struct Cyc_Absyn_Decl*_tmp45;void**_tmp49;struct Cyc_List_List**_tmp48;struct Cyc_Absyn_Stmt*_tmp47;void**_tmp4C;struct Cyc_List_List*_tmp4B;struct Cyc_Absyn_Exp**_tmp4A;struct Cyc_Absyn_Switch_clause***_tmp4E;struct Cyc_List_List*_tmp4D;struct Cyc_Absyn_Stmt*_tmp50;struct _fat_ptr*_tmp4F;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Stmt*_tmp59;struct Cyc_Absyn_Stmt*_tmp58;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Stmt*_tmp56;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Stmt*_tmp5C;struct Cyc_Absyn_Stmt*_tmp5B;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_Absyn_Stmt*_tmp5F;struct Cyc_Absyn_Stmt*_tmp5E;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Exp*_tmp60;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Stmt*_tmp61;struct Cyc_Absyn_Exp*_tmp63;switch(*((int*)_tmp44)){case 0U: _LL1: _LL2:
# 172
 return;case 1U: _LL3: _tmp63=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp63;
# 175
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2U: _LL5: _tmp61=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp62=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp61;struct Cyc_Absyn_Stmt*s2=_tmp62;
# 181
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3U: _LL7: _tmp60=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmp60;
# 186
void*t=Cyc_Tcenv_return_typ(te);
if(eopt == 0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp65=({struct Cyc_Warn_String_Warn_Warg_struct _tmp14E;_tmp14E.tag=0U,({struct _fat_ptr _tmp184=({const char*_tmp67="should return a value of type ";_tag_fat(_tmp67,sizeof(char),31U);});_tmp14E.f1=_tmp184;});_tmp14E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp66=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14D;_tmp14D.tag=2U,_tmp14D.f1=(void*)t;_tmp14D;});void*_tmp64[2U];_tmp64[0]=& _tmp65,_tmp64[1]=& _tmp66;({unsigned _tmp185=s0->loc;Cyc_Warn_warn2(_tmp185,_tag_fat(_tmp64,sizeof(void*),2U));});});else{
# 192
({struct Cyc_Warn_String_Warn_Warg_struct _tmp69=({struct Cyc_Warn_String_Warn_Warg_struct _tmp150;_tmp150.tag=0U,({struct _fat_ptr _tmp186=({const char*_tmp6B="must return a value of type ";_tag_fat(_tmp6B,sizeof(char),29U);});_tmp150.f1=_tmp186;});_tmp150;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14F;_tmp14F.tag=2U,_tmp14F.f1=(void*)t;_tmp14F;});void*_tmp68[2U];_tmp68[0]=& _tmp69,_tmp68[1]=& _tmp6A;({unsigned _tmp187=s0->loc;Cyc_Warn_err2(_tmp187,_tag_fat(_tmp68,sizeof(void*),2U));});});}}
# 194
return;}
# 197
Cyc_Tcexp_tcExp(te,& t,eopt);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp189=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp188=eopt;Cyc_Tcutil_coerce_arg(_tmp189,_tmp188,t,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp154;_tmp154.tag=0U,({struct _fat_ptr _tmp18A=({const char*_tmp72="returns value of type ";_tag_fat(_tmp72,sizeof(char),23U);});_tmp154.f1=_tmp18A;});_tmp154;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp6E=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp153;_tmp153.tag=3U,_tmp153.f1=(void*)eopt->topt;_tmp153;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp152;_tmp152.tag=0U,({struct _fat_ptr _tmp18B=({const char*_tmp71=" but requires ";_tag_fat(_tmp71,sizeof(char),15U);});_tmp152.f1=_tmp18B;});_tmp152;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp70=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp151;_tmp151.tag=2U,_tmp151.f1=(void*)t;_tmp151;});void*_tmp6C[4U];_tmp6C[0]=& _tmp6D,_tmp6C[1]=& _tmp6E,_tmp6C[2]=& _tmp6F,_tmp6C[3]=& _tmp70;({unsigned _tmp18C=s0->loc;Cyc_Warn_err2(_tmp18C,_tag_fat(_tmp6C,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 202
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(eopt))
Cyc_Tcexp_unique_consume_err(eopt->loc);
return;}case 4U: _LL9: _tmp5D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp5E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp5F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmp5D;struct Cyc_Absyn_Stmt*s1=_tmp5E;struct Cyc_Absyn_Stmt*s2=_tmp5F;
# 207
({struct Cyc_Tcenv_Tenv*_tmp18E=te;struct Cyc_Absyn_Exp*_tmp18D=e;Cyc_Tcexp_tcTest(_tmp18E,_tmp18D,({const char*_tmp73="if statement";_tag_fat(_tmp73,sizeof(char),13U);}));});
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5U: _LLB: _tmp5A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f1).f1;_tmp5B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f1).f2;_tmp5C=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LLC: {struct Cyc_Absyn_Exp*e=_tmp5A;struct Cyc_Absyn_Stmt*cont_s=_tmp5B;struct Cyc_Absyn_Stmt*s=_tmp5C;
# 213
({struct Cyc_Tcenv_Tenv*_tmp190=te;struct Cyc_Absyn_Exp*_tmp18F=e;Cyc_Tcexp_tcTest(_tmp190,_tmp18F,({const char*_tmp74="while loop";_tag_fat(_tmp74,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9U: _LLD: _tmp54=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp55=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f1;_tmp56=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f2;_tmp57=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f3).f1;_tmp58=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f3).f2;_tmp59=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f4;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp54;struct Cyc_Absyn_Exp*e2=_tmp55;struct Cyc_Absyn_Stmt*guard_s=_tmp56;struct Cyc_Absyn_Exp*e3=_tmp57;struct Cyc_Absyn_Stmt*cont_s=_tmp58;struct Cyc_Absyn_Stmt*s=_tmp59;
# 218
Cyc_Tcexp_tcExp(te,0,e1);
({struct Cyc_Tcenv_Tenv*_tmp192=te;struct Cyc_Absyn_Exp*_tmp191=e2;Cyc_Tcexp_tcTest(_tmp192,_tmp191,({const char*_tmp75="for loop";_tag_fat(_tmp75,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14U: _LLF: _tmp51=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp52=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f1;_tmp53=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f2;_LL10: {struct Cyc_Absyn_Stmt*s=_tmp51;struct Cyc_Absyn_Exp*e=_tmp52;struct Cyc_Absyn_Stmt*cont_s=_tmp53;
# 226
Cyc_Tcstmt_tcStmt(te,s,1);
({struct Cyc_Tcenv_Tenv*_tmp194=te;struct Cyc_Absyn_Exp*_tmp193=e;Cyc_Tcexp_tcTest(_tmp194,_tmp193,({const char*_tmp76="do loop";_tag_fat(_tmp76,sizeof(char),8U);}));});
return;}case 6U: _LL11: _LL12:
# 230
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 13U: _LL17: _tmp4F=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp50=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL18: {struct _fat_ptr*l=_tmp4F;struct Cyc_Absyn_Stmt*s=_tmp50;
# 237
struct Cyc_Absyn_Tvar*_tmp77=({struct Cyc_Absyn_Tvar*_tmp7C=_cycalloc(sizeof(*_tmp7C));({struct _fat_ptr*_tmp199=({struct _fat_ptr*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct _fat_ptr _tmp198=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U,_tmp155.f1=(struct _fat_ptr)((struct _fat_ptr)*l);_tmp155;});void*_tmp78[1U];_tmp78[0]=& _tmp7A;({struct _fat_ptr _tmp197=({const char*_tmp79="`%s";_tag_fat(_tmp79,sizeof(char),4U);});Cyc_aprintf(_tmp197,_tag_fat(_tmp78,sizeof(void*),1U));});});*_tmp7B=_tmp198;});_tmp7B;});_tmp7C->name=_tmp199;}),({
int _tmp196=Cyc_Tcutil_new_tvar_id();_tmp7C->identity=_tmp196;}),({void*_tmp195=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp7C->kind=_tmp195;});_tmp7C;});
# 237
struct Cyc_Absyn_Tvar*tv=_tmp77;
# 239
({struct Cyc_Tcenv_Tenv*_tmp19A=Cyc_Tcenv_new_named_block(s0->loc,te,tv);Cyc_Tcstmt_tcStmt(_tmp19A,s,0);});
return;}case 11U: _LL19: _tmp4D=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp4E=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL1A: {struct Cyc_List_List*es=_tmp4D;struct Cyc_Absyn_Switch_clause***clauseopt=_tmp4E;
# 243
const struct _tuple14*_tmp7D=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);const struct _tuple14*trip_opt=_tmp7D;
if(trip_opt == (const struct _tuple14*)0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp156;_tmp156.tag=0U,({struct _fat_ptr _tmp19B=({const char*_tmp80="fallthru not in a non-last case";_tag_fat(_tmp80,sizeof(char),32U);});_tmp156.f1=_tmp19B;});_tmp156;});void*_tmp7E[1U];_tmp7E[0]=& _tmp7F;({unsigned _tmp19C=s0->loc;Cyc_Warn_err2(_tmp19C,_tag_fat(_tmp7E,sizeof(void*),1U));});});
return;}{
# 248
struct _tuple14 _tmp81=*trip_opt;struct _tuple14 _stmttmp4=_tmp81;struct _tuple14 _tmp82=_stmttmp4;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp83;_LL22: _tmp83=_tmp82.f2;_tmp84=_tmp82.f3;_LL23: {struct Cyc_List_List*tvs=_tmp83;struct Cyc_List_List*ts_orig=_tmp84;
struct Cyc_List_List*_tmp85=({struct Cyc_List_List*_tmp19D=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp19D,tvs);});struct Cyc_List_List*instantiation=_tmp85;
struct Cyc_List_List*_tmp86=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,ts_orig);struct Cyc_List_List*ts=_tmp86;
for(0;ts != 0 && es != 0;(ts=ts->tl,es=es->tl)){
# 253
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp19F=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp19E=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp19F,_tmp19E,(void*)ts->hd,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15A;_tmp15A.tag=0U,({struct _fat_ptr _tmp1A0=({const char*_tmp8D="fallthru argument has type ";_tag_fat(_tmp8D,sizeof(char),28U);});_tmp15A.f1=_tmp1A0;});_tmp15A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp89=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp159;_tmp159.tag=3U,_tmp159.f1=(void*)((struct Cyc_Absyn_Exp*)es->hd)->topt;_tmp159;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp158;_tmp158.tag=0U,({
struct _fat_ptr _tmp1A1=({const char*_tmp8C="but pattern variable has type ";_tag_fat(_tmp8C,sizeof(char),31U);});_tmp158.f1=_tmp1A1;});_tmp158;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp8B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp157;_tmp157.tag=2U,_tmp157.f1=(void*)ts->hd;_tmp157;});void*_tmp87[4U];_tmp87[0]=& _tmp88,_tmp87[1]=& _tmp89,_tmp87[2]=& _tmp8A,_tmp87[3]=& _tmp8B;({unsigned _tmp1A2=s0->loc;Cyc_Warn_err2(_tmp1A2,_tag_fat(_tmp87,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}
# 260
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)ts->hd)&& !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)es->hd))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)es->hd)->loc);}
# 263
if(es != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15B;_tmp15B.tag=0U,({struct _fat_ptr _tmp1A3=({const char*_tmp90="too many arguments in explicit fallthru";_tag_fat(_tmp90,sizeof(char),40U);});_tmp15B.f1=_tmp1A3;});_tmp15B;});void*_tmp8E[1U];_tmp8E[0]=& _tmp8F;({unsigned _tmp1A4=s0->loc;Cyc_Warn_err2(_tmp1A4,_tag_fat(_tmp8E,sizeof(void*),1U));});});
if(ts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp92=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15C;_tmp15C.tag=0U,({struct _fat_ptr _tmp1A5=({const char*_tmp93="too few arguments in explicit fallthru";_tag_fat(_tmp93,sizeof(char),39U);});_tmp15C.f1=_tmp1A5;});_tmp15C;});void*_tmp91[1U];_tmp91[0]=& _tmp92;({unsigned _tmp1A6=s0->loc;Cyc_Warn_err2(_tmp1A6,_tag_fat(_tmp91,sizeof(void*),1U));});});
return;}}}case 10U: _LL1B: _tmp4A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp4B=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp4C=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LL1C: {struct Cyc_Absyn_Exp**exp=_tmp4A;struct Cyc_List_List*scs0=_tmp4B;void**dtp=_tmp4C;
# 272
struct Cyc_Absyn_Exp*_tmp94=*exp;struct Cyc_Absyn_Exp*e=_tmp94;
if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmp95=({struct _tuple2*_tmp1A7=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1A7,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp95;
struct Cyc_Absyn_Stmt*_tmp96=Cyc_Absyn_new_stmt(s0->r,s0->loc);struct Cyc_Absyn_Stmt*s1=_tmp96;
struct Cyc_Absyn_Decl*_tmp97=({struct Cyc_Absyn_Pat*_tmp1AB=({void*_tmp1A9=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=1U,_tmp9A->f1=vd,({struct Cyc_Absyn_Pat*_tmp1A8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp9A->f2=_tmp1A8;});_tmp9A;});Cyc_Absyn_new_pat(_tmp1A9,e->loc);});struct Cyc_Absyn_Exp*_tmp1AA=e;Cyc_Absyn_let_decl(_tmp1AB,_tmp1AA,s0->loc);});struct Cyc_Absyn_Decl*d=_tmp97;
# 279
struct Cyc_Absyn_Stmt*_tmp98=Cyc_Absyn_decl_stmt(d,s1,s0->loc);struct Cyc_Absyn_Stmt*s2=_tmp98;
s0->r=s2->r;
({struct Cyc_Absyn_Exp*_tmp1AD=({void*_tmp1AC=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=4U,_tmp99->f1=vd;_tmp99;});Cyc_Absyn_varb_exp(_tmp1AC,e->loc);});*exp=_tmp1AD;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 286
void*_tmp9B=Cyc_Tcexp_tcExp(te,0,e);void*t=_tmp9B;
# 288
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(e->loc);{
# 293
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp9C=scs0;struct Cyc_List_List*scs=_tmp9C;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 301
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 303
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct Cyc_Tcpat_TcPatResult _tmp1AE=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& t,e);*_tmp9D=_tmp1AE;});_tmp9D;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 307
te=Cyc_Tcenv_clear_fallthru(te);else{
# 309
struct Cyc_List_List*_tmp9E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmp9E;
struct Cyc_List_List*_tmp9F=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 310
struct Cyc_List_List*new_tvs=_tmp9F;
# 312
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 315
te=Cyc_Tcenv_clear_fallthru(te);}{
# 317
struct Cyc_Absyn_Pat*_tmpA0=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmpA0;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp160;_tmp160.tag=0U,({
struct _fat_ptr _tmp1AF=({const char*_tmpA7="switch on type ";_tag_fat(_tmpA7,sizeof(char),16U);});_tmp160.f1=_tmp1AF;});_tmp160;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15F;_tmp15F.tag=2U,_tmp15F.f1=(void*)t;_tmp15F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp15E;_tmp15E.tag=0U,({struct _fat_ptr _tmp1B0=({const char*_tmpA6=" but case expects type ";_tag_fat(_tmpA6,sizeof(char),24U);});_tmp15E.f1=_tmp1B0;});_tmp15E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmpA5=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp15D;_tmp15D.tag=3U,_tmp15D.f1=(void*)p->topt;_tmp15D;});void*_tmpA1[4U];_tmpA1[0]=& _tmpA2,_tmpA1[1]=& _tmpA3,_tmpA1[2]=& _tmpA4,_tmpA1[3]=& _tmpA5;({unsigned _tmp1B1=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp1B1,_tag_fat(_tmpA1,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}else{
# 324
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 326
({struct Cyc_Core_Opt*_tmp1B2=({struct Cyc_Core_Opt*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->v=this_pattern.patvars;_tmpA8;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1B2;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 330
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15U: _LL1D: _tmp47=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp48=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp49=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LL1E: {struct Cyc_Absyn_Stmt*s=_tmp47;struct Cyc_List_List**scs0=_tmp48;void**dtp=_tmp49;
# 338
({void*_tmp1B6=({void*_tmp1B5=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1B4=Cyc_Absyn_new_stmt(s->r,s->loc);_tmpA9->f1=_tmp1B4;}),({struct Cyc_Absyn_Stmt*_tmp1B3=Cyc_Absyn_skip_stmt(s->loc);_tmpA9->f2=_tmp1B3;});_tmpA9;});Cyc_Absyn_new_stmt(_tmp1B5,s->loc);})->r;s->r=_tmp1B6;});
# 340
Cyc_Tcstmt_tcStmt(te,s,1);{
# 343
struct _tuple2*def_v=({struct _tuple2*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp1B8=({struct _fat_ptr*_tmpC2=_cycalloc(sizeof(*_tmpC2));({struct _fat_ptr _tmp1B7=({const char*_tmpC1="exn";_tag_fat(_tmpC1,sizeof(char),4U);});*_tmpC2=_tmp1B7;});_tmpC2;});_tmpC3->f2=_tmp1B8;});_tmpC3;});
struct Cyc_Absyn_Vardecl*_tmpAA=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmpAA;
struct Cyc_Absyn_Pat*_tmpAB=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->tag=1U,_tmpC0->f1=vd,({struct Cyc_Absyn_Pat*_tmp1B9=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmpC0->f2=_tmp1B9;});_tmpC0;}),0U);struct Cyc_Absyn_Pat*def_pat=_tmpAB;
struct Cyc_Absyn_Stmt*_tmpAC=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->tag=11U,({struct Cyc_Absyn_Exp*_tmp1BA=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=4U,_tmpBE->f1=vd;_tmpBE;}),0U);_tmpBF->f1=_tmp1BA;}),_tmpBF->f2=1;_tmpBF;}),0U),0U);
# 346
struct Cyc_Absyn_Stmt*def_stmt=_tmpAC;
# 348
struct Cyc_Absyn_Switch_clause*_tmpAD=({struct Cyc_Absyn_Switch_clause*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->pattern=def_pat,_tmpBD->pat_vars=0,_tmpBD->where_clause=0,_tmpBD->body=def_stmt,_tmpBD->loc=0U;_tmpBD;});struct Cyc_Absyn_Switch_clause*def_clause=_tmpAD;
({struct Cyc_List_List*_tmp1BC=({struct Cyc_List_List*_tmp1BB=*scs0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BB,({struct Cyc_Absyn_Switch_clause*_tmpAE[1U];_tmpAE[0]=def_clause;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpAE,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*scs0=_tmp1BC;});{
# 355
void*_tmpAF=Cyc_Absyn_exn_type();void*exception_type=_tmpAF;
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmpB0=*scs0;struct Cyc_List_List*scs=_tmpB0;for(0;scs != 0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 364
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 366
if(scs->tl != 0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct Cyc_Tcpat_TcPatResult _tmp1BD=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pattern,& exception_type,0);*_tmpB1=_tmp1BD;});_tmpB1;});
if(next_pattern->tvars_and_bounds_opt != 0 &&(*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 370
te=Cyc_Tcenv_clear_fallthru(te);else{
# 372
struct Cyc_List_List*_tmpB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));struct Cyc_List_List*vs=_tmpB2;
struct Cyc_List_List*_tmpB3=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
# 373
struct Cyc_List_List*new_tvs=_tmpB3;
# 375
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd);}}else{
# 378
te=Cyc_Tcenv_clear_fallthru(te);}{
# 380
struct Cyc_Absyn_Pat*_tmpB4=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Absyn_Pat*p=_tmpB4;
if(!Cyc_Unify_unify((void*)_check_null(p->topt),exception_type)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp164;_tmp164.tag=0U,({struct _fat_ptr _tmp1BE=({const char*_tmpBB="switch on type ";_tag_fat(_tmpBB,sizeof(char),16U);});_tmp164.f1=_tmp1BE;});_tmp164;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp163;_tmp163.tag=2U,_tmp163.f1=(void*)exception_type;_tmp163;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp162;_tmp162.tag=0U,({
struct _fat_ptr _tmp1BF=({const char*_tmpBA=" but case expects type ";_tag_fat(_tmpBA,sizeof(char),24U);});_tmp162.f1=_tmp1BF;});_tmp162;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp161;_tmp161.tag=3U,_tmp161.f1=(void*)p->topt;_tmp161;});void*_tmpB5[4U];_tmpB5[0]=& _tmpB6,_tmpB5[1]=& _tmpB7,_tmpB5[2]=& _tmpB8,_tmpB5[3]=& _tmpB9;({unsigned _tmp1C0=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;Cyc_Warn_err2(_tmp1C0,_tag_fat(_tmpB5,sizeof(void*),4U));});});
Cyc_Unify_explain_failure();}else{
# 387
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 389
({struct Cyc_Core_Opt*_tmp1C1=({struct Cyc_Core_Opt*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->v=this_pattern.patvars;_tmpBC;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp1C1;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 393
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}}default: _LL1F: _tmp45=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp46=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL20: {struct Cyc_Absyn_Decl*d=_tmp45;struct Cyc_Absyn_Stmt*s=_tmp46;
# 397
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmpC4=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_tmpC4;
{void*_tmpC5=d->r;void*_stmttmp5=_tmpC5;void*_tmpC6=_stmttmp5;struct Cyc_List_List*_tmpC8;struct _tuple2*_tmpC7;struct Cyc_List_List*_tmpCA;struct _fat_ptr*_tmpC9;struct Cyc_Absyn_Fndecl*_tmpCB;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Vardecl*_tmpCD;struct Cyc_Absyn_Tvar*_tmpCC;struct Cyc_List_List*_tmpCF;void**_tmpD3;struct Cyc_Absyn_Exp**_tmpD2;struct Cyc_Core_Opt**_tmpD1;struct Cyc_Absyn_Pat*_tmpD0;struct Cyc_Absyn_Vardecl*_tmpD4;switch(*((int*)_tmpC6)){case 0U: _LL25: _tmpD4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;_LL26: {struct Cyc_Absyn_Vardecl*vd=_tmpD4;
# 401
struct Cyc_Absyn_Vardecl*_tmpD5=vd;void**_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;void*_tmpD8;struct Cyc_Absyn_Tqual _tmpD7;enum Cyc_Absyn_Scope _tmpD6;_LL48: _tmpD6=_tmpD5->sc;_tmpD7=_tmpD5->tq;_tmpD8=_tmpD5->type;_tmpD9=_tmpD5->initializer;_tmpDA=(void**)& _tmpD5->rgn;_LL49: {enum Cyc_Absyn_Scope sc=_tmpD6;struct Cyc_Absyn_Tqual tq=_tmpD7;void*t=_tmpD8;struct Cyc_Absyn_Exp*initializer=_tmpD9;void**rgn_ptr=_tmpDA;
void*_tmpDB=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmpDB;
int is_local;
{enum Cyc_Absyn_Scope _tmpDC=sc;switch(_tmpDC){case Cyc_Absyn_Static: _LL4B: _LL4C:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4D: _LL4E:
 goto _LL50;case Cyc_Absyn_ExternC: _LL4F: _LL50:
 vd->escapes=1;is_local=0;goto _LL4A;case Cyc_Absyn_Abstract: _LL51: _LL52:
# 409
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp165;_tmp165.tag=0U,({struct _fat_ptr _tmp1C2=({const char*_tmpDF="bad abstract scope for local variable";_tag_fat(_tmpDF,sizeof(char),38U);});_tmp165.f1=_tmp1C2;});_tmp165;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDE;({unsigned _tmp1C3=d->loc;Cyc_Warn_err2(_tmp1C3,_tag_fat(_tmpDD,sizeof(void*),1U));});});
goto _LL54;case Cyc_Absyn_Register: _LL53: _LL54:
 goto _LL56;case Cyc_Absyn_Public: _LL55: _LL56:
 goto _LL58;default: _LL57: _LL58:
 is_local=1;goto _LL4A;}_LL4A:;}
# 416
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
{void*_tmpE0=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmpE0;void*_tmpE1=_stmttmp6;struct Cyc_Absyn_ArrayInfo _tmpE2;switch(*((int*)_tmpE1)){case 5U: _LL5A: if(is_local){_LL5B:
# 420
 vd->escapes=1;
sc=3U;
is_local=0;
goto _LL59;}else{goto _LL5E;}case 4U: _LL5C: _tmpE2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE1)->f1;_LL5D: {struct Cyc_Absyn_ArrayInfo ai=_tmpE2;
# 426
if(vd->initializer == 0 || ai.num_elts != 0)
goto _LL59;{
struct Cyc_Absyn_Exp*len;
{void*_tmpE3=((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*_stmttmp7=_tmpE3;void*_tmpE4=_stmttmp7;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct _fat_ptr _tmpE9;struct _fat_ptr _tmpEA;switch(*((int*)_tmpE4)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE4)->f1).Wstring_c).tag){case 8U: _LL61: _tmpEA=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE4)->f1).String_c).val;_LL62: {struct _fat_ptr s=_tmpEA;
len=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);goto _LL60;}case 9U: _LL63: _tmpE9=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE4)->f1).Wstring_c).val;_LL64: {struct _fat_ptr s=_tmpE9;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL60;}default: goto _LL6D;}case 27U: _LL65: _tmpE8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE4)->f2;_LL66: {struct Cyc_Absyn_Exp*e=_tmpE8;
_tmpE7=e;goto _LL68;}case 28U: _LL67: _tmpE7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE4)->f1;_LL68: {struct Cyc_Absyn_Exp*e=_tmpE7;
len=e;goto _LL60;}case 36U: _LL69: _tmpE6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE4)->f2;_LL6A: {struct Cyc_List_List*es=_tmpE6;
_tmpE5=es;goto _LL6C;}case 26U: _LL6B: _tmpE5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE4)->f1;_LL6C: {struct Cyc_List_List*es=_tmpE5;
len=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es),0U);goto _LL60;}default: _LL6D: _LL6E:
 len=0;goto _LL60;}_LL60:;}
# 438
ai.num_elts=len;
t=({void*_tmp1C4=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->tag=4U,_tmpEB->f1=ai;_tmpEB;});vd->type=_tmp1C4;});
goto _LL59;}}default: _LL5E: _LL5F:
 goto _LL59;}_LL59:;}{
# 446
struct Cyc_List_List*_tmpEC=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*bound_vars=_tmpEC;
int _tmpED=is_local;int allow_evars=_tmpED;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Tcutil_tmk,allow_evars,1,t);
({int _tmp1C5=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(vd->tq).print_const,t);(vd->tq).real_const=_tmp1C5;});
# 451
if((int)sc == (int)3U ||(int)sc == (int)4U)
({void*_tmpEE=0U;({unsigned _tmp1C7=d->loc;struct _fat_ptr _tmp1C6=({const char*_tmpEF="extern declarations are not yet supported within functions";_tag_fat(_tmpEF,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1C7,_tmp1C6,_tag_fat(_tmpEE,sizeof(void*),0U));});});
if(initializer != 0){
Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
# 456
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({void*_tmpF0=0U;({unsigned _tmp1C9=d->loc;struct _fat_ptr _tmp1C8=({const char*_tmpF1="initializer for static variable needs to be a constant expression";_tag_fat(_tmpF1,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1C9,_tmp1C8,_tag_fat(_tmpF0,sizeof(void*),0U));});});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp1CB=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1CA=initializer;Cyc_Tcutil_coerce_assign(_tmp1CB,_tmp1CA,t);})){
({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmpF3=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _tmp16A;_tmp16A.tag=11U,_tmp16A.f1=vd;_tmp16A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp169;_tmp169.tag=0U,({struct _fat_ptr _tmp1CC=({const char*_tmpF9=" was declared with type ";_tag_fat(_tmpF9,sizeof(char),25U);});_tmp169.f1=_tmp1CC;});_tmp169;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpF5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp168;_tmp168.tag=2U,_tmp168.f1=(void*)t;_tmp168;});struct Cyc_Warn_String_Warn_Warg_struct _tmpF6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp167;_tmp167.tag=0U,({
struct _fat_ptr _tmp1CD=({const char*_tmpF8=" but initialized with type ";_tag_fat(_tmpF8,sizeof(char),28U);});_tmp167.f1=_tmp1CD;});_tmp167;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmpF7=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp166;_tmp166.tag=3U,_tmp166.f1=(void*)initializer->topt;_tmp166;});void*_tmpF2[5U];_tmpF2[0]=& _tmpF3,_tmpF2[1]=& _tmpF4,_tmpF2[2]=& _tmpF5,_tmpF2[3]=& _tmpF6,_tmpF2[4]=& _tmpF7;({unsigned _tmp1CE=d->loc;Cyc_Warn_err2(_tmp1CE,_tag_fat(_tmpF2,sizeof(void*),5U));});});
Cyc_Unify_unify(t,(void*)_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 465
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2U: _LL27: _tmpD0=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;_tmpD1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC6)->f2;_tmpD2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC6)->f3;_tmpD3=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpC6)->f4;_LL28: {struct Cyc_Absyn_Pat*p=_tmpD0;struct Cyc_Core_Opt**vds=_tmpD1;struct Cyc_Absyn_Exp**exp=_tmpD2;void**dtp=_tmpD3;
# 472
struct Cyc_Absyn_Exp*_tmpFA=*exp;struct Cyc_Absyn_Exp*e=_tmpFA;
{void*_tmpFB=p->r;void*_stmttmp8=_tmpFB;void*_tmpFC=_stmttmp8;switch(*((int*)_tmpFC)){case 1U: _LL70: _LL71:
 goto _LL73;case 2U: _LL72: _LL73:
 goto _LL75;case 15U: _LL74: _LL75:
 goto _LL6F;default: _LL76: _LL77:
# 478
 if(!Cyc_Tcstmt_is_var_exp(e)){
struct Cyc_Absyn_Vardecl*_tmpFD=({struct _tuple2*_tmp1CF=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1CF,Cyc_Absyn_new_evar(0,0),0);});struct Cyc_Absyn_Vardecl*vd=_tmpFD;
struct Cyc_Absyn_Decl*_tmpFE=({struct Cyc_Absyn_Pat*_tmp1D3=({void*_tmp1D1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->tag=1U,_tmp100->f1=vd,({struct Cyc_Absyn_Pat*_tmp1D0=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_tmp100->f2=_tmp1D0;});_tmp100;});Cyc_Absyn_new_pat(_tmp1D1,e->loc);});struct Cyc_Absyn_Exp*_tmp1D2=e;Cyc_Absyn_let_decl(_tmp1D3,_tmp1D2,s0->loc);});struct Cyc_Absyn_Decl*d=_tmpFE;
# 483
({void*_tmp1D6=({struct Cyc_Absyn_Decl*_tmp1D5=d;struct Cyc_Absyn_Stmt*_tmp1D4=Cyc_Absyn_new_stmt(s0->r,s0->loc);Cyc_Absyn_decl_stmt(_tmp1D5,_tmp1D4,s0->loc);})->r;s0->r=_tmp1D6;});
({struct Cyc_Absyn_Exp*_tmp1D8=({void*_tmp1D7=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->tag=4U,_tmpFF->f1=vd;_tmpFF;});Cyc_Absyn_varb_exp(_tmp1D7,e->loc);});*exp=_tmp1D8;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL6F:;}
# 489
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 491
void*pat_type=(void*)_check_null(e->topt);
# 493
struct Cyc_Tcpat_TcPatResult _tmp101=Cyc_Tcpat_tcPat(te,p,& pat_type,e);struct Cyc_Tcpat_TcPatResult pat_res=_tmp101;
({struct Cyc_Core_Opt*_tmp1D9=({struct Cyc_Core_Opt*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->v=pat_res.patvars;_tmp102;});*vds=_tmp1D9;});{
void*_tmp103=(void*)_check_null(p->topt);void*pt=_tmp103;
void*_tmp104=(void*)_check_null(e->topt);void*et=_tmp104;
if(!Cyc_Unify_unify(pt,et)&& !({struct Cyc_RgnOrder_RgnPO*_tmp1DB=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1DA=e;Cyc_Tcutil_coerce_assign(_tmp1DB,_tmp1DA,pt);})){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp106=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E;_tmp16E.tag=0U,({
struct _fat_ptr _tmp1DC=({const char*_tmp10B="pattern type ";_tag_fat(_tmp10B,sizeof(char),14U);});_tmp16E.f1=_tmp1DC;});_tmp16E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp107=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16D;_tmp16D.tag=2U,_tmp16D.f1=(void*)pt;_tmp16D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp108=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16C;_tmp16C.tag=0U,({struct _fat_ptr _tmp1DD=({const char*_tmp10A=" does not match definition type ";_tag_fat(_tmp10A,sizeof(char),33U);});_tmp16C.f1=_tmp1DD;});_tmp16C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp109=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16B;_tmp16B.tag=2U,_tmp16B.f1=(void*)et;_tmp16B;});void*_tmp105[4U];_tmp105[0]=& _tmp106,_tmp105[1]=& _tmp107,_tmp105[2]=& _tmp108,_tmp105[3]=& _tmp109;({unsigned _tmp1DE=d->loc;Cyc_Warn_err2(_tmp1DE,_tag_fat(_tmp105,sizeof(void*),4U));});});
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();}else{
# 504
Cyc_Tcpat_check_pat_regions(te,p,pat_res.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}}case 3U: _LL29: _tmpCF=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;_LL2A: {struct Cyc_List_List*vds=_tmpCF;
# 510
void*_tmp10C=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp10C;
for(0;vds != 0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=curr_bl;
({unsigned _tmp1E1=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1E0=te;struct Cyc_List_List*_tmp1DF=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1E1,_tmp1E0,_tmp1DF,& Cyc_Tcutil_tmk,1,1,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);});}
# 515
Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4U: _LL2B: _tmpCC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;_tmpCD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpC6)->f2;_tmpCE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpC6)->f3;_LL2C: {struct Cyc_Absyn_Tvar*tv=_tmpCC;struct Cyc_Absyn_Vardecl*vd=_tmpCD;struct Cyc_Absyn_Exp*open_exp_opt=_tmpCE;
# 521
({void*_tmp1E5=({void*_tmp1E4=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1E3=Cyc_Absyn_new_stmt(s->r,s->loc);_tmp10D->f1=_tmp1E3;}),({struct Cyc_Absyn_Stmt*_tmp1E2=Cyc_Absyn_skip_stmt(s->loc);_tmp10D->f2=_tmp1E2;});_tmp10D;});Cyc_Absyn_new_stmt(_tmp1E4,s->loc);})->r;s->r=_tmp1E5;});{
# 523
struct Cyc_Tcenv_Tenv*_tmp10E=te;struct Cyc_Tcenv_Tenv*te2=_tmp10E;
struct Cyc_Absyn_Vardecl*_tmp10F=vd;void**_tmp111;void**_tmp110;_LL79: _tmp110=(void**)& _tmp10F->type;_tmp111=(void**)& _tmp10F->rgn;_LL7A: {void**t=_tmp110;void**rgn_ptr=_tmp111;
void*_tmp112=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp112;
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 531
struct _tuple2*drname=({struct _tuple2*_tmp128=_cycalloc(sizeof(*_tmp128));((_tmp128->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp1EA=({struct _fat_ptr*_tmp123[1U];({struct _fat_ptr*_tmp1E9=({struct _fat_ptr*_tmp125=_cycalloc(sizeof(*_tmp125));({struct _fat_ptr _tmp1E8=({const char*_tmp124="Core";_tag_fat(_tmp124,sizeof(char),5U);});*_tmp125=_tmp1E8;});_tmp125;});_tmp123[0]=_tmp1E9;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp123,sizeof(struct _fat_ptr*),1U));});((_tmp128->f1).Abs_n).val=_tmp1EA;}),({
struct _fat_ptr*_tmp1E7=({struct _fat_ptr*_tmp127=_cycalloc(sizeof(*_tmp127));({struct _fat_ptr _tmp1E6=({const char*_tmp126="DynamicRegion";_tag_fat(_tmp126,sizeof(char),14U);});*_tmp127=_tmp1E6;});_tmp127;});_tmp128->f2=_tmp1E7;});_tmp128;});
void*_tmp113=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->v=& Cyc_Tcutil_trk;_tmp122;}),0);void*outer_rgn=_tmp113;
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->v=& Cyc_Tcutil_rk;_tmp114;}),0);{
void*_tmp115=({
union Cyc_Absyn_AggrInfo _tmp1EB=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);Cyc_Absyn_aggr_type(_tmp1EB,({void*_tmp121[1U];_tmp121[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp121,sizeof(void*),1U));}));});
# 535
void*dr_type=_tmp115;
# 537
void*_tmp116=({void*_tmp1EE=dr_type;void*_tmp1ED=outer_rgn;struct Cyc_Absyn_Tqual _tmp1EC=
Cyc_Absyn_empty_tqual(0U);
# 537
Cyc_Absyn_at_type(_tmp1EE,_tmp1ED,_tmp1EC,Cyc_Absyn_false_type);});void*exp_type=_tmp116;
# 539
({unsigned _tmp1F1=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1F0=te;struct Cyc_List_List*_tmp1EF=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp1F1,_tmp1F0,_tmp1EF,& Cyc_Tcutil_tmk,1,0,exp_type);});{
void*_tmp117=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);void*key_typ=_tmp117;
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_RgnOrder_RgnPO*_tmp1F3=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp1F2=open_exp_opt;Cyc_Tcutil_coerce_assign(_tmp1F3,_tmp1F2,exp_type);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp119=({struct Cyc_Warn_String_Warn_Warg_struct _tmp172;_tmp172.tag=0U,({struct _fat_ptr _tmp1F4=({const char*_tmp11E="expected ";_tag_fat(_tmp11E,sizeof(char),10U);});_tmp172.f1=_tmp1F4;});_tmp172;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp11A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp171;_tmp171.tag=2U,_tmp171.f1=(void*)exp_type;_tmp171;});struct Cyc_Warn_String_Warn_Warg_struct _tmp11B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp170;_tmp170.tag=0U,({struct _fat_ptr _tmp1F5=({const char*_tmp11D=" but found ";_tag_fat(_tmp11D,sizeof(char),12U);});_tmp170.f1=_tmp1F5;});_tmp170;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp11C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16F;_tmp16F.tag=2U,_tmp16F.f1=(void*)key_typ;_tmp16F;});void*_tmp118[4U];_tmp118[0]=& _tmp119,_tmp118[1]=& _tmp11A,_tmp118[2]=& _tmp11B,_tmp118[3]=& _tmp11C;({unsigned _tmp1F6=s0->loc;Cyc_Warn_err2(_tmp1F6,_tag_fat(_tmp118,sizeof(void*),4U));});});
if(!Cyc_Unify_unify(outer_rgn,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Unify_unify(outer_rgn,Cyc_Absyn_refcnt_rgn_type))
({void*_tmp11F=0U;({unsigned _tmp1F8=s0->loc;struct _fat_ptr _tmp1F7=({const char*_tmp120="open is only allowed on unique or reference-counted keys";_tag_fat(_tmp120,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp1F8,_tmp1F7,_tag_fat(_tmp11F,sizeof(void*),0U));});});
({void*_tmp1F9=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_tmp1F9;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1);}}}else{
# 550
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=2U,_tmp129->f1=tv;_tmp129;});
te2=({unsigned _tmp1FB=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FA=te2;Cyc_Tcenv_add_type_vars(_tmp1FB,_tmp1FA,({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->hd=tv,_tmp12A->tl=0;_tmp12A;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0);}
# 554
({unsigned _tmp1FE=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FD=te;struct Cyc_List_List*_tmp1FC=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp1FE,_tmp1FD,_tmp1FC,& Cyc_Tcutil_bk,1,0,*t);});
if(!({void*_tmp1FF=*t;Cyc_Unify_unify(_tmp1FF,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({void*_tmp12B=0U;({struct _fat_ptr _tmp200=({const char*_tmp12C="region stmt: type of region handle is wrong!";_tag_fat(_tmp12C,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp200,_tag_fat(_tmp12B,sizeof(void*),0U));});});
# 558
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1U: _LL2D: _tmpCB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;_LL2E: {struct Cyc_Absyn_Fndecl*fd=_tmpCB;
# 563
if((int)fd->sc != (int)Cyc_Absyn_Public)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp12E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp173;_tmp173.tag=0U,({struct _fat_ptr _tmp201=({const char*_tmp12F="bad storage class for inner function";_tag_fat(_tmp12F,sizeof(char),37U);});_tmp173.f1=_tmp201;});_tmp173;});void*_tmp12D[1U];_tmp12D[0]=& _tmp12E;({unsigned _tmp202=d->loc;Cyc_Warn_err2(_tmp202,_tag_fat(_tmp12D,sizeof(void*),1U));});});
# 566
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;(unsigned)atts;atts=atts->tl){
void*_tmp130=(void*)atts->hd;void*_stmttmp9=_tmp130;void*_tmp131=_stmttmp9;switch(*((int*)_tmp131)){case 7U: _LL7C: _LL7D:
 goto _LL7F;case 6U: _LL7E: _LL7F:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp133=({struct Cyc_Warn_String_Warn_Warg_struct _tmp176;_tmp176.tag=0U,({struct _fat_ptr _tmp203=({const char*_tmp137="bad attribute ";_tag_fat(_tmp137,sizeof(char),15U);});_tmp176.f1=_tmp203;});_tmp176;});struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp134=({struct Cyc_Warn_Attribute_Warn_Warg_struct _tmp175;_tmp175.tag=10U,_tmp175.f1=(void*)atts->hd;_tmp175;});struct Cyc_Warn_String_Warn_Warg_struct _tmp135=({struct Cyc_Warn_String_Warn_Warg_struct _tmp174;_tmp174.tag=0U,({struct _fat_ptr _tmp204=({const char*_tmp136=" for function";_tag_fat(_tmp136,sizeof(char),14U);});_tmp174.f1=_tmp204;});_tmp174;});void*_tmp132[3U];_tmp132[0]=& _tmp133,_tmp132[1]=& _tmp134,_tmp132[2]=& _tmp135;({unsigned _tmp205=d->loc;Cyc_Warn_err2(_tmp205,_tag_fat(_tmp132,sizeof(void*),3U));});});
goto _LL7B;default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}}
# 573
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_tmp206=Cyc_Tcutil_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp206;});{
void*_tmp138=Cyc_Tcenv_curr_rgn(te);void*curr_bl=_tmp138;
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->sc=fd->sc,_tmp139->name=fd->name,_tmp139->varloc=0U,({struct Cyc_Absyn_Tqual _tmp20B=Cyc_Absyn_const_tqual(0U);_tmp139->tq=_tmp20B;}),({
void*_tmp20A=({void*_tmp209=t;void*_tmp208=curr_bl;struct Cyc_Absyn_Tqual _tmp207=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp209,_tmp208,_tmp207,Cyc_Absyn_false_type);});_tmp139->type=_tmp20A;}),_tmp139->initializer=0,_tmp139->rgn=curr_bl,_tmp139->attributes=0,_tmp139->escapes=0;_tmp139;});
# 581
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_tmp20C=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_tmp20C;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9U: _LL2F: _tmpC9=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;_tmpCA=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpC6)->f2;_LL30: {struct _fat_ptr*n=_tmpC9;struct Cyc_List_List*tds=_tmpCA;
unimp_msg_part=({const char*_tmp13A="namespace";_tag_fat(_tmp13A,sizeof(char),10U);});goto _LL24;}case 10U: _LL31: _tmpC7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpC6)->f2;_LL32: {struct _tuple2*q=_tmpC7;struct Cyc_List_List*tds=_tmpC8;
unimp_msg_part=({const char*_tmp13B="using";_tag_fat(_tmp13B,sizeof(char),6U);});goto _LL24;}case 5U: _LL33: _LL34:
 unimp_msg_part=({const char*_tmp13C="type";_tag_fat(_tmp13C,sizeof(char),5U);});goto _LL24;case 6U: _LL35: _LL36:
 unimp_msg_part=({const char*_tmp13D="datatype";_tag_fat(_tmp13D,sizeof(char),9U);});goto _LL24;case 7U: _LL37: _LL38:
 unimp_msg_part=({const char*_tmp13E="enum";_tag_fat(_tmp13E,sizeof(char),5U);});goto _LL24;case 8U: _LL39: _LL3A:
 unimp_msg_part=({const char*_tmp13F="typedef";_tag_fat(_tmp13F,sizeof(char),8U);});goto _LL24;case 11U: _LL3B: _LL3C:
 unimp_msg_part=({const char*_tmp140="extern \"C\"";_tag_fat(_tmp140,sizeof(char),11U);});goto _LL24;case 12U: _LL3D: _LL3E:
 unimp_msg_part=({const char*_tmp141="extern \"C include\"";_tag_fat(_tmp141,sizeof(char),19U);});goto _LL24;case 13U: _LL3F: _LL40:
 unimp_msg_part=({const char*_tmp142="__cyclone_port_on__";_tag_fat(_tmp142,sizeof(char),20U);});goto _LL24;case 14U: _LL41: _LL42:
 unimp_msg_part=({const char*_tmp143="__cyclone_port_off__";_tag_fat(_tmp143,sizeof(char),21U);});goto _LL24;case 15U: _LL43: _LL44:
 unimp_msg_part=({const char*_tmp144="__tempest_on__";_tag_fat(_tmp144,sizeof(char),15U);});goto _LL24;default: _LL45: _LL46:
 unimp_msg_part=({const char*_tmp145="__tempest_off__";_tag_fat(_tmp145,sizeof(char),16U);});goto _LL24;}_LL24:;}
# 603
({struct Cyc_Warn_String_Warn_Warg_struct _tmp147=({struct Cyc_Warn_String_Warn_Warg_struct _tmp179;_tmp179.tag=0U,({struct _fat_ptr _tmp20D=({const char*_tmp14B="tcStmt: nested ";_tag_fat(_tmp14B,sizeof(char),16U);});_tmp179.f1=_tmp20D;});_tmp179;});struct Cyc_Warn_String_Warn_Warg_struct _tmp148=({struct Cyc_Warn_String_Warn_Warg_struct _tmp178;_tmp178.tag=0U,_tmp178.f1=unimp_msg_part;_tmp178;});struct Cyc_Warn_String_Warn_Warg_struct _tmp149=({struct Cyc_Warn_String_Warn_Warg_struct _tmp177;_tmp177.tag=0U,({struct _fat_ptr _tmp20E=({const char*_tmp14A=" declarations not allowed";_tag_fat(_tmp14A,sizeof(char),26U);});_tmp177.f1=_tmp20E;});_tmp177;});void*_tmp146[3U];_tmp146[0]=& _tmp147,_tmp146[1]=& _tmp148,_tmp146[2]=& _tmp149;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp146,sizeof(void*),3U));});}}_LL0:;}

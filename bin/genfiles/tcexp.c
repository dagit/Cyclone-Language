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
# 111 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 232 "cycboot.h"
struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 303
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 310
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 491
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 493
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 901
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 904
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 906
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 908
extern void*Cyc_Absyn_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned i);
# 911
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 915
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 917
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 934
void*Cyc_Absyn_exn_type();
# 942
extern void*Cyc_Absyn_fat_bound_type;
# 944
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 946
void*Cyc_Absyn_bounds_one();
# 953
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 956
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 958
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 962
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
# 970
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 974
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 978
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp();
# 1048
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1059
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1112
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1114
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1117
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1119
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 27 "unify.h"
void Cyc_Unify_explain_failure();
# 29
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 41
int Cyc_Tcutil_is_arithmetic_type(void*);
# 44
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 52
int Cyc_Tcutil_is_bound_one(void*);
# 54
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 57
int Cyc_Tcutil_is_bits_only_type(void*);
# 64
void*Cyc_Tcutil_pointer_elt_type(void*);
# 66
void*Cyc_Tcutil_pointer_region(void*);
# 69
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 77
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 79
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 87
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 94
void*Cyc_Tcutil_copy_type(void*);
# 97
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 113
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*,void*,struct Cyc_List_List*);
# 117
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 119
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 124
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 127
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 147
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 154
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 163
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 166
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 179
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 188
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 202
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 207
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 218
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 220
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 222
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 233
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 236
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 241
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 246
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple14{int f1;void*f2;};
# 250
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 269
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 283
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 294
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 297
int Cyc_Tcutil_zeroable_type(void*);
# 301
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 304
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 306
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);struct _tuple15{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 75 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 82
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 88
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 92
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 114
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 119
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 123
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
# 137
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 142
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 144
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*eff);
# 149
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*po);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 33
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int isCproto,unsigned);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part);
# 49 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr msg,struct _fat_ptr ap){
# 53
({void*_tmp0=0U;({unsigned _tmp55B=loc;struct _fat_ptr _tmp55A=(struct _fat_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp55B,_tmp55A,_tag_fat(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 57
return*topt;}}
# 66
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 70
if(topt == 0){
# 72
({void*_tmp55C=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=26U,_tmp1->f1=des;_tmp1;});e->r=_tmp55C;});
return;}{
# 75
void*t=*topt;
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;void*_tmpE;struct Cyc_Absyn_Tqual _tmpD;union Cyc_Absyn_AggrInfo _tmpC;switch(*((int*)_tmp3)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->tag == 20U){_LL1: _tmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->f1;_LL2:
# 78
{union Cyc_Absyn_AggrInfo _tmp4=_tmpC;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp4.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp5=0U;({struct _fat_ptr _tmp55D=({const char*_tmp6="struct type not properly set";_tag_fat(_tmp6,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp55D,_tag_fat(_tmp5,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp4.KnownAggr).val;_LLD:
({void*_tmp55E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=29U,_tmp7->f1=_tmp8->name,_tmp7->f2=0,_tmp7->f3=des,_tmp7->f4=_tmp8;_tmp7;});e->r=_tmp55E;});}_LL9:;}
# 82
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4:
({void*_tmp55F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=26U,_tmp9->f1=des;_tmp9;});e->r=_tmp55F;});goto _LL0;case 7U: _LL5: _LL6:
({void*_tmp560=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=30U,_tmpA->f1=t,_tmpA->f2=des;_tmpA;});e->r=_tmp560;});goto _LL0;default: _LL7: _LL8:
({void*_tmp561=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp561;});goto _LL0;}_LL0:;};}
# 92
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 96
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 102
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_tmp10=_tmpF;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->f2 == 0){_LL1: _LL2:
# 105
 if(Cyc_Tc_aggressive_warn)({void*_tmp11=0U;({unsigned _tmp563=e->loc;struct _fat_ptr _tmp562=({const char*_tmp12="assignment in test";_tag_fat(_tmp12,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp563,_tmp562,_tag_fat(_tmp11,sizeof(void*),0U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 112
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp4D7;_tmp4D7.tag=0U,_tmp4D7.f1=(struct _fat_ptr)((struct _fat_ptr)msg_part);_tmp4D7;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp4D6;_tmp4D6.tag=0U,({
struct _fat_ptr _tmp564=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4D6.f1=_tmp564;});_tmp4D6;});void*_tmp13[2U];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16;({unsigned _tmp566=e->loc;struct _fat_ptr _tmp565=({const char*_tmp14="test of %s has type %s instead of integral or pointer type";_tag_fat(_tmp14,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp566,_tmp565,_tag_fat(_tmp13,sizeof(void*),2U));});});}
# 138 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 143
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp17=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp18=_tmp17;struct _fat_ptr _tmp4B;struct _fat_ptr _tmp4A;enum Cyc_Absyn_Sign _tmp49;int _tmp48;int _tmp47;enum Cyc_Absyn_Sign _tmp46;enum Cyc_Absyn_Sign _tmp45;switch((_tmp18.Wstring_c).tag){case 2U: switch(((_tmp18.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_type;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_type;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_type;goto _LL0;}case 3U: _LL7: _LL8:
 t=Cyc_Absyn_wchar_type();goto _LL0;case 4U: _LL9: _tmp45=((_tmp18.Short_c).val).f1;_LLA:
# 153
 t=(int)_tmp45 == (int)1U?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;goto _LL0;case 6U: _LLB: _tmp46=((_tmp18.LongLong_c).val).f1;_LLC:
# 155
 t=(int)_tmp46 == (int)1U?Cyc_Absyn_ulonglong_type: Cyc_Absyn_slonglong_type;goto _LL0;case 7U: _LLD: _tmp47=((_tmp18.Float_c).val).f2;_LLE:
# 157
 if(topt == 0)t=Cyc_Absyn_gen_float_type((unsigned)_tmp47);else{
# 161
void*_tmp19=Cyc_Tcutil_compress(*topt);void*_tmp1A=_tmp19;int _tmp1B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL18: _tmp1B=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->f1;_LL19:
 t=Cyc_Absyn_gen_float_type((unsigned)_tmp1B);goto _LL17;}else{goto _LL1A;}}else{_LL1A: _LL1B:
# 165
 t=Cyc_Absyn_gen_float_type((unsigned)_tmp47);
goto _LL17;}_LL17:;}
# 169
goto _LL0;case 5U: _LLF: _tmp49=((_tmp18.Int_c).val).f1;_tmp48=((_tmp18.Int_c).val).f2;_LL10:
# 171
 if(topt == 0)
t=(int)_tmp49 == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;else{
# 178
void*_tmp1C=Cyc_Tcutil_compress(*topt);void*_tmp1D=_tmp1C;void*_tmp30;struct Cyc_Absyn_Tqual _tmp2F;void*_tmp2E;void*_tmp2D;void*_tmp2C;void*_tmp2B;void*_tmp2A;enum Cyc_Absyn_Sign _tmp29;enum Cyc_Absyn_Sign _tmp28;enum Cyc_Absyn_Sign _tmp27;enum Cyc_Absyn_Sign _tmp26;switch(*((int*)_tmp1D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1D: _tmp26=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL1E:
# 180
{enum Cyc_Absyn_Sign _tmp1E=_tmp26;switch(_tmp1E){case Cyc_Absyn_Unsigned: _LL2C: _LL2D:
 t=Cyc_Absyn_uchar_type;goto _LL2B;case Cyc_Absyn_Signed: _LL2E: _LL2F:
 t=Cyc_Absyn_schar_type;goto _LL2B;default: _LL30: _LL31:
 t=Cyc_Absyn_char_type;goto _LL2B;}_LL2B:;}
# 185
({union Cyc_Absyn_Cnst _tmp567=Cyc_Absyn_Char_c(_tmp26,(char)_tmp48);*c=_tmp567;});
goto _LL1C;case Cyc_Absyn_Short_sz: _LL1F: _tmp27=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL20:
# 188
 t=(int)_tmp27 == (int)1U?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;
({union Cyc_Absyn_Cnst _tmp568=Cyc_Absyn_Short_c(_tmp27,(short)_tmp48);*c=_tmp568;});
goto _LL1C;case Cyc_Absyn_Int_sz: _LL21: _tmp28=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL22:
# 192
 t=(int)_tmp28 == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 195
({union Cyc_Absyn_Cnst _tmp569=Cyc_Absyn_Int_c(_tmp28,_tmp48);*c=_tmp569;});
goto _LL1C;case Cyc_Absyn_Long_sz: _LL23: _tmp29=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL24:
# 198
 t=(int)_tmp29 == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 201
({union Cyc_Absyn_Cnst _tmp56A=Cyc_Absyn_Int_c(_tmp29,_tmp48);*c=_tmp56A;});
goto _LL1C;default: goto _LL29;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f2 != 0){_LL27: _tmp2A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f2)->hd;_LL28: {
# 215
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->tag=9U,({struct Cyc_Absyn_Exp*_tmp56B=Cyc_Absyn_uint_exp((unsigned)_tmp48,0U);_tmp25->f1=_tmp56B;});_tmp25;});
# 222
t=Cyc_Absyn_tag_type((void*)_tmp24);
goto _LL1C;}}else{goto _LL29;}default: goto _LL29;}case 3U: _LL25: _tmp30=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).elt_type;_tmp2F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).elt_tq;_tmp2E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).rgn;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).nullable;_tmp2C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).bounds;_tmp2B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).zero_term;if(_tmp48 == 0){_LL26: {
# 205
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(Cyc_Tcutil_force_type2bool(1,_tmp2D))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp4D8;_tmp4D8.tag=0U,({
struct _fat_ptr _tmp56C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D8.f1=_tmp56C;});_tmp4D8;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({unsigned _tmp56E=e->loc;struct _fat_ptr _tmp56D=({const char*_tmp20="Used NULL when expecting a non-NULL value of type %s";_tag_fat(_tmp20,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp56E,_tmp56D,_tag_fat(_tmp1F,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp22=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=3U,(_tmp23->f1).elt_type=_tmp30,(_tmp23->f1).elt_tq=_tmp2F,
((_tmp23->f1).ptr_atts).rgn=_tmp2E,((_tmp23->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp23->f1).ptr_atts).bounds=_tmp2C,((_tmp23->f1).ptr_atts).zero_term=_tmp2B,((_tmp23->f1).ptr_atts).ptrloc=0;_tmp23;});
goto _LL1C;};}}else{goto _LL29;}default: _LL29: _LL2A:
# 225
 t=(int)_tmp49 == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
goto _LL1C;}_LL1C:;}
# 228
goto _LL0;case 8U: _LL11: _tmp4A=(_tmp18.String_c).val;_LL12:
# 230
 string_numelts=(int)_get_fat_size(_tmp4A,sizeof(char));
_tmp4B=_tmp4A;goto _LL14;case 9U: _LL13: _tmp4B=(_tmp18.Wstring_c).val;_LL14:
# 233
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp4B);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 237
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp56F=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp56F,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 242
void*_tmp31=Cyc_Absyn_thin_bounds_exp(elen);
t=({void*_tmp573=string_elt_typ;void*_tmp572=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp571=Cyc_Absyn_const_tqual(0U);void*_tmp570=_tmp31;Cyc_Absyn_atb_type(_tmp573,_tmp572,_tmp571,_tmp570,Cyc_Absyn_true_type);});
# 245
if(topt != 0){
void*_tmp32=Cyc_Tcutil_compress(*topt);void*_tmp33=_tmp32;struct Cyc_Absyn_Tqual _tmp34;switch(*((int*)_tmp33)){case 4U: _LL33: _tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33)->f1).tq;_LL34:
# 251
 return({void*_tmp576=string_elt_typ;struct Cyc_Absyn_Tqual _tmp575=_tmp34;struct Cyc_Absyn_Exp*_tmp574=elen;Cyc_Absyn_array_type(_tmp576,_tmp575,_tmp574,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});case 3U: _LL35: _LL36:
# 254
 if(!Cyc_Unify_unify(*topt,t)&&({
struct Cyc_RgnOrder_RgnPO*_tmp579=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp578=loc;void*_tmp577=t;Cyc_Tcutil_silent_castable(_tmp579,_tmp578,_tmp577,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 261
t=({void*_tmp57D=string_elt_typ;void*_tmp57C=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp57B=
Cyc_Absyn_const_tqual(0U);
# 261
void*_tmp57A=_tmp31;Cyc_Absyn_atb_type(_tmp57D,_tmp57C,_tmp57B,_tmp57A,Cyc_Absyn_true_type);});
# 263
if(!Cyc_Unify_unify(*topt,t)&&({
struct Cyc_RgnOrder_RgnPO*_tmp580=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp57F=loc;void*_tmp57E=t;Cyc_Tcutil_silent_castable(_tmp580,_tmp57F,_tmp57E,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 270
goto _LL32;default: _LL37: _LL38:
 goto _LL32;}_LL32:;}
# 274
return t;};};default: _LL15: _LL16:
# 276
 if(topt != 0){
void*_tmp35=Cyc_Tcutil_compress(*topt);void*_tmp36=_tmp35;void*_tmp40;struct Cyc_Absyn_Tqual _tmp3F;void*_tmp3E;void*_tmp3D;void*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->tag == 3U){_LL3A: _tmp40=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).elt_type;_tmp3F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).elt_tq;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).rgn;_tmp3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).nullable;_tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).bounds;_tmp3B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).zero_term;_LL3B:
# 280
 if(Cyc_Tcutil_force_type2bool(1,_tmp3D))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0U,({
struct _fat_ptr _tmp581=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D9.f1=_tmp581;});_tmp4D9;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({unsigned _tmp583=e->loc;struct _fat_ptr _tmp582=({const char*_tmp38="Used NULL when expecting a non-NULL value of type %s";_tag_fat(_tmp38,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp583,_tmp582,_tag_fat(_tmp37,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->tag=3U,(_tmp3A->f1).elt_type=_tmp40,(_tmp3A->f1).elt_tq=_tmp3F,((_tmp3A->f1).ptr_atts).rgn=_tmp3E,((_tmp3A->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp3A->f1).ptr_atts).bounds=_tmp3C,((_tmp3A->f1).ptr_atts).zero_term=_tmp3B,((_tmp3A->f1).ptr_atts).ptrloc=0;_tmp3A;});}else{_LL3C: _LL3D:
# 285
 goto _LL39;}_LL39:;}{
# 288
struct Cyc_List_List*_tmp41=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=3U,({void*_tmp588=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=_tmp41;_tmp42;}));(_tmp44->f1).elt_type=_tmp588;}),({
struct Cyc_Absyn_Tqual _tmp587=Cyc_Absyn_empty_tqual(0U);(_tmp44->f1).elt_tq=_tmp587;}),
({void*_tmp586=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->v=_tmp41;_tmp43;}));((_tmp44->f1).ptr_atts).rgn=_tmp586;}),((_tmp44->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp585=Cyc_Tcutil_any_bounds(_tmp41);((_tmp44->f1).ptr_atts).bounds=_tmp585;}),({
void*_tmp584=Cyc_Tcutil_any_bool(_tmp41);((_tmp44->f1).ptr_atts).zero_term=_tmp584;}),((_tmp44->f1).ptr_atts).ptrloc=0;_tmp44;});
# 295
goto _LL0;};}_LL0:;}
# 297
return t;}
# 301
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 306
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp4C=*((void**)_check_null(b));void*_tmp4D=_tmp4C;struct Cyc_Absyn_Vardecl*_tmp59;struct Cyc_Absyn_Vardecl*_tmp58;struct Cyc_Absyn_Vardecl*_tmp57;struct Cyc_Absyn_Fndecl*_tmp56;struct Cyc_Absyn_Vardecl*_tmp55;struct _tuple1*_tmp54;switch(*((int*)_tmp4D)){case 0U: _LL1: _tmp54=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL2:
({void*_tmp4E=0U;({struct _fat_ptr _tmp589=({const char*_tmp4F="unresolved binding in tcVar";_tag_fat(_tmp4F,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp589,_tag_fat(_tmp4E,sizeof(void*),0U));});});case 1U: _LL3: _tmp55=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL4:
# 312
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp55->name,1);
return _tmp55->type;case 2U: _LL5: _tmp56=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL6:
# 319
 if(_tmp56->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp56->name,1);
return Cyc_Tcutil_fndecl2type(_tmp56);case 5U: _LL7: _tmp57=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL8:
 _tmp58=_tmp57;goto _LLA;case 4U: _LL9: _tmp58=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLA:
 _tmp59=_tmp58;goto _LLC;default: _LLB: _tmp59=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLC:
# 325
 if(te->allow_valueof){
void*_tmp50=Cyc_Tcutil_compress(_tmp59->type);void*_tmp51=_tmp50;void*_tmp53;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f2 != 0){_LLE: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f2)->hd;_LLF:
# 328
({void*_tmp58A=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=39U,_tmp52->f1=_tmp53;_tmp52;});e->r=_tmp58A;});
goto _LLD;}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 333
return _tmp59->type;}_LL0:;}
# 337
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 345
struct Cyc_List_List*desc_types;
{void*_tmp5A=fmt->r;void*_tmp5B=_tmp5A;struct _fat_ptr _tmp60;struct _fat_ptr _tmp5F;switch(*((int*)_tmp5B)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).tag == 8){_LL1: _tmp5F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).val;_LL2:
 _tmp60=_tmp5F;goto _LL4;}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp60=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).val;_LL4:
# 349
 desc_types=type_getter(te,(struct _fat_ptr)_tmp60,isCproto,fmt->loc);goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 353
 if(opt_args != 0){
struct Cyc_List_List*_tmp5C=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp5C != 0;_tmp5C=_tmp5C->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp5C->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp5C->hd)->topt))&& !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp5C->hd))
({void*_tmp5D=0U;({unsigned _tmp58C=((struct Cyc_Absyn_Exp*)_tmp5C->hd)->loc;struct _fat_ptr _tmp58B=({const char*_tmp5E="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp5E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp58C,_tmp58B,_tag_fat(_tmp5D,sizeof(void*),0U));});});}}
# 362
return;}_LL0:;}
# 364
if(opt_args != 0){
struct Cyc_List_List*_tmp61=(struct Cyc_List_List*)opt_args->v;
# 367
for(0;desc_types != 0 && _tmp61 != 0;(
desc_types=desc_types->tl,_tmp61=_tmp61->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp61->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp58E=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp58D=e;Cyc_Tcutil_coerce_arg(_tmp58E,_tmp58D,t,& alias_coercion);})){
({struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp4DB;_tmp4DB.tag=0U,({
struct _fat_ptr _tmp58F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4DB.f1=_tmp58F;});_tmp4DB;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0U,({struct _fat_ptr _tmp590=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4DA.f1=_tmp590;});_tmp4DA;});void*_tmp62[2U];_tmp62[0]=& _tmp64,_tmp62[1]=& _tmp65;({unsigned _tmp592=e->loc;struct _fat_ptr _tmp591=({const char*_tmp63="descriptor has type %s but argument has type %s";_tag_fat(_tmp63,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp592,_tmp591,_tag_fat(_tmp62,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 378
if(alias_coercion)
({struct Cyc_List_List*_tmp593=({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->hd=(void*)arg_cnt,_tmp66->tl=*alias_arg_exps;_tmp66;});*alias_arg_exps=_tmp593;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp67=0U;({unsigned _tmp595=((struct Cyc_Absyn_Exp*)_tmp61->hd)->loc;struct _fat_ptr _tmp594=({const char*_tmp68="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp68,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp595,_tmp594,_tag_fat(_tmp67,sizeof(void*),0U));});});}
# 385
if(desc_types != 0)
({void*_tmp69=0U;({unsigned _tmp597=fmt->loc;struct _fat_ptr _tmp596=({const char*_tmp6A="too few arguments";_tag_fat(_tmp6A,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp597,_tmp596,_tag_fat(_tmp69,sizeof(void*),0U));});});
if(_tmp61 != 0){
({void*_tmp6B=0U;({unsigned _tmp599=((struct Cyc_Absyn_Exp*)_tmp61->hd)->loc;struct _fat_ptr _tmp598=({const char*_tmp6C="too many arguments";_tag_fat(_tmp6C,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp599,_tmp598,_tag_fat(_tmp6B,sizeof(void*),0U));});});
# 390
for(0;_tmp61 != 0;_tmp61=_tmp61->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp61->hd);}}}}
# 395
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 397
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6D=p;switch(_tmp6D){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 401
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp4DC;_tmp4DC.tag=0U,({struct _fat_ptr _tmp59A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4DC.f1=_tmp59A;});_tmp4DC;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({unsigned _tmp59C=loc;struct _fat_ptr _tmp59B=({const char*_tmp6F="expecting numeric type but found %s";_tag_fat(_tmp6F,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp59C,_tmp59B,_tag_fat(_tmp6E,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 405
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp4DD;_tmp4DD.tag=0U,({struct _fat_ptr _tmp59D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4DD.f1=_tmp59D;});_tmp4DD;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({unsigned _tmp59F=loc;struct _fat_ptr _tmp59E=({const char*_tmp72="expecting integral or * type but found %s";_tag_fat(_tmp72,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp59F,_tmp59E,_tag_fat(_tmp71,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 410
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp4DE;_tmp4DE.tag=0U,({struct _fat_ptr _tmp5A0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4DE.f1=_tmp5A0;});_tmp4DE;});void*_tmp74[1U];_tmp74[0]=& _tmp76;({unsigned _tmp5A2=loc;struct _fat_ptr _tmp5A1=({const char*_tmp75="expecting integral type but found %s";_tag_fat(_tmp75,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp5A2,_tmp5A1,_tag_fat(_tmp74,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 414
{void*_tmp77=t;void*_tmp7F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp7F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {
# 416
struct Cyc_Absyn_Exp*_tmp78=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp7F);
if(_tmp78 != 0){
struct Cyc_Absyn_Exp*_tmp79=_tmp78;
if(!Cyc_Evexp_c_can_eval(_tmp79)&& !((unsigned)Cyc_Tcenv_allow_valueof))
({void*_tmp7A=0U;({unsigned _tmp5A4=loc;struct _fat_ptr _tmp5A3=({const char*_tmp7B="cannot apply numelts to a pointer with abstract bounds";_tag_fat(_tmp7B,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp5A4,_tmp5A3,_tag_fat(_tmp7A,sizeof(void*),0U));});});}
# 422
goto _LLD;}}else{_LL10: _LL11:
# 424
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=0U,({struct _fat_ptr _tmp5A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4DF.f1=_tmp5A5;});_tmp4DF;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned _tmp5A7=loc;struct _fat_ptr _tmp5A6=({const char*_tmp7D="numelts requires pointer type, not %s";_tag_fat(_tmp7D,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp5A7,_tmp5A6,_tag_fat(_tmp7C,sizeof(void*),1U));});});}_LLD:;}
# 426
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({void*_tmp80=0U;({struct _fat_ptr _tmp5A8=({const char*_tmp81="Non-unary primop";_tag_fat(_tmp81,sizeof(char),17U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp5A8,_tag_fat(_tmp80,sizeof(void*),0U));});});}_LL0:;}
# 433
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp82=(void*)_check_null(e1->topt);
void*_tmp83=(void*)_check_null(e2->topt);
void*_tmp84=Cyc_Tcutil_max_arithmetic_type(_tmp82,_tmp83);
if(!Cyc_Unify_unify(_tmp82,_tmp84))Cyc_Tcutil_unchecked_cast(e1,_tmp84,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(_tmp83,_tmp84))Cyc_Tcutil_unchecked_cast(e2,_tmp84,Cyc_Absyn_No_coercion);
return _tmp84;}
# 443
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 446
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=0U,({struct _fat_ptr _tmp5A9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4E0.f1=_tmp5A9;});_tmp4E0;});void*_tmp85[1U];_tmp85[0]=& _tmp87;({unsigned _tmp5AB=e1->loc;struct _fat_ptr _tmp5AA=({const char*_tmp86="type %s cannot be used here";_tag_fat(_tmp86,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5AB,_tmp5AA,_tag_fat(_tmp85,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 450
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp4E1;_tmp4E1.tag=0U,({struct _fat_ptr _tmp5AC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4E1.f1=_tmp5AC;});_tmp4E1;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({unsigned _tmp5AE=e2->loc;struct _fat_ptr _tmp5AD=({const char*_tmp89="type %s cannot be used here";_tag_fat(_tmp89,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5AE,_tmp5AD,_tag_fat(_tmp88,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 454
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 457
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp8B=t1;void*_tmpA2;struct Cyc_Absyn_Tqual _tmpA1;void*_tmpA0;void*_tmp9F;void*_tmp9E;void*_tmp9D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->tag == 3U){_LL1: _tmpA2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_type;_tmpA1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_tq;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).rgn;_tmp9F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).nullable;_tmp9E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).bounds;_tmp9D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).zero_term;_LL2:
# 462
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmpA2),& Cyc_Tcutil_tmk))
({void*_tmp8C=0U;({unsigned _tmp5B0=e1->loc;struct _fat_ptr _tmp5AF=({const char*_tmp8D="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp8D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5B0,_tmp5AF,_tag_fat(_tmp8C,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp8E=0U;({unsigned _tmp5B2=e1->loc;struct _fat_ptr _tmp5B1=({const char*_tmp8F="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp8F,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5B2,_tmp5B1,_tag_fat(_tmp8E,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp4E2;_tmp4E2.tag=0U,({struct _fat_ptr _tmp5B3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E2.f1=_tmp5B3;});_tmp4E2;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({unsigned _tmp5B5=e2->loc;struct _fat_ptr _tmp5B4=({const char*_tmp91="expecting int but found %s";_tag_fat(_tmp91,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5B5,_tmp5B4,_tag_fat(_tmp90,sizeof(void*),1U));});});{
struct Cyc_Absyn_Exp*_tmp93=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp9E);
if(_tmp93 == 0)return t1;{
struct Cyc_Absyn_Exp*_tmp94=_tmp93;
# 473
if(Cyc_Tcutil_force_type2bool(0,_tmp9D)){
struct _tuple15 _tmp95=Cyc_Evexp_eval_const_uint_exp(_tmp94);struct _tuple15 _tmp96=_tmp95;unsigned _tmp9A;int _tmp99;_LL6: _tmp9A=_tmp96.f1;_tmp99=_tmp96.f2;_LL7:;
if(_tmp99 && _tmp9A == (unsigned)1)
({void*_tmp97=0U;({unsigned _tmp5B7=e1->loc;struct _fat_ptr _tmp5B6=({const char*_tmp98="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmp98,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5B7,_tmp5B6,_tag_fat(_tmp97,sizeof(void*),0U));});});}{
# 484
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9B=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=3U,(_tmp9C->f1).elt_type=_tmpA2,(_tmp9C->f1).elt_tq=_tmpA1,
((_tmp9C->f1).ptr_atts).rgn=_tmpA0,((_tmp9C->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp9C->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmp9C->f1).ptr_atts).zero_term=_tmp9D,((_tmp9C->f1).ptr_atts).ptrloc=0;_tmp9C;});
# 488
Cyc_Tcutil_unchecked_cast(e1,(void*)_tmp9B,Cyc_Absyn_Other_coercion);
return(void*)_tmp9B;};};};}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 495
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp4E4;_tmp4E4.tag=0U,({
# 505
struct _fat_ptr _tmp5B8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E4.f1=_tmp5B8;});_tmp4E4;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp4E3;_tmp4E3.tag=0U,({struct _fat_ptr _tmp5B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E3.f1=_tmp5B9;});_tmp4E3;});void*_tmpA3[2U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6;({unsigned _tmp5BB=e1->loc;struct _fat_ptr _tmp5BA=({const char*_tmpA4="pointer arithmetic on values of different types (%s != %s)";_tag_fat(_tmpA4,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5BB,_tmp5BA,_tag_fat(_tmpA3,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 508
return Cyc_Absyn_sint_type;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp5BC=t1_elt;Cyc_Unify_unify(_tmp5BC,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp4E6;_tmp4E6.tag=0U,({
struct _fat_ptr _tmp5BD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E6.f1=_tmp5BD;});_tmp4E6;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp4E5;_tmp4E5.tag=0U,({struct _fat_ptr _tmp5BE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E5.f1=_tmp5BE;});_tmp4E5;});void*_tmpA7[2U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({unsigned _tmp5C0=e1->loc;struct _fat_ptr _tmp5BF=({const char*_tmpA8="pointer arithmetic on values of different types (%s != %s)";_tag_fat(_tmpA8,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5C0,_tmp5BF,_tag_fat(_tmpA7,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 516
({void*_tmpAB=0U;({unsigned _tmp5C2=e1->loc;struct _fat_ptr _tmp5C1=({const char*_tmpAC="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpAC,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5C2,_tmp5C1,_tag_fat(_tmpAB,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp5C6=e1;Cyc_Tcutil_unchecked_cast(_tmp5C6,({void*_tmp5C5=t1_elt;void*_tmp5C4=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5C3=
Cyc_Absyn_empty_tqual(0U);
# 517
Cyc_Absyn_star_type(_tmp5C5,_tmp5C4,_tmp5C3,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 520
return Cyc_Absyn_sint_type;}else{
# 522
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpAD=0U;({unsigned _tmp5C8=e1->loc;struct _fat_ptr _tmp5C7=({const char*_tmpAE="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpAE,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5C8,_tmp5C7,_tag_fat(_tmpAD,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpAF=0U;({unsigned _tmp5CA=e1->loc;struct _fat_ptr _tmp5C9=({const char*_tmpB0="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpB0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5CA,_tmp5C9,_tag_fat(_tmpAF,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpB3=({struct Cyc_String_pa_PrintArg_struct _tmp4E8;_tmp4E8.tag=0U,({
struct _fat_ptr _tmp5CB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E8.f1=_tmp5CB;});_tmp4E8;});struct Cyc_String_pa_PrintArg_struct _tmpB4=({struct Cyc_String_pa_PrintArg_struct _tmp4E7;_tmp4E7.tag=0U,({struct _fat_ptr _tmp5CC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E7.f1=_tmp5CC;});_tmp4E7;});void*_tmpB1[2U];_tmpB1[0]=& _tmpB3,_tmpB1[1]=& _tmpB4;({unsigned _tmp5CE=e2->loc;struct _fat_ptr _tmp5CD=({const char*_tmpB2="expecting either %s or int but found %s";_tag_fat(_tmpB2,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5CE,_tmp5CD,_tag_fat(_tmpB1,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 531
return t1;}}}
# 535
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp5CF=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp5CF,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({void*_tmpB5=0U;({unsigned _tmp5D1=e1->loc;struct _fat_ptr _tmp5D0=({const char*_tmpB6="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpB6,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5D1,_tmp5D0,_tag_fat(_tmpB5,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp5D5=e2;Cyc_Tcutil_unchecked_cast(_tmp5D5,({void*_tmp5D4=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5D3=
Cyc_Tcutil_pointer_region(t2);
# 540
struct Cyc_Absyn_Tqual _tmp5D2=
# 542
Cyc_Absyn_empty_tqual(0U);
# 540
Cyc_Absyn_star_type(_tmp5D4,_tmp5D3,_tmp5D2,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 545
({void*_tmpB7=0U;({unsigned _tmp5D7=e1->loc;struct _fat_ptr _tmp5D6=({const char*_tmpB8="thin pointer subtraction!";_tag_fat(_tmpB8,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5D7,_tmp5D6,_tag_fat(_tmpB7,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 548
({void*_tmpB9=0U;({unsigned _tmp5D9=e1->loc;struct _fat_ptr _tmp5D8=({const char*_tmpBA="coercing thin pointer to integer to support subtraction";_tag_fat(_tmpBA,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5D9,_tmp5D8,_tag_fat(_tmpB9,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 551
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpBB=0U;({unsigned _tmp5DB=e1->loc;struct _fat_ptr _tmp5DA=({const char*_tmpBC="coercing pointer to integer to support subtraction";_tag_fat(_tmpBC,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp5DB,_tmp5DA,_tag_fat(_tmpBB,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 556
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 559
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 567
if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind ||({
void*_tmp5DC=t1;Cyc_Unify_unify(_tmp5DC,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 572
if(({struct Cyc_RgnOrder_RgnPO*_tmp5DF=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp5DE=loc;void*_tmp5DD=t2;Cyc_Tcutil_silent_castable(_tmp5DF,_tmp5DE,_tmp5DD,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 576
if(({struct Cyc_RgnOrder_RgnPO*_tmp5E2=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp5E1=loc;void*_tmp5E0=t1;Cyc_Tcutil_silent_castable(_tmp5E2,_tmp5E1,_tmp5E0,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 580
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 585
{struct _tuple0 _tmpBD=({struct _tuple0 _tmp4E9;({void*_tmp5E4=Cyc_Tcutil_compress(t1);_tmp4E9.f1=_tmp5E4;}),({void*_tmp5E3=Cyc_Tcutil_compress(t2);_tmp4E9.f2=_tmp5E3;});_tmp4E9;});struct _tuple0 _tmpBE=_tmpBD;void*_tmpC0;void*_tmpBF;switch(*((int*)_tmpBE.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f2)->tag == 3U){_LL1: _tmpC0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f1)->f1).elt_type;_tmpBF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f2)->f1).elt_type;_LL2:
# 587
 if(Cyc_Unify_unify(_tmpC0,_tmpBF))
return Cyc_Absyn_sint_type;
goto _LL0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->tag == 3U){_LL3: _LL4:
# 591
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 594
({struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp4EB;_tmp4EB.tag=0U,({
struct _fat_ptr _tmp5E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4EB.f1=_tmp5E5;});_tmp4EB;});struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=0U,({struct _fat_ptr _tmp5E6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4EA.f1=_tmp5E6;});_tmp4EA;});void*_tmpC1[2U];_tmpC1[0]=& _tmpC3,_tmpC1[1]=& _tmpC4;({unsigned _tmp5E8=loc;struct _fat_ptr _tmp5E7=({const char*_tmpC2="comparison not allowed between %s and %s";_tag_fat(_tmpC2,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5E8,_tmp5E7,_tag_fat(_tmpC1,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 600
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 602
if(({void*_tmp5E9=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5E9 == Cyc_Absyn_sint_type;}))
return Cyc_Absyn_sint_type;
({struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=0U,({
struct _fat_ptr _tmp5EA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4ED.f1=_tmp5EA;});_tmp4ED;});struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp4EC;_tmp4EC.tag=0U,({struct _fat_ptr _tmp5EB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4EC.f1=_tmp5EB;});_tmp4EC;});void*_tmpC5[2U];_tmpC5[0]=& _tmpC7,_tmpC5[1]=& _tmpC8;({unsigned _tmp5ED=loc;struct _fat_ptr _tmp5EC=({const char*_tmpC6="comparison not allowed between %s and %s";_tag_fat(_tmpC6,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5ED,_tmp5EC,_tag_fat(_tmpC5,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 612
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 614
enum Cyc_Absyn_Primop _tmpC9=p;switch(_tmpC9){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 618
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 621
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL15: _LL16:
# 630
 goto _LL18;case Cyc_Absyn_Neq: _LL17: _LL18:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LL19: _LL1A:
# 633
 goto _LL1C;case Cyc_Absyn_Lt: _LL1B: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL1F: _LL20:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LL21: _LL22:
# 638
({void*_tmpCA=0U;({struct _fat_ptr _tmp5EE=({const char*_tmpCB="bad binary primop";_tag_fat(_tmpCB,sizeof(char),18U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp5EE,_tag_fat(_tmpCA,sizeof(void*),0U));});});}_LL0:;}
# 642
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 650
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpCC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpCD=Cyc_Tcexp_tcExp(te,topt,_tmpCC);
if(!Cyc_Tcutil_is_numeric(_tmpCC))
({struct Cyc_String_pa_PrintArg_struct _tmpD0=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,({struct _fat_ptr _tmp5EF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmpCD));_tmp4EE.f1=_tmp5EF;});_tmp4EE;});void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({unsigned _tmp5F1=_tmpCC->loc;struct _fat_ptr _tmp5F0=({const char*_tmpCF="expecting numeric type but found %s";_tag_fat(_tmpCF,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5F1,_tmp5F0,_tag_fat(_tmpCE,sizeof(void*),1U));});});
return _tmpCD;}
# 657
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpD1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpD2=_tmpD1;switch(_tmpD2){case 0U: _LL1: _LL2:
 return({void*_tmpD3=0U;({struct Cyc_Tcenv_Tenv*_tmp5F5=te;unsigned _tmp5F4=loc;void**_tmp5F3=topt;struct _fat_ptr _tmp5F2=({const char*_tmpD4="primitive operator has 0 arguments";_tag_fat(_tmpD4,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp5F5,_tmp5F4,_tmp5F3,_tmp5F2,_tag_fat(_tmpD3,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LL0;case 2U: _LL5: _LL6:
 t=({struct Cyc_Tcenv_Tenv*_tmp5FA=te;unsigned _tmp5F9=loc;void**_tmp5F8=topt;enum Cyc_Absyn_Primop _tmp5F7=p;struct Cyc_Absyn_Exp*_tmp5F6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5FA,_tmp5F9,_tmp5F8,_tmp5F7,_tmp5F6,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpD5=0U;({struct Cyc_Tcenv_Tenv*_tmp5FE=te;unsigned _tmp5FD=loc;void**_tmp5FC=topt;struct _fat_ptr _tmp5FB=({const char*_tmpD6="primitive operator has > 2 arguments";_tag_fat(_tmpD6,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp5FE,_tmp5FD,_tmp5FC,_tmp5FB,_tag_fat(_tmpD5,sizeof(void*),0U));});});}_LL0:;}
# 665
return t;};}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 668
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
void*_tmpD7=Cyc_Tcutil_compress(t);
void*_tmpD8=_tmpD7;struct Cyc_List_List*_tmpF4;void*_tmpF3;struct Cyc_Absyn_Tqual _tmpF2;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_Datatypefield*_tmpF0;struct Cyc_Absyn_Aggrdecl*_tmpEF;switch(*((int*)_tmpD8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpEF=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownAggr).val;_LL2:
# 672
 if(_tmpEF->impl == 0){
({void*_tmpD9=0U;({unsigned _tmp600=loc;struct _fat_ptr _tmp5FF=({const char*_tmpDA="attempt to write an abstract aggregate";_tag_fat(_tmpDA,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp600,_tmp5FF,_tag_fat(_tmpD9,sizeof(void*),0U));});});
return 0;}else{
# 676
_tmpF1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEF->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpF0=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownDatatypefield).val).f2;_LL6:
# 688
{struct Cyc_List_List*_tmpDF=_tmpF0->typs;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){
struct _tuple16*_tmpE0=(struct _tuple16*)_tmpDF->hd;struct _tuple16*_tmpE1=_tmpE0;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE5;_LLE: _tmpE6=_tmpE1->f1;_tmpE5=_tmpE1->f2;_LLF:;
if(_tmpE6.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpE4=({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U,({struct _fat_ptr _tmp601=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmpF0->name));_tmp4EF.f1=_tmp601;});_tmp4EF;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({unsigned _tmp603=loc;struct _fat_ptr _tmp602=({const char*_tmpE3="attempt to over-write a datatype field (%s) with a const member";_tag_fat(_tmpE3,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp603,_tmp602,_tag_fat(_tmpE2,sizeof(void*),1U));});});
return 0;}
# 694
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpE5))return 0;}}
# 696
return 1;}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpF1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD8)->f2;_LL4:
# 678
 for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
struct Cyc_Absyn_Aggrfield*_tmpDB=(struct Cyc_Absyn_Aggrfield*)_tmpF1->hd;
if((_tmpDB->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=0U,_tmp4F0.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmpDB->name);_tmp4F0;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({unsigned _tmp605=loc;struct _fat_ptr _tmp604=({const char*_tmpDD="attempt to over-write an aggregate with const member %s";_tag_fat(_tmpDD,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp605,_tmp604,_tag_fat(_tmpDC,sizeof(void*),1U));});});
return 0;}
# 684
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpDB->type))return 0;}
# 686
return 1;case 4U: _LL7: _tmpF3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).elt_type;_tmpF2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).tq;_LL8:
# 698
 if(_tmpF2.real_const){
({void*_tmpE7=0U;({unsigned _tmp607=loc;struct _fat_ptr _tmp606=({const char*_tmpE8="attempt to over-write a const array";_tag_fat(_tmpE8,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp607,_tmp606,_tag_fat(_tmpE7,sizeof(void*),0U));});});
return 0;}
# 702
return Cyc_Tcexp_check_writable_aggr(loc,_tmpF3);case 6U: _LL9: _tmpF4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD8)->f1;_LLA:
# 704
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
struct _tuple16*_tmpE9=(struct _tuple16*)_tmpF4->hd;struct _tuple16*_tmpEA=_tmpE9;struct Cyc_Absyn_Tqual _tmpEE;void*_tmpED;_LL11: _tmpEE=_tmpEA->f1;_tmpED=_tmpEA->f2;_LL12:;
if(_tmpEE.real_const){
({void*_tmpEB=0U;({unsigned _tmp609=loc;struct _fat_ptr _tmp608=({const char*_tmpEC="attempt to over-write a tuple field with a const member";_tag_fat(_tmpEC,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp609,_tmp608,_tag_fat(_tmpEB,sizeof(void*),0U));});});
return 0;}
# 710
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpED))return 0;}
# 712
return 1;default: _LLB: _LLC:
 return 1;}_LL0:;}
# 720
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 723
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmpF5=e->r;void*_tmpF6=_tmpF5;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct _fat_ptr*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct _fat_ptr*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Vardecl*_tmp11D;struct Cyc_Absyn_Vardecl*_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11B;struct Cyc_Absyn_Vardecl*_tmp11A;switch(*((int*)_tmpF6)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)){case 3U: _LL1: _tmp11A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL2:
 _tmp11B=_tmp11A;goto _LL4;case 4U: _LL3: _tmp11B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL4:
 _tmp11C=_tmp11B;goto _LL6;case 5U: _LL5: _tmp11C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL6:
 _tmp11D=_tmp11C;goto _LL8;case 1U: _LL7: _tmp11D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL8:
 if(!(_tmp11D->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp11F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmp11E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLA:
# 730
{void*_tmpF7=Cyc_Tcutil_compress((void*)_check_null(_tmp11F->topt));void*_tmpF8=_tmpF7;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Tqual _tmp108;struct Cyc_Absyn_Tqual _tmp107;switch(*((int*)_tmpF8)){case 3U: _LL18: _tmp107=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF8)->f1).elt_tq;_LL19:
 _tmp108=_tmp107;goto _LL1B;case 4U: _LL1A: _tmp108=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF8)->f1).tq;_LL1B:
 if(!_tmp108.real_const)return;goto _LL17;case 6U: _LL1C: _tmp109=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF8)->f1;_LL1D: {
# 734
struct _tuple15 _tmpF9=Cyc_Evexp_eval_const_uint_exp(_tmp11E);struct _tuple15 _tmpFA=_tmpF9;unsigned _tmp106;int _tmp105;_LL21: _tmp106=_tmpFA.f1;_tmp105=_tmpFA.f2;_LL22:;
if(!_tmp105){
({void*_tmpFB=0U;({unsigned _tmp60B=e->loc;struct _fat_ptr _tmp60A=({const char*_tmpFC="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmpFC,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp60B,_tmp60A,_tag_fat(_tmpFB,sizeof(void*),0U));});});
return;}
# 739
{struct _handler_cons _tmpFD;_push_handler(& _tmpFD);{int _tmpFF=0;if(setjmp(_tmpFD.handler))_tmpFF=1;if(!_tmpFF){
{struct _tuple16*_tmp100=((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp109,(int)_tmp106);struct _tuple16*_tmp101=_tmp100;struct Cyc_Absyn_Tqual _tmp102;_LL24: _tmp102=_tmp101->f1;_LL25:;
if(!_tmp102.real_const){_npop_handler(0U);return;}}
# 740
;_pop_handler();}else{void*_tmpFE=(void*)Cyc_Core_get_exn_thrown();void*_tmp103=_tmpFE;void*_tmp104;if(((struct Cyc_List_Nth_exn_struct*)_tmp103)->tag == Cyc_List_Nth){_LL27: _LL28:
# 742
 return;}else{_LL29: _tmp104=_tmp103;_LL2A:(int)_rethrow(_tmp104);}_LL26:;}};}
goto _LL17;}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 746
goto _LL0;case 21U: _LLB: _tmp121=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmp120=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLC:
# 748
{void*_tmp10A=Cyc_Tcutil_compress((void*)_check_null(_tmp121->topt));void*_tmp10B=_tmp10A;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_Aggrdecl**_tmp10C;switch(*((int*)_tmp10B)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10B)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp10C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10B)->f1)->f1).KnownAggr).val;_LL2D: {
# 750
struct Cyc_Absyn_Aggrfield*sf=
_tmp10C == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp10C,_tmp120);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp10D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10B)->f2;_LL2F: {
# 755
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp10D,_tmp120);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 760
goto _LL0;case 22U: _LLD: _tmp123=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmp122=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLE:
# 762
{void*_tmp10E=Cyc_Tcutil_compress((void*)_check_null(_tmp123->topt));void*_tmp10F=_tmp10E;void*_tmp115;struct Cyc_Absyn_Tqual _tmp114;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10F)->tag == 3U){_LL33: _tmp115=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10F)->f1).elt_type;_tmp114=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10F)->f1).elt_tq;_LL34:
# 764
 if(!_tmp114.real_const){
void*_tmp110=Cyc_Tcutil_compress(_tmp115);void*_tmp111=_tmp110;struct Cyc_List_List*_tmp113;struct Cyc_Absyn_Aggrdecl**_tmp112;switch(*((int*)_tmp111)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp111)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp111)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp112=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp111)->f1)->f1).KnownAggr).val;_LL39: {
# 767
struct Cyc_Absyn_Aggrfield*sf=
_tmp112 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp112,_tmp122);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp113=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp111)->f2;_LL3B: {
# 772
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp113,_tmp122);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 778
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 781
goto _LL0;case 20U: _LLF: _tmp124=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL10:
# 783
{void*_tmp116=Cyc_Tcutil_compress((void*)_check_null(_tmp124->topt));void*_tmp117=_tmp116;struct Cyc_Absyn_Tqual _tmp119;struct Cyc_Absyn_Tqual _tmp118;switch(*((int*)_tmp117)){case 3U: _LL3F: _tmp118=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp117)->f1).elt_tq;_LL40:
 _tmp119=_tmp118;goto _LL42;case 4U: _LL41: _tmp119=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp117)->f1).tq;_LL42:
 if(!_tmp119.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 788
goto _LL0;case 12U: _LL11: _tmp125=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL12:
 _tmp126=_tmp125;goto _LL14;case 13U: _LL13: _tmp126=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp126);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 793
({struct Cyc_String_pa_PrintArg_struct _tmp129=({struct Cyc_String_pa_PrintArg_struct _tmp4F1;_tmp4F1.tag=0U,({struct _fat_ptr _tmp60C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp4F1.f1=_tmp60C;});_tmp4F1;});void*_tmp127[1U];_tmp127[0]=& _tmp129;({unsigned _tmp60E=e->loc;struct _fat_ptr _tmp60D=({const char*_tmp128="attempt to write a const location: %s";_tag_fat(_tmp128,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp60E,_tmp60D,_tag_fat(_tmp127,sizeof(void*),1U));});});}
# 796
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 799
({struct Cyc_Tcenv_Tenv*_tmp60F=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp60F,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp12A=0U;({struct Cyc_Tcenv_Tenv*_tmp613=te;unsigned _tmp612=loc;void**_tmp611=topt;struct _fat_ptr _tmp610=({const char*_tmp12B="increment/decrement of non-lvalue";_tag_fat(_tmp12B,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp613,_tmp612,_tmp611,_tmp610,_tag_fat(_tmp12A,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 805
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp12C=0U;({unsigned _tmp615=e->loc;struct _fat_ptr _tmp614=({const char*_tmp12D="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp12D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp615,_tmp614,_tag_fat(_tmp12C,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp12E=0U;({unsigned _tmp617=e->loc;struct _fat_ptr _tmp616=({const char*_tmp12F="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp12F,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp617,_tmp616,_tag_fat(_tmp12E,sizeof(void*),0U));});});}else{
# 814
({struct Cyc_String_pa_PrintArg_struct _tmp132=({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0U,({struct _fat_ptr _tmp618=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4F2.f1=_tmp618;});_tmp4F2;});void*_tmp130[1U];_tmp130[0]=& _tmp132;({unsigned _tmp61A=e->loc;struct _fat_ptr _tmp619=({const char*_tmp131="expecting arithmetic or ? type but found %s";_tag_fat(_tmp131,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp61A,_tmp619,_tag_fat(_tmp130,sizeof(void*),1U));});});}}
# 816
return t;};}
# 820
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 822
({struct Cyc_Tcenv_Tenv*_tmp61C=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp61B=e1;Cyc_Tcexp_tcTest(_tmp61C,_tmp61B,({const char*_tmp133="conditional expression";_tag_fat(_tmp133,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 829
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp134=({struct Cyc_List_List _tmp4F6;_tmp4F6.hd=e3,_tmp4F6.tl=0;_tmp4F6;});
struct Cyc_List_List _tmp135=({struct Cyc_List_List _tmp4F5;_tmp4F5.hd=e2,_tmp4F5.tl=& _tmp134;_tmp4F5;});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp61D=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp61D,t,& _tmp135);})){
({struct Cyc_String_pa_PrintArg_struct _tmp138=({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U,({
struct _fat_ptr _tmp61E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4F4.f1=_tmp61E;});_tmp4F4;});struct Cyc_String_pa_PrintArg_struct _tmp139=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({struct _fat_ptr _tmp61F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp4F3.f1=_tmp61F;});_tmp4F3;});void*_tmp136[2U];_tmp136[0]=& _tmp138,_tmp136[1]=& _tmp139;({unsigned _tmp621=loc;struct _fat_ptr _tmp620=({const char*_tmp137="conditional clause types do not match: %s != %s";_tag_fat(_tmp137,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp621,_tmp620,_tag_fat(_tmp136,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 837
return t;};};}
# 841
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 843
({struct Cyc_Tcenv_Tenv*_tmp623=te;struct Cyc_Absyn_Exp*_tmp622=e1;Cyc_Tcexp_tcTest(_tmp623,_tmp622,({const char*_tmp13A="logical-and expression";_tag_fat(_tmp13A,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp625=te;struct Cyc_Absyn_Exp*_tmp624=e2;Cyc_Tcexp_tcTest(_tmp625,_tmp624,({const char*_tmp13B="logical-and expression";_tag_fat(_tmp13B,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 849
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 851
({struct Cyc_Tcenv_Tenv*_tmp627=te;struct Cyc_Absyn_Exp*_tmp626=e1;Cyc_Tcexp_tcTest(_tmp627,_tmp626,({const char*_tmp13C="logical-or expression";_tag_fat(_tmp13C,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp629=te;struct Cyc_Absyn_Exp*_tmp628=e2;Cyc_Tcexp_tcTest(_tmp629,_tmp628,({const char*_tmp13D="logical-or expression";_tag_fat(_tmp13D,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 857
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 864
({struct Cyc_Tcenv_Tenv*_tmp62A=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp62A,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 869
{void*_tmp13E=Cyc_Tcutil_compress(t1);void*_tmp13F=_tmp13E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->tag == 4U){_LL1: _LL2:
({void*_tmp140=0U;({unsigned _tmp62C=loc;struct _fat_ptr _tmp62B=({const char*_tmp141="cannot assign to an array";_tag_fat(_tmp141,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp62C,_tmp62B,_tag_fat(_tmp140,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 874
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp142=0U;({unsigned _tmp62E=loc;struct _fat_ptr _tmp62D=({const char*_tmp143="type is abstract (can't determine size).";_tag_fat(_tmp143,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp62E,_tmp62D,_tag_fat(_tmp142,sizeof(void*),0U));});});
# 878
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp144=0U;({struct Cyc_Tcenv_Tenv*_tmp632=te;unsigned _tmp631=loc;void**_tmp630=topt;struct _fat_ptr _tmp62F=({const char*_tmp145="assignment to non-lvalue";_tag_fat(_tmp145,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp632,_tmp631,_tmp630,_tmp62F,_tag_fat(_tmp144,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp146=0U;({unsigned _tmp634=e2->loc;struct _fat_ptr _tmp633=({const char*_tmp147="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp147,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp634,_tmp633,_tag_fat(_tmp146,sizeof(void*),0U));});});
if(!({struct Cyc_RgnOrder_RgnPO*_tmp636=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp635=e2;Cyc_Tcutil_coerce_assign(_tmp636,_tmp635,t1);})){
void*_tmp148=({struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U,({
struct _fat_ptr _tmp637=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F8.f1=_tmp637;});_tmp4F8;});struct Cyc_String_pa_PrintArg_struct _tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U,({struct _fat_ptr _tmp638=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F7.f1=_tmp638;});_tmp4F7;});void*_tmp149[2U];_tmp149[0]=& _tmp14B,_tmp149[1]=& _tmp14C;({struct Cyc_Tcenv_Tenv*_tmp63C=te;unsigned _tmp63B=loc;void**_tmp63A=topt;struct _fat_ptr _tmp639=({const char*_tmp14A="type mismatch: %s != %s";_tag_fat(_tmp14A,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp63C,_tmp63B,_tmp63A,_tmp639,_tag_fat(_tmp149,sizeof(void*),2U));});});
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return _tmp148;}}else{
# 892
enum Cyc_Absyn_Primop _tmp14D=(enum Cyc_Absyn_Primop)po->v;
struct Cyc_Absyn_Exp*_tmp14E=Cyc_Absyn_copy_exp(e1);
void*_tmp14F=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp14D,_tmp14E,e2);
if((!Cyc_Unify_unify(_tmp14F,t1)&& Cyc_Tcutil_is_arithmetic_type(_tmp14F))&& Cyc_Tcutil_is_arithmetic_type(t1))
_tmp14F=t1;else{
if(!(Cyc_Unify_unify(_tmp14F,t1)|| Cyc_Tcutil_is_arithmetic_type(_tmp14F)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp150=({struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,({
# 900
struct _fat_ptr _tmp63D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FA.f1=_tmp63D;});_tmp4FA;});struct Cyc_String_pa_PrintArg_struct _tmp154=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,({
struct _fat_ptr _tmp63E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F9.f1=_tmp63E;});_tmp4F9;});void*_tmp151[2U];_tmp151[0]=& _tmp153,_tmp151[1]=& _tmp154;({struct Cyc_Tcenv_Tenv*_tmp642=te;unsigned _tmp641=loc;void**_tmp640=topt;struct _fat_ptr _tmp63F=({const char*_tmp152="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_fat(_tmp152,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp642,_tmp641,_tmp640,_tmp63F,_tag_fat(_tmp151,sizeof(void*),2U));});});
Cyc_Unify_unify(_tmp14F,t1);
Cyc_Unify_explain_failure();
return _tmp150;}}
# 906
return _tmp14F;}
# 908
return t1;};};}
# 912
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp643=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp643,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp645=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp644=topt;Cyc_Tcexp_tcExp(_tmp645,_tmp644,e2);});
return(void*)_check_null(e2->topt);}
# 919
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 922
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 925
{void*_tmp155=Cyc_Tcutil_compress(t1);void*_tmp156=_tmp155;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)->f1 == 0){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 932
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp157=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp158=_tmp157;struct _tuple1*_tmp15C;struct Cyc_List_List*_tmp15B;unsigned _tmp15A;enum Cyc_Absyn_Scope _tmp159;_LLA: _tmp15C=_tmp158.name;_tmp15B=_tmp158.typs;_tmp15A=_tmp158.loc;_tmp159=_tmp158.sc;_LLB:;
# 935
if(_tmp15B == 0 || _tmp15B->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple16*)_tmp15B->hd)).f2);
# 938
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 942
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp15D=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp15E=_tmp15D;struct _tuple1*_tmp162;struct Cyc_List_List*_tmp161;unsigned _tmp160;enum Cyc_Absyn_Scope _tmp15F;_LLD: _tmp162=_tmp15E.name;_tmp161=_tmp15E.typs;_tmp160=_tmp15E.loc;_tmp15F=_tmp15E.sc;_LLE:;
# 945
if(_tmp161 == 0 || _tmp161->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple16*)_tmp161->hd)).f2);
# 949
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp647=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp646=e;Cyc_Tcutil_coerce_arg(_tmp647,_tmp646,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 954
({struct Cyc_String_pa_PrintArg_struct _tmp165=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,({
struct _fat_ptr _tmp648=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tu));_tmp4FC.f1=_tmp648;});_tmp4FC;});struct Cyc_String_pa_PrintArg_struct _tmp166=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,({struct _fat_ptr _tmp649=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FB.f1=_tmp649;});_tmp4FB;});void*_tmp163[2U];_tmp163[0]=& _tmp165,_tmp163[1]=& _tmp166;({unsigned _tmp64B=e->loc;struct _fat_ptr _tmp64A=({const char*_tmp164="can't find a field in %s to inject a value of type %s";_tag_fat(_tmp164,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp64B,_tmp64A,_tag_fat(_tmp163,sizeof(void*),2U));});});
return 0;}
# 960
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 966
struct Cyc_List_List*_tmp167=args;
int _tmp168=0;
struct Cyc_Tcenv_Tenv*_tmp169=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp16A=Cyc_Tcenv_clear_abstract_val_ok(_tmp169);
Cyc_Tcexp_tcExp(_tmp16A,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 975
void*_tmp16B=t;void*_tmp1CA;struct Cyc_Absyn_Tqual _tmp1C9;void*_tmp1C8;void*_tmp1C7;void*_tmp1C6;void*_tmp1C5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->tag == 3U){_LL1: _tmp1CA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).elt_type;_tmp1C9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).elt_tq;_tmp1C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).rgn;_tmp1C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).nullable;_tmp1C6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).bounds;_tmp1C5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).zero_term;_LL2:
# 980
 Cyc_Tcenv_check_rgn_accessible(_tmp16A,loc,_tmp1C8);
# 982
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1C6);{
void*_tmp16C=Cyc_Tcutil_compress(_tmp1CA);void*_tmp16D=_tmp16C;struct Cyc_List_List*_tmp1C1;void*_tmp1C0;struct Cyc_Absyn_Tqual _tmp1BF;void*_tmp1BE;struct Cyc_List_List*_tmp1BD;int _tmp1BC;struct Cyc_Absyn_VarargInfo*_tmp1BB;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_List_List*_tmp1B5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->tag == 5U){_LL6: _tmp1C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).tvars;_tmp1C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).effect;_tmp1BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).ret_tqual;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).ret_type;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).args;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).c_varargs;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).cyc_varargs;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).rgn_po;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).attributes;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).requires_clause;_tmp1B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).requires_relns;_tmp1B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).ensures_clause;_tmp1B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16D)->f1).ensures_relns;_LL7:
# 987
 if(_tmp1C1 != 0 || _tmp1BA != 0)
({void*_tmp16E=0U;({unsigned _tmp64D=e->loc;struct _fat_ptr _tmp64C=({const char*_tmp16F="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp16F,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp64D,_tmp64C,_tag_fat(_tmp16E,sizeof(void*),0U));});});
# 991
if(topt != 0)Cyc_Unify_unify(_tmp1BE,*topt);
# 993
while(_tmp167 != 0 && _tmp1BD != 0){
# 995
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp167->hd;
void*t2=(*((struct _tuple9*)_tmp1BD->hd)).f3;
Cyc_Tcexp_tcExp(_tmp16A,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp64F=Cyc_Tcenv_curr_rgnpo(_tmp16A);struct Cyc_Absyn_Exp*_tmp64E=e1;Cyc_Tcutil_coerce_arg(_tmp64F,_tmp64E,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp17F="actual argument has type ";_tag_fat(_tmp17F,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp17E=" but formal has type ";_tag_fat(_tmp17E,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp652=({unsigned long _tmp651=({unsigned long _tmp650=Cyc_strlen((struct _fat_ptr)s0);_tmp650 + Cyc_strlen((struct _fat_ptr)s1);});_tmp651 + Cyc_strlen((struct _fat_ptr)s2);});_tmp652 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({void*_tmp170=0U;({unsigned _tmp655=e1->loc;struct _fat_ptr _tmp654=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,_tmp500.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp500;});struct Cyc_String_pa_PrintArg_struct _tmp174=({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0U,_tmp4FF.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp4FF;});struct Cyc_String_pa_PrintArg_struct _tmp175=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,_tmp4FE.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp4FE;});struct Cyc_String_pa_PrintArg_struct _tmp176=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,_tmp4FD.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp4FD;});void*_tmp171[4U];_tmp171[0]=& _tmp173,_tmp171[1]=& _tmp174,_tmp171[2]=& _tmp175,_tmp171[3]=& _tmp176;({struct _fat_ptr _tmp653=({const char*_tmp172="%s\n\t%s\n%s\n\t%s.";_tag_fat(_tmp172,sizeof(char),15U);});Cyc_aprintf(_tmp653,_tag_fat(_tmp171,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp655,_tmp654,_tag_fat(_tmp170,sizeof(void*),0U));});});else{
# 1007
({void*_tmp177=0U;({unsigned _tmp658=e1->loc;struct _fat_ptr _tmp657=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17A=({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U,_tmp504.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp504;});struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp503;_tmp503.tag=0U,_tmp503.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp503;});struct Cyc_String_pa_PrintArg_struct _tmp17C=({struct Cyc_String_pa_PrintArg_struct _tmp502;_tmp502.tag=0U,_tmp502.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp502;});struct Cyc_String_pa_PrintArg_struct _tmp17D=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,_tmp501.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp501;});void*_tmp178[4U];_tmp178[0]=& _tmp17A,_tmp178[1]=& _tmp17B,_tmp178[2]=& _tmp17C,_tmp178[3]=& _tmp17D;({struct _fat_ptr _tmp656=({const char*_tmp179="%s%s%s%s.";_tag_fat(_tmp179,sizeof(char),10U);});Cyc_aprintf(_tmp656,_tag_fat(_tmp178,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp658,_tmp657,_tag_fat(_tmp177,sizeof(void*),0U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 1012
if(alias_coercion)
({struct Cyc_List_List*_tmp659=({struct Cyc_List_List*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->hd=(void*)_tmp168,_tmp180->tl=*alias_arg_exps;_tmp180;});*alias_arg_exps=_tmp659;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp181=0U;({unsigned _tmp65B=e1->loc;struct _fat_ptr _tmp65A=({const char*_tmp182="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp182,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp65B,_tmp65A,_tag_fat(_tmp181,sizeof(void*),0U));});});
_tmp167=_tmp167->tl;
_tmp1BD=_tmp1BD->tl;
++ _tmp168;}{
# 1023
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1B9;for(0;a != 0;a=a->tl){
void*_tmp183=(void*)a->hd;void*_tmp184=_tmp183;enum Cyc_Absyn_Format_Type _tmp191;int _tmp190;int _tmp18F;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp184)->tag == 19U){_LLB: _tmp191=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp184)->f1;_tmp190=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp184)->f2;_tmp18F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp184)->f3;_LLC:
# 1027
{struct _handler_cons _tmp185;_push_handler(& _tmp185);{int _tmp187=0;if(setjmp(_tmp185.handler))_tmp187=1;if(!_tmp187){
# 1029
{struct Cyc_Absyn_Exp*_tmp188=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp190 - 1);
# 1031
struct Cyc_Core_Opt*fmt_args;
if(_tmp18F == 0)
fmt_args=0;else{
# 1035
fmt_args=({struct Cyc_Core_Opt*_tmp189=_cycalloc(sizeof(*_tmp189));({struct Cyc_List_List*_tmp65C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp18F - 1);_tmp189->v=_tmp65C;});_tmp189;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp18A=_tmp191;switch(_tmp18A){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 1039
 Cyc_Tcexp_check_format_args(_tmp16A,_tmp188,fmt_args,_tmp18F - 1,alias_arg_exps,_tmp1BC,Cyc_Formatstr_get_format_types);
# 1042
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 1045
 Cyc_Tcexp_check_format_args(_tmp16A,_tmp188,fmt_args,_tmp18F - 1,alias_arg_exps,_tmp1BC,Cyc_Formatstr_get_scanf_types);
# 1048
goto _LLF;}_LLF:;};}
# 1029
;_pop_handler();}else{void*_tmp186=(void*)Cyc_Core_get_exn_thrown();void*_tmp18B=_tmp186;void*_tmp18E;if(((struct Cyc_List_Nth_exn_struct*)_tmp18B)->tag == Cyc_List_Nth){_LL17: _LL18:
# 1050
({void*_tmp18C=0U;({unsigned _tmp65E=loc;struct _fat_ptr _tmp65D=({const char*_tmp18D="bad format arguments";_tag_fat(_tmp18D,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp65E,_tmp65D,_tag_fat(_tmp18C,sizeof(void*),0U));});});goto _LL16;}else{_LL19: _tmp18E=_tmp18B;_LL1A:(int)_rethrow(_tmp18E);}_LL16:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1055
if(_tmp1BD != 0)({void*_tmp192=0U;({unsigned _tmp660=loc;struct _fat_ptr _tmp65F=({const char*_tmp193="too few arguments for function";_tag_fat(_tmp193,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp660,_tmp65F,_tag_fat(_tmp192,sizeof(void*),0U));});});else{
# 1057
if((_tmp167 != 0 || _tmp1BC)|| _tmp1BB != 0){
if(_tmp1BC)
for(0;_tmp167 != 0;_tmp167=_tmp167->tl){
Cyc_Tcexp_tcExp(_tmp16A,0,(struct Cyc_Absyn_Exp*)_tmp167->hd);}else{
if(_tmp1BB == 0)
({void*_tmp194=0U;({unsigned _tmp662=loc;struct _fat_ptr _tmp661=({const char*_tmp195="too many arguments for function";_tag_fat(_tmp195,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp662,_tmp661,_tag_fat(_tmp194,sizeof(void*),0U));});});else{
# 1064
struct Cyc_Absyn_VarargInfo _tmp196=*_tmp1BB;struct Cyc_Absyn_VarargInfo _tmp197=_tmp196;void*_tmp1B1;int _tmp1B0;_LL1C: _tmp1B1=_tmp197.type;_tmp1B0=_tmp197.inject;_LL1D:;{
struct Cyc_Absyn_VarargCallInfo*_tmp198=({struct Cyc_Absyn_VarargCallInfo*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF->num_varargs=0,_tmp1AF->injectors=0,_tmp1AF->vai=_tmp1BB;_tmp1AF;});
# 1068
*vararg_call_info=_tmp198;
# 1070
if(!_tmp1B0)
# 1072
for(0;_tmp167 != 0;(_tmp167=_tmp167->tl,_tmp168 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp167->hd;
++ _tmp198->num_varargs;
Cyc_Tcexp_tcExp(_tmp16A,& _tmp1B1,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp664=Cyc_Tcenv_curr_rgnpo(_tmp16A);struct Cyc_Absyn_Exp*_tmp663=e1;Cyc_Tcutil_coerce_arg(_tmp664,_tmp663,_tmp1B1,& alias_coercion);})){
({struct Cyc_String_pa_PrintArg_struct _tmp19B=({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U,({
struct _fat_ptr _tmp665=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp1B1));_tmp506.f1=_tmp665;});_tmp506;});struct Cyc_String_pa_PrintArg_struct _tmp19C=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({struct _fat_ptr _tmp666=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp505.f1=_tmp666;});_tmp505;});void*_tmp199[2U];_tmp199[0]=& _tmp19B,_tmp199[1]=& _tmp19C;({unsigned _tmp668=loc;struct _fat_ptr _tmp667=({const char*_tmp19A="vararg requires type %s but argument has type %s";_tag_fat(_tmp19A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp668,_tmp667,_tag_fat(_tmp199,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 1082
if(alias_coercion)
({struct Cyc_List_List*_tmp669=({struct Cyc_List_List*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->hd=(void*)_tmp168,_tmp19D->tl=*alias_arg_exps;_tmp19D;});*alias_arg_exps=_tmp669;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1B1)&& !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp19E=0U;({unsigned _tmp66B=e1->loc;struct _fat_ptr _tmp66A=({const char*_tmp19F="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp19F,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp66B,_tmp66A,_tag_fat(_tmp19E,sizeof(void*),0U));});});}else{
# 1091
void*_tmp1A0=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1B1));void*_tmp1A1=_tmp1A0;struct Cyc_Absyn_Datatypedecl*_tmp1AE;struct Cyc_List_List*_tmp1AD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1AE=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f1)->f1).KnownDatatype).val;_tmp1AD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A1)->f2;_LL20: {
# 1095
struct Cyc_Absyn_Datatypedecl*_tmp1A2=*Cyc_Tcenv_lookup_datatypedecl(_tmp16A,loc,_tmp1AE->name);
struct Cyc_List_List*fields=0;
if(_tmp1A2->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1A5=({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0U,({
struct _fat_ptr _tmp66C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp1B1));_tmp507.f1=_tmp66C;});_tmp507;});void*_tmp1A3[1U];_tmp1A3[0]=& _tmp1A5;({unsigned _tmp66E=loc;struct _fat_ptr _tmp66D=({const char*_tmp1A4="can't inject into abstract datatype %s";_tag_fat(_tmp1A4,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp66E,_tmp66D,_tag_fat(_tmp1A3,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1A2->fields))->v;}
# 1106
({void*_tmp66F=Cyc_Tcutil_pointer_region(_tmp1B1);Cyc_Unify_unify(_tmp66F,Cyc_Tcenv_curr_rgn(_tmp16A));});{
# 1108
struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1A2->tvs,_tmp1AD);
for(0;_tmp167 != 0;_tmp167=_tmp167->tl){
++ _tmp198->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp167->hd;
# 1113
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp16A,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt))&& !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1A7=0U;({unsigned _tmp671=e1->loc;struct _fat_ptr _tmp670=({const char*_tmp1A8="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp1A8,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp671,_tmp670,_tag_fat(_tmp1A7,sizeof(void*),0U));});});}{
# 1119
struct Cyc_Absyn_Datatypefield*_tmp1A9=({struct Cyc_Tcenv_Tenv*_tmp675=_tmp16A;struct Cyc_Absyn_Exp*_tmp674=e1;void*_tmp673=Cyc_Tcutil_pointer_elt_type(_tmp1B1);struct Cyc_List_List*_tmp672=_tmp1A6;Cyc_Tcexp_tcInjection(_tmp675,_tmp674,_tmp673,_tmp672,fields);});
if(_tmp1A9 != 0)
({struct Cyc_List_List*_tmp677=({
struct Cyc_List_List*_tmp676=_tmp198->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp676,({struct Cyc_List_List*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));
_tmp1AA->hd=_tmp1A9,_tmp1AA->tl=0;_tmp1AA;}));});
# 1121
_tmp198->injectors=_tmp677;});};};}
# 1125
goto _LL1E;};}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({void*_tmp1AB=0U;({unsigned _tmp679=loc;struct _fat_ptr _tmp678=({const char*_tmp1AC="bad inject vararg type";_tag_fat(_tmp1AC,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp679,_tmp678,_tag_fat(_tmp1AB,sizeof(void*),0U));});});goto _LL1E;}_LL1E:;}};}}}}
# 1131
if(*alias_arg_exps == 0)
# 1140 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp16A,loc,(void*)_check_null(_tmp1C0));
# 1142
return _tmp1BE;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({struct _fat_ptr _tmp67A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp508.f1=_tmp67A;});_tmp508;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct Cyc_Tcenv_Tenv*_tmp67E=_tmp16A;unsigned _tmp67D=loc;void**_tmp67C=topt;struct _fat_ptr _tmp67B=({const char*_tmp1B3="expected pointer to function but found %s";_tag_fat(_tmp1B3,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp67E,_tmp67D,_tmp67C,_tmp67B,_tag_fat(_tmp1B2,sizeof(void*),1U));});});}_LL5:;};}else{_LL3: _LL4:
# 1145
 return({struct Cyc_String_pa_PrintArg_struct _tmp1C4=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({struct _fat_ptr _tmp67F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp509.f1=_tmp67F;});_tmp509;});void*_tmp1C2[1U];_tmp1C2[0]=& _tmp1C4;({struct Cyc_Tcenv_Tenv*_tmp683=_tmp16A;unsigned _tmp682=loc;void**_tmp681=topt;struct _fat_ptr _tmp680=({const char*_tmp1C3="expected pointer to function but found %s";_tag_fat(_tmp1C3,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp683,_tmp682,_tmp681,_tmp680,_tag_fat(_tmp1C2,sizeof(void*),1U));});});}_LL0:;};}
# 1149
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1CB=Cyc_Absyn_exn_type();
Cyc_Tcexp_tcExp(te,& _tmp1CB,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp685=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp684=e;Cyc_Tcutil_coerce_arg(_tmp685,_tmp684,_tmp1CB,& bogus);}))
({struct Cyc_String_pa_PrintArg_struct _tmp1CE=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,({struct _fat_ptr _tmp686=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp1CB));_tmp50B.f1=_tmp686;});_tmp50B;});struct Cyc_String_pa_PrintArg_struct _tmp1CF=({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0U,({
struct _fat_ptr _tmp687=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp50A.f1=_tmp687;});_tmp50A;});void*_tmp1CC[2U];_tmp1CC[0]=& _tmp1CE,_tmp1CC[1]=& _tmp1CF;({unsigned _tmp689=loc;struct _fat_ptr _tmp688=({const char*_tmp1CD="expected %s but found %s";_tag_fat(_tmp1CD,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp689,_tmp688,_tag_fat(_tmp1CC,sizeof(void*),2U));});});
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1161
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1163
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1165
({void*_tmp68A=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp68A;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1D0=t1;void*_tmp1EB;struct Cyc_Absyn_Tqual _tmp1EA;void*_tmp1E9;void*_tmp1E8;void*_tmp1E7;void*_tmp1E6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->tag == 3U){_LL1: _tmp1EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_type;_tmp1EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_tq;_tmp1E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).rgn;_tmp1E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).nullable;_tmp1E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).bounds;_tmp1E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).zero_term;_LL2:
# 1169
{void*_tmp1D1=Cyc_Tcutil_compress(_tmp1EB);void*_tmp1D2=_tmp1D1;struct Cyc_List_List*_tmp1E5;void*_tmp1E4;struct Cyc_Absyn_Tqual _tmp1E3;void*_tmp1E2;struct Cyc_List_List*_tmp1E1;int _tmp1E0;struct Cyc_Absyn_VarargInfo*_tmp1DF;struct Cyc_List_List*_tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_List_List*_tmp1D9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->tag == 5U){_LL6: _tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).tvars;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).effect;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).ret_tqual;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).ret_type;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).args;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).c_varargs;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).cyc_varargs;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).rgn_po;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).attributes;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).requires_clause;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).requires_relns;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).ensures_clause;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D2)->f1).ensures_relns;_LL7: {
# 1171
struct Cyc_List_List*instantiation=0;
# 1173
for(0;ts != 0 && _tmp1E5 != 0;(ts=ts->tl,_tmp1E5=_tmp1E5->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1E5->hd,& Cyc_Tcutil_bk);
({unsigned _tmp68E=loc;struct Cyc_Tcenv_Tenv*_tmp68D=te;struct Cyc_List_List*_tmp68C=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp68B=k;Cyc_Tctyp_check_type(_tmp68E,_tmp68D,_tmp68C,_tmp68B,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));({struct _tuple13*_tmp68F=({struct _tuple13*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=(struct Cyc_Absyn_Tvar*)_tmp1E5->hd,_tmp1D3->f2=(void*)ts->hd;_tmp1D3;});_tmp1D4->hd=_tmp68F;}),_tmp1D4->tl=instantiation;_tmp1D4;});}
# 1179
if(ts != 0)
return({void*_tmp1D5=0U;({struct Cyc_Tcenv_Tenv*_tmp693=te;unsigned _tmp692=loc;void**_tmp691=topt;struct _fat_ptr _tmp690=({const char*_tmp1D6="too many type variables in instantiation";_tag_fat(_tmp1D6,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp693,_tmp692,_tmp691,_tmp690,_tag_fat(_tmp1D5,sizeof(void*),0U));});});
# 1184
if(_tmp1E5 == 0){
_tmp1DE=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1DE);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1DE);
_tmp1DE=0;}{
# 1189
void*new_fn_typ=({
struct Cyc_List_List*_tmp694=instantiation;Cyc_Tcutil_substitute(_tmp694,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));
_tmp1D8->tag=5U,(_tmp1D8->f1).tvars=_tmp1E5,(_tmp1D8->f1).effect=_tmp1E4,(_tmp1D8->f1).ret_tqual=_tmp1E3,(_tmp1D8->f1).ret_type=_tmp1E2,(_tmp1D8->f1).args=_tmp1E1,(_tmp1D8->f1).c_varargs=_tmp1E0,(_tmp1D8->f1).cyc_varargs=_tmp1DF,(_tmp1D8->f1).rgn_po=_tmp1DE,(_tmp1D8->f1).attributes=_tmp1DD,(_tmp1D8->f1).requires_clause=_tmp1DC,(_tmp1D8->f1).requires_relns=_tmp1DB,(_tmp1D8->f1).ensures_clause=_tmp1DA,(_tmp1D8->f1).ensures_relns=_tmp1D9;_tmp1D8;}));});
# 1194
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->tag=3U,(_tmp1D7->f1).elt_type=new_fn_typ,(_tmp1D7->f1).elt_tq=_tmp1EA,((_tmp1D7->f1).ptr_atts).rgn=_tmp1E9,((_tmp1D7->f1).ptr_atts).nullable=_tmp1E8,((_tmp1D7->f1).ptr_atts).bounds=_tmp1E7,((_tmp1D7->f1).ptr_atts).zero_term=_tmp1E6,((_tmp1D7->f1).ptr_atts).ptrloc=0;_tmp1D7;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1197
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1200
return({struct Cyc_String_pa_PrintArg_struct _tmp1EE=({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0U,({
struct _fat_ptr _tmp695=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp50C.f1=_tmp695;});_tmp50C;});void*_tmp1EC[1U];_tmp1EC[0]=& _tmp1EE;({struct Cyc_Tcenv_Tenv*_tmp699=te;unsigned _tmp698=loc;void**_tmp697=topt;struct _fat_ptr _tmp696=({const char*_tmp1ED="expecting polymorphic type but found %s";_tag_fat(_tmp1ED,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp699,_tmp698,_tmp697,_tmp696,_tag_fat(_tmp1EC,sizeof(void*),1U));});});};}
# 1205
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1207
({unsigned _tmp69D=loc;struct Cyc_Tcenv_Tenv*_tmp69C=te;struct Cyc_List_List*_tmp69B=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp69A=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1207
Cyc_Tctyp_check_type(_tmp69D,_tmp69C,_tmp69B,_tmp69A,1,0,t);});
# 1209
Cyc_Tcutil_check_no_qual(loc,t);
# 1211
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp6A0=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp69F=loc;void*_tmp69E=t2;Cyc_Tcutil_silent_castable(_tmp6A0,_tmp69F,_tmp69E,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1216
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp6A3=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6A2=loc;void*_tmp6A1=t2;Cyc_Tcutil_castable(_tmp6A3,_tmp6A2,_tmp6A1,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1223
Cyc_Unify_unify(t2,t);{
void*_tmp1EF=({struct Cyc_String_pa_PrintArg_struct _tmp1F2=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0U,({
struct _fat_ptr _tmp6A4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp50E.f1=_tmp6A4;});_tmp50E;});struct Cyc_String_pa_PrintArg_struct _tmp1F3=({struct Cyc_String_pa_PrintArg_struct _tmp50D;_tmp50D.tag=0U,({struct _fat_ptr _tmp6A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp50D.f1=_tmp6A5;});_tmp50D;});void*_tmp1F0[2U];_tmp1F0[0]=& _tmp1F2,_tmp1F0[1]=& _tmp1F3;({struct Cyc_Tcenv_Tenv*_tmp6A8=te;unsigned _tmp6A7=loc;struct _fat_ptr _tmp6A6=({const char*_tmp1F1="cannot cast %s to %s";_tag_fat(_tmp1F1,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6A8,_tmp6A7,& t,_tmp6A6,_tag_fat(_tmp1F0,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();
return _tmp1EF;};}}}
# 1233
{struct _tuple0 _tmp1F4=({struct _tuple0 _tmp50F;_tmp50F.f1=e->r,({void*_tmp6A9=Cyc_Tcutil_compress(t);_tmp50F.f2=_tmp6A9;});_tmp50F;});struct _tuple0 _tmp1F5=_tmp1F4;int _tmp1FD;void*_tmp1FC;void*_tmp1FB;void*_tmp1FA;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F5.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->tag == 3U){_LL1: _tmp1FD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F5.f1)->f1).fat_result;_tmp1FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->f1).ptr_atts).nullable;_tmp1FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->f1).ptr_atts).bounds;_tmp1FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->f1).ptr_atts).zero_term;_LL2:
# 1237
 if((_tmp1FD && !Cyc_Tcutil_force_type2bool(0,_tmp1FA))&& Cyc_Tcutil_force_type2bool(0,_tmp1FC)){
struct Cyc_Absyn_Exp*_tmp1F6=({void*_tmp6AA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6AA,_tmp1FB);});
if(_tmp1F6 != 0){
struct Cyc_Absyn_Exp*_tmp1F7=e;
if((Cyc_Evexp_eval_const_uint_exp(_tmp1F7)).f1 == (unsigned)1)
({void*_tmp1F8=0U;({unsigned _tmp6AC=loc;struct _fat_ptr _tmp6AB=({const char*_tmp1F9="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp1F9,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6AC,_tmp6AB,_tag_fat(_tmp1F8,sizeof(void*),0U));});});}}
# 1245
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1248
return t;};}
# 1252
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp1FE=0;
struct Cyc_Absyn_Tqual _tmp1FF=Cyc_Absyn_empty_tqual(0U);
int _tmp200=0;
if(topt != 0){
void*_tmp201=Cyc_Tcutil_compress(*topt);void*_tmp202=_tmp201;void**_tmp205;struct Cyc_Absyn_Tqual _tmp204;void*_tmp203;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->tag == 3U){_LL1: _tmp205=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->f1).elt_type;_tmp204=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->f1).elt_tq;_tmp203=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->f1).ptr_atts).nullable;_LL2:
# 1259
 _tmp1FE=_tmp205;
_tmp1FF=_tmp204;
_tmp200=Cyc_Tcutil_force_type2bool(0,_tmp203);
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1273
({struct Cyc_Tcenv_Tenv*_tmp6AE=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp6AD=_tmp1FE;Cyc_Tcexp_tcExpNoInst(_tmp6AE,_tmp6AD,e);});
# 1276
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp206=0U;({unsigned _tmp6B0=e->loc;struct _fat_ptr _tmp6AF=({const char*_tmp207="Cannot take the address of an alias-free path";_tag_fat(_tmp207,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6B0,_tmp6AF,_tag_fat(_tmp206,sizeof(void*),0U));});});
# 1282
{void*_tmp208=e->r;void*_tmp209=_tmp208;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*_tmp20C;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->tag == 23U){_LL6: _tmp20D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp20C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL7:
# 1284
{void*_tmp20A=Cyc_Tcutil_compress((void*)_check_null(_tmp20D->topt));void*_tmp20B=_tmp20A;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp20B)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1289
({void*_tmp6B1=(Cyc_Absyn_add_exp(_tmp20D,_tmp20C,0U))->r;e0->r=_tmp6B1;});
return Cyc_Tcexp_tcPlus(te,_tmp20D,_tmp20C);}_LLA:;}
# 1292
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1297
{void*_tmp20E=e->r;void*_tmp20F=_tmp20E;switch(*((int*)_tmp20F)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp20F)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp20F)->f3 == 1){_LL12: _LL13:
# 1300
({void*_tmp210=0U;({unsigned _tmp6B3=e->loc;struct _fat_ptr _tmp6B2=({const char*_tmp211="cannot take the address of a @tagged union member";_tag_fat(_tmp211,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6B3,_tmp6B2,_tag_fat(_tmp210,sizeof(void*),0U));});});goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1305
struct _tuple14 _tmp212=Cyc_Tcutil_addressof_props(e);struct _tuple14 _tmp213=_tmp212;int _tmp217;void*_tmp216;_LL17: _tmp217=_tmp213.f1;_tmp216=_tmp213.f2;_LL18:;
# 1307
if(Cyc_Tcutil_is_noalias_region(_tmp216,0))
({void*_tmp214=0U;({unsigned _tmp6B5=e->loc;struct _fat_ptr _tmp6B4=({const char*_tmp215="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp215,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp6B5,_tmp6B4,_tag_fat(_tmp214,sizeof(void*),0U));});});{
# 1310
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp217){
tq.print_const=1;
tq.real_const=1;}{
# 1316
void*t=_tmp200?
Cyc_Absyn_star_type((void*)_check_null(e->topt),_tmp216,tq,Cyc_Absyn_false_type):
 Cyc_Absyn_at_type((void*)_check_null(e->topt),_tmp216,tq,Cyc_Absyn_false_type);
return t;};};};}
# 1323
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
if(te->allow_valueof)
# 1327
return Cyc_Absyn_uint_type;
# 1329
({unsigned _tmp6B8=loc;struct Cyc_Tcenv_Tenv*_tmp6B7=te;struct Cyc_List_List*_tmp6B6=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp6B8,_tmp6B7,_tmp6B6,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0U,({
struct _fat_ptr _tmp6B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp510.f1=_tmp6B9;});_tmp510;});void*_tmp218[1U];_tmp218[0]=& _tmp21A;({unsigned _tmp6BB=loc;struct _fat_ptr _tmp6BA=({const char*_tmp219="sizeof applied to type %s, which has unknown size here";_tag_fat(_tmp219,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp6BB,_tmp6BA,_tag_fat(_tmp218,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp21B=Cyc_Tcutil_compress(*topt);void*_tmp21C=_tmp21B;void*_tmp221;void*_tmp220;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->f2 != 0){_LL1: _tmp221=_tmp21C;_tmp220=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->f2)->hd;_LL2: {
# 1338
struct Cyc_Absyn_Exp*_tmp21D=Cyc_Absyn_sizeoftype_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21E=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->tag=9U,_tmp21F->f1=_tmp21D;_tmp21F;});
if(Cyc_Unify_unify(_tmp220,(void*)_tmp21E))return _tmp221;
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1344
return Cyc_Absyn_uint_type;}
# 1347
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1354
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1356
({unsigned _tmp6BE=loc;struct Cyc_Tcenv_Tenv*_tmp6BD=te;struct Cyc_List_List*_tmp6BC=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp6BE,_tmp6BD,_tmp6BC,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp222=fs;
void*_tmp223=t_orig;
for(0;_tmp222 != 0;_tmp222=_tmp222->tl){
void*_tmp224=(void*)_tmp222->hd;
void*_tmp225=_tmp224;unsigned _tmp25F;struct _fat_ptr*_tmp25E;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp225)->tag == 0U){_LL1: _tmp25E=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp225)->f1;_LL2: {
# 1364
int bad_type=1;
{void*_tmp226=Cyc_Tcutil_compress(_tmp223);void*_tmp227=_tmp226;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_Aggrdecl**_tmp22E;switch(*((int*)_tmp227)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp227)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp227)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp22E=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp227)->f1)->f1).KnownAggr).val;_LL7:
# 1367
 if((*_tmp22E)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp25E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp22E)->impl))->fields);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp22A=({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0U,_tmp511.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp25E);_tmp511;});void*_tmp228[1U];_tmp228[0]=& _tmp22A;({unsigned _tmp6C0=loc;struct _fat_ptr _tmp6BF=({const char*_tmp229="no field of struct/union has name %s";_tag_fat(_tmp229,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6C0,_tmp6BF,_tag_fat(_tmp228,sizeof(void*),1U));});});else{
# 1372
_tmp223=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp22F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp227)->f2;_LL9: {
# 1376
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp25E,_tmp22F);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp22D=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0U,_tmp512.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp25E);_tmp512;});void*_tmp22B[1U];_tmp22B[0]=& _tmp22D;({unsigned _tmp6C2=loc;struct _fat_ptr _tmp6C1=({const char*_tmp22C="no field of struct/union has name %s";_tag_fat(_tmp22C,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6C2,_tmp6C1,_tag_fat(_tmp22B,sizeof(void*),1U));});});else{
# 1380
_tmp223=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1385
if(bad_type){
if(_tmp222 == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp232=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,({struct _fat_ptr _tmp6C3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp223));_tmp513.f1=_tmp6C3;});_tmp513;});void*_tmp230[1U];_tmp230[0]=& _tmp232;({unsigned _tmp6C5=loc;struct _fat_ptr _tmp6C4=({const char*_tmp231="%s is not a known struct/union type";_tag_fat(_tmp231,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6C5,_tmp6C4,_tag_fat(_tmp230,sizeof(void*),1U));});});else{
# 1389
struct _fat_ptr _tmp233=({struct Cyc_String_pa_PrintArg_struct _tmp246=({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0U,({struct _fat_ptr _tmp6C6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp51A.f1=_tmp6C6;});_tmp51A;});void*_tmp244[1U];_tmp244[0]=& _tmp246;({struct _fat_ptr _tmp6C7=({const char*_tmp245="(%s)";_tag_fat(_tmp245,sizeof(char),5U);});Cyc_aprintf(_tmp6C7,_tag_fat(_tmp244,sizeof(void*),1U));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp222;x=x->tl){
void*_tmp234=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp235=_tmp234;unsigned _tmp23F;struct _fat_ptr*_tmp23E;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp235)->tag == 0U){_LLD: _tmp23E=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp235)->f1;_LLE:
# 1394
 _tmp233=({struct Cyc_String_pa_PrintArg_struct _tmp238=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,_tmp515.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp233);_tmp515;});struct Cyc_String_pa_PrintArg_struct _tmp239=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,_tmp514.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp23E);_tmp514;});void*_tmp236[2U];_tmp236[0]=& _tmp238,_tmp236[1]=& _tmp239;({struct _fat_ptr _tmp6C8=({const char*_tmp237="%s.%s";_tag_fat(_tmp237,sizeof(char),6U);});Cyc_aprintf(_tmp6C8,_tag_fat(_tmp236,sizeof(void*),2U));});});goto _LLC;}else{_LLF: _tmp23F=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp235)->f1;_LL10:
# 1396
 _tmp233=({struct Cyc_String_pa_PrintArg_struct _tmp23C=({struct Cyc_String_pa_PrintArg_struct _tmp517;_tmp517.tag=0U,_tmp517.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp233);_tmp517;});struct Cyc_Int_pa_PrintArg_struct _tmp23D=({struct Cyc_Int_pa_PrintArg_struct _tmp516;_tmp516.tag=1U,_tmp516.f1=(unsigned long)((int)_tmp23F);_tmp516;});void*_tmp23A[2U];_tmp23A[0]=& _tmp23C,_tmp23A[1]=& _tmp23D;({struct _fat_ptr _tmp6C9=({const char*_tmp23B="%s.%d";_tag_fat(_tmp23B,sizeof(char),6U);});Cyc_aprintf(_tmp6C9,_tag_fat(_tmp23A,sizeof(void*),2U));});});goto _LLC;}_LLC:;}
# 1398
({struct Cyc_String_pa_PrintArg_struct _tmp242=({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0U,_tmp519.f1=(struct _fat_ptr)((struct _fat_ptr)_tmp233);_tmp519;});struct Cyc_String_pa_PrintArg_struct _tmp243=({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0U,({struct _fat_ptr _tmp6CA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp223));_tmp518.f1=_tmp6CA;});_tmp518;});void*_tmp240[2U];_tmp240[0]=& _tmp242,_tmp240[1]=& _tmp243;({unsigned _tmp6CC=loc;struct _fat_ptr _tmp6CB=({const char*_tmp241="%s == %s is not a struct/union type";_tag_fat(_tmp241,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6CC,_tmp6CB,_tag_fat(_tmp240,sizeof(void*),2U));});});}}
# 1401
goto _LL0;}}else{_LL3: _tmp25F=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp225)->f1;_LL4: {
# 1403
int bad_type=1;
{void*_tmp247=Cyc_Tcutil_compress(_tmp223);void*_tmp248=_tmp247;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Datatypefield*_tmp258;struct Cyc_Absyn_Aggrdecl**_tmp257;switch(*((int*)_tmp248)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp257=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)->f1).KnownAggr).val;_LL13:
# 1406
 if((*_tmp257)->impl == 0)
goto _LL11;
_tmp259=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp257)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp258=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248)->f1)->f1).KnownDatatypefield).val).f2;_LL19:
# 1426
 if(({unsigned _tmp6CD=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp258->typs);_tmp6CD < _tmp25F;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp253=({struct Cyc_Int_pa_PrintArg_struct _tmp51C;_tmp51C.tag=1U,({
unsigned long _tmp6CE=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp258->typs);_tmp51C.f1=_tmp6CE;});_tmp51C;});struct Cyc_Int_pa_PrintArg_struct _tmp254=({struct Cyc_Int_pa_PrintArg_struct _tmp51B;_tmp51B.tag=1U,_tmp51B.f1=(unsigned long)((int)_tmp25F);_tmp51B;});void*_tmp251[2U];_tmp251[0]=& _tmp253,_tmp251[1]=& _tmp254;({unsigned _tmp6D0=loc;struct _fat_ptr _tmp6CF=({const char*_tmp252="datatype field has too few components: %d < %d";_tag_fat(_tmp252,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6D0,_tmp6CF,_tag_fat(_tmp251,sizeof(void*),2U));});});else{
# 1430
if(_tmp25F != (unsigned)0)
_tmp223=(*((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp258->typs,(int)(_tmp25F - (unsigned)1))).f2;else{
if(_tmp222->tl != 0)
({void*_tmp255=0U;({unsigned _tmp6D2=loc;struct _fat_ptr _tmp6D1=({const char*_tmp256="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp256,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6D2,_tmp6D1,_tag_fat(_tmp255,sizeof(void*),0U));});});}}
# 1435
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp259=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248)->f2;_LL15:
# 1410
 if(({unsigned _tmp6D3=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp259);_tmp6D3 <= _tmp25F;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp24B=({struct Cyc_Int_pa_PrintArg_struct _tmp51E;_tmp51E.tag=1U,({
unsigned long _tmp6D4=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp259);_tmp51E.f1=_tmp6D4;});_tmp51E;});struct Cyc_Int_pa_PrintArg_struct _tmp24C=({struct Cyc_Int_pa_PrintArg_struct _tmp51D;_tmp51D.tag=1U,_tmp51D.f1=(unsigned long)((int)_tmp25F);_tmp51D;});void*_tmp249[2U];_tmp249[0]=& _tmp24B,_tmp249[1]=& _tmp24C;({unsigned _tmp6D6=loc;struct _fat_ptr _tmp6D5=({const char*_tmp24A="struct/union has too few components: %d <= %d";_tag_fat(_tmp24A,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6D6,_tmp6D5,_tag_fat(_tmp249,sizeof(void*),2U));});});else{
# 1414
_tmp223=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp259,(int)_tmp25F))->type;}
bad_type=0;
goto _LL11;case 6U: _LL16: _tmp25A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp248)->f1;_LL17:
# 1418
 if(({unsigned _tmp6D7=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25A);_tmp6D7 <= _tmp25F;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp24F=({struct Cyc_Int_pa_PrintArg_struct _tmp520;_tmp520.tag=1U,({
unsigned long _tmp6D8=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25A);_tmp520.f1=_tmp6D8;});_tmp520;});struct Cyc_Int_pa_PrintArg_struct _tmp250=({struct Cyc_Int_pa_PrintArg_struct _tmp51F;_tmp51F.tag=1U,_tmp51F.f1=(unsigned long)((int)_tmp25F);_tmp51F;});void*_tmp24D[2U];_tmp24D[0]=& _tmp24F,_tmp24D[1]=& _tmp250;({unsigned _tmp6DA=loc;struct _fat_ptr _tmp6D9=({const char*_tmp24E="tuple has too few components: %d <= %d";_tag_fat(_tmp24E,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6DA,_tmp6D9,_tag_fat(_tmp24D,sizeof(void*),2U));});});else{
# 1422
_tmp223=(*((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp25A,(int)_tmp25F)).f2;}
bad_type=0;
goto _LL11;default: _LL1A: _LL1B:
# 1437
 goto _LL11;}_LL11:;}
# 1439
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp25D=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0U,({struct _fat_ptr _tmp6DB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp223));_tmp521.f1=_tmp6DB;});_tmp521;});void*_tmp25B[1U];_tmp25B[0]=& _tmp25D;({unsigned _tmp6DD=loc;struct _fat_ptr _tmp6DC=({const char*_tmp25C="%s is not a known type";_tag_fat(_tmp25C,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6DD,_tmp6DC,_tag_fat(_tmp25B,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1444
return Cyc_Absyn_uint_type;};}
# 1448
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp260=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp260,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp261=t;void*_tmp278;void*_tmp277;void*_tmp276;void*_tmp275;switch(*((int*)_tmp261)){case 1U: _LL1: _LL2: {
# 1454
struct Cyc_List_List*_tmp262=Cyc_Tcenv_lookup_type_vars(_tmp260);
void*_tmp263=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->v=_tmp262;_tmp26B;}));
void*_tmp264=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->v=_tmp262;_tmp26A;}));
void*_tmp265=Cyc_Tcutil_any_bounds(_tmp262);
void*_tmp266=Cyc_Tcutil_any_bool(_tmp262);
struct Cyc_Absyn_PtrAtts _tmp267=({struct Cyc_Absyn_PtrAtts _tmp522;_tmp522.rgn=_tmp264,({void*_tmp6DE=Cyc_Tcutil_any_bool(_tmp262);_tmp522.nullable=_tmp6DE;}),_tmp522.bounds=_tmp265,_tmp522.zero_term=_tmp266,_tmp522.ptrloc=0;_tmp522;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp268=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->tag=3U,(_tmp269->f1).elt_type=_tmp263,({struct Cyc_Absyn_Tqual _tmp6DF=Cyc_Absyn_empty_tqual(0U);(_tmp269->f1).elt_tq=_tmp6DF;}),(_tmp269->f1).ptr_atts=_tmp267;_tmp269;});
Cyc_Unify_unify(t,(void*)_tmp268);
_tmp278=_tmp263;_tmp277=_tmp264;_tmp276=_tmp265;_tmp275=_tmp266;goto _LL4;}case 3U: _LL3: _tmp278=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).elt_type;_tmp277=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).ptr_atts).rgn;_tmp276=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).ptr_atts).bounds;_tmp275=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).ptr_atts).zero_term;_LL4:
# 1464
 Cyc_Tcenv_check_rgn_accessible(_tmp260,loc,_tmp277);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp276);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmp278),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(_tmp260)){
void*_tmp26C=Cyc_Tcutil_compress(_tmp278);void*_tmp26D=_tmp26C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26D)->tag == 5U){_LL8: _LL9:
# 1469
 if(Cyc_Tc_aggressive_warn)
({void*_tmp26E=0U;({unsigned _tmp6E1=loc;struct _fat_ptr _tmp6E0=({const char*_tmp26F="unnecessary dereference for function type";_tag_fat(_tmp26F,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6E1,_tmp6E0,_tag_fat(_tmp26E,sizeof(void*),0U));});});
return t;}else{_LLA: _LLB:
# 1473
({void*_tmp270=0U;({unsigned _tmp6E3=loc;struct _fat_ptr _tmp6E2=({const char*_tmp271="can't dereference abstract pointer type";_tag_fat(_tmp271,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6E3,_tmp6E2,_tag_fat(_tmp270,sizeof(void*),0U));});});}_LL7:;}
# 1476
return _tmp278;default: _LL5: _LL6:
# 1478
 return({struct Cyc_String_pa_PrintArg_struct _tmp274=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0U,({struct _fat_ptr _tmp6E4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp523.f1=_tmp6E4;});_tmp523;});void*_tmp272[1U];_tmp272[0]=& _tmp274;({struct Cyc_Tcenv_Tenv*_tmp6E8=_tmp260;unsigned _tmp6E7=loc;void**_tmp6E6=topt;struct _fat_ptr _tmp6E5=({const char*_tmp273="expecting *, @, or ? type but found %s";_tag_fat(_tmp273,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp6E8,_tmp6E7,_tmp6E6,_tmp6E5,_tag_fat(_tmp272,sizeof(void*),1U));});});}_LL0:;};}
# 1483
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1487
({struct Cyc_Tcenv_Tenv*_tmp6E9=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6E9,0,e);});
# 1489
({int _tmp6EA=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6EA;});{
void*_tmp279=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp27A=_tmp279;enum Cyc_Absyn_AggrKind _tmp294;struct Cyc_List_List*_tmp293;struct Cyc_Absyn_Aggrdecl*_tmp292;struct Cyc_List_List*_tmp291;switch(*((int*)_tmp27A)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp292=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1)->f1).KnownAggr).val;_tmp291=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f2;_LL2: {
# 1492
struct Cyc_Absyn_Aggrfield*_tmp27B=Cyc_Absyn_lookup_decl_field(_tmp292,f);
if(_tmp27B == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp27E=({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0U,({
struct _fat_ptr _tmp6EB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp292->name));_tmp525.f1=_tmp6EB;});_tmp525;});struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,_tmp524.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp524;});void*_tmp27C[2U];_tmp27C[0]=& _tmp27E,_tmp27C[1]=& _tmp27F;({struct Cyc_Tcenv_Tenv*_tmp6EF=te;unsigned _tmp6EE=loc;void**_tmp6ED=topt;struct _fat_ptr _tmp6EC=({const char*_tmp27D="%s has no %s member";_tag_fat(_tmp27D,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp6EF,_tmp6EE,_tmp6ED,_tmp6EC,_tag_fat(_tmp27C,sizeof(void*),2U));});});
# 1497
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp292->impl))->tagged)*is_tagged=1;{
void*t2=_tmp27B->type;
if(_tmp291 != 0){
struct Cyc_List_List*_tmp280=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp292->tvs,_tmp291);
t2=Cyc_Tcutil_substitute(_tmp280,_tmp27B->type);}
# 1505
if(((((int)_tmp292->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp292->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& _tmp27B->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,_tmp526.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp526;});void*_tmp281[1U];_tmp281[0]=& _tmp283;({struct Cyc_Tcenv_Tenv*_tmp6F3=te;unsigned _tmp6F2=loc;void**_tmp6F1=topt;struct _fat_ptr _tmp6F0=({const char*_tmp282="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp282,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6F3,_tmp6F2,_tmp6F1,_tmp6F0,_tag_fat(_tmp281,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp292->impl))->exist_vars != 0){
# 1510
if(!({void*_tmp6F4=t2;Cyc_Unify_unify(_tmp6F4,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,_tmp527.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp527;});void*_tmp284[1U];_tmp284[0]=& _tmp286;({struct Cyc_Tcenv_Tenv*_tmp6F8=te;unsigned _tmp6F7=loc;void**_tmp6F6=topt;struct _fat_ptr _tmp6F5=({const char*_tmp285="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_fat(_tmp285,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp6F8,_tmp6F7,_tmp6F6,_tmp6F5,_tag_fat(_tmp284,sizeof(void*),1U));});});}
# 1513
return t2;};}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp294=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27A)->f1;_tmp293=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27A)->f2;_LL4: {
# 1515
struct Cyc_Absyn_Aggrfield*_tmp287=Cyc_Absyn_lookup_field(_tmp293,f);
if(_tmp287 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0U,_tmp528.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp528;});void*_tmp288[1U];_tmp288[0]=& _tmp28A;({struct Cyc_Tcenv_Tenv*_tmp6FC=te;unsigned _tmp6FB=loc;void**_tmp6FA=topt;struct _fat_ptr _tmp6F9=({const char*_tmp289="type has no %s member";_tag_fat(_tmp289,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp6FC,_tmp6FB,_tmp6FA,_tmp6F9,_tag_fat(_tmp288,sizeof(void*),1U));});});
# 1520
if((((int)_tmp294 == (int)1U && !Cyc_Tcutil_is_bits_only_type(_tmp287->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& _tmp287->requires_clause == 0)
# 1522
return({struct Cyc_String_pa_PrintArg_struct _tmp28D=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,_tmp529.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp529;});void*_tmp28B[1U];_tmp28B[0]=& _tmp28D;({struct Cyc_Tcenv_Tenv*_tmp700=te;unsigned _tmp6FF=loc;void**_tmp6FE=topt;struct _fat_ptr _tmp6FD=({const char*_tmp28C="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp28C,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp700,_tmp6FF,_tmp6FE,_tmp6FD,_tag_fat(_tmp28B,sizeof(void*),1U));});});
return _tmp287->type;}default: _LL5: _LL6:
# 1525
 return({struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0U,({
struct _fat_ptr _tmp701=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp52A.f1=_tmp701;});_tmp52A;});void*_tmp28E[1U];_tmp28E[0]=& _tmp290;({struct Cyc_Tcenv_Tenv*_tmp705=te;unsigned _tmp704=loc;void**_tmp703=topt;struct _fat_ptr _tmp702=({const char*_tmp28F="expecting struct or union, found %s";_tag_fat(_tmp28F,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp705,_tmp704,_tmp703,_tmp702,_tag_fat(_tmp28E,sizeof(void*),1U));});});}_LL0:;};}
# 1531
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1534
({struct Cyc_Tcenv_Tenv*_tmp706=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp706,0,e);});
# 1536
({int _tmp707=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp707;});
{void*_tmp295=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp296=_tmp295;void*_tmp2B9;void*_tmp2B8;void*_tmp2B7;void*_tmp2B6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->tag == 3U){_LL1: _tmp2B9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).elt_type;_tmp2B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).ptr_atts).rgn;_tmp2B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).ptr_atts).bounds;_tmp2B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).ptr_atts).zero_term;_LL2:
# 1539
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2B7);
{void*_tmp297=Cyc_Tcutil_compress(_tmp2B9);void*_tmp298=_tmp297;enum Cyc_Absyn_AggrKind _tmp2B5;struct Cyc_List_List*_tmp2B4;struct Cyc_Absyn_Aggrdecl*_tmp2B3;struct Cyc_List_List*_tmp2B2;switch(*((int*)_tmp298)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp2B3=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298)->f1)->f1).KnownAggr).val;_tmp2B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298)->f2;_LL7: {
# 1542
struct Cyc_Absyn_Aggrfield*_tmp299=Cyc_Absyn_lookup_decl_field(_tmp2B3,f);
if(_tmp299 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp29C=({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0U,({
struct _fat_ptr _tmp708=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_tmp2B3->name));_tmp52C.f1=_tmp708;});_tmp52C;});struct Cyc_String_pa_PrintArg_struct _tmp29D=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0U,_tmp52B.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52B;});void*_tmp29A[2U];_tmp29A[0]=& _tmp29C,_tmp29A[1]=& _tmp29D;({struct Cyc_Tcenv_Tenv*_tmp70C=te;unsigned _tmp70B=loc;void**_tmp70A=topt;struct _fat_ptr _tmp709=({const char*_tmp29B="type %s has no %s member";_tag_fat(_tmp29B,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp70C,_tmp70B,_tmp70A,_tmp709,_tag_fat(_tmp29A,sizeof(void*),2U));});});
# 1547
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B3->impl))->tagged)*is_tagged=1;{
void*t3=_tmp299->type;
if(_tmp2B2 != 0){
struct Cyc_List_List*_tmp29E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2B3->tvs,_tmp2B2);
t3=Cyc_Tcutil_substitute(_tmp29E,_tmp299->type);}{
# 1553
struct Cyc_Absyn_Kind*_tmp29F=Cyc_Tcutil_type_kind(t3);
# 1556
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp29F)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2A0=Cyc_Tcutil_compress(t3);void*_tmp2A1=_tmp2A0;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A1)->tag == 4U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1560
 return({struct Cyc_String_pa_PrintArg_struct _tmp2A4=({struct Cyc_String_pa_PrintArg_struct _tmp52D;_tmp52D.tag=0U,_tmp52D.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52D;});void*_tmp2A2[1U];_tmp2A2[0]=& _tmp2A4;({struct Cyc_Tcenv_Tenv*_tmp710=te;unsigned _tmp70F=loc;void**_tmp70E=topt;struct _fat_ptr _tmp70D=({const char*_tmp2A3="cannot get member %s since its type is abstract";_tag_fat(_tmp2A3,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp710,_tmp70F,_tmp70E,_tmp70D,_tag_fat(_tmp2A2,sizeof(void*),1U));});});}_LLC:;}
# 1565
if(((((int)_tmp2B3->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B3->impl))->tagged)&& !
Cyc_Tcutil_is_bits_only_type(t3))&& !Cyc_Tcenv_in_notreadctxt(te))&& _tmp299->requires_clause == 0)
# 1568
return({struct Cyc_String_pa_PrintArg_struct _tmp2A7=({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0U,_tmp52E.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52E;});void*_tmp2A5[1U];_tmp2A5[0]=& _tmp2A7;({struct Cyc_Tcenv_Tenv*_tmp714=te;unsigned _tmp713=loc;void**_tmp712=topt;struct _fat_ptr _tmp711=({const char*_tmp2A6="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp2A6,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp714,_tmp713,_tmp712,_tmp711,_tag_fat(_tmp2A5,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B3->impl))->exist_vars != 0){
if(!({void*_tmp715=t3;Cyc_Unify_unify(_tmp715,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp52F;_tmp52F.tag=0U,_tmp52F.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52F;});void*_tmp2A8[1U];_tmp2A8[0]=& _tmp2AA;({struct Cyc_Tcenv_Tenv*_tmp719=te;unsigned _tmp718=loc;void**_tmp717=topt;struct _fat_ptr _tmp716=({const char*_tmp2A9="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_fat(_tmp2A9,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp719,_tmp718,_tmp717,_tmp716,_tag_fat(_tmp2A8,sizeof(void*),1U));});});}
# 1575
return t3;};};}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp2B5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp298)->f1;_tmp2B4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp298)->f2;_LL9: {
# 1577
struct Cyc_Absyn_Aggrfield*_tmp2AB=Cyc_Absyn_lookup_field(_tmp2B4,f);
if(_tmp2AB == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,_tmp530.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp530;});void*_tmp2AC[1U];_tmp2AC[0]=& _tmp2AE;({struct Cyc_Tcenv_Tenv*_tmp71D=te;unsigned _tmp71C=loc;void**_tmp71B=topt;struct _fat_ptr _tmp71A=({const char*_tmp2AD="type has no %s field";_tag_fat(_tmp2AD,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp71D,_tmp71C,_tmp71B,_tmp71A,_tag_fat(_tmp2AC,sizeof(void*),1U));});});
# 1582
if(((int)_tmp2B5 == (int)1U && !Cyc_Tcutil_is_bits_only_type(_tmp2AB->type))&& !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B1=({struct Cyc_String_pa_PrintArg_struct _tmp531;_tmp531.tag=0U,_tmp531.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp531;});void*_tmp2AF[1U];_tmp2AF[0]=& _tmp2B1;({struct Cyc_Tcenv_Tenv*_tmp721=te;unsigned _tmp720=loc;void**_tmp71F=topt;struct _fat_ptr _tmp71E=({const char*_tmp2B0="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp2B0,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp721,_tmp720,_tmp71F,_tmp71E,_tag_fat(_tmp2AF,sizeof(void*),1U));});});
return _tmp2AB->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1587
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1590
return({struct Cyc_String_pa_PrintArg_struct _tmp2BC=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,({
struct _fat_ptr _tmp722=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp532.f1=_tmp722;});_tmp532;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BC;({struct Cyc_Tcenv_Tenv*_tmp726=te;unsigned _tmp725=loc;void**_tmp724=topt;struct _fat_ptr _tmp723=({const char*_tmp2BB="expecting struct or union pointer, found %s";_tag_fat(_tmp2BB,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp726,_tmp725,_tmp724,_tmp723,_tag_fat(_tmp2BA,sizeof(void*),1U));});});}
# 1595
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1597
struct _tuple15 _tmp2BD=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2BE=_tmp2BD;unsigned _tmp2CC;int _tmp2CB;_LL1: _tmp2CC=_tmp2BE.f1;_tmp2CB=_tmp2BE.f2;_LL2:;
if(!_tmp2CB)
return({void*_tmp2BF=0U;({struct Cyc_Tcenv_Tenv*_tmp729=te;unsigned _tmp728=loc;struct _fat_ptr _tmp727=({const char*_tmp2C0="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp2C0,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp729,_tmp728,0,_tmp727,_tag_fat(_tmp2BF,sizeof(void*),0U));});});{
# 1601
struct _handler_cons _tmp2C1;_push_handler(& _tmp2C1);{int _tmp2C3=0;if(setjmp(_tmp2C1.handler))_tmp2C3=1;if(!_tmp2C3){
{void*_tmp2C4=(*((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2CC)).f2;_npop_handler(0U);return _tmp2C4;};_pop_handler();}else{void*_tmp2C2=(void*)Cyc_Core_get_exn_thrown();void*_tmp2C5=_tmp2C2;void*_tmp2CA;if(((struct Cyc_List_Nth_exn_struct*)_tmp2C5)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1604
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2C8=({struct Cyc_Int_pa_PrintArg_struct _tmp534;_tmp534.tag=1U,_tmp534.f1=(unsigned long)((int)_tmp2CC);_tmp534;});struct Cyc_Int_pa_PrintArg_struct _tmp2C9=({struct Cyc_Int_pa_PrintArg_struct _tmp533;_tmp533.tag=1U,({
unsigned long _tmp72A=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp533.f1=_tmp72A;});_tmp533;});void*_tmp2C6[2U];_tmp2C6[0]=& _tmp2C8,_tmp2C6[1]=& _tmp2C9;({struct Cyc_Tcenv_Tenv*_tmp72D=te;unsigned _tmp72C=loc;struct _fat_ptr _tmp72B=({const char*_tmp2C7="index is %d but tuple has only %d fields";_tag_fat(_tmp2C7,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp72D,_tmp72C,0,_tmp72B,_tag_fat(_tmp2C6,sizeof(void*),2U));});});}else{_LL6: _tmp2CA=_tmp2C5;_LL7:(int)_rethrow(_tmp2CA);}_LL3:;}};};}
# 1609
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1611
struct Cyc_Tcenv_Tenv*_tmp2CD=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp72E=Cyc_Tcenv_clear_abstract_val_ok(_tmp2CD);Cyc_Tcexp_tcExp(_tmp72E,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp72F=Cyc_Tcenv_clear_abstract_val_ok(_tmp2CD);Cyc_Tcexp_tcExp(_tmp72F,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D0=({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U,({
struct _fat_ptr _tmp730=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp535.f1=_tmp730;});_tmp535;});void*_tmp2CE[1U];_tmp2CE[0]=& _tmp2D0;({struct Cyc_Tcenv_Tenv*_tmp734=_tmp2CD;unsigned _tmp733=e2->loc;void**_tmp732=topt;struct _fat_ptr _tmp731=({const char*_tmp2CF="expecting int subscript, found %s";_tag_fat(_tmp2CF,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp734,_tmp733,_tmp732,_tmp731,_tag_fat(_tmp2CE,sizeof(void*),1U));});});{
# 1621
void*_tmp2D1=t1;struct Cyc_List_List*_tmp2F4;void*_tmp2F3;struct Cyc_Absyn_Tqual _tmp2F2;void*_tmp2F1;void*_tmp2F0;void*_tmp2EF;switch(*((int*)_tmp2D1)){case 3U: _LL1: _tmp2F3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).elt_type;_tmp2F2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).elt_tq;_tmp2F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).rgn;_tmp2F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).bounds;_tmp2EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).zero_term;_LL2:
# 1625
 if(Cyc_Tcutil_force_type2bool(0,_tmp2EF)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp2D2=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp2F0);
if(_tmp2D2 != 0){
struct Cyc_Absyn_Exp*_tmp2D3=_tmp2D2;
struct _tuple15 _tmp2D4=Cyc_Evexp_eval_const_uint_exp(_tmp2D3);struct _tuple15 _tmp2D5=_tmp2D4;unsigned _tmp2E1;int _tmp2E0;_LL8: _tmp2E1=_tmp2D5.f1;_tmp2E0=_tmp2D5.f2;_LL9:;
if(_tmp2E0 && _tmp2E1 == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2D6=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2D7=_tmp2D6;unsigned _tmp2DF;int _tmp2DE;_LLB: _tmp2DF=_tmp2D7.f1;_tmp2DE=_tmp2D7.f2;_LLC:;
if(_tmp2DE){
struct _tuple15 _tmp2D8=Cyc_Evexp_eval_const_uint_exp(_tmp2D3);struct _tuple15 _tmp2D9=_tmp2D8;unsigned _tmp2DD;int _tmp2DC;_LLE: _tmp2DD=_tmp2D9.f1;_tmp2DC=_tmp2D9.f2;_LLF:;
if((_tmp2E0 && _tmp2DD > _tmp2E1)&& _tmp2E1 != (unsigned)1)
({void*_tmp2DA=0U;({unsigned _tmp736=loc;struct _fat_ptr _tmp735=({const char*_tmp2DB="subscript will fail at run-time";_tag_fat(_tmp2DB,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp736,_tmp735,_tag_fat(_tmp2DA,sizeof(void*),0U));});});}}}
# 1641
if(emit_warning)
({void*_tmp2E2=0U;({unsigned _tmp738=e2->loc;struct _fat_ptr _tmp737=({const char*_tmp2E3="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp2E3,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp738,_tmp737,_tag_fat(_tmp2E2,sizeof(void*),0U));});});}else{
# 1645
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2E4=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2E5=_tmp2E4;unsigned _tmp2E7;int _tmp2E6;_LL11: _tmp2E7=_tmp2E5.f1;_tmp2E6=_tmp2E5.f2;_LL12:;
if(_tmp2E6)
# 1650
({unsigned _tmp73B=loc;unsigned _tmp73A=_tmp2E7;void*_tmp739=_tmp2F0;Cyc_Tcutil_check_bound(_tmp73B,_tmp73A,_tmp739,Cyc_Tcenv_abstract_val_ok(_tmp2CD));});}else{
# 1653
if(Cyc_Tcutil_is_bound_one(_tmp2F0)&& !Cyc_Tcutil_force_type2bool(0,_tmp2EF))
({void*_tmp2E8=0U;({unsigned _tmp73D=e1->loc;struct _fat_ptr _tmp73C=({const char*_tmp2E9="subscript applied to pointer to one object";_tag_fat(_tmp2E9,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp73D,_tmp73C,_tag_fat(_tmp2E8,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2F0);}}
# 1658
Cyc_Tcenv_check_rgn_accessible(_tmp2CD,loc,_tmp2F1);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmp2F3),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(_tmp2CD))
({void*_tmp2EA=0U;({unsigned _tmp73F=e1->loc;struct _fat_ptr _tmp73E=({const char*_tmp2EB="can't subscript an abstract pointer";_tag_fat(_tmp2EB,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp73F,_tmp73E,_tag_fat(_tmp2EA,sizeof(void*),0U));});});
return _tmp2F3;case 6U: _LL3: _tmp2F4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D1)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2CD,loc,_tmp2F4,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp2EE=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,({struct _fat_ptr _tmp740=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp536.f1=_tmp740;});_tmp536;});void*_tmp2EC[1U];_tmp2EC[0]=& _tmp2EE;({struct Cyc_Tcenv_Tenv*_tmp744=_tmp2CD;unsigned _tmp743=loc;void**_tmp742=topt;struct _fat_ptr _tmp741=({const char*_tmp2ED="subscript applied to %s";_tag_fat(_tmp2ED,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp744,_tmp743,_tmp742,_tmp741,_tag_fat(_tmp2EC,sizeof(void*),1U));});});}_LL0:;};};}
# 1668
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp2F5=Cyc_Tcutil_compress(*topt);void*_tmp2F6=_tmp2F5;struct Cyc_List_List*_tmp2FA;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F6)->tag == 6U){_LL1: _tmp2FA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F6)->f1;_LL2:
# 1674
 if(({int _tmp745=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FA);_tmp745 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1677
goto _LL0;
# 1679
for(0;es != 0;(es=es->tl,_tmp2FA=_tmp2FA->tl)){
int bogus=0;
void*_tmp2F7=(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp2FA))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp746=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp746,& _tmp2F7,(struct Cyc_Absyn_Exp*)es->hd);});
# 1684
({struct Cyc_RgnOrder_RgnPO*_tmp748=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp747=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp748,_tmp747,(*((struct _tuple16*)_tmp2FA->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));({struct _tuple16*_tmp749=({struct _tuple16*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->f1=(*((struct _tuple16*)_tmp2FA->hd)).f1,_tmp2F8->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2F8;});_tmp2F9->hd=_tmp749;}),_tmp2F9->tl=fields;_tmp2F9;});}
# 1687
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1691
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp74A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp74A,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));({struct _tuple16*_tmp74C=({struct _tuple16*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));({struct Cyc_Absyn_Tqual _tmp74B=Cyc_Absyn_empty_tqual(0U);_tmp2FB->f1=_tmp74B;}),_tmp2FB->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2FB;});_tmp2FC->hd=_tmp74C;}),_tmp2FC->tl=fields;_tmp2FC;});}
# 1696
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->tag=6U,({struct Cyc_List_List*_tmp74D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp2FD->f1=_tmp74D;});_tmp2FD;});}
# 1700
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1705
struct _tuple9*_tmp2FE=targ;void*_tmp300;_LL1: _tmp300=_tmp2FE->f3;_LL2:;
({unsigned _tmp751=loc;struct Cyc_Tcenv_Tenv*_tmp750=te;struct Cyc_List_List*_tmp74F=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp74E=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1706
Cyc_Tctyp_check_type(_tmp751,_tmp750,_tmp74F,_tmp74E,1,0,_tmp300);});
# 1708
({void*_tmp752=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->tag=36U,_tmp2FF->f1=0,_tmp2FF->f2=des;_tmp2FF;});orig_exp->r=_tmp752;});
Cyc_Tcexp_resolve_unresolved_mem(te,loc,& _tmp300,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1721 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1724
void*res_t2;
int _tmp301=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp302=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->tag=0U,({union Cyc_Absyn_Cnst _tmp753=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp301);_tmp318->f1=_tmp753;});_tmp318;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp302,loc);
# 1732
if(zero_term){
struct Cyc_Absyn_Exp*_tmp303=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp301 - 1);
if(!Cyc_Tcutil_is_zero(_tmp303))
({void*_tmp304=0U;({unsigned _tmp755=_tmp303->loc;struct _fat_ptr _tmp754=({const char*_tmp305="zero-terminated array doesn't end with zero.";_tag_fat(_tmp305,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp755,_tmp754,_tag_fat(_tmp304,sizeof(void*),0U));});});}
# 1737
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp758=res;struct Cyc_Absyn_Tqual _tmp757=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1738
struct Cyc_Absyn_Exp*_tmp756=sz_exp;Cyc_Absyn_array_type(_tmp758,_tmp757,_tmp756,
# 1740
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1742
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1745
if(!({struct Cyc_RgnOrder_RgnPO*_tmp75A=Cyc_Tcenv_curr_rgnpo(te);void*_tmp759=res;Cyc_Tcutil_coerce_list(_tmp75A,_tmp759,es);}))
# 1747
({struct Cyc_String_pa_PrintArg_struct _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0U,({
struct _fat_ptr _tmp75B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(res));_tmp537.f1=_tmp75B;});_tmp537;});void*_tmp306[1U];_tmp306[0]=& _tmp308;({unsigned _tmp75D=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp75C=({const char*_tmp307="elements of array do not all have the same type (%s)";_tag_fat(_tmp307,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp75D,_tmp75C,_tag_fat(_tmp306,sizeof(void*),1U));});});
# 1750
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1755
void*_tmp309=(void*)ds->hd;void*_tmp30A=_tmp309;struct Cyc_Absyn_Exp*_tmp317;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp30A)->tag == 1U){_LL1: _LL2:
# 1757
({void*_tmp30B=0U;({unsigned _tmp75F=loc;struct _fat_ptr _tmp75E=({const char*_tmp30C="only array index designators are supported";_tag_fat(_tmp30C,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp75F,_tmp75E,_tag_fat(_tmp30B,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp317=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp30A)->f1;_LL4:
# 1760
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp317);{
struct _tuple15 _tmp30D=Cyc_Evexp_eval_const_uint_exp(_tmp317);struct _tuple15 _tmp30E=_tmp30D;unsigned _tmp316;int _tmp315;_LL6: _tmp316=_tmp30E.f1;_tmp315=_tmp30E.f2;_LL7:;
if(!_tmp315)
({void*_tmp30F=0U;({unsigned _tmp761=_tmp317->loc;struct _fat_ptr _tmp760=({const char*_tmp310="index designator cannot use sizeof or offsetof";_tag_fat(_tmp310,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp761,_tmp760,_tag_fat(_tmp30F,sizeof(void*),0U));});});else{
if(_tmp316 != (unsigned)offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp313=({struct Cyc_Int_pa_PrintArg_struct _tmp539;_tmp539.tag=1U,_tmp539.f1=(unsigned long)offset;_tmp539;});struct Cyc_Int_pa_PrintArg_struct _tmp314=({struct Cyc_Int_pa_PrintArg_struct _tmp538;_tmp538.tag=1U,_tmp538.f1=(unsigned long)((int)_tmp316);_tmp538;});void*_tmp311[2U];_tmp311[0]=& _tmp313,_tmp311[1]=& _tmp314;({unsigned _tmp763=_tmp317->loc;struct _fat_ptr _tmp762=({const char*_tmp312="expecting index designator %d but found %d";_tag_fat(_tmp312,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp763,_tmp762,_tag_fat(_tmp311,sizeof(void*),2U));});});}
# 1767
goto _LL0;};}_LL0:;}}}
# 1771
return res_t2;}
# 1775
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1778
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp319=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp31A=_tmp319;void*_tmp31F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->f2 != 0){_LL1: _tmp31F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->f2)->hd;_LL2:
# 1783
 if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp31B=({void*_tmp764=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp764,Cyc_Absyn_valueof_exp(_tmp31F,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp31B->topt=bound->topt;
# 1788
bound=_tmp31B;}
# 1790
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1792
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U,({
struct _fat_ptr _tmp765=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp53A.f1=_tmp765;});_tmp53A;});void*_tmp31C[1U];_tmp31C[0]=& _tmp31E;({unsigned _tmp767=bound->loc;struct _fat_ptr _tmp766=({const char*_tmp31D="expecting unsigned int, found %s";_tag_fat(_tmp31D,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp767,_tmp766,_tag_fat(_tmp31C,sizeof(void*),1U));});});}_LL0:;}
# 1797
if(!(vd->tq).real_const)
({void*_tmp320=0U;({struct _fat_ptr _tmp768=({const char*_tmp321="comprehension index variable is not declared const!";_tag_fat(_tmp321,sizeof(char),52U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp768,_tag_fat(_tmp320,sizeof(void*),0U));});});
# 1800
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
void**_tmp322=0;
struct Cyc_Absyn_Tqual*_tmp323=0;
void**_tmp324=0;
# 1806
if(topt != 0){
void*_tmp325=Cyc_Tcutil_compress(*topt);void*_tmp326=_tmp325;void*_tmp331;struct Cyc_Absyn_Tqual _tmp330;struct Cyc_Absyn_Exp*_tmp32F;void*_tmp32E;struct Cyc_Absyn_PtrInfo _tmp32D;switch(*((int*)_tmp326)){case 3U: _LL6: _tmp32D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1;_LL7:
# 1809
 _tmp322=({void**_tmp327=_cycalloc(sizeof(*_tmp327));*_tmp327=_tmp32D.elt_type;_tmp327;});
_tmp323=({struct Cyc_Absyn_Tqual*_tmp328=_cycalloc(sizeof(*_tmp328));*_tmp328=_tmp32D.elt_tq;_tmp328;});
_tmp324=({void**_tmp329=_cycalloc(sizeof(*_tmp329));*_tmp329=(_tmp32D.ptr_atts).zero_term;_tmp329;});
goto _LL5;case 4U: _LL8: _tmp331=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).elt_type;_tmp330=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).tq;_tmp32F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).num_elts;_tmp32E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).zero_term;_LL9:
# 1814
 _tmp322=({void**_tmp32A=_cycalloc(sizeof(*_tmp32A));*_tmp32A=_tmp331;_tmp32A;});
_tmp323=({struct Cyc_Absyn_Tqual*_tmp32B=_cycalloc(sizeof(*_tmp32B));*_tmp32B=_tmp330;_tmp32B;});
_tmp324=({void**_tmp32C=_cycalloc(sizeof(*_tmp32C));*_tmp32C=_tmp32E;_tmp32C;});
goto _LL5;default: _LLA: _LLB:
# 1819
 goto _LL5;}_LL5:;}{
# 1822
void*t=Cyc_Tcexp_tcExp(te,_tmp322,body);
# 1824
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp332=0U;({unsigned _tmp76A=body->loc;struct _fat_ptr _tmp769=({const char*_tmp333="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp333,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp76A,_tmp769,_tag_fat(_tmp332,sizeof(void*),0U));});});
if(te->le == 0){
# 1828
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp334=0U;({unsigned _tmp76C=bound->loc;struct _fat_ptr _tmp76B=({const char*_tmp335="bound is not constant";_tag_fat(_tmp335,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp76C,_tmp76B,_tag_fat(_tmp334,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp336=0U;({unsigned _tmp76E=bound->loc;struct _fat_ptr _tmp76D=({const char*_tmp337="body is not constant";_tag_fat(_tmp337,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp76E,_tmp76D,_tag_fat(_tmp336,sizeof(void*),0U));});});}
# 1833
if(_tmp324 != 0 && Cyc_Tcutil_force_type2bool(0,*_tmp324)){
# 1836
struct Cyc_Absyn_Exp*_tmp338=Cyc_Absyn_uint_exp(1U,0U);_tmp338->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,_tmp338,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1840
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt))&& !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp339=0U;({unsigned _tmp770=body->loc;struct _fat_ptr _tmp76F=({const char*_tmp33A="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp33A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp770,_tmp76F,_tag_fat(_tmp339,sizeof(void*),0U));});});{
# 1844
void*_tmp33B=({void*_tmp773=t;struct Cyc_Absyn_Tqual _tmp772=_tmp323 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp323;struct Cyc_Absyn_Exp*_tmp771=bound;Cyc_Absyn_array_type(_tmp773,_tmp772,_tmp771,
_tmp324 == 0?Cyc_Absyn_false_type:*_tmp324,0U);});
return _tmp33B;};};};}
# 1850
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1853
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp33C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp33D=_tmp33C;void*_tmp342;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->f2 != 0){_LL1: _tmp342=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->f2)->hd;_LL2:
# 1858
 if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp33E=({void*_tmp774=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp774,Cyc_Absyn_valueof_exp(_tmp342,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp33E->topt=bound->topt;
# 1863
bound=_tmp33E;}
# 1865
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1867
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp53B;_tmp53B.tag=0U,({
struct _fat_ptr _tmp775=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp53B.f1=_tmp775;});_tmp53B;});void*_tmp33F[1U];_tmp33F[0]=& _tmp341;({unsigned _tmp777=bound->loc;struct _fat_ptr _tmp776=({const char*_tmp340="expecting unsigned int, found %s";_tag_fat(_tmp340,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp777,_tmp776,_tag_fat(_tmp33F,sizeof(void*),1U));});});}_LL0:;}{
# 1872
void**_tmp343=0;
struct Cyc_Absyn_Tqual*_tmp344=0;
void**_tmp345=0;
# 1876
if(topt != 0){
void*_tmp346=Cyc_Tcutil_compress(*topt);void*_tmp347=_tmp346;void*_tmp352;struct Cyc_Absyn_Tqual _tmp351;struct Cyc_Absyn_Exp*_tmp350;void*_tmp34F;struct Cyc_Absyn_PtrInfo _tmp34E;switch(*((int*)_tmp347)){case 3U: _LL6: _tmp34E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp347)->f1;_LL7:
# 1879
 _tmp343=({void**_tmp348=_cycalloc(sizeof(*_tmp348));*_tmp348=_tmp34E.elt_type;_tmp348;});
_tmp344=({struct Cyc_Absyn_Tqual*_tmp349=_cycalloc(sizeof(*_tmp349));*_tmp349=_tmp34E.elt_tq;_tmp349;});
_tmp345=({void**_tmp34A=_cycalloc(sizeof(*_tmp34A));*_tmp34A=(_tmp34E.ptr_atts).zero_term;_tmp34A;});
goto _LL5;case 4U: _LL8: _tmp352=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).elt_type;_tmp351=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).tq;_tmp350=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).num_elts;_tmp34F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).zero_term;_LL9:
# 1884
 _tmp343=({void**_tmp34B=_cycalloc(sizeof(*_tmp34B));*_tmp34B=_tmp352;_tmp34B;});
_tmp344=({struct Cyc_Absyn_Tqual*_tmp34C=_cycalloc(sizeof(*_tmp34C));*_tmp34C=_tmp351;_tmp34C;});
_tmp345=({void**_tmp34D=_cycalloc(sizeof(*_tmp34D));*_tmp34D=_tmp34F;_tmp34D;});
goto _LL5;default: _LLA: _LLB:
# 1889
 goto _LL5;}_LL5:;}
# 1892
({unsigned _tmp77A=loc;struct Cyc_Tcenv_Tenv*_tmp779=te;struct Cyc_List_List*_tmp778=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp77A,_tmp779,_tmp778,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp343 != 0)Cyc_Unify_unify(*_tmp343,t);
# 1895
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp353=0U;({unsigned _tmp77C=bound->loc;struct _fat_ptr _tmp77B=({const char*_tmp354="bound is not constant";_tag_fat(_tmp354,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp77C,_tmp77B,_tag_fat(_tmp353,sizeof(void*),0U));});});}
# 1899
if(_tmp345 != 0 && Cyc_Tcutil_force_type2bool(0,*_tmp345)){
# 1902
struct Cyc_Absyn_Exp*_tmp355=Cyc_Absyn_uint_exp(1U,0U);_tmp355->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,_tmp355,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1906
({void*_tmp356=0U;({unsigned _tmp77E=loc;struct _fat_ptr _tmp77D=({const char*_tmp357="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp357,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp77E,_tmp77D,_tag_fat(_tmp356,sizeof(void*),0U));});});}{
# 1909
void*_tmp358=({void*_tmp781=t;struct Cyc_Absyn_Tqual _tmp780=_tmp344 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp344;struct Cyc_Absyn_Exp*_tmp77F=bound;Cyc_Absyn_array_type(_tmp781,_tmp780,_tmp77F,
_tmp345 == 0?Cyc_Absyn_false_type:*_tmp345,0U);});
return _tmp358;};};}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1924 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1928
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp359=_cycalloc(sizeof(*_tmp359));*_tmp359=ad;_tmp359;});}else{
# 1934
{struct _handler_cons _tmp35A;_push_handler(& _tmp35A);{int _tmp35C=0;if(setjmp(_tmp35A.handler))_tmp35C=1;if(!_tmp35C){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1934
;_pop_handler();}else{void*_tmp35B=(void*)Cyc_Core_get_exn_thrown();void*_tmp35D=_tmp35B;void*_tmp361;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp35D)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1937
({struct Cyc_String_pa_PrintArg_struct _tmp360=({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0U,({struct _fat_ptr _tmp782=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp53C.f1=_tmp782;});_tmp53C;});void*_tmp35E[1U];_tmp35E[0]=& _tmp360;({unsigned _tmp784=loc;struct _fat_ptr _tmp783=({const char*_tmp35F="unbound struct/union name %s";_tag_fat(_tmp35F,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp784,_tmp783,_tag_fat(_tmp35E,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp361=_tmp35D;_LL4:(int)_rethrow(_tmp361);}_LL0:;}};}
# 1940
*ad_opt=ad;
*tn=ad->name;}
# 1943
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,({struct _fat_ptr _tmp785=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp365="struct";_tag_fat(_tmp365,sizeof(char),7U);}):({const char*_tmp366="union";_tag_fat(_tmp366,sizeof(char),6U);}));_tmp53D.f1=_tmp785;});_tmp53D;});void*_tmp362[1U];_tmp362[0]=& _tmp364;({unsigned _tmp787=loc;struct _fat_ptr _tmp786=({const char*_tmp363="can't construct abstract %s";_tag_fat(_tmp363,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp787,_tmp786,_tag_fat(_tmp362,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1951
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp367=Cyc_Tcenv_new_status(te);
if((int)_tmp367 == (int)1U)
te2=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);else{
# 1960
te2=Cyc_Tcenv_set_new_status(_tmp367,te);}{
# 1962
struct _tuple12 _tmp368=({struct _tuple12 _tmp543;({struct Cyc_List_List*_tmp788=Cyc_Tcenv_lookup_type_vars(te2);_tmp543.f1=_tmp788;}),_tmp543.f2=Cyc_Core_heap_region;_tmp543;});
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp368,ad->tvs);
struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp368,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp369);
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp36A);
struct Cyc_List_List*_tmp36D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp369,_tmp36A);
void*res_typ;
# 1973
if(topt != 0){
void*_tmp36E=Cyc_Tcutil_compress(*topt);void*_tmp36F=_tmp36E;struct Cyc_Absyn_Aggrdecl**_tmp372;struct Cyc_List_List*_tmp371;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp372=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f1)->f1).KnownAggr).val;_tmp371=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f2;_LL7:
# 1976
 if(*_tmp372 == *adptr){
{struct Cyc_List_List*_tmp370=_tmp36B;for(0;_tmp370 != 0 && _tmp371 != 0;(
_tmp370=_tmp370->tl,_tmp371=_tmp371->tl)){
Cyc_Unify_unify((void*)_tmp370->hd,(void*)_tmp371->hd);}}
# 1981
res_typ=*topt;
goto _LL5;}
# 1984
goto _LL9;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1986
 res_typ=({union Cyc_Absyn_AggrInfo _tmp789=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp789,_tmp36B);});}_LL5:;}else{
# 1989
res_typ=({union Cyc_Absyn_AggrInfo _tmp78A=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp78A,_tmp36B);});}{
# 1992
struct Cyc_List_List*_tmp373=*ts;
struct Cyc_List_List*_tmp374=_tmp36C;
while(_tmp373 != 0 && _tmp374 != 0){
# 1996
({unsigned _tmp78D=loc;struct Cyc_Tcenv_Tenv*_tmp78C=te2;struct Cyc_List_List*_tmp78B=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp78D,_tmp78C,_tmp78B,& Cyc_Tcutil_ak,1,0,(void*)_tmp373->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp373->hd);
Cyc_Unify_unify((void*)_tmp373->hd,(void*)_tmp374->hd);
_tmp373=_tmp373->tl;
_tmp374=_tmp374->tl;}
# 2002
if(_tmp373 != 0)
({void*_tmp375=0U;({unsigned _tmp78F=loc;struct _fat_ptr _tmp78E=({const char*_tmp376="too many explicit witness types";_tag_fat(_tmp376,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp78F,_tmp78E,_tag_fat(_tmp375,sizeof(void*),0U));});});
# 2005
*ts=_tmp36C;{
# 2008
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple18*_tmp377=(struct _tuple18*)fields->hd;struct _tuple18*_tmp378=_tmp377;struct Cyc_Absyn_Aggrfield*_tmp384;struct Cyc_Absyn_Exp*_tmp383;_LLB: _tmp384=_tmp378->f1;_tmp383=_tmp378->f2;_LLC:;{
void*_tmp379=Cyc_Tcutil_substitute(_tmp36D,_tmp384->type);
({struct Cyc_Tcenv_Tenv*_tmp790=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp790,& _tmp379,_tmp383);});
# 2019
if(!({struct Cyc_RgnOrder_RgnPO*_tmp792=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp791=_tmp383;Cyc_Tcutil_coerce_arg(_tmp792,_tmp791,_tmp379,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp37C=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,_tmp542.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp384->name);_tmp542;});struct Cyc_String_pa_PrintArg_struct _tmp37D=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,({
struct _fat_ptr _tmp793=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp381="struct";_tag_fat(_tmp381,sizeof(char),7U);}):({const char*_tmp382="union";_tag_fat(_tmp382,sizeof(char),6U);}));_tmp541.f1=_tmp793;});_tmp541;});struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,({
struct _fat_ptr _tmp794=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp540.f1=_tmp794;});_tmp540;});struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,({struct _fat_ptr _tmp795=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp379));_tmp53F.f1=_tmp795;});_tmp53F;});struct Cyc_String_pa_PrintArg_struct _tmp380=({struct Cyc_String_pa_PrintArg_struct _tmp53E;_tmp53E.tag=0U,({
struct _fat_ptr _tmp796=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp383->topt)));_tmp53E.f1=_tmp796;});_tmp53E;});void*_tmp37A[5U];_tmp37A[0]=& _tmp37C,_tmp37A[1]=& _tmp37D,_tmp37A[2]=& _tmp37E,_tmp37A[3]=& _tmp37F,_tmp37A[4]=& _tmp380;({unsigned _tmp798=_tmp383->loc;struct _fat_ptr _tmp797=({const char*_tmp37B="field %s of %s %s expects type %s != %s";_tag_fat(_tmp37B,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp798,_tmp797,_tag_fat(_tmp37A,sizeof(void*),5U));});});
Cyc_Unify_explain_failure();}};}{
# 2028
struct Cyc_List_List*_tmp385=0;
{struct Cyc_List_List*_tmp386=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp386 != 0;_tmp386=_tmp386->tl){
_tmp385=({struct Cyc_List_List*_tmp388=_cycalloc(sizeof(*_tmp388));({struct _tuple0*_tmp79B=({struct _tuple0*_tmp387=_cycalloc(sizeof(*_tmp387));({void*_tmp79A=Cyc_Tcutil_substitute(_tmp36D,(*((struct _tuple0*)_tmp386->hd)).f1);_tmp387->f1=_tmp79A;}),({
void*_tmp799=Cyc_Tcutil_substitute(_tmp36D,(*((struct _tuple0*)_tmp386->hd)).f2);_tmp387->f2=_tmp799;});_tmp387;});
# 2030
_tmp388->hd=_tmp79B;}),_tmp388->tl=_tmp385;_tmp388;});}}
# 2033
_tmp385=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp385);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp385);
return res_typ;};};};};};}
# 2039
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 2041
{void*_tmp389=Cyc_Tcutil_compress(ts);void*_tmp38A=_tmp389;enum Cyc_Absyn_AggrKind _tmp399;struct Cyc_List_List*_tmp398;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38A)->tag == 7U){_LL1: _tmp399=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38A)->f1;_tmp398=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38A)->f2;_LL2:
# 2043
 if((int)_tmp399 == (int)1U)
({void*_tmp38B=0U;({unsigned _tmp79D=loc;struct _fat_ptr _tmp79C=({const char*_tmp38C="expecting struct but found union";_tag_fat(_tmp38C,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp79D,_tmp79C,_tag_fat(_tmp38B,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp398);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple18*_tmp38D=(struct _tuple18*)fields->hd;struct _tuple18*_tmp38E=_tmp38D;struct Cyc_Absyn_Aggrfield*_tmp395;struct Cyc_Absyn_Exp*_tmp394;_LL6: _tmp395=_tmp38E->f1;_tmp394=_tmp38E->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp79F=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp79E=& _tmp395->type;Cyc_Tcexp_tcExpInitializer(_tmp79F,_tmp79E,_tmp394);});
# 2052
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7A1=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7A0=_tmp394;Cyc_Tcutil_coerce_arg(_tmp7A1,_tmp7A0,_tmp395->type,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp391=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,_tmp546.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp395->name);_tmp546;});struct Cyc_String_pa_PrintArg_struct _tmp392=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,({
struct _fat_ptr _tmp7A2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp395->type));_tmp545.f1=_tmp7A2;});_tmp545;});struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,({
struct _fat_ptr _tmp7A3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp394->topt)));_tmp544.f1=_tmp7A3;});_tmp544;});void*_tmp38F[3U];_tmp38F[0]=& _tmp391,_tmp38F[1]=& _tmp392,_tmp38F[2]=& _tmp393;({unsigned _tmp7A5=_tmp394->loc;struct _fat_ptr _tmp7A4=({const char*_tmp390="field %s of struct expects type %s != %s";_tag_fat(_tmp390,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp7A5,_tmp7A4,_tag_fat(_tmp38F,sizeof(void*),3U));});});
Cyc_Unify_explain_failure();}}
# 2059
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp396=0U;({struct _fat_ptr _tmp7A6=({const char*_tmp397="tcAnonStruct: wrong type";_tag_fat(_tmp397,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp7A6,_tag_fat(_tmp396,sizeof(void*),0U));});});}_LL0:;}
# 2062
return ts;}
# 2066
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2069
struct _tuple12 _tmp39A=({struct _tuple12 _tmp54C;({struct Cyc_List_List*_tmp7A7=Cyc_Tcenv_lookup_type_vars(te);_tmp54C.f1=_tmp7A7;}),_tmp54C.f2=Cyc_Core_heap_region;_tmp54C;});
struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp39A,tud->tvs);
struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp39B);
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp7A8=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp7A8,_tmp39C);});
# 2074
if(topt != 0){
void*_tmp39D=Cyc_Tcutil_compress(*topt);void*_tmp39E=_tmp39D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2080
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple16*)ts->hd)).f2;
if(_tmp39B != 0)t=Cyc_Tcutil_substitute(_tmp39B,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7AA=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7A9=e;Cyc_Tcutil_coerce_arg(_tmp7AA,_tmp7A9,t,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp3A1=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,({
# 2090
struct _fat_ptr _tmp7AB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp549.f1=_tmp7AB;});_tmp549;});struct Cyc_String_pa_PrintArg_struct _tmp3A2=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,({struct _fat_ptr _tmp7AC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp548.f1=_tmp7AC;});_tmp548;});struct Cyc_String_pa_PrintArg_struct _tmp3A3=({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0U,({
struct _fat_ptr _tmp7AD=(struct _fat_ptr)((struct _fat_ptr)(e->topt == 0?(struct _fat_ptr)({const char*_tmp3A4="?";_tag_fat(_tmp3A4,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp547.f1=_tmp7AD;});_tmp547;});void*_tmp39F[3U];_tmp39F[0]=& _tmp3A1,_tmp39F[1]=& _tmp3A2,_tmp39F[2]=& _tmp3A3;({unsigned _tmp7AF=e->loc;struct _fat_ptr _tmp7AE=({const char*_tmp3A0="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_fat(_tmp3A0,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp7AF,_tmp7AE,_tag_fat(_tmp39F,sizeof(void*),3U));});});
Cyc_Unify_explain_failure();}}
# 2095
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3A7=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,({
# 2098
struct _fat_ptr _tmp7B0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp54A.f1=_tmp7B0;});_tmp54A;});void*_tmp3A5[1U];_tmp3A5[0]=& _tmp3A7;({struct Cyc_Tcenv_Tenv*_tmp7B4=te;unsigned _tmp7B3=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7B2=topt;struct _fat_ptr _tmp7B1=({const char*_tmp3A6="too many arguments for datatype constructor %s";_tag_fat(_tmp3A6,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp7B4,_tmp7B3,_tmp7B2,_tmp7B1,_tag_fat(_tmp3A5,sizeof(void*),1U));});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3AA=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,({
struct _fat_ptr _tmp7B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp54B.f1=_tmp7B5;});_tmp54B;});void*_tmp3A8[1U];_tmp3A8[0]=& _tmp3AA;({struct Cyc_Tcenv_Tenv*_tmp7B9=te;unsigned _tmp7B8=loc;void**_tmp7B7=topt;struct _fat_ptr _tmp7B6=({const char*_tmp3A9="too few arguments for datatype constructor %s";_tag_fat(_tmp3A9,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7B9,_tmp7B8,_tmp7B7,_tmp7B6,_tag_fat(_tmp3A8,sizeof(void*),1U));});});
return res;};}
# 2106
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 2108
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))return 1;
# 2110
if(topt != 0){
void*_tmp3AB=Cyc_Tcutil_compress(*topt);void*_tmp3AC=_tmp3AB;void*_tmp3AD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->tag == 3U){_LL1: _tmp3AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).elt_type;_LL2:
# 2113
 Cyc_Unify_unify(_tmp3AD,t);
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))return 1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2119
return 0;}
# 2126
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2128
enum Cyc_Absyn_AliasQual _tmp3AE=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3AF=_tmp3AE;if(_tmp3AF == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 2134
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2138
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 2142
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3B0=Cyc_Tcutil_compress(handle_type);void*_tmp3B1=_tmp3B0;void*_tmp3B5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f2 != 0){_LL1: _tmp3B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f2)->hd;_LL2:
# 2147
 rgn=_tmp3B5;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2151
({struct Cyc_String_pa_PrintArg_struct _tmp3B4=({struct Cyc_String_pa_PrintArg_struct _tmp54D;_tmp54D.tag=0U,({
struct _fat_ptr _tmp7BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp54D.f1=_tmp7BA;});_tmp54D;});void*_tmp3B2[1U];_tmp3B2[0]=& _tmp3B4;({unsigned _tmp7BC=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _fat_ptr _tmp7BB=({const char*_tmp3B3="expecting region_t type but found %s";_tag_fat(_tmp3B3,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7BC,_tmp7BB,_tag_fat(_tmp3B2,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2158
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp7BD=Cyc_Absyn_uniquergn_exp();*ropt=_tmp7BD;});}}}
# 2165
({struct Cyc_Tcenv_Tenv*_tmp7BE=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7BE,& Cyc_Absyn_uint_type,*e);});{
# 2174 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3B6=0U;({struct _fat_ptr _tmp7BF=({const char*_tmp3B7="calloc with empty type";_tag_fat(_tmp3B7,sizeof(char),23U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp7BF,_tag_fat(_tmp3B6,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp7C2=loc;struct Cyc_Tcenv_Tenv*_tmp7C1=te;struct Cyc_List_List*_tmp7C0=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7C2,_tmp7C1,_tmp7C0,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp3BA=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,({struct _fat_ptr _tmp7C3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(elt_type));_tmp54E.f1=_tmp7C3;});_tmp54E;});void*_tmp3B8[1U];_tmp3B8[0]=& _tmp3BA;({unsigned _tmp7C5=loc;struct _fat_ptr _tmp7C4=({const char*_tmp3B9="calloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3B9,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp7C5,_tmp7C4,_tag_fat(_tmp3B8,sizeof(void*),1U));});});
num_elts=*e;
one_elt=0;}else{
# 2187
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3BB=er;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D5;void*_tmp3D4;switch(*((int*)_tmp3BB)){case 17U: _LL6: _tmp3D4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3BB)->f1;_LL7:
# 2191
 elt_type=_tmp3D4;
({void**_tmp7C6=({void**_tmp3BC=_cycalloc(sizeof(*_tmp3BC));*_tmp3BC=elt_type;_tmp3BC;});*t=_tmp7C6;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->tl)->tl == 0){_LL8: _tmp3D6=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->hd;_tmp3D5=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->tl)->hd;_LL9:
# 2198
{struct _tuple0 _tmp3BD=({struct _tuple0 _tmp551;_tmp551.f1=_tmp3D6->r,_tmp551.f2=_tmp3D5->r;_tmp551;});struct _tuple0 _tmp3BE=_tmp3BD;void*_tmp3C8;void*_tmp3C7;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3BE.f1)->tag == 17U){_LLD: _tmp3C7=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3BE.f1)->f1;_LLE:
# 2201
 if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C7)){
# 2204
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,_tmp3C7))
({struct Cyc_String_pa_PrintArg_struct _tmp3C1=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp7C7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp3C7));_tmp54F.f1=_tmp7C7;});_tmp54F;});void*_tmp3BF[1U];_tmp3BF[0]=& _tmp3C1;({unsigned _tmp7C9=loc;struct _fat_ptr _tmp7C8=({const char*_tmp3C0="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3C0,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp7C9,_tmp7C8,_tag_fat(_tmp3BF,sizeof(void*),1U));});});else{
# 2207
*is_calloc=1;}}
# 2210
elt_type=_tmp3C7;
({void**_tmp7CA=({void**_tmp3C2=_cycalloc(sizeof(*_tmp3C2));*_tmp3C2=elt_type;_tmp3C2;});*t=_tmp7CA;});
num_elts=_tmp3D5;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3BE.f2)->tag == 17U){_LLF: _tmp3C8=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3BE.f2)->f1;_LL10:
# 2217
 if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C8)){
# 2220
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,_tmp3C8))
({struct Cyc_String_pa_PrintArg_struct _tmp3C5=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({struct _fat_ptr _tmp7CB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp3C8));_tmp550.f1=_tmp7CB;});_tmp550;});void*_tmp3C3[1U];_tmp3C3[0]=& _tmp3C5;({unsigned _tmp7CD=loc;struct _fat_ptr _tmp7CC=({const char*_tmp3C4="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3C4,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp7CD,_tmp7CC,_tag_fat(_tmp3C3,sizeof(void*),1U));});});else{
# 2223
*is_calloc=1;}}
# 2226
elt_type=_tmp3C8;
({void**_tmp7CE=({void**_tmp3C6=_cycalloc(sizeof(*_tmp3C6));*_tmp3C6=elt_type;_tmp3C6;});*t=_tmp7CE;});
num_elts=_tmp3D6;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2233
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2235
 No_sizeof: {
# 2238
struct Cyc_Absyn_Exp*_tmp3C9=*e;
{void*_tmp3CA=_tmp3C9->r;void*_tmp3CB=_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CC;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CB)->tag == 14U){_LL14: _tmp3CC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CB)->f2;_LL15:
 _tmp3C9=_tmp3CC;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2243
{void*_tmp3CD=Cyc_Tcutil_compress((void*)_check_null(_tmp3C9->topt));void*_tmp3CE=_tmp3CD;void*_tmp3D2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f2 != 0){_LL19: _tmp3D2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f2)->hd;_LL1A:
# 2245
{void*_tmp3CF=Cyc_Tcutil_compress(_tmp3D2);void*_tmp3D0=_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D0)->tag == 9U){_LL1E: _tmp3D1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D0)->f1;_LL1F:
# 2247
 er=_tmp3D1->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2250
goto _LL18;}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2253
elt_type=Cyc_Absyn_char_type;
({void**_tmp7CF=({void**_tmp3D3=_cycalloc(sizeof(*_tmp3D3));*_tmp3D3=elt_type;_tmp3D3;});*t=_tmp7CF;});
num_elts=*e;
one_elt=0;}
# 2258
goto _LL5;}_LL5:;}}
# 2262
*is_fat=!one_elt;
# 2265
{void*_tmp3D7=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp3DA=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->f1).KnownAggr).val;_LL24:
# 2267
 if(_tmp3DA->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DA->impl))->exist_vars != 0)
({void*_tmp3D8=0U;({unsigned _tmp7D1=loc;struct _fat_ptr _tmp7D0=({const char*_tmp3D9="malloc with existential types not yet implemented";_tag_fat(_tmp3D9,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp7D1,_tmp7D0,_tag_fat(_tmp3D8,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2274
void*(*_tmp3DB)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;
void*_tmp3DC=Cyc_Absyn_false_type;
if(topt != 0){
void*_tmp3DD=Cyc_Tcutil_compress(*topt);void*_tmp3DE=_tmp3DD;void*_tmp3EA;void*_tmp3E9;void*_tmp3E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->tag == 3U){_LL28: _tmp3EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->f1).ptr_atts).nullable;_tmp3E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->f1).ptr_atts).bounds;_tmp3E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->f1).ptr_atts).zero_term;_LL29:
# 2279
 _tmp3DC=_tmp3E8;
if(Cyc_Tcutil_force_type2bool(0,_tmp3EA))
_tmp3DB=Cyc_Absyn_star_type;
# 2284
if(Cyc_Tcutil_force_type2bool(0,_tmp3E8)&& !(*is_calloc)){
({void*_tmp3DF=0U;({unsigned _tmp7D3=loc;struct _fat_ptr _tmp7D2=({const char*_tmp3E0="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp3E0,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp7D3,_tmp7D2,_tag_fat(_tmp3DF,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2290
struct Cyc_Absyn_Exp*_tmp3E1=({void*_tmp7D4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7D4,_tmp3E9);});
if(_tmp3E1 != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp3E2=_tmp3E1;
int _tmp3E3=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp3E3 && Cyc_Evexp_same_const_exp(_tmp3E2,num_elts)){
*is_fat=0;
return({void*_tmp7D8=elt_type;void*_tmp7D7=rgn;struct Cyc_Absyn_Tqual _tmp7D6=Cyc_Absyn_empty_tqual(0U);void*_tmp7D5=_tmp3E9;Cyc_Absyn_atb_type(_tmp7D8,_tmp7D7,_tmp7D6,_tmp7D5,_tmp3DC);});}{
# 2298
void*_tmp3E4=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp3E5=_tmp3E4;void*_tmp3E7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E5)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E5)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E5)->f2 != 0){_LL2D: _tmp3E7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E5)->f2)->hd;_LL2E: {
# 2300
struct Cyc_Absyn_Exp*_tmp3E6=({void*_tmp7D9=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7D9,Cyc_Absyn_valueof_exp(_tmp3E7,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2302
if(Cyc_Evexp_same_const_exp(_tmp3E6,_tmp3E2)){
*is_fat=0;
return({void*_tmp7DD=elt_type;void*_tmp7DC=rgn;struct Cyc_Absyn_Tqual _tmp7DB=Cyc_Absyn_empty_tqual(0U);void*_tmp7DA=_tmp3E9;Cyc_Absyn_atb_type(_tmp7DD,_tmp7DC,_tmp7DB,_tmp7DA,_tmp3DC);});}
# 2306
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;};}
# 2310
goto _LL27;};}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2313
if(!one_elt)_tmp3DB=Cyc_Absyn_fatptr_type;
return({void*(*_tmp7E1)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp3DB;void*_tmp7E0=elt_type;void*_tmp7DF=rgn;struct Cyc_Absyn_Tqual _tmp7DE=Cyc_Absyn_empty_tqual(0U);_tmp7E1(_tmp7E0,_tmp7DF,_tmp7DE,_tmp3DC);});};};}
# 2318
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2320
struct Cyc_Tcenv_Tenv*_tmp3EB=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp3EB,0,e1);{
void*_tmp3EC=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp3EB,& _tmp3EC,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2327
{void*_tmp3ED=Cyc_Tcutil_compress(t1);void*_tmp3EE=_tmp3ED;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EE)->tag == 4U){_LL1: _LL2:
({void*_tmp3EF=0U;({unsigned _tmp7E3=loc;struct _fat_ptr _tmp7E2=({const char*_tmp3F0="cannot assign to an array";_tag_fat(_tmp3F0,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp7E3,_tmp7E2,_tag_fat(_tmp3EF,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2332
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp3F1=0U;({unsigned _tmp7E5=loc;struct _fat_ptr _tmp7E4=({const char*_tmp3F2="Swap not allowed for non-pointer or non-word-sized types.";_tag_fat(_tmp3F2,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp7E5,_tmp7E4,_tag_fat(_tmp3F1,sizeof(void*),0U));});});
# 2336
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp3F3=0U;({struct Cyc_Tcenv_Tenv*_tmp7E9=te;unsigned _tmp7E8=e1->loc;void**_tmp7E7=topt;struct _fat_ptr _tmp7E6=({const char*_tmp3F4="swap non-lvalue";_tag_fat(_tmp3F4,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7E9,_tmp7E8,_tmp7E7,_tmp7E6,_tag_fat(_tmp3F3,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp3F5=0U;({struct Cyc_Tcenv_Tenv*_tmp7ED=te;unsigned _tmp7EC=e2->loc;void**_tmp7EB=topt;struct _fat_ptr _tmp7EA=({const char*_tmp3F6="swap non-lvalue";_tag_fat(_tmp3F6,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7ED,_tmp7EC,_tmp7EB,_tmp7EA,_tag_fat(_tmp3F5,sizeof(void*),0U));});});
# 2341
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2)){
void*_tmp3F7=({struct Cyc_String_pa_PrintArg_struct _tmp3FA=({struct Cyc_String_pa_PrintArg_struct _tmp553;_tmp553.tag=0U,({
struct _fat_ptr _tmp7EE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp553.f1=_tmp7EE;});_tmp553;});struct Cyc_String_pa_PrintArg_struct _tmp3FB=({struct Cyc_String_pa_PrintArg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp7EF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp552.f1=_tmp7EF;});_tmp552;});void*_tmp3F8[2U];_tmp3F8[0]=& _tmp3FA,_tmp3F8[1]=& _tmp3FB;({struct Cyc_Tcenv_Tenv*_tmp7F3=te;unsigned _tmp7F2=loc;void**_tmp7F1=topt;struct _fat_ptr _tmp7F0=({const char*_tmp3F9="type mismatch: %s != %s";_tag_fat(_tmp3F9,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp7F3,_tmp7F2,_tmp7F1,_tmp7F0,_tag_fat(_tmp3F8,sizeof(void*),2U));});});
return _tmp3F7;}
# 2348
return Cyc_Absyn_void_type;};};}
# 2353
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp7F4=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp7F4,s,1);});
# 2356
while(1){
void*_tmp3FC=s->r;void*_tmp3FD=_tmp3FC;struct Cyc_Absyn_Decl*_tmp408;struct Cyc_Absyn_Stmt*_tmp407;struct Cyc_Absyn_Stmt*_tmp406;struct Cyc_Absyn_Stmt*_tmp405;struct Cyc_Absyn_Exp*_tmp404;switch(*((int*)_tmp3FD)){case 1U: _LL1: _tmp404=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f1;_LL2: {
# 2360
void*_tmp3FE=(void*)_check_null(_tmp404->topt);
if(!({void*_tmp7F5=_tmp3FE;Cyc_Unify_unify(_tmp7F5,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp401=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,({
struct _fat_ptr _tmp7F6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_tmp3FE));_tmp554.f1=_tmp7F6;});_tmp554;});void*_tmp3FF[1U];_tmp3FF[0]=& _tmp401;({unsigned _tmp7F8=loc;struct _fat_ptr _tmp7F7=({const char*_tmp400="statement expression returns type %s";_tag_fat(_tmp400,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7F8,_tmp7F7,_tag_fat(_tmp3FF,sizeof(void*),1U));});});
Cyc_Unify_explain_failure();}
# 2366
return _tmp3FE;}case 2U: _LL3: _tmp406=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f1;_tmp405=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f2;_LL4:
 s=_tmp405;continue;case 12U: _LL5: _tmp408=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f1;_tmp407=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f2;_LL6:
 s=_tmp407;continue;default: _LL7: _LL8:
# 2370
 return({void*_tmp402=0U;({struct Cyc_Tcenv_Tenv*_tmp7FC=te;unsigned _tmp7FB=loc;void**_tmp7FA=topt;struct _fat_ptr _tmp7F9=({const char*_tmp403="statement expression must end with expression";_tag_fat(_tmp403,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7FC,_tmp7FB,_tmp7FA,_tmp7F9,_tag_fat(_tmp402,sizeof(void*),0U));});});}_LL0:;}}
# 2375
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp7FD=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7FD,0,e);}));
{void*_tmp409=t;struct Cyc_Absyn_Aggrdecl*_tmp40D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp40D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->f1).KnownAggr).val;_LL2:
# 2379
 if(((int)_tmp40D->kind == (int)Cyc_Absyn_UnionA && _tmp40D->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp40D->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2382
({struct Cyc_String_pa_PrintArg_struct _tmp40C=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp7FE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp555.f1=_tmp7FE;});_tmp555;});void*_tmp40A[1U];_tmp40A[0]=& _tmp40C;({unsigned _tmp800=loc;struct _fat_ptr _tmp7FF=({const char*_tmp40B="expecting @tagged union but found %s";_tag_fat(_tmp40B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp800,_tmp7FF,_tag_fat(_tmp40A,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2385
return Cyc_Absyn_uint_type;}
# 2389
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2393
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp40E=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2398
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp40E)));
void*handle_type=Cyc_Tcexp_tcExp(_tmp40E,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp40F=Cyc_Tcutil_compress(handle_type);void*_tmp410=_tmp40F;void*_tmp414;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2 != 0){_LL1: _tmp414=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2)->hd;_LL2:
# 2403
 rgn=_tmp414;
Cyc_Tcenv_check_rgn_accessible(_tmp40E,loc,rgn);
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2407
({struct Cyc_String_pa_PrintArg_struct _tmp413=({struct Cyc_String_pa_PrintArg_struct _tmp556;_tmp556.tag=0U,({
struct _fat_ptr _tmp801=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp556.f1=_tmp801;});_tmp556;});void*_tmp411[1U];_tmp411[0]=& _tmp413;({unsigned _tmp803=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _fat_ptr _tmp802=({const char*_tmp412="expecting region_t type but found %s";_tag_fat(_tmp412,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp803,_tmp802,_tag_fat(_tmp411,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2414
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp804=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp804;});}}}{
# 2422
void*_tmp415=e1->r;void*_tmp416=_tmp415;struct Cyc_List_List*_tmp42E;struct Cyc_Core_Opt*_tmp42D;struct Cyc_List_List*_tmp42C;switch(*((int*)_tmp416)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2427
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp40E,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp417=0U;({struct _fat_ptr _tmp805=({const char*_tmp418="tcNew: comprehension returned non-array type";_tag_fat(_tmp418,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp805,_tag_fat(_tmp417,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp808=Cyc_Tcenv_curr_rgnpo(_tmp40E);unsigned _tmp807=loc;void*_tmp806=res_typ;Cyc_Tcutil_silent_castable(_tmp808,_tmp807,_tmp806,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2439
return res_typ;}case 36U: _LLA: _tmp42D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp416)->f1;_tmp42C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp416)->f2;_LLB:
# 2441
({void*_tmp809=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->tag=26U,_tmp419->f1=_tmp42C;_tmp419;});e1->r=_tmp809;});
_tmp42E=_tmp42C;goto _LLD;case 26U: _LLC: _tmp42E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp416)->f1;_LLD: {
# 2444
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp41A=Cyc_Tcutil_compress(*topt);void*_tmp41B=_tmp41A;void**_tmp41E;struct Cyc_Absyn_Tqual _tmp41D;void*_tmp41C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->tag == 3U){_LL15: _tmp41E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->f1).elt_type;_tmp41D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->f1).elt_tq;_tmp41C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->f1).ptr_atts).zero_term;_LL16:
# 2450
 elt_typ_opt=_tmp41E;
zero_term=Cyc_Tcutil_force_type2bool(0,_tmp41C);
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2456
void*res_typ=Cyc_Tcexp_tcArray(_tmp40E,e1->loc,elt_typ_opt,0,zero_term,_tmp42E);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp41F=0U;({struct _fat_ptr _tmp80A=({const char*_tmp420="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp420,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp80A,_tag_fat(_tmp41F,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2465
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp80D=Cyc_Tcenv_curr_rgnpo(_tmp40E);unsigned _tmp80C=loc;void*_tmp80B=res_typ;Cyc_Tcutil_silent_castable(_tmp80D,_tmp80C,_tmp80B,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2472
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp416)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2477
void*_tmp421=({void*_tmp811=Cyc_Absyn_char_type;void*_tmp810=rgn;struct Cyc_Absyn_Tqual _tmp80F=Cyc_Absyn_const_tqual(0U);void*_tmp80E=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp811,_tmp810,_tmp80F,_tmp80E,Cyc_Absyn_true_type);});
# 2479
void*_tmp422=Cyc_Tcexp_tcExp(_tmp40E,& _tmp421,e1);
return({void*_tmp815=_tmp422;void*_tmp814=rgn;struct Cyc_Absyn_Tqual _tmp813=Cyc_Absyn_empty_tqual(0U);void*_tmp812=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp815,_tmp814,_tmp813,_tmp812,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2484
void*_tmp423=({void*_tmp819=Cyc_Absyn_wchar_type();void*_tmp818=rgn;struct Cyc_Absyn_Tqual _tmp817=Cyc_Absyn_const_tqual(0U);void*_tmp816=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp819,_tmp818,_tmp817,_tmp816,Cyc_Absyn_true_type);});
# 2486
void*_tmp424=Cyc_Tcexp_tcExp(_tmp40E,& _tmp423,e1);
return({void*_tmp81D=_tmp424;void*_tmp81C=rgn;struct Cyc_Absyn_Tqual _tmp81B=Cyc_Absyn_empty_tqual(0U);void*_tmp81A=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp81D,_tmp81C,_tmp81B,_tmp81A,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2493
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp425=Cyc_Tcutil_compress(*topt);void*_tmp426=_tmp425;void**_tmp428;struct Cyc_Absyn_Tqual _tmp427;switch(*((int*)_tmp426)){case 3U: _LL1A: _tmp428=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp426)->f1).elt_type;_tmp427=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp426)->f1).elt_tq;_LL1B:
# 2499
 topt2=_tmp428;goto _LL19;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp426)->f1)->tag == 18U){_LL1C: _LL1D:
# 2503
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2509
void*telt=Cyc_Tcexp_tcExp(_tmp40E,topt2,e1);
# 2511
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt)&& !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp429=0U;({unsigned _tmp81F=e1->loc;struct _fat_ptr _tmp81E=({const char*_tmp42A="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp42A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp81F,_tmp81E,_tag_fat(_tmp429,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42B=_cycalloc(sizeof(*_tmp42B));
_tmp42B->tag=3U,(_tmp42B->f1).elt_type=telt,({struct Cyc_Absyn_Tqual _tmp822=Cyc_Absyn_empty_tqual(0U);(_tmp42B->f1).elt_tq=_tmp822;}),
((_tmp42B->f1).ptr_atts).rgn=rgn,({void*_tmp821=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(_tmp40E));((_tmp42B->f1).ptr_atts).nullable=_tmp821;}),({
void*_tmp820=Cyc_Absyn_bounds_one();((_tmp42B->f1).ptr_atts).bounds=_tmp820;}),((_tmp42B->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp42B->f1).ptr_atts).ptrloc=0;_tmp42B;});
# 2518
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp825=Cyc_Tcenv_curr_rgnpo(_tmp40E);unsigned _tmp824=loc;void*_tmp823=res_typ;Cyc_Tcutil_silent_castable(_tmp825,_tmp824,_tmp823,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2526
return res_typ;};};}}_LL5:;};}
# 2532
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp827=t=({void*_tmp826=t;Cyc_Tcutil_promote_array(_tmp826,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp827;});
return t;}
# 2542
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2546
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
# 2551
({void*_tmp42F=0U;({unsigned _tmp829=e->loc;struct _fat_ptr _tmp828=({const char*_tmp430="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp430,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp829,_tmp828,_tag_fat(_tmp42F,sizeof(void*),0U));});});{
# 2553
void*_tmp431=e->r;void*_tmp432=_tmp431;switch(*((int*)_tmp432)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp432)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2560
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array_type(t)){
t=({void*_tmp82A=t;Cyc_Tcutil_promote_array(_tmp82A,(Cyc_Tcutil_addressof_props(e)).f2,0);});
Cyc_Tcutil_unchecked_cast(e,t,Cyc_Absyn_Other_coercion);}
# 2565
return t;}_LL0:;};}
# 2576 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp433=e->r;void*_tmp434=_tmp433;struct Cyc_Absyn_Exp*_tmp452;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp434)->tag == 12U){_LL1: _tmp452=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp434)->f1;_LL2:
# 2580
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp452);
({void*_tmp82B=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp452->topt),0);_tmp452->topt=_tmp82B;});
e->topt=_tmp452->topt;
goto _LL0;}else{_LL3: _LL4:
# 2586
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp82C=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp82C;});
# 2589
{void*_tmp435=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp436=_tmp435;void*_tmp451;struct Cyc_Absyn_Tqual _tmp450;void*_tmp44F;void*_tmp44E;void*_tmp44D;void*_tmp44C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->tag == 3U){_LL6: _tmp451=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->f1).elt_type;_tmp450=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->f1).elt_tq;_tmp44F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->f1).ptr_atts).rgn;_tmp44E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->f1).ptr_atts).nullable;_tmp44D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->f1).ptr_atts).bounds;_tmp44C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->f1).ptr_atts).zero_term;_LL7:
# 2591
{void*_tmp437=Cyc_Tcutil_compress(_tmp451);void*_tmp438=_tmp437;struct Cyc_List_List*_tmp44B;void*_tmp44A;struct Cyc_Absyn_Tqual _tmp449;void*_tmp448;struct Cyc_List_List*_tmp447;int _tmp446;struct Cyc_Absyn_VarargInfo*_tmp445;struct Cyc_List_List*_tmp444;struct Cyc_List_List*_tmp443;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_List_List*_tmp441;struct Cyc_Absyn_Exp*_tmp440;struct Cyc_List_List*_tmp43F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->tag == 5U){_LLB: _tmp44B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).tvars;_tmp44A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).effect;_tmp449=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).ret_tqual;_tmp448=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).ret_type;_tmp447=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).args;_tmp446=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).c_varargs;_tmp445=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).cyc_varargs;_tmp444=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).rgn_po;_tmp443=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).attributes;_tmp442=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).requires_clause;_tmp441=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).requires_relns;_tmp440=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).ensures_clause;_tmp43F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438)->f1).ensures_relns;_LLC:
# 2593
 if(_tmp44B != 0){
struct _tuple12 _tmp439=({struct _tuple12 _tmp557;({struct Cyc_List_List*_tmp82D=Cyc_Tcenv_lookup_type_vars(te);_tmp557.f1=_tmp82D;}),_tmp557.f2=Cyc_Core_heap_region;_tmp557;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp439,_tmp44B);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2601
_tmp444=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp444);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp444);{
void*ftyp=({struct Cyc_List_List*_tmp82E=inst;Cyc_Tcutil_substitute(_tmp82E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));
_tmp43E->tag=5U,(_tmp43E->f1).tvars=0,(_tmp43E->f1).effect=_tmp44A,(_tmp43E->f1).ret_tqual=_tmp449,(_tmp43E->f1).ret_type=_tmp448,(_tmp43E->f1).args=_tmp447,(_tmp43E->f1).c_varargs=_tmp446,(_tmp43E->f1).cyc_varargs=_tmp445,(_tmp43E->f1).rgn_po=0,(_tmp43E->f1).attributes=_tmp443,(_tmp43E->f1).requires_clause=_tmp442,(_tmp43E->f1).requires_relns=_tmp441,(_tmp43E->f1).ensures_clause=_tmp440,(_tmp43E->f1).ensures_relns=_tmp43F;_tmp43E;}));});
# 2608
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43A=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->tag=3U,(_tmp43D->f1).elt_type=ftyp,(_tmp43D->f1).elt_tq=_tmp450,((_tmp43D->f1).ptr_atts).rgn=_tmp44F,((_tmp43D->f1).ptr_atts).nullable=_tmp44E,((_tmp43D->f1).ptr_atts).bounds=_tmp44D,((_tmp43D->f1).ptr_atts).zero_term=_tmp44C,((_tmp43D->f1).ptr_atts).ptrloc=0;_tmp43D;});
# 2610
struct Cyc_Absyn_Exp*_tmp43B=Cyc_Absyn_copy_exp(e);
({void*_tmp82F=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=13U,_tmp43C->f1=_tmp43B,_tmp43C->f2=ts;_tmp43C;});e->r=_tmp82F;});
e->topt=(void*)_tmp43A;};}
# 2614
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2617
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2620
goto _LL0;}_LL0:;}
# 2622
return(void*)_check_null(e->topt);}
# 2630
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2632
struct Cyc_List_List*_tmp453=0;
# 2634
{void*_tmp454=fn_exp->r;void*_tmp455=_tmp454;struct Cyc_List_List*_tmp469;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp455)->tag == 10U){_LL1: _tmp469=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp455)->f2;_LL2:
# 2636
{void*_tmp456=e->r;void*_tmp457=_tmp456;struct Cyc_List_List*_tmp466;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp457)->tag == 10U){_LL6: _tmp466=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp457)->f2;_LL7: {
# 2638
struct Cyc_List_List*_tmp458=alias_arg_exps;
int _tmp459=0;
while(_tmp458 != 0){
while(_tmp459 != (int)_tmp458->hd){
if(_tmp469 == 0)
({void*_tmp45A=0U;({struct _fat_ptr _tmp831=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp45D=({struct Cyc_Int_pa_PrintArg_struct _tmp559;_tmp559.tag=1U,_tmp559.f1=(unsigned long)_tmp459;_tmp559;});struct Cyc_Int_pa_PrintArg_struct _tmp45E=({struct Cyc_Int_pa_PrintArg_struct _tmp558;_tmp558.tag=1U,_tmp558.f1=(unsigned long)((int)_tmp458->hd);_tmp558;});void*_tmp45B[2U];_tmp45B[0]=& _tmp45D,_tmp45B[1]=& _tmp45E;({struct _fat_ptr _tmp830=({const char*_tmp45C="bad count %d/%d for alias coercion!";_tag_fat(_tmp45C,sizeof(char),36U);});Cyc_aprintf(_tmp830,_tag_fat(_tmp45B,sizeof(void*),2U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp831,_tag_fat(_tmp45A,sizeof(void*),0U));});});
# 2645
++ _tmp459;
_tmp469=_tmp469->tl;
_tmp466=((struct Cyc_List_List*)_check_null(_tmp466))->tl;}{
# 2650
struct _tuple11 _tmp45F=({struct Cyc_Absyn_Exp*_tmp832=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp469))->hd;Cyc_Tcutil_insert_alias(_tmp832,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp466))->hd)->topt)));});struct _tuple11 _tmp460=_tmp45F;struct Cyc_Absyn_Decl*_tmp463;struct Cyc_Absyn_Exp*_tmp462;_LLB: _tmp463=_tmp460.f1;_tmp462=_tmp460.f2;_LLC:;
_tmp469->hd=(void*)_tmp462;
_tmp453=({struct Cyc_List_List*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->hd=_tmp463,_tmp461->tl=_tmp453;_tmp461;});
_tmp458=_tmp458->tl;};}
# 2655
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp464=0U;({struct _fat_ptr _tmp833=({const char*_tmp465="not a function call!";_tag_fat(_tmp465,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp833,_tag_fat(_tmp464,sizeof(void*),0U));});});}_LL5:;}
# 2658
goto _LL0;}else{_LL3: _LL4:
({void*_tmp467=0U;({struct _fat_ptr _tmp834=({const char*_tmp468="not a function call!";_tag_fat(_tmp468,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp834,_tag_fat(_tmp467,sizeof(void*),0U));});});}_LL0:;}
# 2663
while(_tmp453 != 0){
struct Cyc_Absyn_Decl*_tmp46A=(struct Cyc_Absyn_Decl*)_tmp453->hd;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp837=({struct Cyc_Absyn_Decl*_tmp836=_tmp46A;struct Cyc_Absyn_Stmt*_tmp835=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp836,_tmp835,e->loc);});Cyc_Absyn_stmt_exp(_tmp837,e->loc);});
_tmp453=_tmp453->tl;}
# 2670
e->topt=0;
e->r=fn_exp->r;}
# 2676
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2680
{void*_tmp46B=e->r;void*_tmp46C=_tmp46B;void*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D3;struct _fat_ptr*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D0;int*_tmp4CF;struct Cyc_Absyn_Exp**_tmp4CE;void***_tmp4CD;struct Cyc_Absyn_Exp**_tmp4CC;int*_tmp4CB;void*_tmp4CA;struct Cyc_Absyn_Enumfield*_tmp4C9;struct Cyc_Absyn_Enumdecl*_tmp4C8;struct Cyc_Absyn_Enumfield*_tmp4C7;struct Cyc_List_List*_tmp4C6;struct Cyc_Absyn_Datatypedecl*_tmp4C5;struct Cyc_Absyn_Datatypefield*_tmp4C4;void*_tmp4C3;struct Cyc_List_List*_tmp4C2;struct _tuple1**_tmp4C1;struct Cyc_List_List**_tmp4C0;struct Cyc_List_List*_tmp4BF;struct Cyc_Absyn_Aggrdecl**_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BD;void*_tmp4BC;int*_tmp4BB;struct Cyc_Absyn_Vardecl*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4B8;int*_tmp4B7;struct Cyc_Absyn_Stmt*_tmp4B6;struct Cyc_List_List*_tmp4B5;struct _tuple9*_tmp4B4;struct Cyc_List_List*_tmp4B3;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4AF;struct _fat_ptr*_tmp4AE;int*_tmp4AD;int*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct _fat_ptr*_tmp4AA;int*_tmp4A9;int*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A7;void*_tmp4A6;struct Cyc_List_List*_tmp4A5;void*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp**_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A0;void*_tmp49F;struct Cyc_Absyn_Exp*_tmp49E;enum Cyc_Absyn_Coercion*_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Core_Opt*_tmp48F;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;enum Cyc_Absyn_Incrementor _tmp48C;enum Cyc_Absyn_Primop _tmp48B;struct Cyc_List_List*_tmp48A;void**_tmp489;union Cyc_Absyn_Cnst*_tmp488;struct Cyc_Core_Opt*_tmp487;struct Cyc_List_List*_tmp486;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_List_List*_tmp484;struct Cyc_Absyn_VarargCallInfo**_tmp483;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_VarargCallInfo**_tmp480;int*_tmp47F;struct Cyc_Absyn_Exp*_tmp47E;switch(*((int*)_tmp46C)){case 12U: _LL1: _tmp47E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL2:
# 2685
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp47E);
return;case 10U: _LL3: _tmp482=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp481=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp480=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp47F=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;if(!(*_tmp47F)){_LL4:
# 2689
({void*_tmp46D=0U;({struct _fat_ptr _tmp838=({const char*_tmp46E="unresolved function in tcExpNoInst";_tag_fat(_tmp46E,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp838,_tag_fat(_tmp46D,sizeof(void*),0U));});});}else{_LL1B: _tmp485=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp484=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp483=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL1C: {
# 2722
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp46F;_push_handler(& _tmp46F);{int _tmp471=0;if(setjmp(_tmp46F.handler))_tmp471=1;if(!_tmp471){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp470=(void*)Cyc_Core_get_exn_thrown();void*_tmp472=_tmp470;void*_tmp473;if(((struct Cyc_Core_Failure_exn_struct*)_tmp472)->tag == Cyc_Core_Failure){_LL58: _LL59:
# 2729
 ok=0;
fn_exp=e;
goto _LL57;}else{_LL5A: _tmp473=_tmp472;_LL5B:(int)_rethrow(_tmp473);}_LL57:;}};}
# 2733
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp485,_tmp484,_tmp483,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2740
goto _LL0;}}case 36U: _LL5: _tmp487=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp486=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL6:
# 2693
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp486);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp488=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL8:
# 2698
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp488,e);goto _LL0;case 1U: _LL9: _tmp489=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LLA:
# 2700
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp489);goto _LL0;case 2U: _LLB: _LLC:
# 2702
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp48B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp48A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LLE:
# 2704
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp48B,_tmp48A);goto _LL0;case 5U: _LLF: _tmp48D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp48C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL10:
# 2706
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp48D,_tmp48C);goto _LL0;case 4U: _LL11: _tmp490=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp48F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp48E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL12:
# 2708
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp490,_tmp48F,_tmp48E);goto _LL0;case 6U: _LL13: _tmp493=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp492=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp491=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL14:
# 2710
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp493,_tmp492,_tmp491);goto _LL0;case 7U: _LL15: _tmp495=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp494=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL16:
# 2712
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp495,_tmp494);goto _LL0;case 8U: _LL17: _tmp497=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp496=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL18:
# 2714
 t=Cyc_Tcexp_tcOr(te,loc,_tmp497,_tmp496);goto _LL0;case 9U: _LL19: _tmp499=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp498=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL1A:
# 2716
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp499,_tmp498);goto _LL0;case 11U: _LL1D: _tmp49A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL1E:
# 2742
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp49A);goto _LL0;case 13U: _LL1F: _tmp49C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp49B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL20:
# 2744
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp49C,_tmp49B);goto _LL0;case 14U: _LL21: _tmp49F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp49E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp49D=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL22:
# 2746
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp49F,_tmp49E,_tmp49D);goto _LL0;case 15U: _LL23: _tmp4A0=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL24:
# 2748
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4A0);goto _LL0;case 16U: _LL25: _tmp4A2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4A1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL26:
# 2750
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4A2,e,_tmp4A1);goto _LL0;case 18U: _LL27: _tmp4A3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL28: {
# 2752
void*_tmp474=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4A3);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp474);goto _LL0;}case 17U: _LL29: _tmp4A4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL2A:
# 2755
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4A4);goto _LL0;case 19U: _LL2B: _tmp4A6=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4A5=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL2C:
# 2757
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4A6,_tmp4A5);goto _LL0;case 20U: _LL2D: _tmp4A7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL2E:
# 2759
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4A7);goto _LL0;case 21U: _LL2F: _tmp4AB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4AA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4A9=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp4A8=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL30:
# 2761
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4AB,_tmp4AA,_tmp4A9,_tmp4A8);goto _LL0;case 22U: _LL31: _tmp4AF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4AE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4AD=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp4AC=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL32:
# 2763
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4AF,_tmp4AE,_tmp4AD,_tmp4AC);goto _LL0;case 23U: _LL33: _tmp4B1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4B0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL34:
# 2765
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4B1,_tmp4B0);goto _LL0;case 24U: _LL35: _tmp4B2=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL36:
# 2767
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4B2);goto _LL0;case 25U: _LL37: _tmp4B4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4B3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL38:
# 2769
 t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,_tmp4B4,_tmp4B3);goto _LL0;case 26U: _LL39: _tmp4B5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL3A: {
# 2773
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp475=Cyc_Tcutil_compress(*topt);void*_tmp476=_tmp475;void**_tmp479;struct Cyc_Absyn_Tqual*_tmp478;void*_tmp477;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp476)->tag == 4U){_LL5D: _tmp479=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp476)->f1).elt_type;_tmp478=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp476)->f1).tq;_tmp477=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp476)->f1).zero_term;_LL5E:
# 2779
 elt_topt=_tmp479;
elt_tqopt=_tmp478;
zero_term=Cyc_Tcutil_force_type2bool(0,_tmp477);
goto _LL5C;}else{_LL5F: _LL60:
 goto _LL5C;}_LL5C:;}
# 2786
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4B5);goto _LL0;}case 37U: _LL3B: _tmp4B6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL3C:
# 2788
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4B6);goto _LL0;case 27U: _LL3D: _tmp4BA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4B9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4B8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp4B7=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL3E:
# 2790
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4BA,_tmp4B9,_tmp4B8,_tmp4B7);goto _LL0;case 28U: _LL3F: _tmp4BD=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4BC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4BB=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL40:
# 2792
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4BD,_tmp4BC,_tmp4BB);goto _LL0;case 29U: _LL41: _tmp4C1=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4C0=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4BF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp4BE=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL42:
# 2794
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4C1,_tmp4C0,_tmp4BF,_tmp4BE);goto _LL0;case 30U: _LL43: _tmp4C3=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4C2=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL44:
# 2796
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4C3,_tmp4C2);goto _LL0;case 31U: _LL45: _tmp4C6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4C5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4C4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL46:
# 2798
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp4C6,_tmp4C5,_tmp4C4);goto _LL0;case 32U: _LL47: _tmp4C8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4C7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL48:
# 2800
 t=Cyc_Absyn_enum_type(_tmp4C8->name,_tmp4C8);goto _LL0;case 33U: _LL49: _tmp4CA=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4C9=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL4A:
# 2802
 t=_tmp4CA;goto _LL0;case 34U: _LL4B: _tmp4CF=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).is_calloc;_tmp4CE=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).rgn;_tmp4CD=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).elt_type;_tmp4CC=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).num_elts;_tmp4CB=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).fat_result;_LL4C:
# 2804
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4CE,_tmp4CD,_tmp4CC,_tmp4CF,_tmp4CB);goto _LL0;case 35U: _LL4D: _tmp4D1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4D0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL4E:
# 2806
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4D1,_tmp4D0);goto _LL0;case 38U: _LL4F: _tmp4D3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4D2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL50:
# 2808
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp4D3,_tmp4D2);goto _LL0;case 41U: _LL51: _tmp4D4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL52:
# 2810
 t=Cyc_Tcexp_tcExp(te,topt,_tmp4D4);goto _LL0;case 39U: _LL53: _tmp4D5=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL54:
# 2812
 if(!te->allow_valueof)
({void*_tmp47A=0U;({unsigned _tmp83A=e->loc;struct _fat_ptr _tmp839=({const char*_tmp47B="valueof(-) can only occur within types";_tag_fat(_tmp47B,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp83A,_tmp839,_tag_fat(_tmp47A,sizeof(void*),0U));});});
# 2820
t=Cyc_Absyn_sint_type;
goto _LL0;default: _LL55: _LL56:
# 2823
({void*_tmp47C=0U;({unsigned _tmp83C=e->loc;struct _fat_ptr _tmp83B=({const char*_tmp47D="asm expressions cannot occur within Cyclone code.";_tag_fat(_tmp47D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp83C,_tmp83B,_tag_fat(_tmp47C,sizeof(void*),0U));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}_LL0:;}
# 2826
e->topt=t;}

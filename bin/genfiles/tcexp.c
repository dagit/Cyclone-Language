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
void*_tmp2=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmp2;void*_tmp3=_stmttmp0;struct Cyc_Absyn_Tqual _tmp5;void*_tmp4;union Cyc_Absyn_AggrInfo _tmp6;switch(*((int*)_tmp3)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->tag == 20U){_LL1: _tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp6;
# 78
{union Cyc_Absyn_AggrInfo _tmp7=info;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp7.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp9=0U;({struct _fat_ptr _tmp55D=({const char*_tmpA="struct type not properly set";_tag_fat(_tmpA,sizeof(char),29U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp55D,_tag_fat(_tmp9,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp7.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmp8;
({void*_tmp55E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=29U,_tmpB->f1=ad->name,_tmpB->f2=0,_tmpB->f3=des,_tmpB->f4=ad;_tmpB;});e->r=_tmp55E;});}}_LL9:;}
# 82
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmp4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmp5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4: {void*at=_tmp4;struct Cyc_Absyn_Tqual aq=_tmp5;
({void*_tmp55F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->tag=26U,_tmpC->f1=des;_tmpC;});e->r=_tmp55F;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp560=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->tag=30U,_tmpD->f1=t,_tmpD->f2=des;_tmpD;});e->r=_tmp560;});goto _LL0;default: _LL7: _LL8:
({void*_tmp561=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->tag=26U,_tmpE->f1=des;_tmpE;});e->r=_tmp561;});goto _LL0;}_LL0:;}}
# 92
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 96
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 102
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_stmttmp1=_tmpF;void*_tmp10=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->f2 == 0){_LL1: _LL2:
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
{union Cyc_Absyn_Cnst _tmp17=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _stmttmp2=_tmp17;union Cyc_Absyn_Cnst _tmp18=_stmttmp2;struct _fat_ptr _tmp19;struct _fat_ptr _tmp1A;int _tmp1C;enum Cyc_Absyn_Sign _tmp1B;int _tmp1D;enum Cyc_Absyn_Sign _tmp1E;enum Cyc_Absyn_Sign _tmp1F;switch((_tmp18.Wstring_c).tag){case 2U: switch(((_tmp18.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_type;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_type;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_type;goto _LL0;}case 3U: _LL7: _LL8:
 t=Cyc_Absyn_wchar_type();goto _LL0;case 4U: _LL9: _tmp1F=((_tmp18.Short_c).val).f1;_LLA: {enum Cyc_Absyn_Sign sn=_tmp1F;
# 153
t=(int)sn == (int)1U?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;goto _LL0;}case 6U: _LLB: _tmp1E=((_tmp18.LongLong_c).val).f1;_LLC: {enum Cyc_Absyn_Sign sn=_tmp1E;
# 155
t=(int)sn == (int)1U?Cyc_Absyn_ulonglong_type: Cyc_Absyn_slonglong_type;goto _LL0;}case 7U: _LLD: _tmp1D=((_tmp18.Float_c).val).f2;_LLE: {int i=_tmp1D;
# 157
if(topt == 0)t=Cyc_Absyn_gen_float_type((unsigned)i);else{
# 161
void*_tmp20=Cyc_Tcutil_compress(*topt);void*_stmttmp3=_tmp20;void*_tmp21=_stmttmp3;int _tmp22;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21)->f1)->tag == 2U){_LL18: _tmp22=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21)->f1)->f1;_LL19: {int i=_tmp22;
t=Cyc_Absyn_gen_float_type((unsigned)i);goto _LL17;}}else{goto _LL1A;}}else{_LL1A: _LL1B:
# 165
 t=Cyc_Absyn_gen_float_type((unsigned)i);
goto _LL17;}_LL17:;}
# 169
goto _LL0;}case 5U: _LLF: _tmp1B=((_tmp18.Int_c).val).f1;_tmp1C=((_tmp18.Int_c).val).f2;_LL10: {enum Cyc_Absyn_Sign csn=_tmp1B;int i=_tmp1C;
# 171
if(topt == 0)
t=(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;else{
# 178
void*_tmp23=Cyc_Tcutil_compress(*topt);void*_stmttmp4=_tmp23;void*_tmp24=_stmttmp4;void*_tmp2A;void*_tmp29;void*_tmp28;void*_tmp27;struct Cyc_Absyn_Tqual _tmp26;void*_tmp25;void*_tmp2B;enum Cyc_Absyn_Sign _tmp2C;enum Cyc_Absyn_Sign _tmp2D;enum Cyc_Absyn_Sign _tmp2E;enum Cyc_Absyn_Sign _tmp2F;switch(*((int*)_tmp24)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1D: _tmp2F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f1)->f1;_LL1E: {enum Cyc_Absyn_Sign sn=_tmp2F;
# 180
{enum Cyc_Absyn_Sign _tmp30=sn;switch(_tmp30){case Cyc_Absyn_Unsigned: _LL2C: _LL2D:
 t=Cyc_Absyn_uchar_type;goto _LL2B;case Cyc_Absyn_Signed: _LL2E: _LL2F:
 t=Cyc_Absyn_schar_type;goto _LL2B;default: _LL30: _LL31:
 t=Cyc_Absyn_char_type;goto _LL2B;}_LL2B:;}
# 185
({union Cyc_Absyn_Cnst _tmp567=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp567;});
goto _LL1C;}case Cyc_Absyn_Short_sz: _LL1F: _tmp2E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f1)->f1;_LL20: {enum Cyc_Absyn_Sign sn=_tmp2E;
# 188
t=(int)sn == (int)1U?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;
({union Cyc_Absyn_Cnst _tmp568=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp568;});
goto _LL1C;}case Cyc_Absyn_Int_sz: _LL21: _tmp2D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f1)->f1;_LL22: {enum Cyc_Absyn_Sign sn=_tmp2D;
# 192
t=(int)sn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 195
({union Cyc_Absyn_Cnst _tmp569=Cyc_Absyn_Int_c(sn,i);*c=_tmp569;});
goto _LL1C;}case Cyc_Absyn_Long_sz: _LL23: _tmp2C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp2C;
# 198
t=(int)sn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 201
({union Cyc_Absyn_Cnst _tmp56A=Cyc_Absyn_Int_c(sn,i);*c=_tmp56A;});
goto _LL1C;}default: goto _LL29;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f2 != 0){_LL27: _tmp2B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24)->f2)->hd;_LL28: {void*t1=_tmp2B;
# 215
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp36=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->tag=9U,({struct Cyc_Absyn_Exp*_tmp56B=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp37->f1=_tmp56B;});_tmp37;});struct Cyc_Absyn_ValueofType_Absyn_Type_struct*t2=_tmp36;
# 222
t=Cyc_Absyn_tag_type((void*)t2);
goto _LL1C;}}else{goto _LL29;}default: goto _LL29;}case 3U: _LL25: _tmp25=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).elt_type;_tmp26=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).elt_tq;_tmp27=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).rgn;_tmp28=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).nullable;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_tmp2A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).zero_term;if(i == 0){_LL26: {void*et=_tmp25;struct Cyc_Absyn_Tqual etq=_tmp26;void*rgn=_tmp27;void*nbl=_tmp28;void*bnd=_tmp29;void*zt=_tmp2A;
# 205
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(Cyc_Tcutil_force_type2bool(1,nbl))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp4D8;_tmp4D8.tag=0U,({
struct _fat_ptr _tmp56C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D8.f1=_tmp56C;});_tmp4D8;});void*_tmp31[1U];_tmp31[0]=& _tmp33;({unsigned _tmp56E=e->loc;struct _fat_ptr _tmp56D=({const char*_tmp32="Used NULL when expecting a non-NULL value of type %s";_tag_fat(_tmp32,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp56E,_tmp56D,_tag_fat(_tmp31,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp34=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp34;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=3U,(_tmp35->f1).elt_type=et,(_tmp35->f1).elt_tq=etq,
((_tmp35->f1).ptr_atts).rgn=rgn,((_tmp35->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp35->f1).ptr_atts).bounds=bnd,((_tmp35->f1).ptr_atts).zero_term=zt,((_tmp35->f1).ptr_atts).ptrloc=0;_tmp35;});
goto _LL1C;}}}else{goto _LL29;}default: _LL29: _LL2A:
# 225
 t=(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
goto _LL1C;}_LL1C:;}
# 228
goto _LL0;}case 8U: _LL11: _tmp1A=(_tmp18.String_c).val;_LL12: {struct _fat_ptr s=_tmp1A;
# 230
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp19=s;goto _LL14;}case 9U: _LL13: _tmp19=(_tmp18.Wstring_c).val;_LL14: {struct _fat_ptr s=_tmp19;
# 233
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 237
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp56F=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp56F,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 242
void*_tmp38=Cyc_Absyn_thin_bounds_exp(elen);void*elen_bound=_tmp38;
t=({void*_tmp573=string_elt_typ;void*_tmp572=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp571=Cyc_Absyn_const_tqual(0U);void*_tmp570=elen_bound;Cyc_Absyn_atb_type(_tmp573,_tmp572,_tmp571,_tmp570,Cyc_Absyn_true_type);});
# 245
if(topt != 0){
void*_tmp39=Cyc_Tcutil_compress(*topt);void*_stmttmp5=_tmp39;void*_tmp3A=_stmttmp5;struct Cyc_Absyn_Tqual _tmp3B;switch(*((int*)_tmp3A)){case 4U: _LL33: _tmp3B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A)->f1).tq;_LL34: {struct Cyc_Absyn_Tqual tq=_tmp3B;
# 251
return({void*_tmp576=string_elt_typ;struct Cyc_Absyn_Tqual _tmp575=tq;struct Cyc_Absyn_Exp*_tmp574=elen;Cyc_Absyn_array_type(_tmp576,_tmp575,_tmp574,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL35: _LL36:
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
void*_tmp57A=elen_bound;Cyc_Absyn_atb_type(_tmp57D,_tmp57C,_tmp57B,_tmp57A,Cyc_Absyn_true_type);});
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
return t;}}}default: _LL15: _LL16:
# 276
 if(topt != 0){
void*_tmp3C=Cyc_Tcutil_compress(*topt);void*_stmttmp6=_tmp3C;void*_tmp3D=_stmttmp6;void*_tmp43;void*_tmp42;void*_tmp41;void*_tmp40;struct Cyc_Absyn_Tqual _tmp3F;void*_tmp3E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D)->tag == 3U){_LL3A: _tmp3E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D)->f1).elt_type;_tmp3F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D)->f1).elt_tq;_tmp40=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D)->f1).ptr_atts).rgn;_tmp41=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D)->f1).ptr_atts).nullable;_tmp42=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D)->f1).ptr_atts).bounds;_tmp43=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D)->f1).ptr_atts).zero_term;_LL3B: {void*et=_tmp3E;struct Cyc_Absyn_Tqual etq=_tmp3F;void*rgn=_tmp40;void*nbl=_tmp41;void*bnd=_tmp42;void*zt=_tmp43;
# 280
if(Cyc_Tcutil_force_type2bool(1,nbl))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp46=({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0U,({
struct _fat_ptr _tmp581=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D9.f1=_tmp581;});_tmp4D9;});void*_tmp44[1U];_tmp44[0]=& _tmp46;({unsigned _tmp583=e->loc;struct _fat_ptr _tmp582=({const char*_tmp45="Used NULL when expecting a non-NULL value of type %s";_tag_fat(_tmp45,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp583,_tmp582,_tag_fat(_tmp44,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->tag=3U,(_tmp47->f1).elt_type=et,(_tmp47->f1).elt_tq=etq,((_tmp47->f1).ptr_atts).rgn=rgn,((_tmp47->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp47->f1).ptr_atts).bounds=bnd,((_tmp47->f1).ptr_atts).zero_term=zt,((_tmp47->f1).ptr_atts).ptrloc=0;_tmp47;});}}else{_LL3C: _LL3D:
# 285
 goto _LL39;}_LL39:;}{
# 288
struct Cyc_List_List*_tmp48=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp48;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=3U,({void*_tmp588=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->v=tenv_tvs;_tmp49;}));(_tmp4B->f1).elt_type=_tmp588;}),({
struct Cyc_Absyn_Tqual _tmp587=Cyc_Absyn_empty_tqual(0U);(_tmp4B->f1).elt_tq=_tmp587;}),
({void*_tmp586=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->v=tenv_tvs;_tmp4A;}));((_tmp4B->f1).ptr_atts).rgn=_tmp586;}),((_tmp4B->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp585=Cyc_Tcutil_any_bounds(tenv_tvs);((_tmp4B->f1).ptr_atts).bounds=_tmp585;}),({
void*_tmp584=Cyc_Tcutil_any_bool(tenv_tvs);((_tmp4B->f1).ptr_atts).zero_term=_tmp584;}),((_tmp4B->f1).ptr_atts).ptrloc=0;_tmp4B;});
# 295
goto _LL0;}}_LL0:;}
# 297
return t;}
# 301
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 306
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp4C=*((void**)_check_null(b));void*_stmttmp7=_tmp4C;void*_tmp4D=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp4E;struct Cyc_Absyn_Vardecl*_tmp4F;struct Cyc_Absyn_Vardecl*_tmp50;struct Cyc_Absyn_Fndecl*_tmp51;struct Cyc_Absyn_Vardecl*_tmp52;struct _tuple1*_tmp53;switch(*((int*)_tmp4D)){case 0U: _LL1: _tmp53=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL2: {struct _tuple1*q=_tmp53;
({void*_tmp54=0U;({struct _fat_ptr _tmp589=({const char*_tmp55="unresolved binding in tcVar";_tag_fat(_tmp55,sizeof(char),28U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp589,_tag_fat(_tmp54,sizeof(void*),0U));});});}case 1U: _LL3: _tmp52=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp52;
# 312
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2U: _LL5: _tmp51=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp51;
# 319
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5U: _LL7: _tmp50=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp50;
_tmp4F=vd;goto _LLA;}case 4U: _LL9: _tmp4F=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp4F;
_tmp4E=vd;goto _LLC;}default: _LLB: _tmp4E=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp4E;
# 325
if(te->allow_valueof){
void*_tmp56=Cyc_Tcutil_compress(vd->type);void*_stmttmp8=_tmp56;void*_tmp57=_stmttmp8;void*_tmp58;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57)->f2 != 0){_LLE: _tmp58=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57)->f2)->hd;_LLF: {void*i=_tmp58;
# 328
({void*_tmp58A=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=39U,_tmp59->f1=i;_tmp59;});e->r=_tmp58A;});
goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 333
return vd->type;}}_LL0:;}
# 337
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 345
struct Cyc_List_List*desc_types;
{void*_tmp5A=fmt->r;void*_stmttmp9=_tmp5A;void*_tmp5B=_stmttmp9;struct _fat_ptr _tmp5C;struct _fat_ptr _tmp5D;switch(*((int*)_tmp5B)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).tag == 8){_LL1: _tmp5D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp5D;
_tmp5C=s;goto _LL4;}}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp5C=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp5C;
# 349
desc_types=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 353
 if(opt_args != 0){
struct Cyc_List_List*_tmp5E=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp5E;
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt))&& !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)args->hd))
({void*_tmp5F=0U;({unsigned _tmp58C=((struct Cyc_Absyn_Exp*)args->hd)->loc;struct _fat_ptr _tmp58B=({const char*_tmp60="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp60,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp58C,_tmp58B,_tag_fat(_tmp5F,sizeof(void*),0U));});});}}
# 362
return;}_LL0:;}
# 364
if(opt_args != 0){
struct Cyc_List_List*_tmp61=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp61;
# 367
for(0;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
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
({void*_tmp67=0U;({unsigned _tmp595=((struct Cyc_Absyn_Exp*)args->hd)->loc;struct _fat_ptr _tmp594=({const char*_tmp68="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp68,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp595,_tmp594,_tag_fat(_tmp67,sizeof(void*),0U));});});}
# 385
if(desc_types != 0)
({void*_tmp69=0U;({unsigned _tmp597=fmt->loc;struct _fat_ptr _tmp596=({const char*_tmp6A="too few arguments";_tag_fat(_tmp6A,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp597,_tmp596,_tag_fat(_tmp69,sizeof(void*),0U));});});
if(args != 0){
({void*_tmp6B=0U;({unsigned _tmp599=((struct Cyc_Absyn_Exp*)args->hd)->loc;struct _fat_ptr _tmp598=({const char*_tmp6C="too many arguments";_tag_fat(_tmp6C,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp599,_tmp598,_tag_fat(_tmp6B,sizeof(void*),0U));});});
# 390
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}}}
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
{void*_tmp77=t;void*_tmp78;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp78;
# 416
struct Cyc_Absyn_Exp*_tmp79=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp79;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp7A=eopt;struct Cyc_Absyn_Exp*e=_tmp7A;
if(!Cyc_Evexp_c_can_eval(e)&& !((unsigned)Cyc_Tcenv_allow_valueof))
({void*_tmp7B=0U;({unsigned _tmp5A4=loc;struct _fat_ptr _tmp5A3=({const char*_tmp7C="cannot apply numelts to a pointer with abstract bounds";_tag_fat(_tmp7C,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp5A4,_tmp5A3,_tag_fat(_tmp7B,sizeof(void*),0U));});});}
# 422
goto _LLD;}}else{_LL10: _LL11:
# 424
({struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=0U,({struct _fat_ptr _tmp5A5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4DF.f1=_tmp5A5;});_tmp4DF;});void*_tmp7D[1U];_tmp7D[0]=& _tmp7F;({unsigned _tmp5A7=loc;struct _fat_ptr _tmp5A6=({const char*_tmp7E="numelts requires pointer type, not %s";_tag_fat(_tmp7E,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp5A7,_tmp5A6,_tag_fat(_tmp7D,sizeof(void*),1U));});});}_LLD:;}
# 426
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({void*_tmp80=0U;({struct _fat_ptr _tmp5A8=({const char*_tmp81="Non-unary primop";_tag_fat(_tmp81,sizeof(char),17U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp5A8,_tag_fat(_tmp80,sizeof(void*),0U));});});}_LL0:;}
# 433
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp82=(void*)_check_null(e1->topt);void*t1=_tmp82;
void*_tmp83=(void*)_check_null(e2->topt);void*t2=_tmp83;
void*_tmp84=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_t=_tmp84;
if(!Cyc_Unify_unify(t1,new_t))Cyc_Tcutil_unchecked_cast(e1,new_t,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(t2,new_t))Cyc_Tcutil_unchecked_cast(e2,new_t,Cyc_Absyn_No_coercion);
return new_t;}
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
void*_tmp8B=t1;void*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->tag == 3U){_LL1: _tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_type;_tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_tq;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).rgn;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).bounds;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp8C;struct Cyc_Absyn_Tqual tq=_tmp8D;void*r=_tmp8E;void*n=_tmp8F;void*b=_tmp90;void*zt=_tmp91;
# 462
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({void*_tmp92=0U;({unsigned _tmp5B0=e1->loc;struct _fat_ptr _tmp5AF=({const char*_tmp93="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp93,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5B0,_tmp5AF,_tag_fat(_tmp92,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp94=0U;({unsigned _tmp5B2=e1->loc;struct _fat_ptr _tmp5B1=({const char*_tmp95="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp95,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5B2,_tmp5B1,_tag_fat(_tmp94,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_String_pa_PrintArg_struct _tmp98=({struct Cyc_String_pa_PrintArg_struct _tmp4E2;_tmp4E2.tag=0U,({struct _fat_ptr _tmp5B3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E2.f1=_tmp5B3;});_tmp4E2;});void*_tmp96[1U];_tmp96[0]=& _tmp98;({unsigned _tmp5B5=e2->loc;struct _fat_ptr _tmp5B4=({const char*_tmp97="expecting int but found %s";_tag_fat(_tmp97,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5B5,_tmp5B4,_tag_fat(_tmp96,sizeof(void*),1U));});});{
struct Cyc_Absyn_Exp*_tmp99=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp99;
if(eopt == 0)return t1;{
struct Cyc_Absyn_Exp*_tmp9A=eopt;struct Cyc_Absyn_Exp*ub=_tmp9A;
# 473
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple15 _tmp9B=Cyc_Evexp_eval_const_uint_exp(ub);struct _tuple15 _stmttmpA=_tmp9B;struct _tuple15 _tmp9C=_stmttmpA;int _tmp9E;unsigned _tmp9D;_LL6: _tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;_LL7: {unsigned i=_tmp9D;int known=_tmp9E;
if(known && i == (unsigned)1)
({void*_tmp9F=0U;({unsigned _tmp5B7=e1->loc;struct _fat_ptr _tmp5B6=({const char*_tmpA0="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpA0,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5B7,_tmp5B6,_tag_fat(_tmp9F,sizeof(void*),0U));});});}}{
# 484
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA1=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=3U,(_tmpA2->f1).elt_type=et,(_tmpA2->f1).elt_tq=tq,
((_tmpA2->f1).ptr_atts).rgn=r,((_tmpA2->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpA2->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpA2->f1).ptr_atts).zero_term=zt,((_tmpA2->f1).ptr_atts).ptrloc=0;_tmpA2;});
# 484
struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpA1;
# 488
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,Cyc_Absyn_Other_coercion);
return(void*)new_t1;}}}}}else{_LL3: _LL4:
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
{struct _tuple0 _tmpBD=({struct _tuple0 _tmp4E9;({void*_tmp5E4=Cyc_Tcutil_compress(t1);_tmp4E9.f1=_tmp5E4;}),({void*_tmp5E3=Cyc_Tcutil_compress(t2);_tmp4E9.f2=_tmp5E3;});_tmp4E9;});struct _tuple0 _stmttmpB=_tmpBD;struct _tuple0 _tmpBE=_stmttmpB;void*_tmpC0;void*_tmpBF;switch(*((int*)_tmpBE.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f2)->tag == 3U){_LL1: _tmpBF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f1)->f1).elt_type;_tmpC0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f2)->f1).elt_type;_LL2: {void*t1a=_tmpBF;void*t2a=_tmpC0;
# 587
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->tag == 3U){_LL3: _LL4:
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
struct Cyc_Absyn_Exp*_tmpCC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpCC;
void*_tmpCD=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpCD;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmpD0=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,({struct _fat_ptr _tmp5EF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp4EE.f1=_tmp5EF;});_tmp4EE;});void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({unsigned _tmp5F1=e->loc;struct _fat_ptr _tmp5F0=({const char*_tmpCF="expecting numeric type but found %s";_tag_fat(_tmpCF,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5F1,_tmp5F0,_tag_fat(_tmpCE,sizeof(void*),1U));});});
return t;}
# 657
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpD1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpD1;int _tmpD2=_stmttmpC;switch(_tmpD2){case 0U: _LL1: _LL2:
 return({void*_tmpD3=0U;({struct Cyc_Tcenv_Tenv*_tmp5F5=te;unsigned _tmp5F4=loc;void**_tmp5F3=topt;struct _fat_ptr _tmp5F2=({const char*_tmpD4="primitive operator has 0 arguments";_tag_fat(_tmpD4,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp5F5,_tmp5F4,_tmp5F3,_tmp5F2,_tag_fat(_tmpD3,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LL0;case 2U: _LL5: _LL6:
 t=({struct Cyc_Tcenv_Tenv*_tmp5FA=te;unsigned _tmp5F9=loc;void**_tmp5F8=topt;enum Cyc_Absyn_Primop _tmp5F7=p;struct Cyc_Absyn_Exp*_tmp5F6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5FA,_tmp5F9,_tmp5F8,_tmp5F7,_tmp5F6,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpD5=0U;({struct Cyc_Tcenv_Tenv*_tmp5FE=te;unsigned _tmp5FD=loc;void**_tmp5FC=topt;struct _fat_ptr _tmp5FB=({const char*_tmpD6="primitive operator has > 2 arguments";_tag_fat(_tmpD6,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp5FE,_tmp5FD,_tmp5FC,_tmp5FB,_tag_fat(_tmpD5,sizeof(void*),0U));});});}_LL0:;}
# 665
return t;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 668
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
void*_tmpD7=Cyc_Tcutil_compress(t);void*t=_tmpD7;
void*_tmpD8=t;struct Cyc_List_List*_tmpD9;struct Cyc_Absyn_Tqual _tmpDB;void*_tmpDA;struct Cyc_List_List*_tmpDC;struct Cyc_Absyn_Datatypefield*_tmpDD;struct Cyc_Absyn_Aggrdecl*_tmpDE;switch(*((int*)_tmpD8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpDE=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpDE;
# 672
if(ad->impl == 0){
({void*_tmpDF=0U;({unsigned _tmp600=loc;struct _fat_ptr _tmp5FF=({const char*_tmpE0="attempt to write an abstract aggregate";_tag_fat(_tmpE0,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp600,_tmp5FF,_tag_fat(_tmpDF,sizeof(void*),0U));});});
return 0;}else{
# 676
_tmpDC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpDD=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmpDD;
# 688
{struct Cyc_List_List*_tmpE5=df->typs;struct Cyc_List_List*fs=_tmpE5;for(0;fs != 0;fs=fs->tl){
struct _tuple16*_tmpE6=(struct _tuple16*)fs->hd;struct _tuple16*_stmttmpD=_tmpE6;struct _tuple16*_tmpE7=_stmttmpD;void*_tmpE9;struct Cyc_Absyn_Tqual _tmpE8;_LLE: _tmpE8=_tmpE7->f1;_tmpE9=_tmpE7->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmpE8;void*t=_tmpE9;
if(tq.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpEC=({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U,({struct _fat_ptr _tmp601=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(df->name));_tmp4EF.f1=_tmp601;});_tmp4EF;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEC;({unsigned _tmp603=loc;struct _fat_ptr _tmp602=({const char*_tmpEB="attempt to over-write a datatype field (%s) with a const member";_tag_fat(_tmpEB,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp603,_tmp602,_tag_fat(_tmpEA,sizeof(void*),1U));});});
return 0;}
# 694
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 696
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpDC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD8)->f2;_LL4: {struct Cyc_List_List*fs=_tmpDC;
# 678
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpE1=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmpE1;
if((f->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpE4=({struct Cyc_String_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=0U,_tmp4F0.f1=(struct _fat_ptr)((struct _fat_ptr)*f->name);_tmp4F0;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({unsigned _tmp605=loc;struct _fat_ptr _tmp604=({const char*_tmpE3="attempt to over-write an aggregate with const member %s";_tag_fat(_tmpE3,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp605,_tmp604,_tag_fat(_tmpE2,sizeof(void*),1U));});});
return 0;}
# 684
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 686
return 1;}case 4U: _LL7: _tmpDA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).elt_type;_tmpDB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).tq;_LL8: {void*elt_type=_tmpDA;struct Cyc_Absyn_Tqual tq=_tmpDB;
# 698
if(tq.real_const){
({void*_tmpED=0U;({unsigned _tmp607=loc;struct _fat_ptr _tmp606=({const char*_tmpEE="attempt to over-write a const array";_tag_fat(_tmpEE,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp607,_tmp606,_tag_fat(_tmpED,sizeof(void*),0U));});});
return 0;}
# 702
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpD9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD8)->f1;_LLA: {struct Cyc_List_List*fs=_tmpD9;
# 704
for(0;fs != 0;fs=fs->tl){
struct _tuple16*_tmpEF=(struct _tuple16*)fs->hd;struct _tuple16*_stmttmpE=_tmpEF;struct _tuple16*_tmpF0=_stmttmpE;void*_tmpF2;struct Cyc_Absyn_Tqual _tmpF1;_LL11: _tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmpF1;void*t=_tmpF2;
if(tq.real_const){
({void*_tmpF3=0U;({unsigned _tmp609=loc;struct _fat_ptr _tmp608=({const char*_tmpF4="attempt to over-write a tuple field with a const member";_tag_fat(_tmpF4,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp609,_tmp608,_tag_fat(_tmpF3,sizeof(void*),0U));});});
return 0;}
# 710
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 712
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 720
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 723
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmpF5=e->r;void*_stmttmpF=_tmpF5;void*_tmpF6=_stmttmpF;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;struct _fat_ptr*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;struct _fat_ptr*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Vardecl*_tmp100;struct Cyc_Absyn_Vardecl*_tmp101;struct Cyc_Absyn_Vardecl*_tmp102;struct Cyc_Absyn_Vardecl*_tmp103;switch(*((int*)_tmpF6)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)){case 3U: _LL1: _tmp103=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp103;
_tmp102=vd;goto _LL4;}case 4U: _LL3: _tmp102=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp102;
_tmp101=vd;goto _LL6;}case 5U: _LL5: _tmp101=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp101;
_tmp100=vd;goto _LL8;}case 1U: _LL7: _tmp100=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp100;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmpFE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmpFF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmpFE;struct Cyc_Absyn_Exp*e2=_tmpFF;
# 730
{void*_tmp104=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp104;void*_tmp105=_stmttmp10;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Tqual _tmp107;struct Cyc_Absyn_Tqual _tmp108;switch(*((int*)_tmp105)){case 3U: _LL18: _tmp108=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp105)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp108;
_tmp107=tq;goto _LL1B;}case 4U: _LL1A: _tmp107=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp105)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp107;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp106=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp105)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp106;
# 734
struct _tuple15 _tmp109=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _stmttmp11=_tmp109;struct _tuple15 _tmp10A=_stmttmp11;int _tmp10C;unsigned _tmp10B;_LL21: _tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;_LL22: {unsigned i=_tmp10B;int known=_tmp10C;
if(!known){
({void*_tmp10D=0U;({unsigned _tmp60B=e->loc;struct _fat_ptr _tmp60A=({const char*_tmp10E="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp10E,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp60B,_tmp60A,_tag_fat(_tmp10D,sizeof(void*),0U));});});
return;}
# 739
{struct _handler_cons _tmp10F;_push_handler(& _tmp10F);{int _tmp111=0;if(setjmp(_tmp10F.handler))_tmp111=1;if(!_tmp111){
{struct _tuple16*_tmp112=((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple16*_stmttmp12=_tmp112;struct _tuple16*_tmp113=_stmttmp12;struct Cyc_Absyn_Tqual _tmp114;_LL24: _tmp114=_tmp113->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp114;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 740
;_pop_handler();}else{void*_tmp110=(void*)Cyc_Core_get_exn_thrown();void*_tmp115=_tmp110;void*_tmp116;if(((struct Cyc_List_Nth_exn_struct*)_tmp115)->tag == Cyc_List_Nth){_LL27: _LL28:
# 742
 return;}else{_LL29: _tmp116=_tmp115;_LL2A: {void*exn=_tmp116;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 746
goto _LL0;}case 21U: _LLB: _tmpFC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmpFD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmpFC;struct _fat_ptr*f=_tmpFD;
# 748
{void*_tmp117=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp117;void*_tmp118=_stmttmp13;struct Cyc_List_List*_tmp119;struct Cyc_Absyn_Aggrdecl**_tmp11A;switch(*((int*)_tmp118)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp118)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp118)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp11A=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp118)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp11A;
# 750
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp119=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp118)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp119;
# 755
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 760
goto _LL0;}case 22U: _LLD: _tmpFA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmpFB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmpFA;struct _fat_ptr*f=_tmpFB;
# 762
{void*_tmp11B=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp11B;void*_tmp11C=_stmttmp14;struct Cyc_Absyn_Tqual _tmp11E;void*_tmp11D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C)->tag == 3U){_LL33: _tmp11D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C)->f1).elt_type;_tmp11E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C)->f1).elt_tq;_LL34: {void*elt_typ=_tmp11D;struct Cyc_Absyn_Tqual tq=_tmp11E;
# 764
if(!tq.real_const){
void*_tmp11F=Cyc_Tcutil_compress(elt_typ);void*_stmttmp15=_tmp11F;void*_tmp120=_stmttmp15;struct Cyc_List_List*_tmp121;struct Cyc_Absyn_Aggrdecl**_tmp122;switch(*((int*)_tmp120)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp120)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp120)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp122=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp120)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp122;
# 767
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp121=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp120)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp121;
# 772
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 778
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 781
goto _LL0;}case 20U: _LLF: _tmpF9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpF9;
# 783
{void*_tmp123=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp123;void*_tmp124=_stmttmp16;struct Cyc_Absyn_Tqual _tmp125;struct Cyc_Absyn_Tqual _tmp126;switch(*((int*)_tmp124)){case 3U: _LL3F: _tmp126=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp124)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp126;
_tmp125=tq;goto _LL42;}case 4U: _LL41: _tmp125=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp124)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp125;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 788
goto _LL0;}case 12U: _LL11: _tmpF8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpF8;
_tmpF7=e1;goto _LL14;}case 13U: _LL13: _tmpF7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpF7;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
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
return t;}}
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
struct Cyc_List_List _tmp134=({struct Cyc_List_List _tmp4F6;_tmp4F6.hd=e3,_tmp4F6.tl=0;_tmp4F6;});struct Cyc_List_List l1=_tmp134;
struct Cyc_List_List _tmp135=({struct Cyc_List_List _tmp4F5;_tmp4F5.hd=e2,_tmp4F5.tl=& l1;_tmp4F5;});struct Cyc_List_List l2=_tmp135;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp61D=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp61D,t,& l2);})){
({struct Cyc_String_pa_PrintArg_struct _tmp138=({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U,({
struct _fat_ptr _tmp61E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4F4.f1=_tmp61E;});_tmp4F4;});struct Cyc_String_pa_PrintArg_struct _tmp139=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({struct _fat_ptr _tmp61F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp4F3.f1=_tmp61F;});_tmp4F3;});void*_tmp136[2U];_tmp136[0]=& _tmp138,_tmp136[1]=& _tmp139;({unsigned _tmp621=loc;struct _fat_ptr _tmp620=({const char*_tmp137="conditional clause types do not match: %s != %s";_tag_fat(_tmp137,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp621,_tmp620,_tag_fat(_tmp136,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 837
return t;}}}
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
{void*_tmp13E=Cyc_Tcutil_compress(t1);void*_stmttmp17=_tmp13E;void*_tmp13F=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->tag == 4U){_LL1: _LL2:
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
# 885
void*result=_tmp148;
# 887
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}}else{
# 892
enum Cyc_Absyn_Primop _tmp14D=(enum Cyc_Absyn_Primop)po->v;enum Cyc_Absyn_Primop p=_tmp14D;
struct Cyc_Absyn_Exp*_tmp14E=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp14E;
void*_tmp14F=Cyc_Tcexp_tcBinPrimop(te,loc,0,p,e1copy,e2);void*t_result=_tmp14F;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
t_result=t1;else{
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp150=({struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,({
# 900
struct _fat_ptr _tmp63D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FA.f1=_tmp63D;});_tmp4FA;});struct Cyc_String_pa_PrintArg_struct _tmp154=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,({
struct _fat_ptr _tmp63E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F9.f1=_tmp63E;});_tmp4F9;});void*_tmp151[2U];_tmp151[0]=& _tmp153,_tmp151[1]=& _tmp154;({struct Cyc_Tcenv_Tenv*_tmp642=te;unsigned _tmp641=loc;void**_tmp640=topt;struct _fat_ptr _tmp63F=({const char*_tmp152="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_fat(_tmp152,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp642,_tmp641,_tmp640,_tmp63F,_tag_fat(_tmp151,sizeof(void*),2U));});});
# 898
void*result=_tmp150;
# 902
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}}
# 906
return t_result;}
# 908
return t1;}}}
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
{void*_tmp155=Cyc_Tcutil_compress(t1);void*_stmttmp18=_tmp155;void*_tmp156=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)->f1 == 0){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp156)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 932
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp157=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp157;struct Cyc_Absyn_Datatypefield _tmp158=_stmttmp19;enum Cyc_Absyn_Scope _tmp15C;unsigned _tmp15B;struct Cyc_List_List*_tmp15A;struct _tuple1*_tmp159;_LLA: _tmp159=_tmp158.name;_tmp15A=_tmp158.typs;_tmp15B=_tmp158.loc;_tmp15C=_tmp158.sc;_LLB: {struct _tuple1*n=_tmp159;struct Cyc_List_List*typs=_tmp15A;unsigned loc=_tmp15B;enum Cyc_Absyn_Scope sc=_tmp15C;
# 935
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple16*)typs->hd)).f2);
# 938
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 942
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp15D=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp15D;struct Cyc_Absyn_Datatypefield _tmp15E=_stmttmp1A;enum Cyc_Absyn_Scope _tmp162;unsigned _tmp161;struct Cyc_List_List*_tmp160;struct _tuple1*_tmp15F;_LLD: _tmp15F=_tmp15E.name;_tmp160=_tmp15E.typs;_tmp161=_tmp15E.loc;_tmp162=_tmp15E.sc;_LLE: {struct _tuple1*n=_tmp15F;struct Cyc_List_List*typs=_tmp160;unsigned loc=_tmp161;enum Cyc_Absyn_Scope sc=_tmp162;
# 945
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple16*)typs->hd)).f2);
# 949
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp647=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp646=e;Cyc_Tcutil_coerce_arg(_tmp647,_tmp646,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 954
({struct Cyc_String_pa_PrintArg_struct _tmp165=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,({
struct _fat_ptr _tmp648=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tu));_tmp4FC.f1=_tmp648;});_tmp4FC;});struct Cyc_String_pa_PrintArg_struct _tmp166=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,({struct _fat_ptr _tmp649=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FB.f1=_tmp649;});_tmp4FB;});void*_tmp163[2U];_tmp163[0]=& _tmp165,_tmp163[1]=& _tmp166;({unsigned _tmp64B=e->loc;struct _fat_ptr _tmp64A=({const char*_tmp164="can't find a field in %s to inject a value of type %s";_tag_fat(_tmp164,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp64B,_tmp64A,_tag_fat(_tmp163,sizeof(void*),2U));});});
return 0;}
# 960
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 966
struct Cyc_List_List*_tmp167=args;struct Cyc_List_List*es=_tmp167;
int _tmp168=0;int arg_cnt=_tmp168;
struct Cyc_Tcenv_Tenv*_tmp169=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp169;
struct Cyc_Tcenv_Tenv*_tmp16A=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp16A;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 975
void*_tmp16B=t;void*_tmp171;void*_tmp170;void*_tmp16F;void*_tmp16E;struct Cyc_Absyn_Tqual _tmp16D;void*_tmp16C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->tag == 3U){_LL1: _tmp16C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).elt_type;_tmp16D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).elt_tq;_tmp16E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).rgn;_tmp16F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).nullable;_tmp170=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).bounds;_tmp171=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16B)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp16C;struct Cyc_Absyn_Tqual tq=_tmp16D;void*rgn=_tmp16E;void*x=_tmp16F;void*b=_tmp170;void*zt=_tmp171;
# 980
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 982
Cyc_Tcutil_check_nonzero_bound(loc,b);{
void*_tmp172=Cyc_Tcutil_compress(t1);void*_stmttmp1B=_tmp172;void*_tmp173=_stmttmp1B;struct Cyc_List_List*_tmp180;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17B;struct Cyc_Absyn_VarargInfo*_tmp17A;int _tmp179;struct Cyc_List_List*_tmp178;void*_tmp177;struct Cyc_Absyn_Tqual _tmp176;void*_tmp175;struct Cyc_List_List*_tmp174;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->tag == 5U){_LL6: _tmp174=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).tvars;_tmp175=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).effect;_tmp176=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).ret_tqual;_tmp177=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).ret_type;_tmp178=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).args;_tmp179=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).c_varargs;_tmp17A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).cyc_varargs;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).rgn_po;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).attributes;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).requires_clause;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).requires_relns;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).ensures_clause;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).ensures_relns;_LL7: {struct Cyc_List_List*tvars=_tmp174;void*eff=_tmp175;struct Cyc_Absyn_Tqual res_tq=_tmp176;void*res_typ=_tmp177;struct Cyc_List_List*args_info=_tmp178;int c_vararg=_tmp179;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp17A;struct Cyc_List_List*rgn_po=_tmp17B;struct Cyc_List_List*atts=_tmp17C;struct Cyc_Absyn_Exp*req=_tmp17D;struct Cyc_List_List*req_relns=_tmp17E;struct Cyc_Absyn_Exp*ens=_tmp17F;struct Cyc_List_List*ens_relns=_tmp180;
# 987
if(tvars != 0 || rgn_po != 0)
({void*_tmp181=0U;({unsigned _tmp64D=e->loc;struct _fat_ptr _tmp64C=({const char*_tmp182="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp182,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp64D,_tmp64C,_tag_fat(_tmp181,sizeof(void*),0U));});});
# 991
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 993
while(es != 0 && args_info != 0){
# 995
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp64F=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp64E=e1;Cyc_Tcutil_coerce_arg(_tmp64F,_tmp64E,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp192="actual argument has type ";_tag_fat(_tmp192,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp191=" but formal has type ";_tag_fat(_tmp191,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp652=({unsigned long _tmp651=({unsigned long _tmp650=Cyc_strlen((struct _fat_ptr)s0);_tmp650 + Cyc_strlen((struct _fat_ptr)s1);});_tmp651 + Cyc_strlen((struct _fat_ptr)s2);});_tmp652 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({void*_tmp183=0U;({unsigned _tmp655=e1->loc;struct _fat_ptr _tmp654=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp186=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,_tmp500.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp500;});struct Cyc_String_pa_PrintArg_struct _tmp187=({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0U,_tmp4FF.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp4FF;});struct Cyc_String_pa_PrintArg_struct _tmp188=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,_tmp4FE.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp4FE;});struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,_tmp4FD.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp4FD;});void*_tmp184[4U];_tmp184[0]=& _tmp186,_tmp184[1]=& _tmp187,_tmp184[2]=& _tmp188,_tmp184[3]=& _tmp189;({struct _fat_ptr _tmp653=({const char*_tmp185="%s\n\t%s\n%s\n\t%s.";_tag_fat(_tmp185,sizeof(char),15U);});Cyc_aprintf(_tmp653,_tag_fat(_tmp184,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp655,_tmp654,_tag_fat(_tmp183,sizeof(void*),0U));});});else{
# 1007
({void*_tmp18A=0U;({unsigned _tmp658=e1->loc;struct _fat_ptr _tmp657=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18D=({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U,_tmp504.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp504;});struct Cyc_String_pa_PrintArg_struct _tmp18E=({struct Cyc_String_pa_PrintArg_struct _tmp503;_tmp503.tag=0U,_tmp503.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp503;});struct Cyc_String_pa_PrintArg_struct _tmp18F=({struct Cyc_String_pa_PrintArg_struct _tmp502;_tmp502.tag=0U,_tmp502.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp502;});struct Cyc_String_pa_PrintArg_struct _tmp190=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,_tmp501.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp501;});void*_tmp18B[4U];_tmp18B[0]=& _tmp18D,_tmp18B[1]=& _tmp18E,_tmp18B[2]=& _tmp18F,_tmp18B[3]=& _tmp190;({struct _fat_ptr _tmp656=({const char*_tmp18C="%s%s%s%s.";_tag_fat(_tmp18C,sizeof(char),10U);});Cyc_aprintf(_tmp656,_tag_fat(_tmp18B,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp658,_tmp657,_tag_fat(_tmp18A,sizeof(void*),0U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 1012
if(alias_coercion)
({struct Cyc_List_List*_tmp659=({struct Cyc_List_List*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=(void*)arg_cnt,_tmp193->tl=*alias_arg_exps;_tmp193;});*alias_arg_exps=_tmp659;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp194=0U;({unsigned _tmp65B=e1->loc;struct _fat_ptr _tmp65A=({const char*_tmp195="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp195,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp65B,_tmp65A,_tag_fat(_tmp194,sizeof(void*),0U));});});
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 1023
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp196=(void*)a->hd;void*_stmttmp1C=_tmp196;void*_tmp197=_stmttmp1C;int _tmp19A;int _tmp199;enum Cyc_Absyn_Format_Type _tmp198;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp197)->tag == 19U){_LLB: _tmp198=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp197)->f1;_tmp199=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp197)->f2;_tmp19A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp197)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp198;int fmt_arg_pos=_tmp199;int arg_start_pos=_tmp19A;
# 1027
{struct _handler_cons _tmp19B;_push_handler(& _tmp19B);{int _tmp19D=0;if(setjmp(_tmp19B.handler))_tmp19D=1;if(!_tmp19D){
# 1029
{struct Cyc_Absyn_Exp*_tmp19E=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp19E;
# 1031
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 1035
fmt_args=({struct Cyc_Core_Opt*_tmp19F=_cycalloc(sizeof(*_tmp19F));({struct Cyc_List_List*_tmp65C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp19F->v=_tmp65C;});_tmp19F;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1A0=ft;switch(_tmp1A0){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 1039
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 1042
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 1045
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 1048
goto _LLF;}_LLF:;}}
# 1029
;_pop_handler();}else{void*_tmp19C=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A1=_tmp19C;void*_tmp1A2;if(((struct Cyc_List_Nth_exn_struct*)_tmp1A1)->tag == Cyc_List_Nth){_LL17: _LL18:
# 1050
({void*_tmp1A3=0U;({unsigned _tmp65E=loc;struct _fat_ptr _tmp65D=({const char*_tmp1A4="bad format arguments";_tag_fat(_tmp1A4,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp65E,_tmp65D,_tag_fat(_tmp1A3,sizeof(void*),0U));});});goto _LL16;}else{_LL19: _tmp1A2=_tmp1A1;_LL1A: {void*exn=_tmp1A2;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1055
if(args_info != 0)({void*_tmp1A5=0U;({unsigned _tmp660=loc;struct _fat_ptr _tmp65F=({const char*_tmp1A6="too few arguments for function";_tag_fat(_tmp1A6,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp660,_tmp65F,_tag_fat(_tmp1A5,sizeof(void*),0U));});});else{
# 1057
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({void*_tmp1A7=0U;({unsigned _tmp662=loc;struct _fat_ptr _tmp661=({const char*_tmp1A8="too many arguments for function";_tag_fat(_tmp1A8,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp662,_tmp661,_tag_fat(_tmp1A7,sizeof(void*),0U));});});else{
# 1064
struct Cyc_Absyn_VarargInfo _tmp1A9=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1D=_tmp1A9;struct Cyc_Absyn_VarargInfo _tmp1AA=_stmttmp1D;int _tmp1AC;void*_tmp1AB;_LL1C: _tmp1AB=_tmp1AA.type;_tmp1AC=_tmp1AA.inject;_LL1D: {void*vt=_tmp1AB;int inject=_tmp1AC;
struct Cyc_Absyn_VarargCallInfo*_tmp1AD=({struct Cyc_Absyn_VarargCallInfo*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->num_varargs=0,_tmp1C4->injectors=0,_tmp1C4->vai=cyc_vararg;_tmp1C4;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1AD;
# 1068
*vararg_call_info=vci;
# 1070
if(!inject)
# 1072
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp664=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp663=e1;Cyc_Tcutil_coerce_arg(_tmp664,_tmp663,vt,& alias_coercion);})){
({struct Cyc_String_pa_PrintArg_struct _tmp1B0=({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U,({
struct _fat_ptr _tmp665=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(vt));_tmp506.f1=_tmp665;});_tmp506;});struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({struct _fat_ptr _tmp666=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp505.f1=_tmp666;});_tmp505;});void*_tmp1AE[2U];_tmp1AE[0]=& _tmp1B0,_tmp1AE[1]=& _tmp1B1;({unsigned _tmp668=loc;struct _fat_ptr _tmp667=({const char*_tmp1AF="vararg requires type %s but argument has type %s";_tag_fat(_tmp1AF,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp668,_tmp667,_tag_fat(_tmp1AE,sizeof(void*),2U));});});
Cyc_Unify_explain_failure();}
# 1082
if(alias_coercion)
({struct Cyc_List_List*_tmp669=({struct Cyc_List_List*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->hd=(void*)arg_cnt,_tmp1B2->tl=*alias_arg_exps;_tmp1B2;});*alias_arg_exps=_tmp669;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vt)&& !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1B3=0U;({unsigned _tmp66B=e1->loc;struct _fat_ptr _tmp66A=({const char*_tmp1B4="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp1B4,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp66B,_tmp66A,_tag_fat(_tmp1B3,sizeof(void*),0U));});});}else{
# 1091
void*_tmp1B5=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1E=_tmp1B5;void*_tmp1B6=_stmttmp1E;struct Cyc_List_List*_tmp1B8;struct Cyc_Absyn_Datatypedecl*_tmp1B7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B6)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B6)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B6)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1B7=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B6)->f1)->f1).KnownDatatype).val;_tmp1B8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B6)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1B7;struct Cyc_List_List*targs=_tmp1B8;
# 1095
struct Cyc_Absyn_Datatypedecl*_tmp1B9=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1B9;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1BC=({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0U,({
struct _fat_ptr _tmp66C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(vt));_tmp507.f1=_tmp66C;});_tmp507;});void*_tmp1BA[1U];_tmp1BA[0]=& _tmp1BC;({unsigned _tmp66E=loc;struct _fat_ptr _tmp66D=({const char*_tmp1BB="can't inject into abstract datatype %s";_tag_fat(_tmp1BB,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp66E,_tmp66D,_tag_fat(_tmp1BA,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 1106
({void*_tmp66F=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp66F,Cyc_Tcenv_curr_rgn(te));});{
# 1108
struct Cyc_List_List*_tmp1BD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp1BD;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1113
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt))&& !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1BE=0U;({unsigned _tmp671=e1->loc;struct _fat_ptr _tmp670=({const char*_tmp1BF="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp1BF,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp671,_tmp670,_tag_fat(_tmp1BE,sizeof(void*),0U));});});}{
# 1119
struct Cyc_Absyn_Datatypefield*_tmp1C0=({struct Cyc_Tcenv_Tenv*_tmp675=te;struct Cyc_Absyn_Exp*_tmp674=e1;void*_tmp673=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp672=inst;Cyc_Tcexp_tcInjection(_tmp675,_tmp674,_tmp673,_tmp672,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp1C0;
if(f != 0)
({struct Cyc_List_List*_tmp677=({
struct Cyc_List_List*_tmp676=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp676,({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));
_tmp1C1->hd=f,_tmp1C1->tl=0;_tmp1C1;}));});
# 1121
vci->injectors=_tmp677;});}}}
# 1125
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({void*_tmp1C2=0U;({unsigned _tmp679=loc;struct _fat_ptr _tmp678=({const char*_tmp1C3="bad inject vararg type";_tag_fat(_tmp1C3,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp679,_tmp678,_tag_fat(_tmp1C2,sizeof(void*),0U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1131
if(*alias_arg_exps == 0)
# 1140 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1142
return res_typ;}}}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1C7=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({struct _fat_ptr _tmp67A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp508.f1=_tmp67A;});_tmp508;});void*_tmp1C5[1U];_tmp1C5[0]=& _tmp1C7;({struct Cyc_Tcenv_Tenv*_tmp67E=te;unsigned _tmp67D=loc;void**_tmp67C=topt;struct _fat_ptr _tmp67B=({const char*_tmp1C6="expected pointer to function but found %s";_tag_fat(_tmp1C6,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp67E,_tmp67D,_tmp67C,_tmp67B,_tag_fat(_tmp1C5,sizeof(void*),1U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1145
 return({struct Cyc_String_pa_PrintArg_struct _tmp1CA=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({struct _fat_ptr _tmp67F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp509.f1=_tmp67F;});_tmp509;});void*_tmp1C8[1U];_tmp1C8[0]=& _tmp1CA;({struct Cyc_Tcenv_Tenv*_tmp683=te;unsigned _tmp682=loc;void**_tmp681=topt;struct _fat_ptr _tmp680=({const char*_tmp1C9="expected pointer to function but found %s";_tag_fat(_tmp1C9,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp683,_tmp682,_tmp681,_tmp680,_tag_fat(_tmp1C8,sizeof(void*),1U));});});}_LL0:;}}}
# 1149
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1CB=Cyc_Absyn_exn_type();void*exception_type=_tmp1CB;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp685=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp684=e;Cyc_Tcutil_coerce_arg(_tmp685,_tmp684,exception_type,& bogus);}))
({struct Cyc_String_pa_PrintArg_struct _tmp1CE=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,({struct _fat_ptr _tmp686=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(exception_type));_tmp50B.f1=_tmp686;});_tmp50B;});struct Cyc_String_pa_PrintArg_struct _tmp1CF=({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0U,({
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
{void*_tmp1D0=t1;void*_tmp1D6;void*_tmp1D5;void*_tmp1D4;void*_tmp1D3;struct Cyc_Absyn_Tqual _tmp1D2;void*_tmp1D1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->tag == 3U){_LL1: _tmp1D1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_type;_tmp1D2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).elt_tq;_tmp1D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).rgn;_tmp1D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).nullable;_tmp1D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).bounds;_tmp1D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D0)->f1).ptr_atts).zero_term;_LL2: {void*t0=_tmp1D1;struct Cyc_Absyn_Tqual tq=_tmp1D2;void*rt=_tmp1D3;void*x=_tmp1D4;void*b=_tmp1D5;void*zt=_tmp1D6;
# 1169
{void*_tmp1D7=Cyc_Tcutil_compress(t0);void*_stmttmp1F=_tmp1D7;void*_tmp1D8=_stmttmp1F;struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E0;struct Cyc_Absyn_VarargInfo*_tmp1DF;int _tmp1DE;struct Cyc_List_List*_tmp1DD;void*_tmp1DC;struct Cyc_Absyn_Tqual _tmp1DB;void*_tmp1DA;struct Cyc_List_List*_tmp1D9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->tag == 5U){_LL6: _tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).tvars;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).effect;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).ret_tqual;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).ret_type;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).args;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).c_varargs;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).cyc_varargs;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).rgn_po;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).attributes;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).requires_clause;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).requires_relns;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).ensures_clause;_tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D8)->f1).ensures_relns;_LL7: {struct Cyc_List_List*tvars=_tmp1D9;void*eff=_tmp1DA;struct Cyc_Absyn_Tqual rtq=_tmp1DB;void*rtyp=_tmp1DC;struct Cyc_List_List*args=_tmp1DD;int c_varargs=_tmp1DE;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp1DF;struct Cyc_List_List*rpo=_tmp1E0;struct Cyc_List_List*atts=_tmp1E1;struct Cyc_Absyn_Exp*req=_tmp1E2;struct Cyc_List_List*req_relns=_tmp1E3;struct Cyc_Absyn_Exp*ens=_tmp1E4;struct Cyc_List_List*ens_relns=_tmp1E5;
# 1171
struct Cyc_List_List*instantiation=0;
# 1173
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp68E=loc;struct Cyc_Tcenv_Tenv*_tmp68D=te;struct Cyc_List_List*_tmp68C=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp68B=k;Cyc_Tctyp_check_type(_tmp68E,_tmp68D,_tmp68C,_tmp68B,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));({struct _tuple13*_tmp68F=({struct _tuple13*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp1E6->f2=(void*)ts->hd;_tmp1E6;});_tmp1E7->hd=_tmp68F;}),_tmp1E7->tl=instantiation;_tmp1E7;});}
# 1179
if(ts != 0)
return({void*_tmp1E8=0U;({struct Cyc_Tcenv_Tenv*_tmp693=te;unsigned _tmp692=loc;void**_tmp691=topt;struct _fat_ptr _tmp690=({const char*_tmp1E9="too many type variables in instantiation";_tag_fat(_tmp1E9,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp693,_tmp692,_tmp691,_tmp690,_tag_fat(_tmp1E8,sizeof(void*),0U));});});
# 1184
if(tvars == 0){
rpo=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,rpo);
Cyc_Tcenv_check_rgn_partial_order(te,loc,rpo);
rpo=0;}{
# 1189
void*new_fn_typ=({
struct Cyc_List_List*_tmp694=instantiation;Cyc_Tcutil_substitute(_tmp694,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));
_tmp1EB->tag=5U,(_tmp1EB->f1).tvars=tvars,(_tmp1EB->f1).effect=eff,(_tmp1EB->f1).ret_tqual=rtq,(_tmp1EB->f1).ret_type=rtyp,(_tmp1EB->f1).args=args,(_tmp1EB->f1).c_varargs=c_varargs,(_tmp1EB->f1).cyc_varargs=cyc_varargs,(_tmp1EB->f1).rgn_po=rpo,(_tmp1EB->f1).attributes=atts,(_tmp1EB->f1).requires_clause=req,(_tmp1EB->f1).requires_relns=req_relns,(_tmp1EB->f1).ensures_clause=ens,(_tmp1EB->f1).ensures_relns=ens_relns;_tmp1EB;}));});
# 1194
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->tag=3U,(_tmp1EA->f1).elt_type=new_fn_typ,(_tmp1EA->f1).elt_tq=tq,((_tmp1EA->f1).ptr_atts).rgn=rt,((_tmp1EA->f1).ptr_atts).nullable=x,((_tmp1EA->f1).ptr_atts).bounds=b,((_tmp1EA->f1).ptr_atts).zero_term=zt,((_tmp1EA->f1).ptr_atts).ptrloc=0;_tmp1EA;});}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1197
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1200
return({struct Cyc_String_pa_PrintArg_struct _tmp1EE=({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0U,({
struct _fat_ptr _tmp695=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp50C.f1=_tmp695;});_tmp50C;});void*_tmp1EC[1U];_tmp1EC[0]=& _tmp1EE;({struct Cyc_Tcenv_Tenv*_tmp699=te;unsigned _tmp698=loc;void**_tmp697=topt;struct _fat_ptr _tmp696=({const char*_tmp1ED="expecting polymorphic type but found %s";_tag_fat(_tmp1ED,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp699,_tmp698,_tmp697,_tmp696,_tag_fat(_tmp1EC,sizeof(void*),1U));});});}}
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
# 1224
void*result=_tmp1EF;
# 1226
Cyc_Unify_explain_failure();
return result;}}}}
# 1233
{struct _tuple0 _tmp1F4=({struct _tuple0 _tmp50F;_tmp50F.f1=e->r,({void*_tmp6A9=Cyc_Tcutil_compress(t);_tmp50F.f2=_tmp6A9;});_tmp50F;});struct _tuple0 _stmttmp20=_tmp1F4;struct _tuple0 _tmp1F5=_stmttmp20;void*_tmp1F9;void*_tmp1F8;void*_tmp1F7;int _tmp1F6;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F5.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->tag == 3U){_LL1: _tmp1F6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F5.f1)->f1).fat_result;_tmp1F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->f1).ptr_atts).nullable;_tmp1F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->f1).ptr_atts).bounds;_tmp1F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp1F6;void*nbl=_tmp1F7;void*bds=_tmp1F8;void*zt=_tmp1F9;
# 1237
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp1FA=({void*_tmp6AA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6AA,bds);});struct Cyc_Absyn_Exp*eopt=_tmp1FA;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp1FB=e;struct Cyc_Absyn_Exp*e2=_tmp1FB;
if((Cyc_Evexp_eval_const_uint_exp(e2)).f1 == (unsigned)1)
({void*_tmp1FC=0U;({unsigned _tmp6AC=loc;struct _fat_ptr _tmp6AB=({const char*_tmp1FD="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp1FD,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6AC,_tmp6AB,_tag_fat(_tmp1FC,sizeof(void*),0U));});});}}
# 1245
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1248
return t;}}
# 1252
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp1FE=0;void**topt2=_tmp1FE;
struct Cyc_Absyn_Tqual _tmp1FF=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp1FF;
int _tmp200=0;int nullable=_tmp200;
if(topt != 0){
void*_tmp201=Cyc_Tcutil_compress(*topt);void*_stmttmp21=_tmp201;void*_tmp202=_stmttmp21;void*_tmp205;struct Cyc_Absyn_Tqual _tmp204;void**_tmp203;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->tag == 3U){_LL1: _tmp203=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->f1).elt_type;_tmp204=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->f1).elt_tq;_tmp205=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp203;struct Cyc_Absyn_Tqual tq=_tmp204;void*n=_tmp205;
# 1259
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1273
({struct Cyc_Tcenv_Tenv*_tmp6AE=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp6AD=topt2;Cyc_Tcexp_tcExpNoInst(_tmp6AE,_tmp6AD,e);});
# 1276
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp206=0U;({unsigned _tmp6B0=e->loc;struct _fat_ptr _tmp6AF=({const char*_tmp207="Cannot take the address of an alias-free path";_tag_fat(_tmp207,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6B0,_tmp6AF,_tag_fat(_tmp206,sizeof(void*),0U));});});
# 1282
{void*_tmp208=e->r;void*_stmttmp22=_tmp208;void*_tmp209=_stmttmp22;struct Cyc_Absyn_Exp*_tmp20B;struct Cyc_Absyn_Exp*_tmp20A;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->tag == 23U){_LL6: _tmp20A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp20B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp20A;struct Cyc_Absyn_Exp*e2=_tmp20B;
# 1284
{void*_tmp20C=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp23=_tmp20C;void*_tmp20D=_stmttmp23;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp20D)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1289
({void*_tmp6B1=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp6B1;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1292
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1297
{void*_tmp20E=e->r;void*_stmttmp24=_tmp20E;void*_tmp20F=_stmttmp24;switch(*((int*)_tmp20F)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp20F)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp20F)->f3 == 1){_LL12: _LL13:
# 1300
({void*_tmp210=0U;({unsigned _tmp6B3=e->loc;struct _fat_ptr _tmp6B2=({const char*_tmp211="cannot take the address of a @tagged union member";_tag_fat(_tmp211,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6B3,_tmp6B2,_tag_fat(_tmp210,sizeof(void*),0U));});});goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1305
struct _tuple14 _tmp212=Cyc_Tcutil_addressof_props(e);struct _tuple14 _stmttmp25=_tmp212;struct _tuple14 _tmp213=_stmttmp25;void*_tmp215;int _tmp214;_LL17: _tmp214=_tmp213.f1;_tmp215=_tmp213.f2;_LL18: {int is_const=_tmp214;void*rgn=_tmp215;
# 1307
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({void*_tmp216=0U;({unsigned _tmp6B5=e->loc;struct _fat_ptr _tmp6B4=({const char*_tmp217="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp217,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp6B5,_tmp6B4,_tag_fat(_tmp216,sizeof(void*),0U));});});{
# 1310
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const){
tq.print_const=1;
tq.real_const=1;}{
# 1316
void*t=nullable?
Cyc_Absyn_star_type((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type):
 Cyc_Absyn_at_type((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);
return t;}}}}}
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
void*_tmp21B=Cyc_Tcutil_compress(*topt);void*_stmttmp26=_tmp21B;void*_tmp21C=_stmttmp26;void*_tmp21E;void*_tmp21D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->f2 != 0){_LL1: _tmp21D=_tmp21C;_tmp21E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21C)->f2)->hd;_LL2: {void*tagtyp=_tmp21D;void*tt=_tmp21E;
# 1338
struct Cyc_Absyn_Exp*_tmp21F=Cyc_Absyn_sizeoftype_exp(t,0U);struct Cyc_Absyn_Exp*e=_tmp21F;
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp220=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->tag=9U,_tmp221->f1=e;_tmp221;});struct Cyc_Absyn_ValueofType_Absyn_Type_struct*tt2=_tmp220;
if(Cyc_Unify_unify(tt,(void*)tt2))return tagtyp;
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
struct Cyc_List_List*_tmp222=fs;struct Cyc_List_List*l=_tmp222;
void*_tmp223=t_orig;void*t=_tmp223;
for(0;l != 0;l=l->tl){
void*_tmp224=(void*)l->hd;void*n=_tmp224;
void*_tmp225=n;unsigned _tmp226;struct _fat_ptr*_tmp227;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp225)->tag == 0U){_LL1: _tmp227=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp225)->f1;_LL2: {struct _fat_ptr*n=_tmp227;
# 1364
int bad_type=1;
{void*_tmp228=Cyc_Tcutil_compress(t);void*_stmttmp27=_tmp228;void*_tmp229=_stmttmp27;struct Cyc_List_List*_tmp22A;struct Cyc_Absyn_Aggrdecl**_tmp22B;switch(*((int*)_tmp229)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp229)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp229)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp22B=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp229)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp22B;
# 1367
if((*adp)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp22E=({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0U,_tmp511.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp511;});void*_tmp22C[1U];_tmp22C[0]=& _tmp22E;({unsigned _tmp6C0=loc;struct _fat_ptr _tmp6BF=({const char*_tmp22D="no field of struct/union has name %s";_tag_fat(_tmp22D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6C0,_tmp6BF,_tag_fat(_tmp22C,sizeof(void*),1U));});});else{
# 1372
t=t2;}
bad_type=0;
goto _LL5;}}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp22A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp229)->f2;_LL9: {struct Cyc_List_List*fields=_tmp22A;
# 1376
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp231=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0U,_tmp512.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp512;});void*_tmp22F[1U];_tmp22F[0]=& _tmp231;({unsigned _tmp6C2=loc;struct _fat_ptr _tmp6C1=({const char*_tmp230="no field of struct/union has name %s";_tag_fat(_tmp230,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6C2,_tmp6C1,_tag_fat(_tmp22F,sizeof(void*),1U));});});else{
# 1380
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1385
if(bad_type){
if(l == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp234=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,({struct _fat_ptr _tmp6C3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp513.f1=_tmp6C3;});_tmp513;});void*_tmp232[1U];_tmp232[0]=& _tmp234;({unsigned _tmp6C5=loc;struct _fat_ptr _tmp6C4=({const char*_tmp233="%s is not a known struct/union type";_tag_fat(_tmp233,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6C5,_tmp6C4,_tag_fat(_tmp232,sizeof(void*),1U));});});else{
# 1389
struct _fat_ptr _tmp235=({struct Cyc_String_pa_PrintArg_struct _tmp248=({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0U,({struct _fat_ptr _tmp6C6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp51A.f1=_tmp6C6;});_tmp51A;});void*_tmp246[1U];_tmp246[0]=& _tmp248;({struct _fat_ptr _tmp6C7=({const char*_tmp247="(%s)";_tag_fat(_tmp247,sizeof(char),5U);});Cyc_aprintf(_tmp6C7,_tag_fat(_tmp246,sizeof(void*),1U));});});struct _fat_ptr s=_tmp235;
struct Cyc_List_List*x;
for(x=fs;x != l;x=x->tl){
void*_tmp236=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp28=_tmp236;void*_tmp237=_stmttmp28;unsigned _tmp238;struct _fat_ptr*_tmp239;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp237)->tag == 0U){_LLD: _tmp239=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp237)->f1;_LLE: {struct _fat_ptr*n=_tmp239;
# 1394
s=({struct Cyc_String_pa_PrintArg_struct _tmp23C=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,_tmp515.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp515;});struct Cyc_String_pa_PrintArg_struct _tmp23D=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,_tmp514.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp514;});void*_tmp23A[2U];_tmp23A[0]=& _tmp23C,_tmp23A[1]=& _tmp23D;({struct _fat_ptr _tmp6C8=({const char*_tmp23B="%s.%s";_tag_fat(_tmp23B,sizeof(char),6U);});Cyc_aprintf(_tmp6C8,_tag_fat(_tmp23A,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp238=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp237)->f1;_LL10: {unsigned n=_tmp238;
# 1396
s=({struct Cyc_String_pa_PrintArg_struct _tmp240=({struct Cyc_String_pa_PrintArg_struct _tmp517;_tmp517.tag=0U,_tmp517.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp517;});struct Cyc_Int_pa_PrintArg_struct _tmp241=({struct Cyc_Int_pa_PrintArg_struct _tmp516;_tmp516.tag=1U,_tmp516.f1=(unsigned long)((int)n);_tmp516;});void*_tmp23E[2U];_tmp23E[0]=& _tmp240,_tmp23E[1]=& _tmp241;({struct _fat_ptr _tmp6C9=({const char*_tmp23F="%s.%d";_tag_fat(_tmp23F,sizeof(char),6U);});Cyc_aprintf(_tmp6C9,_tag_fat(_tmp23E,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}
# 1398
({struct Cyc_String_pa_PrintArg_struct _tmp244=({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0U,_tmp519.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp519;});struct Cyc_String_pa_PrintArg_struct _tmp245=({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0U,({struct _fat_ptr _tmp6CA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp518.f1=_tmp6CA;});_tmp518;});void*_tmp242[2U];_tmp242[0]=& _tmp244,_tmp242[1]=& _tmp245;({unsigned _tmp6CC=loc;struct _fat_ptr _tmp6CB=({const char*_tmp243="%s == %s is not a struct/union type";_tag_fat(_tmp243,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6CC,_tmp6CB,_tag_fat(_tmp242,sizeof(void*),2U));});});}}
# 1401
goto _LL0;}}else{_LL3: _tmp226=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp225)->f1;_LL4: {unsigned n=_tmp226;
# 1403
int bad_type=1;
{void*_tmp249=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp249;void*_tmp24A=_stmttmp29;struct Cyc_List_List*_tmp24B;struct Cyc_List_List*_tmp24C;struct Cyc_Absyn_Datatypefield*_tmp24D;struct Cyc_Absyn_Aggrdecl**_tmp24E;switch(*((int*)_tmp24A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp24E=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp24E;
# 1406
if((*adp)->impl == 0)
goto _LL11;
_tmp24C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp24D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp24D;
# 1426
if(({unsigned _tmp6CD=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp6CD < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp259=({struct Cyc_Int_pa_PrintArg_struct _tmp51C;_tmp51C.tag=1U,({
unsigned long _tmp6CE=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp51C.f1=_tmp6CE;});_tmp51C;});struct Cyc_Int_pa_PrintArg_struct _tmp25A=({struct Cyc_Int_pa_PrintArg_struct _tmp51B;_tmp51B.tag=1U,_tmp51B.f1=(unsigned long)((int)n);_tmp51B;});void*_tmp257[2U];_tmp257[0]=& _tmp259,_tmp257[1]=& _tmp25A;({unsigned _tmp6D0=loc;struct _fat_ptr _tmp6CF=({const char*_tmp258="datatype field has too few components: %d < %d";_tag_fat(_tmp258,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6D0,_tmp6CF,_tag_fat(_tmp257,sizeof(void*),2U));});});else{
# 1430
if(n != (unsigned)0)
t=(*((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp25B=0U;({unsigned _tmp6D2=loc;struct _fat_ptr _tmp6D1=({const char*_tmp25C="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp25C,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6D2,_tmp6D1,_tag_fat(_tmp25B,sizeof(void*),0U));});});}}
# 1435
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp24C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24A)->f2;_LL15: {struct Cyc_List_List*fields=_tmp24C;
# 1410
if(({unsigned _tmp6D3=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp6D3 <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp251=({struct Cyc_Int_pa_PrintArg_struct _tmp51E;_tmp51E.tag=1U,({
unsigned long _tmp6D4=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp51E.f1=_tmp6D4;});_tmp51E;});struct Cyc_Int_pa_PrintArg_struct _tmp252=({struct Cyc_Int_pa_PrintArg_struct _tmp51D;_tmp51D.tag=1U,_tmp51D.f1=(unsigned long)((int)n);_tmp51D;});void*_tmp24F[2U];_tmp24F[0]=& _tmp251,_tmp24F[1]=& _tmp252;({unsigned _tmp6D6=loc;struct _fat_ptr _tmp6D5=({const char*_tmp250="struct/union has too few components: %d <= %d";_tag_fat(_tmp250,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6D6,_tmp6D5,_tag_fat(_tmp24F,sizeof(void*),2U));});});else{
# 1414
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp24B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24A)->f1;_LL17: {struct Cyc_List_List*l=_tmp24B;
# 1418
if(({unsigned _tmp6D7=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp6D7 <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp255=({struct Cyc_Int_pa_PrintArg_struct _tmp520;_tmp520.tag=1U,({
unsigned long _tmp6D8=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp520.f1=_tmp6D8;});_tmp520;});struct Cyc_Int_pa_PrintArg_struct _tmp256=({struct Cyc_Int_pa_PrintArg_struct _tmp51F;_tmp51F.tag=1U,_tmp51F.f1=(unsigned long)((int)n);_tmp51F;});void*_tmp253[2U];_tmp253[0]=& _tmp255,_tmp253[1]=& _tmp256;({unsigned _tmp6DA=loc;struct _fat_ptr _tmp6D9=({const char*_tmp254="tuple has too few components: %d <= %d";_tag_fat(_tmp254,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6DA,_tmp6D9,_tag_fat(_tmp253,sizeof(void*),2U));});});else{
# 1422
t=(*((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1437
 goto _LL11;}_LL11:;}
# 1439
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp25F=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0U,({struct _fat_ptr _tmp6DB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp521.f1=_tmp6DB;});_tmp521;});void*_tmp25D[1U];_tmp25D[0]=& _tmp25F;({unsigned _tmp6DD=loc;struct _fat_ptr _tmp6DC=({const char*_tmp25E="%s is not a known type";_tag_fat(_tmp25E,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6DD,_tmp6DC,_tag_fat(_tmp25D,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1444
return Cyc_Absyn_uint_type;}}
# 1448
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp260=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp260;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp261=t;void*_tmp265;void*_tmp264;void*_tmp263;void*_tmp262;switch(*((int*)_tmp261)){case 1U: _LL1: _LL2: {
# 1454
struct Cyc_List_List*_tmp266=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp266;
void*_tmp267=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->v=tenv_tvs;_tmp26F;}));void*t2=_tmp267;
void*_tmp268=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->v=tenv_tvs;_tmp26E;}));void*rt=_tmp268;
void*_tmp269=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp269;
void*_tmp26A=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp26A;
struct Cyc_Absyn_PtrAtts _tmp26B=({struct Cyc_Absyn_PtrAtts _tmp522;_tmp522.rgn=rt,({void*_tmp6DE=Cyc_Tcutil_any_bool(tenv_tvs);_tmp522.nullable=_tmp6DE;}),_tmp522.bounds=b,_tmp522.zero_term=zt,_tmp522.ptrloc=0;_tmp522;});struct Cyc_Absyn_PtrAtts atts=_tmp26B;
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26C=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->tag=3U,(_tmp26D->f1).elt_type=t2,({struct Cyc_Absyn_Tqual _tmp6DF=Cyc_Absyn_empty_tqual(0U);(_tmp26D->f1).elt_tq=_tmp6DF;}),(_tmp26D->f1).ptr_atts=atts;_tmp26D;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_typ=_tmp26C;
Cyc_Unify_unify(t,(void*)new_typ);
_tmp262=t2;_tmp263=rt;_tmp264=b;_tmp265=zt;goto _LL4;}case 3U: _LL3: _tmp262=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).elt_type;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).ptr_atts).rgn;_tmp264=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).ptr_atts).bounds;_tmp265=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp261)->f1).ptr_atts).zero_term;_LL4: {void*t2=_tmp262;void*rt=_tmp263;void*b=_tmp264;void*zt=_tmp265;
# 1464
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp270=Cyc_Tcutil_compress(t2);void*_stmttmp2A=_tmp270;void*_tmp271=_stmttmp2A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp271)->tag == 5U){_LL8: _LL9:
# 1469
 if(Cyc_Tc_aggressive_warn)
({void*_tmp272=0U;({unsigned _tmp6E1=loc;struct _fat_ptr _tmp6E0=({const char*_tmp273="unnecessary dereference for function type";_tag_fat(_tmp273,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6E1,_tmp6E0,_tag_fat(_tmp272,sizeof(void*),0U));});});
return t;}else{_LLA: _LLB:
# 1473
({void*_tmp274=0U;({unsigned _tmp6E3=loc;struct _fat_ptr _tmp6E2=({const char*_tmp275="can't dereference abstract pointer type";_tag_fat(_tmp275,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6E3,_tmp6E2,_tag_fat(_tmp274,sizeof(void*),0U));});});}_LL7:;}
# 1476
return t2;}default: _LL5: _LL6:
# 1478
 return({struct Cyc_String_pa_PrintArg_struct _tmp278=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0U,({struct _fat_ptr _tmp6E4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp523.f1=_tmp6E4;});_tmp523;});void*_tmp276[1U];_tmp276[0]=& _tmp278;({struct Cyc_Tcenv_Tenv*_tmp6E8=te;unsigned _tmp6E7=loc;void**_tmp6E6=topt;struct _fat_ptr _tmp6E5=({const char*_tmp277="expecting *, @, or ? type but found %s";_tag_fat(_tmp277,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp6E8,_tmp6E7,_tmp6E6,_tmp6E5,_tag_fat(_tmp276,sizeof(void*),1U));});});}_LL0:;}}
# 1483
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1487
({struct Cyc_Tcenv_Tenv*_tmp6E9=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6E9,0,e);});
# 1489
({int _tmp6EA=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6EA;});{
void*_tmp279=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp2B=_tmp279;void*_tmp27A=_stmttmp2B;struct Cyc_List_List*_tmp27C;enum Cyc_Absyn_AggrKind _tmp27B;struct Cyc_List_List*_tmp27E;struct Cyc_Absyn_Aggrdecl*_tmp27D;switch(*((int*)_tmp27A)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp27D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1)->f1).KnownAggr).val;_tmp27E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp27D;struct Cyc_List_List*ts=_tmp27E;
# 1492
struct Cyc_Absyn_Aggrfield*_tmp27F=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp27F;
if(finfo == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0U,({
struct _fat_ptr _tmp6EB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp525.f1=_tmp6EB;});_tmp525;});struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,_tmp524.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp524;});void*_tmp280[2U];_tmp280[0]=& _tmp282,_tmp280[1]=& _tmp283;({struct Cyc_Tcenv_Tenv*_tmp6EF=te;unsigned _tmp6EE=loc;void**_tmp6ED=topt;struct _fat_ptr _tmp6EC=({const char*_tmp281="%s has no %s member";_tag_fat(_tmp281,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp6EF,_tmp6EE,_tmp6ED,_tmp6EC,_tag_fat(_tmp280,sizeof(void*),2U));});});
# 1497
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0){
struct Cyc_List_List*_tmp284=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);struct Cyc_List_List*inst=_tmp284;
t2=Cyc_Tcutil_substitute(inst,finfo->type);}
# 1505
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp287=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,_tmp526.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp526;});void*_tmp285[1U];_tmp285[0]=& _tmp287;({struct Cyc_Tcenv_Tenv*_tmp6F3=te;unsigned _tmp6F2=loc;void**_tmp6F1=topt;struct _fat_ptr _tmp6F0=({const char*_tmp286="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp286,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6F3,_tmp6F2,_tmp6F1,_tmp6F0,_tag_fat(_tmp285,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1510
if(!({void*_tmp6F4=t2;Cyc_Unify_unify(_tmp6F4,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,_tmp527.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp527;});void*_tmp288[1U];_tmp288[0]=& _tmp28A;({struct Cyc_Tcenv_Tenv*_tmp6F8=te;unsigned _tmp6F7=loc;void**_tmp6F6=topt;struct _fat_ptr _tmp6F5=({const char*_tmp289="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_fat(_tmp289,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp6F8,_tmp6F7,_tmp6F6,_tmp6F5,_tag_fat(_tmp288,sizeof(void*),1U));});});}
# 1513
return t2;}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp27B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27A)->f1;_tmp27C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27A)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp27B;struct Cyc_List_List*fs=_tmp27C;
# 1515
struct Cyc_Absyn_Aggrfield*_tmp28B=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp28B;
if(finfo == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp28E=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0U,_tmp528.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp528;});void*_tmp28C[1U];_tmp28C[0]=& _tmp28E;({struct Cyc_Tcenv_Tenv*_tmp6FC=te;unsigned _tmp6FB=loc;void**_tmp6FA=topt;struct _fat_ptr _tmp6F9=({const char*_tmp28D="type has no %s member";_tag_fat(_tmp28D,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp6FC,_tmp6FB,_tmp6FA,_tmp6F9,_tag_fat(_tmp28C,sizeof(void*),1U));});});
# 1520
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1522
return({struct Cyc_String_pa_PrintArg_struct _tmp291=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,_tmp529.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp529;});void*_tmp28F[1U];_tmp28F[0]=& _tmp291;({struct Cyc_Tcenv_Tenv*_tmp700=te;unsigned _tmp6FF=loc;void**_tmp6FE=topt;struct _fat_ptr _tmp6FD=({const char*_tmp290="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp290,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp700,_tmp6FF,_tmp6FE,_tmp6FD,_tag_fat(_tmp28F,sizeof(void*),1U));});});
return finfo->type;}default: _LL5: _LL6:
# 1525
 return({struct Cyc_String_pa_PrintArg_struct _tmp294=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0U,({
struct _fat_ptr _tmp701=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp52A.f1=_tmp701;});_tmp52A;});void*_tmp292[1U];_tmp292[0]=& _tmp294;({struct Cyc_Tcenv_Tenv*_tmp705=te;unsigned _tmp704=loc;void**_tmp703=topt;struct _fat_ptr _tmp702=({const char*_tmp293="expecting struct or union, found %s";_tag_fat(_tmp293,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp705,_tmp704,_tmp703,_tmp702,_tag_fat(_tmp292,sizeof(void*),1U));});});}_LL0:;}}
# 1531
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1534
({struct Cyc_Tcenv_Tenv*_tmp706=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp706,0,e);});
# 1536
({int _tmp707=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp707;});
{void*_tmp295=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp2C=_tmp295;void*_tmp296=_stmttmp2C;void*_tmp29A;void*_tmp299;void*_tmp298;void*_tmp297;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->tag == 3U){_LL1: _tmp297=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).elt_type;_tmp298=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).ptr_atts).rgn;_tmp299=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).ptr_atts).bounds;_tmp29A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp296)->f1).ptr_atts).zero_term;_LL2: {void*t2=_tmp297;void*rt=_tmp298;void*b=_tmp299;void*zt=_tmp29A;
# 1539
Cyc_Tcutil_check_nonzero_bound(loc,b);
{void*_tmp29B=Cyc_Tcutil_compress(t2);void*_stmttmp2D=_tmp29B;void*_tmp29C=_stmttmp2D;struct Cyc_List_List*_tmp29E;enum Cyc_Absyn_AggrKind _tmp29D;struct Cyc_List_List*_tmp2A0;struct Cyc_Absyn_Aggrdecl*_tmp29F;switch(*((int*)_tmp29C)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp29F=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C)->f1)->f1).KnownAggr).val;_tmp2A0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp29F;struct Cyc_List_List*ts=_tmp2A0;
# 1542
struct Cyc_Absyn_Aggrfield*_tmp2A1=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2A1;
if(finfo == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A4=({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0U,({
struct _fat_ptr _tmp708=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name));_tmp52C.f1=_tmp708;});_tmp52C;});struct Cyc_String_pa_PrintArg_struct _tmp2A5=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0U,_tmp52B.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52B;});void*_tmp2A2[2U];_tmp2A2[0]=& _tmp2A4,_tmp2A2[1]=& _tmp2A5;({struct Cyc_Tcenv_Tenv*_tmp70C=te;unsigned _tmp70B=loc;void**_tmp70A=topt;struct _fat_ptr _tmp709=({const char*_tmp2A3="type %s has no %s member";_tag_fat(_tmp2A3,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp70C,_tmp70B,_tmp70A,_tmp709,_tag_fat(_tmp2A2,sizeof(void*),2U));});});
# 1547
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t3=finfo->type;
if(ts != 0){
struct Cyc_List_List*_tmp2A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);struct Cyc_List_List*inst=_tmp2A6;
t3=Cyc_Tcutil_substitute(inst,finfo->type);}{
# 1553
struct Cyc_Absyn_Kind*_tmp2A7=Cyc_Tcutil_type_kind(t3);struct Cyc_Absyn_Kind*t3_kind=_tmp2A7;
# 1556
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t3_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2A8=Cyc_Tcutil_compress(t3);void*_stmttmp2E=_tmp2A8;void*_tmp2A9=_stmttmp2E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A9)->tag == 4U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1560
 return({struct Cyc_String_pa_PrintArg_struct _tmp2AC=({struct Cyc_String_pa_PrintArg_struct _tmp52D;_tmp52D.tag=0U,_tmp52D.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52D;});void*_tmp2AA[1U];_tmp2AA[0]=& _tmp2AC;({struct Cyc_Tcenv_Tenv*_tmp710=te;unsigned _tmp70F=loc;void**_tmp70E=topt;struct _fat_ptr _tmp70D=({const char*_tmp2AB="cannot get member %s since its type is abstract";_tag_fat(_tmp2AB,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp710,_tmp70F,_tmp70E,_tmp70D,_tag_fat(_tmp2AA,sizeof(void*),1U));});});}_LLC:;}
# 1565
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !
Cyc_Tcutil_is_bits_only_type(t3))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1568
return({struct Cyc_String_pa_PrintArg_struct _tmp2AF=({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0U,_tmp52E.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52E;});void*_tmp2AD[1U];_tmp2AD[0]=& _tmp2AF;({struct Cyc_Tcenv_Tenv*_tmp714=te;unsigned _tmp713=loc;void**_tmp712=topt;struct _fat_ptr _tmp711=({const char*_tmp2AE="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp2AE,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp714,_tmp713,_tmp712,_tmp711,_tag_fat(_tmp2AD,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
if(!({void*_tmp715=t3;Cyc_Unify_unify(_tmp715,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B2=({struct Cyc_String_pa_PrintArg_struct _tmp52F;_tmp52F.tag=0U,_tmp52F.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp52F;});void*_tmp2B0[1U];_tmp2B0[0]=& _tmp2B2;({struct Cyc_Tcenv_Tenv*_tmp719=te;unsigned _tmp718=loc;void**_tmp717=topt;struct _fat_ptr _tmp716=({const char*_tmp2B1="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_fat(_tmp2B1,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp719,_tmp718,_tmp717,_tmp716,_tag_fat(_tmp2B0,sizeof(void*),1U));});});}
# 1575
return t3;}}}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp29D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29C)->f1;_tmp29E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29C)->f2;_LL9: {enum Cyc_Absyn_AggrKind k=_tmp29D;struct Cyc_List_List*fs=_tmp29E;
# 1577
struct Cyc_Absyn_Aggrfield*_tmp2B3=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2B3;
if(finfo == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2B6=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,_tmp530.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp530;});void*_tmp2B4[1U];_tmp2B4[0]=& _tmp2B6;({struct Cyc_Tcenv_Tenv*_tmp71D=te;unsigned _tmp71C=loc;void**_tmp71B=topt;struct _fat_ptr _tmp71A=({const char*_tmp2B5="type has no %s field";_tag_fat(_tmp2B5,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp71D,_tmp71C,_tmp71B,_tmp71A,_tag_fat(_tmp2B4,sizeof(void*),1U));});});
# 1582
if(((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B9=({struct Cyc_String_pa_PrintArg_struct _tmp531;_tmp531.tag=0U,_tmp531.f1=(struct _fat_ptr)((struct _fat_ptr)*f);_tmp531;});void*_tmp2B7[1U];_tmp2B7[0]=& _tmp2B9;({struct Cyc_Tcenv_Tenv*_tmp721=te;unsigned _tmp720=loc;void**_tmp71F=topt;struct _fat_ptr _tmp71E=({const char*_tmp2B8="cannot read union member %s since it is not `bits-only'";_tag_fat(_tmp2B8,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp721,_tmp720,_tmp71F,_tmp71E,_tag_fat(_tmp2B7,sizeof(void*),1U));});});
return finfo->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1587
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1590
return({struct Cyc_String_pa_PrintArg_struct _tmp2BC=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,({
struct _fat_ptr _tmp722=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp532.f1=_tmp722;});_tmp532;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BC;({struct Cyc_Tcenv_Tenv*_tmp726=te;unsigned _tmp725=loc;void**_tmp724=topt;struct _fat_ptr _tmp723=({const char*_tmp2BB="expecting struct or union pointer, found %s";_tag_fat(_tmp2BB,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp726,_tmp725,_tmp724,_tmp723,_tag_fat(_tmp2BA,sizeof(void*),1U));});});}
# 1595
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1597
struct _tuple15 _tmp2BD=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _stmttmp2F=_tmp2BD;struct _tuple15 _tmp2BE=_stmttmp2F;int _tmp2C0;unsigned _tmp2BF;_LL1: _tmp2BF=_tmp2BE.f1;_tmp2C0=_tmp2BE.f2;_LL2: {unsigned i=_tmp2BF;int known=_tmp2C0;
if(!known)
return({void*_tmp2C1=0U;({struct Cyc_Tcenv_Tenv*_tmp729=te;unsigned _tmp728=loc;struct _fat_ptr _tmp727=({const char*_tmp2C2="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp2C2,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp729,_tmp728,0,_tmp727,_tag_fat(_tmp2C1,sizeof(void*),0U));});});{
# 1601
struct _handler_cons _tmp2C3;_push_handler(& _tmp2C3);{int _tmp2C5=0;if(setjmp(_tmp2C3.handler))_tmp2C5=1;if(!_tmp2C5){
{void*_tmp2C6=(*((struct _tuple16*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp2C6;};_pop_handler();}else{void*_tmp2C4=(void*)Cyc_Core_get_exn_thrown();void*_tmp2C7=_tmp2C4;void*_tmp2C8;if(((struct Cyc_List_Nth_exn_struct*)_tmp2C7)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1604
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2CB=({struct Cyc_Int_pa_PrintArg_struct _tmp534;_tmp534.tag=1U,_tmp534.f1=(unsigned long)((int)i);_tmp534;});struct Cyc_Int_pa_PrintArg_struct _tmp2CC=({struct Cyc_Int_pa_PrintArg_struct _tmp533;_tmp533.tag=1U,({
unsigned long _tmp72A=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp533.f1=_tmp72A;});_tmp533;});void*_tmp2C9[2U];_tmp2C9[0]=& _tmp2CB,_tmp2C9[1]=& _tmp2CC;({struct Cyc_Tcenv_Tenv*_tmp72D=te;unsigned _tmp72C=loc;struct _fat_ptr _tmp72B=({const char*_tmp2CA="index is %d but tuple has only %d fields";_tag_fat(_tmp2CA,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp72D,_tmp72C,0,_tmp72B,_tag_fat(_tmp2C9,sizeof(void*),2U));});});}else{_LL6: _tmp2C8=_tmp2C7;_LL7: {void*exn=_tmp2C8;(int)_rethrow(exn);}}_LL3:;}}}}}
# 1609
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1611
struct Cyc_Tcenv_Tenv*_tmp2CD=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2CD;
({struct Cyc_Tcenv_Tenv*_tmp72E=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp72E,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp72F=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp72F,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D0=({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U,({
struct _fat_ptr _tmp730=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp535.f1=_tmp730;});_tmp535;});void*_tmp2CE[1U];_tmp2CE[0]=& _tmp2D0;({struct Cyc_Tcenv_Tenv*_tmp734=te;unsigned _tmp733=e2->loc;void**_tmp732=topt;struct _fat_ptr _tmp731=({const char*_tmp2CF="expecting int subscript, found %s";_tag_fat(_tmp2CF,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp734,_tmp733,_tmp732,_tmp731,_tag_fat(_tmp2CE,sizeof(void*),1U));});});{
# 1621
void*_tmp2D1=t1;struct Cyc_List_List*_tmp2D2;void*_tmp2D7;void*_tmp2D6;void*_tmp2D5;struct Cyc_Absyn_Tqual _tmp2D4;void*_tmp2D3;switch(*((int*)_tmp2D1)){case 3U: _LL1: _tmp2D3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).elt_type;_tmp2D4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).elt_tq;_tmp2D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).rgn;_tmp2D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).bounds;_tmp2D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp2D3;struct Cyc_Absyn_Tqual tq=_tmp2D4;void*rt=_tmp2D5;void*b=_tmp2D6;void*zt=_tmp2D7;
# 1625
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp2D8=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp2D8;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp2D9=eopt;struct Cyc_Absyn_Exp*e3=_tmp2D9;
struct _tuple15 _tmp2DA=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple15 _stmttmp30=_tmp2DA;struct _tuple15 _tmp2DB=_stmttmp30;int _tmp2DD;unsigned _tmp2DC;_LL8: _tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;_LL9: {unsigned i=_tmp2DC;int known_i=_tmp2DD;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2DE=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _stmttmp31=_tmp2DE;struct _tuple15 _tmp2DF=_stmttmp31;int _tmp2E1;unsigned _tmp2E0;_LLB: _tmp2E0=_tmp2DF.f1;_tmp2E1=_tmp2DF.f2;_LLC: {unsigned j=_tmp2E0;int known_j=_tmp2E1;
if(known_j){
struct _tuple15 _tmp2E2=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple15 _stmttmp32=_tmp2E2;struct _tuple15 _tmp2E3=_stmttmp32;int _tmp2E5;unsigned _tmp2E4;_LLE: _tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;_LLF: {unsigned j=_tmp2E4;int knownj=_tmp2E5;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp2E6=0U;({unsigned _tmp736=loc;struct _fat_ptr _tmp735=({const char*_tmp2E7="subscript will fail at run-time";_tag_fat(_tmp2E7,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp736,_tmp735,_tag_fat(_tmp2E6,sizeof(void*),0U));});});}}}}}}
# 1641
if(emit_warning)
({void*_tmp2E8=0U;({unsigned _tmp738=e2->loc;struct _fat_ptr _tmp737=({const char*_tmp2E9="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp2E9,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp738,_tmp737,_tag_fat(_tmp2E8,sizeof(void*),0U));});});}else{
# 1645
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2EA=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _stmttmp33=_tmp2EA;struct _tuple15 _tmp2EB=_stmttmp33;int _tmp2ED;unsigned _tmp2EC;_LL11: _tmp2EC=_tmp2EB.f1;_tmp2ED=_tmp2EB.f2;_LL12: {unsigned i=_tmp2EC;int known=_tmp2ED;
if(known)
# 1650
({unsigned _tmp73B=loc;unsigned _tmp73A=i;void*_tmp739=b;Cyc_Tcutil_check_bound(_tmp73B,_tmp73A,_tmp739,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1653
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp2EE=0U;({unsigned _tmp73D=e1->loc;struct _fat_ptr _tmp73C=({const char*_tmp2EF="subscript applied to pointer to one object";_tag_fat(_tmp2EF,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp73D,_tmp73C,_tag_fat(_tmp2EE,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1658
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp2F0=0U;({unsigned _tmp73F=e1->loc;struct _fat_ptr _tmp73E=({const char*_tmp2F1="can't subscript an abstract pointer";_tag_fat(_tmp2F1,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp73F,_tmp73E,_tag_fat(_tmp2F0,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp2D2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D1)->f1;_LL4: {struct Cyc_List_List*ts=_tmp2D2;
return Cyc_Tcexp_ithTupleType(te,loc,ts,e2);}default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp2F4=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,({struct _fat_ptr _tmp740=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp536.f1=_tmp740;});_tmp536;});void*_tmp2F2[1U];_tmp2F2[0]=& _tmp2F4;({struct Cyc_Tcenv_Tenv*_tmp744=te;unsigned _tmp743=loc;void**_tmp742=topt;struct _fat_ptr _tmp741=({const char*_tmp2F3="subscript applied to %s";_tag_fat(_tmp2F3,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp744,_tmp743,_tmp742,_tmp741,_tag_fat(_tmp2F2,sizeof(void*),1U));});});}_LL0:;}}}
# 1668
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp2F5=Cyc_Tcutil_compress(*topt);void*_stmttmp34=_tmp2F5;void*_tmp2F6=_stmttmp34;struct Cyc_List_List*_tmp2F7;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F6)->tag == 6U){_LL1: _tmp2F7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F6)->f1;_LL2: {struct Cyc_List_List*ts=_tmp2F7;
# 1674
if(({int _tmp745=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp745 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1677
goto _LL0;
# 1679
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp2F8=(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp2F8;
({struct Cyc_Tcenv_Tenv*_tmp746=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp746,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1684
({struct Cyc_RgnOrder_RgnPO*_tmp748=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp747=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp748,_tmp747,(*((struct _tuple16*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));({struct _tuple16*_tmp749=({struct _tuple16*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->f1=(*((struct _tuple16*)ts->hd)).f1,_tmp2F9->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2F9;});_tmp2FA->hd=_tmp749;}),_tmp2FA->tl=fields;_tmp2FA;});}
# 1687
done=1;
goto _LL0;}}else{_LL3: _LL4:
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
struct _tuple9*_tmp2FE=targ;void*_tmp2FF;_LL1: _tmp2FF=_tmp2FE->f3;_LL2: {void*t=_tmp2FF;
({unsigned _tmp751=loc;struct Cyc_Tcenv_Tenv*_tmp750=te;struct Cyc_List_List*_tmp74F=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp74E=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1706
Cyc_Tctyp_check_type(_tmp751,_tmp750,_tmp74F,_tmp74E,1,0,t);});
# 1708
({void*_tmp752=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->tag=36U,_tmp300->f1=0,_tmp300->f2=des;_tmp300;});orig_exp->r=_tmp752;});
Cyc_Tcexp_resolve_unresolved_mem(te,loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1721 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1724
void*res_t2;
int _tmp301=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp301;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp302=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->tag=0U,({union Cyc_Absyn_Cnst _tmp753=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp318->f1=_tmp753;});_tmp318;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp302;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1732
if(zero_term){
struct Cyc_Absyn_Exp*_tmp303=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp303;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp304=0U;({unsigned _tmp755=e->loc;struct _fat_ptr _tmp754=({const char*_tmp305="zero-terminated array doesn't end with zero.";_tag_fat(_tmp305,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp755,_tmp754,_tag_fat(_tmp304,sizeof(void*),0U));});});}
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
void*_tmp309=(void*)ds->hd;void*_stmttmp35=_tmp309;void*_tmp30A=_stmttmp35;struct Cyc_Absyn_Exp*_tmp30B;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp30A)->tag == 1U){_LL1: _LL2:
# 1757
({void*_tmp30C=0U;({unsigned _tmp75F=loc;struct _fat_ptr _tmp75E=({const char*_tmp30D="only array index designators are supported";_tag_fat(_tmp30D,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp75F,_tmp75E,_tag_fat(_tmp30C,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp30B=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp30A)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp30B;
# 1760
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple15 _tmp30E=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple15 _stmttmp36=_tmp30E;struct _tuple15 _tmp30F=_stmttmp36;int _tmp311;unsigned _tmp310;_LL6: _tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;_LL7: {unsigned i=_tmp310;int known=_tmp311;
if(!known)
({void*_tmp312=0U;({unsigned _tmp761=e->loc;struct _fat_ptr _tmp760=({const char*_tmp313="index designator cannot use sizeof or offsetof";_tag_fat(_tmp313,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp761,_tmp760,_tag_fat(_tmp312,sizeof(void*),0U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp316=({struct Cyc_Int_pa_PrintArg_struct _tmp539;_tmp539.tag=1U,_tmp539.f1=(unsigned long)offset;_tmp539;});struct Cyc_Int_pa_PrintArg_struct _tmp317=({struct Cyc_Int_pa_PrintArg_struct _tmp538;_tmp538.tag=1U,_tmp538.f1=(unsigned long)((int)i);_tmp538;});void*_tmp314[2U];_tmp314[0]=& _tmp316,_tmp314[1]=& _tmp317;({unsigned _tmp763=e->loc;struct _fat_ptr _tmp762=({const char*_tmp315="expecting index designator %d but found %d";_tag_fat(_tmp315,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp763,_tmp762,_tag_fat(_tmp314,sizeof(void*),2U));});});}
# 1767
goto _LL0;}}}}_LL0:;}}}
# 1771
return res_t2;}
# 1775
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1778
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp319=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp319;void*_tmp31A=_stmttmp37;void*_tmp31B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->f2 != 0){_LL1: _tmp31B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31A)->f2)->hd;_LL2: {void*t=_tmp31B;
# 1783
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp31C=({void*_tmp764=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp764,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp31C;
b->topt=bound->topt;
# 1788
bound=b;}
# 1790
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1792
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U,({
struct _fat_ptr _tmp765=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp53A.f1=_tmp765;});_tmp53A;});void*_tmp31D[1U];_tmp31D[0]=& _tmp31F;({unsigned _tmp767=bound->loc;struct _fat_ptr _tmp766=({const char*_tmp31E="expecting unsigned int, found %s";_tag_fat(_tmp31E,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp767,_tmp766,_tag_fat(_tmp31D,sizeof(void*),1U));});});}_LL0:;}
# 1797
if(!(vd->tq).real_const)
({void*_tmp320=0U;({struct _fat_ptr _tmp768=({const char*_tmp321="comprehension index variable is not declared const!";_tag_fat(_tmp321,sizeof(char),52U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp768,_tag_fat(_tmp320,sizeof(void*),0U));});});
# 1800
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
void**_tmp322=0;void**topt2=_tmp322;
struct Cyc_Absyn_Tqual*_tmp323=0;struct Cyc_Absyn_Tqual*tqopt=_tmp323;
void**_tmp324=0;void**ztopt=_tmp324;
# 1806
if(topt != 0){
void*_tmp325=Cyc_Tcutil_compress(*topt);void*_stmttmp38=_tmp325;void*_tmp326=_stmttmp38;void*_tmp32A;struct Cyc_Absyn_Exp*_tmp329;struct Cyc_Absyn_Tqual _tmp328;void*_tmp327;struct Cyc_Absyn_PtrInfo _tmp32B;switch(*((int*)_tmp326)){case 3U: _LL6: _tmp32B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp32B;
# 1809
topt2=({void**_tmp32C=_cycalloc(sizeof(*_tmp32C));*_tmp32C=x.elt_type;_tmp32C;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp32D=_cycalloc(sizeof(*_tmp32D));*_tmp32D=x.elt_tq;_tmp32D;});
ztopt=({void**_tmp32E=_cycalloc(sizeof(*_tmp32E));*_tmp32E=(x.ptr_atts).zero_term;_tmp32E;});
goto _LL5;}case 4U: _LL8: _tmp327=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).elt_type;_tmp328=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).tq;_tmp329=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).num_elts;_tmp32A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).zero_term;_LL9: {void*t=_tmp327;struct Cyc_Absyn_Tqual tq=_tmp328;struct Cyc_Absyn_Exp*b=_tmp329;void*zt=_tmp32A;
# 1814
topt2=({void**_tmp32F=_cycalloc(sizeof(*_tmp32F));*_tmp32F=t;_tmp32F;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp330=_cycalloc(sizeof(*_tmp330));*_tmp330=tq;_tmp330;});
ztopt=({void**_tmp331=_cycalloc(sizeof(*_tmp331));*_tmp331=zt;_tmp331;});
goto _LL5;}default: _LLA: _LLB:
# 1819
 goto _LL5;}_LL5:;}{
# 1822
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
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
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1836
struct Cyc_Absyn_Exp*_tmp338=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp338;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1840
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt))&& !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp339=0U;({unsigned _tmp770=body->loc;struct _fat_ptr _tmp76F=({const char*_tmp33A="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp33A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp770,_tmp76F,_tag_fat(_tmp339,sizeof(void*),0U));});});{
# 1844
void*_tmp33B=({void*_tmp773=t;struct Cyc_Absyn_Tqual _tmp772=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp771=bound;Cyc_Absyn_array_type(_tmp773,_tmp772,_tmp771,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1844
void*res=_tmp33B;
# 1846
return res;}}}}
# 1850
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1853
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp33C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp39=_tmp33C;void*_tmp33D=_stmttmp39;void*_tmp33E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->f2 != 0){_LL1: _tmp33E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33D)->f2)->hd;_LL2: {void*t=_tmp33E;
# 1858
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp33F=({void*_tmp774=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp774,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp33F;
b->topt=bound->topt;
# 1863
bound=b;}
# 1865
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1867
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_String_pa_PrintArg_struct _tmp342=({struct Cyc_String_pa_PrintArg_struct _tmp53B;_tmp53B.tag=0U,({
struct _fat_ptr _tmp775=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp53B.f1=_tmp775;});_tmp53B;});void*_tmp340[1U];_tmp340[0]=& _tmp342;({unsigned _tmp777=bound->loc;struct _fat_ptr _tmp776=({const char*_tmp341="expecting unsigned int, found %s";_tag_fat(_tmp341,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp777,_tmp776,_tag_fat(_tmp340,sizeof(void*),1U));});});}_LL0:;}{
# 1872
void**_tmp343=0;void**topt2=_tmp343;
struct Cyc_Absyn_Tqual*_tmp344=0;struct Cyc_Absyn_Tqual*tqopt=_tmp344;
void**_tmp345=0;void**ztopt=_tmp345;
# 1876
if(topt != 0){
void*_tmp346=Cyc_Tcutil_compress(*topt);void*_stmttmp3A=_tmp346;void*_tmp347=_stmttmp3A;void*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Tqual _tmp349;void*_tmp348;struct Cyc_Absyn_PtrInfo _tmp34C;switch(*((int*)_tmp347)){case 3U: _LL6: _tmp34C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp347)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp34C;
# 1879
topt2=({void**_tmp34D=_cycalloc(sizeof(*_tmp34D));*_tmp34D=x.elt_type;_tmp34D;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp34E=_cycalloc(sizeof(*_tmp34E));*_tmp34E=x.elt_tq;_tmp34E;});
ztopt=({void**_tmp34F=_cycalloc(sizeof(*_tmp34F));*_tmp34F=(x.ptr_atts).zero_term;_tmp34F;});
goto _LL5;}case 4U: _LL8: _tmp348=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).elt_type;_tmp349=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).tq;_tmp34A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).num_elts;_tmp34B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).zero_term;_LL9: {void*t=_tmp348;struct Cyc_Absyn_Tqual tq=_tmp349;struct Cyc_Absyn_Exp*b=_tmp34A;void*zt=_tmp34B;
# 1884
topt2=({void**_tmp350=_cycalloc(sizeof(*_tmp350));*_tmp350=t;_tmp350;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp351=_cycalloc(sizeof(*_tmp351));*_tmp351=tq;_tmp351;});
ztopt=({void**_tmp352=_cycalloc(sizeof(*_tmp352));*_tmp352=zt;_tmp352;});
goto _LL5;}default: _LLA: _LLB:
# 1889
 goto _LL5;}_LL5:;}
# 1892
({unsigned _tmp77A=loc;struct Cyc_Tcenv_Tenv*_tmp779=te;struct Cyc_List_List*_tmp778=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp77A,_tmp779,_tmp778,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)Cyc_Unify_unify(*topt2,t);
# 1895
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp353=0U;({unsigned _tmp77C=bound->loc;struct _fat_ptr _tmp77B=({const char*_tmp354="bound is not constant";_tag_fat(_tmp354,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp77C,_tmp77B,_tag_fat(_tmp353,sizeof(void*),0U));});});}
# 1899
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1902
struct Cyc_Absyn_Exp*_tmp355=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp355;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1906
({void*_tmp356=0U;({unsigned _tmp77E=loc;struct _fat_ptr _tmp77D=({const char*_tmp357="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp357,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp77E,_tmp77D,_tag_fat(_tmp356,sizeof(void*),0U));});});}{
# 1909
void*_tmp358=({void*_tmp781=t;struct Cyc_Absyn_Tqual _tmp780=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp77F=bound;Cyc_Absyn_array_type(_tmp781,_tmp780,_tmp77F,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1909
void*res=_tmp358;
# 1911
return res;}}}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
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
;_pop_handler();}else{void*_tmp35B=(void*)Cyc_Core_get_exn_thrown();void*_tmp35D=_tmp35B;void*_tmp35E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp35D)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1937
({struct Cyc_String_pa_PrintArg_struct _tmp361=({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0U,({struct _fat_ptr _tmp782=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp53C.f1=_tmp782;});_tmp53C;});void*_tmp35F[1U];_tmp35F[0]=& _tmp361;({unsigned _tmp784=loc;struct _fat_ptr _tmp783=({const char*_tmp360="unbound struct/union name %s";_tag_fat(_tmp360,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp784,_tmp783,_tag_fat(_tmp35F,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp35E=_tmp35D;_LL4: {void*exn=_tmp35E;(int)_rethrow(exn);}}_LL0:;}}}
# 1940
*ad_opt=ad;
*tn=ad->name;}
# 1943
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,({struct _fat_ptr _tmp785=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp365="struct";_tag_fat(_tmp365,sizeof(char),7U);}):({const char*_tmp366="union";_tag_fat(_tmp366,sizeof(char),6U);}));_tmp53D.f1=_tmp785;});_tmp53D;});void*_tmp362[1U];_tmp362[0]=& _tmp364;({unsigned _tmp787=loc;struct _fat_ptr _tmp786=({const char*_tmp363="can't construct abstract %s";_tag_fat(_tmp363,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp787,_tmp786,_tag_fat(_tmp362,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1951
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp367=Cyc_Tcenv_new_status(te);enum Cyc_Tcenv_NewStatus status=_tmp367;
if((int)status == (int)1U)
te2=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);else{
# 1960
te2=Cyc_Tcenv_set_new_status(status,te);}{
# 1962
struct _tuple12 _tmp368=({struct _tuple12 _tmp543;({struct Cyc_List_List*_tmp788=Cyc_Tcenv_lookup_type_vars(te2);_tmp543.f1=_tmp788;}),_tmp543.f2=Cyc_Core_heap_region;_tmp543;});struct _tuple12 env=_tmp368;
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp369;
struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp36A;
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp36B;
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp36C;
struct Cyc_List_List*_tmp36D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp36D;
void*res_typ;
# 1973
if(topt != 0){
void*_tmp36E=Cyc_Tcutil_compress(*topt);void*_stmttmp3B=_tmp36E;void*_tmp36F=_stmttmp3B;struct Cyc_List_List*_tmp371;struct Cyc_Absyn_Aggrdecl**_tmp370;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp370=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f1)->f1).KnownAggr).val;_tmp371=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36F)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp370;struct Cyc_List_List*all_typs2=_tmp371;
# 1976
if(*adptr2 == *adptr){
{struct Cyc_List_List*_tmp372=all_typs;struct Cyc_List_List*ats=_tmp372;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1981
res_typ=*topt;
goto _LL5;}
# 1984
goto _LL9;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1986
 res_typ=({union Cyc_Absyn_AggrInfo _tmp789=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp789,all_typs);});}_LL5:;}else{
# 1989
res_typ=({union Cyc_Absyn_AggrInfo _tmp78A=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp78A,all_typs);});}{
# 1992
struct Cyc_List_List*_tmp373=*ts;struct Cyc_List_List*user_ex_ts=_tmp373;
struct Cyc_List_List*_tmp374=exist_typs;struct Cyc_List_List*ex_ts=_tmp374;
while(user_ex_ts != 0 && ex_ts != 0){
# 1996
({unsigned _tmp78D=loc;struct Cyc_Tcenv_Tenv*_tmp78C=te2;struct Cyc_List_List*_tmp78B=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp78D,_tmp78C,_tmp78B,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 2002
if(user_ex_ts != 0)
({void*_tmp375=0U;({unsigned _tmp78F=loc;struct _fat_ptr _tmp78E=({const char*_tmp376="too many explicit witness types";_tag_fat(_tmp376,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp78F,_tmp78E,_tag_fat(_tmp375,sizeof(void*),0U));});});
# 2005
*ts=exist_typs;{
# 2008
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple18*_tmp377=(struct _tuple18*)fields->hd;struct _tuple18*_stmttmp3C=_tmp377;struct _tuple18*_tmp378=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp37A;struct Cyc_Absyn_Aggrfield*_tmp379;_LLB: _tmp379=_tmp378->f1;_tmp37A=_tmp378->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp379;struct Cyc_Absyn_Exp*field_exp=_tmp37A;
void*_tmp37B=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp37B;
({struct Cyc_Tcenv_Tenv*_tmp790=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp790,& inst_fieldtyp,field_exp);});
# 2019
if(!({struct Cyc_RgnOrder_RgnPO*_tmp792=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp791=field_exp;Cyc_Tcutil_coerce_arg(_tmp792,_tmp791,inst_fieldtyp,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,_tmp542.f1=(struct _fat_ptr)((struct _fat_ptr)*field->name);_tmp542;});struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,({
struct _fat_ptr _tmp793=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp383="struct";_tag_fat(_tmp383,sizeof(char),7U);}):({const char*_tmp384="union";_tag_fat(_tmp384,sizeof(char),6U);}));_tmp541.f1=_tmp793;});_tmp541;});struct Cyc_String_pa_PrintArg_struct _tmp380=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,({
struct _fat_ptr _tmp794=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp540.f1=_tmp794;});_tmp540;});struct Cyc_String_pa_PrintArg_struct _tmp381=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,({struct _fat_ptr _tmp795=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(inst_fieldtyp));_tmp53F.f1=_tmp795;});_tmp53F;});struct Cyc_String_pa_PrintArg_struct _tmp382=({struct Cyc_String_pa_PrintArg_struct _tmp53E;_tmp53E.tag=0U,({
struct _fat_ptr _tmp796=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(field_exp->topt)));_tmp53E.f1=_tmp796;});_tmp53E;});void*_tmp37C[5U];_tmp37C[0]=& _tmp37E,_tmp37C[1]=& _tmp37F,_tmp37C[2]=& _tmp380,_tmp37C[3]=& _tmp381,_tmp37C[4]=& _tmp382;({unsigned _tmp798=field_exp->loc;struct _fat_ptr _tmp797=({const char*_tmp37D="field %s of %s %s expects type %s != %s";_tag_fat(_tmp37D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp798,_tmp797,_tag_fat(_tmp37C,sizeof(void*),5U));});});
Cyc_Unify_explain_failure();}}}{
# 2028
struct Cyc_List_List*_tmp385=0;struct Cyc_List_List*rpo_inst=_tmp385;
{struct Cyc_List_List*_tmp386=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp386;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp388=_cycalloc(sizeof(*_tmp388));({struct _tuple0*_tmp79B=({struct _tuple0*_tmp387=_cycalloc(sizeof(*_tmp387));({void*_tmp79A=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp387->f1=_tmp79A;}),({
void*_tmp799=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp387->f2=_tmp799;});_tmp387;});
# 2030
_tmp388->hd=_tmp79B;}),_tmp388->tl=rpo_inst;_tmp388;});}}
# 2033
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}}
# 2039
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 2041
{void*_tmp389=Cyc_Tcutil_compress(ts);void*_stmttmp3D=_tmp389;void*_tmp38A=_stmttmp3D;struct Cyc_List_List*_tmp38C;enum Cyc_Absyn_AggrKind _tmp38B;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38A)->tag == 7U){_LL1: _tmp38B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38A)->f1;_tmp38C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38A)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp38B;struct Cyc_List_List*fs=_tmp38C;
# 2043
if((int)k == (int)1U)
({void*_tmp38D=0U;({unsigned _tmp79D=loc;struct _fat_ptr _tmp79C=({const char*_tmp38E="expecting struct but found union";_tag_fat(_tmp38E,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp79D,_tmp79C,_tag_fat(_tmp38D,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple18*_tmp38F=(struct _tuple18*)fields->hd;struct _tuple18*_stmttmp3E=_tmp38F;struct _tuple18*_tmp390=_stmttmp3E;struct Cyc_Absyn_Exp*_tmp392;struct Cyc_Absyn_Aggrfield*_tmp391;_LL6: _tmp391=_tmp390->f1;_tmp392=_tmp390->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp391;struct Cyc_Absyn_Exp*field_exp=_tmp392;
({struct Cyc_Tcenv_Tenv*_tmp79F=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp79E=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp79F,_tmp79E,field_exp);});
# 2052
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7A1=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7A0=field_exp;Cyc_Tcutil_coerce_arg(_tmp7A1,_tmp7A0,field->type,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,_tmp546.f1=(struct _fat_ptr)((struct _fat_ptr)*field->name);_tmp546;});struct Cyc_String_pa_PrintArg_struct _tmp396=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,({
struct _fat_ptr _tmp7A2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field->type));_tmp545.f1=_tmp7A2;});_tmp545;});struct Cyc_String_pa_PrintArg_struct _tmp397=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,({
struct _fat_ptr _tmp7A3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(field_exp->topt)));_tmp544.f1=_tmp7A3;});_tmp544;});void*_tmp393[3U];_tmp393[0]=& _tmp395,_tmp393[1]=& _tmp396,_tmp393[2]=& _tmp397;({unsigned _tmp7A5=field_exp->loc;struct _fat_ptr _tmp7A4=({const char*_tmp394="field %s of struct expects type %s != %s";_tag_fat(_tmp394,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp7A5,_tmp7A4,_tag_fat(_tmp393,sizeof(void*),3U));});});
Cyc_Unify_explain_failure();}}}
# 2059
goto _LL0;}}}else{_LL3: _LL4:
({void*_tmp398=0U;({struct _fat_ptr _tmp7A6=({const char*_tmp399="tcAnonStruct: wrong type";_tag_fat(_tmp399,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp7A6,_tag_fat(_tmp398,sizeof(void*),0U));});});}_LL0:;}
# 2062
return ts;}
# 2066
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2069
struct _tuple12 _tmp39A=({struct _tuple12 _tmp54C;({struct Cyc_List_List*_tmp7A7=Cyc_Tcenv_lookup_type_vars(te);_tmp54C.f1=_tmp7A7;}),_tmp54C.f2=Cyc_Core_heap_region;_tmp54C;});struct _tuple12 env=_tmp39A;
struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp39B;
struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp39C;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp7A8=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp7A8,all_typs);});
# 2074
if(topt != 0){
void*_tmp39D=Cyc_Tcutil_compress(*topt);void*_stmttmp3F=_tmp39D;void*_tmp39E=_stmttmp3F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2080
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple16*)ts->hd)).f2;
if(inst != 0)t=Cyc_Tcutil_substitute(inst,t);
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
return res;}}
# 2106
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 2108
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))return 1;
# 2110
if(topt != 0){
void*_tmp3AB=Cyc_Tcutil_compress(*topt);void*_stmttmp40=_tmp3AB;void*_tmp3AC=_stmttmp40;void*_tmp3AD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->tag == 3U){_LL1: _tmp3AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).elt_type;_LL2: {void*elt_typ=_tmp3AD;
# 2113
Cyc_Unify_unify(elt_typ,t);
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))return 1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2119
return 0;}
# 2126
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2128
enum Cyc_Absyn_AliasQual _tmp3AE=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp41=_tmp3AE;enum Cyc_Absyn_AliasQual _tmp3AF=_stmttmp41;if(_tmp3AF == Cyc_Absyn_Unique){_LL1: _LL2:
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
void*_tmp3B0=Cyc_Tcutil_compress(handle_type);void*_stmttmp42=_tmp3B0;void*_tmp3B1=_stmttmp42;void*_tmp3B2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f2 != 0){_LL1: _tmp3B2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B1)->f2)->hd;_LL2: {void*r=_tmp3B2;
# 2147
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2151
({struct Cyc_String_pa_PrintArg_struct _tmp3B5=({struct Cyc_String_pa_PrintArg_struct _tmp54D;_tmp54D.tag=0U,({
struct _fat_ptr _tmp7BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp54D.f1=_tmp7BA;});_tmp54D;});void*_tmp3B3[1U];_tmp3B3[0]=& _tmp3B5;({unsigned _tmp7BC=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _fat_ptr _tmp7BB=({const char*_tmp3B4="expecting region_t type but found %s";_tag_fat(_tmp3B4,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7BC,_tmp7BB,_tag_fat(_tmp3B3,sizeof(void*),1U));});});
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
void*_tmp3BB=er;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BC;void*_tmp3BE;switch(*((int*)_tmp3BB)){case 17U: _LL6: _tmp3BE=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3BB)->f1;_LL7: {void*t2=_tmp3BE;
# 2191
elt_type=t2;
({void**_tmp7C6=({void**_tmp3BF=_cycalloc(sizeof(*_tmp3BF));*_tmp3BF=elt_type;_tmp3BF;});*t=_tmp7C6;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->tl)->tl == 0){_LL8: _tmp3BC=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->hd;_tmp3BD=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3BB)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp3BC;struct Cyc_Absyn_Exp*e2=_tmp3BD;
# 2198
{struct _tuple0 _tmp3C0=({struct _tuple0 _tmp551;_tmp551.f1=e1->r,_tmp551.f2=e2->r;_tmp551;});struct _tuple0 _stmttmp43=_tmp3C0;struct _tuple0 _tmp3C1=_stmttmp43;void*_tmp3C2;void*_tmp3C3;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C1.f1)->tag == 17U){_LLD: _tmp3C3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C1.f1)->f1;_LLE: {void*t1=_tmp3C3;
# 2201
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t1)){
# 2204
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t1))
({struct Cyc_String_pa_PrintArg_struct _tmp3C6=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp7C7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp54F.f1=_tmp7C7;});_tmp54F;});void*_tmp3C4[1U];_tmp3C4[0]=& _tmp3C6;({unsigned _tmp7C9=loc;struct _fat_ptr _tmp7C8=({const char*_tmp3C5="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3C5,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp7C9,_tmp7C8,_tag_fat(_tmp3C4,sizeof(void*),1U));});});else{
# 2207
*is_calloc=1;}}
# 2210
elt_type=t1;
({void**_tmp7CA=({void**_tmp3C7=_cycalloc(sizeof(*_tmp3C7));*_tmp3C7=elt_type;_tmp3C7;});*t=_tmp7CA;});
num_elts=e2;
one_elt=0;
goto _LLC;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C1.f2)->tag == 17U){_LLF: _tmp3C2=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C1.f2)->f1;_LL10: {void*t2=_tmp3C2;
# 2217
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 2220
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp3CA=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({struct _fat_ptr _tmp7CB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp550.f1=_tmp7CB;});_tmp550;});void*_tmp3C8[1U];_tmp3C8[0]=& _tmp3CA;({unsigned _tmp7CD=loc;struct _fat_ptr _tmp7CC=({const char*_tmp3C9="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3C9,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp7CD,_tmp7CC,_tag_fat(_tmp3C8,sizeof(void*),1U));});});else{
# 2223
*is_calloc=1;}}
# 2226
elt_type=t2;
({void**_tmp7CE=({void**_tmp3CB=_cycalloc(sizeof(*_tmp3CB));*_tmp3CB=elt_type;_tmp3CB;});*t=_tmp7CE;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2233
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2235
 No_sizeof: {
# 2238
struct Cyc_Absyn_Exp*_tmp3CC=*e;struct Cyc_Absyn_Exp*real_e=_tmp3CC;
{void*_tmp3CD=real_e->r;void*_stmttmp44=_tmp3CD;void*_tmp3CE=_stmttmp44;struct Cyc_Absyn_Exp*_tmp3CF;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CE)->tag == 14U){_LL14: _tmp3CF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CE)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp3CF;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2243
{void*_tmp3D0=Cyc_Tcutil_compress((void*)_check_null(real_e->topt));void*_stmttmp45=_tmp3D0;void*_tmp3D1=_stmttmp45;void*_tmp3D2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f2 != 0){_LL19: _tmp3D2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D1)->f2)->hd;_LL1A: {void*tagt=_tmp3D2;
# 2245
{void*_tmp3D3=Cyc_Tcutil_compress(tagt);void*_stmttmp46=_tmp3D3;void*_tmp3D4=_stmttmp46;struct Cyc_Absyn_Exp*_tmp3D5;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D4)->tag == 9U){_LL1E: _tmp3D5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D4)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp3D5;
# 2247
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2250
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2253
elt_type=Cyc_Absyn_char_type;
({void**_tmp7CF=({void**_tmp3D6=_cycalloc(sizeof(*_tmp3D6));*_tmp3D6=elt_type;_tmp3D6;});*t=_tmp7CF;});
num_elts=*e;
one_elt=0;}
# 2258
goto _LL5;}_LL5:;}}
# 2262
*is_fat=!one_elt;
# 2265
{void*_tmp3D7=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3D8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp3D8=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp3D8;
# 2267
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({void*_tmp3D9=0U;({unsigned _tmp7D1=loc;struct _fat_ptr _tmp7D0=({const char*_tmp3DA="malloc with existential types not yet implemented";_tag_fat(_tmp3DA,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp7D1,_tmp7D0,_tag_fat(_tmp3D9,sizeof(void*),0U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2274
void*(*_tmp3DB)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp3DB;
void*_tmp3DC=Cyc_Absyn_false_type;void*zero_term=_tmp3DC;
if(topt != 0){
void*_tmp3DD=Cyc_Tcutil_compress(*topt);void*_stmttmp47=_tmp3DD;void*_tmp3DE=_stmttmp47;void*_tmp3E1;void*_tmp3E0;void*_tmp3DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->tag == 3U){_LL28: _tmp3DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->f1).ptr_atts).nullable;_tmp3E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->f1).ptr_atts).bounds;_tmp3E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp3DF;void*b=_tmp3E0;void*zt=_tmp3E1;
# 2279
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2284
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({void*_tmp3E2=0U;({unsigned _tmp7D3=loc;struct _fat_ptr _tmp7D2=({const char*_tmp3E3="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp3E3,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp7D3,_tmp7D2,_tag_fat(_tmp3E2,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2290
struct Cyc_Absyn_Exp*_tmp3E4=({void*_tmp7D4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7D4,b);});struct Cyc_Absyn_Exp*eopt=_tmp3E4;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp3E5=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp3E5;
int _tmp3E6=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp3E6;
if(is_constant && Cyc_Evexp_same_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp7D8=elt_type;void*_tmp7D7=rgn;struct Cyc_Absyn_Tqual _tmp7D6=Cyc_Absyn_empty_tqual(0U);void*_tmp7D5=b;Cyc_Absyn_atb_type(_tmp7D8,_tmp7D7,_tmp7D6,_tmp7D5,zero_term);});}{
# 2298
void*_tmp3E7=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_stmttmp48=_tmp3E7;void*_tmp3E8=_stmttmp48;void*_tmp3E9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f2 != 0){_LL2D: _tmp3E9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8)->f2)->hd;_LL2E: {void*tagtyp=_tmp3E9;
# 2300
struct Cyc_Absyn_Exp*_tmp3EA=({void*_tmp7D9=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7D9,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp3EA;
# 2302
if(Cyc_Evexp_same_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp7DD=elt_type;void*_tmp7DC=rgn;struct Cyc_Absyn_Tqual _tmp7DB=Cyc_Absyn_empty_tqual(0U);void*_tmp7DA=b;Cyc_Absyn_atb_type(_tmp7DD,_tmp7DC,_tmp7DB,_tmp7DA,zero_term);});}
# 2306
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2310
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2313
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp7E1)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp7E0=elt_type;void*_tmp7DF=rgn;struct Cyc_Absyn_Tqual _tmp7DE=Cyc_Absyn_empty_tqual(0U);_tmp7E1(_tmp7E0,_tmp7DF,_tmp7DE,zero_term);});}}}
# 2318
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2320
struct Cyc_Tcenv_Tenv*_tmp3EB=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp3EB;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp3EC=(void*)_check_null(e1->topt);void*t1=_tmp3EC;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2327
{void*_tmp3ED=Cyc_Tcutil_compress(t1);void*_stmttmp49=_tmp3ED;void*_tmp3EE=_stmttmp49;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EE)->tag == 4U){_LL1: _LL2:
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
# 2344
void*result=_tmp3F7;
# 2346
return result;}
# 2348
return Cyc_Absyn_void_type;}}}
# 2353
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp7F4=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp7F4,s,1);});
# 2356
while(1){
void*_tmp3FC=s->r;void*_stmttmp4A=_tmp3FC;void*_tmp3FD=_stmttmp4A;struct Cyc_Absyn_Stmt*_tmp3FF;struct Cyc_Absyn_Decl*_tmp3FE;struct Cyc_Absyn_Stmt*_tmp401;struct Cyc_Absyn_Stmt*_tmp400;struct Cyc_Absyn_Exp*_tmp402;switch(*((int*)_tmp3FD)){case 1U: _LL1: _tmp402=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp402;
# 2360
void*_tmp403=(void*)_check_null(e->topt);void*t=_tmp403;
if(!({void*_tmp7F5=t;Cyc_Unify_unify(_tmp7F5,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp406=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,({
struct _fat_ptr _tmp7F6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp554.f1=_tmp7F6;});_tmp554;});void*_tmp404[1U];_tmp404[0]=& _tmp406;({unsigned _tmp7F8=loc;struct _fat_ptr _tmp7F7=({const char*_tmp405="statement expression returns type %s";_tag_fat(_tmp405,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7F8,_tmp7F7,_tag_fat(_tmp404,sizeof(void*),1U));});});
Cyc_Unify_explain_failure();}
# 2366
return t;}case 2U: _LL3: _tmp400=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f1;_tmp401=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f2;_LL4: {struct Cyc_Absyn_Stmt*s1=_tmp400;struct Cyc_Absyn_Stmt*s2=_tmp401;
s=s2;continue;}case 12U: _LL5: _tmp3FE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f1;_tmp3FF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FD)->f2;_LL6: {struct Cyc_Absyn_Decl*d=_tmp3FE;struct Cyc_Absyn_Stmt*s1=_tmp3FF;
s=s1;continue;}default: _LL7: _LL8:
# 2370
 return({void*_tmp407=0U;({struct Cyc_Tcenv_Tenv*_tmp7FC=te;unsigned _tmp7FB=loc;void**_tmp7FA=topt;struct _fat_ptr _tmp7F9=({const char*_tmp408="statement expression must end with expression";_tag_fat(_tmp408,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7FC,_tmp7FB,_tmp7FA,_tmp7F9,_tag_fat(_tmp407,sizeof(void*),0U));});});}_LL0:;}}
# 2375
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp7FD=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7FD,0,e);}));
{void*_tmp409=t;struct Cyc_Absyn_Aggrdecl*_tmp40A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp40A=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp409)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp40A;
# 2379
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2382
({struct Cyc_String_pa_PrintArg_struct _tmp40D=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp7FE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp555.f1=_tmp7FE;});_tmp555;});void*_tmp40B[1U];_tmp40B[0]=& _tmp40D;({unsigned _tmp800=loc;struct _fat_ptr _tmp7FF=({const char*_tmp40C="expecting @tagged union but found %s";_tag_fat(_tmp40C,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp800,_tmp7FF,_tag_fat(_tmp40B,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2385
return Cyc_Absyn_uint_type;}
# 2389
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2393
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp40E=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));struct Cyc_Tcenv_Tenv*te=_tmp40E;
if(*rgn_handle != 0){
# 2398
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp40F=Cyc_Tcutil_compress(handle_type);void*_stmttmp4B=_tmp40F;void*_tmp410=_stmttmp4B;void*_tmp411;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2 != 0){_LL1: _tmp411=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp410)->f2)->hd;_LL2: {void*r=_tmp411;
# 2403
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2407
({struct Cyc_String_pa_PrintArg_struct _tmp414=({struct Cyc_String_pa_PrintArg_struct _tmp556;_tmp556.tag=0U,({
struct _fat_ptr _tmp801=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp556.f1=_tmp801;});_tmp556;});void*_tmp412[1U];_tmp412[0]=& _tmp414;({unsigned _tmp803=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _fat_ptr _tmp802=({const char*_tmp413="expecting region_t type but found %s";_tag_fat(_tmp413,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp803,_tmp802,_tag_fat(_tmp412,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2414
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp804=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp804;});}}}{
# 2422
void*_tmp415=e1->r;void*_stmttmp4C=_tmp415;void*_tmp416=_stmttmp4C;struct Cyc_List_List*_tmp417;struct Cyc_List_List*_tmp419;struct Cyc_Core_Opt*_tmp418;switch(*((int*)_tmp416)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2427
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp41A=0U;({struct _fat_ptr _tmp805=({const char*_tmp41B="tcNew: comprehension returned non-array type";_tag_fat(_tmp41B,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp805,_tag_fat(_tmp41A,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp808=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp807=loc;void*_tmp806=res_typ;Cyc_Tcutil_silent_castable(_tmp808,_tmp807,_tmp806,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2439
return res_typ;}case 36U: _LLA: _tmp418=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp416)->f1;_tmp419=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp416)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp418;struct Cyc_List_List*des=_tmp419;
# 2441
({void*_tmp809=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->tag=26U,_tmp41C->f1=des;_tmp41C;});e1->r=_tmp809;});
_tmp417=des;goto _LLD;}case 26U: _LLC: _tmp417=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp416)->f1;_LLD: {struct Cyc_List_List*des=_tmp417;
# 2444
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp41D=Cyc_Tcutil_compress(*topt);void*_stmttmp4D=_tmp41D;void*_tmp41E=_stmttmp4D;void*_tmp421;struct Cyc_Absyn_Tqual _tmp420;void**_tmp41F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->tag == 3U){_LL15: _tmp41F=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).elt_type;_tmp420=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).elt_tq;_tmp421=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp41F;struct Cyc_Absyn_Tqual tq=_tmp420;void*zt=_tmp421;
# 2450
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2456
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp422=0U;({struct _fat_ptr _tmp80A=({const char*_tmp423="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp423,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp80A,_tag_fat(_tmp422,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2465
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp80D=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp80C=loc;void*_tmp80B=res_typ;Cyc_Tcutil_silent_castable(_tmp80D,_tmp80C,_tmp80B,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2472
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp416)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2477
void*_tmp424=({void*_tmp811=Cyc_Absyn_char_type;void*_tmp810=rgn;struct Cyc_Absyn_Tqual _tmp80F=Cyc_Absyn_const_tqual(0U);void*_tmp80E=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp811,_tmp810,_tmp80F,_tmp80E,Cyc_Absyn_true_type);});void*topt2=_tmp424;
# 2479
void*_tmp425=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp425;
return({void*_tmp815=t;void*_tmp814=rgn;struct Cyc_Absyn_Tqual _tmp813=Cyc_Absyn_empty_tqual(0U);void*_tmp812=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp815,_tmp814,_tmp813,_tmp812,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2484
void*_tmp426=({void*_tmp819=Cyc_Absyn_wchar_type();void*_tmp818=rgn;struct Cyc_Absyn_Tqual _tmp817=Cyc_Absyn_const_tqual(0U);void*_tmp816=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp819,_tmp818,_tmp817,_tmp816,Cyc_Absyn_true_type);});void*topt2=_tmp426;
# 2486
void*_tmp427=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp427;
return({void*_tmp81D=t;void*_tmp81C=rgn;struct Cyc_Absyn_Tqual _tmp81B=Cyc_Absyn_empty_tqual(0U);void*_tmp81A=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp81D,_tmp81C,_tmp81B,_tmp81A,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2493
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp428=Cyc_Tcutil_compress(*topt);void*_stmttmp4E=_tmp428;void*_tmp429=_stmttmp4E;struct Cyc_Absyn_Tqual _tmp42B;void**_tmp42A;switch(*((int*)_tmp429)){case 3U: _LL1A: _tmp42A=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).elt_type;_tmp42B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).elt_tq;_LL1B: {void**elttype=_tmp42A;struct Cyc_Absyn_Tqual tq=_tmp42B;
# 2499
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp429)->f1)->tag == 18U){_LL1C: _LL1D:
# 2503
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2509
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2511
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt)&& !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp42C=0U;({unsigned _tmp81F=e1->loc;struct _fat_ptr _tmp81E=({const char*_tmp42D="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp42D,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp81F,_tmp81E,_tag_fat(_tmp42C,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));
_tmp42E->tag=3U,(_tmp42E->f1).elt_type=telt,({struct Cyc_Absyn_Tqual _tmp822=Cyc_Absyn_empty_tqual(0U);(_tmp42E->f1).elt_tq=_tmp822;}),
((_tmp42E->f1).ptr_atts).rgn=rgn,({void*_tmp821=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmp42E->f1).ptr_atts).nullable=_tmp821;}),({
void*_tmp820=Cyc_Absyn_bounds_one();((_tmp42E->f1).ptr_atts).bounds=_tmp820;}),((_tmp42E->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp42E->f1).ptr_atts).ptrloc=0;_tmp42E;});
# 2518
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp825=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp824=loc;void*_tmp823=res_typ;Cyc_Tcutil_silent_castable(_tmp825,_tmp824,_tmp823,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2526
return res_typ;}}}}_LL5:;}}
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
void*_tmp431=e->r;void*_stmttmp4F=_tmp431;void*_tmp432=_stmttmp4F;switch(*((int*)_tmp432)){case 26U: _LL1: _LL2:
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
return t;}_LL0:;}}
# 2576 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp433=e->r;void*_stmttmp50=_tmp433;void*_tmp434=_stmttmp50;struct Cyc_Absyn_Exp*_tmp435;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp434)->tag == 12U){_LL1: _tmp435=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp434)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp435;
# 2580
Cyc_Tcexp_tcExpNoInst(te,topt,e2);
({void*_tmp82B=Cyc_Absyn_pointer_expand((void*)_check_null(e2->topt),0);e2->topt=_tmp82B;});
e->topt=e2->topt;
goto _LL0;}}else{_LL3: _LL4:
# 2586
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp82C=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp82C;});
# 2589
{void*_tmp436=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp51=_tmp436;void*_tmp437=_stmttmp51;void*_tmp43D;void*_tmp43C;void*_tmp43B;void*_tmp43A;struct Cyc_Absyn_Tqual _tmp439;void*_tmp438;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437)->tag == 3U){_LL6: _tmp438=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437)->f1).elt_type;_tmp439=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437)->f1).elt_tq;_tmp43A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437)->f1).ptr_atts).rgn;_tmp43B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437)->f1).ptr_atts).nullable;_tmp43C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437)->f1).ptr_atts).bounds;_tmp43D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437)->f1).ptr_atts).zero_term;_LL7: {void*t=_tmp438;struct Cyc_Absyn_Tqual tq=_tmp439;void*rt=_tmp43A;void*x=_tmp43B;void*b=_tmp43C;void*zt=_tmp43D;
# 2591
{void*_tmp43E=Cyc_Tcutil_compress(t);void*_stmttmp52=_tmp43E;void*_tmp43F=_stmttmp52;struct Cyc_List_List*_tmp44C;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_List_List*_tmp44A;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_List_List*_tmp448;struct Cyc_List_List*_tmp447;struct Cyc_Absyn_VarargInfo*_tmp446;int _tmp445;struct Cyc_List_List*_tmp444;void*_tmp443;struct Cyc_Absyn_Tqual _tmp442;void*_tmp441;struct Cyc_List_List*_tmp440;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->tag == 5U){_LLB: _tmp440=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).tvars;_tmp441=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).effect;_tmp442=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ret_tqual;_tmp443=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ret_type;_tmp444=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).args;_tmp445=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).c_varargs;_tmp446=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).cyc_varargs;_tmp447=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).rgn_po;_tmp448=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).attributes;_tmp449=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).requires_clause;_tmp44A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).requires_relns;_tmp44B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ensures_clause;_tmp44C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ensures_relns;_LLC: {struct Cyc_List_List*tvs=_tmp440;void*eff=_tmp441;struct Cyc_Absyn_Tqual rtq=_tmp442;void*rtyp=_tmp443;struct Cyc_List_List*args=_tmp444;int c_varargs=_tmp445;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp446;struct Cyc_List_List*rpo=_tmp447;struct Cyc_List_List*atts=_tmp448;struct Cyc_Absyn_Exp*req=_tmp449;struct Cyc_List_List*req_relns=_tmp44A;struct Cyc_Absyn_Exp*ens=_tmp44B;struct Cyc_List_List*ens_relns=_tmp44C;
# 2593
if(tvs != 0){
struct _tuple12 _tmp44D=({struct _tuple12 _tmp557;({struct Cyc_List_List*_tmp82D=Cyc_Tcenv_lookup_type_vars(te);_tmp557.f1=_tmp82D;}),_tmp557.f2=Cyc_Core_heap_region;_tmp557;});struct _tuple12 env=_tmp44D;
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tvs);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2601
rpo=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,rpo);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,rpo);{
void*ftyp=({struct Cyc_List_List*_tmp82E=inst;Cyc_Tcutil_substitute(_tmp82E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp452=_cycalloc(sizeof(*_tmp452));
_tmp452->tag=5U,(_tmp452->f1).tvars=0,(_tmp452->f1).effect=eff,(_tmp452->f1).ret_tqual=rtq,(_tmp452->f1).ret_type=rtyp,(_tmp452->f1).args=args,(_tmp452->f1).c_varargs=c_varargs,(_tmp452->f1).cyc_varargs=cyc_varargs,(_tmp452->f1).rgn_po=0,(_tmp452->f1).attributes=atts,(_tmp452->f1).requires_clause=req,(_tmp452->f1).requires_relns=req_relns,(_tmp452->f1).ensures_clause=ens,(_tmp452->f1).ensures_relns=ens_relns;_tmp452;}));});
# 2608
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44E=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=3U,(_tmp451->f1).elt_type=ftyp,(_tmp451->f1).elt_tq=tq,((_tmp451->f1).ptr_atts).rgn=rt,((_tmp451->f1).ptr_atts).nullable=x,((_tmp451->f1).ptr_atts).bounds=b,((_tmp451->f1).ptr_atts).zero_term=zt,((_tmp451->f1).ptr_atts).ptrloc=0;_tmp451;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_typ=_tmp44E;
# 2610
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp44F;
({void*_tmp82F=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->tag=13U,_tmp450->f1=inner,_tmp450->f2=ts;_tmp450;});e->r=_tmp82F;});
e->topt=(void*)new_typ;}}
# 2614
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2617
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2620
goto _LL0;}_LL0:;}
# 2622
return(void*)_check_null(e->topt);}
# 2630
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2632
struct Cyc_List_List*_tmp453=0;struct Cyc_List_List*decls=_tmp453;
# 2634
{void*_tmp454=fn_exp->r;void*_stmttmp53=_tmp454;void*_tmp455=_stmttmp53;struct Cyc_List_List*_tmp456;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp455)->tag == 10U){_LL1: _tmp456=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp455)->f2;_LL2: {struct Cyc_List_List*es=_tmp456;
# 2636
{void*_tmp457=e->r;void*_stmttmp54=_tmp457;void*_tmp458=_stmttmp54;struct Cyc_List_List*_tmp459;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp458)->tag == 10U){_LL6: _tmp459=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp458)->f2;_LL7: {struct Cyc_List_List*es2=_tmp459;
# 2638
struct Cyc_List_List*_tmp45A=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp45A;
int _tmp45B=0;int arg_cnt=_tmp45B;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({void*_tmp45C=0U;({struct _fat_ptr _tmp831=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp45F=({struct Cyc_Int_pa_PrintArg_struct _tmp559;_tmp559.tag=1U,_tmp559.f1=(unsigned long)arg_cnt;_tmp559;});struct Cyc_Int_pa_PrintArg_struct _tmp460=({struct Cyc_Int_pa_PrintArg_struct _tmp558;_tmp558.tag=1U,_tmp558.f1=(unsigned long)((int)arg_exps->hd);_tmp558;});void*_tmp45D[2U];_tmp45D[0]=& _tmp45F,_tmp45D[1]=& _tmp460;({struct _fat_ptr _tmp830=({const char*_tmp45E="bad count %d/%d for alias coercion!";_tag_fat(_tmp45E,sizeof(char),36U);});Cyc_aprintf(_tmp830,_tag_fat(_tmp45D,sizeof(void*),2U));});});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp831,_tag_fat(_tmp45C,sizeof(void*),0U));});});
# 2645
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2650
struct _tuple11 _tmp461=({struct Cyc_Absyn_Exp*_tmp832=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp832,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple11 _stmttmp55=_tmp461;struct _tuple11 _tmp462=_stmttmp55;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Absyn_Decl*_tmp463;_LLB: _tmp463=_tmp462.f1;_tmp464=_tmp462.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp463;struct Cyc_Absyn_Exp*ve=_tmp464;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->hd=d,_tmp465->tl=decls;_tmp465;});
arg_exps=arg_exps->tl;}}}
# 2655
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp466=0U;({struct _fat_ptr _tmp833=({const char*_tmp467="not a function call!";_tag_fat(_tmp467,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp833,_tag_fat(_tmp466,sizeof(void*),0U));});});}_LL5:;}
# 2658
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp468=0U;({struct _fat_ptr _tmp834=({const char*_tmp469="not a function call!";_tag_fat(_tmp469,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp834,_tag_fat(_tmp468,sizeof(void*),0U));});});}_LL0:;}
# 2663
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp46A=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp46A;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp837=({struct Cyc_Absyn_Decl*_tmp836=d;struct Cyc_Absyn_Stmt*_tmp835=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp836,_tmp835,e->loc);});Cyc_Absyn_stmt_exp(_tmp837,e->loc);});
decls=decls->tl;}
# 2670
e->topt=0;
e->r=fn_exp->r;}
# 2676
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2680
{void*_tmp46B=e->r;void*_stmttmp56=_tmp46B;void*_tmp46C=_stmttmp56;void*_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;struct _fat_ptr*_tmp470;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp471;int*_tmp477;struct Cyc_Absyn_Exp**_tmp476;void***_tmp475;struct Cyc_Absyn_Exp**_tmp474;int*_tmp473;struct Cyc_Absyn_Enumfield*_tmp479;void*_tmp478;struct Cyc_Absyn_Enumfield*_tmp47B;struct Cyc_Absyn_Enumdecl*_tmp47A;struct Cyc_Absyn_Datatypefield*_tmp47E;struct Cyc_Absyn_Datatypedecl*_tmp47D;struct Cyc_List_List*_tmp47C;struct Cyc_List_List*_tmp480;void*_tmp47F;struct Cyc_Absyn_Aggrdecl**_tmp484;struct Cyc_List_List*_tmp483;struct Cyc_List_List**_tmp482;struct _tuple1**_tmp481;int*_tmp487;void*_tmp486;struct Cyc_Absyn_Exp*_tmp485;int*_tmp48B;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Vardecl*_tmp488;struct Cyc_Absyn_Stmt*_tmp48C;struct Cyc_List_List*_tmp48D;struct Cyc_List_List*_tmp48F;struct _tuple9*_tmp48E;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp491;int*_tmp496;int*_tmp495;struct _fat_ptr*_tmp494;struct Cyc_Absyn_Exp*_tmp493;int*_tmp49A;int*_tmp499;struct _fat_ptr*_tmp498;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_List_List*_tmp49D;void*_tmp49C;void*_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp**_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A2;enum Cyc_Absyn_Coercion*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;void*_tmp4A3;struct Cyc_List_List*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Core_Opt*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B2;enum Cyc_Absyn_Incrementor _tmp4B6;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_List_List*_tmp4B8;enum Cyc_Absyn_Primop _tmp4B7;void**_tmp4B9;union Cyc_Absyn_Cnst*_tmp4BA;struct Cyc_List_List*_tmp4BC;struct Cyc_Core_Opt*_tmp4BB;struct Cyc_Absyn_VarargCallInfo**_tmp4BF;struct Cyc_List_List*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BD;int*_tmp4C3;struct Cyc_Absyn_VarargCallInfo**_tmp4C2;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C4;switch(*((int*)_tmp46C)){case 12U: _LL1: _tmp4C4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp4C4;
# 2685
Cyc_Tcexp_tcExpNoInst(te,0,e2);
return;}case 10U: _LL3: _tmp4C0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4C1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4C2=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp4C3=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;if(!(*_tmp4C3)){_LL4: {struct Cyc_Absyn_Exp*e1=_tmp4C0;struct Cyc_List_List*es=_tmp4C1;struct Cyc_Absyn_VarargCallInfo**vci=_tmp4C2;int*resolved=_tmp4C3;
# 2689
({void*_tmp4C5=0U;({struct _fat_ptr _tmp838=({const char*_tmp4C6="unresolved function in tcExpNoInst";_tag_fat(_tmp4C6,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp838,_tag_fat(_tmp4C5,sizeof(void*),0U));});});}}else{_LL1B: _tmp4BD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4BE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4BF=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp4BD;struct Cyc_List_List*es=_tmp4BE;struct Cyc_Absyn_VarargCallInfo**vci=_tmp4BF;
# 2722
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4C7;_push_handler(& _tmp4C7);{int _tmp4C9=0;if(setjmp(_tmp4C7.handler))_tmp4C9=1;if(!_tmp4C9){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp4C8=(void*)Cyc_Core_get_exn_thrown();void*_tmp4CA=_tmp4C8;void*_tmp4CB;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4CA)->tag == Cyc_Core_Failure){_LL58: _LL59:
# 2729
 ok=0;
fn_exp=e;
goto _LL57;}else{_LL5A: _tmp4CB=_tmp4CA;_LL5B: {void*exn=_tmp4CB;(int)_rethrow(exn);}}_LL57:;}}}
# 2733
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2740
goto _LL0;}}case 36U: _LL5: _tmp4BB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4BC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp4BB;struct Cyc_List_List*des=_tmp4BC;
# 2693
Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp4BA=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp4BA;
# 2698
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp4B9=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LLA: {void**b=_tmp4B9;
# 2700
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
# 2702
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp4B7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4B8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp4B7;struct Cyc_List_List*es=_tmp4B8;
# 2704
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp4B5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4B6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp4B5;enum Cyc_Absyn_Incrementor i=_tmp4B6;
# 2706
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp4B2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4B3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4B4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp4B2;struct Cyc_Core_Opt*popt=_tmp4B3;struct Cyc_Absyn_Exp*e2=_tmp4B4;
# 2708
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp4AF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4B0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4B1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp4AF;struct Cyc_Absyn_Exp*e2=_tmp4B0;struct Cyc_Absyn_Exp*e3=_tmp4B1;
# 2710
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp4AD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4AE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp4AD;struct Cyc_Absyn_Exp*e2=_tmp4AE;
# 2712
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp4AB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4AC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp4AB;struct Cyc_Absyn_Exp*e2=_tmp4AC;
# 2714
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp4A9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp4A9;struct Cyc_Absyn_Exp*e2=_tmp4AA;
# 2716
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp4A8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp4A8;
# 2742
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp4A6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4A7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp4A6;struct Cyc_List_List*ts=_tmp4A7;
# 2744
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp4A3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4A4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp4A5=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL22: {void*t1=_tmp4A3;struct Cyc_Absyn_Exp*e1=_tmp4A4;enum Cyc_Absyn_Coercion*c=_tmp4A5;
# 2746
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp4A2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp4A2;
# 2748
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp4A0=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp4A1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp4A0;struct Cyc_Absyn_Exp*e1=_tmp4A1;
# 2750
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp49F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp49F;
# 2752
void*_tmp4CC=Cyc_Tcexp_tcExpNoPromote(te,0,e1);void*t1=_tmp4CC;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 17U: _LL29: _tmp49E=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL2A: {void*t1=_tmp49E;
# 2755
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp49C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp49D=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL2C: {void*t1=_tmp49C;struct Cyc_List_List*l=_tmp49D;
# 2757
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp49B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp49B;
# 2759
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp497=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp498=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp499=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp49A=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp497;struct _fat_ptr*f=_tmp498;int*is_tagged=_tmp499;int*is_read=_tmp49A;
# 2761
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp493=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp494=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp495=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp496=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp493;struct _fat_ptr*f=_tmp494;int*is_tagged=_tmp495;int*is_read=_tmp496;
# 2763
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp491=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp492=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp491;struct Cyc_Absyn_Exp*e2=_tmp492;
# 2765
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp490=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL36: {struct Cyc_List_List*es=_tmp490;
# 2767
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp48E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp48F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL38: {struct _tuple9*t1=_tmp48E;struct Cyc_List_List*des=_tmp48F;
# 2769
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp48D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL3A: {struct Cyc_List_List*des=_tmp48D;
# 2773
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4CD=Cyc_Tcutil_compress(*topt);void*_stmttmp57=_tmp4CD;void*_tmp4CE=_stmttmp57;void*_tmp4D1;struct Cyc_Absyn_Tqual*_tmp4D0;void**_tmp4CF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CE)->tag == 4U){_LL5D: _tmp4CF=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CE)->f1).elt_type;_tmp4D0=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CE)->f1).tq;_tmp4D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CE)->f1).zero_term;_LL5E: {void**et=_tmp4CF;struct Cyc_Absyn_Tqual*etq=_tmp4D0;void*zt=_tmp4D1;
# 2779
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5C;}}else{_LL5F: _LL60:
 goto _LL5C;}_LL5C:;}
# 2786
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp48C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp48C;
# 2788
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp488=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp489=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp48A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp48B=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp488;struct Cyc_Absyn_Exp*e1=_tmp489;struct Cyc_Absyn_Exp*e2=_tmp48A;int*iszeroterm=_tmp48B;
# 2790
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp485=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp486=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp487=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp485;void*t1=_tmp486;int*iszeroterm=_tmp487;
# 2792
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp481=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp482=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp483=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_tmp484=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46C)->f4;_LL42: {struct _tuple1**tn=_tmp481;struct Cyc_List_List**ts=_tmp482;struct Cyc_List_List*args=_tmp483;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp484;
# 2794
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp47F=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp480=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL44: {void*ts=_tmp47F;struct Cyc_List_List*args=_tmp480;
# 2796
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp47C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp47D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_tmp47E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46C)->f3;_LL46: {struct Cyc_List_List*es=_tmp47C;struct Cyc_Absyn_Datatypedecl*tud=_tmp47D;struct Cyc_Absyn_Datatypefield*tuf=_tmp47E;
# 2798
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp47A=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp47B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp47A;struct Cyc_Absyn_Enumfield*ef=_tmp47B;
# 2800
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp478=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp479=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL4A: {void*t2=_tmp478;struct Cyc_Absyn_Enumfield*ef=_tmp479;
# 2802
t=t2;goto _LL0;}case 34U: _LL4B: _tmp473=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).is_calloc;_tmp474=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).rgn;_tmp475=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).elt_type;_tmp476=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).num_elts;_tmp477=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46C)->f1).fat_result;_LL4C: {int*is_calloc=_tmp473;struct Cyc_Absyn_Exp**ropt=_tmp474;void***t2=_tmp475;struct Cyc_Absyn_Exp**e2=_tmp476;int*isfat=_tmp477;
# 2804
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp471=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp472=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp471;struct Cyc_Absyn_Exp*e2=_tmp472;
# 2806
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp46F=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_tmp470=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp46F;struct _fat_ptr*f=_tmp470;
# 2808
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp46E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp46E;
# 2810
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 39U: _LL53: _tmp46D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp46C)->f1;_LL54: {void*t2=_tmp46D;
# 2812
if(!te->allow_valueof)
({void*_tmp4D2=0U;({unsigned _tmp83A=e->loc;struct _fat_ptr _tmp839=({const char*_tmp4D3="valueof(-) can only occur within types";_tag_fat(_tmp4D3,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp83A,_tmp839,_tag_fat(_tmp4D2,sizeof(void*),0U));});});
# 2820
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL55: _LL56:
# 2823
({void*_tmp4D4=0U;({unsigned _tmp83C=e->loc;struct _fat_ptr _tmp83B=({const char*_tmp4D5="asm expressions cannot occur within Cyclone code.";_tag_fat(_tmp4D5,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp83C,_tmp83B,_tag_fat(_tmp4D4,sizeof(void*),0U));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}_LL0:;}
# 2826
e->topt=t;}

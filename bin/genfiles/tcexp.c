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
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 885 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 904
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 907
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 909
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 911
extern void*Cyc_Absyn_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned i);
# 914
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 918
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 920
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 937
void*Cyc_Absyn_exn_type();
# 945
extern void*Cyc_Absyn_fat_bound_type;
# 947
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 949
void*Cyc_Absyn_bounds_one();
# 951
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 955
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 957
void*Cyc_Absyn_star_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 959
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term);
# 963
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 971
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 975
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 979
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 989
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp();
# 1053
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1064
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1118
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1120
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1122
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1125
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1127
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66 "warn.h"
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
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
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 127
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 147
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 155
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
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 207
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
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
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple15{int f1;void*f2;};
# 250
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 273
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 287
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 298
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 301
int Cyc_Tcutil_zeroable_type(void*);
# 305
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 308
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 310
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);struct _tuple16{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 53
int Cyc_Evexp_okay_szofarg(void*t);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 76 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 93
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 120
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 124
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 143
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*rgn);
# 145
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*eff);
# 150
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
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr msg_part);
void Cyc_Tcexp_unique_consume_err(unsigned);
# 45 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){
# 47
Cyc_Warn_verr2(loc,ap);
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
return*topt;}
# 52
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 56
void Cyc_Tcexp_unique_consume_err(unsigned loc){
# 60
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FC;_tmp4FC.tag=0U,({struct _fat_ptr _tmp5F6=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp4FC.f1=_tmp5F6;});_tmp4FC;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp5F7=loc;Cyc_Warn_err2(_tmp5F7,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 64
static struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp3;
void*_tmp4=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->v=tenv_tvs;_tmpA;}));void*t2=_tmp4;
void*_tmp5=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->v=tenv_tvs;_tmp9;}));void*rt=_tmp5;
void*_tmp6=Cyc_Tcutil_any_bool(tenv_tvs);void*nbl=_tmp6;
void*_tmp7=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp7;
void*_tmp8=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp8;
return({struct Cyc_Absyn_PtrInfo _tmp4FD;_tmp4FD.elt_type=t2,({struct Cyc_Absyn_Tqual _tmp5F8=Cyc_Absyn_empty_tqual(0U);_tmp4FD.elt_tq=_tmp5F8;}),(_tmp4FD.ptr_atts).rgn=rt,(_tmp4FD.ptr_atts).nullable=nbl,(_tmp4FD.ptr_atts).bounds=b,(_tmp4FD.ptr_atts).zero_term=zt,(_tmp4FD.ptr_atts).ptrloc=0;_tmp4FD;});}
# 74
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 78
if(topt == 0){
# 80
({void*_tmp5F9=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp5F9;});
return;}{
# 83
void*t=*topt;
void*_tmpC=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmpC;void*_tmpD=_stmttmp0;struct Cyc_Absyn_Tqual _tmpF;void*_tmpE;union Cyc_Absyn_AggrInfo _tmp10;switch(*((int*)_tmpD)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 20U){_LL1: _tmp10=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp10;
# 86
{union Cyc_Absyn_AggrInfo _tmp11=info;struct Cyc_Absyn_Aggrdecl*_tmp12;if((_tmp11.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FE;_tmp4FE.tag=0U,({struct _fat_ptr _tmp5FA=({const char*_tmp15="struct type improperly set";_tag_fat(_tmp15,sizeof(char),27U);});_tmp4FE.f1=_tmp5FA;});_tmp4FE;});void*_tmp13[1U];_tmp13[0]=& _tmp14;({unsigned _tmp5FB=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp5FB,_tag_fat(_tmp13,sizeof(void*),1U));});});}else{_LLC: _tmp12=*(_tmp11.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmp12;
({void*_tmp5FC=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=29U,_tmp16->f1=ad->name,_tmp16->f2=0,_tmp16->f3=des,_tmp16->f4=ad;_tmp16;});e->r=_tmp5FC;});}}_LL9:;}
# 90
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).elt_type;_tmpF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD)->f1).tq;_LL4: {void*at=_tmpE;struct Cyc_Absyn_Tqual aq=_tmpF;
({void*_tmp5FD=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=26U,_tmp17->f1=des;_tmp17;});e->r=_tmp5FD;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp5FE=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->tag=30U,_tmp18->f1=t,_tmp18->f2=des;_tmp18;});e->r=_tmp5FE;});goto _LL0;default: _LL7: _LL8:
({void*_tmp5FF=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=26U,_tmp19->f1=des;_tmp19;});e->r=_tmp5FF;});goto _LL0;}_LL0:;}}
# 100
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 104
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp1A=e->r;void*_stmttmp1=_tmp1A;void*_tmp1B=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1B)->f2 == 0){_LL1: _LL2:
# 107
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF;_tmp4FF.tag=0U,({struct _fat_ptr _tmp600=({const char*_tmp1E="assignment in test";_tag_fat(_tmp1E,sizeof(char),19U);});_tmp4FF.f1=_tmp600;});_tmp4FF;});void*_tmp1C[1U];_tmp1C[0]=& _tmp1D;({unsigned _tmp601=e->loc;Cyc_Warn_warn2(_tmp601,_tag_fat(_tmp1C,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 115
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _tmp504;_tmp504.tag=0U,({struct _fat_ptr _tmp602=({const char*_tmp27="test of ";_tag_fat(_tmp27,sizeof(char),9U);});_tmp504.f1=_tmp602;});_tmp504;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _tmp503;_tmp503.tag=0U,_tmp503.f1=msg_part;_tmp503;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _tmp502;_tmp502.tag=0U,({struct _fat_ptr _tmp603=({const char*_tmp26=" has type ";_tag_fat(_tmp26,sizeof(char),11U);});_tmp502.f1=_tmp603;});_tmp502;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp23=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp501;_tmp501.tag=3U,_tmp501.f1=(void*)e->topt;_tmp501;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24=({struct Cyc_Warn_String_Warn_Warg_struct _tmp500;_tmp500.tag=0U,({
struct _fat_ptr _tmp604=({const char*_tmp25=" instead of integral or pointer type";_tag_fat(_tmp25,sizeof(char),37U);});_tmp500.f1=_tmp604;});_tmp500;});void*_tmp1F[5U];_tmp1F[0]=& _tmp20,_tmp1F[1]=& _tmp21,_tmp1F[2]=& _tmp22,_tmp1F[3]=& _tmp23,_tmp1F[4]=& _tmp24;({unsigned _tmp605=e->loc;Cyc_Warn_err2(_tmp605,_tag_fat(_tmp1F,sizeof(void*),5U));});});}
# 137 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 142
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _stmttmp2=_tmp28;union Cyc_Absyn_Cnst _tmp29=_stmttmp2;struct _fat_ptr _tmp2A;struct _fat_ptr _tmp2B;int _tmp2D;enum Cyc_Absyn_Sign _tmp2C;int _tmp2E;switch((_tmp29.String_c).tag){case 2U: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 return Cyc_Absyn_uchar_type;default: _LL5: _LL6:
 return Cyc_Absyn_char_type;}case 3U: _LL7: _LL8:
 return Cyc_Absyn_wchar_type();case 4U: if(((_tmp29.Short_c).val).f1 == Cyc_Absyn_Unsigned){_LL9: _LLA:
 return Cyc_Absyn_ushort_type;}else{_LLB: _LLC:
 return Cyc_Absyn_sshort_type;}case 6U: if(((_tmp29.LongLong_c).val).f1 == Cyc_Absyn_Unsigned){_LLD: _LLE:
 return Cyc_Absyn_ulonglong_type;}else{_LLF: _LL10:
 return Cyc_Absyn_slonglong_type;}case 7U: _LL11: _tmp2E=((_tmp29.Float_c).val).f2;_LL12: {int i=_tmp2E;
# 157
if(topt == 0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_tmp2F=Cyc_Tcutil_compress(*topt);void*_stmttmp3=_tmp2F;void*_tmp30=_stmttmp3;int _tmp31;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->tag == 2U){_LL1C: _tmp31=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp30)->f1)->f1;_LL1D: {int i=_tmp31;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E: _LL1F:
 return Cyc_Absyn_gen_float_type((unsigned)i);}_LL1B:;}}case 5U: _LL13: _tmp2C=((_tmp29.Int_c).val).f1;_tmp2D=((_tmp29.Int_c).val).f2;_LL14: {enum Cyc_Absyn_Sign csn=_tmp2C;int i=_tmp2D;
# 167
if(topt == 0)
return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_tmp32=Cyc_Tcutil_compress(*topt);void*_stmttmp4=_tmp32;void*_tmp33=_stmttmp4;void*_tmp34;enum Cyc_Absyn_Sign _tmp35;enum Cyc_Absyn_Sign _tmp36;enum Cyc_Absyn_Sign _tmp37;enum Cyc_Absyn_Sign _tmp38;switch(*((int*)_tmp33)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f2){case Cyc_Absyn_Char_sz: _LL21: _tmp38=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL22: {enum Cyc_Absyn_Sign sn=_tmp38;
# 171
({union Cyc_Absyn_Cnst _tmp606=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp606;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp37=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp37;
# 174
({union Cyc_Absyn_Cnst _tmp607=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp607;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp36=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp36;
_tmp35=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp35=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp35;
# 180
({union Cyc_Absyn_Cnst _tmp608=Cyc_Absyn_Int_c(sn,i);*c=_tmp608;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2 != 0){_LL2B: _tmp34=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33)->f2)->hd;_LL2C: {void*t1=_tmp34;
# 189
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=9U,({struct Cyc_Absyn_Exp*_tmp609=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp39->f1=_tmp609;});_tmp39;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 184
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp60A=({union Cyc_Absyn_Cnst _tmp505;(_tmp505.Null_c).tag=1U,(_tmp505.Null_c).val=0;_tmp505;});*c=_tmp60A;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 191
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 194
 if(topt != 0){
void*_tmp3A=Cyc_Tcutil_compress(*topt);void*_stmttmp5=_tmp3A;void*_tmp3B=_stmttmp5;void*_tmp3C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->tag == 3U){_LL30: _tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp3C;
# 197
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp507;_tmp507.tag=0U,({struct _fat_ptr _tmp60B=({const char*_tmp40="Used NULL when expecting a value of type ";_tag_fat(_tmp40,sizeof(char),42U);});_tmp507.f1=_tmp60B;});_tmp507;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp506;_tmp506.tag=2U,_tmp506.f1=(void*)*topt;_tmp506;});void*_tmp3D[2U];_tmp3D[0]=& _tmp3E,_tmp3D[1]=& _tmp3F;({unsigned _tmp60C=e->loc;Cyc_Warn_err2(_tmp60C,_tag_fat(_tmp3D,sizeof(void*),2U));});});
return*topt;}}else{_LL32: _LL33:
 goto _LL2F;}_LL2F:;}{
# 202
struct Cyc_Absyn_PtrInfo _tmp41=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp41;
(pi.ptr_atts).nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8U: _LL17: _tmp2B=(_tmp29.String_c).val;_LL18: {struct _fat_ptr s=_tmp2B;
# 207
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp2A=s;goto _LL1A;}default: _LL19: _tmp2A=(_tmp29.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp2A;
# 210
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 214
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp60D=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp60D,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 219
void*_tmp42=({void*_tmp611=string_elt_typ;void*_tmp610=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp60F=Cyc_Absyn_const_tqual(0U);void*_tmp60E=
Cyc_Absyn_thin_bounds_exp(elen);
# 219
Cyc_Absyn_atb_type(_tmp611,_tmp610,_tmp60F,_tmp60E,Cyc_Absyn_true_type);});void*t=_tmp42;
# 221
if(topt == 0)
return t;{
void*_tmp43=Cyc_Tcutil_compress(*topt);void*_stmttmp6=_tmp43;void*_tmp44=_stmttmp6;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 4U: _LL35: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp45;
# 227
return({void*_tmp614=string_elt_typ;struct Cyc_Absyn_Tqual _tmp613=tq;struct Cyc_Absyn_Exp*_tmp612=elen;Cyc_Absyn_array_type(_tmp614,_tmp613,_tmp612,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 232
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp617=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp616=loc;void*_tmp615=t;Cyc_Tcutil_silent_castable(_tmp617,_tmp616,_tmp615,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 237
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 244
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp46=*((void**)_check_null(b));void*_stmttmp7=_tmp46;void*_tmp47=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp48;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Vardecl*_tmp4A;struct Cyc_Absyn_Fndecl*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;struct _tuple1*_tmp4D;switch(*((int*)_tmp47)){case 0U: _LL1: _tmp4D=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp47)->f1;_LL2: {struct _tuple1*q=_tmp4D;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp508;_tmp508.tag=0U,({struct _fat_ptr _tmp618=({const char*_tmp50="unresolved binding in tcVar";_tag_fat(_tmp50,sizeof(char),28U);});_tmp508.f1=_tmp618;});_tmp508;});void*_tmp4E[1U];_tmp4E[0]=& _tmp4F;({unsigned _tmp619=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp619,_tag_fat(_tmp4E,sizeof(void*),1U));});});}case 1U: _LL3: _tmp4C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp47)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp4C;
# 250
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2U: _LL5: _tmp4B=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp47)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp4B;
# 257
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5U: _LL7: _tmp4A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp47)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp4A;
_tmp49=vd;goto _LLA;}case 4U: _LL9: _tmp49=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp47)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp49;
_tmp48=vd;goto _LLC;}default: _LLB: _tmp48=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp47)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp48;
# 263
if(te->allow_valueof){
void*_tmp51=Cyc_Tcutil_compress(vd->type);void*_stmttmp8=_tmp51;void*_tmp52=_stmttmp8;void*_tmp53;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2 != 0){_LLE: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52)->f2)->hd;_LLF: {void*i=_tmp53;
({void*_tmp61A=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=39U,_tmp54->f1=i;_tmp54;});e->r=_tmp61A;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 268
return vd->type;}}_LL0:;}
# 272
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 279
void*_tmp55=fmt->r;void*_stmttmp9=_tmp55;void*_tmp56=_stmttmp9;struct _fat_ptr _tmp57;struct _fat_ptr _tmp58;switch(*((int*)_tmp56)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).tag == 8){_LL1: _tmp58=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp58;
_tmp57=s;goto _LL4;}}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp57=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp57;
# 282
struct Cyc_List_List*_tmp59=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);struct Cyc_List_List*desc_types=_tmp59;
if(opt_args == 0)
return;{
struct Cyc_List_List*_tmp5A=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp5A;
# 287
for(0;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp61C=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp61B=e;Cyc_Tcutil_coerce_arg(_tmp61C,_tmp61B,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50C;_tmp50C.tag=0U,({struct _fat_ptr _tmp61D=({const char*_tmp61="descriptor has type ";_tag_fat(_tmp61,sizeof(char),21U);});_tmp50C.f1=_tmp61D;});_tmp50C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp50B;_tmp50B.tag=2U,_tmp50B.f1=(void*)t;_tmp50B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50A;_tmp50A.tag=0U,({
struct _fat_ptr _tmp61E=({const char*_tmp60=" but argument has type ";_tag_fat(_tmp60,sizeof(char),24U);});_tmp50A.f1=_tmp61E;});_tmp50A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp509;_tmp509.tag=3U,_tmp509.f1=(void*)e->topt;_tmp509;});void*_tmp5B[4U];_tmp5B[0]=& _tmp5C,_tmp5B[1]=& _tmp5D,_tmp5B[2]=& _tmp5E,_tmp5B[3]=& _tmp5F;({unsigned _tmp61F=e->loc;Cyc_Tcexp_err_and_explain(_tmp61F,_tag_fat(_tmp5B,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp620=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=(void*)arg_cnt,_tmp62->tl=*alias_arg_exps;_tmp62;});*alias_arg_exps=_tmp620;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)args->hd)->loc);}
# 302
if(desc_types != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50D;_tmp50D.tag=0U,({struct _fat_ptr _tmp621=({const char*_tmp65="too few arguments";_tag_fat(_tmp65,sizeof(char),18U);});_tmp50D.f1=_tmp621;});_tmp50D;});void*_tmp63[1U];_tmp63[0]=& _tmp64;({unsigned _tmp622=fmt->loc;Cyc_Warn_err2(_tmp622,_tag_fat(_tmp63,sizeof(void*),1U));});});
if(args != 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp67=({struct Cyc_Warn_String_Warn_Warg_struct _tmp50E;_tmp50E.tag=0U,({struct _fat_ptr _tmp623=({const char*_tmp68="too many arguments";_tag_fat(_tmp68,sizeof(char),19U);});_tmp50E.f1=_tmp623;});_tmp50E;});void*_tmp66[1U];_tmp66[0]=& _tmp67;({unsigned _tmp624=((struct Cyc_Absyn_Exp*)args->hd)->loc;Cyc_Warn_err2(_tmp624,_tag_fat(_tmp66,sizeof(void*),1U));});});
# 307
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 310
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 314
 if(opt_args == 0)
return;
{struct Cyc_List_List*_tmp69=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp69;for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt))&& !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)args->hd))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)args->hd)->loc);}}
# 322
return;}_LL0:;}
# 326
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 328
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6A=p;switch(_tmp6A){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 332
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp510;_tmp510.tag=0U,({struct _fat_ptr _tmp625=({const char*_tmp6E="expecting numeric type but found ";_tag_fat(_tmp6E,sizeof(char),34U);});_tmp510.f1=_tmp625;});_tmp510;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp50F;_tmp50F.tag=2U,_tmp50F.f1=(void*)t;_tmp50F;});void*_tmp6B[2U];_tmp6B[0]=& _tmp6C,_tmp6B[1]=& _tmp6D;({unsigned _tmp626=loc;Cyc_Warn_err2(_tmp626,_tag_fat(_tmp6B,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 336
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp70=({struct Cyc_Warn_String_Warn_Warg_struct _tmp512;_tmp512.tag=0U,({struct _fat_ptr _tmp627=({const char*_tmp72="expecting integral or * type but found ";_tag_fat(_tmp72,sizeof(char),40U);});_tmp512.f1=_tmp627;});_tmp512;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp71=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp511;_tmp511.tag=2U,_tmp511.f1=(void*)t;_tmp511;});void*_tmp6F[2U];_tmp6F[0]=& _tmp70,_tmp6F[1]=& _tmp71;({unsigned _tmp628=loc;Cyc_Warn_err2(_tmp628,_tag_fat(_tmp6F,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 341
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp74=({struct Cyc_Warn_String_Warn_Warg_struct _tmp514;_tmp514.tag=0U,({struct _fat_ptr _tmp629=({const char*_tmp76="expecting integral type but found ";_tag_fat(_tmp76,sizeof(char),35U);});_tmp514.f1=_tmp629;});_tmp514;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp75=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp513;_tmp513.tag=2U,_tmp513.f1=(void*)t;_tmp513;});void*_tmp73[2U];_tmp73[0]=& _tmp74,_tmp73[1]=& _tmp75;({unsigned _tmp62A=loc;Cyc_Warn_err2(_tmp62A,_tag_fat(_tmp73,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 345
{void*_tmp77=t;void*_tmp78;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp78;
# 347
struct Cyc_Absyn_Exp*_tmp79=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp79;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp515;_tmp515.tag=0U,({struct _fat_ptr _tmp62B=({const char*_tmp7C="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp7C,sizeof(char),53U);});_tmp515.f1=_tmp62B;});_tmp515;});void*_tmp7A[1U];_tmp7A[0]=& _tmp7B;({unsigned _tmp62C=loc;Cyc_Warn_err2(_tmp62C,_tag_fat(_tmp7A,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp517;_tmp517.tag=0U,({struct _fat_ptr _tmp62D=({const char*_tmp80="numelts requires pointer type, not ";_tag_fat(_tmp80,sizeof(char),36U);});_tmp517.f1=_tmp62D;});_tmp517;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp7F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp516;_tmp516.tag=2U,_tmp516.f1=(void*)t;_tmp516;});void*_tmp7D[2U];_tmp7D[0]=& _tmp7E,_tmp7D[1]=& _tmp7F;({unsigned _tmp62E=loc;Cyc_Warn_err2(_tmp62E,_tag_fat(_tmp7D,sizeof(void*),2U));});});}_LLD:;}
# 353
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp518;_tmp518.tag=0U,({struct _fat_ptr _tmp62F=({const char*_tmp83="Non-unary primop";_tag_fat(_tmp83,sizeof(char),17U);});_tmp518.f1=_tmp62F;});_tmp518;});void*_tmp81[1U];_tmp81[0]=& _tmp82;({unsigned _tmp630=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp630,_tag_fat(_tmp81,sizeof(void*),1U));});});}_LL0:;}
# 360
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp84=(void*)_check_null(e1->topt);void*t1=_tmp84;
void*_tmp85=(void*)_check_null(e2->topt);void*t2=_tmp85;
void*_tmp86=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_typ=_tmp86;
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,Cyc_Absyn_No_coercion);
return new_typ;}
# 370
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 373
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51B;_tmp51B.tag=0U,({struct _fat_ptr _tmp631=({const char*_tmp8C="type ";_tag_fat(_tmp8C,sizeof(char),6U);});_tmp51B.f1=_tmp631;});_tmp51B;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp89=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp51A;_tmp51A.tag=3U,_tmp51A.f1=(void*)e1->topt;_tmp51A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp8A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp519;_tmp519.tag=0U,({struct _fat_ptr _tmp632=({const char*_tmp8B=" cannot be used here";_tag_fat(_tmp8B,sizeof(char),21U);});_tmp519.f1=_tmp632;});_tmp519;});void*_tmp87[3U];_tmp87[0]=& _tmp88,_tmp87[1]=& _tmp89,_tmp87[2]=& _tmp8A;({struct Cyc_Tcenv_Tenv*_tmp634=te;unsigned _tmp633=e1->loc;Cyc_Tcexp_expr_err(_tmp634,_tmp633,0,_tag_fat(_tmp87,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp8E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51E;_tmp51E.tag=0U,({struct _fat_ptr _tmp635=({const char*_tmp92="type ";_tag_fat(_tmp92,sizeof(char),6U);});_tmp51E.f1=_tmp635;});_tmp51E;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp8F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp51D;_tmp51D.tag=3U,_tmp51D.f1=(void*)e2->topt;_tmp51D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp90=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51C;_tmp51C.tag=0U,({struct _fat_ptr _tmp636=({const char*_tmp91=" cannot be used here";_tag_fat(_tmp91,sizeof(char),21U);});_tmp51C.f1=_tmp636;});_tmp51C;});void*_tmp8D[3U];_tmp8D[0]=& _tmp8E,_tmp8D[1]=& _tmp8F,_tmp8D[2]=& _tmp90;({struct Cyc_Tcenv_Tenv*_tmp638=te;unsigned _tmp637=e2->loc;Cyc_Tcexp_expr_err(_tmp638,_tmp637,0,_tag_fat(_tmp8D,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 380
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp93=t1;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;struct Cyc_Absyn_Tqual _tmp95;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->tag == 3U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_type;_tmp95=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).elt_tq;_tmp96=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).rgn;_tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).nullable;_tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_tmp99=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp94;struct Cyc_Absyn_Tqual tq=_tmp95;void*r=_tmp96;void*n=_tmp97;void*b=_tmp98;void*zt=_tmp99;
# 385
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp51F;_tmp51F.tag=0U,({struct _fat_ptr _tmp639=({const char*_tmp9C="can't do arithmetic on abstract pointer type";_tag_fat(_tmp9C,sizeof(char),45U);});_tmp51F.f1=_tmp639;});_tmp51F;});void*_tmp9A[1U];_tmp9A[0]=& _tmp9B;({unsigned _tmp63A=e1->loc;Cyc_Warn_err2(_tmp63A,_tag_fat(_tmp9A,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp9E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp520;_tmp520.tag=0U,({struct _fat_ptr _tmp63B=({const char*_tmp9F="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp9F,sizeof(char),49U);});_tmp520.f1=_tmp63B;});_tmp520;});void*_tmp9D[1U];_tmp9D[0]=& _tmp9E;({unsigned _tmp63C=e1->loc;Cyc_Warn_err2(_tmp63C,_tag_fat(_tmp9D,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp522;_tmp522.tag=0U,({struct _fat_ptr _tmp63D=({const char*_tmpA3="expecting int but found ";_tag_fat(_tmpA3,sizeof(char),25U);});_tmp522.f1=_tmp63D;});_tmp522;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp521;_tmp521.tag=2U,_tmp521.f1=(void*)t2;_tmp521;});void*_tmpA0[2U];_tmpA0[0]=& _tmpA1,_tmpA0[1]=& _tmpA2;({unsigned _tmp63E=e2->loc;Cyc_Warn_err2(_tmp63E,_tag_fat(_tmpA0,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmpA4=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmpA4;
if(eopt == 0)
return t1;
# 396
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmpA5=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmpA5;struct _tuple16 _tmpA6=_stmttmpA;int _tmpA8;unsigned _tmpA7;_LL6: _tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL7: {unsigned i=_tmpA7;int known=_tmpA8;
if(known && i == (unsigned)1)
({void*_tmpA9=0U;({unsigned _tmp640=e1->loc;struct _fat_ptr _tmp63F=({const char*_tmpAA="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpAA,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp640,_tmp63F,_tag_fat(_tmpA9,sizeof(void*),0U));});});}}{
# 407
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAB=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=3U,(_tmpAC->f1).elt_type=et,(_tmpAC->f1).elt_tq=tq,((_tmpAC->f1).ptr_atts).rgn=r,((_tmpAC->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpAC->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpAC->f1).ptr_atts).zero_term=zt,((_tmpAC->f1).ptr_atts).ptrloc=0;_tmpAC;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpAB;
# 410
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,Cyc_Absyn_Other_coercion);
return(void*)new_t1;}}}}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 417
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpAE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp528;_tmp528.tag=0U,({struct _fat_ptr _tmp641=({const char*_tmpB7="pointer arithmetic on values of different ";_tag_fat(_tmpB7,sizeof(char),43U);});_tmp528.f1=_tmp641;});_tmp528;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp527;_tmp527.tag=0U,({
struct _fat_ptr _tmp642=({const char*_tmpB6="types (";_tag_fat(_tmpB6,sizeof(char),8U);});_tmp527.f1=_tmp642;});_tmp527;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp526;_tmp526.tag=2U,_tmp526.f1=(void*)t1;_tmp526;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp525;_tmp525.tag=0U,({struct _fat_ptr _tmp643=({const char*_tmpB5=" != ";_tag_fat(_tmpB5,sizeof(char),5U);});_tmp525.f1=_tmp643;});_tmp525;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp524;_tmp524.tag=2U,_tmp524.f1=(void*)t2;_tmp524;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp523;_tmp523.tag=0U,({struct _fat_ptr _tmp644=({const char*_tmpB4=")";_tag_fat(_tmpB4,sizeof(char),2U);});_tmp523.f1=_tmp644;});_tmp523;});void*_tmpAD[6U];_tmpAD[0]=& _tmpAE,_tmpAD[1]=& _tmpAF,_tmpAD[2]=& _tmpB0,_tmpAD[3]=& _tmpB1,_tmpAD[4]=& _tmpB2,_tmpAD[5]=& _tmpB3;({unsigned _tmp645=e1->loc;Cyc_Tcexp_err_and_explain(_tmp645,_tag_fat(_tmpAD,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 429
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp646=t1_elt;Cyc_Unify_unify(_tmp646,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52E;_tmp52E.tag=0U,({struct _fat_ptr _tmp647=({const char*_tmpC2="pointer arithmetic on values of different ";_tag_fat(_tmpC2,sizeof(char),43U);});_tmp52E.f1=_tmp647;});_tmp52E;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52D;_tmp52D.tag=0U,({
struct _fat_ptr _tmp648=({const char*_tmpC1="types(";_tag_fat(_tmpC1,sizeof(char),7U);});_tmp52D.f1=_tmp648;});_tmp52D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52C;_tmp52C.tag=2U,_tmp52C.f1=(void*)t1;_tmp52C;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp52B;_tmp52B.tag=0U,({struct _fat_ptr _tmp649=({const char*_tmpC0=" != ";_tag_fat(_tmpC0,sizeof(char),5U);});_tmp52B.f1=_tmp649;});_tmp52B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpBD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52A;_tmp52A.tag=2U,_tmp52A.f1=(void*)t2;_tmp52A;});struct Cyc_Warn_String_Warn_Warg_struct _tmpBE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp529;_tmp529.tag=0U,({struct _fat_ptr _tmp64A=({const char*_tmpBF=")";_tag_fat(_tmpBF,sizeof(char),2U);});_tmp529.f1=_tmp64A;});_tmp529;});void*_tmpB8[6U];_tmpB8[0]=& _tmpB9,_tmpB8[1]=& _tmpBA,_tmpB8[2]=& _tmpBB,_tmpB8[3]=& _tmpBC,_tmpB8[4]=& _tmpBD,_tmpB8[5]=& _tmpBE;({unsigned _tmp64B=e1->loc;Cyc_Tcexp_err_and_explain(_tmp64B,_tag_fat(_tmpB8,sizeof(void*),6U));});});
# 434
({void*_tmpC3=0U;({unsigned _tmp64D=e1->loc;struct _fat_ptr _tmp64C=({const char*_tmpC4="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpC4,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp64D,_tmp64C,_tag_fat(_tmpC3,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp651=e1;Cyc_Tcutil_unchecked_cast(_tmp651,({void*_tmp650=t1_elt;void*_tmp64F=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp64E=
Cyc_Absyn_empty_tqual(0U);
# 435
Cyc_Absyn_star_type(_tmp650,_tmp64F,_tmp64E,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 438
return Cyc_Absyn_sint_type;}
# 440
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpC5=0U;({unsigned _tmp653=e1->loc;struct _fat_ptr _tmp652=({const char*_tmpC6="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpC6,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp653,_tmp652,_tag_fat(_tmpC5,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC7=0U;({unsigned _tmp655=e1->loc;struct _fat_ptr _tmp654=({const char*_tmpC8="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpC8,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp655,_tmp654,_tag_fat(_tmpC7,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpCA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp532;_tmp532.tag=0U,({struct _fat_ptr _tmp656=({const char*_tmpCF="expecting either ";_tag_fat(_tmpCF,sizeof(char),18U);});_tmp532.f1=_tmp656;});_tmp532;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp531;_tmp531.tag=2U,_tmp531.f1=(void*)t1;_tmp531;});struct Cyc_Warn_String_Warn_Warg_struct _tmpCC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp530;_tmp530.tag=0U,({struct _fat_ptr _tmp657=({const char*_tmpCE=" or int but found ";_tag_fat(_tmpCE,sizeof(char),19U);});_tmp530.f1=_tmp657;});_tmp530;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpCD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp52F;_tmp52F.tag=2U,_tmp52F.f1=(void*)t2;_tmp52F;});void*_tmpC9[4U];_tmpC9[0]=& _tmpCA,_tmpC9[1]=& _tmpCB,_tmpC9[2]=& _tmpCC,_tmpC9[3]=& _tmpCD;({unsigned _tmp658=e2->loc;Cyc_Tcexp_err_and_explain(_tmp658,_tag_fat(_tmpC9,sizeof(void*),4U));});});
return t1;}
# 449
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp659=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp659,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({void*_tmpD0=0U;({unsigned _tmp65B=e1->loc;struct _fat_ptr _tmp65A=({const char*_tmpD1="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpD1,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp65B,_tmp65A,_tag_fat(_tmpD0,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp65F=e2;Cyc_Tcutil_unchecked_cast(_tmp65F,({void*_tmp65E=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp65D=
Cyc_Tcutil_pointer_region(t2);
# 454
struct Cyc_Absyn_Tqual _tmp65C=
# 456
Cyc_Absyn_empty_tqual(0U);
# 454
Cyc_Absyn_star_type(_tmp65E,_tmp65D,_tmp65C,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 459
({void*_tmpD2=0U;({unsigned _tmp661=e1->loc;struct _fat_ptr _tmp660=({const char*_tmpD3="thin pointer subtraction!";_tag_fat(_tmpD3,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp661,_tmp660,_tag_fat(_tmpD2,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 462
({void*_tmpD4=0U;({unsigned _tmp663=e1->loc;struct _fat_ptr _tmp662=({const char*_tmpD5="coercing thin pointer to integer to support subtraction";_tag_fat(_tmpD5,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp663,_tmp662,_tag_fat(_tmpD4,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 465
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpD6=0U;({unsigned _tmp665=e1->loc;struct _fat_ptr _tmp664=({const char*_tmpD7="coercing pointer to integer to support subtraction";_tag_fat(_tmpD7,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp665,_tmp664,_tag_fat(_tmpD6,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 470
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 473
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 481
if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind ||({
void*_tmp666=t1;Cyc_Unify_unify(_tmp666,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 486
if(({struct Cyc_RgnOrder_RgnPO*_tmp669=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp668=loc;void*_tmp667=t2;Cyc_Tcutil_silent_castable(_tmp669,_tmp668,_tmp667,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 490
if(({struct Cyc_RgnOrder_RgnPO*_tmp66C=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp66B=loc;void*_tmp66A=t1;Cyc_Tcutil_silent_castable(_tmp66C,_tmp66B,_tmp66A,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 494
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 499
{struct _tuple0 _tmpD8=({struct _tuple0 _tmp533;({void*_tmp66E=Cyc_Tcutil_compress(t1);_tmp533.f1=_tmp66E;}),({void*_tmp66D=Cyc_Tcutil_compress(t2);_tmp533.f2=_tmp66D;});_tmp533;});struct _tuple0 _stmttmpB=_tmpD8;struct _tuple0 _tmpD9=_stmttmpB;void*_tmpDB;void*_tmpDA;switch(*((int*)_tmpD9.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9.f2)->tag == 3U){_LL1: _tmpDA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9.f1)->f1).elt_type;_tmpDB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9.f2)->f1).elt_type;_LL2: {void*t1a=_tmpDA;void*t2a=_tmpDB;
# 501
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD9.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD9.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD9.f2)->f1)->tag == 3U){_LL3: _LL4:
# 505
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 509
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp537;_tmp537.tag=0U,({struct _fat_ptr _tmp66F=({const char*_tmpE2="comparison not allowed between ";_tag_fat(_tmpE2,sizeof(char),32U);});_tmp537.f1=_tmp66F;});_tmp537;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpDE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp536;_tmp536.tag=2U,_tmp536.f1=(void*)t1;_tmp536;});struct Cyc_Warn_String_Warn_Warg_struct _tmpDF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp535;_tmp535.tag=0U,({struct _fat_ptr _tmp670=({const char*_tmpE1=" and ";_tag_fat(_tmpE1,sizeof(char),6U);});_tmp535.f1=_tmp670;});_tmp535;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp534;_tmp534.tag=2U,_tmp534.f1=(void*)t2;_tmp534;});void*_tmpDC[4U];_tmpDC[0]=& _tmpDD,_tmpDC[1]=& _tmpDE,_tmpDC[2]=& _tmpDF,_tmpDC[3]=& _tmpE0;({unsigned _tmp671=loc;Cyc_Tcexp_err_and_explain(_tmp671,_tag_fat(_tmpDC,sizeof(void*),4U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 515
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 517
enum Cyc_Absyn_Primop _tmpE3=p;switch(_tmpE3){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 521
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 524
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL15: _LL16:
# 533
 goto _LL18;case Cyc_Absyn_Neq: _LL17: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A:
# 536
 goto _LL1C;case Cyc_Absyn_Lt: _LL1B: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL1F: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default: _LL21: _LL22:
# 541
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp538;_tmp538.tag=0U,({struct _fat_ptr _tmp672=({const char*_tmpE6="bad binary primop";_tag_fat(_tmpE6,sizeof(char),18U);});_tmp538.f1=_tmp672;});_tmp538;});void*_tmpE4[1U];_tmpE4[0]=& _tmpE5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpE4,sizeof(void*),1U));});}_LL0:;}
# 545
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 553
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpE7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpE7;
void*_tmpE8=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpE8;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpEA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53A;_tmp53A.tag=0U,({struct _fat_ptr _tmp673=({const char*_tmpEC="expecting numeric type but found ";_tag_fat(_tmpEC,sizeof(char),34U);});_tmp53A.f1=_tmp673;});_tmp53A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp539;_tmp539.tag=2U,_tmp539.f1=(void*)t;_tmp539;});void*_tmpE9[2U];_tmpE9[0]=& _tmpEA,_tmpE9[1]=& _tmpEB;({unsigned _tmp674=e->loc;Cyc_Warn_err2(_tmp674,_tag_fat(_tmpE9,sizeof(void*),2U));});});
return t;}
# 560
{struct Cyc_List_List*_tmpED=es;struct Cyc_List_List*es2=_tmpED;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _tmpEE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpEE;int _tmpEF=_stmttmpC;switch(_tmpEF){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B;_tmp53B.tag=0U,({struct _fat_ptr _tmp675=({const char*_tmpF2="primitive operator has 0 arguments";_tag_fat(_tmpF2,sizeof(char),35U);});_tmp53B.f1=_tmp675;});_tmp53B;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF1;({struct Cyc_Tcenv_Tenv*_tmp678=te;unsigned _tmp677=loc;void**_tmp676=topt;Cyc_Tcexp_expr_err(_tmp678,_tmp677,_tmp676,_tag_fat(_tmpF0,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);case 2U: _LL5: _LL6:
 return({struct Cyc_Tcenv_Tenv*_tmp67D=te;unsigned _tmp67C=loc;void**_tmp67B=topt;enum Cyc_Absyn_Primop _tmp67A=p;struct Cyc_Absyn_Exp*_tmp679=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp67D,_tmp67C,_tmp67B,_tmp67A,_tmp679,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpF4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53C;_tmp53C.tag=0U,({struct _fat_ptr _tmp67E=({const char*_tmpF5="primitive operator has > 2 arguments";_tag_fat(_tmpF5,sizeof(char),37U);});_tmp53C.f1=_tmp67E;});_tmp53C;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF4;({struct Cyc_Tcenv_Tenv*_tmp681=te;unsigned _tmp680=loc;void**_tmp67F=topt;Cyc_Tcexp_expr_err(_tmp681,_tmp680,_tmp67F,_tag_fat(_tmpF3,sizeof(void*),1U));});});}_LL0:;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 570
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);void*t=_tmpF6;
void*_tmpF7=t;struct Cyc_List_List*_tmpF8;struct Cyc_Absyn_Tqual _tmpFA;void*_tmpF9;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Datatypefield*_tmpFC;struct Cyc_Absyn_Aggrdecl*_tmpFD;switch(*((int*)_tmpF7)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpFD=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpFD;
# 574
if(ad->impl == 0){
({struct Cyc_Warn_String_Warn_Warg_struct _tmpFF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53E;_tmp53E.tag=0U,({struct _fat_ptr _tmp682=({const char*_tmp101="attempt to write abstract ";_tag_fat(_tmp101,sizeof(char),27U);});_tmp53E.f1=_tmp682;});_tmp53E;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp100=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp53D;_tmp53D.tag=6U,_tmp53D.f1=ad;_tmp53D;});void*_tmpFE[2U];_tmpFE[0]=& _tmpFF,_tmpFE[1]=& _tmp100;({unsigned _tmp683=loc;Cyc_Warn_err2(_tmp683,_tag_fat(_tmpFE,sizeof(void*),2U));});});
return 0;}
# 578
_tmpFB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpFC=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmpFC;
# 590
{struct Cyc_List_List*_tmp107=df->typs;struct Cyc_List_List*fs=_tmp107;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp108=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmp108;struct _tuple17*_tmp109=_stmttmpD;void*_tmp10B;struct Cyc_Absyn_Tqual _tmp10A;_LLE: _tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmp10A;void*t=_tmp10B;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp541;_tmp541.tag=0U,({struct _fat_ptr _tmp684=({const char*_tmp111="attempt to overwrite a datatype field (";_tag_fat(_tmp111,sizeof(char),40U);});_tmp541.f1=_tmp684;});_tmp541;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp540;_tmp540.tag=1U,_tmp540.f1=df->name;_tmp540;});struct Cyc_Warn_String_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp53F;_tmp53F.tag=0U,({
struct _fat_ptr _tmp685=({const char*_tmp110=") with a const member";_tag_fat(_tmp110,sizeof(char),22U);});_tmp53F.f1=_tmp685;});_tmp53F;});void*_tmp10C[3U];_tmp10C[0]=& _tmp10D,_tmp10C[1]=& _tmp10E,_tmp10C[2]=& _tmp10F;({unsigned _tmp686=loc;Cyc_Warn_err2(_tmp686,_tag_fat(_tmp10C,sizeof(void*),3U));});});
return 0;}
# 597
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 599
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpFB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF7)->f2;_LL4: {struct Cyc_List_List*fs=_tmpFB;
# 580
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp102=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp102;
if((f->tq).real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp104=({struct Cyc_Warn_String_Warn_Warg_struct _tmp543;_tmp543.tag=0U,({struct _fat_ptr _tmp687=({const char*_tmp106="attempt to overwrite an aggregate with const member ";_tag_fat(_tmp106,sizeof(char),53U);});_tmp543.f1=_tmp687;});_tmp543;});struct Cyc_Warn_String_Warn_Warg_struct _tmp105=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0U,_tmp542.f1=*f->name;_tmp542;});void*_tmp103[2U];_tmp103[0]=& _tmp104,_tmp103[1]=& _tmp105;({unsigned _tmp688=loc;Cyc_Warn_err2(_tmp688,_tag_fat(_tmp103,sizeof(void*),2U));});});
return 0;}
# 586
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 588
return 1;}case 4U: _LL7: _tmpF9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).elt_type;_tmpFA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).tq;_LL8: {void*elt_type=_tmpF9;struct Cyc_Absyn_Tqual tq=_tmpFA;
# 601
if(tq.real_const){
({void*_tmp112=0U;({unsigned _tmp68A=loc;struct _fat_ptr _tmp689=({const char*_tmp113="attempt to overwrite a const array";_tag_fat(_tmp113,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp68A,_tmp689,_tag_fat(_tmp112,sizeof(void*),0U));});});
return 0;}
# 605
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpF8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF7)->f1;_LLA: {struct Cyc_List_List*fs=_tmpF8;
# 607
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp114=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp114;struct _tuple17*_tmp115=_stmttmpE;void*_tmp117;struct Cyc_Absyn_Tqual _tmp116;_LL11: _tmp116=_tmp115->f1;_tmp117=_tmp115->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp116;void*t=_tmp117;
if(tq.real_const){
({void*_tmp118=0U;({unsigned _tmp68C=loc;struct _fat_ptr _tmp68B=({const char*_tmp119="attempt to overwrite a tuple field with a const member";_tag_fat(_tmp119,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp68C,_tmp68B,_tag_fat(_tmp118,sizeof(void*),0U));});});
return 0;}
# 613
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}
# 615
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 623
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 626
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp11A=e->r;void*_stmttmpF=_tmp11A;void*_tmp11B=_stmttmpF;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct _fat_ptr*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct _fat_ptr*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Vardecl*_tmp125;struct Cyc_Absyn_Vardecl*_tmp126;struct Cyc_Absyn_Vardecl*_tmp127;struct Cyc_Absyn_Vardecl*_tmp128;switch(*((int*)_tmp11B)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11B)->f1)){case 3U: _LL1: _tmp128=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11B)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp128;
_tmp127=vd;goto _LL4;}case 4U: _LL3: _tmp127=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11B)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp127;
_tmp126=vd;goto _LL6;}case 5U: _LL5: _tmp126=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11B)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp126;
_tmp125=vd;goto _LL8;}case 1U: _LL7: _tmp125=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11B)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp125;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmp123=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp11B)->f1;_tmp124=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp11B)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp123;struct Cyc_Absyn_Exp*e2=_tmp124;
# 633
{void*_tmp129=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp129;void*_tmp12A=_stmttmp10;struct Cyc_List_List*_tmp12B;struct Cyc_Absyn_Tqual _tmp12C;struct Cyc_Absyn_Tqual _tmp12D;switch(*((int*)_tmp12A)){case 3U: _LL18: _tmp12D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12A)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp12D;
_tmp12C=tq;goto _LL1B;}case 4U: _LL1A: _tmp12C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12A)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp12C;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp12B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp12A)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp12B;
# 637
struct _tuple16 _tmp12E=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp12E;struct _tuple16 _tmp12F=_stmttmp11;int _tmp131;unsigned _tmp130;_LL21: _tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;_LL22: {unsigned i=_tmp130;int known=_tmp131;
if(!known){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp133=({struct Cyc_Warn_String_Warn_Warg_struct _tmp544;_tmp544.tag=0U,({struct _fat_ptr _tmp68D=({const char*_tmp134="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp134,sizeof(char),47U);});_tmp544.f1=_tmp68D;});_tmp544;});void*_tmp132[1U];_tmp132[0]=& _tmp133;({unsigned _tmp68E=e->loc;Cyc_Warn_err2(_tmp68E,_tag_fat(_tmp132,sizeof(void*),1U));});});
return;}
# 642
{struct _handler_cons _tmp135;_push_handler(& _tmp135);{int _tmp137=0;if(setjmp(_tmp135.handler))_tmp137=1;if(!_tmp137){
{struct _tuple17*_tmp138=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple17*_stmttmp12=_tmp138;struct _tuple17*_tmp139=_stmttmp12;struct Cyc_Absyn_Tqual _tmp13A;_LL24: _tmp13A=_tmp139->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp13A;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 643
;_pop_handler();}else{void*_tmp136=(void*)Cyc_Core_get_exn_thrown();void*_tmp13B=_tmp136;void*_tmp13C;if(((struct Cyc_List_Nth_exn_struct*)_tmp13B)->tag == Cyc_List_Nth){_LL27: _LL28:
# 645
 return;}else{_LL29: _tmp13C=_tmp13B;_LL2A: {void*exn=_tmp13C;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 649
goto _LL0;}case 21U: _LLB: _tmp121=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp11B)->f1;_tmp122=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp11B)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp121;struct _fat_ptr*f=_tmp122;
# 651
{void*_tmp13D=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp13D;void*_tmp13E=_stmttmp13;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_Aggrdecl**_tmp140;switch(*((int*)_tmp13E)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp140=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13E)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp140;
# 653
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp13F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13E)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp13F;
# 658
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 663
goto _LL0;}case 22U: _LLD: _tmp11F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp11B)->f1;_tmp120=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp11B)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp11F;struct _fat_ptr*f=_tmp120;
# 665
{void*_tmp141=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp141;void*_tmp142=_stmttmp14;struct Cyc_Absyn_Tqual _tmp144;void*_tmp143;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->tag == 3U){_LL33: _tmp143=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->f1).elt_type;_tmp144=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->f1).elt_tq;_LL34: {void*elt_typ=_tmp143;struct Cyc_Absyn_Tqual tq=_tmp144;
# 667
if(!tq.real_const){
void*_tmp145=Cyc_Tcutil_compress(elt_typ);void*_stmttmp15=_tmp145;void*_tmp146=_stmttmp15;struct Cyc_List_List*_tmp147;struct Cyc_Absyn_Aggrdecl**_tmp148;switch(*((int*)_tmp146)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp148=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp148;
# 670
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp147=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp147;
# 675
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 681
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 684
goto _LL0;}case 20U: _LLF: _tmp11E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp11B)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp11E;
# 686
{void*_tmp149=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp149;void*_tmp14A=_stmttmp16;struct Cyc_Absyn_Tqual _tmp14B;struct Cyc_Absyn_Tqual _tmp14C;switch(*((int*)_tmp14A)){case 3U: _LL3F: _tmp14C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14A)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp14C;
_tmp14B=tq;goto _LL42;}case 4U: _LL41: _tmp14B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14A)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp14B;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 691
goto _LL0;}case 12U: _LL11: _tmp11D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp11B)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp11D;
_tmp11C=e1;goto _LL14;}case 13U: _LL13: _tmp11C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp11B)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp11C;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 696
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp546;_tmp546.tag=0U,({struct _fat_ptr _tmp68F=({const char*_tmp150="attempt to write a const location: ";_tag_fat(_tmp150,sizeof(char),36U);});_tmp546.f1=_tmp68F;});_tmp546;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp14F=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp545;_tmp545.tag=4U,_tmp545.f1=e;_tmp545;});void*_tmp14D[2U];_tmp14D[0]=& _tmp14E,_tmp14D[1]=& _tmp14F;({unsigned _tmp690=e->loc;Cyc_Warn_err2(_tmp690,_tag_fat(_tmp14D,sizeof(void*),2U));});});}
# 699
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 702
({struct Cyc_Tcenv_Tenv*_tmp691=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp691,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp152=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0U,({struct _fat_ptr _tmp692=({const char*_tmp153="increment/decrement of non-lvalue";_tag_fat(_tmp153,sizeof(char),34U);});_tmp547.f1=_tmp692;});_tmp547;});void*_tmp151[1U];_tmp151[0]=& _tmp152;({struct Cyc_Tcenv_Tenv*_tmp695=te;unsigned _tmp694=loc;void**_tmp693=topt;Cyc_Tcexp_expr_err(_tmp695,_tmp694,_tmp693,_tag_fat(_tmp151,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 708
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp154=0U;({unsigned _tmp697=e->loc;struct _fat_ptr _tmp696=({const char*_tmp155="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp155,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp697,_tmp696,_tag_fat(_tmp154,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp156=0U;({unsigned _tmp699=e->loc;struct _fat_ptr _tmp698=({const char*_tmp157="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp157,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp699,_tmp698,_tag_fat(_tmp156,sizeof(void*),0U));});});}else{
# 717
({struct Cyc_Warn_String_Warn_Warg_struct _tmp159=({struct Cyc_Warn_String_Warn_Warg_struct _tmp549;_tmp549.tag=0U,({struct _fat_ptr _tmp69A=({const char*_tmp15B="expecting arithmetic or ? type but found ";_tag_fat(_tmp15B,sizeof(char),42U);});_tmp549.f1=_tmp69A;});_tmp549;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp15A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp548;_tmp548.tag=2U,_tmp548.f1=(void*)t;_tmp548;});void*_tmp158[2U];_tmp158[0]=& _tmp159,_tmp158[1]=& _tmp15A;({unsigned _tmp69B=e->loc;Cyc_Warn_err2(_tmp69B,_tag_fat(_tmp158,sizeof(void*),2U));});});}}
# 719
return t;}}
# 723
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 725
({struct Cyc_Tcenv_Tenv*_tmp69D=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp69C=e1;Cyc_Tcexp_tcTest(_tmp69D,_tmp69C,({const char*_tmp15C="conditional expression";_tag_fat(_tmp15C,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*_tmp15D=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tako:& Cyc_Tcutil_tmko;struct Cyc_Core_Opt*ko=_tmp15D;
void*_tmp15E=({struct Cyc_Core_Opt*_tmp69E=ko;Cyc_Absyn_new_evar(_tmp69E,Cyc_Tcenv_lookup_opt_type_vars(te));});void*t=_tmp15E;
struct Cyc_List_List _tmp15F=({struct Cyc_List_List _tmp54F;_tmp54F.hd=e3,_tmp54F.tl=0;_tmp54F;});struct Cyc_List_List l1=_tmp15F;
struct Cyc_List_List _tmp160=({struct Cyc_List_List _tmp54E;_tmp54E.hd=e2,_tmp54E.tl=& l1;_tmp54E;});struct Cyc_List_List l2=_tmp160;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp69F=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp69F,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp162=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54D;_tmp54D.tag=0U,({struct _fat_ptr _tmp6A0=({const char*_tmp167="conditional clause types do not match: ";_tag_fat(_tmp167,sizeof(char),40U);});_tmp54D.f1=_tmp6A0;});_tmp54D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp163=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54C;_tmp54C.tag=3U,_tmp54C.f1=(void*)e2->topt;_tmp54C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp164=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0U,({
struct _fat_ptr _tmp6A1=({const char*_tmp166=" != ";_tag_fat(_tmp166,sizeof(char),5U);});_tmp54B.f1=_tmp6A1;});_tmp54B;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp165=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54A;_tmp54A.tag=3U,_tmp54A.f1=(void*)e3->topt;_tmp54A;});void*_tmp161[4U];_tmp161[0]=& _tmp162,_tmp161[1]=& _tmp163,_tmp161[2]=& _tmp164,_tmp161[3]=& _tmp165;({unsigned _tmp6A2=loc;Cyc_Tcexp_err_and_explain(_tmp6A2,_tag_fat(_tmp161,sizeof(void*),4U));});});
return t;}}
# 739
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 741
({struct Cyc_Tcenv_Tenv*_tmp6A4=te;struct Cyc_Absyn_Exp*_tmp6A3=e1;Cyc_Tcexp_tcTest(_tmp6A4,_tmp6A3,({const char*_tmp168="logical-and expression";_tag_fat(_tmp168,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6A6=te;struct Cyc_Absyn_Exp*_tmp6A5=e2;Cyc_Tcexp_tcTest(_tmp6A6,_tmp6A5,({const char*_tmp169="logical-and expression";_tag_fat(_tmp169,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 745
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 747
({struct Cyc_Tcenv_Tenv*_tmp6A8=te;struct Cyc_Absyn_Exp*_tmp6A7=e1;Cyc_Tcexp_tcTest(_tmp6A8,_tmp6A7,({const char*_tmp16A="logical-or expression";_tag_fat(_tmp16A,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6AA=te;struct Cyc_Absyn_Exp*_tmp6A9=e2;Cyc_Tcexp_tcTest(_tmp6AA,_tmp6A9,({const char*_tmp16B="logical-or expression";_tag_fat(_tmp16B,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 753
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 760
({struct Cyc_Tcenv_Tenv*_tmp6AB=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp6AB,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 765
{void*_tmp16C=Cyc_Tcutil_compress(t1);void*_stmttmp17=_tmp16C;void*_tmp16D=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16D)->tag == 4U){_LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp550;_tmp550.tag=0U,({struct _fat_ptr _tmp6AC=({const char*_tmp170="cannot assign to an array";_tag_fat(_tmp170,sizeof(char),26U);});_tmp550.f1=_tmp6AC;});_tmp550;});void*_tmp16E[1U];_tmp16E[0]=& _tmp16F;({unsigned _tmp6AD=loc;Cyc_Warn_err2(_tmp6AD,_tag_fat(_tmp16E,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 770
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp172=({struct Cyc_Warn_String_Warn_Warg_struct _tmp551;_tmp551.tag=0U,({struct _fat_ptr _tmp6AE=({const char*_tmp173="type is abstract (can't determine size)";_tag_fat(_tmp173,sizeof(char),40U);});_tmp551.f1=_tmp6AE;});_tmp551;});void*_tmp171[1U];_tmp171[0]=& _tmp172;({unsigned _tmp6AF=loc;Cyc_Warn_err2(_tmp6AF,_tag_fat(_tmp171,sizeof(void*),1U));});});
# 774
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp175=({struct Cyc_Warn_String_Warn_Warg_struct _tmp552;_tmp552.tag=0U,({struct _fat_ptr _tmp6B0=({const char*_tmp176="assignment to non-lvalue";_tag_fat(_tmp176,sizeof(char),25U);});_tmp552.f1=_tmp6B0;});_tmp552;});void*_tmp174[1U];_tmp174[0]=& _tmp175;({struct Cyc_Tcenv_Tenv*_tmp6B3=te;unsigned _tmp6B2=loc;void**_tmp6B1=topt;Cyc_Tcexp_expr_err(_tmp6B3,_tmp6B2,_tmp6B1,_tag_fat(_tmp174,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e2))
Cyc_Tcexp_unique_consume_err(e2->loc);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6B5=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6B4=e2;Cyc_Tcutil_coerce_assign(_tmp6B5,_tmp6B4,t1);})){
void*_tmp177=({struct Cyc_Warn_String_Warn_Warg_struct _tmp179=({struct Cyc_Warn_String_Warn_Warg_struct _tmp556;_tmp556.tag=0U,({struct _fat_ptr _tmp6B6=({const char*_tmp17E="type mismatch: ";_tag_fat(_tmp17E,sizeof(char),16U);});_tmp556.f1=_tmp6B6;});_tmp556;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp17A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp555;_tmp555.tag=2U,_tmp555.f1=(void*)t1;_tmp555;});struct Cyc_Warn_String_Warn_Warg_struct _tmp17B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp554;_tmp554.tag=0U,({struct _fat_ptr _tmp6B7=({const char*_tmp17D=" != ";_tag_fat(_tmp17D,sizeof(char),5U);});_tmp554.f1=_tmp6B7;});_tmp554;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp553;_tmp553.tag=2U,_tmp553.f1=(void*)t2;_tmp553;});void*_tmp178[4U];_tmp178[0]=& _tmp179,_tmp178[1]=& _tmp17A,_tmp178[2]=& _tmp17B,_tmp178[3]=& _tmp17C;({struct Cyc_Tcenv_Tenv*_tmp6BA=te;unsigned _tmp6B9=loc;void**_tmp6B8=topt;Cyc_Tcexp_expr_err(_tmp6BA,_tmp6B9,_tmp6B8,_tag_fat(_tmp178,sizeof(void*),4U));});});void*result=_tmp177;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 786
return t1;}{
# 788
struct Cyc_Absyn_Exp*_tmp17F=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp17F;
void*_tmp180=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop)po->v,e1copy,e2);void*t_result=_tmp180;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp181=({struct Cyc_Warn_String_Warn_Warg_struct _tmp183=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55B;_tmp55B.tag=0U,({
struct _fat_ptr _tmp6BB=({const char*_tmp18A="Cannot use this operator in an assignment when ";_tag_fat(_tmp18A,sizeof(char),48U);});_tmp55B.f1=_tmp6BB;});_tmp55B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp184=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55A;_tmp55A.tag=0U,({
struct _fat_ptr _tmp6BC=({const char*_tmp189="the arguments have types ";_tag_fat(_tmp189,sizeof(char),26U);});_tmp55A.f1=_tmp6BC;});_tmp55A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp185=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp559;_tmp559.tag=2U,_tmp559.f1=(void*)t1;_tmp559;});struct Cyc_Warn_String_Warn_Warg_struct _tmp186=({struct Cyc_Warn_String_Warn_Warg_struct _tmp558;_tmp558.tag=0U,({struct _fat_ptr _tmp6BD=({const char*_tmp188=" and ";_tag_fat(_tmp188,sizeof(char),6U);});_tmp558.f1=_tmp6BD;});_tmp558;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp187=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp557;_tmp557.tag=2U,_tmp557.f1=(void*)t2;_tmp557;});void*_tmp182[5U];_tmp182[0]=& _tmp183,_tmp182[1]=& _tmp184,_tmp182[2]=& _tmp185,_tmp182[3]=& _tmp186,_tmp182[4]=& _tmp187;({struct Cyc_Tcenv_Tenv*_tmp6C0=te;unsigned _tmp6BF=loc;void**_tmp6BE=topt;Cyc_Tcexp_expr_err(_tmp6C0,_tmp6BF,_tmp6BE,_tag_fat(_tmp182,sizeof(void*),5U));});});
# 793
void*result=_tmp181;
# 796
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 800
return t_result;}}}}
# 804
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp6C1=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp6C1,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp6C3=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp6C2=topt;Cyc_Tcexp_tcExp(_tmp6C3,_tmp6C2,e2);});
return(void*)_check_null(e2->topt);}
# 811
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 814
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 817
{void*_tmp18B=Cyc_Tcutil_compress(t1);void*_stmttmp18=_tmp18B;void*_tmp18C=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f1)->f1 == 0){_LL1: _LL2:
# 819
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
# 822
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 826
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp18D=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp18D;struct Cyc_Absyn_Datatypefield _tmp18E=_stmttmp19;enum Cyc_Absyn_Scope _tmp192;unsigned _tmp191;struct Cyc_List_List*_tmp190;struct _tuple1*_tmp18F;_LLA: _tmp18F=_tmp18E.name;_tmp190=_tmp18E.typs;_tmp191=_tmp18E.loc;_tmp192=_tmp18E.sc;_LLB: {struct _tuple1*n=_tmp18F;struct Cyc_List_List*typs=_tmp190;unsigned loc=_tmp191;enum Cyc_Absyn_Scope sc=_tmp192;
# 829
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 832
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 836
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp193=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp193;struct Cyc_Absyn_Datatypefield _tmp194=_stmttmp1A;enum Cyc_Absyn_Scope _tmp198;unsigned _tmp197;struct Cyc_List_List*_tmp196;struct _tuple1*_tmp195;_LLD: _tmp195=_tmp194.name;_tmp196=_tmp194.typs;_tmp197=_tmp194.loc;_tmp198=_tmp194.sc;_LLE: {struct _tuple1*n=_tmp195;struct Cyc_List_List*typs=_tmp196;unsigned loc=_tmp197;enum Cyc_Absyn_Scope sc=_tmp198;
# 839
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 843
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp6C5=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6C4=e;Cyc_Tcutil_coerce_arg(_tmp6C5,_tmp6C4,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 847
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55F;_tmp55F.tag=0U,({struct _fat_ptr _tmp6C6=({const char*_tmp19F="can't find a field in ";_tag_fat(_tmp19F,sizeof(char),23U);});_tmp55F.f1=_tmp6C6;});_tmp55F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55E;_tmp55E.tag=2U,_tmp55E.f1=(void*)tu;_tmp55E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp19C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55D;_tmp55D.tag=0U,({struct _fat_ptr _tmp6C7=({const char*_tmp19E=" to inject value of type ";_tag_fat(_tmp19E,sizeof(char),26U);});_tmp55D.f1=_tmp6C7;});_tmp55D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp19D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp55C;_tmp55C.tag=2U,_tmp55C.f1=(void*)t1;_tmp55C;});void*_tmp199[4U];_tmp199[0]=& _tmp19A,_tmp199[1]=& _tmp19B,_tmp199[2]=& _tmp19C,_tmp199[3]=& _tmp19D;({unsigned _tmp6C8=e->loc;Cyc_Warn_err2(_tmp6C8,_tag_fat(_tmp199,sizeof(void*),4U));});});
return 0;}
# 852
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 858
struct Cyc_List_List*_tmp1A0=args;struct Cyc_List_List*es=_tmp1A0;
int _tmp1A1=0;int arg_cnt=_tmp1A1;
struct Cyc_Tcenv_Tenv*_tmp1A2=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp1A2;
struct Cyc_Tcenv_Tenv*_tmp1A3=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp1A3;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 867
void*_tmp1A4=t;void*_tmp1AA;void*_tmp1A9;void*_tmp1A8;void*_tmp1A7;struct Cyc_Absyn_Tqual _tmp1A6;void*_tmp1A5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->tag == 3U){_LL1: _tmp1A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).elt_type;_tmp1A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).elt_tq;_tmp1A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).ptr_atts).rgn;_tmp1A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).ptr_atts).nullable;_tmp1A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).ptr_atts).bounds;_tmp1AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp1A5;struct Cyc_Absyn_Tqual tq=_tmp1A6;void*rgn=_tmp1A7;void*x=_tmp1A8;void*b=_tmp1A9;void*zt=_tmp1AA;
# 872
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 874
Cyc_Tcutil_check_nonzero_bound(loc,b);{
void*_tmp1AB=Cyc_Tcutil_compress(t1);void*_stmttmp1B=_tmp1AB;void*_tmp1AC=_stmttmp1B;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_List_List*_tmp1B5;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_VarargInfo*_tmp1B3;int _tmp1B2;struct Cyc_List_List*_tmp1B1;void*_tmp1B0;struct Cyc_Absyn_Tqual _tmp1AF;void*_tmp1AE;struct Cyc_List_List*_tmp1AD;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->tag == 5U){_LL6: _tmp1AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).tvars;_tmp1AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).effect;_tmp1AF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ret_tqual;_tmp1B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ret_type;_tmp1B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).args;_tmp1B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).c_varargs;_tmp1B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).cyc_varargs;_tmp1B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).rgn_po;_tmp1B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).attributes;_tmp1B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).requires_clause;_tmp1B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).requires_relns;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ensures_clause;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ensures_relns;_LL7: {struct Cyc_List_List*tvars=_tmp1AD;void*eff=_tmp1AE;struct Cyc_Absyn_Tqual res_tq=_tmp1AF;void*res_typ=_tmp1B0;struct Cyc_List_List*args_info=_tmp1B1;int c_vararg=_tmp1B2;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp1B3;struct Cyc_List_List*rgn_po=_tmp1B4;struct Cyc_List_List*atts=_tmp1B5;struct Cyc_Absyn_Exp*req=_tmp1B6;struct Cyc_List_List*req_relns=_tmp1B7;struct Cyc_Absyn_Exp*ens=_tmp1B8;struct Cyc_List_List*ens_relns=_tmp1B9;
# 878
if(tvars != 0 || rgn_po != 0)
({void*_tmp1BA=0U;({unsigned _tmp6CA=e->loc;struct _fat_ptr _tmp6C9=({const char*_tmp1BB="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1BB,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp6CA,_tmp6C9,_tag_fat(_tmp1BA,sizeof(void*),0U));});});
# 882
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 884
while(es != 0 && args_info != 0){
# 886
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6CC=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6CB=e1;Cyc_Tcutil_coerce_arg(_tmp6CC,_tmp6CB,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1CB="actual argument has type ";_tag_fat(_tmp1CB,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1CA=" but formal has type ";_tag_fat(_tmp1CA,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp6CF=({unsigned long _tmp6CE=({unsigned long _tmp6CD=Cyc_strlen((struct _fat_ptr)s0);_tmp6CD + Cyc_strlen((struct _fat_ptr)s1);});_tmp6CE + Cyc_strlen((struct _fat_ptr)s2);});_tmp6CF + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({void*_tmp1BC=0U;({unsigned _tmp6D2=e1->loc;struct _fat_ptr _tmp6D1=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1BF=({struct Cyc_String_pa_PrintArg_struct _tmp563;_tmp563.tag=0U,_tmp563.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp563;});struct Cyc_String_pa_PrintArg_struct _tmp1C0=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0U,_tmp562.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp562;});struct Cyc_String_pa_PrintArg_struct _tmp1C1=({struct Cyc_String_pa_PrintArg_struct _tmp561;_tmp561.tag=0U,_tmp561.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp561;});struct Cyc_String_pa_PrintArg_struct _tmp1C2=({struct Cyc_String_pa_PrintArg_struct _tmp560;_tmp560.tag=0U,_tmp560.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp560;});void*_tmp1BD[4U];_tmp1BD[0]=& _tmp1BF,_tmp1BD[1]=& _tmp1C0,_tmp1BD[2]=& _tmp1C1,_tmp1BD[3]=& _tmp1C2;({struct _fat_ptr _tmp6D0=({const char*_tmp1BE="%s\n\t%s\n%s\n\t%s.";_tag_fat(_tmp1BE,sizeof(char),15U);});Cyc_aprintf(_tmp6D0,_tag_fat(_tmp1BD,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp6D2,_tmp6D1,_tag_fat(_tmp1BC,sizeof(void*),0U));});});else{
# 898
({void*_tmp1C3=0U;({unsigned _tmp6D5=e1->loc;struct _fat_ptr _tmp6D4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C6=({struct Cyc_String_pa_PrintArg_struct _tmp567;_tmp567.tag=0U,_tmp567.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp567;});struct Cyc_String_pa_PrintArg_struct _tmp1C7=({struct Cyc_String_pa_PrintArg_struct _tmp566;_tmp566.tag=0U,_tmp566.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp566;});struct Cyc_String_pa_PrintArg_struct _tmp1C8=({struct Cyc_String_pa_PrintArg_struct _tmp565;_tmp565.tag=0U,_tmp565.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp565;});struct Cyc_String_pa_PrintArg_struct _tmp1C9=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0U,_tmp564.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp564;});void*_tmp1C4[4U];_tmp1C4[0]=& _tmp1C6,_tmp1C4[1]=& _tmp1C7,_tmp1C4[2]=& _tmp1C8,_tmp1C4[3]=& _tmp1C9;({struct _fat_ptr _tmp6D3=({const char*_tmp1C5="%s%s%s%s.";_tag_fat(_tmp1C5,sizeof(char),10U);});Cyc_aprintf(_tmp6D3,_tag_fat(_tmp1C4,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp6D5,_tmp6D4,_tag_fat(_tmp1C3,sizeof(void*),0U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 903
if(alias_coercion)
({struct Cyc_List_List*_tmp6D6=({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=(void*)arg_cnt,_tmp1CC->tl=*alias_arg_exps;_tmp1CC;});*alias_arg_exps=_tmp6D6;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 914
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1CD=(void*)a->hd;void*_stmttmp1C=_tmp1CD;void*_tmp1CE=_stmttmp1C;int _tmp1D1;int _tmp1D0;enum Cyc_Absyn_Format_Type _tmp1CF;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CE)->tag == 19U){_LLB: _tmp1CF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CE)->f1;_tmp1D0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CE)->f2;_tmp1D1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CE)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1CF;int fmt_arg_pos=_tmp1D0;int arg_start_pos=_tmp1D1;
# 918
{struct _handler_cons _tmp1D2;_push_handler(& _tmp1D2);{int _tmp1D4=0;if(setjmp(_tmp1D2.handler))_tmp1D4=1;if(!_tmp1D4){
# 920
{struct Cyc_Absyn_Exp*_tmp1D5=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1D5;
# 922
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 926
fmt_args=({struct Cyc_Core_Opt*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));({struct Cyc_List_List*_tmp6D7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1D6->v=_tmp6D7;});_tmp1D6;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1D7=ft;switch(_tmp1D7){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 930
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 933
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 936
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 939
goto _LLF;}_LLF:;}}
# 920
;_pop_handler();}else{void*_tmp1D3=(void*)Cyc_Core_get_exn_thrown();void*_tmp1D8=_tmp1D3;void*_tmp1D9;if(((struct Cyc_List_Nth_exn_struct*)_tmp1D8)->tag == Cyc_List_Nth){_LL17: _LL18:
# 941
({void*_tmp1DA=0U;({unsigned _tmp6D9=loc;struct _fat_ptr _tmp6D8=({const char*_tmp1DB="bad format arguments";_tag_fat(_tmp1DB,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp6D9,_tmp6D8,_tag_fat(_tmp1DA,sizeof(void*),0U));});});goto _LL16;}else{_LL19: _tmp1D9=_tmp1D8;_LL1A: {void*exn=_tmp1D9;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 946
if(args_info != 0)({void*_tmp1DC=0U;({unsigned _tmp6DB=loc;struct _fat_ptr _tmp6DA=({const char*_tmp1DD="too few arguments for function";_tag_fat(_tmp1DD,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp6DB,_tmp6DA,_tag_fat(_tmp1DC,sizeof(void*),0U));});});else{
# 948
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({void*_tmp1DE=0U;({unsigned _tmp6DD=loc;struct _fat_ptr _tmp6DC=({const char*_tmp1DF="too many arguments for function";_tag_fat(_tmp1DF,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp6DD,_tmp6DC,_tag_fat(_tmp1DE,sizeof(void*),0U));});});else{
# 955
struct Cyc_Absyn_VarargInfo _tmp1E0=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1D=_tmp1E0;struct Cyc_Absyn_VarargInfo _tmp1E1=_stmttmp1D;int _tmp1E3;void*_tmp1E2;_LL1C: _tmp1E2=_tmp1E1.type;_tmp1E3=_tmp1E1.inject;_LL1D: {void*vt=_tmp1E2;int inject=_tmp1E3;
struct Cyc_Absyn_VarargCallInfo*_tmp1E4=({struct Cyc_Absyn_VarargCallInfo*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->num_varargs=0,_tmp1FC->injectors=0,_tmp1FC->vai=cyc_vararg;_tmp1FC;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1E4;
# 959
*vararg_call_info=vci;
# 961
if(!inject)
# 963
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6DF=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6DE=e1;Cyc_Tcutil_coerce_arg(_tmp6DF,_tmp6DE,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56B;_tmp56B.tag=0U,({struct _fat_ptr _tmp6E0=({const char*_tmp1EB="vararg requires type ";_tag_fat(_tmp1EB,sizeof(char),22U);});_tmp56B.f1=_tmp6E0;});_tmp56B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56A;_tmp56A.tag=2U,_tmp56A.f1=(void*)vt;_tmp56A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp569;_tmp569.tag=0U,({
struct _fat_ptr _tmp6E1=({const char*_tmp1EA=" but argument has type ";_tag_fat(_tmp1EA,sizeof(char),24U);});_tmp569.f1=_tmp6E1;});_tmp569;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1E9=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp568;_tmp568.tag=3U,_tmp568.f1=(void*)e1->topt;_tmp568;});void*_tmp1E5[4U];_tmp1E5[0]=& _tmp1E6,_tmp1E5[1]=& _tmp1E7,_tmp1E5[2]=& _tmp1E8,_tmp1E5[3]=& _tmp1E9;({unsigned _tmp6E2=loc;Cyc_Tcexp_err_and_explain(_tmp6E2,_tag_fat(_tmp1E5,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp6E3=({struct Cyc_List_List*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->hd=(void*)arg_cnt,_tmp1EC->tl=*alias_arg_exps;_tmp1EC;});*alias_arg_exps=_tmp6E3;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vt)&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}else{
# 980
void*_tmp1ED=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1E=_tmp1ED;void*_tmp1EE=_stmttmp1E;struct Cyc_List_List*_tmp1F0;struct Cyc_Absyn_Datatypedecl*_tmp1EF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1EF=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)->f1).KnownDatatype).val;_tmp1F0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1EF;struct Cyc_List_List*targs=_tmp1F0;
# 984
struct Cyc_Absyn_Datatypedecl*_tmp1F1=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1F1;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56D;_tmp56D.tag=0U,({struct _fat_ptr _tmp6E4=({const char*_tmp1F5="can't inject into abstract ";_tag_fat(_tmp1F5,sizeof(char),28U);});_tmp56D.f1=_tmp6E4;});_tmp56D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56C;_tmp56C.tag=2U,_tmp56C.f1=(void*)vt;_tmp56C;});void*_tmp1F2[2U];_tmp1F2[0]=& _tmp1F3,_tmp1F2[1]=& _tmp1F4;({unsigned _tmp6E5=loc;Cyc_Warn_err2(_tmp6E5,_tag_fat(_tmp1F2,sizeof(void*),2U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 994
({void*_tmp6E6=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp6E6,Cyc_Tcenv_curr_rgn(te));});{
# 996
struct Cyc_List_List*_tmp1F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp1F6;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1001
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt))&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}{
# 1007
struct Cyc_Absyn_Datatypefield*_tmp1F7=({struct Cyc_Tcenv_Tenv*_tmp6EA=te;struct Cyc_Absyn_Exp*_tmp6E9=e1;void*_tmp6E8=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp6E7=inst;Cyc_Tcexp_tcInjection(_tmp6EA,_tmp6E9,_tmp6E8,_tmp6E7,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp1F7;
if(f != 0)
({struct Cyc_List_List*_tmp6EC=({
struct Cyc_List_List*_tmp6EB=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6EB,({struct Cyc_List_List*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->hd=f,_tmp1F8->tl=0;_tmp1F8;}));});
# 1009
vci->injectors=_tmp6EC;});}}}
# 1012
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56E;_tmp56E.tag=0U,({struct _fat_ptr _tmp6ED=({const char*_tmp1FB="bad inject vararg type";_tag_fat(_tmp1FB,sizeof(char),23U);});_tmp56E.f1=_tmp6ED;});_tmp56E;});void*_tmp1F9[1U];_tmp1F9[0]=& _tmp1FA;({unsigned _tmp6EE=loc;Cyc_Warn_err2(_tmp6EE,_tag_fat(_tmp1F9,sizeof(void*),1U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1018
if(*alias_arg_exps == 0)
# 1027 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1029
return res_typ;}}}else{_LL8: _LL9:
# 1031
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1FE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp570;_tmp570.tag=0U,({struct _fat_ptr _tmp6EF=({const char*_tmp200="expected pointer to function but found ";_tag_fat(_tmp200,sizeof(char),40U);});_tmp570.f1=_tmp6EF;});_tmp570;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1FF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56F;_tmp56F.tag=2U,_tmp56F.f1=(void*)t;_tmp56F;});void*_tmp1FD[2U];_tmp1FD[0]=& _tmp1FE,_tmp1FD[1]=& _tmp1FF;({struct Cyc_Tcenv_Tenv*_tmp6F2=te;unsigned _tmp6F1=loc;void**_tmp6F0=topt;Cyc_Tcexp_expr_err(_tmp6F2,_tmp6F1,_tmp6F0,_tag_fat(_tmp1FD,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1034
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp202=({struct Cyc_Warn_String_Warn_Warg_struct _tmp572;_tmp572.tag=0U,({struct _fat_ptr _tmp6F3=({const char*_tmp204="expected pointer to function but found ";_tag_fat(_tmp204,sizeof(char),40U);});_tmp572.f1=_tmp6F3;});_tmp572;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp203=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp571;_tmp571.tag=2U,_tmp571.f1=(void*)t;_tmp571;});void*_tmp201[2U];_tmp201[0]=& _tmp202,_tmp201[1]=& _tmp203;({struct Cyc_Tcenv_Tenv*_tmp6F6=te;unsigned _tmp6F5=loc;void**_tmp6F4=topt;Cyc_Tcexp_expr_err(_tmp6F6,_tmp6F5,_tmp6F4,_tag_fat(_tmp201,sizeof(void*),2U));});});}_LL0:;}}}
# 1038
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp205=Cyc_Absyn_exn_type();void*exception_type=_tmp205;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6F8=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6F7=e;Cyc_Tcutil_coerce_arg(_tmp6F8,_tmp6F7,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp207=({struct Cyc_Warn_String_Warn_Warg_struct _tmp576;_tmp576.tag=0U,({struct _fat_ptr _tmp6F9=({const char*_tmp20C="expected ";_tag_fat(_tmp20C,sizeof(char),10U);});_tmp576.f1=_tmp6F9;});_tmp576;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp208=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp575;_tmp575.tag=2U,_tmp575.f1=(void*)exception_type;_tmp575;});struct Cyc_Warn_String_Warn_Warg_struct _tmp209=({struct Cyc_Warn_String_Warn_Warg_struct _tmp574;_tmp574.tag=0U,({struct _fat_ptr _tmp6FA=({const char*_tmp20B=" but found ";_tag_fat(_tmp20B,sizeof(char),12U);});_tmp574.f1=_tmp6FA;});_tmp574;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp20A=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp573;_tmp573.tag=3U,_tmp573.f1=(void*)e->topt;_tmp573;});void*_tmp206[4U];_tmp206[0]=& _tmp207,_tmp206[1]=& _tmp208,_tmp206[2]=& _tmp209,_tmp206[3]=& _tmp20A;({unsigned _tmp6FB=loc;Cyc_Warn_err2(_tmp6FB,_tag_fat(_tmp206,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1049
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1051
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp20D=t1;struct Cyc_Absyn_PtrAtts _tmp210;struct Cyc_Absyn_Tqual _tmp20F;void*_tmp20E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20D)->tag == 3U){_LL1: _tmp20E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20D)->f1).elt_type;_tmp20F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20D)->f1).elt_tq;_tmp210=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20D)->f1).ptr_atts;_LL2: {void*t=_tmp20E;struct Cyc_Absyn_Tqual tq=_tmp20F;struct Cyc_Absyn_PtrAtts atts=_tmp210;
# 1054
{void*_tmp211=Cyc_Tcutil_compress(t);void*_stmttmp1F=_tmp211;void*_tmp212=_stmttmp1F;struct Cyc_Absyn_FnInfo _tmp213;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp212)->tag == 5U){_LL6: _tmp213=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp212)->f1;_LL7: {struct Cyc_Absyn_FnInfo info=_tmp213;
# 1056
struct Cyc_List_List*_tmp214=info.tvars;struct Cyc_List_List*tvars=_tmp214;
struct Cyc_List_List*instantiation=0;
# 1059
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp6FF=loc;struct Cyc_Tcenv_Tenv*_tmp6FE=te;struct Cyc_List_List*_tmp6FD=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6FC=k;Cyc_Tctyp_check_type(_tmp6FF,_tmp6FE,_tmp6FD,_tmp6FC,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*_tmp216));({struct _tuple14*_tmp700=({struct _tuple14*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp215->f2=(void*)ts->hd;_tmp215;});_tmp216->hd=_tmp700;}),_tmp216->tl=instantiation;_tmp216;});}
# 1065
info.tvars=tvars;
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp218=({struct Cyc_Warn_String_Warn_Warg_struct _tmp577;_tmp577.tag=0U,({struct _fat_ptr _tmp701=({const char*_tmp219="too many type variables in instantiation";_tag_fat(_tmp219,sizeof(char),41U);});_tmp577.f1=_tmp701;});_tmp577;});void*_tmp217[1U];_tmp217[0]=& _tmp218;({struct Cyc_Tcenv_Tenv*_tmp704=te;unsigned _tmp703=loc;void**_tmp702=topt;Cyc_Tcexp_expr_err(_tmp704,_tmp703,_tmp702,_tag_fat(_tmp217,sizeof(void*),1U));});});
# 1071
if(tvars == 0){
({struct Cyc_List_List*_tmp705=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_tmp705;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1076
void*new_fn_typ=({struct Cyc_List_List*_tmp706=instantiation;Cyc_Tcutil_substitute(_tmp706,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->tag=5U,_tmp21A->f1=info;_tmp21A;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp578;_tmp578.elt_type=new_fn_typ,_tmp578.elt_tq=tq,_tmp578.ptr_atts=atts;_tmp578;}));}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1080
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1083
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp21C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57A;_tmp57A.tag=0U,({struct _fat_ptr _tmp707=({const char*_tmp21E="expecting polymorphic type but found ";_tag_fat(_tmp21E,sizeof(char),38U);});_tmp57A.f1=_tmp707;});_tmp57A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp579;_tmp579.tag=2U,_tmp579.f1=(void*)t1;_tmp579;});void*_tmp21B[2U];_tmp21B[0]=& _tmp21C,_tmp21B[1]=& _tmp21D;({struct Cyc_Tcenv_Tenv*_tmp70A=te;unsigned _tmp709=loc;void**_tmp708=topt;Cyc_Tcexp_expr_err(_tmp70A,_tmp709,_tmp708,_tag_fat(_tmp21B,sizeof(void*),2U));});});}
# 1087
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1089
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1091
({void*_tmp70B=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp70B;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1096
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1098
({unsigned _tmp70F=loc;struct Cyc_Tcenv_Tenv*_tmp70E=te;struct Cyc_List_List*_tmp70D=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp70C=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1098
Cyc_Tctyp_check_type(_tmp70F,_tmp70E,_tmp70D,_tmp70C,1,0,t);});
# 1100
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp712=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp711=loc;void*_tmp710=t2;Cyc_Tcutil_silent_castable(_tmp712,_tmp711,_tmp710,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1106
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp715=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp714=loc;void*_tmp713=t2;Cyc_Tcutil_castable(_tmp715,_tmp714,_tmp713,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1113
Cyc_Unify_unify(t2,t);{
void*_tmp21F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp221=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57E;_tmp57E.tag=0U,({struct _fat_ptr _tmp716=({const char*_tmp226="cannot cast ";_tag_fat(_tmp226,sizeof(char),13U);});_tmp57E.f1=_tmp716;});_tmp57E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp222=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57D;_tmp57D.tag=2U,_tmp57D.f1=(void*)t2;_tmp57D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp223=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0U,({struct _fat_ptr _tmp717=({const char*_tmp225=" to ";_tag_fat(_tmp225,sizeof(char),5U);});_tmp57C.f1=_tmp717;});_tmp57C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp224=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57B;_tmp57B.tag=2U,_tmp57B.f1=(void*)t;_tmp57B;});void*_tmp220[4U];_tmp220[0]=& _tmp221,_tmp220[1]=& _tmp222,_tmp220[2]=& _tmp223,_tmp220[3]=& _tmp224;({struct Cyc_Tcenv_Tenv*_tmp719=te;unsigned _tmp718=loc;Cyc_Tcexp_expr_err(_tmp719,_tmp718,& t,_tag_fat(_tmp220,sizeof(void*),4U));});});void*result=_tmp21F;
Cyc_Unify_explain_failure();
return result;}}}}{
# 1122
struct _tuple0 _tmp227=({struct _tuple0 _tmp57F;_tmp57F.f1=e->r,({void*_tmp71A=Cyc_Tcutil_compress(t);_tmp57F.f2=_tmp71A;});_tmp57F;});struct _tuple0 _stmttmp20=_tmp227;struct _tuple0 _tmp228=_stmttmp20;void*_tmp22C;void*_tmp22B;void*_tmp22A;int _tmp229;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->tag == 3U){_LL1: _tmp229=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp228.f1)->f1).fat_result;_tmp22A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).ptr_atts).nullable;_tmp22B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).ptr_atts).bounds;_tmp22C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp229;void*nbl=_tmp22A;void*bds=_tmp22B;void*zt=_tmp22C;
# 1126
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp22D=({void*_tmp71B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp71B,bds);});struct Cyc_Absyn_Exp*eopt=_tmp22D;
if(eopt != 0){
if((Cyc_Evexp_eval_const_uint_exp(e)).f1 == (unsigned)1)
({void*_tmp22E=0U;({unsigned _tmp71D=loc;struct _fat_ptr _tmp71C=({const char*_tmp22F="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp22F,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp71D,_tmp71C,_tag_fat(_tmp22E,sizeof(void*),0U));});});}}
# 1132
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;}_LL0:;}}}
# 1138
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp230=0;void**topt2=_tmp230;
struct Cyc_Absyn_Tqual _tmp231=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp231;
int _tmp232=0;int nullable=_tmp232;
if(topt != 0){
void*_tmp233=Cyc_Tcutil_compress(*topt);void*_stmttmp21=_tmp233;void*_tmp234=_stmttmp21;void*_tmp237;struct Cyc_Absyn_Tqual _tmp236;void**_tmp235;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp234)->tag == 3U){_LL1: _tmp235=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp234)->f1).elt_type;_tmp236=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp234)->f1).elt_tq;_tmp237=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp234)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp235;struct Cyc_Absyn_Tqual tq=_tmp236;void*n=_tmp237;
# 1145
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1159
({struct Cyc_Tcenv_Tenv*_tmp71F=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp71E=topt2;Cyc_Tcexp_tcExpNoInst(_tmp71F,_tmp71E,e);});
# 1161
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp239=({struct Cyc_Warn_String_Warn_Warg_struct _tmp580;_tmp580.tag=0U,({struct _fat_ptr _tmp720=({const char*_tmp23A="cannot take the address of an alias-free path";_tag_fat(_tmp23A,sizeof(char),46U);});_tmp580.f1=_tmp720;});_tmp580;});void*_tmp238[1U];_tmp238[0]=& _tmp239;({unsigned _tmp721=e->loc;Cyc_Warn_err2(_tmp721,_tag_fat(_tmp238,sizeof(void*),1U));});});
# 1166
{void*_tmp23B=e->r;void*_stmttmp22=_tmp23B;void*_tmp23C=_stmttmp22;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp23D;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23C)->tag == 23U){_LL6: _tmp23D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23C)->f1;_tmp23E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23C)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp23D;struct Cyc_Absyn_Exp*e2=_tmp23E;
# 1168
{void*_tmp23F=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp23=_tmp23F;void*_tmp240=_stmttmp23;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp240)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1173
({void*_tmp722=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp722;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1176
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1181
{void*_tmp241=e->r;void*_stmttmp24=_tmp241;void*_tmp242=_stmttmp24;switch(*((int*)_tmp242)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp242)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp242)->f3 == 1){_LL12: _LL13:
# 1184
({void*_tmp243=0U;({unsigned _tmp724=e->loc;struct _fat_ptr _tmp723=({const char*_tmp244="cannot take the address of a @tagged union member";_tag_fat(_tmp244,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp724,_tmp723,_tag_fat(_tmp243,sizeof(void*),0U));});});goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1189
struct _tuple15 _tmp245=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp25=_tmp245;struct _tuple15 _tmp246=_stmttmp25;void*_tmp248;int _tmp247;_LL17: _tmp247=_tmp246.f1;_tmp248=_tmp246.f2;_LL18: {int is_const=_tmp247;void*rgn=_tmp248;
# 1191
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({void*_tmp249=0U;({unsigned _tmp726=e->loc;struct _fat_ptr _tmp725=({const char*_tmp24A="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp24A,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp726,_tmp725,_tag_fat(_tmp249,sizeof(void*),0U));});});{
# 1194
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _tmp727=tq.real_const=1;tq.print_const=_tmp727;});
# 1198
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);}}}}
# 1202
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1205
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _tmp72A=loc;struct Cyc_Tcenv_Tenv*_tmp729=te;struct Cyc_List_List*_tmp728=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp72A,_tmp729,_tmp728,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp24C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp583;_tmp583.tag=0U,({struct _fat_ptr _tmp72B=({const char*_tmp250="sizeof applied to type ";_tag_fat(_tmp250,sizeof(char),24U);});_tmp583.f1=_tmp72B;});_tmp583;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp24D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp582;_tmp582.tag=2U,_tmp582.f1=(void*)t;_tmp582;});struct Cyc_Warn_String_Warn_Warg_struct _tmp24E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,({struct _fat_ptr _tmp72C=({const char*_tmp24F=" which has unknown size here";_tag_fat(_tmp24F,sizeof(char),29U);});_tmp581.f1=_tmp72C;});_tmp581;});void*_tmp24B[3U];_tmp24B[0]=& _tmp24C,_tmp24B[1]=& _tmp24D,_tmp24B[2]=& _tmp24E;({unsigned _tmp72D=loc;Cyc_Warn_err2(_tmp72D,_tag_fat(_tmp24B,sizeof(void*),3U));});});
if(topt == 0)
return Cyc_Absyn_uint_type;{
void*_tmp251=Cyc_Tcutil_compress(*topt);void*_stmttmp26=_tmp251;void*_tmp252=_stmttmp26;void*_tmp253;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp252)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp252)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp252)->f2 != 0){_LL1: _tmp253=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp252)->f2)->hd;_LL2: {void*tt=_tmp253;
# 1215
if(({void*_tmp72F=tt;Cyc_Unify_unify(_tmp72F,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp254=_cycalloc(sizeof(*_tmp254));_tmp254->tag=9U,({struct Cyc_Absyn_Exp*_tmp72E=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp254->f1=_tmp72E;});_tmp254;}));}))
return Cyc_Tcutil_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_uint_type;}_LL0:;}}
# 1222
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)
return sf->type;
return 0;}
# 1230
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1232
({unsigned _tmp732=loc;struct Cyc_Tcenv_Tenv*_tmp731=te;struct Cyc_List_List*_tmp730=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp732,_tmp731,_tmp730,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp255=fs;struct Cyc_List_List*l=_tmp255;
void*_tmp256=t_orig;void*t=_tmp256;
for(0;l != 0;l=l->tl){
void*_tmp257=(void*)l->hd;void*n=_tmp257;
void*_tmp258=n;unsigned _tmp259;struct _fat_ptr*_tmp25A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp258)->tag == 0U){_LL1: _tmp25A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp258)->f1;_LL2: {struct _fat_ptr*n=_tmp25A;
# 1240
int bad_type=1;
{void*_tmp25B=Cyc_Tcutil_compress(t);void*_stmttmp27=_tmp25B;void*_tmp25C=_stmttmp27;struct Cyc_List_List*_tmp25D;struct Cyc_Absyn_Aggrdecl**_tmp25E;switch(*((int*)_tmp25C)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25C)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25C)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp25E=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25C)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp25E;
# 1243
if((*adp)->impl == 0)
goto _LL5;
_tmp25D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL9;}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp25D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp25C)->f2;_LL9: {struct Cyc_List_List*fields=_tmp25D;
# 1247
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp260=({struct Cyc_Warn_String_Warn_Warg_struct _tmp585;_tmp585.tag=0U,({struct _fat_ptr _tmp733=({const char*_tmp262="no field of struct/union has name %s";_tag_fat(_tmp262,sizeof(char),37U);});_tmp585.f1=_tmp733;});_tmp585;});struct Cyc_Warn_String_Warn_Warg_struct _tmp261=({struct Cyc_Warn_String_Warn_Warg_struct _tmp584;_tmp584.tag=0U,_tmp584.f1=*n;_tmp584;});void*_tmp25F[2U];_tmp25F[0]=& _tmp260,_tmp25F[1]=& _tmp261;({unsigned _tmp734=loc;Cyc_Warn_err2(_tmp734,_tag_fat(_tmp25F,sizeof(void*),2U));});});else{
# 1251
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1256
if(bad_type){
if(l == fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp264=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp587;_tmp587.tag=2U,_tmp587.f1=(void*)t;_tmp587;});struct Cyc_Warn_String_Warn_Warg_struct _tmp265=({struct Cyc_Warn_String_Warn_Warg_struct _tmp586;_tmp586.tag=0U,({struct _fat_ptr _tmp735=({const char*_tmp266=" is not a known struct/union type";_tag_fat(_tmp266,sizeof(char),34U);});_tmp586.f1=_tmp735;});_tmp586;});void*_tmp263[2U];_tmp263[0]=& _tmp264,_tmp263[1]=& _tmp265;({unsigned _tmp736=loc;Cyc_Warn_err2(_tmp736,_tag_fat(_tmp263,sizeof(void*),2U));});});else{
# 1260
struct _fat_ptr _tmp267=({struct Cyc_String_pa_PrintArg_struct _tmp27D=({struct Cyc_String_pa_PrintArg_struct _tmp590;_tmp590.tag=0U,({struct _fat_ptr _tmp737=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp590.f1=_tmp737;});_tmp590;});void*_tmp27B[1U];_tmp27B[0]=& _tmp27D;({struct _fat_ptr _tmp738=({const char*_tmp27C="(%s)";_tag_fat(_tmp27C,sizeof(char),5U);});Cyc_aprintf(_tmp738,_tag_fat(_tmp27B,sizeof(void*),1U));});});struct _fat_ptr s=_tmp267;
{struct Cyc_List_List*x=fs;for(0;x != l;x=x->tl){
void*_tmp268=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp28=_tmp268;void*_tmp269=_stmttmp28;unsigned _tmp26A;struct _fat_ptr*_tmp26B;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp269)->tag == 0U){_LLD: _tmp26B=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp269)->f1;_LLE: {struct _fat_ptr*n=_tmp26B;
s=({struct Cyc_String_pa_PrintArg_struct _tmp26E=({struct Cyc_String_pa_PrintArg_struct _tmp589;_tmp589.tag=0U,_tmp589.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp589;});struct Cyc_String_pa_PrintArg_struct _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp588;_tmp588.tag=0U,_tmp588.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp588;});void*_tmp26C[2U];_tmp26C[0]=& _tmp26E,_tmp26C[1]=& _tmp26F;({struct _fat_ptr _tmp739=({const char*_tmp26D="%s.%s";_tag_fat(_tmp26D,sizeof(char),6U);});Cyc_aprintf(_tmp739,_tag_fat(_tmp26C,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp26A=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp269)->f1;_LL10: {unsigned n=_tmp26A;
s=({struct Cyc_String_pa_PrintArg_struct _tmp272=({struct Cyc_String_pa_PrintArg_struct _tmp58B;_tmp58B.tag=0U,_tmp58B.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp58B;});struct Cyc_Int_pa_PrintArg_struct _tmp273=({struct Cyc_Int_pa_PrintArg_struct _tmp58A;_tmp58A.tag=1U,_tmp58A.f1=(unsigned long)((int)n);_tmp58A;});void*_tmp270[2U];_tmp270[0]=& _tmp272,_tmp270[1]=& _tmp273;({struct _fat_ptr _tmp73A=({const char*_tmp271="%s.%d";_tag_fat(_tmp271,sizeof(char),6U);});Cyc_aprintf(_tmp73A,_tag_fat(_tmp270,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}}
# 1266
({struct Cyc_Warn_String_Warn_Warg_struct _tmp275=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58F;_tmp58F.tag=0U,_tmp58F.f1=(struct _fat_ptr)s;_tmp58F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp276=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58E;_tmp58E.tag=0U,({struct _fat_ptr _tmp73B=({const char*_tmp27A=" == ";_tag_fat(_tmp27A,sizeof(char),5U);});_tmp58E.f1=_tmp73B;});_tmp58E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp277=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58D;_tmp58D.tag=2U,_tmp58D.f1=(void*)t;_tmp58D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp278=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58C;_tmp58C.tag=0U,({struct _fat_ptr _tmp73C=({const char*_tmp279=" is not a struct/union type";_tag_fat(_tmp279,sizeof(char),28U);});_tmp58C.f1=_tmp73C;});_tmp58C;});void*_tmp274[4U];_tmp274[0]=& _tmp275,_tmp274[1]=& _tmp276,_tmp274[2]=& _tmp277,_tmp274[3]=& _tmp278;({unsigned _tmp73D=loc;Cyc_Warn_err2(_tmp73D,_tag_fat(_tmp274,sizeof(void*),4U));});});}}
# 1269
goto _LL0;}}else{_LL3: _tmp259=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp258)->f1;_LL4: {unsigned n=_tmp259;
# 1271
int bad_type=1;
{void*_tmp27E=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp27E;void*_tmp27F=_stmttmp29;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp281;struct Cyc_Absyn_Datatypefield*_tmp282;struct Cyc_Absyn_Aggrdecl**_tmp283;switch(*((int*)_tmp27F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp283=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp283;
# 1274
if((*adp)->impl == 0)
goto _LL11;
_tmp281=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp282=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp282;
# 1294
if(({unsigned _tmp73E=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp73E < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp291=({struct Cyc_Int_pa_PrintArg_struct _tmp592;_tmp592.tag=1U,({
unsigned long _tmp73F=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp592.f1=_tmp73F;});_tmp592;});struct Cyc_Int_pa_PrintArg_struct _tmp292=({struct Cyc_Int_pa_PrintArg_struct _tmp591;_tmp591.tag=1U,_tmp591.f1=(unsigned long)((int)n);_tmp591;});void*_tmp28F[2U];_tmp28F[0]=& _tmp291,_tmp28F[1]=& _tmp292;({unsigned _tmp741=loc;struct _fat_ptr _tmp740=({const char*_tmp290="datatype field has too few components: %d < %d";_tag_fat(_tmp290,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp741,_tmp740,_tag_fat(_tmp28F,sizeof(void*),2U));});});else{
# 1298
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp293=0U;({unsigned _tmp743=loc;struct _fat_ptr _tmp742=({const char*_tmp294="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp294,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp743,_tmp742,_tag_fat(_tmp293,sizeof(void*),0U));});});}}
# 1303
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp281=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27F)->f2;_LL15: {struct Cyc_List_List*fields=_tmp281;
# 1278
if(({unsigned _tmp744=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp744 <= n;}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp285=({struct Cyc_Warn_String_Warn_Warg_struct _tmp596;_tmp596.tag=0U,({struct _fat_ptr _tmp745=({const char*_tmp28A="struct/union has too few components: ";_tag_fat(_tmp28A,sizeof(char),38U);});_tmp596.f1=_tmp745;});_tmp596;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp286=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp595;_tmp595.tag=12U,({
int _tmp746=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp595.f1=_tmp746;});_tmp595;});struct Cyc_Warn_String_Warn_Warg_struct _tmp287=({struct Cyc_Warn_String_Warn_Warg_struct _tmp594;_tmp594.tag=0U,({struct _fat_ptr _tmp747=({const char*_tmp289=" <= ";_tag_fat(_tmp289,sizeof(char),5U);});_tmp594.f1=_tmp747;});_tmp594;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp288=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp593;_tmp593.tag=12U,_tmp593.f1=(int)n;_tmp593;});void*_tmp284[4U];_tmp284[0]=& _tmp285,_tmp284[1]=& _tmp286,_tmp284[2]=& _tmp287,_tmp284[3]=& _tmp288;({unsigned _tmp748=loc;Cyc_Warn_err2(_tmp748,_tag_fat(_tmp284,sizeof(void*),4U));});});else{
# 1282
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp280=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp27F)->f1;_LL17: {struct Cyc_List_List*l=_tmp280;
# 1286
if(({unsigned _tmp749=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp749 <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp28D=({struct Cyc_Int_pa_PrintArg_struct _tmp598;_tmp598.tag=1U,({
unsigned long _tmp74A=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp598.f1=_tmp74A;});_tmp598;});struct Cyc_Int_pa_PrintArg_struct _tmp28E=({struct Cyc_Int_pa_PrintArg_struct _tmp597;_tmp597.tag=1U,_tmp597.f1=(unsigned long)((int)n);_tmp597;});void*_tmp28B[2U];_tmp28B[0]=& _tmp28D,_tmp28B[1]=& _tmp28E;({unsigned _tmp74C=loc;struct _fat_ptr _tmp74B=({const char*_tmp28C="tuple has too few components: %d <= %d";_tag_fat(_tmp28C,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp74C,_tmp74B,_tag_fat(_tmp28B,sizeof(void*),2U));});});else{
# 1290
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1305
 goto _LL11;}_LL11:;}
# 1307
if(bad_type)
({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp296=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59A;_tmp59A.tag=2U,_tmp59A.f1=(void*)t;_tmp59A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp297=({struct Cyc_Warn_String_Warn_Warg_struct _tmp599;_tmp599.tag=0U,({struct _fat_ptr _tmp74D=({const char*_tmp298=" is not a known type";_tag_fat(_tmp298,sizeof(char),21U);});_tmp599.f1=_tmp74D;});_tmp599;});void*_tmp295[2U];_tmp295[0]=& _tmp296,_tmp295[1]=& _tmp297;({unsigned _tmp74E=loc;Cyc_Warn_err2(_tmp74E,_tag_fat(_tmp295,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 1312
return Cyc_Absyn_uint_type;}}
# 1316
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp299=t;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp299)->tag == 1U){_LL1: _LL2: {
# 1322
struct Cyc_Absyn_PtrInfo _tmp29A=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp29A;
({void*_tmp750=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp29B=_cycalloc(sizeof(*_tmp29B));({struct Cyc_List_List*_tmp74F=Cyc_Tcenv_lookup_type_vars(te);_tmp29B->v=_tmp74F;});_tmp29B;}));pi.elt_type=_tmp750;});{
void*_tmp29C=Cyc_Absyn_pointer_type(pi);void*new_typ=_tmp29C;
Cyc_Unify_unify(t,new_typ);
t=Cyc_Tcutil_compress(t);
goto _LL0;}}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1330
void*_tmp29D=t;void*_tmp2A1;void*_tmp2A0;void*_tmp29F;void*_tmp29E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29D)->tag == 3U){_LL6: _tmp29E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29D)->f1).elt_type;_tmp29F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29D)->f1).ptr_atts).rgn;_tmp2A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29D)->f1).ptr_atts).bounds;_tmp2A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29D)->f1).ptr_atts).zero_term;_LL7: {void*t2=_tmp29E;void*rt=_tmp29F;void*b=_tmp2A0;void*zt=_tmp2A1;
# 1332
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2A2=Cyc_Tcutil_compress(t2);void*_stmttmp2A=_tmp2A2;void*_tmp2A3=_stmttmp2A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2A3)->tag == 5U){_LLB: _LLC:
# 1337
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59B;_tmp59B.tag=0U,({struct _fat_ptr _tmp751=({const char*_tmp2A6="unnecessary dereference for function type";_tag_fat(_tmp2A6,sizeof(char),42U);});_tmp59B.f1=_tmp751;});_tmp59B;});void*_tmp2A4[1U];_tmp2A4[0]=& _tmp2A5;({unsigned _tmp752=loc;Cyc_Warn_warn2(_tmp752,_tag_fat(_tmp2A4,sizeof(void*),1U));});});
return t;}else{_LLD: _LLE:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59C;_tmp59C.tag=0U,({struct _fat_ptr _tmp753=({const char*_tmp2A9="cannot dereference abstract pointer type";_tag_fat(_tmp2A9,sizeof(char),41U);});_tmp59C.f1=_tmp753;});_tmp59C;});void*_tmp2A7[1U];_tmp2A7[0]=& _tmp2A8;({unsigned _tmp754=loc;Cyc_Warn_err2(_tmp754,_tag_fat(_tmp2A7,sizeof(void*),1U));});});}_LLA:;}
# 1342
return t2;}}else{_LL8: _LL9:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59E;_tmp59E.tag=0U,({struct _fat_ptr _tmp755=({const char*_tmp2AD="expecting pointer type but found ";_tag_fat(_tmp2AD,sizeof(char),34U);});_tmp59E.f1=_tmp755;});_tmp59E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2AC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp59D;_tmp59D.tag=2U,_tmp59D.f1=(void*)t;_tmp59D;});void*_tmp2AA[2U];_tmp2AA[0]=& _tmp2AB,_tmp2AA[1]=& _tmp2AC;({struct Cyc_Tcenv_Tenv*_tmp758=te;unsigned _tmp757=loc;void**_tmp756=topt;Cyc_Tcexp_expr_err(_tmp758,_tmp757,_tmp756,_tag_fat(_tmp2AA,sizeof(void*),2U));});});}_LL5:;}}}
# 1347
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1351
({int _tmp759=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp759;});{
void*_tmp2AE=Cyc_Tcutil_compress(aggr_type);void*_stmttmp2B=_tmp2AE;void*_tmp2AF=_stmttmp2B;struct Cyc_List_List*_tmp2B1;enum Cyc_Absyn_AggrKind _tmp2B0;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_Aggrdecl*_tmp2B2;switch(*((int*)_tmp2AF)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AF)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AF)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2B2=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AF)->f1)->f1).KnownAggr).val;_tmp2B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AF)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2B2;struct Cyc_List_List*ts=_tmp2B3;
# 1354
struct Cyc_Absyn_Aggrfield*_tmp2B4=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2B4;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2B6=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=6U,_tmp5A2.f1=ad;_tmp5A2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A1;_tmp5A1.tag=0U,({struct _fat_ptr _tmp75A=({const char*_tmp2BB=" has no ";_tag_fat(_tmp2BB,sizeof(char),9U);});_tmp5A1.f1=_tmp75A;});_tmp5A1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=0U,_tmp5A0.f1=*f;_tmp5A0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp59F;_tmp59F.tag=0U,({struct _fat_ptr _tmp75B=({const char*_tmp2BA=" member";_tag_fat(_tmp2BA,sizeof(char),8U);});_tmp59F.f1=_tmp75B;});_tmp59F;});void*_tmp2B5[4U];_tmp2B5[0]=& _tmp2B6,_tmp2B5[1]=& _tmp2B7,_tmp2B5[2]=& _tmp2B8,_tmp2B5[3]=& _tmp2B9;({struct Cyc_Tcenv_Tenv*_tmp75E=te;unsigned _tmp75D=loc;void**_tmp75C=topt;Cyc_Tcexp_expr_err(_tmp75E,_tmp75D,_tmp75C,_tag_fat(_tmp2B5,sizeof(void*),4U));});});
# 1358
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp75F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp75F,finfo->type);});{
struct Cyc_Absyn_Kind*_tmp2BC=Cyc_Tcutil_type_kind(t2);struct Cyc_Absyn_Kind*t2_kind=_tmp2BC;
# 1366
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2BD=Cyc_Tcutil_compress(t2);void*_stmttmp2C=_tmp2BD;void*_tmp2BE=_stmttmp2C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BE)->tag == 4U){_LL8: _LL9:
 goto _LL7;}else{_LLA: _LLB:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=0U,({struct _fat_ptr _tmp760=({const char*_tmp2C4="cannot get member ";_tag_fat(_tmp2C4,sizeof(char),19U);});_tmp5A5.f1=_tmp760;});_tmp5A5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=0U,_tmp5A4.f1=*f;_tmp5A4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A3;_tmp5A3.tag=0U,({
struct _fat_ptr _tmp761=({const char*_tmp2C3=" since its type is abstract";_tag_fat(_tmp2C3,sizeof(char),28U);});_tmp5A3.f1=_tmp761;});_tmp5A3;});void*_tmp2BF[3U];_tmp2BF[0]=& _tmp2C0,_tmp2BF[1]=& _tmp2C1,_tmp2BF[2]=& _tmp2C2;({struct Cyc_Tcenv_Tenv*_tmp764=te;unsigned _tmp763=loc;void**_tmp762=topt;Cyc_Tcexp_expr_err(_tmp764,_tmp763,_tmp762,_tag_fat(_tmp2BF,sizeof(void*),3U));});});}_LL7:;}
# 1374
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=0U,({struct _fat_ptr _tmp765=({const char*_tmp2CA="cannot read union member ";_tag_fat(_tmp2CA,sizeof(char),26U);});_tmp5A8.f1=_tmp765;});_tmp5A8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0U,_tmp5A7.f1=*f;_tmp5A7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=0U,({
struct _fat_ptr _tmp766=({const char*_tmp2C9=" since it is not `bits-only'";_tag_fat(_tmp2C9,sizeof(char),29U);});_tmp5A6.f1=_tmp766;});_tmp5A6;});void*_tmp2C5[3U];_tmp2C5[0]=& _tmp2C6,_tmp2C5[1]=& _tmp2C7,_tmp2C5[2]=& _tmp2C8;({struct Cyc_Tcenv_Tenv*_tmp769=te;unsigned _tmp768=loc;void**_tmp767=topt;Cyc_Tcexp_expr_err(_tmp769,_tmp768,_tmp767,_tag_fat(_tmp2C5,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1380
if(!({void*_tmp76A=t2;Cyc_Unify_unify(_tmp76A,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=0U,({struct _fat_ptr _tmp76B=({const char*_tmp2D0="must use pattern-matching to access field ";_tag_fat(_tmp2D0,sizeof(char),43U);});_tmp5AB.f1=_tmp76B;});_tmp5AB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=0U,_tmp5AA.f1=*f;_tmp5AA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0U,({struct _fat_ptr _tmp76C=({const char*_tmp2CF="\n\tdue to existential type variables.";_tag_fat(_tmp2CF,sizeof(char),37U);});_tmp5A9.f1=_tmp76C;});_tmp5A9;});void*_tmp2CB[3U];_tmp2CB[0]=& _tmp2CC,_tmp2CB[1]=& _tmp2CD,_tmp2CB[2]=& _tmp2CE;({struct Cyc_Tcenv_Tenv*_tmp76F=te;unsigned _tmp76E=loc;void**_tmp76D=topt;Cyc_Tcexp_expr_err(_tmp76F,_tmp76E,_tmp76D,_tag_fat(_tmp2CB,sizeof(void*),3U));});});}
# 1383
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2B0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2AF)->f1;_tmp2B1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2AF)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2B0;struct Cyc_List_List*fs=_tmp2B1;
# 1385
struct Cyc_Absyn_Aggrfield*_tmp2D1=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2D1;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AE;_tmp5AE.tag=0U,({struct _fat_ptr _tmp770=({const char*_tmp2D7="type has no ";_tag_fat(_tmp2D7,sizeof(char),13U);});_tmp5AE.f1=_tmp770;});_tmp5AE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AD;_tmp5AD.tag=0U,_tmp5AD.f1=*f;_tmp5AD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AC;_tmp5AC.tag=0U,({struct _fat_ptr _tmp771=({const char*_tmp2D6=" member";_tag_fat(_tmp2D6,sizeof(char),8U);});_tmp5AC.f1=_tmp771;});_tmp5AC;});void*_tmp2D2[3U];_tmp2D2[0]=& _tmp2D3,_tmp2D2[1]=& _tmp2D4,_tmp2D2[2]=& _tmp2D5;({struct Cyc_Tcenv_Tenv*_tmp774=te;unsigned _tmp773=loc;void**_tmp772=topt;Cyc_Tcexp_expr_err(_tmp774,_tmp773,_tmp772,_tag_fat(_tmp2D2,sizeof(void*),3U));});});
# 1390
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1392
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=0U,({struct _fat_ptr _tmp775=({const char*_tmp2DD="cannot read union member ";_tag_fat(_tmp2DD,sizeof(char),26U);});_tmp5B1.f1=_tmp775;});_tmp5B1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=0U,_tmp5B0.f1=*f;_tmp5B0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AF;_tmp5AF.tag=0U,({struct _fat_ptr _tmp776=({const char*_tmp2DC=" since it is not `bits-only'";_tag_fat(_tmp2DC,sizeof(char),29U);});_tmp5AF.f1=_tmp776;});_tmp5AF;});void*_tmp2D8[3U];_tmp2D8[0]=& _tmp2D9,_tmp2D8[1]=& _tmp2DA,_tmp2D8[2]=& _tmp2DB;({struct Cyc_Tcenv_Tenv*_tmp779=te;unsigned _tmp778=loc;void**_tmp777=topt;Cyc_Tcexp_expr_err(_tmp779,_tmp778,_tmp777,_tag_fat(_tmp2D8,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
# 1395
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B3;_tmp5B3.tag=0U,({struct _fat_ptr _tmp77A=({const char*_tmp2E1="expecting struct or union, found ";_tag_fat(_tmp2E1,sizeof(char),34U);});_tmp5B3.f1=_tmp77A;});_tmp5B3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2E0=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=2U,_tmp5B2.f1=(void*)aggr_type;_tmp5B2;});void*_tmp2DE[2U];_tmp2DE[0]=& _tmp2DF,_tmp2DE[1]=& _tmp2E0;({struct Cyc_Tcenv_Tenv*_tmp77D=te;unsigned _tmp77C=loc;void**_tmp77B=topt;Cyc_Tcexp_expr_err(_tmp77D,_tmp77C,_tmp77B,_tag_fat(_tmp2DE,sizeof(void*),2U));});});}_LL0:;}}
# 1400
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1404
({struct Cyc_Tcenv_Tenv*_tmp77E=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp77E,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,(void*)_check_null(e->topt),f,is_tagged,is_read);}
# 1409
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1413
void*t2=({struct Cyc_Tcenv_Tenv*_tmp780=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _tmp77F=loc;Cyc_Tcexp_tcDeref(_tmp780,_tmp77F,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1418
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1420
struct _tuple16 _tmp2E2=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple16 _stmttmp2D=_tmp2E2;struct _tuple16 _tmp2E3=_stmttmp2D;int _tmp2E5;unsigned _tmp2E4;_LL1: _tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;_LL2: {unsigned i=_tmp2E4;int known=_tmp2E5;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B4;_tmp5B4.tag=0U,({
struct _fat_ptr _tmp781=({const char*_tmp2E8="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp2E8,sizeof(char),47U);});_tmp5B4.f1=_tmp781;});_tmp5B4;});void*_tmp2E6[1U];_tmp2E6[0]=& _tmp2E7;({struct Cyc_Tcenv_Tenv*_tmp783=te;unsigned _tmp782=loc;Cyc_Tcexp_expr_err(_tmp783,_tmp782,0,_tag_fat(_tmp2E6,sizeof(void*),1U));});});{
struct _handler_cons _tmp2E9;_push_handler(& _tmp2E9);{int _tmp2EB=0;if(setjmp(_tmp2E9.handler))_tmp2EB=1;if(!_tmp2EB){{void*_tmp2EC=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp2EC;};_pop_handler();}else{void*_tmp2EA=(void*)Cyc_Core_get_exn_thrown();void*_tmp2ED=_tmp2EA;void*_tmp2EE;if(((struct Cyc_List_Nth_exn_struct*)_tmp2ED)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1426
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B9;_tmp5B9.tag=0U,({struct _fat_ptr _tmp784=({const char*_tmp2F7="index is ";_tag_fat(_tmp2F7,sizeof(char),10U);});_tmp5B9.f1=_tmp784;});_tmp5B9;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2F1=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B8;_tmp5B8.tag=12U,_tmp5B8.f1=(int)i;_tmp5B8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B7;_tmp5B7.tag=0U,({struct _fat_ptr _tmp785=({const char*_tmp2F6=" but tuple has only ";_tag_fat(_tmp2F6,sizeof(char),21U);});_tmp5B7.f1=_tmp785;});_tmp5B7;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp2F3=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B6;_tmp5B6.tag=12U,({
int _tmp786=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp5B6.f1=_tmp786;});_tmp5B6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B5;_tmp5B5.tag=0U,({struct _fat_ptr _tmp787=({const char*_tmp2F5=" fields";_tag_fat(_tmp2F5,sizeof(char),8U);});_tmp5B5.f1=_tmp787;});_tmp5B5;});void*_tmp2EF[5U];_tmp2EF[0]=& _tmp2F0,_tmp2EF[1]=& _tmp2F1,_tmp2EF[2]=& _tmp2F2,_tmp2EF[3]=& _tmp2F3,_tmp2EF[4]=& _tmp2F4;({struct Cyc_Tcenv_Tenv*_tmp789=te;unsigned _tmp788=loc;Cyc_Tcexp_expr_err(_tmp789,_tmp788,0,_tag_fat(_tmp2EF,sizeof(void*),5U));});});}else{_LL6: _tmp2EE=_tmp2ED;_LL7: {void*exn=_tmp2EE;(int)_rethrow(exn);}}_LL3:;}}}}}
# 1431
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1433
struct Cyc_Tcenv_Tenv*_tmp2F8=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp2F8;
({struct Cyc_Tcenv_Tenv*_tmp78A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp78A,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp78B=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp78B,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BB;_tmp5BB.tag=0U,({struct _fat_ptr _tmp78C=({const char*_tmp2FC="expecting int subscript, found ";_tag_fat(_tmp2FC,sizeof(char),32U);});_tmp5BB.f1=_tmp78C;});_tmp5BB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BA;_tmp5BA.tag=2U,_tmp5BA.f1=(void*)t2;_tmp5BA;});void*_tmp2F9[2U];_tmp2F9[0]=& _tmp2FA,_tmp2F9[1]=& _tmp2FB;({struct Cyc_Tcenv_Tenv*_tmp78F=te;unsigned _tmp78E=e2->loc;void**_tmp78D=topt;Cyc_Tcexp_expr_err(_tmp78F,_tmp78E,_tmp78D,_tag_fat(_tmp2F9,sizeof(void*),2U));});});{
# 1442
void*_tmp2FD=t1;struct Cyc_List_List*_tmp2FE;void*_tmp303;void*_tmp302;void*_tmp301;struct Cyc_Absyn_Tqual _tmp300;void*_tmp2FF;switch(*((int*)_tmp2FD)){case 3U: _LL1: _tmp2FF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_type;_tmp300=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_tq;_tmp301=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).rgn;_tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).bounds;_tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp2FF;struct Cyc_Absyn_Tqual tq=_tmp300;void*rt=_tmp301;void*b=_tmp302;void*zt=_tmp303;
# 1446
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp304=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp304;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp305=eopt;struct Cyc_Absyn_Exp*e3=_tmp305;
struct _tuple16 _tmp306=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp2E=_tmp306;struct _tuple16 _tmp307=_stmttmp2E;int _tmp309;unsigned _tmp308;_LL8: _tmp308=_tmp307.f1;_tmp309=_tmp307.f2;_LL9: {unsigned i=_tmp308;int known_i=_tmp309;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp30A=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp2F=_tmp30A;struct _tuple16 _tmp30B=_stmttmp2F;int _tmp30D;unsigned _tmp30C;_LLB: _tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;_LLC: {unsigned j=_tmp30C;int known_j=_tmp30D;
if(known_j){
struct _tuple16 _tmp30E=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp30=_tmp30E;struct _tuple16 _tmp30F=_stmttmp30;int _tmp311;unsigned _tmp310;_LLE: _tmp310=_tmp30F.f1;_tmp311=_tmp30F.f2;_LLF: {unsigned j=_tmp310;int knownj=_tmp311;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp312=0U;({unsigned _tmp791=loc;struct _fat_ptr _tmp790=({const char*_tmp313="subscript will fail at run-time";_tag_fat(_tmp313,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp791,_tmp790,_tag_fat(_tmp312,sizeof(void*),0U));});});}}}}}}
# 1462
if(emit_warning)
({void*_tmp314=0U;({unsigned _tmp793=e2->loc;struct _fat_ptr _tmp792=({const char*_tmp315="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp315,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp793,_tmp792,_tag_fat(_tmp314,sizeof(void*),0U));});});}else{
# 1466
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp316=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp31=_tmp316;struct _tuple16 _tmp317=_stmttmp31;int _tmp319;unsigned _tmp318;_LL11: _tmp318=_tmp317.f1;_tmp319=_tmp317.f2;_LL12: {unsigned i=_tmp318;int known=_tmp319;
if(known)
# 1471
({unsigned _tmp796=loc;unsigned _tmp795=i;void*_tmp794=b;Cyc_Tcutil_check_bound(_tmp796,_tmp795,_tmp794,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1474
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp31A=0U;({unsigned _tmp798=e1->loc;struct _fat_ptr _tmp797=({const char*_tmp31B="subscript applied to pointer to one object";_tag_fat(_tmp31B,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp798,_tmp797,_tag_fat(_tmp31A,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1479
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp31C=0U;({unsigned _tmp79A=e1->loc;struct _fat_ptr _tmp799=({const char*_tmp31D="can't subscript an abstract pointer";_tag_fat(_tmp31D,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp79A,_tmp799,_tag_fat(_tmp31C,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp2FE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FD)->f1;_LL4: {struct Cyc_List_List*ts=_tmp2FE;
return Cyc_Tcexp_ithTupleType(te,loc,ts,e2);}default: _LL5: _LL6:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp31F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5BD;_tmp5BD.tag=0U,({struct _fat_ptr _tmp79B=({const char*_tmp321="subscript applied to ";_tag_fat(_tmp321,sizeof(char),22U);});_tmp5BD.f1=_tmp79B;});_tmp5BD;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp320=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5BC;_tmp5BC.tag=2U,_tmp5BC.f1=(void*)t1;_tmp5BC;});void*_tmp31E[2U];_tmp31E[0]=& _tmp31F,_tmp31E[1]=& _tmp320;({struct Cyc_Tcenv_Tenv*_tmp79E=te;unsigned _tmp79D=loc;void**_tmp79C=topt;Cyc_Tcexp_expr_err(_tmp79E,_tmp79D,_tmp79C,_tag_fat(_tmp31E,sizeof(void*),2U));});});}_LL0:;}}}
# 1489
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp322=Cyc_Tcutil_compress(*topt);void*_stmttmp32=_tmp322;void*_tmp323=_stmttmp32;struct Cyc_List_List*_tmp324;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp323)->tag == 6U){_LL1: _tmp324=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp323)->f1;_LL2: {struct Cyc_List_List*ts=_tmp324;
# 1496
if(({int _tmp79F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp79F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
goto _LL0;
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp325=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp325;
({struct Cyc_Tcenv_Tenv*_tmp7A0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7A0,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1503
({struct Cyc_RgnOrder_RgnPO*_tmp7A2=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7A1=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp7A2,_tmp7A1,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp327=_cycalloc(sizeof(*_tmp327));({struct _tuple17*_tmp7A3=({struct _tuple17*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp326->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp326;});_tmp327->hd=_tmp7A3;}),_tmp327->tl=fields;_tmp327;});}
# 1506
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1510
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp7A4=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp7A4,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp329=_cycalloc(sizeof(*_tmp329));({struct _tuple17*_tmp7A6=({struct _tuple17*_tmp328=_cycalloc(sizeof(*_tmp328));({struct Cyc_Absyn_Tqual _tmp7A5=Cyc_Absyn_empty_tqual(0U);_tmp328->f1=_tmp7A5;}),_tmp328->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp328;});_tmp329->hd=_tmp7A6;}),_tmp329->tl=fields;_tmp329;});}
# 1515
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->tag=6U,({struct Cyc_List_List*_tmp7A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp32A->f1=_tmp7A7;});_tmp32A;});}
# 1519
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1524
struct _tuple9*_tmp32B=targ;void*_tmp32C;_LL1: _tmp32C=_tmp32B->f3;_LL2: {void*t=_tmp32C;
({unsigned _tmp7AB=loc;struct Cyc_Tcenv_Tenv*_tmp7AA=te;struct Cyc_List_List*_tmp7A9=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp7A8=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1525
Cyc_Tctyp_check_type(_tmp7AB,_tmp7AA,_tmp7A9,_tmp7A8,1,0,t);});
# 1527
({void*_tmp7AC=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp32D=_cycalloc(sizeof(*_tmp32D));_tmp32D->tag=36U,_tmp32D->f1=0,_tmp32D->f2=des;_tmp32D;});orig_exp->r=_tmp7AC;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1540 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1543
void*res_t2;
int _tmp32E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp32E;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32F=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345->tag=0U,({union Cyc_Absyn_Cnst _tmp7AD=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp345->f1=_tmp7AD;});_tmp345;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp32F;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1551
if(zero_term){
struct Cyc_Absyn_Exp*_tmp330=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp330;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp331=0U;({unsigned _tmp7AF=e->loc;struct _fat_ptr _tmp7AE=({const char*_tmp332="zero-terminated array doesn't end with zero.";_tag_fat(_tmp332,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp7AF,_tmp7AE,_tag_fat(_tmp331,sizeof(void*),0U));});});}
# 1556
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp7B2=res;struct Cyc_Absyn_Tqual _tmp7B1=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1557
struct Cyc_Absyn_Exp*_tmp7B0=sz_exp;Cyc_Absyn_array_type(_tmp7B2,_tmp7B1,_tmp7B0,
# 1559
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1561
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1564
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7B4=Cyc_Tcenv_curr_rgnpo(te);void*_tmp7B3=res;Cyc_Tcutil_coerce_list(_tmp7B4,_tmp7B3,es);}))
# 1566
({struct Cyc_String_pa_PrintArg_struct _tmp335=({struct Cyc_String_pa_PrintArg_struct _tmp5BE;_tmp5BE.tag=0U,({
struct _fat_ptr _tmp7B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(res));_tmp5BE.f1=_tmp7B5;});_tmp5BE;});void*_tmp333[1U];_tmp333[0]=& _tmp335;({unsigned _tmp7B7=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp7B6=({const char*_tmp334="elements of array do not all have the same type (%s)";_tag_fat(_tmp334,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp7B7,_tmp7B6,_tag_fat(_tmp333,sizeof(void*),1U));});});
# 1569
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1574
void*_tmp336=(void*)ds->hd;void*_stmttmp33=_tmp336;void*_tmp337=_stmttmp33;struct Cyc_Absyn_Exp*_tmp338;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp337)->tag == 1U){_LL1: _LL2:
# 1576
({void*_tmp339=0U;({unsigned _tmp7B9=loc;struct _fat_ptr _tmp7B8=({const char*_tmp33A="only array index designators are supported";_tag_fat(_tmp33A,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp7B9,_tmp7B8,_tag_fat(_tmp339,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp338=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp337)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp338;
# 1579
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp33B=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp34=_tmp33B;struct _tuple16 _tmp33C=_stmttmp34;int _tmp33E;unsigned _tmp33D;_LL6: _tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_LL7: {unsigned i=_tmp33D;int known=_tmp33E;
if(!known)
({void*_tmp33F=0U;({unsigned _tmp7BB=e->loc;struct _fat_ptr _tmp7BA=({const char*_tmp340="index designator cannot use sizeof or offsetof";_tag_fat(_tmp340,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp7BB,_tmp7BA,_tag_fat(_tmp33F,sizeof(void*),0U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp343=({struct Cyc_Int_pa_PrintArg_struct _tmp5C0;_tmp5C0.tag=1U,_tmp5C0.f1=(unsigned long)offset;_tmp5C0;});struct Cyc_Int_pa_PrintArg_struct _tmp344=({struct Cyc_Int_pa_PrintArg_struct _tmp5BF;_tmp5BF.tag=1U,_tmp5BF.f1=(unsigned long)((int)i);_tmp5BF;});void*_tmp341[2U];_tmp341[0]=& _tmp343,_tmp341[1]=& _tmp344;({unsigned _tmp7BD=e->loc;struct _fat_ptr _tmp7BC=({const char*_tmp342="expecting index designator %d but found %d";_tag_fat(_tmp342,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp7BD,_tmp7BC,_tag_fat(_tmp341,sizeof(void*),2U));});});}
# 1586
goto _LL0;}}}}_LL0:;}}}
# 1590
return res_t2;}
# 1594
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1597
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp346=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp35=_tmp346;void*_tmp347=_stmttmp35;void*_tmp348;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f2 != 0){_LL1: _tmp348=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp347)->f2)->hd;_LL2: {void*t=_tmp348;
# 1602
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp349=({void*_tmp7BE=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7BE,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp349;
b->topt=bound->topt;
# 1607
bound=b;}
# 1609
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1611
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp34B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C2;_tmp5C2.tag=0U,({struct _fat_ptr _tmp7BF=({const char*_tmp34D="expecting unsigned int, found ";_tag_fat(_tmp34D,sizeof(char),31U);});_tmp5C2.f1=_tmp7BF;});_tmp5C2;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp34C=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C1;_tmp5C1.tag=3U,_tmp5C1.f1=(void*)bound->topt;_tmp5C1;});void*_tmp34A[2U];_tmp34A[0]=& _tmp34B,_tmp34A[1]=& _tmp34C;({unsigned _tmp7C0=bound->loc;Cyc_Warn_err2(_tmp7C0,_tag_fat(_tmp34A,sizeof(void*),2U));});});}_LL0:;}
# 1615
if(!(vd->tq).real_const)
({void*_tmp34E=0U;({struct _fat_ptr _tmp7C1=({const char*_tmp34F="comprehension index variable is not declared const!";_tag_fat(_tmp34F,sizeof(char),52U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp7C1,_tag_fat(_tmp34E,sizeof(void*),0U));});});
# 1618
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
void**_tmp350=0;void**topt2=_tmp350;
struct Cyc_Absyn_Tqual*_tmp351=0;struct Cyc_Absyn_Tqual*tqopt=_tmp351;
void**_tmp352=0;void**ztopt=_tmp352;
# 1624
if(topt != 0){
void*_tmp353=Cyc_Tcutil_compress(*topt);void*_stmttmp36=_tmp353;void*_tmp354=_stmttmp36;void*_tmp358;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Tqual _tmp356;void*_tmp355;struct Cyc_Absyn_PtrInfo _tmp359;switch(*((int*)_tmp354)){case 3U: _LL6: _tmp359=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp354)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp359;
# 1627
topt2=({void**_tmp35A=_cycalloc(sizeof(*_tmp35A));*_tmp35A=x.elt_type;_tmp35A;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp35B=_cycalloc(sizeof(*_tmp35B));*_tmp35B=x.elt_tq;_tmp35B;});
ztopt=({void**_tmp35C=_cycalloc(sizeof(*_tmp35C));*_tmp35C=(x.ptr_atts).zero_term;_tmp35C;});
goto _LL5;}case 4U: _LL8: _tmp355=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).elt_type;_tmp356=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).tq;_tmp357=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).num_elts;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354)->f1).zero_term;_LL9: {void*t=_tmp355;struct Cyc_Absyn_Tqual tq=_tmp356;struct Cyc_Absyn_Exp*b=_tmp357;void*zt=_tmp358;
# 1632
topt2=({void**_tmp35D=_cycalloc(sizeof(*_tmp35D));*_tmp35D=t;_tmp35D;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp35E=_cycalloc(sizeof(*_tmp35E));*_tmp35E=tq;_tmp35E;});
ztopt=({void**_tmp35F=_cycalloc(sizeof(*_tmp35F));*_tmp35F=zt;_tmp35F;});
goto _LL5;}default: _LLA: _LLB:
# 1637
 goto _LL5;}_LL5:;}{
# 1640
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1642
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);
if(te->le == 0){
# 1646
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp360=0U;({unsigned _tmp7C3=bound->loc;struct _fat_ptr _tmp7C2=({const char*_tmp361="bound is not constant";_tag_fat(_tmp361,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp7C3,_tmp7C2,_tag_fat(_tmp360,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp362=0U;({unsigned _tmp7C5=bound->loc;struct _fat_ptr _tmp7C4=({const char*_tmp363="body is not constant";_tag_fat(_tmp363,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp7C5,_tmp7C4,_tag_fat(_tmp362,sizeof(void*),0U));});});}
# 1651
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1654
struct Cyc_Absyn_Exp*_tmp364=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp364;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1658
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt))&& !
Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);{
# 1662
void*_tmp365=({void*_tmp7C8=t;struct Cyc_Absyn_Tqual _tmp7C7=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp7C6=bound;Cyc_Absyn_array_type(_tmp7C8,_tmp7C7,_tmp7C6,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1662
void*res=_tmp365;
# 1664
return res;}}}}
# 1668
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1671
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp366=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp366;void*_tmp367=_stmttmp37;void*_tmp368;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp367)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp367)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp367)->f2 != 0){_LL1: _tmp368=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp367)->f2)->hd;_LL2: {void*t=_tmp368;
# 1676
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp369=({void*_tmp7C9=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7C9,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp369;
b->topt=bound->topt;
bound=b;}
# 1681
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1683
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C4;_tmp5C4.tag=0U,({struct _fat_ptr _tmp7CA=({const char*_tmp36D="expecting unsigned int, found ";_tag_fat(_tmp36D,sizeof(char),31U);});_tmp5C4.f1=_tmp7CA;});_tmp5C4;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp36C=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C3;_tmp5C3.tag=3U,_tmp5C3.f1=(void*)bound->topt;_tmp5C3;});void*_tmp36A[2U];_tmp36A[0]=& _tmp36B,_tmp36A[1]=& _tmp36C;({unsigned _tmp7CB=bound->loc;Cyc_Warn_err2(_tmp7CB,_tag_fat(_tmp36A,sizeof(void*),2U));});});}_LL0:;}{
# 1687
void**_tmp36E=0;void**topt2=_tmp36E;
struct Cyc_Absyn_Tqual*_tmp36F=0;struct Cyc_Absyn_Tqual*tqopt=_tmp36F;
void**_tmp370=0;void**ztopt=_tmp370;
# 1691
if(topt != 0){
void*_tmp371=Cyc_Tcutil_compress(*topt);void*_stmttmp38=_tmp371;void*_tmp372=_stmttmp38;void*_tmp376;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Tqual _tmp374;void*_tmp373;struct Cyc_Absyn_PtrInfo _tmp377;switch(*((int*)_tmp372)){case 3U: _LL6: _tmp377=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp372)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp377;
# 1694
topt2=({void**_tmp378=_cycalloc(sizeof(*_tmp378));*_tmp378=x.elt_type;_tmp378;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp379=_cycalloc(sizeof(*_tmp379));*_tmp379=x.elt_tq;_tmp379;});
ztopt=({void**_tmp37A=_cycalloc(sizeof(*_tmp37A));*_tmp37A=(x.ptr_atts).zero_term;_tmp37A;});
goto _LL5;}case 4U: _LL8: _tmp373=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp372)->f1).elt_type;_tmp374=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp372)->f1).tq;_tmp375=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp372)->f1).num_elts;_tmp376=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp372)->f1).zero_term;_LL9: {void*t=_tmp373;struct Cyc_Absyn_Tqual tq=_tmp374;struct Cyc_Absyn_Exp*b=_tmp375;void*zt=_tmp376;
# 1699
topt2=({void**_tmp37B=_cycalloc(sizeof(*_tmp37B));*_tmp37B=t;_tmp37B;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp37C=_cycalloc(sizeof(*_tmp37C));*_tmp37C=tq;_tmp37C;});
ztopt=({void**_tmp37D=_cycalloc(sizeof(*_tmp37D));*_tmp37D=zt;_tmp37D;});
goto _LL5;}default: _LLA: _LLB:
# 1704
 goto _LL5;}_LL5:;}
# 1707
({unsigned _tmp7CE=loc;struct Cyc_Tcenv_Tenv*_tmp7CD=te;struct Cyc_List_List*_tmp7CC=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp7CE,_tmp7CD,_tmp7CC,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)Cyc_Unify_unify(*topt2,t);
# 1710
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp37E=0U;({unsigned _tmp7D0=bound->loc;struct _fat_ptr _tmp7CF=({const char*_tmp37F="bound is not constant";_tag_fat(_tmp37F,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp7D0,_tmp7CF,_tag_fat(_tmp37E,sizeof(void*),0U));});});}
# 1714
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1717
struct Cyc_Absyn_Exp*_tmp380=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp380;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1721
({void*_tmp381=0U;({unsigned _tmp7D2=loc;struct _fat_ptr _tmp7D1=({const char*_tmp382="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp382,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp7D2,_tmp7D1,_tag_fat(_tmp381,sizeof(void*),0U));});});}{
# 1724
void*_tmp383=({void*_tmp7D5=t;struct Cyc_Absyn_Tqual _tmp7D4=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp7D3=bound;Cyc_Absyn_array_type(_tmp7D5,_tmp7D4,_tmp7D3,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1724
void*res=_tmp383;
# 1726
return res;}}}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1739 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1743
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp384=_cycalloc(sizeof(*_tmp384));*_tmp384=ad;_tmp384;});}else{
# 1749
{struct _handler_cons _tmp385;_push_handler(& _tmp385);{int _tmp387=0;if(setjmp(_tmp385.handler))_tmp387=1;if(!_tmp387){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1749
;_pop_handler();}else{void*_tmp386=(void*)Cyc_Core_get_exn_thrown();void*_tmp388=_tmp386;void*_tmp389;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp388)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1752
({struct Cyc_String_pa_PrintArg_struct _tmp38C=({struct Cyc_String_pa_PrintArg_struct _tmp5C5;_tmp5C5.tag=0U,({struct _fat_ptr _tmp7D6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp5C5.f1=_tmp7D6;});_tmp5C5;});void*_tmp38A[1U];_tmp38A[0]=& _tmp38C;({unsigned _tmp7D8=loc;struct _fat_ptr _tmp7D7=({const char*_tmp38B="unbound struct/union name %s";_tag_fat(_tmp38B,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp7D8,_tmp7D7,_tag_fat(_tmp38A,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp389=_tmp388;_LL4: {void*exn=_tmp389;(int)_rethrow(exn);}}_LL0:;}}}
# 1755
*ad_opt=ad;
*tn=ad->name;}
# 1758
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp38F=({struct Cyc_String_pa_PrintArg_struct _tmp5C6;_tmp5C6.tag=0U,({struct _fat_ptr _tmp7D9=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp390="struct";_tag_fat(_tmp390,sizeof(char),7U);}):({const char*_tmp391="union";_tag_fat(_tmp391,sizeof(char),6U);}));_tmp5C6.f1=_tmp7D9;});_tmp5C6;});void*_tmp38D[1U];_tmp38D[0]=& _tmp38F;({unsigned _tmp7DB=loc;struct _fat_ptr _tmp7DA=({const char*_tmp38E="can't construct abstract %s";_tag_fat(_tmp38E,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp7DB,_tmp7DA,_tag_fat(_tmp38D,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1766
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp392=Cyc_Tcenv_new_status(te);enum Cyc_Tcenv_NewStatus status=_tmp392;
if((int)status == (int)1U)
te2=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);else{
# 1775
te2=Cyc_Tcenv_set_new_status(status,te);}{
# 1777
struct _tuple13 _tmp393=({struct _tuple13 _tmp5CF;({struct Cyc_List_List*_tmp7DC=Cyc_Tcenv_lookup_type_vars(te2);_tmp5CF.f1=_tmp7DC;}),_tmp5CF.f2=Cyc_Core_heap_region;_tmp5CF;});struct _tuple13 env=_tmp393;
struct Cyc_List_List*_tmp394=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp394;
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp395;
struct Cyc_List_List*_tmp396=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp396;
struct Cyc_List_List*_tmp397=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp397;
struct Cyc_List_List*_tmp398=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp398;
void*res_typ;
# 1788
if(topt != 0){
void*_tmp399=Cyc_Tcutil_compress(*topt);void*_stmttmp39=_tmp399;void*_tmp39A=_stmttmp39;struct Cyc_List_List*_tmp39C;struct Cyc_Absyn_Aggrdecl**_tmp39B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39A)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39A)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp39B=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39A)->f1)->f1).KnownAggr).val;_tmp39C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39A)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp39B;struct Cyc_List_List*all_typs2=_tmp39C;
# 1791
if(*adptr2 == *adptr){
{struct Cyc_List_List*_tmp39D=all_typs;struct Cyc_List_List*ats=_tmp39D;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1796
res_typ=*topt;
goto _LL5;}
# 1799
goto _LL9;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1801
 res_typ=({union Cyc_Absyn_AggrInfo _tmp7DD=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp7DD,all_typs);});}_LL5:;}else{
# 1804
res_typ=({union Cyc_Absyn_AggrInfo _tmp7DE=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp7DE,all_typs);});}{
# 1807
struct Cyc_List_List*_tmp39E=*ts;struct Cyc_List_List*user_ex_ts=_tmp39E;
struct Cyc_List_List*_tmp39F=exist_typs;struct Cyc_List_List*ex_ts=_tmp39F;
while(user_ex_ts != 0 && ex_ts != 0){
# 1811
({unsigned _tmp7E1=loc;struct Cyc_Tcenv_Tenv*_tmp7E0=te2;struct Cyc_List_List*_tmp7DF=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp7E1,_tmp7E0,_tmp7DF,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1817
if(user_ex_ts != 0)
({void*_tmp3A0=0U;({unsigned _tmp7E3=loc;struct _fat_ptr _tmp7E2=({const char*_tmp3A1="too many explicit witness types";_tag_fat(_tmp3A1,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7E3,_tmp7E2,_tag_fat(_tmp3A0,sizeof(void*),0U));});});
# 1820
*ts=exist_typs;{
# 1823
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3A2=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3A=_tmp3A2;struct _tuple19*_tmp3A3=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Aggrfield*_tmp3A4;_LLB: _tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A3->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3A4;struct Cyc_Absyn_Exp*field_exp=_tmp3A5;
void*_tmp3A6=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3A6;
({struct Cyc_Tcenv_Tenv*_tmp7E4=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7E4,& inst_fieldtyp,field_exp);});
# 1831
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7E6=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp7E5=field_exp;Cyc_Tcutil_coerce_arg(_tmp7E6,_tmp7E5,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=0U,({struct _fat_ptr _tmp7E7=({const char*_tmp3B3="field ";_tag_fat(_tmp3B3,sizeof(char),7U);});_tmp5CE.f1=_tmp7E7;});_tmp5CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=0U,_tmp5CD.f1=*field->name;_tmp5CD;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=0U,({struct _fat_ptr _tmp7E8=({const char*_tmp3B2=" of ";_tag_fat(_tmp3B2,sizeof(char),5U);});_tmp5CC.f1=_tmp7E8;});_tmp5CC;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3AB=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=6U,_tmp5CB.f1=ad;_tmp5CB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=0U,({
struct _fat_ptr _tmp7E9=({const char*_tmp3B1=" expects type ";_tag_fat(_tmp3B1,sizeof(char),15U);});_tmp5CA.f1=_tmp7E9;});_tmp5CA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3AD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=2U,_tmp5C9.f1=(void*)inst_fieldtyp;_tmp5C9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C8;_tmp5C8.tag=0U,({struct _fat_ptr _tmp7EA=({const char*_tmp3B0=" != ";_tag_fat(_tmp3B0,sizeof(char),5U);});_tmp5C8.f1=_tmp7EA;});_tmp5C8;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3AF=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5C7;_tmp5C7.tag=3U,_tmp5C7.f1=(void*)field_exp->topt;_tmp5C7;});void*_tmp3A7[8U];_tmp3A7[0]=& _tmp3A8,_tmp3A7[1]=& _tmp3A9,_tmp3A7[2]=& _tmp3AA,_tmp3A7[3]=& _tmp3AB,_tmp3A7[4]=& _tmp3AC,_tmp3A7[5]=& _tmp3AD,_tmp3A7[6]=& _tmp3AE,_tmp3A7[7]=& _tmp3AF;({unsigned _tmp7EB=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp7EB,_tag_fat(_tmp3A7,sizeof(void*),8U));});});}}{
# 1836
struct Cyc_List_List*_tmp3B4=0;struct Cyc_List_List*rpo_inst=_tmp3B4;
{struct Cyc_List_List*_tmp3B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3B5;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct _tuple0*_tmp7EE=({struct _tuple0*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));({void*_tmp7ED=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3B6->f1=_tmp7ED;}),({
void*_tmp7EC=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3B6->f2=_tmp7EC;});_tmp3B6;});
# 1838
_tmp3B7->hd=_tmp7EE;}),_tmp3B7->tl=rpo_inst;_tmp3B7;});}}
# 1841
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}}
# 1847
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1849
{void*_tmp3B8=Cyc_Tcutil_compress(ts);void*_stmttmp3B=_tmp3B8;void*_tmp3B9=_stmttmp3B;struct Cyc_List_List*_tmp3BB;enum Cyc_Absyn_AggrKind _tmp3BA;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B9)->tag == 7U){_LL1: _tmp3BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B9)->f1;_tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B9)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3BA;struct Cyc_List_List*fs=_tmp3BB;
# 1851
if((int)k == (int)1U)
({void*_tmp3BC=0U;({unsigned _tmp7F0=loc;struct _fat_ptr _tmp7EF=({const char*_tmp3BD="expecting struct but found union";_tag_fat(_tmp3BD,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7F0,_tmp7EF,_tag_fat(_tmp3BC,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3BE=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3C=_tmp3BE;struct _tuple19*_tmp3BF=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Aggrfield*_tmp3C0;_LL6: _tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3C0;struct Cyc_Absyn_Exp*field_exp=_tmp3C1;
({struct Cyc_Tcenv_Tenv*_tmp7F2=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp7F1=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp7F2,_tmp7F1,field_exp);});
# 1860
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7F4=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7F3=field_exp;Cyc_Tcutil_coerce_arg(_tmp7F4,_tmp7F3,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0U,({struct _fat_ptr _tmp7F5=({const char*_tmp3CB="field ";_tag_fat(_tmp3CB,sizeof(char),7U);});_tmp5D5.f1=_tmp7F5;});_tmp5D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0U,_tmp5D4.f1=*field->name;_tmp5D4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0U,({
struct _fat_ptr _tmp7F6=({const char*_tmp3CA=" of struct expects type ";_tag_fat(_tmp3CA,sizeof(char),25U);});_tmp5D3.f1=_tmp7F6;});_tmp5D3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3C6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=2U,_tmp5D2.f1=(void*)field->type;_tmp5D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D1;_tmp5D1.tag=0U,({struct _fat_ptr _tmp7F7=({const char*_tmp3C9=" != ";_tag_fat(_tmp3C9,sizeof(char),5U);});_tmp5D1.f1=_tmp7F7;});_tmp5D1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3C8=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5D0;_tmp5D0.tag=3U,_tmp5D0.f1=(void*)field_exp->topt;_tmp5D0;});void*_tmp3C2[6U];_tmp3C2[0]=& _tmp3C3,_tmp3C2[1]=& _tmp3C4,_tmp3C2[2]=& _tmp3C5,_tmp3C2[3]=& _tmp3C6,_tmp3C2[4]=& _tmp3C7,_tmp3C2[5]=& _tmp3C8;({unsigned _tmp7F8=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp7F8,_tag_fat(_tmp3C2,sizeof(void*),6U));});});}}
# 1865
goto _LL0;}}}else{_LL3: _LL4:
({void*_tmp3CC=0U;({struct _fat_ptr _tmp7F9=({const char*_tmp3CD="tcAnonStruct: wrong type";_tag_fat(_tmp3CD,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp7F9,_tag_fat(_tmp3CC,sizeof(void*),0U));});});}_LL0:;}
# 1868
return ts;}
# 1872
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1875
struct _tuple13 _tmp3CE=({struct _tuple13 _tmp5DD;({struct Cyc_List_List*_tmp7FA=Cyc_Tcenv_lookup_type_vars(te);_tmp5DD.f1=_tmp7FA;}),_tmp5DD.f2=Cyc_Core_heap_region;_tmp5DD;});struct _tuple13 env=_tmp3CE;
struct Cyc_List_List*_tmp3CF=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3CF;
struct Cyc_List_List*_tmp3D0=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3D0;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp7FB=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp7FB,all_typs);});
# 1880
if(topt != 0){
void*_tmp3D1=Cyc_Tcutil_compress(*topt);void*_stmttmp3D=_tmp3D1;void*_tmp3D2=_stmttmp3D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D2)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1886
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(inst != 0)t=Cyc_Tcutil_substitute(inst,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp7FD=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp7FC=e;Cyc_Tcutil_coerce_arg(_tmp7FD,_tmp7FC,t,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp3D5=({struct Cyc_String_pa_PrintArg_struct _tmp5D8;_tmp5D8.tag=0U,({
# 1896
struct _fat_ptr _tmp7FE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp5D8.f1=_tmp7FE;});_tmp5D8;});struct Cyc_String_pa_PrintArg_struct _tmp3D6=({struct Cyc_String_pa_PrintArg_struct _tmp5D7;_tmp5D7.tag=0U,({struct _fat_ptr _tmp7FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5D7.f1=_tmp7FF;});_tmp5D7;});struct Cyc_String_pa_PrintArg_struct _tmp3D7=({struct Cyc_String_pa_PrintArg_struct _tmp5D6;_tmp5D6.tag=0U,({
struct _fat_ptr _tmp800=(struct _fat_ptr)((struct _fat_ptr)(e->topt == 0?(struct _fat_ptr)({const char*_tmp3D8="?";_tag_fat(_tmp3D8,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp5D6.f1=_tmp800;});_tmp5D6;});void*_tmp3D3[3U];_tmp3D3[0]=& _tmp3D5,_tmp3D3[1]=& _tmp3D6,_tmp3D3[2]=& _tmp3D7;({unsigned _tmp802=e->loc;struct _fat_ptr _tmp801=({const char*_tmp3D4="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_fat(_tmp3D4,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp802,_tmp801,_tag_fat(_tmp3D3,sizeof(void*),3U));});});
Cyc_Unify_explain_failure();}}
# 1901
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=0U,({
struct _fat_ptr _tmp803=({const char*_tmp3DC="too many arguments for datatype constructor ";_tag_fat(_tmp3DC,sizeof(char),45U);});_tmp5DA.f1=_tmp803;});_tmp5DA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5D9;_tmp5D9.tag=1U,_tmp5D9.f1=tuf->name;_tmp5D9;});void*_tmp3D9[2U];_tmp3D9[0]=& _tmp3DA,_tmp3D9[1]=& _tmp3DB;({struct Cyc_Tcenv_Tenv*_tmp806=te;unsigned _tmp805=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp804=topt;Cyc_Tcexp_expr_err(_tmp806,_tmp805,_tmp804,_tag_fat(_tmp3D9,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=0U,({
struct _fat_ptr _tmp807=({const char*_tmp3E0="too few arguments for datatype constructor ";_tag_fat(_tmp3E0,sizeof(char),44U);});_tmp5DC.f1=_tmp807;});_tmp5DC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp3DF=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=1U,_tmp5DB.f1=tuf->name;_tmp5DB;});void*_tmp3DD[2U];_tmp3DD[0]=& _tmp3DE,_tmp3DD[1]=& _tmp3DF;({struct Cyc_Tcenv_Tenv*_tmp80A=te;unsigned _tmp809=loc;void**_tmp808=topt;Cyc_Tcexp_expr_err(_tmp80A,_tmp809,_tmp808,_tag_fat(_tmp3DD,sizeof(void*),2U));});});
return res;}}
# 1911
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1913
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1916
if(topt == 0)
return 0;{
void*_tmp3E1=Cyc_Tcutil_compress(*topt);void*_stmttmp3E=_tmp3E1;void*_tmp3E2=_stmttmp3E;void*_tmp3E3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->tag == 3U){_LL1: _tmp3E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).elt_type;_LL2: {void*elt_typ=_tmp3E3;
# 1920
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1926
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1928
enum Cyc_Absyn_AliasQual _tmp3E4=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp3F=_tmp3E4;enum Cyc_Absyn_AliasQual _tmp3E5=_stmttmp3F;if(_tmp3E5 == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 1934
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 1938
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 1942
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3E6=Cyc_Tcutil_compress(handle_type);void*_stmttmp40=_tmp3E6;void*_tmp3E7=_stmttmp40;void*_tmp3E8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E7)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E7)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E7)->f2 != 0){_LL1: _tmp3E8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E7)->f2)->hd;_LL2: {void*r=_tmp3E8;
# 1947
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1951
({struct Cyc_String_pa_PrintArg_struct _tmp3EB=({struct Cyc_String_pa_PrintArg_struct _tmp5DE;_tmp5DE.tag=0U,({
struct _fat_ptr _tmp80B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp5DE.f1=_tmp80B;});_tmp5DE;});void*_tmp3E9[1U];_tmp3E9[0]=& _tmp3EB;({unsigned _tmp80D=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _fat_ptr _tmp80C=({const char*_tmp3EA="expecting region_t type but found %s";_tag_fat(_tmp3EA,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp80D,_tmp80C,_tag_fat(_tmp3E9,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 1958
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp80E=Cyc_Absyn_uniquergn_exp();*ropt=_tmp80E;});}}}
# 1965
({struct Cyc_Tcenv_Tenv*_tmp80F=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp80F,& Cyc_Absyn_uint_type,*e);});{
# 1974 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3EC=0U;({struct _fat_ptr _tmp810=({const char*_tmp3ED="calloc with empty type";_tag_fat(_tmp3ED,sizeof(char),23U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp810,_tag_fat(_tmp3EC,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp813=loc;struct Cyc_Tcenv_Tenv*_tmp812=te;struct Cyc_List_List*_tmp811=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp813,_tmp812,_tmp811,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp3F0=({struct Cyc_String_pa_PrintArg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp814=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(elt_type));_tmp5DF.f1=_tmp814;});_tmp5DF;});void*_tmp3EE[1U];_tmp3EE[0]=& _tmp3F0;({unsigned _tmp816=loc;struct _fat_ptr _tmp815=({const char*_tmp3EF="calloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3EF,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp816,_tmp815,_tag_fat(_tmp3EE,sizeof(void*),1U));});});
num_elts=*e;
one_elt=0;}else{
# 1987
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3F1=er;struct Cyc_Absyn_Exp*_tmp3F3;struct Cyc_Absyn_Exp*_tmp3F2;void*_tmp3F4;switch(*((int*)_tmp3F1)){case 17U: _LL6: _tmp3F4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F1)->f1;_LL7: {void*t2=_tmp3F4;
# 1991
elt_type=t2;
({void**_tmp817=({void**_tmp3F5=_cycalloc(sizeof(*_tmp3F5));*_tmp3F5=elt_type;_tmp3F5;});*t=_tmp817;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F1)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F1)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F1)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F1)->f2)->tl)->tl == 0){_LL8: _tmp3F2=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F1)->f2)->hd;_tmp3F3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F1)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp3F2;struct Cyc_Absyn_Exp*e2=_tmp3F3;
# 1998
{struct _tuple0 _tmp3F6=({struct _tuple0 _tmp5E2;_tmp5E2.f1=e1->r,_tmp5E2.f2=e2->r;_tmp5E2;});struct _tuple0 _stmttmp41=_tmp3F6;struct _tuple0 _tmp3F7=_stmttmp41;void*_tmp3F8;void*_tmp3F9;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F7.f1)->tag == 17U){_LLD: _tmp3F9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F7.f1)->f1;_LLE: {void*t1=_tmp3F9;
# 2001
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t1)){
# 2004
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t1))
({struct Cyc_String_pa_PrintArg_struct _tmp3FC=({struct Cyc_String_pa_PrintArg_struct _tmp5E0;_tmp5E0.tag=0U,({struct _fat_ptr _tmp818=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp5E0.f1=_tmp818;});_tmp5E0;});void*_tmp3FA[1U];_tmp3FA[0]=& _tmp3FC;({unsigned _tmp81A=loc;struct _fat_ptr _tmp819=({const char*_tmp3FB="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3FB,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp81A,_tmp819,_tag_fat(_tmp3FA,sizeof(void*),1U));});});else{
# 2007
*is_calloc=1;}}
# 2010
elt_type=t1;
({void**_tmp81B=({void**_tmp3FD=_cycalloc(sizeof(*_tmp3FD));*_tmp3FD=elt_type;_tmp3FD;});*t=_tmp81B;});
num_elts=e2;
one_elt=0;
goto _LLC;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F7.f2)->tag == 17U){_LLF: _tmp3F8=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F7.f2)->f1;_LL10: {void*t2=_tmp3F8;
# 2017
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 2020
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp400=({struct Cyc_String_pa_PrintArg_struct _tmp5E1;_tmp5E1.tag=0U,({struct _fat_ptr _tmp81C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp5E1.f1=_tmp81C;});_tmp5E1;});void*_tmp3FE[1U];_tmp3FE[0]=& _tmp400;({unsigned _tmp81E=loc;struct _fat_ptr _tmp81D=({const char*_tmp3FF="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp3FF,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp81E,_tmp81D,_tag_fat(_tmp3FE,sizeof(void*),1U));});});else{
# 2023
*is_calloc=1;}}
# 2026
elt_type=t2;
({void**_tmp81F=({void**_tmp401=_cycalloc(sizeof(*_tmp401));*_tmp401=elt_type;_tmp401;});*t=_tmp81F;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2033
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2035
 No_sizeof: {
# 2038
struct Cyc_Absyn_Exp*_tmp402=*e;struct Cyc_Absyn_Exp*real_e=_tmp402;
{void*_tmp403=real_e->r;void*_stmttmp42=_tmp403;void*_tmp404=_stmttmp42;struct Cyc_Absyn_Exp*_tmp405;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp404)->tag == 14U){_LL14: _tmp405=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp404)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp405;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2043
{void*_tmp406=Cyc_Tcutil_compress((void*)_check_null(real_e->topt));void*_stmttmp43=_tmp406;void*_tmp407=_stmttmp43;void*_tmp408;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp407)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp407)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp407)->f2 != 0){_LL19: _tmp408=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp407)->f2)->hd;_LL1A: {void*tagt=_tmp408;
# 2045
{void*_tmp409=Cyc_Tcutil_compress(tagt);void*_stmttmp44=_tmp409;void*_tmp40A=_stmttmp44;struct Cyc_Absyn_Exp*_tmp40B;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40A)->tag == 9U){_LL1E: _tmp40B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40A)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp40B;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2049
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2052
elt_type=Cyc_Absyn_char_type;
({void**_tmp820=({void**_tmp40C=_cycalloc(sizeof(*_tmp40C));*_tmp40C=elt_type;_tmp40C;});*t=_tmp820;});
num_elts=*e;
one_elt=0;}
# 2057
goto _LL5;}_LL5:;}}
# 2061
*is_fat=!one_elt;
# 2064
{void*_tmp40D=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp40E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp40E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40D)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp40E;
# 2066
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({void*_tmp40F=0U;({unsigned _tmp822=loc;struct _fat_ptr _tmp821=({const char*_tmp410="malloc with existential types not yet implemented";_tag_fat(_tmp410,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp822,_tmp821,_tag_fat(_tmp40F,sizeof(void*),0U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2073
void*(*_tmp411)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp411;
void*_tmp412=Cyc_Absyn_false_type;void*zero_term=_tmp412;
if(topt != 0){
void*_tmp413=Cyc_Tcutil_compress(*topt);void*_stmttmp45=_tmp413;void*_tmp414=_stmttmp45;void*_tmp417;void*_tmp416;void*_tmp415;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp414)->tag == 3U){_LL28: _tmp415=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp414)->f1).ptr_atts).nullable;_tmp416=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp414)->f1).ptr_atts).bounds;_tmp417=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp414)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp415;void*b=_tmp416;void*zt=_tmp417;
# 2078
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2083
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({void*_tmp418=0U;({unsigned _tmp824=loc;struct _fat_ptr _tmp823=({const char*_tmp419="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp419,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp824,_tmp823,_tag_fat(_tmp418,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2089
struct Cyc_Absyn_Exp*_tmp41A=({void*_tmp825=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp825,b);});struct Cyc_Absyn_Exp*eopt=_tmp41A;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp41B=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp41B;
int _tmp41C=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp41C;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp829=elt_type;void*_tmp828=rgn;struct Cyc_Absyn_Tqual _tmp827=Cyc_Absyn_empty_tqual(0U);void*_tmp826=b;Cyc_Absyn_atb_type(_tmp829,_tmp828,_tmp827,_tmp826,zero_term);});}{
# 2097
void*_tmp41D=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_stmttmp46=_tmp41D;void*_tmp41E=_stmttmp46;void*_tmp41F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41E)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41E)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41E)->f2 != 0){_LL2D: _tmp41F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41E)->f2)->hd;_LL2E: {void*tagtyp=_tmp41F;
# 2099
struct Cyc_Absyn_Exp*_tmp420=({void*_tmp82A=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp82A,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp420;
# 2101
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp82E=elt_type;void*_tmp82D=rgn;struct Cyc_Absyn_Tqual _tmp82C=Cyc_Absyn_empty_tqual(0U);void*_tmp82B=b;Cyc_Absyn_atb_type(_tmp82E,_tmp82D,_tmp82C,_tmp82B,zero_term);});}
# 2105
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2109
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2112
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp832)(void*,void*,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp831=elt_type;void*_tmp830=rgn;struct Cyc_Absyn_Tqual _tmp82F=Cyc_Absyn_empty_tqual(0U);_tmp832(_tmp831,_tmp830,_tmp82F,zero_term);});}}}
# 2117
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2119
struct Cyc_Tcenv_Tenv*_tmp421=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp421;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp422=(void*)_check_null(e1->topt);void*t1=_tmp422;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2126
{void*_tmp423=Cyc_Tcutil_compress(t1);void*_stmttmp47=_tmp423;void*_tmp424=_stmttmp47;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp424)->tag == 4U){_LL1: _LL2:
({void*_tmp425=0U;({unsigned _tmp834=loc;struct _fat_ptr _tmp833=({const char*_tmp426="cannot assign to an array";_tag_fat(_tmp426,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp834,_tmp833,_tag_fat(_tmp425,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2131
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp427=0U;({unsigned _tmp836=loc;struct _fat_ptr _tmp835=({const char*_tmp428="Swap not allowed for non-pointer or non-word-sized types.";_tag_fat(_tmp428,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp836,_tmp835,_tag_fat(_tmp427,sizeof(void*),0U));});});
# 2135
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp42A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=0U,({struct _fat_ptr _tmp837=({const char*_tmp42B="swap non-lvalue";_tag_fat(_tmp42B,sizeof(char),16U);});_tmp5E3.f1=_tmp837;});_tmp5E3;});void*_tmp429[1U];_tmp429[0]=& _tmp42A;({struct Cyc_Tcenv_Tenv*_tmp83A=te;unsigned _tmp839=e1->loc;void**_tmp838=topt;Cyc_Tcexp_expr_err(_tmp83A,_tmp839,_tmp838,_tag_fat(_tmp429,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp42D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=0U,({struct _fat_ptr _tmp83B=({const char*_tmp42E="swap non-lvalue";_tag_fat(_tmp42E,sizeof(char),16U);});_tmp5E4.f1=_tmp83B;});_tmp5E4;});void*_tmp42C[1U];_tmp42C[0]=& _tmp42D;({struct Cyc_Tcenv_Tenv*_tmp83E=te;unsigned _tmp83D=e2->loc;void**_tmp83C=topt;Cyc_Tcexp_expr_err(_tmp83E,_tmp83D,_tmp83C,_tag_fat(_tmp42C,sizeof(void*),1U));});});
# 2140
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp430=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=0U,({struct _fat_ptr _tmp83F=({const char*_tmp435="type mismatch: ";_tag_fat(_tmp435,sizeof(char),16U);});_tmp5E8.f1=_tmp83F;});_tmp5E8;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp431=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=2U,_tmp5E7.f1=(void*)t1;_tmp5E7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp432=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=0U,({struct _fat_ptr _tmp840=({const char*_tmp434=" != ";_tag_fat(_tmp434,sizeof(char),5U);});_tmp5E6.f1=_tmp840;});_tmp5E6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp433=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=2U,_tmp5E5.f1=(void*)t2;_tmp5E5;});void*_tmp42F[4U];_tmp42F[0]=& _tmp430,_tmp42F[1]=& _tmp431,_tmp42F[2]=& _tmp432,_tmp42F[3]=& _tmp433;({struct Cyc_Tcenv_Tenv*_tmp843=te;unsigned _tmp842=loc;void**_tmp841=topt;Cyc_Tcexp_expr_err(_tmp843,_tmp842,_tmp841,_tag_fat(_tmp42F,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2148
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp844=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp844,s,1);});
# 2151
while(1){
void*_tmp436=s->r;void*_stmttmp48=_tmp436;void*_tmp437=_stmttmp48;struct Cyc_Absyn_Stmt*_tmp438;struct Cyc_Absyn_Stmt*_tmp439;struct Cyc_Absyn_Exp*_tmp43A;switch(*((int*)_tmp437)){case 1U: _LL1: _tmp43A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp437)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp43A;
# 2155
void*_tmp43B=(void*)_check_null(e->topt);void*t=_tmp43B;
if(!({void*_tmp845=t;Cyc_Unify_unify(_tmp845,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp43D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=0U,({struct _fat_ptr _tmp846=({const char*_tmp43F="statement expression returns type ";_tag_fat(_tmp43F,sizeof(char),35U);});_tmp5EA.f1=_tmp846;});_tmp5EA;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp43E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=2U,_tmp5E9.f1=(void*)t;_tmp5E9;});void*_tmp43C[2U];_tmp43C[0]=& _tmp43D,_tmp43C[1]=& _tmp43E;({unsigned _tmp847=loc;Cyc_Tcexp_err_and_explain(_tmp847,_tag_fat(_tmp43C,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp439=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp437)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp439;
s=s2;continue;}case 12U: _LL5: _tmp438=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp437)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp438;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp441=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=0U,({
struct _fat_ptr _tmp848=({const char*_tmp442="statement expression must end with expression";_tag_fat(_tmp442,sizeof(char),46U);});_tmp5EB.f1=_tmp848;});_tmp5EB;});void*_tmp440[1U];_tmp440[0]=& _tmp441;({struct Cyc_Tcenv_Tenv*_tmp84B=te;unsigned _tmp84A=loc;void**_tmp849=topt;Cyc_Tcexp_expr_err(_tmp84B,_tmp84A,_tmp849,_tag_fat(_tmp440,sizeof(void*),1U));});});}_LL0:;}}
# 2166
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp84C=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp84C,0,e);}));
{void*_tmp443=t;struct Cyc_Absyn_Aggrdecl*_tmp444;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp443)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp443)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp443)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp444=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp443)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp444;
# 2170
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2174
({struct Cyc_Warn_String_Warn_Warg_struct _tmp446=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=0U,({struct _fat_ptr _tmp84D=({const char*_tmp448="expecting @tagged union but found ";_tag_fat(_tmp448,sizeof(char),35U);});_tmp5ED.f1=_tmp84D;});_tmp5ED;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp447=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=2U,_tmp5EC.f1=(void*)t;_tmp5EC;});void*_tmp445[2U];_tmp445[0]=& _tmp446,_tmp445[1]=& _tmp447;({unsigned _tmp84E=loc;Cyc_Warn_err2(_tmp84E,_tag_fat(_tmp445,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}
# 2177
return Cyc_Absyn_uint_type;}
# 2180
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp850=te;struct Cyc_Absyn_Exp*_tmp84F=e;Cyc_Tcexp_tcTest(_tmp850,_tmp84F,({const char*_tmp449="@assert";_tag_fat(_tmp449,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({void*_tmp44A=0U;({unsigned _tmp852=loc;struct _fat_ptr _tmp851=({const char*_tmp44B="@assert expression may have side effects";_tag_fat(_tmp44B,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp852,_tmp851,_tag_fat(_tmp44A,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 2188
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2192
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp44C=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));struct Cyc_Tcenv_Tenv*te=_tmp44C;
if(*rgn_handle != 0){
# 2197
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp44D=Cyc_Tcutil_compress(handle_type);void*_stmttmp49=_tmp44D;void*_tmp44E=_stmttmp49;void*_tmp44F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->f2 != 0){_LL1: _tmp44F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->f2)->hd;_LL2: {void*r=_tmp44F;
# 2202
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2206
({struct Cyc_Warn_String_Warn_Warg_struct _tmp451=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=0U,({struct _fat_ptr _tmp853=({const char*_tmp453="expecting region_t type but found ";_tag_fat(_tmp453,sizeof(char),35U);});_tmp5EF.f1=_tmp853;});_tmp5EF;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp452=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=2U,_tmp5EE.f1=(void*)handle_type;_tmp5EE;});void*_tmp450[2U];_tmp450[0]=& _tmp451,_tmp450[1]=& _tmp452;({unsigned _tmp854=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;Cyc_Warn_err2(_tmp854,_tag_fat(_tmp450,sizeof(void*),2U));});});
# 2208
goto _LL0;}_LL0:;}else{
# 2210
if(topt != 0){
# 2213
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp855=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp855;});}}}{
# 2220
void*_tmp454=e1->r;void*_stmttmp4A=_tmp454;void*_tmp455=_stmttmp4A;struct Cyc_List_List*_tmp456;struct Cyc_List_List*_tmp458;struct Cyc_Core_Opt*_tmp457;switch(*((int*)_tmp455)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2225
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp459=0U;({struct _fat_ptr _tmp856=({const char*_tmp45A="tcNew: comprehension returned non-array type";_tag_fat(_tmp45A,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp856,_tag_fat(_tmp459,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp859=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp858=loc;void*_tmp857=res_typ;Cyc_Tcutil_silent_castable(_tmp859,_tmp858,_tmp857,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2237
return res_typ;}case 36U: _LLA: _tmp457=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp455)->f1;_tmp458=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp455)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp457;struct Cyc_List_List*des=_tmp458;
# 2239
({void*_tmp85A=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->tag=26U,_tmp45B->f1=des;_tmp45B;});e1->r=_tmp85A;});
_tmp456=des;goto _LLD;}case 26U: _LLC: _tmp456=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp455)->f1;_LLD: {struct Cyc_List_List*des=_tmp456;
# 2242
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp45C=Cyc_Tcutil_compress(*topt);void*_stmttmp4B=_tmp45C;void*_tmp45D=_stmttmp4B;void*_tmp460;struct Cyc_Absyn_Tqual _tmp45F;void**_tmp45E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45D)->tag == 3U){_LL15: _tmp45E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45D)->f1).elt_type;_tmp45F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45D)->f1).elt_tq;_tmp460=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45D)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp45E;struct Cyc_Absyn_Tqual tq=_tmp45F;void*zt=_tmp460;
# 2248
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2254
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp461=0U;({struct _fat_ptr _tmp85B=({const char*_tmp462="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp462,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp85B,_tag_fat(_tmp461,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2263
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp85E=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp85D=loc;void*_tmp85C=res_typ;Cyc_Tcutil_silent_castable(_tmp85E,_tmp85D,_tmp85C,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2270
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp455)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2275
void*_tmp463=({void*_tmp862=Cyc_Absyn_char_type;void*_tmp861=rgn;struct Cyc_Absyn_Tqual _tmp860=Cyc_Absyn_const_tqual(0U);void*_tmp85F=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp862,_tmp861,_tmp860,_tmp85F,Cyc_Absyn_true_type);});void*topt2=_tmp463;
# 2277
void*_tmp464=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp464;
return({void*_tmp866=t;void*_tmp865=rgn;struct Cyc_Absyn_Tqual _tmp864=Cyc_Absyn_empty_tqual(0U);void*_tmp863=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp866,_tmp865,_tmp864,_tmp863,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2282
void*_tmp465=({void*_tmp86A=Cyc_Absyn_wchar_type();void*_tmp869=rgn;struct Cyc_Absyn_Tqual _tmp868=Cyc_Absyn_const_tqual(0U);void*_tmp867=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp86A,_tmp869,_tmp868,_tmp867,Cyc_Absyn_true_type);});void*topt2=_tmp465;
# 2284
void*_tmp466=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp466;
return({void*_tmp86E=t;void*_tmp86D=rgn;struct Cyc_Absyn_Tqual _tmp86C=Cyc_Absyn_empty_tqual(0U);void*_tmp86B=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp86E,_tmp86D,_tmp86C,_tmp86B,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2291
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp467=Cyc_Tcutil_compress(*topt);void*_stmttmp4C=_tmp467;void*_tmp468=_stmttmp4C;struct Cyc_Absyn_Tqual _tmp46A;void**_tmp469;switch(*((int*)_tmp468)){case 3U: _LL1A: _tmp469=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).elt_type;_tmp46A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).elt_tq;_LL1B: {void**elttype=_tmp469;struct Cyc_Absyn_Tqual tq=_tmp46A;
# 2297
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp468)->f1)->tag == 18U){_LL1C: _LL1D:
# 2301
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2307
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2309
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);{
struct Cyc_Absyn_PtrInfo _tmp46B=Cyc_Tcexp_fresh_pointer_type(te);struct Cyc_Absyn_PtrInfo pi=_tmp46B;
pi.elt_type=telt;
(pi.ptr_atts).rgn=rgn;
({void*_tmp86F=Cyc_Absyn_bounds_one();(pi.ptr_atts).bounds=_tmp86F;});
(pi.ptr_atts).zero_term=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp872=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp871=loc;void*_tmp870=res_typ;Cyc_Tcutil_silent_castable(_tmp872,_tmp871,_tmp870,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2323
return res_typ;}}}}}_LL5:;}}
# 2329
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp874=t=({void*_tmp873=t;Cyc_Tcutil_promote_array(_tmp873,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp874;});
return t;}
# 2339
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2343
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(e->loc);{
void*_tmp46C=e->r;void*_stmttmp4D=_tmp46C;void*_tmp46D=_stmttmp4D;switch(*((int*)_tmp46D)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46D)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2352
 if(Cyc_Tcutil_is_array_type(t))
({void*_tmp876=t=({void*_tmp875=t;Cyc_Tcutil_promote_array(_tmp875,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp876;});
return t;}_LL0:;}}
# 2365 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*t=({void*_tmp877=Cyc_Tcutil_compress(Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0));e->topt=_tmp877;});
# 2369
{void*_tmp46E=e->r;void*_stmttmp4E=_tmp46E;void*_tmp46F=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp470;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46F)->tag == 12U){_LL1: _tmp470=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp470;
return t;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2374
void*_tmp471=t;void*_tmp472;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp471)->tag == 3U){_LL6: _tmp472=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp471)->f1).elt_type;_LL7: {void*t2=_tmp472;
# 2376
void*_tmp473=Cyc_Tcutil_compress(t2);void*_stmttmp4F=_tmp473;void*_tmp474=_stmttmp4F;struct Cyc_Absyn_FnInfo _tmp475;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp474)->tag == 5U){_LLB: _tmp475=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp474)->f1;_LLC: {struct Cyc_Absyn_FnInfo info=_tmp475;
# 2378
struct _tuple13 _tmp476=({struct _tuple13 _tmp5F0;({struct Cyc_List_List*_tmp878=Cyc_Tcenv_lookup_type_vars(te);_tmp5F0.f1=_tmp878;}),_tmp5F0.f2=Cyc_Core_heap_region;_tmp5F0;});struct _tuple13 env=_tmp476;
struct Cyc_List_List*_tmp477=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);struct Cyc_List_List*inst=_tmp477;
struct Cyc_List_List*_tmp478=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*ts=_tmp478;
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp479;
({void*_tmp879=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->tag=13U,_tmp47A->f1=inner,_tmp47A->f2=ts;_tmp47A;});e->r=_tmp879;});
({void*_tmp87A=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_tmp87A;});
return(void*)_check_null(e->topt);}}else{_LLD: _LLE:
 return t;}_LLA:;}}else{_LL8: _LL9:
# 2387
 return t;}_LL5:;}}}
# 2396
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2398
struct Cyc_List_List*_tmp47B=0;struct Cyc_List_List*decls=_tmp47B;
# 2400
{void*_tmp47C=fn_exp->r;void*_stmttmp50=_tmp47C;void*_tmp47D=_stmttmp50;struct Cyc_List_List*_tmp47E;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47D)->tag == 10U){_LL1: _tmp47E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47D)->f2;_LL2: {struct Cyc_List_List*es=_tmp47E;
# 2402
{void*_tmp47F=e->r;void*_stmttmp51=_tmp47F;void*_tmp480=_stmttmp51;struct Cyc_List_List*_tmp481;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp480)->tag == 10U){_LL6: _tmp481=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp480)->f2;_LL7: {struct Cyc_List_List*es2=_tmp481;
# 2404
struct Cyc_List_List*_tmp482=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp482;
int _tmp483=0;int arg_cnt=_tmp483;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp485=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=0U,({struct _fat_ptr _tmp87B=({const char*_tmp48C="bad count ";_tag_fat(_tmp48C,sizeof(char),11U);});_tmp5F5.f1=_tmp87B;});_tmp5F5;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp486=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=12U,_tmp5F4.f1=arg_cnt;_tmp5F4;});struct Cyc_Warn_String_Warn_Warg_struct _tmp487=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=0U,({struct _fat_ptr _tmp87C=({const char*_tmp48B="/";_tag_fat(_tmp48B,sizeof(char),2U);});_tmp5F3.f1=_tmp87C;});_tmp5F3;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp488=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=12U,_tmp5F2.f1=(int)arg_exps->hd;_tmp5F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp489=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=0U,({
struct _fat_ptr _tmp87D=({const char*_tmp48A=" for alias coercion!";_tag_fat(_tmp48A,sizeof(char),21U);});_tmp5F1.f1=_tmp87D;});_tmp5F1;});void*_tmp484[5U];_tmp484[0]=& _tmp485,_tmp484[1]=& _tmp486,_tmp484[2]=& _tmp487,_tmp484[3]=& _tmp488,_tmp484[4]=& _tmp489;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp484,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2416
struct _tuple12 _tmp48D=({struct Cyc_Absyn_Exp*_tmp87E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp87E,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp52=_tmp48D;struct _tuple12 _tmp48E=_stmttmp52;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Decl*_tmp48F;_LLB: _tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp48F;struct Cyc_Absyn_Exp*ve=_tmp490;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->hd=d,_tmp491->tl=decls;_tmp491;});
arg_exps=arg_exps->tl;}}}
# 2421
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp492=0U;({struct _fat_ptr _tmp87F=({const char*_tmp493="not a function call!";_tag_fat(_tmp493,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp87F,_tag_fat(_tmp492,sizeof(void*),0U));});});}_LL5:;}
# 2424
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp494=0U;({struct _fat_ptr _tmp880=({const char*_tmp495="not a function call!";_tag_fat(_tmp495,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp880,_tag_fat(_tmp494,sizeof(void*),0U));});});}_LL0:;}
# 2428
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp496=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp496;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp883=({struct Cyc_Absyn_Decl*_tmp882=d;struct Cyc_Absyn_Stmt*_tmp881=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp882,_tmp881,e->loc);});Cyc_Absyn_stmt_exp(_tmp883,e->loc);});
decls=decls->tl;}
# 2434
e->topt=0;
e->r=fn_exp->r;}
# 2439
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2443
{void*_tmp497=e->r;void*_stmttmp53=_tmp497;void*_tmp498=_stmttmp53;void*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct _fat_ptr*_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp49E;int*_tmp4A4;struct Cyc_Absyn_Exp**_tmp4A3;void***_tmp4A2;struct Cyc_Absyn_Exp**_tmp4A1;int*_tmp4A0;struct Cyc_Absyn_Enumfield*_tmp4A6;void*_tmp4A5;struct Cyc_Absyn_Enumfield*_tmp4A8;struct Cyc_Absyn_Enumdecl*_tmp4A7;struct Cyc_Absyn_Datatypefield*_tmp4AB;struct Cyc_Absyn_Datatypedecl*_tmp4AA;struct Cyc_List_List*_tmp4A9;struct Cyc_List_List*_tmp4AD;void*_tmp4AC;struct Cyc_Absyn_Aggrdecl**_tmp4B1;struct Cyc_List_List*_tmp4B0;struct Cyc_List_List**_tmp4AF;struct _tuple1**_tmp4AE;int*_tmp4B4;void*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B2;int*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Vardecl*_tmp4B5;struct Cyc_Absyn_Stmt*_tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_List_List*_tmp4BC;struct _tuple9*_tmp4BB;struct Cyc_List_List*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4BE;int*_tmp4C3;int*_tmp4C2;struct _fat_ptr*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C0;int*_tmp4C7;int*_tmp4C6;struct _fat_ptr*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_List_List*_tmp4CA;void*_tmp4C9;void*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Exp**_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CF;enum Cyc_Absyn_Coercion*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D1;void*_tmp4D0;struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Core_Opt*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;enum Cyc_Absyn_Incrementor _tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_List_List*_tmp4E5;enum Cyc_Absyn_Primop _tmp4E4;void**_tmp4E6;union Cyc_Absyn_Cnst*_tmp4E7;struct Cyc_List_List*_tmp4E9;struct Cyc_Core_Opt*_tmp4E8;struct Cyc_Absyn_VarargCallInfo**_tmp4EC;struct Cyc_List_List*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4ED;switch(*((int*)_tmp498)){case 12U: _LL1: _tmp4ED=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp4ED;
# 2448
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=(void*)_check_null(e2->topt);
return;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp498)->f4 == 0){_LL3: _LL4:
# 2452
({void*_tmp4EE=0U;({struct _fat_ptr _tmp884=({const char*_tmp4EF="unresolved function in tcExpNoInst";_tag_fat(_tmp4EF,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp884,_tag_fat(_tmp4EE,sizeof(void*),0U));});});}else{_LL1B: _tmp4EA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4EB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4EC=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp4EA;struct Cyc_List_List*es=_tmp4EB;struct Cyc_Absyn_VarargCallInfo**vci=_tmp4EC;
# 2475
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4F0;_push_handler(& _tmp4F0);{int _tmp4F2=0;if(setjmp(_tmp4F0.handler))_tmp4F2=1;if(!_tmp4F2){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp4F1=(void*)Cyc_Core_get_exn_thrown();void*_tmp4F3=_tmp4F1;void*_tmp4F4;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4F3)->tag == Cyc_Core_Failure){_LL5A: _LL5B:
# 2480
 ok=0;
fn_exp=e;
goto _LL59;}else{_LL5C: _tmp4F4=_tmp4F3;_LL5D: {void*exn=_tmp4F4;(int)_rethrow(exn);}}_LL59:;}}}
# 2484
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2491
goto _LL0;}}case 36U: _LL5: _tmp4E8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4E9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp4E8;struct Cyc_List_List*des=_tmp4E9;
# 2456
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp4E7=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp4E7;
# 2460
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp4E6=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LLA: {void**b=_tmp4E6;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp4E4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4E5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp4E4;struct Cyc_List_List*es=_tmp4E5;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp4E2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4E3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp4E2;enum Cyc_Absyn_Incrementor i=_tmp4E3;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp4DF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4E0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4E1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp4DF;struct Cyc_Core_Opt*popt=_tmp4E0;struct Cyc_Absyn_Exp*e2=_tmp4E1;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp4DC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4DD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4DE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp4DC;struct Cyc_Absyn_Exp*e2=_tmp4DD;struct Cyc_Absyn_Exp*e3=_tmp4DE;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp4DA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4DB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp4DA;struct Cyc_Absyn_Exp*e2=_tmp4DB;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp4D8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4D9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp4D8;struct Cyc_Absyn_Exp*e2=_tmp4D9;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp4D6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4D7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp4D6;struct Cyc_Absyn_Exp*e2=_tmp4D7;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp4D5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp4D5;
# 2492
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp4D3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4D4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp4D3;struct Cyc_List_List*ts=_tmp4D4;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp4D0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4D1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4D2=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp498)->f4;_LL22: {void*t1=_tmp4D0;struct Cyc_Absyn_Exp*e1=_tmp4D1;enum Cyc_Absyn_Coercion*c=_tmp4D2;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp4CF=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp4CF;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp4CD=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4CE=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp4CD;struct Cyc_Absyn_Exp*e1=_tmp4CE;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp4CC=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp4CC;
_tmp4CB=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17U: _LL29: _tmp4CB=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL2A: {void*t1=_tmp4CB;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp4C9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4CA=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL2C: {void*t1=_tmp4C9;struct Cyc_List_List*l=_tmp4CA;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp4C8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp4C8;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp4C4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4C5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4C6=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_tmp4C7=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp498)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp4C4;struct _fat_ptr*f=_tmp4C5;int*is_tagged=_tmp4C6;int*is_read=_tmp4C7;
# 2502
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4C0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4C2=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_tmp4C3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp498)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4C0;struct _fat_ptr*f=_tmp4C1;int*is_tagged=_tmp4C2;int*is_read=_tmp4C3;
# 2504
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4BE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4BF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4BE;struct Cyc_Absyn_Exp*e2=_tmp4BF;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4BD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL36: {struct Cyc_List_List*es=_tmp4BD;
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4BB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4BC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL38: {struct _tuple9*t1=_tmp4BB;struct Cyc_List_List*des=_tmp4BC;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4BA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4BA;
# 2511
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4F5=Cyc_Tcutil_compress(*topt);void*_stmttmp54=_tmp4F5;void*_tmp4F6=_stmttmp54;void*_tmp4F9;struct Cyc_Absyn_Tqual*_tmp4F8;void**_tmp4F7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F6)->tag == 4U){_LL5F: _tmp4F7=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F6)->f1).elt_type;_tmp4F8=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F6)->f1).tq;_tmp4F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F6)->f1).zero_term;_LL60: {void**et=_tmp4F7;struct Cyc_Absyn_Tqual*etq=_tmp4F8;void*zt=_tmp4F9;
# 2517
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}
# 2523
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4B9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4B9;
# 2525
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4B5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4B6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4B7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_tmp4B8=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp498)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4B5;struct Cyc_Absyn_Exp*e1=_tmp4B6;struct Cyc_Absyn_Exp*e2=_tmp4B7;int*iszeroterm=_tmp4B8;
# 2527
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4B2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4B3=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4B4=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4B2;void*t1=_tmp4B3;int*iszeroterm=_tmp4B4;
# 2529
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4AE=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4AF=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4B0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_tmp4B1=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp498)->f4;_LL42: {struct _tuple1**tn=_tmp4AE;struct Cyc_List_List**ts=_tmp4AF;struct Cyc_List_List*args=_tmp4B0;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4B1;
# 2531
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4AC=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4AD=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL44: {void*ts=_tmp4AC;struct Cyc_List_List*args=_tmp4AD;
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4A9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4AA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_tmp4AB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp498)->f3;_LL46: {struct Cyc_List_List*es=_tmp4A9;struct Cyc_Absyn_Datatypedecl*tud=_tmp4AA;struct Cyc_Absyn_Datatypefield*tuf=_tmp4AB;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4A7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4A8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4A7;struct Cyc_Absyn_Enumfield*ef=_tmp4A8;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4A5=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4A6=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL4A: {void*t2=_tmp4A5;struct Cyc_Absyn_Enumfield*ef=_tmp4A6;
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4A0=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp498)->f1).is_calloc;_tmp4A1=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp498)->f1).rgn;_tmp4A2=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp498)->f1).elt_type;_tmp4A3=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp498)->f1).num_elts;_tmp4A4=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp498)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4A0;struct Cyc_Absyn_Exp**ropt=_tmp4A1;void***t2=_tmp4A2;struct Cyc_Absyn_Exp**e2=_tmp4A3;int*isfat=_tmp4A4;
# 2537
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp49E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp49F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp49E;struct Cyc_Absyn_Exp*e2=_tmp49F;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp49C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp49D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp49C;struct _fat_ptr*f=_tmp49D;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp49B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp49B;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42U: _LL53: _tmp49A=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL54: {struct Cyc_Absyn_Exp*e=_tmp49A;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39U: _LL55: _tmp499=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL56: {void*t2=_tmp499;
# 2543
if(!te->allow_valueof)
({void*_tmp4FA=0U;({unsigned _tmp886=e->loc;struct _fat_ptr _tmp885=({const char*_tmp4FB="valueof(-) can only occur within types";_tag_fat(_tmp4FB,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp886,_tmp885,_tag_fat(_tmp4FA,sizeof(void*),0U));});});
# 2551
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL57: _LL58:
# 2555
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2557
e->topt=t;}

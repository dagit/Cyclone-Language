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
# 956
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 959
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 961
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term);
# 965
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zt);
# 973
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 977
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 983
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp();
# 1054
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1065
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1119
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1121
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1123
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e);
# 1126
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1128
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
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp540;_tmp540.tag=0U,({struct _fat_ptr _tmp636=({const char*_tmp2="Cannot consume non-unique paths; do swap instead";_tag_fat(_tmp2,sizeof(char),49U);});_tmp540.f1=_tmp636;});_tmp540;});void*_tmp0[1U];_tmp0[0]=& _tmp1;({unsigned _tmp637=loc;Cyc_Warn_err2(_tmp637,_tag_fat(_tmp0,sizeof(void*),1U));});});}
# 64
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 68
if(topt == 0){
# 70
({void*_tmp638=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=26U,_tmp3->f1=des;_tmp3;});e->r=_tmp638;});
return;}{
# 73
void*t=*topt;
void*_tmp4=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmp4;void*_tmp5=_stmttmp0;struct Cyc_Absyn_Tqual _tmp7;void*_tmp6;union Cyc_Absyn_AggrInfo _tmp8;switch(*((int*)_tmp5)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 20U){_LL1: _tmp8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->f1;_LL2: {union Cyc_Absyn_AggrInfo info=_tmp8;
# 76
{union Cyc_Absyn_AggrInfo _tmp9=info;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp9.UnknownAggr).tag == 1){_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp541;_tmp541.tag=0U,({struct _fat_ptr _tmp639=({const char*_tmpD="struct type improperly set";_tag_fat(_tmpD,sizeof(char),27U);});_tmp541.f1=_tmp639;});_tmp541;});void*_tmpB[1U];_tmpB[0]=& _tmpC;({unsigned _tmp63A=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp63A,_tag_fat(_tmpB,sizeof(void*),1U));});});}else{_LLC: _tmpA=*(_tmp9.KnownAggr).val;_LLD: {struct Cyc_Absyn_Aggrdecl*ad=_tmpA;
({void*_tmp63B=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->tag=29U,_tmpE->f1=ad->name,_tmpE->f2=0,_tmpE->f3=des,_tmpE->f4=ad;_tmpE;});e->r=_tmp63B;});}}_LL9:;}
# 80
goto _LL0;}}else{goto _LL7;}case 4U: _LL3: _tmp6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5)->f1).elt_type;_tmp7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5)->f1).tq;_LL4: {void*at=_tmp6;struct Cyc_Absyn_Tqual aq=_tmp7;
({void*_tmp63C=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->tag=26U,_tmpF->f1=des;_tmpF;});e->r=_tmp63C;});goto _LL0;}case 7U: _LL5: _LL6:
({void*_tmp63D=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=30U,_tmp10->f1=t,_tmp10->f2=des;_tmp10;});e->r=_tmp63D;});goto _LL0;default: _LL7: _LL8:
({void*_tmp63E=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=26U,_tmp11->f1=des;_tmp11;});e->r=_tmp63E;});goto _LL0;}_LL0:;}}
# 90
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 94
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 100
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp12=e->r;void*_stmttmp1=_tmp12;void*_tmp13=_stmttmp1;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp13)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp13)->f2 == 0){_LL1: _LL2:
# 103
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _tmp542;_tmp542.tag=0U,({struct _fat_ptr _tmp63F=({const char*_tmp16="assignment in test";_tag_fat(_tmp16,sizeof(char),19U);});_tmp542.f1=_tmp63F;});_tmp542;});void*_tmp14[1U];_tmp14[0]=& _tmp15;({unsigned _tmp640=e->loc;Cyc_Warn_warn2(_tmp640,_tag_fat(_tmp14,sizeof(void*),1U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 111
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _tmp547;_tmp547.tag=0U,({struct _fat_ptr _tmp641=({const char*_tmp1F="test of ";_tag_fat(_tmp1F,sizeof(char),9U);});_tmp547.f1=_tmp641;});_tmp547;});struct Cyc_Warn_String_Warn_Warg_struct _tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _tmp546;_tmp546.tag=0U,_tmp546.f1=msg_part;_tmp546;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp545;_tmp545.tag=0U,({struct _fat_ptr _tmp642=({const char*_tmp1E=" has type ";_tag_fat(_tmp1E,sizeof(char),11U);});_tmp545.f1=_tmp642;});_tmp545;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1B=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp544;_tmp544.tag=3U,_tmp544.f1=(void*)e->topt;_tmp544;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp543;_tmp543.tag=0U,({
struct _fat_ptr _tmp643=({const char*_tmp1D=" instead of integral or pointer type";_tag_fat(_tmp1D,sizeof(char),37U);});_tmp543.f1=_tmp643;});_tmp543;});void*_tmp17[5U];_tmp17[0]=& _tmp18,_tmp17[1]=& _tmp19,_tmp17[2]=& _tmp1A,_tmp17[3]=& _tmp1B,_tmp17[4]=& _tmp1C;({unsigned _tmp644=e->loc;Cyc_Warn_err2(_tmp644,_tag_fat(_tmp17,sizeof(void*),5U));});});}
# 133 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 138
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
union Cyc_Absyn_Cnst _tmp20=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _stmttmp2=_tmp20;union Cyc_Absyn_Cnst _tmp21=_stmttmp2;struct _fat_ptr _tmp22;struct _fat_ptr _tmp23;int _tmp25;enum Cyc_Absyn_Sign _tmp24;int _tmp26;switch((_tmp21.String_c).tag){case 2U: switch(((_tmp21.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 return Cyc_Absyn_uchar_type;default: _LL5: _LL6:
 return Cyc_Absyn_char_type;}case 3U: _LL7: _LL8:
 return Cyc_Absyn_wchar_type();case 4U: if(((_tmp21.Short_c).val).f1 == Cyc_Absyn_Unsigned){_LL9: _LLA:
 return Cyc_Absyn_ushort_type;}else{_LLB: _LLC:
 return Cyc_Absyn_sshort_type;}case 6U: if(((_tmp21.LongLong_c).val).f1 == Cyc_Absyn_Unsigned){_LLD: _LLE:
 return Cyc_Absyn_ulonglong_type;}else{_LLF: _LL10:
 return Cyc_Absyn_slonglong_type;}case 7U: _LL11: _tmp26=((_tmp21.Float_c).val).f2;_LL12: {int i=_tmp26;
# 153
if(topt == 0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_tmp27=Cyc_Tcutil_compress(*topt);void*_stmttmp3=_tmp27;void*_tmp28=_stmttmp3;int _tmp29;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28)->f1)->tag == 2U){_LL1C: _tmp29=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28)->f1)->f1;_LL1D: {int i=_tmp29;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E: _LL1F:
 return Cyc_Absyn_gen_float_type((unsigned)i);}_LL1B:;}}case 5U: _LL13: _tmp24=((_tmp21.Int_c).val).f1;_tmp25=((_tmp21.Int_c).val).f2;_LL14: {enum Cyc_Absyn_Sign csn=_tmp24;int i=_tmp25;
# 163
if(topt == 0)
return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_tmp2A=Cyc_Tcutil_compress(*topt);void*_stmttmp4=_tmp2A;void*_tmp2B=_stmttmp4;void*_tmp2C;enum Cyc_Absyn_Sign _tmp2D;enum Cyc_Absyn_Sign _tmp2E;enum Cyc_Absyn_Sign _tmp2F;enum Cyc_Absyn_Sign _tmp30;switch(*((int*)_tmp2B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f2){case Cyc_Absyn_Char_sz: _LL21: _tmp30=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_LL22: {enum Cyc_Absyn_Sign sn=_tmp30;
# 167
({union Cyc_Absyn_Cnst _tmp645=Cyc_Absyn_Char_c(sn,(char)i);*c=_tmp645;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _LL23: _tmp2F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_LL24: {enum Cyc_Absyn_Sign sn=_tmp2F;
# 170
({union Cyc_Absyn_Cnst _tmp646=Cyc_Absyn_Short_c(sn,(short)i);*c=_tmp646;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _LL25: _tmp2E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_LL26: {enum Cyc_Absyn_Sign sn=_tmp2E;
_tmp2D=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _LL27: _tmp2D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_tmp2D;
# 176
({union Cyc_Absyn_Cnst _tmp647=Cyc_Absyn_Int_c(sn,i);*c=_tmp647;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f2 != 0){_LL2B: _tmp2C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B)->f2)->hd;_LL2C: {void*t1=_tmp2C;
# 185
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=9U,({struct Cyc_Absyn_Exp*_tmp648=Cyc_Absyn_uint_exp((unsigned)i,0U);_tmp31->f1=_tmp648;});_tmp31;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3U: _LL29: if(i == 0){_LL2A: {
# 180
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _tmp649=({union Cyc_Absyn_Cnst _tmp548;(_tmp548.Null_c).tag=1U,(_tmp548.Null_c).val=0;_tmp548;});*c=_tmp649;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}}else{goto _LL2D;}default: _LL2D: _LL2E:
# 187
 return(int)csn == (int)1U?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL20:;}}case 1U: _LL15: _LL16:
# 190
 if(topt != 0){
void*_tmp32=Cyc_Tcutil_compress(*topt);void*_stmttmp5=_tmp32;void*_tmp33=_stmttmp5;void*_tmp34;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33)->tag == 3U){_LL30: _tmp34=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33)->f1).ptr_atts).nullable;_LL31: {void*nbl=_tmp34;
# 193
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp36=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54A;_tmp54A.tag=0U,({struct _fat_ptr _tmp64A=({const char*_tmp38="Used NULL when expecting a value of type ";_tag_fat(_tmp38,sizeof(char),42U);});_tmp54A.f1=_tmp64A;});_tmp54A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp37=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp549;_tmp549.tag=2U,_tmp549.f1=(void*)*topt;_tmp549;});void*_tmp35[2U];_tmp35[0]=& _tmp36,_tmp35[1]=& _tmp37;({unsigned _tmp64B=e->loc;Cyc_Warn_err2(_tmp64B,_tag_fat(_tmp35,sizeof(void*),2U));});});
return*topt;}}else{_LL32: _LL33:
 goto _LL2F;}_LL2F:;}{
# 198
struct Cyc_List_List*_tmp39=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp39;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->tag=3U,({void*_tmp650=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->v=tenv_tvs;_tmp3A;}));(_tmp3C->f1).elt_type=_tmp650;}),({
struct Cyc_Absyn_Tqual _tmp64F=Cyc_Absyn_empty_tqual(0U);(_tmp3C->f1).elt_tq=_tmp64F;}),
({void*_tmp64E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->v=tenv_tvs;_tmp3B;}));((_tmp3C->f1).ptr_atts).rgn=_tmp64E;}),((_tmp3C->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp64D=Cyc_Tcutil_any_bounds(tenv_tvs);((_tmp3C->f1).ptr_atts).bounds=_tmp64D;}),({
void*_tmp64C=Cyc_Tcutil_any_bool(tenv_tvs);((_tmp3C->f1).ptr_atts).zero_term=_tmp64C;}),((_tmp3C->f1).ptr_atts).ptrloc=0;_tmp3C;});}case 8U: _LL17: _tmp23=(_tmp21.String_c).val;_LL18: {struct _fat_ptr s=_tmp23;
# 205
string_numelts=(int)_get_fat_size(s,sizeof(char));
_tmp22=s;goto _LL1A;}default: _LL19: _tmp22=(_tmp21.Wstring_c).val;_LL1A: {struct _fat_ptr s=_tmp22;
# 208
if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 212
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp651=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp651,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 217
void*_tmp3D=({void*_tmp655=string_elt_typ;void*_tmp654=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp653=Cyc_Absyn_const_tqual(0U);void*_tmp652=
Cyc_Absyn_thin_bounds_exp(elen);
# 217
Cyc_Absyn_atb_type(_tmp655,_tmp654,_tmp653,_tmp652,Cyc_Absyn_true_type);});void*t=_tmp3D;
# 219
if(topt == 0)
return t;{
void*_tmp3E=Cyc_Tcutil_compress(*topt);void*_stmttmp6=_tmp3E;void*_tmp3F=_stmttmp6;struct Cyc_Absyn_Tqual _tmp40;switch(*((int*)_tmp3F)){case 4U: _LL35: _tmp40=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).tq;_LL36: {struct Cyc_Absyn_Tqual tq=_tmp40;
# 225
return({void*_tmp658=string_elt_typ;struct Cyc_Absyn_Tqual _tmp657=tq;struct Cyc_Absyn_Exp*_tmp656=elen;Cyc_Absyn_array_type(_tmp658,_tmp657,_tmp656,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3U: _LL37: _LL38:
# 230
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_tmp65B=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp65A=loc;void*_tmp659=t;Cyc_Tcutil_silent_castable(_tmp65B,_tmp65A,_tmp659,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
return*topt;}
# 235
return t;default: _LL39: _LL3A:
 return t;}_LL34:;}}}}}_LL0:;}
# 242
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp41=*((void**)_check_null(b));void*_stmttmp7=_tmp41;void*_tmp42=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp43;struct Cyc_Absyn_Vardecl*_tmp44;struct Cyc_Absyn_Vardecl*_tmp45;struct Cyc_Absyn_Fndecl*_tmp46;struct Cyc_Absyn_Vardecl*_tmp47;struct _tuple1*_tmp48;switch(*((int*)_tmp42)){case 0U: _LL1: _tmp48=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp42)->f1;_LL2: {struct _tuple1*q=_tmp48;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54B;_tmp54B.tag=0U,({struct _fat_ptr _tmp65C=({const char*_tmp4B="unresolved binding in tcVar";_tag_fat(_tmp4B,sizeof(char),28U);});_tmp54B.f1=_tmp65C;});_tmp54B;});void*_tmp49[1U];_tmp49[0]=& _tmp4A;({unsigned _tmp65D=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp65D,_tag_fat(_tmp49,sizeof(void*),1U));});});}case 1U: _LL3: _tmp47=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp42)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp47;
# 248
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2U: _LL5: _tmp46=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp42)->f1;_LL6: {struct Cyc_Absyn_Fndecl*fd=_tmp46;
# 255
if(fd->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5U: _LL7: _tmp45=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp42)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp45;
_tmp44=vd;goto _LLA;}case 4U: _LL9: _tmp44=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp42)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_tmp44;
_tmp43=vd;goto _LLC;}default: _LLB: _tmp43=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp42)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp43;
# 261
if(te->allow_valueof){
void*_tmp4C=Cyc_Tcutil_compress(vd->type);void*_stmttmp8=_tmp4C;void*_tmp4D=_stmttmp8;void*_tmp4E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D)->f2 != 0){_LLE: _tmp4E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D)->f2)->hd;_LLF: {void*i=_tmp4E;
({void*_tmp65E=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=39U,_tmp4F->f1=i;_tmp4F;});e->r=_tmp65E;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 266
return vd->type;}}_LL0:;}
# 270
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 278
struct Cyc_List_List*desc_types;
{void*_tmp50=fmt->r;void*_stmttmp9=_tmp50;void*_tmp51=_stmttmp9;struct _fat_ptr _tmp52;struct _fat_ptr _tmp53;switch(*((int*)_tmp51)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp51)->f1).String_c).tag == 8){_LL1: _tmp53=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp51)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp53;
_tmp52=s;goto _LL4;}}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp51)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp51)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp52=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp51)->f2)->r)->f1).String_c).val;_LL4: {struct _fat_ptr s=_tmp52;
# 282
desc_types=type_getter(te,(struct _fat_ptr)s,isCproto,fmt->loc);goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 286
 if(opt_args != 0){
struct Cyc_List_List*_tmp54=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp54;
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt))&& !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)args->hd))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)args->hd)->loc);}}
# 295
return;}_LL0:;}
# 297
if(opt_args != 0){
struct Cyc_List_List*_tmp55=(struct Cyc_List_List*)opt_args->v;struct Cyc_List_List*args=_tmp55;
# 300
for(0;desc_types != 0 && args != 0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp660=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp65F=e;Cyc_Tcutil_coerce_arg(_tmp660,_tmp65F,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp57=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54F;_tmp54F.tag=0U,({struct _fat_ptr _tmp661=({const char*_tmp5C="descriptor has type ";_tag_fat(_tmp5C,sizeof(char),21U);});_tmp54F.f1=_tmp661;});_tmp54F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp54E;_tmp54E.tag=2U,_tmp54E.f1=(void*)t;_tmp54E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp59=({struct Cyc_Warn_String_Warn_Warg_struct _tmp54D;_tmp54D.tag=0U,({
struct _fat_ptr _tmp662=({const char*_tmp5B=" but argument has type ";_tag_fat(_tmp5B,sizeof(char),24U);});_tmp54D.f1=_tmp662;});_tmp54D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5A=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp54C;_tmp54C.tag=3U,_tmp54C.f1=(void*)e->topt;_tmp54C;});void*_tmp56[4U];_tmp56[0]=& _tmp57,_tmp56[1]=& _tmp58,_tmp56[2]=& _tmp59,_tmp56[3]=& _tmp5A;({unsigned _tmp663=e->loc;Cyc_Tcexp_err_and_explain(_tmp663,_tag_fat(_tmp56,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp664=({struct Cyc_List_List*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->hd=(void*)arg_cnt,_tmp5D->tl=*alias_arg_exps;_tmp5D;});*alias_arg_exps=_tmp664;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !
Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(((struct Cyc_Absyn_Exp*)args->hd)->loc);}
# 316
if(desc_types != 0)
({void*_tmp5E=0U;({unsigned _tmp666=fmt->loc;struct _fat_ptr _tmp665=({const char*_tmp5F="too few arguments";_tag_fat(_tmp5F,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp666,_tmp665,_tag_fat(_tmp5E,sizeof(void*),0U));});});
if(args != 0){
({void*_tmp60=0U;({unsigned _tmp668=((struct Cyc_Absyn_Exp*)args->hd)->loc;struct _fat_ptr _tmp667=({const char*_tmp61="too many arguments";_tag_fat(_tmp61,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp668,_tmp667,_tag_fat(_tmp60,sizeof(void*),0U));});});
# 321
for(0;args != 0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}}}
# 326
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 328
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp62=p;switch(_tmp62){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 332
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp64=({struct Cyc_Warn_String_Warn_Warg_struct _tmp551;_tmp551.tag=0U,({struct _fat_ptr _tmp669=({const char*_tmp66="expecting numeric type but found ";_tag_fat(_tmp66,sizeof(char),34U);});_tmp551.f1=_tmp669;});_tmp551;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp550;_tmp550.tag=2U,_tmp550.f1=(void*)t;_tmp550;});void*_tmp63[2U];_tmp63[0]=& _tmp64,_tmp63[1]=& _tmp65;({unsigned _tmp66A=loc;Cyc_Warn_err2(_tmp66A,_tag_fat(_tmp63,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 336
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp68=({struct Cyc_Warn_String_Warn_Warg_struct _tmp553;_tmp553.tag=0U,({struct _fat_ptr _tmp66B=({const char*_tmp6A="expecting integral or * type but found ";_tag_fat(_tmp6A,sizeof(char),40U);});_tmp553.f1=_tmp66B;});_tmp553;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp69=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp552;_tmp552.tag=2U,_tmp552.f1=(void*)t;_tmp552;});void*_tmp67[2U];_tmp67[0]=& _tmp68,_tmp67[1]=& _tmp69;({unsigned _tmp66C=loc;Cyc_Warn_err2(_tmp66C,_tag_fat(_tmp67,sizeof(void*),2U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 341
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp555;_tmp555.tag=0U,({struct _fat_ptr _tmp66D=({const char*_tmp6E="expecting integral type but found ";_tag_fat(_tmp6E,sizeof(char),35U);});_tmp555.f1=_tmp66D;});_tmp555;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp554;_tmp554.tag=2U,_tmp554.f1=(void*)t;_tmp554;});void*_tmp6B[2U];_tmp6B[0]=& _tmp6C,_tmp6B[1]=& _tmp6D;({unsigned _tmp66E=loc;Cyc_Warn_err2(_tmp66E,_tag_fat(_tmp6B,sizeof(void*),2U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 345
{void*_tmp6F=t;void*_tmp70;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->tag == 3U){_LLE: _tmp70=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).bounds;_LLF: {void*b=_tmp70;
# 347
struct Cyc_Absyn_Exp*_tmp71=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp71;
if((eopt != 0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp73=({struct Cyc_Warn_String_Warn_Warg_struct _tmp556;_tmp556.tag=0U,({struct _fat_ptr _tmp66F=({const char*_tmp74="cannot use numelts on a pointer with abstract bounds";_tag_fat(_tmp74,sizeof(char),53U);});_tmp556.f1=_tmp66F;});_tmp556;});void*_tmp72[1U];_tmp72[0]=& _tmp73;({unsigned _tmp670=loc;Cyc_Warn_err2(_tmp670,_tag_fat(_tmp72,sizeof(void*),1U));});});
goto _LLD;}}else{_LL10: _LL11:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp76=({struct Cyc_Warn_String_Warn_Warg_struct _tmp558;_tmp558.tag=0U,({struct _fat_ptr _tmp671=({const char*_tmp78="numelts requires pointer type, not ";_tag_fat(_tmp78,sizeof(char),36U);});_tmp558.f1=_tmp671;});_tmp558;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp77=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp557;_tmp557.tag=2U,_tmp557.f1=(void*)t;_tmp557;});void*_tmp75[2U];_tmp75[0]=& _tmp76,_tmp75[1]=& _tmp77;({unsigned _tmp672=loc;Cyc_Warn_err2(_tmp672,_tag_fat(_tmp75,sizeof(void*),2U));});});}_LLD:;}
# 353
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp7A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp559;_tmp559.tag=0U,({struct _fat_ptr _tmp673=({const char*_tmp7B="Non-unary primop";_tag_fat(_tmp7B,sizeof(char),17U);});_tmp559.f1=_tmp673;});_tmp559;});void*_tmp79[1U];_tmp79[0]=& _tmp7A;({unsigned _tmp674=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2)(_tmp674,_tag_fat(_tmp79,sizeof(void*),1U));});});}_LL0:;}
# 360
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp7C=(void*)_check_null(e1->topt);void*t1=_tmp7C;
void*_tmp7D=(void*)_check_null(e2->topt);void*t2=_tmp7D;
void*_tmp7E=Cyc_Tcutil_max_arithmetic_type(t1,t2);void*new_typ=_tmp7E;
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,Cyc_Absyn_No_coercion);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,Cyc_Absyn_No_coercion);
return new_typ;}
# 370
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 373
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp80=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55C;_tmp55C.tag=0U,({struct _fat_ptr _tmp675=({const char*_tmp84="type ";_tag_fat(_tmp84,sizeof(char),6U);});_tmp55C.f1=_tmp675;});_tmp55C;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp81=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp55B;_tmp55B.tag=3U,_tmp55B.f1=(void*)e1->topt;_tmp55B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp82=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55A;_tmp55A.tag=0U,({struct _fat_ptr _tmp676=({const char*_tmp83=" cannot be used here";_tag_fat(_tmp83,sizeof(char),21U);});_tmp55A.f1=_tmp676;});_tmp55A;});void*_tmp7F[3U];_tmp7F[0]=& _tmp80,_tmp7F[1]=& _tmp81,_tmp7F[2]=& _tmp82;({struct Cyc_Tcenv_Tenv*_tmp678=te;unsigned _tmp677=e1->loc;Cyc_Tcexp_expr_err(_tmp678,_tmp677,0,_tag_fat(_tmp7F,sizeof(void*),3U));});});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp86=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55F;_tmp55F.tag=0U,({struct _fat_ptr _tmp679=({const char*_tmp8A="type ";_tag_fat(_tmp8A,sizeof(char),6U);});_tmp55F.f1=_tmp679;});_tmp55F;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp87=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp55E;_tmp55E.tag=3U,_tmp55E.f1=(void*)e2->topt;_tmp55E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp88=({struct Cyc_Warn_String_Warn_Warg_struct _tmp55D;_tmp55D.tag=0U,({struct _fat_ptr _tmp67A=({const char*_tmp89=" cannot be used here";_tag_fat(_tmp89,sizeof(char),21U);});_tmp55D.f1=_tmp67A;});_tmp55D;});void*_tmp85[3U];_tmp85[0]=& _tmp86,_tmp85[1]=& _tmp87,_tmp85[2]=& _tmp88;({struct Cyc_Tcenv_Tenv*_tmp67C=te;unsigned _tmp67B=e2->loc;Cyc_Tcexp_expr_err(_tmp67C,_tmp67B,0,_tag_fat(_tmp85,sizeof(void*),3U));});});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 380
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp8B=t1;void*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->tag == 3U){_LL1: _tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_type;_tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_tq;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).rgn;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).bounds;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).zero_term;_LL2: {void*et=_tmp8C;struct Cyc_Absyn_Tqual tq=_tmp8D;void*r=_tmp8E;void*n=_tmp8F;void*b=_tmp90;void*zt=_tmp91;
# 385
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Tcutil_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp93=({struct Cyc_Warn_String_Warn_Warg_struct _tmp560;_tmp560.tag=0U,({struct _fat_ptr _tmp67D=({const char*_tmp94="can't do arithmetic on abstract pointer type";_tag_fat(_tmp94,sizeof(char),45U);});_tmp560.f1=_tmp67D;});_tmp560;});void*_tmp92[1U];_tmp92[0]=& _tmp93;({unsigned _tmp67E=e1->loc;Cyc_Warn_err2(_tmp67E,_tag_fat(_tmp92,sizeof(void*),1U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp96=({struct Cyc_Warn_String_Warn_Warg_struct _tmp561;_tmp561.tag=0U,({struct _fat_ptr _tmp67F=({const char*_tmp97="can't do arithmetic on non-aliasing pointer type";_tag_fat(_tmp97,sizeof(char),49U);});_tmp561.f1=_tmp67F;});_tmp561;});void*_tmp95[1U];_tmp95[0]=& _tmp96;({unsigned _tmp680=e1->loc;Cyc_Warn_err2(_tmp680,_tag_fat(_tmp95,sizeof(void*),1U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp99=({struct Cyc_Warn_String_Warn_Warg_struct _tmp563;_tmp563.tag=0U,({struct _fat_ptr _tmp681=({const char*_tmp9B="expecting int but found ";_tag_fat(_tmp9B,sizeof(char),25U);});_tmp563.f1=_tmp681;});_tmp563;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp9A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp562;_tmp562.tag=2U,_tmp562.f1=(void*)t2;_tmp562;});void*_tmp98[2U];_tmp98[0]=& _tmp99,_tmp98[1]=& _tmp9A;({unsigned _tmp682=e2->loc;Cyc_Warn_err2(_tmp682,_tag_fat(_tmp98,sizeof(void*),2U));});});{
struct Cyc_Absyn_Exp*_tmp9C=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp9C;
if(eopt == 0)
return t1;
# 396
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _tmp9D=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple16 _stmttmpA=_tmp9D;struct _tuple16 _tmp9E=_stmttmpA;int _tmpA0;unsigned _tmp9F;_LL6: _tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;_LL7: {unsigned i=_tmp9F;int known=_tmpA0;
if(known && i == (unsigned)1)
({void*_tmpA1=0U;({unsigned _tmp684=e1->loc;struct _fat_ptr _tmp683=({const char*_tmpA2="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_fat(_tmpA2,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp684,_tmp683,_tag_fat(_tmpA1,sizeof(void*),0U));});});}}{
# 407
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=3U,(_tmpA4->f1).elt_type=et,(_tmpA4->f1).elt_tq=tq,((_tmpA4->f1).ptr_atts).rgn=r,((_tmpA4->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmpA4->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmpA4->f1).ptr_atts).zero_term=zt,((_tmpA4->f1).ptr_atts).ptrloc=0;_tmpA4;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1=_tmpA3;
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmpA6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp569;_tmp569.tag=0U,({struct _fat_ptr _tmp685=({const char*_tmpAF="pointer arithmetic on values of different ";_tag_fat(_tmpAF,sizeof(char),43U);});_tmp569.f1=_tmp685;});_tmp569;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp568;_tmp568.tag=0U,({
struct _fat_ptr _tmp686=({const char*_tmpAE="types (";_tag_fat(_tmpAE,sizeof(char),8U);});_tmp568.f1=_tmp686;});_tmp568;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpA8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp567;_tmp567.tag=2U,_tmp567.f1=(void*)t1;_tmp567;});struct Cyc_Warn_String_Warn_Warg_struct _tmpA9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp566;_tmp566.tag=0U,({struct _fat_ptr _tmp687=({const char*_tmpAD=" != ";_tag_fat(_tmpAD,sizeof(char),5U);});_tmp566.f1=_tmp687;});_tmp566;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpAA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp565;_tmp565.tag=2U,_tmp565.f1=(void*)t2;_tmp565;});struct Cyc_Warn_String_Warn_Warg_struct _tmpAB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp564;_tmp564.tag=0U,({struct _fat_ptr _tmp688=({const char*_tmpAC=")";_tag_fat(_tmpAC,sizeof(char),2U);});_tmp564.f1=_tmp688;});_tmp564;});void*_tmpA5[6U];_tmpA5[0]=& _tmpA6,_tmpA5[1]=& _tmpA7,_tmpA5[2]=& _tmpA8,_tmpA5[3]=& _tmpA9,_tmpA5[4]=& _tmpAA,_tmpA5[5]=& _tmpAB;({unsigned _tmp689=e1->loc;Cyc_Tcexp_err_and_explain(_tmp689,_tag_fat(_tmpA5,sizeof(void*),6U));});});
return Cyc_Absyn_sint_type;}
# 429
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp68A=t1_elt;Cyc_Unify_unify(_tmp68A,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpB1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56F;_tmp56F.tag=0U,({struct _fat_ptr _tmp68B=({const char*_tmpBA="pointer arithmetic on values of different ";_tag_fat(_tmpBA,sizeof(char),43U);});_tmp56F.f1=_tmp68B;});_tmp56F;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56E;_tmp56E.tag=0U,({
struct _fat_ptr _tmp68C=({const char*_tmpB9="types(";_tag_fat(_tmpB9,sizeof(char),7U);});_tmp56E.f1=_tmp68C;});_tmp56E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56D;_tmp56D.tag=2U,_tmp56D.f1=(void*)t1;_tmp56D;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56C;_tmp56C.tag=0U,({struct _fat_ptr _tmp68D=({const char*_tmpB8=" != ";_tag_fat(_tmpB8,sizeof(char),5U);});_tmp56C.f1=_tmp68D;});_tmp56C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpB5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp56B;_tmp56B.tag=2U,_tmp56B.f1=(void*)t2;_tmp56B;});struct Cyc_Warn_String_Warn_Warg_struct _tmpB6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp56A;_tmp56A.tag=0U,({struct _fat_ptr _tmp68E=({const char*_tmpB7=")";_tag_fat(_tmpB7,sizeof(char),2U);});_tmp56A.f1=_tmp68E;});_tmp56A;});void*_tmpB0[6U];_tmpB0[0]=& _tmpB1,_tmpB0[1]=& _tmpB2,_tmpB0[2]=& _tmpB3,_tmpB0[3]=& _tmpB4,_tmpB0[4]=& _tmpB5,_tmpB0[5]=& _tmpB6;({unsigned _tmp68F=e1->loc;Cyc_Tcexp_err_and_explain(_tmp68F,_tag_fat(_tmpB0,sizeof(void*),6U));});});
# 434
({void*_tmpBB=0U;({unsigned _tmp691=e1->loc;struct _fat_ptr _tmp690=({const char*_tmpBC="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpBC,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp691,_tmp690,_tag_fat(_tmpBB,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp695=e1;Cyc_Tcutil_unchecked_cast(_tmp695,({void*_tmp694=t1_elt;void*_tmp693=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp692=
Cyc_Absyn_empty_tqual(0U);
# 435
Cyc_Absyn_star_type(_tmp694,_tmp693,_tmp692,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 438
return Cyc_Absyn_sint_type;}
# 440
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpBD=0U;({unsigned _tmp697=e1->loc;struct _fat_ptr _tmp696=({const char*_tmpBE="can't perform arithmetic on abstract pointer type";_tag_fat(_tmpBE,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp697,_tmp696,_tag_fat(_tmpBD,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpBF=0U;({unsigned _tmp699=e1->loc;struct _fat_ptr _tmp698=({const char*_tmpC0="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmpC0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp699,_tmp698,_tag_fat(_tmpBF,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpC2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp573;_tmp573.tag=0U,({struct _fat_ptr _tmp69A=({const char*_tmpC7="expecting either ";_tag_fat(_tmpC7,sizeof(char),18U);});_tmp573.f1=_tmp69A;});_tmp573;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp572;_tmp572.tag=2U,_tmp572.f1=(void*)t1;_tmp572;});struct Cyc_Warn_String_Warn_Warg_struct _tmpC4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp571;_tmp571.tag=0U,({struct _fat_ptr _tmp69B=({const char*_tmpC6=" or int but found ";_tag_fat(_tmpC6,sizeof(char),19U);});_tmp571.f1=_tmp69B;});_tmp571;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpC5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp570;_tmp570.tag=2U,_tmp570.f1=(void*)t2;_tmp570;});void*_tmpC1[4U];_tmpC1[0]=& _tmpC2,_tmpC1[1]=& _tmpC3,_tmpC1[2]=& _tmpC4,_tmpC1[3]=& _tmpC5;({unsigned _tmp69C=e2->loc;Cyc_Tcexp_err_and_explain(_tmp69C,_tag_fat(_tmpC1,sizeof(void*),4U));});});
return t1;}
# 449
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_tmp69D=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_tmp69D,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({void*_tmpC8=0U;({unsigned _tmp69F=e1->loc;struct _fat_ptr _tmp69E=({const char*_tmpC9="coercing fat pointer to thin pointer to support subtraction";_tag_fat(_tmpC9,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp69F,_tmp69E,_tag_fat(_tmpC8,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmp6A3=e2;Cyc_Tcutil_unchecked_cast(_tmp6A3,({void*_tmp6A2=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp6A1=
Cyc_Tcutil_pointer_region(t2);
# 454
struct Cyc_Absyn_Tqual _tmp6A0=
# 456
Cyc_Absyn_empty_tqual(0U);
# 454
Cyc_Absyn_star_type(_tmp6A2,_tmp6A1,_tmp6A0,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 459
({void*_tmpCA=0U;({unsigned _tmp6A5=e1->loc;struct _fat_ptr _tmp6A4=({const char*_tmpCB="thin pointer subtraction!";_tag_fat(_tmpCB,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp6A5,_tmp6A4,_tag_fat(_tmpCA,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 462
({void*_tmpCC=0U;({unsigned _tmp6A7=e1->loc;struct _fat_ptr _tmp6A6=({const char*_tmpCD="coercing thin pointer to integer to support subtraction";_tag_fat(_tmpCD,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp6A7,_tmp6A6,_tag_fat(_tmpCC,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 465
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpCE=0U;({unsigned _tmp6A9=e1->loc;struct _fat_ptr _tmp6A8=({const char*_tmpCF="coercing pointer to integer to support subtraction";_tag_fat(_tmpCF,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp6A9,_tmp6A8,_tag_fat(_tmpCE,sizeof(void*),0U));});});
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
void*_tmp6AA=t1;Cyc_Unify_unify(_tmp6AA,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 486
if(({struct Cyc_RgnOrder_RgnPO*_tmp6AD=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6AC=loc;void*_tmp6AB=t2;Cyc_Tcutil_silent_castable(_tmp6AD,_tmp6AC,_tmp6AB,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 490
if(({struct Cyc_RgnOrder_RgnPO*_tmp6B0=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp6AF=loc;void*_tmp6AE=t1;Cyc_Tcutil_silent_castable(_tmp6B0,_tmp6AF,_tmp6AE,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}
# 494
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 499
{struct _tuple0 _tmpD0=({struct _tuple0 _tmp574;({void*_tmp6B2=Cyc_Tcutil_compress(t1);_tmp574.f1=_tmp6B2;}),({void*_tmp6B1=Cyc_Tcutil_compress(t2);_tmp574.f2=_tmp6B1;});_tmp574;});struct _tuple0 _stmttmpB=_tmpD0;struct _tuple0 _tmpD1=_stmttmpB;void*_tmpD3;void*_tmpD2;switch(*((int*)_tmpD1.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f2)->tag == 3U){_LL1: _tmpD2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f1)->f1).elt_type;_tmpD3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f2)->f1).elt_type;_LL2: {void*t1a=_tmpD2;void*t2a=_tmpD3;
# 501
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD1.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD1.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD1.f2)->f1)->tag == 3U){_LL3: _LL4:
# 505
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 509
({struct Cyc_Warn_String_Warn_Warg_struct _tmpD5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp578;_tmp578.tag=0U,({struct _fat_ptr _tmp6B3=({const char*_tmpDA="comparison not allowed between ";_tag_fat(_tmpDA,sizeof(char),32U);});_tmp578.f1=_tmp6B3;});_tmp578;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp577;_tmp577.tag=2U,_tmp577.f1=(void*)t1;_tmp577;});struct Cyc_Warn_String_Warn_Warg_struct _tmpD7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp576;_tmp576.tag=0U,({struct _fat_ptr _tmp6B4=({const char*_tmpD9=" and ";_tag_fat(_tmpD9,sizeof(char),6U);});_tmp576.f1=_tmp6B4;});_tmp576;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpD8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp575;_tmp575.tag=2U,_tmp575.f1=(void*)t2;_tmp575;});void*_tmpD4[4U];_tmpD4[0]=& _tmpD5,_tmpD4[1]=& _tmpD6,_tmpD4[2]=& _tmpD7,_tmpD4[3]=& _tmpD8;({unsigned _tmp6B5=loc;Cyc_Tcexp_err_and_explain(_tmp6B5,_tag_fat(_tmpD4,sizeof(void*),4U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 515
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 517
enum Cyc_Absyn_Primop _tmpDB=p;switch(_tmpDB){case Cyc_Absyn_Plus: _LL1: _LL2:
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmpDD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp579;_tmp579.tag=0U,({struct _fat_ptr _tmp6B6=({const char*_tmpDE="bad binary primop";_tag_fat(_tmpDE,sizeof(char),18U);});_tmp579.f1=_tmp6B6;});_tmp579;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmpDC,sizeof(void*),1U));});}_LL0:;}
# 545
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 553
if((int)p == (int)2U &&((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpDF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e=_tmpDF;
void*_tmpE0=Cyc_Tcexp_tcExp(te,topt,e);void*t=_tmpE0;
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _tmpE2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57B;_tmp57B.tag=0U,({struct _fat_ptr _tmp6B7=({const char*_tmpE4="expecting numeric type but found ";_tag_fat(_tmpE4,sizeof(char),34U);});_tmp57B.f1=_tmp6B7;});_tmp57B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmpE3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp57A;_tmp57A.tag=2U,_tmp57A.f1=(void*)t;_tmp57A;});void*_tmpE1[2U];_tmpE1[0]=& _tmpE2,_tmpE1[1]=& _tmpE3;({unsigned _tmp6B8=e->loc;Cyc_Warn_err2(_tmp6B8,_tag_fat(_tmpE1,sizeof(void*),2U));});});
return t;}
# 560
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpE5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _stmttmpC=_tmpE5;int _tmpE6=_stmttmpC;switch(_tmpE6){case 0U: _LL1: _LL2:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpE8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57C;_tmp57C.tag=0U,({struct _fat_ptr _tmp6B9=({const char*_tmpE9="primitive operator has 0 arguments";_tag_fat(_tmpE9,sizeof(char),35U);});_tmp57C.f1=_tmp6B9;});_tmp57C;});void*_tmpE7[1U];_tmpE7[0]=& _tmpE8;({struct Cyc_Tcenv_Tenv*_tmp6BC=te;unsigned _tmp6BB=loc;void**_tmp6BA=topt;Cyc_Tcexp_expr_err(_tmp6BC,_tmp6BB,_tmp6BA,_tag_fat(_tmpE7,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LL0;case 2U: _LL5: _LL6:
 t=({struct Cyc_Tcenv_Tenv*_tmp6C1=te;unsigned _tmp6C0=loc;void**_tmp6BF=topt;enum Cyc_Absyn_Primop _tmp6BE=p;struct Cyc_Absyn_Exp*_tmp6BD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp6C1,_tmp6C0,_tmp6BF,_tmp6BE,_tmp6BD,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});goto _LL0;default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmpEB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp57D;_tmp57D.tag=0U,({struct _fat_ptr _tmp6C2=({const char*_tmpEC="primitive operator has > 2 arguments";_tag_fat(_tmpEC,sizeof(char),37U);});_tmp57D.f1=_tmp6C2;});_tmp57D;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEB;({struct Cyc_Tcenv_Tenv*_tmp6C5=te;unsigned _tmp6C4=loc;void**_tmp6C3=topt;Cyc_Tcexp_expr_err(_tmp6C5,_tmp6C4,_tmp6C3,_tag_fat(_tmpEA,sizeof(void*),1U));});});}_LL0:;}
# 568
return t;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 571
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
void*_tmpED=Cyc_Tcutil_compress(t);void*t=_tmpED;
void*_tmpEE=t;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Tqual _tmpF1;void*_tmpF0;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Datatypefield*_tmpF3;struct Cyc_Absyn_Aggrdecl*_tmpF4;switch(*((int*)_tmpEE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpF4=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF4;
# 575
if(ad->impl == 0){
({void*_tmpF5=0U;({unsigned _tmp6C7=loc;struct _fat_ptr _tmp6C6=({const char*_tmpF6="attempt to write an abstract aggregate";_tag_fat(_tmpF6,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6C7,_tmp6C6,_tag_fat(_tmpF5,sizeof(void*),0U));});});
return 0;}
# 579
_tmpF2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpF3=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpEE)->f1)->f1).KnownDatatypefield).val).f2;_LL6: {struct Cyc_Absyn_Datatypefield*df=_tmpF3;
# 591
{struct Cyc_List_List*_tmpFB=df->typs;struct Cyc_List_List*fs=_tmpFB;for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmpFC=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpD=_tmpFC;struct _tuple17*_tmpFD=_stmttmpD;void*_tmpFF;struct Cyc_Absyn_Tqual _tmpFE;_LLE: _tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;_LLF: {struct Cyc_Absyn_Tqual tq=_tmpFE;void*t=_tmpFF;
if(tq.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp57E;_tmp57E.tag=0U,({struct _fat_ptr _tmp6C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(df->name));_tmp57E.f1=_tmp6C8;});_tmp57E;});void*_tmp100[1U];_tmp100[0]=& _tmp102;({unsigned _tmp6CA=loc;struct _fat_ptr _tmp6C9=({const char*_tmp101="attempt to over-write a datatype field (%s) with a const member";_tag_fat(_tmp101,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp6CA,_tmp6C9,_tag_fat(_tmp100,sizeof(void*),1U));});});
return 0;}
# 597
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 599
return 1;}}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpF2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEE)->f2;_LL4: {struct Cyc_List_List*fs=_tmpF2;
# 581
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmpF7=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmpF7;
if((f->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFA=({struct Cyc_String_pa_PrintArg_struct _tmp57F;_tmp57F.tag=0U,_tmp57F.f1=(struct _fat_ptr)((struct _fat_ptr)*f->name);_tmp57F;});void*_tmpF8[1U];_tmpF8[0]=& _tmpFA;({unsigned _tmp6CC=loc;struct _fat_ptr _tmp6CB=({const char*_tmpF9="attempt to over-write an aggregate with const member %s";_tag_fat(_tmpF9,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp6CC,_tmp6CB,_tag_fat(_tmpF8,sizeof(void*),1U));});});
return 0;}
# 587
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 589
return 1;}case 4U: _LL7: _tmpF0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEE)->f1).elt_type;_tmpF1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEE)->f1).tq;_LL8: {void*elt_type=_tmpF0;struct Cyc_Absyn_Tqual tq=_tmpF1;
# 601
if(tq.real_const){
({void*_tmp103=0U;({unsigned _tmp6CE=loc;struct _fat_ptr _tmp6CD=({const char*_tmp104="attempt to over-write a const array";_tag_fat(_tmp104,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6CE,_tmp6CD,_tag_fat(_tmp103,sizeof(void*),0U));});});
return 0;}
# 605
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}case 6U: _LL9: _tmpEF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpEE)->f1;_LLA: {struct Cyc_List_List*fs=_tmpEF;
# 607
for(0;fs != 0;fs=fs->tl){
struct _tuple17*_tmp105=(struct _tuple17*)fs->hd;struct _tuple17*_stmttmpE=_tmp105;struct _tuple17*_tmp106=_stmttmpE;void*_tmp108;struct Cyc_Absyn_Tqual _tmp107;_LL11: _tmp107=_tmp106->f1;_tmp108=_tmp106->f2;_LL12: {struct Cyc_Absyn_Tqual tq=_tmp107;void*t=_tmp108;
if(tq.real_const){
({void*_tmp109=0U;({unsigned _tmp6D0=loc;struct _fat_ptr _tmp6CF=({const char*_tmp10A="attempt to over-write a tuple field with a const member";_tag_fat(_tmp10A,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp6D0,_tmp6CF,_tag_fat(_tmp109,sizeof(void*),0U));});});
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
{void*_tmp10B=e->r;void*_stmttmpF=_tmp10B;void*_tmp10C=_stmttmpF;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct _fat_ptr*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct _fat_ptr*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Vardecl*_tmp116;struct Cyc_Absyn_Vardecl*_tmp117;struct Cyc_Absyn_Vardecl*_tmp118;struct Cyc_Absyn_Vardecl*_tmp119;switch(*((int*)_tmp10C)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)){case 3U: _LL1: _tmp119=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp119;
_tmp118=vd;goto _LL4;}case 4U: _LL3: _tmp118=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp118;
_tmp117=vd;goto _LL6;}case 5U: _LL5: _tmp117=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_tmp117;
_tmp116=vd;goto _LL8;}case 1U: _LL7: _tmp116=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmp116;
if(!(vd->tq).real_const)return;goto _LL0;}default: goto _LL15;}case 23U: _LL9: _tmp114=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_tmp115=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10C)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_Absyn_Exp*e2=_tmp115;
# 633
{void*_tmp11A=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp10=_tmp11A;void*_tmp11B=_stmttmp10;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Tqual _tmp11D;struct Cyc_Absyn_Tqual _tmp11E;switch(*((int*)_tmp11B)){case 3U: _LL18: _tmp11E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B)->f1).elt_tq;_LL19: {struct Cyc_Absyn_Tqual tq=_tmp11E;
_tmp11D=tq;goto _LL1B;}case 4U: _LL1A: _tmp11D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_tmp11D;
if(!tq.real_const)return;goto _LL17;}case 6U: _LL1C: _tmp11C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp11B)->f1;_LL1D: {struct Cyc_List_List*ts=_tmp11C;
# 637
struct _tuple16 _tmp11F=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp11=_tmp11F;struct _tuple16 _tmp120=_stmttmp11;int _tmp122;unsigned _tmp121;_LL21: _tmp121=_tmp120.f1;_tmp122=_tmp120.f2;_LL22: {unsigned i=_tmp121;int known=_tmp122;
if(!known){
({void*_tmp123=0U;({unsigned _tmp6D2=e->loc;struct _fat_ptr _tmp6D1=({const char*_tmp124="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp124,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6D2,_tmp6D1,_tag_fat(_tmp123,sizeof(void*),0U));});});
return;}
# 642
{struct _handler_cons _tmp125;_push_handler(& _tmp125);{int _tmp127=0;if(setjmp(_tmp125.handler))_tmp127=1;if(!_tmp127){
{struct _tuple17*_tmp128=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i);struct _tuple17*_stmttmp12=_tmp128;struct _tuple17*_tmp129=_stmttmp12;struct Cyc_Absyn_Tqual _tmp12A;_LL24: _tmp12A=_tmp129->f1;_LL25: {struct Cyc_Absyn_Tqual tq=_tmp12A;
if(!tq.real_const){_npop_handler(0U);return;}}}
# 643
;_pop_handler();}else{void*_tmp126=(void*)Cyc_Core_get_exn_thrown();void*_tmp12B=_tmp126;void*_tmp12C;if(((struct Cyc_List_Nth_exn_struct*)_tmp12B)->tag == Cyc_List_Nth){_LL27: _LL28:
# 645
 return;}else{_LL29: _tmp12C=_tmp12B;_LL2A: {void*exn=_tmp12C;(int)_rethrow(exn);}}_LL26:;}}}
goto _LL17;}}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 649
goto _LL0;}case 21U: _LLB: _tmp112=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_tmp113=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10C)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp112;struct _fat_ptr*f=_tmp113;
# 651
{void*_tmp12D=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp13=_tmp12D;void*_tmp12E=_stmttmp13;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_Aggrdecl**_tmp130;switch(*((int*)_tmp12E)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp130=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12E)->f1)->f1).KnownAggr).val;_LL2D: {struct Cyc_Absyn_Aggrdecl**adp=_tmp130;
# 653
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp12F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12E)->f2;_LL2F: {struct Cyc_List_List*fs=_tmp12F;
# 658
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 663
goto _LL0;}case 22U: _LLD: _tmp110=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_tmp111=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10C)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp110;struct _fat_ptr*f=_tmp111;
# 665
{void*_tmp131=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp14=_tmp131;void*_tmp132=_stmttmp14;struct Cyc_Absyn_Tqual _tmp134;void*_tmp133;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132)->tag == 3U){_LL33: _tmp133=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132)->f1).elt_type;_tmp134=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132)->f1).elt_tq;_LL34: {void*elt_typ=_tmp133;struct Cyc_Absyn_Tqual tq=_tmp134;
# 667
if(!tq.real_const){
void*_tmp135=Cyc_Tcutil_compress(elt_typ);void*_stmttmp15=_tmp135;void*_tmp136=_stmttmp15;struct Cyc_List_List*_tmp137;struct Cyc_Absyn_Aggrdecl**_tmp138;switch(*((int*)_tmp136)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp136)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp136)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp138=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp136)->f1)->f1).KnownAggr).val;_LL39: {struct Cyc_Absyn_Aggrdecl**adp=_tmp138;
# 670
struct Cyc_Absyn_Aggrfield*sf=
adp == 0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp137=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp136)->f2;_LL3B: {struct Cyc_List_List*fs=_tmp137;
# 675
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf == 0 || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 681
goto _LL32;}}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 684
goto _LL0;}case 20U: _LLF: _tmp10F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp10F;
# 686
{void*_tmp139=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp16=_tmp139;void*_tmp13A=_stmttmp16;struct Cyc_Absyn_Tqual _tmp13B;struct Cyc_Absyn_Tqual _tmp13C;switch(*((int*)_tmp13A)){case 3U: _LL3F: _tmp13C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13A)->f1).elt_tq;_LL40: {struct Cyc_Absyn_Tqual tq=_tmp13C;
_tmp13B=tq;goto _LL42;}case 4U: _LL41: _tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13A)->f1).tq;_LL42: {struct Cyc_Absyn_Tqual tq=_tmp13B;
if(!tq.real_const)return;goto _LL3E;}default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 691
goto _LL0;}case 12U: _LL11: _tmp10E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp10E;
_tmp10D=e1;goto _LL14;}case 13U: _LL13: _tmp10D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10C)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp10D;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 696
({struct Cyc_String_pa_PrintArg_struct _tmp13F=({struct Cyc_String_pa_PrintArg_struct _tmp580;_tmp580.tag=0U,({struct _fat_ptr _tmp6D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp580.f1=_tmp6D3;});_tmp580;});void*_tmp13D[1U];_tmp13D[0]=& _tmp13F;({unsigned _tmp6D5=e->loc;struct _fat_ptr _tmp6D4=({const char*_tmp13E="attempt to write a const location: %s";_tag_fat(_tmp13E,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp6D5,_tmp6D4,_tag_fat(_tmp13D,sizeof(void*),1U));});});}
# 699
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 702
({struct Cyc_Tcenv_Tenv*_tmp6D6=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp6D6,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp141=({struct Cyc_Warn_String_Warn_Warg_struct _tmp581;_tmp581.tag=0U,({struct _fat_ptr _tmp6D7=({const char*_tmp142="increment/decrement of non-lvalue";_tag_fat(_tmp142,sizeof(char),34U);});_tmp581.f1=_tmp6D7;});_tmp581;});void*_tmp140[1U];_tmp140[0]=& _tmp141;({struct Cyc_Tcenv_Tenv*_tmp6DA=te;unsigned _tmp6D9=loc;void**_tmp6D8=topt;Cyc_Tcexp_expr_err(_tmp6DA,_tmp6D9,_tmp6D8,_tag_fat(_tmp140,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 708
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i == (int)0U ||(int)i == (int)1U)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp143=0U;({unsigned _tmp6DC=e->loc;struct _fat_ptr _tmp6DB=({const char*_tmp144="can't perform arithmetic on abstract pointer type";_tag_fat(_tmp144,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6DC,_tmp6DB,_tag_fat(_tmp143,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp145=0U;({unsigned _tmp6DE=e->loc;struct _fat_ptr _tmp6DD=({const char*_tmp146="can't perform arithmetic on non-aliasing pointer type";_tag_fat(_tmp146,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp6DE,_tmp6DD,_tag_fat(_tmp145,sizeof(void*),0U));});});}else{
# 717
({struct Cyc_String_pa_PrintArg_struct _tmp149=({struct Cyc_String_pa_PrintArg_struct _tmp582;_tmp582.tag=0U,({struct _fat_ptr _tmp6DF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp582.f1=_tmp6DF;});_tmp582;});void*_tmp147[1U];_tmp147[0]=& _tmp149;({unsigned _tmp6E1=e->loc;struct _fat_ptr _tmp6E0=({const char*_tmp148="expecting arithmetic or ? type but found %s";_tag_fat(_tmp148,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp6E1,_tmp6E0,_tag_fat(_tmp147,sizeof(void*),1U));});});}}
# 719
return t;}}
# 723
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 725
({struct Cyc_Tcenv_Tenv*_tmp6E3=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp6E2=e1;Cyc_Tcexp_tcTest(_tmp6E3,_tmp6E2,({const char*_tmp14A="conditional expression";_tag_fat(_tmp14A,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 732
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp14B=({struct Cyc_List_List _tmp588;_tmp588.hd=e3,_tmp588.tl=0;_tmp588;});struct Cyc_List_List l1=_tmp14B;
struct Cyc_List_List _tmp14C=({struct Cyc_List_List _tmp587;_tmp587.hd=e2,_tmp587.tl=& l1;_tmp587;});struct Cyc_List_List l2=_tmp14C;
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6E4=Cyc_Tcenv_curr_rgnpo(te);Cyc_Tcutil_coerce_list(_tmp6E4,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp586;_tmp586.tag=0U,({struct _fat_ptr _tmp6E5=({const char*_tmp153="conditional clause types do not match: ";_tag_fat(_tmp153,sizeof(char),40U);});_tmp586.f1=_tmp6E5;});_tmp586;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp14F=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp585;_tmp585.tag=3U,_tmp585.f1=(void*)e2->topt;_tmp585;});struct Cyc_Warn_String_Warn_Warg_struct _tmp150=({struct Cyc_Warn_String_Warn_Warg_struct _tmp584;_tmp584.tag=0U,({
struct _fat_ptr _tmp6E6=({const char*_tmp152=" != ";_tag_fat(_tmp152,sizeof(char),5U);});_tmp584.f1=_tmp6E6;});_tmp584;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp151=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp583;_tmp583.tag=3U,_tmp583.f1=(void*)e3->topt;_tmp583;});void*_tmp14D[4U];_tmp14D[0]=& _tmp14E,_tmp14D[1]=& _tmp14F,_tmp14D[2]=& _tmp150,_tmp14D[3]=& _tmp151;({unsigned _tmp6E7=loc;Cyc_Tcexp_err_and_explain(_tmp6E7,_tag_fat(_tmp14D,sizeof(void*),4U));});});
return t;}}}
# 742
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 744
({struct Cyc_Tcenv_Tenv*_tmp6E9=te;struct Cyc_Absyn_Exp*_tmp6E8=e1;Cyc_Tcexp_tcTest(_tmp6E9,_tmp6E8,({const char*_tmp154="logical-and expression";_tag_fat(_tmp154,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6EB=te;struct Cyc_Absyn_Exp*_tmp6EA=e2;Cyc_Tcexp_tcTest(_tmp6EB,_tmp6EA,({const char*_tmp155="logical-and expression";_tag_fat(_tmp155,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 750
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 752
({struct Cyc_Tcenv_Tenv*_tmp6ED=te;struct Cyc_Absyn_Exp*_tmp6EC=e1;Cyc_Tcexp_tcTest(_tmp6ED,_tmp6EC,({const char*_tmp156="logical-or expression";_tag_fat(_tmp156,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp6EF=te;struct Cyc_Absyn_Exp*_tmp6EE=e2;Cyc_Tcexp_tcTest(_tmp6EF,_tmp6EE,({const char*_tmp157="logical-or expression";_tag_fat(_tmp157,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 758
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 765
({struct Cyc_Tcenv_Tenv*_tmp6F0=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp6F0,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 770
{void*_tmp158=Cyc_Tcutil_compress(t1);void*_stmttmp17=_tmp158;void*_tmp159=_stmttmp17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159)->tag == 4U){_LL1: _LL2:
({void*_tmp15A=0U;({unsigned _tmp6F2=loc;struct _fat_ptr _tmp6F1=({const char*_tmp15B="cannot assign to an array";_tag_fat(_tmp15B,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp6F2,_tmp6F1,_tag_fat(_tmp15A,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 775
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp15C=0U;({unsigned _tmp6F4=loc;struct _fat_ptr _tmp6F3=({const char*_tmp15D="type is abstract (can't determine size)";_tag_fat(_tmp15D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6F4,_tmp6F3,_tag_fat(_tmp15C,sizeof(void*),0U));});});
# 779
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp15F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp589;_tmp589.tag=0U,({struct _fat_ptr _tmp6F5=({const char*_tmp160="assignment to non-lvalue";_tag_fat(_tmp160,sizeof(char),25U);});_tmp589.f1=_tmp6F5;});_tmp589;});void*_tmp15E[1U];_tmp15E[0]=& _tmp15F;({struct Cyc_Tcenv_Tenv*_tmp6F8=te;unsigned _tmp6F7=loc;void**_tmp6F6=topt;Cyc_Tcexp_expr_err(_tmp6F8,_tmp6F7,_tmp6F6,_tag_fat(_tmp15E,sizeof(void*),1U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e2))
Cyc_Tcexp_unique_consume_err(e2->loc);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp6FA=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp6F9=e2;Cyc_Tcutil_coerce_assign(_tmp6FA,_tmp6F9,t1);})){
void*_tmp161=({struct Cyc_Warn_String_Warn_Warg_struct _tmp163=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58D;_tmp58D.tag=0U,({struct _fat_ptr _tmp6FB=({const char*_tmp168="type mismatch: ";_tag_fat(_tmp168,sizeof(char),16U);});_tmp58D.f1=_tmp6FB;});_tmp58D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp164=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58C;_tmp58C.tag=2U,_tmp58C.f1=(void*)t1;_tmp58C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp165=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58B;_tmp58B.tag=0U,({struct _fat_ptr _tmp6FC=({const char*_tmp167=" != ";_tag_fat(_tmp167,sizeof(char),5U);});_tmp58B.f1=_tmp6FC;});_tmp58B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp166=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58A;_tmp58A.tag=2U,_tmp58A.f1=(void*)t2;_tmp58A;});void*_tmp162[4U];_tmp162[0]=& _tmp163,_tmp162[1]=& _tmp164,_tmp162[2]=& _tmp165,_tmp162[3]=& _tmp166;({struct Cyc_Tcenv_Tenv*_tmp6FF=te;unsigned _tmp6FE=loc;void**_tmp6FD=topt;Cyc_Tcexp_expr_err(_tmp6FF,_tmp6FE,_tmp6FD,_tag_fat(_tmp162,sizeof(void*),4U));});});void*result=_tmp161;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}}else{
# 792
enum Cyc_Absyn_Primop _tmp169=(enum Cyc_Absyn_Primop)po->v;enum Cyc_Absyn_Primop p=_tmp169;
struct Cyc_Absyn_Exp*_tmp16A=Cyc_Absyn_copy_exp(e1);struct Cyc_Absyn_Exp*e1copy=_tmp16A;
void*_tmp16B=Cyc_Tcexp_tcBinPrimop(te,loc,0,p,e1copy,e2);void*t_result=_tmp16B;
if((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))
t_result=t1;else{
if(!(Cyc_Unify_unify(t_result,t1)|| Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp16C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp592;_tmp592.tag=0U,({
struct _fat_ptr _tmp700=({const char*_tmp175="Cannot use this operator in an assignment when ";_tag_fat(_tmp175,sizeof(char),48U);});_tmp592.f1=_tmp700;});_tmp592;});struct Cyc_Warn_String_Warn_Warg_struct _tmp16F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp591;_tmp591.tag=0U,({
struct _fat_ptr _tmp701=({const char*_tmp174="the arguments have types ";_tag_fat(_tmp174,sizeof(char),26U);});_tmp591.f1=_tmp701;});_tmp591;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp170=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp590;_tmp590.tag=2U,_tmp590.f1=(void*)t1;_tmp590;});struct Cyc_Warn_String_Warn_Warg_struct _tmp171=({struct Cyc_Warn_String_Warn_Warg_struct _tmp58F;_tmp58F.tag=0U,({struct _fat_ptr _tmp702=({const char*_tmp173=" and ";_tag_fat(_tmp173,sizeof(char),6U);});_tmp58F.f1=_tmp702;});_tmp58F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp172=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp58E;_tmp58E.tag=2U,_tmp58E.f1=(void*)t2;_tmp58E;});void*_tmp16D[5U];_tmp16D[0]=& _tmp16E,_tmp16D[1]=& _tmp16F,_tmp16D[2]=& _tmp170,_tmp16D[3]=& _tmp171,_tmp16D[4]=& _tmp172;({struct Cyc_Tcenv_Tenv*_tmp705=te;unsigned _tmp704=loc;void**_tmp703=topt;Cyc_Tcexp_expr_err(_tmp705,_tmp704,_tmp703,_tag_fat(_tmp16D,sizeof(void*),5U));});});
# 798
void*result=_tmp16C;
# 801
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}}
# 805
return t_result;}
# 807
return t1;}}}
# 811
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp706=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp706,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp708=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp707=topt;Cyc_Tcexp_tcExp(_tmp708,_tmp707,e2);});
return(void*)_check_null(e2->topt);}
# 818
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 821
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 824
{void*_tmp176=Cyc_Tcutil_compress(t1);void*_stmttmp18=_tmp176;void*_tmp177=_stmttmp18;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->f1)->f1 == 0){_LL1: _LL2:
# 826
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp177)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
# 829
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 833
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp178=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp19=_tmp178;struct Cyc_Absyn_Datatypefield _tmp179=_stmttmp19;enum Cyc_Absyn_Scope _tmp17D;unsigned _tmp17C;struct Cyc_List_List*_tmp17B;struct _tuple1*_tmp17A;_LLA: _tmp17A=_tmp179.name;_tmp17B=_tmp179.typs;_tmp17C=_tmp179.loc;_tmp17D=_tmp179.sc;_LLB: {struct _tuple1*n=_tmp17A;struct Cyc_List_List*typs=_tmp17B;unsigned loc=_tmp17C;enum Cyc_Absyn_Scope sc=_tmp17D;
# 836
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 839
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 843
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp17E=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _stmttmp1A=_tmp17E;struct Cyc_Absyn_Datatypefield _tmp17F=_stmttmp1A;enum Cyc_Absyn_Scope _tmp183;unsigned _tmp182;struct Cyc_List_List*_tmp181;struct _tuple1*_tmp180;_LLD: _tmp180=_tmp17F.name;_tmp181=_tmp17F.typs;_tmp182=_tmp17F.loc;_tmp183=_tmp17F.sc;_LLE: {struct _tuple1*n=_tmp180;struct Cyc_List_List*typs=_tmp181;unsigned loc=_tmp182;enum Cyc_Absyn_Scope sc=_tmp183;
# 846
if(typs == 0 || typs->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)typs->hd)).f2);
# 850
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_tmp70A=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp709=e;Cyc_Tcutil_coerce_arg(_tmp70A,_tmp709,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 854
({struct Cyc_Warn_String_Warn_Warg_struct _tmp185=({struct Cyc_Warn_String_Warn_Warg_struct _tmp596;_tmp596.tag=0U,({struct _fat_ptr _tmp70B=({const char*_tmp18A="can't find a field in ";_tag_fat(_tmp18A,sizeof(char),23U);});_tmp596.f1=_tmp70B;});_tmp596;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp186=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp595;_tmp595.tag=2U,_tmp595.f1=(void*)tu;_tmp595;});struct Cyc_Warn_String_Warn_Warg_struct _tmp187=({struct Cyc_Warn_String_Warn_Warg_struct _tmp594;_tmp594.tag=0U,({struct _fat_ptr _tmp70C=({const char*_tmp189=" to inject a value of type ";_tag_fat(_tmp189,sizeof(char),28U);});_tmp594.f1=_tmp70C;});_tmp594;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp188=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp593;_tmp593.tag=2U,_tmp593.f1=(void*)t1;_tmp593;});void*_tmp184[4U];_tmp184[0]=& _tmp185,_tmp184[1]=& _tmp186,_tmp184[2]=& _tmp187,_tmp184[3]=& _tmp188;({unsigned _tmp70D=e->loc;Cyc_Warn_err2(_tmp70D,_tag_fat(_tmp184,sizeof(void*),4U));});});
return 0;}
# 859
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 865
struct Cyc_List_List*_tmp18B=args;struct Cyc_List_List*es=_tmp18B;
int _tmp18C=0;int arg_cnt=_tmp18C;
struct Cyc_Tcenv_Tenv*_tmp18D=Cyc_Tcenv_new_block(loc,te_orig);struct Cyc_Tcenv_Tenv*te=_tmp18D;
struct Cyc_Tcenv_Tenv*_tmp18E=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_tmp18E;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 874
void*_tmp18F=t;void*_tmp195;void*_tmp194;void*_tmp193;void*_tmp192;struct Cyc_Absyn_Tqual _tmp191;void*_tmp190;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->tag == 3U){_LL1: _tmp190=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).elt_type;_tmp191=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).elt_tq;_tmp192=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).rgn;_tmp193=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).nullable;_tmp194=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).bounds;_tmp195=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).zero_term;_LL2: {void*t1=_tmp190;struct Cyc_Absyn_Tqual tq=_tmp191;void*rgn=_tmp192;void*x=_tmp193;void*b=_tmp194;void*zt=_tmp195;
# 879
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 881
Cyc_Tcutil_check_nonzero_bound(loc,b);{
void*_tmp196=Cyc_Tcutil_compress(t1);void*_stmttmp1B=_tmp196;void*_tmp197=_stmttmp1B;struct Cyc_List_List*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_VarargInfo*_tmp19E;int _tmp19D;struct Cyc_List_List*_tmp19C;void*_tmp19B;struct Cyc_Absyn_Tqual _tmp19A;void*_tmp199;struct Cyc_List_List*_tmp198;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->tag == 5U){_LL6: _tmp198=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).tvars;_tmp199=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).effect;_tmp19A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ret_tqual;_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ret_type;_tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).args;_tmp19D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).c_varargs;_tmp19E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).cyc_varargs;_tmp19F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).rgn_po;_tmp1A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).attributes;_tmp1A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).requires_clause;_tmp1A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).requires_relns;_tmp1A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ensures_clause;_tmp1A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp197)->f1).ensures_relns;_LL7: {struct Cyc_List_List*tvars=_tmp198;void*eff=_tmp199;struct Cyc_Absyn_Tqual res_tq=_tmp19A;void*res_typ=_tmp19B;struct Cyc_List_List*args_info=_tmp19C;int c_vararg=_tmp19D;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmp19E;struct Cyc_List_List*rgn_po=_tmp19F;struct Cyc_List_List*atts=_tmp1A0;struct Cyc_Absyn_Exp*req=_tmp1A1;struct Cyc_List_List*req_relns=_tmp1A2;struct Cyc_Absyn_Exp*ens=_tmp1A3;struct Cyc_List_List*ens_relns=_tmp1A4;
# 886
if(tvars != 0 || rgn_po != 0)
({void*_tmp1A5=0U;({unsigned _tmp70F=e->loc;struct _fat_ptr _tmp70E=({const char*_tmp1A6="function should have been instantiated prior to use -- probably a compiler bug";_tag_fat(_tmp1A6,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp70F,_tmp70E,_tag_fat(_tmp1A5,sizeof(void*),0U));});});
# 890
if(topt != 0)Cyc_Unify_unify(res_typ,*topt);
# 892
while(es != 0 && args_info != 0){
# 894
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f3;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp711=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp710=e1;Cyc_Tcutil_coerce_arg(_tmp711,_tmp710,t2,& alias_coercion);})){
struct _fat_ptr s0=({const char*_tmp1B6="actual argument has type ";_tag_fat(_tmp1B6,sizeof(char),26U);});
struct _fat_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _fat_ptr s2=({const char*_tmp1B5=" but formal has type ";_tag_fat(_tmp1B5,sizeof(char),22U);});
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp714=({unsigned long _tmp713=({unsigned long _tmp712=Cyc_strlen((struct _fat_ptr)s0);_tmp712 + Cyc_strlen((struct _fat_ptr)s1);});_tmp713 + Cyc_strlen((struct _fat_ptr)s2);});_tmp714 + Cyc_strlen((struct _fat_ptr)s3);})>= (unsigned long)70)
({void*_tmp1A7=0U;({unsigned _tmp717=e1->loc;struct _fat_ptr _tmp716=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1AA=({struct Cyc_String_pa_PrintArg_struct _tmp59A;_tmp59A.tag=0U,_tmp59A.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp59A;});struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp599;_tmp599.tag=0U,_tmp599.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp599;});struct Cyc_String_pa_PrintArg_struct _tmp1AC=({struct Cyc_String_pa_PrintArg_struct _tmp598;_tmp598.tag=0U,_tmp598.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp598;});struct Cyc_String_pa_PrintArg_struct _tmp1AD=({struct Cyc_String_pa_PrintArg_struct _tmp597;_tmp597.tag=0U,_tmp597.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp597;});void*_tmp1A8[4U];_tmp1A8[0]=& _tmp1AA,_tmp1A8[1]=& _tmp1AB,_tmp1A8[2]=& _tmp1AC,_tmp1A8[3]=& _tmp1AD;({struct _fat_ptr _tmp715=({const char*_tmp1A9="%s\n\t%s\n%s\n\t%s.";_tag_fat(_tmp1A9,sizeof(char),15U);});Cyc_aprintf(_tmp715,_tag_fat(_tmp1A8,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp717,_tmp716,_tag_fat(_tmp1A7,sizeof(void*),0U));});});else{
# 906
({void*_tmp1AE=0U;({unsigned _tmp71A=e1->loc;struct _fat_ptr _tmp719=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp59E;_tmp59E.tag=0U,_tmp59E.f1=(struct _fat_ptr)((struct _fat_ptr)s0);_tmp59E;});struct Cyc_String_pa_PrintArg_struct _tmp1B2=({struct Cyc_String_pa_PrintArg_struct _tmp59D;_tmp59D.tag=0U,_tmp59D.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp59D;});struct Cyc_String_pa_PrintArg_struct _tmp1B3=({struct Cyc_String_pa_PrintArg_struct _tmp59C;_tmp59C.tag=0U,_tmp59C.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp59C;});struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp59B;_tmp59B.tag=0U,_tmp59B.f1=(struct _fat_ptr)((struct _fat_ptr)s3);_tmp59B;});void*_tmp1AF[4U];_tmp1AF[0]=& _tmp1B1,_tmp1AF[1]=& _tmp1B2,_tmp1AF[2]=& _tmp1B3,_tmp1AF[3]=& _tmp1B4;({struct _fat_ptr _tmp718=({const char*_tmp1B0="%s%s%s%s.";_tag_fat(_tmp1B0,sizeof(char),10U);});Cyc_aprintf(_tmp718,_tag_fat(_tmp1AF,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp71A,_tmp719,_tag_fat(_tmp1AE,sizeof(void*),0U));});});}
Cyc_Unify_unify((void*)_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 911
if(alias_coercion)
({struct Cyc_List_List*_tmp71B=({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->hd=(void*)arg_cnt,_tmp1B7->tl=*alias_arg_exps;_tmp1B7;});*alias_arg_exps=_tmp71B;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 922
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a != 0;a=a->tl){
void*_tmp1B8=(void*)a->hd;void*_stmttmp1C=_tmp1B8;void*_tmp1B9=_stmttmp1C;int _tmp1BC;int _tmp1BB;enum Cyc_Absyn_Format_Type _tmp1BA;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->tag == 19U){_LLB: _tmp1BA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->f1;_tmp1BB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->f2;_tmp1BC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1B9)->f3;_LLC: {enum Cyc_Absyn_Format_Type ft=_tmp1BA;int fmt_arg_pos=_tmp1BB;int arg_start_pos=_tmp1BC;
# 926
{struct _handler_cons _tmp1BD;_push_handler(& _tmp1BD);{int _tmp1BF=0;if(setjmp(_tmp1BD.handler))_tmp1BF=1;if(!_tmp1BF){
# 928
{struct Cyc_Absyn_Exp*_tmp1C0=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,fmt_arg_pos - 1);struct Cyc_Absyn_Exp*fmt_arg=_tmp1C0;
# 930
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos == 0)
fmt_args=0;else{
# 934
fmt_args=({struct Cyc_Core_Opt*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));({struct Cyc_List_List*_tmp71C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,arg_start_pos - 1);_tmp1C1->v=_tmp71C;});_tmp1C1;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1C2=ft;switch(_tmp1C2){case Cyc_Absyn_Printf_ft: _LL10: _LL11:
# 938
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 941
goto _LLF;case Cyc_Absyn_Scanf_ft: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
# 944
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 947
goto _LLF;}_LLF:;}}
# 928
;_pop_handler();}else{void*_tmp1BE=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C3=_tmp1BE;void*_tmp1C4;if(((struct Cyc_List_Nth_exn_struct*)_tmp1C3)->tag == Cyc_List_Nth){_LL17: _LL18:
# 949
({void*_tmp1C5=0U;({unsigned _tmp71E=loc;struct _fat_ptr _tmp71D=({const char*_tmp1C6="bad format arguments";_tag_fat(_tmp1C6,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp71E,_tmp71D,_tag_fat(_tmp1C5,sizeof(void*),0U));});});goto _LL16;}else{_LL19: _tmp1C4=_tmp1C3;_LL1A: {void*exn=_tmp1C4;(int)_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 954
if(args_info != 0)({void*_tmp1C7=0U;({unsigned _tmp720=loc;struct _fat_ptr _tmp71F=({const char*_tmp1C8="too few arguments for function";_tag_fat(_tmp1C8,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp720,_tmp71F,_tag_fat(_tmp1C7,sizeof(void*),0U));});});else{
# 956
if((es != 0 || c_vararg)|| cyc_vararg != 0){
if(c_vararg)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg == 0)
({void*_tmp1C9=0U;({unsigned _tmp722=loc;struct _fat_ptr _tmp721=({const char*_tmp1CA="too many arguments for function";_tag_fat(_tmp1CA,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp722,_tmp721,_tag_fat(_tmp1C9,sizeof(void*),0U));});});else{
# 963
struct Cyc_Absyn_VarargInfo _tmp1CB=*cyc_vararg;struct Cyc_Absyn_VarargInfo _stmttmp1D=_tmp1CB;struct Cyc_Absyn_VarargInfo _tmp1CC=_stmttmp1D;int _tmp1CE;void*_tmp1CD;_LL1C: _tmp1CD=_tmp1CC.type;_tmp1CE=_tmp1CC.inject;_LL1D: {void*vt=_tmp1CD;int inject=_tmp1CE;
struct Cyc_Absyn_VarargCallInfo*_tmp1CF=({struct Cyc_Absyn_VarargCallInfo*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->num_varargs=0,_tmp1E5->injectors=0,_tmp1E5->vai=cyc_vararg;_tmp1E5;});struct Cyc_Absyn_VarargCallInfo*vci=_tmp1CF;
# 967
*vararg_call_info=vci;
# 969
if(!inject)
# 971
for(0;es != 0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp724=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp723=e1;Cyc_Tcutil_coerce_arg(_tmp724,_tmp723,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A2;_tmp5A2.tag=0U,({struct _fat_ptr _tmp725=({const char*_tmp1D6="vararg requires type ";_tag_fat(_tmp1D6,sizeof(char),22U);});_tmp5A2.f1=_tmp725;});_tmp5A2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1D2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A1;_tmp5A1.tag=2U,_tmp5A1.f1=(void*)vt;_tmp5A1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A0;_tmp5A0.tag=0U,({
struct _fat_ptr _tmp726=({const char*_tmp1D5=" but argument has type ";_tag_fat(_tmp1D5,sizeof(char),24U);});_tmp5A0.f1=_tmp726;});_tmp5A0;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1D4=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp59F;_tmp59F.tag=3U,_tmp59F.f1=(void*)e1->topt;_tmp59F;});void*_tmp1D0[4U];_tmp1D0[0]=& _tmp1D1,_tmp1D0[1]=& _tmp1D2,_tmp1D0[2]=& _tmp1D3,_tmp1D0[3]=& _tmp1D4;({unsigned _tmp727=loc;Cyc_Tcexp_err_and_explain(_tmp727,_tag_fat(_tmp1D0,sizeof(void*),4U));});});
if(alias_coercion)
({struct Cyc_List_List*_tmp728=({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=(void*)arg_cnt,_tmp1D7->tl=*alias_arg_exps;_tmp1D7;});*alias_arg_exps=_tmp728;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vt)&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}else{
# 988
void*_tmp1D8=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_stmttmp1E=_tmp1D8;void*_tmp1D9=_stmttmp1E;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Datatypedecl*_tmp1DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f1)->f1).KnownDatatype).tag == 2){_LL1F: _tmp1DA=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f1)->f1).KnownDatatype).val;_tmp1DB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D9)->f2;_LL20: {struct Cyc_Absyn_Datatypedecl*td=_tmp1DA;struct Cyc_List_List*targs=_tmp1DB;
# 992
struct Cyc_Absyn_Datatypedecl*_tmp1DC=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_tmp1DC;
struct Cyc_List_List*fields=0;
if(td->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1DF=({struct Cyc_String_pa_PrintArg_struct _tmp5A3;_tmp5A3.tag=0U,({
struct _fat_ptr _tmp729=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(vt));_tmp5A3.f1=_tmp729;});_tmp5A3;});void*_tmp1DD[1U];_tmp1DD[0]=& _tmp1DF;({unsigned _tmp72B=loc;struct _fat_ptr _tmp72A=({const char*_tmp1DE="can't inject into abstract datatype %s";_tag_fat(_tmp1DE,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp72B,_tmp72A,_tag_fat(_tmp1DD,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;}
# 1003
({void*_tmp72C=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_tmp72C,Cyc_Tcenv_curr_rgn(te));});{
# 1005
struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,targs);struct Cyc_List_List*inst=_tmp1E0;
for(0;es != 0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1010
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt))&& !
Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);}{
# 1016
struct Cyc_Absyn_Datatypefield*_tmp1E1=({struct Cyc_Tcenv_Tenv*_tmp730=te;struct Cyc_Absyn_Exp*_tmp72F=e1;void*_tmp72E=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_tmp72D=inst;Cyc_Tcexp_tcInjection(_tmp730,_tmp72F,_tmp72E,_tmp72D,fields);});struct Cyc_Absyn_Datatypefield*f=_tmp1E1;
if(f != 0)
({struct Cyc_List_List*_tmp732=({
struct Cyc_List_List*_tmp731=vci->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp731,({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));
_tmp1E2->hd=f,_tmp1E2->tl=0;_tmp1E2;}));});
# 1018
vci->injectors=_tmp732;});}}}
# 1022
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({void*_tmp1E3=0U;({unsigned _tmp734=loc;struct _fat_ptr _tmp733=({const char*_tmp1E4="bad inject vararg type";_tag_fat(_tmp1E4,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp734,_tmp733,_tag_fat(_tmp1E3,sizeof(void*),0U));});});goto _LL1E;}_LL1E:;}}}}}}
# 1028
if(*alias_arg_exps == 0)
# 1037 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)_check_null(eff));
# 1039
return res_typ;}}}else{_LL8: _LL9:
# 1041
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A5;_tmp5A5.tag=0U,({struct _fat_ptr _tmp735=({const char*_tmp1E9="expected pointer to function but found ";_tag_fat(_tmp1E9,sizeof(char),40U);});_tmp5A5.f1=_tmp735;});_tmp5A5;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A4;_tmp5A4.tag=2U,_tmp5A4.f1=(void*)t;_tmp5A4;});void*_tmp1E6[2U];_tmp1E6[0]=& _tmp1E7,_tmp1E6[1]=& _tmp1E8;({struct Cyc_Tcenv_Tenv*_tmp738=te;unsigned _tmp737=loc;void**_tmp736=topt;Cyc_Tcexp_expr_err(_tmp738,_tmp737,_tmp736,_tag_fat(_tmp1E6,sizeof(void*),2U));});});}_LL5:;}}}else{_LL3: _LL4:
# 1044
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp1EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A7;_tmp5A7.tag=0U,({struct _fat_ptr _tmp739=({const char*_tmp1ED="expected pointer to function but found ";_tag_fat(_tmp1ED,sizeof(char),40U);});_tmp5A7.f1=_tmp739;});_tmp5A7;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1EC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5A6;_tmp5A6.tag=2U,_tmp5A6.f1=(void*)t;_tmp5A6;});void*_tmp1EA[2U];_tmp1EA[0]=& _tmp1EB,_tmp1EA[1]=& _tmp1EC;({struct Cyc_Tcenv_Tenv*_tmp73C=te;unsigned _tmp73B=loc;void**_tmp73A=topt;Cyc_Tcexp_expr_err(_tmp73C,_tmp73B,_tmp73A,_tag_fat(_tmp1EA,sizeof(void*),2U));});});}_LL0:;}}}
# 1048
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1EE=Cyc_Absyn_exn_type();void*exception_type=_tmp1EE;
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp73E=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp73D=e;Cyc_Tcutil_coerce_arg(_tmp73E,_tmp73D,exception_type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AB;_tmp5AB.tag=0U,({struct _fat_ptr _tmp73F=({const char*_tmp1F5="expected ";_tag_fat(_tmp1F5,sizeof(char),10U);});_tmp5AB.f1=_tmp73F;});_tmp5AB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AA;_tmp5AA.tag=2U,_tmp5AA.f1=(void*)exception_type;_tmp5AA;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5A9;_tmp5A9.tag=0U,({struct _fat_ptr _tmp740=({const char*_tmp1F4=" but found ";_tag_fat(_tmp1F4,sizeof(char),12U);});_tmp5A9.f1=_tmp740;});_tmp5A9;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp1F3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5A8;_tmp5A8.tag=3U,_tmp5A8.f1=(void*)e->topt;_tmp5A8;});void*_tmp1EF[4U];_tmp1EF[0]=& _tmp1F0,_tmp1EF[1]=& _tmp1F1,_tmp1EF[2]=& _tmp1F2,_tmp1EF[3]=& _tmp1F3;({unsigned _tmp741=loc;Cyc_Warn_err2(_tmp741,_tag_fat(_tmp1EF,sizeof(void*),4U));});});
if(topt != 0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1060
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1062
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1064
({void*_tmp742=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp742;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1F6=t1;void*_tmp1FC;void*_tmp1FB;void*_tmp1FA;void*_tmp1F9;struct Cyc_Absyn_Tqual _tmp1F8;void*_tmp1F7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->tag == 3U){_LL1: _tmp1F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).elt_type;_tmp1F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).elt_tq;_tmp1F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).rgn;_tmp1FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).nullable;_tmp1FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).bounds;_tmp1FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F6)->f1).ptr_atts).zero_term;_LL2: {void*t0=_tmp1F7;struct Cyc_Absyn_Tqual tq=_tmp1F8;void*rt=_tmp1F9;void*x=_tmp1FA;void*b=_tmp1FB;void*zt=_tmp1FC;
# 1068
{void*_tmp1FD=Cyc_Tcutil_compress(t0);void*_stmttmp1F=_tmp1FD;void*_tmp1FE=_stmttmp1F;struct Cyc_List_List*_tmp20B;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_List_List*_tmp207;struct Cyc_List_List*_tmp206;struct Cyc_Absyn_VarargInfo*_tmp205;int _tmp204;struct Cyc_List_List*_tmp203;void*_tmp202;struct Cyc_Absyn_Tqual _tmp201;void*_tmp200;struct Cyc_List_List*_tmp1FF;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->tag == 5U){_LL6: _tmp1FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).tvars;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).effect;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ret_tqual;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ret_type;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).args;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).c_varargs;_tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).cyc_varargs;_tmp206=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).rgn_po;_tmp207=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).attributes;_tmp208=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).requires_clause;_tmp209=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).requires_relns;_tmp20A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ensures_clause;_tmp20B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FE)->f1).ensures_relns;_LL7: {struct Cyc_List_List*tvars=_tmp1FF;void*eff=_tmp200;struct Cyc_Absyn_Tqual rtq=_tmp201;void*rtyp=_tmp202;struct Cyc_List_List*args=_tmp203;int c_varargs=_tmp204;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp205;struct Cyc_List_List*rpo=_tmp206;struct Cyc_List_List*atts=_tmp207;struct Cyc_Absyn_Exp*req=_tmp208;struct Cyc_List_List*req_relns=_tmp209;struct Cyc_Absyn_Exp*ens=_tmp20A;struct Cyc_List_List*ens_relns=_tmp20B;
# 1070
struct Cyc_List_List*instantiation=0;
# 1072
for(0;ts != 0 && tvars != 0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Tcutil_bk);
({unsigned _tmp746=loc;struct Cyc_Tcenv_Tenv*_tmp745=te;struct Cyc_List_List*_tmp744=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp743=k;Cyc_Tctyp_check_type(_tmp746,_tmp745,_tmp744,_tmp743,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));({struct _tuple14*_tmp747=({struct _tuple14*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->f1=(struct Cyc_Absyn_Tvar*)tvars->hd,_tmp20C->f2=(void*)ts->hd;_tmp20C;});_tmp20D->hd=_tmp747;}),_tmp20D->tl=instantiation;_tmp20D;});}
# 1078
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp20F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AC;_tmp5AC.tag=0U,({struct _fat_ptr _tmp748=({const char*_tmp210="too many type variables in instantiation";_tag_fat(_tmp210,sizeof(char),41U);});_tmp5AC.f1=_tmp748;});_tmp5AC;});void*_tmp20E[1U];_tmp20E[0]=& _tmp20F;({struct Cyc_Tcenv_Tenv*_tmp74B=te;unsigned _tmp74A=loc;void**_tmp749=topt;Cyc_Tcexp_expr_err(_tmp74B,_tmp74A,_tmp749,_tag_fat(_tmp20E,sizeof(void*),1U));});});
# 1083
if(tvars == 0){
rpo=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,rpo);
Cyc_Tcenv_check_rgn_partial_order(te,loc,rpo);
rpo=0;}{
# 1088
void*new_fn_typ=({
struct Cyc_List_List*_tmp74C=instantiation;Cyc_Tcutil_substitute(_tmp74C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp212=_cycalloc(sizeof(*_tmp212));
_tmp212->tag=5U,(_tmp212->f1).tvars=tvars,(_tmp212->f1).effect=eff,(_tmp212->f1).ret_tqual=rtq,(_tmp212->f1).ret_type=rtyp,(_tmp212->f1).args=args,(_tmp212->f1).c_varargs=c_varargs,(_tmp212->f1).cyc_varargs=cyc_varargs,(_tmp212->f1).rgn_po=rpo,(_tmp212->f1).attributes=atts,(_tmp212->f1).requires_clause=req,(_tmp212->f1).requires_relns=req_relns,(_tmp212->f1).ensures_clause=ens,(_tmp212->f1).ensures_relns=ens_relns;_tmp212;}));});
# 1093
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->tag=3U,(_tmp211->f1).elt_type=new_fn_typ,(_tmp211->f1).elt_tq=tq,((_tmp211->f1).ptr_atts).rgn=rt,((_tmp211->f1).ptr_atts).nullable=x,((_tmp211->f1).ptr_atts).bounds=b,((_tmp211->f1).ptr_atts).zero_term=zt,((_tmp211->f1).ptr_atts).ptrloc=0;_tmp211;});}}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1096
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1099
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp214=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AE;_tmp5AE.tag=0U,({struct _fat_ptr _tmp74D=({const char*_tmp216="expecting polymorphic type but found ";_tag_fat(_tmp216,sizeof(char),38U);});_tmp5AE.f1=_tmp74D;});_tmp5AE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp215=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AD;_tmp5AD.tag=2U,_tmp5AD.f1=(void*)t1;_tmp5AD;});void*_tmp213[2U];_tmp213[0]=& _tmp214,_tmp213[1]=& _tmp215;({struct Cyc_Tcenv_Tenv*_tmp750=te;unsigned _tmp74F=loc;void**_tmp74E=topt;Cyc_Tcexp_expr_err(_tmp750,_tmp74F,_tmp74E,_tag_fat(_tmp213,sizeof(void*),2U));});});}}
# 1103
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1105
({unsigned _tmp754=loc;struct Cyc_Tcenv_Tenv*_tmp753=te;struct Cyc_List_List*_tmp752=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp751=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1105
Cyc_Tctyp_check_type(_tmp754,_tmp753,_tmp752,_tmp751,1,0,t);});
# 1107
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_tmp757=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp756=loc;void*_tmp755=t2;Cyc_Tcutil_silent_castable(_tmp757,_tmp756,_tmp755,t);}))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1113
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_tmp75A=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp759=loc;void*_tmp758=t2;Cyc_Tcutil_castable(_tmp75A,_tmp759,_tmp758,t);});
if((int)crc != (int)0U)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1120
Cyc_Unify_unify(t2,t);{
void*_tmp217=({struct Cyc_Warn_String_Warn_Warg_struct _tmp219=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B2;_tmp5B2.tag=0U,({struct _fat_ptr _tmp75B=({const char*_tmp21E="cannot cast ";_tag_fat(_tmp21E,sizeof(char),13U);});_tmp5B2.f1=_tmp75B;});_tmp5B2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B1;_tmp5B1.tag=2U,_tmp5B1.f1=(void*)t2;_tmp5B1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp21B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B0;_tmp5B0.tag=0U,({struct _fat_ptr _tmp75C=({const char*_tmp21D=" to ";_tag_fat(_tmp21D,sizeof(char),5U);});_tmp5B0.f1=_tmp75C;});_tmp5B0;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp21C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5AF;_tmp5AF.tag=2U,_tmp5AF.f1=(void*)t;_tmp5AF;});void*_tmp218[4U];_tmp218[0]=& _tmp219,_tmp218[1]=& _tmp21A,_tmp218[2]=& _tmp21B,_tmp218[3]=& _tmp21C;({struct Cyc_Tcenv_Tenv*_tmp75E=te;unsigned _tmp75D=loc;Cyc_Tcexp_expr_err(_tmp75E,_tmp75D,& t,_tag_fat(_tmp218,sizeof(void*),4U));});});void*result=_tmp217;
Cyc_Unify_explain_failure();
return result;}}}}
# 1129
{struct _tuple0 _tmp21F=({struct _tuple0 _tmp5B3;_tmp5B3.f1=e->r,({void*_tmp75F=Cyc_Tcutil_compress(t);_tmp5B3.f2=_tmp75F;});_tmp5B3;});struct _tuple0 _stmttmp20=_tmp21F;struct _tuple0 _tmp220=_stmttmp20;void*_tmp224;void*_tmp223;void*_tmp222;int _tmp221;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp220.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->tag == 3U){_LL1: _tmp221=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp220.f1)->f1).fat_result;_tmp222=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->f1).ptr_atts).nullable;_tmp223=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->f1).ptr_atts).bounds;_tmp224=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220.f2)->f1).ptr_atts).zero_term;_LL2: {int fat_result=_tmp221;void*nbl=_tmp222;void*bds=_tmp223;void*zt=_tmp224;
# 1133
if((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl)){
struct Cyc_Absyn_Exp*_tmp225=({void*_tmp760=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp760,bds);});struct Cyc_Absyn_Exp*eopt=_tmp225;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp226=e;struct Cyc_Absyn_Exp*e2=_tmp226;
if((Cyc_Evexp_eval_const_uint_exp(e2)).f1 == (unsigned)1)
({void*_tmp227=0U;({unsigned _tmp762=loc;struct _fat_ptr _tmp761=({const char*_tmp228="cast from ? pointer to * pointer will lose size information";_tag_fat(_tmp228,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp762,_tmp761,_tag_fat(_tmp227,sizeof(void*),0U));});});}}
# 1141
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1144
return t;}}
# 1148
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp229=0;void**topt2=_tmp229;
struct Cyc_Absyn_Tqual _tmp22A=Cyc_Absyn_empty_tqual(0U);struct Cyc_Absyn_Tqual tq2=_tmp22A;
int _tmp22B=0;int nullable=_tmp22B;
if(topt != 0){
void*_tmp22C=Cyc_Tcutil_compress(*topt);void*_stmttmp21=_tmp22C;void*_tmp22D=_stmttmp21;void*_tmp230;struct Cyc_Absyn_Tqual _tmp22F;void**_tmp22E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->tag == 3U){_LL1: _tmp22E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->f1).elt_type;_tmp22F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->f1).elt_tq;_tmp230=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->f1).ptr_atts).nullable;_LL2: {void**elttype=_tmp22E;struct Cyc_Absyn_Tqual tq=_tmp22F;void*n=_tmp230;
# 1155
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1169
({struct Cyc_Tcenv_Tenv*_tmp764=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp763=topt2;Cyc_Tcexp_tcExpNoInst(_tmp764,_tmp763,e);});
# 1172
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp231=0U;({unsigned _tmp766=e->loc;struct _fat_ptr _tmp765=({const char*_tmp232="Cannot take the address of an alias-free path";_tag_fat(_tmp232,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp766,_tmp765,_tag_fat(_tmp231,sizeof(void*),0U));});});
# 1178
{void*_tmp233=e->r;void*_stmttmp22=_tmp233;void*_tmp234=_stmttmp22;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_Absyn_Exp*_tmp235;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp234)->tag == 23U){_LL6: _tmp235=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp234)->f1;_tmp236=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp234)->f2;_LL7: {struct Cyc_Absyn_Exp*e1=_tmp235;struct Cyc_Absyn_Exp*e2=_tmp236;
# 1180
{void*_tmp237=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp23=_tmp237;void*_tmp238=_stmttmp23;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp238)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1185
({void*_tmp767=(Cyc_Absyn_add_exp(e1,e2,0U))->r;e0->r=_tmp767;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}_LLA:;}
# 1188
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1193
{void*_tmp239=e->r;void*_stmttmp24=_tmp239;void*_tmp23A=_stmttmp24;switch(*((int*)_tmp23A)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp23A)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp23A)->f3 == 1){_LL12: _LL13:
# 1196
({void*_tmp23B=0U;({unsigned _tmp769=e->loc;struct _fat_ptr _tmp768=({const char*_tmp23C="cannot take the address of a @tagged union member";_tag_fat(_tmp23C,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp769,_tmp768,_tag_fat(_tmp23B,sizeof(void*),0U));});});goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1201
struct _tuple15 _tmp23D=Cyc_Tcutil_addressof_props(e);struct _tuple15 _stmttmp25=_tmp23D;struct _tuple15 _tmp23E=_stmttmp25;void*_tmp240;int _tmp23F;_LL17: _tmp23F=_tmp23E.f1;_tmp240=_tmp23E.f2;_LL18: {int is_const=_tmp23F;void*rgn=_tmp240;
# 1203
if(Cyc_Tcutil_is_noalias_region(rgn,0))
({void*_tmp241=0U;({unsigned _tmp76B=e->loc;struct _fat_ptr _tmp76A=({const char*_tmp242="using & would manufacture an alias to an alias-free pointer";_tag_fat(_tmp242,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp76B,_tmp76A,_tag_fat(_tmp241,sizeof(void*),0U));});});{
# 1206
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const){
tq.print_const=1;
tq.real_const=1;}{
# 1212
void*t=nullable?
Cyc_Absyn_star_type((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type):
 Cyc_Absyn_at_type((void*)_check_null(e->topt),rgn,tq,Cyc_Absyn_false_type);
return t;}}}}}
# 1219
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
if(te->allow_valueof)
# 1223
return Cyc_Absyn_uint_type;
# 1225
({unsigned _tmp76E=loc;struct Cyc_Tcenv_Tenv*_tmp76D=te;struct Cyc_List_List*_tmp76C=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp76E,_tmp76D,_tmp76C,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp244=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B6;_tmp5B6.tag=0U,({struct _fat_ptr _tmp76F=({const char*_tmp248="sizeof applied to type ";_tag_fat(_tmp248,sizeof(char),24U);});_tmp5B6.f1=_tmp76F;});_tmp5B6;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp245=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5B5;_tmp5B5.tag=2U,_tmp5B5.f1=(void*)t;_tmp5B5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp246=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B4;_tmp5B4.tag=0U,({struct _fat_ptr _tmp770=({const char*_tmp247=" which has unknown size here";_tag_fat(_tmp247,sizeof(char),29U);});_tmp5B4.f1=_tmp770;});_tmp5B4;});void*_tmp243[3U];_tmp243[0]=& _tmp244,_tmp243[1]=& _tmp245,_tmp243[2]=& _tmp246;({unsigned _tmp771=loc;Cyc_Warn_err2(_tmp771,_tag_fat(_tmp243,sizeof(void*),3U));});});
if(topt != 0){
void*_tmp249=Cyc_Tcutil_compress(*topt);void*_stmttmp26=_tmp249;void*_tmp24A=_stmttmp26;void*_tmp24B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f2 != 0){_LL1: _tmp24B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f2)->hd;_LL2: {void*tt=_tmp24B;
# 1233
if(({void*_tmp773=tt;Cyc_Unify_unify(_tmp773,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->tag=9U,({struct Cyc_Absyn_Exp*_tmp772=Cyc_Absyn_sizeoftype_exp(t,0U);_tmp24C->f1=_tmp772;});_tmp24C;}));}))
return Cyc_Tcutil_compress(*topt);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1238
return Cyc_Absyn_uint_type;}
# 1241
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _fat_ptr)*n,(struct _fat_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1248
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1250
({unsigned _tmp776=loc;struct Cyc_Tcenv_Tenv*_tmp775=te;struct Cyc_List_List*_tmp774=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp776,_tmp775,_tmp774,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp24D=fs;struct Cyc_List_List*l=_tmp24D;
void*_tmp24E=t_orig;void*t=_tmp24E;
for(0;l != 0;l=l->tl){
void*_tmp24F=(void*)l->hd;void*n=_tmp24F;
void*_tmp250=n;unsigned _tmp251;struct _fat_ptr*_tmp252;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp250)->tag == 0U){_LL1: _tmp252=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp250)->f1;_LL2: {struct _fat_ptr*n=_tmp252;
# 1258
int bad_type=1;
{void*_tmp253=Cyc_Tcutil_compress(t);void*_stmttmp27=_tmp253;void*_tmp254=_stmttmp27;struct Cyc_List_List*_tmp255;struct Cyc_Absyn_Aggrdecl**_tmp256;switch(*((int*)_tmp254)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp256=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp254)->f1)->f1).KnownAggr).val;_LL7: {struct Cyc_Absyn_Aggrdecl**adp=_tmp256;
# 1261
if((*adp)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp259=({struct Cyc_String_pa_PrintArg_struct _tmp5B7;_tmp5B7.tag=0U,_tmp5B7.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5B7;});void*_tmp257[1U];_tmp257[0]=& _tmp259;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp258="no field of struct/union has name %s";_tag_fat(_tmp258,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp778,_tmp777,_tag_fat(_tmp257,sizeof(void*),1U));});});else{
# 1266
t=t2;}
bad_type=0;
goto _LL5;}}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp255=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp254)->f2;_LL9: {struct Cyc_List_List*fields=_tmp255;
# 1270
void*t2=((void*(*)(void*(*pred)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp25C=({struct Cyc_String_pa_PrintArg_struct _tmp5B8;_tmp5B8.tag=0U,_tmp5B8.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5B8;});void*_tmp25A[1U];_tmp25A[0]=& _tmp25C;({unsigned _tmp77A=loc;struct _fat_ptr _tmp779=({const char*_tmp25B="no field of struct/union has name %s";_tag_fat(_tmp25B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_fat(_tmp25A,sizeof(void*),1U));});});else{
# 1274
t=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1279
if(bad_type){
if(l == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp25F=({struct Cyc_String_pa_PrintArg_struct _tmp5B9;_tmp5B9.tag=0U,({struct _fat_ptr _tmp77B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5B9.f1=_tmp77B;});_tmp5B9;});void*_tmp25D[1U];_tmp25D[0]=& _tmp25F;({unsigned _tmp77D=loc;struct _fat_ptr _tmp77C=({const char*_tmp25E="%s is not a known struct/union type";_tag_fat(_tmp25E,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp77D,_tmp77C,_tag_fat(_tmp25D,sizeof(void*),1U));});});else{
# 1283
struct _fat_ptr _tmp260=({struct Cyc_String_pa_PrintArg_struct _tmp273=({struct Cyc_String_pa_PrintArg_struct _tmp5C0;_tmp5C0.tag=0U,({struct _fat_ptr _tmp77E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp5C0.f1=_tmp77E;});_tmp5C0;});void*_tmp271[1U];_tmp271[0]=& _tmp273;({struct _fat_ptr _tmp77F=({const char*_tmp272="(%s)";_tag_fat(_tmp272,sizeof(char),5U);});Cyc_aprintf(_tmp77F,_tag_fat(_tmp271,sizeof(void*),1U));});});struct _fat_ptr s=_tmp260;
struct Cyc_List_List*x;
for(x=fs;x != l;x=x->tl){
void*_tmp261=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_stmttmp28=_tmp261;void*_tmp262=_stmttmp28;unsigned _tmp263;struct _fat_ptr*_tmp264;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp262)->tag == 0U){_LLD: _tmp264=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp262)->f1;_LLE: {struct _fat_ptr*n=_tmp264;
# 1288
s=({struct Cyc_String_pa_PrintArg_struct _tmp267=({struct Cyc_String_pa_PrintArg_struct _tmp5BB;_tmp5BB.tag=0U,_tmp5BB.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5BB;});struct Cyc_String_pa_PrintArg_struct _tmp268=({struct Cyc_String_pa_PrintArg_struct _tmp5BA;_tmp5BA.tag=0U,_tmp5BA.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_tmp5BA;});void*_tmp265[2U];_tmp265[0]=& _tmp267,_tmp265[1]=& _tmp268;({struct _fat_ptr _tmp780=({const char*_tmp266="%s.%s";_tag_fat(_tmp266,sizeof(char),6U);});Cyc_aprintf(_tmp780,_tag_fat(_tmp265,sizeof(void*),2U));});});goto _LLC;}}else{_LLF: _tmp263=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp262)->f1;_LL10: {unsigned n=_tmp263;
# 1290
s=({struct Cyc_String_pa_PrintArg_struct _tmp26B=({struct Cyc_String_pa_PrintArg_struct _tmp5BD;_tmp5BD.tag=0U,_tmp5BD.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5BD;});struct Cyc_Int_pa_PrintArg_struct _tmp26C=({struct Cyc_Int_pa_PrintArg_struct _tmp5BC;_tmp5BC.tag=1U,_tmp5BC.f1=(unsigned long)((int)n);_tmp5BC;});void*_tmp269[2U];_tmp269[0]=& _tmp26B,_tmp269[1]=& _tmp26C;({struct _fat_ptr _tmp781=({const char*_tmp26A="%s.%d";_tag_fat(_tmp26A,sizeof(char),6U);});Cyc_aprintf(_tmp781,_tag_fat(_tmp269,sizeof(void*),2U));});});goto _LLC;}}_LLC:;}
# 1292
({struct Cyc_String_pa_PrintArg_struct _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp5BF;_tmp5BF.tag=0U,_tmp5BF.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5BF;});struct Cyc_String_pa_PrintArg_struct _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp5BE;_tmp5BE.tag=0U,({struct _fat_ptr _tmp782=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5BE.f1=_tmp782;});_tmp5BE;});void*_tmp26D[2U];_tmp26D[0]=& _tmp26F,_tmp26D[1]=& _tmp270;({unsigned _tmp784=loc;struct _fat_ptr _tmp783=({const char*_tmp26E="%s == %s is not a struct/union type";_tag_fat(_tmp26E,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp784,_tmp783,_tag_fat(_tmp26D,sizeof(void*),2U));});});}}
# 1295
goto _LL0;}}else{_LL3: _tmp251=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp250)->f1;_LL4: {unsigned n=_tmp251;
# 1297
int bad_type=1;
{void*_tmp274=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp274;void*_tmp275=_stmttmp29;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_Absyn_Datatypefield*_tmp278;struct Cyc_Absyn_Aggrdecl**_tmp279;switch(*((int*)_tmp275)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp279=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownAggr).val;_LL13: {struct Cyc_Absyn_Aggrdecl**adp=_tmp279;
# 1300
if((*adp)->impl == 0)
goto _LL11;
_tmp277=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;goto _LL15;}}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp278=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)->f1).KnownDatatypefield).val).f2;_LL19: {struct Cyc_Absyn_Datatypefield*tuf=_tmp278;
# 1320
if(({unsigned _tmp785=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp785 < n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp284=({struct Cyc_Int_pa_PrintArg_struct _tmp5C2;_tmp5C2.tag=1U,({
unsigned long _tmp786=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tuf->typs);_tmp5C2.f1=_tmp786;});_tmp5C2;});struct Cyc_Int_pa_PrintArg_struct _tmp285=({struct Cyc_Int_pa_PrintArg_struct _tmp5C1;_tmp5C1.tag=1U,_tmp5C1.f1=(unsigned long)((int)n);_tmp5C1;});void*_tmp282[2U];_tmp282[0]=& _tmp284,_tmp282[1]=& _tmp285;({unsigned _tmp788=loc;struct _fat_ptr _tmp787=({const char*_tmp283="datatype field has too few components: %d < %d";_tag_fat(_tmp283,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp788,_tmp787,_tag_fat(_tmp282,sizeof(void*),2U));});});else{
# 1324
if(n != (unsigned)0)
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(tuf->typs,(int)(n - (unsigned)1))).f2;else{
if(l->tl != 0)
({void*_tmp286=0U;({unsigned _tmp78A=loc;struct _fat_ptr _tmp789=({const char*_tmp287="datatype field index 0 refers to the tag; cannot be indexed through";_tag_fat(_tmp287,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp78A,_tmp789,_tag_fat(_tmp286,sizeof(void*),0U));});});}}
# 1329
bad_type=0;
goto _LL11;}}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp277=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp275)->f2;_LL15: {struct Cyc_List_List*fields=_tmp277;
# 1304
if(({unsigned _tmp78B=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp78B <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27C=({struct Cyc_Int_pa_PrintArg_struct _tmp5C4;_tmp5C4.tag=1U,({
unsigned long _tmp78C=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);_tmp5C4.f1=_tmp78C;});_tmp5C4;});struct Cyc_Int_pa_PrintArg_struct _tmp27D=({struct Cyc_Int_pa_PrintArg_struct _tmp5C3;_tmp5C3.tag=1U,_tmp5C3.f1=(unsigned long)((int)n);_tmp5C3;});void*_tmp27A[2U];_tmp27A[0]=& _tmp27C,_tmp27A[1]=& _tmp27D;({unsigned _tmp78E=loc;struct _fat_ptr _tmp78D=({const char*_tmp27B="struct/union has too few components: %d <= %d";_tag_fat(_tmp27B,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp78E,_tmp78D,_tag_fat(_tmp27A,sizeof(void*),2U));});});else{
# 1308
t=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(fields,(int)n))->type;}
bad_type=0;
goto _LL11;}case 6U: _LL16: _tmp276=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp275)->f1;_LL17: {struct Cyc_List_List*l=_tmp276;
# 1312
if(({unsigned _tmp78F=(unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp78F <= n;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp280=({struct Cyc_Int_pa_PrintArg_struct _tmp5C6;_tmp5C6.tag=1U,({
unsigned long _tmp790=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(l);_tmp5C6.f1=_tmp790;});_tmp5C6;});struct Cyc_Int_pa_PrintArg_struct _tmp281=({struct Cyc_Int_pa_PrintArg_struct _tmp5C5;_tmp5C5.tag=1U,_tmp5C5.f1=(unsigned long)((int)n);_tmp5C5;});void*_tmp27E[2U];_tmp27E[0]=& _tmp280,_tmp27E[1]=& _tmp281;({unsigned _tmp792=loc;struct _fat_ptr _tmp791=({const char*_tmp27F="tuple has too few components: %d <= %d";_tag_fat(_tmp27F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp792,_tmp791,_tag_fat(_tmp27E,sizeof(void*),2U));});});else{
# 1316
t=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(l,(int)n)).f2;}
bad_type=0;
goto _LL11;}default: _LL1A: _LL1B:
# 1331
 goto _LL11;}_LL11:;}
# 1333
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp5C7;_tmp5C7.tag=0U,({struct _fat_ptr _tmp793=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5C7.f1=_tmp793;});_tmp5C7;});void*_tmp288[1U];_tmp288[0]=& _tmp28A;({unsigned _tmp795=loc;struct _fat_ptr _tmp794=({const char*_tmp289="%s is not a known type";_tag_fat(_tmp289,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp795,_tmp794,_tag_fat(_tmp288,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1338
return Cyc_Absyn_uint_type;}}
# 1342
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp28B=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp28B;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp28C=t;void*_tmp290;void*_tmp28F;void*_tmp28E;void*_tmp28D;switch(*((int*)_tmp28C)){case 1U: _LL1: _LL2: {
# 1348
struct Cyc_List_List*_tmp291=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_List_List*tenv_tvs=_tmp291;
void*_tmp292=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp29A=_cycalloc(sizeof(*_tmp29A));_tmp29A->v=tenv_tvs;_tmp29A;}));void*t2=_tmp292;
void*_tmp293=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->v=tenv_tvs;_tmp299;}));void*rt=_tmp293;
void*_tmp294=Cyc_Tcutil_any_bounds(tenv_tvs);void*b=_tmp294;
void*_tmp295=Cyc_Tcutil_any_bool(tenv_tvs);void*zt=_tmp295;
struct Cyc_Absyn_PtrAtts _tmp296=({struct Cyc_Absyn_PtrAtts _tmp5C8;_tmp5C8.rgn=rt,({void*_tmp796=Cyc_Tcutil_any_bool(tenv_tvs);_tmp5C8.nullable=_tmp796;}),_tmp5C8.bounds=b,_tmp5C8.zero_term=zt,_tmp5C8.ptrloc=0;_tmp5C8;});struct Cyc_Absyn_PtrAtts atts=_tmp296;
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp297=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->tag=3U,(_tmp298->f1).elt_type=t2,({struct Cyc_Absyn_Tqual _tmp797=Cyc_Absyn_empty_tqual(0U);(_tmp298->f1).elt_tq=_tmp797;}),(_tmp298->f1).ptr_atts=atts;_tmp298;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_typ=_tmp297;
Cyc_Unify_unify(t,(void*)new_typ);
_tmp28D=t2;_tmp28E=rt;_tmp28F=b;_tmp290=zt;goto _LL4;}case 3U: _LL3: _tmp28D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).elt_type;_tmp28E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).rgn;_tmp28F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).bounds;_tmp290=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).zero_term;_LL4: {void*t2=_tmp28D;void*rt=_tmp28E;void*b=_tmp28F;void*zt=_tmp290;
# 1358
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp29B=Cyc_Tcutil_compress(t2);void*_stmttmp2A=_tmp29B;void*_tmp29C=_stmttmp2A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp29C)->tag == 5U){_LL8: _LL9:
# 1363
 if(Cyc_Tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C9;_tmp5C9.tag=0U,({struct _fat_ptr _tmp798=({const char*_tmp29F="unnecessary dereference for function type";_tag_fat(_tmp29F,sizeof(char),42U);});_tmp5C9.f1=_tmp798;});_tmp5C9;});void*_tmp29D[1U];_tmp29D[0]=& _tmp29E;({unsigned _tmp799=loc;Cyc_Warn_warn2(_tmp799,_tag_fat(_tmp29D,sizeof(void*),1U));});});
return t;}else{_LLA: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CA;_tmp5CA.tag=0U,({struct _fat_ptr _tmp79A=({const char*_tmp2A2="can't dereference abstract pointer type";_tag_fat(_tmp2A2,sizeof(char),40U);});_tmp5CA.f1=_tmp79A;});_tmp5CA;});void*_tmp2A0[1U];_tmp2A0[0]=& _tmp2A1;({unsigned _tmp79B=loc;Cyc_Warn_err2(_tmp79B,_tag_fat(_tmp2A0,sizeof(void*),1U));});});}_LL7:;}
# 1368
return t2;}default: _LL5: _LL6:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CC;_tmp5CC.tag=0U,({struct _fat_ptr _tmp79C=({const char*_tmp2A6="expecting pointer type but found ";_tag_fat(_tmp2A6,sizeof(char),34U);});_tmp5CC.f1=_tmp79C;});_tmp5CC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5CB;_tmp5CB.tag=2U,_tmp5CB.f1=(void*)t;_tmp5CB;});void*_tmp2A3[2U];_tmp2A3[0]=& _tmp2A4,_tmp2A3[1]=& _tmp2A5;({struct Cyc_Tcenv_Tenv*_tmp79F=te;unsigned _tmp79E=loc;void**_tmp79D=topt;Cyc_Tcexp_expr_err(_tmp79F,_tmp79E,_tmp79D,_tag_fat(_tmp2A3,sizeof(void*),2U));});});}_LL0:;}}
# 1374
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1378
({struct Cyc_Tcenv_Tenv*_tmp7A0=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp7A0,0,e);});
# 1380
({int _tmp7A1=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp7A1;});{
void*_tmp2A7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp2B=_tmp2A7;void*_tmp2A8=_stmttmp2B;struct Cyc_List_List*_tmp2AA;enum Cyc_Absyn_AggrKind _tmp2A9;struct Cyc_List_List*_tmp2AC;struct Cyc_Absyn_Aggrdecl*_tmp2AB;switch(*((int*)_tmp2A8)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp2AB=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8)->f1)->f1).KnownAggr).val;_tmp2AC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8)->f2;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2AB;struct Cyc_List_List*ts=_tmp2AC;
# 1383
struct Cyc_Absyn_Aggrfield*_tmp2AD=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2AD;
if(finfo == 0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2AF=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5D0;_tmp5D0.tag=6U,_tmp5D0.f1=ad;_tmp5D0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CF;_tmp5CF.tag=0U,({struct _fat_ptr _tmp7A2=({const char*_tmp2B4=" has no ";_tag_fat(_tmp2B4,sizeof(char),9U);});_tmp5CF.f1=_tmp7A2;});_tmp5CF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CE;_tmp5CE.tag=0U,_tmp5CE.f1=*f;_tmp5CE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD;_tmp5CD.tag=0U,({struct _fat_ptr _tmp7A3=({const char*_tmp2B3=" member";_tag_fat(_tmp2B3,sizeof(char),8U);});_tmp5CD.f1=_tmp7A3;});_tmp5CD;});void*_tmp2AE[4U];_tmp2AE[0]=& _tmp2AF,_tmp2AE[1]=& _tmp2B0,_tmp2AE[2]=& _tmp2B1,_tmp2AE[3]=& _tmp2B2;({struct Cyc_Tcenv_Tenv*_tmp7A6=te;unsigned _tmp7A5=loc;void**_tmp7A4=topt;Cyc_Tcexp_expr_err(_tmp7A6,_tmp7A5,_tmp7A4,_tag_fat(_tmp2AE,sizeof(void*),4U));});});
# 1387
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts != 0)
t2=({struct Cyc_List_List*_tmp7A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);Cyc_Tcutil_substitute(_tmp7A7,finfo->type);});
# 1393
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D3;_tmp5D3.tag=0U,({struct _fat_ptr _tmp7A8=({const char*_tmp2BA="cannot read union member ";_tag_fat(_tmp2BA,sizeof(char),26U);});_tmp5D3.f1=_tmp7A8;});_tmp5D3;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D2;_tmp5D2.tag=0U,_tmp5D2.f1=*f;_tmp5D2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D1;_tmp5D1.tag=0U,({
struct _fat_ptr _tmp7A9=({const char*_tmp2B9=" since it is not `bits-only'";_tag_fat(_tmp2B9,sizeof(char),29U);});_tmp5D1.f1=_tmp7A9;});_tmp5D1;});void*_tmp2B5[3U];_tmp2B5[0]=& _tmp2B6,_tmp2B5[1]=& _tmp2B7,_tmp2B5[2]=& _tmp2B8;({struct Cyc_Tcenv_Tenv*_tmp7AC=te;unsigned _tmp7AB=loc;void**_tmp7AA=topt;Cyc_Tcexp_expr_err(_tmp7AC,_tmp7AB,_tmp7AA,_tag_fat(_tmp2B5,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1399
if(!({void*_tmp7AD=t2;Cyc_Unify_unify(_tmp7AD,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D6;_tmp5D6.tag=0U,({struct _fat_ptr _tmp7AE=({const char*_tmp2C0="must use pattern-matching to access field ";_tag_fat(_tmp2C0,sizeof(char),43U);});_tmp5D6.f1=_tmp7AE;});_tmp5D6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D5;_tmp5D5.tag=0U,_tmp5D5.f1=*f;_tmp5D5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D4;_tmp5D4.tag=0U,({struct _fat_ptr _tmp7AF=({const char*_tmp2BF="\n\tdue to existential type variables.";_tag_fat(_tmp2BF,sizeof(char),37U);});_tmp5D4.f1=_tmp7AF;});_tmp5D4;});void*_tmp2BB[3U];_tmp2BB[0]=& _tmp2BC,_tmp2BB[1]=& _tmp2BD,_tmp2BB[2]=& _tmp2BE;({struct Cyc_Tcenv_Tenv*_tmp7B2=te;unsigned _tmp7B1=loc;void**_tmp7B0=topt;Cyc_Tcexp_expr_err(_tmp7B2,_tmp7B1,_tmp7B0,_tag_fat(_tmp2BB,sizeof(void*),3U));});});}
# 1402
return t2;}}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp2A9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A8)->f1;_tmp2AA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A8)->f2;_LL4: {enum Cyc_Absyn_AggrKind k=_tmp2A9;struct Cyc_List_List*fs=_tmp2AA;
# 1404
struct Cyc_Absyn_Aggrfield*_tmp2C1=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2C1;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D9;_tmp5D9.tag=0U,({struct _fat_ptr _tmp7B3=({const char*_tmp2C7="type has no ";_tag_fat(_tmp2C7,sizeof(char),13U);});_tmp5D9.f1=_tmp7B3;});_tmp5D9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D8;_tmp5D8.tag=0U,_tmp5D8.f1=*f;_tmp5D8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5D7;_tmp5D7.tag=0U,({struct _fat_ptr _tmp7B4=({const char*_tmp2C6=" member";_tag_fat(_tmp2C6,sizeof(char),8U);});_tmp5D7.f1=_tmp7B4;});_tmp5D7;});void*_tmp2C2[3U];_tmp2C2[0]=& _tmp2C3,_tmp2C2[1]=& _tmp2C4,_tmp2C2[2]=& _tmp2C5;({struct Cyc_Tcenv_Tenv*_tmp7B7=te;unsigned _tmp7B6=loc;void**_tmp7B5=topt;Cyc_Tcexp_expr_err(_tmp7B7,_tmp7B6,_tmp7B5,_tag_fat(_tmp2C2,sizeof(void*),3U));});});
# 1409
if((((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1411
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DC;_tmp5DC.tag=0U,({struct _fat_ptr _tmp7B8=({const char*_tmp2CD="cannot read union member ";_tag_fat(_tmp2CD,sizeof(char),26U);});_tmp5DC.f1=_tmp7B8;});_tmp5DC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DB;_tmp5DB.tag=0U,_tmp5DB.f1=*f;_tmp5DB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DA;_tmp5DA.tag=0U,({struct _fat_ptr _tmp7B9=({const char*_tmp2CC=" since it is not `bits-only'";_tag_fat(_tmp2CC,sizeof(char),29U);});_tmp5DA.f1=_tmp7B9;});_tmp5DA;});void*_tmp2C8[3U];_tmp2C8[0]=& _tmp2C9,_tmp2C8[1]=& _tmp2CA,_tmp2C8[2]=& _tmp2CB;({struct Cyc_Tcenv_Tenv*_tmp7BC=te;unsigned _tmp7BB=loc;void**_tmp7BA=topt;Cyc_Tcexp_expr_err(_tmp7BC,_tmp7BB,_tmp7BA,_tag_fat(_tmp2C8,sizeof(void*),3U));});});
return finfo->type;}default: _LL5: _LL6:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DE;_tmp5DE.tag=0U,({struct _fat_ptr _tmp7BD=({const char*_tmp2D1="expecting struct or union, found ";_tag_fat(_tmp2D1,sizeof(char),34U);});_tmp5DE.f1=_tmp7BD;});_tmp5DE;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp2D0=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5DD;_tmp5DD.tag=3U,_tmp5DD.f1=(void*)e->topt;_tmp5DD;});void*_tmp2CE[2U];_tmp2CE[0]=& _tmp2CF,_tmp2CE[1]=& _tmp2D0;({struct Cyc_Tcenv_Tenv*_tmp7C0=te;unsigned _tmp7BF=loc;void**_tmp7BE=topt;Cyc_Tcexp_expr_err(_tmp7C0,_tmp7BF,_tmp7BE,_tag_fat(_tmp2CE,sizeof(void*),2U));});});}_LL0:;}}
# 1419
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1422
({struct Cyc_Tcenv_Tenv*_tmp7C1=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp7C1,0,e);});
# 1424
({int _tmp7C2=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp7C2;});
{void*_tmp2D2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp2C=_tmp2D2;void*_tmp2D3=_stmttmp2C;void*_tmp2D7;void*_tmp2D6;void*_tmp2D5;void*_tmp2D4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3)->tag == 3U){_LL1: _tmp2D4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3)->f1).elt_type;_tmp2D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3)->f1).ptr_atts).rgn;_tmp2D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3)->f1).ptr_atts).bounds;_tmp2D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3)->f1).ptr_atts).zero_term;_LL2: {void*t2=_tmp2D4;void*rt=_tmp2D5;void*b=_tmp2D6;void*zt=_tmp2D7;
# 1427
Cyc_Tcutil_check_nonzero_bound(loc,b);
{void*_tmp2D8=Cyc_Tcutil_compress(t2);void*_stmttmp2D=_tmp2D8;void*_tmp2D9=_stmttmp2D;struct Cyc_List_List*_tmp2DB;enum Cyc_Absyn_AggrKind _tmp2DA;struct Cyc_List_List*_tmp2DD;struct Cyc_Absyn_Aggrdecl*_tmp2DC;switch(*((int*)_tmp2D9)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp2DC=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->f1)->f1).KnownAggr).val;_tmp2DD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_tmp2DC;struct Cyc_List_List*ts=_tmp2DD;
# 1430
struct Cyc_Absyn_Aggrfield*_tmp2DE=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2DE;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E3;_tmp5E3.tag=0U,({struct _fat_ptr _tmp7C3=({const char*_tmp2E7="type ";_tag_fat(_tmp2E7,sizeof(char),6U);});_tmp5E3.f1=_tmp7C3;});_tmp5E3;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp2E1=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp5E2;_tmp5E2.tag=6U,_tmp5E2.f1=ad;_tmp5E2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E1;_tmp5E1.tag=0U,({struct _fat_ptr _tmp7C4=({const char*_tmp2E6=" has no ";_tag_fat(_tmp2E6,sizeof(char),9U);});_tmp5E1.f1=_tmp7C4;});_tmp5E1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E0;_tmp5E0.tag=0U,_tmp5E0.f1=*f;_tmp5E0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5DF;_tmp5DF.tag=0U,({struct _fat_ptr _tmp7C5=({const char*_tmp2E5=" member";_tag_fat(_tmp2E5,sizeof(char),8U);});_tmp5DF.f1=_tmp7C5;});_tmp5DF;});void*_tmp2DF[5U];_tmp2DF[0]=& _tmp2E0,_tmp2DF[1]=& _tmp2E1,_tmp2DF[2]=& _tmp2E2,_tmp2DF[3]=& _tmp2E3,_tmp2DF[4]=& _tmp2E4;({struct Cyc_Tcenv_Tenv*_tmp7C8=te;unsigned _tmp7C7=loc;void**_tmp7C6=topt;Cyc_Tcexp_expr_err(_tmp7C8,_tmp7C7,_tmp7C6,_tag_fat(_tmp2DF,sizeof(void*),5U));});});
# 1434
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)*is_tagged=1;{
void*t3=finfo->type;
if(ts != 0){
struct Cyc_List_List*_tmp2E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(ad->tvs,ts);struct Cyc_List_List*inst=_tmp2E8;
t3=Cyc_Tcutil_substitute(inst,finfo->type);}{
# 1440
struct Cyc_Absyn_Kind*_tmp2E9=Cyc_Tcutil_type_kind(t3);struct Cyc_Absyn_Kind*t3_kind=_tmp2E9;
# 1443
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,t3_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2EA=Cyc_Tcutil_compress(t3);void*_stmttmp2E=_tmp2EA;void*_tmp2EB=_stmttmp2E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2EB)->tag == 4U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E6;_tmp5E6.tag=0U,({struct _fat_ptr _tmp7C9=({const char*_tmp2F1="cannot get member ";_tag_fat(_tmp2F1,sizeof(char),19U);});_tmp5E6.f1=_tmp7C9;});_tmp5E6;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E5;_tmp5E5.tag=0U,_tmp5E5.f1=*f;_tmp5E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E4;_tmp5E4.tag=0U,({
struct _fat_ptr _tmp7CA=({const char*_tmp2F0=" since its type is abstract";_tag_fat(_tmp2F0,sizeof(char),28U);});_tmp5E4.f1=_tmp7CA;});_tmp5E4;});void*_tmp2EC[3U];_tmp2EC[0]=& _tmp2ED,_tmp2EC[1]=& _tmp2EE,_tmp2EC[2]=& _tmp2EF;({struct Cyc_Tcenv_Tenv*_tmp7CD=te;unsigned _tmp7CC=loc;void**_tmp7CB=topt;Cyc_Tcexp_expr_err(_tmp7CD,_tmp7CC,_tmp7CB,_tag_fat(_tmp2EC,sizeof(void*),3U));});});}_LLC:;}
# 1452
if(((((int)ad->kind == (int)Cyc_Absyn_UnionA && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)&& !
Cyc_Tcutil_is_bits_only_type(t3))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause == 0)
# 1455
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=0U,({struct _fat_ptr _tmp7CE=({const char*_tmp2F7="cannot read union member ";_tag_fat(_tmp2F7,sizeof(char),26U);});_tmp5E9.f1=_tmp7CE;});_tmp5E9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E8;_tmp5E8.tag=0U,_tmp5E8.f1=*f;_tmp5E8;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E7;_tmp5E7.tag=0U,({
struct _fat_ptr _tmp7CF=({const char*_tmp2F6=" since it is not `bits-only'";_tag_fat(_tmp2F6,sizeof(char),29U);});_tmp5E7.f1=_tmp7CF;});_tmp5E7;});void*_tmp2F2[3U];_tmp2F2[0]=& _tmp2F3,_tmp2F2[1]=& _tmp2F4,_tmp2F2[2]=& _tmp2F5;({struct Cyc_Tcenv_Tenv*_tmp7D2=te;unsigned _tmp7D1=loc;void**_tmp7D0=topt;Cyc_Tcexp_expr_err(_tmp7D2,_tmp7D1,_tmp7D0,_tag_fat(_tmp2F2,sizeof(void*),3U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
if(!({void*_tmp7D3=t3;Cyc_Unify_unify(_tmp7D3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0U,({
struct _fat_ptr _tmp7D4=({const char*_tmp2FD="must use pattern-matching to access field ";_tag_fat(_tmp2FD,sizeof(char),43U);});_tmp5EC.f1=_tmp7D4;});_tmp5EC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2FA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=0U,_tmp5EB.f1=*f;_tmp5EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=0U,({
struct _fat_ptr _tmp7D5=({const char*_tmp2FC="\n\tdue to extistential types";_tag_fat(_tmp2FC,sizeof(char),28U);});_tmp5EA.f1=_tmp7D5;});_tmp5EA;});void*_tmp2F8[3U];_tmp2F8[0]=& _tmp2F9,_tmp2F8[1]=& _tmp2FA,_tmp2F8[2]=& _tmp2FB;({struct Cyc_Tcenv_Tenv*_tmp7D8=te;unsigned _tmp7D7=loc;void**_tmp7D6=topt;Cyc_Tcexp_expr_err(_tmp7D8,_tmp7D7,_tmp7D6,_tag_fat(_tmp2F8,sizeof(void*),3U));});});}
# 1463
return t3;}}}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp2DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D9)->f1;_tmp2DB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D9)->f2;_LL9: {enum Cyc_Absyn_AggrKind k=_tmp2DA;struct Cyc_List_List*fs=_tmp2DB;
# 1465
struct Cyc_Absyn_Aggrfield*_tmp2FE=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp2FE;
if(finfo == 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp300=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EF;_tmp5EF.tag=0U,({struct _fat_ptr _tmp7D9=({const char*_tmp304="type has no ";_tag_fat(_tmp304,sizeof(char),13U);});_tmp5EF.f1=_tmp7D9;});_tmp5EF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp301=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=0U,_tmp5EE.f1=*f;_tmp5EE;});struct Cyc_Warn_String_Warn_Warg_struct _tmp302=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5ED;_tmp5ED.tag=0U,({struct _fat_ptr _tmp7DA=({const char*_tmp303=" field";_tag_fat(_tmp303,sizeof(char),7U);});_tmp5ED.f1=_tmp7DA;});_tmp5ED;});void*_tmp2FF[3U];_tmp2FF[0]=& _tmp300,_tmp2FF[1]=& _tmp301,_tmp2FF[2]=& _tmp302;({struct Cyc_Tcenv_Tenv*_tmp7DD=te;unsigned _tmp7DC=loc;void**_tmp7DB=topt;Cyc_Tcexp_expr_err(_tmp7DD,_tmp7DC,_tmp7DB,_tag_fat(_tmp2FF,sizeof(void*),3U));});});
# 1470
if(((int)k == (int)1U && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp306=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=0U,({struct _fat_ptr _tmp7DE=({const char*_tmp30A="cannot read union member ";_tag_fat(_tmp30A,sizeof(char),26U);});_tmp5F2.f1=_tmp7DE;});_tmp5F2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp307=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=0U,_tmp5F1.f1=*f;_tmp5F1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp308=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,({
struct _fat_ptr _tmp7DF=({const char*_tmp309=" since it is not `bits-only'";_tag_fat(_tmp309,sizeof(char),29U);});_tmp5F0.f1=_tmp7DF;});_tmp5F0;});void*_tmp305[3U];_tmp305[0]=& _tmp306,_tmp305[1]=& _tmp307,_tmp305[2]=& _tmp308;({struct Cyc_Tcenv_Tenv*_tmp7E2=te;unsigned _tmp7E1=loc;void**_tmp7E0=topt;Cyc_Tcexp_expr_err(_tmp7E2,_tmp7E1,_tmp7E0,_tag_fat(_tmp305,sizeof(void*),3U));});});
return finfo->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1476
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1479
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp30C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=0U,({struct _fat_ptr _tmp7E3=({const char*_tmp30E="expecting struct or union pointer, found ";_tag_fat(_tmp30E,sizeof(char),42U);});_tmp5F4.f1=_tmp7E3;});_tmp5F4;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp30D=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=3U,_tmp5F3.f1=(void*)e->topt;_tmp5F3;});void*_tmp30B[2U];_tmp30B[0]=& _tmp30C,_tmp30B[1]=& _tmp30D;({struct Cyc_Tcenv_Tenv*_tmp7E6=te;unsigned _tmp7E5=loc;void**_tmp7E4=topt;Cyc_Tcexp_expr_err(_tmp7E6,_tmp7E5,_tmp7E4,_tag_fat(_tmp30B,sizeof(void*),2U));});});}
# 1484
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1486
struct _tuple16 _tmp30F=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple16 _stmttmp2F=_tmp30F;struct _tuple16 _tmp310=_stmttmp2F;int _tmp312;unsigned _tmp311;_LL1: _tmp311=_tmp310.f1;_tmp312=_tmp310.f2;_LL2: {unsigned i=_tmp311;int known=_tmp312;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp314=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5;_tmp5F5.tag=0U,({
struct _fat_ptr _tmp7E7=({const char*_tmp315="tuple projection cannot use sizeof or offsetof";_tag_fat(_tmp315,sizeof(char),47U);});_tmp5F5.f1=_tmp7E7;});_tmp5F5;});void*_tmp313[1U];_tmp313[0]=& _tmp314;({struct Cyc_Tcenv_Tenv*_tmp7E9=te;unsigned _tmp7E8=loc;Cyc_Tcexp_expr_err(_tmp7E9,_tmp7E8,0,_tag_fat(_tmp313,sizeof(void*),1U));});});{
struct _handler_cons _tmp316;_push_handler(& _tmp316);{int _tmp318=0;if(setjmp(_tmp316.handler))_tmp318=1;if(!_tmp318){
{void*_tmp319=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0U);return _tmp319;};_pop_handler();}else{void*_tmp317=(void*)Cyc_Core_get_exn_thrown();void*_tmp31A=_tmp317;void*_tmp31B;if(((struct Cyc_List_Nth_exn_struct*)_tmp31A)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1493
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp31D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=0U,({struct _fat_ptr _tmp7EA=({const char*_tmp324="index is ";_tag_fat(_tmp324,sizeof(char),10U);});_tmp5FA.f1=_tmp7EA;});_tmp5FA;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp31E=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F9;_tmp5F9.tag=12U,_tmp5F9.f1=(int)i;_tmp5F9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp31F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F8;_tmp5F8.tag=0U,({struct _fat_ptr _tmp7EB=({const char*_tmp323=" but tuple has only ";_tag_fat(_tmp323,sizeof(char),21U);});_tmp5F8.f1=_tmp7EB;});_tmp5F8;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp320=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp5F7;_tmp5F7.tag=12U,({
int _tmp7EC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp5F7.f1=_tmp7EC;});_tmp5F7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp321=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6;_tmp5F6.tag=0U,({struct _fat_ptr _tmp7ED=({const char*_tmp322=" fields";_tag_fat(_tmp322,sizeof(char),8U);});_tmp5F6.f1=_tmp7ED;});_tmp5F6;});void*_tmp31C[5U];_tmp31C[0]=& _tmp31D,_tmp31C[1]=& _tmp31E,_tmp31C[2]=& _tmp31F,_tmp31C[3]=& _tmp320,_tmp31C[4]=& _tmp321;({struct Cyc_Tcenv_Tenv*_tmp7EF=te;unsigned _tmp7EE=loc;Cyc_Tcexp_expr_err(_tmp7EF,_tmp7EE,0,_tag_fat(_tmp31C,sizeof(void*),5U));});});}else{_LL6: _tmp31B=_tmp31A;_LL7: {void*exn=_tmp31B;(int)_rethrow(exn);}}_LL3:;}}}}}
# 1498
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1500
struct Cyc_Tcenv_Tenv*_tmp325=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));struct Cyc_Tcenv_Tenv*te=_tmp325;
({struct Cyc_Tcenv_Tenv*_tmp7F0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F0,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp7F1=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F1,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp327=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FC;_tmp5FC.tag=0U,({struct _fat_ptr _tmp7F2=({const char*_tmp329="expecting int subscript, found ";_tag_fat(_tmp329,sizeof(char),32U);});_tmp5FC.f1=_tmp7F2;});_tmp5FC;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp328=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=2U,_tmp5FB.f1=(void*)t2;_tmp5FB;});void*_tmp326[2U];_tmp326[0]=& _tmp327,_tmp326[1]=& _tmp328;({struct Cyc_Tcenv_Tenv*_tmp7F5=te;unsigned _tmp7F4=e2->loc;void**_tmp7F3=topt;Cyc_Tcexp_expr_err(_tmp7F5,_tmp7F4,_tmp7F3,_tag_fat(_tmp326,sizeof(void*),2U));});});{
# 1509
void*_tmp32A=t1;struct Cyc_List_List*_tmp32B;void*_tmp330;void*_tmp32F;void*_tmp32E;struct Cyc_Absyn_Tqual _tmp32D;void*_tmp32C;switch(*((int*)_tmp32A)){case 3U: _LL1: _tmp32C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32A)->f1).elt_type;_tmp32D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32A)->f1).elt_tq;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32A)->f1).ptr_atts).rgn;_tmp32F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32A)->f1).ptr_atts).bounds;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32A)->f1).ptr_atts).zero_term;_LL2: {void*t=_tmp32C;struct Cyc_Absyn_Tqual tq=_tmp32D;void*rt=_tmp32E;void*b=_tmp32F;void*zt=_tmp330;
# 1513
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp331=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);struct Cyc_Absyn_Exp*eopt=_tmp331;
if(eopt != 0){
struct Cyc_Absyn_Exp*_tmp332=eopt;struct Cyc_Absyn_Exp*e3=_tmp332;
struct _tuple16 _tmp333=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp30=_tmp333;struct _tuple16 _tmp334=_stmttmp30;int _tmp336;unsigned _tmp335;_LL8: _tmp335=_tmp334.f1;_tmp336=_tmp334.f2;_LL9: {unsigned i=_tmp335;int known_i=_tmp336;
if(known_i && i == (unsigned)1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp337=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp31=_tmp337;struct _tuple16 _tmp338=_stmttmp31;int _tmp33A;unsigned _tmp339;_LLB: _tmp339=_tmp338.f1;_tmp33A=_tmp338.f2;_LLC: {unsigned j=_tmp339;int known_j=_tmp33A;
if(known_j){
struct _tuple16 _tmp33B=Cyc_Evexp_eval_const_uint_exp(e3);struct _tuple16 _stmttmp32=_tmp33B;struct _tuple16 _tmp33C=_stmttmp32;int _tmp33E;unsigned _tmp33D;_LLE: _tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_LLF: {unsigned j=_tmp33D;int knownj=_tmp33E;
if((known_i && j > i)&& i != (unsigned)1)
({void*_tmp33F=0U;({unsigned _tmp7F7=loc;struct _fat_ptr _tmp7F6=({const char*_tmp340="subscript will fail at run-time";_tag_fat(_tmp340,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7F7,_tmp7F6,_tag_fat(_tmp33F,sizeof(void*),0U));});});}}}}}}
# 1529
if(emit_warning)
({void*_tmp341=0U;({unsigned _tmp7F9=e2->loc;struct _fat_ptr _tmp7F8=({const char*_tmp342="subscript on thin, zero-terminated pointer could be expensive.";_tag_fat(_tmp342,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp7F9,_tmp7F8,_tag_fat(_tmp341,sizeof(void*),0U));});});}else{
# 1533
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _tmp343=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple16 _stmttmp33=_tmp343;struct _tuple16 _tmp344=_stmttmp33;int _tmp346;unsigned _tmp345;_LL11: _tmp345=_tmp344.f1;_tmp346=_tmp344.f2;_LL12: {unsigned i=_tmp345;int known=_tmp346;
if(known)
# 1538
({unsigned _tmp7FC=loc;unsigned _tmp7FB=i;void*_tmp7FA=b;Cyc_Tcutil_check_bound(_tmp7FC,_tmp7FB,_tmp7FA,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1541
if(Cyc_Tcutil_is_bound_one(b)&& !Cyc_Tcutil_force_type2bool(0,zt))
({void*_tmp347=0U;({unsigned _tmp7FE=e1->loc;struct _fat_ptr _tmp7FD=({const char*_tmp348="subscript applied to pointer to one object";_tag_fat(_tmp348,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp7FE,_tmp7FD,_tag_fat(_tmp347,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1546
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk)&& !Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp349=0U;({unsigned _tmp800=e1->loc;struct _fat_ptr _tmp7FF=({const char*_tmp34A="can't subscript an abstract pointer";_tag_fat(_tmp34A,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp800,_tmp7FF,_tag_fat(_tmp349,sizeof(void*),0U));});});
return t;}case 6U: _LL3: _tmp32B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp32A)->f1;_LL4: {struct Cyc_List_List*ts=_tmp32B;
return Cyc_Tcexp_ithTupleType(te,loc,ts,e2);}default: _LL5: _LL6:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp34C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FE;_tmp5FE.tag=0U,({struct _fat_ptr _tmp801=({const char*_tmp34E="subscript applied to ";_tag_fat(_tmp34E,sizeof(char),22U);});_tmp5FE.f1=_tmp801;});_tmp5FE;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp34D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5FD;_tmp5FD.tag=2U,_tmp5FD.f1=(void*)t1;_tmp5FD;});void*_tmp34B[2U];_tmp34B[0]=& _tmp34C,_tmp34B[1]=& _tmp34D;({struct Cyc_Tcenv_Tenv*_tmp804=te;unsigned _tmp803=loc;void**_tmp802=topt;Cyc_Tcexp_expr_err(_tmp804,_tmp803,_tmp802,_tag_fat(_tmp34B,sizeof(void*),2U));});});}_LL0:;}}}
# 1556
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp34F=Cyc_Tcutil_compress(*topt);void*_stmttmp34=_tmp34F;void*_tmp350=_stmttmp34;struct Cyc_List_List*_tmp351;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp350)->tag == 6U){_LL1: _tmp351=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp350)->f1;_LL2: {struct Cyc_List_List*ts=_tmp351;
# 1562
if(({int _tmp805=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp805 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1565
goto _LL0;
# 1567
for(0;es != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
void*_tmp352=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(ts))->hd)).f2;void*topt2=_tmp352;
({struct Cyc_Tcenv_Tenv*_tmp806=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp806,& topt2,(struct Cyc_Absyn_Exp*)es->hd);});
# 1572
({struct Cyc_RgnOrder_RgnPO*_tmp808=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp807=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_tmp808,_tmp807,(*((struct _tuple17*)ts->hd)).f2,& bogus);});
fields=({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));({struct _tuple17*_tmp809=({struct _tuple17*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->f1=(*((struct _tuple17*)ts->hd)).f1,_tmp353->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp353;});_tmp354->hd=_tmp809;}),_tmp354->tl=fields;_tmp354;});}
# 1575
done=1;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1579
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp80A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp80A,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp356=_cycalloc(sizeof(*_tmp356));({struct _tuple17*_tmp80C=({struct _tuple17*_tmp355=_cycalloc(sizeof(*_tmp355));({struct Cyc_Absyn_Tqual _tmp80B=Cyc_Absyn_empty_tqual(0U);_tmp355->f1=_tmp80B;}),_tmp355->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp355;});_tmp356->hd=_tmp80C;}),_tmp356->tl=fields;_tmp356;});}
# 1584
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->tag=6U,({struct Cyc_List_List*_tmp80D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp357->f1=_tmp80D;});_tmp357;});}
# 1588
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1593
struct _tuple9*_tmp358=targ;void*_tmp359;_LL1: _tmp359=_tmp358->f3;_LL2: {void*t=_tmp359;
({unsigned _tmp811=loc;struct Cyc_Tcenv_Tenv*_tmp810=te;struct Cyc_List_List*_tmp80F=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp80E=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1594
Cyc_Tctyp_check_type(_tmp811,_tmp810,_tmp80F,_tmp80E,1,0,t);});
# 1596
({void*_tmp812=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp35A=_cycalloc(sizeof(*_tmp35A));_tmp35A->tag=36U,_tmp35A->f1=0,_tmp35A->f2=des;_tmp35A;});orig_exp->r=_tmp812;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return(void*)_check_null(orig_exp->topt);}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1609 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1612
void*res_t2;
int _tmp35B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);int num_es=_tmp35B;
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35C=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->tag=0U,({union Cyc_Absyn_Cnst _tmp813=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,num_es);_tmp372->f1=_tmp813;});_tmp372;});struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp=_tmp35C;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1620
if(zero_term){
struct Cyc_Absyn_Exp*_tmp35D=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,num_es - 1);struct Cyc_Absyn_Exp*e=_tmp35D;
if(!Cyc_Tcutil_is_zero(e))
({void*_tmp35E=0U;({unsigned _tmp815=e->loc;struct _fat_ptr _tmp814=({const char*_tmp35F="zero-terminated array doesn't end with zero.";_tag_fat(_tmp35F,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp815,_tmp814,_tag_fat(_tmp35E,sizeof(void*),0U));});});}
# 1625
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp818=res;struct Cyc_Absyn_Tqual _tmp817=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1626
struct Cyc_Absyn_Exp*_tmp816=sz_exp;Cyc_Absyn_array_type(_tmp818,_tmp817,_tmp816,
# 1628
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1630
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1633
if(!({struct Cyc_RgnOrder_RgnPO*_tmp81A=Cyc_Tcenv_curr_rgnpo(te);void*_tmp819=res;Cyc_Tcutil_coerce_list(_tmp81A,_tmp819,es);}))
# 1635
({struct Cyc_String_pa_PrintArg_struct _tmp362=({struct Cyc_String_pa_PrintArg_struct _tmp5FF;_tmp5FF.tag=0U,({
struct _fat_ptr _tmp81B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(res));_tmp5FF.f1=_tmp81B;});_tmp5FF;});void*_tmp360[1U];_tmp360[0]=& _tmp362;({unsigned _tmp81D=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _fat_ptr _tmp81C=({const char*_tmp361="elements of array do not all have the same type (%s)";_tag_fat(_tmp361,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp81D,_tmp81C,_tag_fat(_tmp360,sizeof(void*),1U));});});
# 1638
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1643
void*_tmp363=(void*)ds->hd;void*_stmttmp35=_tmp363;void*_tmp364=_stmttmp35;struct Cyc_Absyn_Exp*_tmp365;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp364)->tag == 1U){_LL1: _LL2:
# 1645
({void*_tmp366=0U;({unsigned _tmp81F=loc;struct _fat_ptr _tmp81E=({const char*_tmp367="only array index designators are supported";_tag_fat(_tmp367,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp81F,_tmp81E,_tag_fat(_tmp366,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp365=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp364)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmp365;
# 1648
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _tmp368=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple16 _stmttmp36=_tmp368;struct _tuple16 _tmp369=_stmttmp36;int _tmp36B;unsigned _tmp36A;_LL6: _tmp36A=_tmp369.f1;_tmp36B=_tmp369.f2;_LL7: {unsigned i=_tmp36A;int known=_tmp36B;
if(!known)
({void*_tmp36C=0U;({unsigned _tmp821=e->loc;struct _fat_ptr _tmp820=({const char*_tmp36D="index designator cannot use sizeof or offsetof";_tag_fat(_tmp36D,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp821,_tmp820,_tag_fat(_tmp36C,sizeof(void*),0U));});});else{
if(i != (unsigned)offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp370=({struct Cyc_Int_pa_PrintArg_struct _tmp601;_tmp601.tag=1U,_tmp601.f1=(unsigned long)offset;_tmp601;});struct Cyc_Int_pa_PrintArg_struct _tmp371=({struct Cyc_Int_pa_PrintArg_struct _tmp600;_tmp600.tag=1U,_tmp600.f1=(unsigned long)((int)i);_tmp600;});void*_tmp36E[2U];_tmp36E[0]=& _tmp370,_tmp36E[1]=& _tmp371;({unsigned _tmp823=e->loc;struct _fat_ptr _tmp822=({const char*_tmp36F="expecting index designator %d but found %d";_tag_fat(_tmp36F,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp823,_tmp822,_tag_fat(_tmp36E,sizeof(void*),2U));});});}
# 1655
goto _LL0;}}}}_LL0:;}}}
# 1659
return res_t2;}
# 1663
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1666
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp373=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp37=_tmp373;void*_tmp374=_stmttmp37;void*_tmp375;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->f2 != 0){_LL1: _tmp375=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374)->f2)->hd;_LL2: {void*t=_tmp375;
# 1671
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp376=({void*_tmp824=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp824,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp376;
b->topt=bound->topt;
# 1676
bound=b;}
# 1678
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1680
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp378=({struct Cyc_Warn_String_Warn_Warg_struct _tmp603;_tmp603.tag=0U,({struct _fat_ptr _tmp825=({const char*_tmp37A="expecting unsigned int, found ";_tag_fat(_tmp37A,sizeof(char),31U);});_tmp603.f1=_tmp825;});_tmp603;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp379=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp602;_tmp602.tag=3U,_tmp602.f1=(void*)bound->topt;_tmp602;});void*_tmp377[2U];_tmp377[0]=& _tmp378,_tmp377[1]=& _tmp379;({unsigned _tmp826=bound->loc;Cyc_Warn_err2(_tmp826,_tag_fat(_tmp377,sizeof(void*),2U));});});}_LL0:;}
# 1684
if(!(vd->tq).real_const)
({void*_tmp37B=0U;({struct _fat_ptr _tmp827=({const char*_tmp37C="comprehension index variable is not declared const!";_tag_fat(_tmp37C,sizeof(char),52U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp827,_tag_fat(_tmp37B,sizeof(void*),0U));});});
# 1687
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
void**_tmp37D=0;void**topt2=_tmp37D;
struct Cyc_Absyn_Tqual*_tmp37E=0;struct Cyc_Absyn_Tqual*tqopt=_tmp37E;
void**_tmp37F=0;void**ztopt=_tmp37F;
# 1693
if(topt != 0){
void*_tmp380=Cyc_Tcutil_compress(*topt);void*_stmttmp38=_tmp380;void*_tmp381=_stmttmp38;void*_tmp385;struct Cyc_Absyn_Exp*_tmp384;struct Cyc_Absyn_Tqual _tmp383;void*_tmp382;struct Cyc_Absyn_PtrInfo _tmp386;switch(*((int*)_tmp381)){case 3U: _LL6: _tmp386=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp386;
# 1696
topt2=({void**_tmp387=_cycalloc(sizeof(*_tmp387));*_tmp387=x.elt_type;_tmp387;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp388=_cycalloc(sizeof(*_tmp388));*_tmp388=x.elt_tq;_tmp388;});
ztopt=({void**_tmp389=_cycalloc(sizeof(*_tmp389));*_tmp389=(x.ptr_atts).zero_term;_tmp389;});
goto _LL5;}case 4U: _LL8: _tmp382=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).elt_type;_tmp383=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).tq;_tmp384=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).num_elts;_tmp385=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).zero_term;_LL9: {void*t=_tmp382;struct Cyc_Absyn_Tqual tq=_tmp383;struct Cyc_Absyn_Exp*b=_tmp384;void*zt=_tmp385;
# 1701
topt2=({void**_tmp38A=_cycalloc(sizeof(*_tmp38A));*_tmp38A=t;_tmp38A;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp38B=_cycalloc(sizeof(*_tmp38B));*_tmp38B=tq;_tmp38B;});
ztopt=({void**_tmp38C=_cycalloc(sizeof(*_tmp38C));*_tmp38C=zt;_tmp38C;});
goto _LL5;}default: _LLA: _LLB:
# 1706
 goto _LL5;}_LL5:;}{
# 1709
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1711
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);
if(te->le == 0){
# 1715
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp38D=0U;({unsigned _tmp829=bound->loc;struct _fat_ptr _tmp828=({const char*_tmp38E="bound is not constant";_tag_fat(_tmp38E,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp829,_tmp828,_tag_fat(_tmp38D,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp38F=0U;({unsigned _tmp82B=bound->loc;struct _fat_ptr _tmp82A=({const char*_tmp390="body is not constant";_tag_fat(_tmp390,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp82B,_tmp82A,_tag_fat(_tmp38F,sizeof(void*),0U));});});}
# 1720
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1723
struct Cyc_Absyn_Exp*_tmp391=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp391;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1727
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt))&& !
Cyc_Tcutil_is_noalias_path(body))
Cyc_Tcexp_unique_consume_err(body->loc);{
# 1731
void*_tmp392=({void*_tmp82E=t;struct Cyc_Absyn_Tqual _tmp82D=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp82C=bound;Cyc_Absyn_array_type(_tmp82E,_tmp82D,_tmp82C,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1731
void*res=_tmp392;
# 1733
return res;}}}}
# 1737
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1740
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp393=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_stmttmp39=_tmp393;void*_tmp394=_stmttmp39;void*_tmp395;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->f2 != 0){_LL1: _tmp395=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp394)->f2)->hd;_LL2: {void*t=_tmp395;
# 1745
if((int)Cyc_Tcenv_new_status(te)== (int)Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp396=({void*_tmp82F=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp82F,Cyc_Absyn_valueof_exp(t,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*b=_tmp396;
b->topt=bound->topt;
bound=b;}
# 1750
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1752
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp398=({struct Cyc_Warn_String_Warn_Warg_struct _tmp605;_tmp605.tag=0U,({struct _fat_ptr _tmp830=({const char*_tmp39A="expecting unsigned int, found ";_tag_fat(_tmp39A,sizeof(char),31U);});_tmp605.f1=_tmp830;});_tmp605;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp399=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp604;_tmp604.tag=3U,_tmp604.f1=(void*)bound->topt;_tmp604;});void*_tmp397[2U];_tmp397[0]=& _tmp398,_tmp397[1]=& _tmp399;({unsigned _tmp831=bound->loc;Cyc_Warn_err2(_tmp831,_tag_fat(_tmp397,sizeof(void*),2U));});});}_LL0:;}{
# 1756
void**_tmp39B=0;void**topt2=_tmp39B;
struct Cyc_Absyn_Tqual*_tmp39C=0;struct Cyc_Absyn_Tqual*tqopt=_tmp39C;
void**_tmp39D=0;void**ztopt=_tmp39D;
# 1760
if(topt != 0){
void*_tmp39E=Cyc_Tcutil_compress(*topt);void*_stmttmp3A=_tmp39E;void*_tmp39F=_stmttmp3A;void*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A2;struct Cyc_Absyn_Tqual _tmp3A1;void*_tmp3A0;struct Cyc_Absyn_PtrInfo _tmp3A4;switch(*((int*)_tmp39F)){case 3U: _LL6: _tmp3A4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1;_LL7: {struct Cyc_Absyn_PtrInfo x=_tmp3A4;
# 1763
topt2=({void**_tmp3A5=_cycalloc(sizeof(*_tmp3A5));*_tmp3A5=x.elt_type;_tmp3A5;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));*_tmp3A6=x.elt_tq;_tmp3A6;});
ztopt=({void**_tmp3A7=_cycalloc(sizeof(*_tmp3A7));*_tmp3A7=(x.ptr_atts).zero_term;_tmp3A7;});
goto _LL5;}case 4U: _LL8: _tmp3A0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39F)->f1).elt_type;_tmp3A1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39F)->f1).tq;_tmp3A2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39F)->f1).num_elts;_tmp3A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39F)->f1).zero_term;_LL9: {void*t=_tmp3A0;struct Cyc_Absyn_Tqual tq=_tmp3A1;struct Cyc_Absyn_Exp*b=_tmp3A2;void*zt=_tmp3A3;
# 1768
topt2=({void**_tmp3A8=_cycalloc(sizeof(*_tmp3A8));*_tmp3A8=t;_tmp3A8;});
tqopt=({struct Cyc_Absyn_Tqual*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));*_tmp3A9=tq;_tmp3A9;});
ztopt=({void**_tmp3AA=_cycalloc(sizeof(*_tmp3AA));*_tmp3AA=zt;_tmp3AA;});
goto _LL5;}default: _LLA: _LLB:
# 1773
 goto _LL5;}_LL5:;}
# 1776
({unsigned _tmp834=loc;struct Cyc_Tcenv_Tenv*_tmp833=te;struct Cyc_List_List*_tmp832=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp834,_tmp833,_tmp832,& Cyc_Tcutil_tmk,1,1,t);});
if(topt2 != 0)Cyc_Unify_unify(*topt2,t);
# 1779
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp3AB=0U;({unsigned _tmp836=bound->loc;struct _fat_ptr _tmp835=({const char*_tmp3AC="bound is not constant";_tag_fat(_tmp3AC,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp836,_tmp835,_tag_fat(_tmp3AB,sizeof(void*),0U));});});}
# 1783
if(ztopt != 0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1786
struct Cyc_Absyn_Exp*_tmp3AD=Cyc_Absyn_uint_exp(1U,0U);struct Cyc_Absyn_Exp*e1=_tmp3AD;e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1790
({void*_tmp3AE=0U;({unsigned _tmp838=loc;struct _fat_ptr _tmp837=({const char*_tmp3AF="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_fat(_tmp3AF,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp838,_tmp837,_tag_fat(_tmp3AE,sizeof(void*),0U));});});}{
# 1793
void*_tmp3B0=({void*_tmp83B=t;struct Cyc_Absyn_Tqual _tmp83A=tqopt == 0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_tmp839=bound;Cyc_Absyn_array_type(_tmp83B,_tmp83A,_tmp839,
ztopt == 0?Cyc_Absyn_false_type:*ztopt,0U);});
# 1793
void*res=_tmp3B0;
# 1795
return res;}}}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1808 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1812
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp3B1=_cycalloc(sizeof(*_tmp3B1));*_tmp3B1=ad;_tmp3B1;});}else{
# 1818
{struct _handler_cons _tmp3B2;_push_handler(& _tmp3B2);{int _tmp3B4=0;if(setjmp(_tmp3B2.handler))_tmp3B4=1;if(!_tmp3B4){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1818
;_pop_handler();}else{void*_tmp3B3=(void*)Cyc_Core_get_exn_thrown();void*_tmp3B5=_tmp3B3;void*_tmp3B6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3B5)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1821
({struct Cyc_String_pa_PrintArg_struct _tmp3B9=({struct Cyc_String_pa_PrintArg_struct _tmp606;_tmp606.tag=0U,({struct _fat_ptr _tmp83C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp606.f1=_tmp83C;});_tmp606;});void*_tmp3B7[1U];_tmp3B7[0]=& _tmp3B9;({unsigned _tmp83E=loc;struct _fat_ptr _tmp83D=({const char*_tmp3B8="unbound struct/union name %s";_tag_fat(_tmp3B8,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp83E,_tmp83D,_tag_fat(_tmp3B7,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp3B6=_tmp3B5;_LL4: {void*exn=_tmp3B6;(int)_rethrow(exn);}}_LL0:;}}}
# 1824
*ad_opt=ad;
*tn=ad->name;}
# 1827
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3BC=({struct Cyc_String_pa_PrintArg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp83F=(struct _fat_ptr)((int)ad->kind == (int)Cyc_Absyn_StructA?({const char*_tmp3BD="struct";_tag_fat(_tmp3BD,sizeof(char),7U);}):({const char*_tmp3BE="union";_tag_fat(_tmp3BE,sizeof(char),6U);}));_tmp607.f1=_tmp83F;});_tmp607;});void*_tmp3BA[1U];_tmp3BA[0]=& _tmp3BC;({unsigned _tmp841=loc;struct _fat_ptr _tmp840=({const char*_tmp3BB="can't construct abstract %s";_tag_fat(_tmp3BB,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp841,_tmp840,_tag_fat(_tmp3BA,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1835
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp3BF=Cyc_Tcenv_new_status(te);enum Cyc_Tcenv_NewStatus status=_tmp3BF;
if((int)status == (int)1U)
te2=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);else{
# 1844
te2=Cyc_Tcenv_set_new_status(status,te);}{
# 1846
struct _tuple13 _tmp3C0=({struct _tuple13 _tmp610;({struct Cyc_List_List*_tmp842=Cyc_Tcenv_lookup_type_vars(te2);_tmp610.f1=_tmp842;}),_tmp610.f2=Cyc_Core_heap_region;_tmp610;});struct _tuple13 env=_tmp3C0;
struct Cyc_List_List*_tmp3C1=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);struct Cyc_List_List*all_inst=_tmp3C1;
struct Cyc_List_List*_tmp3C2=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);struct Cyc_List_List*exist_inst=_tmp3C2;
struct Cyc_List_List*_tmp3C3=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,all_inst);struct Cyc_List_List*all_typs=_tmp3C3;
struct Cyc_List_List*_tmp3C4=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,exist_inst);struct Cyc_List_List*exist_typs=_tmp3C4;
struct Cyc_List_List*_tmp3C5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(all_inst,exist_inst);struct Cyc_List_List*inst=_tmp3C5;
void*res_typ;
# 1857
if(topt != 0){
void*_tmp3C6=Cyc_Tcutil_compress(*topt);void*_stmttmp3B=_tmp3C6;void*_tmp3C7=_stmttmp3B;struct Cyc_List_List*_tmp3C9;struct Cyc_Absyn_Aggrdecl**_tmp3C8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C7)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C7)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C7)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp3C8=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C7)->f1)->f1).KnownAggr).val;_tmp3C9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C7)->f2;_LL7: {struct Cyc_Absyn_Aggrdecl**adptr2=_tmp3C8;struct Cyc_List_List*all_typs2=_tmp3C9;
# 1860
if(*adptr2 == *adptr){
{struct Cyc_List_List*_tmp3CA=all_typs;struct Cyc_List_List*ats=_tmp3CA;for(0;ats != 0 && all_typs2 != 0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1865
res_typ=*topt;
goto _LL5;}
# 1868
goto _LL9;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1870
 res_typ=({union Cyc_Absyn_AggrInfo _tmp843=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp843,all_typs);});}_LL5:;}else{
# 1873
res_typ=({union Cyc_Absyn_AggrInfo _tmp844=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp844,all_typs);});}{
# 1876
struct Cyc_List_List*_tmp3CB=*ts;struct Cyc_List_List*user_ex_ts=_tmp3CB;
struct Cyc_List_List*_tmp3CC=exist_typs;struct Cyc_List_List*ex_ts=_tmp3CC;
while(user_ex_ts != 0 && ex_ts != 0){
# 1880
({unsigned _tmp847=loc;struct Cyc_Tcenv_Tenv*_tmp846=te2;struct Cyc_List_List*_tmp845=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_tmp847,_tmp846,_tmp845,& Cyc_Tcutil_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1886
if(user_ex_ts != 0)
({void*_tmp3CD=0U;({unsigned _tmp849=loc;struct _fat_ptr _tmp848=({const char*_tmp3CE="too many explicit witness types";_tag_fat(_tmp3CE,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp849,_tmp848,_tag_fat(_tmp3CD,sizeof(void*),0U));});});
# 1889
*ts=exist_typs;{
# 1892
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3CF=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3C=_tmp3CF;struct _tuple19*_tmp3D0=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Aggrfield*_tmp3D1;_LLB: _tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;_LLC: {struct Cyc_Absyn_Aggrfield*field=_tmp3D1;struct Cyc_Absyn_Exp*field_exp=_tmp3D2;
void*_tmp3D3=Cyc_Tcutil_substitute(inst,field->type);void*inst_fieldtyp=_tmp3D3;
({struct Cyc_Tcenv_Tenv*_tmp84A=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp84A,& inst_fieldtyp,field_exp);});
# 1900
if(!({struct Cyc_RgnOrder_RgnPO*_tmp84C=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_Absyn_Exp*_tmp84B=field_exp;Cyc_Tcutil_coerce_arg(_tmp84C,_tmp84B,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60F;_tmp60F.tag=0U,({struct _fat_ptr _tmp84D=({const char*_tmp3E0="field ";_tag_fat(_tmp3E0,sizeof(char),7U);});_tmp60F.f1=_tmp84D;});_tmp60F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60E;_tmp60E.tag=0U,_tmp60E.f1=*field->name;_tmp60E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D;_tmp60D.tag=0U,({struct _fat_ptr _tmp84E=({const char*_tmp3DF=" of ";_tag_fat(_tmp3DF,sizeof(char),5U);});_tmp60D.f1=_tmp84E;});_tmp60D;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp3D8=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _tmp60C;_tmp60C.tag=6U,_tmp60C.f1=ad;_tmp60C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3D9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60B;_tmp60B.tag=0U,({
struct _fat_ptr _tmp84F=({const char*_tmp3DE=" expects type ";_tag_fat(_tmp3DE,sizeof(char),15U);});_tmp60B.f1=_tmp84F;});_tmp60B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3DA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60A;_tmp60A.tag=2U,_tmp60A.f1=(void*)inst_fieldtyp;_tmp60A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp609;_tmp609.tag=0U,({struct _fat_ptr _tmp850=({const char*_tmp3DD=" != ";_tag_fat(_tmp3DD,sizeof(char),5U);});_tmp609.f1=_tmp850;});_tmp609;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3DC=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp608;_tmp608.tag=3U,_tmp608.f1=(void*)field_exp->topt;_tmp608;});void*_tmp3D4[8U];_tmp3D4[0]=& _tmp3D5,_tmp3D4[1]=& _tmp3D6,_tmp3D4[2]=& _tmp3D7,_tmp3D4[3]=& _tmp3D8,_tmp3D4[4]=& _tmp3D9,_tmp3D4[5]=& _tmp3DA,_tmp3D4[6]=& _tmp3DB,_tmp3D4[7]=& _tmp3DC;({unsigned _tmp851=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp851,_tag_fat(_tmp3D4,sizeof(void*),8U));});});}}{
# 1905
struct Cyc_List_List*_tmp3E1=0;struct Cyc_List_List*rpo_inst=_tmp3E1;
{struct Cyc_List_List*_tmp3E2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;struct Cyc_List_List*rpo=_tmp3E2;for(0;rpo != 0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));({struct _tuple0*_tmp854=({struct _tuple0*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));({void*_tmp853=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_tmp3E3->f1=_tmp853;}),({
void*_tmp852=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f2);_tmp3E3->f2=_tmp852;});_tmp3E3;});
# 1907
_tmp3E4->hd=_tmp854;}),_tmp3E4->tl=rpo_inst;_tmp3E4;});}}
# 1910
rpo_inst=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
return res_typ;}}}}}}
# 1916
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*ts,struct Cyc_List_List*args){
# 1918
{void*_tmp3E5=Cyc_Tcutil_compress(ts);void*_stmttmp3D=_tmp3E5;void*_tmp3E6=_stmttmp3D;struct Cyc_List_List*_tmp3E8;enum Cyc_Absyn_AggrKind _tmp3E7;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E6)->tag == 7U){_LL1: _tmp3E7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E6)->f1;_tmp3E8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E6)->f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp3E7;struct Cyc_List_List*fs=_tmp3E8;
# 1920
if((int)k == (int)1U)
({void*_tmp3E9=0U;({unsigned _tmp856=loc;struct _fat_ptr _tmp855=({const char*_tmp3EA="expecting struct but found union";_tag_fat(_tmp3EA,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp856,_tmp855,_tag_fat(_tmp3E9,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,fs);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3EB=(struct _tuple19*)fields->hd;struct _tuple19*_stmttmp3E=_tmp3EB;struct _tuple19*_tmp3EC=_stmttmp3E;struct Cyc_Absyn_Exp*_tmp3EE;struct Cyc_Absyn_Aggrfield*_tmp3ED;_LL6: _tmp3ED=_tmp3EC->f1;_tmp3EE=_tmp3EC->f2;_LL7: {struct Cyc_Absyn_Aggrfield*field=_tmp3ED;struct Cyc_Absyn_Exp*field_exp=_tmp3EE;
({struct Cyc_Tcenv_Tenv*_tmp858=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp857=& field->type;Cyc_Tcexp_tcExpInitializer(_tmp858,_tmp857,field_exp);});
# 1929
if(!({struct Cyc_RgnOrder_RgnPO*_tmp85A=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp859=field_exp;Cyc_Tcutil_coerce_arg(_tmp85A,_tmp859,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0U,({struct _fat_ptr _tmp85B=({const char*_tmp3F8="field ";_tag_fat(_tmp3F8,sizeof(char),7U);});_tmp616.f1=_tmp85B;});_tmp616;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp615;_tmp615.tag=0U,_tmp615.f1=*field->name;_tmp615;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp614;_tmp614.tag=0U,({
struct _fat_ptr _tmp85C=({const char*_tmp3F7=" of struct expects type ";_tag_fat(_tmp3F7,sizeof(char),25U);});_tmp614.f1=_tmp85C;});_tmp614;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp3F3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp613;_tmp613.tag=2U,_tmp613.f1=(void*)field->type;_tmp613;});struct Cyc_Warn_String_Warn_Warg_struct _tmp3F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp612;_tmp612.tag=0U,({struct _fat_ptr _tmp85D=({const char*_tmp3F6=" != ";_tag_fat(_tmp3F6,sizeof(char),5U);});_tmp612.f1=_tmp85D;});_tmp612;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp3F5=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _tmp611;_tmp611.tag=3U,_tmp611.f1=(void*)field_exp->topt;_tmp611;});void*_tmp3EF[6U];_tmp3EF[0]=& _tmp3F0,_tmp3EF[1]=& _tmp3F1,_tmp3EF[2]=& _tmp3F2,_tmp3EF[3]=& _tmp3F3,_tmp3EF[4]=& _tmp3F4,_tmp3EF[5]=& _tmp3F5;({unsigned _tmp85E=field_exp->loc;Cyc_Tcexp_err_and_explain(_tmp85E,_tag_fat(_tmp3EF,sizeof(void*),6U));});});}}
# 1934
goto _LL0;}}}else{_LL3: _LL4:
({void*_tmp3F9=0U;({struct _fat_ptr _tmp85F=({const char*_tmp3FA="tcAnonStruct: wrong type";_tag_fat(_tmp3FA,sizeof(char),25U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp85F,_tag_fat(_tmp3F9,sizeof(void*),0U));});});}_LL0:;}
# 1937
return ts;}
# 1941
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1944
struct _tuple13 _tmp3FB=({struct _tuple13 _tmp61E;({struct Cyc_List_List*_tmp860=Cyc_Tcenv_lookup_type_vars(te);_tmp61E.f1=_tmp860;}),_tmp61E.f2=Cyc_Core_heap_region;_tmp61E;});struct _tuple13 env=_tmp3FB;
struct Cyc_List_List*_tmp3FC=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);struct Cyc_List_List*inst=_tmp3FC;
struct Cyc_List_List*_tmp3FD=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);struct Cyc_List_List*all_typs=_tmp3FD;
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp861=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp861,all_typs);});
# 1949
if(topt != 0){
void*_tmp3FE=Cyc_Tcutil_compress(*topt);void*_stmttmp3F=_tmp3FE;void*_tmp3FF=_stmttmp3F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FF)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FF)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 1955
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0 && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(inst != 0)t=Cyc_Tcutil_substitute(inst,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_tmp863=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_tmp862=e;Cyc_Tcutil_coerce_arg(_tmp863,_tmp862,t,& bogus);})){
({struct Cyc_String_pa_PrintArg_struct _tmp402=({struct Cyc_String_pa_PrintArg_struct _tmp619;_tmp619.tag=0U,({
# 1965
struct _fat_ptr _tmp864=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp619.f1=_tmp864;});_tmp619;});struct Cyc_String_pa_PrintArg_struct _tmp403=({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0U,({struct _fat_ptr _tmp865=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp618.f1=_tmp865;});_tmp618;});struct Cyc_String_pa_PrintArg_struct _tmp404=({struct Cyc_String_pa_PrintArg_struct _tmp617;_tmp617.tag=0U,({
struct _fat_ptr _tmp866=(struct _fat_ptr)((struct _fat_ptr)(e->topt == 0?(struct _fat_ptr)({const char*_tmp405="?";_tag_fat(_tmp405,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp617.f1=_tmp866;});_tmp617;});void*_tmp400[3U];_tmp400[0]=& _tmp402,_tmp400[1]=& _tmp403,_tmp400[2]=& _tmp404;({unsigned _tmp868=e->loc;struct _fat_ptr _tmp867=({const char*_tmp401="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_fat(_tmp401,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp868,_tmp867,_tag_fat(_tmp400,sizeof(void*),3U));});});
Cyc_Unify_explain_failure();}}
# 1970
if(es != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp407=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61B;_tmp61B.tag=0U,({
struct _fat_ptr _tmp869=({const char*_tmp409="too many arguments for datatype constructor ";_tag_fat(_tmp409,sizeof(char),45U);});_tmp61B.f1=_tmp869;});_tmp61B;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp408=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp61A;_tmp61A.tag=1U,_tmp61A.f1=tuf->name;_tmp61A;});void*_tmp406[2U];_tmp406[0]=& _tmp407,_tmp406[1]=& _tmp408;({struct Cyc_Tcenv_Tenv*_tmp86C=te;unsigned _tmp86B=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp86A=topt;Cyc_Tcexp_expr_err(_tmp86C,_tmp86B,_tmp86A,_tag_fat(_tmp406,sizeof(void*),2U));});});
if(ts != 0)
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp40B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61D;_tmp61D.tag=0U,({
struct _fat_ptr _tmp86D=({const char*_tmp40D="too few arguments for datatype constructor ";_tag_fat(_tmp40D,sizeof(char),44U);});_tmp61D.f1=_tmp86D;});_tmp61D;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp40C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp61C;_tmp61C.tag=1U,_tmp61C.f1=tuf->name;_tmp61C;});void*_tmp40A[2U];_tmp40A[0]=& _tmp40B,_tmp40A[1]=& _tmp40C;({struct Cyc_Tcenv_Tenv*_tmp870=te;unsigned _tmp86F=loc;void**_tmp86E=topt;Cyc_Tcexp_expr_err(_tmp870,_tmp86F,_tmp86E,_tag_fat(_tmp40A,sizeof(void*),2U));});});
return res;}}
# 1980
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1982
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1985
if(topt == 0)
return 0;{
void*_tmp40E=Cyc_Tcutil_compress(*topt);void*_stmttmp40=_tmp40E;void*_tmp40F=_stmttmp40;void*_tmp410;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40F)->tag == 3U){_LL1: _tmp410=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40F)->f1).elt_type;_LL2: {void*elt_typ=_tmp410;
# 1989
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1995
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1997
enum Cyc_Absyn_AliasQual _tmp411=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp41=_tmp411;enum Cyc_Absyn_AliasQual _tmp412=_stmttmp41;if(_tmp412 == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 2003
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2007
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 2011
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp413=Cyc_Tcutil_compress(handle_type);void*_stmttmp42=_tmp413;void*_tmp414=_stmttmp42;void*_tmp415;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp414)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp414)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp414)->f2 != 0){_LL1: _tmp415=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp414)->f2)->hd;_LL2: {void*r=_tmp415;
# 2016
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2020
({struct Cyc_String_pa_PrintArg_struct _tmp418=({struct Cyc_String_pa_PrintArg_struct _tmp61F;_tmp61F.tag=0U,({
struct _fat_ptr _tmp871=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp61F.f1=_tmp871;});_tmp61F;});void*_tmp416[1U];_tmp416[0]=& _tmp418;({unsigned _tmp873=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _fat_ptr _tmp872=({const char*_tmp417="expecting region_t type but found %s";_tag_fat(_tmp417,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp873,_tmp872,_tag_fat(_tmp416,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2027
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp874=Cyc_Absyn_uniquergn_exp();*ropt=_tmp874;});}}}
# 2034
({struct Cyc_Tcenv_Tenv*_tmp875=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp875,& Cyc_Absyn_uint_type,*e);});{
# 2043 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp419=0U;({struct _fat_ptr _tmp876=({const char*_tmp41A="calloc with empty type";_tag_fat(_tmp41A,sizeof(char),23U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp876,_tag_fat(_tmp419,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned _tmp879=loc;struct Cyc_Tcenv_Tenv*_tmp878=te;struct Cyc_List_List*_tmp877=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_tmp879,_tmp878,_tmp877,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp41D=({struct Cyc_String_pa_PrintArg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp87A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(elt_type));_tmp620.f1=_tmp87A;});_tmp620;});void*_tmp41B[1U];_tmp41B[0]=& _tmp41D;({unsigned _tmp87C=loc;struct _fat_ptr _tmp87B=({const char*_tmp41C="calloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp41C,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp87C,_tmp87B,_tag_fat(_tmp41B,sizeof(void*),1U));});});
num_elts=*e;
one_elt=0;}else{
# 2056
void*er=(*e)->r;
retry_sizeof: {
void*_tmp41E=er;struct Cyc_Absyn_Exp*_tmp420;struct Cyc_Absyn_Exp*_tmp41F;void*_tmp421;switch(*((int*)_tmp41E)){case 17U: _LL6: _tmp421=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp41E)->f1;_LL7: {void*t2=_tmp421;
# 2060
elt_type=t2;
({void**_tmp87D=({void**_tmp422=_cycalloc(sizeof(*_tmp422));*_tmp422=elt_type;_tmp422;});*t=_tmp87D;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41E)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41E)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41E)->f2)->tl)->tl == 0){_LL8: _tmp41F=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41E)->f2)->hd;_tmp420=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp41E)->f2)->tl)->hd;_LL9: {struct Cyc_Absyn_Exp*e1=_tmp41F;struct Cyc_Absyn_Exp*e2=_tmp420;
# 2067
{struct _tuple0 _tmp423=({struct _tuple0 _tmp623;_tmp623.f1=e1->r,_tmp623.f2=e2->r;_tmp623;});struct _tuple0 _stmttmp43=_tmp423;struct _tuple0 _tmp424=_stmttmp43;void*_tmp425;void*_tmp426;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp424.f1)->tag == 17U){_LLD: _tmp426=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp424.f1)->f1;_LLE: {void*t1=_tmp426;
# 2070
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t1)){
# 2073
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t1))
({struct Cyc_String_pa_PrintArg_struct _tmp429=({struct Cyc_String_pa_PrintArg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp87E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp621.f1=_tmp87E;});_tmp621;});void*_tmp427[1U];_tmp427[0]=& _tmp429;({unsigned _tmp880=loc;struct _fat_ptr _tmp87F=({const char*_tmp428="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp428,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp880,_tmp87F,_tag_fat(_tmp427,sizeof(void*),1U));});});else{
# 2076
*is_calloc=1;}}
# 2079
elt_type=t1;
({void**_tmp881=({void**_tmp42A=_cycalloc(sizeof(*_tmp42A));*_tmp42A=elt_type;_tmp42A;});*t=_tmp881;});
num_elts=e2;
one_elt=0;
goto _LLC;}}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp424.f2)->tag == 17U){_LLF: _tmp425=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp424.f2)->f1;_LL10: {void*t2=_tmp425;
# 2086
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 2089
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp42D=({struct Cyc_String_pa_PrintArg_struct _tmp622;_tmp622.tag=0U,({struct _fat_ptr _tmp882=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp622.f1=_tmp882;});_tmp622;});void*_tmp42B[1U];_tmp42B[0]=& _tmp42D;({unsigned _tmp884=loc;struct _fat_ptr _tmp883=({const char*_tmp42C="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_fat(_tmp42C,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp884,_tmp883,_tag_fat(_tmp42B,sizeof(void*),1U));});});else{
# 2092
*is_calloc=1;}}
# 2095
elt_type=t2;
({void**_tmp885=({void**_tmp42E=_cycalloc(sizeof(*_tmp42E));*_tmp42E=elt_type;_tmp42E;});*t=_tmp885;});
num_elts=e1;
one_elt=0;
goto _LLC;}}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2102
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2104
 No_sizeof: {
# 2107
struct Cyc_Absyn_Exp*_tmp42F=*e;struct Cyc_Absyn_Exp*real_e=_tmp42F;
{void*_tmp430=real_e->r;void*_stmttmp44=_tmp430;void*_tmp431=_stmttmp44;struct Cyc_Absyn_Exp*_tmp432;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp431)->tag == 14U){_LL14: _tmp432=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp431)->f2;_LL15: {struct Cyc_Absyn_Exp*e=_tmp432;
real_e=e;goto _LL13;}}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2112
{void*_tmp433=Cyc_Tcutil_compress((void*)_check_null(real_e->topt));void*_stmttmp45=_tmp433;void*_tmp434=_stmttmp45;void*_tmp435;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->f2 != 0){_LL19: _tmp435=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp434)->f2)->hd;_LL1A: {void*tagt=_tmp435;
# 2114
{void*_tmp436=Cyc_Tcutil_compress(tagt);void*_stmttmp46=_tmp436;void*_tmp437=_stmttmp46;struct Cyc_Absyn_Exp*_tmp438;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp437)->tag == 9U){_LL1E: _tmp438=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp437)->f1;_LL1F: {struct Cyc_Absyn_Exp*vexp=_tmp438;
er=vexp->r;goto retry_sizeof;}}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2118
goto _LL18;}}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2121
elt_type=Cyc_Absyn_char_type;
({void**_tmp886=({void**_tmp439=_cycalloc(sizeof(*_tmp439));*_tmp439=elt_type;_tmp439;});*t=_tmp886;});
num_elts=*e;
one_elt=0;}
# 2126
goto _LL5;}_LL5:;}}
# 2130
*is_fat=!one_elt;
# 2133
{void*_tmp43A=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp43B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43A)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43A)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp43B=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43A)->f1)->f1).KnownAggr).val;_LL24: {struct Cyc_Absyn_Aggrdecl*ad=_tmp43B;
# 2135
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({void*_tmp43C=0U;({unsigned _tmp888=loc;struct _fat_ptr _tmp887=({const char*_tmp43D="malloc with existential types not yet implemented";_tag_fat(_tmp43D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp888,_tmp887,_tag_fat(_tmp43C,sizeof(void*),0U));});});
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2142
void*(*_tmp43E)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=Cyc_Absyn_at_type;void*(*ptr_maker)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=_tmp43E;
void*_tmp43F=Cyc_Absyn_false_type;void*zero_term=_tmp43F;
if(topt != 0){
void*_tmp440=Cyc_Tcutil_compress(*topt);void*_stmttmp47=_tmp440;void*_tmp441=_stmttmp47;void*_tmp444;void*_tmp443;void*_tmp442;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp441)->tag == 3U){_LL28: _tmp442=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp441)->f1).ptr_atts).nullable;_tmp443=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp441)->f1).ptr_atts).bounds;_tmp444=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp441)->f1).ptr_atts).zero_term;_LL29: {void*n=_tmp442;void*b=_tmp443;void*zt=_tmp444;
# 2147
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2152
if(Cyc_Tcutil_force_type2bool(0,zt)&& !(*is_calloc)){
({void*_tmp445=0U;({unsigned _tmp88A=loc;struct _fat_ptr _tmp889=({const char*_tmp446="converting malloc to calloc to ensure zero-termination";_tag_fat(_tmp446,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp88A,_tmp889,_tag_fat(_tmp445,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2158
struct Cyc_Absyn_Exp*_tmp447=({void*_tmp88B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp88B,b);});struct Cyc_Absyn_Exp*eopt=_tmp447;
if(eopt != 0 && !one_elt){
struct Cyc_Absyn_Exp*_tmp448=eopt;struct Cyc_Absyn_Exp*upper_exp=_tmp448;
int _tmp449=Cyc_Evexp_c_can_eval(num_elts);int is_constant=_tmp449;
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_tmp88F=elt_type;void*_tmp88E=rgn;struct Cyc_Absyn_Tqual _tmp88D=Cyc_Absyn_empty_tqual(0U);void*_tmp88C=b;Cyc_Absyn_atb_type(_tmp88F,_tmp88E,_tmp88D,_tmp88C,zero_term);});}{
# 2166
void*_tmp44A=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_stmttmp48=_tmp44A;void*_tmp44B=_stmttmp48;void*_tmp44C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44B)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44B)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44B)->f2 != 0){_LL2D: _tmp44C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44B)->f2)->hd;_LL2E: {void*tagtyp=_tmp44C;
# 2168
struct Cyc_Absyn_Exp*_tmp44D=({void*_tmp890=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp890,Cyc_Absyn_valueof_exp(tagtyp,0U),0,Cyc_Absyn_No_coercion,0U);});struct Cyc_Absyn_Exp*tagtyp_exp=_tmp44D;
# 2170
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_tmp894=elt_type;void*_tmp893=rgn;struct Cyc_Absyn_Tqual _tmp892=Cyc_Absyn_empty_tqual(0U);void*_tmp891=b;Cyc_Absyn_atb_type(_tmp894,_tmp893,_tmp892,_tmp891,zero_term);});}
# 2174
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;}}
# 2178
goto _LL27;}}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2181
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_tmp898)(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zero_term)=ptr_maker;void*_tmp897=elt_type;void*_tmp896=rgn;struct Cyc_Absyn_Tqual _tmp895=Cyc_Absyn_empty_tqual(0U);_tmp898(_tmp897,_tmp896,_tmp895,zero_term);});}}}
# 2186
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2188
struct Cyc_Tcenv_Tenv*_tmp44E=Cyc_Tcenv_enter_lhs(te);struct Cyc_Tcenv_Tenv*te2=_tmp44E;
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*_tmp44F=(void*)_check_null(e1->topt);void*t1=_tmp44F;
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2195
{void*_tmp450=Cyc_Tcutil_compress(t1);void*_stmttmp49=_tmp450;void*_tmp451=_stmttmp49;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp451)->tag == 4U){_LL1: _LL2:
({void*_tmp452=0U;({unsigned _tmp89A=loc;struct _fat_ptr _tmp899=({const char*_tmp453="cannot assign to an array";_tag_fat(_tmp453,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp89A,_tmp899,_tag_fat(_tmp452,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2200
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp454=0U;({unsigned _tmp89C=loc;struct _fat_ptr _tmp89B=({const char*_tmp455="Swap not allowed for non-pointer or non-word-sized types.";_tag_fat(_tmp455,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp89C,_tmp89B,_tag_fat(_tmp454,sizeof(void*),0U));});});
# 2204
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp457=({struct Cyc_Warn_String_Warn_Warg_struct _tmp624;_tmp624.tag=0U,({struct _fat_ptr _tmp89D=({const char*_tmp458="swap non-lvalue";_tag_fat(_tmp458,sizeof(char),16U);});_tmp624.f1=_tmp89D;});_tmp624;});void*_tmp456[1U];_tmp456[0]=& _tmp457;({struct Cyc_Tcenv_Tenv*_tmp8A0=te;unsigned _tmp89F=e1->loc;void**_tmp89E=topt;Cyc_Tcexp_expr_err(_tmp8A0,_tmp89F,_tmp89E,_tag_fat(_tmp456,sizeof(void*),1U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp45A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp8A1=({const char*_tmp45B="swap non-lvalue";_tag_fat(_tmp45B,sizeof(char),16U);});_tmp625.f1=_tmp8A1;});_tmp625;});void*_tmp459[1U];_tmp459[0]=& _tmp45A;({struct Cyc_Tcenv_Tenv*_tmp8A4=te;unsigned _tmp8A3=e2->loc;void**_tmp8A2=topt;Cyc_Tcexp_expr_err(_tmp8A4,_tmp8A3,_tmp8A2,_tag_fat(_tmp459,sizeof(void*),1U));});});
# 2209
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp45D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp629;_tmp629.tag=0U,({struct _fat_ptr _tmp8A5=({const char*_tmp462="type mismatch: ";_tag_fat(_tmp462,sizeof(char),16U);});_tmp629.f1=_tmp8A5;});_tmp629;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp45E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp628;_tmp628.tag=2U,_tmp628.f1=(void*)t1;_tmp628;});struct Cyc_Warn_String_Warn_Warg_struct _tmp45F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp627;_tmp627.tag=0U,({struct _fat_ptr _tmp8A6=({const char*_tmp461=" != ";_tag_fat(_tmp461,sizeof(char),5U);});_tmp627.f1=_tmp8A6;});_tmp627;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp460=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp626;_tmp626.tag=2U,_tmp626.f1=(void*)t2;_tmp626;});void*_tmp45C[4U];_tmp45C[0]=& _tmp45D,_tmp45C[1]=& _tmp45E,_tmp45C[2]=& _tmp45F,_tmp45C[3]=& _tmp460;({struct Cyc_Tcenv_Tenv*_tmp8A9=te;unsigned _tmp8A8=loc;void**_tmp8A7=topt;Cyc_Tcexp_expr_err(_tmp8A9,_tmp8A8,_tmp8A7,_tag_fat(_tmp45C,sizeof(void*),4U));});});
return Cyc_Absyn_void_type;}}}
# 2218
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp8AA=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp8AA,s,1);});
# 2221
while(1){
void*_tmp463=s->r;void*_stmttmp4A=_tmp463;void*_tmp464=_stmttmp4A;struct Cyc_Absyn_Stmt*_tmp465;struct Cyc_Absyn_Stmt*_tmp466;struct Cyc_Absyn_Exp*_tmp467;switch(*((int*)_tmp464)){case 1U: _LL1: _tmp467=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp464)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp467;
# 2225
void*_tmp468=(void*)_check_null(e->topt);void*t=_tmp468;
if(!({void*_tmp8AB=t;Cyc_Unify_unify(_tmp8AB,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp46A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62B;_tmp62B.tag=0U,({struct _fat_ptr _tmp8AC=({const char*_tmp46C="statement expression returns type ";_tag_fat(_tmp46C,sizeof(char),35U);});_tmp62B.f1=_tmp8AC;});_tmp62B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp46B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62A;_tmp62A.tag=2U,_tmp62A.f1=(void*)t;_tmp62A;});void*_tmp469[2U];_tmp469[0]=& _tmp46A,_tmp469[1]=& _tmp46B;({unsigned _tmp8AD=loc;Cyc_Tcexp_err_and_explain(_tmp8AD,_tag_fat(_tmp469,sizeof(void*),2U));});});
return t;}case 2U: _LL3: _tmp466=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp464)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_tmp466;
s=s2;continue;}case 12U: _LL5: _tmp465=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp464)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmp465;
s=s1;continue;}default: _LL7: _LL8:
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp46E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0U,({
struct _fat_ptr _tmp8AE=({const char*_tmp46F="statement expression must end with expression";_tag_fat(_tmp46F,sizeof(char),46U);});_tmp62C.f1=_tmp8AE;});_tmp62C;});void*_tmp46D[1U];_tmp46D[0]=& _tmp46E;({struct Cyc_Tcenv_Tenv*_tmp8B1=te;unsigned _tmp8B0=loc;void**_tmp8AF=topt;Cyc_Tcexp_expr_err(_tmp8B1,_tmp8B0,_tmp8AF,_tag_fat(_tmp46D,sizeof(void*),1U));});});}_LL0:;}}
# 2236
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp8B2=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp8B2,0,e);}));
{void*_tmp470=t;struct Cyc_Absyn_Aggrdecl*_tmp471;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp471=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp470)->f1)->f1).KnownAggr).val;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmp471;
# 2240
if(((int)ad->kind == (int)Cyc_Absyn_UnionA && ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2243
({struct Cyc_Warn_String_Warn_Warg_struct _tmp473=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62E;_tmp62E.tag=0U,({struct _fat_ptr _tmp8B3=({const char*_tmp475="expecting @tagged union but found ";_tag_fat(_tmp475,sizeof(char),35U);});_tmp62E.f1=_tmp8B3;});_tmp62E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp474=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62D;_tmp62D.tag=2U,_tmp62D.f1=(void*)t;_tmp62D;});void*_tmp472[2U];_tmp472[0]=& _tmp473,_tmp472[1]=& _tmp474;({unsigned _tmp8B4=loc;Cyc_Warn_err2(_tmp8B4,_tag_fat(_tmp472,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}
# 2246
return Cyc_Absyn_uint_type;}
# 2249
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
({struct Cyc_Tcenv_Tenv*_tmp8B6=te;struct Cyc_Absyn_Exp*_tmp8B5=e;Cyc_Tcexp_tcTest(_tmp8B6,_tmp8B5,({const char*_tmp476="@assert";_tag_fat(_tmp476,sizeof(char),8U);}));});
if(!Cyc_Absyn_no_side_effects_exp(e))
({void*_tmp477=0U;({unsigned _tmp8B8=loc;struct _fat_ptr _tmp8B7=({const char*_tmp478="@assert expression may have side effects";_tag_fat(_tmp478,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp8B8,_tmp8B7,_tag_fat(_tmp477,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 2257
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2261
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp479=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));struct Cyc_Tcenv_Tenv*te=_tmp479;
if(*rgn_handle != 0){
# 2266
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp47A=Cyc_Tcutil_compress(handle_type);void*_stmttmp4B=_tmp47A;void*_tmp47B=_stmttmp4B;void*_tmp47C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->f2 != 0){_LL1: _tmp47C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp47B)->f2)->hd;_LL2: {void*r=_tmp47C;
# 2271
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2275
({struct Cyc_String_pa_PrintArg_struct _tmp47F=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({
struct _fat_ptr _tmp8B9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp62F.f1=_tmp8B9;});_tmp62F;});void*_tmp47D[1U];_tmp47D[0]=& _tmp47F;({unsigned _tmp8BB=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _fat_ptr _tmp8BA=({const char*_tmp47E="expecting region_t type but found %s";_tag_fat(_tmp47E,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp8BB,_tmp8BA,_tag_fat(_tmp47D,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2279
if(topt != 0){
# 2282
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)({struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Absyn_uniquergn_exp();*rgn_handle=_tmp8BC;});}}}{
# 2289
void*_tmp480=e1->r;void*_stmttmp4C=_tmp480;void*_tmp481=_stmttmp4C;struct Cyc_List_List*_tmp482;struct Cyc_List_List*_tmp484;struct Cyc_Core_Opt*_tmp483;switch(*((int*)_tmp481)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2294
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp485=0U;({struct _fat_ptr _tmp8BD=({const char*_tmp486="tcNew: comprehension returned non-array type";_tag_fat(_tmp486,sizeof(char),45U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8BD,_tag_fat(_tmp485,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8C0=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8BF=loc;void*_tmp8BE=res_typ;Cyc_Tcutil_silent_castable(_tmp8C0,_tmp8BF,_tmp8BE,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2306
return res_typ;}case 36U: _LLA: _tmp483=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp481)->f1;_tmp484=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp481)->f2;_LLB: {struct Cyc_Core_Opt*nopt=_tmp483;struct Cyc_List_List*des=_tmp484;
# 2308
({void*_tmp8C1=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->tag=26U,_tmp487->f1=des;_tmp487;});e1->r=_tmp8C1;});
_tmp482=des;goto _LLD;}case 26U: _LLC: _tmp482=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp481)->f1;_LLD: {struct Cyc_List_List*des=_tmp482;
# 2311
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp488=Cyc_Tcutil_compress(*topt);void*_stmttmp4D=_tmp488;void*_tmp489=_stmttmp4D;void*_tmp48C;struct Cyc_Absyn_Tqual _tmp48B;void**_tmp48A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->tag == 3U){_LL15: _tmp48A=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->f1).elt_type;_tmp48B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->f1).elt_tq;_tmp48C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->f1).ptr_atts).zero_term;_LL16: {void**elt_typ=_tmp48A;struct Cyc_Absyn_Tqual tq=_tmp48B;void*zt=_tmp48C;
# 2317
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL14;}}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2323
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp48D=0U;({struct _fat_ptr _tmp8C2=({const char*_tmp48E="tcExpNoPromote on Array_e returned non-array type";_tag_fat(_tmp48E,sizeof(char),50U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8C2,_tag_fat(_tmp48D,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2332
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_tmp8C5=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8C4=loc;void*_tmp8C3=res_typ;Cyc_Tcutil_silent_castable(_tmp8C5,_tmp8C4,_tmp8C3,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2339
return res_typ;}}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp481)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2344
void*_tmp48F=({void*_tmp8C9=Cyc_Absyn_char_type;void*_tmp8C8=rgn;struct Cyc_Absyn_Tqual _tmp8C7=Cyc_Absyn_const_tqual(0U);void*_tmp8C6=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8C9,_tmp8C8,_tmp8C7,_tmp8C6,Cyc_Absyn_true_type);});void*topt2=_tmp48F;
# 2346
void*_tmp490=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp490;
return({void*_tmp8CD=t;void*_tmp8CC=rgn;struct Cyc_Absyn_Tqual _tmp8CB=Cyc_Absyn_empty_tqual(0U);void*_tmp8CA=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp8CD,_tmp8CC,_tmp8CB,_tmp8CA,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2351
void*_tmp491=({void*_tmp8D1=Cyc_Absyn_wchar_type();void*_tmp8D0=rgn;struct Cyc_Absyn_Tqual _tmp8CF=Cyc_Absyn_const_tqual(0U);void*_tmp8CE=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp8D1,_tmp8D0,_tmp8CF,_tmp8CE,Cyc_Absyn_true_type);});void*topt2=_tmp491;
# 2353
void*_tmp492=Cyc_Tcexp_tcExp(te,& topt2,e1);void*t=_tmp492;
return({void*_tmp8D5=t;void*_tmp8D4=rgn;struct Cyc_Absyn_Tqual _tmp8D3=Cyc_Absyn_empty_tqual(0U);void*_tmp8D2=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp8D5,_tmp8D4,_tmp8D3,_tmp8D2,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2360
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp493=Cyc_Tcutil_compress(*topt);void*_stmttmp4E=_tmp493;void*_tmp494=_stmttmp4E;struct Cyc_Absyn_Tqual _tmp496;void**_tmp495;switch(*((int*)_tmp494)){case 3U: _LL1A: _tmp495=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp494)->f1).elt_type;_tmp496=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp494)->f1).elt_tq;_LL1B: {void**elttype=_tmp495;struct Cyc_Absyn_Tqual tq=_tmp496;
# 2366
topt2=elttype;goto _LL19;}case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp494)->f1)->tag == 18U){_LL1C: _LL1D:
# 2370
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2376
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2378
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt)&& !Cyc_Tcutil_is_noalias_path(e1))
Cyc_Tcexp_unique_consume_err(e1->loc);{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp497=_cycalloc(sizeof(*_tmp497));
_tmp497->tag=3U,(_tmp497->f1).elt_type=telt,({struct Cyc_Absyn_Tqual _tmp8D8=Cyc_Absyn_empty_tqual(0U);(_tmp497->f1).elt_tq=_tmp8D8;}),
((_tmp497->f1).ptr_atts).rgn=rgn,({void*_tmp8D7=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));((_tmp497->f1).ptr_atts).nullable=_tmp8D7;}),({
void*_tmp8D6=Cyc_Absyn_bounds_one();((_tmp497->f1).ptr_atts).bounds=_tmp8D6;}),((_tmp497->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp497->f1).ptr_atts).ptrloc=0;_tmp497;});
# 2385
if((topt != 0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_tmp8DB=Cyc_Tcenv_curr_rgnpo(te);unsigned _tmp8DA=loc;void*_tmp8D9=res_typ;Cyc_Tcutil_silent_castable(_tmp8DB,_tmp8DA,_tmp8D9,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}
# 2391
return res_typ;}}}}_LL5:;}}
# 2397
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp8DD=t=({void*_tmp8DC=t;Cyc_Tcutil_promote_array(_tmp8DC,(Cyc_Tcutil_addressof_props(e)).f2,0);});e->topt=_tmp8DD;});
return t;}
# 2407
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2411
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)&& !Cyc_Tcutil_is_noalias_path(e))
Cyc_Tcexp_unique_consume_err(e->loc);{
void*_tmp498=e->r;void*_stmttmp4F=_tmp498;void*_tmp499=_stmttmp4F;switch(*((int*)_tmp499)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp499)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2420
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array_type(t)){
t=({void*_tmp8DE=t;Cyc_Tcutil_promote_array(_tmp8DE,(Cyc_Tcutil_addressof_props(e)).f2,0);});
Cyc_Tcutil_unchecked_cast(e,t,Cyc_Absyn_Other_coercion);}
# 2425
return t;}_LL0:;}}
# 2437 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp49A=e->r;void*_stmttmp50=_tmp49A;void*_tmp49B=_stmttmp50;struct Cyc_Absyn_Exp*_tmp49C;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp49B)->tag == 12U){_LL1: _tmp49C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp49B)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp49C;
# 2441
Cyc_Tcexp_tcExpNoInst(te,topt,e2);
({void*_tmp8DF=Cyc_Absyn_pointer_expand((void*)_check_null(e2->topt),0);e2->topt=_tmp8DF;});
e->topt=e2->topt;
goto _LL0;}}else{_LL3: _LL4:
# 2447
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp8E0=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp8E0;});
# 2450
{void*_tmp49D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp51=_tmp49D;void*_tmp49E=_stmttmp51;void*_tmp4A4;void*_tmp4A3;void*_tmp4A2;void*_tmp4A1;struct Cyc_Absyn_Tqual _tmp4A0;void*_tmp49F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->tag == 3U){_LL6: _tmp49F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).elt_type;_tmp4A0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).elt_tq;_tmp4A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).ptr_atts).rgn;_tmp4A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).ptr_atts).nullable;_tmp4A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).ptr_atts).bounds;_tmp4A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E)->f1).ptr_atts).zero_term;_LL7: {void*t=_tmp49F;struct Cyc_Absyn_Tqual tq=_tmp4A0;void*rt=_tmp4A1;void*x=_tmp4A2;void*b=_tmp4A3;void*zt=_tmp4A4;
# 2452
{void*_tmp4A5=Cyc_Tcutil_compress(t);void*_stmttmp52=_tmp4A5;void*_tmp4A6=_stmttmp52;struct Cyc_List_List*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_List_List*_tmp4AF;struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_VarargInfo*_tmp4AD;int _tmp4AC;struct Cyc_List_List*_tmp4AB;void*_tmp4AA;struct Cyc_Absyn_Tqual _tmp4A9;void*_tmp4A8;struct Cyc_List_List*_tmp4A7;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->tag == 5U){_LLB: _tmp4A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).tvars;_tmp4A8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).effect;_tmp4A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).ret_tqual;_tmp4AA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).ret_type;_tmp4AB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).args;_tmp4AC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).c_varargs;_tmp4AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).cyc_varargs;_tmp4AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).rgn_po;_tmp4AF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).attributes;_tmp4B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).requires_clause;_tmp4B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).requires_relns;_tmp4B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).ensures_clause;_tmp4B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A6)->f1).ensures_relns;_LLC: {struct Cyc_List_List*tvs=_tmp4A7;void*eff=_tmp4A8;struct Cyc_Absyn_Tqual rtq=_tmp4A9;void*rtyp=_tmp4AA;struct Cyc_List_List*args=_tmp4AB;int c_varargs=_tmp4AC;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp4AD;struct Cyc_List_List*rpo=_tmp4AE;struct Cyc_List_List*atts=_tmp4AF;struct Cyc_Absyn_Exp*req=_tmp4B0;struct Cyc_List_List*req_relns=_tmp4B1;struct Cyc_Absyn_Exp*ens=_tmp4B2;struct Cyc_List_List*ens_relns=_tmp4B3;
# 2454
if(tvs != 0){
struct _tuple13 _tmp4B4=({struct _tuple13 _tmp630;({struct Cyc_List_List*_tmp8E1=Cyc_Tcenv_lookup_type_vars(te);_tmp630.f1=_tmp8E1;}),_tmp630.f2=Cyc_Core_heap_region;_tmp630;});struct _tuple13 env=_tmp4B4;
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& env,tvs);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple14*))Cyc_Core_snd,inst);
# 2462
rpo=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,rpo);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,rpo);{
void*ftyp=({struct Cyc_List_List*_tmp8E2=inst;Cyc_Tcutil_substitute(_tmp8E2,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));
_tmp4B9->tag=5U,(_tmp4B9->f1).tvars=0,(_tmp4B9->f1).effect=eff,(_tmp4B9->f1).ret_tqual=rtq,(_tmp4B9->f1).ret_type=rtyp,(_tmp4B9->f1).args=args,(_tmp4B9->f1).c_varargs=c_varargs,(_tmp4B9->f1).cyc_varargs=cyc_varargs,(_tmp4B9->f1).rgn_po=0,(_tmp4B9->f1).attributes=atts,(_tmp4B9->f1).requires_clause=req,(_tmp4B9->f1).requires_relns=req_relns,(_tmp4B9->f1).ensures_clause=ens,(_tmp4B9->f1).ensures_relns=ens_relns;_tmp4B9;}));});
# 2469
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B5=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8->tag=3U,(_tmp4B8->f1).elt_type=ftyp,(_tmp4B8->f1).elt_tq=tq,((_tmp4B8->f1).ptr_atts).rgn=rt,((_tmp4B8->f1).ptr_atts).nullable=x,((_tmp4B8->f1).ptr_atts).bounds=b,((_tmp4B8->f1).ptr_atts).zero_term=zt,((_tmp4B8->f1).ptr_atts).ptrloc=0;_tmp4B8;});struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_typ=_tmp4B5;
# 2471
struct Cyc_Absyn_Exp*_tmp4B6=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp4B6;
({void*_tmp8E3=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->tag=13U,_tmp4B7->f1=inner,_tmp4B7->f2=ts;_tmp4B7;});e->r=_tmp8E3;});
e->topt=(void*)new_typ;}}
# 2475
goto _LLA;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2478
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2481
goto _LL0;}_LL0:;}
# 2483
return(void*)_check_null(e->topt);}
# 2491
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2493
struct Cyc_List_List*_tmp4BA=0;struct Cyc_List_List*decls=_tmp4BA;
# 2495
{void*_tmp4BB=fn_exp->r;void*_stmttmp53=_tmp4BB;void*_tmp4BC=_stmttmp53;struct Cyc_List_List*_tmp4BD;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4BC)->tag == 10U){_LL1: _tmp4BD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4BC)->f2;_LL2: {struct Cyc_List_List*es=_tmp4BD;
# 2497
{void*_tmp4BE=e->r;void*_stmttmp54=_tmp4BE;void*_tmp4BF=_stmttmp54;struct Cyc_List_List*_tmp4C0;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4BF)->tag == 10U){_LL6: _tmp4C0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4BF)->f2;_LL7: {struct Cyc_List_List*es2=_tmp4C0;
# 2499
struct Cyc_List_List*_tmp4C1=alias_arg_exps;struct Cyc_List_List*arg_exps=_tmp4C1;
int _tmp4C2=0;int arg_cnt=_tmp4C2;
while(arg_exps != 0){
while(arg_cnt != (int)arg_exps->hd){
if(es == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp8E4=({const char*_tmp4CB="bad count ";_tag_fat(_tmp4CB,sizeof(char),11U);});_tmp635.f1=_tmp8E4;});_tmp635;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4C5=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp634;_tmp634.tag=12U,_tmp634.f1=arg_cnt;_tmp634;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp8E5=({const char*_tmp4CA="/";_tag_fat(_tmp4CA,sizeof(char),2U);});_tmp633.f1=_tmp8E5;});_tmp633;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp4C7=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp632;_tmp632.tag=12U,_tmp632.f1=(int)arg_exps->hd;_tmp632;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp631;_tmp631.tag=0U,({
struct _fat_ptr _tmp8E6=({const char*_tmp4C9=" for alias coercion!";_tag_fat(_tmp4C9,sizeof(char),21U);});_tmp631.f1=_tmp8E6;});_tmp631;});void*_tmp4C3[5U];_tmp4C3[0]=& _tmp4C4,_tmp4C3[1]=& _tmp4C5,_tmp4C3[2]=& _tmp4C6,_tmp4C3[3]=& _tmp4C7,_tmp4C3[4]=& _tmp4C8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4C3,sizeof(void*),5U));});
++ arg_cnt;
es=es->tl;
es2=((struct Cyc_List_List*)_check_null(es2))->tl;}{
# 2511
struct _tuple12 _tmp4CC=({struct Cyc_Absyn_Exp*_tmp8E7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcutil_insert_alias(_tmp8E7,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es2))->hd)->topt)));});struct _tuple12 _stmttmp55=_tmp4CC;struct _tuple12 _tmp4CD=_stmttmp55;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Decl*_tmp4CE;_LLB: _tmp4CE=_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;_LLC: {struct Cyc_Absyn_Decl*d=_tmp4CE;struct Cyc_Absyn_Exp*ve=_tmp4CF;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->hd=d,_tmp4D0->tl=decls;_tmp4D0;});
arg_exps=arg_exps->tl;}}}
# 2516
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp4D1=0U;({struct _fat_ptr _tmp8E8=({const char*_tmp4D2="not a function call!";_tag_fat(_tmp4D2,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8E8,_tag_fat(_tmp4D1,sizeof(void*),0U));});});}_LL5:;}
# 2519
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp4D3=0U;({struct _fat_ptr _tmp8E9=({const char*_tmp4D4="not a function call!";_tag_fat(_tmp4D4,sizeof(char),21U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8E9,_tag_fat(_tmp4D3,sizeof(void*),0U));});});}_LL0:;}
# 2523
while(decls != 0){
struct Cyc_Absyn_Decl*_tmp4D5=(struct Cyc_Absyn_Decl*)decls->hd;struct Cyc_Absyn_Decl*d=_tmp4D5;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp8EC=({struct Cyc_Absyn_Decl*_tmp8EB=d;struct Cyc_Absyn_Stmt*_tmp8EA=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp8EB,_tmp8EA,e->loc);});Cyc_Absyn_stmt_exp(_tmp8EC,e->loc);});
decls=decls->tl;}
# 2529
e->topt=0;
e->r=fn_exp->r;}
# 2534
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2538
{void*_tmp4D6=e->r;void*_stmttmp56=_tmp4D6;void*_tmp4D7=_stmttmp56;void*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;struct _fat_ptr*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;int*_tmp4E3;struct Cyc_Absyn_Exp**_tmp4E2;void***_tmp4E1;struct Cyc_Absyn_Exp**_tmp4E0;int*_tmp4DF;struct Cyc_Absyn_Enumfield*_tmp4E5;void*_tmp4E4;struct Cyc_Absyn_Enumfield*_tmp4E7;struct Cyc_Absyn_Enumdecl*_tmp4E6;struct Cyc_Absyn_Datatypefield*_tmp4EA;struct Cyc_Absyn_Datatypedecl*_tmp4E9;struct Cyc_List_List*_tmp4E8;struct Cyc_List_List*_tmp4EC;void*_tmp4EB;struct Cyc_Absyn_Aggrdecl**_tmp4F0;struct Cyc_List_List*_tmp4EF;struct Cyc_List_List**_tmp4EE;struct _tuple1**_tmp4ED;int*_tmp4F3;void*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;int*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Vardecl*_tmp4F4;struct Cyc_Absyn_Stmt*_tmp4F8;struct Cyc_List_List*_tmp4F9;struct Cyc_List_List*_tmp4FB;struct _tuple9*_tmp4FA;struct Cyc_List_List*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FD;int*_tmp502;int*_tmp501;struct _fat_ptr*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;int*_tmp506;int*_tmp505;struct _fat_ptr*_tmp504;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_List_List*_tmp509;void*_tmp508;void*_tmp50A;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp**_tmp50C;struct Cyc_Absyn_Exp*_tmp50E;enum Cyc_Absyn_Coercion*_tmp511;struct Cyc_Absyn_Exp*_tmp510;void*_tmp50F;struct Cyc_List_List*_tmp513;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Core_Opt*_tmp51F;struct Cyc_Absyn_Exp*_tmp51E;enum Cyc_Absyn_Incrementor _tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_List_List*_tmp524;enum Cyc_Absyn_Primop _tmp523;void**_tmp525;union Cyc_Absyn_Cnst*_tmp526;struct Cyc_List_List*_tmp528;struct Cyc_Core_Opt*_tmp527;struct Cyc_Absyn_VarargCallInfo**_tmp52B;struct Cyc_List_List*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;int*_tmp52F;struct Cyc_Absyn_VarargCallInfo**_tmp52E;struct Cyc_List_List*_tmp52D;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp530;switch(*((int*)_tmp4D7)){case 12U: _LL1: _tmp530=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL2: {struct Cyc_Absyn_Exp*e2=_tmp530;
# 2543
Cyc_Tcexp_tcExpNoInst(te,0,e2);
return;}case 10U: _LL3: _tmp52C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp52D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp52E=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_tmp52F=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7)->f4;if(!(*_tmp52F)){_LL4: {struct Cyc_Absyn_Exp*e1=_tmp52C;struct Cyc_List_List*es=_tmp52D;struct Cyc_Absyn_VarargCallInfo**vci=_tmp52E;int*resolved=_tmp52F;
# 2547
({void*_tmp531=0U;({struct _fat_ptr _tmp8ED=({const char*_tmp532="unresolved function in tcExpNoInst";_tag_fat(_tmp532,sizeof(char),35U);});((int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos)(_tmp8ED,_tag_fat(_tmp531,sizeof(void*),0U));});});}}else{_LL1B: _tmp529=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp52A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp52B=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp529;struct Cyc_List_List*es=_tmp52A;struct Cyc_Absyn_VarargCallInfo**vci=_tmp52B;
# 2580
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp533;_push_handler(& _tmp533);{int _tmp535=0;if(setjmp(_tmp533.handler))_tmp535=1;if(!_tmp535){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp534=(void*)Cyc_Core_get_exn_thrown();void*_tmp536=_tmp534;void*_tmp537;if(((struct Cyc_Core_Failure_exn_struct*)_tmp536)->tag == Cyc_Core_Failure){_LL5A: _LL5B:
# 2587
 ok=0;
fn_exp=e;
goto _LL59;}else{_LL5C: _tmp537=_tmp536;_LL5D: {void*exn=_tmp537;(int)_rethrow(exn);}}_LL59:;}}}
# 2591
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps != 0 && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2598
goto _LL0;}}case 36U: _LL5: _tmp527=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp528=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL6: {struct Cyc_Core_Opt*nopt=_tmp527;struct Cyc_List_List*des=_tmp528;
# 2551
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0U: _LL7: _tmp526=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL8: {union Cyc_Absyn_Cnst*c=_tmp526;
# 2556
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1U: _LL9: _tmp525=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LLA: {void**b=_tmp525;
# 2558
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2U: _LLB: _LLC:
# 2560
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp523=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp524=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LLE: {enum Cyc_Absyn_Primop p=_tmp523;struct Cyc_List_List*es=_tmp524;
# 2562
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5U: _LLF: _tmp521=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp522=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp521;enum Cyc_Absyn_Incrementor i=_tmp522;
# 2564
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4U: _LL11: _tmp51E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp51F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp520=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp51E;struct Cyc_Core_Opt*popt=_tmp51F;struct Cyc_Absyn_Exp*e2=_tmp520;
# 2566
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6U: _LL13: _tmp51B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp51C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp51D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp51B;struct Cyc_Absyn_Exp*e2=_tmp51C;struct Cyc_Absyn_Exp*e3=_tmp51D;
# 2568
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7U: _LL15: _tmp519=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp51A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp519;struct Cyc_Absyn_Exp*e2=_tmp51A;
# 2570
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8U: _LL17: _tmp517=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp518=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp517;struct Cyc_Absyn_Exp*e2=_tmp518;
# 2572
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9U: _LL19: _tmp515=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp516=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp515;struct Cyc_Absyn_Exp*e2=_tmp516;
# 2574
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11U: _LL1D: _tmp514=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp514;
# 2600
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13U: _LL1F: _tmp512=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp513=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp512;struct Cyc_List_List*ts=_tmp513;
# 2602
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14U: _LL21: _tmp50F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp510=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp511=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D7)->f4;_LL22: {void*t1=_tmp50F;struct Cyc_Absyn_Exp*e1=_tmp510;enum Cyc_Absyn_Coercion*c=_tmp511;
# 2604
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15U: _LL23: _tmp50E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmp50E;
# 2606
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16U: _LL25: _tmp50C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp50D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL26: {struct Cyc_Absyn_Exp**rgn_handle=_tmp50C;struct Cyc_Absyn_Exp*e1=_tmp50D;
# 2608
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,e,e1);goto _LL0;}case 18U: _LL27: _tmp50B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmp50B;
# 2610
void*_tmp538=Cyc_Tcexp_tcExpNoPromote(te,0,e1);void*t1=_tmp538;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 17U: _LL29: _tmp50A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL2A: {void*t1=_tmp50A;
# 2613
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19U: _LL2B: _tmp508=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp509=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL2C: {void*t1=_tmp508;struct Cyc_List_List*l=_tmp509;
# 2615
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20U: _LL2D: _tmp507=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmp507;
# 2617
t=Cyc_Tcexp_tcDeref(te,loc,topt,e1);goto _LL0;}case 21U: _LL2F: _tmp503=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp504=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp505=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_tmp506=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D7)->f4;_LL30: {struct Cyc_Absyn_Exp*e1=_tmp503;struct _fat_ptr*f=_tmp504;int*is_tagged=_tmp505;int*is_read=_tmp506;
# 2619
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,e1,f,is_tagged,is_read);goto _LL0;}case 22U: _LL31: _tmp4FF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp500=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp501=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_tmp502=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D7)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmp4FF;struct _fat_ptr*f=_tmp500;int*is_tagged=_tmp501;int*is_read=_tmp502;
# 2621
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23U: _LL33: _tmp4FD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4FE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmp4FD;struct Cyc_Absyn_Exp*e2=_tmp4FE;
# 2623
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e1,e2);goto _LL0;}case 24U: _LL35: _tmp4FC=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL36: {struct Cyc_List_List*es=_tmp4FC;
# 2625
t=Cyc_Tcexp_tcTuple(te,loc,topt,es);goto _LL0;}case 25U: _LL37: _tmp4FA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4FB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL38: {struct _tuple9*t1=_tmp4FA;struct Cyc_List_List*des=_tmp4FB;
# 2627
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 26U: _LL39: _tmp4F9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL3A: {struct Cyc_List_List*des=_tmp4F9;
# 2631
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp539=Cyc_Tcutil_compress(*topt);void*_stmttmp57=_tmp539;void*_tmp53A=_stmttmp57;void*_tmp53D;struct Cyc_Absyn_Tqual*_tmp53C;void**_tmp53B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53A)->tag == 4U){_LL5F: _tmp53B=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53A)->f1).elt_type;_tmp53C=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53A)->f1).tq;_tmp53D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53A)->f1).zero_term;_LL60: {void**et=_tmp53B;struct Cyc_Absyn_Tqual*etq=_tmp53C;void*zt=_tmp53D;
# 2637
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}
# 2643
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 37U: _LL3B: _tmp4F8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL3C: {struct Cyc_Absyn_Stmt*s=_tmp4F8;
# 2645
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 27U: _LL3D: _tmp4F4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4F5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp4F6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_tmp4F7=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D7)->f4;_LL3E: {struct Cyc_Absyn_Vardecl*vd=_tmp4F4;struct Cyc_Absyn_Exp*e1=_tmp4F5;struct Cyc_Absyn_Exp*e2=_tmp4F6;int*iszeroterm=_tmp4F7;
# 2647
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 28U: _LL3F: _tmp4F1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4F2=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp4F3=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_LL40: {struct Cyc_Absyn_Exp*e1=_tmp4F1;void*t1=_tmp4F2;int*iszeroterm=_tmp4F3;
# 2649
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 29U: _LL41: _tmp4ED=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4EE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp4EF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_tmp4F0=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D7)->f4;_LL42: {struct _tuple1**tn=_tmp4ED;struct Cyc_List_List**ts=_tmp4EE;struct Cyc_List_List*args=_tmp4EF;struct Cyc_Absyn_Aggrdecl**sd_opt=_tmp4F0;
# 2651
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 30U: _LL43: _tmp4EB=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4EC=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL44: {void*ts=_tmp4EB;struct Cyc_List_List*args=_tmp4EC;
# 2653
t=Cyc_Tcexp_tcAnonStruct(te,loc,ts,args);goto _LL0;}case 31U: _LL45: _tmp4E8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4E9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_tmp4EA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D7)->f3;_LL46: {struct Cyc_List_List*es=_tmp4E8;struct Cyc_Absyn_Datatypedecl*tud=_tmp4E9;struct Cyc_Absyn_Datatypefield*tuf=_tmp4EA;
# 2655
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 32U: _LL47: _tmp4E6=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4E7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL48: {struct Cyc_Absyn_Enumdecl*ed=_tmp4E6;struct Cyc_Absyn_Enumfield*ef=_tmp4E7;
# 2657
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 33U: _LL49: _tmp4E4=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4E5=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL4A: {void*t2=_tmp4E4;struct Cyc_Absyn_Enumfield*ef=_tmp4E5;
# 2659
t=t2;goto _LL0;}case 34U: _LL4B: _tmp4DF=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1).is_calloc;_tmp4E0=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1).rgn;_tmp4E1=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1).elt_type;_tmp4E2=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1).num_elts;_tmp4E3=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1).fat_result;_LL4C: {int*is_calloc=_tmp4DF;struct Cyc_Absyn_Exp**ropt=_tmp4E0;void***t2=_tmp4E1;struct Cyc_Absyn_Exp**e2=_tmp4E2;int*isfat=_tmp4E3;
# 2661
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,t2,e2,is_calloc,isfat);goto _LL0;}case 35U: _LL4D: _tmp4DD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4DE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL4E: {struct Cyc_Absyn_Exp*e1=_tmp4DD;struct Cyc_Absyn_Exp*e2=_tmp4DE;
# 2663
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 38U: _LL4F: _tmp4DB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_tmp4DC=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4D7)->f2;_LL50: {struct Cyc_Absyn_Exp*e=_tmp4DB;struct _fat_ptr*f=_tmp4DC;
# 2665
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 41U: _LL51: _tmp4DA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL52: {struct Cyc_Absyn_Exp*e1=_tmp4DA;
# 2667
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 42U: _LL53: _tmp4D9=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL54: {struct Cyc_Absyn_Exp*e=_tmp4D9;
# 2669
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 39U: _LL55: _tmp4D8=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4D7)->f1;_LL56: {void*t2=_tmp4D8;
# 2671
if(!te->allow_valueof)
({void*_tmp53E=0U;({unsigned _tmp8EF=e->loc;struct _fat_ptr _tmp8EE=({const char*_tmp53F="valueof(-) can only occur within types";_tag_fat(_tmp53F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp8EF,_tmp8EE,_tag_fat(_tmp53E,sizeof(void*),0U));});});
# 2679
t=Cyc_Absyn_sint_type;
goto _LL0;}default: _LL57: _LL58:
# 2683
 t=Cyc_Absyn_void_type;
goto _LL0;}_LL0:;}
# 2686
e->topt=t;}
